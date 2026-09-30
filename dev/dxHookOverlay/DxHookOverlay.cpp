#include "DxHookOverlay.h"

#include <QDebug>
#include <QSurfaceFormat>
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>
#include "uiBackend.h"
#include <QDir>

DxHookOverlay::DxHookOverlay(
    UiBackend* uiBackend,
    QObject* parent
    )
    : QObject(parent)
    , m_uiBackend(uiBackend)
{
}


DxHookOverlay::~DxHookOverlay()
{
    if (m_context) {
        m_context->makeCurrent(m_surface);

        delete m_fbo;
        m_fbo = nullptr;

        m_renderControl.invalidate();

        m_context->doneCurrent();
    }

    delete m_window;
    m_window = nullptr;

    delete m_surface;
    m_surface = nullptr;

    delete m_context;
    m_context = nullptr;
}


bool DxHookOverlay::initialize(int width, int height)
{
    m_width = width;
    m_height = height;

    if (!initializeOpenGL())
        return false;

    if (!initializeQml())
        return false;

    if (!initializeFramebuffer())
        return false;

    return true;
}


bool DxHookOverlay::initializeOpenGL()
{
    QSurfaceFormat format;

    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setVersion(2, 0);
    format.setProfile(QSurfaceFormat::NoProfile);

    format.setRedBufferSize(8);
    format.setGreenBufferSize(8);
    format.setBlueBufferSize(8);
    format.setAlphaBufferSize(8);

    m_context = new QOpenGLContext(this);
    m_context->setFormat(format);

    if (!m_context->create()) {
        qWarning() << "DxHookOverlay: failed to create OpenGL context";
        return false;
    }

    m_surface = new QOffscreenSurface();
    m_surface->setFormat(m_context->format());
    m_surface->create();

    if (!m_surface->isValid()) {
        qWarning() << "DxHookOverlay: failed to create offscreen surface";
        return false;
    }

    if (!m_context->makeCurrent(m_surface)) {
        qWarning() << "DxHookOverlay: failed to make OpenGL context current";
        return false;
    }

    return true;
}


void DxHookOverlay::registerQmlContext()
{
    if (!m_uiBackend) {
        qWarning() << "DxHookOverlay: m_uiBackend is null";
        return;
    }

    /*
     * Это то же самое, что у тебя сейчас делает Core/main engine:
     *
     * _uiBackend.someMethod()
     *
     * Поэтому регистрируем тот же QObject во втором engine.
     */
    m_engine.rootContext()->setContextProperty(
        QStringLiteral("_uiBackend"),
        m_uiBackend
        );
}


bool DxHookOverlay::initializeQml()
{
    // Context properties должны быть зарегистрированы
    // до component.create()
    registerQmlContext();

    if (!m_uiBackend) {
        qWarning() << "DxHookOverlay: uiBackend is null";
        return false;
    }

    // ВАЖНО:
    // QML использует image://imageprovider/...
    m_engine.addImageProvider(
        QStringLiteral("imageprovider"),
        m_uiBackend->imageProvider()
        );

    m_window = new QQuickWindow(&m_renderControl);

    m_window->setWidth(m_width);
    m_window->setHeight(m_height);
    m_window->setColor(Qt::transparent);

    // Qt 5: initialize() возвращает void
    m_renderControl.initialize(m_context);

    QQmlComponent component(
        &m_engine,
        QUrl(QStringLiteral(
            "qrc:/resources/qml/DxOverlayWindow.qml"
            ))
        );

    if (component.status() == QQmlComponent::Error) {
        qWarning() << "DxHookOverlay: QML errors:";

        for (const QQmlError& error : component.errors())
            qWarning() << error.toString();

        return false;
    }

    QObject* object = component.create();

    if (!object) {
        qWarning() << "DxHookOverlay: failed to create QML object";

        for (const QQmlError& error : component.errors())
            qWarning() << error.toString();

        return false;
    }

    m_root = qobject_cast<QQuickItem*>(object);

    if (!m_root) {
        qWarning()
        << "DxHookOverlay: root object is not QQuickItem";

        delete object;
        return false;
    }

    m_root->setParentItem(m_window->contentItem());

    m_root->setWidth(m_width);
    m_root->setHeight(m_height);

    return true;
}


bool DxHookOverlay::initializeFramebuffer()
{
    if (!m_context)
        return false;

    if (!m_context->makeCurrent(m_surface)) {
        qWarning() << "DxHookOverlay: failed to make context current";
        return false;
    }

    QOpenGLFramebufferObjectFormat fboFormat;

    fboFormat.setAttachment(
        QOpenGLFramebufferObject::CombinedDepthStencil
        );

    fboFormat.setInternalTextureFormat(GL_RGBA8);

    m_fbo = new QOpenGLFramebufferObject(
        m_width,
        m_height,
        fboFormat
        );

    if (!m_fbo->isValid()) {
        qWarning() << "DxHookOverlay: FBO is invalid";

        delete m_fbo;
        m_fbo = nullptr;

        return false;
    }

    m_context->doneCurrent();

    return true;
}


bool DxHookOverlay::render()
{
    if (!m_context ||
        !m_surface ||
        !m_window ||
        !m_root ||
        !m_fbo)
    {
        return false;
    }

    if (!m_context->makeCurrent(m_surface)) {
        qWarning() << "DxHookOverlay: failed to make context current";
        return false;
    }

    QOpenGLFunctions* gl = m_context->functions();

    /*
     * Пока используем обычный FBO.
     */
    m_fbo->bind();

    gl->glViewport(
        0,
        0,
        m_width,
        m_height
        );

    gl->glClearColor(
        0.0f,
        0.0f,
        0.0f,
        0.0f
        );

    gl->glClear(
        GL_COLOR_BUFFER_BIT |
        GL_DEPTH_BUFFER_BIT |
        GL_STENCIL_BUFFER_BIT
        );

    /*
     * Обновляем QML scene graph.
     */
    m_renderControl.polishItems();
    m_renderControl.sync();

    /*
     * Рендер QML.
     */
    m_renderControl.render();

    gl->glFinish();

    /*
     * Забираем результат из FBO в CPU.
     */
    QImage result(m_width, m_height,QImage::Format_RGBA8888 );

    gl->glReadPixels(
        0,
        0,
        m_width,
        m_height,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        result.bits()
        );

    m_fbo->release();

    /*
     * OpenGL имеет начало координат снизу,
     * QImage — сверху.
     */
    m_image = result.mirrored(false, true);

    m_context->doneCurrent();

    return true;
}

void DxHookOverlay::runOverlay(bool gameLaunched)
{
    if (!gameLaunched)
        return;

    std::wstring processName = L"W40k.exe"; // Укажите точное имя исполняемого файла Dawn of War

    QString appDir = QCoreApplication::applicationDirPath();
    QString qDllPath = QDir::toNativeSeparators(appDir + "/DxHookOverlay.dll");
    std::wstring dllPath = qDllPath.toStdWString();

    DWORD pid = GetProcessIdByName(processName);


    if (InjectDLL(pid, dllPath))
        qDebug() << "Оверлей успешно внедрен!";
    else
        qDebug()  << "Ошибка внедрения.";

}


bool DxHookOverlay::InjectDLL(DWORD processId, const std::wstring& dllPath) {
    if (processId == 0) return false;

    // 1. Открываем процесс игры с необходимыми правами
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (!hProcess) {
        qDebug() << L"Не удалось открыть процесс игры. Ошибка: " << GetLastError();
        return false;
    }

    // 2. Выделяем память внутри процесса игры под строку с путем к DLL
    size_t pathSize = (dllPath.length() + 1) * sizeof(wchar_t);
    LPVOID pRemoteBuf = VirtualAllocEx(hProcess, nullptr, pathSize, MEM_COMMIT, PAGE_READWRITE);
    if (!pRemoteBuf) {
        qDebug() << L"Не удалось выделить память в игре.";
        CloseHandle(hProcess);
        return false;
    }

    // 3. Записываем путь к нашей DLL в выделенную память игры
    if (!WriteProcessMemory(hProcess, pRemoteBuf, dllPath.c_str(), pathSize, nullptr)) {
        qDebug() << L"Не удалось записать путь DLL в память игры.";
        VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // 4. Получаем адрес функции LoadLibraryW из kernel32.dll (он одинаков для всех процессов)
    LPTHREAD_START_ROUTINE pLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(
        GetModuleHandleW(L"kernel32.dll"), "LoadLibraryW");

    if (!pLoadLibrary) {
        VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // 5. Создаем удаленный поток в игре, который вызовет LoadLibraryW(путь_к_DLL)
    HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0, pLoadLibrary, pRemoteBuf, 0, nullptr);
    if (!hThread) {
        qDebug() << L"Не удалось создать удаленный поток в игре.";
        VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // Ожидаем завершения загрузки и очищаем за собой память в целевом процессе
    WaitForSingleObject(hThread, INFINITE);

    CloseHandle(hThread);
    VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
    CloseHandle(hProcess);
    return true;
}

DWORD DxHookOverlay::GetProcessIdByName(const std::wstring& processName) {
    DWORD pid = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W processEntry;
        processEntry.dwSize = sizeof(processEntry);

        if (Process32FirstW(snapshot, &processEntry)) {
            do {
                if (processName == processEntry.szExeFile) {
                    pid = processEntry.th32ProcessID;
                    break;
                }
            } while (Process32NextW(snapshot, &processEntry));
        }
        CloseHandle(snapshot);
    }
    return pid;
}


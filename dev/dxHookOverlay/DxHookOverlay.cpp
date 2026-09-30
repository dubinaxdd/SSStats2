#include "DxHookOverlay.h"

#include <QDebug>
#include <QSurfaceFormat>
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>
#include "uiBackend.h"
#include <QDir>
#include <cstring>


namespace
{
constexpr wchar_t SHARED_MEMORY_NAME[] =
    L"Local\\DX9OverlayImage";

// Максимальный поддерживаемый размер.
constexpr uint32_t MAX_IMAGE_WIDTH = 3840;
constexpr uint32_t MAX_IMAGE_HEIGHT = 2160;

constexpr uint32_t BYTES_PER_PIXEL = 4;

constexpr uint32_t MAX_IMAGE_SIZE =
    MAX_IMAGE_WIDTH *
    MAX_IMAGE_HEIGHT *
    BYTES_PER_PIXEL;


/*
     * Shared memory layout.
     *
     * ВАЖНО:
     * Точно такую же структуру должна использовать DLL.
     */
struct SharedImage
{
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t format;

    uint64_t frame;

    /*
         * Какой buffer сейчас опубликован
         * для чтения DLL.
         *
         * 0 -> pixels[0]
         * 1 -> pixels[1]
         */
    volatile LONG activeBuffer;

    /*
         * Два независимых буфера.
         */
    uint8_t pixels[2][MAX_IMAGE_SIZE];
};


HANDLE g_sharedMemory = nullptr;
SharedImage* g_sharedImage = nullptr;


bool IsValidImageSize(uint32_t width, uint32_t height)
{
    return width > 0 &&
           height > 0 &&
           width <= MAX_IMAGE_WIDTH &&
           height <= MAX_IMAGE_HEIGHT;
}
}


// ------------------------------------------------------------
// Создание / открытие shared memory
// ------------------------------------------------------------

bool CreateSharedImage(uint32_t width, uint32_t height)
{
    if (!IsValidImageSize(width, height))
    {
        qWarning()
        << "Invalid shared image size:"
        << width
        << "x"
        << height;

        return false;
    }

    if (g_sharedImage)
    {
        /*
         * Mapping уже существует.
         *
         * Проверяем, совпадает ли его размер.
         */
        if (g_sharedImage->width != width ||
            g_sharedImage->height != height)
        {
            qWarning()
            << "Shared memory size mismatch:"
            << "existing ="
            << g_sharedImage->width
            << "x"
            << g_sharedImage->height
            << "requested ="
            << width
            << "x"
            << height;

            return false;
        }

        return true;
    }


    g_sharedMemory = CreateFileMappingW(
        INVALID_HANDLE_VALUE,
        nullptr,
        PAGE_READWRITE,
        0,
        static_cast<DWORD>(sizeof(SharedImage)),
        SHARED_MEMORY_NAME
        );

    if (!g_sharedMemory)
    {
        qWarning()
        << "CreateFileMappingW failed:"
        << GetLastError();

        return false;
    }


    const DWORD mappingError = GetLastError();

    const bool alreadyExists =
        (mappingError == ERROR_ALREADY_EXISTS);


    g_sharedImage =
        reinterpret_cast<SharedImage*>(
            MapViewOfFile(
                g_sharedMemory,
                FILE_MAP_ALL_ACCESS,
                0,
                0,
                sizeof(SharedImage)
                )
            );


    if (!g_sharedImage)
    {
        qWarning()
        << "MapViewOfFile failed:"
        << GetLastError();

        CloseHandle(g_sharedMemory);
        g_sharedMemory = nullptr;

        return false;
    }


    if (!alreadyExists)
    {
        g_sharedImage->width = width;
        g_sharedImage->height = height;

        g_sharedImage->pitch =
            width * BYTES_PER_PIXEL;

        // 1 = BGRA8
        g_sharedImage->format = 1;

        g_sharedImage->frame = 0;

        g_sharedImage->activeBuffer = 0;

        std::memset(
            g_sharedImage->pixels,
            0,
            sizeof(g_sharedImage->pixels)
            );

        qDebug()
            << "Shared memory created:"
            << width
            << "x"
            << height;
    }
    else
    {
        /*
         * Mapping уже был создан другим процессом.
         */
        qDebug()
            << "Shared memory already exists:"
            << g_sharedImage->width
            << "x"
            << g_sharedImage->height;


        if (g_sharedImage->width != width ||
            g_sharedImage->height != height)
        {
            qWarning()
            << "Existing shared memory has wrong size";

            UnmapViewOfFile(g_sharedImage);
            g_sharedImage = nullptr;

            CloseHandle(g_sharedMemory);
            g_sharedMemory = nullptr;

            return false;
        }
    }

    return true;
}



// ------------------------------------------------------------
// Отправка QImage
//
// QImage:
//     RGBA8888
//
// Shared memory:
//     BGRA8
// ------------------------------------------------------------

bool SendQImageToSharedMemory(const QImage& image)
{
    if (image.isNull())
    {
        qWarning()
        << "SendQImageToSharedMemory: image is null";

        return false;
    }


    const uint32_t width =
        static_cast<uint32_t>(image.width());

    const uint32_t height =
        static_cast<uint32_t>(image.height());


    if (!IsValidImageSize(width, height))
    {
        qWarning()
        << "SendQImageToSharedMemory: invalid size:"
        << width
        << "x"
        << height;

        return false;
    }


    if (!g_sharedImage)
    {
        if (!CreateSharedImage(width, height))
            return false;
    }


    /*
     * Размер shared memory должен совпадать
     * с текущим QImage.
     */
    if (g_sharedImage->width != width ||
        g_sharedImage->height != height)
    {
        qWarning()
        << "SendQImageToSharedMemory: size mismatch:"
        << "image =" << width << "x" << height
        << "shared =" << g_sharedImage->width
        << "x" << g_sharedImage->height;

        return false;
    }

    QImage converted =
        image.convertToFormat(QImage::Format_RGBA8888);

    const uint8_t* src =
        converted.constBits();

    const uint32_t srcPitch =
        static_cast<uint32_t>(
            converted.bytesPerLine()
            );


    /*
     * Читаем текущий активный buffer.
     */
    const LONG active =
        InterlockedCompareExchange(
            &g_sharedImage->activeBuffer,
            0,
            0
            );


    /*
     * Пишем в ДРУГОЙ buffer.
     */
    const LONG writeBuffer =
        (active == 0) ? 1 : 0;


    uint8_t* dst =
        g_sharedImage->pixels[writeBuffer];


    const uint32_t dstPitch =
        g_sharedImage->pitch;


    /*
     * RGBA8888 -> BGRA8
     */
    for (uint32_t y = 0; y < height; ++y)
    {
        const uint8_t* srcRow =
            src + y * srcPitch;

        uint8_t* dstRow =
            dst + y * dstPitch;


        for (uint32_t x = 0; x < width; ++x)
        {
            const uint32_t offset = x * 4;

            const uint8_t r =
                srcRow[offset + 0];

            const uint8_t g =
                srcRow[offset + 1];

            const uint8_t b =
                srcRow[offset + 2];

            const uint8_t a =
                srcRow[offset + 3];


            dstRow[offset + 0] = b;
            dstRow[offset + 1] = g;
            dstRow[offset + 2] = r;
            dstRow[offset + 3] = a;
        }
    }

    ++g_sharedImage->frame;
    InterlockedExchange(&g_sharedImage->activeBuffer, writeBuffer);

    return true;
}


DxHookOverlay::DxHookOverlay(GameController *gameController,
                             UiBackend* uiBackend,
                             QObject* parent
                             )
    : QObject(parent)
    , p_gameController(gameController)
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

    m_renderTimer = new QTimer(this);

    connect(
        m_renderTimer,
        &QTimer::timeout,
        this,
        [this]()
        {
            render();
        }
        );

    m_renderTimer->start(16); // примерно 60 FPS

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

    if (!m_window)
    {
        qWarning()
        << "DxHookOverlay: m_window is null";

        return false;
    }

    if (!m_context->makeCurrent(m_surface))
    {
        qWarning()
        << "DxHookOverlay: failed to make context current";

        return false;
    }

    QOpenGLFramebufferObjectFormat fboFormat;

    fboFormat.setAttachment(
        QOpenGLFramebufferObject::CombinedDepthStencil
        );

    fboFormat.setInternalTextureFormat(
        GL_RGBA8
        );

    m_fbo =
        new QOpenGLFramebufferObject(
            m_width,
            m_height,
            fboFormat
            );

    if (!m_fbo->isValid())
    {
        qWarning()
        << "DxHookOverlay: FBO is invalid";

        delete m_fbo;
        m_fbo = nullptr;

        m_context->doneCurrent();

        return false;
    }

    qDebug()
        << "DxHookOverlay: FBO created:"
        << "handle =" << m_fbo->handle()
        << "size =" << m_fbo->size()
        << "valid =" << m_fbo->isValid();

    /*
     * ---------------------------------------------------------
     * ВАЖНО
     *
     * Говорим QQuickWindow, что QML нужно
     * рендерить именно в наш FBO.
     * ---------------------------------------------------------
     */

    m_window->setRenderTarget(m_fbo);

    qDebug()
        << "DxHookOverlay: QQuickWindow render target set to FBO:"
        << m_fbo->handle();

    /*
     * Проверяем, что FBO действительно можно bind'нуть.
     */

    m_fbo->bind();

    GLint framebuffer = 0;

    QOpenGLFunctions* gl =
        m_context->functions();

    gl->glGetIntegerv(
        GL_FRAMEBUFFER_BINDING,
        &framebuffer
        );

    qDebug()
        << "DxHookOverlay: framebuffer after bind:"
        << framebuffer;

    m_fbo->release();

    m_context->doneCurrent();

    return true;
}


/*bool DxHookOverlay::render()
{
    if (!m_context ||
        !m_surface ||
        !m_window ||
        !m_root ||
        !m_fbo)
    {
        return false;
    }

    if (!m_context->makeCurrent(m_surface))
    {
        qWarning()
        << "DxHookOverlay: failed to make context current";

        return false;
    }

    QOpenGLFunctions* gl =
        m_context->functions();

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


    m_renderControl.polishItems();
    m_renderControl.sync();
    m_renderControl.render();
    gl->glFinish();

    QImage result(
        m_width,
        m_height,
        QImage::Format_RGBA8888
        );

    gl->glReadPixels(
        0,
        0,
        m_width,
        m_height,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        result.bits()
        );


    const GLenum glError = gl->glGetError();

    if (glError != GL_NO_ERROR)
    {
        qWarning()
        << "DxHookOverlay: glReadPixels error:"
        << Qt::hex
        << glError;

        m_fbo->release();
        m_context->doneCurrent();

        return false;
    }

    m_fbo->release();


    m_image =
        result.mirrored(false, true);


    if (!SendQImageToSharedMemory(m_image))
    {
        qWarning()
        << "DxHookOverlay: failed to send frame";
    }

    m_context->doneCurrent();

    return true;
}*/


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

    if (!m_context->makeCurrent(m_surface))
    {
        qWarning()
        << "DxHookOverlay: failed to make context current";

        return false;
    }

    QOpenGLFunctions* gl =
        m_context->functions();

    /*
     * ---------------------------------------------------------
     * 1. Привязываем наш FBO
     * ---------------------------------------------------------
     */

    m_fbo->bind();

    GLint framebufferBefore = 0;

    gl->glGetIntegerv(
        GL_FRAMEBUFFER_BINDING,
        &framebufferBefore
        );

    qDebug()
        << "FRAMEBUFFER AFTER FBO BIND:"
        << framebufferBefore;

    gl->glViewport(
        0,
        0,
        m_width,
        m_height
        );

    gl->glDisable(GL_SCISSOR_TEST);

    /*
     * Полностью очищаем FBO прозрачным цветом.
     */
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
     * ---------------------------------------------------------
     * 2. Диагностика QML
     * ---------------------------------------------------------
     */

    qDebug()
        << "QML ROOT:"
        << "width =" << m_root->width()
        << "height =" << m_root->height()
        << "visible =" << m_root->isVisible()
        << "opacity =" << m_root->opacity()
        << "enabled =" << m_root->isEnabled();

    const auto children =
        m_root->childItems();

    qDebug()
        << "QML ROOT CHILDREN:"
        << children.size();

    for (QQuickItem* child : children)
    {
        if (!child)
            continue;

        qDebug()
            << "  child:"
            << child
            << "x =" << child->x()
            << "y =" << child->y()
            << "width =" << child->width()
            << "height =" << child->height()
            << "visible =" << child->isVisible()
            << "opacity =" << child->opacity();
    }

    /*
     * ---------------------------------------------------------
     * 3. Обновляем QML scene graph
     * ---------------------------------------------------------
     */

    m_renderControl.polishItems();

    m_renderControl.sync();

    /*
     * ---------------------------------------------------------
     * 4. Рендерим QML
     * ---------------------------------------------------------
     */

    m_renderControl.render();

    /*
     * QQuickRenderControl после render()
     * восстанавливает framebuffer.
     *
     * Поэтому снова привязываем наш FBO.
     */
    m_fbo->bind();

    GLint framebufferAfterRebind = 0;

    gl->glGetIntegerv(
        GL_FRAMEBUFFER_BINDING,
        &framebufferAfterRebind
        );

    qDebug()
        << "FRAMEBUFFER AFTER REBIND:"
        << framebufferAfterRebind;

    /*
     * ---------------------------------------------------------
     * 5. Дожидаемся завершения OpenGL
     * ---------------------------------------------------------
     */

    gl->glFinish();

    /*
     * ---------------------------------------------------------
     * 6. Читаем FBO
     * ---------------------------------------------------------
     */

    QImage result(
        m_width,
        m_height,
        QImage::Format_RGBA8888
        );

    gl->glReadPixels(
        0,
        0,
        m_width,
        m_height,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        result.bits()
        );

    /*
     * Проверяем ошибку OpenGL.
     */
    const GLenum glError =
        gl->glGetError();

    if (glError != GL_NO_ERROR)
    {
        qWarning()
        << "DxHookOverlay: glReadPixels error:"
        << Qt::hex
        << glError;

        m_fbo->release();

        m_context->doneCurrent();

        return false;
    }

    /*
     * ---------------------------------------------------------
     * 7. Проверяем содержимое FBO
     * ---------------------------------------------------------
     */

    const QColor topLeft =
        result.pixelColor(
            0,
            0
            );

    const QColor center =
        result.pixelColor(
            result.width() / 2,
            result.height() / 2
            );

    qDebug()
        << "FBO PIXELS:"
        << "topLeft =" << topLeft
        << "center =" << center;

    /*
     * ---------------------------------------------------------
     * 8. OpenGL имеет начало координат снизу,
     *    QImage — сверху.
     * ---------------------------------------------------------
     */

    m_image =
        result.mirrored(
            false,
            true
            );

    /*
     * Освобождаем FBO.
     */
    m_fbo->release();

    /*
     * ---------------------------------------------------------
     * 9. Передаём изображение в shared memory
     * ---------------------------------------------------------
     */

    if (!SendQImageToSharedMemory(m_image))
    {
        qWarning()
        << "DxHookOverlay: failed to send frame";

        m_context->doneCurrent();

        return false;
    }

    /*
     * ---------------------------------------------------------
     * 10. Освобождаем OpenGL context
     * ---------------------------------------------------------
     */

    m_context->doneCurrent();

    return true;
}

void DxHookOverlay::runOverlay(bool gameLaunched)
{
    if (!gameLaunched)
        return;


    if (!p_gameController)
    {
        qWarning()
        << "DxHookOverlay: gameController is null";

        return;
    }


    HWND gameHwnd =
        p_gameController->gameHwnd();


    if (!gameHwnd)
    {
        qWarning()
        << "DxHookOverlay: game HWND is null";

        return;
    }


    /*
     * Получаем именно CLIENT AREA игры.
     *
     * Это лучше, чем GetWindowRect(),
     * потому что нам нужен размер области,
     * где реально рисуется игра.
     */
    RECT clientRect{};


    if (!GetClientRect(gameHwnd, &clientRect))
    {
        qWarning()
        << "DxHookOverlay: GetClientRect failed:"
        << GetLastError();

        return;
    }


    const int gameWidth =
        clientRect.right - clientRect.left;

    const int gameHeight =
        clientRect.bottom - clientRect.top;


    qDebug()
        << "Game client size:"
        << gameWidth
        << "x"
        << gameHeight;


    if (gameWidth <= 0 ||
        gameHeight <= 0)
    {
        qWarning()
        << "DxHookOverlay: invalid game size";

        return;
    }


    /*
     * Проверяем максимальный размер.
     */
    if (gameWidth >
            static_cast<int>(MAX_IMAGE_WIDTH) ||
        gameHeight >
            static_cast<int>(MAX_IMAGE_HEIGHT))
    {
        qWarning()
        << "DxHookOverlay: game is larger than"
        << MAX_IMAGE_WIDTH
        << "x"
        << MAX_IMAGE_HEIGHT;

        return;
    }


    /*
     * Теперь создаём QML/FBO именно
     * под размер игры.
     */
    if (!initialize(gameWidth, gameHeight))
    {
        qWarning()
        << "DxHookOverlay: initialize failed";

        return;
    }


    /*
     * Shared memory создаём уже
     * с реальным размером игры.
     */
    if (!CreateSharedImage(
            static_cast<uint32_t>(gameWidth),
            static_cast<uint32_t>(gameHeight)))
    {
        qWarning()
        << "DxHookOverlay: failed to create shared memory";

        return;
    }


    /*
     * Теперь внедряем DLL.
     */
    std::wstring processName =
        L"W40k.exe";


    QString appDir =
        QCoreApplication::applicationDirPath();


    QString qDllPath =
        QDir::toNativeSeparators(
            appDir + "/DxHookOverlay.dll"
            );


    std::wstring dllPath =
        qDllPath.toStdWString();


    DWORD pid =
        GetProcessIdByName(processName);


    if (pid == 0)
    {
        qWarning()
        << "DxHookOverlay: game process not found";

        return;
    }


    if (InjectDLL(pid, dllPath))
    {
        qDebug()
        << "Оверлей успешно внедрен!";
    }
    else
    {
        qWarning()
        << "Ошибка внедрения.";
    }
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


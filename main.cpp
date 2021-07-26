#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QLocale>
#include <QTranslator>
#include <dev/core/core.h>
#include "Windows.h"
#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QDebug>

HHOOK mouseHook = NULL;
HHOOK keyboardHook = NULL;
int KeyboardDelay = 0;
Core* core = NULL;

void UpdateKeySate(BYTE *keystate, int keycode)
{
    keystate[keycode] = GetKeyState(keycode);
}


LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {

    MSLLHOOKSTRUCT mKey = *((MSLLHOOKSTRUCT*)lParam);

    switch( wParam )
    {
        case WM_LBUTTONDOWN:
        {
            QMouseEvent *event = new QMouseEvent(QEvent::Type::MouseButtonPress, QPointF(mKey.pt.x, mKey.pt.y),Qt::MouseButton::LeftButton,Qt::MouseButtons(), Qt::KeyboardModifiers() );
            QGuiApplication::postEvent(core, event);
            break;
        }

        case WM_RBUTTONDOWN:
        {
            QMouseEvent *event = new QMouseEvent(QEvent::Type::MouseButtonPress, QPointF(mKey.pt.x, mKey.pt.y),Qt::MouseButton::RightButton,Qt::MouseButtons(), Qt::KeyboardModifiers() );
            QGuiApplication::postEvent(core, event);
            break;

        }

        case WM_MOUSEMOVE:{
            QMouseEvent *event = new QMouseEvent(QEvent::Type::MouseMove, QPointF(mKey.pt.x, mKey.pt.y), Qt::MouseButton(),Qt::MouseButtons(), Qt::KeyboardModifiers() );
            QGuiApplication::postEvent(core, event);
            break;

        }
    }
    return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {

    KBDLLHOOKSTRUCT cKey = *((KBDLLHOOKSTRUCT*)lParam);

    wchar_t buffer[5];

    BYTE keyboard_state[265];

    GetKeyboardState(keyboard_state);
    UpdateKeySate(keyboard_state, VK_SHIFT);
    UpdateKeySate(keyboard_state, VK_CAPITAL);
    UpdateKeySate(keyboard_state, VK_CONTROL);
    UpdateKeySate(keyboard_state, VK_MENU);

    HKL keyboard_layout = GetKeyboardLayout(0);
    char lpszName[0x100] = {0};

    DWORD dwMsg = 1;
    dwMsg += cKey.scanCode << 16;
    dwMsg += cKey.flags << 24;

    int i = GetKeyNameText(dwMsg, (LPTSTR)lpszName, 255);

    int result = ToUnicodeEx(cKey.vkCode, cKey.scanCode, keyboard_state, buffer, 4, 0, keyboard_layout);

    buffer[4] = L'\0';

    if(KeyboardDelay == 0)
    {
        QEvent *event = new QKeyEvent(QEvent::KeyPress, cKey.vkCode, Qt::NoModifier, 0);
        //core->event(event);
        QGuiApplication::postEvent(core, event);
    }

    KeyboardDelay++;
    if(KeyboardDelay == 2)
        KeyboardDelay = 0;

    qDebug()<<"key: " << cKey.vkCode << " " << QString::fromUtf16((ushort*)buffer) << " " << QString::fromUtf16((ushort*)lpszName);

    //Тут фильтруем кнопки которые блокировать не надо,типа CTRL ALT DEL
    bool isPriorityKey = false;
    if(cKey.vkCode == 192  ||
        cKey.vkCode == 27  ||
        cKey.vkCode == 9   ||
        cKey.vkCode == 20  ||
        cKey.vkCode == 160 ||
        cKey.vkCode == 162 ||
        cKey.vkCode == 91  ||
        cKey.vkCode == 164 ||
        cKey.vkCode == 165 ||
        cKey.vkCode == 163 ||
        cKey.vkCode == 161 ||
        cKey.vkCode == 13  ||
        cKey.vkCode == 46 )
    {
        isPriorityKey = true;
    }

    //qDebug() << cKey.vkCode;

    return core->ssController()->getInputBlocked() && !isPriorityKey ? 1 : CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}


int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "SSStats2_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/resources/qml/main.qml"));

    QQmlContext *context = engine.rootContext();

    core = new Core(context, &app);

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    core->startTopmost();

    mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);
    if (mouseHook == NULL) {
        //qDebug() << "MouseHook failed";
    }

    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc , NULL, 0);
    if (keyboardHook == NULL) {
        //qDebug() << "KeyboardHook failed";
    }



    return app.exec();
}

#include "hookmanager.h"
#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QSet>

HookManager *HookManager::p_instance = NULL;

HHOOK mouseHook = NULL;
HHOOK keyboardHook = NULL;
QSet<unsigned long> pressedKeysSet;

void UpdateKeySate(BYTE *keystate, int keycode)
{
    keystate[keycode] = GetKeyState(keycode);
}


LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {

    MSLLHOOKSTRUCT mKey = *((MSLLHOOKSTRUCT*)lParam);

    switch(wParam)
    {
        case WM_MOUSEWHEEL:
        {
            const MSLLHOOKSTRUCT *mllhs = (const MSLLHOOKSTRUCT *) lParam;
            short delta = HIWORD(mllhs->mouseData);
            HookManager::instance()->core()->onWheelEvent(std::move(QWheelEvent( QPointF(mKey.pt.x, mKey.pt.y), delta, Qt::MouseButtons(), Qt::KeyboardModifiers())));
            break;
        }

        case WM_LBUTTONDOWN:
        {
            HookManager::instance()->core()->onMouseEvent(std::move(QMouseEvent(QEvent::Type::MouseButtonPress, QPointF(mKey.pt.x, mKey.pt.y), Qt::MouseButton::LeftButton, Qt::MouseButtons(), Qt::KeyboardModifiers())));
            break;
        }
        case WM_RBUTTONDOWN:
        {
            HookManager::instance()->core()->onMouseEvent(std::move(QMouseEvent(QEvent::Type::MouseButtonPress, QPointF(mKey.pt.x, mKey.pt.y), Qt::MouseButton::RightButton, Qt::MouseButtons(), Qt::KeyboardModifiers())));
            break;
        }
        case WM_MOUSEMOVE:
        {
            HookManager::instance()->core()->onMouseEvent(std::move(QMouseEvent(QEvent::Type::MouseMove, QPointF(mKey.pt.x, mKey.pt.y), Qt::MouseButton(), Qt::MouseButtons(), Qt::KeyboardModifiers())));
            break;
        }    
    }

    return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {

    KBDLLHOOKSTRUCT cKey = *((KBDLLHOOKSTRUCT*)lParam);

    // Расшифровываем клавишу
    char lpszName[0x100] = {0};
    DWORD dwMsg = 1;
    dwMsg += cKey.scanCode << 16;
    dwMsg += cKey.flags << 24;
    int get_key_name_text_result = GetKeyNameText(dwMsg, (LPTSTR)lpszName, 255);

    wchar_t buffer[5];
    BYTE keyboard_state[265];
    GetKeyboardState(keyboard_state);
    UpdateKeySate(keyboard_state, VK_SHIFT);
    UpdateKeySate(keyboard_state, VK_CAPITAL);
    UpdateKeySate(keyboard_state, VK_CONTROL);
    UpdateKeySate(keyboard_state, VK_MENU);
    HKL keyboard_layout = GetKeyboardLayout(0);

    int to_unicode_ex_result = ToUnicodeEx(cKey.vkCode, cKey.scanCode, keyboard_state, buffer, 4, 0, keyboard_layout);

    Qt::KeyboardModifier modifer = Qt::NoModifier;

    if(cKey.flags == 32)
        modifer = Qt::AltModifier;


    buffer[4] = L'\0';
    // ======================

    if (WM_KEYDOWN == wParam || WM_SYSKEYDOWN == wParam)
    {
        if(!pressedKeysSet.contains(cKey.vkCode)){ // Если клавиша не была отжата и это "залипшая" клавиша, то не повторяем ее обработку
            //qDebug() << "WM_DOWN " << "key: " << cKey.vkCode << " " << QString::fromUtf16((ushort*)buffer) << " " << QString::fromUtf16((ushort*)lpszName);
            pressedKeysSet.insert(cKey.vkCode); // Добавляем код клавиши в список зажатых
            //QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, cKey.vkCode, modifer, 0);
            //emit HookManager::instance()->keyEvent(event);
            HookManager::instance()->core()->onKeyEvent(std::move(QKeyEvent(QEvent::KeyPress, cKey.vkCode, modifer, 0)));
        }
    }

    if (WM_KEYUP == wParam) {
        pressedKeysSet.remove(cKey.vkCode); // Удаляем код клавиши из списка зажатых
        //qDebug() << "WM_KEYUP " << "key: " << cKey.vkCode << " " << QString::fromUtf16((ushort*)buffer) << " " << QString::fromUtf16((ushort*)lpszName);
    }

    //Тут фильтруем кнопки которые блокировать не надо, типа CTRL ALT DEL
    bool isPriorityKey = false;
    if(cKey.vkCode == 192 || // 0xc0 - '`~' key
            cKey.vkCode == 27  || // 0x1b - ESC
            cKey.vkCode == 9   || // 0x09 - TAB
            cKey.vkCode == 20  || // 0x14 - CAPS LOCK
            cKey.vkCode == 160 || // 0xa0 - Left SHIFT
            cKey.vkCode == 162 || // 0xa2 - Left CONTROL
            cKey.vkCode == 91  || // 0x5b - Left Windows
            cKey.vkCode == 164 || // 0xa4 - Left MENU
            cKey.vkCode == 165 || // 0xa5 - Right MENU
            cKey.vkCode == 163 || // 0xa3 - Right CONTROL
            cKey.vkCode == 161 || // 0xa1 - Right SHIFT
            cKey.vkCode == 13  || // 0x0d - ENTER
            cKey.vkCode == 44  || // 0x2c - PRINT SCREEN
            cKey.vkCode == 46 )   // 0x2e - DEL
    {
        isPriorityKey = true;
    }

    //qDebug() << cKey.vkCode;

    return (HookManager::instance()->inputBlock() && !isPriorityKey) ? 1 : CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

DWORD WINAPI MyMouseLogger(LPVOID lpParm)
{
    mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);

    MSG message;
    while (GetMessage(&message, NULL, 0, 0) > 0)
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    UnhookWindowsHookEx(mouseHook);
    return 0;
}


DWORD WINAPI MyKeyboadLogger(LPVOID lpParm)
{
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);

    MSG message;
    while (GetMessage(&message, NULL, 0, 0) > 0)
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    UnhookWindowsHookEx(keyboardHook);
    return 0;
}

bool HookManager::inputBlock()
{
    return m_inputBlock;
}

void HookManager::onInputBlockStateChanged(bool state)
{
    m_inputBlock = state;
}

Core *HookManager::core() const
{
    return m_core;
}

void HookManager::setCore(Core *newCore)
{
    m_core = newCore;
    p_instance->setParent(m_core);

    m_mousehThread = NULL;
    m_mousehThread = CreateThread(NULL, 0, MyMouseLogger, NULL, 0, NULL);

    m_keyboradThread = NULL;
    m_keyboradThread = CreateThread(NULL, 0, MyKeyboadLogger, NULL, 0, NULL);


   // if (m_mousehThread != NULL )
  //      WaitForSingleObject(m_mousehThread, /*INFINITE*/1000);
  //  else
  //      Q_ASSERT(false);

}


void HookManager::reconnectHook()
{
    ///UnhookWindowsHookEx(mouseHook);
   /// UnhookWindowsHookEx(keyboardHook);



    //mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);
  ////  mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, GetModuleHandle(NULL), NULL);
    //Q_ASSERT_X(mouseHook, "HookManager", "Mouse Hook failed");
    /*if (mouseHook == NULL)
        qDebug() << "INFO: MouseHook failed";
    else
        qDebug() << "INFO: MouseHook accepted";*/

    //keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc , NULL, 0);
  ////  keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc , GetModuleHandle(NULL), NULL);
    //Q_ASSERT_X(keyboardHook, "HookManager", "Keyboard Hook failed");
   /* if (keyboardHook == NULL)
        qDebug() << "INFO: KeyboardHook failed";
    else
        qDebug() << "INFO: KeyboardHook accepted";*/
}


HookManager::HookManager()
{
    m_inputBlock = false;


    //mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);
   //// mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, GetModuleHandle(NULL), NULL);
    //Q_ASSERT_X(mouseHook, "HookManager", "Mouse Hook failed");
    /*if (mouseHook == NULL)
        qDebug() << "INFO: MouseHook failed";
    else
        qDebug() << "INFO: MouseHook accepted";*/

    //keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc , NULL, 0);
   //// keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc , GetModuleHandle(NULL), NULL);
    //Q_ASSERT_X(keyboardHook, "HookManager", "Keyboard Hook failed");
   /* if (keyboardHook == NULL)
        qDebug() << "INFO: KeyboardHook failed";
    else
        qDebug() << "INFO: KeyboardHook accepted";*/
}

HookManager::~HookManager()
{
    UnhookWindowsHookEx(mouseHook);
    UnhookWindowsHookEx(keyboardHook);

    CloseHandle(m_mousehThread);
    CloseHandle(m_keyboradThread);
}

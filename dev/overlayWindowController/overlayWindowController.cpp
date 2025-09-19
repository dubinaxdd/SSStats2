#include "overlayWindowController.h"
#include <QTextCodec>

OverlayWindowController::OverlayWindowController(SettingsController *settingsController, GameController* soulstormController, QObject *parent)
    : QObject(parent)
    , p_gameController(soulstormController)
    , p_settingsController(settingsController)
{
    m_topmostTimer = new QTimer(this);
    m_topmostTimer->setInterval(500);

    QObject::connect(m_topmostTimer, &QTimer::timeout, this, &OverlayWindowController::topmostTimerTimout, Qt::QueuedConnection);
}

void OverlayWindowController::grubStatsWindow()
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString s = codec->toUnicode("DowStatsClientOverlay");
    LPCWSTR lps = (LPCWSTR)s.utf16();

    m_dowStatsHwnd = FindWindowW(NULL, lps);

    m_defaultWidth = GetSystemMetrics(SM_CXSCREEN);
    m_defaultHeight = GetSystemMetrics(SM_CYSCREEN);

    m_defaultWindowLong = GetWindowLongPtr(m_dowStatsHwnd, GWL_EXSTYLE);
    //m_defaultWindowLong = GetWindowLongPtr(m_dowStatsHwnd, GWL_STYLE);



    SetWindowPos(m_dowStatsHwnd, HWND_BOTTOM, m_gameRect.left, m_gameRect.top, m_gameRect.right - m_gameRect.left, m_gameRect.bottom - m_gameRect.top, m_defaultWindowLong );

    if (m_uiBackendPtr)
        m_uiBackendPtr->setWindowTopmost(false);

}

void OverlayWindowController::topmostTimerTimout()
{
    //TODO
    //Костыль для того чтоб форма всегда была с верху сса,
    //ибо директикс сса по таймеру обновляет флаги стиля окна, конкретнро WS_EX_TOPMOST
    //установка флага Qt::WindowStaysOnTopHint на кородкое время выводет икно повер сса
    //Соответственно затираем флаг и выставлем заного по таймеру.
    //Время устанавливаемое таймеру возможно придется менять из за разницы систем, надо тестить
    if (!m_uiBackendPtr)
        return;

    if (!p_settingsController->getSettings()->overlayVisible)
        return;

    if (p_gameController->gameWindowed())
        m_topmostTimer->setInterval(1500);
    else
        m_topmostTimer->setInterval(500);

    if (p_gameController->gameStateReader()->getGameInitialized())
    {
        if (m_dowStatsHwnd)
        {
            if (p_gameController->gameWindowed())
            {

                RECT ssRect;
                if (GetWindowRect(p_gameController->gameHwnd(), &ssRect))
                {

                   int titleBarHeight = getGameTitleBarHeight();

                    m_gameRect = ssRect;
                    SetWindowPos(m_dowStatsHwnd, HWND_TOP, m_gameRect.left, m_gameRect.top + titleBarHeight, m_gameRect.right - m_gameRect.left, m_gameRect.bottom - m_gameRect.top - titleBarHeight, m_defaultWindowLong );

                    m_uiBackendPtr->setSsWindowPosition(m_gameRect.left, m_gameRect.top + titleBarHeight);

                    LONG ssLong = GetWindowLongPtr(p_gameController->gameHwnd(), 0);
                    SetWindowPos(p_gameController->gameHwnd(), m_dowStatsHwnd, ssRect.left, ssRect.top, ssRect.right - ssRect.left, ssRect.bottom - ssRect.top, ssLong );

                    m_uiBackendPtr->setGameWindowed(p_gameController->gameWindowed());
                }
            }
            else if(p_gameController->currentGame()->gameType == GameType::DefinitiveEdition)
            {
                /*RECT ssRect;
                if (GetWindowRect(p_gameController->gameHwnd(), &ssRect))
                {
                    int titleBarHeight = getGameTitleBarHeight();

                    m_gameRect = ssRect;
                    SetWindowPos(m_dowStatsHwnd, HWND_TOP, m_gameRect.left, m_gameRect.top + titleBarHeight, m_gameRect.right - m_gameRect.left, m_gameRect.bottom - m_gameRect.top - titleBarHeight, m_defaultWindowLong );
                }*/
            }
            else
            {
                RECT ssRect;
                {
                    if (GetWindowRect(p_gameController->gameHwnd(), &ssRect))
                    {
                        if (m_gameRect.bottom != ssRect.bottom || m_gameRect.top != ssRect.top || m_gameRect.right != ssRect.right || m_gameRect.left)
                        {
                            m_gameRect = ssRect;
                            SetWindowPos(m_dowStatsHwnd, HWND_TOPMOST, m_gameRect.left, m_gameRect.top, m_gameRect.right - m_gameRect.left, m_gameRect.bottom - m_gameRect.top, m_defaultWindowLong);
                        }
                    }
                }

                BringWindowToTop(m_dowStatsHwnd);
            }
        }
    }
}

int OverlayWindowController::getGameTitleBarHeight()
{
    if (!p_gameController->gameHwnd())
        return 0;

    RECT ssRect;
    if (GetWindowRect(p_gameController->gameHwnd(), &ssRect))
    {

        RECT crect;
        GetClientRect( p_gameController->gameHwnd(), &crect );

        int titleHeight = (ssRect.bottom - ssRect.top) - (crect.bottom - crect.top);

        if (titleHeight > 4)
            titleHeight = titleHeight - 4;

        return titleHeight;
    }

    return 0;
}

void OverlayWindowController::ssMaximized(bool maximized)
{
    if (!m_uiBackendPtr)
        return;

    if (!p_settingsController->getSettings()->overlayVisible)
        return;

    if (maximized)
    {
        int width =  GetSystemMetrics(SM_CXSCREEN);
        int height = GetSystemMetrics(SM_CYSCREEN);

        m_widthInGame = width;
        m_heightInGame = height;

        m_uiBackendPtr->setMouseArea(width, height);

        if (!p_gameController->gameWindowed())
        {
            RECT ssRect;
            if (GetWindowRect(p_gameController->gameHwnd(), &ssRect))
            {
                m_gameRect = ssRect;
                SetWindowPos(m_dowStatsHwnd, HWND_TOPMOST, m_gameRect.left, m_gameRect.top, m_gameRect.right - m_gameRect.left, m_gameRect.bottom - m_gameRect.top, m_defaultWindowLong);
                //SetWindowPos(m_dowStatsHwnd, HWND_TOP, m_gameRect.left, m_gameRect.top, m_gameRect.right - m_gameRect.left, m_gameRect.bottom - m_gameRect.top, m_defaultWindowLong);

                m_uiBackendPtr->setWindowTopmost(true);
            }
            m_topmostTimer->start();
            m_uiBackendPtr->setGameWindowed(p_gameController->gameWindowed());
        }
        else
        {
            RECT ssRect;
            if (GetWindowRect(p_gameController->gameHwnd(), &ssRect))
            {
                if(m_gameRect.bottom != ssRect.bottom || m_gameRect.top != ssRect.top || m_gameRect.right != ssRect.right || m_gameRect.left != ssRect.left)
                {
                    int titleBarHeight = getGameTitleBarHeight();

                    m_gameRect = ssRect;
                    SetWindowPos(m_dowStatsHwnd, p_gameController->gameHwnd(), m_gameRect.left, m_gameRect.top + titleBarHeight, m_gameRect.right - m_gameRect.left, m_gameRect.bottom - m_gameRect.top - titleBarHeight, m_defaultWindowLong );

                    LONG ssLong = GetWindowLongPtr(p_gameController->gameHwnd(), 0);
                    SetWindowPos(p_gameController->gameHwnd(), m_dowStatsHwnd, m_gameRect.left, m_gameRect.top, m_gameRect.right - m_gameRect.left, m_gameRect.bottom - m_gameRect.top, ssLong );

                    m_uiBackendPtr->setSsWindowPosition(m_gameRect.left, m_gameRect.top + titleBarHeight);
                }
            }
            m_uiBackendPtr->setGameWindowed(p_gameController->gameWindowed());
        }

        m_topmostTimer->start();
    }
    else
    {
        m_topmostTimer->stop();
        SetWindowPos(m_dowStatsHwnd, HWND_BOTTOM, 0,0,0,0, m_defaultWindowLong );
        m_uiBackendPtr->setWindowTopmost(false);
    }

    m_uiBackendPtr->setGameWindowed(p_gameController->gameWindowed());
}

void OverlayWindowController::gameInitialized()
{
   // HookManager::instance()->reconnectHook();

    m_topmostTimer->start();

    if(p_gameController->getSsMaximized() && (GetSystemMetrics(SM_CXSCREEN) != m_widthInGame || GetSystemMetrics(SM_CYSCREEN) != m_heightInGame))
    {
        ssMaximized(true);
    }
}

void OverlayWindowController::gameLaunched(bool ssLaunched)
{
    if (!m_uiBackendPtr)
        return;

    if (!ssLaunched)
    {
        m_topmostTimer->stop();
        SetWindowPos(m_dowStatsHwnd, HWND_BOTTOM, m_gameRect.left, m_gameRect.top, m_gameRect.right - m_gameRect.left, m_gameRect.bottom - m_gameRect.top, m_defaultWindowLong );
        m_uiBackendPtr->setWindowTopmost(false);
    }
}

void OverlayWindowController::onSsShutdowned()
{
    ssMaximized(false);
}

void OverlayWindowController::onExit()
{
    m_topmostTimer->stop();

    ssMaximized(false);

    if(p_gameController->currentGame()->gameType == GameType::DefinitiveEdition)
        return;

    if(p_gameController->gameHwnd())
    {
        RECT ssRect;
        if (GetWindowRect(p_gameController->gameHwnd(), &ssRect))
            SetWindowPos(p_gameController->gameHwnd(), HWND_TOPMOST, ssRect.left, ssRect.top, ssRect.right - ssRect.left, ssRect.bottom - ssRect.top, p_gameController->defaultGameWindowLong() );

        BringWindowToTop(p_gameController->gameHwnd());
    }
}

void OverlayWindowController::setUiBackend(UiBackend *uiBackend)
{
    m_uiBackendPtr = uiBackend;

    if (!m_uiBackendPtr)
        return;

    m_topmostTimer->stop();
    SetWindowPos(m_dowStatsHwnd, HWND_BOTTOM, m_gameRect.left, m_gameRect.top, m_gameRect.right - m_gameRect.left, m_gameRect.bottom - m_gameRect.top, m_defaultWindowLong );
    m_uiBackendPtr->setWindowTopmost(false);
}

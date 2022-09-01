#include "overlayWindowController.h"
#include <QTextCodec>

OverlayWindowController::OverlayWindowController(UiBackend* uiBackend, SoulstormController* soulstormController, QObject *parent)
    : QObject(parent)
    , p_uiBackend(uiBackend)
    , p_soulstormController(soulstormController)
{
    m_topmostTimer = new QTimer();
    m_topmostTimer->setInterval(500);

    QObject::connect(m_topmostTimer, &QTimer::timeout, this, &OverlayWindowController::topmostTimerTimout, Qt::QueuedConnection);
}

void OverlayWindowController::grubStatsWindow()
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString s = codec->toUnicode("DowStats2Overlay");
    LPCWSTR lps = (LPCWSTR)s.utf16();

    m_ssStatsHwnd = FindWindowW(NULL, lps);

    m_defaultWidth = GetSystemMetrics(SM_CXSCREEN);
    m_defaultHeight = GetSystemMetrics(SM_CYSCREEN);

    m_defaultWindowLong = GetWindowLongPtr(m_ssStatsHwnd, GWL_EXSTYLE);
    SetWindowPos(m_ssStatsHwnd, HWND_BOTTOM, m_ssRect.left, m_ssRect.top, m_ssRect.right - m_ssRect.left, m_ssRect.bottom - m_ssRect.top, m_defaultWindowLong );
    p_uiBackend->setWindowTopmost(false);
}

void OverlayWindowController::topmostTimerTimout()
{
    //TODO
    //Костыль для того чтоб форма всегда была с верху сса,
    //ибо директикс сса по таймеру обновляет флаги стиля окна, конкретнро WS_EX_TOPMOST
    //установка флага Qt::WindowStaysOnTopHint на кородкое время выводет икно повер сса
    //Соответственно затираем флаг и выставлем заного по таймеру.
    //Время устанавливаемое таймеру возможно придется менять из за разницы систем, надо тестить

    if (p_soulstormController->gameStateReader()->getGameInitialized())
    {
        if (m_ssStatsHwnd)
        {
            if (p_soulstormController->ssWindowed())
            {
                RECT ssRect;
                if (GetWindowRect(p_soulstormController->soulstormHwnd(), &ssRect))
                {

                    if(m_ssRect.bottom != ssRect.bottom || m_ssRect.top != ssRect.top || m_ssRect.right != ssRect.right || m_ssRect.left != ssRect.left)
                    {
                        m_ssRect = ssRect;
                        SetWindowPos(m_ssStatsHwnd, p_soulstormController->soulstormHwnd(), m_ssRect.left, m_ssRect.top, m_ssRect.right - m_ssRect.left, m_ssRect.bottom - m_ssRect.top, m_defaultWindowLong );

                        p_uiBackend->setSsWindowPosition(m_ssRect.left, m_ssRect.top);
                    }


                    LONG ssLong = GetWindowLongPtr(p_soulstormController->soulstormHwnd(), 0);
                    SetWindowPos(p_soulstormController->soulstormHwnd(), m_ssStatsHwnd, ssRect.left, ssRect.top, ssRect.right - ssRect.left, ssRect.bottom - ssRect.top, ssLong );

                    p_uiBackend->setSsWindowed(p_soulstormController->ssWindowed());
                }
            }
            else
            {
                RECT ssRect;
                {
                    if (GetWindowRect(p_soulstormController->soulstormHwnd(), &ssRect))
                    {
                        if (m_ssRect.bottom != ssRect.bottom || m_ssRect.top != ssRect.top || m_ssRect.right != ssRect.right || m_ssRect.left)
                        {
                            m_ssRect = ssRect;
                            SetWindowPos(m_ssStatsHwnd, HWND_TOPMOST, m_ssRect.left, m_ssRect.top, m_ssRect.right - m_ssRect.left, m_ssRect.bottom - m_ssRect.top, m_defaultWindowLong);
                        }
                    }
                }

                BringWindowToTop(m_ssStatsHwnd);
            }
        }
    }
}

void OverlayWindowController::ssMaximized(bool maximized)
{
    if (maximized)
    {
        int width =  GetSystemMetrics(SM_CXSCREEN);
        int height = GetSystemMetrics(SM_CYSCREEN);

        m_widthInGame = width;
        m_heightInGame = height;

        p_uiBackend->setMouseArea(width, height);

        if (!p_soulstormController->ssWindowed())
        {
            RECT ssRect;
            if (GetWindowRect(p_soulstormController->soulstormHwnd(), &ssRect))
            {
                m_ssRect = ssRect;
                SetWindowPos(m_ssStatsHwnd, HWND_TOPMOST, m_ssRect.left, m_ssRect.top, m_ssRect.right - m_ssRect.left, m_ssRect.bottom - m_ssRect.top, m_defaultWindowLong);
                p_uiBackend->setWindowTopmost(true);
            }
            m_topmostTimer->start();
            p_uiBackend->setSsWindowed(p_soulstormController->ssWindowed());
        }
        else
        {
            RECT ssRect;
            if (GetWindowRect(p_soulstormController->soulstormHwnd(), &ssRect))
            {
                if(m_ssRect.bottom != ssRect.bottom || m_ssRect.top != ssRect.top || m_ssRect.right != ssRect.right || m_ssRect.left != ssRect.left)
                {
                        m_ssRect = ssRect;
                        SetWindowPos(m_ssStatsHwnd, p_soulstormController->soulstormHwnd(), m_ssRect.left, m_ssRect.top, m_ssRect.right - m_ssRect.left, m_ssRect.bottom - m_ssRect.top, m_defaultWindowLong );

                        LONG ssLong = GetWindowLongPtr(p_soulstormController->soulstormHwnd(), 0);
                        SetWindowPos(p_soulstormController->soulstormHwnd(), m_ssStatsHwnd, m_ssRect.left, m_ssRect.top, m_ssRect.right - m_ssRect.left, m_ssRect.bottom - m_ssRect.top, ssLong );

                        p_uiBackend->setSsWindowPosition(m_ssRect.left, m_ssRect.top);
                }
            }
            p_uiBackend->setSsWindowed(p_soulstormController->ssWindowed());
        }

        m_topmostTimer->start();
    }
    else
    {
        m_topmostTimer->stop();
        SetWindowPos(m_ssStatsHwnd, HWND_BOTTOM, 0,0,0,0, m_defaultWindowLong );
        p_uiBackend->setWindowTopmost(false);
    }

    p_uiBackend->setSsWindowed(p_soulstormController->ssWindowed());
}

void OverlayWindowController::gameInitialized()
{
   // HookManager::instance()->reconnectHook();

    m_topmostTimer->start();

    if(p_soulstormController->getSsMaximized() && (GetSystemMetrics(SM_CXSCREEN) != m_widthInGame || GetSystemMetrics(SM_CYSCREEN) != m_heightInGame))
    {
        ssMaximized(true);
    }
}

void OverlayWindowController::ssLaunched(bool ssLaunched)
{
    if (!ssLaunched)
    {
        m_topmostTimer->stop();
        SetWindowPos(m_ssStatsHwnd, HWND_BOTTOM, m_ssRect.left, m_ssRect.top, m_ssRect.right - m_ssRect.left, m_ssRect.bottom - m_ssRect.top, m_defaultWindowLong );
        p_uiBackend->setWindowTopmost(false);
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

    if(p_soulstormController->soulstormHwnd())
    {
        RECT ssRect;
        if (GetWindowRect(p_soulstormController->soulstormHwnd(), &ssRect))
            SetWindowPos(p_soulstormController->soulstormHwnd(), HWND_TOPMOST, ssRect.left, ssRect.top, ssRect.right - ssRect.left, ssRect.bottom - ssRect.top, p_soulstormController->defaultSoulstormWindowLong() );

        BringWindowToTop(p_soulstormController->soulstormHwnd());
    }
}

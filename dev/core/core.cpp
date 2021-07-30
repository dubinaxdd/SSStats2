#include "core.h"
#include "QDebug"
#include "QFile"
#include "QSettings"
#include "../ssConroller/gameInfoReader/gameinforeader.h"


Core::Core(QQmlContext *context, QObject* parent)
    : QObject(parent)
    , m_keyboardProcessor(new KeyboardProcessor(this))
    , m_uiBackend(new UiBackend(context))
    , m_ssController(new SsController(this))
{
    context->setContextProperty("_uiBackend", m_uiBackend);

    m_topmostTimer = new QTimer();
    m_topmostTimer->setInterval(100);
    connect(m_topmostTimer, &QTimer::timeout, this, &Core::topmostTimerTimout, Qt::QueuedConnection);

    QObject::connect(m_keyboardProcessor, &KeyboardProcessor::expandKeyPressed, m_uiBackend, &UiBackend::expandKeyPressed, Qt::QueuedConnection);
    QObject::connect(m_uiBackend, &UiBackend::sendExpand, m_ssController, &SsController::blockInput, Qt::QueuedConnection );
    QObject::connect(m_ssController, &SsController::ssLounched, m_uiBackend, &UiBackend::receiveSsLounched, Qt::QueuedConnection );
    QObject::connect(m_ssController, &SsController::ssMaximized, m_uiBackend, &UiBackend::receiveSsMaximized, Qt::QueuedConnection );
    QObject::connect(m_ssController, &SsController::ssMaximized, this, &Core::ssMaximized, Qt::DirectConnection );

    QObject::connect(m_ssController->gameInfoReader(), &GameInfoReader::gameInitialized, this, &Core::gameInitialized, Qt::DirectConnection );

}


void Core::topmostTimerTimout()
{
    //TODO
    //Костыль для того чтоб форма всегда была с верху сса,
    //ибо директикс сса по таймеру обновляет флаги стиля окна, конкретнро WS_EX_TOPMOST
    //установка флага Qt::WindowStaysOnTopHint на кородкое время выводет икно повер сса
    //Соответственно затираем флаг и выставлем заного по таймеру.
    //Время устанавливаемое таймеру возможно придется менять из за разницы систем, надо тестить
    if (m_ssStatsHwnd){
        BringWindowToTop(m_ssStatsHwnd);
    }
}


void Core::ssMaximized(bool maximized)
{


    if (maximized)
    {
        //m_widthInGame =  GetSystemMetrics(SM_CXSCREEN);
        //m_heightInGame = GetSystemMetrics(SM_CYSCREEN);

        int width =  GetSystemMetrics(SM_CXSCREEN);
        int height = GetSystemMetrics(SM_CYSCREEN);

        m_widthInGame = width;
        m_heightInGame = height;

       /* DEVMODE devmode;
        devmode.dmPelsWidth = width;
        devmode.dmPelsHeight = height;
        devmode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
        devmode.dmSize = sizeof(DEVMODE);

        long result = ChangeDisplaySettings(&devmode, 0);*/

       // SetWindowLongA(m_ssStatsHwnd, 0, result);

        SetWindowPos(m_ssStatsHwnd, HWND_TOP, 0, 0, width, height, m_defaultWindowLong );
        //m_uiBackend->setWindowRect(width, height);
        m_uiBackend->setWindowTopmost(true);
        m_topmostTimer->start();
    }
    else
    {
       /* DEVMODE devmode;
        devmode.dmPelsWidth = m_defaultWidth;
        devmode.dmPelsHeight = m_defaultHeight;
        devmode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
        devmode.dmSize = sizeof(DEVMODE);

        long result = ChangeDisplaySettings(&devmode, 0);*/
       // SetWindowLongA(m_ssStatsHwnd, 0, result);

        //SetWindowLongA(m_ssStatsHwnd, 0, m_defaultWindowLong);

        m_topmostTimer->stop();
        SetWindowPos(m_ssStatsHwnd, HWND_BOTTOM, 0, 0, m_defaultWidth, m_defaultHeight, m_defaultWindowLong );
        //m_uiBackend->setWindowRect(m_defaultWidth, m_defaultHeight);
        m_uiBackend->setWindowTopmost(false);

    }
}

void Core::gameInitialized()
{

    if(m_ssController->getSsMaximized() && (GetSystemMetrics(SM_CXSCREEN) != m_widthInGame || GetSystemMetrics(SM_CYSCREEN) != m_heightInGame))
    {
        ssMaximized(true);
    }
}

UiBackend *Core::uiBackend() const
{
    return m_uiBackend;
}

SsController *Core::ssController() const
{
    return m_ssController;
}

bool Core::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress && m_keyboardProcessor)
    {
        QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);
        m_keyboardProcessor->keyPressEvent(keyEvent);
        return true;
    }

     if (event->type() == QEvent::MouseButtonPress)
     {
         QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
         m_uiBackend->mousePressEvent(mouseEvent->pos());
         return true;
     }

    QObject::event(event);
    return false;

}

void Core::grubStatsWindow()
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString s = codec->toUnicode("SSStats2");
    LPCWSTR lps = (LPCWSTR)s.utf16();

    m_ssStatsHwnd = FindWindowW(NULL, lps);

    m_defaultWidth = GetSystemMetrics(SM_CXSCREEN);
    m_defaultHeight = GetSystemMetrics(SM_CYSCREEN);

    m_defaultWindowLong = GetWindowLong(m_ssStatsHwnd, GWL_EXSTYLE);

    m_uiBackend->setWindowTopmost(false);
}

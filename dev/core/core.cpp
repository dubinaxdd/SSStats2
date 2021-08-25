#include "core.h"
#include "QDebug"
#include "QFile"
#include "../ssConroller/gameInfoReader/gameinforeader.h"
#include "../baseTypes/baseTypes.h"


Core::Core(QQmlContext *context, QObject* parent)
    : QObject(parent)
    , m_keyboardProcessor(new KeyboardProcessor(this))
    , m_settingsController(new SettingsController(this))
    , m_uiBackend(new UiBackend(context))
    , m_ssController(new SsController(this))
{
    registerTypes();

    context->setContextProperty("_uiBackend", m_uiBackend);

    m_topmostTimer = new QTimer();
    m_topmostTimer->setInterval(500);
    connect(m_topmostTimer, &QTimer::timeout, this, &Core::topmostTimerTimout, Qt::QueuedConnection);


    QObject::connect(m_ssController, &SsController::ssMaximized,            this,                       &Core::ssMaximized,                             Qt::DirectConnection);
    QObject::connect(m_ssController, &SsController::ssLaunchStateChanged,   this,                       &Core::ssLaunched,                              Qt::QueuedConnection);
    QObject::connect(m_ssController, &SsController::ssLaunchStateChanged,   m_uiBackend,                &UiBackend::onSsLaunchStateChanged,             Qt::QueuedConnection);
    QObject::connect(m_ssController, &SsController::ssMaximized,            m_uiBackend,                &UiBackend::receiveSsMaximized,                 Qt::QueuedConnection);
    QObject::connect(m_ssController, &SsController::sendPlayersTestStats,   m_uiBackend->gamePanel(),   &GamePanel::receivePlayersTestStats,            Qt::QueuedConnection);
    QObject::connect(m_ssController, &SsController::ssLaunchStateChanged,   m_settingsController,       &SettingsController::onSsLaunchStateChanged,    Qt::QueuedConnection);

    QObject::connect(m_ssController->gameInfoReader(),  &GameInfoReader::gameInitialized,       this,                       &Core::gameInitialized,         Qt::DirectConnection);
    QObject::connect(m_ssController->gameInfoReader(),  &GameInfoReader::gameStarted,           m_uiBackend,                &UiBackend::onGameStarted,      Qt::QueuedConnection);
    QObject::connect(m_ssController->gameInfoReader(),  &GameInfoReader::gameStopped,           m_uiBackend,                &UiBackend::onGameStopped,      Qt::QueuedConnection);
    QObject::connect(m_ssController->gameInfoReader(),  &GameInfoReader::startingMission,       m_uiBackend,                &UiBackend::onStartingMission,  Qt::QueuedConnection);
    QObject::connect(m_ssController->gameInfoReader(),  &GameInfoReader::gameStarted,           m_ssController->apmMeter(), &APMMeter::onGameStarted,       Qt::QueuedConnection);
    QObject::connect(m_ssController->gameInfoReader(),  &GameInfoReader::gameLoaded,            m_ssController->apmMeter(), &APMMeter::onGameStarted,       Qt::QueuedConnection);
    QObject::connect(m_ssController->gameInfoReader(),  &GameInfoReader::gameStopped,           m_ssController->apmMeter(), &APMMeter::onGameStopped,       Qt::QueuedConnection);

    QObject::connect(m_ssController->apmMeter(),        &APMMeter::currentApmCalculated,        m_uiBackend->gamePanel(),       &GamePanel::onCurrentApmChanged,            Qt::QueuedConnection);
    QObject::connect(m_ssController->apmMeter(),        &APMMeter::averageApmCalculated,        m_uiBackend->gamePanel(),       &GamePanel::onAverageApmChanged,            Qt::QueuedConnection);

    QObject::connect(m_ssController->statsCollector(),  &StatsCollector::sendServerPlayrStats,  m_uiBackend->statisticPanel(),  &StatisticPanel::receiveServerPlayerStats,  Qt::QueuedConnection);
    QObject::connect(m_ssController->statsCollector(),  &StatsCollector::sendPlayersCount,  m_uiBackend->statisticPanel(),  &StatisticPanel::receivePlayersCount,  Qt::QueuedConnection);
    QObject::connect(m_ssController->playersSteamScanner(),  &PlayersSteamScanner::sendSteamPlayersInfoMap,  m_uiBackend->statisticPanel(),  &StatisticPanel::receivePlayersInfoMapFromScanner,  Qt::QueuedConnection);

    QObject::connect(m_keyboardProcessor, &KeyboardProcessor::expandKeyPressed, m_uiBackend, &UiBackend::expandKeyPressed, Qt::QueuedConnection);

    QObject::connect(m_uiBackend, &UiBackend::sendExpand, m_ssController, &SsController::blockInput, Qt::QueuedConnection);
    QObject::connect(m_uiBackend, &UiBackend::switchNoFogStateChanged, m_settingsController, &SettingsController::onSwitchNoFogStateChanged, Qt::QueuedConnection);

    QObject::connect(m_settingsController, &SettingsController::noFogStateInitialized, m_uiBackend, &UiBackend::onNoFogStateChanged, Qt::QueuedConnection);
    QObject::connect(m_settingsController, &SettingsController::noFogStateChanged, m_ssController->memoryController(), &MemoryController::onNoFogStateChanged, Qt::QueuedConnection);
    QObject::connect(m_settingsController, &SettingsController::noFogStateInitialized, m_ssController->memoryController(), &MemoryController::onNoFogStateChanged, Qt::QueuedConnection);
}

void Core::topmostTimerTimout()
{
    //TODO
    //Костыль для того чтоб форма всегда была с верху сса,
    //ибо директикс сса по таймеру обновляет флаги стиля окна, конкретнро WS_EX_TOPMOST
    //установка флага Qt::WindowStaysOnTopHint на кородкое время выводет икно повер сса
    //Соответственно затираем флаг и выставлем заного по таймеру.
    //Время устанавливаемое таймеру возможно придется менять из за разницы систем, надо тестить

    if (m_ssController->gameInfoReader()->getGameInitialized())
    {
        if (m_ssStatsHwnd){
            if (m_ssController->ssWindowed())
            {
                RECT ssRect;
                if (GetWindowRect(m_ssController->soulstormHwnd(), &ssRect))
                {

                    if(m_ssRect.bottom != ssRect.bottom || m_ssRect.top != ssRect.top || m_ssRect.right != ssRect.right || m_ssRect.left != ssRect.left)
                    {
                        m_ssRect = ssRect;
                        SetWindowPos(m_ssStatsHwnd, m_ssController->soulstormHwnd(), ssRect.left, ssRect.top, ssRect.right - ssRect.left, ssRect.bottom - ssRect.top, m_defaultWindowLong );

                        m_uiBackend->setSsWindowPosition(m_ssRect.left, m_ssRect.top);
                    }

                    LONG ssLong = GetWindowLongPtr(m_ssController->soulstormHwnd(), 0);
                    SetWindowPos(m_ssController->soulstormHwnd(), m_ssStatsHwnd, ssRect.left, ssRect.top, ssRect.right - ssRect.left, ssRect.bottom - ssRect.top, ssLong );

                    m_uiBackend->setSsWindowed(m_ssController->ssWindowed());


                }
           }
           BringWindowToTop(m_ssStatsHwnd);
        }
    }

}


void Core::ssMaximized(bool maximized)
{
    if (maximized)
    {
        int width =  GetSystemMetrics(SM_CXSCREEN);
        int height = GetSystemMetrics(SM_CYSCREEN);

        m_widthInGame = width;
        m_heightInGame = height;

        m_uiBackend->setMouseArea(width, height);

        if (!m_ssController->ssWindowed())
        {
            RECT ssRect;
            if (GetWindowRect(m_ssController->soulstormHwnd(), &ssRect))
            {
                m_ssRect = ssRect;
                //MoveWindow(m_ssStatsHwnd, ssRect.left, ssRect.top, ssRect.right - ssRect.left, ssRect.bottom - ssRect.top, true);
                SetWindowPos(m_ssStatsHwnd, HWND_TOPMOST, ssRect.left, ssRect.top, ssRect.right - ssRect.left, ssRect.bottom - ssRect.top, m_defaultWindowLong );
                m_uiBackend->setWindowTopmost(true);
            }
            m_topmostTimer->start();
            m_uiBackend->setSsWindowed(m_ssController->ssWindowed());
        }
        else
        {
            RECT ssRect;
            if (GetWindowRect(m_ssController->soulstormHwnd(), &ssRect))
            {
                if(m_ssRect.bottom != ssRect.bottom || m_ssRect.top != ssRect.top || m_ssRect.right != ssRect.right || m_ssRect.left != ssRect.left)
                {
                    m_ssRect = ssRect;
                    SetWindowPos(m_ssStatsHwnd, m_ssController->soulstormHwnd(), ssRect.left, ssRect.top, ssRect.right - ssRect.left, ssRect.bottom - ssRect.top, m_defaultWindowLong );

                    LONG ssLong = GetWindowLongPtr(m_ssController->soulstormHwnd(), 0);
                    SetWindowPos(m_ssController->soulstormHwnd(), m_ssStatsHwnd, ssRect.left, ssRect.top, ssRect.right - ssRect.left, ssRect.bottom - ssRect.top, ssLong );
                    //m_uiBackend->setWindowedMode();

                    m_uiBackend->setSsWindowPosition(m_ssRect.left, m_ssRect.top);
                }
            }
            m_uiBackend->setSsWindowed(m_ssController->ssWindowed());
        }


        m_topmostTimer->start();
    }
    else
    {
        m_topmostTimer->stop();
        SetWindowPos(m_ssStatsHwnd, HWND_BOTTOM, m_ssRect.left, m_ssRect.top, m_ssRect.right - m_ssRect.left, m_ssRect.bottom - m_ssRect.top, m_defaultWindowLong );
        m_uiBackend->setWindowTopmost(false);
    }

    m_uiBackend->setSsWindowed(m_ssController->ssWindowed());
}

void Core::gameInitialized()
{
    m_topmostTimer->start();

    if(m_ssController->getSsMaximized() && (GetSystemMetrics(SM_CXSCREEN) != m_widthInGame || GetSystemMetrics(SM_CYSCREEN) != m_heightInGame))
    {
        ssMaximized(true);
    }
}

void Core::ssLaunched(bool ssLaunched)
{
    if (!ssLaunched)
    {
        m_topmostTimer->stop();
        SetWindowPos(m_ssStatsHwnd, HWND_BOTTOM, m_ssRect.left, m_ssRect.top, m_ssRect.right - m_ssRect.left, m_ssRect.bottom - m_ssRect.top, m_defaultWindowLong );
        m_uiBackend->setWindowTopmost(false);
    }
}

void Core::registerTypes()
{
    qRegisterMetaType<QVector<PlayerStats>>("QVector<PlayerStats>");
    qRegisterMetaType<ServerPlayerStats>("ServerPlayerStats");
    qRegisterMetaType<QList<SearchStemIdPlayerInfo>>("QList<SearchStemIdPlayerInfo>");

}

UiBackend *Core::uiBackend() const
{
    return m_uiBackend;
}

SsController *Core::ssController() const
{
    return m_ssController;
}

SettingsController *Core::settingsController() const
{
    return m_settingsController;
}

bool Core::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress && m_keyboardProcessor)
    {
        QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);
        m_keyboardProcessor->keyPressEvent(keyEvent);
        m_ssController->apmMeter()->onKeyPressEvent(keyEvent);
        return true;
    }

     if (event->type() == QEvent::MouseButtonPress)
     {
         QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
         m_uiBackend->mousePressEvent(mouseEvent->pos());
         m_ssController->apmMeter()->onMousePressEvent(mouseEvent->pos());
         return true;
     }

     if (event->type() == QEvent::MouseMove)
     {
         QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
         m_uiBackend->mouseMoveEvent(mouseEvent->pos());
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

    m_defaultWindowLong = GetWindowLongPtr(m_ssStatsHwnd, GWL_EXSTYLE);
    SetWindowPos(m_ssStatsHwnd, HWND_BOTTOM, m_ssRect.left, m_ssRect.top, m_ssRect.right - m_ssRect.left, m_ssRect.bottom - m_ssRect.top, m_defaultWindowLong );
    m_uiBackend->setWindowTopmost(false);
}

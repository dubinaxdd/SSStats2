#include <core.h>
#include <QDebug>
#include <QFile>
#include <gameinforeader.h>
#include <lobbyeventreader.h>
#include <baseTypes.h>
#include <hookmanager.h>
#include <winuser.h>
#include <version.h>

Core::Core(QQmlContext *context, QObject* parent)
    : QObject(parent)
    , m_logger(new Logger(this))
    , m_keyboardProcessor(new KeyboardProcessor(this))
    , m_settingsController(new SettingsController(this))
    , m_uiBackend(new UiBackend(m_settingsController, context))
    , m_ssController(new SsController(m_settingsController, this))
    , m_discordController(new DiscordController(m_settingsController, this))
{
    registerTypes();

    ///> TODO: По идее версию билда из другого места надо писать, но пишем отсюда ибо удобно.
    qInfo(logInfo()) << "Build version: " << QString::fromStdString(PROJECT_VERSION_MAJOR)
                                     + "." + QString::fromStdString(PROJECT_VERSION_MINOR)
                                     + "." + QString::fromStdString(GIT_REL);


    context->setContextProperty("_uiBackend", m_uiBackend);

    HookManager::instance()->setCore(this);

    m_topmostTimer = new QTimer();
    m_topmostTimer->setInterval(500);

    connect(m_topmostTimer, &QTimer::timeout, this, &Core::topmostTimerTimout, Qt::QueuedConnection);

    QObject::connect(m_ssController, &SsController::ssMaximized,            this,                       &Core::ssMaximized,                             Qt::DirectConnection);
    QObject::connect(m_ssController, &SsController::ssLaunchStateChanged,   this,                       &Core::ssLaunched,                              Qt::QueuedConnection);
    QObject::connect(m_ssController, &SsController::ssLaunchStateChanged,   m_uiBackend,                &UiBackend::onSsLaunchStateChanged,             Qt::QueuedConnection);
    QObject::connect(m_ssController, &SsController::ssMaximized,            m_uiBackend,                &UiBackend::receiveSsMaximized,                 Qt::QueuedConnection);
    QObject::connect(m_ssController->gameInfoReader(), &GameInfoReader::sendPlayersTestStats,   m_uiBackend->gamePanel(),   &GamePanel::receivePlayersTestStats,            Qt::QueuedConnection);


    QObject::connect(m_ssController->gameInfoReader(),  &GameInfoReader::gameInitialized,       this,                       &Core::gameInitialized,         Qt::DirectConnection);
    QObject::connect(m_ssController->gameInfoReader(),  &GameInfoReader::ssShutdown,            this,                       &Core::onSsShutdowned,          Qt::QueuedConnection);
    QObject::connect(m_ssController->gameInfoReader(),  &GameInfoReader::loadStarted,           m_uiBackend,                &UiBackend::onLoadStarted,      Qt::QueuedConnection);
    QObject::connect(m_ssController->gameInfoReader(),  &GameInfoReader::gameStopped,           m_uiBackend,                &UiBackend::onGameStopped,      Qt::QueuedConnection);
    QObject::connect(m_ssController->gameInfoReader(),  &GameInfoReader::startingMission,       m_uiBackend,                &UiBackend::onStartingMission,  Qt::QueuedConnection);
    QObject::connect(m_ssController->gameInfoReader(),  &GameInfoReader::gameOver,              m_uiBackend,                &UiBackend::onGameOver,         Qt::QueuedConnection);
    QObject::connect(m_ssController->gameInfoReader(),  &GameInfoReader::sendNotification,              m_uiBackend,        &UiBackend::receiveNotification,         Qt::QueuedConnection);


    QObject::connect(m_ssController->apmMeter(),        &APMMeter::apmCalculated,        m_uiBackend->gamePanel(),       &GamePanel::onApmChanged,            Qt::QueuedConnection);

    QObject::connect(m_ssController->statsCollector(),  &StatsCollector::sendServerPlayrStats,  m_uiBackend->statisticPanel(),  &StatisticPanel::receiveServerPlayerStats,  Qt::QueuedConnection);
    QObject::connect(m_ssController->statsCollector(),  &StatsCollector::sendPlayersCount,  m_uiBackend->statisticPanel(),  &StatisticPanel::receivePlayersCount,  Qt::QueuedConnection);
    QObject::connect(m_ssController->statsCollector(),  &StatsCollector::sendCurrentPlayerHostState,  m_uiBackend->statisticPanel(),  &StatisticPanel::receiveCurrentPlayerHostState,  Qt::QueuedConnection);

    QObject::connect(m_ssController->lobbyEventReader(),  &LobbyEventReader::quitFromParty,  m_uiBackend->statisticPanel(),  &StatisticPanel::onQuitParty,  Qt::QueuedConnection);

    QObject::connect(m_ssController->playersSteamScanner(),  &PlayersSteamScanner::sendSteamPlayersInfoMap,  m_uiBackend->statisticPanel(),  &StatisticPanel::receivePlayersInfoMapFromScanner,  Qt::QueuedConnection);

    QObject::connect(m_keyboardProcessor, &KeyboardProcessor::expandKeyPressed, m_uiBackend, &UiBackend::expandKeyPressed, Qt::QueuedConnection);
    QObject::connect(m_keyboardProcessor, &KeyboardProcessor::altTabPressed, m_ssController, &SsController::minimizeSsWithWin7Support, Qt::QueuedConnection);

    QObject::connect(m_uiBackend, &UiBackend::sendExpand, m_ssController, &SsController::blockInput, Qt::QueuedConnection);

    QObject::connect(m_uiBackend, &UiBackend::noFogStateChanged, m_ssController->memoryController(), &MemoryController::onNoFogStateChanged, Qt::QueuedConnection);


    QObject::connect(m_ssController, &SsController::inputBlockStateChanged, HookManager::instance(), &HookManager::onInputBlockStateChanged, Qt::QueuedConnection);


    QObject::connect(m_uiBackend, &UiBackend::sendExit, this, &Core::onExit, Qt::QueuedConnection);
    QObject::connect(m_uiBackend, &UiBackend::sendLaunchSoulstormWithSupportMode, m_ssController, &SsController::launchSoulstormWithSupportMode, Qt::QueuedConnection);
    QObject::connect(m_uiBackend, &UiBackend::sendLaunchSoulstorm, m_ssController, &SsController::onLaunchSoulstorm, Qt::QueuedConnection);

    QObject::connect(m_discordController, &DiscordController::sendAvatar, m_uiBackend->imageProvider(), &ImageProvider::addDiscordAvatar, Qt::QueuedConnection);
    QObject::connect(m_discordController, &DiscordController::sendAttachmentImage, m_uiBackend->imageProvider(), &ImageProvider::addAttachmentImage, Qt::QueuedConnection);

    QObject::connect(m_discordController, &DiscordController::sendNews, m_uiBackend->newsPage(), &MessagesPage::receiveMessages, Qt::QueuedConnection);
    QObject::connect(m_uiBackend->imageProvider(), &ImageProvider::updateAvatars, m_uiBackend->newsPage(), &MessagesPage::onAvatarUpdate,  Qt::QueuedConnection);
    QObject::connect(m_discordController, &DiscordController::sendEvents, m_uiBackend->eventsPage(), &MessagesPage::receiveMessages, Qt::QueuedConnection);
    QObject::connect(m_uiBackend->imageProvider(), &ImageProvider::updateAvatars, m_uiBackend->eventsPage(), &MessagesPage::onAvatarUpdate,  Qt::QueuedConnection);

    QObject::connect(m_uiBackend->imageProvider(), &ImageProvider::updateAttachments, m_uiBackend->newsPage(), &MessagesPage::onAttachmetImagesUpdate,  Qt::QueuedConnection);
    QObject::connect(m_uiBackend->imageProvider(), &ImageProvider::updateAttachments, m_uiBackend->eventsPage(), &MessagesPage::onAttachmetImagesUpdate,  Qt::QueuedConnection);

    QObject::connect(m_uiBackend->newsPage(), &MessagesPage::sendLastReadedMessageId, m_discordController, &DiscordController::setLastReadedNewsMessageID, Qt::QueuedConnection);
    QObject::connect(m_uiBackend->eventsPage(), &MessagesPage::sendLastReadedMessageId, m_discordController, &DiscordController::setLastReadedEventsMessageID, Qt::QueuedConnection);

    m_settingsController->initializeSettings();
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
        if (m_ssStatsHwnd)
        {
            if (m_ssController->ssWindowed())
            {
                RECT ssRect;
                if (GetWindowRect(m_ssController->soulstormHwnd(), &ssRect))
                {

                    if(m_ssRect.bottom != ssRect.bottom || m_ssRect.top != ssRect.top || m_ssRect.right != ssRect.right || m_ssRect.left != ssRect.left)
                    {
                        m_ssRect = ssRect;
                        SetWindowPos(m_ssStatsHwnd, m_ssController->soulstormHwnd(), m_ssRect.left, m_ssRect.top, m_ssRect.right - m_ssRect.left, m_ssRect.bottom - m_ssRect.top, m_defaultWindowLong );

                        m_uiBackend->setSsWindowPosition(m_ssRect.left, m_ssRect.top);
                    }


                    LONG ssLong = GetWindowLongPtr(m_ssController->soulstormHwnd(), 0);
                    SetWindowPos(m_ssController->soulstormHwnd(), m_ssStatsHwnd, ssRect.left, ssRect.top, ssRect.right - ssRect.left, ssRect.bottom - ssRect.top, ssLong );

                    m_uiBackend->setSsWindowed(m_ssController->ssWindowed());
                }
            }
            else
            {
                RECT ssRect;
                {
                    if (GetWindowRect(m_ssController->soulstormHwnd(), &ssRect))
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
                SetWindowPos(m_ssStatsHwnd, HWND_TOPMOST, m_ssRect.left, m_ssRect.top, m_ssRect.right - m_ssRect.left, m_ssRect.bottom - m_ssRect.top, m_defaultWindowLong);
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
                        SetWindowPos(m_ssStatsHwnd, m_ssController->soulstormHwnd(), m_ssRect.left, m_ssRect.top, m_ssRect.right - m_ssRect.left, m_ssRect.bottom - m_ssRect.top, m_defaultWindowLong );

                        LONG ssLong = GetWindowLongPtr(m_ssController->soulstormHwnd(), 0);
                        SetWindowPos(m_ssController->soulstormHwnd(), m_ssStatsHwnd, m_ssRect.left, m_ssRect.top, m_ssRect.right - m_ssRect.left, m_ssRect.bottom - m_ssRect.top, ssLong );

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
        SetWindowPos(m_ssStatsHwnd, HWND_BOTTOM, 0,0,0,0, m_defaultWindowLong );
        m_uiBackend->setWindowTopmost(false);
    }

    m_uiBackend->setSsWindowed(m_ssController->ssWindowed());
}

void Core::gameInitialized()
{
   // HookManager::instance()->reconnectHook();

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

void Core::onSsShutdowned()
{
    ssMaximized(false);
}

void Core::onExit()
{
    m_topmostTimer->stop();

    ssMaximized(false);


    if(m_ssController->soulstormHwnd())
    {
        RECT ssRect;
        if (GetWindowRect(m_ssController->soulstormHwnd(), &ssRect))
        {
            SetWindowPos(m_ssController->soulstormHwnd(), HWND_TOPMOST, ssRect.left, ssRect.top, ssRect.right - ssRect.left, ssRect.bottom - ssRect.top, m_ssController->defaultSoulstormWindowLong() );
        }

        BringWindowToTop(m_ssController->soulstormHwnd());
    }

    qInfo(logInfo()) << "DowStats2 сlosed";

    m_logger->deleteLater();

    emit sendExit();
}

void Core::registerTypes()
{
    qRegisterMetaType<QVector<PlayerStats>>("QVector<PlayerStats>");
    qRegisterMetaType<ServerPlayerStats>("ServerPlayerStats");
    qRegisterMetaType<QList<SearchStemIdPlayerInfo>>("QList<SearchStemIdPlayerInfo>");
    qRegisterMetaType<SsGameState>("SsGameState");
    qRegisterMetaType<SendingReplayInfo>("SendingReplayInfo");
    qRegisterMetaType<SearchStemIdPlayerInfo>("SearchStemIdPlayerInfo");
    qRegisterMetaType<QList<DiscordMessage>>("QList<DiscordMessage>");
    qRegisterMetaType<QMap<QString, QImage>>("QMap<QString, QImage>");
    qRegisterMetaType<DiscordMessage>("DiscordMessage");
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
   /* if (event->type() == QEvent::KeyPress && m_keyboardProcessor)
    {
        QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);
        m_keyboardProcessor->keyPressEvent(keyEvent);
        m_ssController->apmMeter()->onKeyPressEvent(keyEvent);
        delete keyEvent;
        return true;
    }

    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
        m_uiBackend->mousePressEvent(mouseEvent->pos());
        m_ssController->apmMeter()->onMousePressEvent(mouseEvent->pos());
        delete mouseEvent;
        return true;
    }

    if (event->type() == QEvent::MouseMove)
    {
        QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
        m_uiBackend->mouseMoveEvent(mouseEvent->pos());
        delete mouseEvent;
        return true;
    }*/

    //delete event;
    QObject::event(event);
    return false;
}

void Core::onKeyEvent(QKeyEvent event)
{
    if(uiBackend()->getShowClient()){
        if (event.type() == QEvent::KeyPress && m_keyboardProcessor)
        {
            m_keyboardProcessor->keyPressEvent(std::move(event));
            m_ssController->apmMeter()->onKeyPressEvent();
        }
    }
}

void Core::onMouseEvent(QMouseEvent event)
{
    if(uiBackend()->getShowClient()){
        if (event.type() == QEvent::MouseButtonPress)
        {
            m_uiBackend->mousePressEvent(event.pos());
            m_ssController->apmMeter()->onMousePressEvent();
        }
        else if (event.type() == QEvent::MouseMove)
        {
            m_uiBackend->mouseMoveEvent(event.pos());
        }
    }
}

void Core::onWheelEvent(QWheelEvent event)
{
    m_uiBackend->onMouseWheel(event.delta());
}

void Core::grubStatsWindow()
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString s = codec->toUnicode("DowStats2Overlay");
    LPCWSTR lps = (LPCWSTR)s.utf16();

    m_ssStatsHwnd = FindWindowW(NULL, lps);

    m_defaultWidth = GetSystemMetrics(SM_CXSCREEN);
    m_defaultHeight = GetSystemMetrics(SM_CYSCREEN);

    m_defaultWindowLong = GetWindowLongPtr(m_ssStatsHwnd, GWL_EXSTYLE);
    SetWindowPos(m_ssStatsHwnd, HWND_BOTTOM, m_ssRect.left, m_ssRect.top, m_ssRect.right - m_ssRect.left, m_ssRect.bottom - m_ssRect.top, m_defaultWindowLong );
    m_uiBackend->setWindowTopmost(false);
}

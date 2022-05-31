#include <core.h>
#include <QDebug>
#include <QFile>
#include <WarningsLogReader.h>
#include <LobbyEventReader.h>
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
    , m_soulstormController(new SoulstormController(m_settingsController, this))
    , m_discordWebProcessor(new DiscordWebProcessor(m_settingsController, this))
    , m_modsProcessor(new ModsProcessor(m_soulstormController->ssPath(), this))
    , m_soundProcessor(new SoundProcessor(this))
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

    QObject::connect(m_topmostTimer, &QTimer::timeout, this, &Core::topmostTimerTimout, Qt::QueuedConnection);

    QObject::connect(m_soulstormController,                    &SoulstormController::ssMaximized,                     this,                           &Core::ssMaximized,                             Qt::DirectConnection);
    QObject::connect(m_soulstormController,                    &SoulstormController::ssMaximized,                     m_soundProcessor,               &SoundProcessor::setSoulstormMaximized,                             Qt::DirectConnection);

    QObject::connect(m_soulstormController,                    &SoulstormController::ssLaunchStateChanged,            this,                           &Core::ssLaunched,                              Qt::QueuedConnection);
    QObject::connect(m_soulstormController,                    &SoulstormController::ssLaunchStateChanged,            m_uiBackend,                    &UiBackend::onSsLaunchStateChanged,             Qt::QueuedConnection);
    QObject::connect(m_soulstormController,                    &SoulstormController::ssMaximized,                     m_uiBackend,                    &UiBackend::receiveSsMaximized,                 Qt::QueuedConnection);
    QObject::connect(m_soulstormController->statsServerProcessor(),  &StatsServerProcessor::sendServerPlayrStats,          m_uiBackend->statisticPanel(),  &StatisticPanel::receiveServerPlayerStats,      Qt::QueuedConnection);
    QObject::connect(m_soulstormController->statsServerProcessor(),  &StatsServerProcessor::sendCurrentPlayerHostState,    m_uiBackend->statisticPanel(),  &StatisticPanel::receiveCurrentPlayerHostState, Qt::QueuedConnection);
    QObject::connect(m_soulstormController->statsServerProcessor(),  &StatsServerProcessor::sendNotification,              m_uiBackend,                    &UiBackend::receiveNotification,                Qt::QueuedConnection);
    QObject::connect(m_soulstormController->warningsLogReader(),   &WarningsLogReader::gameInitialized,         this,                       &Core::gameInitialized,                  Qt::DirectConnection);
    QObject::connect(m_soulstormController->warningsLogReader(),   &WarningsLogReader::ssShutdown,              this,                       &Core::onSsShutdowned,                   Qt::QueuedConnection);
    QObject::connect(m_soulstormController->warningsLogReader(),   &WarningsLogReader::sendCurrentMissionState, m_uiBackend,                &UiBackend::setMissionCurrentState,         Qt::QueuedConnection);
    QObject::connect(m_soulstormController->warningsLogReader(),   &WarningsLogReader::sendCurrentMissionState, m_soundProcessor,           &SoundProcessor::receiveCurrentMissionState,         Qt::QueuedConnection);
    QObject::connect(m_soulstormController->warningsLogReader(),   &WarningsLogReader::sendNotification,        m_uiBackend,                &UiBackend::receiveNotification,         Qt::QueuedConnection);
    QObject::connect(m_soulstormController->warningsLogReader(),   &WarningsLogReader::sendPlayersTestStats,    m_uiBackend->gamePanel(),   &GamePanel::receivePlayersTestStats,     Qt::QueuedConnection);

    QObject::connect(m_soulstormController->apmMeter(),        &APMMeter::apmCalculated,        m_uiBackend->gamePanel(),       &GamePanel::onApmChanged,            Qt::QueuedConnection);
    QObject::connect(m_soulstormController->lobbyEventReader(),  &LobbyEventReader::quitFromParty,  m_uiBackend->statisticPanel(),  &StatisticPanel::onQuitParty,  Qt::QueuedConnection);

    QObject::connect(m_soulstormController->lobbyEventReader(),  &LobbyEventReader::quitFromParty,      m_soundProcessor,  &SoundProcessor::activeIsFirstConnection,  Qt::QueuedConnection);
    QObject::connect(m_soulstormController->lobbyEventReader(),  &LobbyEventReader::hostParty,      m_soundProcessor,  &SoundProcessor::activeIsFirstConnection,  Qt::QueuedConnection);
    QObject::connect(m_soulstormController->lobbyEventReader(),  &LobbyEventReader::playerConnected,    m_soundProcessor,  &SoundProcessor::playPlayerJoinSound,  Qt::QueuedConnection);
    QObject::connect(m_soulstormController->lobbyEventReader(),  &LobbyEventReader::playerDisconnected, m_soundProcessor,  &SoundProcessor::playPlayerLeftSound,  Qt::QueuedConnection);
    QObject::connect(m_soulstormController->lobbyEventReader(),  &LobbyEventReader::playerKicked,       m_soundProcessor,  &SoundProcessor::playPlayerLeftSound,  Qt::QueuedConnection);


    QObject::connect(m_soulstormController, &SoulstormController::inputBlockStateChanged, HookManager::instance(), &HookManager::onInputBlockStateChanged, Qt::QueuedConnection);
    QObject::connect(m_soulstormController->dowServerProcessor(),  &DowServerProcessor::sendSteamIds,  m_uiBackend->statisticPanel(),  &StatisticPanel::receivePlayersInfoMapFromScanner,  Qt::QueuedConnection);

    QObject::connect(m_keyboardProcessor, &KeyboardProcessor::expandKeyPressed, m_uiBackend, &UiBackend::expandKeyPressed, Qt::QueuedConnection);
    QObject::connect(m_keyboardProcessor, &KeyboardProcessor::altTabPressed, m_soulstormController, &SoulstormController::minimizeSsWithWin7Support, Qt::QueuedConnection);

    QObject::connect(m_uiBackend,                       &UiBackend::sendExpand,                         m_soulstormController,                     &SoulstormController::blockSsWindowInput,                  Qt::QueuedConnection);
    QObject::connect(m_uiBackend,                       &UiBackend::noFogStateChanged,                  m_soulstormController->soulstormMemoryController(), &SoulstormMemoryController::onNoFogStateChanged,             Qt::QueuedConnection);
    QObject::connect(m_uiBackend,                       &UiBackend::sendExit,                           this,                               &Core::onExit,                                      Qt::QueuedConnection);
    QObject::connect(m_uiBackend,                       &UiBackend::sendLaunchSoulstormWithSupportMode, m_soulstormController,                     &SoulstormController::launchSoulstormWithSupportMode,      Qt::QueuedConnection);
    QObject::connect(m_uiBackend,                       &UiBackend::sendLaunchSoulstorm,                m_soulstormController,                     &SoulstormController::launchSoulstorm,                     Qt::QueuedConnection);
    QObject::connect(m_uiBackend->imageProvider(),      &ImageProvider::updateAttachments,              m_uiBackend->newsPage(),            &MessagesPage::onAttachmetImagesUpdate,             Qt::QueuedConnection);
    QObject::connect(m_uiBackend->imageProvider(),      &ImageProvider::updateAttachments,              m_uiBackend->eventsPage(),          &MessagesPage::onAttachmetImagesUpdate,             Qt::QueuedConnection);
    QObject::connect(m_uiBackend->imageProvider(),      &ImageProvider::updateAvatars,                  m_uiBackend->newsPage(),            &MessagesPage::onAvatarUpdate,                      Qt::QueuedConnection);
    QObject::connect(m_uiBackend->imageProvider(),      &ImageProvider::updateAvatars,                  m_uiBackend->eventsPage(),          &MessagesPage::onAvatarUpdate,                      Qt::QueuedConnection);
    QObject::connect(m_uiBackend->newsPage(),           &MessagesPage::sendLastReadedMessageId,         m_discordWebProcessor,              &DiscordWebProcessor::setLastReadedNewsMessageID,     Qt::QueuedConnection);
    QObject::connect(m_uiBackend->eventsPage(),         &MessagesPage::sendLastReadedMessageId,         m_discordWebProcessor,              &DiscordWebProcessor::setLastReadedEventsMessageID,   Qt::QueuedConnection);
    QObject::connect(m_uiBackend->settingsPageModel(),  &SettingsPageModel::startInstall,               m_modsProcessor,                    &ModsProcessor::onModInstallRequest,                Qt::QueuedConnection);
    QObject::connect(m_uiBackend->settingsPageModel(),  &SettingsPageModel::startUninstall,             m_modsProcessor,                    &ModsProcessor::onUninstallRequest,                 Qt::QueuedConnection);
    QObject::connect(m_uiBackend->settingsPageModel(),  &SettingsPageModel::sendUnlockRaces,            m_modsProcessor,                   &ModsProcessor::unlockRaces,                 Qt::QueuedConnection);


    QObject::connect(m_uiBackend->newsPage(),        &MessagesPage::sendNextMessagesRequest,              m_discordWebProcessor,            &DiscordWebProcessor::requestNextNews,             Qt::QueuedConnection);
    QObject::connect(m_uiBackend->eventsPage(),      &MessagesPage::sendNextMessagesRequest,              m_discordWebProcessor,            &DiscordWebProcessor::requestNextEvents,             Qt::QueuedConnection);

    QObject::connect(m_discordWebProcessor, &DiscordWebProcessor::sendAvatar, m_uiBackend->imageProvider(), &ImageProvider::addDiscordAvatar, Qt::QueuedConnection);
    QObject::connect(m_discordWebProcessor, &DiscordWebProcessor::sendAttachmentImage, m_uiBackend->imageProvider(), &ImageProvider::addAttachmentImage, Qt::QueuedConnection);
    QObject::connect(m_discordWebProcessor, &DiscordWebProcessor::sendNews, m_uiBackend->newsPage(), &MessagesPage::receiveMessages, Qt::QueuedConnection);
    QObject::connect(m_discordWebProcessor, &DiscordWebProcessor::sendNextNews, m_uiBackend->newsPage(), &MessagesPage::receiveNextMessages, Qt::QueuedConnection);
    QObject::connect(m_discordWebProcessor, &DiscordWebProcessor::sendEvents, m_uiBackend->eventsPage(), &MessagesPage::receiveMessages, Qt::QueuedConnection);
    QObject::connect(m_discordWebProcessor, &DiscordWebProcessor::sendNextEvents, m_uiBackend->eventsPage(), &MessagesPage::receiveNextMessages, Qt::QueuedConnection);

    QObject::connect(m_modsProcessor, &ModsProcessor::modInstallCompleeted, m_uiBackend->settingsPageModel(), &SettingsPageModel::receiveInstallCompleeted, Qt::QueuedConnection);
    QObject::connect(m_modsProcessor, &ModsProcessor::installProgress, m_uiBackend->settingsPageModel(), &SettingsPageModel::receiveDownloadProgress, Qt::QueuedConnection);
    QObject::connect(m_modsProcessor, &ModsProcessor::downloadError, m_uiBackend->settingsPageModel(), &SettingsPageModel::receiveDownloadError, Qt::QueuedConnection);
    QObject::connect(m_modsProcessor, &ModsProcessor::sendUnlockRacesStatus, m_uiBackend->settingsPageModel(), &SettingsPageModel::receiveUnlockRacesStatus, Qt::QueuedConnection);



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

    if (m_soulstormController->warningsLogReader()->getGameInitialized())
    {
        if (m_ssStatsHwnd)
        {
            if (m_soulstormController->ssWindowed())
            {
                RECT ssRect;
                if (GetWindowRect(m_soulstormController->soulstormHwnd(), &ssRect))
                {

                    if(m_ssRect.bottom != ssRect.bottom || m_ssRect.top != ssRect.top || m_ssRect.right != ssRect.right || m_ssRect.left != ssRect.left)
                    {
                        m_ssRect = ssRect;
                        SetWindowPos(m_ssStatsHwnd, m_soulstormController->soulstormHwnd(), m_ssRect.left, m_ssRect.top, m_ssRect.right - m_ssRect.left, m_ssRect.bottom - m_ssRect.top, m_defaultWindowLong );

                        m_uiBackend->setSsWindowPosition(m_ssRect.left, m_ssRect.top);
                    }


                    LONG ssLong = GetWindowLongPtr(m_soulstormController->soulstormHwnd(), 0);
                    SetWindowPos(m_soulstormController->soulstormHwnd(), m_ssStatsHwnd, ssRect.left, ssRect.top, ssRect.right - ssRect.left, ssRect.bottom - ssRect.top, ssLong );

                    m_uiBackend->setSsWindowed(m_soulstormController->ssWindowed());
                }
            }
            else
            {
                RECT ssRect;
                {
                    if (GetWindowRect(m_soulstormController->soulstormHwnd(), &ssRect))
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

        if (!m_soulstormController->ssWindowed())
        {
            RECT ssRect;
            if (GetWindowRect(m_soulstormController->soulstormHwnd(), &ssRect))
            {
                m_ssRect = ssRect;
                SetWindowPos(m_ssStatsHwnd, HWND_TOPMOST, m_ssRect.left, m_ssRect.top, m_ssRect.right - m_ssRect.left, m_ssRect.bottom - m_ssRect.top, m_defaultWindowLong);
                m_uiBackend->setWindowTopmost(true);
            }
            m_topmostTimer->start();
            m_uiBackend->setSsWindowed(m_soulstormController->ssWindowed());
        }
        else
        {
            RECT ssRect;
            if (GetWindowRect(m_soulstormController->soulstormHwnd(), &ssRect))
            {
                if(m_ssRect.bottom != ssRect.bottom || m_ssRect.top != ssRect.top || m_ssRect.right != ssRect.right || m_ssRect.left != ssRect.left)
                {
                        m_ssRect = ssRect;
                        SetWindowPos(m_ssStatsHwnd, m_soulstormController->soulstormHwnd(), m_ssRect.left, m_ssRect.top, m_ssRect.right - m_ssRect.left, m_ssRect.bottom - m_ssRect.top, m_defaultWindowLong );

                        LONG ssLong = GetWindowLongPtr(m_soulstormController->soulstormHwnd(), 0);
                        SetWindowPos(m_soulstormController->soulstormHwnd(), m_ssStatsHwnd, m_ssRect.left, m_ssRect.top, m_ssRect.right - m_ssRect.left, m_ssRect.bottom - m_ssRect.top, ssLong );

                        m_uiBackend->setSsWindowPosition(m_ssRect.left, m_ssRect.top);
                }
            }
            m_uiBackend->setSsWindowed(m_soulstormController->ssWindowed());
        }

        m_topmostTimer->start();
    }
    else
    {
        m_topmostTimer->stop();
        SetWindowPos(m_ssStatsHwnd, HWND_BOTTOM, 0,0,0,0, m_defaultWindowLong );
        m_uiBackend->setWindowTopmost(false);
    }

    m_uiBackend->setSsWindowed(m_soulstormController->ssWindowed());
}

void Core::gameInitialized()
{
   // HookManager::instance()->reconnectHook();

    m_topmostTimer->start();

    if(m_soulstormController->getSsMaximized() && (GetSystemMetrics(SM_CXSCREEN) != m_widthInGame || GetSystemMetrics(SM_CYSCREEN) != m_heightInGame))
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


    if(m_soulstormController->soulstormHwnd())
    {
        RECT ssRect;
        if (GetWindowRect(m_soulstormController->soulstormHwnd(), &ssRect))
        {
            SetWindowPos(m_soulstormController->soulstormHwnd(), HWND_TOPMOST, ssRect.left, ssRect.top, ssRect.right - ssRect.left, ssRect.bottom - ssRect.top, m_soulstormController->defaultSoulstormWindowLong() );
        }

        BringWindowToTop(m_soulstormController->soulstormHwnd());
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
    qRegisterMetaType<SsMissionState>("SsMissionState");
    qRegisterMetaType<SendingReplayInfo>("SendingReplayInfo");
    qRegisterMetaType<SearchStemIdPlayerInfo>("SearchStemIdPlayerInfo");
    qRegisterMetaType<QList<DiscordMessage>>("QList<DiscordMessage>");
    qRegisterMetaType<QMap<QString, QImage>>("QMap<QString, QImage>");
    qRegisterMetaType<DiscordMessage>("DiscordMessage");
    qRegisterMetaType<QVector<PartyData>>("QVector<PartyData>");
    qRegisterMetaType<PartyData>("PartyData");
    qRegisterMetaType<InstMod>("InstMod");

}

UiBackend *Core::uiBackend() const
{
    return m_uiBackend;
}

SoulstormController *Core::soulstormController() const
{
    return m_soulstormController;
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
        m_soulstormController->apmMeter()->onKeyPressEvent(keyEvent);
        delete keyEvent;
        return true;
    }

    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
        m_uiBackend->mousePressEvent(mouseEvent->pos());
        m_soulstormController->apmMeter()->onMousePressEvent(mouseEvent->pos());
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
            m_soulstormController->apmMeter()->onKeyPressEvent();
        }
    }
}

void Core::onMouseEvent(QMouseEvent event)
{
    if(uiBackend()->getShowClient()){
        if (event.type() == QEvent::MouseButtonPress)
        {
            m_uiBackend->mousePressEvent(event.pos());
            m_soulstormController->apmMeter()->onMousePressEvent();
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

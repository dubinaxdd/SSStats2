#include <core.h>
#include <QDebug>
#include <QFile>
#include <gameStateReader.h>
#include <lobbyEventReader.h>
#include <baseTypes.h>
#include <hookManager.h>
#include <winuser.h>
#include <version.h>

Core::Core(QQmlContext *context, QObject* parent)
    : QObject(parent)
    , m_logger(new Logger(this))
    , m_settingsController(new SettingsController(this))
    , m_keyboardProcessor(new KeyboardProcessor(this))
    , m_uiBackend(new UiBackend(m_settingsController, context))
    , m_soulstormController(new SoulstormController(m_settingsController, this))
    , m_overlayWindowController(new OverlayWindowController(m_settingsController, m_uiBackend, m_soulstormController, this))
    , m_discordWebProcessor(new DiscordWebProcessor(m_settingsController, this))
    , m_modsProcessor(new ModsProcessor(m_soulstormController->ssPath(), this))
    , m_soundProcessor(new SoundProcessor(this))
    , m_statsServerProcessor ( new StatsServerProcessor(m_settingsController, m_soulstormController->ssPath(), m_soulstormController->steamPath(), this))
    , m_rankedModServiceProcessor(new RankedModServiceProcessor(m_settingsController, this))
    , m_mapManager(new MapManager(m_settingsController, m_soulstormController->ssPath(), this))
{
    registerTypes();

    ///> TODO: По идее версию билда из другого места надо писать, но пишем отсюда ибо удобно.
    qInfo(logInfo()) << "Build version: " << QString::fromStdString(PROJECT_VERSION_MAJOR)
                                     + "." + QString::fromStdString(PROJECT_VERSION_MINOR)
                                     + "." + QString::fromStdString(GIT_REL);

    m_uiBackend->replayManager()->setSsPath(m_soulstormController->ssPath());
    m_uiBackend->mapManagerPage()->setSsPath(m_soulstormController->ssPath());

    context->setContextProperty("_uiBackend", m_uiBackend);

    HookManager::instance()->setCore(this);

    QObject::connect(m_keyboardProcessor, &KeyboardProcessor::expandKeyPressed, m_uiBackend, &UiBackend::expandKeyPressed, Qt::QueuedConnection);
    QObject::connect(m_keyboardProcessor, &KeyboardProcessor::altTabPressed, m_soulstormController, &SoulstormController::minimizeSsWithWin7Support, Qt::QueuedConnection);

    QObject::connect(m_soulstormController,                    &SoulstormController::ssMaximized,                     m_overlayWindowController,    &OverlayWindowController::ssMaximized,                             Qt::DirectConnection);
    QObject::connect(m_soulstormController,                    &SoulstormController::ssMaximized,                     m_soundProcessor,             &SoundProcessor::setSoulstormMaximized,                             Qt::DirectConnection);
    QObject::connect(m_soulstormController,                    &SoulstormController::ssLaunchStateChanged,            m_overlayWindowController,    &OverlayWindowController::ssLaunched,                              Qt::QueuedConnection);
    QObject::connect(m_soulstormController,                    &SoulstormController::ssLaunchStateChanged,            m_uiBackend,                  &UiBackend::onSsLaunchStateChanged,             Qt::QueuedConnection);
    QObject::connect(m_soulstormController,                    &SoulstormController::ssMaximized,                     m_uiBackend,                  &UiBackend::receiveSsMaximized,                 Qt::QueuedConnection);
    QObject::connect(m_soulstormController,                    &SoulstormController::inputBlockStateChanged, HookManager::instance(), &HookManager::onInputBlockStateChanged, Qt::QueuedConnection);

    QObject::connect(m_soulstormController->gameStateReader(),   &GameStateReader::gameInitialized,         m_overlayWindowController,  &OverlayWindowController::gameInitialized,                  Qt::DirectConnection);
    QObject::connect(m_soulstormController->gameStateReader(),   &GameStateReader::ssShutdown,              m_overlayWindowController,  &OverlayWindowController::onSsShutdowned,                   Qt::QueuedConnection);
    QObject::connect(m_soulstormController->gameStateReader(),   &GameStateReader::sendCurrentMissionState, m_uiBackend,                &UiBackend::setMissionCurrentState,         Qt::QueuedConnection);
    QObject::connect(m_soulstormController->gameStateReader(),   &GameStateReader::sendCurrentMissionState, m_soundProcessor,           &SoundProcessor::receiveCurrentMissionState,         Qt::QueuedConnection);
    QObject::connect(m_soulstormController->gameStateReader(),   &GameStateReader::sendPlayersTestStats,    m_uiBackend->gamePanel(),   &GamePanel::receivePlayersTestStats,     Qt::QueuedConnection);
    QObject::connect(m_soulstormController->gameStateReader(),   &GameStateReader::gameInitialized,         m_statsServerProcessor,  [&](){m_statsServerProcessor->parseCurrentPlayerSteamId();}, Qt::QueuedConnection);
    QObject::connect(m_soulstormController->gameStateReader(),   &GameStateReader::sendCurrentMod,  m_rankedModServiceProcessor, &RankedModServiceProcessor::receiveCurrentMod,   Qt::QueuedConnection);
    QObject::connect(m_soulstormController->gameStateReader(),   &GameStateReader::sendCurrentMod,  m_statsServerProcessor, &StatsServerProcessor::receiveCurrentMod,   Qt::QueuedConnection);


    QObject::connect(m_soulstormController->gameStateReader(),   &GameStateReader::sendGameRankedMode, m_uiBackend, [&](bool gameRankedMode){ m_uiBackend->gamePanel()->setGameRankedMode(gameRankedMode);},         Qt::QueuedConnection);
    QObject::connect(m_soulstormController->gameStateReader(),   &GameStateReader::sendGameRankedMode, m_statsServerProcessor, &StatsServerProcessor::receiveRankedMode, Qt::QueuedConnection);

    QObject::connect(m_soulstormController->lobbyEventReader(),  &LobbyEventReader::quitFromParty,  m_uiBackend->statisticPanel(),  &StatisticPanel::onQuitParty,  Qt::QueuedConnection);
    QObject::connect(m_soulstormController->lobbyEventReader(),  &LobbyEventReader::quitFromParty,      m_soundProcessor,  &SoundProcessor::activeIsFirstConnection,  Qt::QueuedConnection);
    QObject::connect(m_soulstormController->lobbyEventReader(),  &LobbyEventReader::hostParty,      m_soundProcessor,  &SoundProcessor::activeIsFirstConnection,  Qt::QueuedConnection);
    QObject::connect(m_soulstormController->lobbyEventReader(),  &LobbyEventReader::playerConnected,    m_soundProcessor,  &SoundProcessor::playPlayerJoinSound,  Qt::QueuedConnection);
    QObject::connect(m_soulstormController->lobbyEventReader(),  &LobbyEventReader::playerDisconnected, m_soundProcessor,  &SoundProcessor::playPlayerLeftSound,  Qt::QueuedConnection);
    QObject::connect(m_soulstormController->lobbyEventReader(),  &LobbyEventReader::playerKicked,       m_soundProcessor,  &SoundProcessor::playPlayerLeftSound,  Qt::QueuedConnection);

    QObject::connect(m_soulstormController->replayDataCollector(), &ReplayDataCollector::sendReplayToServer,       m_statsServerProcessor, &StatsServerProcessor::sendReplayToServer,   Qt::QueuedConnection);
    QObject::connect(m_soulstormController->replayDataCollector(),   &ReplayDataCollector::sendNotification,        m_uiBackend,                &UiBackend::receiveNotification,         Qt::QueuedConnection);

    QObject::connect(m_soulstormController->dowServerProcessor(), &DowServerProcessor::sendPlayersInfoFromDowServer, m_statsServerProcessor, &StatsServerProcessor::receivePlayresInfoFromDowServer, Qt::QueuedConnection);
    QObject::connect(m_soulstormController->dowServerProcessor(),  &DowServerProcessor::sendPlayersInfoFromDowServer,  m_uiBackend->statisticPanel(),  &StatisticPanel::receivePlayresInfoFromDowServer,  Qt::QueuedConnection);
    QObject::connect(m_soulstormController->dowServerProcessor(), &DowServerProcessor::sendPlayersInfoFromDowServer, m_rankedModServiceProcessor, &RankedModServiceProcessor::receivePlayresInfoFromDowServer, Qt::QueuedConnection);

    QObject::connect(m_soulstormController->apmMeter(),        &APMMeter::apmCalculated,        m_uiBackend->gamePanel(),       &GamePanel::onApmChanged,            Qt::QueuedConnection);

    QObject::connect(m_uiBackend,                       &UiBackend::sendExit,                           this,                               &Core::onExit,                  Qt::QueuedConnection);
    QObject::connect(m_uiBackend,                       &UiBackend::sendExpand,                         m_soulstormController,              &SoulstormController::blockSsWindowInput,                  Qt::QueuedConnection);
    QObject::connect(m_uiBackend,                       &UiBackend::noFogStateChanged,                  m_soulstormController->soulstormMemoryController(), &SoulstormMemoryController::onNoFogStateChanged,             Qt::QueuedConnection);
    QObject::connect(m_uiBackend,                       &UiBackend::sendLaunchSoulstormWithSupportMode, m_soulstormController,              &SoulstormController::launchSoulstormWithSupportMode,      Qt::QueuedConnection);
    QObject::connect(m_uiBackend,                       &UiBackend::sendLaunchSoulstorm,                m_soulstormController,              &SoulstormController::launchSoulstorm,                     Qt::QueuedConnection);
    QObject::connect(m_uiBackend,                       &UiBackend::rankedModeStateChanged, m_rankedModServiceProcessor, &RankedModServiceProcessor::sendRankedMode , Qt::QueuedConnection);
    QObject::connect(m_uiBackend->imageProvider(),      &ImageProvider::updateAttachments,              m_uiBackend->newsPage(),            &MessagesPage::onAttachmetImagesUpdate,             Qt::QueuedConnection);
    QObject::connect(m_uiBackend->imageProvider(),      &ImageProvider::updateAttachments,              m_uiBackend->eventsPage(),          &MessagesPage::onAttachmetImagesUpdate,             Qt::QueuedConnection);
    QObject::connect(m_uiBackend->imageProvider(),      &ImageProvider::updateYoutubeImages,            m_uiBackend->newsPage(),            &MessagesPage::onYoutubeImagesUpdate,             Qt::QueuedConnection);
    QObject::connect(m_uiBackend->imageProvider(),      &ImageProvider::updateYoutubeImages,            m_uiBackend->eventsPage(),          &MessagesPage::onYoutubeImagesUpdate,             Qt::QueuedConnection);
    QObject::connect(m_uiBackend->imageProvider(),      &ImageProvider::updateAvatars,                  m_uiBackend->newsPage(),            &MessagesPage::onAvatarUpdate,                      Qt::QueuedConnection);
    QObject::connect(m_uiBackend->imageProvider(),      &ImageProvider::updateAvatars,                  m_uiBackend->eventsPage(),          &MessagesPage::onAvatarUpdate,                      Qt::QueuedConnection);
    QObject::connect(m_uiBackend->newsPage(),           &MessagesPage::sendLastReadedMessageId,         m_discordWebProcessor,              &DiscordWebProcessor::setLastReadedNewsMessageID,     Qt::QueuedConnection);
    QObject::connect(m_uiBackend->eventsPage(),         &MessagesPage::sendLastReadedMessageId,         m_discordWebProcessor,              &DiscordWebProcessor::setLastReadedEventsMessageID,   Qt::QueuedConnection);
    QObject::connect(m_uiBackend->modsPage(),           &ModsPage::startInstall,               m_modsProcessor,                    &ModsProcessor::onModInstallRequest,                Qt::QueuedConnection);
    QObject::connect(m_uiBackend->modsPage(),           &ModsPage::startUninstall,             m_modsProcessor,                    &ModsProcessor::onUninstallRequest,                 Qt::QueuedConnection);
    QObject::connect(m_uiBackend->modsPage(),           &ModsPage::sendUnlockRaces,            m_modsProcessor,                   &ModsProcessor::unlockRaces,                 Qt::QueuedConnection);
    QObject::connect(m_uiBackend->settingsPageModel(), &SettingsPageModel::volumeChanged, m_soundProcessor, &SoundProcessor::setVolume, Qt::QueuedConnection);
    QObject::connect(m_uiBackend->newsPage(),        &MessagesPage::sendNextMessagesRequest,              m_discordWebProcessor,            &DiscordWebProcessor::requestNextNews,             Qt::QueuedConnection);
    QObject::connect(m_uiBackend->eventsPage(),      &MessagesPage::sendNextMessagesRequest,              m_discordWebProcessor,            &DiscordWebProcessor::requestNextEvents,             Qt::QueuedConnection);
    QObject::connect(m_uiBackend->mapManagerPage(), &MapManagerPage::sendRemoveMap, m_mapManager, &MapManager::receiveRemoveMap,  Qt::QueuedConnection);
    QObject::connect(m_uiBackend->mapManagerPage(), &MapManagerPage::sendInstallMap, m_mapManager, &MapManager::receiveInstallMap,  Qt::QueuedConnection);
    QObject::connect(m_uiBackend->mapManagerPage(), &MapManagerPage::sendInstallAllMaps, m_mapManager, &MapManager::receiveInstallAllMaps,  Qt::QueuedConnection);
    QObject::connect(m_uiBackend->mapManagerPage(), &MapManagerPage::sendInstallDefaultMaps, m_mapManager, &MapManager::receiveInstallDefaultMaps,  Qt::QueuedConnection);
    QObject::connect(m_uiBackend->mapManagerPage(), &MapManagerPage::sendLoadMapsInfo, m_mapManager, &MapManager::receiveLoadMapsInfo,  Qt::QueuedConnection);
    QObject::connect(m_uiBackend->settingsPageModel(),  &SettingsPageModel::enableEventsSoundWhenGameMaximizedChanged, m_soundProcessor,  &SoundProcessor::setEnableSoundsWhenGameMaximized, Qt::QueuedConnection);
    QObject::connect(m_uiBackend->settingsPageModel(),  &SettingsPageModel::enableEventsSoundWhenGameMinimizedChanged, m_soundProcessor,  &SoundProcessor::setEnableSoundsWhenGameMinimized, Qt::QueuedConnection);
    QObject::connect(m_uiBackend->settingsPageModel(),  &SettingsPageModel::enableGameLoadEventSoundChanged, m_soundProcessor,  &SoundProcessor::setEnableGameLoadEventSound, Qt::QueuedConnection);
    QObject::connect(m_uiBackend->settingsPageModel(),  &SettingsPageModel::enableGameStartEventSoundChanged, m_soundProcessor,  &SoundProcessor::setEnableGameStartEventSound, Qt::QueuedConnection);

    QObject::connect(m_statsServerProcessor,                    &StatsServerProcessor::sendServerPlayrStats,          m_uiBackend->statisticPanel(),  &StatisticPanel::receiveServerPlayerStats,      Qt::QueuedConnection);
    QObject::connect(m_statsServerProcessor,                    &StatsServerProcessor::sendNotification,              m_uiBackend,                    &UiBackend::receiveNotification,                Qt::QueuedConnection);
    QObject::connect(m_statsServerProcessor, &StatsServerProcessor::sendCurrentPlayerSteamID, m_soulstormController->dowServerProcessor(), &DowServerProcessor::setCurrentPlayerSteamID, Qt::QueuedConnection);
    QObject::connect(m_statsServerProcessor, &StatsServerProcessor::sendCurrentPlayerSteamID, m_rankedModServiceProcessor, &RankedModServiceProcessor::setCurrentPlayerSteamIdSlot, Qt::QueuedConnection);

    QObject::connect(m_discordWebProcessor, &DiscordWebProcessor::sendAvatar, m_uiBackend->imageProvider(), &ImageProvider::addDiscordAvatar, Qt::QueuedConnection);
    QObject::connect(m_discordWebProcessor, &DiscordWebProcessor::sendAttachmentImage, m_uiBackend->imageProvider(), &ImageProvider::addAttachmentImage, Qt::QueuedConnection);
    QObject::connect(m_discordWebProcessor, &DiscordWebProcessor::sendYoutubeImage, m_uiBackend->imageProvider(), &ImageProvider::addYoutubeImage, Qt::QueuedConnection);
    QObject::connect(m_discordWebProcessor, &DiscordWebProcessor::sendNews, m_uiBackend->newsPage(), &MessagesPage::receiveMessages, Qt::QueuedConnection);
    QObject::connect(m_discordWebProcessor, &DiscordWebProcessor::sendNextNews, m_uiBackend->newsPage(), &MessagesPage::receiveNextMessages, Qt::QueuedConnection);
    QObject::connect(m_discordWebProcessor, &DiscordWebProcessor::sendEvents, m_uiBackend->eventsPage(), &MessagesPage::receiveMessages, Qt::QueuedConnection);
    QObject::connect(m_discordWebProcessor, &DiscordWebProcessor::sendNextEvents, m_uiBackend->eventsPage(), &MessagesPage::receiveNextMessages, Qt::QueuedConnection);

    QObject::connect(m_modsProcessor, &ModsProcessor::modInstallCompleeted, m_uiBackend->modsPage(), &ModsPage::receiveInstallCompleeted, Qt::QueuedConnection);
    QObject::connect(m_modsProcessor, &ModsProcessor::installProgress, m_uiBackend->modsPage(), &ModsPage::receiveDownloadProgress, Qt::QueuedConnection);
    QObject::connect(m_modsProcessor, &ModsProcessor::downloadError, m_uiBackend->modsPage(), &ModsPage::receiveDownloadError, Qt::QueuedConnection);
    QObject::connect(m_modsProcessor, &ModsProcessor::sendUnlockRacesStatus, m_uiBackend->modsPage(), &ModsPage::receiveUnlockRacesStatus, Qt::QueuedConnection);

    QObject::connect(m_rankedModServiceProcessor,   &RankedModServiceProcessor::sendPlyersRankedState, m_uiBackend->statisticPanel(), &StatisticPanel::receivePlyersRankedState , Qt::QueuedConnection);
    QObject::connect(m_rankedModServiceProcessor, &RankedModServiceProcessor::sendOnlineCount, m_uiBackend, &UiBackend::receiveOnlineCount, Qt::QueuedConnection);
    QObject::connect(m_rankedModServiceProcessor,   &RankedModServiceProcessor::sendPlyersRankedState, m_soulstormController->gameStateReader(), &GameStateReader::receivePlyersRankedState , Qt::QueuedConnection);

    QObject::connect(m_mapManager, &MapManager::sendMapItem, m_uiBackend->mapManagerPage(), &MapManagerPage::receiveMapItem, Qt::QueuedConnection);
    QObject::connect(m_mapManager, &MapManager::sendDownloadingProgress, m_uiBackend->mapManagerPage(), &MapManagerPage::receiveDownloadingProgress, Qt::QueuedConnection);
    QObject::connect(m_mapManager, &MapManager::sendMapImage, m_uiBackend->imageProvider(), &ImageProvider::receiveMapImage, Qt::QueuedConnection);
    QObject::connect(m_mapManager, &MapManager::mapsInfoLoaded, m_uiBackend->mapManagerPage(), &MapManagerPage::receiveMapsInfoLoaded, Qt::QueuedConnection);

    QObject::connect(m_statsServerProcessor, &StatsServerProcessor::sendStatisticModName, m_uiBackend, &UiBackend::receiveCurrentModName, Qt::QueuedConnection);
    QObject::connect(m_soulstormController->gameStateReader(),   &GameStateReader::sendCurrentMod,  m_uiBackend, &UiBackend::receiveCurrentModTechnicalName,   Qt::QueuedConnection);

    //QObject::connect(m_soulstormController, &SoulstormController::sendAuthKey, m_statsServerProcessor, &StatsServerProcessor::receiveAuthKey, Qt::QueuedConnection);

    QObject::connect(m_rankedModServiceProcessor, &RankedModServiceProcessor::sendModsOnlineCountMap, m_uiBackend->onlineStatisticPanel(), &OnlineStatisticPanel::receiveModsOnlineCountMap, Qt::QueuedConnection);

    m_settingsController->initializeSettings();
}

void Core::registerTypes()
{
    qRegisterMetaType<QVector<PlayerStats>>("QVector<PlayerStats>");
    qRegisterMetaType<ServerPlayerStats>("ServerPlayerStats");
    qRegisterMetaType<QList<PlayerInfoFromDowServer>>("QList<PlayerInfoFromDowServer>");
    qRegisterMetaType<SsMissionState>("SsMissionState");
    qRegisterMetaType<SendingReplayInfo>("SendingReplayInfo");
    qRegisterMetaType<PlayerInfoFromDowServer>("PlayerInfoFromDowServer");
    qRegisterMetaType<QList<DiscordMessage>>("QList<DiscordMessage>");
    qRegisterMetaType<QMap<QString, QImage>>("QMap<QString, QImage>");
    qRegisterMetaType<DiscordMessage>("DiscordMessage");
    qRegisterMetaType<QVector<PartyData>>("QVector<PartyData>");
    qRegisterMetaType<PartyData>("PartyData");
    qRegisterMetaType<InstMod>("InstMod");
    qRegisterMetaType<QVector<WinCondition>>("QVector<WinCondition>");
    qRegisterMetaType<QVector<PlyersRankedState>>("QVector<PlyersRankedState>");
    qRegisterMetaType<QVector<ReplayListInfo>>("QVector<ReplayListInfo>");
    qRegisterMetaType<MapItem*>("MapItem*");
    qRegisterMetaType<QList<MapFileHash>>("QList<MapFileHash>");
    qRegisterMetaType<QMap<QString, int>>("QMap<QString, int>");
}

OverlayWindowController *Core::overlayWindowController() const
{
    return m_overlayWindowController;
}

UiBackend *Core::uiBackend() const
{
    return m_uiBackend;
}

bool Core::event(QEvent *event)
{
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

void Core::onExit()
{
    qInfo(logInfo()) << "DowStats2 сlosed";

    m_overlayWindowController->onExit();
    m_logger->deleteLater();

    emit sendExit();
}

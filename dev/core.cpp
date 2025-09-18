#include <core.h>
#include <QDebug>
#include <QFile>
#include <gameStateReader.h>
#include <lobbyEventReader.h>
#include <baseTypes.h>
#include <hookManager.h>
#include <winuser.h>
#include <version.h>
#include <theme.h>

Core::Core(QQmlContext *context, QObject* parent)
    : QObject(parent)
    , m_logger(new Logger(this))
    , m_settingsController(new SettingsController(this))
    , m_keyboardProcessor(new KeyboardProcessor(this))
    , m_gameController(new GameController(m_settingsController, this))
    , m_overlayWindowController(new OverlayWindowController(m_settingsController, m_gameController, this))
    , m_discordWebProcessor(new DiscordWebProcessor(m_settingsController, this))
    , m_modsProcessor(new ModsProcessor(m_gameController->currentGame(), this))
    , m_soundProcessor(new SoundProcessor(this))
    , m_statsServerProcessor ( new StatsServerProcessor(m_settingsController, m_gameController->currentGame(), m_gameController->steamPath(), this))
    , m_rankedModServiceProcessor(new RankedModServiceProcessor(m_settingsController, this))
    , m_mapManager(new MapManager(m_settingsController, m_gameController->currentGame(), this))
    , m_balanceModManager(new BalanceModManager(m_settingsController, this))
    , m_uiBackend(new UiBackend(this, context))
{
    registerTypes();

    ///> TODO: По идее версию билда из другого места надо писать, но пишем отсюда ибо удобно.
    qInfo(logInfo()) << "Build version: " << QString::fromStdString(PROJECT_VERSION_MAJOR)
                                     + "." + QString::fromStdString(PROJECT_VERSION_MINOR)
                                     + "." + QString::fromStdString(GIT_REL);

    context->setContextProperty("_uiBackend", m_uiBackend);

    m_uiBackend->replayManager()->setGamePath(m_gameController->currentGame());
    m_uiBackend->mapManagerPage()->setGamePath(m_gameController->currentGame());

    HookManager::instance()->setCore(this);

    addConnections();

    m_overlayWindowController->setUiBackend(m_uiBackend);
    m_balanceModManager->setGamePath(m_gameController->currentGame());
    m_uiBackend->setGamePath(m_gameController->currentGame());
    m_uiBackend->setSteamPath(m_gameController->steamPath());

    m_settingsController->initializeSettings();

    m_uiBackend->setGamePathArray(m_gameController->gamePathArray());

}

void Core::registerTypes()
{
    qmlRegisterUncreatableType<DowStatsStyle::Theme>("Theme", 1, 0, "Theme", "theme style");
    qmlRegisterUncreatableType<BanType>("BanType", 1, 0, "BanType", "ban type");
    qmlRegisterUncreatableType<NotificationType>("NotificationType", 1, 0, "NotificationType", "notification type");
    qmlRegisterUncreatableType<GameType>("GameType", 1, 0, "GameType", "GameType");

    qRegisterMetaType<QVector<PlayerStats>>("QVector<PlayerStats>");
    qRegisterMetaType<ServerPlayerStats>("ServerPlayerStats");
    qRegisterMetaType<QList<PlayerInfoFromDowServer>>("QList<PlayerInfoFromDowServer>");
    qRegisterMetaType<GameMissionState>("GameMissionState");
    qRegisterMetaType<SendingReplayInfo>("SendingReplayInfo");
    qRegisterMetaType<PlayerInfoFromDowServer>("PlayerInfoFromDowServer");
    qRegisterMetaType<QList<DiscordMessage>>("QList<DiscordMessage>");
    qRegisterMetaType<DiscordMessage>("DiscordMessage");
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
    qRegisterMetaType<QList <ModInfo>>("QList <ModInfo>");
    qRegisterMetaType<LaunchMod>("LaunchMod");
    qRegisterMetaType<NotificationInfo>("NotificationInfo");
    qRegisterMetaType<RankDiversion>("RankDiversion");
    qRegisterMetaType<UniqueOnlineStatistic>("UniqueOnlineStatistic");
    qRegisterMetaType<DowServerRequestParametres>("DowServerRequestParametres");
}

void Core::addConnections()
{
    QObject::connect(m_keyboardProcessor, &KeyboardProcessor::expandKeyPressed, m_uiBackend, &UiBackend::expandKeyPressed, Qt::QueuedConnection);
    QObject::connect(m_keyboardProcessor, &KeyboardProcessor::altTabPressed, m_gameController, &GameController::minimizeSsWithWin7Support, Qt::QueuedConnection);

    QObject::connect(m_gameController,                    &GameController::gameMaximized,          m_overlayWindowController,    &OverlayWindowController::ssMaximized,      Qt::DirectConnection);
    QObject::connect(m_gameController,                    &GameController::gameMaximized,          m_soundProcessor,             &SoundProcessor::setGameMaximized,     Qt::DirectConnection);
    QObject::connect(m_gameController,                    &GameController::gameLaunchStateChanged, m_overlayWindowController,    &OverlayWindowController::gameLaunched,       Qt::QueuedConnection);
    QObject::connect(m_gameController,                    &GameController::gameLaunchStateChanged, m_balanceModManager,          &BalanceModManager::onGameLaunchStateChanged, Qt::QueuedConnection);
    QObject::connect(m_gameController,                    &GameController::inputBlockStateChanged, HookManager::instance(),    &HookManager::onInputBlockStateChanged,     Qt::QueuedConnection);
    QObject::connect(m_gameController->gameStateReader(),     &GameStateReader::gameInitialized,         m_overlayWindowController,  &OverlayWindowController::gameInitialized, Qt::DirectConnection);
    QObject::connect(m_gameController->gameStateReader(),     &GameStateReader::ssShutdown,              m_overlayWindowController,  &OverlayWindowController::onSsShutdowned,  Qt::QueuedConnection);
    QObject::connect(m_gameController->gameStateReader(),     &GameStateReader::sendCurrentMissionState, m_soundProcessor,           &SoundProcessor::receiveCurrentMissionState, Qt::QueuedConnection);
    QObject::connect(m_gameController->gameStateReader(),     &GameStateReader::gameInitialized,         m_statsServerProcessor,  [&](){m_statsServerProcessor->parseCurrentPlayerSteamId();}, Qt::QueuedConnection);
    QObject::connect(m_gameController->gameStateReader(),     &GameStateReader::sendCurrentMod,  m_rankedModServiceProcessor, &RankedModServiceProcessor::receiveCurrentMod,   Qt::QueuedConnection);
    QObject::connect(m_gameController->gameStateReader(),     &GameStateReader::sendCurrentMod,  m_statsServerProcessor, &StatsServerProcessor::receiveCurrentMod,   Qt::QueuedConnection);
    QObject::connect(m_gameController->lobbyEventReader(),    &LobbyEventReader::quitFromParty,      m_soundProcessor,  &SoundProcessor::activeIsFirstConnection,  Qt::QueuedConnection);
    QObject::connect(m_gameController->lobbyEventReader(),    &LobbyEventReader::hostParty,          m_soundProcessor,  &SoundProcessor::activeIsFirstConnection,  Qt::QueuedConnection);
    QObject::connect(m_gameController->lobbyEventReader(),    &LobbyEventReader::playerConnected,    m_soundProcessor,  &SoundProcessor::playPlayerJoinSound,  Qt::QueuedConnection);
    QObject::connect(m_gameController->lobbyEventReader(),    &LobbyEventReader::playerDisconnected, m_soundProcessor,  &SoundProcessor::playPlayerLeftSound,  Qt::QueuedConnection);
    QObject::connect(m_gameController->lobbyEventReader(),    &LobbyEventReader::playerKicked,       m_soundProcessor,  &SoundProcessor::playPlayerLeftSound,  Qt::QueuedConnection);
    QObject::connect(m_gameController->replayDataCollector(), &ReplayDataCollector::sendReplayToServer,       m_statsServerProcessor, &StatsServerProcessor::sendReplayToServer,   Qt::QueuedConnection);
    QObject::connect(m_gameController->dowServerProcessor(),  &DowServerProcessor::sendPlayersInfoFromDowServer, m_statsServerProcessor, &StatsServerProcessor::receivePlayresInfoFromDowServer, Qt::QueuedConnection);
    QObject::connect(m_gameController->dowServerProcessor(),  &DowServerProcessor::sendPlayersInfoFromDowServer, m_rankedModServiceProcessor, &RankedModServiceProcessor::receivePlayresInfoFromDowServer, Qt::QueuedConnection);

    QObject::connect(m_statsServerProcessor, &StatsServerProcessor::sendServerPlayerStats,     m_gameController->advertisingProcessor(),  &AdvertisingProcessor::receiveServerPlayerStats,      Qt::QueuedConnection);
    QObject::connect(m_statsServerProcessor, &StatsServerProcessor::sendCurrentPlayerSteamID, m_gameController->dowServerProcessor(), &DowServerProcessor::setCurrentPlayerSteamID, Qt::QueuedConnection);
    QObject::connect(m_statsServerProcessor, &StatsServerProcessor::replaySended,             m_gameController->advertisingProcessor(), &AdvertisingProcessor::onReplaySended, Qt::QueuedConnection);
    QObject::connect(m_statsServerProcessor, &StatsServerProcessor::sendCurrentPlayerSteamID, m_rankedModServiceProcessor, &RankedModServiceProcessor::setCurrentPlayerSteamIdSlot, Qt::QueuedConnection);
    QObject::connect(m_statsServerProcessor, &StatsServerProcessor::sendCurrentPlayerSteamID, m_balanceModManager, &BalanceModManager::setCurrentPlayerSteamId, Qt::QueuedConnection);

    QObject::connect(m_rankedModServiceProcessor,   &RankedModServiceProcessor::sendPlyersRankedState, m_gameController->gameStateReader(), &GameStateReader::receivePlyersRankedState , Qt::QueuedConnection);
    QObject::connect(m_uiBackend->gamePage(), &GamePage::currentGameChanged, m_mapManager, &MapManager::receiveLoadMapsInfo, Qt::QueuedConnection);


    QObject::connect(m_rankedModServiceProcessor,   &RankedModServiceProcessor::sendPlyersRankedState, m_gameController->replayDataCollector(), &ReplayDataCollector::receivePlyersRankedState , Qt::QueuedConnection);


    //QObject::connect(m_soulstormController, &SoulstormController::sendAuthKey, m_statsServerProcessor, &StatsServerProcessor::receiveAuthKey, Qt::QueuedConnection);
    //TODO: нужно для отладки спамилки рекламы
    //QObject::connect(m_uiBackend->statisticPanel(), &StatisticPanel::manualStatsRequest, m_soulstormController->advertisingProcessor(), &AdvertisingProcessor::onReplaySended, Qt::QueuedConnection);
}

GameController *Core::gameController() const
{
    return m_gameController;
}

RankedModServiceProcessor *Core::rankedModServiceProcessor() const
{
    return m_rankedModServiceProcessor;
}

StatsServerProcessor *Core::statsServerProcessor() const
{
    return m_statsServerProcessor;
}

SoundProcessor *Core::soundProcessor() const
{
    return m_soundProcessor;
}

MapManager *Core::mapManager() const
{
    return m_mapManager;
}

DiscordWebProcessor *Core::discordWebProcessor() const
{
    return m_discordWebProcessor;
}

ModsProcessor *Core::modsProcessor() const
{
    return m_modsProcessor;
}

BalanceModManager *Core::balanceModManager() const
{
    return m_balanceModManager;
}

SettingsController *Core::settingsController() const
{
    return m_settingsController;
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
            m_gameController->apmMeter()->onKeyPressEvent();
        }
    }
}

void Core::onMouseEvent(QMouseEvent event)
{
    if(uiBackend()->getShowClient()){
        if (event.type() == QEvent::MouseButtonPress)
        {
            m_uiBackend->mousePressEvent(event.pos());
            m_gameController->apmMeter()->onMousePressEvent();
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

void Core::exit()
{
    qInfo(logInfo()) << "DowStatsClient сlosed";

    m_overlayWindowController->onExit();
    m_logger->deleteLater();

    emit sendExit();
}

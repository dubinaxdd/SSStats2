#include "gameController.h"
#include <QTextCodec>
#include <QDebug>
#include <QGuiApplication>
#include <QFile>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>
#include <QDir>
#include <QStandardPaths>
#include <QDesktopServices>

#define SS_FULLSCREENIZE_TIMER_INTERVAL 2000

#define WINDOW_STATE_CHECK_INTERVAL 500
///<Интервал таймера проверки запуска/не запускака, свернутости/не развернутости

GameController::GameController(SettingsController *settingsController, QObject *parent)
    : QObject(parent)
    , m_steamPath(getSteamPathFromRegistry())
    , m_lobbyEventReader(new LobbyEventReader(this))
    , m_apmMeter(new APMMeter(this))
    , m_soulstormMemoryController(new SoulstormMemoryController(settingsController, this))
    , m_settingsController(settingsController)
    , m_gameStateReader(new GameStateReader(m_settingsController, this))
    , m_dowServerProcessor(new DowServerProcessor(this))
    , m_replayDataCollector(new ReplayDataCollector(this))
    , m_advertisingProcessor(new AdvertisingProcessor(m_settingsController, this))
    , m_gameProcess(nullptr)
{
    getGamePathFromRegistry();
    m_currentGame = m_gamePathArray.first();

    m_lobbyEventReader->setCurrentGame(&m_currentGame);
    m_gameStateReader->setCurrentGame(&m_currentGame);
    m_replayDataCollector->setCurrentGame(&m_currentGame);
    m_dowServerProcessor->setGameType(m_currentGame.gameType);

    if(m_steamPath.isEmpty() || !QDir(m_steamPath).exists())
        qWarning(logWarning()) << "Steam is not installed!" << m_steamPath;
    else
        qInfo(logInfo()) << "Steam path: " << m_steamPath;

    if(m_currentGame.gamePath.isEmpty() || !QDir(m_currentGame.gamePath).exists())
        qWarning(logWarning()) << "Game is not installed!" << m_currentGame.gamePath;
    else
        qInfo(logInfo()) << "Worked with Game from: " << m_currentGame.gamePath;

    m_gameMemoryReader = new GameMemoryReader();
    m_gameMemoryReader->setGameType(m_currentGame.gameType);

    m_gameWindowControllTimer = new QTimer(this);
    m_gameWindowControllTimer->setInterval(WINDOW_STATE_CHECK_INTERVAL);


    QObject::connect(this, &GameController::gameLaunchStateChanged, m_soulstormMemoryController, &SoulstormMemoryController::onGameLaunchStateChanged, Qt::QueuedConnection);
    QObject::connect(m_gameWindowControllTimer, &QTimer::timeout, this, &GameController::checkWindowState, Qt::QueuedConnection);
    QObject::connect(m_gameStateReader, &GameStateReader::gameInitialized,          this, &GameController::gameInitialized, Qt::QueuedConnection);

    QObject::connect(m_gameStateReader, &GameStateReader::ssShutdown,               this, &GameController::ssShutdown, Qt::QueuedConnection);
    QObject::connect(m_gameStateReader, &GameStateReader::sendCurrentMissionState,  m_apmMeter, &APMMeter::receiveMissionCurrentState,   Qt::QueuedConnection);
    QObject::connect(m_gameStateReader, &GameStateReader::sendCurrentMissionState,  m_lobbyEventReader, &LobbyEventReader::receiveCurrentMissionState,   Qt::QueuedConnection);
    QObject::connect(m_gameStateReader, &GameStateReader::sendCurrentMissionState,  m_replayDataCollector, &ReplayDataCollector::receiveCurrentMissionState,   Qt::QueuedConnection);
    QObject::connect(m_gameStateReader, &GameStateReader::sendCurrentModVersion,   m_dowServerProcessor, &DowServerProcessor::setCurrentModVersion, Qt::QueuedConnection);
    QObject::connect(m_gameStateReader, &GameStateReader::sendCurrentModVersion,          m_replayDataCollector, &ReplayDataCollector::receiveCurrentModVersion, Qt::QueuedConnection);
    QObject::connect(m_gameStateReader, &GameStateReader::sendCurrentMod,          m_replayDataCollector, &ReplayDataCollector::receiveCurrentMod, Qt::QueuedConnection);
    QObject::connect(m_gameStateReader, &GameStateReader::sendCurrentWinConditions,   m_replayDataCollector, &ReplayDataCollector::receiveCurrentWinConditions, Qt::QueuedConnection);
    QObject::connect(m_gameStateReader, &GameStateReader::localPlayerDroppedToObserver,  m_apmMeter, [=]{m_apmMeter->stopAnalys();},   Qt::QueuedConnection);
    QObject::connect(m_gameStateReader, &GameStateReader::sendRequestParametres,  m_dowServerProcessor, &DowServerProcessor::setRequestParametres,   Qt::QueuedConnection);

    QObject::connect(m_lobbyEventReader, &LobbyEventReader::requestSessionId,   m_gameMemoryReader, &GameMemoryReader::findSessionId, Qt::QueuedConnection);
    QObject::connect(m_lobbyEventReader, &LobbyEventReader::quitFromParty,      m_replayDataCollector,    &ReplayDataCollector::onQuitParty, Qt::QueuedConnection);

    QObject::connect(m_lobbyEventReader, &LobbyEventReader::playerConnected,    this, &GameController::requestDowPlayersData, Qt::QueuedConnection);
    QObject::connect(m_lobbyEventReader, &LobbyEventReader::playerDisconnected, this, &GameController::requestDowPlayersData, Qt::QueuedConnection);

    QObject::connect(m_lobbyEventReader, &LobbyEventReader::playerKicked,       m_dowServerProcessor, &DowServerProcessor::onPlayerDisconnected, Qt::QueuedConnection);
    QObject::connect(m_lobbyEventReader, &LobbyEventReader::playersListChanged,       m_dowServerProcessor, &DowServerProcessor::onAutomatchPlayersListChanged, Qt::QueuedConnection);
    QObject::connect(m_lobbyEventReader, &LobbyEventReader::findIgnoredPlayersId, m_gameMemoryReader, &GameMemoryReader::findIgnoredPlayersId, Qt::QueuedConnection);

    QObject::connect(m_dowServerProcessor, &DowServerProcessor::sendPlayersInfoFromDowServer, m_replayDataCollector, &ReplayDataCollector::receivePlayresInfoFromDowServer, Qt::QueuedConnection);
    QObject::connect(m_dowServerProcessor, &DowServerProcessor::sendCurrentPlayerId, m_lobbyEventReader, &LobbyEventReader::receiveCurrentPlayerId, Qt::QueuedConnection);

    QObject::connect(m_apmMeter, &APMMeter::sendAverrageApm, m_replayDataCollector,  &ReplayDataCollector::receiveAverrageApm,       Qt::QueuedConnection);

    QObject::connect(m_gameMemoryReader, &GameMemoryReader::sendSessionId, m_dowServerProcessor, &DowServerProcessor::setSessionId, Qt::QueuedConnection);
    QObject::connect(m_gameMemoryReader, &GameMemoryReader::sendSessionId, m_advertisingProcessor, &AdvertisingProcessor::setSessionId, Qt::QueuedConnection);
    QObject::connect(m_gameMemoryReader, &GameMemoryReader::sendSessionId, m_lobbyEventReader, [&]{m_lobbyEventReader->setSessionIdReceived(true); m_lobbyEventReader->setSessionIdRequested(false);});
    QObject::connect(m_gameMemoryReader, &GameMemoryReader::sendPlayersIdList, m_dowServerProcessor, &DowServerProcessor::onAutomatchPlayersListChanged, Qt::QueuedConnection);

    QObject::connect(m_gameStateReader, &GameStateReader::matchIdParsed, m_replayDataCollector, &ReplayDataCollector::setGameId, Qt::QueuedConnection);
    QObject::connect(m_replayDataCollector, &ReplayDataCollector::requestGameResults, m_dowServerProcessor, &DowServerProcessor::requestGameResult, Qt::QueuedConnection);
    QObject::connect(m_dowServerProcessor, &DowServerProcessor::sendGameResults, m_replayDataCollector, &ReplayDataCollector::receiveGameResults , Qt::QueuedConnection);
    QObject::connect(m_gameStateReader, &GameStateReader::sendGameRankedMode, m_replayDataCollector, &ReplayDataCollector::setRankedState, Qt::QueuedConnection);


    m_lobbyEventReader->checkPatyState();

    m_gameMemoryReader->moveToThread(&m_gameMemoryReaderThread);
    m_gameMemoryReaderThread.start();
    m_gameWindowControllTimer->start();
}

GameController::~GameController()
{
    m_gameMemoryReader->abort();
    m_gameMemoryReaderThread.quit();
    m_gameMemoryReaderThread.wait();
    m_gameMemoryReader->deleteLater();
}

void GameController::blockGameWindowInput(bool state)
{
    if (m_gameHwnd)
    {
        inputBlocked = state;
        EnableWindow(m_gameHwnd, !inputBlocked);
        emit inputBlockStateChanged(inputBlocked);
    }
}

void GameController::launchGame()
{
    if (m_currentGame.gameType == GameType::GameTypeEnum::DefinitiveEdition)
    {
        ///TODO: Если собрался это переделать - помни, так сделать потому что игра начила тупить при запуске если сделать как ты хочешь.
        QDesktopServices::openUrl(QUrl("steam://rungameid/3556750"));
        return;
    }

    QDir ssPath(m_currentGame.gamePath);

    if(!ssPath.exists())
        return;

    bool win7SupportMode = m_settingsController->getSettings()->win7SupportMode;
    bool launchGameInWindow = m_settingsController->getSettings()->launchGameInWindow;

    QSettings* gameSettings = new QSettings(m_currentGame.gameSettingsPath + "\\Local.ini", QSettings::Format::IniFormat);

    if (launchGameInWindow)
        gameSettings->setValue("global/screenwindowed", 1);
    else
    {
        if (win7SupportMode)
        {
            m_gameWindowWidth = gameSettings->value("global/screenwidth", 0).toInt();
            m_gameWindowHeight = gameSettings->value("global/screenheight", 0).toInt();
            gameSettings->setValue("global/screenwindowed", 1);
        }
        else
            gameSettings->setValue("global/screenwindowed", 0);
    }

    delete gameSettings;

    if (m_currentGame.gameType != GameType::GameTypeEnum::DefinitiveEdition)
        writeCurrentModSettingInGame();

    if(m_gameProcess == nullptr || !m_gameProcess->isOpen())
    {
        QString params = "";

        if (m_settingsController->getSettings()->skipIntroVideo)
            params.append("-nomovies");

        m_gameProcess = new QProcess(this);
        m_gameProcess->startDetached(m_currentGame.gamePath + "\\Soulstorm.exe", {params});

        m_useWindows7SupportMode = win7SupportMode;
    }

}

void GameController::onCurrentGameChanged()
{
    QMutex mutex;
    mutex.lock();
    m_dowServerProcessor->setGameType(m_currentGame.gameType);
    m_gameMemoryReader->setGameType(m_currentGame.gameType);
    mutex.unlock();

    checkWindowState();
}

void GameController::minimizeSsWithWin7Support()
{
    minimizeGame();
}

void GameController::checkWindowState()
{
    QDir gamePath(m_currentGame.gamePath);

    if(!gamePath.exists())
        return;

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");

    QString gameName;

    if (m_currentGame.gameType == GameType::GameTypeEnum::DefinitiveEdition)
        gameName = codec->toUnicode("Warhammer 40,000: Dawn of War");
    else
        gameName = codec->toUnicode("Dawn of War: Soulstorm");

    LPCWSTR lps = (LPCWSTR)gameName.utf16();

    m_gameHwnd = FindWindowW(NULL, lps);           ///<Ищем окно соулсторма

    m_soulstormMemoryController->setGameHwnd(m_gameHwnd);
    m_gameStateReader->setGameLounched(m_gameHwnd);

    if (m_gameHwnd && !m_gameWindowCreated)
    {
        m_gameWindowCreated = true;

        if (m_useWindows7SupportMode)
        {
            fullscrenizeGame();
        }
    }
   /* else
    {
        emit gameLaunchStateChanged(false);
        return;
    }*/


    if(m_gameInitialized != m_gameStateReader->getGameInitialized())
        m_gameInitialized = m_gameStateReader->getGameInitialized();
    else
    {
        if (!m_gameInitialized)
        {
            if (m_firstTimerTick)
            {
                m_firstTimerTick = false;
                emit gameLaunchStateChanged(false);
            }
            return;
        }
    }


    if (m_gameHwnd && m_gameInitialized)              ///<Если игра запущена и инициализирована
    {
        if(!m_ssLounchState)                                   ///<Если перед этим игра не была запущена
        {
            m_ssLounchState = true;                                ///<Устанавливаем запущенное состояние

            m_defaultGameWindowLong = GetWindowLong(m_gameHwnd, GWL_EXSTYLE);
            emit gameLaunchStateChanged(m_ssLounchState);                      ///<Отправляем сигнал о запуске игры
            qInfo(logInfo()) << "Game window accepted";

            if (m_settingsController->getSettings()->win7SupportMode && !m_settingsController->getSettings()->launchGameInWindow)
            {
                QSettings* ssSettings = new QSettings(m_currentGame.gameSettingsPath + "\\Local.ini", QSettings::Format::IniFormat);
                if( ssSettings->value("global/screenwindowed", 0).toInt() == 1)
                {
                    m_gameWindowWidth = ssSettings->value("global/screenwidth", 0).toInt();
                    m_gameWindowHeight = ssSettings->value("global/screenheight", 0).toInt();
                    m_useWindows7SupportMode = true;
                }
            }

            if( IsIconic(m_gameHwnd) || GetForegroundWindow() != m_gameHwnd)                      ///<Если игра свернута
            {
                m_ssMaximized = false;                              ///<Устанавливаем свернутое состояние
                emit gameMaximized(m_ssMaximized);                    ///<Отправляем сигнал о свернутости
                qInfo(logInfo()) << "Game minimized";
            }
            else                                                 ///<Если игра развернута
            {
                if (getUseWindows7SupportMode())
                    updateGameWindow();
                m_ssMaximized = true;                               ///<Естанавливаем развернутое состояние
                emit gameMaximized(m_ssMaximized);                    ///<Отправляем сигнал о развернутости
                qInfo(logInfo()) << "Game fullscreenized";
            }
        }
        else                                                ///<Если перед этим игра уже была запущена
        {
            if (m_useWindows7SupportMode)
            {
                if (!IsIconic(m_gameHwnd) && GetForegroundWindow() == m_gameHwnd)
                {
                    if (!m_ssMaximized)
                        fullscrenizeGame();
                }

            }
            else
            {
                if( IsIconic(m_gameHwnd) || GetForegroundWindow() != m_gameHwnd)                      ///<Если игра свернута
                {
                    if(m_ssMaximized)                                   ///<Если перед этим игра была развернута
                    {
                        m_ssMaximized = false;                              ///<Устанавливаем свернутое состояние
                        emit gameMaximized(m_ssMaximized);                    ///<Отправляем сигнал о свернутости
                        qInfo(logInfo()) << "Game minimized";
                    }
                }
                else                                                 ///<Если игра развернута
                {
                    if(!m_ssMaximized)
                    {
                        m_ssMaximized = true;                               ///<Естанавливаем развернутое состояние
                        emit gameMaximized(m_ssMaximized);                    ///<Отправляем сигнал о развернутости
                        qInfo(logInfo()) << "Game fullscreenized";
                    }
                }
            }
        }
    }
    else                                                ///<Если игра не запущена или не инициализирована
    {
        if(m_ssLounchState)                                    ///<Если игра была перед этим запущена
        {
            m_useWindows7SupportMode = false;
            m_gameWindowed = false;                               ///<Устанавливаем не оконный режим
            m_ssMaximized = false;                              ///<Устанавливаем свернутое состояние
            m_ssLounchState = false;                               ///<Устанавливаем выключенное состояние
            m_gameInitialized = false;
            m_gameWindowCreated = false;
            ChangeDisplaySettings(0, 0);
            m_gameHwnd=NULL;                               ///<Окно игры делаем  null
            m_gameStateReader->stopedGame();
            m_gameStateReader->setGameLounched(false);
            emit gameMaximized(m_ssMaximized);                    ///<Отправляем сигнал о свернутости
            emit gameLaunchStateChanged(m_ssLounchState);                      ///<Отправляем сигнал о том что сс выключен
            qWarning(logWarning()) << "Game window closed";
        }
        else
        {
            m_gameInitialized = false;
            qWarning(logWarning()) << "Game window not accepted";
        }
    }

    if (m_firstTimerTick)
    {
        m_firstTimerTick = false;
        emit gameLaunchStateChanged(m_ssLounchState);
    }
}

void GameController::gameInitialized()
{
    parseSsSettings();
    m_lobbyEventReader->activateReading(true);
}

void GameController::ssShutdown()
{
    m_lobbyEventReader->activateReading(false);
    m_lobbyEventReader->setSessionIdRequested(false);
    m_lobbyEventReader->setSessionIdReceived(false);

    /*m_gameHwnd = NULL;
    m_defaultGameWindowLong = NULL;
    m_gameProcess->deleteLater();*/
}

QString GameController::getGamePathFromRegistry()
{
    findDefinitiveEdition();
    findSoulstormPath();

    if (m_gamePathArray.isEmpty())
        return "";
    else
        return m_gamePathArray.last().gamePath;
}

QString GameController::getSteamPathFromRegistry()
{
    QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Valve\\Steam\\", QSettings::NativeFormat);

    QString steam_path =  settings.value("InstallPath").toString();
    if(steam_path.isEmpty())
    {
        QSettings settings_second("HKEY_CURRENT_USER\\Software\\Valve\\Steam", QSettings::NativeFormat);
        steam_path = settings_second.value("SteamPath").toString();
    }
    return steam_path;
}

void GameController::parseSsSettings()
{
    QDir ssPath(m_currentGame.gameSettingsPath);

    if(!ssPath.exists())
        return;

    QSettings* ssSettings = new QSettings(m_currentGame.gameSettingsPath + "\\Local.ini", QSettings::Format::IniFormat);

    int gameWindowed = ssSettings->value("global/screenwindowed", 0).toInt();
    m_gameWindowed = gameWindowed == 1;
    m_currentProfile = ssSettings->value("global/playerprofile","profile").toString();

    m_gameStateReader->setCurrentProfile(m_currentProfile);

    qInfo(logInfo()) << "Current profile: " << m_currentProfile;
    qInfo(logInfo()) << "Windowed mode = " << m_gameWindowed;

    delete ssSettings;
}

void GameController::updateGameWindow()
{
    minimizeGame();
    fullscrenizeGame();
    SetForegroundWindow(m_gameHwnd);
}

void GameController::writeCurrentModSettingInGame()
{
    QDir gamePath(m_currentGame.gamePath);
    QDir gameSettingsDir(m_currentGame.gamePath);

    if(!gamePath.exists() || !gameSettingsDir.exists())
        return;

    QSettings* gameSettings = new QSettings(m_currentGame.gamePath + "\\Local.ini", QSettings::Format::IniFormat);

    LaunchMod launchMode = m_settingsController->getSettings()->launchMode;

    switch (launchMode)
    {
        case LaunchMod::OriginalSoulstorm :
        {
                gameSettings->setValue("global/currentmoddc", "dxp2");
/*
            QFile fileCheck(m_currentGame->gamePath + "\\Engine\\Data\\art\\ui\\textures\\wxp_loadscreen_dawn_of_war111.dds" );
            QFile file(m_currentGame->gamePath + "\\Engine\\Data\\art\\ui\\textures\\wxp_loadscreen_dawn_of_war.dds" );

            if (fileCheck.exists() && file.exists())
                fileCheck.remove();

            if (file.exists())
                file.rename(m_currentGame->gamePath + "\\Engine\\Data\\art\\ui\\textures\\wxp_loadscreen_dawn_of_war111.dds");
*/
            break;
        }
        case LaunchMod::DowStatsBalanceMod :
        {
            gameSettings->setValue("global/currentmoddc", m_settingsController->getSettings()->lastActualBalanceMod.toLower());

/*
            QFile fileCheck(m_currentGame->gamePath + "\\Engine\\Data\\art\\ui\\textures\\wxp_loadscreen_dawn_of_war.dds" );
            QFile file(m_currentGame->gamePath + "\\Engine\\Data\\art\\ui\\textures\\wxp_loadscreen_dawn_of_war111.dds" );

            if (fileCheck.exists() && file.exists())
                fileCheck.remove();

            if (file.exists())
                file.rename(m_currentGame->gamePath + "\\Engine\\Data\\art\\ui\\textures\\wxp_loadscreen_dawn_of_war.dds");
*/
            break;
        }
        default: break;
    }

    delete gameSettings;
}

void GameController::findSoulstormPath()
{
    QString path = "";

    QSettings steam("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Steam App 9450", QSettings::NativeFormat);
    path = steam.value("installlocation", "").toString();

    if (path.isEmpty())
        path = steam.value("InstallLocation", "").toString();

    if (!checkGamePath(path, "Soulstorm.exe"))
        path = "";

    if(path.isEmpty())
    {
        QSettings sega("HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\THQ\\Dawn of War - Soulstorm", QSettings::NativeFormat);
        path = sega.value("installlocation", "").toString();

        if (path.isEmpty())
            path = sega.value("InstallLocation", "").toString();

        if (!checkGamePath(path, "Soulstorm.exe"))
            path = "";
    }

    if(path.isEmpty())
    {
        QSettings sega("HKEY_LOCAL_MACHINE\\SOFTWARE\\SEGA\\Dawn of War Soulstorm", QSettings::NativeFormat);
        path = sega.value("installlocation", "").toString();

        if (path.isEmpty())
            path = sega.value("InstallLocation", "").toString();

        if (!checkGamePath(path, "Soulstorm.exe"))
            path = "";
    }

    if(path.isEmpty())
    {
        QSettings sega("HKEY_LOCAL_MACHINE\\SOFTWARE\\THQ\\Dawn of War Soulstorm", QSettings::NativeFormat);
        path = sega.value("installlocation", "").toString();

        if (path.isEmpty())
            path = sega.value("InstallLocation", "").toString();

        if (!checkGamePath(path, "Soulstorm.exe"))
            path = "";
    }

    if(path.isEmpty())
    {
        QSettings sega("HKEY_LOCAL_MACHINE\\SOFTWARE\\THQ\\Dawn of War - Soulstorm", QSettings::NativeFormat);
        path = sega.value("installlocation", "").toString();

        if (path.isEmpty())
            path = sega.value("InstallLocation", "").toString();

        if (!checkGamePath(path, "Soulstorm.exe"))
            path = "";
    }

    if(path.isEmpty())
    {
        QSettings sega("HKEY_LOCAL_MACHINE\\SOFTWARE\\SEGA\\Dawn of War - Soulstorm", QSettings::NativeFormat);
        path = sega.value("installlocation", "").toString();

        if (path.isEmpty())
            path = sega.value("InstallLocation", "").toString();

        if (!checkGamePath(path, "Soulstorm.exe"))
            path = "";
    }

    if(path.isEmpty())
    {
        QSettings steam("HKEY_CURRENT_USER\\SOFTWARE\\Valve\\Steam", QSettings::NativeFormat);
        path = steam.value("SteamPath", "").toString() + "\\steamapps\\common\\Dawn of War Soulstorm";

        if (!checkGamePath(path, "Soulstorm.exe"))
            return;
    }

    if(!path.isEmpty())
    {
        GamePath gamePath;
        gamePath.gamePath = path;
        gamePath.gameSettingsPath = path;
        gamePath.gameType = GameType::GameTypeEnum::SoulstormSteam;
        gamePath.uiGameName = "Warhammer 40,000: Dawn of War - Soulstorm";

        m_gamePathArray.append(gamePath);
    }
}

void GameController::findDefinitiveEdition()
{
    QSettings relic("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Steam App 3556750", QSettings::NativeFormat);
    QString path = relic.value("installlocation", "").toString();

    if (path.isEmpty())
        path = relic.value("InstallLocation", "").toString();


    if (!checkGamePath(path, "W40k.exe"))
        path = "";

    if (path.isEmpty())
    {
        QString steamPath = getSteamPathFromRegistry();
        path = steamPath + "\\steamapps\\common\\Dawn of War Definitive Edition";

        if (!checkGamePath(path, "W40k.exe"))
            return;
    }

    QString gameSettingsPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).replace('/',"\\").replace("\\Roaming\\DowStatsClient", "") + "\\Roaming\\Relic Entertainment\\Dawn of War";

    if (!path.isEmpty())
    {
        GamePath gamePath;
        gamePath.gamePath = path;
        gamePath.gameSettingsPath = gameSettingsPath;
        gamePath.gameType = GameType::GameTypeEnum::DefinitiveEdition;
        gamePath.uiGameName = "Warhammer 40,000: Dawn of War - Definitive Edition";

        m_gamePathArray.append(gamePath);
    }
}

bool GameController::checkGamePath(QString path, QString exeName)
{
    QFile ssDir;
    ssDir.setFileName(path + QDir::separator() + exeName);
    return ssDir.exists();
}

QVector<GamePath> *GameController::gamePathArray()
{
    return &m_gamePathArray;
}

GamePath *GameController::currentGame()
{
    return &m_currentGame;
}

AdvertisingProcessor *GameController::advertisingProcessor() const
{
    return m_advertisingProcessor;
}

ReplayDataCollector *GameController::replayDataCollector() const
{
    return m_replayDataCollector;
}

const QString &GameController::steamPath() const
{
    return m_steamPath;
}

DowServerProcessor *GameController::dowServerProcessor() const
{
    return m_dowServerProcessor;
}

void GameController::fullscrenizeGame()
{
    if(m_gameHwnd)
    {
        DEVMODE deviceMode;

        deviceMode.dmSize = sizeof(deviceMode);
        deviceMode.dmPelsWidth = m_gameWindowWidth;
        deviceMode.dmPelsHeight = m_gameWindowHeight;
        deviceMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

        ChangeDisplaySettings(&deviceMode, CDS_FULLSCREEN);

        SetWindowLongW(m_gameHwnd, GWL_STYLE , WS_OVERLAPPED | WS_VISIBLE );
        SetWindowPos(m_gameHwnd,0,0,0,m_gameWindowWidth + 1, m_gameWindowHeight + 1, SWP_SHOWWINDOW );
        ShowWindow(m_gameHwnd, SW_SHOWNORMAL);

        m_defaultGameWindowLong = GetWindowLong(m_gameHwnd, GWL_EXSTYLE);

        m_ssMaximized = true;
        emit gameMaximized(m_ssMaximized);
        qInfo(logInfo()) << "Soulstorm maximized with win7 support mode";
    }
}

void GameController::minimizeGame()
{
    if (!m_useWindows7SupportMode)
        return;

    ChangeDisplaySettings(0, 0);

    ShowWindow(m_gameHwnd, SW_MINIMIZE);
    m_ssMaximized = false;
    emit gameMaximized(m_ssMaximized);
    qInfo(logInfo()) << "Soulstorm minimized with win7 support mode";
}

void GameController::requestDowPlayersData()
{
    if (!m_lobbyEventReader->automatchProcessed())
        m_dowServerProcessor->requestPartysData();
}

void GameController::onAutomatchNamesListChanged(QStringList automatchNamesList)
{
    for (auto& item : m_dowServerProcessor->lastPlayersInfo())
    {
        if(automatchNamesList.contains(item.name))
            automatchNamesList.removeAll(item.name);
    }

    m_gameMemoryReader->findIgnoredPlayersId(automatchNamesList);
}

bool GameController::getUseWindows7SupportMode() const
{
    return m_useWindows7SupportMode;
}

LobbyEventReader *GameController::lobbyEventReader() const
{
    return m_lobbyEventReader;
}

LONG GameController::defaultGameWindowLong() const
{
    return m_defaultGameWindowLong;
}

GameMemoryReader *GameController::gameMemoryReader() const
{
    return m_gameMemoryReader;
}

APMMeter *GameController::apmMeter() const
{
    return m_apmMeter;
}

SoulstormMemoryController *GameController::soulstormMemoryController() const
{
    return m_soulstormMemoryController;
}

HWND GameController::gameHwnd() const
{
    return m_gameHwnd;
}

bool GameController::gameWindowed() const
{
    return m_gameWindowed && !m_useWindows7SupportMode;
}

GameStateReader *GameController::gameStateReader() const
{
    return m_gameStateReader;
}

bool GameController::getSsMaximized()
{
    return m_ssMaximized;
}

bool GameController::getInputBlocked() const
{
    return inputBlocked;
}

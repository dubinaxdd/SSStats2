#include "sscontroller.h"
#include <QTextCodec>
#include <QDebug>
#include <QGuiApplication>
#include <QFile>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>

#define SS_FULLSCREENIZE_TIMER_INTERVAL 2000

#define WINDOW_STATE_CHECK_INTERVAL 1000
///<Интервал таймера проверки запуска/не запускака, свернутости/не развернутости

SsController::SsController(SettingsController *settingsController, QObject *parent)
    : QObject(parent)
    , m_settingsController(settingsController)
    , m_memoryController(new MemoryController(settingsController, this))
    , m_apmMeter(new APMMeter(this))
    , m_dowServerProcessor(new DowServerProcessor(this))
    , m_soulstormProcess(nullptr)
{

    m_ssPath = getSsPathFromRegistry();
    qInfo(logInfo()) << "Worked with Soulstorm from: " << m_ssPath;

    m_gameInfoReader = new GameInfoReader(m_ssPath, this);
    m_lobbyEventReader = new LobbyEventReader(m_ssPath, this);
    m_steamPath = getSteamPathFromRegistry();
    m_statsCollector = new StatsCollector(m_ssPath, m_steamPath, this);

    m_playersSteamScanner = new PlayersSteamScanner(/*this*/);

    QObject::connect(m_gameInfoReader, &GameInfoReader::gameInitialized, this, &SsController::gameInitialized, Qt::QueuedConnection);
    QObject::connect(m_gameInfoReader, &GameInfoReader::ssShutdown, this, &SsController::ssShutdown, Qt::QueuedConnection);

    QObject::connect(this, &SsController::ssLaunchStateChanged, m_memoryController, &MemoryController::onSsLaunchStateChanged, Qt::QueuedConnection);

    QObject::connect(m_lobbyEventReader, &LobbyEventReader::requestSessionId, m_playersSteamScanner, &PlayersSteamScanner::findSessionId, Qt::QueuedConnection);
    QObject::connect(m_gameInfoReader, &GameInfoReader::gameInitialized, m_playersSteamScanner, &PlayersSteamScanner::findSessionId, Qt::QueuedConnection);

    QObject::connect(m_lobbyEventReader, &LobbyEventReader::quitFromParty, m_gameInfoReader, &GameInfoReader::onQuitParty, Qt::QueuedConnection);

    m_ssWindowControllTimer = new QTimer(this);
    m_ssWindowControllTimer->setInterval(WINDOW_STATE_CHECK_INTERVAL);
    QObject::connect(m_ssWindowControllTimer, &QTimer::timeout, this, &SsController::checkWindowState, Qt::QueuedConnection);

    QObject::connect(m_gameInfoReader,  &GameInfoReader::gameOver,          m_apmMeter, &APMMeter::onGameStopped,      Qt::QueuedConnection);
    QObject::connect(m_gameInfoReader,  &GameInfoReader::startingMission,   m_apmMeter, &APMMeter::onGameStarted,   Qt::QueuedConnection);
    QObject::connect(m_gameInfoReader,  &GameInfoReader::gameStopped,       m_apmMeter, &APMMeter::onGameStopped,   Qt::QueuedConnection);
    QObject::connect(m_gameInfoReader,  &GameInfoReader::loadStarted,       m_apmMeter, &APMMeter::onGameStopped,   Qt::QueuedConnection);

    QObject::connect(m_gameInfoReader,  &GameInfoReader::loadStarted,       m_lobbyEventReader, &LobbyEventReader::onLoadStarted,   Qt::QueuedConnection);
    QObject::connect(m_gameInfoReader,  &GameInfoReader::gameStopped,       m_lobbyEventReader, &LobbyEventReader::onGameStopped,   Qt::QueuedConnection);

    QObject::connect(m_gameInfoReader,  &GameInfoReader::sendReplayToServer,       m_statsCollector, &StatsCollector::sendReplayToServer,   Qt::QueuedConnection);

    QObject::connect(m_apmMeter, &APMMeter::sendAverrageApm, m_gameInfoReader,  &GameInfoReader::receiveAverrageApm,       Qt::QueuedConnection);

    QObject::connect(m_playersSteamScanner, &PlayersSteamScanner::sendSessionId, m_dowServerProcessor, &DowServerProcessor::setSessionID, Qt::QueuedConnection);
    QObject::connect(m_statsCollector, &StatsCollector::sendCurrentPlayerSteamID, m_dowServerProcessor, &DowServerProcessor::setCurrentPlayerSteamID, Qt::QueuedConnection);
    QObject::connect(m_lobbyEventReader, &LobbyEventReader::playerConnected, m_dowServerProcessor, &DowServerProcessor::requestPartysData, Qt::QueuedConnection);
    QObject::connect(m_lobbyEventReader, &LobbyEventReader::playerDisconnected, m_dowServerProcessor, &DowServerProcessor::requestPartysData, Qt::QueuedConnection);
    QObject::connect(m_lobbyEventReader, &LobbyEventReader::playerKicked, m_dowServerProcessor, &DowServerProcessor::onPlayerDisconnected, Qt::QueuedConnection);

    QObject::connect(m_dowServerProcessor, &DowServerProcessor::sendSteamIds, m_statsCollector, &StatsCollector::receivePlayresStemIdFromScanner, Qt::QueuedConnection);
    QObject::connect(m_dowServerProcessor, &DowServerProcessor::sendSteamIds, m_gameInfoReader, &GameInfoReader::receivePlayresStemIdFromScanner, Qt::QueuedConnection);
    QObject::connect(m_gameInfoReader, &GameInfoReader::sendCurrentModeVersion, m_dowServerProcessor, &DowServerProcessor::setCurrentModVersion, Qt::QueuedConnection);

    m_lobbyEventReader->checkPatyState();

    m_playersSteamScanner->moveToThread(&m_playersSteamScannerThread);
    m_playersSteamScannerThread.start();
    m_ssWindowControllTimer->start();

    m_statsCollector->parseCurrentPlayerSteamId();
}

SsController::~SsController()
{
    m_playersSteamScannerThread.quit();
    m_playersSteamScannerThread.wait();

    m_playersSteamScannerThread.deleteLater();
    m_playersSteamScanner->deleteLater();
}

void SsController::blockInput(bool state)
{
    if (m_soulstormHwnd)
    {
        inputBlocked = state;
        EnableWindow(m_soulstormHwnd, !inputBlocked);
        emit inputBlockStateChanged(inputBlocked);
    }
}

void SsController::onLaunchSoulstorm()
{
    if(m_soulstormProcess == nullptr || !m_soulstormProcess->isOpen())
    {
        m_soulstormProcess = new QProcess(this);
        m_soulstormProcess->startDetached(m_ssPath+"\\Soulstorm.exe", {""});
    }
}

void SsController::launchSoulstormWithSupportMode()
{
    launchSoulstorm();
}

void SsController::minimizeSsWithWin7Support()
{
    minimizeSoulstorm();
}

void SsController::checkWindowState()
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString ss = codec->toUnicode("Dawn of War: Soulstorm");
    LPCWSTR lps = (LPCWSTR)ss.utf16();

    m_soulstormHwnd = FindWindowW(NULL, lps);           ///<Ищем окно соулсторма

    m_memoryController->setSoulstormHwnd(m_soulstormHwnd);
    m_gameInfoReader->setGameLounched(m_soulstormHwnd);

    if (m_soulstormHwnd && !m_ssWindowCreated)
    {
        m_ssWindowCreated = true;

        if (useWindows7SupportMode)
        {
            fullscrenizeSoulstorm();
        }
    }


    if(m_gameInitialized != m_gameInfoReader->getGameInitialized())
        m_gameInitialized = m_gameInfoReader->getGameInitialized();
    else
    {
         if (!m_gameInitialized)
             return;
    }


    if (m_soulstormHwnd && m_gameInitialized)              ///<Если игра запущена и инициализирована
    {
        if(!m_ssLounchState)                                   ///<Если перед этим игра не была запущена
        {
            m_ssLounchState = true;                                ///<Устанавливаем запущенное состояние

            m_defaultSoulstormWindowLong = GetWindowLong(m_soulstormHwnd, GWL_EXSTYLE);
            emit ssLaunchStateChanged(m_ssLounchState);                      ///<Отправляем сигнал о запуске игры
            qInfo(logInfo()) << "Soulstorm window accepted";

            if( IsIconic(m_soulstormHwnd))                      ///<Если игра свернута
            {
                m_ssMaximized = false;                              ///<Устанавливаем свернутое состояние
                emit ssMaximized(m_ssMaximized);                    ///<Отправляем сигнал о свернутости
                qInfo(logInfo()) << "Soulstorm minimized";
            }
            else                                                 ///<Если игра развернута
            {
                m_ssMaximized = true;                               ///<Естанавливаем развернутое состояние
                emit ssMaximized(m_ssMaximized);                    ///<Отправляем сигнал о развернутости
                qInfo(logInfo()) << "Soulstorm fullscreenized";
            }
        }
        else                                                ///<Если перед этим игра уже была запущена
        {
            if (useWindows7SupportMode)
            {
                if (!IsIconic(m_soulstormHwnd))
                {
                 if (!m_ssMaximized)
                    fullscrenizeSoulstorm();
                }

            }
            else
            {
                if( IsIconic(m_soulstormHwnd))                      ///<Если игра свернута
                {
                    if(m_ssMaximized)                                   ///<Если перед этим игра была развернута
                    {
                        m_ssMaximized = false;                              ///<Устанавливаем свернутое состояние
                        emit ssMaximized(m_ssMaximized);                    ///<Отправляем сигнал о свернутости
                        qInfo(logInfo()) << "Soulstorm minimized";
                    }
                }
                else                                                 ///<Если игра развернута
                {
                    if(!m_ssMaximized)
                    {
                        m_ssMaximized = true;                               ///<Естанавливаем развернутое состояние
                        emit ssMaximized(m_ssMaximized);                    ///<Отправляем сигнал о развернутости
                        qInfo(logInfo()) << "Soulstorm fullscreenized";
                    }
                }
            }
        }
    }
    else                                                ///<Если игра не запущена или не инициализирована
    {
        if(m_ssLounchState)                                    ///<Если игра была перед этим запущена
        {
            m_statsCollector->setCurrentPlayerAccepted(false);
            useWindows7SupportMode = false;
            m_ssWindowed = false;                               ///<Устанавливаем не оконный режим
            m_ssMaximized = false;                              ///<Устанавливаем свернутое состояние
            m_ssLounchState = false;                               ///<Устанавливаем выключенное состояние
            m_gameInitialized = false;
            m_ssWindowCreated = false;
            ChangeDisplaySettings(0, 0);
            m_soulstormHwnd=NULL;                               ///<Окно игры делаем  null
            m_gameInfoReader->stopedGame();
            m_gameInfoReader->setGameLounched(false);
            emit ssMaximized(m_ssMaximized);                    ///<Отправляем сигнал о свернутости
            emit ssLaunchStateChanged(m_ssLounchState);                      ///<Отправляем сигнал о том что сс выключен
            qWarning(logWarning()) << "Soulstorm window closed";
        }
        else
        {
            m_gameInitialized = false;
            qWarning(logWarning()) << "Soulstorm window not accepted";
        }
    }
}

void SsController::gameInitialized()
{
    parseSsSettings();
    m_statsCollector->parseCurrentPlayerSteamId();
    m_lobbyEventReader->activateReading(true);
}

void SsController::ssShutdown()
{
    m_lobbyEventReader->activateReading(false);
}

void SsController::receivePlayrStemids(QMap<QString, QString> infoMap)
{
    qInfo(logInfo()) << infoMap;
}

QString SsController::getSsPathFromRegistry()
{
    QString path = QCoreApplication::applicationDirPath();
    if(QFile::exists(path+"/Soulstorm.exe"))
        return path;

    QSettings thq("HKEY_LOCAL_MACHINE\\SOFTWARE\\THQ\\Dawn of War - Soulstorm", QSettings::NativeFormat);
    path = thq.value("installlocation", "").toString();

    if(path.isEmpty())
    {
        QSettings sega("HKEY_LOCAL_MACHINE\\SOFTWARE\\SEGA\\Dawn of War - Soulstorm", QSettings::NativeFormat);
        path = sega.value("installlocation", "").toString();
    }

    if(path.isEmpty())
    {
        QSettings steam("HKEY_CURRENT_USER\\SOFTWARE\\Valve\\Steam", QSettings::NativeFormat);
        path = steam.value("SteamPath", "").toString() + "\\steamapps\\common\\Dawn of War Soulstorm";
    }

    return path;
}

QString SsController::getSteamPathFromRegistry()
{
    QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Valve\\Steam\\", QSettings::NativeFormat);

    QString steam_path =  settings.value("InstallPath").toString();
    if(steam_path.isEmpty())
    {
        QSettings settings_second("HKEY_CURRENT_USER\\Software\\Valve\\Steam", QSettings::NativeFormat);
        steam_path = settings_second.value("SteamPath").toString();
    }
    qInfo(logInfo()) << "Steam path: " << steam_path;

    return steam_path;
}

void SsController::parseSsSettings()
{

    QSettings* ssSettings = new QSettings(m_ssPath+"\\Local.ini", QSettings::Format::IniFormat);
    m_ssWindowed = ssSettings->value("global/screenwindowed", 0).toInt();
    currentProfile = ssSettings->value("global/playerprofile","profile").toString();

    m_gameInfoReader->setCurrentProfile(currentProfile);

    qInfo(logInfo()) << "Current profile: " << currentProfile;
    qInfo(logInfo()) << "Windowed mode = " << m_ssWindowed;

    delete ssSettings;
}

void SsController::launchSoulstorm()
{

    QSettings* ssSettings = new QSettings(m_ssPath+"\\Local.ini", QSettings::Format::IniFormat);
    m_ssWindowWidth = ssSettings->value("global/screenwidth", 0).toInt();
    m_ssWindowHeight = ssSettings->value("global/screenheight", 0).toInt();

    ssSettings->setValue("global/screenwindowed", 1);

    delete ssSettings;

    if(m_soulstormProcess == nullptr || !m_soulstormProcess->isOpen())
    {
        m_soulstormProcess = new QProcess(this);
        m_soulstormProcess->startDetached(m_ssPath+"\\Soulstorm.exe", {""});
        useWindows7SupportMode = true;
    }
}

DowServerProcessor *SsController::dowServerProcessor() const
{
    return m_dowServerProcessor;
}

void SsController::fullscrenizeSoulstorm()
{
    if(m_soulstormHwnd)
    {
        DEVMODE deviceMode;

        deviceMode.dmSize = sizeof(deviceMode);
        deviceMode.dmPelsWidth = m_ssWindowWidth;
        deviceMode.dmPelsHeight = m_ssWindowHeight;
        deviceMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

        ChangeDisplaySettings(&deviceMode, CDS_FULLSCREEN);

        SetWindowLongW(m_soulstormHwnd, GWL_STYLE , /*GetWindowLong(m_soulstormHwnd, GWL_STYLE) |*/ WS_OVERLAPPED /*|  WS_POPUP*/ | WS_VISIBLE );
        SetWindowPos(m_soulstormHwnd,0,0,0,m_ssWindowWidth + 1, m_ssWindowHeight + 1, SWP_SHOWWINDOW );
        ShowWindow(m_soulstormHwnd, SW_SHOWNORMAL);

        m_defaultSoulstormWindowLong = GetWindowLong(m_soulstormHwnd, GWL_EXSTYLE);

        m_ssMaximized = true;
        emit ssMaximized(m_ssMaximized);
        qInfo(logInfo()) << "Soulstorm maximized with win7 support mode";
    }
}

void SsController::minimizeSoulstorm()
{
    if (!useWindows7SupportMode)
        return;

    ChangeDisplaySettings(0, 0);

    ShowWindow(m_soulstormHwnd, SW_MINIMIZE);
    m_ssMaximized = false;
    emit ssMaximized(m_ssMaximized);
    qInfo(logInfo()) << "Soulstorm minimized with win7 support mode";
}

bool SsController::getUseWindows7SupportMode() const
{
    return useWindows7SupportMode;
}

LobbyEventReader *SsController::lobbyEventReader() const
{
    return m_lobbyEventReader;
}

LONG SsController::defaultSoulstormWindowLong() const
{
    return m_defaultSoulstormWindowLong;
}

PlayersSteamScanner *SsController::playersSteamScanner() const
{
    return m_playersSteamScanner;
}

APMMeter *SsController::apmMeter() const
{
    return m_apmMeter;
}

MemoryController *SsController::memoryController() const
{
    return m_memoryController;
}

StatsCollector *SsController::statsCollector() const
{
    return m_statsCollector;
}

HWND SsController::soulstormHwnd() const
{
    return m_soulstormHwnd;
}

bool SsController::ssWindowed() const
{
    return m_ssWindowed && !useWindows7SupportMode;
}

GameInfoReader *SsController::gameInfoReader() const
{
    return m_gameInfoReader;
}

const QString &SsController::ssPath() const
{
    return m_ssPath;
}

bool SsController::getSsMaximized()
{
    return m_ssMaximized;
}

bool SsController::getInputBlocked() const
{
    return inputBlocked;
}

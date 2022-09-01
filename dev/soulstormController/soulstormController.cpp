#include "soulstormController.h"
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

SoulstormController::SoulstormController(SettingsController *settingsController, QObject *parent)
    : QObject(parent)
    , m_settingsController(settingsController)
    , m_soulstormMemoryController(new SoulstormMemoryController(settingsController, this))
    , m_apmMeter(new APMMeter(this))
    , m_dowServerProcessor(new DowServerProcessor(this))
    , m_soulstormProcess(nullptr)
{

    m_ssPath = getSsPathFromRegistry();
    qInfo(logInfo()) << "Worked with Soulstorm from: " << m_ssPath;

    m_gameStateReader = new GameStateReader(m_ssPath, this);
    m_lobbyEventReader = new LobbyEventReader(m_ssPath, this);
    m_replayDataCollector = new ReplayDataCollector(m_ssPath, this);

    m_steamPath = getSteamPathFromRegistry();

    m_soulstormMemoryReader = new SoulstormMemoryReader(/*this*/);

    m_ssWindowControllTimer = new QTimer(this);
    m_ssWindowControllTimer->setInterval(WINDOW_STATE_CHECK_INTERVAL);


    QObject::connect(this, &SoulstormController::ssLaunchStateChanged, m_soulstormMemoryController, &SoulstormMemoryController::onSsLaunchStateChanged, Qt::QueuedConnection);

    QObject::connect(m_ssWindowControllTimer, &QTimer::timeout, this, &SoulstormController::checkWindowState, Qt::QueuedConnection);

    QObject::connect(m_gameStateReader, &GameStateReader::gameInitialized,          this, &SoulstormController::gameInitialized, Qt::QueuedConnection);
    QObject::connect(m_gameStateReader, &GameStateReader::gameInitialized,          m_soulstormMemoryReader, &SoulstormMemoryReader::findSessionId, Qt::QueuedConnection);
    QObject::connect(m_gameStateReader, &GameStateReader::ssShutdown,               this, &SoulstormController::ssShutdown, Qt::QueuedConnection);
    QObject::connect(m_gameStateReader, &GameStateReader::sendCurrentMissionState,  m_apmMeter, &APMMeter::receiveMissionCurrentState,   Qt::QueuedConnection);
    QObject::connect(m_gameStateReader, &GameStateReader::sendCurrentMissionState,  m_lobbyEventReader, &LobbyEventReader::receiveCurrentMissionState,   Qt::QueuedConnection);
    QObject::connect(m_gameStateReader, &GameStateReader::sendCurrentModeVersion,   m_dowServerProcessor, &DowServerProcessor::setCurrentModVersion, Qt::QueuedConnection);
    QObject::connect(m_gameStateReader, &GameStateReader::sendCurrentMode,   m_replayDataCollector, &ReplayDataCollector::receiveCurrentMode, Qt::QueuedConnection);

    QObject::connect(m_gameStateReader, &GameStateReader::localPlayerDroppedToObserver,  m_apmMeter, [=]{m_apmMeter->stopAnalys();},   Qt::QueuedConnection);
    QObject::connect(m_lobbyEventReader, &LobbyEventReader::requestSessionId,   m_soulstormMemoryReader, &SoulstormMemoryReader::findSessionId, Qt::QueuedConnection);
    QObject::connect(m_lobbyEventReader, &LobbyEventReader::quitFromParty,      m_replayDataCollector,    &ReplayDataCollector::onQuitParty, Qt::QueuedConnection);
    QObject::connect(m_lobbyEventReader, &LobbyEventReader::playerConnected,    m_dowServerProcessor, &DowServerProcessor::requestPartysData, Qt::QueuedConnection);
    QObject::connect(m_lobbyEventReader, &LobbyEventReader::playerDisconnected, m_dowServerProcessor, &DowServerProcessor::requestPartysData, Qt::QueuedConnection);
    QObject::connect(m_lobbyEventReader, &LobbyEventReader::playerKicked,       m_dowServerProcessor, &DowServerProcessor::onPlayerDisconnected, Qt::QueuedConnection);

    QObject::connect(m_dowServerProcessor, &DowServerProcessor::sendPlayersInfoFromDowServer, m_replayDataCollector, &ReplayDataCollector::receivePlayresInfoFromDowServer, Qt::QueuedConnection);

    QObject::connect(m_apmMeter, &APMMeter::sendAverrageApm, m_replayDataCollector,  &ReplayDataCollector::receiveAverrageApm,       Qt::QueuedConnection);

    QObject::connect(m_soulstormMemoryReader, &SoulstormMemoryReader::sendSessionId, m_dowServerProcessor, &DowServerProcessor::setSessionID, Qt::QueuedConnection);

    m_lobbyEventReader->checkPatyState();

    m_soulstormMemoryReader->moveToThread(&m_soulstormMemoryReaderThread);
    m_soulstormMemoryReaderThread.start();
    m_ssWindowControllTimer->start();
}

SoulstormController::~SoulstormController()
{
    m_soulstormMemoryReaderThread.quit();
    m_soulstormMemoryReaderThread.wait();

    m_soulstormMemoryReaderThread.deleteLater();
    m_soulstormMemoryReader->deleteLater();
}

void SoulstormController::blockSsWindowInput(bool state)
{
    if (m_soulstormHwnd)
    {
        inputBlocked = state;
        EnableWindow(m_soulstormHwnd, !inputBlocked);
        emit inputBlockStateChanged(inputBlocked);
    }
}

void SoulstormController::launchSoulstorm()
{
    QSettings* ssSettings = new QSettings(m_ssPath+"\\Local.ini", QSettings::Format::IniFormat);
    ssSettings->setValue("global/screenwindowed", 0);

    if(m_soulstormProcess == nullptr || !m_soulstormProcess->isOpen())
    {
        QString params = "";

        if (m_settingsController->getSettings()->skipIntroVideo)
            params.append("-nomovies");

        m_soulstormProcess = new QProcess(this);
        m_soulstormProcess->startDetached(m_ssPath+"\\Soulstorm.exe", {params});
        m_useWindows7SupportMode = false;
    }
}

void SoulstormController::launchSoulstormWithSupportMode()
{
    QSettings* ssSettings = new QSettings(m_ssPath+"\\Local.ini", QSettings::Format::IniFormat);
    m_ssWindowWidth = ssSettings->value("global/screenwidth", 0).toInt();
    m_ssWindowHeight = ssSettings->value("global/screenheight", 0).toInt();

    ssSettings->setValue("global/screenwindowed", 1);

    delete ssSettings;

    if(m_soulstormProcess == nullptr || !m_soulstormProcess->isOpen())
    {
        QString params = "";

        if (m_settingsController->getSettings()->skipIntroVideo)
            params.append("-nomovies");

        m_soulstormProcess = new QProcess(this);
        m_soulstormProcess->startDetached(m_ssPath+"\\Soulstorm.exe", {params});
        m_useWindows7SupportMode = true;
    }
}

void SoulstormController::minimizeSsWithWin7Support()
{
    minimizeSoulstorm();
}

void SoulstormController::checkWindowState()
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString ss = codec->toUnicode("Dawn of War: Soulstorm");
    LPCWSTR lps = (LPCWSTR)ss.utf16();

    m_soulstormHwnd = FindWindowW(NULL, lps);           ///<Ищем окно соулсторма

    m_soulstormMemoryController->setSoulstormHwnd(m_soulstormHwnd);
    m_gameStateReader->setGameLounched(m_soulstormHwnd);

    if (m_soulstormHwnd && !m_ssWindowCreated)
    {
        m_ssWindowCreated = true;

        if (m_useWindows7SupportMode)
        {
            fullscrenizeSoulstorm();
        }
    }


    if(m_gameInitialized != m_gameStateReader->getGameInitialized())
        m_gameInitialized = m_gameStateReader->getGameInitialized();
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
            if (m_useWindows7SupportMode)
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
            m_useWindows7SupportMode = false;
            m_ssWindowed = false;                               ///<Устанавливаем не оконный режим
            m_ssMaximized = false;                              ///<Устанавливаем свернутое состояние
            m_ssLounchState = false;                               ///<Устанавливаем выключенное состояние
            m_gameInitialized = false;
            m_ssWindowCreated = false;
            ChangeDisplaySettings(0, 0);
            m_soulstormHwnd=NULL;                               ///<Окно игры делаем  null
            m_gameStateReader->stopedGame();
            m_gameStateReader->setGameLounched(false);
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

void SoulstormController::gameInitialized()
{
    parseSsSettings();
    m_lobbyEventReader->activateReading(true);
}

void SoulstormController::ssShutdown()
{
    m_lobbyEventReader->activateReading(false);
}

QString SoulstormController::getSsPathFromRegistry()
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
        QSettings sega("HKEY_LOCAL_MACHINE\\SOFTWARE\\SEGA\\Dawn of War Soulstorm", QSettings::NativeFormat);
        path = sega.value("installlocation", "").toString();
    }


    if(path.isEmpty())
    {
        QSettings steam("HKEY_CURRENT_USER\\SOFTWARE\\Valve\\Steam", QSettings::NativeFormat);
        path = steam.value("SteamPath", "").toString() + "\\steamapps\\common\\Dawn of War Soulstorm";
    }

    return path;
}

QString SoulstormController::getSteamPathFromRegistry()
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

void SoulstormController::parseSsSettings()
{

    QSettings* ssSettings = new QSettings(m_ssPath+"\\Local.ini", QSettings::Format::IniFormat);
    m_ssWindowed = ssSettings->value("global/screenwindowed", 0).toInt();
    m_currentProfile = ssSettings->value("global/playerprofile","profile").toString();

    m_gameStateReader->setCurrentProfile(m_currentProfile);

    qInfo(logInfo()) << "Current profile: " << m_currentProfile;
    qInfo(logInfo()) << "Windowed mode = " << m_ssWindowed;

    delete ssSettings;
}

ReplayDataCollector *SoulstormController::replayDataCollector() const
{
    return m_replayDataCollector;
}

const QString &SoulstormController::steamPath() const
{
    return m_steamPath;
}

DowServerProcessor *SoulstormController::dowServerProcessor() const
{
    return m_dowServerProcessor;
}

void SoulstormController::fullscrenizeSoulstorm()
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

void SoulstormController::minimizeSoulstorm()
{
    if (!m_useWindows7SupportMode)
        return;

    ChangeDisplaySettings(0, 0);

    ShowWindow(m_soulstormHwnd, SW_MINIMIZE);
    m_ssMaximized = false;
    emit ssMaximized(m_ssMaximized);
    qInfo(logInfo()) << "Soulstorm minimized with win7 support mode";
}

bool SoulstormController::getUseWindows7SupportMode() const
{
    return m_useWindows7SupportMode;
}

LobbyEventReader *SoulstormController::lobbyEventReader() const
{
    return m_lobbyEventReader;
}

LONG SoulstormController::defaultSoulstormWindowLong() const
{
    return m_defaultSoulstormWindowLong;
}

SoulstormMemoryReader *SoulstormController::soulstormMemoryReader() const
{
    return m_soulstormMemoryReader;
}

APMMeter *SoulstormController::apmMeter() const
{
    return m_apmMeter;
}

SoulstormMemoryController *SoulstormController::soulstormMemoryController() const
{
    return m_soulstormMemoryController;
}

HWND SoulstormController::soulstormHwnd() const
{
    return m_soulstormHwnd;
}

bool SoulstormController::ssWindowed() const
{
    return m_ssWindowed && !m_useWindows7SupportMode;
}

GameStateReader *SoulstormController::gameStateReader() const
{
    return m_gameStateReader;
}

const QString &SoulstormController::ssPath() const
{
    return m_ssPath;
}

bool SoulstormController::getSsMaximized()
{
    return m_ssMaximized;
}

bool SoulstormController::getInputBlocked() const
{
    return inputBlocked;
}

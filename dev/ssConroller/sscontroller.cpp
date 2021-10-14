#include "sscontroller.h"
#include <QTextCodec>
#include <QDebug>
#include <QGuiApplication>
#include <QFile>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>

#define WINDOW_STATE_CHECK_INTERVAL 200
///<Интервал таймера проверки запуска/не запускака, свернутости/не развернутости


SsController::SsController(QObject *parent)
    : QObject(parent)
    , m_memoryController(new MemoryController(this))
    , m_apmMeter(new APMMeter(this))
{
    m_ssPath = getSsPathFromRegistry();
    qInfo(logInfo()) << "Worked with Soulstorm from: " << m_ssPath;
    m_gameInfoReader = new GameInfoReader(m_ssPath,this);

    m_steamPath = getSteamPathFromRegistry();

    m_statsCollector = new StatsCollector(m_ssPath, m_steamPath, this);

    m_playersSteamScanner = new PlayersSteamScanner(/*this*/);

    QObject::connect(m_gameInfoReader, &GameInfoReader::gameInitialized, this, &SsController::gameInitialized, Qt::QueuedConnection);
    QObject::connect(m_gameInfoReader, &GameInfoReader::ssShutdown, this, &SsController::ssShutdown, Qt::QueuedConnection);
    QObject::connect(m_gameInfoReader, &GameInfoReader::startingMission, this, &SsController::readTestStats, Qt::QueuedConnection);
    QObject::connect(this, &SsController::ssLaunchStateChanged, m_memoryController, &MemoryController::onSsLaunchStateChanged, Qt::QueuedConnection);

    QObject::connect(m_gameInfoReader, &GameInfoReader::loadStarted, m_playersSteamScanner->scanTimer(), &QTimer::stop, Qt::QueuedConnection);
    QObject::connect(m_gameInfoReader, &GameInfoReader::gameStopped, m_playersSteamScanner->scanTimer(), static_cast<void (QTimer::*)(void)>(&QTimer::start), Qt::QueuedConnection);

    m_ssWindowControllTimer = new QTimer(this);
    m_ssWindowControllTimer->setInterval(WINDOW_STATE_CHECK_INTERVAL);
    QObject::connect(m_ssWindowControllTimer, &QTimer::timeout, this, &SsController::checkWindowState, Qt::QueuedConnection);

    QObject::connect(m_playersSteamScanner, &PlayersSteamScanner::sendSteamPlayersInfoMap, m_statsCollector, &StatsCollector::receivePlayresStemIdFromScanner, Qt::QueuedConnection);

    QObject::connect(m_gameInfoReader,  &GameInfoReader::gameOver,          m_apmMeter, &APMMeter::onGameStopped,      Qt::QueuedConnection);
    QObject::connect(m_gameInfoReader,  &GameInfoReader::startingMission,   m_apmMeter, &APMMeter::onGameStarted,   Qt::QueuedConnection);
    QObject::connect(m_gameInfoReader,  &GameInfoReader::gameStopped,       m_apmMeter, &APMMeter::onGameStopped,   Qt::QueuedConnection);
    QObject::connect(m_gameInfoReader,  &GameInfoReader::loadStarted,       m_apmMeter, &APMMeter::onGameStopped,   Qt::QueuedConnection);

    QObject::connect(m_gameInfoReader,  &GameInfoReader::sendReplayToServer,       m_statsCollector, &StatsCollector::sendReplayToServer,   Qt::QueuedConnection);

    QObject::connect(m_apmMeter, &APMMeter::sendAverrageApm, m_gameInfoReader,  &GameInfoReader::receiveAverrageApm,       Qt::QueuedConnection);
    QObject::connect(m_playersSteamScanner, &PlayersSteamScanner::sendSteamPlayersInfoMap, m_gameInfoReader, &GameInfoReader::receivePlayresStemIdFromScanner, Qt::QueuedConnection);


    m_playersSteamScanner->moveToThread(&m_playersSteamScannerThread);
    m_playersSteamScannerThread.start();
    m_ssWindowControllTimer->start();
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

void SsController::checkWindowState()
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString ss = codec->toUnicode("Dawn of War: Soulstorm");
    LPCWSTR lps = (LPCWSTR)ss.utf16();

    m_soulstormHwnd = FindWindowW(NULL, lps);           ///<Ищем окно соулсторма

    m_memoryController->setSoulstormHwnd(m_soulstormHwnd);
    m_gameInfoReader->setGameLounched(m_soulstormHwnd);

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
             m_defaultSoulstormWindowLong = GetWindowLongPtr(m_soulstormHwnd, GWL_EXSTYLE);
             parseSsSettings();                                  ///<Считываем настройки соулсторма
             emit ssLaunchStateChanged(m_ssLounchState);                      ///<Отправляем сигнал о запуске игры
             qInfo(logInfo()) << "Soulstorm window opened";

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
                 qInfo(logInfo()) << "Soulstorm fullscreen";
             }
         }
         else                                                ///<Если перед этим игра уже была запущена
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
                     qInfo(logInfo()) << "Soulstorm fullscreen";
                 }
             }
         }
     }
     else                                                ///<Если игра не запущена или не инициализирована
     {
         if(m_ssLounchState)                                    ///<Если игра была перед этим запущена
         {
             m_statsCollector->setCurrentPlayerAccepted(false);
             m_ssWindowed = false;                               ///<Устанавливаем не оконный режим
             m_ssMaximized = false;                              ///<Устанавливаем свернутое состояние
             m_ssLounchState = false;                               ///<Устанавливаем выключенное состояние
             m_soulstormHwnd=NULL;                               ///<Окно игры делаем  null
             m_gameInfoReader->ssWindowClosed();                 ///<Говорим инфоРидеру что окно сс закрыто
             emit ssMaximized(m_ssMaximized);                    ///<Отправляем сигнал о свернутости
             emit ssLaunchStateChanged(m_ssLounchState);                      ///<Отправляем сигнал о том что сс выключен

             //m_ssLaunchControllTimer->stop();                    ///<Останавливаем таймер контроля запуска
             qWarning(logWarning()) << "Soulstorm window closed";
         }
         else
         {
             qWarning(logWarning()) << "Soulstorm window not accepted";
         }
     }
}

void SsController::gameInitialized()
{
    parseSsSettings();

    m_statsCollector->parseCurrentPlayerSteamId();
}

void SsController::ssShutdown()
{


}

void SsController::readTestStats()
{
    QString statsPath = m_ssPath + "\\Profiles\\" + currentProfile + "\\teststats.lua";
    qInfo(logInfo()) << "teststats.lua path: " << statsPath;

    QFile file(statsPath);

    if(!file.open(QIODevice::ReadOnly))
        return;
    if(!file.isReadable())
        return;

    // в начале файла лежат байты из-за этого корневой ключ может не читаться
    int k=0;
    while(file.read(1)!="G")
        k++;
    file.seek(k);

    //QByteArray testStats = file.readAll();

    QTextStream textStream(&file);
    QStringList fileLines = textStream.readAll().split("\r");

    file.close();

    QStringList playerNames;
    QStringList playerRaces;
    QStringList playerTeam;

    for(int i = 0; i < fileLines.size(); i++ )
    {
        if (fileLines[i].contains("PName")){

            QString name = fileLines[i].right(fileLines[i].length() - 12);
            name = name.left(name.length() - 2);
            playerNames.append(name);
        }

        if (fileLines[i].contains("PRace")){

            QString race = fileLines[i].right(fileLines[i].length() - 12);
            race = race.left(race.length() - 2);
            playerRaces.append(race);
        }


        if (fileLines[i].contains("PTeam")){

            QString team = fileLines[i].right(fileLines[i].length() - 11);
            team = team.left(team.length() - 1);
            playerTeam.append(team);
        }
    }

    QVector<PlayerStats> playerStats;

    playerStats.resize(8);

    for(int i = 0; i < playerNames.count(); i++ )
    {
        playerStats[i].name = playerNames.at(i).toLocal8Bit();
        qInfo(logInfo()) << "Player from test stats" << playerStats[i].name;
    }

    for(int i = 0; i < playerRaces.count(); i++ )
        playerStats[i].race = playerRaces.at(i);

    for(int i = 0; i < playerTeam.count(); i++ )
        playerStats[i].team = playerTeam.at(i);

    //Сортировка игроков по команде
    for(int i = 0; i < playerStats.count(); i++)
    {
        for(int j = 0; j < playerStats.count() - 1; j++)
        {
            if(playerStats[j].team > playerStats[j + 1].team)
            {
                auto buffer = playerStats[j];
                playerStats[j] = playerStats[j + 1];
                playerStats[j + 1] = buffer;
            }
        }
    }

    emit sendPlayersTestStats(playerStats);

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
    return m_ssWindowed;
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

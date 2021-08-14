#include "sscontroller.h"
#include <QTextCodec>
#include <QDebug>
#include <QGuiApplication>
#include <QFile>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>

#define CHECK_SS_TIMER_INTERVAL 100  ///<Интервал таймера проверки запуска/не запускака, свернутости/не развернутости

SsController::SsController(QObject *parent) : QObject(parent)
{
    m_ssPath = getSsPathFromRegistry();
    qDebug() << "INFO: Worked with Soulstorm from: " << m_ssPath;
    m_gameInfoReader = new GameInfoReader(m_ssPath,this);

    m_steamPath = getSteamPathFromRegistry();

    m_statsCollector = new StatsCollector(m_steamPath, this);


    QObject::connect(m_gameInfoReader, &GameInfoReader::gameInitialized, this, &SsController::gameInitialized, Qt::QueuedConnection);
    QObject::connect(m_gameInfoReader, &GameInfoReader::ssShutdown, this, &SsController::ssShutdown, Qt::QueuedConnection);
    QObject::connect(m_gameInfoReader, &GameInfoReader::startingMission, this, &SsController::readTestStats, Qt::QueuedConnection);



    m_ssLaunchControllTimer = new QTimer(this);
    m_ssLaunchControllTimer->setInterval(CHECK_SS_TIMER_INTERVAL);
    QObject::connect(m_ssLaunchControllTimer, &QTimer::timeout, this, &SsController::checkSS, Qt::QueuedConnection);
}

void SsController::blockInput(bool block)
{
    if (m_soulstormHwnd)
    {
        inputBlocked = block;
        EnableWindow(m_soulstormHwnd, !inputBlocked);
    }
}


void SsController::checkSS()
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString ss = codec->toUnicode("Dawn of War: Soulstorm");
    LPCWSTR lps = (LPCWSTR)ss.utf16();

    m_soulstormHwnd = FindWindowW(NULL, lps);           ///<Ищем окно соулсторма

    if (m_soulstormHwnd)                                ///<Если игра запущена
    {
        if(!m_ssLounchState)                                   ///<Если перед этим игра не была запущена
        {
            parseSsSettings();                                  ///<Считываем настройки соулсторма
            m_ssLounchState = true;                                ///<Устанавливаем запущенное состояние
            emit ssLaunchStateChanged(m_ssLounchState);                      ///<Отправляем сигнал о запуске игры
            qDebug() << "INFO: Soulstorm window opened";
        }
        else                                                ///<Если перед этим игра уже была запущена
        {
            if( IsIconic(m_soulstormHwnd))                      ///<Если игра свернута
            {
                if(m_ssMaximized)                                   ///<Если перед этим игра была развернута
                {
                    m_ssMaximized = false;                              ///<Устанавливаем свернутое состояние
                    emit ssMaximized(m_ssMaximized);                    ///<Отправляем сигнал о свернутости
                    qDebug() << "INFO: Soulstorm minimized";
                }
            }
            else                                                 ///<Если игра развернута
            {
                if(!m_ssMaximized)
                {
                    m_ssMaximized = true;                               ///<Естанавливаем развернутое состояние
                    emit ssMaximized(m_ssMaximized);                    ///<Отправляем сигнал о развернутости
                    qDebug() << "INFO: Soulstorm fullscreen";
                }
            }
        }
    }
    else                                                ///<Если игра не запущена
    {
        if(m_ssLounchState)                                    ///<Если игра была перед этим запущена
        {

            m_ssWindowed = false;                               ///<Устанавливаем не оконный режим
            m_ssMaximized = false;                              ///<Устанавливаем свернутое состояние
            emit ssMaximized(m_ssMaximized);                    ///<Отправляем сигнал о свернутости
            m_ssLounchState = false;                               ///<Устанавливаем выключенное состояние
            emit ssLaunchStateChanged(m_ssLounchState);                      ///<Отправляем сигнал о том что сс выключен
            m_gameInfoReader->ssWindowClosed();                 ///<Говорим инфоРидеру что окно сс закрыто
            m_soulstormHwnd=NULL;                               ///<Окно игры делаем  null
            m_ssLaunchControllTimer->stop();                    ///<Останавливаем таймер контроля запуска
            qDebug() << "WARNING: Soulstorm window closed";
        }
        else
        {
           // qDebug() << "WARNING: Soulstorm window not accepted";
        }
    }
}

void SsController::gameInitialized()
{
    parseSsSettings();
    m_ssLaunchControllTimer->start();                   ///<Запускаем таймер который будет определять игра запущена/не запущена, максимизирована/не максимизирована
    m_statsCollector->parseCurrentPlayerSteamId();
}

void SsController::ssShutdown()
{


}

void SsController::readTestStats()
{
    QString statsPath = m_ssPath + "\\Profiles\\" + currentProfile + "\\teststats.lua";
    qDebug() << "INFO: teststats.lua path: " << statsPath;

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
        playerStats[i].name = playerNames.at(i);

    for(int i = 0; i < playerRaces.count(); i++ )
        playerStats[i].race = playerRaces.at(i);

    for(int i = 0; i < playerTeam.count(); i++ )
        playerStats[i].team = playerTeam.at(i);

    emit sendPlayersTestStats(playerStats);

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
    qDebug() << "INFO: Steam path:" << steam_path;

    return steam_path;
}

void SsController::parseSsSettings()
{

    QSettings* ssSettings = new QSettings(m_ssPath+"\\Local.ini", QSettings::Format::IniFormat);
    m_ssWindowed = ssSettings->value("global/screenwindowed", 0).toInt();
    currentProfile = ssSettings->value("global/playerprofile","profile").toString();


    qDebug() << "INFO: Current profile: " << currentProfile;
    qDebug() << "INFO: Windowed mode = " << m_ssWindowed;


    delete ssSettings;
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

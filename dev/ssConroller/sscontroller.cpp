#include "sscontroller.h"
#include <QTextCodec>
#include <QDebug>
#include <QGuiApplication>
#include <QFile>
#include <QSettings>

#define CHECK_SS_TIMER_INTERVAL 100  ///<Интервал таймера проверки запуска/не запускака, свернутости/не развернутости

SsController::SsController(QObject *parent) : QObject(parent)
{
    m_ssPath = getSsPathFromRegistry();
    qDebug() << "INFO: Worked with Soulstorm from: " << m_ssPath;
    m_gameInfoReader = new GameInfoReader(m_ssPath,this);

    QObject::connect(m_gameInfoReader, &GameInfoReader::gameInitialized, this, &SsController::gameInitialized, Qt::QueuedConnection);

    m_ssLounchControllTimer = new QTimer(this);
    m_ssLounchControllTimer->setInterval(CHECK_SS_TIMER_INTERVAL);
    QObject::connect(m_ssLounchControllTimer, &QTimer::timeout, this, &SsController::checkSS, Qt::QueuedConnection);
    //m_ssLounchControllTimer->start();
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
        if(!m_ssLounched)                                   ///<Если перед этим игра не была запущена
        {
            parseSsSettings();                                  ///<Считываем настройки соулсторма
            m_ssLounched = true;                                ///<Устанавливаем запущенное состояние
            emit ssLounched(m_ssLounched);                      ///<Отправляем сигнал о запуске игры
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
        if(m_ssLounched)                                    ///<Если игра была перед этим запущена
        {

            m_ssWindowed = false;                               ///<Устанавливаем не оконный режим
            m_ssMaximized = false;                              ///<Устанавливаем свернутое состояние
            emit ssMaximized(m_ssMaximized);                    ///<Отправляем сигнал о свернутости
            m_ssLounched = false;                               ///<Устанавливаем выключенное состояние
            emit ssLounched(m_ssLounched);                      ///<Отправляем сигнал о том что сс выключен
            m_gameInfoReader->ssWindowClosed();                 ///<Говорим инфоРидеру что окно сс закрыто
            m_soulstormHwnd=NULL;                               ///<Окно игры делаем  null
            m_ssLounchControllTimer->stop();                    ///<Останавливаем таймер контроля запуска
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
    m_ssLounchControllTimer->start();                   ///<Запускаем таймер который будет определять игра запущена/не запущена, максимизирована/не максимизирована
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

void SsController::parseSsSettings()
{
    QSettings* ssSettings = new QSettings(m_ssPath+"\\Local.ini", QSettings::Format::IniFormat);
    int windowed = ssSettings->value("global/screenwindowed", 0).toInt();

    if(m_ssWindowed != windowed && m_ssWindowed == true)
    {

    }
    m_ssWindowed = windowed;

    qDebug() << "INFO: Windowed mode = " << m_ssWindowed;


    delete ssSettings;
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

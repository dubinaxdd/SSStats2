#include "sscontroller.h"
#include <QTextCodec>
#include <QDebug>
#include <QGuiApplication>
#include <QFile>
#include <QSettings>

SsController::SsController(QObject *parent) : QObject(parent)
{
    m_delayBlockInputTimer = new QTimer(this);
    m_delayBlockInputTimer->setSingleShot(true);

    QObject::connect(m_delayBlockInputTimer, &QTimer::timeout, this, &SsController::delayBlockInputTimerTimout, Qt::QueuedConnection);

    m_ssPath = getSsPathFromRegistry();
    qDebug() << "INFO: Worked with Soulstorm from: " << m_ssPath;
    m_gameInfoReader = new GameInfoReader(m_ssPath,this);


    m_ssLounchControllTimer = new QTimer(this);
    m_ssLounchControllTimer->setInterval(1000);
    QObject::connect(m_ssLounchControllTimer, &QTimer::timeout, this, &SsController::checkSS, Qt::QueuedConnection);
    m_ssLounchControllTimer->start();
}

void SsController::blockInput(bool block)
{
    inputBlocked = block;

    if (inputBlocked)
    {
        EnableWindow(m_soulstormHwnd, !inputBlocked);
        return;
    }

    m_delayBlockInputTimer->start(300);
}

void SsController::delayBlockInputTimerTimout()
{
    if (m_soulstormHwnd)
        EnableWindow(m_soulstormHwnd, !inputBlocked);
}

void SsController::checkSS()
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString ss = codec->toUnicode("Dawn of War: Soulstorm");
    LPCWSTR lps = (LPCWSTR)ss.utf16();

    m_soulstormHwnd = FindWindowW(NULL, lps);

    if (m_soulstormHwnd)
    {
        if(!m_ssLounched)
        {
            m_ssLounched = true;
            emit ssLounched(m_ssLounched);
            qDebug() << "INFO: Soulstorm window opened";
        }
        else
        {
            if( IsIconic(m_soulstormHwnd))
            {
                if(m_ssMaximized)
                {
                    m_ssMaximized = false;
                    emit ssMaximized(m_ssMaximized);
                    qDebug() << "INFO: Soulstorm minimized";
                }
            }
            else
            {
                if(!m_ssMaximized)
                {
                    m_ssMaximized = true;
                    emit ssMaximized(m_ssMaximized);
                    qDebug() << "INFO: Soulstorm fullscreen";
                }
            }
        }
    }
    else
    {
        if(m_ssLounched)
        {
            m_ssLounched = false;
            emit ssLounched(m_ssLounched);
            qDebug() << "WARNING: Soulstorm window closed";
        }
        else
        {
           // qDebug() << "WARNING: Soulstorm window not accepted";
        }
    }


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

bool SsController::getInputBlocked() const
{
    return inputBlocked;
}

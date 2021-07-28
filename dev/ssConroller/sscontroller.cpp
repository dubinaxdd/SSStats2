#include "sscontroller.h"
#include <QTextCodec>
#include <QDebug>

SsController::SsController(QObject *parent) : QObject(parent)
{
    m_delayBlockInputTimer = new QTimer(this);
    m_delayBlockInputTimer->setSingleShot(true);

    QObject::connect(m_delayBlockInputTimer, &QTimer::timeout, this, &SsController::delayBlockInputTimerTimout, Qt::QueuedConnection);

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


  /*  if (m_soulstormHwnd == NULL)
    {

    }
    else
    {
        return;
    }*/

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
            //qDebug() << "INFO: Soulstorm window accepted";
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

bool SsController::getInputBlocked() const
{
    return inputBlocked;
}

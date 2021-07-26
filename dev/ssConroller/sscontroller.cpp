#include "sscontroller.h"
#include <QTextCodec>
#include <QDebug>

SsController::SsController(QObject *parent) : QObject(parent)
{
    m_delayBlockInputTimer = new QTimer(this);
    m_delayBlockInputTimer->setSingleShot(true);

    QObject::connect(m_delayBlockInputTimer, &QTimer::timeout, this, &SsController::delayBlockInputTimerTimout, Qt::QueuedConnection);

    QTextCodec *codecc = QTextCodec::codecForName("UTF-8");
    QString ssc = codecc->toUnicode("Dawn of War: Soulstorm");
    LPCWSTR lpss = (LPCWSTR)ssc.utf16();

    m_soulstormHwnd = FindWindowW(NULL, lpss);

    if (m_soulstormHwnd)
        qDebug() << "Soulstorm window accepted";
    else
        qDebug() << "Warning: Soulstorm window not accepted";


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

bool SsController::getInputBlocked() const
{
    return inputBlocked;
}

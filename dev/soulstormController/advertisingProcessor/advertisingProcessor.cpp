#include "advertisingProcessor.h"
#include "QDebug"

AdvertisingProcessor::AdvertisingProcessor(SettingsController *settingsController, QObject *parent)
    : QObject(parent)
    , m_settingsController(settingsController)
{
    m_sendingTimer = new QTimer(this);
    m_sendingTimer->setInterval(1000);
    connect(m_sendingTimer, &QTimer::timeout, this,  &AdvertisingProcessor::onSendingTimerTimeout, Qt::QueuedConnection);
}

void AdvertisingProcessor::onReplaySended()
{
    if (m_currentPlayer.statsAvailable && m_settingsController->getSettings()->enableAdvertising)
        m_sendingTimer->start();
}

void AdvertisingProcessor::receiveServerPlayerStats(ServerPlayerStats serverPlayerStats)
{
    if (serverPlayerStats.isCurrentPlayer)
        m_currentPlayer = serverPlayerStats;
}

void AdvertisingProcessor::onSendingTimerTimeout()
{
    m_currentRoom++;
    QString text = "Hi everyone, my Solo MMR is " + QString::number(m_currentPlayer.mmr1v1) + ", more information on dowstats.ru";

    emit sendAdvertisingMesssage(m_currentRoom, text);

    if (m_currentRoom == 30)
    {
        m_currentRoom = 0;
        m_sendingTimer->stop();
    }
}

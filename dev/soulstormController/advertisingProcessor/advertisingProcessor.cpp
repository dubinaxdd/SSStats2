#include "advertisingProcessor.h"
#include <QDebug>
#include <QNetworkReply>

AdvertisingProcessor::AdvertisingProcessor(SettingsController *settingsController, QObject *parent)
    : AbstractDowServerProcessor(parent)
    , m_settingsController(settingsController)
    , m_advertisingStartDelayTimer(new QTimer(this))
{
    m_advertisingStartDelayTimer->setInterval(5000);
    m_advertisingStartDelayTimer->setSingleShot(true);
    connect(m_advertisingStartDelayTimer, &QTimer::timeout, this, &AdvertisingProcessor::startAdvertising, Qt::QueuedConnection);
}

void AdvertisingProcessor::onReplaySended()
{
    m_advertisingStartDelayTimer->start();
}

void AdvertisingProcessor::receiveServerPlayerStats(ServerPlayerStats serverPlayerStats)
{
    if (serverPlayerStats.isCurrentPlayer)
        m_currentPlayer = serverPlayerStats;
}

void AdvertisingProcessor::joinChannel(int room, QString text)
{
    if (m_sessionID.isEmpty())
        return;

    QString urlString = "https://dow1ss-lobby.reliclink.com/game/chat/joinChannel?&chatroomID=" + QString::number(room)+ "&doRetry=1&sessionID=" + m_sessionID.toLocal8Bit();
    QNetworkRequest newRequest = createDowServerRequest(urlString);
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        if (checkReplyErrors("AdvertisingProcessor::joinChannel", reply))
        {
            m_currentRoom = 0;
            return;
        }

        sendAdvertisingMessage(room, text);
    });
}

void AdvertisingProcessor::sendAdvertisingMessage(int room, QString text)
{
    if (m_sessionID.isEmpty())
        return;

    QString urlString = "https://dow1ss-lobby.reliclink.com:443/game/chat/sendText?message=" + text + "&subject=&chatroomID=" + QString::number(room)+ "&sessionID=" + m_sessionID.toLocal8Bit();
    QNetworkRequest newRequest = createDowServerRequest(urlString);
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        if (checkReplyErrors("AdvertisingProcessor::sendAdvertisingMessage", reply))
        {
            m_currentRoom = 0;
            return;
        }

        leaveChannel(room, text);
    });
}

void AdvertisingProcessor::leaveChannel(int room, QString text)
{
    if (m_sessionID.isEmpty())
        return;

    QString urlString = "https://dow1ss-lobby.reliclink.com:443/game/chat/leaveChannel?&chatroomID=" + QString::number(room)+ "&sessionID=" + m_sessionID.toLocal8Bit();
    QNetworkRequest newRequest = createDowServerRequest(urlString);
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        if (checkReplyErrors("AdvertisingProcessor::leaveChannel", reply))
        {
            m_currentRoom = 0;
            return;
        }

        if (m_currentRoom == 30)
            m_currentRoom = 0;
        else
        {
            m_currentRoom++;
            joinChannel(m_currentRoom, text);
        }
    });
}

void AdvertisingProcessor::startAdvertising()
{
    if (!(m_currentPlayer.statsAvailable && m_settingsController->getSettings()->enableAdvertising))
        return;

    m_currentRoom = 1;
    QString text = "Hi everyone, my Solo MMR is " + QString::number(m_currentPlayer.mmr1v1) + ", more information on dowstats.ru";

    joinChannel(m_currentRoom, text);
}

#include "advertisingProcessor.h"
#include <QDebug>
#include <QNetworkReply>

AdvertisingProcessor::AdvertisingProcessor(SettingsController *settingsController, QObject *parent)
    : AbstractDowServerProcessor(parent)
    , m_settingsController(settingsController)
    , m_delayTimer(new QTimer(this))
    , m_requestTimer(new QTimer(this))
{
    m_delayTimer->setInterval(3000);
    m_delayTimer->setSingleShot(true);
    connect(m_delayTimer, &QTimer::timeout, this, &AdvertisingProcessor::startAdvertising, Qt::QueuedConnection);

    m_requestTimer->setInterval(200);
    m_requestTimer->setSingleShot(true);
    connect(m_requestTimer, &QTimer::timeout, this, &AdvertisingProcessor::sendNextRequest, Qt::QueuedConnection);
}

void AdvertisingProcessor::onReplaySended()
{
    m_delayTimer->start();
}

void AdvertisingProcessor::receiveServerPlayerStats(ServerPlayerStats serverPlayerStats)
{
    if (serverPlayerStats.isCurrentPlayer)
        m_currentPlayer = serverPlayerStats;
}

void AdvertisingProcessor::joinChannel()
{
    if (m_sessionID.isEmpty())
        return;

    QString urlString = "https://dow1ss-lobby.reliclink.com/game/chat/joinChannel?&chatroomID=" + QString::number(m_currentRoom)+ "&doRetry=1&sessionID=" + m_sessionID.toLocal8Bit();
    QNetworkRequest newRequest = createDowServerRequest(urlString);
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        if (checkReplyErrors("AdvertisingProcessor::joinChannel", reply))
        {
            m_currentRoom = 0;
            return;
        }

        QByteArray replyByteArray = reply->readAll();


        m_currentRequestType = SendAdvertisingMessage;
        m_requestTimer->start();

        if (replyByteArray.contains(m_currentPlayer.steamId.toLatin1()))
            m_playerRealRoom = m_currentRoom;




        reply->deleteLater();
    });
}

void AdvertisingProcessor::sendAdvertisingMessage()
{
    if (m_sessionID.isEmpty())
        return;

    QString urlString = "https://dow1ss-lobby.reliclink.com:443/game/chat/sendText?message=" + m_currentText + "&subject=&chatroomID=" + QString::number(m_currentRoom)+ "&sessionID=" + m_sessionID.toLocal8Bit();
    QNetworkRequest newRequest = createDowServerRequest(urlString);
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        if (checkReplyErrors("AdvertisingProcessor::sendAdvertisingMessage", reply))
        {
            m_currentRoom = 0;
            return;
        }

        if (m_playerRealRoom == m_currentRoom)
        {
            if (m_currentRoom == 30)
                m_currentRoom = 0;
            else
            {
                m_currentRoom++;
                m_currentRequestType = JoinChannel;
                m_requestTimer->start();
            }
        }

        m_currentRequestType = LeaveChannel;
        m_requestTimer->start();
        reply->deleteLater();
    });
}

void AdvertisingProcessor::leaveChannel()
{
    if (m_sessionID.isEmpty())
        return;

    QString urlString = "https://dow1ss-lobby.reliclink.com:443/game/chat/leaveChannel?&chatroomID=" + QString::number(m_currentRoom)+ "&sessionID=" + m_sessionID.toLocal8Bit();
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
            joinChannel();
        }

        reply->deleteLater();
    });
}

void AdvertisingProcessor::sendNextRequest()
{
    switch (m_currentRequestType){
        case JoinChannel: joinChannel(); break;
        case SendAdvertisingMessage: sendAdvertisingMessage(); break;
        case LeaveChannel: leaveChannel(); break;
    }
}

void AdvertisingProcessor::startAdvertising()
{
    if (!(m_currentPlayer.statsAvailable && m_settingsController->getSettings()->enableAdvertising))
        return;

    m_playerRealRoom = 0;
    m_currentRoom = 1;
    m_currentText = "Hi everyone, I use DoW Stats, my Solo MMR: " + QString::number(m_currentPlayer.mmr1v1) + ", games played: " + QString::number(m_currentPlayer.gamesCount) + ", see new game features, replays and more information on dowstats.ru";

    joinChannel();
}

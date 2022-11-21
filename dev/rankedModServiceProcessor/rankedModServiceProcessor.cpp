#include "rankedModServiceProcessor.h"
#include <logger.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <defines.h>

RankedModServiceProcessor::RankedModServiceProcessor(QObject *parent) : QObject(parent)
{
    m_networkManager = new QNetworkAccessManager(this);

    m_pingTimer = new QTimer(this);
    m_pingTimer->setInterval(1000);

    m_rankedStateTimer = new QTimer(this);
    m_rankedStateTimer->setInterval(1000);

    QObject::connect(m_pingTimer, &QTimer::timeout, this, &RankedModServiceProcessor::pingTimerTimeout, Qt::QueuedConnection);
    QObject::connect(m_rankedStateTimer, &QTimer::timeout, this, &RankedModServiceProcessor::rankedStateTimerTimeout, Qt::QueuedConnection);

    m_pingTimer->start();
    m_rankedStateTimer->start();
}

void RankedModServiceProcessor::setCurrentPlayerSteamIdSlot(QString currentPlayerSteamId)
{
    m_currentPlayerSteamId = currentPlayerSteamId;
}

void RankedModServiceProcessor::receivePlayresInfoFromDowServer(QList<PlayerInfoFromDowServer> playersInfoInfoFromDowServer, int playersCount)
{
    m_playersInfoInfoFromDowServer = playersInfoInfoFromDowServer;
}

void RankedModServiceProcessor::sendRankedMode(bool rankedMode)
{
    if (m_currentPlayerSteamId.isEmpty())
        return;

    QString ranked;

    if (rankedMode)
        ranked = "true";
    else
        ranked = "false";

    QString urlString = "http://crosspick.ru:8081/setRankedMode?sid=" + m_currentPlayerSteamId + "&rankedMode=" +ranked ;

    QNetworkRequest newRequest = QNetworkRequest(QUrl(urlString));
    newRequest.setRawHeader("Token", QString::fromStdString(RANKED_SERVICE_TOKEN).toLatin1());

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        reply->deleteLater();
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
        reply->deleteLater();
    });

}

void RankedModServiceProcessor::pingTimerTimeout()
{
    if (m_currentPlayerSteamId.isEmpty())
        return;

    QString urlString = "http://crosspick.ru:8081/pingRequest?sid=" + m_currentPlayerSteamId;

    QNetworkRequest newRequest = QNetworkRequest(QUrl(urlString));
    newRequest.setRawHeader("Token", QString::fromStdString(RANKED_SERVICE_TOKEN).toLatin1());

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receivePingRecponce(reply);
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
        reply->deleteLater();
    });

}

void RankedModServiceProcessor::rankedStateTimerTimeout()
{
    if (m_playersInfoInfoFromDowServer.isEmpty())
    {
        if(m_currentPlayerSteamId.isEmpty())
            return;

        PlayerInfoFromDowServer newPlayer;
        newPlayer.steamId = m_currentPlayerSteamId;

        m_playersInfoInfoFromDowServer.append(newPlayer);
    }

    QString sidsListString = "";

    QVector<PlyersRankedState> plyersRankedState;

    for (int i = 0; i < m_playersInfoInfoFromDowServer.count(); i++)
    {
        sidsListString.append(m_playersInfoInfoFromDowServer.at(i).steamId);

        if (i != m_playersInfoInfoFromDowServer.count() - 1)
            sidsListString.append(',');

        PlyersRankedState newPlyersRankedState;
        newPlyersRankedState.steamId = m_playersInfoInfoFromDowServer.at(i).steamId;
        newPlyersRankedState.name = m_playersInfoInfoFromDowServer.at(i).name;

        plyersRankedState.append(newPlyersRankedState);
    }

    QString urlString = "http://crosspick.ru:8081/getRankedMode?sid=" + sidsListString;

    QNetworkRequest newRequest = QNetworkRequest(QUrl(urlString));
    newRequest.setRawHeader("Token", QString::fromStdString(RANKED_SERVICE_TOKEN).toLatin1());

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveRankedState(reply, std::move(plyersRankedState));
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
        reply->deleteLater();
    });

}

void RankedModServiceProcessor::receiveRankedState(QNetworkReply *reply, QVector<PlyersRankedState> plyersRankedState)
{
    if (checkReplyErrors(reply))
        return;

    QByteArray replyByteArray = reply->readAll();

    reply->deleteLater();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyByteArray);

    if (!jsonDoc.isArray())
    {
        qWarning(logWarning()) << "Bad reply from ranked microservice:" << QString::fromLatin1(replyByteArray);
        return;
    }

    QJsonArray jsonArray = jsonDoc.array();

    QVector<PlyersRankedState> newPlyersRankedStateArray;


    for(int i = 0; i < jsonArray.count(); i++)
    {

        for (int j = 0; j < plyersRankedState.count(); j++)
        {
            if (plyersRankedState.at(j).steamId == jsonArray.at(i)["SID"].toString())
            {
                PlyersRankedState newPlyersRankedState;

                newPlyersRankedState.name = plyersRankedState.at(j).name;
                newPlyersRankedState.steamId = plyersRankedState.at(j).steamId;
                newPlyersRankedState.isRanked = jsonArray.at(i)["Ranked"].toBool();
                newPlyersRankedState.isOnline = jsonArray.at(i)["Online"].toBool();

                newPlyersRankedStateArray.append(newPlyersRankedState);
            }
        }
    }

    emit sendPlyersRankedState(newPlyersRankedStateArray);
}

void RankedModServiceProcessor::receivePingRecponce(QNetworkReply *reply)
{
    if (checkReplyErrors(reply))
        return;

    QByteArray replyByteArray = reply->readAll();

    reply->deleteLater();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyByteArray);

    if (!jsonDoc.isArray())
    {
        qWarning(logWarning()) << "Bad reply from ranked microservice:" << QString::fromLatin1(replyByteArray);
        return;
    }

    QJsonArray jsonArray = jsonDoc.array();

    int onlineCount = jsonArray.at(0)["onlineCount"].toInt();

    emit sendOnlineCount(onlineCount);
}

bool RankedModServiceProcessor::checkReplyErrors(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "Connection error:" << reply->errorString();
        reply->deleteLater();
        return true;
    }

    return false;
}

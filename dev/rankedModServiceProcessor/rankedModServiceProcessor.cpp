#include "rankedModServiceProcessor.h"
#include <logger.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

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

    if(m_playersSteamIds.isEmpty())
        m_playersSteamIds.append(m_currentPlayerSteamId);
}

void RankedModServiceProcessor::receivePlayresInfoFromDowServer(QList<PlayerInfoFromDowServer> playersInfoInfoFromDowServer, int playersCount)
{
    m_playersSteamIds.clear();

    for (int i = 0; i < playersInfoInfoFromDowServer.count(); i++)
        m_playersSteamIds.append(playersInfoInfoFromDowServer.at(i).steamId);

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

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        reply->deleteLater();
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
        reply->deleteLater();
    });

}

void RankedModServiceProcessor::rankedStateTimerTimeout()
{
    if (m_playersSteamIds.isEmpty())
        return;

    QString sidsListString = "";

    QVector<PlyersRankedState> plyersRankedState;

    for (int i = 0; i < m_playersSteamIds.count(); i++)
    {
        sidsListString.append(m_playersSteamIds.at(i));

        if (i != m_playersSteamIds.count() - 1)
            sidsListString.append(',');

        PlyersRankedState newPlyersRankedState;
        newPlyersRankedState.steamId = m_playersSteamIds.at(i);

        plyersRankedState.append(newPlyersRankedState);
    }

    QString urlString = "http://crosspick.ru:8081/getRankedMode?sid=" + sidsListString;


    QNetworkRequest newRequest = QNetworkRequest(QUrl(urlString));

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

        reply->deleteLater();
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

                newPlyersRankedState.steamId = plyersRankedState.at(j).steamId;
                newPlyersRankedState.isRanked = jsonArray.at(i)["Ranked"].toBool();
                newPlyersRankedState.isOnline = jsonArray.at(i)["Online"].toBool();

                newPlyersRankedStateArray.append(newPlyersRankedState);
            }
        }
    }

    emit sendPlyersRankedState(newPlyersRankedStateArray);
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

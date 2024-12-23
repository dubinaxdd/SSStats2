#include "rankedModServiceProcessor.h"
#include <logger.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <defines.h>

RankedModServiceProcessor::RankedModServiceProcessor(SettingsController *settingsController, QObject *parent)
    : QObject(parent)
    , m_settingsController(settingsController)
    , m_networkManager (new QNetworkAccessManager(this))
{
    m_pingTimer = new QTimer(this);
    m_pingTimer->setInterval(1000);

    m_rankedStateTimer = new QTimer(this);
    m_rankedStateTimer->setInterval(1000);

    m_uniquePlayersOnlineStatisticTimer = new QTimer(this);
    m_uniquePlayersOnlineStatisticTimer->start(60000);

    QObject::connect(m_pingTimer, &QTimer::timeout, this, &RankedModServiceProcessor::pingTimerTimeout, Qt::QueuedConnection);
    QObject::connect(m_rankedStateTimer, &QTimer::timeout, this, &RankedModServiceProcessor::rankedStateTimerTimeout, Qt::QueuedConnection);
    QObject::connect(m_uniquePlayersOnlineStatisticTimer, &QTimer::timeout, this, &RankedModServiceProcessor::requestUniquePlayersOnlineStatistic, Qt::QueuedConnection);
    QObject::connect(m_settingsController, &SettingsController::settingsLoaded, this, &RankedModServiceProcessor::onSettingsLoaded, Qt::QueuedConnection);

    requestUniquePlayersOnlineStatistic();
}

void RankedModServiceProcessor::setCurrentPlayerSteamIdSlot(QString currentPlayerSteamId)
{
    m_currentPlayerSteamId = currentPlayerSteamId;
    sendRankedMode(true);
}

void RankedModServiceProcessor::receivePlayresInfoFromDowServer(QList<PlayerInfoFromDowServer> playersInfoInfoFromDowServer)
{
    m_playersInfoInfoFromDowServer = playersInfoInfoFromDowServer;

    qInfo(logInfo()) << "RankedModServiceProcessor::receivePlayresInfoFromDowServer" << "players info received";
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
}

void RankedModServiceProcessor::pingTimerTimeout()
{
    sendPingRequest();
}

void RankedModServiceProcessor::rankedStateTimerTimeout()
{
    QString sidsListString = "";

    QVector<PlyersRankedState> plyersRankedState;

    if (m_playersInfoInfoFromDowServer.isEmpty())
    {
        if(m_currentPlayerSteamId.isEmpty())
            return;

        sidsListString.append(m_currentPlayerSteamId);

        PlyersRankedState newPlyersRankedState;
        newPlyersRankedState.steamId = m_currentPlayerSteamId;
        plyersRankedState.append(newPlyersRankedState);
    }

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
}

void RankedModServiceProcessor::receiveRankedState(QNetworkReply *reply, QVector<PlyersRankedState> plyersRankedState)
{
    if (checkReplyErrors("RankedModServiceProcessor::receiveRankedState", reply))
        return;

    QByteArray replyByteArray = reply->readAll();

    reply->deleteLater();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyByteArray);

    if (!jsonDoc.isArray())
    {
        qWarning(logWarning()) << "RankedModServiceProcessor::receiveRankedState:" << "Bad reply from ranked microservice:" << QString::fromLatin1(replyByteArray);
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
    if (checkReplyErrors("RankedModServiceProcessor::receivePingRecponce", reply))
        return;

    QByteArray replyByteArray = reply->readAll();

    reply->deleteLater();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyByteArray);

    if (!jsonDoc.isArray())
    {
        qWarning(logWarning()) << "RankedModServiceProcessor::receivePingRecponce:" << "Bad reply from ranked microservice:" << QString::fromLatin1(replyByteArray);
        return;
    }

    QJsonArray jsonArray = jsonDoc.array();

    int onlineCount = jsonArray.at(0)["onlineCount"].toInt();


    if (!jsonArray.at(0)["modArray"].isArray())
    {
        qWarning(logWarning()) << "RankedModServiceProcessor::receivePingRecponce: invalid mod array." << QString::fromStdString(replyByteArray.toStdString());
        return;
    }

    QJsonArray modArray = jsonArray.at(0)["modArray"].toArray();

    QMap<QString, int> modsMap;

    for (std::size_t i = 0; i < modArray.count(); i++)
    {
        if (!modArray.at(i).isObject())
        {
            qWarning(logWarning()) << "RankedModServiceProcessor::receivePingRecponce: invalid mod object." << QString::fromStdString(replyByteArray.toStdString());
            return;
        }

        QJsonObject modObject = modArray.at(i).toObject();

        QString modName = modObject.value("ModName").toString();
        int onlineCount = modObject.value("OnlineCount").toInt();

        modsMap.insert(modName, onlineCount);
    }

    emit sendOnlineCount(onlineCount);
    emit sendModsOnlineCountMap(modsMap);
}

void RankedModServiceProcessor::receiveUniquePlayersOnlineStatistic(QNetworkReply *reply)
{
    if (checkReplyErrors("RankedModServiceProcessor::receivePingRecponce", reply))
        return;

    QByteArray replyByteArray = reply->readAll();

    reply->deleteLater();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyByteArray);

    if (!jsonDoc.isObject())
    {
        qWarning(logWarning()) << "RankedModServiceProcessor::receiveUniquePlayersOnlineStatistic:" << "Bad reply from ranked microservice:" << QString::fromLatin1(replyByteArray);
        return;
    }

    QJsonObject jsonObject = jsonDoc.object();

    UniqueOnlineStatistic uniqueOnlineStatistic;

    uniqueOnlineStatistic.day = jsonObject.value("day").toInt();
    uniqueOnlineStatistic.month = jsonObject.value("month").toInt();
    uniqueOnlineStatistic.year = jsonObject.value("year").toInt();
    uniqueOnlineStatistic.total = jsonObject.value("total").toInt();

    emit sendUniquePlayersOnlineStatistic(uniqueOnlineStatistic);
}

void RankedModServiceProcessor::receiveCurrentMod(QString modName)
{
    m_currentMod = modName;

    if (m_currentMod.contains("dowstats_balance_mod"))
        m_currentMod = "dowstats_balance_mod";
}

void RankedModServiceProcessor::onSettingsLoaded()
{
    qInfo(logInfo()) << "RankedModServiceProcessor::onSettingsLoaded()" << "load started";

    m_currentMod = m_settingsController->getSettings()->currentMod;

    if (m_currentMod.contains("dowstats_balance_mod"))
        m_currentMod = "dowstats_balance_mod";

    m_pingTimer->start();
    m_rankedStateTimer->start();

    qInfo(logInfo()) << "RankedModServiceProcessor::onSettingsLoaded()" << "load finished";
}

bool RankedModServiceProcessor::checkReplyErrors(QString funcName, QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << funcName + ":" << "Connection error:" << reply->errorString();
        reply->deleteLater();
        return true;
    }

    return false;
}

void RankedModServiceProcessor::sendPingRequest()
{
    //if (m_currentPlayerSteamId.isEmpty())
    //    return;

    QString urlString = "http://crosspick.ru:8081/pingRequest?sid=" + m_currentPlayerSteamId + "&gameMod=" + m_currentMod;

    QNetworkRequest newRequest = QNetworkRequest(QUrl(urlString));
    newRequest.setRawHeader("Token", QString::fromStdString(RANKED_SERVICE_TOKEN).toLatin1());

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receivePingRecponce(reply);
    });
}

void RankedModServiceProcessor::requestUniquePlayersOnlineStatistic()
{
    QString urlString = "http://crosspick.ru:8081/uniq";

    QNetworkRequest newRequest = QNetworkRequest(QUrl(urlString));
    newRequest.setRawHeader("Token", QString::fromStdString(RANKED_SERVICE_TOKEN).toLatin1());

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveUniquePlayersOnlineStatistic(reply);
    });
}

#include "DowServerProcessor.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QSslConfiguration>
#include <defines.h>

#define QUEUE_TIMER_INTERVAL 1000

DowServerProcessor::DowServerProcessor(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
    m_queueTimer = new QTimer(this);
    m_queueTimer->setInterval(QUEUE_TIMER_INTERVAL);

    QObject::connect(m_queueTimer, &QTimer::timeout, this, &DowServerProcessor::checkQueue, Qt::QueuedConnection);
    m_queueTimer->start();
}

bool DowServerProcessor::checkReplyErrors(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "Connection error:" << reply->errorString();
        reply->deleteLater();
        return true;
    }

    return false;
}

QNetworkRequest DowServerProcessor::createDowServerRequest(QString url)
{
    QNetworkRequest newRequest;

    newRequest.setUrl(QUrl(url));
    newRequest.setRawHeader("Cookie", "reliclink=" + QString(RELICLINK).toLocal8Bit() + "; AWSELB=" + QString(AWSELB).toLocal8Bit());
    newRequest.setRawHeader("Host", "dow1ss-lobby.reliclink.com");
    newRequest.setRawHeader("Content-Content-Type","application/x-www-form-urlencoded; charset=UTF-8");

    QSslConfiguration sslConf;
    sslConf.setPeerVerifyMode(QSslSocket::VerifyNone);
    newRequest.setSslConfiguration(sslConf);

    return std::move(newRequest);
}

QVector<QString> DowServerProcessor::getPlayersInCurrentRoom(QVector<PartyData> partyDataArray)
{
    if (m_profileID.isEmpty())
        return QVector<QString>();

    QVector<QString> profileIDs;

    for (int i = 0; i < partyDataArray.count(); i++)
    {
        QVector<PlayerData> playersDataArray = partyDataArray.at(i).profilesIds;

        for(int j = 0; j < playersDataArray.count(); j++)
        {
            if (playersDataArray.at(j).profileID == m_profileID)
            {
                for (int k = 0; k < playersDataArray.count(); k++)
                    profileIDs.append(playersDataArray.at(k).profileID);

                return profileIDs;
            }
        }
    }

    return QVector<QString>();
}

void DowServerProcessor::rquestChannellData(int id)
{
    if (m_sessionID.isEmpty())
        return;

    QString urlString = "https://dow1ss-lobby.reliclink.com/game/chat/joinChannel?chatroomID=1&doRetry=1&sessionID=" + m_sessionID.toLocal8Bit();
    QNetworkRequest newRequest = createDowServerRequest(urlString);
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveChannellData(reply, id);
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
        reply->deleteLater();
    });
}

void DowServerProcessor::requestProfileID(QString steamID)
{
    if (m_sessionID.isEmpty())
        return;

    QString urlString = "https://dow1ss-lobby.reliclink.com/game/account/getProfileID?profile_names=%5b%22%2fsteam%2f" + steamID.toLocal8Bit() + "%22%5d&sessionID=" + m_sessionID.toLocal8Bit();
    QNetworkRequest newRequest = createDowServerRequest(urlString);
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveProfileID(reply, steamID);
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
        reply->deleteLater();
    });
}

void DowServerProcessor::requestFindAdvertisements()
{
    if (m_profileID.isEmpty() || m_statGroupId.isEmpty() || m_modName.isEmpty() || m_modVersion.isEmpty() || m_sessionID.isEmpty())
        return;

    QString urlString = "https://dow1ss-lobby.reliclink.com/game/advertisement/findAdvertisements?profile_ids=%5b"
                        + m_profileID + "%5d&statGroup_ids=%5b"
                        + m_statGroupId + "%5d&race_ids=%5b0%5d&matchType_id=0&modName="
                        + m_modName + "&modVersion="
                        + m_modVersion + "&modDLLFile=WXPMod.dll&modDLLChecksum=1077236955&dataChecksum=206085050&appBinaryChecksum=1817556062&cheatsEnabled=0&sessionID="
                        + m_sessionID.toLocal8Bit();

    QNetworkRequest newRequest = createDowServerRequest(urlString);
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveFindAdvertisements(reply);
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
        reply->deleteLater();
    });
}

void DowServerProcessor::requestPlayersSids(QVector<QString> profileIDs)
{
    if (m_sessionID.isEmpty() || profileIDs.count() < 1)
        return;

    QString profilesIDsString;

    profilesIDsString += "%5b";

    for (int i = 0; i < profileIDs.count(); i++)
    {
        if (i != 0)
            profilesIDsString += "%2c";

        profilesIDsString += profileIDs.at(i);
    }

    profilesIDsString += "%5d";

    QString urlString = "https://dow1ss-lobby.reliclink.com/game/account/getProfileName?profile_ids=" + profilesIDsString.toLocal8Bit() + "&sessionID=" + m_sessionID.toLocal8Bit();
    QNetworkRequest newRequest = createDowServerRequest(urlString);
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receivePlayersSids(reply);
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
        reply->deleteLater();
    });
}

void DowServerProcessor::setSessionID(QString sessionID)
{
    m_sessionID = sessionID;

    qInfo(logInfo()) << "sessionID =" << m_sessionID;

    if(!m_steamID.isEmpty() && !m_sessionID.isEmpty())
    {
        addQuery(QueryType::ProfileID);
        //requestProfileID(m_steamID);
    }


}

void DowServerProcessor::setCurrentPlayerSteamID(QString steamID)
{
    m_steamID = steamID;

    if(!m_steamID.isEmpty() && !m_sessionID.isEmpty())
    {
        addQuery(QueryType::ProfileID);
        //requestProfileID(m_steamID);
    }

}

void DowServerProcessor::requestPartysData()
{
    if (m_profileID.isEmpty() || m_statGroupId.isEmpty() || m_modName.isEmpty() || m_modVersion.isEmpty() || m_sessionID.isEmpty())
        m_needUpdateLatter = true;

    addQuery(QueryType::FindAdvertisements);
    //requestFindAdvertisements();
}

void DowServerProcessor::receiveChannellData(QNetworkReply *reply, int id)
{
    if (checkReplyErrors(reply))
        return;

    QByteArray replyByteArray = reply->readAll();
    reply->deleteLater();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyByteArray);

    if(!jsonDoc.isArray())
        return;

    qDebug() << jsonDoc.array();
}

void DowServerProcessor::receiveProfileID(QNetworkReply *reply, QString steamID)
{
    if (checkReplyErrors(reply))
        return;

    QByteArray replyByteArray = reply->readAll();
    reply->deleteLater();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyByteArray);

    if (jsonDoc.isArray())
    {
        QJsonArray messageArray = jsonDoc.array();

        if (messageArray.count() >= 2 && messageArray.at(1).isArray())
        {
            QJsonArray palyersArray = messageArray.at(1).toArray();

            if (palyersArray.count() >= 1 && palyersArray.at(0).isArray())
            {
                QJsonArray palyerDataArray = palyersArray.at(0).toArray();
                if (palyerDataArray.count() == 8)
                {
                    m_profileID = QString::number(palyerDataArray.at(1).toInt());
                    m_statGroupId = QString::number(palyerDataArray.at(6).toInt());
                    qInfo(logInfo()) << "profileID=" << m_profileID;
                    qInfo(logInfo()) << "statsGroupID=" << m_statGroupId;

                    if (m_needUpdateLatter)
                    {
                        requestPartysData();
                        m_needUpdateLatter = false;
                    }
                }
            }
        }
    }
}

void DowServerProcessor::receiveFindAdvertisements(QNetworkReply *reply)
{
    if (checkReplyErrors(reply))
        return;

    QByteArray replyByteArray = reply->readAll();
    reply->deleteLater();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyByteArray);

    if (jsonDoc.isArray())
    {
        QJsonArray messageArray = jsonDoc.array();



        if (messageArray.count() >= 2 && messageArray.at(1).isArray())
        {
            QJsonArray partysArrayJson = messageArray.at(1).toArray();

            QVector<PartyData> partysArray;

            for (int i = 0; i < partysArrayJson.count(); i++)
            {

                if (!partysArrayJson.at(i).isArray())
                    continue;

                QJsonArray partyDataArray = partysArrayJson.at(i).toArray();

                if (partyDataArray.count() != 18 && !partyDataArray.at(17).isArray())
                    continue;

                PartyData newPartyData;

                newPartyData.partyId = QString::number(partyDataArray.at(0).toInt());
                newPartyData.hostPlayerId = QString::number(partyDataArray.at(2).toInt());
                newPartyData.partyMapName = partyDataArray.at(4).toString();
                newPartyData.partyName = partyDataArray.at(6).toString();

                QJsonArray playersJsonArray = partyDataArray.at(17).toArray();

                for (int j = 0; j < playersJsonArray.count(); j++)
                {
                    if (!playersJsonArray.at(j).isArray())
                        continue;

                    QJsonArray playerDataJson = playersJsonArray.at(j).toArray();
                    PlayerData newPlayerData;

                    newPlayerData.partyID = QString::number(playerDataJson.at(0).toInt());
                    newPlayerData.profileID = QString::number(playerDataJson.at(1).toInt());

                    newPartyData.profilesIds.append(newPlayerData);
                }
                partysArray.append(newPartyData);
            }

            emit sendPartysArray(partysArray);

            m_profileIdsForQueue = getPlayersInCurrentRoom(partysArray);
            addQuery(QueryType::PlayersSids);

            //requestPlayersSids();
        }
    }
}

void DowServerProcessor::receivePlayersSids(QNetworkReply *reply)
{
    if (checkReplyErrors(reply))
        return;

    QByteArray replyByteArray = reply->readAll();
    reply->deleteLater();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyByteArray);

    qDebug() << jsonDoc;
}

void DowServerProcessor::checkQueue()
{
    if (m_requestsQueue.isEmpty())
        return;

    QueryType needType = m_requestsQueue.first();

    m_requestsQueue.remove(0);


    switch (needType) {
        case QueryType::FindAdvertisements :{
            requestFindAdvertisements();
            break;
        }

        case QueryType::PlayersSids :{
            requestPlayersSids(m_profileIdsForQueue);
            break;
        }

        case QueryType::ProfileID :{
            requestProfileID(m_steamID);
            break;
        }

        case QueryType::ChannelData :{
            rquestChannellData(1);
            break;
        }
    }
}

void DowServerProcessor::addQuery(QueryType type)
{
    if (m_requestsQueue.contains(type) && type != QueryType::PlayersSids)
        return;

    m_requestsQueue.append(type);
}

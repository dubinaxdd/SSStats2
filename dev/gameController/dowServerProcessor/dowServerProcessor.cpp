#include "dowServerProcessor.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QSslConfiguration>
#include <defines.h>

#define QUEUE_TIMER_INTERVAL 500
#define ASD_TIMER_INTERVAL 3000

DowServerProcessor::DowServerProcessor(QObject *parent)
    : AbstractDowServerProcessor(parent)
{
    m_queueTimer = new QTimer(this);
    m_queueTimer->setInterval(QUEUE_TIMER_INTERVAL);

    m_requestDataAftrePlayerDisconectTimer = new QTimer(this);
    m_requestDataAftrePlayerDisconectTimer->setInterval(ASD_TIMER_INTERVAL);
    m_requestDataAftrePlayerDisconectTimer->setSingleShot(true);

    QObject::connect(m_queueTimer, &QTimer::timeout, this, &DowServerProcessor::checkQueue, Qt::QueuedConnection);
    QObject::connect(m_requestDataAftrePlayerDisconectTimer, &QTimer::timeout, this, &DowServerProcessor::playerDiscoonectTimerTimeout, Qt::QueuedConnection);
}

QVector<PlayerData> DowServerProcessor::getPlayersInCurrentRoom(QVector<PartyData> partyDataArray)
{
    if (m_profileID.isEmpty())
        return QVector<PlayerData>();

    QVector<PlayerData> playersData;

    for (int i = 0; i < partyDataArray.count(); i++)
    {
        QVector<PlayerData> playersDataArray = partyDataArray.at(i).profilesIds;

        for(int j = 0; j < playersDataArray.count(); j++)
        {
            if (playersDataArray.at(j).profileID == m_profileID)
            {
                for (int k = 0; k < playersDataArray.count(); k++)
                    playersData.append(playersDataArray.at(k));

                return playersData;
            }
        }
    }

    return QVector<PlayerData>();
}

void DowServerProcessor::rquestChannellData(int id)
{
    if (m_parametres.sesionId.isEmpty())
        return;

    QString urlString;

    if (m_gameType == GameType::GameTypeEnum::SoulstormSteam)
        urlString = "https://dow1ss-lobby.reliclink.com/game/chat/joinChannel?chatroomID=1&doRetry=1&sessionID=" + m_parametres.sesionId.toLocal8Bit();
    else if (m_gameType == GameType::GameTypeEnum::DefinitiveEdition)
        urlString = "https://dow-api.reliclink.com:443/game/chat/joinChannel?chatroomID=1&doRetry=1&sessionID=" + m_parametres.sesionId.toLocal8Bit();
    else
        return;


    QNetworkRequest newRequest = createDowServerRequest(urlString);
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveChannellData(reply, id);
    });
}

void DowServerProcessor::requestProfileID(QString steamID)
{
    if (m_parametres.sesionId.isEmpty())
        return;

    QString urlString;

    if (m_gameType == GameType::GameTypeEnum::SoulstormSteam)
        urlString = "https://dow1ss-lobby.reliclink.com/game/account/getProfileID?profile_names=%5b%22%2fsteam%2f" + steamID.toLocal8Bit() + "%22%5d&sessionID=" + m_parametres.sesionId.toLocal8Bit();
    else if (m_gameType == GameType::GameTypeEnum::DefinitiveEdition)
        urlString = "https://dow-api.reliclink.com:443/game/account/getProfileID?profile_names=%5b%22%2fsteam%2f" + steamID.toLocal8Bit() + "%22%5d&sessionID=" + m_parametres.sesionId.toLocal8Bit();
    else
        return;

    QNetworkRequest newRequest = createDowServerRequest(urlString);
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveProfileID(reply, steamID);
    });
}

void DowServerProcessor::requestFindAdvertisements()
{
    if (m_profileID.isEmpty() || m_statGroupId.isEmpty() || m_modName.isEmpty() || m_modVersion.isEmpty() || m_parametres.sesionId.isEmpty())
    {
        m_neeedRequestAdvertisements = true;
        return;
    }

    QString urlString;

    if (m_gameType == GameType::GameTypeEnum::SoulstormSteam)
    {
        urlString = "https://dow1ss-lobby.reliclink.com/game/advertisement/findAdvertisements?profile_ids=%5b"
                    + m_profileID + "%5d&statGroup_ids=%5b"
                    + m_statGroupId + "%5d&race_ids=%5b0%5d&matchType_id=0&modName="
                    + m_modName + "&modVersion="
                    + m_modVersion + "&modDLLFile=WXPMod.dll&modDLLChecksum=1077236955&dataChecksum=206085050&appBinaryChecksum=1817556062&cheatsEnabled=0&sessionID="
                    + m_parametres.sesionId.toLocal8Bit();
    }
    else if (m_gameType == GameType::GameTypeEnum::DefinitiveEdition)
    {
        urlString = "https://dow-api.reliclink.com:443/game/advertisement/findAdvertisements?"
                    "appBinaryChecksum=" + m_parametres.appBinaryChecksum.toLocal8Bit() +
                    "&dataChecksum=" + m_parametres.dataChecksum.toLocal8Bit() +
                    "&matchType_id=0&modDLLChecksum=" + m_parametres.modDLLChecksum.toLocal8Bit() +
                    "&modDLLFile=DXP3Mod.dll"
                    "&modName=Soulstorm"
                    "&modVersion=" + m_modVersion +
                    "&sessionID=" + m_parametres.sesionId.toLocal8Bit();
    }
    else
        return;

    QNetworkRequest newRequest = createDowServerRequest(urlString);
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveFindAdvertisements(reply);
    });
}

void DowServerProcessor::requestPlayersSids(QVector<PlayerData> profilesData)
{
    if (m_parametres.sesionId.isEmpty() || profilesData.count() < 1)
        return;

    QString profilesIDsString;

    profilesIDsString += "%5b";

    for (int i = 0; i < profilesData.count(); i++)
    {
        if (i != 0)
            profilesIDsString += "%2c";

        profilesIDsString += profilesData.at(i).profileID;
    }

    profilesIDsString += "%5d";

    QString urlString;

    if (m_gameType == GameType::GameTypeEnum::SoulstormSteam)
        urlString = "https://dow1ss-lobby.reliclink.com/game/account/getProfileName?profile_ids=" + profilesIDsString.toLocal8Bit() + "&sessionID=" + m_parametres.sesionId.toLocal8Bit();
    else if (m_gameType == GameType::GameTypeEnum::DefinitiveEdition)
        urlString = "https://dow-api.reliclink.com:443/game/account/getProfileName?profile_ids=" + profilesIDsString.toLocal8Bit() + "&sessionID=" + m_parametres.sesionId.toLocal8Bit();
    else
        return;

    QNetworkRequest newRequest = createDowServerRequest(urlString);
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receivePlayersSids(reply, profilesData);
    });
}

void DowServerProcessor::onPlayerDisconnected()
{
    m_requestDataAftrePlayerDisconectTimer->start();
}

void DowServerProcessor::onAutomatchPlayersListChanged(QStringList playersList)
{
    qInfo(logInfo()) << "DowServerProcessor::onAutomatchPlayersListChanged: - " << playersList;

    QVector<PlayerData> profilesData;

    for (auto& item : playersList)
    {
        PlayerData newPlayerData;
        newPlayerData.profileID = item;

        profilesData.append(newPlayerData);
    }

    m_profileIdsForQueue = profilesData;
    addQuery(QueryType::PlayersSids);

}

void DowServerProcessor::setDowServerRequestParametres(DowServerRequestParametres parametres)
{
    AbstractDowServerProcessor::setDeowServerRequestParametres(parametres);

    qInfo(logInfo()) << "sessionID =" << parametres.sesionId << "appBinaryChecksum:" << parametres.appBinaryChecksum
                     << "dataChecksum:" << parametres.dataChecksum << "modDLLChecksum:" << parametres.modDLLChecksum;

    if(!m_steamID.isEmpty() && !m_parametres.sesionId.isEmpty())
    {
        addQuery(QueryType::ProfileID);

        if (m_neeedRequestAdvertisements)
        {
            addQuery(QueryType::FindAdvertisements);
            m_neeedRequestAdvertisements = false;
        }
    }
}

void DowServerProcessor::setCurrentPlayerSteamID(QString steamID)
{
    m_steamID = steamID;

    if(!m_steamID.isEmpty() && !m_parametres.sesionId.isEmpty())
        addQuery(QueryType::ProfileID);
}

void DowServerProcessor::setCurrentModVersion(QString modVersion)
{
    m_modVersion = modVersion;
}

void DowServerProcessor::requestPartysData()
{
    if (m_profileID.isEmpty() || m_statGroupId.isEmpty() || m_modName.isEmpty() || m_modVersion.isEmpty() || m_parametres.sesionId.isEmpty())
        m_needUpdateLatter = true;

    addQuery(QueryType::FindAdvertisements);
}

void DowServerProcessor::receiveChannellData(QNetworkReply *reply, int id)
{
    if (checkReplyErrors("DowServerProcessor::receiveChannellData", reply))
        return;

    QByteArray replyByteArray = reply->readAll();
    reply->deleteLater();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyByteArray);

    if(!jsonDoc.isArray())
        return;
}

void DowServerProcessor::receiveProfileID(QNetworkReply *reply, QString steamID)
{
    if (checkReplyErrors("DowServerProcessor::receiveProfileID", reply))
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

                if (palyerDataArray.count() >= 8)
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
    if (checkReplyErrors("DowServerProcessor::receiveFindAdvertisements", reply))
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

                    //if (playerDataJson.at(5).toInt() == 3)
                    //    continue;

                    newPlayerData.partyID = QString::number(playerDataJson.at(0).toInt());
                    newPlayerData.profileID = QString::number(playerDataJson.at(1).toInt());
                    //newPlayerData.position = playerDataJson.at(5).toInt();

                    newPartyData.profilesIds.append(newPlayerData);
                }
                partysArray.append(newPartyData);
            }

            m_profileIdsForQueue = getPlayersInCurrentRoom(partysArray);
            addQuery(QueryType::PlayersSids);
        }
    }
}

void DowServerProcessor::receivePlayersSids(QNetworkReply *reply, QVector<PlayerData> profilesData)
{
    if (checkReplyErrors("DowServerProcessor::receivePlayersSids", reply))
        return;

    QByteArray replyByteArray = reply->readAll();
    reply->deleteLater();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyByteArray);

    QList<PlayerInfoFromDowServer> playersInfo;

    if (jsonDoc.isArray())
    {
        QJsonArray messageArray = jsonDoc.array();

        if (messageArray.count() >= 2 && messageArray.at(1).isArray())
        {
            QJsonArray playersJsonArray = messageArray.at(1).toArray();

            for (int i = 0; i < playersJsonArray.count(); i++)
            {
                if (!playersJsonArray.at(i).isArray())
                    continue;

                QJsonArray needPlayerJson = playersJsonArray.at(i).toArray();

                if (needPlayerJson.count() < 8)
                    continue;

                PlayerInfoFromDowServer newPlayerInfo;

                newPlayerInfo.steamId = needPlayerJson.at(2).toString().right(17);

                if (newPlayerInfo.steamId == m_steamID)
                    newPlayerInfo.isCurrentPlayer = true;

                newPlayerInfo.playerID = QString::number(needPlayerJson.at(1).toInt());
                newPlayerInfo.closeConnection = false;
                newPlayerInfo.name = needPlayerJson.at(4).toString();
                playersInfo.append(newPlayerInfo);
            }
        }
    }

    for (int i = 0; i < playersInfo.count(); i++)
    {
        for (int j = 0; j < profilesData.count(); j++)
        {
            if (playersInfo.at(i).playerID == profilesData.at(j).profileID )
            {
                playersInfo[i].position = profilesData.at(j).position + 1;
                break;
            }
        }
    }

    for (int i = 0; i < playersInfo.count(); i++)
        qInfo(logInfo()) << "Receive player data from DOW server:" << playersInfo.at(i).name << playersInfo.at(i).steamId;

    m_lastPlayersInfo = playersInfo;
    emit sendPlayersInfoFromDowServer(playersInfo);
}

void DowServerProcessor::playerDiscoonectTimerTimeout()
{
    addQuery(QueryType::FindAdvertisements);
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

QList<PlayerInfoFromDowServer> DowServerProcessor::lastPlayersInfo() const
{
    return m_lastPlayersInfo;
}

void DowServerProcessor::setGameType(GameType::GameTypeEnum newGameType)
{
    AbstractDowServerProcessor::setGameType(newGameType);
    m_queueTimer->start();
    //m_gameType = newGameType;
}

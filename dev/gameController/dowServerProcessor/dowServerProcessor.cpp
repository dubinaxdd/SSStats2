#include "dowServerProcessor.h"
#include <QNetworkReply>
#include <QJsonDocument>
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

    m_repeatGameResultRequestTimer.setInterval(10000); //Раз в 10 секунд пытаемся найти результаты игры, повторяем 12 раз
    m_repeatGameResultRequestTimer.setSingleShot(true);

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
                m_currentPlayerIsObserver = playersDataArray.at(j).isObserver;

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
    if (m_sessionId.isEmpty())
        return;

    QString urlString;

    if (m_gameType == GameType::GameTypeEnum::SoulstormSteam)
        urlString = "https://dow1ss-lobby.reliclink.com/game/chat/joinChannel?chatroomID=1&doRetry=1&sessionID=" + m_sessionId.toLocal8Bit();
    else if (m_gameType == GameType::GameTypeEnum::DefinitiveEdition)
        urlString = "https://dow-api.reliclink.com:443/game/chat/joinChannel?chatroomID=1&doRetry=1&sessionID=" + m_sessionId.toLocal8Bit();
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
    if (m_sessionId.isEmpty())
        return;

    QString urlString;

    if (m_gameType == GameType::GameTypeEnum::SoulstormSteam)
        urlString = "https://dow1ss-lobby.reliclink.com/game/account/getProfileID?profile_names=%5b%22%2fsteam%2f" + steamID.toLocal8Bit() + "%22%5d&sessionID=" + m_sessionId.toLocal8Bit();
    else if (m_gameType == GameType::GameTypeEnum::DefinitiveEdition)
        urlString = "https://dow-api.reliclink.com:443/game/account/getProfileID?profile_names=%5b%22%2fsteam%2f" + steamID.toLocal8Bit() + "%22%5d&sessionID=" + m_sessionId.toLocal8Bit();
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
    if (m_profileID.isEmpty() || m_statGroupId.isEmpty() || m_modName.isEmpty() || m_modVersion.isEmpty() || m_sessionId.isEmpty())
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
                    + m_sessionId.toLocal8Bit();
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
                    "&sessionID=" + m_sessionId.toLocal8Bit();
    }
    else
        return;

    QNetworkRequest newRequest = createDowServerRequest(urlString);
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveFindAdvertisements(reply);
    });
}

void DowServerProcessor::requestPlayersSids(QVector<PlayerData> profilesData, bool needSedGameResults, SendingReplayInfo lastGameResult)
{
    if (m_sessionId.isEmpty() || profilesData.count() < 1)
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
        urlString = "https://dow1ss-lobby.reliclink.com/game/account/getProfileName?profile_ids=" + profilesIDsString.toLocal8Bit() + "&sessionID=" + m_sessionId.toLocal8Bit();
    else if (m_gameType == GameType::GameTypeEnum::DefinitiveEdition)
        urlString = "https://dow-api.reliclink.com:443/game/account/getProfileName?profile_ids=" + profilesIDsString.toLocal8Bit() + "&sessionID=" + m_sessionId.toLocal8Bit();
    else
        return;

    QNetworkRequest newRequest = createDowServerRequest(urlString);
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receivePlayersSids(reply, profilesData, needSedGameResults, lastGameResult);
    });
}

void DowServerProcessor::requestPersonalStats(QList<PlayerInfoFromDowServer> playersInfo)
{
    if (m_currentMod != "dowde")
        return;

    QString idListString = m_profileID;

    for (auto& item : playersInfo)
    {
        if(idListString.isEmpty())
            idListString.append(item.playerID);
        else
            idListString.append("," + item.playerID);
    }

    QString urlString = "https://dow-api.reliclink.com/community/leaderboard/getpersonalstat?profile_ids=[" + idListString + "]&title=dow1-de";

    QNetworkRequest newRequest;
    newRequest.setUrl(urlString);
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        recievePersonalStats(reply);
    });
}

Race DowServerProcessor::getRaceById(int gaceId)
{
    switch(gaceId)
    {
        case 1: return Race::ChaosMarines;
        case 2: return Race::DarkEldar;
        case 3: return Race::Eldar;
        case 4: return Race::ImperialGuard;
        case 5: return Race::Necrons;
        case 6: return Race::Orks;
        case 7: return Race::SistersOfBattle;
        case 8: return Race::SpaceMarines;
        case 9: return Race::TauEmpire;
        default: return Race::SpaceMarines;
    }
}

void DowServerProcessor::requestGameResult(SendingReplayInfo lastGameResult)
{
    if (m_profileID.isEmpty())
        return;

    if (m_repeatRequestGameResultsCount == 0)
        m_repeatGameResultRequestTimer.start();

    QString urlString;

    QString needPlayerId = "";

    if (m_currentPlayerIsObserver)
        needPlayerId = m_nonObserverPlayerId;
    else
        needPlayerId = m_profileID;

    if (m_gameType == GameType::GameTypeEnum::DefinitiveEdition)
        urlString = "https://dow-api.reliclink.com/community/leaderboard/getrecentmatchhistorybyprofileid?title=dow1-de&profile_id=" + needPlayerId;
    else
        return;

    QNetworkRequest newRequest;
    newRequest.setUrl(urlString);
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveGameResults(reply, lastGameResult);
    });
}

void DowServerProcessor::receiveCurrentMod(QString modName)
{
    m_currentMod = modName;
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

    if (profilesData.isEmpty())
        return;

    m_currentPlayerIsObserver = false;
    m_profilesDataForQueue = profilesData;
    addQuery(QueryType::PlayersSids);

}

void DowServerProcessor::setRequestParametres(DowServerRequestParametres parametres)
{
    AbstractDowServerProcessor::setRequestParametres(parametres);

    qInfo(logInfo()) << "appBinaryChecksum:" << parametres.appBinaryChecksum << "dataChecksum:" << parametres.dataChecksum << "modDLLChecksum:" << parametres.modDLLChecksum;

    if(!m_steamID.isEmpty() && !m_sessionId.isEmpty())
    {
        addQuery(QueryType::ProfileID);

        if (m_neeedRequestAdvertisements)
        {
            addQuery(QueryType::FindAdvertisements);
            m_neeedRequestAdvertisements = false;
        }
    }
}

void DowServerProcessor::setSessionId(QString sessionId)
{
    AbstractDowServerProcessor::setSessionId(sessionId);

    if(!m_steamID.isEmpty() && !m_sessionId.isEmpty())
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

    if(!m_steamID.isEmpty() && !m_sessionId.isEmpty())
        addQuery(QueryType::ProfileID);
}

void DowServerProcessor::setCurrentModVersion(QString modVersion)
{
    m_modVersion = modVersion;
}

void DowServerProcessor::requestPartysData()
{
    if (m_profileID.isEmpty() || m_statGroupId.isEmpty() || m_modName.isEmpty() || m_modVersion.isEmpty() || m_sessionId.isEmpty())
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

                    emit sendCurrentPlayerId(m_profileID);
                    qInfo(logInfo()) << "statsGroupID=" << m_statGroupId;

                    PlayerInfoFromDowServer currentPlayerInfo;
                    currentPlayerInfo.isCurrentPlayer = true;
                    currentPlayerInfo.playerID = m_profileID;
                    currentPlayerInfo.steamId = m_steamID;


                    requestPersonalStats({currentPlayerInfo});

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

                    newPlayerData.partyID = QString::number(playerDataJson.at(0).toInt());
                    newPlayerData.profileID = QString::number(playerDataJson.at(1).toInt());

                    if (m_gameType == GameType::GameTypeEnum::DefinitiveEdition)
                        newPlayerData.isObserver = playerDataJson.at(5).toInt() == -1;

                    //newPlayerData.position = playerDataJson.at(5).toInt();

                    newPartyData.profilesIds.append(newPlayerData);
                }
                partysArray.append(newPartyData);
            }

            m_profilesDataForQueue = getPlayersInCurrentRoom(partysArray);

            if (m_currentPlayerIsObserver)
            {
                for(auto& item : m_profilesDataForQueue)
                {
                    if (!item.isObserver)
                    {
                        m_nonObserverPlayerId = item.profileID;
                        break;
                    }
                }
            }

            addQuery(QueryType::PlayersSids);
        }
    }
}

void DowServerProcessor::receivePlayersSids(QNetworkReply *reply, QVector<PlayerData> profilesData, bool needSendGameResults, SendingReplayInfo lastGameResult)
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
                playersInfo[i].isObserver = profilesData.at(j).isObserver;
                break;
            }
        }
    }

    for (int i = 0; i < playersInfo.count(); i++)
        qInfo(logInfo()) << "Receive player data from DOW server:" << playersInfo.at(i).name << playersInfo.at(i).steamId << playersInfo.at(i).playerID << "is observer:" << playersInfo.at(i).isObserver;

    if(!needSendGameResults)
        emit sendPlayersInfoFromDowServer(playersInfo);

    if (needSendGameResults)
    {
        lastGameResult.playersInfoFromDowServer = playersInfo;
        emit sendGameResults(m_gameResult, lastGameResult);
    }


    requestPersonalStats(playersInfo);
}

void DowServerProcessor::receiveGameResults(QNetworkReply *reply, SendingReplayInfo lastGameResult)
{
    if (checkReplyErrors("DowServerProcessor::receivePlayersSids", reply))
        return;

    QByteArray replyByteArray = reply->readAll();
    reply->deleteLater();

    QJsonDocument jsonDocument = QJsonDocument::fromJson(replyByteArray);

    if (!jsonDocument.isObject())
    {
        qWarning(logWarning()) << "DowServerProcessor::receiveGameResults - game results parse error";
        return;
    }

    QJsonObject jsonObject = jsonDocument.object();

    if (jsonObject.value("result").toObject().value("message").toString() != "SUCCESS")
    {
        qWarning(logWarning()) << "DowServerProcessor::receiveGameResults - game results is not SUCCESS";
        return;
    }

    QJsonArray matchHistoryStats = jsonObject.value("matchHistoryStats").toArray();

    bool gameResultsFinded = false;
    for(auto item : matchHistoryStats)
    {
        m_gameResult = item.toObject();

        if(QString::number(m_gameResult.value("id").toInt()) != lastGameResult.gameId)
            continue;

        QJsonArray matchhistorymember = m_gameResult.value("matchhistorymember").toArray();
        m_profilesDataForQueue.clear();

        for(auto item : matchhistorymember)
        {
            QString playerId = QString::number(item.toObject().value("profile_id").toInt());

            PlayerData newPlayerData;
            newPlayerData.profileID = playerId;
            //newPlayerData.partyID = gameId;
            m_profilesDataForQueue.append(newPlayerData);
        }

        gameResultsFinded = true;
        m_repeatRequestGameResultsCount = 0;
        m_repeatGameResultRequestTimer.stop();

        requestPlayersSids(m_profilesDataForQueue, true, lastGameResult);
        break;
    }

    if (gameResultsFinded)
    {
        qInfo(logInfo()) << "Game results finded";
        return;
    }

    if (m_repeatRequestGameResultsCount >= 12)
    {
        m_repeatGameResultRequestTimer.stop();
        m_repeatRequestGameResultsCount = 0;
        emit sendNotification(tr("Game results are not found."), true);
         QFile::remove(lastGameResult.replayPath);
    }
    else
    {
        if (m_repeatRequestGameResultsCount == 0)
            emit sendNotification(tr("The game results are not ready yet, the game will be sent within two minutes."), true);

        m_repeatGameResultRequestTimer.disconnect();
        QObject::connect(&m_repeatGameResultRequestTimer, &QTimer::timeout, this, [=]{requestGameResult(lastGameResult);}, Qt::QueuedConnection);
        m_repeatGameResultRequestTimer.start();
        qInfo(logInfo()) << "Try to find geme results";
        m_repeatRequestGameResultsCount++;
    }
}

void DowServerProcessor::recievePersonalStats(QNetworkReply *reply)
{
    if (checkReplyErrors("DowServerProcessor::recievePersonalStats", reply))
        return;

    QByteArray replyByteArray = reply->readAll();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyByteArray);

    if (!jsonDoc.isObject())
        return;

    QJsonObject jsonObject = jsonDoc.object();

    if(!jsonObject.value("statGroups").isArray())
        return;

    if(!jsonObject.value("leaderboardStats").isArray())
        return;

    QJsonArray statsGroups = jsonObject.value("statGroups").toArray();
    QJsonArray leaderboardStats = jsonObject.value("leaderboardStats").toArray();

    QVector<RelicStats> relicStatsArray;

    for (const auto& statsGroupsItem : statsGroups)
    {
        uint id = statsGroupsItem.toObject().value("id").toInt();

        RelicStats plyerStats;

        plyerStats.steamId =statsGroupsItem.toObject().value("members").toArray().at(0).toObject().value("name").toString().replace("/steam/", "");
        plyerStats.country = statsGroupsItem.toObject().value("members").toArray().at(0).toObject().value("country").toString();

        for (const auto& leaderboardStatsItem : leaderboardStats)
        {
            if (id != leaderboardStatsItem.toObject().value("statgroup_id").toInt())
                continue;

            plyerStats.gamesCount += leaderboardStatsItem.toObject().value("wins").toInt();
            plyerStats.gamesCount += leaderboardStatsItem.toObject().value("losses").toInt();
            plyerStats.winCount += leaderboardStatsItem.toObject().value("wins").toInt();

            int leaderboardId = leaderboardStatsItem.toObject().value("leaderboard_id").toInt();
            int rating = leaderboardStatsItem.toObject().value("rating").toInt();

            if (1 <= leaderboardId && leaderboardId <= 9)
            {
                if (rating > plyerStats.rating_1x1)
                {
                    plyerStats.rating_1x1 = rating;
                    plyerStats.race_1x1 = getRaceById(leaderboardId);
                }
            }
            else if (10 <= leaderboardId && leaderboardId <= 18)
            {
                if (rating > plyerStats.rating_2x2)
                {
                    plyerStats.rating_2x2 = rating;
                    plyerStats.race_2x2 = getRaceById(leaderboardId - 9);
                }
            }
            else if (19 <= leaderboardId && leaderboardId <= 27)
            {
                if (rating > plyerStats.rating_3x3)
                {
                    plyerStats.rating_3x3 = rating;
                    plyerStats.race_3x3 = getRaceById(leaderboardId - 18);
                }
            }
        }

        relicStatsArray.append(plyerStats);
    }

    emit sendRelicStats(relicStatsArray);
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
            requestPlayersSids(m_profilesDataForQueue);
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

void DowServerProcessor::setGameType(GameType::GameTypeEnum newGameType)
{
    AbstractDowServerProcessor::setGameType(newGameType);
    m_queueTimer->start();
}

#include "RankedModServiceProcessor.h"
#include <logger.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <defines.h>

RankedModServiceProcessor::RankedModServiceProcessor(SettingsController *settingsController, QObject *parent)
    : AbstractWebServiceProcessor(settingsController, parent)
{
        connect(&m_pingTimer, &QTimer::timeout, this, &RankedModServiceProcessor::requestRankedState, Qt::QueuedConnection);
    connect(&m_uniquePlayersOnlineStatistic, &QTimer::timeout, this, &RankedModServiceProcessor::requestUniquePlayersOnlineStatistic);
    connect(&m_uniquePlayersOnlineStatistic, &QTimer::timeout, this, &RankedModServiceProcessor::requestModsOnlineCount);

    m_serviceName = "Online Web Service";
    m_url = ONLINE_WEB_SERVICE;
    m_reconnectTimer.setInterval(5000);
    m_pingTimer.setInterval(1000);
    m_pingResponseTimer.setInterval(10000);
    m_pingResponseTimer.setSingleShot(true);
    m_uniquePlayersOnlineStatistic.setInterval(60000);
    m_uniquePlayersOnlineStatistic.start();

}

void RankedModServiceProcessor::setCurrentPlayerSteamId(QString currentPlayerSteamId)
{
    m_currentPlayerSteamId = currentPlayerSteamId;
    setRankedMode(true);
}

void RankedModServiceProcessor::setPlayresInfoFromDowServer(QList<PlayerInfoFromDowServer> playersInfoInfoFromDowServer)
{
    m_playersInfoInfoFromDowServer = playersInfoInfoFromDowServer;
    requestRankedState();

    qInfo(logInfo()) << "RankedModServiceProcessor::receivePlayresInfoFromDowServer" << "players info received";
}

void RankedModServiceProcessor::setRankedMode(bool rankedMode)
{
    m_rankedMode = rankedMode;
    sendPing();
}

void RankedModServiceProcessor::requestRankedState()
{
    QJsonArray players;

    if (m_playersInfoInfoFromDowServer.isEmpty())
    {
        if(m_currentPlayerSteamId.isEmpty())
            return;

        players.append(m_currentPlayerSteamId);
    }

    for (int i = 0; i < m_playersInfoInfoFromDowServer.count(); i++)
        players.append(m_playersInfoInfoFromDowServer.at(i).steamId);

    QJsonObject dataObject;
    dataObject.insert("players", players);

    QJsonObject messageObject;
    messageObject.insert("op", PlyersStateRequest);
    messageObject.insert("data", dataObject);

    QJsonDocument message;
    message.setObject(messageObject);

    m_webSocket.sendTextMessage(message.toJson().replace('\n',""));
}

void RankedModServiceProcessor::requestModsOnlineCount()
{
    QJsonObject messageObject;
    messageObject.insert("op", ModsOnlineCountRequest);

    QJsonDocument message;
    message.setObject(messageObject);

    m_webSocket.sendTextMessage(message.toJson().replace('\n',""));
}

void RankedModServiceProcessor::receiveRankedState(QJsonObject data)
{
    QVector<PlyersRankedState> plyersRankedState;

    for(auto& item : m_playersInfoInfoFromDowServer)
    {
        PlyersRankedState newPlyersRankedState;
        newPlyersRankedState.name = item.name;
        newPlyersRankedState.steamId = item.steamId;
        plyersRankedState.append(std::move(newPlyersRankedState));
    }

    PlyersRankedState newPlyersRankedState;
    newPlyersRankedState.steamId = m_currentPlayerSteamId;
    newPlyersRankedState.name = m_currentPlayerName;
    plyersRankedState.append(std::move(newPlyersRankedState));

    QJsonArray jsonArray = data.value("players_state").toArray();
    QVector<PlyersRankedState> newPlyersRankedStateArray;

    for(int i = 0; i < jsonArray.count(); i++)
    {

        for (int j = 0; j < plyersRankedState.count(); j++)
        {
            if (plyersRankedState.at(j).steamId == jsonArray.at(i)["steam_id"].toString())
            {
                PlyersRankedState newPlyersRankedState;

                newPlyersRankedState.name = plyersRankedState.at(j).name;
                newPlyersRankedState.steamId = plyersRankedState.at(j).steamId;
                newPlyersRankedState.isRanked = jsonArray.at(i)["is_ranked"].toBool();
                newPlyersRankedState.isOnline = jsonArray.at(i)["is_online"].toBool();

                newPlyersRankedStateArray.append(newPlyersRankedState);
            }
        }
    }

    emit sendPlyersRankedState(newPlyersRankedStateArray);
}

void RankedModServiceProcessor::receivePingRecponse(QJsonObject data)
{
    m_pingResponseTimer.stop();
}

void RankedModServiceProcessor::receiveModsOnlineCount(QJsonObject data)
{
    QJsonArray modArray = data.value("mods_online_count").toArray();
    QList<ModOnlineCount> modOnlineCountList;

    int allModsOnline = 0;

    for (std::size_t i = 0; i < modArray.count(); i++)
    {
        if (!modArray.at(i).isObject())
        {
            qWarning(logWarning()) << "RankedModServiceProcessor::receiveModsOnlineCount: invalid mod object.";
            return;
        }

        QJsonObject modObject = modArray.at(i).toObject();

        ModOnlineCount newModOnlineCount;

        newModOnlineCount.modUiName = modObject.value("ui_name").toString();
        newModOnlineCount.onlineCount = modObject.value("online_count").toInt();
        newModOnlineCount.order = modObject.value("order").toInt();

        allModsOnline += newModOnlineCount.onlineCount;

        modOnlineCountList.append(newModOnlineCount);
    }

    std::sort(modOnlineCountList.begin(), modOnlineCountList.end(), [](const ModOnlineCount &a, const ModOnlineCount &b) {
        return a.order < b.order;
    });

    emit sendOnlineCount(allModsOnline);
    emit sendModsOnlineCount(modOnlineCountList);
}

void RankedModServiceProcessor::receiveUniquePlayersOnlineStatistic(QJsonObject data)
{
    QJsonObject jsonObject = data.value("unique_players_online_statistic").toObject();

    UniqueOnlineStatistic uniqueOnlineStatistic;

    uniqueOnlineStatistic.day = jsonObject.value("last_day").toInt();
    uniqueOnlineStatistic.month = jsonObject.value("last_month").toInt();
    uniqueOnlineStatistic.year = jsonObject.value("last_year").toInt();
    uniqueOnlineStatistic.total = jsonObject.value("all_times").toInt();

    emit sendUniquePlayersOnlineStatistic(uniqueOnlineStatistic);
}

void RankedModServiceProcessor::setCurrentMod(QString modName)
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

    m_pingTimer.start();

    m_webSocket.open(QUrl(m_url));
    m_reconnectTimer.start();

    qInfo(logInfo()) << "RankedModServiceProcessor::onSettingsLoaded()" << "load finished";
}

void RankedModServiceProcessor::readMessage(QString message)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(message.toUtf8());

    if (!jsonDocument.isObject())
        return;

    QJsonObject messageObject = jsonDocument.object();

    OpCode opCode = static_cast<OpCode>(messageObject.value("op").toInt());

    switch(opCode)
    {
        case PingResponse: receivePingRecponse(messageObject.value("data").toObject()); break;
        case PlyersStateResponse: receiveRankedState(messageObject.value("data").toObject()); break;
        case UniquePlayersOnlineStatisticResponse: receiveUniquePlayersOnlineStatistic(messageObject.value("data").toObject()); break;
        case ModsOnlineCountResponse: receiveModsOnlineCount(messageObject.value("data").toObject()); break;
        default: break;
    }
}

void RankedModServiceProcessor::onConnected()
{
    AbstractWebServiceProcessor::onConnected();

    sendPing();
    requestUniquePlayersOnlineStatistic();
    requestModsOnlineCount();
}

void RankedModServiceProcessor::sendPing()
{
    if (m_currentPlayerSteamId.isEmpty() || m_currentMod.isEmpty())
        return;

    QJsonObject dataObject;
    dataObject.insert("steam_id", m_currentPlayerSteamId);
    dataObject.insert("current_mod", m_currentMod);
    dataObject.insert("ranked_state", m_rankedMode);

    QJsonObject messageObject;
    messageObject.insert("op", PingRequest);
    messageObject.insert("data", dataObject);

    QJsonDocument message;
    message.setObject(messageObject);

    m_webSocket.sendTextMessage(message.toJson().replace('\n',""));
    m_pingResponseTimer.start();
}

void RankedModServiceProcessor::requestUniquePlayersOnlineStatistic()
{
    QJsonObject messageObject;
    messageObject.insert("op", UniquePlayersOnlineStatisticRequest);

    QJsonDocument message;
    message.setObject(messageObject);

    m_webSocket.sendTextMessage(message.toJson().replace('\n',""));
}

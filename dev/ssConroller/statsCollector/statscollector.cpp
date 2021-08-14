#include "statscollector.h"
#include <QFile>
#include <QTextStream>
#include "../../../defines.h"
#include "QUrl"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#define SERVER_ADDRESS "http://207.154.238.90"
//#define SERVER_ADDRESS "https://dowstats.ru/"
#define SERVER_VERSION "108"

StatsCollector::StatsCollector(QString steamPath, QObject *parent)
    : QObject(parent)
    , m_steamPath(steamPath)
{
    m_networkManager = new QNetworkAccessManager(this);
}


void StatsCollector::parseCurrentPlayerSteamId()
{
    QFile file(m_steamPath+"\\config\\loginusers.vdf");

    qDebug() << "INFO: loginusers path: " +  m_steamPath+"\\config\\loginusers.vdf";


    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream textStream(&file);
        QStringList fileLines = textStream.readAll().split("\n");

        //qDebug() << fileLines;

        file.close();

        QStringList steamIDs;
        QStringList accountNames;
        QStringList personaNames;
        QStringList timestamps;

        for(int i = 0; i < fileLines.size(); i++ )
        {
            if (fileLines[i].contains("AccountName")){
                QString accountName = fileLines[i].right(fileLines[i].length() - 18);
                accountName = accountName.left(accountName.length() - 1);
                accountNames.append(accountName);

                QString steamID = fileLines[i-2].right(fileLines[i-2].length() - 2);
                steamID = steamID.left(steamID.length() - 1);
                steamIDs.append(steamID);
            }

            if (fileLines[i].contains("PersonaName")){
                QString personaName = fileLines[i].right(fileLines[i].length() - 18);
                personaName = personaName.left(personaName.length() - 1);
                personaNames.append(personaName);
            }

            if (fileLines[i].contains("Timestamp")){
                QString timestamp = fileLines[i].right(fileLines[i].length() - 16);
                timestamp = timestamp.left(timestamp.length() - 1);
                timestamps.append(timestamp);
            }
        }

        for(int i = 0; i < steamIDs.count(); i++)
        {
            QNetworkReply *reply = m_networkManager->get(QNetworkRequest(QUrl("http://api.steampowered.com/ISteamUser/GetPlayerSummaries/v0002/?key="+QLatin1String(STEAM_API_KEY) + "&steamids=" + steamIDs.at(i) + "&format=json")));
            QObject::connect(reply, &QNetworkReply::readyRead, this, [=](){
                receiveSteamInfoReply(reply);
            });
            //qDebug() << steamIDs.at(i);
        }
    }
}

void StatsCollector::getPlayrStatsFromServer(QString steamId)
{
    QNetworkReply *reply = m_networkManager->get(QNetworkRequest(QUrl(QString::fromStdString(SERVER_ADDRESS) + "/stats.php?key="+QLatin1String(SERVER_KEY) + "&sids=" + steamId + "&version="+SERVER_VERSION+"&sender_sid="+ steamId +"&")));
    QObject::connect(reply, &QNetworkReply::readyRead, this, [=](){
        receivePlayrStatsFromServer(reply, steamId);
    });
}

void StatsCollector::receiveSteamInfoReply(QNetworkReply *reply)
{
    if (!currentPlayerSteamId.isEmpty())
        return;

    QByteArray replyByteArray = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyByteArray);

    //qDebug() << jsonDoc;

    QVariantMap playerInfo = jsonDoc.object().toVariantMap();
    QVariantMap response = playerInfo.value("response", QVariantMap()).toMap();
    QVariantList players = response.value("players", QVariantList()).toList();
    QVariantMap player = players.value(0, QVariantMap()).toMap();

    QString playerName;
    QString senderName;
    QString steamId;

    playerName = player.value("personaname", "").toString();

    if(!playerName.isEmpty())
        senderName = playerName;

    steamId = player.value("steamid", "").toString();

    currentPlayerSteamId = steamId;


    // если игрок на данном аккаунте сейчас играет, и игра Soulstorm, то добавим его в список
    // после этого можно так же прерывать цикл, так как нужный игрок найден
    if(player.value("personastate", 0).toInt()==1 && player.value("gameid", 0).toInt()==9450)
        qDebug() << "INFO: Player" << playerName << "is online";
    else
        qDebug() << "INFO: Player" << playerName << "is offline";

    qDebug() << "INFO: Player's nickname and Steam ID associated with Soulstorm:" << senderName << steamId;

    getPlayrStatsFromServer(steamId);

    reply->deleteLater();
     //register_player(senderName, sender_steamID, true);
    // return true;
}

void StatsCollector::receivePlayrStatsFromServer(QNetworkReply *reply, QString steamId)
{
    QByteArray replyByteArray = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyByteArray);

    if (!jsonDoc.isArray())
        return;

    QJsonArray jsonArray = jsonDoc.array();

    for(int i = 0; i < jsonArray.count(); i++)
    {
        ServerPlayrStats playerInfo;

        playerInfo.apm = jsonArray.at(i)["apm"].toInt();
        playerInfo.gamesCount = jsonArray.at(i)["gamesCount"].toInt();
        playerInfo.mmr = jsonArray.at(i)["mmr"].toInt();
        playerInfo.mmr1v1 = jsonArray.at(i)["mmr1v1"].toInt();
        playerInfo.name = jsonArray.at(i)["name"].toString();
        playerInfo.race = jsonArray.at(i)["race"].toInt();
        playerInfo.winRate = jsonArray.at(i)["winRate"].toInt();
        playerInfo.winsCount = jsonArray.at(i)["winsCount"].toInt();

        if (currentPlayerSteamId == steamId)
            playerInfo.isCurrentPlayer = true;
        else
            playerInfo.isCurrentPlayer = false;

        emit sendServerPlayrStats(std::move(playerInfo));
    }

    reply->deleteLater();
}



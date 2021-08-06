#include "statscollector.h"
#include <QFile>
#include <QTextStream>
#include "../../../defines.h"
#include "QUrl"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

StatsCollector::StatsCollector(QString steamPath, QObject *parent)
    : QObject(parent)
    , m_steamPath(steamPath)
{
    m_networkManager = new QNetworkAccessManager(this);
    //connect(manager, SIGNAL(finished(QNetworkReply*)),
    //        this, SLOT(replyFinished(QNetworkReply*)));

    QObject::connect(m_networkManager, &QNetworkAccessManager::finished, this, &StatsCollector::receiveManagerReply, Qt::QueuedConnection);


}


void StatsCollector::parseCurrentPlayerSteamId()
{
    QFile file(m_steamPath+"\\config\\loginusers.vdf");

    //qDebug() << m_steamPath+"\\config\\loginusers.vdf";


    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream textStream(&file);
        QStringList fileLines = textStream.readAll().split("\n");

        qDebug() << fileLines;

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
            m_networkManager->get(QNetworkRequest(QUrl("http://api.steampowered.com/ISteamUser/GetPlayerSummaries/v0002/?key="+QLatin1String(STEAM_API_KEY) + "&steamids=" + steamIDs.at(i) + "&format=json")));
            qDebug() << steamIDs.at(i);
        }
    }
}

void StatsCollector::receiveManagerReply(QNetworkReply *reply)
{
    QByteArray replyByteArray = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyByteArray);

    QVariantMap player_info = jsonDoc.object().toVariantMap();
    QVariantMap response = player_info.value("response", QVariantMap()).toMap();
    QVariantList players = response.value("players", QVariantList()).toList();
    QVariantMap player = players.value(0, QVariantMap()).toMap();

    QString player_name;
    QString sender_name;
    QString steamId;

    player_name = player.value("personaname", "").toString();

    if(!player_name.isEmpty())
        sender_name = player_name;

    steamId = player.value("steamid", "").toString();

    // если игрок на данном аккаунте сейчас играет, и игра Soulstorm, то добавим его в список
    // после этого можно так же прерывать цикл, так как нужный игрок найден
    if(player.value("personastate", 0).toInt()==1 && player.value("gameid", 0).toInt()==9450)
        qDebug() << "INFO: Player" << player_name << "is online";
    else
        qDebug() << "INFO: Player" << player_name << "is offline";
    qDebug() << "INFO: Player's nickname and Steam ID associated with Soulstorm:" << sender_name << steamId;

     //register_player(sender_name, sender_steamID, true);
      // return true;
}

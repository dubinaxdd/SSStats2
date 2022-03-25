#include <statscollector.h>
#include <QFile>
#include <QTextStream>
#include <defines.h>
#include <QUrl>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QBuffer>
#include <defines.h>
#include <repreader.h>
#include <version.h>

#define CURRENT_PLAYER_STATS_REQUEST_TIMER_INTERVAL 5000


StatsCollector::StatsCollector(QString ssPath, QString steamPath, QObject *parent)
    : QObject(parent)
    , m_ssPath(ssPath)
    , m_steamPath(steamPath)
{
    m_networkManager = new QNetworkAccessManager(this);
    m_currentPlayerStats = QSharedPointer<ServerPlayerStats>(new ServerPlayerStats);

    m_currentPlayerStatsRequestTimer = new QTimer(this);
    m_currentPlayerStatsRequestTimer->setInterval(CURRENT_PLAYER_STATS_REQUEST_TIMER_INTERVAL);
    connect(m_currentPlayerStatsRequestTimer, &QTimer::timeout, this, &StatsCollector::currentPlayerStatsRequestTimerTimeout, Qt::QueuedConnection);

    m_currentPlayerStatsRequestTimer->start();

    m_clientVersion.append(PROJECT_VERSION_MAJOR);
    m_clientVersion.append(PROJECT_VERSION_MINOR);
    m_clientVersion.append(GIT_REL);

    qInfo(logInfo()) << "OpenSSL available:" << QSslSocket::supportsSsl() << QSslSocket::sslLibraryBuildVersionString() << QSslSocket::sslLibraryVersionString();
    qInfo(logInfo()) << "Protocol version:" << m_clientVersion;

}

void StatsCollector::receivePlayresStemIdFromScanner(QList<SearchStemIdPlayerInfo> playersInfoFromScanner, int playersCount )
{
    //m_playerStats.clear();

    emit sendPlayersCount(playersCount);

    emit sendCurrentPlayerHostState(false);

    for(int i = 0; i < playersInfoFromScanner.count(); i++)
    {

        QSharedPointer <ServerPlayerStats> newPlayer(new ServerPlayerStats);

        newPlayer->steamId = playersInfoFromScanner.at(i).steamId;
        newPlayer->position = playersInfoFromScanner.at(i).position;

        registerPlayer(playersInfoFromScanner.at(i).name, playersInfoFromScanner.at(i).steamId, false);

        QNetworkReply *reply = m_networkManager->get(QNetworkRequest(QUrl("http://api.steampowered.com/ISteamUser/GetPlayerSummaries/v0002/?key="+QLatin1String(STEAM_API_KEY) + "&steamids=" + playersInfoFromScanner.at(i).steamId + "&format=json")));
        QObject::connect(reply, &QNetworkReply::finished, this, [=](){
            receivePlayerSteamData(reply, newPlayer);
        });

        QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
            reply->deleteLater();
        });
    }
}

void StatsCollector::receivePlayerStemIdForHostedGame(SearchStemIdPlayerInfo playerInfoFromScanner)
{
    emit sendPlayersCount(8);
    emit sendCurrentPlayerHostState(true);

    QSharedPointer <ServerPlayerStats> newPlayer(new ServerPlayerStats);

    newPlayer->steamId = playerInfoFromScanner.steamId;
    newPlayer->position = playerInfoFromScanner.position;

    registerPlayer(playerInfoFromScanner.name, playerInfoFromScanner.steamId, false);

    QNetworkReply *reply = m_networkManager->get(QNetworkRequest(QUrl("http://api.steampowered.com/ISteamUser/GetPlayerSummaries/v0002/?key="+QLatin1String(STEAM_API_KEY) + "&steamids=" + playerInfoFromScanner.steamId + "&format=json")));
    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receivePlayerSteamData(reply, newPlayer);
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
        reply->deleteLater();
    });
}

void StatsCollector::parseCurrentPlayerSteamId()
{
    QFile file(m_steamPath+"\\config\\loginusers.vdf");

    qInfo(logInfo()) << "loginusers path: " <<  m_steamPath + "\\config\\loginusers.vdf";

    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream textStream(&file);
        QStringList fileLines = textStream.readAll().split("\n");

        file.close();

        QStringList steamIDs;
        QStringList accountNames;
        QStringList personaNames;
        QStringList timestamps;
        QStringList mostRecents;

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

            if (fileLines[i].contains("MostRecent") || fileLines[i].contains("mostrecent")){
                QString mostRecent = fileLines[i].right(fileLines[i].length() - 17);
                mostRecent = mostRecent.left(mostRecent.length() - 1);
                mostRecents.append(mostRecent);
            }
        }

        for(int i = 0; i < steamIDs.count(); i++)
        {
            if(mostRecents.at(i) == "1")
            {
                QNetworkReply *reply = m_networkManager->get(QNetworkRequest(QUrl("http://api.steampowered.com/ISteamUser/GetPlayerSummaries/v0002/?key="+QLatin1String(STEAM_API_KEY) + "&steamids=" + steamIDs.at(i) + "&format=json")));
                QObject::connect(reply, &QNetworkReply::finished, this, [=](){
                    receiveSteamInfoReply(reply);
                });


                QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
                    reply->deleteLater();
                });

                return;
            }
        }
    }
}

void StatsCollector::getPlayerStatsFromServer(QSharedPointer <ServerPlayerStats> playerInfo)
{
    QNetworkReply *reply = m_networkManager->get(QNetworkRequest(QUrl(QString::fromStdString(SERVER_ADDRESS) + "/stats.php?key="+QLatin1String(SERVER_KEY) + "&sids=" + playerInfo->steamId + "&version="+m_clientVersion+"&sender_sid="+ m_currentPlayerStats->steamId +"&")));
    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receivePlayerStatsFromServer(reply, playerInfo);
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
        reply->deleteLater();
    });
}

void StatsCollector::getPlayerMediumAvatar(QString url, QSharedPointer <ServerPlayerStats> playerInfo)
{
    QNetworkReply *reply = m_networkManager->get(QNetworkRequest(QUrl(url)));
    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receivePlayerMediumAvatar(reply, playerInfo);
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
        reply->deleteLater();
    });
}

void StatsCollector::receiveSteamInfoReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "Connection error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    if (m_currentPlayerAccepted)
    {
        reply->deleteLater();
        return;
    }

    QByteArray replyByteArray = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyByteArray);

    QVariantMap playerInfo = jsonDoc.object().toVariantMap();
    QVariantMap response = playerInfo.value("response", QVariantMap()).toMap();
    QVariantList players = response.value("players", QVariantList()).toList();
    QVariantMap player = players.value(0, QVariantMap()).toMap();

    QString playerName;
    QString senderName;
    QString steamId;
    QString avatarUrl;

    playerName = player.value("personaname", "").toString();
    avatarUrl = player.value("avatarmedium", "").toString();

    if(!playerName.isEmpty())
        senderName = playerName;

    steamId = player.value("steamid", "").toString();

    m_currentPlayerAccepted = true;

    qInfo(logInfo()) << "Player's nickname and Steam ID associated with Soulstorm:" << senderName << steamId;

    m_currentPlayerStats->steamId = steamId;
    m_currentPlayerStats->isCurrentPlayer = true;

    registerPlayer(playerName, steamId, true);

    getPlayerStatsFromServer(m_currentPlayerStats);
    getPlayerMediumAvatar(avatarUrl, m_currentPlayerStats);

    reply->deleteLater();
}

void StatsCollector::receivePlayerStatsFromServer(QNetworkReply *reply, QSharedPointer <ServerPlayerStats> playerInfo)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "Connection error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray replyByteArray = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyByteArray);

    if (!jsonDoc.isArray())
    {
        reply->deleteLater();
        return;
    }

    QJsonArray jsonArray = jsonDoc.array();

    for(int i = 0; i < jsonArray.count(); i++)
    {
        playerInfo->apm = jsonArray.at(i)["apm"].toInt();
        playerInfo->gamesCount = jsonArray.at(i)["gamesCount"].toInt();
        playerInfo->mmr = jsonArray.at(i)["mmr"].toInt();
        playerInfo->mmr1v1 = jsonArray.at(i)["mmr1v1"].toInt();
        playerInfo->isBanned = jsonArray.at(i)["isBanned"].toBool();
        playerInfo->name = jsonArray.at(i)["name"].toString();
        playerInfo->race = jsonArray.at(i)["race"].toInt();
        playerInfo->winRate = jsonArray.at(i)["winRate"].toInt();
        playerInfo->winsCount = jsonArray.at(i)["winsCount"].toInt();

        playerInfo->statsAvailable = true;


        if (playerInfo->avatarAvailable && playerInfo->statsAvailable)
            emit sendServerPlayrStats(*playerInfo.data());

    }

    reply->deleteLater();
}

void StatsCollector::receivePlayerMediumAvatar(QNetworkReply *reply, QSharedPointer <ServerPlayerStats> playerInfo)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "Connection error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray replyByteArray = reply->readAll();

    QImage avatarMedium = QImage::fromData(replyByteArray);


    if (avatarMedium.isNull())
    {
        reply->deleteLater();
        return;
    }

    playerInfo->avatarAvailable = true;
    playerInfo->avatar = avatarMedium;

    if (playerInfo->avatarAvailable && playerInfo->statsAvailable)
        emit sendServerPlayrStats(*playerInfo.data());

    reply->deleteLater();
}

void StatsCollector::receivePlayerSteamData(QNetworkReply *reply, QSharedPointer <ServerPlayerStats> playerInfo)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "Connection error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray replyByteArray = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyByteArray);

    QVariantMap info = jsonDoc.object().toVariantMap();
    QVariantMap response = info.value("response", QVariantMap()).toMap();
    QVariantList players = response.value("players", QVariantList()).toList();
    QVariantMap player = players.value(0, QVariantMap()).toMap();

    QString steamId;
    QString avatarUrl;

    avatarUrl = player.value("avatarmedium", "").toString();

    getPlayerStatsFromServer(playerInfo);
    getPlayerMediumAvatar(avatarUrl, playerInfo);

    reply->deleteLater();
}

void StatsCollector::currentPlayerStatsRequestTimerTimeout()
{
    if (!m_currentPlayerStats->steamId.isEmpty())
        getPlayerStatsFromServer(m_currentPlayerStats);
}

void StatsCollector::sendReplayToServer(SendingReplayInfo replayInfo)
{
    if (replayInfo.playersInfo.count() < 2 || replayInfo.playersInfo.count() > 8)
        return;

    for (int i = 0; i < replayInfo.playersInfo.count(); i++)
    {
        if (replayInfo.playersInfo[i].playerName == m_currentPlayerStats->name)
            replayInfo.playersInfo[i].playerSid = m_currentPlayerStats->steamId;

        if (replayInfo.playersInfo[i].playerSid == "")
        {
            qWarning() << "Player" << replayInfo.playersInfo[i].playerName << "have not steam id, replay not sended";
            return;
        }
    }


    QString url;

    url = QString::fromStdString(SERVER_ADDRESS) + "/connect.php?";

    int winnerCount = 0;

    for(int i = 0; i < replayInfo.playersInfo.count(); i++)
    {
        url += "p" + QString::number(i+1) + "=" + replayInfo.playersInfo.at(i).playerName + "&";
        url += "sid" + QString::number(i+1) + "=" + replayInfo.playersInfo.at(i).playerSid + "&";
        url += "r" + QString::number(i+1) + "=" + QString::number(replayInfo.playersInfo.at(i).playerRace + 1) + "&";

        if (replayInfo.playersInfo.at(i).isWinner)
        {
            winnerCount++;
            url += "w" + QString::number(winnerCount) + "=" + QString::number(i+1) + "&";
        }
    }

    url += "apm=" + QString::number(replayInfo.apm) + "&";
    url += "type=" + QString::number(static_cast<int>(replayInfo.gameType)) + "&";
    url += "map=" + replayInfo.mapName + "&";
    url += "gtime=" +  QString::number(replayInfo.gameTime) + "&";
    url += "sid=" + m_currentPlayerStats->steamId + "&";
    url += "mod=" + replayInfo.mod + "&";

    QString winCondition;

    switch (replayInfo.winBy)
    {
        case WinCondition::ANNIHILATE: winCondition = "ANNIHILATE"; break;
        case WinCondition::CONTROLAREA: winCondition = "CONTROLAREA"; break;
        case WinCondition::STRATEGICOBJECTIVE: winCondition = "STRATEGICOBJECTIVE"; break;
        case WinCondition::ASSASSINATE: winCondition = "ASSASSINATE"; break;
        case WinCondition::DESTROYHQ: winCondition = "DESTROYHQ"; break;
        case WinCondition::ECONOMICVICTORY: winCondition = "ECONOMICVICTORY"; break;
        case WinCondition::SUDDENDEATH: winCondition = "SUDDENDEATH"; break;
        case WinCondition::GAMETIMER: break;
    }

    url += "winby=" + winCondition + "&";

    url += "version=" + m_clientVersion + "&";

    qInfo(logInfo()) << "Replay sending url:" << url;

    url += "key=" + QLatin1String(SERVER_KEY);


    RepReader repReader(m_ssPath+"/Playback/temp.rec");

    repReader.convertReplayToSteamVersion();
    repReader.isStandart(replayInfo.gameType);
    repReader.RenameReplay();

    QByteArray playback = repReader.getReplayData();


    QNetworkRequest request = QNetworkRequest(QUrl(url));

    QByteArray postData, boundary="1BEF0A57BE110FD467A";
    //параметр 2 - файл
    postData.append("--"+boundary+"\r\n");//разделитель
    //имя параметра
    postData.append("Content-Disposition: form-data; name=\"");
    postData.append("ReplayCRC32");
    postData.append("\"\r\n\r\n");
    //значение параметра
    postData.append(CRC32fromByteArray(playback).toUtf8());
    postData.append("\r\n");
    //параметр 2 - файл
    postData.append("--"+boundary+"\r\n");//разделитель
    //имя параметра
    postData.append("Content-Disposition: form-data; name=\"file\";");
    //имя файла
    postData.append("filename=\"");
    postData.append(GetRandomString().toUtf8() +".rec");
    postData.append("\"\r\n");
    //тип содержимого файла
    postData.append("Content-Type: application/octet-stream\r\n");
    //передаем в base64
    postData.append("Content-Transfer-Encoding: binary\r\n\r\n");
    //данные
    postData.append(playback);
    postData.append("\r\n");
    //"хвост" запроса
    postData.append("--"+boundary+"--\r\n");

    request.setHeader(QNetworkRequest::ContentTypeHeader,"multipart/form-data; boundary="+boundary);
    request.setHeader(QNetworkRequest::ContentLengthHeader,QByteArray::number(postData.length()));

    request.setRawHeader("User-Agent", "");

    QNetworkReply *reply = m_networkManager->post(request, postData);
    qInfo(logInfo()) << "Replay sended to server";

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        reply->deleteLater();
    });
}

QString StatsCollector::GetRandomString() const
{
   const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
   const int randomStringLength = 12; // assuming you want random strings of 12 characters

   QString randomString;
   for(int i=0; i < randomStringLength; ++i)
   {
       int index = qrand() % possibleCharacters.length();
       QChar nextChar = possibleCharacters.at(index);
       randomString.append(nextChar);
   }
   return randomString;
}

QString StatsCollector::CRC32fromByteArray( const QByteArray & array )
{
    quint32 crc32 = 0xffffffff;
    QByteArray buf(256, 0);
    qint64 n;
    QBuffer buffer;
    buffer.setData( array );
    if ( !buffer.open( QIODevice::ReadOnly ) )
        return 0;
    while( ( n = buffer.read( buf.data(), 256) ) > 0 )
        for ( qint64 i = 0; i < n; i++ )
            crc32 = ( crc32 >> 8 ) ^ CRC32Table[(crc32 ^ buf.at(i)) & 0xff ];
    buffer.close();
    crc32 ^= 0xffffffff;
    return QString("%1").arg(crc32, 8, 16, QChar('0'));
}

void StatsCollector::setCurrentPlayerAccepted(bool newCurrentPlayerAccepted)
{
    m_currentPlayerAccepted = newCurrentPlayerAccepted;
}


void StatsCollector::registerPlayer(QString name, QString sid, bool init)
{
    QByteArray enc_name = QUrl::toPercentEncoding(name.toLocal8Bit(),""," ");
    QString reg_url = QString::fromStdString(SERVER_ADDRESS)+"/regplayer.php?name="+enc_name+"&sid="+sid+"&version="+m_clientVersion+"&sender_sid="+m_currentPlayerStats->steamId+"&";
    if( init )
        reg_url += "init=true&";
    else
        reg_url += "init=false&";

    reg_url += "key="+QLatin1String(SERVER_KEY)+"&";

    QNetworkReply *reply = m_networkManager->get(QNetworkRequest(QUrl(reg_url)));
    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        reply->deleteLater();
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
        reply->deleteLater();
    });
}



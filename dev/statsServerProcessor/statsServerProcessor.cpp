#include <statsServerProcessor.h>
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
#include <QDebug>

#define CURRENT_PLAYER_STATS_REQUEST_TIMER_INTERVAL 5000

using namespace ReplayReader;

StatsServerProcessor::StatsServerProcessor(QString ssPath, QString steamPath, QObject *parent)
    : QObject(parent)
    , m_ssPath(ssPath)
    , m_steamPath(steamPath)
{
    m_machineUniqueId = QSysInfo::machineUniqueId();

    m_networkManager = new QNetworkAccessManager(this);
    m_currentPlayerStats = QSharedPointer <QList<ServerPlayerStats>>(new QList<ServerPlayerStats>);

    m_currentPlayerStatsRequestTimer = new QTimer(this);
    m_currentPlayerStatsRequestTimer->setInterval(CURRENT_PLAYER_STATS_REQUEST_TIMER_INTERVAL);
    connect(m_currentPlayerStatsRequestTimer, &QTimer::timeout, this, &StatsServerProcessor::currentPlayerStatsRequestTimerTimeout, Qt::QueuedConnection);

    m_currentPlayerStatsRequestTimer->start();

    m_clientVersion.append(PROJECT_VERSION_MAJOR);
    m_clientVersion.append(PROJECT_VERSION_MINOR);
    m_clientVersion.append(GIT_REL);

    qInfo(logInfo()) << "OpenSSL available:" << QSslSocket::supportsSsl() << QSslSocket::sslLibraryBuildVersionString() << QSslSocket::sslLibraryVersionString();
    qInfo(logInfo()) << "Protocol version:" << m_clientVersion;

}

void StatsServerProcessor::receivePlayresStemIdFromScanner(QList<SearchStemIdPlayerInfo> playersInfoFromScanner, int playersCount )
{
    emit sendPlayersCount(playersCount - 1);
    emit sendCurrentPlayerHostState(false);

    QSharedPointer <QList<ServerPlayerStats>> playersInfo(new QList<ServerPlayerStats>);

    for(int i = 0; i < playersInfoFromScanner.count(); i++)
    {
        if (playersInfoFromScanner[i].isCurrentPlayer)
            continue;

        ServerPlayerStats newPlayerInfo;

        newPlayerInfo.steamId = playersInfoFromScanner.at(i).steamId;
        newPlayerInfo.position = playersInfoFromScanner.at(i).position;

        playersInfo.data()->append(newPlayerInfo);
    }

    getPlayerStatsFromServer(playersInfo);
}

void StatsServerProcessor::parseCurrentPlayerSteamId()
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
                QSharedPointer <QList<ServerPlayerStats>> playersInfo(new QList<ServerPlayerStats>);

                ServerPlayerStats currentPlayerInfo;

                currentPlayerInfo.steamId = steamIDs.at(i);
                currentPlayerInfo.isCurrentPlayer = true;
                playersInfo.data()->append(currentPlayerInfo);

                m_currentPlayerStats = playersInfo;

                emit sendCurrentPlayerSteamID(currentPlayerInfo.steamId);

                getPlayerStatsFromServer(playersInfo);

                return;
            }
        }
    }
}

void StatsServerProcessor::getPlayerStatsFromServer(QSharedPointer <QList<ServerPlayerStats>> playersInfo)
{
    QString sidsString;

    for(int i = 0; i < playersInfo.data()->count(); i++)
    {
        if ( i != 0)
            sidsString += ",";

        sidsString += playersInfo.data()->at(i).steamId;
    }

    QNetworkRequest newRequest = QNetworkRequest(QUrl(QString::fromStdString(SERVER_ADDRESS) + "/api/stats.php?sids=" + sidsString + "&version="+m_clientVersion+"&sender_sid="+ m_currentPlayerStats.data()->at(0).steamId));
    newRequest.setRawHeader("key", QString::fromStdString(SERVER_KEY).toLatin1());
    newRequest.setRawHeader("machineUniqueId", m_machineUniqueId.toLatin1());

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receivePlayerStatsFromServer(reply, playersInfo);
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
        reply->deleteLater();
    });
}

void StatsServerProcessor::getPlayersMediumAvatar(QSharedPointer<QList<ServerPlayerStats>> playersInfo)
{
    for (int i = 0; i < playersInfo.data()->count(); i++)
    {
        QSharedPointer<ServerPlayerStats> playerInfo(new ServerPlayerStats(playersInfo.data()->at(i)));

        QNetworkReply *reply = m_networkManager->get(QNetworkRequest(QUrl(playerInfo.data()->avatarUrl)));
        QObject::connect(reply, &QNetworkReply::finished, this, [=](){
            receivePlayerMediumAvatar(reply, playerInfo);
        });

        QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
            reply->deleteLater();
            qWarning(logWarning()) << "Connection error:" << reply->errorString();
        });
    }
}

void StatsServerProcessor::receivePlayerStatsFromServer(QNetworkReply *reply, QSharedPointer <QList<ServerPlayerStats>> playersInfo)
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
        qWarning(logWarning()) << "Bad reply from dowstats:" << QString::fromLatin1(replyByteArray);

        reply->deleteLater();
        return;
    }

    QJsonArray jsonArray = jsonDoc.array();

    for(int i = 0; i < jsonArray.count(); i++)
    {
        playersInfo.get()->operator[](i).apm = jsonArray.at(i)["apm"].toInt();
        playersInfo.get()->operator[](i).gamesCount = jsonArray.at(i)["gamesCount"].toInt();
        playersInfo.get()->operator[](i).mmr = jsonArray.at(i)["mmr"].toInt();
        playersInfo.get()->operator[](i).mmr1v1 = jsonArray.at(i)["mmr1v1"].toInt();
        playersInfo.get()->operator[](i).isBanned = jsonArray.at(i)["isBanned"].toBool();
        playersInfo.get()->operator[](i).name = jsonArray.at(i)["name"].toString();
        playersInfo.get()->operator[](i).race = jsonArray.at(i)["race"].toInt();
        playersInfo.get()->operator[](i).winRate = jsonArray.at(i)["winRate"].toInt();
        playersInfo.get()->operator[](i).winsCount = jsonArray.at(i)["winsCount"].toInt();
        playersInfo.get()->operator[](i).avatarUrl = jsonArray.at(i)["avatarUrl"].toString();
        playersInfo.get()->operator[](i).calibrateGamesLeft = jsonArray.at(i)["calibrateGamesLeft"].toInt();

        //playersInfo.get()->operator[](i).calibrateGamesLeft = 5;

        playersInfo.get()->operator[](i).statsAvailable = true;

       // emit sendServerPlayrStats(playersInfo.get()->operator[](i));
    }

    //if (playerInfo->avatarAvailable && playerInfo->statsAvailable)
    //    emit sendServerPlayrStats(*playerInfo.data());

    getPlayersMediumAvatar(playersInfo);
    reply->deleteLater();
}

void StatsServerProcessor::receivePlayerMediumAvatar(QNetworkReply *reply, QSharedPointer <ServerPlayerStats> playerInfo)
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

void StatsServerProcessor::currentPlayerStatsRequestTimerTimeout()
{
    if (!m_currentPlayerStats.data()->isEmpty() && !m_currentPlayerStats.data()->at(0).steamId.isEmpty())
        getPlayerStatsFromServer(m_currentPlayerStats);
}

void StatsServerProcessor::sendReplayToServer(SendingReplayInfo replayInfo)
{
    if (replayInfo.playersInfo.count() < 2 || replayInfo.playersInfo.count() > 8)
        return;

    for (int i = 0; i < replayInfo.playersInfo.count(); i++)
    {
        if (replayInfo.playersInfo[i].playerName == m_currentPlayerStats.data()->at(0).name)
            replayInfo.playersInfo[i].playerSid = m_currentPlayerStats.data()->at(0).steamId;

        if (replayInfo.playersInfo[i].playerSid == "")
        {
            qWarning() << "Player" << replayInfo.playersInfo[i].playerName << "does not have a steam id, the replay has not been uploaded to the server";
            emit sendNotification("Player " + replayInfo.playersInfo[i].playerName + " does not have a steam id, the replay has not been uploaded to the server", true);
            return;
        }
    }

    QString url;

    //url = QString::fromStdString(SERVER_ADDRESS) + "/api/send_replay.php?";
    url = QString::fromStdString(SERVER_ADDRESS) + "/api/send_replay2.php?";

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
    url += "sid=" + m_currentPlayerStats.data()->at(0).steamId + "&";
    url += "mod=" + replayInfo.mod + "&";

    if (replayInfo.isFullStdGame)
        url += "isFullStdGame=" + QString::number(1) + "&";
    else
        url += "isFullStdGame=" + QString::number(0) + "&";

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

    request.setRawHeader("key", QString::fromStdString(SERVER_KEY).toLatin1());
    request.setRawHeader("machineUniqueId", m_machineUniqueId.toLatin1());

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
    qInfo(logInfo()) << "The replay has been uploaded to the server";

    emit sendNotification("The replay has been uploaded to the server", false);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){  
        reply->deleteLater();
    });
}

QString StatsServerProcessor::GetRandomString() const
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

QString StatsServerProcessor::CRC32fromByteArray( const QByteArray & array )
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

void StatsServerProcessor::setCurrentPlayerAccepted(bool newCurrentPlayerAccepted)
{
    m_currentPlayerAccepted = newCurrentPlayerAccepted;
}


void StatsServerProcessor::registerPlayer(QString name, QString sid, bool init)
{
    QByteArray enc_name = QUrl::toPercentEncoding(name.toLocal8Bit(),""," ");
    QString reg_url = QString::fromStdString(SERVER_ADDRESS)+"/regplayer.php?name="+enc_name+"&sid="+sid+"&version="+m_clientVersion+"&sender_sid="+m_currentPlayerStats.data()->at(0).steamId+"&";
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


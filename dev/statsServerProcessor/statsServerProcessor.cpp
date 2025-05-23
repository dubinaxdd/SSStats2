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

StatsServerProcessor::StatsServerProcessor(SettingsController *settingsController, QString ssPath, QString steamPath, QObject *parent)
    : QObject(parent)
    , m_settingsController(settingsController)
    , m_rankDiversionTimer(new QTimer(this))
    , m_steamPath(steamPath)
    , m_ssPath(ssPath)
    , m_networkManager( new QNetworkAccessManager(this))
{
    m_machineUniqueId = QSysInfo::machineUniqueId();
    m_currentPlayerStats = QSharedPointer <QList<ServerPlayerStats>>(new QList<ServerPlayerStats>);


    m_currentPlayerStatsRequestTimer = new QTimer(this);
    m_currentPlayerStatsRequestTimer->setInterval(CURRENT_PLAYER_STATS_REQUEST_TIMER_INTERVAL);

    m_rankDiversionTimer->setInterval(60000);

    connect(m_currentPlayerStatsRequestTimer, &QTimer::timeout, this, &StatsServerProcessor::currentPlayerStatsRequestTimerTimeout, Qt::QueuedConnection);
    connect(m_currentPlayerStatsRequestTimer, &QTimer::timeout, this, &StatsServerProcessor::requestClientLastVersion, Qt::QueuedConnection);

    connect(m_settingsController, &SettingsController::settingsLoaded, this, &StatsServerProcessor::onSettingsLoaded, Qt::QueuedConnection);
    connect(m_rankDiversionTimer, &QTimer::timeout, this, &StatsServerProcessor::onRankDiversionTimerTimeout, Qt::QueuedConnection);

    m_clientVersion.append(PROJECT_VERSION_MAJOR);
    m_clientVersion.append(PROJECT_VERSION_MINOR);
    m_clientVersion.append(GIT_REL);

    qInfo(logInfo()) << "OpenSSL available:" << QSslSocket::supportsSsl() << QSslSocket::sslLibraryBuildVersionString() << QSslSocket::sslLibraryVersionString();
    qInfo(logInfo()) << "Protocol version:" << m_clientVersion;
}

void StatsServerProcessor::receivePlayresInfoFromDowServer(QList<PlayerInfoFromDowServer> playersInfoInfoFromDowServer)
{
    QSharedPointer <QList<ServerPlayerStats>> playersInfo(new QList<ServerPlayerStats>);

    for(int i = 0; i < playersInfoInfoFromDowServer.count(); i++)
    {
        if (playersInfoInfoFromDowServer[i].isCurrentPlayer)
        {
            m_currentPlayerStats.get()->operator[](0).dowServerName = playersInfoInfoFromDowServer.at(i).name;
            continue;
        }

        ServerPlayerStats newPlayerInfo;

        newPlayerInfo.steamId = playersInfoInfoFromDowServer.at(i).steamId;
        newPlayerInfo.position = playersInfoInfoFromDowServer.at(i).position;
        newPlayerInfo.dowServerName = playersInfoInfoFromDowServer.at(i).name;

        playersInfo.data()->append(newPlayerInfo);
    }

    if (playersInfo.data()->count() > 0)
        getPlayerStatsFromServer(playersInfo);

    qInfo(logInfo()) << "StatsServerProcessor::receivePlayresInfoFromDowServer" << "players info received";
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
    QString nickNames;

    for(int i = 0; i < playersInfo.data()->count(); i++)
    {
        if ( i != 0)
        {
            sidsString += ",";
            nickNames += ",";
        }

        sidsString += playersInfo.data()->at(i).steamId;
        nickNames += playersInfo.data()->at(i).dowServerName.toUtf8().toBase64();
    }

    QUrl url = QUrl(QString::fromStdString(SERVER_ADDRESS) + "/api/stats4.php?sids=" + sidsString
                    + "&version=" + m_clientVersion
                    + "&sender_sid=" + m_currentPlayerStats.data()->at(0).steamId
                    + "&mod_tech_name=" + m_currentMod
                    + "&nicks=" + nickNames);

    QNetworkRequest newRequest = QNetworkRequest(url);

    newRequest.setRawHeader("key", QString::fromStdString(SERVER_KEY).toLatin1());
    newRequest.setRawHeader("machineUniqueId", m_machineUniqueId.toLatin1());

    //if (!m_authKey.isEmpty())
    //    newRequest.setRawHeader("steamAuthId", m_authKey.toLatin1());

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receivePlayerStatsFromServer(reply, playersInfo);
    });
}

void StatsServerProcessor::receivePlayerStatsFromServer(QNetworkReply *reply, QSharedPointer <QList<ServerPlayerStats>> playersInfo)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "StatsServerProcessor::receivePlayerStatsFromServer:" << "Connection error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray replyByteArray = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyByteArray);

    if (!jsonDoc.isObject())
    {
        qWarning(logWarning()) << "StatsServerProcessor::receivePlayerStatsFromServer:" << "Bad reply from dowstats:" << QString::fromLatin1(replyByteArray);
        reply->deleteLater();
        return;
    }

    QJsonObject jsonObject = jsonDoc.object();

    QString modName = jsonObject.value("modName").toString();
    emit sendStatisticModName(modName);
    //QString seasonName = jsonObject.value("seasonName").toString();

    if(!jsonObject.value("stats").isArray())
    {
        qWarning(logWarning()) << "StatsServerProcessor::receivePlayerStatsFromServer:" << "Bad reply from dowstats:" << QString::fromLatin1(replyByteArray);
        reply->deleteLater();
        return;
    }

    QJsonArray statsArray = jsonObject.value("stats").toArray();

    for(int i = 0; i < statsArray.count(); i++)
    {
        playersInfo.get()->operator[](i).apm = statsArray.at(i)["apm"].toInt();
        playersInfo.get()->operator[](i).gamesCount = statsArray.at(i)["gamesCount"].toInt();
        playersInfo.get()->operator[](i).mmr = statsArray.at(i)["mmr"].toInt();
        playersInfo.get()->operator[](i).mmr1v1 = statsArray.at(i)["mmr1v1"].toInt();
        playersInfo.get()->operator[](i).isBanned = statsArray.at(i)["isBanned"].toBool();
        playersInfo.get()->operator[](i).name = statsArray.at(i)["name"].toString();
        playersInfo.get()->operator[](i).rank = statsArray.at(i)["rank"].toInt();
        playersInfo.get()->operator[](i).race = statsArray.at(i)["race"].toInt();
        playersInfo.get()->operator[](i).winRate = statsArray.at(i)["winRate"].toInt();
        playersInfo.get()->operator[](i).winsCount = statsArray.at(i)["winsCount"].toInt();
        playersInfo.get()->operator[](i).avatarUrl = statsArray.at(i)["avatarUrl"].toString();
        playersInfo.get()->operator[](i).statsAvailable = true;
        playersInfo.get()->operator[](i).banType = BanType::NotBanned;

        if (!playersInfo.get()->operator[](i).isBanned)
            continue;

        QString banType = statsArray.at(i)["banType"].toString();

        if (banType == "former_cheater")
            playersInfo.get()->operator[](i).banType = BanType::FormerCheater;
        if (banType == "banned")
            playersInfo.get()->operator[](i).banType = BanType::Banned;
        if (banType == "software_use")
            playersInfo.get()->operator[](i).banType = BanType::SoftwareUseBan;
        if (banType == "cheater")
            playersInfo.get()->operator[](i).banType = BanType::Cheater;

        m_softwareBanActivated = playersInfo.get()->operator[](i).isCurrentPlayer && playersInfo.get()->operator[](i).banType == BanType::SoftwareUseBan;

        if (m_softwareBanActivated)
            emit sendSoftwareBanActivated(statsArray.at(i)["banReason"].toString());
    }

    getPlayersMediumAvatar(playersInfo);
    reply->deleteLater();
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
    }
}

void StatsServerProcessor::receivePlayerMediumAvatar(QNetworkReply *reply, QSharedPointer <ServerPlayerStats> playerInfo)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "StatsServerProcessor::receivePlayerMediumAvatar:" << "Connection error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray replyByteArray = reply->readAll();
    QImage avatarMedium = QImage::fromData(replyByteArray);

    reply->deleteLater();

    if (avatarMedium.isNull())
        return;

    playerInfo->avatarAvailable = true;
    playerInfo->avatar = avatarMedium;

    if (playerInfo->avatarAvailable && playerInfo->statsAvailable)
        emit sendServerPlayerStats(*playerInfo.data());
}

void StatsServerProcessor::currentPlayerStatsRequestTimerTimeout()
{
    if (!m_currentPlayerStats.data()->isEmpty() && !m_currentPlayerStats.data()->at(0).steamId.isEmpty())
        getPlayerStatsFromServer(m_currentPlayerStats);
}

void StatsServerProcessor::onSettingsLoaded()
{
    qInfo(logInfo()) << "StatsServerProcessor::onSettingsLoaded()" << "load started";

    m_currentMod = m_settingsController->getSettings()->currentMod;

    if (m_currentMod.contains("dowstats_balance_mod"))
        m_currentMod = "dowstats_balance_mod";

    parseCurrentPlayerSteamId();
    m_currentPlayerStatsRequestTimer->start();

    requestRankDiversion();
    m_rankDiversionTimer->start();

    qInfo(logInfo()) << "StatsServerProcessor::onSettingsLoaded()" << "load finished";
}

void StatsServerProcessor::receiveRankDiversion(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "StatsServerProcessor::receiveRankDiversion:" << "Connection error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray replyByteArray = reply->readAll();
    reply->deleteLater();

    RankDiversion newRankDiversion;

    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyByteArray);

    if (!jsonDoc.isObject())
        return;

    QJsonObject jsonObject = jsonDoc.object();

    if (!jsonObject.value("ranks").isObject())
        return;

    QJsonObject ranksObject = jsonObject.value("ranks").toObject();

    if (!ranksObject.value("7").isObject())
        return;

    QJsonObject rank7Object = ranksObject.value("7").toObject();

    newRankDiversion.rank7Min = rank7Object.value("min").toInt();
    newRankDiversion.rank7Max = rank7Object.value("max").toInt();

    if (!ranksObject.value("6").isObject())
        return;

    QJsonObject rank6Object = ranksObject.value("6").toObject();
    newRankDiversion.rank6Min = rank6Object.value("min").toInt();
    newRankDiversion.rank6Max = rank6Object.value("max").toInt();

    if (!ranksObject.value("5").isObject())
        return;

    QJsonObject rank5Object = ranksObject.value("5").toObject();
    newRankDiversion.rank5Min = rank5Object.value("min").toInt();
    newRankDiversion.rank5Max = rank5Object.value("max").toInt();

    if (!ranksObject.value("4").isObject())
        return;

    QJsonObject rank4Object = ranksObject.value("4").toObject();
    newRankDiversion.rank4Min = rank4Object.value("min").toInt();
    newRankDiversion.rank4Max = rank4Object.value("max").toInt();

    if (!ranksObject.value("3").isObject())
        return;

    QJsonObject rank3Object = ranksObject.value("3").toObject();
    newRankDiversion.rank3Min = rank3Object.value("min").toInt();
    newRankDiversion.rank3Max = rank3Object.value("max").toInt();

    if (!ranksObject.value("2").isObject())
        return;

    QJsonObject rank2Object = ranksObject.value("2").toObject();
    newRankDiversion.rank2Min = rank2Object.value("min").toInt();
    newRankDiversion.rank2Max = rank2Object.value("max").toInt();

    if (!ranksObject.value("1").isObject())
        return;

    QJsonObject rank1Object = ranksObject.value("1").toObject();
    newRankDiversion.rank1Min = rank1Object.value("min").toInt();
    newRankDiversion.rank1Max = rank1Object.value("max").toInt();

    emit sendRankDiversion(newRankDiversion);
}

void StatsServerProcessor::onRankDiversionTimerTimeout()
{
    requestRankDiversion();
}

void StatsServerProcessor::requestClientLastVersion()
{
    QUrl url = QUrl(m_settingsController->getSettings()->updateCheckAddress);

    if (!url.isValid())
        return;

    QNetworkRequest newRequest = QNetworkRequest(url);
    newRequest.setRawHeader("key", QString::fromStdString(SERVER_KEY).toLatin1());

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveClientLastVersion(reply);
    });
}

void StatsServerProcessor::receiveClientLastVersion(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "StatsServerProcessor::receiveClientLastVersion:" << "Connection error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray replyByteArray = reply->readAll();
    reply->deleteLater();

    QString versionDoc = QString(replyByteArray);

    if (versionDoc.isEmpty())
        return;

    if (!versionDoc.contains("Version="))
        return;

    if (!versionDoc.contains("URL"))
        return;

    QString version = versionDoc.mid(versionDoc.indexOf("Version=") + 8, versionDoc.indexOf("URL") - 2 - (versionDoc.indexOf("Version=") + 8));

    if(version.isEmpty())
        return;

    emit sendActualClientVersion(version);
}

void StatsServerProcessor::sendReplayToServer(SendingReplayInfo replayInfo)
{
    if (replayInfo.playersInfo.count() < 2 || replayInfo.playersInfo.count() > 8 || m_softwareBanActivated)
        return;

    //Определяем сид для текущего игрока
    for (int i = 0; i < replayInfo.playersInfo.count(); i++)
    {
        if (replayInfo.playersInfo[i].playerName == m_currentPlayerStats.data()->at(0).name ||
            replayInfo.playersInfo[i].playerName == "[" + m_currentPlayerStats.data()->at(0).name + "]" ||
            replayInfo.playersInfo[i].playerName == "[[" + m_currentPlayerStats.data()->at(0).name + "]]"||
            replayInfo.playersInfo[i].playerName == m_currentPlayerStats.data()->at(0).dowServerName ||
            replayInfo.playersInfo[i].playerName == "[" + m_currentPlayerStats.data()->at(0).dowServerName + "]" ||
            replayInfo.playersInfo[i].playerName == "[[" + m_currentPlayerStats.data()->at(0).dowServerName + "]]"
            )
        {
            replayInfo.playersInfo[i].playerSid = m_currentPlayerStats.data()->at(0).steamId;
            replayInfo.playersInfo[i].playerName = m_currentPlayerStats.data()->at(0).dowServerName;
        }

        if (replayInfo.playersInfo[i].playerSid == "")
        {
            qWarning() << "Player" << replayInfo.playersInfo[i].playerName << "does not have a steam id, the replay has not been uploaded to the server";
            emit sendNotification(tr("Player ") + replayInfo.playersInfo[i].playerName + tr(" does not have a steam id, the replay has not been uploaded to the server"), true);
            return;
        }
    }

    QString url;

    //url = QString::fromStdString(SERVER_ADDRESS) + "/api/send_replay.php?";
    //url = QString::fromStdString(SERVER_ADDRESS) + "/api/send_replay2.php?";
    //url = QString::fromStdString(SERVER_ADDRESS) + "/api/send_replay3.php?";
    url = QString::fromStdString(SERVER_ADDRESS) + "/api/send_replay4.php?";

    int winnerCount = 0;

    for(int i = 0; i < replayInfo.playersInfo.count(); i++)
    {
        url += "p" + QString::number(i+1) + "=" + replayInfo.playersInfo.at(i).playerName.toUtf8().toBase64() + "&";
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
    url += "mod_version=" + replayInfo.modVersion + "&";

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

    if (m_rankedMode)
        url += "isRanked=1";
    else
        url += "isRanked=0";

    qInfo(logInfo()) << "Replay sending url:" << url;

   // url += "key=" + QLatin1String(SERVER_KEY);


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
    emit replaySended();


    if (m_rankedMode)
    {
        qInfo(logInfo()) << "The ranked game has been uploaded to the server";
        emit sendNotification(tr("The ranked game has been uploaded to the server"), false);
    }
    else
    {
        qInfo(logInfo()) << "The unranked game has been uploaded to the server";
        emit sendNotification(tr("The unranked game has been uploaded to the server"), false);
    }

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){  
        reply->deleteLater();
    });
}

void StatsServerProcessor::receiveRankedMode(bool reankedMode)
{
    m_rankedMode = reankedMode;
}

void StatsServerProcessor::receiveCurrentMod(QString modName)
{
    m_currentMod = modName;

    if (m_currentMod.contains("dowstats_balance_mod"))
        m_currentMod = "dowstats_balance_mod";

    requestRankDiversion();
    m_rankDiversionTimer->start();
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

void StatsServerProcessor::receiveAuthKey(const QString &newAuthKey)
{
    if(!newAuthKey.isEmpty())
        m_authKey = newAuthKey;
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

void StatsServerProcessor::requestRankDiversion()
{
    QUrl url = QUrl(QString::fromStdString(SERVER_ADDRESS) + "/api/rank-diversion.php?mod_tech_name=" + m_currentMod);

    QNetworkRequest newRequest = QNetworkRequest(url);
    newRequest.setRawHeader("key", QString::fromStdString(SERVER_KEY).toLatin1());

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveRankDiversion(reply);
    });
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
}

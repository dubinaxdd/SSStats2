#include "replayDataCollector.h"
#include <logger.h>
#include <qjsonobject.h>
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QCoreApplication>

using namespace ReplayReader;

ReplayDataCollector::ReplayDataCollector(QObject *parent)
    : QObject(parent)
{
    m_findGameResultsTimer.setSingleShot(true);
    m_findGameResultsTimer.setInterval(5000);

    connect(&m_findGameResultsTimer, &QTimer::timeout, this, [&]{

        if(!m_lastReplayInfo.gameId.isEmpty())
        {

            QFile sourceFile(m_currentGame->gameSettingsPath + "/Playback/temp.rec");
            QFile destFile(m_lastReplayInfo.replayPath);

            if (sourceFile.exists())
            {
                if (destFile.exists())
                    destFile.remove(); // Удалить, если существует, перед копированием

                sourceFile.copy(destFile.fileName());
            }

            emit requestGameResults(m_lastReplayInfo);
            m_lastGameId = "";
            qInfo(logInfo()) << "ReplayDataCollector::readDefinitiveReplayData() Request game results";
        }
    });
}

void ReplayDataCollector::receiveCurrentMissionState(GameMissionState missionCurrentState)
{
    switch (missionCurrentState)
    {
        case GameMissionState::gameStoped : readReplayData(); break;
        case GameMissionState::gameLoadStarted : determinateRankedState(); break;
        default: break;
    }
}


void ReplayDataCollector::readReplayData()
{
    if (m_gameWillBePlayedInOtherSession)
    {
        qWarning(logWarning()) << "Game will be played in other session, replay not sended";
        return;
    }

    m_lastReplayInfo = SendingReplayInfo();
    m_lastReplayInfo.apm = m_lastAverrageApm;
    m_lastReplayInfo.replayPath = getDowstatsTempReplayPath();
    m_lastReplayInfo.gameId = m_lastGameId;
    m_lastReplayInfo.mod = m_currentMode;
    m_lastReplayInfo.modVersion = "Game version: " + m_currentGameVersion + "; Mod version: " + m_currentModVerion;
    m_lastReplayInfo.isRnked = m_rankedState;


    if (m_currentGame->gameType == GameType::GameTypeEnum::DefinitiveEdition)
        readDefinitiveReplayData();
    else
        readSoulstormReplayData(m_lastReplayInfo);
}

Race ReplayDataCollector::getRaceByNumber(int number)
{
    switch(number)
    {
        case 0: return Race::ChaosMarines;
        case 1: return Race::DarkEldar;
        case 2: return Race::Eldar;
        case 3: return Race::ImperialGuard;
        case 4: return Race::Necrons;
        case 5: return Race::Orks;
        case 6: return Race::SistersOfBattle;
        case 7: return Race::SpaceMarines;
        case 8: return Race::TauEmpire;
        default: return Race::SpaceMarines;
    }
}

bool ReplayDataCollector::readSoulstormReplayData(SendingReplayInfo replayInfo)
{
    m_testStatsPath = updateTestStatsFilePath();

    qInfo(logInfo()) << "teststats.lua path: " << m_testStatsPath;

    QFile file(m_testStatsPath);

    if(!file.open(QIODevice::ReadOnly))
        return false;
    if(!file.isReadable())
        return false;

    // в начале файла лежат байты из-за этого корневой ключ может не читаться
    int k=0;
    while(file.read(1)!="G")
        k++;
    file.seek(k);

    //QByteArray testStats = file.readAll();

    QTextStream textStream(&file);
    QStringList fileLines = textStream.readAll().split("\r");

    file.close();

    int playersCount = 0;
    QString winBy;
    int teamsCount = 0;
    int duration = 0;
    QString scenario;

    QStringList playerNames;
    QStringList playerRaces;
    QStringList playerTeams;
    QList<int> playerFinalStates;
    QList<bool> playerHumanFlags;

    for(int i = 0; i < fileLines.size(); i++ )
    {
        if (fileLines[i].contains("Players"))
        {
            QString temp = fileLines[i].right(fileLines[i].length() - 12);
            temp = temp.left(temp.length() - 1);
            playersCount = temp.toInt();
        }

        if (fileLines[i].contains("WinBy"))
        {
            QString temp = fileLines[i].right(fileLines[i].length() - 11);
            winBy = temp.left(temp.length() - 2);
        }

        if (fileLines[i].contains("Teams"))
        {
            QString temp = fileLines[i].right(fileLines[i].length() - 10);
            temp = temp.left(temp.length() - 1);
            teamsCount = temp.toInt();
        }

        if (fileLines[i].contains("PFnlState"))
        {
            QString temp = fileLines[i].right(fileLines[i].length() - 15);
            temp = temp.left(temp.length() - 1);
            playerFinalStates.append(temp.toInt());
        }

        if (fileLines[i].contains("Duration"))
        {
            QString temp = fileLines[i].right(fileLines[i].length() - 13);
            temp = temp.left(temp.length() - 1);
            duration = temp.toInt();
        }


        if (fileLines[i].contains("PName"))
        {
            QString name = fileLines[i].right(fileLines[i].length() - 12);
            name = name.left(name.length() - 2);
            playerNames.append(name);
        }

        if (fileLines[i].contains("PRace"))
        {
            QString race = fileLines[i].right(fileLines[i].length() - 12);
            race = race.left(race.length() - 2);
            playerRaces.append(race);
        }


        if (fileLines[i].contains("PTeam"))
        {
            QString team = fileLines[i].right(fileLines[i].length() - 11);
            team = team.left(team.length() - 1);
            playerTeams.append(team);
        }

        if (fileLines[i].contains("PHuman"))
        {
            QString playerHumanFlag = fileLines[i].right(fileLines[i].length() - 12);
            playerHumanFlag = playerHumanFlag.left(playerHumanFlag.length() - 1);
            playerHumanFlags.append(playerHumanFlag.toInt());
        }

        if (fileLines[i].contains("Scenario"))
        {
            QString temp = fileLines[i].right(fileLines[i].length() - 14);
            scenario = temp.left(temp.length() - 2);
        }
    }

    QString warning = tr("    The replay has not been uploaded to the server!") + "\n";

    bool checkFailed = false;

    bool haveEqualNames = checkEqualNames(&playerNames);

    //Проверка на одинаковые никнеймы
    if (haveEqualNames)
    {
        checkFailed = true;
        warning += "    The players have equal names\n";
    }

    bool haveEqualNamesForStats = false;

    if (!haveEqualNames)
        haveEqualNamesForStats = checkEqualNamesInStats();

    if (haveEqualNamesForStats)
    {
        checkFailed = true;
        warning += tr("    The observer have equal name with other player") + "\n";
    }

    QVector<PlayerStats> playerStats;

    for(int i = 0; i < playersCount; i++ )
    {
        PlayerStats newPlayerStats;
        newPlayerStats.name = playerNames.at(i).toLocal8Bit();
        newPlayerStats.race = playerRaces.at(i);
        newPlayerStats.team = playerTeams.at(i);
        newPlayerStats.pHuman = playerHumanFlags.at(i);
        newPlayerStats.finalState = static_cast<FinalState>(playerFinalStates.at(i));

        playerStats.append(newPlayerStats);
    }

    //Сортировка игроков по команде
    for(int i = 0; i < playerStats.count(); i++)
    {
        for(int j = 0; j < playerStats.count() - 1; j++)
        {
            if(playerStats[j].team > playerStats[j + 1].team)
            {
                auto buffer = playerStats[j];
                playerStats[j] = playerStats[j + 1];
                playerStats[j + 1] = buffer;
            }
        }
    }

    //Проверка на наличие компьютера
    bool computersFinded = checkAi(&playerStats);

    //Проверка на наличие ИИ
    if (computersFinded)
    {
        checkFailed = true;
        warning += tr("    There was AI in the game") + "\n";
    }

    //Выводим информацию об игре
    qInfo(logInfo()) << "Players count:" << playersCount;
    qInfo(logInfo()) << "Computers finded:" << computersFinded;
    qInfo(logInfo()) << "Teams count:" << teamsCount;
    qInfo(logInfo()) << "Duration:" << duration;
    qInfo(logInfo()) << "Win by:" << winBy;
    qInfo(logInfo()) << "Scenario:" << scenario;
    qInfo(logInfo()) << "APM:" << m_lastAverrageApm;


    bool isStdWinConditions = m_winCoditionsVector.contains( WinCondition::ANNIHILATE)
                              && !m_winCoditionsVector.contains( WinCondition::ASSASSINATE)
                              && !m_winCoditionsVector.contains( WinCondition::DESTROYHQ)
                              && !m_winCoditionsVector.contains( WinCondition::ECONOMICVICTORY)
                              && !m_winCoditionsVector.contains( WinCondition::SUDDENDEATH);

    if (!isStdWinConditions)
    {
        checkFailed = true;
        warning += tr("    Standard winning conditions were not set up for the game") + "\n";
    }

    bool isFullStdGame = false;

    if (playersCount == 2)
    {
        //Проверка условий победы для игр 1х1
        isFullStdGame = m_winCoditionsVector.contains( WinCondition::ANNIHILATE)
                        && m_winCoditionsVector.contains( WinCondition::CONTROLAREA)
                        && m_winCoditionsVector.contains( WinCondition::STRATEGICOBJECTIVE)
                        && !m_winCoditionsVector.contains( WinCondition::ASSASSINATE)
                        && !m_winCoditionsVector.contains( WinCondition::DESTROYHQ)
                        && !m_winCoditionsVector.contains( WinCondition::ECONOMICVICTORY)
                        && !m_winCoditionsVector.contains( WinCondition::SUDDENDEATH);
    }


    //Выводим в лог информацию о состоянии игрока
    for(int i = 0; i < playerStats.count(); i++)
    {
        qInfo(logInfo()) << "Player name:" << playerStats.at(i).name;

        switch (playerStats.at(i).finalState)
        {
        case inGame: qInfo(logInfo()) << "State In Game"; break;
        case winner: qInfo(logInfo()) << "State Winner";  break;
        case loser: qInfo(logInfo()) << "State Looser";   break;
        default: qInfo(logInfo()) << "State Unknown";     break;
        }
    }

    //Проверка на количество команд
    if (teamsCount > 2)
    {
        checkFailed = true;
        warning += tr("    There were more than two teams in the game") + "\n";
    }

    //Проверка на равенство команд
    QMap<QString, int> teamsCounter;

    for (int i = 0; i < playerStats.count(); i++)
    {
        if (teamsCounter.contains(playerStats.at(i).team))
            teamsCounter.insert(playerStats.at(i).team, teamsCounter.value(playerStats.at(i).team) + 1);
        else
            teamsCounter.insert(playerStats.at(i).team, 1);
    }

    int count = 0;

    if (teamsCounter.count() > 0)
        count = teamsCounter.first();

    else
    {
        checkFailed = true;
        warning += tr("    Team identification failure occured") + "\n";
    }

    for (int i = 0; i < teamsCounter.count(); i++)
    {
        if (teamsCounter.values().at(i) != count)
        {
            checkFailed = true;
            warning += tr("    Teams didn't have an equal number of players") + "\n";
        }
    }

    //Проверка на длительность игры
    if(duration <= 45)
    {
        checkFailed = true;
        warning += tr("    Game lasted less than 45 seconds") + "\n";
    }

    //Проверка на наличие победителя
    bool winnerAccepted = checkWinner(&playerStats);

    if (!winnerAccepted)
    {
        checkFailed = true;
        warning += tr("    No game winner has been determined") + "\n";
    }

    //Отправка реплея
    for(int i = 0; i < playersCount; i++)
    {
        PlayerInfoForReplaySending newPlayer;
        newPlayer.playerName = playerStats[i].name;

        //Выставляем сиды для всех игроков, кроме текущего игрока, определение сида для текущего игрока происходит в statsServerProcessor
        for(int j = 0; j < m_playersInfoFromDowServer.count(); j++)
        {
            if(m_playersInfoFromDowServer[j].isCurrentPlayer)
                continue;

            if(newPlayer.playerName == m_playersInfoFromDowServer[j].name ||
                newPlayer.playerName == "[" + m_playersInfoFromDowServer[j].name + "]"  ||
                newPlayer.playerName == "[[" + m_playersInfoFromDowServer[j].name + "]]"
                )
            {
                newPlayer.playerSid = m_playersInfoFromDowServer[j].steamId;
                newPlayer.playerName = m_playersInfoFromDowServer[j].name;
                break;
            }
        }

        if (playerStats[i].race == "guard_race")
            newPlayer.playerRace = Race::ImperialGuard;
        if (playerStats[i].race == "tau_race")
            newPlayer.playerRace = Race::TauEmpire;
        if (playerStats[i].race == "ork_race")
            newPlayer.playerRace = Race::Orks;
        if (playerStats[i].race == "chaos_marine_race")
            newPlayer.playerRace = Race::ChaosMarines;
        if (playerStats[i].race == "necron_race")
            newPlayer.playerRace = Race::Necrons;
        if (playerStats[i].race == "space_marine_race")
            newPlayer.playerRace = Race::SpaceMarines;
        if (playerStats[i].race == "sisters_race")
            newPlayer.playerRace = Race::SistersOfBattle;
        if (playerStats[i].race == "dark_eldar_race")
            newPlayer.playerRace = Race::DarkEldar;
        if (playerStats[i].race == "eldar_race")
            newPlayer.playerRace = Race::Eldar;

        newPlayer.isWinner = playerStats[i].finalState == FinalState::winner;

        replayInfo.playersInfo.append(newPlayer);
    }

    //replayInfo.apm = m_lastAverrageApm;

    switch (playersCount)
    {
    case 1: return false;
    case 2: replayInfo.gameType = GameTypeForReplaySending::GameType1x1; break;
    case 3: return false;
    case 4: replayInfo.gameType = GameTypeForReplaySending::GameType2x2; break;
    case 5: return false;
    case 6: replayInfo.gameType = GameTypeForReplaySending::GameType3x3; break;
    case 7: return false;
    case 8: replayInfo.gameType = GameTypeForReplaySending::GameType4x4; break;
    }

    bool lastGameSettingsValide = checkMissionSettingsValide(replayInfo.gameType);

    if (!lastGameSettingsValide)
    {
        checkFailed = true;
        warning += tr("    Game settings not valide") + "\n";
    }

    if (checkFailed)
    {
        emit sendNotification(warning, true);
        qWarning(logWarning()) << warning;
        return false;
    }

    replayInfo.mapName = scenario;
    replayInfo.gameTime = duration;
    replayInfo.isFullStdGame = isFullStdGame;

    if (winBy == "ANNIHILATE" || winBy == "annihilate" )
        replayInfo.winBy = WinCondition::ANNIHILATE;
    if (winBy == "CONTROLAREA")
        replayInfo.winBy = WinCondition::CONTROLAREA;
    if (winBy == "STRATEGICOBJECTIVE")
        replayInfo.winBy = WinCondition::STRATEGICOBJECTIVE;

    emit sendReplayToServer(std::move(replayInfo));

    //m_allPlayersInfoFromScanner.clear();
    //qInfo(logInfo()) << "Players history cleared";

    qInfo(logInfo()) << "Readed played game settings";

    return true;
}

void ReplayDataCollector::readDefinitiveReplayData()
{
    m_findGameResultsTimer.start();
}

void ReplayDataCollector::receiveAverrageApm(int apm)
{
    m_lastAverrageApm = apm;
}

bool ReplayDataCollector::checkEqualNames(QStringList *playerNames)
{
    bool haveEqualNames = false;

    for (int i = 0; i < playerNames->count(); i++ )
    {
        for (int j = 0; j < playerNames->count(); j++ )
        {
            if (i == j)
                continue;

            if (playerNames->at(i) == playerNames->at(j))
            {
                haveEqualNames = true;
                break;
            }
        }

        if (haveEqualNames)
            break;
    }

    return haveEqualNames;
}

bool ReplayDataCollector::checkAi(QVector<PlayerStats> *playerStats)
{
    bool computersFinded = false;

    for(int i = 0; i < playerStats->count(); i++ )
    {
        if (playerStats->at(i).pHuman == false)
        {
            computersFinded = true;
            break;
        }
    }

    return computersFinded;
}

bool ReplayDataCollector::checkWinner(QVector<PlayerStats> *playerStats)
{
    bool winnerAccepted = false;

    for(int i = 0; i < playerStats->count(); i++)
    {
        if(playerStats->at(i).finalState == FinalState::winner)
        {
            winnerAccepted = true;
            break;
        }
    }

    return winnerAccepted;
}

bool ReplayDataCollector::checkEqualNamesInStats()
{
    bool haveEqualNames = false;

    for (int i = 0; i < m_playersInfoFromDowServer.count(); i++ )
    {
        for (int j = 0; j < m_playersInfoFromDowServer.count(); j++ )
        {
            if (i == j)
                continue;

            if (m_playersInfoFromDowServer.at(i).name == m_playersInfoFromDowServer.at(j).name)
            {
                haveEqualNames = true;
                break;
            }
        }

        if (haveEqualNames)
            break;
    }

    return haveEqualNames;
}

void ReplayDataCollector::receivePlayresInfoFromDowServer(QList<PlayerInfoFromDowServer> playersInfoFromDowServer)
{   
    m_playersInfoFromDowServer = playersInfoFromDowServer;

    qInfo(logInfo()) << "ReplayDataCollector::receivePlayresInfoFromDowServer" << "players info received";
}

void ReplayDataCollector::onQuitParty()
{
    m_playersInfoFromDowServer.clear();
}

void ReplayDataCollector::receiveCurrentMod(QString currentMode)
{
    m_currentMode = currentMode;

    if(m_currentMode.toLower().contains("dowstats_balance_mod"))
        m_currentMode = "dowstats_balance_mod";
}

void ReplayDataCollector::receiveCurrentModVersion(QString version)
{
    m_currentModVerion = version;
}

void ReplayDataCollector::receiveCurrentGameVersion(QString version)
{
    m_currentGameVersion = version;
}

void ReplayDataCollector::receiveCurrentWinConditions(QVector<WinCondition> winConditions)
{
    m_winCoditionsVector = winConditions;

    if (winConditions.count() > 0)
        m_gameWillBePlayedInOtherSession = false;
}

bool ReplayDataCollector::checkMissionSettingsValide(int gameType)
{
    RepReader repReader(m_currentGame->gameSettingsPath + "/Playback/temp.rec");
    return repReader.isStandart(gameType);
}

QString ReplayDataCollector::updateTestStatsFilePath()
{
    QDir dir(m_currentGame->gameSettingsPath + "\\Profiles\\");

    QFileInfoList dirContent = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    QString statsPath;
    QDateTime fileModified;

    for (int i = 0; i < dirContent.count(); i++)
    {

        QDir path(dirContent.at(i).absoluteFilePath() + "\\teststats.lua");

        QFileInfo info(path.absolutePath());

        if (info.isFile() && info.lastModified() > fileModified)
        {
            fileModified = info.lastModified();
            statsPath = path.absolutePath();
        }
    }

    return statsPath;
}

void ReplayDataCollector::parseGameResults(QJsonObject gameResults,  SendingReplayInfo lastGameResults)
{
    QJsonArray playersJsonArray = gameResults.value("matchhistoryreportresults").toArray();

    lastGameResults.isAutomatch = gameResults.value("description").toString() == "AUTOMATCH";

    QVector<int> teams;
    bool winnerAccepted = false;

    for(const auto &item : qAsConst(playersJsonArray))
    {
        PlayerInfoForReplaySending player;
        QJsonObject playerJson = item.toObject();

        player.relicId = QString::number(playerJson.value("profile_id").toInt());
        player.playerRace = getRaceByNumber(playerJson.value("race_id").toInt());
        player.playerType = 0;
        player.playerTeam = playerJson.value("teamid").toInt();
        player.isWinner = playerJson.value("resulttype").toInt() > 0;


        if (!teams.contains(player.playerTeam))
            teams.append(player.playerTeam);

        if (player.isWinner)
            winnerAccepted = true;

        bool playerFindedInDowPlayersArray = false;
        for(auto& playerFromDowServer : lastGameResults.playersInfoFromDowServer)
        {
            if (player.relicId == playerFromDowServer.playerID)
            {
                player.playerName = playerFromDowServer.name;
                player.playerSid = playerFromDowServer.steamId;
                playerFindedInDowPlayersArray = true;
                break;
            }
        }

        if (!playerFindedInDowPlayersArray)
        {
            qWarning(logWarning()) << "ReplayDataCollector::receiveGameResults - player not finded in dowServerPlayersArray" << player.relicId;
            return;
        }

        lastGameResults.playersInfo.append(player);

        qInfo(logInfo()) << "Parsed player info";
        qInfo(logInfo()) << "Player Name" << player.playerName;
        qInfo(logInfo()) << "Player relicId" << player.relicId;
        qInfo(logInfo()) << "Player sid" << player.playerSid;
        qInfo(logInfo()) << "Player Race" << player.playerRace;
        qInfo(logInfo()) << "Is Winner" << player.isWinner;
        qInfo(logInfo()) << "Player type" << player.playerType;
        qInfo(logInfo()) << "Player team" << player.playerTeam;
    }

    //Сортировка игроков по победе
    std::sort(lastGameResults.playersInfo.begin(), lastGameResults.playersInfo.end(), [](PlayerInfoForReplaySending &a, PlayerInfoForReplaySending &b){
        return a.isWinner > b.isWinner;
    });

    QString warning = tr("    The replay has not been uploaded to the server!") + "\n";
    bool checkFailed = false;

    RepReader repReader(lastGameResults.replayPath);
    lastGameResults.mapName = repReader.replay.Map;
    lastGameResults.gameTime = repReader.replay.Duration;

    //Проверка на наличие ИИ

    bool computersFinded = false;
    for(auto& item : repReader.replay.Players)
    {
        if (item->Type == 1 || item->Type == 3 || item->Type == 11){
            computersFinded = true;
            break;
        }
    }

    if (computersFinded)
    {
        checkFailed = true;
        warning += tr("    There was AI in the game") + "\n";
    }

    //Выводим информацию об игре
    qInfo(logInfo()) << "Players count:" << lastGameResults.playersInfo.count();
    qInfo(logInfo()) << "Computers finded:" << computersFinded;
    qInfo(logInfo()) << "Teams count:" << teams.count();
    qInfo(logInfo()) << "Duration:" << lastGameResults.gameTime;
    qInfo(logInfo()) << "Scenario:" << lastGameResults.mapName;
    qInfo(logInfo()) << "APM:" << m_lastAverrageApm;


    bool isStdWinConditions = m_winCoditionsVector.contains( WinCondition::ANNIHILATE)
                              && !m_winCoditionsVector.contains( WinCondition::ASSASSINATE)
                              && !m_winCoditionsVector.contains( WinCondition::DESTROYHQ)
                              && !m_winCoditionsVector.contains( WinCondition::ECONOMICVICTORY)
                              && !m_winCoditionsVector.contains( WinCondition::SUDDENDEATH);

    if (!isStdWinConditions)
    {
        checkFailed = true;
        warning += tr("    Standard winning conditions were not set up for the game") + "\n";
    }

    bool isFullStdGame = false;

    if (lastGameResults.playersInfo.count() == 2)
    {
        //Проверка условий победы для игр 1х1
        isFullStdGame = m_winCoditionsVector.contains( WinCondition::ANNIHILATE)
                        && m_winCoditionsVector.contains( WinCondition::CONTROLAREA)
                        && m_winCoditionsVector.contains( WinCondition::STRATEGICOBJECTIVE)
                        && !m_winCoditionsVector.contains( WinCondition::ASSASSINATE)
                        && !m_winCoditionsVector.contains( WinCondition::DESTROYHQ)
                        && !m_winCoditionsVector.contains( WinCondition::ECONOMICVICTORY)
                        && !m_winCoditionsVector.contains( WinCondition::SUDDENDEATH);
    }


    //Проверка на количество команд
    if (teams.count() > 2)
    {
        checkFailed = true;
        warning += tr("    There were more than two teams in the game") + "\n";
    }

    //Проверка на равенство команд
    QMap<int, int> teamsCounter;

    for (int i = 0; i < lastGameResults.playersInfo.count(); i++)
    {
        int playerTeam = lastGameResults.playersInfo.at(i).playerTeam;

        if (teamsCounter.contains(playerTeam))
            teamsCounter.insert(playerTeam, teamsCounter.value(playerTeam) + 1);
        else
            teamsCounter.insert(playerTeam, 1);
    }

    int count = 0;

    if (teamsCounter.count() > 0)
        count = teamsCounter.first();
    else
    {
        checkFailed = true;
        warning += tr("    Team identification failure occured") + "\n";
    }

    for (int i = 0; i < teamsCounter.count(); i++)
    {
        if (teamsCounter.values().at(i) != count)
        {
            checkFailed = true;
            warning += tr("    Teams didn't have an equal number of players") + "\n";
        }
    }

    //Проверка на длительность игры
    if(repReader.replay.Duration <= 45)
    {
        checkFailed = true;
        warning += tr("    Game lasted less than 45 seconds") + "\n";
    }

    //Проверка на наличие победителя
    if (!winnerAccepted)
    {
        checkFailed = true;
        warning += tr("    No game winner has been determined") + "\n";
    }

    //Отправка реплея
    lastGameResults.apm = m_lastAverrageApm;

    switch (lastGameResults.playersInfo.count())
    {
    case 2: lastGameResults.gameType = GameTypeForReplaySending::GameType1x1; break;
    case 4: lastGameResults.gameType = GameTypeForReplaySending::GameType2x2; break;
    case 6: lastGameResults.gameType = GameTypeForReplaySending::GameType3x3; break;
    case 8: lastGameResults.gameType = GameTypeForReplaySending::GameType4x4; break;
    default: return;
    }

    bool lastGameSettingsValide = repReader.isStandart(lastGameResults.gameType);

    if (!lastGameSettingsValide)
    {
        checkFailed = true;
        warning += tr("    Game settings not valide") + "\n";
    }

    if (checkFailed)
    {
        emit sendNotification(warning, true);
        qWarning(logWarning()) << warning;
        return;
    }


    lastGameResults.isFullStdGame = isFullStdGame;
    //анигиляция, потому что невозможно определить условие победы
    lastGameResults.winBy = WinCondition::ANNIHILATE;

    emit sendReplayToServer(std::move(lastGameResults));

    qInfo(logInfo()) << "Readed played game settings";
}

void ReplayDataCollector::determinateRankedState()
{
    if(m_currentGame->gameType != GameType::GameTypeEnum::DefinitiveEdition)
        return;

    bool ranked = false;

    if (m_playersInfoFromDowServer.isEmpty() || m_plyersRankedState.isEmpty())
        ranked = true;

    for (auto& player : m_playersInfoFromDowServer)
    {
        bool finded = false;
        bool playerisRanked = false;

        for (auto& item : m_plyersRankedState){
            if(player.steamId == item.steamId)
            {
                finded = true;
                playerisRanked = item.isRanked;
                break;
            }
        }

        if (finded && playerisRanked)
        {
            ranked = true;
            break;
        }
        else
        {
            if(!finded)
            {
                ranked = true;
                break;
            }
        }
    }

    setRankedState(ranked);
    emit sendRankedState(ranked);

}

QString ReplayDataCollector::getDowstatsTempReplayPath()
{
    return QCoreApplication::applicationDirPath() + "\\dowstatstemp.rec";
}

void ReplayDataCollector::setRankedState(bool newRankedState)
{
    m_rankedState = newRankedState;
}

void ReplayDataCollector::receivePlyersRankedState(QVector<PlyersRankedState> plyersRankedState)
{
    m_plyersRankedState = plyersRankedState;
}

void ReplayDataCollector::receiveGameResults(QJsonObject gameResults, SendingReplayInfo lastGameResult)
{
    parseGameResults(gameResults, lastGameResult);
}

void ReplayDataCollector::setGameId(QString gameId)
{
    m_lastGameId = gameId;
}

void ReplayDataCollector::setCurrentGame(GamePath *newCurrentGame)
{
    m_currentGame = newCurrentGame;
    QFile::remove(getDowstatsTempReplayPath());
}

#include "replayDataCollector.h"
#include <logger.h>
#include <repreader.h>
#include <QDir>

using namespace ReplayReader;

ReplayDataCollector::ReplayDataCollector(QString ssPath, QObject *parent)
    : QObject(parent)
    , m_ssPath(ssPath)
{

}

void ReplayDataCollector::receiveCurrentMissionState(SsMissionState missionCurrentState)
{
    switch (missionCurrentState)
    {
        case SsMissionState::gameStoped : readReplayData(); break;
        default: break;
    }
}


bool ReplayDataCollector::readReplayData()
{
    if (m_gameWillBePlayedInOtherSession)
    {
        qWarning(logWarning()) << "Game will be played in other session, replay not sended";
        return false;
    }
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

    QString warning = "    The replay has not been uploaded to the server!\n";

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
        warning += "    The observer have equal name with other player\n";
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
        warning += "    There was AI in the game\n";
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
        warning += "    Standard winning conditions were not set up for the game\n";
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
        warning += "    There were more than two teams in the game\n";
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
        warning += "    Team identification failure occured\n";
    }

    for (int i = 0; i < teamsCounter.count(); i++)
    {
        if (teamsCounter.values().at(i) != count)
        {
            checkFailed = true;
            warning += "    Teams didn't have an equal number of playersn";
        }
    }

    //Проверка на длительность игры
    if(duration <= 45)
    {
        checkFailed = true;
        warning += "    Game lasted less than 45 seconds\n";
    }

    //Проверка на наличие победителя
    bool winnerAccepted = checkWinner(&playerStats);

    if (!winnerAccepted)
    {
        checkFailed = true;
        warning += "    No game winner has been determined\n";
    }

    //Отправка реплея
    SendingReplayInfo replayInfo;

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

    replayInfo.apm = m_lastAverrageApm;

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
        warning += "    Game settings not valide\n";
    }

    if (checkFailed)
    {
        emit sendNotification(warning, true);
        qWarning(logWarning()) << warning;
        return false;
    }

    replayInfo.mapName = scenario;
    replayInfo.gameTime = duration;

    QString modName = m_currentMode;

    if(modName.contains("dowstats_balance_mod"))
        modName = "dowstats_balance_mod";
    replayInfo.modVersion = m_currentModVerion;
    replayInfo.mod = modName;
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
}

void ReplayDataCollector::receiveCurrentModVersion(QString version)
{
    m_currentModVerion = version;
}

void ReplayDataCollector::receiveCurrentWinConditions(QVector<WinCondition> winConditions)
{
    m_winCoditionsVector = winConditions;

    if (winConditions.count() > 0)
        m_gameWillBePlayedInOtherSession = false;
}

bool ReplayDataCollector::checkMissionSettingsValide(int gameType)
{
    RepReader repReader(m_ssPath+"/Playback/temp.rec");

    return repReader.isStandart(gameType);
}

QString ReplayDataCollector::updateTestStatsFilePath()
{
    QDir dir(m_ssPath + "\\Profiles\\");

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

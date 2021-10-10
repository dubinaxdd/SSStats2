#define GAME_INFO_READER_TIMER_INTERVAL 1000

#include "gameinforeader.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

GameInfoReader::GameInfoReader(QString sspath, QObject *parent)
    : QObject(parent)
    , m_ssPath(sspath)

{
    m_gameInfoReadTimer = new QTimer();
    m_gameInfoReadTimer->setInterval(GAME_INFO_READER_TIMER_INTERVAL);
    QObject::connect(m_gameInfoReadTimer, &QTimer::timeout, this, &GameInfoReader::readGameInfo, Qt::QueuedConnection );
    m_gameInfoReadTimer->start();
}

void GameInfoReader::readGameInfo()
{
    if (!m_gameLounched)
        return;

    QFile file(m_ssPath+"\\warnings.log");

    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream textStream(&file);
        QStringList fileLines = textStream.readAll().split("\r");

        int counter = fileLines.size();

        while (counter!=0)
        {
            QString line = fileLines.at(counter-1);

            ///Проверка на выключение соулсторма
            if(line.contains("GAME -- Shutdown quit") || line.contains("MOD -- Shutting down Mod"))
            {
                if (m_ssCurrentState != SsState::ssShutdowned)
                {
                    m_gameCurrentState = SsGameState::gameStoped;
                    m_ssCurrentState = SsState::ssShutdowned;
                    emit gameStopped();
                    emit ssShutdown();
                    qInfo(logInfo()) << "SS Shutdown";
                }
                break;
            }

            ///Проверка на окончание инициализации соулсторма
            if(line.contains("MOD -- Initializing Mod"))
            {
                checkGameInitialize();
                break;
            }


            if(line.contains("MOD -- Game Over at frame"))
            {
                if (m_gameCurrentState != SsGameState::gameOver)
                {
                    m_gameCurrentState = SsGameState::gameOver;
                    checkGameInitialize();
                    emit startingMission(m_gameCurrentState);
                    emit gameOver();
                }
                break;
            }

            ///Проверка на окончание игры
            if(line.contains("APP -- Game Stop"))
            {
                if (m_gameCurrentState != SsGameState::gameStoped)
                {
                    checkGameInitialize();
                    readGameParametresAfterStop();
                    m_gameCurrentState = SsGameState::gameStoped;
                    emit gameStopped();

                    qInfo(logInfo()) << "Game Stoped";
                }
                break;
            }

            ///Проверка на старт миссии игры
            if (line.contains("GAME -- Starting mission"))
            {
                if(m_gameCurrentState == SsGameState::gameLoadStarted
                   || m_gameCurrentState == SsGameState::playbackLoadStarted
                   || m_gameCurrentState == SsGameState::savedGameLoadStarted)
                {
                    if(m_gameCurrentState == SsGameState::gameLoadStarted)
                    {
                        m_gameCurrentState = SsGameState::gameStarted;

                        m_winCoditionsVector.clear();

                        int winConditionsReadCounter = counter;
                        QString winConditionsReadLine;

                        while (!winConditionsReadLine.contains("APP -- Game Start"))
                        {
                            winConditionsReadLine = fileLines.at(winConditionsReadCounter-1);

                            if(winConditionsReadLine.contains("MOD -- Loading Win Condition"))
                            {
                                if(winConditionsReadLine.contains("ANNIHILATE")){
                                    m_winCoditionsVector.append(WinCondition::ANNIHILATE);
                                    qInfo(logInfo()) << "Loaded ANNIHILATE";
                                }

                                if(winConditionsReadLine.contains("ASSASSINATE")){
                                    m_winCoditionsVector.append(WinCondition::ASSASSINATE);
                                    qInfo(logInfo()) << "Loaded ASSASSINATE";
                                }

                                if(winConditionsReadLine.contains("CONTROLAREA")){
                                    m_winCoditionsVector.append(WinCondition::CONTROLAREA);
                                    qInfo(logInfo()) << "Loaded CONTROLAREA";
                                }

                                if(winConditionsReadLine.contains("DESTROYHQ")){
                                    m_winCoditionsVector.append(WinCondition::DESTROYHQ);
                                    qInfo(logInfo()) << "Loaded DESTROYHQ";
                                }

                                if(winConditionsReadLine.contains("ECONOMICVICTORY")){
                                    m_winCoditionsVector.append(WinCondition::ECONOMICVICTORY);
                                    qInfo(logInfo()) << "Loaded ECONOMICVICTORY";
                                }

                                if(winConditionsReadLine.contains("GAMETIMER")){
                                    m_winCoditionsVector.append(WinCondition::GAMETIMER);
                                    qInfo(logInfo()) << "Loaded GAMETIMER";
                                }

                                if(winConditionsReadLine.contains("STRATEGICOBJECTIVE")){
                                    m_winCoditionsVector.append(WinCondition::STRATEGICOBJECTIVE);
                                    qInfo(logInfo()) << "Loaded STRATEGICOBJECTIVE";
                                }

                                if(winConditionsReadLine.contains("SUDDENDEATH")){
                                    m_winCoditionsVector.append(WinCondition::SUDDENDEATH);
                                    qInfo(logInfo()) << "Loaded SUDDENDEATH";
                                }
                            }

                            winConditionsReadCounter--;
                        }

                        qInfo(logInfo()) << "Starting game mission";
                    }
                    else if (m_gameCurrentState == SsGameState::playbackLoadStarted)
                    {
                        m_gameCurrentState = SsGameState::playbackStarted;
                        qInfo(logInfo()) << "Starting playback mission";
                    }
                    else if (m_gameCurrentState == SsGameState::savedGameLoadStarted)
                    {
                        m_gameCurrentState = SsGameState::savedGameStarted;
                        qInfo(logInfo()) << "Starting saved game mission";
                    }

                    checkGameInitialize();
                    emit startingMission(m_gameCurrentState);

                }
                else if(m_gameCurrentState != SsGameState::gameStarted
                      && m_gameCurrentState != SsGameState::playbackStarted
                      && m_gameCurrentState != SsGameState::savedGameStarted
                      && m_gameCurrentState != SsGameState::unknownGameStarted
                      && m_gameCurrentState != SsGameState::gameOver)
                {
                    m_gameCurrentState = SsGameState::unknownGameStarted;
                    qInfo(logInfo()) << "Starting unknown mission";

                    checkGameInitialize();
                    emit startingMission(m_gameCurrentState);
                }

                break;
            }


            ///Проверка на старт загрузки игры
            if (line.contains("APP -- Game Start"))
            {
                //Что бы точно понять норм игра это или долбаный реплей чекаем ближайшие строки
                int checkCounter = counter - 1;

                while (checkCounter!=0 && checkCounter != (counter - 10))
                {
                    QString checkLine = fileLines.at(checkCounter-1);

                    ///Проверка на реплей
                    if(checkLine.contains("APP -- Game Playback"))
                    {
                        if (m_gameCurrentState != SsGameState::playbackLoadStarted)
                        {
                            m_gameCurrentState = SsGameState::playbackLoadStarted;
                            checkGameInitialize();
                            emit loadStarted(m_gameCurrentState);
                            qInfo(logInfo()) << "Playback load started";
                        }
                        break;
                    }

                    ///Проверка на загруженную игру
                    if (checkLine.contains("APP -- Game Load"))
                    {
                        if (m_gameCurrentState != SsGameState::savedGameLoadStarted)
                        {
                            m_gameCurrentState = SsGameState::savedGameLoadStarted;
                            checkGameInitialize();
                            emit loadStarted(m_gameCurrentState);
                            qInfo(logInfo()) << "Saved game load started";
                        }
                        break;
                    }

                    checkCounter--;
                }

                if (m_gameCurrentState != SsGameState::savedGameLoadStarted
                    && m_gameCurrentState != SsGameState::playbackLoadStarted
                    && m_gameCurrentState != SsGameState::gameLoadStarted)
                {
                    m_gameCurrentState = SsGameState::gameLoadStarted;
                    checkGameInitialize();
                    emit loadStarted(m_gameCurrentState);
                    qInfo(logInfo()) << "Game load started";
                }
                break;
            }
            counter--;
        }
    }
}

void GameInfoReader::readGameParametresAfterStop()
{
    //if (m_gameCurrentState != SsGameState::gameStarted)
    //    return;

    bool isStdWinConditions = m_winCoditionsVector.contains( WinCondition::ANNIHILATE)
                           && m_winCoditionsVector.contains( WinCondition::CONTROLAREA)
                           && m_winCoditionsVector.contains( WinCondition::STRATEGICOBJECTIVE)
                           && !m_winCoditionsVector.contains( WinCondition::ASSASSINATE)
                           && !m_winCoditionsVector.contains( WinCondition::DESTROYHQ)
                           && !m_winCoditionsVector.contains( WinCondition::ECONOMICVICTORY)
                           && !m_winCoditionsVector.contains( WinCondition::SUDDENDEATH);

    if (!isStdWinConditions)
        return;

    QString statsPath = m_ssPath + "\\Profiles\\" + m_currentProfile + "\\teststats.lua";
    qInfo(logInfo()) << "teststats.lua path: " << statsPath;

    QFile file(statsPath);

    if(!file.open(QIODevice::ReadOnly))
        return;
    if(!file.isReadable())
        return;

    // в начале файла лежат байты из-за этого корневой ключ может не читаться
    int k=0;
    while(file.read(1)!="G")
        k++;
    file.seek(k);

    //QByteArray testStats = file.readAll();

    QTextStream textStream(&file);
    QStringList fileLines = textStream.readAll().split("\r");

    file.close();

    int playersCount;
    QString winBy;
    int teamsCount;
    int duration;
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

    QVector<PlayerStats> playerStats;

    playerStats.resize(playersCount);

    for(int i = 0; i < playerNames.count(); i++ )
        playerStats[i].name = playerNames.at(i).toLocal8Bit();

    for(int i = 0; i < playerRaces.count(); i++ )
        playerStats[i].race = playerRaces.at(i);

    for(int i = 0; i < playerTeams.count(); i++ )
        playerStats[i].team = playerTeams.at(i);

    for(int i = 0; i < playerHumanFlags.count(); i++ )
        playerStats[i].pHuman = playerHumanFlags.at(i);

    for(int i = 0; i < playerFinalStates.count(); i++ )
        playerStats[i].finalState = static_cast<FinalState>(playerFinalStates.at(i));


    bool computersFinded = false;

    for(int i = 0; i < playerNames.count(); i++ )
    {
        if (playerStats[i].pHuman == false)
        {
            computersFinded = true;
            break;
        }
    }

    qInfo(logInfo()) << "Players count:" << playersCount;
    qInfo(logInfo()) << "Computers finded:" << computersFinded;
    qInfo(logInfo()) << "Teams count:" << teamsCount;
    qInfo(logInfo()) << "Duration:" << duration;
    qInfo(logInfo()) << "Win by:" << winBy;
    qInfo(logInfo()) << "Scenario:" << scenario;
    qInfo(logInfo()) << "APM:" << m_lastAverrageApm;

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

    if (computersFinded)
        return;

    if (teamsCount > 2)
        return;

    if(duration <= 30)
        return;

    SendingReplayInfo replayInfo;

    for(int i = 0; i < playersCount; i++)
    {
        PlayerInfoForReplaySendong newPlayer;

        newPlayer.playerName = playerStats[i].name;

        for(int j = 0; j < m_playersInfoFromScanner.count(); j++)
        {
            if(newPlayer.playerName == m_playersInfoFromScanner[j].name)
            {
                newPlayer.playerSid = m_playersInfoFromScanner[j].steamId;
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
        case 1: return;
        case 2: replayInfo.gameType = GameTypeForReplaySending::GameType1x1; break;
        case 3: return;
        case 4: replayInfo.gameType = GameTypeForReplaySending::GameType2x2; break;
        case 5: return;
        case 6: replayInfo.gameType = GameTypeForReplaySending::GameType3x3; break;
        case 7: return;
        case 8: replayInfo.gameType = GameTypeForReplaySending::GameType4x4; break;
    }

    replayInfo.mapName = scenario;
    replayInfo.gameTime = duration;
    replayInfo.mod = m_currentMode;

    if (winBy == "ANNIHILATE")
        replayInfo.winBy = WinCondition::ANNIHILATE;
    if (winBy == "CONTROLAREA")
        replayInfo.winBy = WinCondition::CONTROLAREA;
    if (winBy == "STRATEGICOBJECTIVE")
        replayInfo.winBy = WinCondition::STRATEGICOBJECTIVE;

    emit sendReplayToServer(std::move(replayInfo));

    qInfo(logInfo()) << "Readed played game settings";
}

void GameInfoReader::receiveAverrageApm(int apm)
{
    m_lastAverrageApm = apm;
}

void GameInfoReader::receivePlayresStemIdFromScanner(QList<SearchStemIdPlayerInfo> playersInfoFromScanner, int playersCount)
{
    for (int i = 0; i < playersInfoFromScanner.count();i++)
    {
        bool playerFinded = false;
        for(int j = 0; j < m_playersInfoFromScanner.count(); j++)
        {
            if (playersInfoFromScanner[i].name == m_playersInfoFromScanner[j].name)
            {
                playerFinded = true;
                break;
            }
        }

        if(!playerFinded)
        {
            m_playersInfoFromScanner.append(playersInfoFromScanner[i]);
            qInfo(logInfo()) << "Finded player:" << m_playersInfoFromScanner.last().name;
        }
    }

    m_playersCountFromScanner = playersCount;
}

void GameInfoReader::setGameLounched(bool newGameLounched)
{
    m_gameLounched = newGameLounched;

    if (m_gameLounched)
        readGameInfo();
}

void GameInfoReader::setCurrentProfile(const QString &newCurrentProfile)
{
    m_currentProfile = newCurrentProfile;
}

void GameInfoReader::ssWindowClosed()
{
    m_gameCurrentState = SsGameState::gameStoped;
    m_ssCurrentState = SsState::ssShutdowned;
    emit gameStopped();
    emit ssShutdown();
    qInfo(logInfo()) << "SS Shutdown";
}

bool GameInfoReader::getGameInitialized()
{
    if (m_ssCurrentState == SsState::ssInitialized)
        return true;
    else
        return false;
}

void GameInfoReader::checkGameInitialize()
{
    if(m_ssCurrentState != SsState::ssInitialized)
    {
        qInfo(logInfo()) << "SS Initialized";
        m_ssCurrentState = SsState::ssInitialized;
        emit gameInitialized();

        checkCurrentMode();
    }
}

void GameInfoReader::checkCurrentMode()
{
    QFile file(m_ssPath+"\\warnings.log");

    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream textStream(&file);
        QStringList fileLines = textStream.readAll().split("\r");

        int counter = fileLines.size();

        while (counter!=0)
        {
            QString line = fileLines.at(counter-1);

            ///Дергаем текущий мод
            if(line.contains("MOD -- Initializing Mod"))
            {
                m_currentMode = line.right(line.size() - 39);

                for (int i = 0; i < m_currentMode.size(); i++)
                {
                    if(m_currentMode.at(i) == ',')
                        m_currentMode = m_currentMode.left(i);
                }

                qInfo(logInfo()) << "Current mode:" << m_currentMode;
                break;
            }

            counter--;
        }
    }
}

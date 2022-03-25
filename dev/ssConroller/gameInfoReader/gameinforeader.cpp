#define GAME_INFO_READER_TIMER_INTERVAL 1000
#define RACES_READ_TIMER_INTERVAL 100

#include <gameinforeader.h>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QSettings>
#include <repreader.h>

GameInfoReader::GameInfoReader(QString sspath, QObject *parent)
    : QObject(parent)
    , m_ssPath(sspath)

{

    m_readRacesSingleShootTimer = new QTimer();
    m_readRacesSingleShootTimer->setInterval(RACES_READ_TIMER_INTERVAL);
    QObject::connect(m_readRacesSingleShootTimer, &QTimer::timeout, this, &GameInfoReader::readRacesTimerTimeout, Qt::QueuedConnection );

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
                    ssWindowClosed();
                break;
            }

            ///Проверка на окончание инициализации соулсторма
            if(line.contains("MOD -- Initializing Mod"))
            {
                checkGameInitialize();
                break;
            }

            ///Проверка на достижение условия победы
            if(line.contains("MOD -- Game Over at frame")||line.contains("storing simulation results for match"))
            {
                if (m_gameCurrentState != SsGameState::gameOver)
                {
                    m_gameCurrentState = SsGameState::gameOver;
                    checkGameInitialize();
                    testStatsTemp = QStringList();
                    readRacesTimerTimeout();
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
                    m_gameCurrentState = SsGameState::gameStoped;
                    checkGameInitialize();
                    readTestStatsTemp();
                    readGameParametresAfterStop();
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
                            m_gameWillBePlayed = true;

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
                    testStatsTemp = QStringList();
                    readRacesTimerTimeout();
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
                    testStatsTemp = QStringList();
                    readRacesTimerTimeout();
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
                            m_readRacesSingleShootTimer->start();
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
                            readTestStatsTemp();
                            m_readRacesSingleShootTimer->start();
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
                    m_readRacesSingleShootTimer->start();
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
    if (!m_gameWillBePlayed)
    {
        qWarning(logWarning()) << "Game will be played in other session, replay not sended";
        return;
    }

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


    if (playersCount == 2)
    {

        bool isStdWinConditions = m_winCoditionsVector.contains( WinCondition::ANNIHILATE)
                               && m_winCoditionsVector.contains( WinCondition::CONTROLAREA)
                               && m_winCoditionsVector.contains( WinCondition::STRATEGICOBJECTIVE)
                               && !m_winCoditionsVector.contains( WinCondition::ASSASSINATE)
                               && !m_winCoditionsVector.contains( WinCondition::DESTROYHQ)
                               && !m_winCoditionsVector.contains( WinCondition::ECONOMICVICTORY)
                               && !m_winCoditionsVector.contains( WinCondition::SUDDENDEATH);

        if (!isStdWinConditions)
        {
            qWarning(logWarning()) << "Game have not standard win conditions, replay not sended";
            return;
        }
    }
    else
    {
        bool isStdWinConditions = m_winCoditionsVector.contains( WinCondition::ANNIHILATE)
                               && !m_winCoditionsVector.contains( WinCondition::ASSASSINATE)
                               && !m_winCoditionsVector.contains( WinCondition::DESTROYHQ)
                               && !m_winCoditionsVector.contains( WinCondition::ECONOMICVICTORY)
                               && !m_winCoditionsVector.contains( WinCondition::SUDDENDEATH);

        if (!isStdWinConditions)
        {
            qWarning(logWarning()) << "Game have not standard win conditions, replay not sended";
            return;
        }
    }




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
    {
        qWarning(logWarning()) << "Game have AI, raplay not sended";
        return;
    }

    if (teamsCount > 2)
    {
        qWarning(logWarning()) << "Game have more then 2 teams, raplay not sended";
        return;
    }

    if(duration <= 30)
    {
        qWarning(logWarning()) << "Game have duration < 30 sec, raplay not sended";
        return;
    }

    bool winnerAccepted = false;

    for(int i = 0; i < playersCount; i++)
    {
        if(playerStats[i].finalState == FinalState::winner)
        {
            winnerAccepted = true;
            break;
        }
    }

    if (!winnerAccepted)
    {
        qWarning(logWarning()) << "Game not have winner, raplay not sended";
        return;
    }


    SendingReplayInfo replayInfo;

    for(int i = 0; i < playersCount; i++)
    {

        PlayerInfoForReplaySendong newPlayer;

        newPlayer.playerName = playerStats[i].name;

        //Берем сиды из последнего скана
        for(int j = 0; j < m_playersInfoFromScanner.count(); j++)
        {
            if(newPlayer.playerName == m_playersInfoFromScanner[j].name)
            {
                newPlayer.playerSid = m_playersInfoFromScanner[j].steamId;
                break;
            }
        }

        //Если сида не нашлось в последнем скане, берем сид из всей истории
        if (newPlayer.playerSid == "")
        {
            for(int j = 0; j < m_allPlayersInfoFromScanner.count(); j++)
            {
                if(newPlayer.playerName == m_allPlayersInfoFromScanner[j].name)
                {
                    newPlayer.playerSid = m_allPlayersInfoFromScanner[j].steamId;
                    break;
                }
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

    m_lastGameSettingsValide = checkMissionSettingsValide(replayInfo.gameType);

    qInfo(logInfo()) << "Game settings valide:" << m_lastGameSettingsValide;

    if (!m_lastGameSettingsValide)
        return;

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

    m_allPlayersInfoFromScanner.clear();
    qInfo(logInfo()) << "Players history cleared";

    qInfo(logInfo()) << "Readed played game settings";
}

void GameInfoReader::readRacesTimerTimeout()
{
    m_readRacesSingleShootTimer->stop();

    QString statsPath = m_ssPath + "\\Profiles\\" + m_currentProfile + "\\teststats.lua";

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

    QTextStream textStream(&file);
    QStringList fileLines = textStream.readAll().split("\r");

    file.close();

    if (testStatsTemp == fileLines)
    {
        m_readRacesSingleShootTimer->start();
        return;
    }

    testStatsTemp = fileLines;

    QStringList playerNames;
    QStringList playerRaces;
    QStringList playerTeam;

    for(int i = 0; i < fileLines.size(); i++ )
    {
        if (fileLines[i].contains("PName")){

            QString name = fileLines[i].right(fileLines[i].length() - 12);
            name = name.left(name.length() - 2);
            playerNames.append(name);
        }

        if (fileLines[i].contains("PRace")){

            QString race = fileLines[i].right(fileLines[i].length() - 12);
            race = race.left(race.length() - 2);
            playerRaces.append(race);
        }


        if (fileLines[i].contains("PTeam")){

            QString team = fileLines[i].right(fileLines[i].length() - 11);
            team = team.left(team.length() - 1);
            playerTeam.append(team);
        }
    }

    QVector<PlayerStats> playerStats;

    playerStats.resize(8);

    for(int i = 0; i < playerNames.count(); i++ )
    {
        playerStats[i].name = playerNames.at(i).toLocal8Bit();
        qInfo(logInfo()) << "Player from test stats" << playerStats[i].name;
    }

    for(int i = 0; i < playerRaces.count(); i++ )
        playerStats[i].race = playerRaces.at(i);

    for(int i = 0; i < playerTeam.count(); i++ )
        playerStats[i].team = playerTeam.at(i);

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

    emit sendPlayersTestStats(playerStats);
}

void GameInfoReader::receiveAverrageApm(int apm)
{
    m_lastAverrageApm = apm;
}

void GameInfoReader::receivePlayresStemIdFromScanner(QList<SearchStemIdPlayerInfo> playersInfoFromScanner, int playersCount)
{

    if(m_gameCurrentState != SsGameState::gameStoped && m_gameCurrentState != SsGameState::unknown)
        return;

    m_playersInfoFromScanner = playersInfoFromScanner;
    m_playersCountFromScanner = playersCount;

    for (int i = 0; i < playersInfoFromScanner.count(); i++)
    {
        bool playerFinded = false;
        for(int j = 0; j < m_allPlayersInfoFromScanner.count(); j++)
        {
            if (playersInfoFromScanner[i].name == m_allPlayersInfoFromScanner[j].name)
            {
                playerFinded = true;
                break;
            }
        }

        if(!playerFinded)
        {
            m_allPlayersInfoFromScanner.append(playersInfoFromScanner[i]);
            qInfo(logInfo()) << "Finded player in joined game:" << m_allPlayersInfoFromScanner.last().name;
        }
    }
}

void GameInfoReader::receivePlayerStemIdForHostedGame(SearchStemIdPlayerInfo playerInfoFromScanner)
{
    if(m_gameCurrentState != SsGameState::gameStoped && m_gameCurrentState != SsGameState::unknown)
        return;

    //m_playersInfoFromScanner.append(playerInfoFromScanner);

    bool playerFinded = false;
    for(int j = 0; j < m_playersInfoFromScanner.count(); j++)
    {
        if (playerInfoFromScanner.steamId == m_playersInfoFromScanner[j].steamId)
        {
            playerFinded = true;
            break;
        }
    }

    if(!playerFinded)
    {
        m_playersInfoFromScanner.append(playerInfoFromScanner);
        qInfo(logInfo()) << "Finded player in hosted game:" << m_playersInfoFromScanner.last().name;
    }

    //qInfo(logInfo()) << "Finded player:" << m_playersInfoFromScanner.last().name;
}

void GameInfoReader::onQuitParty()
{
    m_playersInfoFromScanner.clear();
}

void GameInfoReader::setGameLounched(bool newGameLounched)
{
    if (m_gameLounched == newGameLounched)
        return;

    m_gameLounched = newGameLounched;

    if (m_gameLounched)
        readGameInfo();
    else
        ssWindowClosed();
}

void GameInfoReader::stopedGame()
{
    if (m_gameCurrentState != SsGameState::gameStoped)
    {
        qInfo(logInfo()) << "Game Stoped";
        m_gameCurrentState = SsGameState::gameStoped;
        emit gameStopped();
    }
}

void GameInfoReader::setCurrentProfile(const QString &newCurrentProfile)
{
    m_currentProfile = newCurrentProfile;
}

void GameInfoReader::ssWindowClosed()
{
    stopedGame();

    if (m_ssCurrentState == SsState::ssShutdowned)
        return;

    m_ssCurrentState = SsState::ssShutdowned;
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
        parseSsSettings();
        readTestStatsTemp();
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

bool GameInfoReader::checkMissionSettingsValide(int gameType)
{
    //QByteArray playback;
   // QString mod_name;
    //QString playback_name;


    RepReader repReader(m_ssPath+"/Playback/temp.rec");

    return repReader.isStandart(gameType);

   // if(!repReader.isStandart(gameType))
    //    return 17;
    // конвертируем реплей в стимовскую версию
   /* repReader.convertReplayToSteamVersion();

    mod_name = repReader.replay.MOD;

    QString new_name = repReader.RenameReplay();
    // переименовываем название реплея в игре по стандарту
    if(!new_name.isEmpty())
        playback_name = new_name+".rec";
    else
        qDebug() << "Could not change playback name";

    playback = repReader.getReplayData();*/


}

void GameInfoReader::readTestStatsTemp()
{
    QString statsPath = m_ssPath + "\\Profiles\\" + m_currentProfile + "\\teststats.lua";

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

    QTextStream textStream(&file);
    testStatsTemp = textStream.readAll().split("\r");

    file.close();

    qInfo(logInfo()) << "testStats temp readed";
}

void GameInfoReader::parseSsSettings()
{
    QSettings* ssSettings = new QSettings(m_ssPath+"\\Local.ini", QSettings::Format::IniFormat);
    m_currentProfile = ssSettings->value("global/playerprofile","profile").toString();
    delete ssSettings;
}


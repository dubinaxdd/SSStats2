#define GAME_INFO_READER_TIMER_INTERVAL 1000
#define RACES_READ_TIMER_INTERVAL 100

#include <WarningsLogReader.h>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QSettings>
#include <repreader.h>
#include <QDir>

using namespace ReplayReader;


WarningsLogReader::WarningsLogReader(QString sspath, QObject *parent)
    : QObject(parent)
    , m_ssPath(sspath)

{
    m_readRacesSingleShootTimer = new QTimer(this);
    m_readRacesSingleShootTimer->setInterval(RACES_READ_TIMER_INTERVAL);
    QObject::connect(m_readRacesSingleShootTimer, &QTimer::timeout, this, &WarningsLogReader::readRacesTimerTimeout, Qt::QueuedConnection );

    m_gameInfoReadTimer = new QTimer(this);
    m_gameInfoReadTimer->setInterval(GAME_INFO_READER_TIMER_INTERVAL);
    QObject::connect(m_gameInfoReadTimer, &QTimer::timeout, this, &WarningsLogReader::readGameInfo, Qt::QueuedConnection );

    m_gameInfoReadTimer->start();
}

void WarningsLogReader::readGameInfo()
{
    if (!m_gameLounched)
        return;

    QFile file(m_ssPath+"\\warnings.log");

    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream textStream(&file);
        QStringList* fileLines = new QStringList();

        *fileLines = textStream.readAll().split("\r");

        int counter = fileLines->size();

        while (counter!=0)
        {
            QString line = fileLines->at(counter-1);

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

            ///Проверка имени локального игрока
            if(line.contains("GAME -- Local player"))
            {
                setLocalPlayerName(line);
            }

            ///Проверка на выбадение в обсы локального игрока
            if(line.contains("MOD -- Player"))
            {
                playerDroppedToObserver(line);
            }

            ///Проверка на достижение условия победы
            if(line.contains("MOD -- Game Over at frame")/*||line.contains("storing simulation results for match")*/)
            {
                missionOver();
                break;
            }

            ///Проверка на окончание игры
            if(line.contains("APP -- Game Stop"))
            {
                missionStoped();
                break;
            }

            ///Проверка на принудительное окончание игры
            if(line.contains("Lobby - LIE_Back2FE"))
            {
                forceMissionStoped();
                break;
            }

            ///Проверка на старт миссии игры
            if (line.contains("GAME -- Starting mission"))
            {
                missionStarted(fileLines, counter);
                break;
            }


            ///Проверка на старт загрузки игры
            if (line.contains("APP -- Game Start"))
            {
                missionLoad(fileLines, counter);
                break;
            }
            counter--;
        }

        delete fileLines;
    }
}

void WarningsLogReader::readReplayDataAfterStop()
{
    if (!m_gameWillBePlayedInOtherSession)
    {
        qWarning(logWarning()) << "Game will be played in other session, replay not sended";
        return;
    }
    m_testStatsPath = updaTetestStatsFilePath();

    qInfo(logInfo()) << "teststats.lua path: " << m_testStatsPath;

    QFile file(m_testStatsPath);

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
    bool computersFinded = false;

    for(int i = 0; i < playerNames.count(); i++ )
    {
        if (playerStats[i].pHuman == false)
        {
            computersFinded = true;
            break;
        }
    }

    QString warning;

    warning += "    The replay has not been uploaded to the server!\n";

    bool checkFailed = false;

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


    if (playersCount == 2)
    {
        //Проверка условий победы для игр 1х1
        bool isStdWinConditions = m_winCoditionsVector.contains( WinCondition::ANNIHILATE)
                               && m_winCoditionsVector.contains( WinCondition::CONTROLAREA)
                               && m_winCoditionsVector.contains( WinCondition::STRATEGICOBJECTIVE)
                               && !m_winCoditionsVector.contains( WinCondition::ASSASSINATE)
                               && !m_winCoditionsVector.contains( WinCondition::DESTROYHQ)
                               && !m_winCoditionsVector.contains( WinCondition::ECONOMICVICTORY)
                               && !m_winCoditionsVector.contains( WinCondition::SUDDENDEATH);

        if (!isStdWinConditions)
        {
            checkFailed = true;
            warning += "    Standard winning conditions were not set up for the game\n";
        }
    }
    else
    {
        //Проверка условий победы для игр больше чем 1х1

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
    if(duration <= 30)
    {
        checkFailed = true;
        warning += "    Game lasted less than 30 seconds\n";
    }

    //Проверка на наличие победителя
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
        checkFailed = true;
        warning += "    No game winner has been determined\n";
    }

    //Отправка реплея
    SendingReplayInfo replayInfo;

    for(int i = 0; i < playersCount; i++)
    {
        PlayerInfoForReplaySendong newPlayer;
        newPlayer.playerName = playerStats[i].name;

        //bool badName = true;

        //Берем сиды из последнего скана
        for(int j = 0; j < m_playersInfoFromScanner.count(); j++)
        {
            if(newPlayer.playerName == m_playersInfoFromScanner[j].name)
            {
                newPlayer.playerSid = m_playersInfoFromScanner[j].steamId;
                //badName = false;
                break;
            }
        }

        //Если не нашлось соответствия имени то светим ошибку
       /* if (badName && !computersFinded)
        {
            checkFailed = true;
            warning += "    Bad name: " + newPlayer.playerName + "\n";
        }
*/
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

    if (!m_lastGameSettingsValide)
    {
        checkFailed = true;
        warning += "    Game settings not valide\n";
    }

    if (checkFailed)
    {
        emit sendNotification(warning, true);
        qWarning(logWarning()) << warning;
        return;
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

    //m_allPlayersInfoFromScanner.clear();
    //qInfo(logInfo()) << "Players history cleared";

    qInfo(logInfo()) << "Readed played game settings";
}

void WarningsLogReader::readRacesTimerTimeout()
{
    m_readRacesSingleShootTimer->stop();

    m_testStatsPath = updaTetestStatsFilePath();

    QFile file(m_testStatsPath);

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

    qInfo(logInfo()) << "testStats temp readed in" << m_testStatsPath;

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

void WarningsLogReader::receiveAverrageApm(int apm)
{
    m_lastAverrageApm = apm;
}

void WarningsLogReader::receivePlayresStemIdFromScanner(QList<SearchStemIdPlayerInfo> playersInfoFromScanner, int playersCount)
{
    //if(m_missionCurrentState != SsMissionState::gameStoped && m_missionCurrentState != SsMissionState::unknown)
    //    return;

    for (int i = 0; i < playersInfoFromScanner.count(); i++)
        qInfo(logInfo()) << "Receive player data from DOW server:"<< playersInfoFromScanner.at(i).name << playersInfoFromScanner.at(i).steamId;

    m_playersInfoFromScanner = playersInfoFromScanner;
}

void WarningsLogReader::onQuitParty()
{
    m_playersInfoFromScanner.clear();
}

void WarningsLogReader::setGameLounched(bool newGameLounched)
{
    if (m_gameLounched == newGameLounched)
        return;

    m_gameLounched = newGameLounched;

    if (m_gameLounched)
        readGameInfo();
    else
        ssWindowClosed();
}

void WarningsLogReader::stopedGame()
{
    if (m_missionCurrentState != SsMissionState::gameStoped
            && m_missionCurrentState != SsMissionState::playbackStoped
            && m_missionCurrentState != SsMissionState::savedGameStoped
            && m_missionCurrentState != SsMissionState::unknownGameStoped)
    {
        qInfo(logInfo()) << "Game Stoped";
        m_missionCurrentState = SsMissionState::gameStoped;
        emit sendCurrentMissionState(m_missionCurrentState);
    }
}

void WarningsLogReader::setCurrentProfile(const QString &newCurrentProfile)
{
    m_currentProfile = newCurrentProfile;
}

void WarningsLogReader::ssWindowClosed()
{
    stopedGame();

    if (m_ssCurrentState == SsState::ssShutdowned)
        return;

    m_ssCurrentState = SsState::ssShutdowned;
    emit ssShutdown();
    qInfo(logInfo()) << "SS Shutdown";
}

bool WarningsLogReader::getGameInitialized()
{
    if (m_ssCurrentState == SsState::ssInitialized)
        return true;
    else
        return false;
}

void WarningsLogReader::checkGameInitialize()
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

void WarningsLogReader::checkCurrentMode()
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
                    {
                        m_currentModeVersion  = m_currentMode.right(m_currentMode.size() - i - 2);
                        m_currentMode = m_currentMode.left(i);
                    }
                }

                qInfo(logInfo()) << "Current mode:" << m_currentMode;
                qInfo(logInfo()) << "Current mode version:" << m_currentModeVersion;

                emit sendCurrentModeVersion(m_currentModeVersion);

                break;
            }

            counter--;
        }
    }
}

bool WarningsLogReader::checkMissionSettingsValide(int gameType)
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

void WarningsLogReader::readTestStatsTemp()
{

    m_testStatsPath = updaTetestStatsFilePath();
    QFile file(m_testStatsPath);

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

    qInfo(logInfo()) << "testStats temp readed in" << m_testStatsPath;
}

void WarningsLogReader::parseSsSettings()
{
    QSettings* ssSettings = new QSettings(m_ssPath+"\\Local.ini", QSettings::Format::IniFormat);
    m_currentProfile = ssSettings->value("global/playerprofile","profile").toString();
    delete ssSettings;
}

QString WarningsLogReader::updaTetestStatsFilePath()
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

void WarningsLogReader::missionLoad(QStringList* fileLines, int counter)
{
    //Что бы точно понять норм игра это или долбаный реплей чекаем ближайшие строки
    int checkCounter = counter - 1;

    while (checkCounter!=0 && checkCounter != (counter - 10))
    {
        QString checkLine = fileLines->at(checkCounter-1);

        ///Проверка на реплей
        if(checkLine.contains("APP -- Game Playback"))
        {
            if (m_missionCurrentState != SsMissionState::playbackLoadStarted)
            {
                m_missionCurrentState = SsMissionState::playbackLoadStarted;
                checkGameInitialize();
                m_readRacesSingleShootTimer->start();
                emit sendCurrentMissionState(m_missionCurrentState);
                qInfo(logInfo()) << "Playback load started";
            }
            break;
        }

        ///Проверка на загруженную игру
        if (checkLine.contains("APP -- Game Load"))
        {
            if (m_missionCurrentState != SsMissionState::savedGameLoadStarted)
            {
                m_playerDroppedToObserver = false;
                m_missionCurrentState = SsMissionState::savedGameLoadStarted;
                checkGameInitialize();
                readTestStatsTemp();
                m_readRacesSingleShootTimer->start();
                emit sendCurrentMissionState(m_missionCurrentState);
                qInfo(logInfo()) << "Saved game load started";
            }
            break;
        }

        checkCounter--;
    }

    if (m_missionCurrentState != SsMissionState::savedGameLoadStarted
        && m_missionCurrentState != SsMissionState::playbackLoadStarted
        && m_missionCurrentState != SsMissionState::gameLoadStarted)
    {
        m_playerDroppedToObserver = false;
        m_missionCurrentState = SsMissionState::gameLoadStarted;
        checkGameInitialize();
        m_readRacesSingleShootTimer->start();
        emit sendCurrentMissionState(m_missionCurrentState);
        qInfo(logInfo()) << "Game load started";
    }
}

void WarningsLogReader::missionStarted(QStringList* fileLines, int counter)
{
    if(m_missionCurrentState == SsMissionState::gameLoadStarted
       || m_missionCurrentState == SsMissionState::playbackLoadStarted
       || m_missionCurrentState == SsMissionState::savedGameLoadStarted)
    {
        if(m_missionCurrentState == SsMissionState::gameLoadStarted)
        {
            m_missionCurrentState = SsMissionState::gameStarted;
            readWinCondotions(fileLines, counter);
            qInfo(logInfo()) << "Starting game mission";
        }
        else if (m_missionCurrentState == SsMissionState::playbackLoadStarted)
        {
            m_missionCurrentState = SsMissionState::playbackStarted;
            qInfo(logInfo()) << "Starting playback mission";
        }
        else if (m_missionCurrentState == SsMissionState::savedGameLoadStarted)
        {
            m_missionCurrentState = SsMissionState::savedGameStarted;
            qInfo(logInfo()) << "Starting saved game mission";
        }
        checkGameInitialize();
        testStatsTemp = QStringList();
        readRacesTimerTimeout();
        emit sendCurrentMissionState(m_missionCurrentState);

        if (fileLines->at(counter - 1).contains("GAME -- Local player"))
            setLocalPlayerName(fileLines->at(counter - 1));

        if (m_playerDroppedToObserver)
            emit localPlayerDroppedToObserver();

    }
    else if(m_missionCurrentState != SsMissionState::gameStarted
          && m_missionCurrentState != SsMissionState::playbackStarted
          && m_missionCurrentState != SsMissionState::savedGameStarted
          && m_missionCurrentState != SsMissionState::unknownGameStarted
          && m_missionCurrentState != SsMissionState::gameOver)
    {
        m_missionCurrentState = SsMissionState::unknownGameStarted;
        qInfo(logInfo()) << "Starting unknown mission";
        checkGameInitialize();
        testStatsTemp = QStringList();
        readRacesTimerTimeout();
        emit sendCurrentMissionState(m_missionCurrentState);
    }
}

void WarningsLogReader::missionOver()
{
    if (m_missionCurrentState != SsMissionState::gameOver
            && m_missionCurrentState != SsMissionState::playbackOver
            && m_missionCurrentState != SsMissionState::savedGameOver
            && m_missionCurrentState != SsMissionState::unknownGameOver)
    {
        switch(m_missionCurrentState)
        {
            case SsMissionState::gameStarted : m_missionCurrentState = SsMissionState::gameOver; break;
            case SsMissionState::playbackStarted : m_missionCurrentState = SsMissionState::playbackOver; break;
            case SsMissionState::savedGameStarted : m_missionCurrentState = SsMissionState::savedGameOver; break;
            case unknownGameStarted : m_missionCurrentState = SsMissionState::unknownGameOver; break;
            default: break;
        }

        checkGameInitialize();
        testStatsTemp = QStringList();
        readRacesTimerTimeout();

        emit sendCurrentMissionState(m_missionCurrentState);

        qInfo(logInfo()) << "Mission over";
    }
}

void WarningsLogReader::missionStoped()
{
    if (m_missionCurrentState != SsMissionState::gameStoped
        && m_missionCurrentState != SsMissionState::playbackStoped
        && m_missionCurrentState != SsMissionState::savedGameStoped
        && m_missionCurrentState != SsMissionState::unknownGameStoped)
    {
        checkGameInitialize();
        readTestStatsTemp();

        switch(m_missionCurrentState)
        {
            case SsMissionState::gameOver :
            case SsMissionState::gameStarted : m_missionCurrentState = SsMissionState::gameStoped; readReplayDataAfterStop(); break;
            case SsMissionState::playbackOver :
            case SsMissionState::playbackStarted : m_missionCurrentState = SsMissionState::playbackStoped; break;
            case SsMissionState::savedGameOver :
            case SsMissionState::savedGameStarted : m_missionCurrentState = SsMissionState::savedGameStoped; break;
            case unknown :
            case unknownGameOver :
            case unknownGameStarted : m_missionCurrentState = SsMissionState::unknownGameStoped; break;
            default: break;
        }

        emit sendCurrentMissionState(m_missionCurrentState);
        qInfo(logInfo()) << "Mission Stoped";
    }
}

void WarningsLogReader::forceMissionStoped()
{
    checkGameInitialize();
    readTestStatsTemp();

    m_missionCurrentState = SsMissionState::unknownGameStoped;

    emit sendCurrentMissionState(m_missionCurrentState);
    qInfo(logInfo()) << "Force Mission Stoped";
}

void WarningsLogReader::setLocalPlayerName(QString str)
{
    QString localPlayerName = str.right(str.length() - 37);

    for(int i = localPlayerName.count() - 1; i != 0; i--)
    {
        if(localPlayerName.at(i) == ',')
        {
            localPlayerName = localPlayerName.left(i);
            break;
        }
    }

    if(m_localPlayerName != localPlayerName)
    {
        m_localPlayerName = localPlayerName;
        qInfo(logInfo()) << "Local player name" << m_localPlayerName;
    }
}

void WarningsLogReader::playerDroppedToObserver(QString str)
{
    if (m_playerDroppedToObserver)
        return;

    QString playerName = str.right(str.length() - 29);

    if (str.contains(m_localPlayerName))
    {
        m_playerDroppedToObserver = true;
        emit localPlayerDroppedToObserver();
        qInfo(logInfo()) << "Local player dropped to observer" << playerName;
    }
}

void WarningsLogReader::readWinCondotions(QStringList *fileLines, int counter)
{
    m_winCoditionsVector.clear();

    int winConditionsReadCounter = counter;
    QString winConditionsReadLine;

    while (!winConditionsReadLine.contains("APP -- Game Start"))
    {
        m_gameWillBePlayedInOtherSession = true;

        winConditionsReadLine = fileLines->at(winConditionsReadCounter-1);

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
}


#define GAME_INFO_READER_TIMER_INTERVAL 1000
#define RACES_READ_TIMER_INTERVAL 100

#include <gameStateReader.h>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QSettings>
#include <QDir>

GameStateReader::GameStateReader(SettingsController *settingsController, QObject *parent)
    : QObject(parent)
    , m_settingsController(settingsController)
{
    m_readRacesSingleShootTimer = new QTimer(this);
    m_readRacesSingleShootTimer->setInterval(RACES_READ_TIMER_INTERVAL);
    QObject::connect(m_readRacesSingleShootTimer, &QTimer::timeout, this, &GameStateReader::readRacesTimerTimeout, Qt::QueuedConnection );

    m_gameInfoReadTimer = new QTimer(this);
    m_gameInfoReadTimer->setInterval(GAME_INFO_READER_TIMER_INTERVAL);
    QObject::connect(m_gameInfoReadTimer, &QTimer::timeout, this, &GameStateReader::readGameInfo, Qt::QueuedConnection );
}

void GameStateReader::readGameInfo()
{
    if (!m_gameLounched)
        return;

    QFile file(m_currentGame->gameSettingsPath + "\\warnings.log");

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
                if (forceMissionStoped())
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

void GameStateReader::readRacesTimerTimeout()
{
    m_readRacesSingleShootTimer->stop();

    m_testStatsPath = updateTestStatsFilePath();

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

            //TODO: повторяем два раза ибо могут быть двойные скобки
            if (name.count() >= 3 && name.at(0) == '[' && name.at(name.count()-1) == ']')
                name = name.mid(1, name.count() - 2);

            if (name.count() >= 3 && name.at(0) == '[' && name.at(name.count()-1) == ']')
                name = name.mid(1, name.count() - 2);

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

    determinateRankedMode(playerStats);
}

void GameStateReader::setGameLounched(bool newGameLounched)
{
    if (m_gameLounched == newGameLounched)
        return;

    m_gameLounched = newGameLounched;

    if (m_gameLounched)
        readGameInfo();
    else
        ssWindowClosed();
}

void GameStateReader::stopedGame()
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

void GameStateReader::receivePlyersRankedState(QVector<PlyersRankedState> plyersRankedState)
{
    m_plyersRankedState = plyersRankedState;
}

void GameStateReader::setCurrentProfile(const QString &newCurrentProfile)
{
    m_currentProfile = newCurrentProfile;
}

void GameStateReader::ssWindowClosed()
{
    stopedGame();

    if (m_ssCurrentState == SsState::ssShutdowned)
        return;

    m_ssCurrentState = SsState::ssShutdowned;
    emit ssShutdown();
    qInfo(logInfo()) << "SS Shutdown";
}

bool GameStateReader::getGameInitialized()
{
    if (m_ssCurrentState == SsState::ssInitialized)
        return true;
    else
        return false;
}

void GameStateReader::checkGameInitialize()
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

void GameStateReader::checkCurrentMode()
{
    QFile file(m_currentGame->gameSettingsPath + "\\warnings.log");

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
                QString currentMode;

                currentMode = line.right(line.size() - 39);

                for (int i = 0; i < currentMode.size(); i++)
                {
                    if(currentMode.at(i) == ',')
                    {
                        m_currentModeVersion  = currentMode.right(currentMode.size() - i - 2);
                        currentMode = currentMode.left(i);
                    }
                }

                if (currentMode.contains("itializing Mod DoWDE"))
                    currentMode = "dowde";

                qInfo(logInfo()) << "Current mode:" << currentMode;
                qInfo(logInfo()) << "Current mode version:" << m_currentModeVersion;

                m_settingsController->getSettings()->currentMod = currentMode;
                m_settingsController->getSettings()->currentModVersion = m_currentModeVersion;
                m_settingsController->saveSettings();

                emit sendCurrentMod(currentMode);
                emit sendCurrentModVersion(m_currentModeVersion);

                break;
            }

            counter--;
        }
    }
}

void GameStateReader::readTestStatsTemp()
{

    m_testStatsPath = updateTestStatsFilePath();
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

void GameStateReader::parseSsSettings()
{
    QSettings* ssSettings = new QSettings(m_currentGame->gameSettingsPath + "\\Local.ini", QSettings::Format::IniFormat);
    m_currentProfile = ssSettings->value("global/playerprofile","profile").toString();
    delete ssSettings;
}

QString GameStateReader::updateTestStatsFilePath()
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

void GameStateReader::missionLoad(QStringList* fileLines, int counter)
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

    ///Проверка на обычную игру
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

void GameStateReader::missionStarted(QStringList* fileLines, int counter)
{
    if(m_missionCurrentState == SsMissionState::gameLoadStarted
       || m_missionCurrentState == SsMissionState::playbackLoadStarted
       || m_missionCurrentState == SsMissionState::savedGameLoadStarted)
    {
        if(m_missionCurrentState == SsMissionState::gameLoadStarted)
        {
            m_missionCurrentState = SsMissionState::gameStarted;
            readWinConditions(fileLines, counter);
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

void GameStateReader::missionOver()
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

void GameStateReader::missionStoped()
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
            case SsMissionState::gameStarted : m_missionCurrentState = SsMissionState::gameStoped; break;
            case SsMissionState::playbackOver :
            case SsMissionState::playbackStarted : m_missionCurrentState = SsMissionState::playbackStoped; break;
            case SsMissionState::savedGameOver :
            case SsMissionState::savedGameStarted : m_missionCurrentState = SsMissionState::savedGameStoped; break;
            case unknown :
            case unknownGameOver :
            case unknownGameStarted : m_missionCurrentState = SsMissionState::unknownGameStoped; break;
            default: break;
        }

        m_lockRanked = false;
        emit sendCurrentMissionState(m_missionCurrentState);
        qInfo(logInfo()) << "Mission Stoped";
    }
}

bool GameStateReader::forceMissionStoped()
{
    if(m_missionCurrentState == SsMissionState::unknownGameStoped
            || m_missionCurrentState == SsMissionState::gameOver
            || m_missionCurrentState == SsMissionState::gameStoped
            || m_missionCurrentState == SsMissionState::savedGameOver
            || m_missionCurrentState == SsMissionState::savedGameStoped
            || m_missionCurrentState == SsMissionState::playbackOver
            || m_missionCurrentState == SsMissionState::playbackStoped)
        return false;

    checkGameInitialize();
    readTestStatsTemp();

    m_missionCurrentState = SsMissionState::unknownGameStoped;
    m_lockRanked = false;

    emit sendCurrentMissionState(m_missionCurrentState);
    qInfo(logInfo()) << "Force Mission Stoped";
    return true;
}

void GameStateReader::setLocalPlayerName(QString str)
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

void GameStateReader::playerDroppedToObserver(QString str)
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

void GameStateReader::readWinConditions(QStringList *fileLines, int counter)
{
    QVector<WinCondition> winCoditionsVector;
    winCoditionsVector.resize(0);

    int winConditionsReadCounter = counter;
    QString winConditionsReadLine;

    while (!winConditionsReadLine.contains("APP -- Game Start"))
    {
        //m_gameWillBePlayedInOtherSession = true;

        winConditionsReadLine = fileLines->at(winConditionsReadCounter-1);

        if(winConditionsReadLine.contains("MOD -- Loading Win Condition"))
        {
            if(winConditionsReadLine.contains("ANNIHILATE") || winConditionsReadLine.contains("annihilate")){
                winCoditionsVector.append(WinCondition::ANNIHILATE);
                qInfo(logInfo()) << "Loaded ANNIHILATE";
            }

            if(winConditionsReadLine.contains("ASSASSINATE")){
                winCoditionsVector.append(WinCondition::ASSASSINATE);
                qInfo(logInfo()) << "Loaded ASSASSINATE";
            }

            if(winConditionsReadLine.contains("CONTROLAREA")){
                winCoditionsVector.append(WinCondition::CONTROLAREA);
                qInfo(logInfo()) << "Loaded CONTROLAREA";
            }

            if(winConditionsReadLine.contains("DESTROYHQ")){
                winCoditionsVector.append(WinCondition::DESTROYHQ);
                qInfo(logInfo()) << "Loaded DESTROYHQ";
            }

            if(winConditionsReadLine.contains("ECONOMICVICTORY")){
                winCoditionsVector.append(WinCondition::ECONOMICVICTORY);
                qInfo(logInfo()) << "Loaded ECONOMICVICTORY";
            }

            if(winConditionsReadLine.contains("GAMETIMER")){
                winCoditionsVector.append(WinCondition::GAMETIMER);
                qInfo(logInfo()) << "Loaded GAMETIMER";
            }

            if(winConditionsReadLine.contains("STRATEGICOBJECTIVE")){
                winCoditionsVector.append(WinCondition::STRATEGICOBJECTIVE);
                qInfo(logInfo()) << "Loaded STRATEGICOBJECTIVE";
            }

            if(winConditionsReadLine.contains("SUDDENDEATH")){
                winCoditionsVector.append(WinCondition::SUDDENDEATH);
                qInfo(logInfo()) << "Loaded SUDDENDEATH";
            }
        }

        winConditionsReadCounter--;
    }

    emit sendCurrentWinConditions(winCoditionsVector);
}

void GameStateReader::determinateRankedMode(QVector<PlayerStats> playerStats)
{
    if(m_lockRanked)
        return;

    bool isRanked = false;
    bool playersNotFinded = true;

    //Ищем соответствия в testStats.lua чтобы учитывать рейтинговый режим только игрокв (обсов не учитываем)
    for (int i = 0; i < m_plyersRankedState.count(); i++)
    {
        if (m_plyersRankedState.at(i).name.isEmpty())
        {
            qWarning(logWarning()) << "GameStateReader::determinateRankedMode: Player name from Ranked server is empty";
            continue;
        }

        bool finded = false;

        for(int j = 0; j < playerStats.count(); j++)
        {
            if (playerStats.at(j).name.isEmpty())
                continue;

            if ( playerStats.at(j).name == m_plyersRankedState.at(i).name )
            {
                finded = true;
                break;
            }
        }

        if(!finded)
            continue;

        playersNotFinded = false;

        if (m_plyersRankedState.at(i).isRanked)
        {
            isRanked = true;
            break;
        }

    }

    m_rankedMode = isRanked;

    if (m_missionCurrentState == SsMissionState::unknown
        ||  m_missionCurrentState == SsMissionState::unknownGameStarted
        ||  m_missionCurrentState == SsMissionState::unknownGameStoped
        ||  m_missionCurrentState == SsMissionState::gameOver
        ||  m_missionCurrentState == SsMissionState::playbackLoadStarted
        ||  m_missionCurrentState == SsMissionState::savedGameLoadStarted
        )
        m_rankedMode = false;

    if (!playersNotFinded)
        m_lockRanked = true;

    emit sendGameRankedMode(m_rankedMode);
}

void GameStateReader::setCurrentGame(GamePath *newCurrentGame)
{
    m_currentGame = newCurrentGame;
    m_gameInfoReadTimer->start();
}

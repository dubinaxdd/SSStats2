#define GAME_INFO_READER_TIMER_INTERVAL 400
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
                if (m_ssCurrentState != GameState::gameShutdowned)
                    ssWindowClosed();
                break;
            }

            ///Проверка на окончание инициализации игры
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

            ///Проверка на выпадение в обсы локального игрока
            if(line.contains("MOD -- Player") && line.contains("has been killed"))
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
                missionStoped(fileLines, counter);
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

            ///Чтение чексумм
            if (line.contains("SetOwnVersionAndDataChecksums") && !m_dataChecksummReaded)
            {
                readCheckSumm(line);
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

    if (m_currentGame->gameType == GameType::GameTypeEnum::DefinitiveEdition)
        return;

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
    if (m_missionCurrentState != GameMissionState::gameStoped
            && m_missionCurrentState != GameMissionState::playbackStoped
            && m_missionCurrentState != GameMissionState::savedGameStoped
            && m_missionCurrentState != GameMissionState::unknownGameStoped)
    {
        qInfo(logInfo()) << "Game Stoped";
        m_missionCurrentState = GameMissionState::gameStoped;
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

    if (m_ssCurrentState == GameState::gameShutdowned)
        return;

    m_ssCurrentState = GameState::gameShutdowned;
    emit ssShutdown();
    qInfo(logInfo()) << "Game Shutdown";
}

bool GameStateReader::getGameInitialized()
{
    if (m_ssCurrentState == GameState::gameInitialized)
        return true;
    else
        return false;
}

void GameStateReader::checkGameInitialize()
{
    if(m_ssCurrentState != GameState::gameInitialized)
    {
        qInfo(logInfo()) << "Game Initialized";
        m_ssCurrentState = GameState::gameInitialized;
        parseGmaeSettings();
        readTestStatsTemp();
        emit gameInitialized();
        checkCurrentGameVersion();
        checkCurrentMode();
        m_dataChecksummReaded = false;
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
                int prefixIndex = line.indexOf("MOD -- Initializing Mod");

                QString currentMode = line.right(line.count() - prefixIndex - 24);

                if (!currentMode.contains(", "))
                    return;

                int postfixIndex = currentMode.indexOf(", ");

                int nameLength =  postfixIndex;

                if (nameLength <= 0)
                    return;

                currentMode = currentMode.left(nameLength).toLower();

                postfixIndex = line.indexOf(", ");
                m_currentModeVersion = line.right(line.size() - postfixIndex - 2);

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

void GameStateReader::checkCurrentGameVersion()
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
            if(line.contains("GAME -- Warhammer,"))
            {
                int prefixIndex = line.indexOf("GAME -- Warhammer,");
                int endIndex = line.indexOf(", Build");

                QString gameVersion = line.mid(prefixIndex + 19 , endIndex - (prefixIndex + 19));
                qInfo(logInfo()) << "Game version:" << gameVersion;

                emit sendCurrentGameVersion(gameVersion);
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

void GameStateReader::parseGmaeSettings()
{
    QSettings* ssSettings = new QSettings(m_currentGame->gameSettingsPath + "\\Local.ini", QSettings::Format::IniFormat);
    m_currentProfile = ssSettings->value("global/playerprofile","profile").toString();
    delete ssSettings;
}

void GameStateReader::readCheckSumm(QString line)
{
    //SetOwnVersionAndDataChecksums modName:Soulstorm   modDllFile:DXP3Mod.dll   modVersion:1.0  appBinaryCRC:3235273467 dataCRC:58725797 modDllCRC:1581112045

    DowServerRequestParametres requestParametres;

    requestParametres.modName = readParam(line, "modName:");
    requestParametres.modDllFile = readParam(line, "modDllFile:");
    requestParametres.modVersion = readParam(line, "modVersion:");

    //В лог пишется как unsigned int но в запросах отправляется как int, поэтому кастим в int
    requestParametres.appBinaryChecksum = QString::number(static_cast<int>(readParam(line, "appBinaryCRC:").toUInt()));
    requestParametres.dataChecksum = QString::number(static_cast<int>(readParam(line, "dataCRC:").toUInt()));
    requestParametres.modDLLChecksum = QString::number(static_cast<int>(readParam(line, "modDllCRC:").toUInt()));

    m_dataChecksummReaded = true;

    emit sendRequestParametres(requestParametres);
}

QString GameStateReader::readParam(QString &line, QString pattern)
{
    if(line.contains(pattern))
    {
        int startIndex = line.indexOf(pattern) + pattern.size();
        int endIndex = startIndex;

        for(int i = startIndex; i < line.size(); i++)
        {
            if (i == line.size()-1)
            {
                endIndex = i;
                break;
            }

            if(line.at(i) == " ")
            {
                endIndex = i - 1;
                break;
            }
        }

        return line.mid(startIndex, endIndex - startIndex + 1);
    }

    return "";
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
            if (m_missionCurrentState != GameMissionState::playbackLoadStarted)
            {
                m_missionCurrentState = GameMissionState::playbackLoadStarted;
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
            if (m_missionCurrentState != GameMissionState::savedGameLoadStarted)
            {
                m_playerDroppedToObserver = false;
                m_missionCurrentState = GameMissionState::savedGameLoadStarted;
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
    if (m_missionCurrentState != GameMissionState::savedGameLoadStarted
        && m_missionCurrentState != GameMissionState::playbackLoadStarted
        && m_missionCurrentState != GameMissionState::gameLoadStarted)
    {
        m_playerDroppedToObserver = false;
        m_missionCurrentState = GameMissionState::gameLoadStarted;
        checkGameInitialize();
        m_readRacesSingleShootTimer->start();
        emit sendCurrentMissionState(m_missionCurrentState);
        qInfo(logInfo()) << "Game load started";
    }
}

void GameStateReader::missionStarted(QStringList* fileLines, int counter)
{
    if(m_missionCurrentState == GameMissionState::gameLoadStarted
       || m_missionCurrentState == GameMissionState::playbackLoadStarted
       || m_missionCurrentState == GameMissionState::savedGameLoadStarted)
    {
        if(m_missionCurrentState == GameMissionState::gameLoadStarted)
        {
            m_missionCurrentState = GameMissionState::gameStarted;
            readWinConditions(fileLines, counter);
            qInfo(logInfo()) << "Starting game mission";
        }
        else if (m_missionCurrentState == GameMissionState::playbackLoadStarted)
        {
            m_missionCurrentState = GameMissionState::playbackStarted;
            qInfo(logInfo()) << "Starting playback mission";
        }
        else if (m_missionCurrentState == GameMissionState::savedGameLoadStarted)
        {
            m_missionCurrentState = GameMissionState::savedGameStarted;
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
    else if(m_missionCurrentState != GameMissionState::gameStarted
          && m_missionCurrentState != GameMissionState::playbackStarted
          && m_missionCurrentState != GameMissionState::savedGameStarted
          && m_missionCurrentState != GameMissionState::unknownGameStarted
          && m_missionCurrentState != GameMissionState::gameOver)
    {
        m_missionCurrentState = GameMissionState::unknownGameStarted;
        qInfo(logInfo()) << "Starting unknown mission";
        checkGameInitialize();
        testStatsTemp = QStringList();
        readRacesTimerTimeout();
        emit sendCurrentMissionState(m_missionCurrentState);
    }
}

void GameStateReader::missionOver()
{
    if (m_missionCurrentState != GameMissionState::gameOver
            && m_missionCurrentState != GameMissionState::playbackOver
            && m_missionCurrentState != GameMissionState::savedGameOver
            && m_missionCurrentState != GameMissionState::unknownGameOver)
    {
        switch(m_missionCurrentState)
        {
            case GameMissionState::gameStarted : m_missionCurrentState = GameMissionState::gameOver; break;
            case GameMissionState::playbackStarted : m_missionCurrentState = GameMissionState::playbackOver; break;
            case GameMissionState::savedGameStarted : m_missionCurrentState = GameMissionState::savedGameOver; break;
            case unknownGameStarted : m_missionCurrentState = GameMissionState::unknownGameOver; break;
            default: break;
        }

        checkGameInitialize();
        testStatsTemp = QStringList();
        readRacesTimerTimeout();

        emit sendCurrentMissionState(m_missionCurrentState);

        qInfo(logInfo()) << "Mission over";
    }
}

void GameStateReader::missionStoped(QStringList* fileLines, int counter)
{
    if (m_missionCurrentState != GameMissionState::gameStoped
        && m_missionCurrentState != GameMissionState::playbackStoped
        && m_missionCurrentState != GameMissionState::savedGameStoped
        && m_missionCurrentState != GameMissionState::unknownGameStoped)
    {
        checkGameInitialize();
        readTestStatsTemp();

        switch(m_missionCurrentState)
        {
            case GameMissionState::gameOver :
            case GameMissionState::gameStarted : m_missionCurrentState = GameMissionState::gameStoped; break;
            case GameMissionState::playbackOver :
            case GameMissionState::playbackStarted : m_missionCurrentState = GameMissionState::playbackStoped; break;
            case GameMissionState::savedGameOver :
            case GameMissionState::savedGameStarted : m_missionCurrentState = GameMissionState::savedGameStoped; break;
            case unknown :
            case unknownGameOver :
            case unknownGameStarted : m_missionCurrentState = GameMissionState::unknownGameStoped; break;
            default: break;
        }

        if (m_missionCurrentState == GameMissionState::gameStoped)
        {
            m_lastMatchId = "";
            QString pattern = "ReportSimStats - storing simulation results for match";

            //Ищем в следующей строке ID матча
            int i = 2;

            while (i < 70)
            {
                if (fileLines->at(counter - i).contains(pattern))
                {
                    QString line = fileLines->at(counter - i);
                    int startIndex = line.indexOf(pattern) + pattern.size() + 3;
                    m_lastMatchId = line.right(line.size() - startIndex);
                    qInfo(logInfo()) << "Match ID: " << m_lastMatchId;
                    emit matchIdParsed(m_lastMatchId);
                    break;
                }
                else
                    i++;
            }

            if (m_lastMatchId.isEmpty())
                qWarning(logWarning()) << "Match ID not finded";
        }


        m_lockRanked = false;
        emit sendCurrentMissionState(m_missionCurrentState);
        qInfo(logInfo()) << "Mission Stoped";
    }
}

bool GameStateReader::forceMissionStoped()
{
    if(m_missionCurrentState == GameMissionState::unknownGameStoped
            || m_missionCurrentState == GameMissionState::gameOver
            || m_missionCurrentState == GameMissionState::gameStoped
            || m_missionCurrentState == GameMissionState::savedGameOver
            || m_missionCurrentState == GameMissionState::savedGameStoped
            || m_missionCurrentState == GameMissionState::playbackOver
            || m_missionCurrentState == GameMissionState::playbackStoped)
        return false;

    checkGameInitialize();
    readTestStatsTemp();

    m_missionCurrentState = GameMissionState::unknownGameStoped;
    m_lockRanked = false;

    emit sendCurrentMissionState(m_missionCurrentState);
    qInfo(logInfo()) << "Force Mission Stoped";
    return true;
}

void GameStateReader::setLocalPlayerName(QString str)
{
    if (!str.contains("GAME -- Local player"))
        return;

    int prefixIndex = str.indexOf("GAME -- Local player");

    QString localPlayerName = str.right(str.count() - prefixIndex - 22);

    if (!localPlayerName.contains("finished loading"))
        return;

    int postfixIndex = localPlayerName.indexOf("finished loading");

    int nameLength =  postfixIndex - 5;

    if (nameLength <= 0)
        return;

    localPlayerName = localPlayerName.left(nameLength);

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

    if (!str.contains("MOD -- Player"))
        return;

    int prefixIndex = str.indexOf("MOD -- Player");

    QString playerName = str.right(str.count() - prefixIndex - 14);

    if (!playerName.contains("has been killed"))
        return;

    int postfixIndex = playerName.indexOf("has been killed");

    int nameLength =  postfixIndex -1;

    if (nameLength <= 0)
        return;

    playerName = playerName.left(nameLength);

    if (!m_localPlayerName.isEmpty() && playerName == m_localPlayerName)
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

    if (m_missionCurrentState == GameMissionState::unknown
        ||  m_missionCurrentState == GameMissionState::unknownGameStarted
        ||  m_missionCurrentState == GameMissionState::unknownGameStoped
        ||  m_missionCurrentState == GameMissionState::gameOver
        ||  m_missionCurrentState == GameMissionState::playbackLoadStarted
        ||  m_missionCurrentState == GameMissionState::savedGameLoadStarted
        )
        m_rankedMode = false;

    if (!playersNotFinded)
        m_lockRanked = true;

    emit sendRankedState(m_rankedMode);
}

void GameStateReader::setCurrentGame(GamePath *newCurrentGame)
{
    m_currentGame = newCurrentGame;
    m_gameInfoReadTimer->start();
}

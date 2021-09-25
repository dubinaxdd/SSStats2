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
            if(line.contains("GAME -- Shutdown quit"))
            {
                if (m_ssCurrentState != SsState::ssShutdowned)
                {
                    m_gameCurrentState = SsGameState::gameStoped;
                    m_ssCurrentState = SsState::ssShutdowned;

                    emit ssShutdown();
                    qDebug() << "INFO: SS Shutdown";
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
                m_gameCurrentState = SsGameState::gameOver;
                emit gameOver();
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

                    qDebug() << "INFO: Game Stoped";
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
                                    m_winCoditionsVector.append(WinConditions::ANNIHILATE);
                                    qDebug() << "Loaded ANNIHILATE";
                                }

                                if(winConditionsReadLine.contains("ASSASSINATE")){
                                    m_winCoditionsVector.append(WinConditions::ASSASSINATE);
                                    qDebug() << "Loaded ASSASSINATE";
                                }

                                if(winConditionsReadLine.contains("CONTROLAREA")){
                                    m_winCoditionsVector.append(WinConditions::CONTROLAREA);
                                    qDebug() << "Loaded CONTROLAREA";
                                }

                                if(winConditionsReadLine.contains("DESTROYHQ")){
                                    m_winCoditionsVector.append(WinConditions::DESTROYHQ);
                                    qDebug() << "Loaded DESTROYHQ";
                                }

                                if(winConditionsReadLine.contains("ECONOMICVICTORY")){
                                    m_winCoditionsVector.append(WinConditions::ECONOMICVICTORY);
                                    qDebug() << "Loaded ECONOMICVICTORY";
                                }

                                if(winConditionsReadLine.contains("GAMETIMER")){
                                    m_winCoditionsVector.append(WinConditions::GAMETIMER);
                                    qDebug() << "Loaded GAMETIMER";
                                }

                                if(winConditionsReadLine.contains("STRATEGICOBJECTIVE")){
                                    m_winCoditionsVector.append(WinConditions::STRATEGICOBJECTIVE);
                                    qDebug() << "Loaded STRATEGICOBJECTIVE";
                                }

                                if(winConditionsReadLine.contains("SUDDENDEATH")){
                                    m_winCoditionsVector.append(WinConditions::SUDDENDEATH);
                                    qDebug() << "Loaded SUDDENDEATH";
                                }
                            }

                            winConditionsReadCounter--;
                        }

                        qDebug() << "INFO: Starting game mission";
                    }
                    else if (m_gameCurrentState == SsGameState::playbackLoadStarted)
                    {
                        m_gameCurrentState = SsGameState::playbackStarted;
                        qDebug() << "INFO: Starting playback mission";
                    }
                    else if (m_gameCurrentState == SsGameState::savedGameLoadStarted)
                    {
                        m_gameCurrentState = SsGameState::savedGameStarted;
                        qDebug() << "INFO: Starting saved game mission";
                    }

                    checkGameInitialize();
                    emit startingMission(m_gameCurrentState);

                }
                else if(m_gameCurrentState != SsGameState::gameStarted
                      && m_gameCurrentState != SsGameState::playbackStarted
                      && m_gameCurrentState != SsGameState::savedGameStarted
                      && m_gameCurrentState != SsGameState::unknownGameStarted)
                {
                    m_gameCurrentState = SsGameState::unknownGameStarted;
                    qDebug() << "INFO: Starting unknown mission";

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
                            qDebug() << "INFO: Playback load started";
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
                            qDebug() << "INFO: Saved game load started";
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
                    qDebug() << "INFO: Game load started";
                }
                break;
            }
            counter--;
        }
    }
}

void GameInfoReader::readGameParametresAfterStop()
{
    if (m_gameCurrentState != SsGameState::gameStarted)
        return;

    bool isStdWinConditions = m_winCoditionsVector.contains( WinConditions::ANNIHILATE)
                           && m_winCoditionsVector.contains( WinConditions::CONTROLAREA)
                           && m_winCoditionsVector.contains( WinConditions::STRATEGICOBJECTIVE)
                           && !m_winCoditionsVector.contains( WinConditions::ASSASSINATE)
                           && !m_winCoditionsVector.contains( WinConditions::DESTROYHQ)
                           && !m_winCoditionsVector.contains( WinConditions::ECONOMICVICTORY)
                           && !m_winCoditionsVector.contains( WinConditions::SUDDENDEATH);

    if (!isStdWinConditions)
        return;

    QString statsPath = m_ssPath + "\\Profiles\\" + m_currentProfile + "\\teststats.lua";
    qDebug() << "INFO: teststats.lua path: " << statsPath;

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
        playerStats[i].name = playerNames.at(i);

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

    qDebug() << "players count" << playersCount;
    qDebug() << "computersFinded" << computersFinded;
    qDebug() << "teamsCount" << teamsCount;
    qDebug() << "duration" << duration;
    qDebug() << "winBy" << winBy;
    qDebug() << "scenario" << scenario;

    for(int i = 0; i < playerStats.count(); i++)
    {
        qDebug() << playerStats.at(i).name;

        switch (playerStats.at(i).finalState)
        {
            case inGame: qDebug() << "In Game"; break;
            case winner: qDebug() << "Winner";  break;
            case loser: qDebug() << "Looser";   break;
            default: qDebug() << "Unknown";     break;
        }
    }

    if (computersFinded)
        return;

    if (teamsCount > 2)
        return;

    if(duration <= 30)
        return;

    emit sendReplayToServer();


    //qDebug() << "INFO: Read played game settings";
}

void GameInfoReader::setCurrentProfile(const QString &newCurrentProfile)
{
    m_currentProfile = newCurrentProfile;
}

void GameInfoReader::ssWindowClosed()
{
    m_gameCurrentState = SsGameState::gameStoped;
    m_ssCurrentState = SsState::ssShutdowned;
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
        qDebug() << "INFO: SS Initialized";
        m_ssCurrentState = SsState::ssInitialized;
        emit gameInitialized();
    }
}

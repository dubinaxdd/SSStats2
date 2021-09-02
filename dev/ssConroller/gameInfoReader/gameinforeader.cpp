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

            ///Проверка на окончание игры
            if(line.contains("APP -- Game Stop"))
            {
                if (m_gameCurrentState != SsGameState::gameStoped)
                {
                    m_gameCurrentState = SsGameState::gameStoped;
                    checkGameInitialize();
                    emit gameStopped();
                    readGameParametresAfterStop();
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
    qDebug() << "INFO: Read played game settings";
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

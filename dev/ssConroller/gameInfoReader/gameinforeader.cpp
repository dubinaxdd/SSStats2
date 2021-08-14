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
    m_gameInfoReadTimer->setInterval(/*1000*/GAME_INFO_READER_TIMER_INTERVAL);
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
                if (!m_ssShutdowned)
                {
                    m_gameStoped = true;
                    m_gameStarted = false;
                    m_gamePlayback = false;
                    m_gameLoad = false;
                    m_ssInitialized = false;
                    m_ssShutdowned = true;
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
                if (!m_gameStoped)
                {
                    if (m_gameStarted)
                    {
                        m_gameStoped = true;
                        m_gameStarted = false;
                        emit gameStopped();
                        readGameParametresAfterStop();
                    }

                    m_gameStoped = true;
                    m_gameStarted = false;
                    m_gamePlayback = false;
                    m_gameLoad = false;

                    checkGameInitialize();
                    m_ssShutdowned = false;
                    m_missionStarted = false;

                    emit gameStopped();

                    qDebug() << "INFO: Game Stoped";
                }
                break;
            }

            ///Проверка на старт миссии игры
            if (line.contains("GAME -- Starting mission"))
            {
                if(!m_missionStarted)
                {
                    m_missionStarted = true;
                    checkGameInitialize();
                    emit startingMission();
                    qDebug() << "INFO: Starting mission";
                }
                break;
            }


            ///Проверка на старт игры
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
                        if (!m_gamePlayback)
                        {
                            m_gameStoped = false;
                            m_gameStarted = false;
                            m_gamePlayback = true;
                            m_gameLoad = false;
                            checkGameInitialize();
                            m_ssShutdowned = false;
                            qDebug() << "INFO: Game Playback";   
                        }
                        break;
                    }

                    ///Проверка на загруженную игру
                    if (checkLine.contains("APP -- Game Load"))
                    {
                        if (!m_gameLoad)
                        {
                            m_gameStoped = false;
                            m_gameStarted = false;
                            m_gamePlayback = false;
                            m_gameLoad = true;
                            checkGameInitialize();
                            m_ssShutdowned = false;
                            qDebug() << "INFO: Game Load";
                        }
                        break;
                    }

                    checkCounter--;
                }

                if (!m_gameLoad && !m_gamePlayback && !m_gameStarted)
                {
                    m_gameStoped = false;
                    m_gameStarted = true;
                    m_gamePlayback = false;
                    m_gameLoad = false;
                    checkGameInitialize();
                    m_ssShutdowned = false;
                    emit gameStarted();
                    qDebug() << "INFO: Game Started";
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
    m_ssInitialized = false;
    m_gameStoped = true;
    m_ssShutdowned = true;
}

bool GameInfoReader::getGameInitialized()
{
    return m_ssInitialized;
}

void GameInfoReader::checkGameInitialize()
{
    if(!m_ssInitialized)
    {
        qDebug() << "INFO: SS Initialized";
        m_ssInitialized = true;
        m_ssShutdowned = false;
        emit gameInitialized();

    }
}

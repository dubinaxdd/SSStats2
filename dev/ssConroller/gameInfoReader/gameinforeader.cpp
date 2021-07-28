#include "gameinforeader.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

GameInfoReader::GameInfoReader(QString sspath, QObject *parent)
    : QObject(parent)
    , m_ssPath(sspath)

{
    m_gameInfoReadTimer = new QTimer();
    m_gameInfoReadTimer->setInterval(1000);
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

            if(line.contains("APP -- Game Stop"))
            {
                if (!m_gameStoped)
                {
                    if (m_gameStarted)
                    {
                        m_gameStoped = true;
                        m_gameStarted = false;
                        m_gamePlayback = false;
                        m_gameLoad = false;
                        emit gameStoped();

                        qDebug() << "INFO: Game Stoped";
                        readGameParametresAfterStop();
                    }
                }
                break;
            }

            if (line.contains("APP -- Game Start"))
            {
                //Что бы точно понять норм игра это или долбаный реплей чекаем ближайшие строки
                int checkCounter = counter - 1;

                while (checkCounter!=0 && checkCounter != (counter - 10))
                {
                    QString checkLine = fileLines.at(checkCounter-1);
                    if(checkLine.contains("APP -- Game Playback"))
                    {
                        if (!m_gamePlayback)
                        {
                            m_gameStoped = false;
                            m_gameStarted = false;
                            m_gamePlayback = true;
                            m_gameLoad = false;
                            qDebug() << "INFO: Game Playback";
                            break;
                        }
                    }

                    if (checkLine.contains("APP -- Game Load"))
                    {
                        if (!m_gameLoad)
                        {
                            m_gameStoped = false;
                            m_gameStarted = false;
                            m_gamePlayback = false;
                            m_gameLoad = true;
                            qDebug() << "INFO: Game Load";
                            break;
                        }
                    }

                    checkCounter--;
                }

                if (!m_gameLoad && !m_gamePlayback && !m_gameStarted)
                {
                    m_gameStoped = false;
                    m_gameStarted = true;
                    m_gamePlayback = false;
                    m_gameLoad = false;

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

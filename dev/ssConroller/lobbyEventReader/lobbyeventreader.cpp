#include "lobbyeventreader.h"
#include <QTextStream>
#include <QFile>
#include <QDebug>

#define READ_EVETS_TIMER_INTERVAL 200

LobbyEventReader::LobbyEventReader(QString sspath, QObject *parent) : QObject(parent)
  , m_ssPath(sspath)
  , m_lobbyEventsReadTimer(new QTimer(this))
{
    m_lobbyEventsReadTimer->setInterval(READ_EVETS_TIMER_INTERVAL);
    QObject::connect(m_lobbyEventsReadTimer, &QTimer::timeout, this, &LobbyEventReader::readLobbyEvents, Qt::QueuedConnection);
}

void LobbyEventReader::activateReading(bool activated)
{
    m_readingActivated = activated;
    if(m_readingActivated)
        m_lobbyEventsReadTimer->start();
    else
        m_lobbyEventsReadTimer->stop();
}

void LobbyEventReader::onLoadStarted()
{
    activateReading(false);
}

void LobbyEventReader::onGameStopped()
{
    activateReading(true);
}

void LobbyEventReader::readLobbyEvents()
{
    if (!m_readingActivated)
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

            if (counter == fileLines.size() - 1)
                m_lastLogTime = line.mid(1, 11);

            if (counter == 1)
                m_preLastLogTime = m_lastLogTime;

            if (line.contains(m_preLastLogTime))
            {
                m_preLastLogTime = m_lastLogTime;
                break;
            }

            if (line.contains("LIE_QuitGame"))
            {
                if(line.contains(m_preLastLogTime))
                    break;

                m_preLastLogTime = m_lastLogTime;
                emit quitFromParty();
                qDebug() << "Quit from paty";
                break;


            }

            if (line.contains("LIE_JoinGame"))
            {
                if(line.contains(m_preLastLogTime))
                    break;

                m_preLastLogTime = m_lastLogTime;

                isHostedGame = false;
                emit joinToParty();
                qDebug() << "Join to paty";
                break;


            }

            if (line.contains("LIE_HostGame"))
            {
                if(line.contains(m_preLastLogTime))
                    break;

                m_preLastLogTime = m_lastLogTime;
                isHostedGame = true;
                emit hostParty();
                qDebug() << "Hosting party";
                break;


            }

            if (line.contains("Ignoring New Peer for local player") || line.contains("LIE_OnPlayerUpdate") || line.contains("Net UPDATE PLAYER information for player") || line.contains("MatchEvent:MET_MatchInfoUpdated") || line.contains("Lobby -- Remote player disconnected") || line.contains("New Peer for remote player"))
            {
                if(line.contains(m_preLastLogTime))
                    break;

                if (!isHostedGame)
                {
                    m_preLastLogTime = m_lastLogTime;
                    emit playerConnected();
                    qDebug() << "Player connected";
                    break;
                }

            }

            if (line.contains("New Peer for remote player") && isHostedGame)
            {

                if(line.contains(m_preLastLogTime))
                    break;
                else
                {
                    int counter2 = counter;

                    QString line2;

                    while (!line2.contains(m_preLastLogTime) && m_preLastLogTime != "baneblade")
                    {
                        line2 = fileLines.at(counter2-1);


                        if (line2.contains("New Peer for remote player"))
                        {
                            QString ssId = line2.right(line2.size() - 51);

                            for (int i = 0; i < ssId.size(); i++)
                            {
                                if(ssId.at(i) == "L")
                                    ssId = ssId.left(i - 1);
                            }

                            QString needLine = fileLines.at(counter);

                            if (!needLine.contains("*******************************NewPeer"))
                                break;

                            needLine = needLine.at(54);
                            int playerPosition = needLine.toInt();

                            emit playerConnectedToHostedGame(ssId.toInt(), playerPosition);

                            qDebug() << "Player connected:" << ssId << "position:" << playerPosition;
                        }

                        counter2--;

                    }

                    //isHostedGame = true;

                    m_preLastLogTime = m_lastLogTime;

                    break;
                }

            }

            counter--;
        }
    }
}

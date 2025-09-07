#include <lobbyEventReader.h>
#include <QTextStream>
#include <QFile>
#include <QDebug>

#define READ_EVETS_TIMER_INTERVAL 200

LobbyEventReader::LobbyEventReader(QObject *parent) : QObject(parent)
  , m_lobbyEventsReadTimer(new QTimer(this))
{
    m_ignotingPlayersListRequestTimer.setInterval(3000);
    m_ignotingPlayersListRequestTimer.setSingleShot(true);

    m_lobbyEventsReadTimer->setInterval(READ_EVETS_TIMER_INTERVAL);
    QObject::connect(m_lobbyEventsReadTimer, &QTimer::timeout, this, &LobbyEventReader::readLobbyEvents, Qt::QueuedConnection);
    QObject::connect(m_lobbyEventsReadTimer, &QTimer::timeout, this, &LobbyEventReader::readAutomatchEvents, Qt::QueuedConnection);

    QObject::connect(&m_ignotingPlayersListRequestTimer, &QTimer::timeout, this, &LobbyEventReader::requestIgnoredPlayers, Qt::QueuedConnection);
}

void LobbyEventReader::activateReading(bool activated)
{
    m_readingActivated = activated;
    if(m_readingActivated)
        m_lobbyEventsReadTimer->start();
    else
        m_lobbyEventsReadTimer->stop();
}

void LobbyEventReader::receiveCurrentMissionState(GameMissionState gameCurrentState)
{
    switch (gameCurrentState)
    {
        case GameMissionState::gameStoped :
        case GameMissionState::playbackStoped :
        case GameMissionState::savedGameStoped :
        case GameMissionState::unknownGameStoped : activateReading(true); break;
        default: activateReading(false);
    }

    if (gameCurrentState == GameMissionState::gameLoadStarted)
    {
        if (m_automatchProcessed && m_automatchNamesList.count() > m_automatchPlayersList.count())
            m_ignotingPlayersListRequestTimer.start();
    }
}

void LobbyEventReader::receiveCurrentPlayerId(QString id)
{
    m_localPlayerId = id;
}

void LobbyEventReader::readLobbyEvents()
{
    if (!m_readingActivated)
        return;

    QFile file(m_currentGame->gameSettingsPath + "\\warnings.log");

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

            if (line.contains("Lobby - LIE_GetNews received"))
            {
                if(line.contains(m_preLastLogTime))
                    break;

                m_preLastLogTime = m_lastLogTime;
                tryRequestSessionId();
                qInfo(logInfo()) << "Lobby news received";
                break;
            }

            if (line.contains("GAME -- Closing session"))
            {
                if(line.contains(m_preLastLogTime))
                    break;

                m_preLastLogTime = m_lastLogTime;
                emit quitFromParty();
                qInfo(logInfo()) << "Lobby closing session";
                break;
            }

            if (line.contains("LIE_QuitGame"))
            {
                if(line.contains(m_preLastLogTime))
                    break;

                m_preLastLogTime = m_lastLogTime;
                emit quitFromParty();
                qInfo(logInfo()) << "Quit from paty";
                break;
            }

            if (line.contains("LIE_JoinGame"))
            {
                if(line.contains(m_preLastLogTime))
                    break;

                m_preLastLogTime = m_lastLogTime;

                isHostedGame = false;
                tryRequestSessionId();
                emit joinToParty();
                qInfo(logInfo()) << "Join to paty";
                break;
            }

            if (line.contains("LIE_HostGame"))
            {
                if(line.contains(m_preLastLogTime))
                    break;

                m_preLastLogTime = m_lastLogTime;
                isHostedGame = true;
                tryRequestSessionId();
                emit hostParty();
                qInfo(logInfo()) << "Hosting party";
                break;
            }


            if (line.contains("Lobby - LIE_Ban received")
                || line.contains("Lobby -- kicked player")
                )
            {
                if(line.contains(m_preLastLogTime))
                    break;

                m_preLastLogTime = m_lastLogTime;
                emit playerKicked();
                qInfo(logInfo()) << "Player kicked";
                break;
            }


            if (line.contains("Ignoring New Peer for local player")
                    || line.contains("New Peer for remote player")
                    )
            {
                if(line.contains(m_preLastLogTime))
                    break;

                m_preLastLogTime = m_lastLogTime;
                tryRequestSessionId();
                emit playerConnected();
                qInfo(logInfo()) << "Player connected";
                break;
            }


            if (line.contains("Lobby -- Remote player disconnected")
                || line.contains("Lobby - LIE_Ban received")
                || line.contains("Lobby -- kicked player")
                )
            {
                if(line.contains(m_preLastLogTime))
                    break;

                m_preLastLogTime = m_lastLogTime;
                emit playerDisconnected();
                qInfo(logInfo()) << "Player disconnected";
                break;
            }

            counter--;
        }
    }
}

void LobbyEventReader::readAutomatchEvents()
{
    if (!m_readingActivated)
        return;

    QFile file(m_currentGame->gameSettingsPath + "\\warnings.log");

    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream textStream(&file);
        QStringList fileLines = textStream.readAll().split("\r");

        int counter = fileLines.size();

        while (counter!=0)
        {
            QString line = fileLines.at(counter-1);

            if (counter == fileLines.size() - 1)
                m_lastAutomatchLogTime = line;

            if (counter == 1)
                m_preLastAutomatchLogTime = m_lastAutomatchLogTime;

            if (line.contains(m_preLastAutomatchLogTime))
            {
                m_preLastAutomatchLogTime = m_lastAutomatchLogTime;
                break;
            }

            if (line.contains("Lobby - LIE_StartAutoMatch received"))
            {
                if(line.contains(m_preLastAutomatchLogTime))
                    break;

                m_preLastAutomatchLogTime = m_lastAutomatchLogTime;
                m_automatchProcessed = true;
                m_automatchPlayersList.clear();
                m_automatchNamesList.clear();
                m_automatchPlayersListChanged = true;
                m_automatchNamesListChanged = true;

                if (!m_localPlayerId.isEmpty())
                    m_automatchPlayersList.append(m_localPlayerId);

                tryRequestSessionId();
                emit automatchModeChanged(m_automatchProcessed);
                qInfo(logInfo()) << "Automatch search started";
                break;
            }

            if (line.contains("Lobby - LIE_StopAutoMatch received") || line.contains("GAME -- Ending mission"))
            {
                if(line.contains(m_preLastAutomatchLogTime))
                    break;

                m_preLastAutomatchLogTime = m_lastAutomatchLogTime;
                m_automatchProcessed = false;
                m_automatchPlayersList.clear();
                m_automatchNamesList.clear();
                emit automatchModeChanged(m_automatchProcessed);
                qInfo(logInfo()) << "Automatch search stoped";
                break;
            }

            if (m_automatchProcessed && line.contains("Lobby - LIE_HostGame received"))
            {
                if(line.contains(m_preLastAutomatchLogTime))
                    break;

                m_automatchPlayersList.clear();
                m_automatchNamesList.clear();
                qInfo(logInfo()) << "Hosting Automatch";
                break;
            }


            if (m_automatchProcessed && line.contains("Lobby - New Peer for remote player:"))
            {
                if(line.contains(m_preLastAutomatchLogTime))
                    break;

                parseAytomatchPlayers(line);
                qInfo(logInfo()) << "Automatch player connected";
            }


            if (m_automatchProcessed && line.contains("Lobby -- Net UPDATE PLAYER information for player"))
            {
                if(line.contains(m_preLastAutomatchLogTime))
                    break;

                parseAutomatchPlayerName(line);
                qInfo(logInfo()) << "Automatch player name parse";
            }

            counter--;
        }


        m_preLastAutomatchLogTime = m_lastAutomatchLogTime;

        if (m_automatchPlayersListChanged)
        {
            emit automatchPlayersListChanged(m_automatchPlayersList);
            m_automatchPlayersListChanged = false;
        }

        if (m_automatchNamesListChanged)
            m_automatchNamesListChanged = false;
    }
}

void LobbyEventReader::tryRequestSessionId()
{
    if(!m_sessionIdReceived && !m_sessionIdRequested)
    {
        m_sessionIdRequested = true;
        emit requestSessionId();
    }
}

void LobbyEventReader::requestIgnoredPlayers()
{
    qInfo(logInfo()) << "LobbyEventReader::requestIgnoredPlayers() - Start Find Ignored Players Id:" << m_automatchPlayersList;

    if (m_automatchPlayersList.isEmpty())
        m_automatchPlayersList.append(m_localPlayerId);

    emit findIgnoredPlayersId(m_automatchPlayersList);
}

void LobbyEventReader::parseAytomatchPlayers(QString str)
{
    //Lobby - New Peer for remote player: 12427431 Local: 10008417

    QString searchPattern = "Lobby - New Peer for remote player:";
    int startIndex = str.indexOf(searchPattern) + searchPattern.length() + 1;
    QString playerId = str.mid(startIndex, 8);

    if(!m_automatchPlayersList.contains(playerId))
    {
        m_automatchPlayersList.append(playerId);
        m_automatchPlayersListChanged = true;
    }

    searchPattern = "Local:";
    startIndex = str.indexOf(searchPattern) + searchPattern.length() + 1;
    playerId = str.mid(startIndex, 8);

    if(!m_automatchPlayersList.contains(playerId))
    {
        m_localPlayerId = playerId;
        m_automatchPlayersList.append(playerId);
        m_automatchPlayersListChanged = true;
    }

}

void LobbyEventReader::parseAutomatchPlayerName(QString str)
{
    //Lobby -- Net UPDATE PLAYER information for player (0 - 1 - [ELM] Nazgul) received

    QString searchPattern = "Lobby -- Net UPDATE PLAYER information for player";
    int startIndex = str.indexOf(searchPattern) + searchPattern.length() + 10;
    QString playerName = str.right(str.length() - startIndex).replace(") received", "");

    if(!m_automatchNamesList.contains(playerName))
    {
        qInfo(logInfo()) << "LobbyEventReader::parseAutomatchPlayerName(QString str) - Name added" << playerName;
        m_automatchNamesList.append(playerName);
        m_automatchNamesListChanged = true;
    }
}

void LobbyEventReader::checkPatyState()
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

            if (line.contains("LIE_QuitGame"))
            {
                emit quitFromParty();
                qInfo(logInfo()) << "Quit from paty";
                break;
            }

            if (line.contains("LIE_JoinGame"))
            {
                isHostedGame = false;
                emit joinToParty();
                qInfo(logInfo()) << "Join to paty";
                break;
            }

            if (line.contains("LIE_HostGame"))
            {
                isHostedGame = true;
                emit hostParty();
                qInfo(logInfo()) << "Hosting party";
                break;
            }
            counter--;
        }
    }
}

void LobbyEventReader::setCurrentGame(GamePath *newCurrentGame)
{
    m_currentGame = newCurrentGame;
}

void LobbyEventReader::setSessionIdReceived(bool newSessionIdReceived)
{
    m_sessionIdReceived = newSessionIdReceived;
}

void LobbyEventReader::setSessionIdRequested(bool newSessionIdRequested)
{
    m_sessionIdRequested = newSessionIdRequested;
}

bool LobbyEventReader::automatchProcessed() const
{
    return m_automatchProcessed;
}


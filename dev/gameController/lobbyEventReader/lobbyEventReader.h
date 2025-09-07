#ifndef LOBBYEVENTREADER_H
#define LOBBYEVENTREADER_H

#include <QObject>
#include <QTimer>
#include <logger.h>
#include <baseTypes.h>

enum eventType
{
    playerConnected,
    connectedToGame,
    hostedGame,
    quitGame
};

class LobbyEventReader : public QObject
{
    Q_OBJECT
public:
    explicit LobbyEventReader(QObject *parent = nullptr);

signals:
    void playerConnected();                 //игрок присоединился к игре которая создана не нами
    void playerDisconnected();
    void playerKicked();

    void joinToParty();
    void hostParty();
    void quitFromParty();
    void requestSessionId();

    void automatchPlayersListChanged(QStringList automatchPlayersList);
    void automatchModeChanged(bool automachMode);

    void findIgnoredPlayersId(QStringList automatchNamesList);

public slots:
    void activateReading(bool activated);
    void receiveCurrentMissionState(GameMissionState gameCurrentState);
    void receiveCurrentPlayerId(QString id);

private slots:
    void readLobbyEvents();
    void readAutomatchEvents();
    void tryRequestSessionId();

    void requestIgnoredPlayers();

public:
    void checkPatyState();
    void setCurrentGame(GamePath *newCurrentGame);
    void setSessionIdReceived(bool newSessionIdReceived);
    bool sessionIdRequested() const;
    void setSessionIdRequested(bool newSessionIdRequested);
    bool automatchProcessed() const;

private:
    void parseAytomatchPlayers(QString str);
    void parseAutomatchPlayerName(QString str);

private:
    QTimer* m_lobbyEventsReadTimer;
    QString m_preLastLogTime = "baneblade";
    QString m_lastLogTime;

    QString m_preLastAutomatchLogTime = "baneblade";
    QString m_lastAutomatchLogTime;

    QStringList m_automatchPlayersList;
    QStringList m_automatchNamesList;

    QString m_localPlayerId = "";
    QTimer m_ignotingPlayersListRequestTimer;

    bool isHostedGame = false;
    bool m_readingActivated = false;

    bool m_automatchPlayersListChanged = false;
    bool m_automatchNamesListChanged = false;

    const GamePath* m_currentGame;

    bool m_sessionIdReceived = false;
    bool m_sessionIdRequested = false;
    bool m_automatchProcessed = false;


};

#endif // LOBBYEVENTREADER_H

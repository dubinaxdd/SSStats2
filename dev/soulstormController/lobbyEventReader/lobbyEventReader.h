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

public slots:
    void activateReading(bool activated);


    void receiveCurrentMissionState(SsMissionState gameCurrentState);

private slots:
    void readLobbyEvents();

public:
    void checkPatyState();

    void setCurrentGame(GamePath *newCurrentGame);

private:
    QTimer* m_lobbyEventsReadTimer;
    QString m_preLastLogTime = "baneblade";
    QString m_lastLogTime;

    bool isHostedGame = false;

    bool m_readingActivated = false;

    const GamePath* m_currentGame;

};

#endif // LOBBYEVENTREADER_H

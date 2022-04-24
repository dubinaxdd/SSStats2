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
    explicit LobbyEventReader(QString sspath, QObject *parent = nullptr);

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

private:
    QTimer* m_lobbyEventsReadTimer;
    QString m_ssPath;
    QString m_preLastLogTime = "baneblade";
    QString m_lastLogTime;

    bool isHostedGame = false;

    bool m_readingActivated = false;

};

#endif // LOBBYEVENTREADER_H

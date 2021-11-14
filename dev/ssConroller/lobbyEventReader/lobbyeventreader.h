#ifndef LOBBYEVENTREADER_H
#define LOBBYEVENTREADER_H

#include <QObject>
#include <QTimer>


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
    void playerConnectedToHostedGame(int ssId, int playerPosition);     //игрок присоединился к игре которую создали мы
    void joinToParty();
    void hostParty();
    void quitFromParty();

public slots:
    void activateReading(bool activated);
    void onLoadStarted();
    void onGameStopped();

private slots:
    void readLobbyEvents();

private:
    QTimer* m_lobbyEventsReadTimer;
    QString m_ssPath;
    QString m_preLastLogTime = "baneblade";
    QString m_lastLogTime;

    bool isHostedGame = false;

    bool m_readingActivated = false;

};

#endif // LOBBYEVENTREADER_H

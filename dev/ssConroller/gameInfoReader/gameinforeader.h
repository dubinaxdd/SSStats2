#ifndef GAMEINFOREADER_H
#define GAMEINFOREADER_H

#include <QObject>
#include <QTimer>

class GameInfoReader : public QObject
{
    Q_OBJECT
public:
    explicit GameInfoReader(QString sspath, QObject *parent = nullptr);
    void ssWindowClosed();
    bool getGameInitialized();
    void checkGameInitialize();


private slots:
    void readGameInfo();
    void readGameParametresAfterStop();


signals:
    void gameStoped();
    void gameStarted();
    void gameInitialized();
    void ssShutdown();
    void startingMission();

private:
    QTimer* m_gameInfoReadTimer;
    QString m_ssPath;

    bool m_gameStoped = false;
    bool m_gameStarted = false;
    bool m_gamePlayback = false;
    bool m_gameLoad = false;
    bool m_scrimishMission = false;
    bool m_ssInitialized = false;
    bool m_ssShutdowned = true;
    bool m_missionStarted = false;

};

#endif // GAMEINFOREADER_H

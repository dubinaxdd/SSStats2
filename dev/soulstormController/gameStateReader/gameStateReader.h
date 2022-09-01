#ifndef GAMESTATEREADER_H
#define GAMESTATEREADER_H

#include <QObject>
#include <QTimer>
#include <baseTypes.h>
#include <logger.h>

class GameStateReader : public QObject
{ 
    Q_OBJECT
public:
    explicit GameStateReader(QString sspath, QObject *parent = nullptr);

    void ssWindowClosed();
    bool getGameInitialized();
    void checkGameInitialize();

    void setCurrentProfile(const QString &newCurrentProfile);
    void setGameLounched(bool newGameLounched);
    void stopedGame();

private slots:
    void readGameInfo();
    void readRacesTimerTimeout();

signals:
    void gameInitialized();                                 //Сигнал о инициализации игры, когда игра включилась и появилось меню
    void ssShutdown();                                      //Сигнал о выключении игры

    void sendCurrentMissionState(SsMissionState missionCurrentState);
    void sendPlayersTestStats(QVector<PlayerStats> testStats);
    void sendCurrentMode(QString currentMod);
    void sendCurrentModeVersion(QString modVersion);
    void localPlayerDroppedToObserver();

    void sendCurrentWinConditions(QVector<WinCondition>);

private:
    void checkCurrentMode();
    void readTestStatsTemp();
    void parseSsSettings(); 

    QString updateTestStatsFilePath();

    void missionLoad(QStringList* fileLines, int counter);
    void missionStarted(QStringList *fileLines, int counter);
    void missionOver();
    void missionStoped();

    bool forceMissionStoped();

    void setLocalPlayerName(QString str);
    void playerDroppedToObserver(QString str);

    void readWinConditions(QStringList *fileLines, int counter);

private:
    QTimer* m_gameInfoReadTimer;
    QTimer* m_readRacesSingleShootTimer;

    QString m_ssPath;
    QString m_currentProfile;
    QString m_currentModeVersion;
    QString m_testStatsPath;
    QString m_localPlayerName;

    SsState m_ssCurrentState = SsState::ssShutdowned;
    SsMissionState m_missionCurrentState = SsMissionState::unknown;

    bool m_gameLounched = false;
    bool m_playerDroppedToObserver = false;

    QStringList testStatsTemp;
};

#endif // GAMESTATEREADER_H

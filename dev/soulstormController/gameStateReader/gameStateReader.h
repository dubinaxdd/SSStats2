#ifndef GAMESTATEREADER_H
#define GAMESTATEREADER_H

#include <QObject>
#include <QTimer>
#include <baseTypes.h>
#include <logger.h>
#include <settingsController.h>

class GameStateReader : public QObject
{ 
    Q_OBJECT
public:
    explicit GameStateReader(SettingsController *settingsController, QString sspath, QObject *parent = nullptr);

    void ssWindowClosed();
    bool getGameInitialized();
    void checkGameInitialize();

    void setCurrentProfile(const QString &newCurrentProfile);
    void setGameLounched(bool newGameLounched);
    void stopedGame();

public slots:
    void receivePlyersRankedState(QVector<PlyersRankedState> plyersRankedState);

private slots:
    void readGameInfo();
    void readRacesTimerTimeout();

signals:
    void gameInitialized();                                 //Сигнал о инициализации игры, когда игра включилась и появилось меню
    void ssShutdown();                                      //Сигнал о выключении игры

    void sendCurrentMissionState(SsMissionState missionCurrentState);
    void sendPlayersTestStats(QVector<PlayerStats> testStats);
    void sendCurrentMod(QString currentMod);
    void sendCurrentModVersion(QString modVersion);
    void localPlayerDroppedToObserver();

    void sendCurrentWinConditions(QVector<WinCondition>);
    void sendGameRankedMode(bool gameRankedMode);

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

    void determinateRankedMode(QVector<PlayerStats> playerStats);

private:
    SettingsController *m_settingsController;

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

    QVector<PlyersRankedState> m_plyersRankedState;
    bool m_lockRanked = false;
    bool m_rankedMode = true;
};

#endif // GAMESTATEREADER_H

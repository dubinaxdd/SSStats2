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
    explicit GameStateReader(SettingsController *settingsController, QObject *parent = nullptr);

    void ssWindowClosed();
    bool getGameInitialized();
    void checkGameInitialize();

    void setCurrentProfile(const QString &newCurrentProfile);
    void setGameLounched(bool newGameLounched);
    void stopedGame();

    void setCurrentGame(GamePath *newCurrentGame);

public slots:
    void receivePlyersRankedState(QVector<PlyersRankedState> plyersRankedState);

private slots:
    void readGameInfo();
    void readRacesTimerTimeout();

signals:
    void gameInitialized();                                 //Сигнал о инициализации игры, когда игра включилась и появилось меню
    void ssShutdown();                                      //Сигнал о выключении игры

    void sendCurrentMissionState(GameMissionState missionCurrentState);
    void sendPlayersTestStats(QVector<PlayerStats> testStats);
    void sendCurrentMod(QString currentMod);
    void sendCurrentModVersion(QString modVersion);
    void localPlayerDroppedToObserver();
    void sendCurrentWinConditions(QVector<WinCondition>);
    void sendRankedState(bool gameRankedMode);
    void sendRequestParametres(DowServerRequestParametres requestParametres);
    void matchIdParsed(QString matchId);

private:
    void checkCurrentMode();
    void readTestStatsTemp();
    void parseGmaeSettings();
    void readCheckSumm(QString line);
    QString readParam(QString& line, QString pattern);

    QString updateTestStatsFilePath();

    void missionLoad(QStringList* fileLines, int counter);
    void missionStarted(QStringList *fileLines, int counter);
    void missionOver();
    void missionStoped(QStringList* fileLines, int counter);
    bool forceMissionStoped();
    void setLocalPlayerName(QString str);
    void playerDroppedToObserver(QString str);
    void readWinConditions(QStringList *fileLines, int counter);
    void determinateRankedMode(QVector<PlayerStats> playerStats);

private:
    SettingsController *m_settingsController;

    QTimer* m_gameInfoReadTimer;
    QTimer* m_readRacesSingleShootTimer;

    QString m_currentProfile;
    QString m_currentModeVersion;
    QString m_testStatsPath;
    QString m_localPlayerName;
    QString m_lastMatchId;

    GameState m_ssCurrentState = GameState::gameShutdowned;
    GameMissionState m_missionCurrentState = GameMissionState::unknown;

    bool m_gameLounched = false;
    bool m_playerDroppedToObserver = false;

    QStringList testStatsTemp;

    QVector<PlyersRankedState> m_plyersRankedState;
    bool m_lockRanked = false;
    bool m_rankedMode = true;

    bool m_dataChecksummReaded = false;

    const GamePath* m_currentGame;
};

#endif // GAMESTATEREADER_H

#ifndef WARNINGSLOGREADER_H
#define WARNINGSLOGREADER_H

#include <QObject>
#include <QTimer>
#include <baseTypes.h>
#include <logger.h>

class WarningsLogReader : public QObject
{ 
    Q_OBJECT
public:
    explicit WarningsLogReader(QString sspath, QObject *parent = nullptr);

    void ssWindowClosed();
    bool getGameInitialized();
    void checkGameInitialize();

    void setCurrentProfile(const QString &newCurrentProfile);
    void setGameLounched(bool newGameLounched);
    void stopedGame();

private slots:
    void readGameInfo();
    void readReplayDataAfterStop();
    void readRacesTimerTimeout();

public slots:
    void receiveAverrageApm(int apm);
    void receivePlayresStemIdFromScanner(QList<SearchStemIdPlayerInfo> playersInfoFromScanner , int playersCount);
    void onQuitParty();

signals:
    /*void loadStarted(SsGameState gameCurrentState);       //Сигнал о начале загрузки игры/реплея/сохраненки
    void startingMission(SsGameState gameCurrentState);     //Сигнал о старте миссии после загрузки
    void gameOver();                                        //Сигнал о победе какой-то из сторон
    void gameStopped();                                     //Сигнал о завершении игры и выхода в меню
 */
    void gameInitialized();                                 //Сигнал о инициализации игры, когда игра включилась и появилось меню
    void ssShutdown();                                      //Сигнал о выключении игры


    void sendCurrentMissionState(SsMissionState gameCurrentState);
    void sendPlayersTestStats(QVector<PlayerStats> testStats);
    void sendReplayToServer(SendingReplayInfo replayInfo);
    void sendNotification(QString warningString, bool isWarning);
    void sendCurrentModeVersion(QString modVersion);
    void localPlayerDroppedToObserver();

private:
    void checkCurrentMode();
    bool checkMissionSettingsValide(int gameType);
    void readTestStatsTemp();
    void parseSsSettings(); 

    QString updaTetestStatsFilePath();

    void missionLoad(QStringList* fileLines, int counter);
    void missionStarted(QStringList *fileLines, int counter);
    void missionOver();
    void missionStoped();

    bool forceMissionStoped();

    void setLocalPlayerName(QString str);
    void playerDroppedToObserver(QString str);

    void readWinCondotions(QStringList *fileLines, int counter);

    bool checkEqualNames(QStringList* playerNames);
    bool checkAi(QVector<PlayerStats> *playerStats);
    bool checkWinner(QVector<PlayerStats> *playerStats);
    bool checkEqualNamesInStats();

private:
    QTimer* m_gameInfoReadTimer;
    QTimer* m_readRacesSingleShootTimer;

    QString m_ssPath;
    QString m_currentProfile;
    QString m_currentMode;
    QString m_currentModeVersion;

    QString m_testStatsPath;

    QString m_localPlayerName;

    QVector<WinCondition> m_winCoditionsVector;

    SsState m_ssCurrentState = SsState::ssShutdowned;
    SsMissionState m_missionCurrentState = SsMissionState::unknown;

    int m_lastAverrageApm = 0;

    bool m_gameLounched = false;
    bool m_lastGameSettingsValide = false;
    bool m_gameWillBePlayedInOtherSession = false;
    bool m_playerDroppedToObserver = false;

    int m_playersCountFromScanner;
    QList<SearchStemIdPlayerInfo> m_playersInfoFromScanner;

    QStringList testStatsTemp;
};

#endif // WARNINGSLOGREADER_H

#ifndef REPLAYDATACOLLECTOR_H
#define REPLAYDATACOLLECTOR_H

#include <QObject>
#include <baseTypes.h>
#include <QTimer>
#include <repreader.h>

class ReplayDataCollector : public QObject
{
    Q_OBJECT
public:
    explicit ReplayDataCollector(QObject *parent = nullptr);
    void setCurrentGame(GamePath *newCurrentGame);

public slots:
    void receiveCurrentMissionState(GameMissionState missionCurrentState);
    void receiveAverrageApm(int apm);
    void receivePlayresInfoFromDowServer(QList<PlayerInfoFromDowServer> playersInfoFromDowServer );
    void onQuitParty();
    void receiveCurrentMod(QString currentMode);
    void receiveCurrentModVersion(QString version);
    void receiveCurrentGameVersion(QString version);
    void receiveCurrentWinConditions(QVector<WinCondition> winConditions);
    void receiveGameResults(QJsonObject gameResults, SendingReplayInfo lastGameResult);
    void setGameId(QString gameId);
    void setRankedState(bool newRankedState);
    void receivePlyersRankedState(QVector<PlyersRankedState> plyersRankedState);

signals:
    void sendReplayToServer(SendingReplayInfo replayInfo);
    void sendNotification(QString warningString, bool isWarning);
    void requestGameResults(SendingReplayInfo lastGameResults);
    void sendRankedState(bool isRanked);

private:
    bool checkEqualNames(QStringList* playerNames);
    bool checkAi(QVector<PlayerStats> *playerStats);
    bool checkWinner(QVector<PlayerStats> *playerStats);
    bool checkEqualNamesInStats();
    void readReplayData();
    Race getRaceByNumber(int number);

    bool readSoulstormReplayData(SendingReplayInfo replayInfo);
    void readDefinitiveReplayData();

    bool checkMissionSettingsValide(int gameType);
    QString updateTestStatsFilePath();
    void parseGameResults(QJsonObject gameResults,  SendingReplayInfo lastGameResult);

    void determinateRankedState();
    QString getDowstatsTempReplayPath();

private:
    QVector<WinCondition> m_winCoditionsVector;
    int m_lastAverrageApm = 0;
    QList<PlayerInfoFromDowServer> m_playersInfoFromDowServer;
    QString m_currentMode;
    QString m_currentModVerion;
    QString m_currentGameVersion;
    QString m_testStatsPath;
    QString m_lastGameId;
    bool m_gameWillBePlayedInOtherSession = true;
    bool m_rankedState = true;

    const GamePath* m_currentGame;
    QTimer m_findGameResultsTimer;
   // QString m_lastReplayPath;
    SendingReplayInfo m_lastReplayInfo;

    QVector<PlyersRankedState> m_plyersRankedState;
};

#endif // REPLAYDATACOLLECTOR_H

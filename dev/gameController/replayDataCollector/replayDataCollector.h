#ifndef REPLAYDATACOLLECTOR_H
#define REPLAYDATACOLLECTOR_H

#include <QObject>
#include <baseTypes.h>
#include <QTimer>

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
    void receiveCurrentWinConditions(QVector<WinCondition> winConditions);
    void setCurrentPlayersId(const QStringList &newCurrentPlayersId);
    void receiveGameResults(QString gameResults);

signals:
    void sendReplayToServer(SendingReplayInfo replayInfo);
    void sendNotification(QString warningString, bool isWarning);
    void requestGameResults(QStringList playersIdList);

private:
    bool checkEqualNames(QStringList* playerNames);
    bool checkAi(QVector<PlayerStats> *playerStats);
    bool checkWinner(QVector<PlayerStats> *playerStats);
    bool checkEqualNamesInStats();
    void readReplayData();
    Race getRaceByNumber(int number);

    bool readSoulstormReplayData();
    void readDefinitiveReplayData();

    bool checkMissionSettingsValide(int gameType);
    QString updateTestStatsFilePath();
    void parseGameResults(QString gameResults);

private:
    QVector<WinCondition> m_winCoditionsVector;
    int m_lastAverrageApm = 0;
    QList<PlayerInfoFromDowServer> m_playersInfoFromDowServer;
    QString m_currentMode;
    QString m_currentModVerion;
    QString m_testStatsPath;
    bool m_gameWillBePlayedInOtherSession = true;
    QStringList m_currentPlayersId;

    const GamePath* m_currentGame;

    QList<FinalState> m_playerFinalStateList;

    QTimer m_findGameResultsTimer;

};

#endif // REPLAYDATACOLLECTOR_H

#ifndef REPLAYDATACOLLECTOR_H
#define REPLAYDATACOLLECTOR_H

#include <QObject>
#include <baseTypes.h>

class ReplayDataCollector : public QObject
{
    Q_OBJECT
public:
    explicit ReplayDataCollector(QString ssPath, QObject *parent = nullptr);

    void readReplayData();

public slots:
    void receiveCurrentMissionState(SsMissionState missionCurrentState);
    void receiveAverrageApm(int apm);
    void receivePlayresInfoFromDowServer(QList<PlayerInfoFromDowServer> playersInfoFromDowServer , int playersCount);
    void onQuitParty();
    void receiveCurrentMode(QString currentMode);
    void receiveCurrentWinConditions(QVector<WinCondition> winCondotoins);


signals:
    void sendReplayToServer(SendingReplayInfo replayInfo);
    void sendNotification(QString warningString, bool isWarning);

private:
    bool checkEqualNames(QStringList* playerNames);
    bool checkAi(QVector<PlayerStats> *playerStats);
    bool checkWinner(QVector<PlayerStats> *playerStats);
    bool checkEqualNamesInStats();

    bool checkMissionSettingsValide(int gameType);
    QString updateTestStatsFilePath();

private:
    QVector<WinCondition> m_winCoditionsVector;
    int m_lastAverrageApm = 0;
    QList<PlayerInfoFromDowServer> m_playersInfoFromScanner;
    QString m_ssPath;
    QString m_currentMode;
    QString m_testStatsPath;
    bool m_gameWillBePlayedInOtherSession = true;

};

#endif // REPLAYDATACOLLECTOR_H

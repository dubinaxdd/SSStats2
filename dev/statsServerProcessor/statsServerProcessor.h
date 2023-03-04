#ifndef STATSSERVERPROCESSOR_H
#define STATSSERVERPROCESSOR_H

#include <QObject>
#include <QNetworkAccessManager>
#include <baseTypes.h>
#include <QImage>
#include <QSharedPointer>
#include <QTimer>
#include <logger.h>
#include <settingsController.h>

class StatsServerProcessor : public QObject
{
    Q_OBJECT
public:
    explicit StatsServerProcessor(SettingsController* settingsController, QString ssPath, QString steamPath, QObject *parent = nullptr);

    void parseCurrentPlayerSteamId();
    void getPlayerStatsFromServer(QSharedPointer<QList<ServerPlayerStats> > playersInfo);
    void getPlayersMediumAvatar(QSharedPointer<QList<ServerPlayerStats>> playersInfo);

signals:
    void sendServerPlayrStats(ServerPlayerStats serverPlayrStats);
    void sendPlayersCount(int playersCount);
    void sendCurrentPlayerHostState(bool);
    void sendCurrentPlayerSteamID(QString steamID);
    void sendNotification(QString text, bool isWarning);
    void sendStatisticModName(QString statisticModName);

public slots:
    void receivePlayresInfoFromDowServer(QList<PlayerInfoFromDowServer> playersInfoInfoFromDowServer , int playersCount);
    void sendReplayToServer(SendingReplayInfo replayInfo);
    void receiveRankedMode(bool reankedMode);
    void receiveCurrentMod(QString modName);

private slots:
    void receivePlayerStatsFromServer(QNetworkReply *reply, QSharedPointer<QList<ServerPlayerStats> > playersInfo);
    void receivePlayerMediumAvatar(QNetworkReply* reply, QSharedPointer<ServerPlayerStats> playerInfo);
    void currentPlayerStatsRequestTimerTimeout();
    void onSettingsLoaded();

private:
    void registerPlayer(QString name, QString sid, bool init);
    QString GetRandomString() const;
    QString CRC32fromByteArray( const QByteArray & array );

private:
    SettingsController* m_settingsController;
    QTimer *m_currentPlayerStatsRequestTimer;
    QString m_steamPath;
    QString m_ssPath;
    QString m_clientVersion;
    QMap<QString, QString> AllPlayersInfo;
    QNetworkAccessManager *m_networkManager;
    QSharedPointer <QList<ServerPlayerStats>> m_currentPlayerStats;
    QList<ServerPlayerStats*> m_playerStats;
    bool m_rankedMode = true;

    QString m_machineUniqueId = "";
    QString m_currentMode = "";
};

#endif // STATSSERVERPROCESSOR_H

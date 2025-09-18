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
    explicit StatsServerProcessor(SettingsController* settingsController, GamePath* currentGame, QString steamPath, QObject *parent = nullptr);

    void parseCurrentPlayerSteamId();
    void getPlayerStatsFromServer(QSharedPointer<QList<ServerPlayerStats> > playersInfo);
    void getPlayersMediumAvatar(QSharedPointer<QList<ServerPlayerStats>> playersInfo);

signals:
    void sendServerPlayerStats(ServerPlayerStats serverPlayrStats);
    void sendCurrentPlayerSteamID(QString steamID);
    void sendNotification(QString text, bool isWarning);
    void sendStatisticModName(QString statisticModName);
    void sendRankDiversion(RankDiversion rankDiversion);
    void sendActualClientVersion(QString version);
    void sendSoftwareBanActivated(QString reason);
    void replaySended();

public slots:
    void receivePlayresInfoFromDowServer(QList<PlayerInfoFromDowServer> playersInfoInfoFromDowServer );
    void sendReplayToServer(SendingReplayInfo replayInfo);
    void receiveCurrentMod(QString modName);
    void receiveAuthKey(const QString &newAuthKey);

private slots:
    void receivePlayerStatsFromServer(QNetworkReply *reply, QSharedPointer<QList<ServerPlayerStats> > playersInfo);
    void receivePlayerMediumAvatar(QNetworkReply* reply, QSharedPointer<ServerPlayerStats> playerInfo);
    void currentPlayerStatsRequestTimerTimeout();
    void receiveRankDiversion(QNetworkReply* reply);
    void onRankDiversionTimerTimeout();
    void requestClientLastVersion();
    void receiveClientLastVersion(QNetworkReply *reply);
    void onSettingsLoaded();

private:
    void registerPlayer(QString name, QString sid, bool init);
    QString GetRandomString() const;
    QString CRC32fromByteArray( const QByteArray & array );
    void requestRankDiversion();


private:
    GamePath* m_currentGame;
    SettingsController* m_settingsController;
    QTimer *m_currentPlayerStatsRequestTimer;
    QTimer *m_rankDiversionTimer;
    QString m_steamPath;
    QString m_clientVersion;
    QString m_authKey = "";
    QMap<QString, QString> AllPlayersInfo;
    QNetworkAccessManager *m_networkManager;
    QSharedPointer <QList<ServerPlayerStats>> m_currentPlayerStats;
    bool m_rankedMode = true;

    QString m_machineUniqueId = "";
    QString m_currentMod = "";

    bool m_softwareBanActivated = false;
};

#endif // STATSSERVERPROCESSOR_H

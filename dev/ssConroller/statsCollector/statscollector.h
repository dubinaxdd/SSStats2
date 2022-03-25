#ifndef STATSCOLLECTOR_H
#define STATSCOLLECTOR_H

#include <QObject>
#include <QNetworkAccessManager>
#include <baseTypes.h>
#include <QImage>
#include <QSharedPointer>
#include <QTimer>
#include <logger.h>

class StatsCollector : public QObject
{
    Q_OBJECT
public:
    explicit StatsCollector(QString ssPath, QString steamPath, QObject *parent = nullptr);

    void parseCurrentPlayerSteamId();
    void getPlayerStatsFromServer(QSharedPointer<ServerPlayerStats> playerInfo);
    void getPlayerMediumAvatar(QString url, QSharedPointer<ServerPlayerStats> playerInfo);
    void setCurrentPlayerAccepted(bool newCurrentPlayerAccepted);

signals:
    void sendServerPlayrStats(ServerPlayerStats serverPlayrStats);
    void sendPlayersCount(int playersCount);
    void sendCurrentPlayerHostState(bool);

public slots:
    void receivePlayresStemIdFromScanner(QList<SearchStemIdPlayerInfo> playersInfoFromScanner , int playersCount);
    void receivePlayerStemIdForHostedGame(SearchStemIdPlayerInfo playerInfoFromScanner);
    void sendReplayToServer(SendingReplayInfo replayInfo);

private slots:
    void receiveSteamInfoReply(QNetworkReply* reply);
    void receivePlayerStatsFromServer(QNetworkReply *reply, QSharedPointer<ServerPlayerStats> playerInfo);
    void receivePlayerMediumAvatar(QNetworkReply* reply, QSharedPointer<ServerPlayerStats> playerInfo);
    void receivePlayerSteamData(QNetworkReply* reply, QSharedPointer<ServerPlayerStats> playerInfo);

    void currentPlayerStatsRequestTimerTimeout();


private:
    void registerPlayer(QString name, QString sid, bool init);
    QString GetRandomString() const;
    QString CRC32fromByteArray( const QByteArray & array );

private:

    QTimer *m_currentPlayerStatsRequestTimer;

    QString m_steamPath;
    QString m_ssPath;
    bool m_currentPlayerAccepted = false;
    QMap<QString, QString> AllPlayersInfo;

    QNetworkAccessManager *m_networkManager;

    QSharedPointer<ServerPlayerStats> m_currentPlayerStats;

    QList<ServerPlayerStats*> m_playerStats;

    QString m_clientVersion;

};

#endif // STATSCOLLECTOR_H

#ifndef STATSCOLLECTOR_H
#define STATSCOLLECTOR_H

#include <QObject>
#include <QNetworkAccessManager>
#include "../../baseTypes/baseTypes.h"
#include <QImage>
#include <QSharedPointer>

class StatsCollector : public QObject
{
    Q_OBJECT
public:
    explicit StatsCollector(QString steamPath, QObject *parent = nullptr);

    void parseCurrentPlayerSteamId();
    void getPlayerStatsFromServer(QSharedPointer<ServerPlayerStats> playerInfo);
    void getPlayerMediumAvatar(QString url, QSharedPointer<ServerPlayerStats> playerInfo);




signals:                           
    void sendServerPlayrStats(ServerPlayerStats serverPlayrStats);
    void sendPlayersCount(int playersCount);

public slots:
    void receivePlayresStemIdFromScanner(QList<SearchStemIdPlayerInfo> playersInfoFromScanner );

private slots:
    void receiveSteamInfoReply(QNetworkReply* reply);
    void receivePlayerStatsFromServer(QNetworkReply *reply, QSharedPointer<ServerPlayerStats> playerInfo);
    void receivePlayerMediumAvatar(QNetworkReply* reply, QSharedPointer<ServerPlayerStats> playerInfo);

    void receivePlayerSteamData(QNetworkReply* reply, QSharedPointer<ServerPlayerStats> playerInfo);

private:
    QString m_steamPath;
    bool m_currentPlayerAccepted;
    QMap<QString, QString> AllPlayersInfo;

    QNetworkAccessManager *m_networkManager;

    QSharedPointer<ServerPlayerStats> m_currentPlayerStats;

    QList<ServerPlayerStats*> m_playerStats;

};

#endif // STATSCOLLECTOR_H

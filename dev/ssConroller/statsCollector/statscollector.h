#ifndef STATSCOLLECTOR_H
#define STATSCOLLECTOR_H

#include <QObject>
#include <QNetworkAccessManager>
#include "../../baseTypes/baseTypes.h"
#include <QImage>

class StatsCollector : public QObject
{
    Q_OBJECT
public:
    explicit StatsCollector(QString steamPath, QObject *parent = nullptr);

    void parseCurrentPlayerSteamId();
    void getPlayerStatsFromServer(ServerPlayerStats* playerInfo);
    void getPlayerMediumAvatar(QString url, ServerPlayerStats *playerInfo);




signals:                           
    void sendServerPlayrStats(ServerPlayerStats serverPlayrStats);
    void sendPlayersCount(int playersCount);

public slots:
    void receivePlayresStemIdFromScanner(QList<SearchStemIdPlayerInfo> playersInfoFromScanner );

private slots:
    void receiveSteamInfoReply(QNetworkReply* reply);
    void receivePlayerStatsFromServer(QNetworkReply *reply, ServerPlayerStats* playerInfo);
    void receivePlayerMediumAvatar(QNetworkReply* reply, ServerPlayerStats *playerInfo);

    void receivePlayerSteamData(QNetworkReply* reply, ServerPlayerStats* playerInfo);

private:
    QString m_steamPath;
    bool m_currentPlayerAccepted;
    QMap<QString, QString> AllPlayersInfo;

    QNetworkAccessManager *m_networkManager;

    ServerPlayerStats m_currentPlayerStats;

    QList<ServerPlayerStats> m_playerStats;

};

#endif // STATSCOLLECTOR_H

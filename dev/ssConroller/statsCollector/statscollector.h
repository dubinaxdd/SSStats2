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
    void getPlayerStatsFromServer(ServerPlayrStats* playerInfo);
    void getPlayerMediumAvatar(QString url, ServerPlayrStats *playerInfo);


signals:                           
    void sendServerPlayrStats(ServerPlayrStats serverPlayrStats);


private slots:
    void receiveSteamInfoReply(QNetworkReply* reply);
    void receivePlayerStatsFromServer(QNetworkReply *reply, ServerPlayrStats* playerInfo);
    void receivePlayerMediumAvatar(QNetworkReply* reply, ServerPlayrStats *playerInfo);

private:
    QString m_steamPath;
    bool m_currentPlayerAccepted;

    QNetworkAccessManager *m_networkManager;

    ServerPlayrStats m_currentPlayerStats;

};

#endif // STATSCOLLECTOR_H

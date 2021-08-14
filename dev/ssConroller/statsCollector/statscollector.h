#ifndef STATSCOLLECTOR_H
#define STATSCOLLECTOR_H

#include <QObject>
#include <QNetworkAccessManager>
#include "../../baseTypes/baseTypes.h"

class StatsCollector : public QObject
{
    Q_OBJECT
public:
    explicit StatsCollector(QString steamPath, QObject *parent = nullptr);

    void parseCurrentPlayerSteamId();
    void getPlayrStatsFromServer(QString steamId);


signals:
    void sendServerPlayrStats(ServerPlayrStats serverPlayrStats);


private slots:
    void receiveSteamInfoReply(QNetworkReply* reply);
    void receivePlayrStatsFromServer(QNetworkReply* reply, QString steamId);

private:
    QString m_steamPath;
    bool m_currentPlayerAccepted;

    QString currentPlayerSteamId = "";

    QNetworkAccessManager *m_networkManager;

};

#endif // STATSCOLLECTOR_H

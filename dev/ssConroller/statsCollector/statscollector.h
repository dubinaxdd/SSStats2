#ifndef STATSCOLLECTOR_H
#define STATSCOLLECTOR_H

#include <QObject>
#include <QNetworkAccessManager>

class StatsCollector : public QObject
{
    Q_OBJECT
public:
    explicit StatsCollector(QString steamPath, QObject *parent = nullptr);

    void parseCurrentPlayerSteamId();
    void getPlayrStatsFromServer(QString steamId);


signals:


private slots:
    void receiveSteamInfoReply(QNetworkReply* reply);
    void receivePlayrStatsFromServer(QNetworkReply* reply);

private:
    QString m_steamPath;
    bool m_currentPlayerAccepted;

    QNetworkAccessManager *m_networkManager;

};

#endif // STATSCOLLECTOR_H

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


signals:


private slots:
    void receiveManagerReply(QNetworkReply* reply);

private:
    QString m_steamPath;
    bool m_currentPlayerAccepted;

    QNetworkAccessManager *m_networkManager;

};

#endif // STATSCOLLECTOR_H

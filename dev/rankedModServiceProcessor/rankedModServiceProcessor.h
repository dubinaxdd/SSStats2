#ifndef RANKEDMODSERVICEPROCESSOR_H
#define RANKEDMODSERVICEPROCESSOR_H

#include <QObject>
#include <QTimer>
#include <QNetworkAccessManager>
#include <baseTypes.h>
#include <QNetworkReply>
#include <QMap>

class RankedModServiceProcessor : public QObject
{
    Q_OBJECT
public:
    explicit RankedModServiceProcessor(QObject *parent = nullptr);

signals:
    void sendPlyersRankedState(QVector<PlyersRankedState> plyersRankedState);
    void sendOnlineCount(int onlineCount);
    void sendModsOnlineCountMap(QMap<QString, int>);

public slots:
    void setCurrentPlayerSteamIdSlot(QString currentPlayerSteamId);
    void receivePlayresInfoFromDowServer(QList<PlayerInfoFromDowServer> playersInfoInfoFromDowServer , int playersCount);

    void sendRankedMode(bool rankedMode);

private slots:
    void pingTimerTimeout();
    void rankedStateTimerTimeout();

    void receiveRankedState(QNetworkReply *reply, QVector<PlyersRankedState> plyersRankedState);
    void receivePingRecponce(QNetworkReply *reply);

private:
    bool checkReplyErrors(QNetworkReply *reply);
    void sendPingRequest();

private:
    QTimer* m_pingTimer;
    QTimer* m_rankedStateTimer;
    QNetworkAccessManager *m_networkManager;

    QString m_currentPlayerSteamId = "";
    QList<PlayerInfoFromDowServer> m_playersInfoInfoFromDowServer;
};

#endif // RANKEDMODSERVICEPROCESSOR_H

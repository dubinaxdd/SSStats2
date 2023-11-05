#ifndef RANKEDMODSERVICEPROCESSOR_H
#define RANKEDMODSERVICEPROCESSOR_H

#include <QObject>
#include <QTimer>
#include <QNetworkAccessManager>
#include <baseTypes.h>
#include <QNetworkReply>
#include <QMap>
#include <settingsController.h>

class RankedModServiceProcessor : public QObject
{
    Q_OBJECT
public:
    explicit RankedModServiceProcessor(SettingsController* settingsController, QObject *parent = nullptr);

signals:
    void sendPlyersRankedState(QVector<PlyersRankedState> plyersRankedState);
    void sendOnlineCount(int onlineCount);
    void sendModsOnlineCountMap(QMap<QString, int>);
    void sendUniquePlayersOnlineStatistic(UniqueOnlineStatistic uniqueOnlineStatistic);

public slots:
    void setCurrentPlayerSteamIdSlot(QString currentPlayerSteamId);
    void receivePlayresInfoFromDowServer(QList<PlayerInfoFromDowServer> playersInfoInfoFromDowServer );
    void sendRankedMode(bool rankedMode);
    void receiveCurrentMod(QString modName);

private slots:
    void pingTimerTimeout();
    void rankedStateTimerTimeout();
    void receiveRankedState(QNetworkReply *reply, QVector<PlyersRankedState> plyersRankedState);
    void receivePingRecponce(QNetworkReply *reply);
    void receiveUniquePlayersOnlineStatistic(QNetworkReply *reply);

    void onSettingsLoaded();

private:
    bool checkReplyErrors(QString funcName, QNetworkReply *reply);
    void sendPingRequest();
    void requestUniquePlayersOnlineStatistic();

private:
    SettingsController* m_settingsController;
    QTimer* m_pingTimer;
    QTimer* m_rankedStateTimer;
    QTimer* m_uniquePlayersOnlineStatisticTimer;
    QNetworkAccessManager *m_networkManager;
    QString m_currentMod = "";
    QString m_currentPlayerSteamId = "";
    QList<PlayerInfoFromDowServer> m_playersInfoInfoFromDowServer;
};

#endif // RANKEDMODSERVICEPROCESSOR_H

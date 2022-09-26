#ifndef RANKEDMODSERVICEPROCESSOR_H
#define RANKEDMODSERVICEPROCESSOR_H

#include <QObject>
#include <QTimer>
#include <QNetworkAccessManager>
#include <baseTypes.h>
#include <QNetworkReply>

class RankedModServiceProcessor : public QObject
{
    Q_OBJECT
public:
    explicit RankedModServiceProcessor(QObject *parent = nullptr);

signals:
    void sendPlyersRankedState(QVector<PlyersRankedState> plyersRankedState);

public slots:
    void setCurrentPlayerSteamIdSlot(QString currentPlayerSteamId);
    void receivePlayresInfoFromDowServer(QList<PlayerInfoFromDowServer> playersInfoInfoFromDowServer , int playersCount);

    void sendRankedMode(bool rankedMode);

private slots:
    void pingTimerTimeout();
    void rankedStateTimerTimeout();

    void receiveRankedState(QNetworkReply *reply, QVector<PlyersRankedState> plyersRankedState);


private:
    bool checkReplyErrors(QNetworkReply *reply);

private:
    QTimer* m_pingTimer;
    QTimer* m_rankedStateTimer;
    QNetworkAccessManager *m_networkManager;

    QString m_currentPlayerSteamId = "";
    QVector<QString> m_playersSteamIds;


};

#endif // RANKEDMODSERVICEPROCESSOR_H

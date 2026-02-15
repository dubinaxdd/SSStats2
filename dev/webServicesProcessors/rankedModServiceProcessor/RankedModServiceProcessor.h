#ifndef RANKEDMODSERVICEPROCESSOR_H
#define RANKEDMODSERVICEPROCESSOR_H

#include <AbstractWebServiceProcessor.h>
#include <QMap>

class RankedModServiceProcessor : public AbstractWebServiceProcessor
{
    Q_OBJECT
public:
    explicit RankedModServiceProcessor(SettingsController* settingsController, QObject *parent = nullptr);
    //~RankedModServiceProcessor() override;

    enum OpCode : int
    {
        PingRequest = 0,
        PingResponse,
        PlyersStateRequest,
        PlyersStateResponse,
        UniquePlayersOnlineStatisticRequest,
        UniquePlayersOnlineStatisticResponse,
        ModsOnlineCountRequest,
        ModsOnlineCountResponse
    };

signals:
    void sendPlyersRankedState(QVector<PlyersRankedState> plyersRankedState);
    void sendOnlineCount(int onlineCount);
    void sendModsOnlineCount(QList<ModOnlineCount> modOnlineCountList);
    void sendUniquePlayersOnlineStatistic(UniqueOnlineStatistic uniqueOnlineStatistic);

public slots:
    void setCurrentPlayerSteamId(QString currentPlayerSteamId);
    void setPlayresInfoFromDowServer(QList<PlayerInfoFromDowServer> playersInfoInfoFromDowServer );
    void setRankedMode(bool rankedMode);
    void setCurrentMod(QString modName);

private slots:
    void requestRankedState();
    void requestModsOnlineCount();
    void receiveRankedState(QJsonObject data);
    void receivePingRecponse(QJsonObject data);
    void receiveModsOnlineCount(QJsonObject data);
    void receiveUniquePlayersOnlineStatistic(QJsonObject data);

    void onSettingsLoaded() override;
    void readMessage(QString message) override;
    void onConnected() override;

private:
    void sendPing() override;
    void requestUniquePlayersOnlineStatistic();

private:
    QTimer m_uniquePlayersOnlineStatistic;
    QString m_currentMod = "";
    QString m_currentPlayerSteamId = "";
    QString m_currentPlayerName = "";
    QList<PlayerInfoFromDowServer> m_playersInfoInfoFromDowServer;
    bool m_rankedMode = true;
};

#endif // RANKEDMODSERVICEPROCESSOR_H

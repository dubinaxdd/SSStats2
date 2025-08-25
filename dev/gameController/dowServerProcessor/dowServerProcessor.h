#ifndef DOWSERVERPROCESSOR_H
#define DOWSERVERPROCESSOR_H

#include <QObject>
#include <QNetworkAccessManager>
#include <abstractDowServerProcessor.h>
#include <logger.h>
#include <baseTypes.h>
#include <QTimer>

class DowServerProcessor : public AbstractDowServerProcessor
{
    Q_OBJECT
public:
    explicit DowServerProcessor(QObject *parent = nullptr);
    ~DowServerProcessor(){};

    enum QueryType : int
    {
        ProfileID = 0,
        FindAdvertisements = 1,
        PlayersSids = 2,
        ChannelData = 3
    };

    void setGameType(GameType newGameType) override;

private:
    QVector<PlayerData> getPlayersInCurrentRoom(QVector<PartyData> partyDataArray);

    void rquestChannellData(int id);
    void requestProfileID(QString steamID);
    void requestFindAdvertisements();
    void requestPlayersSids(QVector<PlayerData> profilesData);

public slots:
    void setSessionID(QString sessionID);
    void setCurrentPlayerSteamID(QString steamID);
    void setCurrentModVersion(QString modVersion);
    void requestPartysData();
    void onPlayerDisconnected();

private slots:
    void receiveChannellData(QNetworkReply *reply, int id);
    void receiveProfileID(QNetworkReply *reply, QString steamID);
    void receiveFindAdvertisements(QNetworkReply *reply);
    void receivePlayersSids(QNetworkReply *reply, QVector<PlayerData> profilesData);

    void playerDiscoonectTimerTimeout();

    void checkQueue();
    void addQuery(DowServerProcessor::QueryType type);

signals:
    void sendPartysArray(QVector<PartyData> partyDataArray);
    void sendPlayersInfoFromDowServer(QList<PlayerInfoFromDowServer> playersInfo );

private:
    QVector<QueryType> m_requestsQueue;
    QTimer *m_queueTimer;
    QTimer *m_requestDataAftrePlayerDisconectTimer;
    QVector<PlayerData> m_profileIdsForQueue;

    QString m_steamID = "";
    QString m_profileID = "";
    QString m_statGroupId = "";
    QString m_modName = "winter+assault";
    QString m_modVersion = "1.0";

    bool m_needUpdateLatter = false;
};

#endif // DOWSERVERPROCESSOR_H

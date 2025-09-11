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

    void setGameType(GameType::GameTypeEnum newGameType) override;

    QList<PlayerInfoFromDowServer> lastPlayersInfo() const;

private:
    QVector<PlayerData> getPlayersInCurrentRoom(QVector<PartyData> partyDataArray);

    void rquestChannellData(int id);
    void requestProfileID(QString steamID);
    void requestFindAdvertisements();
    void requestPlayersSids(QVector<PlayerData> profilesData);

public slots:
    void setRequestParametres(DowServerRequestParametres parametres) override;
    void setSessionId(QString sessionId) override;
    void setCurrentPlayerSteamID(QString steamID);
    void setCurrentModVersion(QString modVersion);
    void requestPartysData();
    void onPlayerDisconnected();
    void onAutomatchPlayersListChanged(QStringList playersList);

private slots:
    void receiveChannellData(QNetworkReply *reply, int id);
    void receiveProfileID(QNetworkReply *reply, QString steamID);
    void receiveFindAdvertisements(QNetworkReply *reply);
    void receivePlayersSids(QNetworkReply *reply, QVector<PlayerData> profilesData);

    void playerDiscoonectTimerTimeout();

    void checkQueue();
    void addQuery(DowServerProcessor::QueryType type);

signals:
    void sendPlayersInfoFromDowServer(QList<PlayerInfoFromDowServer> playersInfo );
    void sendCurrentPlayerId(QString id);

private:
    QVector<QueryType> m_requestsQueue;
    QTimer *m_queueTimer;
    QTimer *m_requestDataAftrePlayerDisconectTimer;
    QVector<PlayerData> m_profileIdsForQueue;

    QList<PlayerInfoFromDowServer> m_lastPlayersInfo;

    QString m_steamID = "";
    QString m_profileID = "";
    QString m_statGroupId = "";
    QString m_modName = "winter+assault";
    QString m_modVersion = "1.0";

    bool m_needUpdateLatter = false;
    bool m_neeedRequestAdvertisements = false;
};

#endif // DOWSERVERPROCESSOR_H

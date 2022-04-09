#ifndef DOWSERVERPROCESSOR_H
#define DOWSERVERPROCESSOR_H

#include <QObject>
#include <QNetworkAccessManager>
#include <logger.h>
#include <baseTypes.h>

class DowServerProcessor : public QObject
{
    Q_OBJECT
public:
    explicit DowServerProcessor(QObject *parent = nullptr);

private:
    bool checkReplyErrors(QNetworkReply *reply);
    QNetworkRequest createDowServerRequest(QString url);

    void rquestChannellData(int id);
    void requestProfileID(QString steamID);
    void requestFindAdvertisements();

public slots:
    void setSessionID(QString sessionID);
    void setCurrentPlayerSteamID(QString steamID);
    void requestPartysData();

private slots:
    void receiveChannellData(QNetworkReply *reply, int id);
    void receiveProfileID(QNetworkReply *reply, QString steamID);
    void receiveFindAdvertisements(QNetworkReply *reply);


signals:
    void sendPartysArray(QVector<PartyData>);

private:
    QNetworkAccessManager *m_networkManager;
    QString m_sessionID = "";
    QString m_steamID = "";
    QString m_profileID = "";
    QString m_statGroupId = "";
    QString m_modName = "winter+assault";
    QString m_modVersion = "1.0";

};

#endif // DOWSERVERPROCESSOR_H

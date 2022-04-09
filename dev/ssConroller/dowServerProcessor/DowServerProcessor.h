#ifndef DOWSERVERPROCESSOR_H
#define DOWSERVERPROCESSOR_H

#include <QObject>
#include <QNetworkAccessManager>
#include <logger.h>

class DowServerProcessor : public QObject
{
    Q_OBJECT
public:
    explicit DowServerProcessor(QObject *parent = nullptr);


private:
    bool checkReplyErrors(QNetworkReply *reply);

    void rquestChannellData(int id);
    void requestProfileID(QString steamID);

public slots:
    void setSessionID(QString sessionID);
    void setCurrentPlayerSteamID(QString steamID);

private slots:
    void receiveChannellData(QNetworkReply *reply, int id);
    void receiveProfileID(QNetworkReply *reply, QString steamID);

private:
    QNetworkAccessManager *m_networkManager;
    QString m_sessionID = "";
    QString m_steamID = "";
    QString m_profileID = "";

};

#endif // DOWSERVERPROCESSOR_H

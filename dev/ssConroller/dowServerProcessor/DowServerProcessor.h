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
    void rquestChannellData(int id);

public slots:
    void setSessionID(QString sessionID);

private slots:
    void receiveChannellData(QNetworkReply *reply, int id);

private:
    QNetworkAccessManager *m_networkManager;
    QString m_sessionID = "";
    QString steamID = "";
    QString ssID = "";

};

#endif // DOWSERVERPROCESSOR_H

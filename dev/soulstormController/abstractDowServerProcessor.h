#ifndef ABSTRACTDOWSERVERPROCESSOR_H
#define ABSTRACTDOWSERVERPROCESSOR_H

#include <QObject>
#include <QNetworkAccessManager>
#include <logger.h>
#include <baseTypes.h>

class AbstractDowServerProcessor : public QObject
{
    Q_OBJECT
public:
    explicit AbstractDowServerProcessor(QObject *parent = nullptr);
    virtual ~AbstractDowServerProcessor() = 0;

public slots:
    void setSessionID(QString sessionID);

protected:
    bool checkReplyErrors(QString funcName, QNetworkReply *reply);
    QNetworkRequest createDowServerRequest(QString url);

protected:
    QNetworkAccessManager *m_networkManager;
    QString m_sessionID = "";

};

#endif // ABSTRACTDOWSERVERPROCESSOR_H

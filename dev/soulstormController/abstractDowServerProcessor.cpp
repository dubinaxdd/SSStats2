#include "abstractDowServerProcessor.h"
#include <QNetworkReply>
#include <QDebug>
#include <defines.h>

AbstractDowServerProcessor::AbstractDowServerProcessor(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
}

bool AbstractDowServerProcessor::checkReplyErrors(QString funcName, QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << funcName + ":" << "Connection error:" << reply->errorString();
        reply->deleteLater();
        return true;
    }

    return false;
}

QNetworkRequest AbstractDowServerProcessor::createDowServerRequest(QString url)
{
    QNetworkRequest newRequest;

    newRequest.setUrl(QUrl(url));
    newRequest.setRawHeader("Cookie", "reliclink=" + QString(RELICLINK).toLocal8Bit() + "; AWSELB=" + QString(AWSELB).toLocal8Bit());
    newRequest.setRawHeader("Host", "dow1ss-lobby.reliclink.com");
    newRequest.setRawHeader("Content-Content-Type","application/x-www-form-urlencoded; charset=UTF-8");

    QSslConfiguration sslConf;
    sslConf.setPeerVerifyMode(QSslSocket::VerifyNone);
    newRequest.setSslConfiguration(sslConf);

    return newRequest;
}

void AbstractDowServerProcessor::setSessionID(QString sessionID)
{
    m_sessionID = sessionID;
    qInfo(logInfo()) << "sessionID =" << m_sessionID;
}

AbstractDowServerProcessor::~AbstractDowServerProcessor() {}

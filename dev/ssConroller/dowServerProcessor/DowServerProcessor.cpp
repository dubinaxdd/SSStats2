#include "DowServerProcessor.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QSslConfiguration>
#include <defines.h>


DowServerProcessor::DowServerProcessor(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
    m_networkManager->setStrictTransportSecurityEnabled(false);
}

void DowServerProcessor::rquestChannellData(int id)
{
    QNetworkRequest newRequest;

    QString urlString = "https://dow1ss-lobby.reliclink.com/game/chat/joinChannel?chatroomID=1&doRetry=1&sessionID=" + m_sessionID.toLocal8Bit();

    newRequest.setUrl(QUrl(urlString));
    newRequest.setRawHeader("Cookie", "reliclink=" + QString(RELICLINK).toLocal8Bit() + "; AWSELB=" + QString(AWSELB).toLocal8Bit());
    newRequest.setRawHeader("Host", "dow1ss-lobby.reliclink.com");
    //newRequest.setRawHeader("User-Agent", "DowStats2");
    newRequest.setRawHeader("Content-Content-Type","application/x-www-form-urlencoded; charset=UTF-8");

    QSslConfiguration sslConf;
    sslConf.setPeerVerifyMode(QSslSocket::VerifyNone);
    newRequest.setSslConfiguration(sslConf);


    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveChannellData(reply, id);
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
        reply->deleteLater();
    });
}

void DowServerProcessor::setSessionID(QString sessionID)
{
    m_sessionID = sessionID;
    rquestChannellData(0);
}

void DowServerProcessor::receiveChannellData(QNetworkReply *reply, int id)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "Connection error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray replyByteArray = reply->readAll();
    reply->deleteLater();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyByteArray);

    if(!jsonDoc.isArray())
        return;

    qDebug() << jsonDoc.array();
}

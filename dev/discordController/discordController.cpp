#include "discordController.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <defines.h>

DiscordController::DiscordController(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{

    requestNews();
}

void DiscordController::requestNews()
{
    QNetworkRequest newRequest;

    newRequest.setUrl(QUrl("https://discord.com/api/v9/channels/852182655570411592/messages"));
    newRequest.setRawHeader("Authorization", QString(DISCORD_TOKEN).toLocal8Bit());
    newRequest.setRawHeader("Host", "discord.com");
    newRequest.setRawHeader("User-Agent", "Dowstats 2.0");
    newRequest.setRawHeader("Content-Type","application/json");

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveNews(reply);
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
        reply->deleteLater();
    });

}

void DiscordController::receiveNews(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "Connection error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray replyByteArray = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyByteArray);

    reply->deleteLater();
}

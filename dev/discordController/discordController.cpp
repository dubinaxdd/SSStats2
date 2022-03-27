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

}

void DiscordController::requestNews()
{
    QNetworkRequest newRequest;

    newRequest.setUrl(QUrl("https://discord.com/api/v9/channels/852182655570411592/messages"));
    newRequest.setRawHeader("Authorization", QString(DISCORD_TOKEN).toLocal8Bit());
    newRequest.setRawHeader("Host", "discord.com");
    newRequest.setRawHeader("User-Agent", "DowStats2");
    newRequest.setRawHeader("Content-Type","application/json");

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveNews(reply);
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
        reply->deleteLater();
    });

}

void DiscordController::requestUserAvatar(QString userId, QString avatarId)
{
    if (m_avatarMap.keys().contains(avatarId))
        return;

    QNetworkRequest newRequest;

    newRequest.setUrl(QUrl("https://cdn.discordapp.com/avatars/" + userId + "/" + avatarId + ".png"));

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveUserAvatar(reply, avatarId);
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

    reply->deleteLater();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyByteArray);

    if(!jsonDoc.isArray())
        return;

    QJsonArray replyJsonArray = jsonDoc.array();

    discordNewsList.clear();

    for (int i = 0; i < replyJsonArray.count(); i++)
    {
        DiscordMessage newDiscordMessage;

        if(!replyJsonArray.at(i).isObject())
            continue;

        QJsonObject newObject = replyJsonArray.at(i).toObject();

        newDiscordMessage.content = newObject.value("content").toString();

        QDateTime newDateTime;
        newDateTime.setDate(QDate::fromString(newObject.value("timestamp").toString().left(10),"yyyy-MM-dd"));
        newDateTime.setTime(QTime::fromString(newObject.value("timestamp").toString().mid(11, 8),"hh:mm:ss"));
        newDiscordMessage.timestamp = newDateTime;

        QJsonObject authorObject = newObject.value("author").toObject();
        newDiscordMessage.userId = authorObject.value("id").toString();
        newDiscordMessage.userName = authorObject.value("username").toString();
        newDiscordMessage.avatarId = authorObject.value("avatar").toString();

        requestUserAvatar(newDiscordMessage.userId, newDiscordMessage.avatarId);

        discordNewsList.append(std::move(newDiscordMessage));
    }


    emit sendNews(discordNewsList);
}

void DiscordController::receiveUserAvatar(QNetworkReply *reply, QString avatarId)
{

    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "Connection error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray replyByteArray = reply->readAll();

    reply->deleteLater();

    QImage avatar = QImage::fromData(replyByteArray);

    if (avatar.isNull())
        return;

    m_avatarMap.insert(avatarId, avatar);
}

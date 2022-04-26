#include "discordController.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <defines.h>

#define REQUEST_TIMER_INTERVAL 1000
#define REQUEST_TIMER_INTERVAL2 5000

DiscordController::DiscordController(SettingsController* settingsController, QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_requestTimer(new QTimer(this))
    , m_settingsController(settingsController)
{
    QObject::connect(m_settingsController, &SettingsController::settingsLoaded, this, &DiscordController::onSettingsLoaded, Qt::QueuedConnection);
    QObject::connect(m_requestTimer, &QTimer::timeout, this, &DiscordController::requestMessages, Qt::QueuedConnection);

    m_requestTimer->setInterval(REQUEST_TIMER_INTERVAL);
    m_requestTimer->start();
}

void DiscordController::requestNews()
{
    QNetworkRequest newRequest;

    QString urlString = "https://discord.com/api/v9/channels/" + QString(NEWS_CHANNEL_ID).toLocal8Bit() + "/messages";

    newRequest.setUrl(QUrl(urlString));
    newRequest.setRawHeader("Authorization", QString(DISCORD_TOKEN).toLocal8Bit());
    newRequest.setRawHeader("Host", "discord.com");
    newRequest.setRawHeader("User-Agent", "DowStats2");
    newRequest.setRawHeader("Content-Type","application/json");

    m_readyToRequest = false;

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveNews(reply);
        m_readyToRequest = true;
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
        reply->deleteLater();
        m_readyToRequest = true;
    });

}

void DiscordController::requestEvents()
{
    QNetworkRequest newRequest;

    QString urlString = "https://discord.com/api/v9/channels/" + QString(EVENTS_CHANNEL_ID).toLocal8Bit() + "/messages";

    newRequest.setUrl(QUrl(urlString));
    newRequest.setRawHeader("Authorization", QString(DISCORD_TOKEN).toLocal8Bit());
    newRequest.setRawHeader("Host", "discord.com");
    newRequest.setRawHeader("User-Agent", "DowStats2");
    newRequest.setRawHeader("Content-Type","application/json");

    m_readyToRequest = false;

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveEvents(reply);
        m_readyToRequest = true;
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
        reply->deleteLater();
        m_readyToRequest = true;
    });
}

void DiscordController::requestNewsChannel()
{
    QNetworkRequest newRequest;

    QString urlString = "https://discord.com/api/v9/channels/" + QString(NEWS_CHANNEL_ID).toLocal8Bit();

    newRequest.setUrl(QUrl(urlString));
    newRequest.setRawHeader("Authorization", QString(DISCORD_TOKEN).toLocal8Bit());
    newRequest.setRawHeader("Host", "discord.com");
    newRequest.setRawHeader("User-Agent", "DowStats2");
    newRequest.setRawHeader("Content-Type","application/json");

    m_readyToRequest = false;

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveNewsChannel(reply);
        m_readyToRequest = true;
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
        reply->deleteLater();
        m_readyToRequest = true;
    });
}

void DiscordController::requestEventsChannel()
{
    QNetworkRequest newRequest;

    QString urlString = "https://discord.com/api/v9/channels/" + QString(EVENTS_CHANNEL_ID).toLocal8Bit();

    newRequest.setUrl(QUrl(urlString));
    newRequest.setRawHeader("Authorization", QString(DISCORD_TOKEN).toLocal8Bit());
    newRequest.setRawHeader("Host", "discord.com");
    newRequest.setRawHeader("User-Agent", "DowStats2");
    newRequest.setRawHeader("Content-Type","application/json");

    m_readyToRequest = false;

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveEventsChannel(reply);
        m_readyToRequest = true;
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
        reply->deleteLater();
        m_readyToRequest = true;
    });
}

void DiscordController::requestUserAvatar(QString userId, QString avatarId)
{
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

void DiscordController::requestAttachmentImage(QString attachmentId, QString url)
{
    QNetworkRequest newRequest;

    newRequest.setUrl(QUrl(url));

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveAttachmentImage(reply, attachmentId);
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
        reply->deleteLater();
    });
}

QList<DiscordMessage> DiscordController::parseMessagesJson(QByteArray byteArray)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(byteArray);

    if(!jsonDoc.isArray())
        return QList<DiscordMessage>();

    QJsonArray replyJsonArray = jsonDoc.array();

    QList<DiscordMessage> discordNewsList;

    for (int i = 0; i < replyJsonArray.count(); i++)
    {
        DiscordMessage newDiscordMessage;

        if(!replyJsonArray.at(i).isObject())
            continue;

        QJsonObject newObject = replyJsonArray.at(i).toObject();

        newDiscordMessage.content = newObject.value("content").toString();
        newDiscordMessage.messageId = newObject.value("id").toString();

        QDateTime newDateTime;
        newDateTime.setDate(QDate::fromString(newObject.value("timestamp").toString().left(10),"yyyy-MM-dd"));
        newDateTime.setTime(QTime::fromString(newObject.value("timestamp").toString().mid(11, 8),"hh:mm:ss"));
        newDiscordMessage.timestamp = newDateTime;

        QJsonObject authorObject = newObject.value("author").toObject();
        newDiscordMessage.userId = authorObject.value("id").toString();
        newDiscordMessage.userName = authorObject.value("username").toString();
        newDiscordMessage.avatarId = authorObject.value("avatar").toString();

        newDiscordMessage.attachmentId = "0";

        if (newObject.value("attachments").isArray())
        {
            QJsonArray attachmentsArray = newObject.value("attachments").toArray();

            for (int j = 0; j < attachmentsArray.count(); j++)
            {
                if(attachmentsArray.at(j).isObject() )
                {
                    QJsonObject attachmentObject = attachmentsArray.at(j).toObject();

                    if (attachmentObject.value("content_type").toString() != "image/png" && attachmentObject.value("content_type").toString() != "image/jpeg")
                        continue;

                    QString url = attachmentObject.value("url").toString();
                    QString attachmentId = attachmentObject.value("id").toString();

                    newDiscordMessage.attachmentId = attachmentId;
                    newDiscordMessage.attachmentImageWidth = attachmentObject.value("width").toInt();
                    newDiscordMessage.attachmentImageHeight = attachmentObject.value("height").toInt();


                    requestAttachmentImage(attachmentId, url);

                    break;
                }
            }
        }

        if(!m_avatarIdList.contains(newDiscordMessage.avatarId))
        {
            m_avatarIdList.append(newDiscordMessage.avatarId);
            requestUserAvatar(newDiscordMessage.userId, newDiscordMessage.avatarId);
        }

        discordNewsList.append(std::move(newDiscordMessage));
    }

    return std::move(discordNewsList);
}

void DiscordController::setLastReadedNewsMessageID(QString id)
{
    m_lastReadedNewsMessageID = id;
    m_settingsController->getSettings()->lastReadedNewsMessageID = m_lastReadedNewsMessageID;
}

void DiscordController::setLastReadedEventsMessageID(QString id)
{
    m_lastReadedEventMessageID = id;
    m_settingsController->getSettings()->lastReadedEventsMessageID = m_lastReadedEventMessageID;
}

void DiscordController::receiveNews(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "Connection error:" << reply->errorString();
        delete reply;
        return;
    }

    QByteArray replyByteArray = reply->readAll();

    delete reply;

    QList<DiscordMessage> newsList = parseMessagesJson(replyByteArray);

    bool isNewMessage = true;

    for(int i = 0; i < newsList.count(); i++)
    {
        if (newsList.at(i).messageId == m_lastReadedNewsMessageID)
            isNewMessage = false;

        newsList[i].isNew = isNewMessage;
    }

    emit sendNews(std::move(newsList));
}

void DiscordController::receiveEvents(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "Connection error:" << reply->errorString();
        delete reply;
        return;
    }

    QByteArray replyByteArray = reply->readAll();

    delete reply;

    QList<DiscordMessage> eventsList = parseMessagesJson(replyByteArray);

    bool isNewMessage = true;

    for(int i = 0; i < eventsList.count(); i++)
    {
        if (eventsList.at(i).messageId == m_lastReadedEventMessageID)
            isNewMessage = false;

        eventsList[i].isNew = isNewMessage;
    }

    emit sendEvents(std::move(eventsList));
}

void DiscordController::receiveNewsChannel(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "Connection error:" << reply->errorString();
        delete reply;
        return;
    }

    QByteArray replyByteArray = reply->readAll();

    delete reply;

    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyByteArray);

    if(!jsonDoc.isObject())
        return;

    QJsonObject newJsonObject = jsonDoc.object();

    QString lastMessageId = newJsonObject.value("last_message_id").toString();

    if (lastMessageId != m_lastNewsMessageID)
    {
        m_lastNewsMessageID = lastMessageId;
        requestNews();
    }
}

void DiscordController::receiveEventsChannel(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "Connection error:" << reply->errorString();
        delete reply;
        return;
    }

    QByteArray replyByteArray = reply->readAll();

    delete reply;

    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyByteArray);

    if(!jsonDoc.isObject())
        return;

    QJsonObject newJsonObject = jsonDoc.object();

    QString lastMessageId = newJsonObject.value("last_message_id").toString();

    if (lastMessageId != m_lastEventMessageID)
    {
        m_lastEventMessageID = lastMessageId;
        requestEvents();
    }
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

    emit sendAvatar(avatarId, std::move(avatar));
}

void DiscordController::receiveAttachmentImage(QNetworkReply *reply, QString attachmentId)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "Connection error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray replyByteArray = reply->readAll();

    delete reply;

    QImage image = QImage::fromData(replyByteArray);

    if (image.isNull())
        return;

    emit sendAttachmentImage(attachmentId, std::move(image));
}

void DiscordController::onSettingsLoaded()
{
    m_lastReadedNewsMessageID = m_settingsController->getSettings()->lastReadedNewsMessageID;
    m_lastReadedEventMessageID = m_settingsController->getSettings()->lastReadedEventsMessageID;
}

void DiscordController::requestMessages()
{
    if (!m_readyToRequest)
        return;

    if(m_requestNewsNow)
        requestNewsChannel();
    else
    {
        requestEventsChannel();
        m_requestTimer->setInterval(REQUEST_TIMER_INTERVAL2);
    }

    m_requestNewsNow = !m_requestNewsNow;
}

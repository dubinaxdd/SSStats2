#include "newsServiceWebProcessor.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <defines.h>

//#define REQUEST_TIMER_INTERVAL 2000
//#define REQUEST_TIMER_INTERVAL2 4000

#define REQUEST_TIMER_INTERVAL 500
#define REQUEST_TIMER_INTERVAL2 500

DiscordWebProcessor::DiscordWebProcessor(SettingsController* settingsController, QObject *parent)
    : QObject(parent)
    , m_settingsController(settingsController)
    , m_requestTimer(new QTimer(this))
    , m_networkManager(new QNetworkAccessManager(this))
{
    m_newsChannelId = QString(/*TEST_CHANNEL_ID*/NEWS_CHANNEL_ID).toLocal8Bit();
    m_eventsChannelId = QString(/*TEST_CHANNEL_ID*/EVENTS_CHANNEL_ID).toLocal8Bit();

    QObject::connect(m_settingsController, &SettingsController::settingsLoaded, this, &DiscordWebProcessor::onSettingsLoaded, Qt::QueuedConnection);
    QObject::connect(m_requestTimer, &QTimer::timeout, this, &DiscordWebProcessor::requestMessages, Qt::QueuedConnection);

    m_requestTimer->setInterval(REQUEST_TIMER_INTERVAL);
    m_requestTimer->start();
}

void DiscordWebProcessor::requestNews()
{
    QNetworkRequest newRequest;

    QString urlString = "";
/*
    if (m_isFirstNewsRequest)
        urlString = "https://discord.com/api/v9/channels/" + m_newsChannelId + "/messages?limit=5";
    else
        urlString = "https://discord.com/api/v9/channels/" + m_newsChannelId + "/messages?after=" + m_afterNewsMessageID + "&limit=5";
*/
    if (m_isFirstNewsRequest)
        urlString = "http://crosspick.ru:8080/news/messages?limit=5";
    else
        urlString = "http://crosspick.ru:8080/news/messages?after=" + m_afterNewsMessageID + "&limit=5";


    m_isFirstNewsRequest = false;

    newRequest.setUrl(QUrl(urlString));
    //newRequest.setRawHeader("Authorization", QString(DISCORD_TOKEN).toLocal8Bit());
    //newRequest.setRawHeader("Host", "discord.com");
    //newRequest.setRawHeader("User-Agent", "DowStats2");
    //newRequest.setRawHeader("Content-Type","application/json");

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

void DiscordWebProcessor::requestEvents()
{
    QNetworkRequest newRequest;

    QString urlString = "";
/*
    if (m_isFirstEventsRequest)
        urlString = "https://discord.com/api/v9/channels/" + m_eventsChannelId + "/messages?limit=5";
    else
        urlString = "https://discord.com/api/v9/channels/" + m_eventsChannelId + "/messages?after=" + m_afterEventsMessageID  + "&limit=5";
*/
    if (m_isFirstEventsRequest)
        urlString = "http://crosspick.ru:8080/events/messages?limit=5";
    else
        urlString = "http://crosspick.ru:8080/events/messages?after=" + m_afterEventsMessageID + "&limit=5";


    m_isFirstEventsRequest = false;

    newRequest.setUrl(QUrl(urlString));
    /*newRequest.setRawHeader("Authorization", QString(DISCORD_TOKEN).toLocal8Bit());
    newRequest.setRawHeader("Host", "discord.com");
    newRequest.setRawHeader("User-Agent", "DowStats2");
    newRequest.setRawHeader("Content-Type","application/json");
*/
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

void DiscordWebProcessor::requestNewsLastMessageId()
{
    QNetworkRequest newRequest;

    QString urlString = "http://crosspick.ru:8080/news/lastmessageid";

    newRequest.setUrl(QUrl(urlString));

    m_readyToRequest = false;

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveNewsLastMessageId(reply);
        m_readyToRequest = true;
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
        reply->deleteLater();
        m_readyToRequest = true;
    });
}

void DiscordWebProcessor::requestEventsLastMessageId()
{
    QNetworkRequest newRequest;

    QString urlString = "http://crosspick.ru:8080/events/lastmessageid";

    newRequest.setUrl(QUrl(urlString));

    m_readyToRequest = false;

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveEventsLastMessageId(reply);
        m_readyToRequest = true;
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
        reply->deleteLater();
        m_readyToRequest = true;
    });
}

void DiscordWebProcessor::requestUserAvatar(QString userId, QString avatarId)
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

void DiscordWebProcessor::requestAttachmentImage(QString attachmentId, QString url)
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

void DiscordWebProcessor::requestYoutubeImage(QString youtubeId, QString url)
{
    QNetworkRequest newRequest;

    newRequest.setUrl(QUrl(url));

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveYoutubeImage(reply, youtubeId);
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
        reply->deleteLater();
    });
}

QList<DiscordMessage> DiscordWebProcessor::parseMessagesJson(QByteArray byteArray)
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

        //Дергаем превьюху ютубовских видосов
        if (newDiscordMessage.content.contains("https://youtu.be/"))
        {
            QString youtubeLink = "";

            for(int i = 0; i < newDiscordMessage.content.count(); i++)
            {
                if ( newDiscordMessage.content.mid(i, 17) == "https://youtu.be/")
                {


                    for(int j = i; j < newDiscordMessage.content.count(); j++)
                    {
                        if(newDiscordMessage.content[j] != '\n' && newDiscordMessage.content[j] != ' ' && newDiscordMessage.content[j] != '\0' )
                            youtubeLink.append(newDiscordMessage.content[j]);
                        else
                            break;
                    }
                    break;
                }
            }

            newDiscordMessage.youtubeId = youtubeLink.replace("https://youtu.be/", "");

            requestYoutubeImage(newDiscordMessage.youtubeId, "https://img.youtube.com/vi/" + newDiscordMessage.youtubeId +"/0.jpg");
        }

        discordNewsList.append(std::move(newDiscordMessage));
    }

    return discordNewsList;
}

void DiscordWebProcessor::setLastReadedNewsMessageID(QString id)
{
    m_lastReadedNewsMessageID = id;
    m_settingsController->getSettings()->lastReadedNewsMessageID = m_lastReadedNewsMessageID;
    m_settingsController->saveSettings();
}

void DiscordWebProcessor::setLastReadedEventsMessageID(QString id)
{
    m_lastReadedEventMessageID = id;
    m_settingsController->getSettings()->lastReadedEventsMessageID = m_lastReadedEventMessageID;
    m_settingsController->saveSettings();
}

void DiscordWebProcessor::requestNextNews(QString messageId)
{
    QNetworkRequest newRequest;

    //QString urlString = "https://discord.com/api/v9/channels/" + m_newsChannelId + "/messages?limit=5&before=" + messageId;

    QString urlString = "http://crosspick.ru:8080/news/messages?limit=5&before=" + messageId;

    newRequest.setUrl(QUrl(urlString));
    /*newRequest.setRawHeader("Authorization", QString(DISCORD_TOKEN).toLocal8Bit());
    newRequest.setRawHeader("Host", "discord.com");
    newRequest.setRawHeader("User-Agent", "DowStats2");
    newRequest.setRawHeader("Content-Type","application/json");
*/

    m_readyToRequest = false;

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveNextNews(reply);
        m_readyToRequest = true;
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
        reply->deleteLater();
        m_readyToRequest = true;
    });
}

void DiscordWebProcessor::requestNextEvents(QString messageId)
{
    QNetworkRequest newRequest;

    //QString urlString = "https://discord.com/api/v9/channels/" + m_eventsChannelId + "/messages?limit=5&before=" + messageId;

    QString urlString = "http://crosspick.ru:8080/events/messages?limit=5&before=" + messageId;

    newRequest.setUrl(QUrl(urlString));
   /* newRequest.setRawHeader("Authorization", QString(DISCORD_TOKEN).toLocal8Bit());
    newRequest.setRawHeader("Host", "discord.com");
    newRequest.setRawHeader("User-Agent", "DowStats2");
    newRequest.setRawHeader("Content-Type","application/json");
*/
    m_readyToRequest = false;

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveNextEvents(reply);
        m_readyToRequest = true;
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
        reply->deleteLater();
        m_readyToRequest = true;
    });
}

void DiscordWebProcessor::receiveNews(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "Connection error:" << reply->errorString();

        m_requestTimer->setInterval(REQUEST_TIMER_INTERVAL2 * 2);
        m_needRequestNews = true;
        delete reply;
        return;
    }

    m_requestTimer->setInterval(REQUEST_TIMER_INTERVAL2);

    QByteArray replyByteArray = reply->readAll();

    delete reply;

    QList<DiscordMessage> newsList = parseMessagesJson(replyByteArray);

    if (newsList.count() == 0)

        return;


    bool isNew = true;

    for(int i = 0; i < newsList.count(); i++)
    {
        if (newsList.at(i).messageId == m_lastReadedNewsMessageID)
        {
            isNew = false;
            m_isNewNewsMessage = false;
        }

        newsList[i].isNew = isNew;
    }

    emit sendNews(std::move(newsList));
}

void DiscordWebProcessor::receiveNextNews(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "Connection error:" << reply->errorString();
        emit sendNextNews(QList<DiscordMessage>());
        delete reply;
        return;
    }


    QByteArray replyByteArray = reply->readAll();
    delete reply;

    QList<DiscordMessage> newsList = parseMessagesJson(replyByteArray);

    for(int i = 0; i < newsList.count(); i++)
    {
        if (newsList.at(i).messageId == m_lastReadedNewsMessageID)
            m_isNewNewsMessage = false;

        newsList[i].isNew = m_isNewNewsMessage;
    }

    emit sendNextNews(std::move(newsList));
}

void DiscordWebProcessor::receiveEvents(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "Connection error:" << reply->errorString();
        m_requestTimer->setInterval(REQUEST_TIMER_INTERVAL2 * 2);
        m_needRequestEvents = true;
        delete reply;
        return;
    }

    m_requestTimer->setInterval(REQUEST_TIMER_INTERVAL2);

    QByteArray replyByteArray = reply->readAll();

    delete reply;

    QList<DiscordMessage> eventsList = parseMessagesJson(replyByteArray);

    if (eventsList.count() == 0)
        return;

    m_lastEventMessageID = eventsList.first().messageId;

    bool isNew = true;

    for(int i = 0; i < eventsList.count(); i++)
    {
        if (eventsList.at(i).messageId == m_lastReadedEventMessageID)
        {

            isNew = false;
            m_isNewEventsMessage = false;
        }

        eventsList[i].isNew = isNew;
    }

    emit sendEvents(std::move(eventsList));
}

void DiscordWebProcessor::receiveNextEvents(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "Connection error:" << reply->errorString();
        emit sendNextEvents(QList<DiscordMessage>());
        delete reply;
        return;
    }

    QByteArray replyByteArray = reply->readAll();

    delete reply;

    QList<DiscordMessage> eventsList = parseMessagesJson(replyByteArray);

    for(int i = 0; i < eventsList.count(); i++)
    {
        if (eventsList.at(i).messageId == m_lastReadedEventMessageID)
            m_isNewEventsMessage = false;

        eventsList[i].isNew = m_isNewEventsMessage;
    }

    emit sendNextEvents(std::move(eventsList));
}

void DiscordWebProcessor::receiveNewsLastMessageId(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "Connection error:" << reply->errorString();
        delete reply;
        return;
    }

    QByteArray replyByteArray = reply->readAll();
    delete reply;

    QString lastMessageId = QString::fromStdString(replyByteArray.toStdString());

    if (lastMessageId != m_lastNewsMessageID)
    {
        qInfo(logInfo()) << "Request NEWS" << lastMessageId << m_lastNewsMessageID;

        m_afterNewsMessageID = m_lastNewsMessageID;
        m_lastNewsMessageID = lastMessageId;
        m_needRequestNews  = true;
    }
}

void DiscordWebProcessor::receiveEventsLastMessageId(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "Connection error:" << reply->errorString();
        delete reply;
        return;
    }

    QByteArray replyByteArray = reply->readAll();
    delete reply;

    QString lastMessageId = QString::fromStdString(replyByteArray.toStdString());

    if (lastMessageId != m_lastEventMessageID)
    {
        qInfo(logInfo()) << "Request EVENTS" << lastMessageId << m_lastEventMessageID;

        m_afterEventsMessageID = m_lastEventMessageID;
        m_lastEventMessageID = lastMessageId;
        m_needRequestEvents = true;
    }
}

void DiscordWebProcessor::receiveUserAvatar(QNetworkReply *reply, QString avatarId)
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

void DiscordWebProcessor::receiveAttachmentImage(QNetworkReply *reply, QString attachmentId)
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

void DiscordWebProcessor::receiveYoutubeImage(QNetworkReply *reply, QString youtubeId)
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

    emit sendYoutubeImage(youtubeId, std::move(image));
}

void DiscordWebProcessor::onSettingsLoaded()
{
    m_lastReadedNewsMessageID = m_settingsController->getSettings()->lastReadedNewsMessageID;
    m_lastReadedEventMessageID = m_settingsController->getSettings()->lastReadedEventsMessageID;
}

void DiscordWebProcessor::requestMessages()
{
    if (!m_readyToRequest)
        return;

    if(m_needRequestNews)
    {
        m_needRequestNews = false;
        requestNews();
        return;
    }

    if(m_needRequestEvents)
    {
        m_needRequestEvents = false;
        requestEvents();
        m_requestTimer->setInterval(REQUEST_TIMER_INTERVAL2);
        return;
    }

    if(m_requestNewsNow)
        requestNewsLastMessageId();
    else
        requestEventsLastMessageId();

    m_requestNewsNow = !m_requestNewsNow;
}

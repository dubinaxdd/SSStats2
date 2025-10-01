#include "newsServiceWebProcessor.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <defines.h>

DiscordWebProcessor::DiscordWebProcessor(SettingsController* settingsController, QObject *parent)
    : QObject(parent)
    , m_settingsController(settingsController)
    , m_requestTimer(new QTimer(this))
    , m_networkManager(new QNetworkAccessManager(this))
{
    QObject::connect(m_settingsController, &SettingsController::settingsLoaded, this, &DiscordWebProcessor::onSettingsLoaded, Qt::QueuedConnection);

    connect(&m_webSocket, &QWebSocket::connected, this, &DiscordWebProcessor::onConnected, Qt::QueuedConnection);
    connect(&m_webSocket, &QWebSocket::disconnected, this,  &DiscordWebProcessor::onDisconnected, Qt::QueuedConnection);
    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &DiscordWebProcessor::readMessage, Qt::QueuedConnection);

    connect(&m_reconnectTimer, &QTimer::timeout, this, &DiscordWebProcessor::reconnect, Qt::QueuedConnection);
    connect(&m_pingTimer, &QTimer::timeout, this, &DiscordWebProcessor::sendPing, Qt::QueuedConnection);
    connect(&m_pingResponceTimer, &QTimer::timeout, this, [&]{m_webSocket.close();});

    m_reconnectTimer.setInterval(5000);
    m_pingTimer.setInterval(20000);
    m_pingResponceTimer.setInterval(10000);
    m_pingResponceTimer.setSingleShot(true);

}

void DiscordWebProcessor::requestNews()
{
#ifdef TEST_NEWS_CHANNEL
    requestMessagesFromServer(RequestTestFromIdByLimit, true, m_lastNewsMessageID);   //DEBUG TEST CHANNEL
#else
    requestMessagesFromServer(RequestNewsFromIdByLimit, true, m_lastNewsMessageID);
#endif
}

void DiscordWebProcessor::requestEvents()
{
    requestMessagesFromServer(RequestEventsFromIdByLimit, true, m_lastEventMessageID);
}

void DiscordWebProcessor::requestAttachmentImage(QString attachmentId, QString url)
{
    QNetworkRequest newRequest;

    newRequest.setUrl(QUrl(url.toUtf8()));

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveAttachmentImage(reply, attachmentId);
    });
}

void DiscordWebProcessor::requestYoutubeImage(QString youtubeId)
{
    QNetworkRequest newRequest;

    newRequest.setUrl(QUrl("https://img.youtube.com/vi/" + youtubeId +"/0.jpg"));

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveYoutubeImage(reply, youtubeId);
    });
}

QList<DiscordMessage> DiscordWebProcessor::parseMessagesJson(QJsonArray messagesArray)
{

    QList<DiscordMessage> discordNewsList;

    for (int i = 0; i < messagesArray.count(); i++)
    {
        if(!messagesArray.at(i).isObject())
            continue;

        discordNewsList.append(std::move(parseMessage (messagesArray.at(i).toObject())));
    }

    return discordNewsList;
}

DiscordMessage DiscordWebProcessor::parseMessage(QJsonObject messageJson)
{
    DiscordMessage newDiscordMessage;

    newDiscordMessage.content = messageJson.value("content").toString();
    newDiscordMessage.messageId = messageJson.value("id").toString();

    QDateTime newDateTime;
    newDateTime.setDate(QDate::fromString(messageJson.value("timestamp").toString().left(10),"yyyy-MM-dd"));
    newDateTime.setTime(QTime::fromString(messageJson.value("timestamp").toString().mid(11, 8),"hh:mm:ss"));
    newDiscordMessage.timestamp = newDateTime;


    newDiscordMessage.userId = messageJson.value("userId").toString();
    newDiscordMessage.userName = messageJson.value("userName").toString();
    newDiscordMessage.avatarId = messageJson.value("avatarId").toString();
    newDiscordMessage.avatarUrl = messageJson.value("avatarUrl").toString();

    newDiscordMessage.attachmentImageId = messageJson.value("attacmentImageId").toString();
    newDiscordMessage.attachmentImageUrl = messageJson.value("attacmentImageUrl").toString();
    newDiscordMessage.attachmentImageWidth = messageJson.value("attacmentImageWidth").toInt();
    newDiscordMessage.attachmentImageHeight = messageJson.value("attacmentImageHeight").toInt();

    if (!newDiscordMessage.attachmentImageId.isEmpty())
        requestAttachmentImage(newDiscordMessage.attachmentImageId, newDiscordMessage.attachmentImageUrl);

    if(!m_avatarIdList.contains(newDiscordMessage.avatarId))
    {
        m_avatarIdList.append(newDiscordMessage.avatarId);
        requestUserAvatar(newDiscordMessage.avatarId, newDiscordMessage.avatarUrl);
    }

    //Дергаем превьюху ютубовских видосов
    if (newDiscordMessage.content.contains("https://youtu.be/")
        || newDiscordMessage.content.contains("https://www.youtube.com/watch?v=")
        || newDiscordMessage.content.contains("https://www.youtube.com/live/"))
    {
        QString youtubeLink = "";



        if (newDiscordMessage.content.contains("https://youtu.be/"))
        {
            int index = newDiscordMessage.content.indexOf("https://youtu.be/");

            for(int i = index; i < newDiscordMessage.content.count(); i++)
            {
                if(newDiscordMessage.content[i] != '\n' && newDiscordMessage.content[i] != ' ' && newDiscordMessage.content[i] != '\0' )
                    youtubeLink.append(newDiscordMessage.content[i]);
                else
                    break;
            }
        }


        if (youtubeLink.isEmpty())
        {

            if(newDiscordMessage.content.contains("https://www.youtube.com/watch?v="))
            {
                int index = newDiscordMessage.content.indexOf("https://www.youtube.com/watch?v=");

                for(int i = index; i < newDiscordMessage.content.count(); i++)
                {
                    if(newDiscordMessage.content[i] != '\n' && newDiscordMessage.content[i] != ' ' && newDiscordMessage.content[i] != '\0' )
                        youtubeLink.append(newDiscordMessage.content[i]);
                    else
                        break;
                }
            }
        }


        if (youtubeLink.isEmpty())
        {

            if (newDiscordMessage.content.contains("https://www.youtube.com/live/"))
            {
                int index = newDiscordMessage.content.indexOf("https://www.youtube.com/live/");

                for(int i = index; i < newDiscordMessage.content.count(); i++)
                {
                    if(newDiscordMessage.content[i] != '\n' && newDiscordMessage.content[i] != ' ' && newDiscordMessage.content[i] != '\0' )
                        youtubeLink.append(newDiscordMessage.content[i]);
                    else
                        break;
                }
            }
        }


        if (!youtubeLink.isEmpty())
        {
            newDiscordMessage.youtubeId = youtubeLink.replace("https://youtu.be/", "").replace("https://www.youtube.com/watch?v=", "");
            newDiscordMessage.youtubeId = newDiscordMessage.youtubeId.replace("https://www.youtube.com/live/", "");

            for (int i = 0; i < newDiscordMessage.youtubeId.count(); i++)
            {
                if (newDiscordMessage.youtubeId.at(i) == "?" || newDiscordMessage.youtubeId.at(i) == "&" )
                {
                    newDiscordMessage.youtubeId = newDiscordMessage.youtubeId.left(i);
                    break;
                }
            }

            requestYoutubeImage(newDiscordMessage.youtubeId);
        }
    }

    return newDiscordMessage;
}

void DiscordWebProcessor::requestUserAvatar(QString avatarId, QString avatarUrl)
{
    QNetworkRequest newRequest;
    newRequest.setUrl(QUrl(avatarUrl));

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveUserAvatar(reply, avatarId);
    });
}

void DiscordWebProcessor::requestLastMessageId()
{
    QJsonObject messageObject;
    messageObject.insert("op", RequestLastMessagesId);

    QJsonDocument message;
    message.setObject(messageObject);

    m_webSocket.sendTextMessage(message.toJson().replace('\n',""));
}

void DiscordWebProcessor::requestMessagesFromServer(OpCode opCode, bool includeFirst, QString lastMessageID)
{
    QJsonObject messageObject;
    messageObject.insert("op", opCode);

    messageObject.insert("messageId", lastMessageID);
    messageObject.insert("limit", 10);
    messageObject.insert("includeFirst", includeFirst);

    QJsonDocument message;
    message.setObject(messageObject);

    m_webSocket.sendTextMessage(message.toJson().replace('\n',""));
}

void DiscordWebProcessor::receiveNews(QJsonArray messagesArray)
{
    QList<DiscordMessage> newsList = parseMessagesJson(messagesArray);

    if (newsList.count() == 0)
        return;

    m_lastNewsMessageID = newsList.last().messageId;


    for(int i = 0; i < newsList.count(); i++)
    {
        if (newsList.at(i).messageId == m_lastReadedNewsMessageID)
            m_isNewNewsMessage = false;

        newsList[i].isNew = m_isNewNewsMessage;
    }

    emit sendNews(std::move(newsList));
}

void DiscordWebProcessor::receiveEvents(QJsonArray messagesArray)
{
    QList<DiscordMessage> newsList = parseMessagesJson(messagesArray);

    if (newsList.count() == 0)
        return;

    m_lastEventMessageID = newsList.last().messageId;


    for(int i = 0; i < newsList.count(); i++)
    {
        if (newsList.at(i).messageId == m_lastReadedEventMessageID)
            m_isNewEventsMessage = false;


        newsList[i].isNew = m_isNewEventsMessage;
    }

    emit sendEvents(std::move(newsList));
}

void DiscordWebProcessor::receiveCreateNewsMessage(QJsonObject messageObject)
{
    DiscordMessage newMessage = parseMessage(messageObject);
    emit sendCreateNewsMessage(std::move(newMessage));
}

void DiscordWebProcessor::receiveCreateEventMessage(QJsonObject messageObject)
{
    DiscordMessage newMessage = parseMessage(messageObject);
    emit sendCreateEventsMessage(std::move(newMessage));
}

void DiscordWebProcessor::receiveUpdateNewsMessage(QJsonObject messageObject)
{
    DiscordMessage newMessage = parseMessage(messageObject);
    emit sendUpdateNewsMessage(std::move(newMessage));
}

void DiscordWebProcessor::receiveUpdateEventMessage(QJsonObject messageObject)
{
    DiscordMessage newMessage = parseMessage(messageObject);
    emit sendUpdateEventsMessage(std::move(newMessage));
}

void DiscordWebProcessor::receivePingResponce()
{
    m_pingResponceTimer.stop();
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
    if (m_newsMessagesEnd)
        return;

#ifdef TEST_NEWS_CHANNEL
    requestMessagesFromServer(RequestTestFromIdByLimit, false, m_lastNewsMessageID);   //DEBUG TEST CHANNEL
#else
    requestMessagesFromServer(RequestNewsFromIdByLimit, false, m_lastNewsMessageID);
#endif
}

void DiscordWebProcessor::requestNextEvents(QString messageId)
{
    if (m_eventsMessagesEnd)
        return;

    requestMessagesFromServer(RequestEventsFromIdByLimit, false, m_lastEventMessageID);
}

void DiscordWebProcessor::receiveUserAvatar(QNetworkReply *reply, QString avatarId)
{

    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "DiscordWebProcessor::receiveUserAvatar:" << "Connection error:" << reply->errorString();
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
        qWarning(logWarning()) << "DiscordWebProcessor::receiveAttachmentImage:" << "Connection error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray replyByteArray = reply->readAll();

    reply->deleteLater();

    QImage image = QImage::fromData(replyByteArray);

    if (image.isNull())
        return;

    emit sendAttachmentImage(attachmentId, std::move(image));
}

void DiscordWebProcessor::receiveYoutubeImage(QNetworkReply *reply, QString youtubeId)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "DiscordWebProcessor::receiveYoutubeImage:" << "Connection error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray replyByteArray = reply->readAll();

    reply->deleteLater();

    QImage image = QImage::fromData(replyByteArray);

    if (image.isNull())
        return;

    emit sendYoutubeImage(youtubeId, std::move(image));
}

void DiscordWebProcessor::onSettingsLoaded()
{
    qInfo(logInfo()) << "DiscordWebProcessor::onSettingsLoaded()" << "load started";

    m_lastReadedNewsMessageID = m_settingsController->getSettings()->lastReadedNewsMessageID;
    m_lastReadedEventMessageID = m_settingsController->getSettings()->lastReadedEventsMessageID;

    qInfo(logInfo()) << "DiscordWebProcessor::onSettingsLoaded()" << "load finished";

    m_webSocket.open(QUrl(DISCORD_WEB_SERVICE));
    m_reconnectTimer.start();
}

void DiscordWebProcessor::readMessage(QString message)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(message.toUtf8());

    if (!jsonDocument.isObject())
        return;

    QJsonObject messageObject = jsonDocument.object();

    EventType eventType = static_cast<EventType>(messageObject.value("op").toInt());

    switch(eventType)
    {
        case LastMessagesIdAnswer:

#ifdef TEST_NEWS_CHANNEL
            m_lastNewsMessageID = messageObject.value("test_last_message").toString();  //DEBUG TEST CHANNEL
#else
            m_lastNewsMessageID = messageObject.value("news_last_message").toString();
#endif
            m_lastEventMessageID = messageObject.value("events_last_message").toString();

            requestNews();
            requestEvents();
            break;

#ifdef TEST_NEWS_CHANNEL
        case CreateTestMessage: receiveCreateNewsMessage(messageObject.value("content").toObject()); break; //DEBUG TEST CHANNEL
        case UpdateTestMessage: receiveUpdateNewsMessage(messageObject.value("content").toObject()); break; //DEBUG TEST CHANNEL
        case DeleteTestMessage: emit sendRemoveNewsMessage(messageObject.value("messageId").toString()); break; //DEBUG TEST CHANNEL
        case TestMessagesAnswer: receiveNews(messageObject.value("messages").toArray()); break; //DEBUG TEST CHANNEL
        case TestMessagesEnd: m_newsMessagesEnd = true; break; //DEBUG TEST CHANNEL
#else
        case CreateNewsMessage: receiveCreateNewsMessage(messageObject.value("content").toObject()); break;
        case UpdateNewsMessage: receiveUpdateNewsMessage(messageObject.value("content").toObject()); break;
        case DeleteNewsMessage: emit sendRemoveNewsMessage(messageObject.value("messageId").toString()); break;
        case NewsMessagesAnswer: receiveNews(messageObject.value("messages").toArray()); break;
        case NewsMessagesEnd: m_newsMessagesEnd = true; break;
#endif
        case CreateEventMessage: receiveCreateEventMessage(messageObject.value("content").toObject()); break;
        case UpdateEventMessage: receiveUpdateEventMessage(messageObject.value("content").toObject()); break;
        case DeleteEventMessage: emit sendRemoveEventsMessage(messageObject.value("messageId").toString()); break;
        case EventsMessagesEnd: m_eventsMessagesEnd = true; break;

        case AllMesagesReceive: break;
        case EventsMessagesAnswer: receiveEvents(messageObject.value("messages").toArray()); break;
        case PingResponce: receivePingResponce(); break;
        default: break;
    }
}

void DiscordWebProcessor::reconnect()
{
    qInfo(logInfo()) << "DiscordWebProcessor::reconnect() Try reconnect";
    m_webSocket.open(QUrl(DISCORD_WEB_SERVICE));
}

void DiscordWebProcessor::onConnected()
{
    m_pingTimer.start();
    m_reconnectTimer.stop();
    qInfo(logInfo()) << "DiscordWebProcessor::reconnect() Connected";
    m_webSocketConnected = true;
    requestLastMessageId();
}

void DiscordWebProcessor::onDisconnected()
{
    m_pingTimer.stop();
    m_reconnectTimer.start();
    m_webSocketConnected = false;
    qInfo(logInfo()) << "DiscordWebProcessor::reconnect() Disconnected";
}

void DiscordWebProcessor::sendPing()
{
    QJsonObject messageObject;
    messageObject.insert("op", Ping);

    QJsonDocument message;
    message.setObject(messageObject);

    m_webSocket.sendTextMessage(message.toJson().replace('\n',""));
    m_pingResponceTimer.start();
}

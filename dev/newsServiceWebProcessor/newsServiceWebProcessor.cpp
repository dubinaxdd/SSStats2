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
#define REQUEST_TIMER_INTERVAL2 5000

DiscordWebProcessor::DiscordWebProcessor(SettingsController* settingsController, QObject *parent)
    : QObject(parent)
    , m_settingsController(settingsController)
    , m_requestTimer(new QTimer(this))
    , m_networkManager(new QNetworkAccessManager(this))
{
    QObject::connect(m_settingsController, &SettingsController::settingsLoaded, this, &DiscordWebProcessor::onSettingsLoaded, Qt::QueuedConnection);
    //QObject::connect(m_requestTimer, &QTimer::timeout, this, &DiscordWebProcessor::requestMessages, Qt::QueuedConnection);

   // m_requestTimer->setInterval(REQUEST_TIMER_INTERVAL);
    //m_requestTimer->start();

    connect(&m_webSocket, &QWebSocket::connected, this, &DiscordWebProcessor::onConnected, Qt::QueuedConnection);
    connect(&m_webSocket, &QWebSocket::disconnected, this,  &DiscordWebProcessor::onDisconnected, Qt::QueuedConnection);
    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &DiscordWebProcessor::readMessage, Qt::QueuedConnection);

    connect(&m_reconnectTimer, &QTimer::timeout, this, &DiscordWebProcessor::reconnect, Qt::QueuedConnection);

    m_reconnectTimer.setInterval(5000);
    m_reconnectTimer.start();

    m_webSocket.open(QUrl("ws://127.0.0.1:50789"));

}

void DiscordWebProcessor::requestNews()
{
    //requestMessages(RequestNewsFromIdByLimit, true);
    requestMessagesFromServer(RequestTestFromIdByLimit, true, m_lastNewsMessageID);   //DEBUG TEST CHANNEL

    /*QNetworkRequest newRequest;

    QString urlString = "";

    if (m_isFirstNewsRequest)
        urlString = "http://crosspick.ru:8080/news/messages?limit=5";
        //urlString = "http://crosspick.ru:8080/test/messages?limit=5";
    else
        urlString = "http://crosspick.ru:8080/news/messages?after=" + m_afterNewsMessageID + "&limit=5";
        //urlString = "http://crosspick.ru:8080/test/messages?after=" + m_afterNewsMessageID + "&limit=5";


    m_isFirstNewsRequest = false;

    newRequest.setUrl(QUrl(urlString));

    m_readyToRequest = false;

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveNews(reply);
        m_readyToRequest = true;
    });*/
}

void DiscordWebProcessor::requestEvents()
{
    requestMessagesFromServer(RequestEventsFromIdByLimit, true, m_lastEventMessageID);
    //requestMessages(RequestTestFromIdByLimit, true);   //DEBUG TEST CHANNEL


    /*QNetworkRequest newRequest;

    QString urlString = "";

    if (m_isFirstEventsRequest)
        urlString = "http://crosspick.ru:8080/events/messages?limit=5";
    else
        urlString = "http://crosspick.ru:8080/events/messages?after=" + m_afterEventsMessageID + "&limit=5";


    m_isFirstEventsRequest = false;

    newRequest.setUrl(QUrl(urlString));

    m_readyToRequest = false;

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveEvents(reply);
        m_readyToRequest = true;
    });*/
}

/*void DiscordWebProcessor::requestNewsLastMessageId()
{
    QNetworkRequest newRequest;

    QString urlString = "http://crosspick.ru:8080/news/lastmessageid";
    //QString urlString = "http://crosspick.ru:8080/test/lastmessageid";

    newRequest.setUrl(QUrl(urlString));

    m_readyToRequest = false;

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveNewsLastMessageId(reply);
        m_readyToRequest = true;
    });
}*/

/*void DiscordWebProcessor::requestEventsLastMessageId()
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
}*/

/*void DiscordWebProcessor::requestUserAvatar(QString userId, QString avatarId)
{
    QNetworkRequest newRequest;

    //newRequest.setUrl(QUrl("https://cdn.discordapp.com/avatars/" + userId + "/" + avatarId + ".png"));

    newRequest.setUrl(QUrl("http://crosspick.ru:8080/user/avatar?id=" + userId));

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveUserAvatar(reply, avatarId);
    });
}*/

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
        DiscordMessage newDiscordMessage;

        if(!messagesArray.at(i).isObject())
            continue;

        QJsonObject newObject = messagesArray.at(i).toObject();

        newDiscordMessage.content = newObject.value("content").toString();
        newDiscordMessage.messageId = newObject.value("id").toString();

        QDateTime newDateTime;
        newDateTime.setDate(QDate::fromString(newObject.value("timestamp").toString().left(10),"yyyy-MM-dd"));
        newDateTime.setTime(QTime::fromString(newObject.value("timestamp").toString().mid(11, 8),"hh:mm:ss"));
        newDiscordMessage.timestamp = newDateTime;


        newDiscordMessage.userId = newObject.value("userId").toString();
        newDiscordMessage.userName = newObject.value("userName").toString();
        newDiscordMessage.avatarId = newObject.value("avatarId").toString();
        newDiscordMessage.avatarUrl = newObject.value("avatarUrl").toString();

        newDiscordMessage.attachmentImageId = newObject.value("attacmentImageId").toString();
        newDiscordMessage.attachmentImageUrl = newObject.value("attacmentImageUrl").toString();
        newDiscordMessage.attachmentImageWidth = newObject.value("attacmentImageWidth").toInt();
        newDiscordMessage.attachmentImageHeight = newObject.value("attacmentImageHeight").toInt();

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

        discordNewsList.append(std::move(newDiscordMessage));
    }

    return discordNewsList;
}

/*QList<DiscordMessage> DiscordWebProcessor::parseMessagesJson(QByteArray byteArray)
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
        newDiscordMessage.userName = authorObject.value("global_name").toString();
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

        discordNewsList.append(std::move(newDiscordMessage));
    }

    return discordNewsList;
}*/

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
    messageObject.insert("limit", 5);
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

void DiscordWebProcessor::receiveEvents(QJsonArray messagesArray)
{
    QList<DiscordMessage> newsList = parseMessagesJson(messagesArray);

    if (newsList.count() == 0)
        return;

    m_lastEventMessageID = newsList.last().messageId;

    bool isNew = true;

    for(int i = 0; i < newsList.count(); i++)
    {
        if (newsList.at(i).messageId == m_lastReadedEventMessageID)
        {
            isNew = false;
            m_isNewEventsMessage = false;
        }

        newsList[i].isNew = isNew;
    }

    emit sendEvents(std::move(newsList));
}

void DiscordWebProcessor::requestUserAvatar(QString avatarId, QString avatarUrl)
{
    QNetworkRequest newRequest;

    //newRequest.setUrl(QUrl("https://cdn.discordapp.com/avatars/" + userId + "/" + avatarId + ".png"));

    newRequest.setUrl(QUrl(avatarUrl));

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveUserAvatar(reply, avatarId);
    });
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

    //requestMessages(RequestNewsFromIdByLimit, false);
    requestMessagesFromServer(RequestTestFromIdByLimit, false, m_lastNewsMessageID);   //DEBUG TEST CHANNEL


    /* QNetworkRequest newRequest;

    //QString urlString = "https://discord.com/api/v9/channels/" + m_newsChannelId + "/messages?limit=5&before=" + messageId;

    QString urlString = "http://crosspick.ru:8080/news/messages?limit=5&before=" + messageId;
    //QString urlString = "http://crosspick.ru:8080/test/messages?limit=5&before=" + messageId;

    newRequest.setUrl(QUrl(urlString));

    m_readyToRequest = false;

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveNextNews(reply);
        m_readyToRequest = true;
    });*/
}

void DiscordWebProcessor::requestNextEvents(QString messageId)
{
    if (m_eventsMessagesEnd)
        return;

    requestMessagesFromServer(RequestEventsFromIdByLimit, false, m_lastEventMessageID);
    //requestMessages(RequestTestFromIdByLimit, false);   //DEBUG TEST CHANNEL

    /*QNetworkRequest newRequest;

    //QString urlString = "https://discord.com/api/v9/channels/" + m_eventsChannelId + "/messages?limit=5&before=" + messageId;

    QString urlString = "http://crosspick.ru:8080/events/messages?limit=5&before=" + messageId;

    newRequest.setUrl(QUrl(urlString));
   /* newRequest.setRawHeader("Authorization", QString(DISCORD_TOKEN).toLocal8Bit());
    newRequest.setRawHeader("Host", "discord.com");
    newRequest.setRawHeader("User-Agent", "DowStatsClient");
    newRequest.setRawHeader("Content-Type","application/json");
*/
   /* m_readyToRequest = false;

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveNextEvents(reply);
        m_readyToRequest = true;
    });*/

}

/*void DiscordWebProcessor::receiveNews(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "DiscordWebProcessor::receiveNews:" << "Connection error:" << reply->errorString();
        m_requestTimer->setInterval(REQUEST_TIMER_INTERVAL2);
        m_needRequestNews = true;
        reply->deleteLater();
        return;
    }

    QByteArray replyByteArray = reply->readAll();

    reply->deleteLater();

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
}*/

/*void DiscordWebProcessor::receiveNextNews(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "DiscordWebProcessor::receiveNextNews:" << "Connection error:" << reply->errorString();
        emit sendNextNews(QList<DiscordMessage>());
        reply->deleteLater();
        return;
    }


    QByteArray replyByteArray = reply->readAll();
    reply->deleteLater();

    QList<DiscordMessage> newsList = parseMessagesJson(replyByteArray);

    for(int i = 0; i < newsList.count(); i++)
    {
        if (newsList.at(i).messageId == m_lastReadedNewsMessageID)
            m_isNewNewsMessage = false;

        newsList[i].isNew = m_isNewNewsMessage;
    }

    emit sendNextNews(std::move(newsList));
}*/

/*void DiscordWebProcessor::receiveEvents(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "DiscordWebProcessor::receiveEvents:" << "Connection error:" << reply->errorString();
        m_requestTimer->setInterval(REQUEST_TIMER_INTERVAL2);
        m_needRequestEvents = true;
        reply->deleteLater();
        return;
    }

    QByteArray replyByteArray = reply->readAll();

    reply->deleteLater();

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
}*/

/*void DiscordWebProcessor::receiveNextEvents(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "DiscordWebProcessor::receiveNextEvents:" << "Connection error:" << reply->errorString();
        emit sendNextEvents(QList<DiscordMessage>());
        reply->deleteLater();
        return;
    }

    QByteArray replyByteArray = reply->readAll();

    reply->deleteLater();

    QList<DiscordMessage> eventsList = parseMessagesJson(replyByteArray);

    for(int i = 0; i < eventsList.count(); i++)
    {
        if (eventsList.at(i).messageId == m_lastReadedEventMessageID)
            m_isNewEventsMessage = false;

        eventsList[i].isNew = m_isNewEventsMessage;
    }

    emit sendNextEvents(std::move(eventsList));
}*/

/*void DiscordWebProcessor::receiveNewsLastMessageId(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "DiscordWebProcessor::receiveNewsLastMessageId:" << "Connection error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray replyByteArray = reply->readAll();
    reply->deleteLater();

    QString lastMessageId = QString::fromStdString(replyByteArray.toStdString());

    if (lastMessageId != m_lastNewsMessageID)
    {
        qInfo(logInfo()) << "Request NEWS" << lastMessageId << m_lastNewsMessageID;

        m_afterNewsMessageID = m_lastNewsMessageID;
        m_lastNewsMessageID = lastMessageId;
        m_needRequestNews  = true;
    }
}*/

/*void DiscordWebProcessor::receiveEventsLastMessageId(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "DiscordWebProcessor::receiveEventsLastMessageId:" << "Connection error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray replyByteArray = reply->readAll();
    reply->deleteLater();

    QString lastMessageId = QString::fromStdString(replyByteArray.toStdString());

    if (lastMessageId != m_lastEventMessageID)
    {
        qInfo(logInfo()) << "Request EVENTS" << lastMessageId << m_lastEventMessageID;

        m_afterEventsMessageID = m_lastEventMessageID;
        m_lastEventMessageID = lastMessageId;
        m_needRequestEvents = true;
    }
}*/

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
}

/*void DiscordWebProcessor::requestMessages()
{
    if (!m_webSocketConnected)
        return;

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
}*/

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
            //m_lastNewsMessageID = messageObject.value("news_last_message").toString();
            m_lastEventMessageID = messageObject.value("events_last_message").toString();
            m_lastNewsMessageID = messageObject.value("test_last_message").toString();  //DEBUG TEST CHANNEL
            //m_needRequestNews  = true;
            //m_needRequestEvents = true;
            requestNews();
            requestEvents();
            break;
        case CreateNewsMessage: break;
        case UpdateNewsMessage: break;
        case CreateEventMessage: break;
        case UpdateEventMessage: break;
        case CreateTestMessage: break;
        case UpdateTestMessage: break;
        case AllMesagesReceive: break;
        case NewsMessagesAnswer: /*receiveNews(messageObject.value("messages").toArray());*/ break;
        case EventsMessagesAnswer: receiveEvents(messageObject.value("messages").toArray()); break;
        case TestMessagesAnswer: receiveNews(messageObject.value("messages").toArray()); break; //DEBUG TEST CHANNEL
        case NewsMessagesEnd: /*m_newsMessagesEnd = true;*/ break;
        case EventsMessagesEnd: m_eventsMessagesEnd = true; break;
        case TestMessagesEnd: m_newsMessagesEnd = true; break; //DEBUG TEST CHANNEL
    }
}

void DiscordWebProcessor::reconnect()
{
     qDebug() << "ASDASDASDASD Try reconnect";
    m_webSocket.open(QUrl("ws://127.0.0.1:50789"));
}

void DiscordWebProcessor::onConnected()
{
    m_reconnectTimer.stop();
    qDebug() << "ASDASDASDASD Connected";
    m_webSocketConnected = true;
    requestLastMessageId();
}

void DiscordWebProcessor::onDisconnected()
{
    m_reconnectTimer.start();
    m_webSocketConnected = false;
    qDebug() << "ASDASDASDASD Disconnected";
}

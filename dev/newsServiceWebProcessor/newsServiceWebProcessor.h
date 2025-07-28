#ifndef NEWSSERVICEWEBPROCESSOR_H
#define NEWSSERVICEWEBPROCESSOR_H

#include <QObject>
#include <QNetworkAccessManager>
#include <logger.h>
#include <baseTypes.h>
#include <QTimer>
#include <QWebSocket>
#include <settingsController.h>

class DiscordWebProcessor : public QObject
{
    Q_OBJECT
public:
    explicit DiscordWebProcessor(SettingsController* settingsController, QObject *parent = nullptr);

    enum OpCode : int
    {
        Autentication = 0,
        RequestLastMessagesId = 1,
        RequestNewsFromIdToId = 2,
        RequestEventsFromIdToId = 3,
        RequestTestFromIdToId = 4,
        RequestNewsFromIdByLimit = 5,
        RequestEventsFromIdByLimit = 6,
        RequestTestFromIdByLimit = 7
    };

    enum EventType: int
    {
        LastMessagesIdAnswer = 1,
        CreateNewsMessage = 2,
        UpdateNewsMessage = 3,
        CreateEventMessage = 4,
        UpdateEventMessage = 5,
        CreateTestMessage = 6,
        UpdateTestMessage = 7,
        AllMesagesReceive = 8,
        NewsMessagesAnswer = 9,
        EventsMessagesAnswer = 10,
        TestMessagesAnswer = 11,
        NewsMessagesEnd = 12,
        EventsMessagesEnd = 13,
        TestMessagesEnd = 14
    };

private:
    void requestNews();
    void requestEvents();
    //void requestNewsLastMessageId();
    //void requestEventsLastMessageId();

    //void requestUserAvatar(QString userId, QString avatarId);
    void requestAttachmentImage(QString attachmentId, QString url);
    void requestYoutubeImage(QString youtubeId);

    //QList<DiscordMessage> parseMessagesJson(QByteArray byteArray);
    QList<DiscordMessage> parseMessagesJson(QJsonArray messagesArray);


//////////////////////
    void requestLastMessageId();
    void requestMessagesFromServer(OpCode opCode, bool includeFirst, QString lastMessageID);
    void receiveNews(QJsonArray messagesArray);
    void receiveEvents(QJsonArray messagesArray);

    void requestUserAvatar(QString avatarId, QString avatarUrl);

public slots:
    void setLastReadedNewsMessageID(QString id);
    void setLastReadedEventsMessageID(QString id);
    void requestNextNews(QString messageId);
    void requestNextEvents(QString messageId);

private slots:
    //void receiveNews(QNetworkReply* reply);
    //void receiveNextNews(QNetworkReply* reply);
    //void receiveEvents(QNetworkReply* reply);
    //void receiveNextEvents(QNetworkReply* reply);
    //void receiveNewsLastMessageId(QNetworkReply* reply);
    //void receiveEventsLastMessageId(QNetworkReply* reply);
    void receiveUserAvatar(QNetworkReply* reply, QString avatarId);
    void receiveAttachmentImage(QNetworkReply* reply, QString attachmentId);
    void receiveYoutubeImage(QNetworkReply* reply, QString youtubeId);
    void onSettingsLoaded();
    //void requestMessages();

///////////////
    void readMessage(QString messgae);
    void reconnect();
    void onConnected();
    void onDisconnected();



signals:
    void sendNews(QList<DiscordMessage> news);
    //void sendNextNews(QList<DiscordMessage> news);
    void sendEvents(QList<DiscordMessage> news);
    //void sendNextEvents(QList<DiscordMessage> news);
    void sendAvatar(QString avatarId, QImage discordAvatar);
    void sendAttachmentImage(QString attachmentId, QImage attachmentImage);
    void sendYoutubeImage(QString url, QImage attachmentImage);

private:

    QWebSocket m_webSocket;
    QTimer m_reconnectTimer;
    bool m_webSocketConnected = false;

    SettingsController* m_settingsController;

    QTimer* m_requestTimer;

    QNetworkAccessManager *m_networkManager;
    QList<QString> m_avatarIdList;

    QString m_lastNewsMessageID = "";
    QString m_lastEventMessageID = "";

    QString m_lastReadedNewsMessageID = "";
    QString m_lastReadedEventMessageID = "";

    bool m_isNewNewsMessage = true;
    bool m_isNewEventsMessage = true;

    bool m_newsMessagesEnd = false;
    bool m_eventsMessagesEnd = false;

};

#endif // NEWSSERVICEWEBPROCESSOR_H

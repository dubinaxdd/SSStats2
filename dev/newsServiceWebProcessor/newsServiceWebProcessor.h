#ifndef NEWSSERVICEWEBPROCESSOR_H
#define NEWSSERVICEWEBPROCESSOR_H

#include <QObject>
#include <QNetworkAccessManager>
#include <logger.h>
#include <baseTypes.h>
#include <QTimer>
#include <settingsController.h>

class DiscordWebProcessor : public QObject
{
    Q_OBJECT
public:
    explicit DiscordWebProcessor(SettingsController* settingsController, QObject *parent = nullptr);

private:
    void requestNews();
    void requestEvents();
    void requestNewsLastMessageId();
    void requestEventsLastMessageId();

    void requestUserAvatar(QString userId, QString avatarId);
    void requestAttachmentImage(QString attachmentId, QString url);
    void requestYoutubeImage(QString youtubeId);

    QList<DiscordMessage> parseMessagesJson(QByteArray byteArray);


public slots:
    void setLastReadedNewsMessageID(QString id);
    void setLastReadedEventsMessageID(QString id);
    void requestNextNews(QString messageId);
    void requestNextEvents(QString messageId);

private slots:
    void receiveNews(QNetworkReply* reply);
    void receiveNextNews(QNetworkReply* reply);
    void receiveEvents(QNetworkReply* reply);
    void receiveNextEvents(QNetworkReply* reply);
    void receiveNewsLastMessageId(QNetworkReply* reply);
    void receiveEventsLastMessageId(QNetworkReply* reply);
    void receiveUserAvatar(QNetworkReply* reply, QString avatarId);
    void receiveAttachmentImage(QNetworkReply* reply, QString attachmentId);
    void receiveYoutubeImage(QNetworkReply* reply, QString youtubeId);
    void onSettingsLoaded();

    void requestMessages();


signals:
    void sendNews(QList<DiscordMessage> news);
    void sendNextNews(QList<DiscordMessage> news);
    void sendEvents(QList<DiscordMessage> news);
    void sendNextEvents(QList<DiscordMessage> news);
    void sendAvatar(QString avatarId, QImage discordAvatar);
    void sendAttachmentImage(QString attachmentId, QImage attachmentImage);
    void sendYoutubeImage(QString url, QImage attachmentImage);

private:

    SettingsController* m_settingsController;

    QTimer* m_requestTimer;

    QNetworkAccessManager *m_networkManager;
    QList<QString> m_avatarIdList;

    QString m_afterNewsMessageID = "";
    QString m_afterEventsMessageID = "";

    QString m_lastNewsMessageID = "";
    QString m_lastEventMessageID = "";

    QString m_lastReadedNewsMessageID = "";
    QString m_lastReadedEventMessageID = "";

    QString m_newsChannelId;
    QString m_eventsChannelId;

    bool m_requestNewsNow = true;
    bool m_readyToRequest = true;

    bool m_needRequestNews = false;
    bool m_needRequestEvents = false;

    bool m_isFirstNewsRequest = true;
    bool m_isFirstEventsRequest = true;

    bool m_isNewNewsMessage = true;
    bool m_isNewEventsMessage = true;

};

#endif // NEWSSERVICEWEBPROCESSOR_H

#ifndef DISCORDWEBPROCESSOR_H
#define DISCORDWEBPROCESSOR_H

#include <QObject>
#include <QNetworkAccessManager>
#include <logger.h>
#include <baseTypes.h>
#include <QTimer>
#include <settingscontroller.h>

class DiscordWebProcessor : public QObject
{
    Q_OBJECT
public:
    explicit DiscordWebProcessor(SettingsController* settingsController, QObject *parent = nullptr);

private:
    void requestNews();
    void requestEvents();
    void requestNewsChannel();
    void requestEventsChannel();

    void requestUserAvatar(QString userId, QString avatarId);
    void requestAttachmentImage(QString attachmentId, QString url);

    QList<DiscordMessage> parseMessagesJson(QByteArray byteArray);


public slots:
    void setLastReadedNewsMessageID(QString id);
    void setLastReadedEventsMessageID(QString id);

private slots:
    void receiveNews(QNetworkReply* reply);
    void receiveEvents(QNetworkReply* reply);
    void receiveNewsChannel(QNetworkReply* reply);
    void receiveEventsChannel(QNetworkReply* reply);
    void receiveUserAvatar(QNetworkReply* reply, QString avatarId);
    void receiveAttachmentImage(QNetworkReply* reply, QString attachmentId);
    void onSettingsLoaded();

    void requestMessages();


signals:
    void sendNews(QList<DiscordMessage> news);
    void sendEvents(QList<DiscordMessage> news);
    void sendAvatar(QString avatarId, QImage discordAvatar);
    void sendAttachmentImage(QString attachmentId, QImage attachmentImage);


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

    bool m_requestNewsNow = true;
    bool m_readyToRequest = true;

    bool m_needRequestNews = false;
    bool m_needRequestEvents = false;

    bool m_isFirstNewsRequest = true;
    bool m_isFirstEventsRequest = true;


};

#endif // DISCORDWEBPROCESSOR_H
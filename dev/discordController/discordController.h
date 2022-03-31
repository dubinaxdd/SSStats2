#ifndef DISCORDCONTROLLER_H
#define DISCORDCONTROLLER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <logger.h>
#include <baseTypes.h>
#include <QTimer>

class DiscordController : public QObject
{
    Q_OBJECT
public:
    explicit DiscordController(QObject *parent = nullptr);

public slots:
    void requestNews();
    void requestEvents();
    void requestNewsChannel();
    void requestEventsChannel();

private:
    void requestUserAvatar(QString userId, QString avatarId);
    void requestAttachmentImage(QString attachmentId, QString url);

    QList<DiscordMessage> parseMessagesJson(QByteArray byteArray);

private slots:
    void receiveNews(QNetworkReply* reply);
    void receiveEvents(QNetworkReply* reply);
    void receiveNewsChannel(QNetworkReply* reply);
    void receiveEventsChannel(QNetworkReply* reply);
    void receiveUserAvatar(QNetworkReply* reply, QString avatarId);
    void receiveAttachmentImage(QNetworkReply* reply, QString attachmentId);

    void requestMessages();


signals:
    void sendNews(QList<DiscordMessage> news);
    void sendEvents(QList<DiscordMessage> news);
    void sendAvatar(QString avatarId, QImage discordAvatar);
    void sendAttachmentImage(QString attachmentId, QImage attachmentImage);


private:

    QTimer* m_requestTimer;

    QNetworkAccessManager *m_networkManager;
    QList<QString> m_avatarIdList;

    QString m_lastNewsMessageID = "";
    QString m_lastEventMessageID = "";

    bool m_requestNewsNow = true;

};

#endif // DISCORDCONTROLLER_H

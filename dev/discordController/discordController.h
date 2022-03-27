#ifndef DISCORDCONTROLLER_H
#define DISCORDCONTROLLER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <logger.h>
#include <baseTypes.h>

class DiscordController : public QObject
{
    Q_OBJECT
public:
    explicit DiscordController(QObject *parent = nullptr);
    void requestNews();

private:
    void requestUserAvatar(QString userId, QString avatarId);

private slots:
    void receiveNews(QNetworkReply* reply);
    void receiveUserAvatar(QNetworkReply* reply, QString avatarId);


signals:
    void sendNews(QList<DiscordMessage> news);

private:

    QNetworkAccessManager *m_networkManager;
    QList<DiscordMessage> discordNewsList;
    QMap<QString, QImage> m_avatarMap;

};

#endif // DISCORDCONTROLLER_H

#ifndef DISCORDCONTROLLER_H
#define DISCORDCONTROLLER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <logger.h>

class DiscordController : public QObject
{
    Q_OBJECT
public:
    explicit DiscordController(QObject *parent = nullptr);

private:
    void requestNews();


private slots:
    void receiveNews(QNetworkReply* reply);


signals:

private:

    QNetworkAccessManager *m_networkManager;

};

#endif // DISCORDCONTROLLER_H

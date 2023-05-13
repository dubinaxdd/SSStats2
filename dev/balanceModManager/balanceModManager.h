#ifndef BALANCEMODMANAGER_H
#define BALANCEMODMANAGER_H

#include <QObject>
#include <QStringList>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <logger.h>

class BalanceModManager : public QObject
{
    Q_OBJECT
public:
    explicit BalanceModManager(QObject *parent = nullptr);

    void downloadMod(QString version);

private:
    void downloadVersionsInfo();
    void receiveVersionsInfo(QNetworkReply *reply);

signals:
    void sendBalanceModVersoins(QStringList modVersions);

private:
    QStringList m_modVersions;
    QNetworkAccessManager *m_networkManager;

};

#endif // BALANCEMODMANAGER_H

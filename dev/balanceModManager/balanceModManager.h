#ifndef BALANCEMODMANAGER_H
#define BALANCEMODMANAGER_H

#include <QObject>
#include <QStringList>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <logger.h>
#include <baseTypes.h>
#include <QTimer>

class BalanceModManager : public QObject
{
    Q_OBJECT
public:
    explicit BalanceModManager(QObject *parent = nullptr);

    void downloadMod(QString version);

private:

    void receiveVersionsInfo(QNetworkReply *reply);

private slots:
    void downloadModsInfo();

signals:
    void sendModsInfo(QList <ModInfo> modInfo);

private:
    QList <ModInfo> m_modInfoList;
    QNetworkAccessManager *m_networkManager;
    QByteArray m_modInfoHash;
    QTimer* m_modsInfoRequestTimer;

};

#endif // BALANCEMODMANAGER_H

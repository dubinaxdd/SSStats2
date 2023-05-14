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
    void setSsPath(const QString &newSsPath);

private:
    void receiveVersionsInfo(QNetworkReply *reply);
    void downloadModsInfo();
    void checkCurrentModInGame();

private slots:
    void modsInfoTimerTimeout();

signals:
    void sendModsInfo(QList <ModInfo> modInfo);
    void sendCurrentModInGame(QString modName);

private:
    QList <ModInfo> m_modInfoList;
    QNetworkAccessManager *m_networkManager;
    QByteArray m_modInfoHash;
    QTimer* m_modsInfoRequestTimer;
    QString m_ssPath = "";
    QString m_currentModName = "";

};

#endif // BALANCEMODMANAGER_H

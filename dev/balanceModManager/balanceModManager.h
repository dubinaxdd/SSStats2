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

    void downloadMod(QString modTechnicalName);
    void setSsPath(const QString &newSsPath);
    void checkDownloadingQuery();

private:
    void receiveVersionsInfo(QNetworkReply *reply);
    void receiveMod(QNetworkReply *reply, QString modTechnicalName);
    void receiveChangeLog(QNetworkReply *reply, QString modTechnicalName);
    void downloadModsInfo();
    void checkCurrentModInGame();
    QString getChangeLogFromLocalFiles(QString modTechnicalName);

private slots:
    void modsInfoTimerTimeout();

public slots:
    void requestChangeLog(QString modTechnicalName);
    void requestDownloadMod(QString modTechnicalName);
    void uninstalMod(QString modTechnicalName);

signals:
    void sendModsInfo(QList <ModInfo> modInfo);
    void sendCurrentModInGame(QString modName);
    void changeLogReceived(QString modTechnicalName, QString changeLog);
    void sendModDownloadProgress(int progress, QString modTechnicalName);
    void sendModDownloaded(QString modTechnicalName);

private:
    QList <ModInfo> m_modInfoList;
    QNetworkAccessManager *m_networkManager;
    QByteArray m_modInfoHash;
    QTimer* m_modsInfoRequestTimer;
    QString m_ssPath = "";
    QString m_currentModName = "";

    bool m_modDownloadingProcessed = false;
    QStringList m_downloadingQuery;

};

#endif // BALANCEMODMANAGER_H

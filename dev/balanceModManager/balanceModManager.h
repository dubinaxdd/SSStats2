#ifndef BALANCEMODMANAGER_H
#define BALANCEMODMANAGER_H

#include <QObject>
#include <QStringList>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <logger.h>
#include <baseTypes.h>
#include <QTimer>
#include <QThread>
#include <BalanceModInstaller.h>
#include <settingsController.h>

class BalanceModManager : public QObject
{
    Q_OBJECT
public:
    explicit BalanceModManager(SettingsController* settingsController, QObject *parent = nullptr);
    ~BalanceModManager();

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

    void newActualModDetected(QString modTechnicalName, bool installed);

private slots:
    void modsInfoTimerTimeout();
    void onModInstalled(QString modTechnicalName);
    void onSettingsLoaded();

public slots:
    void requestChangeLog(QString modTechnicalName);
    void requestDownloadMod(QString modTechnicalName);
    void uninstalMod(QString modTechnicalName);
    void receiveCustomHotKeyPath(QString customHotKeysPath);

signals:
    void sendModsInfo(QList <ModInfo> modInfo);
    void sendCurrentModInGame(QString modName);
    void sendCustomHotKeysPath(QString customHotKeysPath);
    void changeLogReceived(QString modTechnicalName, QString changeLog);
    void sendModDownloadProgress(int progress, QString modTechnicalName);
    void sendModDownloaded(QString modTechnicalName);

    void installMod(InstallModData data);

private:

    BalanceModInstaller* m_balanceModInstaller;
    QThread* m_balanceModInstallerThread;

    SettingsController* m_settingsController;

    QList <ModInfo> m_modInfoList;
    QNetworkAccessManager *m_networkManager;
    QByteArray m_modInfoHash;
    QTimer* m_modsInfoRequestTimer;
    QString m_ssPath = "";
    QString m_currentModName = "";
    QString m_currentProfile = "";

    bool m_modDownloadingProcessed = false;
    QStringList m_downloadingQuery;

    QString m_customHotKeysPath = "";
    QString m_lastActualMod = "";


};

#endif // BALANCEMODMANAGER_H

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

    void downloadMod(QString modTechnicalName, bool overwritePrifiles);
    void setGamePath(GamePath *currentGame);
    void checkDownloadingQuery();

    bool showBalanceModBetaVersions() const;

private:
    void receiveVersionsInfo(QNetworkReply *reply);
    void receiveMod(QNetworkReply *reply, QString modTechnicalName, bool overwritePrifiles);
    void receiveChangeLog(QNetworkReply *reply, QString modTechnicalName);
    void downloadModsInfo();
    void checkCurrentModInGame();
    QString getChangeLogFromLocalFiles(QString modTechnicalName);

    bool getProfileExist(QString modTechnicalName);

    void newActualModDetected(QString modTechnicalName, bool installed);
    void updateTemplateProfilePath(QString modTechnicalName);
    void requestBetaTestPlayersList();
    void receiveBetaTestPlayersList(QNetworkReply *reply);

private slots:
    void modsInfoTimerTimeout();
    void onModInstalled(QString modTechnicalName);
    void onModUninstalled(QString modTechnicalName);
    void onSettingsLoaded();

public slots:
    void requestChangeLog(QString modTechnicalName);
    void requestDownloadMod(QString modTechnicalName);
    void uninstalMod(QString modTechnicalName);
    void receiveTemplateProfilePath(QString templateProfilePath);
    void activateMod(QString modTechnicalName);
    void updateTemplateProfilePath(bool useCustomTemplateProfilePath);
    void setProfileCopyMode(bool overwritePrifiles, QString modTechnicalName);
    void onGameLaunchStateChanged(bool lounched);
    void setCurrentPlayerSteamId(QString steamId);
    void updateHotKeysOnMod(QString modTechnicalName);

signals:
    void sendModsInfo(QList <ModInfo> modInfo);
    void sendCurrentModInGame(QString modName);
    void sendTemplateProfilePath(QString templateProfilePath);
    void changeLogReceived(QString modTechnicalName, QString changeLog);
    void sendModDownloadProgress(int progress, QString modTechnicalName);
    void sendModDownloaded(QString modTechnicalName);
    void uninstallMod(QString ssPath, QString modTechnicalName);
    void installMod(InstallModData data, bool overwritePrifiles);
    void sendInstallingModError(QString modTechnicalName);
    void requestProfileCopyMode(QString modTechnicalName);
    void sendModReadyForInstall(QString modUiName);
    void sendUpdateHotKeysOnMod(QString modTechnicalName, QString ssPath);
    void onHotKeysUpdated(QString modTechnicalName, bool result);
    void showBalanceModBetaVersionsChanged(bool howBalanceModBetaVersions);

private:
    GamePath* m_currentGame;

    BalanceModInstaller* m_balanceModInstaller;
    QThread* m_balanceModInstallerThread;
    QTimer* m_checkDownloadingQueryTimer;

    SettingsController* m_settingsController;

#ifdef BALANCE_MOD_TEST
    QString m_balanceModServerAddres = "http://crosspick.ru/dow_stats_client/test/";
#else
    QString m_balanceModServerAddres = "http://crosspick.ru/dow_stats_client/dow_stats_balance_mod/";
#endif

    QList <ModInfo> m_modInfoList;
    QNetworkAccessManager *m_networkManager;
    QByteArray m_modInfoHash;
    QTimer* m_modsInfoRequestTimer;
    QString m_currentModName = "";
    QString m_currentProfile = "";

    bool m_modDownloadingProcessed = false;
    QStringList m_downloadingQuery;

    QString m_templateProfilePath = "";
    QString m_lastActualMod = "";

    bool m_gameLounchedState = false;
    bool m_gameLounchedStateReceived = false;
    bool m_showBalanceModBetaVersions = false;
    bool m_betaTestPlayersListReceived = false;

    QString m_currentPlayerSteamId = "";
};

#endif // BALANCEMODMANAGER_H

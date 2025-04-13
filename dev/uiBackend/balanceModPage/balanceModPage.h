#ifndef BALANCEMODPAGE_H
#define BALANCEMODPAGE_H

#include <QAbstractListModel>
#include <baseTypes.h>
#include <settingsController.h>
#include <QUuid>
#include <balanceModManager.h>
#include <balanceModPageProxyModel.h>

class BalanceModPage : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString selectedModName READ selectedModName NOTIFY selectedModInfoChanged)
    Q_PROPERTY(QString selectedModVersion READ selectedModVersion NOTIFY selectedModInfoChanged)
    Q_PROPERTY(QString selectedChangeLog READ selectedChangeLog NOTIFY selectedModInfoChanged)
    Q_PROPERTY(bool selectedModIsCurrent READ selectedModIsCurrent NOTIFY selectedModInfoChanged)
    Q_PROPERTY(bool selectedModIsActual READ selectedModIsActual NOTIFY selectedModInfoChanged)
    Q_PROPERTY(bool selectedModIsInstalled READ selectedModIsInstalled NOTIFY selectedModInfoChanged)
    Q_PROPERTY(bool selectedModDownladingProcessed READ selectedModDownladingProcessed NOTIFY selectedModInfoChanged)

    Q_PROPERTY(QString downloadingProgress MEMBER m_downloadingProgress NOTIFY downloadingProgressChanged)
    Q_PROPERTY(QString currentModInGame READ currentModInGame WRITE setCurrentModInGame NOTIFY currentModInGameChanged)

    Q_PROPERTY(QString templateProfilePath READ templateProfilePath WRITE setTemplateProfilePath NOTIFY templateProfilePathChanged)

    Q_PROPERTY(bool autoUpdateBalanceMod READ autoUpdateBalanceMod WRITE setAutoUpdateBalanceMod NOTIFY autoUpdateBalanceModChanged)
    Q_PROPERTY(bool autoUninstallPreviousBalanceMod READ autoUninstallPreviousBalanceMod WRITE setAutoUninstallPreviousBalanceMod NOTIFY autoUninstallPreviousBalanceModChanged)
    Q_PROPERTY(bool useCustomTemplateProfilePath READ useCustomTemplateProfilePath WRITE setUseCustomTemplateProfilePath NOTIFY useCustomTemplateProfilePathChanged)

    Q_PROPERTY(bool profileCopyModeRequestMessageVisible READ profileCopyModeRequestMessageVisible WRITE setProfileCopyModeRequestMessageVisible NOTIFY profileCopyModeRequestMessageVisibleChanged)

    Q_PROPERTY(bool haveAvilableMods READ haveAvilableMods NOTIFY haveAvilableModsChanged)

    Q_PROPERTY(bool betaVersionsAvialble MEMBER m_betaVersionsAvialble NOTIFY betaVersionsAvialbleChanged)
    Q_PROPERTY(bool betaVersionsVisible READ betaVersionsVisible WRITE setBetaVersionsVisible NOTIFY betaVersionsVisibleChanged)
    Q_PROPERTY(BalanceModPageProxyModel* proxyModel MEMBER m_balanceModPageProxyModel CONSTANT)

public:
    explicit BalanceModPage(BalanceModManager* balanceModManager, SettingsController* settingsController, QObject *parent = nullptr);

    enum DataRoles {
        Selected = Qt::UserRole + 1,
        IsInstalled = Qt::UserRole + 2,
        Version = Qt::UserRole + 3,
        IsCurrentMod = Qt::UserRole + 4,
        DownladingProcessed = Qt::UserRole + 5,
        IsBeta = Qt::UserRole + 6,
        UiName
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount( const QModelIndex& parent ) const override;

    Q_INVOKABLE void slectItem(int itemIndex);
    Q_INVOKABLE void downloadSelectedMod();
    Q_INVOKABLE void uninstallSelectedMod();
    Q_INVOKABLE void updateHotKeysOnSelectedMod();
    Q_INVOKABLE void choiseTemplateProfilePath(QString templateProfilePath);
    Q_INVOKABLE void activateSelectedModInGame();
    Q_INVOKABLE void downloadLatestMod();
    Q_INVOKABLE void continueModInstallation(bool overwritePrifiles);

    const QString selectedModName() const;
    const QString selectedModVersion() const;
    const QString selectedChangeLog() const;
    const bool selectedModIsCurrent() const;
    const bool selectedModIsActual() const;
    const bool selectedModIsInstalled() const;
    const bool selectedModDownladingProcessed() const;

    const QString &currentModInGame() const;
    void setCurrentModInGame(const QString &newCurrentModInGame);

    const QString &templateProfilePath() const;
    void setTemplateProfilePath(const QString &newTemplateProfilePath);

    bool autoUpdateBalanceMod() const;
    void setAutoUpdateBalanceMod(bool newAutoUpdateBalanceMod);

    bool autoUninstallPreviousBalanceMod() const;
    void setAutoUninstallPreviousBalanceMod(bool newAutoUninstallPreviousBalanceMod);

    bool useCustomTemplateProfilePath() const;
    void setUseCustomTemplateProfilePath(bool newUseCustomTemplateProfilePath);

    bool profileCopyModeRequestMessageVisible() const;
    void setProfileCopyModeRequestMessageVisible(bool newProfileCopyModeRequestMessageVisible);

    bool isLatestModInstalled();


    bool downloadingProcessed() const;
    bool haveAvilableMods() const;

    bool betaVersionsVisible() const;
    void setBetaVersionsVisible(bool newBetaVersionsVisible);

protected:
   QHash<int, QByteArray> roleNames() const override;

private:
   void activateModInGame(int modIndex);
   void activateModInGame(QString modTechnicalName);
   void uninstallMod(int modIndex);
   void uninstallPreviousMod();
   QString formatingVersion(int itemIndex) const;

public slots:
   void receiveVersions(QList<ModInfo> modsInfo);
   void receiveCurrentModInGame(QString modName);
   void receiveChangeLog(QString modTechnicalName, QString changeLog);
   void receiveModDownloadProgress(int progress, QString modTechnicalName);
   void receiveModDownloaded(QString modTechnicalName);
   void receiveTemplateProfilePath(QString templateProfilePath);
   void receiveInstallingModError(QString modTechnicalName);
   void receiveProfileCopyModeRequest(QString modTechnicalName);
   void receiveModReadyForInstall(QString modTechnicalName);
   void receiveHotKeysUpdated(QString modTechnicalName, bool result);

private slots:
    void onSettingsLoaded();
    void onShowBalanceModBetaVersionsChanged(bool showBalanceModBetaVersions);

signals:
   void selectedModInfoChanged();
   void currentModInGameChanged();
   void downloadingProgressChanged();
   void templateProfilePathChanged();
   void autoUpdateBalanceModChanged();
   void autoUninstallPreviousBalanceModChanged();
   void useCustomTemplateProfilePathChanged();
   void changeLaunchMod(LaunchMod);
   void sendNotification(NotificationInfo notificationString);
   void profileCopyModeRequestMessageVisibleChanged();
   void haveAvilableModsChanged();
   void betaVersionsAvialbleChanged();
   void betaVersionsVisibleChanged();

private:
   BalanceModManager* m_balanceModManagerPtr;
   SettingsController* m_settingsController;
   BalanceModPageProxyModel* m_balanceModPageProxyModel;

   QList<ModInfo> m_modsInfo;
   int m_selectedItemIndex = 0;
   QString m_currentModInGame = "";
   QString m_downloadingProgress = "";
   QString m_templateProfilePath = "";
   bool m_autoUpdateBalanceMod = true;
   bool m_autoUninstallPreviousBalanceMod = false;
   bool m_useCustomTemplateProfilePath = false;
   QUuid m_uuid;
   QString m_currentDownloadedMod = "";
   bool m_downloadingProcessed = false;

   QString m_profileCopyModeRequestTechnicalName = "";
   bool m_profileCopyModeRequestMessageVisible = false;
   bool m_modsInfoReceived;
   bool m_betaVersionsAvialble = false;
   bool m_betaVersionsVisible = true;
};

#endif // BALANCEMODPAGE_H

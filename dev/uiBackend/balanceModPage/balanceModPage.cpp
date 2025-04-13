#include "balanceModPage.h"
#include <QDebug>
#include <QDir>

BalanceModPage::BalanceModPage(BalanceModManager* balanceModManager, SettingsController *settingsController, QObject *parent)
    : QAbstractListModel(parent)
    , m_balanceModManagerPtr(balanceModManager)
    , m_settingsController(settingsController)
{
    m_balanceModPageProxyModel = new BalanceModPageProxyModel(this);
    m_balanceModPageProxyModel->setSourceModel(this);

    QObject::connect(m_settingsController, &SettingsController::settingsLoaded, this, &BalanceModPage::onSettingsLoaded, Qt::QueuedConnection);
    QObject::connect(m_balanceModManagerPtr, &BalanceModManager::sendModsInfo, this, &BalanceModPage::receiveVersions, Qt::QueuedConnection);
    QObject::connect(m_balanceModManagerPtr, &BalanceModManager::sendCurrentModInGame, this, &BalanceModPage::receiveCurrentModInGame, Qt::QueuedConnection);
    QObject::connect(m_balanceModManagerPtr, &BalanceModManager::changeLogReceived, this, &BalanceModPage::receiveChangeLog, Qt::QueuedConnection);
    QObject::connect(m_balanceModManagerPtr, &BalanceModManager::sendModDownloadProgress, this, &BalanceModPage::receiveModDownloadProgress, Qt::QueuedConnection);
    QObject::connect(m_balanceModManagerPtr, &BalanceModManager::sendModDownloaded, this, &BalanceModPage::receiveModDownloaded, Qt::QueuedConnection);
    QObject::connect(m_balanceModManagerPtr, &BalanceModManager::sendTemplateProfilePath, this, &BalanceModPage::receiveTemplateProfilePath, Qt::QueuedConnection);
    QObject::connect(m_balanceModManagerPtr, &BalanceModManager::sendInstallingModError, this, &BalanceModPage::receiveInstallingModError, Qt::QueuedConnection);
    QObject::connect(m_balanceModManagerPtr, &BalanceModManager::requestProfileCopyMode, this, &BalanceModPage::receiveProfileCopyModeRequest, Qt::QueuedConnection);
    QObject::connect(m_balanceModManagerPtr, &BalanceModManager::sendModReadyForInstall, this, &BalanceModPage::receiveModReadyForInstall, Qt::QueuedConnection);
    QObject::connect(m_balanceModManagerPtr, &BalanceModManager::onHotKeysUpdated, this, &BalanceModPage::receiveHotKeysUpdated, Qt::QueuedConnection);
    QObject::connect(m_balanceModManagerPtr, &BalanceModManager::showBalanceModBetaVersionsChanged, this, &BalanceModPage::onShowBalanceModBetaVersionsChanged, Qt::QueuedConnection);
}

void BalanceModPage::onSettingsLoaded()
{
    qInfo(logInfo()) << "BalanceModPage::onSettingsLoaded()" << "load started";

    m_autoUpdateBalanceMod  =  m_settingsController->getSettings()->autoUpdateBalanceMod;
    emit autoUpdateBalanceModChanged();

    m_autoUninstallPreviousBalanceMod = m_settingsController->getSettings()->autoUninstallPreviousBalanceMod;
    emit autoUninstallPreviousBalanceModChanged();

    m_useCustomTemplateProfilePath = m_settingsController->getSettings()->useCustomTemplateProfilePath;
    emit useCustomTemplateProfilePathChanged();

    qInfo(logInfo()) << "BalanceModPage::onSettingsLoaded()" << "load finished";
}

void BalanceModPage::onShowBalanceModBetaVersionsChanged(bool showBalanceModBetaVersions)
{
    m_betaVersionsAvialble = showBalanceModBetaVersions;
    emit betaVersionsAvialbleChanged();
}

bool BalanceModPage::betaVersionsVisible() const
{
    return m_betaVersionsVisible;
}

void BalanceModPage::setBetaVersionsVisible(bool newBetaVersionsVisible)
{
    if (m_betaVersionsVisible == newBetaVersionsVisible)
        return;
    m_betaVersionsVisible = newBetaVersionsVisible;
    emit betaVersionsVisibleChanged();

    m_balanceModPageProxyModel->setBetVersionsVisible(newBetaVersionsVisible);

    slectItem(0);
}

bool BalanceModPage::downloadingProcessed() const
{
    return m_downloadingProcessed;
}

bool BalanceModPage::haveAvilableMods() const
{
    return m_modsInfo.count() > 0;
}

bool BalanceModPage::profileCopyModeRequestMessageVisible() const
{
    return m_profileCopyModeRequestMessageVisible;
}

void BalanceModPage::setProfileCopyModeRequestMessageVisible(bool newProfileCopyModeRequestMessageVisible)
{
    if (m_profileCopyModeRequestMessageVisible == newProfileCopyModeRequestMessageVisible)
        return;
    m_profileCopyModeRequestMessageVisible = newProfileCopyModeRequestMessageVisible;
    emit profileCopyModeRequestMessageVisibleChanged();
}

bool BalanceModPage::isLatestModInstalled()
{
    for (int i = 0; i < m_modsInfo.count(); i++)
    {
        if (m_modsInfo.at(i).isLatest)
            return m_modsInfo.at(i).isInstalled;
    }

    return false;
}

QVariant BalanceModPage::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_modsInfo.count())
        return QVariant();

    switch (role) {
        case UiName: return m_modsInfo.at(index.row()).uiName;
        case Version: return formatingVersion(index.row());
        case IsInstalled: return m_modsInfo.at(index.row()).isInstalled;
        case Selected: return index.row() == m_selectedItemIndex;
        case IsCurrentMod: return m_modsInfo.at(index.row()).isCurrentMod;
        case DownladingProcessed: return m_modsInfo.at(index.row()).downloadingProcessed;
        case IsBeta: return m_modsInfo.at(index.row()).isBeta;
    }

    return QVariant();
}

int BalanceModPage::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_modsInfo.count();
}

void BalanceModPage::slectItem(int itemIndex)
{
    QModelIndex proxyIndex = m_balanceModPageProxyModel->index(itemIndex,0);
    int sourceIndex = m_balanceModPageProxyModel->mapToSource(proxyIndex).row();
    int previousIndex = m_selectedItemIndex;

    m_selectedItemIndex = sourceIndex;

    QModelIndex previous = QAbstractItemModel::createIndex(previousIndex, 0);
    QModelIndex current = QAbstractItemModel::createIndex(sourceIndex, 0);

    emit dataChanged(previous, previous);
    emit dataChanged(current, current);

    emit selectedModInfoChanged();

    if(m_modsInfo.at(m_selectedItemIndex).changelog.isEmpty())
        m_balanceModManagerPtr->requestChangeLog(m_modsInfo.at(m_selectedItemIndex).technicalName);
}

void BalanceModPage::downloadSelectedMod()
{
    m_balanceModManagerPtr->requestDownloadMod(m_modsInfo.at(m_selectedItemIndex).technicalName);

    m_modsInfo[m_selectedItemIndex].downloadingProcessed = true;

    emit selectedModInfoChanged();

    QModelIndex index = QAbstractItemModel::createIndex(m_selectedItemIndex, 0);
    emit dataChanged(index, index);
}

void BalanceModPage::uninstallSelectedMod()
{
    uninstallMod(m_selectedItemIndex);
}

void BalanceModPage::updateHotKeysOnSelectedMod()
{
    m_balanceModManagerPtr->updateHotKeysOnMod(m_modsInfo.at(m_selectedItemIndex).technicalName);
}

void BalanceModPage::choiseTemplateProfilePath(QString templateProfilePath)
{
    setTemplateProfilePath(templateProfilePath.replace("file:///", "").replace("/", "\\"));
    m_balanceModManagerPtr->receiveTemplateProfilePath(m_templateProfilePath);
}

void BalanceModPage::activateSelectedModInGame()
{
    activateModInGame(m_selectedItemIndex);
}

void BalanceModPage::downloadLatestMod()
{
    for (int i = 0; i < m_modsInfo.count(); i++)
    {
        if(m_modsInfo[i].isLatest && !m_modsInfo[i].isInstalled)
        {
            m_balanceModManagerPtr->requestDownloadMod(m_modsInfo.at(i).technicalName);

            m_modsInfo[i].downloadingProcessed = true;

            emit selectedModInfoChanged();

            QModelIndex index = QAbstractItemModel::createIndex(i, 0);
            emit dataChanged(index, index);
        }
    }
}

void BalanceModPage::continueModInstallation(bool overwritePrifiles)
{
    m_balanceModManagerPtr->setProfileCopyMode(overwritePrifiles, m_profileCopyModeRequestTechnicalName);
    m_profileCopyModeRequestTechnicalName = "";
}

void BalanceModPage::activateModInGame(int modIndex)
{
    beginResetModel();
    for (int i = 0; i < m_modsInfo.count(); i++)
        m_modsInfo[i].isCurrentMod = false;

    m_modsInfo[modIndex].isCurrentMod = true;
    endResetModel();

    if (m_modsInfo[modIndex].isLatest)
        emit changeLaunchMod(LaunchMod::DowStatsBalanceMod);
    else
        emit changeLaunchMod(LaunchMod::LastSelectedMod);

    emit selectedModInfoChanged();
    m_balanceModManagerPtr->activateMod(m_modsInfo[modIndex].technicalName);
    setCurrentModInGame(m_modsInfo[modIndex].uiName);

    emit selectedModInfoChanged();
}

void BalanceModPage::activateModInGame(QString modTechnicalName)
{
    beginResetModel();
    for (int i = 0; i < m_modsInfo.count(); i++)
        m_modsInfo[i].isCurrentMod = m_modsInfo[i].technicalName == modTechnicalName;
    endResetModel();

    emit selectedModInfoChanged();
    m_balanceModManagerPtr->activateMod(modTechnicalName);
    setCurrentModInGame(modTechnicalName);
}

void BalanceModPage::uninstallMod(int modIndex)
{
    m_balanceModManagerPtr->uninstalMod(m_modsInfo.at(modIndex).technicalName);

    m_modsInfo[modIndex].isInstalled = false;

    emit selectedModInfoChanged();

    QModelIndex index = QAbstractItemModel::createIndex(modIndex, 0);
    emit dataChanged(index, index);


    NotificationInfo notificationInfo;

    notificationInfo.text = m_modsInfo[modIndex].uiName + tr(" has been uninstalled.");
    notificationInfo.type = NotificationType::Delete;

    emit sendNotification(notificationInfo);

    //Активируем латест версию мода если удалена устаревшая версия мода
    if (!m_modsInfo[modIndex].isLatest && m_modsInfo[modIndex].isCurrentMod)
    {
        for (int i = 0; i < m_modsInfo.count(); i++)
        {
            if(m_modsInfo.at(i).isLatest)
            {
                if (m_modsInfo.at(i).isInstalled)
                    activateModInGame(i);
                else
                {
                    activateModInGame("dxp2");
                    emit changeLaunchMod(LaunchMod::OriginalSoulstorm);
                }

                break;
            }
        }
    }
    //В остальных случаях, если это текущий мод, то активируем ваниллу
    else if (m_modsInfo[modIndex].isCurrentMod)
    {
        activateModInGame("dxp2");
        emit changeLaunchMod(LaunchMod::OriginalSoulstorm);
    }
}

void BalanceModPage::uninstallPreviousMod()
{
    for(int i = 0; i < m_modsInfo.count(); i++)
    {
        if (m_modsInfo.at(i).isPrevious && m_modsInfo.at(i).isInstalled)
        {
            uninstallMod(i);
            break;
        }
    }
}

QString BalanceModPage::formatingVersion(int itemIndex) const
{
    if(m_modsInfo.count() > itemIndex)
    {
        QString version = m_modsInfo.at(itemIndex).version;

        if (m_modsInfo.at(itemIndex).isLatest && m_modsInfo.at(itemIndex).isBeta)
            version += tr(" (Latest Beta)");
        else if(m_modsInfo.at(itemIndex).isLatest)
                version += tr(" (Latest)");
        else if(m_modsInfo.at(itemIndex).isBeta)
                version += tr(" (Beta)");

        return version;
    }

    return "";
}

const QString BalanceModPage::selectedModName() const
{
    if(m_modsInfo.count() > m_selectedItemIndex)
        return m_modsInfo.at(m_selectedItemIndex).uiName;

    return "";
}

const QString BalanceModPage::selectedModVersion() const
{
    return formatingVersion(m_selectedItemIndex);
}

const QString BalanceModPage::selectedChangeLog() const
{
    if(m_modsInfo.count() > m_selectedItemIndex)
        return m_modsInfo.at(m_selectedItemIndex).changelog;

    return "";
}

const bool BalanceModPage::selectedModIsCurrent() const
{
    if(m_modsInfo.count() > m_selectedItemIndex)
        return m_modsInfo.at(m_selectedItemIndex).isCurrentMod;

    return false;
}

const bool BalanceModPage::selectedModIsActual() const
{
    if(m_modsInfo.count() > m_selectedItemIndex)
        return m_modsInfo.at(m_selectedItemIndex).isLatest;

    return false;
}

const bool BalanceModPage::selectedModIsInstalled() const
{
    if(m_modsInfo.count() > m_selectedItemIndex)
        return m_modsInfo.at(m_selectedItemIndex).isInstalled;

    return false;
}

const bool BalanceModPage::selectedModDownladingProcessed() const
{
    if(m_modsInfo.count() > m_selectedItemIndex)
        return m_modsInfo.at(m_selectedItemIndex).downloadingProcessed;

    return false;
}

QHash<int, QByteArray> BalanceModPage::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[UiName] = "uiName";
    roles[Version] = "version";
    roles[IsInstalled] = "isInstalled";
    roles[Selected] = "selected";
    roles[IsCurrentMod] = "isCurrentMod";
    roles[DownladingProcessed] = "downladingProcessed";
    roles[IsBeta] = "isBeta";

    return roles;
}

void BalanceModPage::receiveVersions(QList <ModInfo> modsInfo)
{
   /* if ((m_modsInfo.count() == 0 && modsInfo.count() > 0 && m_modsInfoReceived) ||
         (m_modsInfo.count() < modsInfo.count() && m_modsInfoReceived))
    {
        NotificationInfo notificationInfo;
        notificationInfo.text = "New DoW Stats balance mod version is available.";
        notificationInfo.type = NotificationType::Info;
        notificationInfo.uuid = m_uuid;
        emit sendNotification(notificationInfo);
    }*/

    m_modsInfoReceived = true;

    if(modsInfo.count() < 1)
        return;

    m_selectedItemIndex = 0;

    beginResetModel();
    m_modsInfo = modsInfo;
    endResetModel();

    emit selectedModInfoChanged();
    emit haveAvilableModsChanged();

}

void BalanceModPage::receiveCurrentModInGame(QString modName)
{
    setCurrentModInGame(modName);

    beginResetModel();

    for(int i = 0; i < m_modsInfo.count(); i++)
        m_modsInfo[i].isCurrentMod = modName.toLower() == m_modsInfo[i].technicalName.toLower();

    endResetModel();
}

void BalanceModPage::receiveChangeLog(QString modTechnicalName, QString changeLog)
{
    for(int i = 0; i < m_modsInfo.count(); i++)
    {
        if (modTechnicalName == m_modsInfo.at(i).technicalName)
        {
            m_modsInfo[i].changelog = changeLog;
            emit selectedModInfoChanged();
        }
    }
}

void BalanceModPage::receiveModDownloadProgress(int progress, QString modTechnicalName)
{
    QString modUiName = "";

    for (int i = 0; i < m_modsInfo.count(); i++)
    {
        if (m_modsInfo.at(i).technicalName == modTechnicalName)
        {
            modUiName = m_modsInfo.at(i).uiName;
            break;
        }
    }

    m_downloadingProgress = tr("Downloading ") + modUiName + ": "  + QString::number(progress) + "%";
    m_downloadingProcessed = true;
    emit downloadingProgressChanged();

    if (m_currentDownloadedMod != modTechnicalName)
    {
        m_uuid = QUuid().createUuid();
        m_currentDownloadedMod = modTechnicalName;
    }

    NotificationInfo notificationInfo;
    notificationInfo.text = m_downloadingProgress;
    notificationInfo.type = NotificationType::Downloading;
    notificationInfo.uuid = m_uuid;
    emit sendNotification(notificationInfo);


}

void BalanceModPage::receiveModDownloaded(QString modTechnicalName)
{
    m_downloadingProgress = "";
    m_downloadingProcessed = false;
    emit downloadingProgressChanged();

    for (int i = 0; i < m_modsInfo.count(); i++)
    {
        if ( m_modsInfo.at(i).technicalName == modTechnicalName)
        {
            m_modsInfo[i].isInstalled = true;
            m_modsInfo[i].downloadingProcessed = false;
            m_modsInfo[i].installingError = false;

            if (m_selectedItemIndex == i)
                emit selectedModInfoChanged();

            if(m_modsInfo[i].isLatest)
            {
                activateModInGame(i);

                if ( m_settingsController->getSettings()->autoUninstallPreviousBalanceMod)
                        uninstallPreviousMod();
            }

            QModelIndex index = QAbstractItemModel::createIndex(i, 0);
            emit dataChanged(index, index);

            NotificationInfo notificationInfo;

            notificationInfo.text = m_modsInfo[i].uiName + tr(" has been installed.");
            notificationInfo.type = NotificationType::Ready;
            notificationInfo.uuid = m_uuid;

            emit sendNotification(notificationInfo);

            m_currentDownloadedMod = "";

            break;
        }
    }
}

void BalanceModPage::receiveTemplateProfilePath(QString templateProfilePath)
{
    setTemplateProfilePath(templateProfilePath);
}

void BalanceModPage::receiveInstallingModError(QString modTechnicalName)
{
    QString modUiName = "";

    for (int i = 0; i < m_modsInfo.count(); i++)
    {
        if ( m_modsInfo.at(i).technicalName == modTechnicalName)
        {
            modUiName = m_modsInfo.at(i).uiName;

            m_modsInfo[i].isInstalled = false;
            m_modsInfo[i].downloadingProcessed = false;
            m_modsInfo[i].installingError = true;

            if (m_selectedItemIndex == i)
                emit selectedModInfoChanged();

            QModelIndex index = QAbstractItemModel::createIndex(i, 0);
            emit dataChanged(index, index);


            NotificationInfo notificationInfo;

            notificationInfo.text = m_modsInfo[i].uiName + tr(" installation error.");
            notificationInfo.type = NotificationType::Warning;
            notificationInfo.uuid = m_uuid;

            emit sendNotification(notificationInfo);

            m_currentDownloadedMod = "";

            break;
        }
    }

    m_downloadingProgress = tr("Installing error: ") + modUiName;
    m_downloadingProcessed = false;
    emit downloadingProgressChanged();
}

void BalanceModPage::receiveProfileCopyModeRequest(QString modTechnicalName)
{
    m_profileCopyModeRequestTechnicalName = modTechnicalName;
    setProfileCopyModeRequestMessageVisible(true);
}

void BalanceModPage::receiveModReadyForInstall(QString modTechnicalName)
{

    NotificationInfo notificationInfo;

    notificationInfo.text = modTechnicalName + tr(" ready for install.");
    notificationInfo.type = NotificationType::Info;
    notificationInfo.uuid = m_uuid;

    emit sendNotification(notificationInfo);
}

void BalanceModPage::receiveHotKeysUpdated(QString modTechnicalName, bool result)
{
    for (int i = 0; i < m_modsInfo.count(); i++)
    {
        if ( m_modsInfo.at(i).technicalName == modTechnicalName)
        {
            NotificationInfo notificationInfo;

            if (result)
            {
                notificationInfo.text = tr("Extended hotkeys added for ") + m_modsInfo.at(i).uiName;
                notificationInfo.type = NotificationType::Ready;
            }
            else
            {
                notificationInfo.text = tr("An error occurred while updating one of the hotkey files for ") + m_modsInfo.at(i).uiName;
                notificationInfo.type = NotificationType::Warning;
            }

            notificationInfo.uuid = QUuid().createUuid();

            emit sendNotification(notificationInfo);
        }
    }
}

bool BalanceModPage::useCustomTemplateProfilePath() const
{
    return m_useCustomTemplateProfilePath;
}

void BalanceModPage::setUseCustomTemplateProfilePath(bool newUseCustomTemplateProfilePath)
{
    if (m_useCustomTemplateProfilePath == newUseCustomTemplateProfilePath)
        return;
    m_useCustomTemplateProfilePath = newUseCustomTemplateProfilePath;
    emit useCustomTemplateProfilePathChanged();

    m_settingsController->getSettings()->useCustomTemplateProfilePath = m_useCustomTemplateProfilePath;
    m_settingsController->saveSettings();

    m_balanceModManagerPtr->updateTemplateProfilePath(m_useCustomTemplateProfilePath);
}

bool BalanceModPage::autoUninstallPreviousBalanceMod() const
{
    return m_autoUninstallPreviousBalanceMod;
}

void BalanceModPage::setAutoUninstallPreviousBalanceMod(bool newAutoUninstallPreviousBalanceMod)
{
    if (m_autoUninstallPreviousBalanceMod == newAutoUninstallPreviousBalanceMod)
        return;
    m_autoUninstallPreviousBalanceMod = newAutoUninstallPreviousBalanceMod;
    emit autoUninstallPreviousBalanceModChanged();

    m_settingsController->getSettings()->autoUninstallPreviousBalanceMod = m_autoUninstallPreviousBalanceMod;
    m_settingsController->saveSettings();
}

const QString &BalanceModPage::templateProfilePath() const
{
    return m_templateProfilePath;
}

void BalanceModPage::setTemplateProfilePath(const QString &newTemplateProfilePath)
{
    if (m_templateProfilePath == newTemplateProfilePath)
        return;
    m_templateProfilePath = newTemplateProfilePath;
    emit templateProfilePathChanged();
}

const QString &BalanceModPage::currentModInGame() const
{
    return m_currentModInGame;
}

void BalanceModPage::setCurrentModInGame(const QString &newCurrentModInGame)
{
    QString uiModName = newCurrentModInGame;

    if (newCurrentModInGame == "dxp2")
        uiModName = tr("Original Soulstorm");
    else
    {
        for (int i = 0; i < m_modsInfo.count(); i++)
        {
            if (m_modsInfo.at(i).technicalName.toLower() == newCurrentModInGame.toLower())
            {
                uiModName = m_modsInfo.at(i).uiName;
                break;
            }
        }
    }

    if (m_currentModInGame == uiModName)
        return;

    m_currentModInGame = uiModName;
    emit currentModInGameChanged();
}

bool BalanceModPage::autoUpdateBalanceMod() const
{
    return m_autoUpdateBalanceMod;
}

void BalanceModPage::setAutoUpdateBalanceMod(bool newAutoUpdateBalanceMod)
{
    if (m_autoUpdateBalanceMod == newAutoUpdateBalanceMod)
        return;

    m_autoUpdateBalanceMod = newAutoUpdateBalanceMod;
    emit autoUpdateBalanceModChanged();

    m_settingsController->getSettings()->autoUpdateBalanceMod = m_autoUpdateBalanceMod;
    m_settingsController->saveSettings();
}

#include "SettingsPageModel.h"
#include <QDebug>

SettingsPageModel::SettingsPageModel(SettingsController *settingsController, QObject *parent)
    : QObject(parent)
    , m_settingsController(settingsController)
{
    QObject::connect(m_settingsController, &SettingsController::settingsLoaded, this, &SettingsPageModel::onSettingsLoaded, Qt::QueuedConnection);
}

void SettingsPageModel::installRussianFonts()
{
    m_russianFontsInstallInProcess = true;
    m_russianFontsInstallProgress = "Progress: 0%";
    emit russianFontsInstallProgressChanged();
    emit startRussianFontsInstall();
    emit russianFontsInstallInProcessChanged();
}

void SettingsPageModel::uninstallRussianFonts()
{
    m_russianFontsInstalledStatus = false;
    m_russianFontsInstallProgress = "Not installed";

    m_settingsController->getSettings()->russianFontsInstalled = m_russianFontsInstalledStatus;
    m_settingsController->saveSettings();

    emit startRussianFontsUninstall();
    emit russianFontsInstallProgressChanged();
    emit russianFontsInstallStatusChanged();
}

void SettingsPageModel::receiveRussianFontsDownloadProgress(int progress)
{
    m_russianFontsInstallProgress = "Progress: " + QString::number(progress) + "%";
    emit russianFontsInstallProgressChanged();
}

void SettingsPageModel::receiveRussianFontsInstallCompleeted()
{
    m_russianFontsInstalledStatus = true;
    m_russianFontsInstallInProcess = false;
    m_russianFontsInstallProgress = "Installed";

    m_settingsController->getSettings()->russianFontsInstalled = m_russianFontsInstalledStatus;
    m_settingsController->saveSettings();

    emit russianFontsInstallProgressChanged();
    emit russianFontsInstallStatusChanged();
    emit russianFontsInstallInProcessChanged();
}

void SettingsPageModel::receiveRussianFontsDownloadError()
{
    m_russianFontsInstallProgress = "Download error";
    emit russianFontsInstallProgressChanged();

    m_russianFontsInstallInProcess = false;
    emit russianFontsInstallInProcessChanged();
}

void SettingsPageModel::onSettingsLoaded()
{
    m_russianFontsInstalledStatus = m_settingsController->getSettings()->russianFontsInstalled;
    m_russianFontsInstallProgress = m_russianFontsInstalledStatus ? "Installed" : "Not installed";

    emit russianFontsInstallStatusChanged();
    emit russianFontsInstallProgressChanged();
}

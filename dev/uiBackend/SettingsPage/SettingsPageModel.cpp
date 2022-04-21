#include "SettingsPageModel.h"
#include <QDebug>

SettingsPageModel::SettingsPageModel(SettingsController *settingsController, QObject *parent)
    : QObject(parent)
    , m_settingsController(settingsController)
{
    QObject::connect(m_settingsController, &SettingsController::settingsLoaded, this, &SettingsPageModel::onSettingsLoaded, Qt::QueuedConnection);
}

void SettingsPageModel::onSettingsLoaded()
{
    m_russianFontsInstalledStatus = m_settingsController->getSettings()->russianFontsInstalled;
    m_russianFontsInstallProgress = m_russianFontsInstalledStatus ? "Installed" : "Not installed";
    emit russianFontsInstallStatusChanged();
    emit russianFontsInstallProgressChanged();

    m_cameraModInstalledStatus = m_settingsController->getSettings()->cameraModInstalled;
    m_cameraModInstallProgress = m_cameraModInstalledStatus ? "Installed" : "Not installed";
    emit cameraModInstallStatusChanged();
    emit cameraModInstallProgressChanged();
}

void SettingsPageModel::receiveDownloadProgress(InstMod mod, int progress)
{
    switch (mod) {
        case InstMod::RussianFonts : receiveRussianFontsDownloadProgress(progress); break;
        case InstMod::CameraMod : receiveCameraModDownloadProgress(progress); break;
    }
}

void SettingsPageModel::receiveInstallCompleeted(InstMod mod)
{
    switch (mod) {
        case InstMod::RussianFonts : receiveRussianFontsInstallCompleeted(); break;
        case InstMod::CameraMod : receiveCameraModInstallCompleeted(); break;
    }
}

void SettingsPageModel::receiveDownloadError(InstMod mod)
{
    switch (mod) {
        case InstMod::RussianFonts : receiveRussianFontsDownloadError(); break;
        case InstMod::CameraMod : receiveCameraModDownloadError(); break;
    }
}


void SettingsPageModel::installRussianFonts()
{
    m_russianFontsInstallInProcess = true;
    m_russianFontsInstallProgress = "Progress: 0%";
    emit russianFontsInstallProgressChanged();
    emit startInstall(InstMod::RussianFonts);
    emit russianFontsInstallInProcessChanged();
}

void SettingsPageModel::uninstallRussianFonts()
{
    m_russianFontsInstalledStatus = false;
    m_russianFontsInstallProgress = "Not installed";

    m_settingsController->getSettings()->russianFontsInstalled = m_russianFontsInstalledStatus;
    m_settingsController->saveSettings();

    emit startUninstall(InstMod::RussianFonts);
    emit russianFontsInstallProgressChanged();
    emit russianFontsInstallStatusChanged();
}

void SettingsPageModel::installCameraMod()
{
    m_cameraModInstallInProcess = true;
    m_cameraModInstallProgress = "Progress: 0%";
    emit cameraModInstallProgressChanged();
    emit startInstall(InstMod::CameraMod);
    emit cameraModInstallInProcessChanged();
}

void SettingsPageModel::uninstallCameraMod()
{
    m_cameraModInstalledStatus = false;
    m_cameraModInstallProgress = "Not installed";

    m_settingsController->getSettings()->cameraModInstalled = m_cameraModInstalledStatus;
    m_settingsController->saveSettings();

    emit startUninstall(InstMod::CameraMod);
    emit cameraModInstallProgressChanged();
    emit cameraModInstallStatusChanged();
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

void SettingsPageModel::receiveCameraModDownloadProgress(int progress)
{
    m_cameraModInstallProgress = "Progress: " + QString::number(progress) + "%";
    emit cameraModInstallProgressChanged();
}

void SettingsPageModel::receiveCameraModInstallCompleeted()
{
    m_cameraModInstalledStatus = true;
    m_cameraModInstallInProcess = false;
    m_cameraModInstallProgress = "Installed";

    m_settingsController->getSettings()->cameraModInstalled = m_cameraModInstalledStatus;
    m_settingsController->saveSettings();

    emit cameraModInstallProgressChanged();
    emit cameraModInstallStatusChanged();
    emit cameraModInstallInProcessChanged();
}

void SettingsPageModel::receiveCameraModDownloadError()
{
    m_cameraModInstallProgress = "Download error";
    emit cameraModInstallProgressChanged();

    m_cameraModInstallInProcess = false;
    emit cameraModInstallInProcessChanged();
}



#include "modsPage.h"

ModsPage::ModsPage(SettingsController *settingsController, QObject *parent)
    : QObject(parent)
    , m_settingsController(settingsController)
{
    QObject::connect(m_settingsController, &SettingsController::settingsLoaded, this, &ModsPage::onSettingsLoaded, Qt::QueuedConnection);
}

void ModsPage::onSettingsLoaded()
{
    m_russianFontsInstalledStatus = m_settingsController->getSettings()->russianFontsInstalled;
    m_russianFontsInstallProgress = m_russianFontsInstalledStatus ? "Installed" : "Not installed";
    emit russianFontsInstallStatusChanged();
    emit russianFontsInstallProgressChanged();

    m_cameraModInstalledStatus = m_settingsController->getSettings()->cameraModInstalled;
    m_cameraModInstallProgress = m_cameraModInstalledStatus ? "Installed" : "Not installed";
    emit cameraModInstallStatusChanged();
    emit cameraModInstallProgressChanged();

    m_gridHotkeysInstalledStatus = m_settingsController->getSettings()->gridHotkeysInstalled;
    m_gridHotkeysInstallProgress = m_gridHotkeysInstalledStatus ? "Installed" : "Not installed";
    emit gridHotkeysInstallStatusChanged();
    emit gridHotkeysInstallProgressChanged();
}


void ModsPage::receiveDownloadProgress(InstMod mod, int progress)
{
    switch (mod) {
        case InstMod::RussianFonts : receiveRussianFontsDownloadProgress(progress); break;
        case InstMod::CameraMod : receiveCameraModDownloadProgress(progress); break;
        case InstMod::GridHotkeys : receiveGridHotkeysDownloadProgress(progress); break;
    }
}

void ModsPage::receiveInstallCompleeted(InstMod mod)
{
    switch (mod) {
        case InstMod::RussianFonts : receiveRussianFontsInstallCompleeted(); break;
        case InstMod::CameraMod : receiveCameraModInstallCompleeted(); break;
        case InstMod::GridHotkeys : receiveGridHotkeysInstallCompleeted(); break;
    }
}

void ModsPage::receiveDownloadError(InstMod mod)
{
    switch (mod) {
        case InstMod::RussianFonts : receiveRussianFontsDownloadError(); break;
        case InstMod::CameraMod : receiveCameraModDownloadError(); break;
        case InstMod::GridHotkeys : receiveGridHotkeysDownloadError(); break;
    }
}

void ModsPage::receiveUnlockRacesStatus(bool status)
{
    if (status)
        m_unlockRacesStatus = "Status: Races unlocked";
    else
        m_unlockRacesStatus = "Status: Unlock races error";

    emit unlockRacesStatusChanged();
}


void ModsPage::installRussianFonts()
{
    m_russianFontsInstallInProcess = true;
    m_russianFontsInstallProgress = "Progress: 0%";
    emit russianFontsInstallProgressChanged();
    emit startInstall(InstMod::RussianFonts);
    emit russianFontsInstallInProcessChanged();
}

void ModsPage::uninstallRussianFonts()
{
    m_russianFontsInstalledStatus = false;
    m_russianFontsInstallProgress = "Not installed";

    m_settingsController->getSettings()->russianFontsInstalled = m_russianFontsInstalledStatus;
    m_settingsController->saveSettings();

    emit startUninstall(InstMod::RussianFonts);
    emit russianFontsInstallProgressChanged();
    emit russianFontsInstallStatusChanged();
}

void ModsPage::installCameraMod()
{
    m_cameraModInstallInProcess = true;
    m_cameraModInstallProgress = "Progress: 0%";
    emit cameraModInstallProgressChanged();
    emit startInstall(InstMod::CameraMod);
    emit cameraModInstallInProcessChanged();
}

void ModsPage::uninstallCameraMod()
{
    m_cameraModInstalledStatus = false;
    m_cameraModInstallProgress = "Not installed";

    m_settingsController->getSettings()->cameraModInstalled = m_cameraModInstalledStatus;
    m_settingsController->saveSettings();

    emit startUninstall(InstMod::CameraMod);
    emit cameraModInstallProgressChanged();
    emit cameraModInstallStatusChanged();
}

void ModsPage::installGridHotkeys()
{
    m_gridHotkeysInstallInProcess = true;
    m_gridHotkeysInstallProgress = "Progress: 0%";
    emit gridHotkeysInstallProgressChanged();
    emit startInstall(InstMod::GridHotkeys);
    emit gridHotkeysInstallInProcessChanged();
}

void ModsPage::uninstallGridHotkeys()
{
    m_gridHotkeysInstalledStatus = false;
    m_gridHotkeysInstallProgress = "Not installed";

    m_settingsController->getSettings()->gridHotkeysInstalled = m_gridHotkeysInstalledStatus;
    m_settingsController->saveSettings();

    emit startUninstall(InstMod::GridHotkeys);
    emit gridHotkeysInstallProgressChanged();
    emit gridHotkeysInstallStatusChanged();
}

void ModsPage::unlockRaces()
{
    m_unlockRacesStatus = "Status: Processed...";
    emit unlockRacesStatusChanged();

    emit sendUnlockRaces();
}

void ModsPage::receiveRussianFontsDownloadProgress(int progress)
{
    m_russianFontsInstallProgress = "Progress: " + QString::number(progress) + "%";
    emit russianFontsInstallProgressChanged();
}

void ModsPage::receiveRussianFontsInstallCompleeted()
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

void ModsPage::receiveRussianFontsDownloadError()
{
    m_russianFontsInstallProgress = "Download error";
    emit russianFontsInstallProgressChanged();

    m_russianFontsInstallInProcess = false;
    emit russianFontsInstallInProcessChanged();
}

void ModsPage::receiveCameraModDownloadProgress(int progress)
{
    m_cameraModInstallProgress = "Progress: " + QString::number(progress) + "%";
    emit cameraModInstallProgressChanged();
}

void ModsPage::receiveCameraModInstallCompleeted()
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

void ModsPage::receiveCameraModDownloadError()
{
    m_cameraModInstallProgress = "Download error";
    emit cameraModInstallProgressChanged();

    m_cameraModInstallInProcess = false;
    emit cameraModInstallInProcessChanged();
}

void ModsPage::receiveGridHotkeysDownloadProgress(int progress)
{
    m_gridHotkeysInstallProgress = "Progress: " + QString::number(progress) + "%";
    emit gridHotkeysInstallProgressChanged();
}

void ModsPage::receiveGridHotkeysInstallCompleeted()
{
    m_gridHotkeysInstalledStatus = true;
    m_gridHotkeysInstallInProcess = false;
    m_gridHotkeysInstallProgress = "Installed";

    m_settingsController->getSettings()->gridHotkeysInstalled = m_gridHotkeysInstalledStatus;
    m_settingsController->saveSettings();

    emit gridHotkeysInstallProgressChanged();
    emit gridHotkeysInstallStatusChanged();
    emit gridHotkeysInstallInProcessChanged();
}

void ModsPage::receiveGridHotkeysDownloadError()
{
    m_gridHotkeysInstallProgress = "Download error";
    emit gridHotkeysInstallProgressChanged();

    m_gridHotkeysInstallInProcess = false;
    emit gridHotkeysInstallInProcessChanged();
}

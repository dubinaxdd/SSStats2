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
    m_enableEventsSoundWhenGameMinimized = m_settingsController->getSettings()->enableEventsSoundWhenGameMinimized;
    emit enableEventsSoundWhenGameMinimizedChanged(m_enableEventsSoundWhenGameMinimized);

    m_enableEventsSoundWhenGameMaximized = m_settingsController->getSettings()->enableEventsSoundWhenGameMaximized;
    emit enableEventsSoundWhenGameMaximizedChanged(m_enableEventsSoundWhenGameMaximized);

    m_enableGameLoadEventSound = m_settingsController->getSettings()->enableGameLoadEventSound;
    m_enableGameStartEventSound = m_settingsController->getSettings()->enableGameStartEventSound;
    emit enableGameLoadEventSoundChanged(m_enableGameLoadEventSound);
    emit enableGameStartEventSoundChanged(m_enableGameStartEventSound);

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

    m_overlayVisible =  m_settingsController->getSettings()->overlayVisible;
    emit overlayVisibleChanged();

    m_win7SupportMode =  m_settingsController->getSettings()->win7SupportMode;
    emit win7SupportModeChanged();

    m_skipIntroVideo =  m_settingsController->getSettings()->skipIntroVideo;
    emit skipIntroVideoChanged();

    m_volume = m_settingsController->getSettings()->volume;
    emit volumeChanged(m_volume);
}

void SettingsPageModel::receiveDownloadProgress(InstMod mod, int progress)
{
    switch (mod) {
        case InstMod::RussianFonts : receiveRussianFontsDownloadProgress(progress); break;
        case InstMod::CameraMod : receiveCameraModDownloadProgress(progress); break;
        case InstMod::GridHotkeys : receiveGridHotkeysDownloadProgress(progress); break;
    }
}

void SettingsPageModel::receiveInstallCompleeted(InstMod mod)
{
    switch (mod) {
        case InstMod::RussianFonts : receiveRussianFontsInstallCompleeted(); break;
        case InstMod::CameraMod : receiveCameraModInstallCompleeted(); break;
        case InstMod::GridHotkeys : receiveGridHotkeysInstallCompleeted(); break;
    }
}

void SettingsPageModel::receiveDownloadError(InstMod mod)
{
    switch (mod) {
        case InstMod::RussianFonts : receiveRussianFontsDownloadError(); break;
        case InstMod::CameraMod : receiveCameraModDownloadError(); break;
        case InstMod::GridHotkeys : receiveGridHotkeysDownloadError(); break;
    }
}

void SettingsPageModel::receiveUnlockRacesStatus(bool status)
{
    if (status)
        m_unlockRacesStatus = "Status: Races unlocked";
    else
        m_unlockRacesStatus = "Status: Unlock races error";

    emit unlockRacesStatusChanged();
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

void SettingsPageModel::installGridHotkeys()
{
    m_gridHotkeysInstallInProcess = true;
    m_gridHotkeysInstallProgress = "Progress: 0%";
    emit gridHotkeysInstallProgressChanged();
    emit startInstall(InstMod::GridHotkeys);
    emit gridHotkeysInstallInProcessChanged();
}

void SettingsPageModel::uninstallGridHotkeys()
{
    m_gridHotkeysInstalledStatus = false;
    m_gridHotkeysInstallProgress = "Not installed";

    m_settingsController->getSettings()->gridHotkeysInstalled = m_gridHotkeysInstalledStatus;
    m_settingsController->saveSettings();

    emit startUninstall(InstMod::GridHotkeys);
    emit gridHotkeysInstallProgressChanged();
    emit gridHotkeysInstallStatusChanged();
}

void SettingsPageModel::unlockRaces()
{
    m_unlockRacesStatus = "Status: Processed...";
    emit unlockRacesStatusChanged();

    emit sendUnlockRaces();
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

void SettingsPageModel::receiveGridHotkeysDownloadProgress(int progress)
{
    m_gridHotkeysInstallProgress = "Progress: " + QString::number(progress) + "%";
    emit gridHotkeysInstallProgressChanged();
}

void SettingsPageModel::receiveGridHotkeysInstallCompleeted()
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

void SettingsPageModel::receiveGridHotkeysDownloadError()
{
    m_gridHotkeysInstallProgress = "Download error";
    emit gridHotkeysInstallProgressChanged();

    m_gridHotkeysInstallInProcess = false;
    emit gridHotkeysInstallInProcessChanged();
}

int SettingsPageModel::volume() const
{
    return m_volume;
}

void SettingsPageModel::setVolume(int newVolume)
{
    if (m_volume == newVolume)
        return;
    m_volume = newVolume;
    emit volumeChanged(m_volume);

    m_settingsController->getSettings()->volume = m_volume;
    m_settingsController->saveSettings();
}

bool SettingsPageModel::enableGameStartEventSound() const
{
    return m_enableGameStartEventSound;
}

void SettingsPageModel::setEnableGameStartEventSound(bool newEnableGameStartEventSound)
{
    if (m_enableGameStartEventSound == newEnableGameStartEventSound)
        return;
    m_enableGameStartEventSound = newEnableGameStartEventSound;
    emit enableGameStartEventSoundChanged(m_enableGameStartEventSound);

    m_settingsController->getSettings()->enableGameStartEventSound = m_enableGameStartEventSound;
    m_settingsController->saveSettings();
}

bool SettingsPageModel::enableGameLoadEventSound() const
{
    return m_enableGameLoadEventSound;
}

void SettingsPageModel::setEnableGameLoadEventSound(bool newEnableGameLoadEventSound)
{
    if (m_enableGameLoadEventSound == newEnableGameLoadEventSound)
        return;
    m_enableGameLoadEventSound = newEnableGameLoadEventSound;
    emit enableGameLoadEventSoundChanged(m_enableGameLoadEventSound);

    m_settingsController->getSettings()->enableGameLoadEventSound = m_enableGameLoadEventSound;
    m_settingsController->saveSettings();
}

bool SettingsPageModel::enableEventsSoundWhenGameMaximized() const
{
    return m_enableEventsSoundWhenGameMaximized;
}

void SettingsPageModel::setEnableEventsSoundWhenGameMaximized(bool newEnableEventsSoundWhenGameMaximized)
{
    if (m_enableEventsSoundWhenGameMaximized == newEnableEventsSoundWhenGameMaximized)
        return;
    m_enableEventsSoundWhenGameMaximized = newEnableEventsSoundWhenGameMaximized;
    emit enableEventsSoundWhenGameMaximizedChanged(m_enableEventsSoundWhenGameMaximized);

    m_settingsController->getSettings()->enableEventsSoundWhenGameMaximized = m_enableEventsSoundWhenGameMaximized;
    m_settingsController->saveSettings();
}

bool SettingsPageModel::enableEventsSoundWhenGameMinimized() const
{
    return m_enableEventsSoundWhenGameMinimized;
}

void SettingsPageModel::setEnableEventsSoundWhenGameMinimized(bool newEnableEventsSoundWhenGameMinimized)
{
    if (m_enableEventsSoundWhenGameMinimized == newEnableEventsSoundWhenGameMinimized)
        return;

    m_enableEventsSoundWhenGameMinimized = newEnableEventsSoundWhenGameMinimized;
    emit enableEventsSoundWhenGameMinimizedChanged(m_enableEventsSoundWhenGameMinimized);

    m_settingsController->getSettings()->enableEventsSoundWhenGameMinimized = m_enableEventsSoundWhenGameMinimized;
    m_settingsController->saveSettings();
}


bool SettingsPageModel::skipIntroVideo() const
{
    return m_skipIntroVideo;
}

void SettingsPageModel::setSkipIntroVideo(bool newSkipIntroVideo)
{
    if (m_skipIntroVideo ==  newSkipIntroVideo)
        return;

    m_skipIntroVideo = newSkipIntroVideo;
    emit skipIntroVideoChanged();

    m_settingsController->getSettings()->skipIntroVideo = m_skipIntroVideo;
    m_settingsController->saveSettings();
}

bool SettingsPageModel::win7SupportMode() const
{
    return m_win7SupportMode;
}

void SettingsPageModel::setWin7SupportMode(bool newWin7SupportMode)
{
    if (m_win7SupportMode == newWin7SupportMode)
        return;
    m_win7SupportMode = newWin7SupportMode;
    emit win7SupportModeChanged();

    m_settingsController->getSettings()->win7SupportMode = m_win7SupportMode;
    m_settingsController->saveSettings();
}

bool SettingsPageModel::overlayVisible() const
{
    return m_overlayVisible;
}

void SettingsPageModel::setOverlayVisible(bool newOverlayVisible)
{
    if (m_overlayVisible == newOverlayVisible)
        return;
    m_overlayVisible = newOverlayVisible;
    emit overlayVisibleChanged();

    m_settingsController->getSettings()->overlayVisible = m_overlayVisible;
    m_settingsController->saveSettings();
}



#include "modsPage.h"

ModsPage::ModsPage(SettingsController *settingsController, QObject *parent)
    : QObject(parent)
    , m_settingsController(settingsController)
    , m_russianFontsMod(new ModItem(this))
    , m_cameraMod(new ModItem(this))
    , m_gridHotkeysMod(new ModItem(this))
    , m_transparentCameraTrapezoidMod(new ModItem(this))
{
    QObject::connect(m_settingsController, &SettingsController::settingsLoaded, this, &ModsPage::onSettingsLoaded, Qt::QueuedConnection);

    QObject::connect(m_russianFontsMod, &ModItem::startInstall, this, [&]{emit startInstall(InstMod::RussianFonts);});
    QObject::connect(m_cameraMod, &ModItem::startInstall, this, [&]{emit startInstall(InstMod::CameraMod);});
    QObject::connect(m_gridHotkeysMod, &ModItem::startInstall, this, [&]{emit startInstall(InstMod::GridHotkeys);});
    QObject::connect(m_transparentCameraTrapezoidMod, &ModItem::startInstall, this, [&]{emit startInstall(InstMod::TransparentCameraTrapezoid);});

    QObject::connect(m_russianFontsMod, &ModItem::startUninstall, this, [&]{
        emit startUninstall(InstMod::RussianFonts);
        m_settingsController->getSettings()->russianFontsInstalled = false;
        m_settingsController->saveSettings();
    });
    QObject::connect(m_cameraMod, &ModItem::startUninstall, this, [&]{
        emit startUninstall(InstMod::CameraMod);
        m_settingsController->getSettings()->cameraModInstalled = false;
        m_settingsController->saveSettings();
    });
    QObject::connect(m_gridHotkeysMod, &ModItem::startUninstall, this, [&]{
        emit startUninstall(InstMod::GridHotkeys);
        m_settingsController->getSettings()->gridHotkeysInstalled = false;
        m_settingsController->saveSettings();
    });
    QObject::connect(m_transparentCameraTrapezoidMod, &ModItem::startUninstall, this, [&]{
        emit startUninstall(InstMod::TransparentCameraTrapezoid);
        m_settingsController->getSettings()->transparentCameraTrapezoidInstalled = false;
        m_settingsController->saveSettings();
    });
}

void ModsPage::onSettingsLoaded()
{
    m_russianFontsMod->setInstalledStatus(m_settingsController->getSettings()->russianFontsInstalled);
    m_cameraMod->setInstalledStatus(m_settingsController->getSettings()->cameraModInstalled);
    m_gridHotkeysMod->setInstalledStatus(m_settingsController->getSettings()->gridHotkeysInstalled);
    m_transparentCameraTrapezoidMod->setInstalledStatus(m_settingsController->getSettings()->transparentCameraTrapezoidInstalled);
}


void ModsPage::receiveDownloadProgress(InstMod mod, int progress)
{
    switch (mod) {
        case InstMod::RussianFonts :    m_russianFontsMod->setDownloadProgress(progress);   break;
        case InstMod::CameraMod :       m_cameraMod->setDownloadProgress(progress);         break;
        case InstMod::GridHotkeys :     m_gridHotkeysMod->setDownloadProgress(progress);    break;
        case InstMod::TransparentCameraTrapezoid :     m_transparentCameraTrapezoidMod->setDownloadProgress(progress);    break;
    }
}

void ModsPage::receiveInstallCompleeted(InstMod mod)
{
    switch (mod) {
        case InstMod::RussianFonts :
        {
            m_russianFontsMod->setInstallCompleeted();
            m_settingsController->getSettings()->russianFontsInstalled = true;
            break;
        }

        case InstMod::CameraMod : {
            m_cameraMod->setInstallCompleeted();
            m_settingsController->getSettings()->cameraModInstalled = true;
            break;
        }

        case InstMod::GridHotkeys :{
            m_gridHotkeysMod->setInstallCompleeted();
            m_settingsController->getSettings()->gridHotkeysInstalled = true;
            break;
        }

        case InstMod::TransparentCameraTrapezoid :{
            m_transparentCameraTrapezoidMod->setInstallCompleeted();
            m_settingsController->getSettings()->transparentCameraTrapezoidInstalled = true;
            break;
        }
    }

    m_settingsController->saveSettings();
}

void ModsPage::receiveDownloadError(InstMod mod)
{
    switch (mod) {
        case InstMod::RussianFonts :                m_russianFontsMod->setDownloadError();  break;
        case InstMod::CameraMod :                   m_cameraMod->setDownloadError();        break;
        case InstMod::GridHotkeys :                 m_gridHotkeysMod->setDownloadError();   break;
        case InstMod::TransparentCameraTrapezoid :  m_transparentCameraTrapezoidMod->setDownloadError();   break;
    }
}

void ModsPage::receiveUnlockRacesStatus(bool status)
{
    if (status)
        m_unlockRacesStatus = tr("Status: Races unlocked");
    else
        m_unlockRacesStatus = tr("Status: Unlock races error");

    emit unlockRacesStatusChanged();
}


void ModsPage::unlockRaces()
{
    m_unlockRacesStatus = tr("Status: Processed...");
    emit unlockRacesStatusChanged();
    emit sendUnlockRaces();
}

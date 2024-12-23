#include "modsPage.h"

ModsPage::ModsPage(ModsProcessor* modsProcessor, SettingsController *settingsController, QObject *parent)
    : QObject(parent)
    , m_modsProcessorPtr(modsProcessor)
    , m_settingsController(settingsController)
    , m_russianFontsMod(new ModItem(this))
    , m_cameraMod(new ModItem(this))
    , m_gridHotkeysMod(new ModItem(this))
    , m_transparentCameraTrapezoidMod(new ModItem(this))
{
    QObject::connect(m_settingsController, &SettingsController::settingsLoaded, this, &ModsPage::onSettingsLoaded, Qt::QueuedConnection);

    QObject::connect(m_russianFontsMod, &ModItem::startInstall, this, [&]{m_modsProcessorPtr->modInstallRequest(InstMod::RussianFonts);});
    QObject::connect(m_cameraMod, &ModItem::startInstall, this, [&]{m_modsProcessorPtr->modInstallRequest(InstMod::CameraMod);});
    QObject::connect(m_gridHotkeysMod, &ModItem::startInstall, this, [&]{m_modsProcessorPtr->modInstallRequest(InstMod::GridHotkeys);});
    QObject::connect(m_transparentCameraTrapezoidMod, &ModItem::startInstall, this, [&]{m_modsProcessorPtr->modInstallRequest(InstMod::TransparentCameraTrapezoid);});

    QObject::connect(m_russianFontsMod, &ModItem::startUninstall, this, [&]{
        m_modsProcessorPtr->uninstallRequest(InstMod::RussianFonts);
        m_settingsController->getSettings()->russianFontsInstalled = false;
        m_settingsController->saveSettings();
    });
    QObject::connect(m_cameraMod, &ModItem::startUninstall, this, [&]{
        m_modsProcessorPtr->uninstallRequest(InstMod::CameraMod);
        m_settingsController->getSettings()->cameraModInstalled = false;
        m_settingsController->saveSettings();
    });
    QObject::connect(m_gridHotkeysMod, &ModItem::startUninstall, this, [&]{
        m_modsProcessorPtr->uninstallRequest(InstMod::GridHotkeys);
        m_settingsController->getSettings()->gridHotkeysInstalled = false;
        m_settingsController->saveSettings();
    });
    QObject::connect(m_transparentCameraTrapezoidMod, &ModItem::startUninstall, this, [&]{
        m_modsProcessorPtr->uninstallRequest(InstMod::TransparentCameraTrapezoid);
        m_settingsController->getSettings()->transparentCameraTrapezoidInstalled = false;
        m_settingsController->saveSettings();
    });

    QObject::connect(m_modsProcessorPtr, &ModsProcessor::modInstallCompleeted, this, &ModsPage::receiveInstallCompleeted, Qt::QueuedConnection);
    QObject::connect(m_modsProcessorPtr, &ModsProcessor::installProgress, this, &ModsPage::receiveDownloadProgress, Qt::QueuedConnection);
    QObject::connect(m_modsProcessorPtr, &ModsProcessor::downloadError, this, &ModsPage::receiveDownloadError, Qt::QueuedConnection);
    QObject::connect(m_modsProcessorPtr, &ModsProcessor::sendUnlockRacesStatus, this, &ModsPage::receiveUnlockRacesStatus, Qt::QueuedConnection);
}

void ModsPage::onSettingsLoaded()
{
    qInfo(logInfo()) << "ModsPage::onSettingsLoaded()" << "load started";

    m_russianFontsMod->setInstalledStatus(m_settingsController->getSettings()->russianFontsInstalled);
    m_cameraMod->setInstalledStatus(m_settingsController->getSettings()->cameraModInstalled);
    m_gridHotkeysMod->setInstalledStatus(m_settingsController->getSettings()->gridHotkeysInstalled);
    m_transparentCameraTrapezoidMod->setInstalledStatus(m_settingsController->getSettings()->transparentCameraTrapezoidInstalled);

    qInfo(logInfo()) << "ModsPage::onSettingsLoaded()" << "load finished";
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
    m_modsProcessorPtr->unlockRaces();
}

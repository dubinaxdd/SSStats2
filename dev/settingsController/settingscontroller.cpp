#include "settingscontroller.h"

#define SETTINGS_FILE "settings/settings.ini"

SettingsController::SettingsController(QObject *parent) : QObject(parent)
{
    this->initializeSettings();
}

void SettingsController::initializeSettings()
{
    ss_stats_settings = new QSettings(SETTINGS_FILE, QSettings::IniFormat);

    // Если настроек нет, сохраняем дефолтные
    if(!ss_stats_settings->contains("no_fog")) ss_stats_settings->setValue("no_fog", false);

//    twitchWebSocketConnect->setShowHostIcon(chatSettings.value("showUserServerIcon").toBool());
}

void SettingsController::setNoFogState(bool state)
{
    ss_stats_settings->setValue("no_fog", state);
}

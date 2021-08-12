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
    if(!ss_stats_settings->contains("game/no_fog")) ss_stats_settings->setValue("game/no_fog", false);

    // Загружаем настройки и генерируем соответствующие сигналы    ;
    emit noFogStateInitialized(ss_stats_settings->value("game/no_fog").toBool());
}

void SettingsController::onSwitchNoFogStateChanged(bool state)
{
    ss_stats_settings->setValue("game/no_fog", state);
    emit noFogStateChanged(state);
}

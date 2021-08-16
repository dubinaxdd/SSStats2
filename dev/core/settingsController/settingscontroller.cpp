#include "settingscontroller.h"

#define SETTINGS_FILE "settings/settings.ini"
#define NO_FOG_STATE_KEY "game/no_fog"
#define DEFAULT_NO_FOG_STATE false


SettingsController::SettingsController(QObject *parent) : QObject(parent)
{
    this->initializeSettings();
}

void SettingsController::initializeSettings()
{
    ss_stats_settings = new QSettings(SETTINGS_FILE, QSettings::IniFormat);

    // Если настроек нет, сохраняем дефолтные
    if(!ss_stats_settings->contains(NO_FOG_STATE_KEY)) ss_stats_settings->setValue(NO_FOG_STATE_KEY, DEFAULT_NO_FOG_STATE);

    // Загружаем настройки и генерируем соответствующие сигналы
    emit noFogStateInitialized(ss_stats_settings->value(NO_FOG_STATE_KEY).toBool());
}

void SettingsController::onSwitchNoFogStateChanged(bool state)
{
    ss_stats_settings->setValue(NO_FOG_STATE_KEY, state);
    emit noFogStateChanged(state);
}

bool SettingsController::noFogState()
{
    return ss_stats_settings->value(NO_FOG_STATE_KEY, DEFAULT_NO_FOG_STATE).toBool();
}

// Переотправляем сигналы инициализации настроек относящихся к изменению процесса DoW при запуске или перезапуске процесса DoW
void SettingsController::onSsLaunchStateChanged(bool state)
{
    if (state)
    {
        emit noFogStateInitialized(ss_stats_settings->value(NO_FOG_STATE_KEY).toBool());
    }
}

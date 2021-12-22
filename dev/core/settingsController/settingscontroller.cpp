#include "settingscontroller.h"
#include "../logger/logger.h"

SettingsController::SettingsController(QObject *parent) : QObject(parent)
{
    m_settings = new Settings();
}

SettingsController::~SettingsController()
{
    saveSettings();
    delete m_settings;
}

void SettingsController::initializeSettings()
{
    ss_stats_settings = new QSettings("settings/settings.ini", QSettings::IniFormat, this);

    ss_stats_settings->sync();

    // Если настроек нет, сохраняем дефолтные
    m_settings->noFog = ss_stats_settings->value("game/no_fog", true).toBool();
    m_settings->smallGamePanelActive = ss_stats_settings->value("client/small_pannel_active", false).toBool();
    m_settings->showGamePanelPreset = ss_stats_settings->value("client/show_game_panel_preset", true).toBool();
    m_settings->scale = ss_stats_settings->value("client/scale", 1.0).toFloat();

    emit settingsLoaded();

    qInfo(logInfo()) << "Settings loaded";
}

void SettingsController::saveSettings()
{
    ss_stats_settings->setValue("game/no_fog", m_settings->noFog);
    ss_stats_settings->setValue("client/small_pannel_active", m_settings->smallGamePanelActive);
    ss_stats_settings->setValue("client/show_game_panel_preset", m_settings->showGamePanelPreset);
    ss_stats_settings->setValue("client/scale", m_settings->scale);

    ss_stats_settings->sync();
}

Settings *SettingsController::getSettings()
{
    return m_settings;
}


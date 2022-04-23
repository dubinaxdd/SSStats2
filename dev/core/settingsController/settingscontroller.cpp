#include <settingscontroller.h>
#include <logger.h>

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
    m_settings->lastReadedNewsMessageID = ss_stats_settings->value("client/last_readed_news_message_id", "").toString();
    m_settings->lastReadedEventsMessageID = ss_stats_settings->value("client/last_readed_events_message_id", "").toString();
    m_settings->russianFontsInstalled = ss_stats_settings->value("mods/russian_fonts_installed", false).toBool();
    m_settings->cameraModInstalled = ss_stats_settings->value("mods/camera_mod_installed", false).toBool();
    m_settings->gridHotkeysInstalled = ss_stats_settings->value("mods/grid_hotkeys_installed", false).toBool();
    m_settings->overlayVisible = ss_stats_settings->value("client/overlay_visible", true).toBool();
    m_settings->win7SupportMode = ss_stats_settings->value("client/win7_support_mode", false).toBool();

    emit settingsLoaded();

    qInfo(logInfo()) << "Settings loaded";
}

void SettingsController::saveSettings()
{
    ss_stats_settings->setValue("game/no_fog", m_settings->noFog);
    ss_stats_settings->setValue("client/small_pannel_active", m_settings->smallGamePanelActive);
    ss_stats_settings->setValue("client/show_game_panel_preset", m_settings->showGamePanelPreset);
    ss_stats_settings->setValue("client/scale", m_settings->scale);
    ss_stats_settings->setValue("client/last_readed_news_message_id", m_settings->lastReadedNewsMessageID);
    ss_stats_settings->setValue("client/last_readed_events_message_id", m_settings->lastReadedEventsMessageID);
    ss_stats_settings->setValue("mods/russian_fonts_installed", m_settings->russianFontsInstalled);
    ss_stats_settings->setValue("mods/camera_mod_installed", m_settings->cameraModInstalled);
    ss_stats_settings->setValue("mods/grid_hotkeys_installed", m_settings->gridHotkeysInstalled);
    ss_stats_settings->setValue("client/overlay_visible", m_settings->overlayVisible);
    ss_stats_settings->setValue("client/win7_support_mode", m_settings->win7SupportMode);

    ss_stats_settings->sync();
}

Settings *SettingsController::getSettings()
{
    return m_settings;
}


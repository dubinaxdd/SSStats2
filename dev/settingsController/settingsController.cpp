#include <settingsController.h>
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

    //[game]
    m_settings->noFog = ss_stats_settings->value("game/no_fog", true).toBool();
    m_settings->skipIntroVideo = ss_stats_settings->value("game/skip_intro_video", false).toBool();
    m_settings->currentMod = ss_stats_settings->value("game/current_mod", "dxp2").toString();
    m_settings->currentModVersion = ss_stats_settings->value("game/current_mod_version", "1.0").toString();
    m_settings->launchGameInWindow = ss_stats_settings->value("game/launch_game_in_window", false).toBool();
    m_settings->launchMode = static_cast<LaunchMode>(ss_stats_settings->value("game/launch_mode", LaunchMode::LastSelectedMod).toInt());

    //[client]
    m_settings->smallGamePanelActive = ss_stats_settings->value("client/small_pannel_active", false).toBool();
    m_settings->showGamePanelPreset = ss_stats_settings->value("client/show_game_panel_preset", true).toBool();
    m_settings->scale = ss_stats_settings->value("client/scale", 1.0).toDouble();
    m_settings->lastReadedNewsMessageID = ss_stats_settings->value("client/last_readed_news_message_id", "").toString();
    m_settings->lastReadedEventsMessageID = ss_stats_settings->value("client/last_readed_events_message_id", "").toString();
    m_settings->overlayVisible = ss_stats_settings->value("client/overlay_visible", true).toBool();
    m_settings->win7SupportMode = ss_stats_settings->value("client/win7_support_mode", false).toBool();
    m_settings->volume = ss_stats_settings->value("client/volume", 50).toInt();
    m_settings->enableEventsSoundWhenGameMaximized = ss_stats_settings->value("client/enable_events_sound_when_game_maximized", true).toBool();
    m_settings->enableEventsSoundWhenGameMinimized = ss_stats_settings->value("client/enable_events_sound_when_game_minimized", true).toBool();
    m_settings->enableGameLoadEventSound = ss_stats_settings->value("client/enable_game_load_event_sound", true).toBool();
    m_settings->enableGameStartEventSound = ss_stats_settings->value("client/enable_game_start_event_sound", true).toBool();
    m_settings->currentTheme = ss_stats_settings->value("client/current_theme", 0).toInt();
    m_settings->autorun = ss_stats_settings->value("client/autorun", true).toBool();

    //[mods]
    m_settings->russianFontsInstalled = ss_stats_settings->value("mods/russian_fonts_installed", false).toBool();
    m_settings->cameraModInstalled = ss_stats_settings->value("mods/camera_mod_installed", false).toBool();
    m_settings->gridHotkeysInstalled = ss_stats_settings->value("mods/grid_hotkeys_installed", false).toBool();
    m_settings->transparentCameraTrapezoidInstalled = ss_stats_settings->value("mods/transparent_camera_trapezoid_installed", false).toBool();
    m_settings->autoinstallDefaultMaps = ss_stats_settings->value("mods/autoinstall_default_maps", true).toBool();
    m_settings->autoinstallAllMaps = ss_stats_settings->value("mods/autoinstall_all_maps", false).toBool();

    //[balance_mod]
    m_settings->lastActualBalanceMod = ss_stats_settings->value("balance_mod/last_actual_balance_mod", "").toString();
    m_settings->templateProfilePath = ss_stats_settings->value("balance_mod/template_profile_path", "").toString();
    m_settings->autoUpdateBalanceMod = ss_stats_settings->value("balance_mod/auto_update_balance_mode", true).toBool();

    emit settingsLoaded();

    qInfo(logInfo()) << "Settings loaded";
}

void SettingsController::saveSettings()
{
    //[game]
    ss_stats_settings->setValue("game/no_fog", m_settings->noFog);
    ss_stats_settings->setValue("game/skip_intro_video", m_settings->skipIntroVideo);
    ss_stats_settings->setValue("game/current_mod", m_settings->currentMod);
    ss_stats_settings->setValue("game/current_mod_version", m_settings->currentModVersion);
    ss_stats_settings->setValue("game/launch_game_in_window", m_settings->launchGameInWindow);
    ss_stats_settings->setValue("game/launch_mode", m_settings->launchMode);

    //[client]
    ss_stats_settings->setValue("client/small_pannel_active", m_settings->smallGamePanelActive);
    ss_stats_settings->setValue("client/show_game_panel_preset", m_settings->showGamePanelPreset);
    ss_stats_settings->setValue("client/scale", m_settings->scale);
    ss_stats_settings->setValue("client/last_readed_news_message_id", m_settings->lastReadedNewsMessageID);
    ss_stats_settings->setValue("client/last_readed_events_message_id", m_settings->lastReadedEventsMessageID);
    ss_stats_settings->setValue("client/overlay_visible", m_settings->overlayVisible);
    ss_stats_settings->setValue("client/win7_support_mode", m_settings->win7SupportMode);
    ss_stats_settings->setValue("client/enable_events_sound_when_game_maximized", m_settings->enableEventsSoundWhenGameMaximized);
    ss_stats_settings->setValue("client/enable_events_sound_when_game_minimized", m_settings->enableEventsSoundWhenGameMinimized);
    ss_stats_settings->setValue("client/enable_game_load_event_sound", m_settings->enableGameLoadEventSound);
    ss_stats_settings->setValue("client/enable_game_start_event_sound", m_settings->enableGameStartEventSound);
    ss_stats_settings->setValue("client/volume", m_settings->volume);
    ss_stats_settings->setValue("client/current_theme", m_settings->currentTheme);
    ss_stats_settings->setValue("client/autorun", m_settings->autorun);

    //[mods]
    ss_stats_settings->setValue("mods/russian_fonts_installed", m_settings->russianFontsInstalled);
    ss_stats_settings->setValue("mods/camera_mod_installed", m_settings->cameraModInstalled);
    ss_stats_settings->setValue("mods/grid_hotkeys_installed", m_settings->gridHotkeysInstalled);
    ss_stats_settings->setValue("mods/transparent_camera_trapezoid_installed", m_settings->transparentCameraTrapezoidInstalled);
    ss_stats_settings->setValue("mods/autoinstall_default_maps", m_settings->autoinstallDefaultMaps);
    ss_stats_settings->setValue("mods/autoinstall_all_maps", m_settings->autoinstallAllMaps);

    //[balance_mod]
    ss_stats_settings->setValue("balance_mod/last_actual_balance_mod", m_settings->lastActualBalanceMod);
    ss_stats_settings->setValue("balance_mod/template_profile_path", m_settings->templateProfilePath);
    ss_stats_settings->setValue("balance_mod/auto_update_balance_mode", m_settings->autoUpdateBalanceMod);

    ss_stats_settings->sync();
}

Settings *SettingsController::getSettings()
{
    return m_settings;
}


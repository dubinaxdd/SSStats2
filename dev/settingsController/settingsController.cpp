#include <settingsController.h>
#include <logger.h>
#include <QDir>
#include <QCoreApplication>

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
    dow_stats_settings = new QSettings(QCoreApplication::applicationDirPath() + QDir::separator()  + "settings/settings.ini", QSettings::IniFormat, this);
    dow_stats_settings->sync();

    // Если настроек нет, сохраняем дефолтные

    //[game]
    m_settings->noFog = dow_stats_settings->value("game/no_fog", true).toBool();
    m_settings->skipIntroVideo = dow_stats_settings->value("game/skip_intro_video", false).toBool();
    m_settings->currentMod = dow_stats_settings->value("game/current_mod", "dxp2").toString();
    m_settings->currentModVersion = dow_stats_settings->value("game/current_mod_version", "1.0").toString();
    m_settings->launchGameInWindow = dow_stats_settings->value("game/launch_game_in_window", false).toBool();
    m_settings->launchMode = static_cast<LaunchMod>(dow_stats_settings->value("game/launch_mode", LaunchMod::LastSelectedMod).toInt());
    m_settings->lastGamePath = dow_stats_settings->value("game/last_game_path", "").toString();

    //[client]
    m_settings->smallGamePanelActive = dow_stats_settings->value("client/small_pannel_active", false).toBool();
    m_settings->showGamePanelPreset = dow_stats_settings->value("client/show_game_panel_preset", true).toBool();
    m_settings->scale = dow_stats_settings->value("client/scale", 1.0).toDouble();
    m_settings->lastReadedNewsMessageID = dow_stats_settings->value("client/last_readed_news_message_id", "").toString();
    m_settings->lastReadedEventsMessageID = dow_stats_settings->value("client/last_readed_events_message_id", "").toString();
    m_settings->overlayVisible = dow_stats_settings->value("client/overlay_visible", true).toBool();
    m_settings->win7SupportMode = dow_stats_settings->value("client/win7_support_mode", false).toBool();
    m_settings->volume = dow_stats_settings->value("client/volume", 50).toInt();
    m_settings->enableEventsSoundWhenGameMaximized = dow_stats_settings->value("client/enable_events_sound_when_game_maximized", true).toBool();
    m_settings->enableEventsSoundWhenGameMinimized = dow_stats_settings->value("client/enable_events_sound_when_game_minimized", true).toBool();
    m_settings->enableGameLoadEventSound = dow_stats_settings->value("client/enable_game_load_event_sound", true).toBool();
    m_settings->enableGameStartEventSound = dow_stats_settings->value("client/enable_game_start_event_sound", true).toBool();
    m_settings->currentTheme = dow_stats_settings->value("client/current_theme", 0).toInt();
    m_settings->autorun = dow_stats_settings->value("client/autorun", true).toBool();
    m_settings->enableAdvertising = dow_stats_settings->value("client/enable_advertising", false).toBool();
    m_settings->language = dow_stats_settings->value("client/language", "system").toString();

    //[mods]
    m_settings->russianFontsInstalled = dow_stats_settings->value("mods/russian_fonts_installed", false).toBool();
    m_settings->cameraModInstalled = dow_stats_settings->value("mods/camera_mod_installed", false).toBool();
    m_settings->gridHotkeysInstalled = dow_stats_settings->value("mods/grid_hotkeys_installed", false).toBool();
    m_settings->autoinstallDefaultMaps = dow_stats_settings->value("mods/autoinstall_default_maps", true).toBool();
    m_settings->autoinstallAllMaps = dow_stats_settings->value("mods/autoinstall_all_maps", false).toBool();

    //[balance_mod]
    m_settings->lastActualBalanceMod = dow_stats_settings->value("balance_mod/last_actual_balance_mod", "").toString();
    m_settings->templateProfilePath = dow_stats_settings->value("balance_mod/template_profile_path", "").toString();
    m_settings->autoUpdateBalanceMod = dow_stats_settings->value("balance_mod/auto_update_balance_mode", false).toBool();
    m_settings->autoUninstallPreviousBalanceMod = dow_stats_settings->value("balance_mod/auto_uninstall_previous_balance_mod", false).toBool();
    m_settings->useCustomTemplateProfilePath = dow_stats_settings->value("balance_mod/use_custom_template_profile_path", false).toBool();
    //m_settings->showBalanceModBetaVersions = ss_stats_settings->value("balance_mod/show_balance_mod_beta_versions", false).toBool();

    m_updaterSettings = new QSettings(QCoreApplication::applicationDirPath() + QDir::separator() + "Updater.ini", QSettings::IniFormat, this);
    m_updaterSettings->sync();

    m_settings->updateCheckAddress = m_updaterSettings->value("UpdateURL", "").toString();
    emit settingsLoaded();

    qInfo(logInfo()) << "Settings loaded";
}

void SettingsController::saveSettings()
{
    //[game]
    dow_stats_settings->setValue("game/no_fog", m_settings->noFog);
    dow_stats_settings->setValue("game/skip_intro_video", m_settings->skipIntroVideo);
    dow_stats_settings->setValue("game/current_mod", m_settings->currentMod);
    dow_stats_settings->setValue("game/current_mod_version", m_settings->currentModVersion);
    dow_stats_settings->setValue("game/launch_game_in_window", m_settings->launchGameInWindow);
    dow_stats_settings->setValue("game/launch_mode", m_settings->launchMode);
    dow_stats_settings->setValue("game/last_game_path", m_settings->lastGamePath);

    //[client]
    dow_stats_settings->setValue("client/small_pannel_active", m_settings->smallGamePanelActive);
    dow_stats_settings->setValue("client/show_game_panel_preset", m_settings->showGamePanelPreset);
    dow_stats_settings->setValue("client/scale", m_settings->scale);
    dow_stats_settings->setValue("client/last_readed_news_message_id", m_settings->lastReadedNewsMessageID);
    dow_stats_settings->setValue("client/last_readed_events_message_id", m_settings->lastReadedEventsMessageID);
    dow_stats_settings->setValue("client/overlay_visible", m_settings->overlayVisible);
    dow_stats_settings->setValue("client/win7_support_mode", m_settings->win7SupportMode);
    dow_stats_settings->setValue("client/enable_events_sound_when_game_maximized", m_settings->enableEventsSoundWhenGameMaximized);
    dow_stats_settings->setValue("client/enable_events_sound_when_game_minimized", m_settings->enableEventsSoundWhenGameMinimized);
    dow_stats_settings->setValue("client/enable_game_load_event_sound", m_settings->enableGameLoadEventSound);
    dow_stats_settings->setValue("client/enable_game_start_event_sound", m_settings->enableGameStartEventSound);
    dow_stats_settings->setValue("client/volume", m_settings->volume);
    dow_stats_settings->setValue("client/current_theme", m_settings->currentTheme);
    dow_stats_settings->setValue("client/autorun", m_settings->autorun);
    dow_stats_settings->setValue("client/enable_advertising", m_settings->enableAdvertising);
    dow_stats_settings->setValue("client/language", m_settings->language);

    //[mods]
    dow_stats_settings->setValue("mods/russian_fonts_installed", m_settings->russianFontsInstalled);
    dow_stats_settings->setValue("mods/camera_mod_installed", m_settings->cameraModInstalled);
    dow_stats_settings->setValue("mods/grid_hotkeys_installed", m_settings->gridHotkeysInstalled);
    dow_stats_settings->setValue("mods/autoinstall_default_maps", m_settings->autoinstallDefaultMaps);
    dow_stats_settings->setValue("mods/autoinstall_all_maps", m_settings->autoinstallAllMaps);

    //[balance_mod]
    dow_stats_settings->setValue("balance_mod/last_actual_balance_mod", m_settings->lastActualBalanceMod);
    dow_stats_settings->setValue("balance_mod/template_profile_path", m_settings->templateProfilePath);
    dow_stats_settings->setValue("balance_mod/auto_update_balance_mode", m_settings->autoUpdateBalanceMod);
    dow_stats_settings->setValue("balance_mod/auto_uninstall_previous_balance_mod", m_settings->autoUninstallPreviousBalanceMod);
    dow_stats_settings->setValue("balance_mod/use_custom_template_profile_path", m_settings->useCustomTemplateProfilePath);
    //ss_stats_settings->setValue("balance_mod/show_balance_mod_beta_versions", m_settings->showBalanceModBetaVersions);

    dow_stats_settings->sync();
}

Settings *SettingsController::getSettings()
{
    return m_settings;
}


#ifndef SETTINGSCONTROLLER_H
#define SETTINGSCONTROLLER_H

#include <QObject>
#include <QSettings>
#include <logger.h>

struct Settings
{
    bool noFog = false;
    bool smallGamePanelActive = false;
    bool showGamePanelPreset = false;
    double scale = 1.0;
    QString lastReadedNewsMessageID = "";
    QString lastReadedEventsMessageID = "";
    bool russianFontsInstalled = false;
    bool cameraModInstalled = false;
    bool gridHotkeysInstalled = false;
    bool transparentCameraTrapezoidInstalled = false;
    bool overlayVisible = true;
    bool win7SupportMode = false;
    bool launchGameInWindow = false;
    bool skipIntroVideo = false;
    bool enableEventsSoundWhenGameMaximized = false;
    bool enableEventsSoundWhenGameMinimized = false;
    bool enableGameLoadEventSound = false;
    bool enableGameStartEventSound = false;
    int  volume = 50;
    bool autoinstallDefaultMaps = true;
    bool autoinstallAllMaps = false;
    QString currentMod = "dxp2";
    QString currentModVersion = "1.0";
    int currentTheme = 0;
    QString lastActualBalanceMod = "";
    QString balanceModHotKeysPath = "";
    bool autoUpdateBalanceMod = true;
};

class SettingsController : public QObject
{
    Q_OBJECT

public:
    explicit SettingsController(QObject *parent = nullptr);
    ~SettingsController();

    void initializeSettings();
    void saveSettings();
    Settings *getSettings();


signals:
    void settingsLoaded();


private:
    QSettings *ss_stats_settings;
    Settings* m_settings;

};

#endif // SETTINGSCONTROLLER_H

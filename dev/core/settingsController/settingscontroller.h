#ifndef SETTINGSCONTROLLER_H
#define SETTINGSCONTROLLER_H

#include <QObject>
#include <QSettings>
#include "../logger/logger.h"

struct Settings
{
    bool noFog = false;
    bool smallGamePanelActive = false;
    bool showGamePanelPreset = false;
    float scale = 1.0;
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

#ifndef SETTINGSCONTROLLER_H
#define SETTINGSCONTROLLER_H

#include <QObject>
#include <QSettings>

class SettingsController : public QObject
{
    Q_OBJECT
public:
    explicit SettingsController(QObject *parent = nullptr);

    void initializeSettings();
    bool noFogState();
signals:
    void noFogStateInitialized(bool);
    void noFogStateChanged(bool);
public slots:
    void onSwitchNoFogStateChanged(bool state);

    void onSsLaunchStateChanged(bool state);
private:
    QSettings *ss_stats_settings;

};

#endif // SETTINGSCONTROLLER_H

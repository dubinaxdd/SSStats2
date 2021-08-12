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
signals:
    void noFogStateInitialized(bool);
public slots:
    void setNoFogState(bool state);

private:
    QSettings *ss_stats_settings;

};

#endif // SETTINGSCONTROLLER_H

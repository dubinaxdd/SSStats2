#ifndef MEMORYCONTROLLER_H
#define MEMORYCONTROLLER_H

#include <QObject>
#include <QDebug>
#include <QTextCodec>

#include <Windows.h>
#include "../../core/logger/logger.h"
#include "../../core/settingsController/settingscontroller.h"

class MemoryController : public QObject
{
    Q_OBJECT
public:
    explicit MemoryController(SettingsController* settingsController, QObject *parent = nullptr);

    void setSoulstormHwnd(HWND newSoulstormHwnd);

public slots:
    void onNoFogStateChanged(bool state);
    void onSsLaunchStateChanged(bool state);
    void onSettingsLoaded();
signals:

private:
    bool m_ssLaunchState = false;
    HWND m_soulstormHwnd = NULL;
    bool targetNoFog = false;
    bool currentNoFog = false;
    bool force = false; // применить изменения принудительно
    SettingsController* m_settingsController;
};

#endif // MEMORYCONTROLLER_H

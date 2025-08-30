#ifndef OVERLAYWINDOWCONTROLLER_H
#define OVERLAYWINDOWCONTROLLER_H

#include <QObject>
#include <QTimer>
#include "Windows.h"
#include <uiBackend.h>
#include <gameController.h>

class UiBackend;

class OverlayWindowController : public QObject
{
    Q_OBJECT
public:
    explicit OverlayWindowController(SettingsController* settingsController, GameController* soulstormController, QObject *parent = nullptr);
    void grubStatsWindow();
    void onExit();
    void setUiBackend(UiBackend* uiBackend);

public slots:

    void onSsShutdowned();
    void ssMaximized(bool maximized);
    void gameInitialized();
    void ssLaunched(bool ssLaunched);

private slots:
    void topmostTimerTimout();

private:
    int getGameTitleBarHeight();

private:
    QTimer* m_topmostTimer;

    int m_defaultWidth;
    int m_defaultHeight;
    int m_widthInGame;
    int m_heightInGame;

    HWND m_dowStatsHwnd;
    LONG m_defaultWindowLong;
    RECT m_gameRect;

    UiBackend* m_uiBackendPtr = nullptr;
    GameController* p_gameController;
    SettingsController* p_settingsController;


};

#endif // OVERLAYWINDOWCONTROLLER_H

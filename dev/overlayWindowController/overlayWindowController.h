#ifndef OVERLAYWINDOWCONTROLLER_H
#define OVERLAYWINDOWCONTROLLER_H

#include <QObject>
#include <QTimer>
#include "Windows.h"
#include <uiBackend.h>
#include <soulstormController.h>

class OverlayWindowController : public QObject
{
    Q_OBJECT
public:
    explicit OverlayWindowController(UiBackend* uiBackend, SoulstormController* soulstormController, QObject *parent = nullptr);
    void grubStatsWindow();
    void onExit();

public slots:

    void onSsShutdowned();
    void ssMaximized(bool maximized);
    void gameInitialized();
    void ssLaunched(bool ssLaunched);

private slots:
    void topmostTimerTimout();

private:
    QTimer* m_topmostTimer;

    int m_defaultWidth;
    int m_defaultHeight;
    int m_widthInGame;
    int m_heightInGame;

    HWND m_ssStatsHwnd;
    LONG m_defaultWindowLong;
    RECT m_ssRect;

    UiBackend* p_uiBackend;
    SoulstormController* p_soulstormController;


};

#endif // OVERLAYWINDOWCONTROLLER_H

#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QTextCodec>
#include <QTimer>
#include "Windows.h"
#include "../keyboardProcessor/keyboardprocessor.h"
#include <QQmlContext>
#include "../core/settingsController/settingscontroller.h"
#include "../uiBackend/uibackend.h"
#include "../ssConroller/sscontroller.h"

class Core : public QObject
{
    Q_OBJECT

public:
    Core(QQmlContext *context, QObject* parent = nullptr);
    bool event(QEvent *event) override;
    void grubStatsWindow();

    SsController *ssController() const;
    SettingsController *settingsController() const;

    UiBackend *uiBackend() const;

public slots:
    void onKeyEvent(QKeyEvent *event);
    void onMouseEvent(QMouseEvent *event);

private slots:
    void topmostTimerTimout();
    void ssMaximized(bool maximized);
    void gameInitialized();
    void ssLaunched(bool ssLaunched);

private:
    void registerTypes();

private:
    QTimer* m_topmostTimer;

    HWND m_ssStatsHwnd;
    LONG m_defaultWindowLong;
    RECT m_ssRect;

    KeyboardProcessor* m_keyboardProcessor;
    SettingsController* m_settingsController;
    UiBackend* m_uiBackend;
    SsController* m_ssController;


    int m_defaultWidth;
    int m_defaultHeight;
    int m_widthInGame;
    int m_heightInGame;

};

//Q_DECLARE_METATYPE(Core);
#endif // CORE_H

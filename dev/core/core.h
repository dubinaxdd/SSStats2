#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QTextCodec>
#include <QTimer>
#include "Windows.h"
#include <keyboardprocessor.h>
#include <QQmlContext>
#include <settingscontroller.h>
#include <uibackend.h>
#include <SsController.h>
#include <discordController.h>
#include <logger.h>
#include <ModsProcessor.h>


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
	
signals:
    void sendExit();

public slots:
    void onKeyEvent(QKeyEvent event);
    void onMouseEvent(QMouseEvent event);
    void onWheelEvent(QWheelEvent event);


private slots:
    void topmostTimerTimout();
    void onSsShutdowned();
    void ssMaximized(bool maximized);
    void gameInitialized();
    void ssLaunched(bool ssLaunched);
    void onExit();

private:
    void registerTypes();

private:

    Logger *m_logger;
    QTimer* m_topmostTimer;

    SettingsController* m_settingsController;
    KeyboardProcessor* m_keyboardProcessor;
    UiBackend* m_uiBackend;
    SsController* m_ssController;
    DiscordController* m_discordController;
    ModsProcessor* m_modsProcessor;

    int m_defaultWidth;
    int m_defaultHeight;
    int m_widthInGame;
    int m_heightInGame;

    HWND m_ssStatsHwnd;
    LONG m_defaultWindowLong;
    RECT m_ssRect;

};

//Q_DECLARE_METATYPE(Core);
#endif // CORE_H

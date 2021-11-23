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
#include "logger/logger.h"


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


private slots:
    void topmostTimerTimout();
    void ssMaximized(bool maximized);
    void gameInitialized();
    void ssLaunched(bool ssLaunched);
    void onExit();

private:
    void registerTypes();

private:

    Logger *m_logger;
    QTimer* m_topmostTimer;

    HWND m_ssStatsHwnd;
    LONG m_defaultWindowLong;
    RECT m_ssRect;

    SettingsController* m_settingsController;
    KeyboardProcessor* m_keyboardProcessor;
    UiBackend* m_uiBackend;
    SsController* m_ssController;


    int m_defaultWidth;
    int m_defaultHeight;
    int m_widthInGame;
    int m_heightInGame;

};

//Q_DECLARE_METATYPE(Core);
#endif // CORE_H

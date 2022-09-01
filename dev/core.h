#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <keyboardProcessor.h>
#include <QQmlContext>
#include <settingsController.h>
#include <uiBackend.h>
#include <soulstormController.h>
#include <discordWebProcessor.h>
#include <logger.h>
#include <modsProcessor.h>
#include <soundProcessor.h>
#include <overlayWindowController.h>
#include <statsServerProcessor.h>

class Core : public QObject
{
    Q_OBJECT
public:
    Core(QQmlContext *context, QObject* parent = nullptr);

    bool event(QEvent *event) override;
    UiBackend *uiBackend() const;
    OverlayWindowController *overlayWindowController() const;

signals:
    void sendExit();

private slots:
    void onExit();

public slots:
    void onKeyEvent(QKeyEvent event);
    void onMouseEvent(QMouseEvent event);
    void onWheelEvent(QWheelEvent event);

private:
    void registerTypes();

private:
    Logger *m_logger;
    SettingsController* m_settingsController;
    KeyboardProcessor* m_keyboardProcessor;
    UiBackend* m_uiBackend;
    SoulstormController* m_soulstormController;
    OverlayWindowController* m_overlayWindowController;
    DiscordWebProcessor* m_discordWebProcessor;
    ModsProcessor* m_modsProcessor;
    SoundProcessor* m_soundProcessor;
    StatsServerProcessor* m_statsServerProcessor;
};

//Q_DECLARE_METATYPE(Core);
#endif // CORE_H

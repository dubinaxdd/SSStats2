#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <keyboardProcessor.h>
#include <QQmlContext>
#include <settingsController.h>
#include <uiBackend.h>
#include <soulstormController.h>
#include <newsServiceWebProcessor.h>
#include <logger.h>
#include <modsProcessor.h>
#include <soundProcessor.h>
#include <overlayWindowController.h>
#include <statsServerProcessor.h>
#include <rankedModServiceProcessor.h>
#include <mapManager.h>
#include <balanceModManager.h>

class UiBackend;
class OverlayWindowController;

class Core : public QObject
{
    Q_OBJECT
public:
    Core(QQmlContext *context, QObject* parent = nullptr);

    bool event(QEvent *event) override;
    void exit();

    UiBackend *uiBackend() const;
    OverlayWindowController *overlayWindowController() const;
    SettingsController *settingsController() const;
    BalanceModManager *balanceModManager() const;
    ModsProcessor *modsProcessor() const;
    DiscordWebProcessor *discordWebProcessor() const;
    MapManager *mapManager() const;
    SoundProcessor *soundProcessor() const;
    StatsServerProcessor *statsServerProcessor() const;
    SoulstormController *soulstormController() const;
    RankedModServiceProcessor *rankedModServiceProcessor() const;

signals:
    void sendExit();
    void languageChanged(QString language);

public slots:
    void onKeyEvent(QKeyEvent event);
    void onMouseEvent(QMouseEvent event);
    void onWheelEvent(QWheelEvent event);

private:
    void registerTypes();
    void addConnections();

private:
    Logger *m_logger;
    SettingsController* m_settingsController;
    KeyboardProcessor* m_keyboardProcessor;
    SoulstormController* m_soulstormController;
    OverlayWindowController* m_overlayWindowController;
    DiscordWebProcessor* m_discordWebProcessor;
    ModsProcessor* m_modsProcessor;
    SoundProcessor* m_soundProcessor;
    StatsServerProcessor* m_statsServerProcessor;
    RankedModServiceProcessor* m_rankedModServiceProcessor;
    MapManager* m_mapManager;
    BalanceModManager* m_balanceModManager;
    UiBackend* m_uiBackend;
};

//Q_DECLARE_METATYPE(Core);
#endif // CORE_H

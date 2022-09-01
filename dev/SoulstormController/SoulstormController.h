#ifndef SSCONTROLLER_H
#define SSCONTROLLER_H

#include <QObject>
#include "Windows.h"
#include <QTimer>
#include <gameStateReader.h>
#include <soulstormMemoryReader.h>
#include <baseTypes.h>
#include <QVector>
#include <statsServerProcessor.h>
#include <soulstormMemoryController.h>
#include <apmMeter.h>
#include <QThread>
#include <logger.h>
#include <settingsController.h>
#include <lobbyEventReader.h>
#include <dowServerProcessor.h>
#include <QProcess>

class SoulstormController : public QObject
{
    Q_OBJECT
public:
    explicit SoulstormController(SettingsController* settingsController, QObject *parent = nullptr);
    ~SoulstormController();

    GameStateReader   *gameStateReader()    const;
    StatsServerProcessor *statsServerProcessor()       const;
    SoulstormMemoryController    *soulstormMemoryController()     const;
    APMMeter            *apmMeter()             const;
    SoulstormMemoryReader *soulstormMemoryReader()  const;
    LobbyEventReader    *lobbyEventReader()     const;
    DowServerProcessor  *dowServerProcessor()   const;

    HWND soulstormHwnd() const;
    LONG defaultSoulstormWindowLong() const;

    const QString &ssPath() const;

    bool getInputBlocked() const;
    bool getSsMaximized();
    bool ssWindowed() const;
    bool getUseWindows7SupportMode() const;



public slots:
    void blockSsWindowInput(bool state);
    void launchSoulstormWithSupportMode();
    void minimizeSsWithWin7Support();
    void launchSoulstorm();

private slots:
    void checkWindowState();
    void gameInitialized();
    void ssShutdown();
    void fullscrenizeSoulstorm();
    void minimizeSoulstorm();


signals:
    void ssLaunchStateChanged(bool lounched);
    void ssMaximized(bool maximized);
    void inputBlockStateChanged(bool);

private:
    QString getSsPathFromRegistry();
    QString getSteamPathFromRegistry();
    void parseSsSettings();

private:
    HWND m_soulstormHwnd = NULL;
    LONG m_defaultSoulstormWindowLong = NULL;

    QTimer* m_ssWindowControllTimer;
    
    QString m_ssPath;
    QString m_steamPath;

    QString m_currentProfile;

    bool inputBlocked = false;
    bool m_ssLounchState = false;
    bool m_ssMaximized = false;
    bool m_ssWindowed = false;

    bool m_gameInitialized = false;
    bool m_ssWindowCreated = false;

    GameStateReader* m_gameStateReader;
    LobbyEventReader* m_lobbyEventReader;

    SoulstormMemoryReader* m_soulstormMemoryReader;

    QThread m_soulstormMemoryReaderThread;

    APMMeter* m_apmMeter;
    StatsServerProcessor* m_statsServerProcessor;
    SoulstormMemoryController* m_soulstormMemoryController;
    SettingsController* m_settingsController;
    DowServerProcessor* m_dowServerProcessor;

    QProcess *m_soulstormProcess;
    bool m_useWindows7SupportMode = false;

    int m_ssWindowWidth = 0;
    int m_ssWindowHeight = 0;

};

#endif // SSCONTROLLER_H

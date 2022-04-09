#ifndef SSCONTROLLER_H
#define SSCONTROLLER_H

#include <QObject>
#include "Windows.h"
#include <QTimer>
#include <gameinforeader.h>
#include <playerssteamscanner.h>
#include <baseTypes.h>
#include <QVector>
#include <statscollector.h>
#include <memorycontroller.h>
#include <apmmeter.h>
#include <QThread>
#include <QMutex>
#include <logger.h>
#include <settingscontroller.h>
#include <lobbyeventreader.h>
#include <DowServerProcessor.h>
#include <QProcess>

class SsController : public QObject
{
    Q_OBJECT
public:
    explicit SsController(SettingsController* settingsController, QObject *parent = nullptr);
    ~SsController();

    bool getInputBlocked() const;

    const QString &ssPath() const;
    bool getSsMaximized();

    GameInfoReader *gameInfoReader() const;

    bool ssWindowed() const;

    HWND soulstormHwnd() const;

    StatsCollector *statsCollector() const;

    MemoryController *memoryController() const;

    APMMeter *apmMeter() const;

    PlayersSteamScanner *playersSteamScanner() const;

    LONG defaultSoulstormWindowLong() const;

    LobbyEventReader *lobbyEventReader() const;

    bool getUseWindows7SupportMode() const;

    DowServerProcessor *dowServerProcessor() const;

public slots:
    void blockInput(bool state);
    void launchSoulstormWithSupportMode();
    void minimizeSsWithWin7Support();
    void onLaunchSoulstorm();

private slots:
    void checkWindowState();
    void gameInitialized();
    void ssShutdown();
    //void readTestStats();

    void receivePlayrStemids(QMap<QString, QString> infoMap);
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
    void launchSoulstorm();

private:
    HWND m_soulstormHwnd = NULL;
    LONG m_defaultSoulstormWindowLong = NULL;

    QTimer* m_ssWindowControllTimer;
    
    QString m_ssPath;
    QString m_steamPath;

    QString currentProfile;

    bool inputBlocked = false;
    bool m_ssLounchState = false;
    bool m_ssMaximized = false;
    bool m_ssWindowed = false;

    bool m_gameInitialized;
    bool m_ssWindowCreated = false;

    GameInfoReader* m_gameInfoReader;
    LobbyEventReader* m_lobbyEventReader;

    PlayersSteamScanner* m_playersSteamScanner;

    QThread m_playersSteamScannerThread;
    QMutex m_playersSteamScannerMutex;

    APMMeter* m_apmMeter;
    StatsCollector* m_statsCollector;
    MemoryController* m_memoryController;
    SettingsController* m_settingsController;
    DowServerProcessor* m_dowServerProcessor;

    QProcess *m_soulstormProcess;
    bool useWindows7SupportMode = false;

    int m_ssWindowWidth = 0;
    int m_ssWindowHeight = 0;

};

#endif // SSCONTROLLER_H

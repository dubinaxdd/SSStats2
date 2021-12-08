#ifndef SSCONTROLLER_H
#define SSCONTROLLER_H

#include <QObject>
#include "Windows.h"
#include <QTimer>
#include "gameInfoReader/gameinforeader.h"
#include "playerSteamScanner/playerssteamscanner.h"
#include "../baseTypes/baseTypes.h"
#include <QVector>
#include "statsCollector/statscollector.h"
#include "memoryController/memorycontroller.h"
#include "apmMeter/apmmeter.h"
#include <QThread>
#include <QMutex>
#include "../core/logger/logger.h"
#include "../core/settingsController/settingscontroller.h"
#include "lobbyEventReader/lobbyeventreader.h"
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

public slots:
    void blockInput(bool state);
    void launchSoulstormWithSupportMode();

private slots:
    void checkWindowState();
    void gameInitialized();
    void ssShutdown();
    //void readTestStats();

    void receivePlayrStemids(QMap<QString, QString> infoMap);
    void fullscrenizeSoulstorm();

signals:
    void ssLaunchStateChanged(bool lounched);
    void ssMaximized(bool maximized);
    //void sendPlayersTestStats(QVector<PlayerStats> testStats);
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

    QProcess *m_soulstormProcess;
    bool useWindows7SupportMode = false;


};

#endif // SSCONTROLLER_H

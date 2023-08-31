#ifndef SSCONTROLLER_H
#define SSCONTROLLER_H

#include <QObject>
#include "Windows.h"
#include <QTimer>
#include <gameStateReader.h>
#include <soulstormMemoryReader.h>
#include <baseTypes.h>
#include <QVector>
#include <soulstormMemoryController.h>
#include <apmMeter.h>
#include <QThread>
#include <logger.h>
#include <settingsController.h>
#include <lobbyEventReader.h>
#include <dowServerProcessor.h>
#include <replayDataCollector.h>
#include <QProcess>

class SoulstormController : public QObject
{
    Q_OBJECT
public:
    explicit SoulstormController(SettingsController* settingsController, QObject *parent = nullptr);
    ~SoulstormController();

    GameStateReader   *gameStateReader()    const;
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



    const QString &steamPath() const;

    ReplayDataCollector *replayDataCollector() const;

public slots:
    void blockSsWindowInput(bool state);
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
    void sendAuthKey(QString authKey);

private:
    QString getSsPathFromRegistry();
    QString getSteamPathFromRegistry();
    void parseSsSettings();
    void updateSoulstormWindow();
    void writeCurrentModSettingInGame();

private:
    const QString m_ssPath;
    const QString m_steamPath;

    HWND m_soulstormHwnd = NULL;
    LONG m_defaultSoulstormWindowLong = NULL;

    QTimer* m_ssWindowControllTimer;
    
    QString m_currentProfile;

    bool inputBlocked = false;
    bool m_ssLounchState = false;
    bool m_ssMaximized = false;
    bool m_ssWindowed = false;

    bool m_gameInitialized = false;
    bool m_ssWindowCreated = false;


    LobbyEventReader* m_lobbyEventReader;

    SoulstormMemoryReader* m_soulstormMemoryReader;

    QThread m_soulstormMemoryReaderThread;

    APMMeter* m_apmMeter;
    SoulstormMemoryController* m_soulstormMemoryController;
    SettingsController* m_settingsController;
    GameStateReader* m_gameStateReader;
    DowServerProcessor* m_dowServerProcessor;
    ReplayDataCollector* m_replayDataCollector;

    QProcess *m_soulstormProcess;
    bool m_useWindows7SupportMode = false;

    int m_ssWindowWidth = 0;
    int m_ssWindowHeight = 0;

};

#endif // SSCONTROLLER_H

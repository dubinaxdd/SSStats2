#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include "Windows.h"
#include <QTimer>
#include <gameStateReader.h>
#include <gameMemoryReader.h>
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
#include <advertisingProcessor.h>

class GameController : public QObject
{
    Q_OBJECT
public:
    explicit GameController(SettingsController* settingsController, QObject *parent = nullptr);
    ~GameController();

    GameStateReader   *gameStateReader()    const;
    SoulstormMemoryController    *soulstormMemoryController()     const;
    APMMeter            *apmMeter()             const;
    GameMemoryReader *soulstormMemoryReader()  const;
    LobbyEventReader    *lobbyEventReader()     const;
    DowServerProcessor  *dowServerProcessor()   const;

    HWND soulstormHwnd() const;
    LONG defaultSoulstormWindowLong() const;

    bool getInputBlocked() const;
    bool getSsMaximized();
    bool ssWindowed() const;
    bool getUseWindows7SupportMode() const;

    const QString &steamPath() const;

    ReplayDataCollector *replayDataCollector() const;

    AdvertisingProcessor *advertisingProcessor() const;

    GamePath *currentGame() const;

public slots:
    void blockSsWindowInput(bool state);
    void minimizeSsWithWin7Support();
    void launchGame();

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
    QString getGamePathFromRegistry();
    QString getSteamPathFromRegistry();
    void parseSsSettings();
    void updateSoulstormWindow();
    void writeCurrentModSettingInGame();

    void findSoulstormPath();
    void findDefinitiveEdition();

private:

    QVector<GamePath> m_gamePathArray;
    GamePath* m_currentGame;

    const QString m_steamPath;

    HWND m_gameHwnd = NULL;
    LONG m_defaultSoulstormWindowLong = NULL;

    QTimer* m_ssWindowControllTimer;
    
    QString m_currentProfile;

    bool inputBlocked = false;
    bool m_ssLounchState = false;
    bool m_ssMaximized = false;
    bool m_ssWindowed = false;

    bool m_gameInitialized = false;
    bool m_gameWindowCreated = false;

    bool m_firstTimerTick = true;


    LobbyEventReader* m_lobbyEventReader;

    GameMemoryReader* m_gameMemoryReader;

    QThread m_soulstormMemoryReaderThread;

    APMMeter* m_apmMeter;
    SoulstormMemoryController* m_soulstormMemoryController;
    SettingsController* m_settingsController;
    GameStateReader* m_gameStateReader;
    DowServerProcessor* m_dowServerProcessor;
    ReplayDataCollector* m_replayDataCollector;
    AdvertisingProcessor* m_advertisingProcessor;

    QProcess *m_soulstormProcess;
    bool m_useWindows7SupportMode = false;

    int m_gameWindowWidth = 0;
    int m_gameWindowHeight = 0;

};

#endif // GAMECONTROLLER_H

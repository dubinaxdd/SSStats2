#ifndef SOULSTORMMEMORYCONTROLLER_H
#define SOULSTORMMEMORYCONTROLLER_H

#include <QObject>
#include <QDebug>
#include <QTextCodec>

#include <Windows.h>
#include <logger.h>
#include <settingsController.h>

class SoulstormMemoryController : public QObject
{
    Q_OBJECT
public:
    explicit SoulstormMemoryController(SettingsController* settingsController, QObject *parent = nullptr);

    void setGameHwnd(HWND newGameHwnd);

public slots:
    void onNoFogStateChanged(bool state);
    void onGameLaunchStateChanged(bool state);
    void onSettingsLoaded();
    void setCurrentGame(GamePath *newCurrentGame);
    void receiveCurrentMissionState(GameMissionState missionCurrentState);

signals:

private:
    void disableFogDE(bool disableFog);
    void disableFogSS();

private:
    GamePath *m_currentGame;

    bool m_gameLaunchState = false;
    HWND m_gameHwnd = NULL;
    bool m_targetNoFog = false;
    bool currentNoFog = false;
    bool force = false; // применить изменения принудительно
    SettingsController* m_settingsController;
};

#endif // SOULSTORMMEMORYCONTROLLER_H

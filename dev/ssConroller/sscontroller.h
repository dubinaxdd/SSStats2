#ifndef SSCONTROLLER_H
#define SSCONTROLLER_H

#include <QObject>
#include "Windows.h"
#include <QTimer>
#include "gameInfoReader/gameinforeader.h"
#include "gameInfoReader/playerssteamscanner.h"
#include "../baseTypes/baseTypes.h"
#include <QVector>
#include "statsCollector/statscollector.h"
#include "memoryController/memorycontroller.h"

class SsController : public QObject
{
    Q_OBJECT
public:
    explicit SsController(QObject *parent = nullptr);

    bool getInputBlocked() const;

    const QString &ssPath() const;
    bool getSsMaximized();

    GameInfoReader *gameInfoReader() const;

    bool ssWindowed() const;

    HWND soulstormHwnd() const;

    StatsCollector *statsCollector() const;

    MemoryController *memoryController() const;

public slots:
    void blockInput(bool block);

private slots:
    void checkSS();
    void gameInitialized();
    void ssShutdown();
    void readTestStats();

signals:
    void ssLaunchStateChanged(bool lounched);
    void ssMaximized(bool maximized);
    void sendPlayersTestStats(QVector<PlayerStats> testStats);

private:
    QString getSsPathFromRegistry();
    QString getSteamPathFromRegistry();
    void parseSsSettings();


private:
    HWND m_soulstormHwnd = NULL;

    QTimer* m_ssLounchControllTimer;
    QTimer* m_ssSteamPlayersScanTimer;
    
    QString m_ssPath;
    QString m_steamPath;

    QString currentProfile;

    bool inputBlocked = false;
    bool m_ssLounchState = false;
    bool m_ssMaximized = false;
    bool m_ssWindowed = false;

    GameInfoReader* m_gameInfoReader;
    PlayersSteamScanner* m_playersSteamScanner;

    StatsCollector* m_statsCollector;
    MemoryController* m_memoryController;


};

#endif // SSCONTROLLER_H

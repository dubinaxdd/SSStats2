#ifndef SSCONTROLLER_H
#define SSCONTROLLER_H

#include <QObject>
#include "Windows.h"
#include <QTimer>
#include "gameInfoReader/gameinforeader.h"
#include "../baseTypes/baseTypes.h"
#include <QVector>

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

public slots:
    void blockInput(bool block);

private slots:
    void checkSS();
    void gameInitialized();
    void ssShutdown();
    void readTestStats();

signals:
    void ssLounched(bool lounched);
    void ssMaximized(bool maximized);
    void sendPlayersTestStats(QVector<PlayerStats> testStats);

private:
    QString getSsPathFromRegistry();
    void parseSsSettings();

private:
    HWND m_soulstormHwnd = NULL;
    QTimer* m_ssLounchControllTimer;
    QString m_ssPath;

    QString currentProfile;

    bool inputBlocked = false;
    bool m_ssLounched = false;
    bool m_ssMaximized = false;
    bool m_ssWindowed = false;

    GameInfoReader* m_gameInfoReader;


};

#endif // SSCONTROLLER_H

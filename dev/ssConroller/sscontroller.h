#ifndef SSCONTROLLER_H
#define SSCONTROLLER_H

#include <QObject>
#include "Windows.h"
#include <QTimer>
#include "gameInfoReader/gameinforeader.h"

class SsController : public QObject
{
    Q_OBJECT
public:
    explicit SsController(QObject *parent = nullptr);

    bool getInputBlocked() const;

public slots:
    void blockInput(bool block);
    void delayBlockInputTimerTimout();

private slots:
    void checkSS();

signals:
    void ssLounched(bool lounched);
    void ssMaximized(bool maximized);

private:
    QString getSsPathFromRegistry();

private:
    HWND m_soulstormHwnd;
    QTimer* m_delayBlockInputTimer;
    QTimer* m_ssLounchControllTimer;
    QString m_ssPath;

    bool inputBlocked;
    bool m_ssLounched = false;
    bool m_ssMaximized = false;

    GameInfoReader* m_gameInfoReader;


};

#endif // SSCONTROLLER_H

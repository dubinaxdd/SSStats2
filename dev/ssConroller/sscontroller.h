#ifndef SSCONTROLLER_H
#define SSCONTROLLER_H

#include <QObject>
#include "Windows.h"
#include <QTimer>

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

private:
    HWND m_soulstormHwnd;
    QTimer* m_delayBlockInputTimer;

    QTimer* m_ssLounchControllTimer;

    bool inputBlocked;
    bool m_ssLounched = false;


};

#endif // SSCONTROLLER_H

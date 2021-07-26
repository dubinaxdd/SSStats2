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

public slots:
    void blockInput(bool block);
    void delayBlockInputTimerTimout();

signals:

private:
    HWND m_soulstormHwnd;
    QTimer* m_delayBlockInputTimer;
    bool inputBlocked;


};

#endif // SSCONTROLLER_H

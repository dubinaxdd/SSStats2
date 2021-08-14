#ifndef MEMORYCONTROLLER_H
#define MEMORYCONTROLLER_H

#include <QObject>
#include <QDebug>
#include <QTextCodec>

#include <Windows.h>

class MemoryController : public QObject
{
    Q_OBJECT
public:
    explicit MemoryController(QObject *parent = nullptr);

public slots:
    void onNoFogStateChanged(bool state);
    void onSsLaunchStateChanged(bool state);
signals:

private:
    bool m_ssLaunchState = false;
    HWND m_soulstormHwnd = NULL;
    bool targetNoFog = false;
    bool currentNoFog = false;
    bool force = false; // применить изменения принудительно
};

#endif // MEMORYCONTROLLER_H

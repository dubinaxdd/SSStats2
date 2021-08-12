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
signals:

private:
    HWND m_soulstormHwnd = NULL;
    bool targetNoFog;
    bool currentNoFog;
    bool force = false; // применить изменения изменения принудительно
};

#endif // MEMORYCONTROLLER_H

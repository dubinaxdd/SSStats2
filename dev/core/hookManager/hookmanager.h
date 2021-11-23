#ifndef HOOKMANAGER_H
#define HOOKMANAGER_H

#include <QObject>
#include <windows.h>
#include "../core.h"
//#pragma comment(lib, "user32.lib")


class HookManager : public QObject
{
    Q_OBJECT

public:
    static HookManager* instance() {
        if (!p_instance)
            p_instance = new HookManager();
        return p_instance;
    }

    bool inputBlock();

    void reconnectHook();

    void setCore(Core *newCore);

    Core *core() const;

public slots:
    void onInputBlockStateChanged(bool state);

private:
    static HookManager *p_instance;
    bool m_inputBlock = false;
    Core* m_core;

    HookManager();
    ~HookManager();

//signals:
   // void keyEvent(QKeyEvent event);
   // void mouseEvent(QMouseEvent event);

};

#endif // HOOKMANAGER_H

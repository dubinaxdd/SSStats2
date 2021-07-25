#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QTextCodec>
#include <QTimer>
#include "Windows.h"
#include "../keyboardProcessor/keyboardprocessor.h"
#include <QQmlContext>
#include "../uiBackend/uibackend.h"

class Core : public QObject
{
    Q_OBJECT
public:
    Core(QQmlContext *context, QObject* parent = nullptr);
    bool event(QEvent *event) override;
    void startTopmost();

private slots:
    void topmostTimerTimout();

private:
    QTimer* m_topmostTimer;
    HWND m_ssStatsHwnd;
    KeyboardProcessor* m_keyboardProcessor;
    UiBackend* m_uiBackend;

};

//Q_DECLARE_METATYPE(Core);
#endif // CORE_H

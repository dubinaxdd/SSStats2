#ifndef APMMETER_H
#define APMMETER_H

#include <QObject>
#include <QKeyEvent>
#include <QPoint>
#include <QTimer>
#include <QDateTime>
#include "../../baseTypes/baseTypes.h"
#include "../../core/logger/logger.h"

#define TICKS_FOR_ANALYSE 20
#define MEASURE_TICK_LENGTH 500 // msec

//typedef struct {
//	quint64	timePoint;
//    bool	mouseAction;
//    bool	keyAction;
//    QPoint mousePosition
//} APMAction;


class APMMeter : public QObject
{
    Q_OBJECT
public:
    explicit APMMeter(QObject *parent = nullptr);

public slots:
    void onKeyPressEvent (QKeyEvent *event);
    void onMousePressEvent (QPoint mousePosition);

    void onGameStarted(SsGameState gameCurrentState);
    void onGameStopped();

signals:
    void currentApmCalculated(quint64);
    void averageApmCalculated(quint64);

    void sendAverrageApm(int apm);

private slots:
    void calculateAPM();

private:
    bool started = false;

    quint64 start_time = 0;

    quint64	current_tick_mouse_actions_count;
    quint64	current_tick_keys_actions_count;

    quint64	full_actions_count;

    quint64	tick_actions_count;

    quint64 m_lastAverrageApm;

    QVector<quint64> ticksActionsArray; // QVector быстрее аналогичных классов типа QList и т.д.

    QTimer *measureTickTimer;

//    QVector<APMFrame> actionsArray;
};

#endif // APMMETER_H

#ifndef APMMETER_H
#define APMMETER_H

#include <QObject>
#include <QKeyEvent>
#include <QPoint>
#include <QTimer>
#include <QDateTime>

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

    void start();
    void stop();

signals:
    void currentApmAnalysed(quint64);
    void averageApmAnalysed(quint64);

private slots:
    void analyseAPM();
private:

    quint64 start_time = 0;

    quint64	full_actions_count;

    quint64	tick_actions_count;
    QVector<quint64> ticksActionsArray; // QVector быстрее аналогичных классов типа QList и т.д.

    QTimer *measureTickTimer;

//    QVector<APMFrame> actionsArray;

};

#endif // APMMETER_H

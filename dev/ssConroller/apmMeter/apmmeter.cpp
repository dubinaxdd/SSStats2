#include "apmmeter.h"
#include <QDebug>

APMMeter::APMMeter(QObject *parent) : QObject(parent)
{
    measureTickTimer = new QTimer(this);
    QObject::connect(measureTickTimer, &QTimer::timeout, this, &APMMeter::calculateAPM);
}

void APMMeter::onKeyPressEvent(QKeyEvent *event)
{
    if(started) {
        ++current_tick_keys_actions_count;
       // qDebug() << "keyPress: key =" << event->key();
    }
}

void APMMeter::onMousePressEvent(QPoint mousePosition)
{
    if(started) {
        ++current_tick_mouse_actions_count;
        //qDebug() << "mousePress: x =" << mousePosition.x() << " y=" << mousePosition.y();
    }
}

void APMMeter::onGameStarted()
{
    //qDebug() << "Game STARTED. APM analyse started.";
    started = true;

    // RESET ALL APM DATA
    current_tick_mouse_actions_count = 0;
    current_tick_keys_actions_count = 0;
    full_actions_count = 0;
    ticksActionsArray.clear();
    // ===

    start_time = QDateTime::currentMSecsSinceEpoch();
    measureTickTimer->start(MEASURE_TICK_LENGTH);
}

void APMMeter::onGameStopped()
{
    //qDebug() << "Game STOPPED. APM analyse stopped.";
    started = false;

    measureTickTimer->stop();
}

void APMMeter::calculateAPM()
{
    // ADD CURRENT TICK DATA
    quint64 joint_tick_actions_count = current_tick_keys_actions_count + current_tick_mouse_actions_count;
    ticksActionsArray.append(joint_tick_actions_count);
    full_actions_count += joint_tick_actions_count;

    if(ticksActionsArray.size() > TICKS_FOR_ANALYSE) ticksActionsArray.removeFirst(); // Сохраняем в массиве только последние TICKS_FOR_ANALYSE тиков

    // RESET CURRENT TICK ACTIONS
    current_tick_mouse_actions_count = 0;
    current_tick_keys_actions_count = 0;

    // CALCULATING APM
    auto analysed_ticks = 1;
    auto actions_sum = 0;

    QVector<quint64>::const_iterator i;
    for(i = ticksActionsArray.constEnd() - 1; i >= ticksActionsArray.constBegin(); --i){ // Если в массиве на текущий момент меньше чем TICKS_FOR_ANALYSE тиков, то анализ проведем на имеющимся их числе
//        qDebug() << QString("Tick %1 actions: %2").arg(analysed_ticks).arg(*i);
        actions_sum += *i;

        ++analysed_ticks;
//        if(analysed_ticks == TICKS_FOR_ANALYSE) break; // Мы взяли из массива последние TICKS_FOR_ANALYSE тиков по MEASURE_TICK_LENGTH времени
    }

    quint64 current_analyse_duration = analysed_ticks * MEASURE_TICK_LENGTH;
    float current_aps = actions_sum / ((float) current_analyse_duration / 1000);
    quint64 current_apm = current_aps * 60; // Аппроксимируем APS в APM
    emit currentApmCalculated(current_apm);

    quint64 full_analyse_duration = (QDateTime::currentMSecsSinceEpoch() - start_time);
    float average_aps = full_actions_count / ((float) full_analyse_duration / 1000);
    quint64 average_apm = average_aps * 60;
    emit averageApmCalculated(average_apm);
//    qDebug() << QString("TIME_MOMENT: %1   CurrentAPM: %2   AverageAPM: %3").arg(full_analyse_duration).arg(current_apm).arg(average_apm);
}

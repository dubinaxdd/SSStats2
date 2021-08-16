#include "apmmeter.h"

APMMeter::APMMeter(QObject *parent) : QObject(parent)
{
 measureTickTimer = new QTimer(this);
 QObject::connect(measureTickTimer, &QTimer::timeout, this, &APMMeter::analyseAPM);
}

void APMMeter::onKeyPressEvent(QKeyEvent *event)
{

}

void APMMeter::onMousePressEvent(QPoint mousePosition)
{

}

void APMMeter::start()
{
    full_actions_count = 0;
    start_time = QDateTime::currentMSecsSinceEpoch();
    measureTickTimer->start(MEASURE_TICK_LENGTH);
}

void APMMeter::stop()
{

}

void APMMeter::analyseAPM()
{
    auto analysed_ticks = 0;
    auto actions_sum = 0;

    QVector<quint64>::const_iterator i;
    for(i = ticksActionsArray.constEnd() - 1; i >= ticksActionsArray.constBegin(); --i){ // Если в массиве на текущий момент меньше чем TICKS_FOR_ANALYSE тиков, то анализ проведем на имеющимся их числе

        actions_sum += *i;

        ++analysed_ticks;
        if(analysed_ticks == TICKS_FOR_ANALYSE) break; // Мы взяли из массива последние TICKS_FOR_ANALYSE тиков по MEASURE_TICK_LENGTH времени
    }


    quint64 current_analyse_duration_sec = analysed_ticks * (float) MEASURE_TICK_LENGTH / 1000;
    quint64 current_apm = actions_sum / current_analyse_duration_sec * 60; // Сумму действий за анализируемый период делим на период (в секундах) и умножаем на 60 для получения APM
    emit currentApmAnalysed(current_apm);

    quint64 full_analyse_duration_sec = (QDateTime::currentMSecsSinceEpoch() - start_time) / 1000;
    quint64 average_apm = full_actions_count / full_analyse_duration_sec * 60;
    emit averageApmAnalysed(average_apm);
}

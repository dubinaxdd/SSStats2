#include <apmmeter.h>
#include <QDebug>

#define TICKS_FOR_ANALYSE 20
#define MEASURE_TICK_INTERVAL 500

APMMeter::APMMeter(QObject *parent) : QObject(parent)
{
    m_measureTickTimer = new QTimer(this);
    QObject::connect(m_measureTickTimer, &QTimer::timeout, this, &APMMeter::calculateAPM, Qt::QueuedConnection);
}

void APMMeter::onKeyPressEvent()
{
    if (m_isStarted)
        ++m_currentTickKeysActionsCount;
}

void APMMeter::onMousePressEvent()
{
    if (m_isStarted)
        ++m_currentTickMouseActionsCount;
}

void APMMeter::onGameStarted(SsGameState gameCurrentState)
{
    if(!(gameCurrentState == SsGameState::gameStarted || gameCurrentState == SsGameState::savedGameStarted))
        return;

    qInfo(logInfo()) << "APM analyse started";
    m_isStarted = true;

    // RESET ALL APM DATA
    m_currentTickMouseActionsCount = 0;
    m_currentTickKeysActionsCount = 0;
    m_fullActionsCount = 0;

    m_ticksActionsArray.clear();

    for(int i = 0; i < TICKS_FOR_ANALYSE; i++)
        m_ticksActionsArray.append(0);

    m_startTime = QDateTime::currentDateTime();//::currentMSecsSinceEpoch();
    m_measureTickTimer->start(MEASURE_TICK_INTERVAL);
}

void APMMeter::onGameStopped()
{
    qInfo(logInfo()) << "APM analyse stopped";
    m_isStarted = false;
    m_measureTickTimer->stop();
    m_ticksActionsArray.clear();
    emit sendAverrageApm(m_lastAverrageApm);
}

void APMMeter::calculateAPM()
{
    int jointTickActionsCount = m_currentTickMouseActionsCount + m_currentTickKeysActionsCount;

    m_currentTickMouseActionsCount = 0;
    m_currentTickKeysActionsCount = 0;


    m_fullActionsCount += jointTickActionsCount;
    m_ticksActionsArray.append(jointTickActionsCount);

    if (m_ticksActionsArray.size() > TICKS_FOR_ANALYSE)
        m_ticksActionsArray.removeFirst();

    double actionsSum = 0;

    for(int i = 0; i < m_ticksActionsArray.size(); i++)
        actionsSum += m_ticksActionsArray.at(i);

    double currentAnalyseDuration = m_ticksActionsArray.size() * MEASURE_TICK_INTERVAL;

    double currentApm = actionsSum  / currentAnalyseDuration * 1000.0 * 60.0;

    double fullAnalyseDuration = m_startTime.msecsTo(QDateTime::currentDateTime());

    if (fullAnalyseDuration == 0)
        return;

    double averageApm = m_fullActionsCount / fullAnalyseDuration * 1000.0  * 60.0;

    m_lastAverrageApm = averageApm;
    emit apmCalculated(static_cast<int>(currentApm), static_cast<int>(averageApm));
}

#ifndef APMMETER_H
#define APMMETER_H

#include <QObject>
#include <QKeyEvent>
#include <QPoint>
#include <QTimer>
#include <QDateTime>
#include <QVector>
#include "../../baseTypes/baseTypes.h"
#include "../../core/logger/logger.h"

class APMMeter : public QObject
{
    Q_OBJECT
public:
    explicit APMMeter(QObject *parent = nullptr);

public slots:
    void onKeyPressEvent();
    void onMousePressEvent();

    void onGameStarted(SsGameState gameCurrentState);
    void onGameStopped();

signals:
    void apmCalculated(quint16 capm, quint16 aapm);

    void sendAverrageApm(int apm);

private slots:
    void calculateAPM();

private:
    bool m_isStarted = false;

    QDateTime m_startTime;
    quint8	m_currentTickMouseActionsCount = 0;
    quint8	m_currentTickKeysActionsCount = 0;
    quint64	m_fullActionsCount = 0;
    quint64	m_tickActionsCount = 0;
    int m_lastAverrageApm = 0;
    QVector<quint8> m_ticksActionsArray;
    QTimer *m_measureTickTimer;
};

#endif // APMMETER_H

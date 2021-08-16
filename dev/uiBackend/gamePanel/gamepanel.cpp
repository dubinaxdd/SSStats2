#include "gamepanel.h"
#include <QDebug>

GamePanel::GamePanel(QObject *parent) : QObject(parent)
{
}

void GamePanel::onCurrentApmChanged(quint64 val)
{
    m_currentApm = QString::number(val);
    emit currentApmUpdate();
}

void GamePanel::onAverageApmChanged(quint64 val)
{
    m_averageApm = QString::number(val);
    emit averageApmUpdate();
}

void GamePanel::onGameStopped()
{
    m_currentApm = QString("-");
    m_averageApm = QString("-");
    emit currentApmUpdate();
    emit averageApmUpdate();
}

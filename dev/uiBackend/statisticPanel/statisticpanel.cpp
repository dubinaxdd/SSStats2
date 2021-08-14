#include "statisticpanel.h"
#include <QDebug>

StatisticPanel::StatisticPanel(QObject *parent) : QObject(parent)
{

}

void StatisticPanel::receiveServerPlayerStats(ServerPlayrStats serverPlayrStats)
{
    if (serverPlayrStats.isCurrentPlayer)
    {
        m_currentPlayerApm = QString::number(serverPlayrStats.apm);
        m_currentPlayerGamesCount = QString::number(serverPlayrStats.gamesCount);
        m_currentPlayerMmr = QString::number(serverPlayrStats.mmr);
        m_currentPlayerMmr1v1 = QString::number(serverPlayrStats.mmr1v1);
        m_currentPlayerName = serverPlayrStats.name;
        m_currentPlayerRace = QString::number(serverPlayrStats.race);
        m_currentPlayerWinRate = QString::number(serverPlayrStats.winRate);
        m_currentPlayerWinsCount = QString::number(serverPlayrStats.winsCount);

        emit currentPlayerStatsChanged();
    }
}

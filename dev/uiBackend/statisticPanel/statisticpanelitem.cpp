#include "statisticpanelitem.h"

StatisticPanelItem::StatisticPanelItem(QObject *parent) : QObject(parent)
{

}

void StatisticPanelItem::setPlayersStats(ServerPlayerStats newPlayersStats)
{
    m_playersStats = newPlayersStats;
    emit playersStatsChanged();
}

QString StatisticPanelItem::getRaceFromNumber(int raceNumber)
{

    switch(raceNumber)
    {
        case 1 : return "Space Marines";
        case 2 : return "Chaos";
        case 3 : return "Orks";
        case 4 : return "Eldar";
        case 5 : return "Imperial Guard";
        case 6 : return "Necrons";
        case 7 : return "Tau Empire";
        case 8 : return "Sisters of Battle";
        case 9 : return "Dark Eldar";
    }

    return "";
}

ServerPlayerStats StatisticPanelItem::playersStats()
{
    return m_playersStats;
}

void StatisticPanelItem::setVisible(bool newVisible)
{
    m_visible = newVisible;
    emit itemVisibleChanged();
}

bool StatisticPanelItem::getVisible()
{
    return m_visible;
}

void StatisticPanelItem::setPlayerSteamId(QString steamId)
{
    m_playersStats.steamId = steamId;
}

QString StatisticPanelItem::getPlayerName()
{
    return m_playersStats.name;
}

QString StatisticPanelItem::getPlayerMmr()
{
    return QString::number(m_playersStats.mmr);
}

QString StatisticPanelItem::getPlayerMmr1v1()
{
    return QString::number(m_playersStats.mmr1v1);
}

QString StatisticPanelItem::getPlayerGamesCount()
{
    return QString::number(m_playersStats.gamesCount);
}

QString StatisticPanelItem::getPlayerRace()
{
    return getRaceFromNumber(m_playersStats.race);
}

QString StatisticPanelItem::getPlayerWinRate()
{
    return QString::number(m_playersStats.winRate);
}

QString StatisticPanelItem::getPlayerApm()
{
    return QString::number(m_playersStats.apm);
}

bool StatisticPanelItem::getPlayerIsBanned()
{
    return m_playersStats.isBanned;
}

#include "statisticpanel.h"
#include <QDebug>

StatisticPanel::StatisticPanel(ImageProvider *imageProvider, QObject *parent)
    : QObject(parent)
    , m_imageProvider(imageProvider)
{

}

void StatisticPanel::receiveServerPlayerStats(ServerPlayerStats serverPlayerStats)
{

    //qDebug() << serverPlayerStats.name << serverPlayerStats.mmr << serverPlayerStats.apm;


    if (serverPlayerStats.isCurrentPlayer)
    {
        m_currentPlayerApm = QString::number(serverPlayerStats.apm);
        m_currentPlayerGamesCount = QString::number(serverPlayerStats.gamesCount);
        m_currentPlayerMmr = QString::number(serverPlayerStats.mmr);
        m_currentPlayerMmr1v1 = QString::number(serverPlayerStats.mmr1v1);
        m_currentPlayerName = serverPlayerStats.name;

        switch(serverPlayerStats.race)
        {
            case 1 : m_currentPlayerRace = "Space Marines"; break;
            case 2 : m_currentPlayerRace = "Chaos"; break;
            case 3 : m_currentPlayerRace = "Orks"; break;
            case 4 : m_currentPlayerRace = "Eldar"; break;
            case 5 : m_currentPlayerRace = "Imperial Guard"; break;
            case 6 : m_currentPlayerRace = "Necrons"; break;
            case 7 : m_currentPlayerRace = "Tau Empire"; break;
            case 8 : m_currentPlayerRace = "Sisters of Battle"; break;
            case 9 : m_currentPlayerRace = "Dark Eldar"; break;
        }

        m_currentPlayerWinRate = QString::number(serverPlayerStats.winRate);
        m_currentPlayerWinsCount = QString::number(serverPlayerStats.winsCount);

        m_imageProvider->setCurrentPlayerAvatarMedium(serverPlayerStats.avatar);

        emit currentPlayerStatsChanged();
    }
}

void StatisticPanel::receivePlayersCount(int playersCount)
{
    if (m_playersCount == playersCount)
        return;
    m_playersCount = playersCount;

    if(m_playersCount > 1)
        m_player2StatsVisible = true;
    else
        m_player2StatsVisible = false;

    if(m_playersCount > 2)
        m_player3StatsVisible = true;
    else
        m_player3StatsVisible = false;

    if(m_playersCount > 3)
        m_player4StatsVisible = true;
    else
        m_player4StatsVisible = false;

    if(m_playersCount > 4)
        m_player5StatsVisible = true;
    else
        m_player5StatsVisible = false;

    if(m_playersCount > 5)
        m_player6StatsVisible = true;
    else
        m_player6StatsVisible = false;

    if(m_playersCount > 6)
        m_player7StatsVisible = true;
    else
        m_player7StatsVisible = false;

    if(m_playersCount > 7)
        m_player8StatsVisible = true;
    else
        m_player8StatsVisible = false;

    emit playersStatsChanged();

}

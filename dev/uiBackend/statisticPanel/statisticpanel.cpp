#include "statisticpanel.h"
#include <QDebug>

StatisticPanel::StatisticPanel(ImageProvider *imageProvider, QObject *parent)
    : QObject(parent)
    , m_imageProvider(imageProvider)
{

}

void StatisticPanel::receiveServerPlayerStats(ServerPlayerStats serverPlayerStats)
{
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

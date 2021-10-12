#include "statisticpanel.h"
#include <QDebug>

StatisticPanel::StatisticPanel(ImageProvider *imageProvider, QObject *parent)
    : QObject(parent)
    , m_imageProvider(imageProvider)
{
    m_curentPlayerStatsItem = new StatisticPanelItem(this);

    for(int i = 0; i < 7; i++)
        m_playersStatsItems[i] = new StatisticPanelItem(this);

    emit playersItemsInitialized();
}

void StatisticPanel::receiveServerPlayerStats(ServerPlayerStats serverPlayerStats)
{
    if (m_blockUpdate)
        return;

    if (serverPlayerStats.isCurrentPlayer)
    {

        m_curentPlayerStatsItem->setPlayersStats(serverPlayerStats);
        m_imageProvider->setCurrentPlayerAvatarMedium(serverPlayerStats.avatar);
        m_curentPlayerStatsItem->setVisible(true);

        emit currentPlayerStatsChanged();
    }
    else
    {
        for (int i = 0; i < 7; i++)
        {
            if(serverPlayerStats.steamId == m_playersStatsItems[i]->playersStats().steamId)
            {
                m_playersStatsItems[i]->setPlayersStats(serverPlayerStats);

                switch (i)
                {
                    case 0: m_imageProvider->setPlayer2AvatarMedium(serverPlayerStats.avatar); break;
                    case 1: m_imageProvider->setPlayer3AvatarMedium(serverPlayerStats.avatar); break;
                    case 2: m_imageProvider->setPlayer4AvatarMedium(serverPlayerStats.avatar); break;
                    case 3: m_imageProvider->setPlayer5AvatarMedium(serverPlayerStats.avatar); break;
                    case 4: m_imageProvider->setPlayer6AvatarMedium(serverPlayerStats.avatar); break;
                    case 5: m_imageProvider->setPlayer7AvatarMedium(serverPlayerStats.avatar); break;
                    case 6: m_imageProvider->setPlayer8AvatarMedium(serverPlayerStats.avatar); break;
                }
            }
        }
        emit playersStatsChanged();
    }
}

void StatisticPanel::receivePlayersCount(int playersCount)
{
    if (m_blockUpdate)
        return;

    if (currentPlayerIsHost)
    {
        if(playersCount >= m_playersCount)
            m_playersCount = playersCount;
        else
            return;
    }
    else
    {

        if (m_playersCount == playersCount)
            return;
        m_playersCount = playersCount;
    }



    if(m_playersCount > 1)
        m_playersStatsItems[0]->setVisible(true);
    else
        m_playersStatsItems[0]->setVisible(false);

    if(m_playersCount > 2)
        m_playersStatsItems[1]->setVisible(true);
    else
        m_playersStatsItems[1]->setVisible(false);


    if(m_playersCount > 3)
        m_playersStatsItems[2]->setVisible(true);
    else
        m_playersStatsItems[2]->setVisible(false);


    if(m_playersCount > 4)
        m_playersStatsItems[3]->setVisible(true);
    else
        m_playersStatsItems[3]->setVisible(false);


    if(m_playersCount > 5)
        m_playersStatsItems[4]->setVisible(true);
    else
        m_playersStatsItems[4]->setVisible(false);


    if(m_playersCount > 6)
        m_playersStatsItems[5]->setVisible(true);
    else
        m_playersStatsItems[5]->setVisible(false);


    if(m_playersCount > 7)
        m_playersStatsItems[6]->setVisible(true);
    else
        m_playersStatsItems[6]->setVisible(false);

    emit playersStatsChanged();
}

void StatisticPanel::receivePlayersInfoMapFromScanner(QList<SearchStemIdPlayerInfo> playersInfo, int playersCount)
{
    if (m_blockUpdate)
        return;

    int needPlayersCount = playersCount;

    for (int i = 0; i < playersInfo.count(); i++)
    {
        if(playersInfo.at(i).steamId == m_curentPlayerStatsItem->playersStats().steamId)
        {
            needPlayersCount--;
            playersInfo.removeAt(i);
            break;
        }
    }

    for (int i = 0; i < playersInfo.count() - 1; i++)
    {
        for (int j = 1; j < playersInfo.count(); j++)
        {
            if(playersInfo.at(j-1).position > playersInfo.at(j).position)
            {
                SearchStemIdPlayerInfo buffer = playersInfo.at(j-1);
                playersInfo[j-1] = playersInfo.at(j);
                playersInfo[j] = buffer;
            }
        }
    }

    if(needPlayersCount > 7 )
        needPlayersCount = 7;

    if(playersInfo.count() < needPlayersCount)
        needPlayersCount = playersInfo.count();

    for (int i = 0; i < needPlayersCount; i++)
    {
            m_playersStatsItems[i]->setPlayerSteamId(playersInfo.at(i).steamId);
    }
}

void StatisticPanel::receiveCurrentPlayerHostState(bool isHost)
{
    if(currentPlayerIsHost != isHost)
        m_playersCount = 1;


    currentPlayerIsHost = isHost;

    //qDebug() << "Player is host:" << currentPlayerIsHost;
}

StatisticPanelItem *StatisticPanel::getCurentPlayerStatsItem()
{
    return m_curentPlayerStatsItem;
}

StatisticPanelItem *StatisticPanel::getPlayer2StatsItem()
{
    return m_playersStatsItems[0];
}

StatisticPanelItem *StatisticPanel::getPlayer3StatsItem()
{
    return m_playersStatsItems[1];
}

StatisticPanelItem *StatisticPanel::getPlayer4StatsItem()
{
    return m_playersStatsItems[2];
}

StatisticPanelItem *StatisticPanel::getPlayer5StatsItem()
{
    return m_playersStatsItems[3];
}

StatisticPanelItem *StatisticPanel::getPlayer6StatsItem()
{
    return m_playersStatsItems[4];
}

StatisticPanelItem *StatisticPanel::getPlayer7StatsItem()
{
    return m_playersStatsItems[5];
}

StatisticPanelItem *StatisticPanel::getPlayer8StatsItem()
{
    return m_playersStatsItems[6];
}

void StatisticPanel::setBlockUpdate(bool newBlockUpdate)
{
    m_blockUpdate = newBlockUpdate;
}

void StatisticPanel::setExpandPatyStatistic(bool newExpandPatyStatistic)
{
    m_expandPatyStatistic = newExpandPatyStatistic;
    emit expandPatyStatisticChanged();
}










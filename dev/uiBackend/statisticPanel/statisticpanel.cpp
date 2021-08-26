#include "statisticpanel.h"
#include <QDebug>

StatisticPanel::StatisticPanel(ImageProvider *imageProvider, QObject *parent)
    : QObject(parent)
    , m_imageProvider(imageProvider)
{

    m_playersStats.resize(8);
}

void StatisticPanel::receiveServerPlayerStats(ServerPlayerStats serverPlayerStats)
{

    //qDebug() << serverPlayerStats.name << serverPlayerStats.mmr << serverPlayerStats.apm;


    if (serverPlayerStats.isCurrentPlayer)
    {
        m_currentPlayerSteamId = serverPlayerStats.steamId;
        m_currentPlayerApm = QString::number(serverPlayerStats.apm);
        m_currentPlayerGamesCount = QString::number(serverPlayerStats.gamesCount);
        m_currentPlayerMmr = QString::number(serverPlayerStats.mmr);
        m_currentPlayerMmr1v1 = QString::number(serverPlayerStats.mmr1v1);
        m_currentPlayerName = serverPlayerStats.name;

        m_currentPlayerRace = getRaceFromNumber(serverPlayerStats.race);

        m_currentPlayerWinRate = QString::number(serverPlayerStats.winRate);
        m_currentPlayerWinsCount = QString::number(serverPlayerStats.winsCount);

        m_imageProvider->setCurrentPlayerAvatarMedium(serverPlayerStats.avatar);

        emit currentPlayerStatsChanged();
    }
    else
    {
        for (int i = 0; i < m_playersStats.count(); i++)
        {
            if(serverPlayerStats.steamId == m_playersStats.at(i).steamId)
            {
                m_playersStats[i] =  serverPlayerStats;

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
    //qDebug() << "asd" << playersCount;

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

void StatisticPanel::receivePlayersInfoMapFromScanner(QList<SearchStemIdPlayerInfo> playersInfo)
{
    for (int i = 0; i < playersInfo.count(); i++)
    {
        if(playersInfo.at(i).steamId == m_currentPlayerSteamId)
        {
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

    for (int i = 0; i < playersInfo.count(); i++)
    {
        m_playersStats[i].steamId = playersInfo.at(i).steamId;
    }
}

QString StatisticPanel::getRaceFromNumber(int raceNumber)
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

QString StatisticPanel::getPlayer2Name()
{
    return m_playersStats.at(0).name;
}

QString StatisticPanel::getPlayer3Name()
{
    return m_playersStats.at(1).name;
}

QString StatisticPanel::getPlayer4Name()
{
    return m_playersStats.at(2).name;
}

QString StatisticPanel::getPlayer5Name()
{
    return m_playersStats.at(3).name;
}

QString StatisticPanel::getPlayer6Name()
{
    return m_playersStats.at(4).name;
}

QString StatisticPanel::getPlayer7Name()
{
    return m_playersStats.at(5).name;
}

QString StatisticPanel::getPlayer8Name()
{
    return m_playersStats.at(6).name;
}

QString StatisticPanel::getPlayer2Mmr()
{
    return QString::number(m_playersStats.at(0).mmr);
}

QString StatisticPanel::getPlayer3Mmr()
{
    return QString::number(m_playersStats.at(1).mmr);
}

QString StatisticPanel::getPlayer4Mmr()
{
    return QString::number(m_playersStats.at(2).mmr);
}

QString StatisticPanel::getPlayer5Mmr()
{
    return QString::number(m_playersStats.at(3).mmr);
}

QString StatisticPanel::getPlayer6Mmr()
{
    return QString::number(m_playersStats.at(4).mmr);
}

QString StatisticPanel::getPlayer7Mmr()
{
    return QString::number(m_playersStats.at(5).mmr);
}

QString StatisticPanel::getPlayer8Mmr()
{
    return QString::number(m_playersStats.at(6).mmr);
}

QString StatisticPanel::getPlayer2Mmr1v1()
{
    return QString::number(m_playersStats.at(0).mmr1v1);
}

QString StatisticPanel::getPlayer3Mmr1v1()
{
    return QString::number(m_playersStats.at(1).mmr1v1);
}

QString StatisticPanel::getPlayer4Mmr1v1()
{
    return QString::number(m_playersStats.at(2).mmr1v1);
}

QString StatisticPanel::getPlayer5Mmr1v1()
{
    return QString::number(m_playersStats.at(3).mmr1v1);
}

QString StatisticPanel::getPlayer6Mmr1v1()
{
    return QString::number(m_playersStats.at(4).mmr1v1);
}

QString StatisticPanel::getPlayer7Mmr1v1()
{
    return QString::number(m_playersStats.at(5).mmr1v1);
}

QString StatisticPanel::getPlayer8Mmr1v1()
{
    return QString::number(m_playersStats.at(6).mmr1v1);
}


QString StatisticPanel::getPlayer2GamesCount()
{
    return QString::number(m_playersStats.at(0).gamesCount);
}

QString StatisticPanel::getPlayer3GamesCount()
{
    return QString::number(m_playersStats.at(1).gamesCount);
}

QString StatisticPanel::getPlayer4GamesCount()
{
    return QString::number(m_playersStats.at(2).gamesCount);
}

QString StatisticPanel::getPlayer5GamesCount()
{
    return QString::number(m_playersStats.at(3).gamesCount);
}

QString StatisticPanel::getPlayer6GamesCount()
{
    return QString::number(m_playersStats.at(4).gamesCount);
}

QString StatisticPanel::getPlayer7GamesCount()
{
    return QString::number(m_playersStats.at(5).gamesCount);
}

QString StatisticPanel::getPlayer8GamesCount()
{
    return QString::number(m_playersStats.at(6).gamesCount);
}

QString StatisticPanel::getPlayer2Race()
{
    return getRaceFromNumber(m_playersStats.at(0).race);
}

QString StatisticPanel::getPlayer3Race()
{
    return getRaceFromNumber(m_playersStats.at(1).race);
}

QString StatisticPanel::getPlayer4Race()
{
    return getRaceFromNumber(m_playersStats.at(2).race);
}

QString StatisticPanel::getPlayer5Race()
{
    return getRaceFromNumber(m_playersStats.at(3).race);
}

QString StatisticPanel::getPlayer6Race()
{
    return getRaceFromNumber(m_playersStats.at(4).race);
}

QString StatisticPanel::getPlayer7Race()
{
    return getRaceFromNumber(m_playersStats.at(5).race);
}

QString StatisticPanel::getPlayer8Race()
{
    return getRaceFromNumber(m_playersStats.at(6).race);
}

QString StatisticPanel::getPlayer2WinRate()
{
    return QString::number(m_playersStats.at(0).winRate);
}

QString StatisticPanel::getPlayer3WinRate()
{
    return QString::number(m_playersStats.at(1).winRate);
}

QString StatisticPanel::getPlayer4WinRate()
{
    return QString::number(m_playersStats.at(2).winRate);
}

QString StatisticPanel::getPlayer5WinRate()
{
    return QString::number(m_playersStats.at(3).winRate);
}

QString StatisticPanel::getPlayer6WinRate()
{
    return QString::number(m_playersStats.at(4).winRate);
}

QString StatisticPanel::getPlayer7WinRate()
{
    return QString::number(m_playersStats.at(5).winRate);
}

QString StatisticPanel::getPlayer8WinRate()
{
    return QString::number(m_playersStats.at(6).winRate);
}

QString StatisticPanel::getPlayer2Apm()
{
    return QString::number(m_playersStats.at(0).apm);
}

QString StatisticPanel::getPlayer3Apm()
{
    return QString::number(m_playersStats.at(1).apm);
}

QString StatisticPanel::getPlayer4Apm()
{
    return QString::number(m_playersStats.at(2).apm);
}

QString StatisticPanel::getPlayer5Apm()
{
    return QString::number(m_playersStats.at(3).apm);
}

QString StatisticPanel::getPlayer6Apm()
{
    return QString::number(m_playersStats.at(4).apm);
}

QString StatisticPanel::getPlayer7Apm()
{
    return QString::number(m_playersStats.at(5).apm);
}

QString StatisticPanel::getPlayer8Apm()
{
    return QString::number(m_playersStats.at(6).apm);
}











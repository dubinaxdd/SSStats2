#include <statisticPanelItem.h>
StatisticPanelItem::StatisticPanelItem(QObject *parent) : QObject(parent)
{

}

void StatisticPanelItem::setPlayersStats(ServerPlayerStats newPlayersStats)
{
    m_playersStats = newPlayersStats;
    emit playersStatsChanged();
}

QString StatisticPanelItem::getRaceFromNumber(int raceNumber) const
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

const QString &StatisticPanelItem::getTempSid() const
{
    return tempSid;
}

const QString &StatisticPanelItem::avatarId() const
{
    return m_avatarId;
}

void StatisticPanelItem::setAvatarId(const QString &newAvatarId)
{
    m_avatarId = newAvatarId;
    emit playersStatsChanged();
}

bool StatisticPanelItem::getIsRanked() const
{
    return isRanked;
}

void StatisticPanelItem::setIsRanked(bool newIsRanked)
{
    if (isRanked == newIsRanked)
        return;
    isRanked = newIsRanked;
    emit isRankedChanged();
}

bool StatisticPanelItem::getIsOnline() const
{
    return isOnline;
}

void StatisticPanelItem::setIsOnline(bool newIsOnline)
{
    if (isOnline == newIsOnline)
        return;
    isOnline = newIsOnline;
    emit isOnlineChanged();
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

bool StatisticPanelItem::getVisible() const
{
    return m_visible;
}

int StatisticPanelItem::getPosition() const
{
    return m_playersStats.position;
}

QImage StatisticPanelItem::getAvatar() const
{
    return m_playersStats.avatar;
}

QString StatisticPanelItem::getSteamId() const
{
    return m_playersStats.steamId;
}

int StatisticPanelItem::getCalibrateGamesLeft() const
{
    return m_playersStats.calibrateGamesLeft;
}

BanType::BanTypeEnum StatisticPanelItem::banType() const
{
    return m_playersStats.banType;
}

void StatisticPanelItem::setPlayerSteamId(QString steamId)
{
    tempSid = steamId;
    m_playersStats.steamId = steamId;
}

QString StatisticPanelItem::getPlayerName() const
{
    return m_playersStats.name;
}

QString StatisticPanelItem::getPlayerMmr() const
{
    return QString::number(m_playersStats.mmr);
}

QString StatisticPanelItem::getPlayerMmr1v1() const
{
    return QString::number(m_playersStats.mmr1v1);
}

QString StatisticPanelItem::getPlayerGamesCount() const
{
    return QString::number(m_playersStats.gamesCount);
}

int StatisticPanelItem::getPlayerRank() const
{
    return m_playersStats.rank;
}

QString StatisticPanelItem::getPlayerRace() const
{
    return getRaceFromNumber(m_playersStats.race);
}

QString StatisticPanelItem::getPlayerWinRate() const
{
    return QString::number(m_playersStats.winRate);
}

QString StatisticPanelItem::getPlayerApm() const
{
    return QString::number(m_playersStats.apm);
}

bool StatisticPanelItem::getPlayerIsBanned() const
{
    return m_playersStats.isBanned;
}

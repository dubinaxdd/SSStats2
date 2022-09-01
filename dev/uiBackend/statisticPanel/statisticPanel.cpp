#include <statisticpanel.h>
#include <QDebug>

StatisticPanel::StatisticPanel(ImageProvider *imageProvider, QObject *parent)
    : QAbstractListModel(parent)
    , m_imageProvider(imageProvider)
{
    m_curentPlayerStatsItem = new StatisticPanelItem(this);
    emit playersItemsInitialized();
}

QVariant StatisticPanel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_playersStatsItems.count())
        return QVariant();

    const StatisticPanelItem* item = m_playersStatsItems[index.row()];

    switch (role)
    {
        case PlayerName: return item->getPlayerName();
        case PlayerMmr: return item->getPlayerMmr();
        case PlayerMmr1v1: return item->getPlayerMmr1v1();
        case PlayerGamesCount: return item->getPlayerGamesCount();
        case PlayerRace: return item->getPlayerRace();
        case PlayerWinRate: return item->getPlayerWinRate();
        case PlayerApm: return item->getPlayerApm();
        case PlayerIsBanned: return item->getPlayerIsBanned();
        case PlayerVisible: return item->getVisible();
        case PlayerAvatarId: return item->avatarId();
        case SteamId: return item->getSteamId();
        case CalibrateGamesLeft: return item->getCalibrateGamesLeft();
    }

    return QVariant();
}

int StatisticPanel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_playersStatsItems.count();
}

QHash<int, QByteArray> StatisticPanel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[PlayerName]       = "playerName";
    roles[PlayerMmr]        = "playerMmr";
    roles[PlayerMmr1v1]     = "playerMmr1v1";
    roles[PlayerGamesCount] = "playerGamesCount";
    roles[PlayerRace]       = "playerRace";
    roles[PlayerWinRate]    = "playerWinRate";
    roles[PlayerApm]        = "playerApm";
    roles[PlayerIsBanned]   = "playerIsBanned";
    roles[PlayerVisible]    = "playerVisible";
    roles[PlayerAvatarId]   = "avatarId";
    roles[SteamId]          = "steamId";
    roles[CalibrateGamesLeft] = "calibrateGamesLeft";

    return roles;
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
        for(int i = 0; i < m_playersStatsItems.count(); i++)
        {
            if (serverPlayerStats.steamId == m_playersStatsItems.at(i)->getTempSid())
            {
                 m_playersStatsItems[i]->setPlayersStats(serverPlayerStats);
                 m_imageProvider->addPlayerAvatar("playerAvatar" + m_playersStatsItems.at(i)->getTempSid(), m_playersStatsItems.at(i)->getAvatar());
                 m_playersStatsItems[i]->setAvatarId( "playerAvatar" + m_playersStatsItems.at(i)->getTempSid());
                 break;
            }
        }

        sortStatsBySoloMmr();

        QModelIndex first = QAbstractItemModel::createIndex(0, 0);
        QModelIndex last = QAbstractItemModel::createIndex(m_playersStatsItems.count() -1, 0);
        emit dataChanged(first, last);

    }
}

void StatisticPanel::receivePlayersInfoMapFromScanner(QList<SearchStemIdPlayerInfo> playersInfo, int playersCount)
{
    Q_UNUSED(playersCount);

    m_playersInfo = playersInfo;

    beginRemoveRows(QModelIndex(), 0, m_playersStatsItems.count() - 1);
    for(int i = 0; i < m_playersStatsItems.count(); i++)
        delete m_playersStatsItems.at(i);

    m_playersStatsItems.clear();

    endRemoveRows();

    beginInsertRows(QModelIndex(), 0, m_playersInfo.count()-2);

    for(int i = 0; i < m_playersInfo.count(); i++)
    {
        if(m_playersInfo[i].isCurrentPlayer)
            continue;

        StatisticPanelItem* newItem = new StatisticPanelItem(this);
        newItem->setPlayerSteamId(m_playersInfo.at(i).steamId);
        m_playersStatsItems.append(newItem);
    }

    endInsertRows();
}

void StatisticPanel::receiveCurrentPlayerHostState(bool isHost)
{
    if(currentPlayerIsHost != isHost)
        m_playersCount = 1;

    currentPlayerIsHost = isHost;
}

void StatisticPanel::onQuitParty()
{
    beginRemoveRows(QModelIndex(), 0, m_playersStatsItems.count() - 1);

    for(int i = 0; i < m_playersStatsItems.count(); i++)
        delete m_playersStatsItems.at(i);

    m_playersStatsItems.clear();

    endRemoveRows();
}

StatisticPanelItem *StatisticPanel::getCurentPlayerStatsItem()
{
    return m_curentPlayerStatsItem;
}

void StatisticPanel::sortStatsBySoloMmr()
{
    for (int i = 0; i < m_playersStatsItems.count(); i++)
    {
        for (int j = 1; j < m_playersStatsItems.count(); j++)
        {
            if (m_playersStatsItems.at(j - 1)->getPlayerMmr1v1() <  m_playersStatsItems.at(j)->getPlayerMmr1v1())
            {
                StatisticPanelItem* buffer = m_playersStatsItems.at(j - 1);
                m_playersStatsItems[j-1] = m_playersStatsItems.at(j);
                m_playersStatsItems[j] = buffer;
            }
        }
    }
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









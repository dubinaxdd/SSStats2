#include <statisticPanel.h>
#include <QDebug>
#include <logger.h>

StatisticPanel::StatisticPanel(Core *core, ImageProvider *imageProvider, QObject *parent)
    : QAbstractListModel(parent)
    , m_corePtr(core)
    , m_imageProvider(imageProvider)
{
    QObject::connect(m_corePtr->statsServerProcessor(),                      &StatsServerProcessor::sendServerPlayerStats,       this, &StatisticPanel::receiveServerPlayerStats,        Qt::QueuedConnection);
    QObject::connect(m_corePtr->gameController()->lobbyEventReader(),   &LobbyEventReader::quitFromParty,                  this, &StatisticPanel::onQuitParty,                     Qt::QueuedConnection);
    QObject::connect(m_corePtr->gameController()->dowServerProcessor(), &DowServerProcessor::sendPlayersInfoFromDowServer, this, &StatisticPanel::receivePlayresInfoFromDowServer, Qt::QueuedConnection);
    QObject::connect(m_corePtr->rankedModServiceProcessor(),                 &RankedModServiceProcessor::sendPlyersRankedState, this, &StatisticPanel::receivePlyersRankedState ,       Qt::QueuedConnection);

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
        case CustomGamesMmr: return item->getCustomGamesMmr();
        case PlayerGamesCount: return item->getPlayerGamesCount();
        case PlayerRank: return item->getPlayerRank();
        case PlayerRace: return item->getPlayerRace();
        case PlayerWinRate: return item->getPlayerWinRate();
        case PlayerApm: return item->getPlayerApm();
        case PlayerIsBanned: return item->getPlayerIsBanned();
        case PlayerVisible: return item->getVisible();
        case PlayerAvatarId: return item->avatarId();
        case SteamId: return item->getSteamId();
        case CalibrateGamesLeft: return item->getCalibrateGamesLeft();
        case BanType: return item->banType();
        case IsOnline: return item->getIsOnline();
        case IsRanked: return item->getIsRanked();

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
    roles[CustomGamesMmr]   = "customGamesMmr";
    roles[PlayerGamesCount] = "playerGamesCount";
    roles[PlayerRank]       = "playerRank";
    roles[PlayerRace]       = "playerRace";
    roles[PlayerWinRate]    = "playerWinRate";
    roles[PlayerApm]        = "playerApm";
    roles[PlayerIsBanned]   = "playerIsBanned";
    roles[PlayerVisible]    = "playerVisible";
    roles[PlayerAvatarId]   = "avatarId";
    roles[SteamId]          = "steamId";
    roles[CalibrateGamesLeft] = "calibrateGamesLeft";
    roles[BanType] = "banType";
    roles[IsOnline] = "isOnline";
    roles[IsRanked] = "isRanked";

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

        if (m_playersStatsItems.count() > 0)
        {
            QModelIndex first = QAbstractItemModel::createIndex(0, 0);
            QModelIndex last = QAbstractItemModel::createIndex(m_playersStatsItems.count() -1, 0);
            emit dataChanged(first, last);
        }
    }
}

void StatisticPanel::receivePlayresInfoFromDowServer(QList<PlayerInfoFromDowServer> playersInfo)
{
    m_playersInfo = playersInfo;

    if (m_playersStatsItems.count() > 0)
    {
        beginRemoveRows(QModelIndex(), 0, m_playersStatsItems.count() - 1);
        for(int i = 0; i < m_playersStatsItems.count(); i++)
            delete m_playersStatsItems.at(i);

        m_playersStatsItems.clear();

        endRemoveRows();
    }


    if(m_playersInfo.count() > 1)
    {
        beginInsertRows(QModelIndex(), 0, m_playersInfo.count()-2); //TODO: тут баг

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

    qInfo(logInfo()) << "StatisticPanel::receivePlayresInfoFromDowServer" << "players info received";
}

void StatisticPanel::receivePlyersRankedState(QVector<PlyersRankedState> plyersRankedState)
{
    for (int i = 0; i < m_playersStatsItems.count(); i++)
    {
        for (int j = 0; j < plyersRankedState.count(); j++)
        {
            if (plyersRankedState.at(j).steamId == m_playersStatsItems.at(i)->getSteamId())
            {
                m_playersStatsItems[i]->setIsOnline(plyersRankedState.at(j).isOnline);
                m_playersStatsItems[i]->setIsRanked(plyersRankedState.at(j).isRanked);
            }
        }
    }

    for (int j = 0; j < plyersRankedState.count(); j++)
    {
        if (plyersRankedState.at(j).steamId == m_curentPlayerStatsItem->getSteamId())
        {
            m_curentPlayerStatsItem->setIsOnline(plyersRankedState.at(j).isOnline);
            m_curentPlayerStatsItem->setIsRanked(plyersRankedState.at(j).isRanked);

            emit currentPlayerStatsChanged();
        }
    }

    if (m_playersStatsItems.count() < 1)
        return;

    QModelIndex first = QAbstractItemModel::createIndex(0, 0);
    QModelIndex last = QAbstractItemModel::createIndex(m_playersStatsItems.count() - 1, 0);

    emit dataChanged(first, last);

}

void StatisticPanel::onQuitParty()
{
    if (m_playersStatsItems.count() < 1)
        return;

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

void StatisticPanel::updateStatistic()
{
    m_corePtr->gameController()->dowServerProcessor()->requestPartysData();
}

void StatisticPanel::setExpandPatyStatistic(bool newExpandPatyStatistic)
{
    m_expandPatyStatistic = newExpandPatyStatistic;
    emit expandPatyStatisticChanged();
}










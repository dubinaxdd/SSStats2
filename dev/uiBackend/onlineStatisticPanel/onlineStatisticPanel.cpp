#include "onlineStatisticPanel.h"
#include "QDebug"

OnlineStatisticPanel::OnlineStatisticPanel(RankedModServiceProcessor* rankedModServiceProcessor,QObject *parent)
    : QAbstractListModel(parent)
    , m_rankedModServiceProcessorPtr(rankedModServiceProcessor)
{
    QObject::connect(m_rankedModServiceProcessorPtr, &RankedModServiceProcessor::sendUniquePlayersOnlineStatistic, this, &OnlineStatisticPanel::receiveUniquePlayersOnlineStatistic, Qt::QueuedConnection);
    QObject::connect(m_rankedModServiceProcessorPtr, &RankedModServiceProcessor::sendModsOnlineCountMap,           this, &OnlineStatisticPanel::receiveModsOnlineCountMap,           Qt::QueuedConnection);
}

QVariant OnlineStatisticPanel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_modsOnlineCountMap.count())
        return QVariant();

    if (role == ModName)
        return m_modsOnlineCountMap.keys().at(index.row());
    else if (role == OnlineCount)
        return m_modsOnlineCountMap.values().at(index.row());

    return QVariant();
}

int OnlineStatisticPanel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_modsOnlineCountMap.count();
}

int OnlineStatisticPanel::getUniquePlayersOnDay()
{
    return m_uniqueOnlineStatistic.day;
}

int OnlineStatisticPanel::getUniquePlayersOnMonth()
{
    return m_uniqueOnlineStatistic.month;
}

int OnlineStatisticPanel::getUniquePlayersOnYear()
{
    return m_uniqueOnlineStatistic.year;
}

int OnlineStatisticPanel::getUniquePlayersOnTotal()
{
    return m_uniqueOnlineStatistic.total;
}

QHash<int, QByteArray> OnlineStatisticPanel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ModName] = "modName";
    roles[OnlineCount] = "onlineCount";

    return roles;
}

void OnlineStatisticPanel::receiveModsOnlineCountMap(QMap<QString, int> modsOnlineCountMap)
{
    //TODO: Костыль который надо будет убрать как перепишется мик5росервис онлайна
    modsOnlineCountMap["Other mods"] += modsOnlineCountMap["TournamentPatch"] += modsOnlineCountMap["Tribun mod"];
    modsOnlineCountMap.remove("TournamentPatch");
    modsOnlineCountMap.remove("Tribun mod");

    if (m_modsOnlineCountMap.count() > 0)
    {
        beginRemoveRows(QModelIndex(), 0, m_modsOnlineCountMap.count() - 1);
        m_modsOnlineCountMap.clear();
        endRemoveRows();
    }

    if (modsOnlineCountMap.count() > 0)
    {
        beginInsertRows(QModelIndex(), 0, modsOnlineCountMap.count() - 1);
        m_modsOnlineCountMap = modsOnlineCountMap;
        endInsertRows();
    }
}

void OnlineStatisticPanel::receiveUniquePlayersOnlineStatistic(UniqueOnlineStatistic uniqueOnlineStatistic)
{
    m_uniqueOnlineStatistic = uniqueOnlineStatistic;
    emit uniquePlayersOnlineStatisticChanged();
}

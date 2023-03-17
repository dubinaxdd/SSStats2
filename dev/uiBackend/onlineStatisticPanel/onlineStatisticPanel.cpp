#include "onlineStatisticPanel.h"

OnlineStatisticPanel::OnlineStatisticPanel(QObject *parent)
    : QAbstractListModel(parent)
{

}

QVariant OnlineStatisticPanel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_modsOnlineCountMap.count())
        return QVariant();

    if (role == ModName)
        return m_modsOnlineCountMap.keys().at(index.row());
    else if (role == OnlineCount)
        return m_modsOnlineCountMap.values().at(index.row());
}

int OnlineStatisticPanel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_modsOnlineCountMap.count();
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
    beginRemoveRows(QModelIndex(), 0, m_modsOnlineCountMap.count() - 1);
    m_modsOnlineCountMap.clear();
    endRemoveRows();

    beginInsertRows(QModelIndex(), 0, modsOnlineCountMap.count() - 1);
    m_modsOnlineCountMap = modsOnlineCountMap;
    endInsertRows();
}

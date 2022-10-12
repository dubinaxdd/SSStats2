#include "playersListModel.h"

PlayersListModel::PlayersListModel(QObject *parent) : QAbstractListModel(parent)
{

}

QVariant PlayersListModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_players.count())
        return QVariant();

    const ReplayPlayer &player = m_players[index.row()];

    if (role == Name)
        return player.name;
    else if (role == Team)
        return player.team;
    else if (role == Type)
        return player.type;
    else if (role == Race)
        return player.race;
    else if (role == Color)
        return player.color;
    else if (role == BadgeUrl)
        return player.badgeUrl;
    else if (role == BannerUrl)
        return player.bannerUrl;

    return QVariant();
}

int PlayersListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_players.count();
}

void PlayersListModel::setPlayersList(QVector<ReplayPlayer> players)
{
    if (m_players.count() > 0)
    {
        beginRemoveRows(QModelIndex(), 0, m_players.count() - 1);
        m_players.clear();
        endRemoveRows();
    }

    if (players.count() > 0)
    {
        beginInsertRows(QModelIndex(), 0, players.count() - 1);
        m_players = players;
        endInsertRows();
    }
}

QHash<int, QByteArray> PlayersListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Name] = "name";
    roles[Team] = "team";
    roles[Type] = "type";
    roles[Race] = "race";
    roles[Color] = "color";
    roles[BadgeUrl] = "badgeUrl";
    roles[BannerUrl] = "bannerUrl";

    return roles;
}

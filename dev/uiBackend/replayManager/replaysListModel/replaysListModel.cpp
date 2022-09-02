#include "replaysListModel.h"

ReplaysListModel::ReplaysListModel(QObject *parent) : QAbstractListModel(parent)
{

}

QVariant ReplaysListModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= replaysList.count())
        return QVariant();

    const ReplayListInfo &replayInfo = replaysList[index.row()];
    if (role == FolderName)
        return replayInfo.folderName;
    else if (role == Name)
        return replayInfo.name;
    else if (role == Map)
        return replayInfo.map;

    return QVariant();
}

int ReplaysListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return replaysList.count();
}

QHash<int, QByteArray> ReplaysListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[FolderName] = "folderName";
    roles[Name] = "name";
    roles[Map] = "map";

    return roles;
}

void ReplaysListModel::setReplaysList(const QVector<ReplayListInfo> &newReplaysList)
{

    beginInsertRows(QModelIndex(), 0, newReplaysList.count() - 1);

    replaysList = newReplaysList;

    endInsertRows();
}

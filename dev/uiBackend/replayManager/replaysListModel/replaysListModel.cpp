#include "replaysListModel.h"
#include <QDebug>

ReplaysListModel::ReplaysListModel(QObject *parent) : QAbstractListModel(parent)
{

}

QVariant ReplaysListModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= replaysList.count())
        return QVariant();

    const ReplayListInfo &replayInfo = replaysList[index.row()];

    if (role == FileName)
        return replayInfo.fileName;
    else if (role == Name)
        return replayInfo.name;
    else if (role == Map)
        return replayInfo.map;
    else if (role == Mod)
        return replayInfo.mod;

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
    roles[FileName] = "fileName";
    roles[Name] = "name";
    roles[Map] = "map";
    roles[Mod] = "mod";

    return roles;
}

void ReplaysListModel::setReplaysList(const QVector<ReplayListInfo> &newReplaysList)
{

    beginInsertRows(QModelIndex(), 0, newReplaysList.count() - 1);

    replaysList = newReplaysList;

    endInsertRows();
}

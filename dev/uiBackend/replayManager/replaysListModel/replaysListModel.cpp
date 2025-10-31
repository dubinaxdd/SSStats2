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
        return replayInfo.mapUrl;
    else if (role == Mod)
    {
        if (replayInfo.mod == "dxp2")
            return tr("Soulstorm");

        if (replayInfo.mod == "DoWDE")
            return tr("Definitive Edition");

        if (m_modInfo)
        {
            for (int i = 0; i < m_modInfo->count(); i++)
            {
                if (m_modInfo->at(i).technicalName.toLower() == replayInfo.mod.toLower())
                    return m_modInfo->at(i).uiName;
            }
        }

        return replayInfo.mod;
    }
    else if (role == Selected)
        return replayInfo.selected;

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
    roles[Selected] = "selected";

    return roles;
}

void ReplaysListModel::replace(int index)
{
    ReplayListInfo buffer = replaysList.at(index);
    replaysList[index] = replaysList.at(index + 1);
    replaysList[index+1] = buffer;
}

void ReplaysListModel::setModInfo(QList<ModInfo> *newModInfo)
{
    m_modInfo = newModInfo;

    QModelIndex first = QAbstractItemModel::createIndex(0,0);
    QModelIndex last = QAbstractItemModel::createIndex(replaysList.count() - 1, 0);

    emit dataChanged(first, last);
}

void ReplaysListModel::setReplaysList(const QVector<ReplayListInfo> &newReplaysList)
{
    if(replaysList.count() > 0)
    {
        beginRemoveRows(QModelIndex(), 0, replaysList.count() - 1);
        replaysList.clear();
        endRemoveRows();
    }

    if (newReplaysList.count() > 0)
    {
        beginInsertRows(QModelIndex(), 0, newReplaysList.count() - 1);
        replaysList = newReplaysList;
        endInsertRows();
    }

    sort();
}

void ReplaysListModel::sort()
{

    for (int i = 0; i < replaysList.count(); i++)
    {
        for (int j = 0; j < replaysList.count() - 1; j++)
        {
            switch (m_sortType)
            {
                case (SortType::SortByFileName):
                {
                    if (replaysList[j].fileName > replaysList[j+1].fileName)
                    {
                        replace(j);
                    }

                    break;
                };

                case (SortType::SortByName):
                {
                    if (replaysList[j].name > replaysList[j+1].name)
                    {
                        replace(j);
                    }

                    break;
                };


                case (SortType::SortByMap):
                {
                    if (replaysList[j].map > replaysList[j+1].map)
                    {
                        replace(j);
                    }

                    break;
                };

                case (SortType::SortByMod):
                {
                    if (replaysList[j].mod > replaysList[j+1].mod)
                    {
                        replace(j);
                    }

                    break;
                };


                case (SortType::SortByTime):
                {
                    if (replaysList[j].time < replaysList[j+1].time)
                    {
                        replace(j);
                    }

                    break;
                };

            }
        }
    }

    if(replaysList.count() > 0)
        setSelected(0);
}

void ReplaysListModel::setSortType(int sortType)
{
    m_sortType = static_cast<SortType>(sortType);
    sort();
}

void ReplaysListModel::setSelected(int index)
{
    for(int i = 0; i < replaysList.count(); i++)
        replaysList[i].selected = false;

    replaysList[index].selected = true;

    emit select(replaysList[index].fileName);

    QModelIndex first = QAbstractItemModel::createIndex(0,0);
    QModelIndex last = QAbstractItemModel::createIndex(replaysList.count() - 1, 0);

    emit dataChanged(first, last);
}

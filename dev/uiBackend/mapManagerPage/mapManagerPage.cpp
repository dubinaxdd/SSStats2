#include "mapManagerPage.h"
#include <QDebug>

MapManagerPage::MapManagerPage(QObject *parent) : QAbstractListModel(parent)
{

}

QVariant MapManagerPage::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_mapItemArray.count())
        return QVariant();

    MapItem* mapItem = m_mapItemArray[index.row()];

    if (role == MapName)
        return mapItem->mapName;
    else if (role == Authors)
        return mapItem->authors;
    else if (role == Description)
        return mapItem->description;
    else if (role == Tags)
    {
        QString tags;

        for(int i = 0; i < mapItem->tags.count(); i++ )
        {
            tags += mapItem->tags.at(i);

            if (i != mapItem->tags.count()-1)
                 tags += ", ";
        }

        return tags;
    }
    else if (role == NeedInstall)
        return mapItem->needInstall;
    else if (role == NeedUpdate)
        return mapItem->needUpdate;
    else if (role == DownloadingProcessed)
        return mapItem->downloadProcessed;

    return QVariant();
}

int MapManagerPage::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_mapItemArray.count();
}

void MapManagerPage::receiveMapItem(MapItem *mapItem)
{
    for (int i = 0; i < m_mapItemArray.count(); i++)
    {
        if(mapItem->modContentHash == m_mapItemArray.at(i)->modContentHash)
        {
            QModelIndex index = QAbstractItemModel::createIndex(i, 0);
            emit dataChanged(index, index);

            checkUpdates();
            return;
        }
    }

    beginInsertRows(QModelIndex(), m_mapItemArray.count(), m_mapItemArray.count());
    m_mapItemArray.append(mapItem);
    endInsertRows();

    for(int i = 0; i < m_mapItemArray.count(); i++)
    {
        for(int j = 0; j < m_mapItemArray.count() - 1; j++)
        {
            QString name1 = m_mapItemArray.at(j)->mapName.right(3) + m_mapItemArray.at(j)->mapName;
            QString name2 = m_mapItemArray.at(j+1)->mapName.right(3) + m_mapItemArray.at(j+1)->mapName;

            if(name1 > name2)
            {
                MapItem *buffer = m_mapItemArray.at(j+1);
                m_mapItemArray[j+1] = m_mapItemArray.at(j);
                m_mapItemArray[j] = buffer;
            }
        }
    }

    QModelIndex first = QAbstractItemModel::createIndex(0,0);
    QModelIndex last = QAbstractItemModel::createIndex(m_mapItemArray.count() - 1, 0);

    emit dataChanged(first, last);

    checkUpdates();

}

QHash<int, QByteArray> MapManagerPage::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[MapName] = "mapName";
    roles[Authors] = "authors";
    roles[Description] = "description";
    roles[Tags] = "tags";
    roles[NeedInstall] = "needInstall";
    roles[NeedUpdate] = "needUpdate";
    roles[DownloadingProcessed] = "downloadingProcessed";

    return roles;
}

void MapManagerPage::checkUpdates()
{
    m_updatesAvailable = false;

    for (int i = 0; i < m_mapItemArray.count(); i++)
    {
        if (m_mapItemArray.at(i)->needInstall || m_mapItemArray.at(i)->needUpdate)
            m_updatesAvailable = true;
    }

    emit updatesAvailableChanged();
}

bool MapManagerPage::updatesAvailable() const
{
    return m_updatesAvailable;
}

void MapManagerPage::setUpdatesAvailable(bool newUpdatesAvailable)
{
    if (m_updatesAvailable == newUpdatesAvailable)
        return;
    m_updatesAvailable = newUpdatesAvailable;
    emit updatesAvailableChanged();
}

void MapManagerPage::removeMap(int index)
{
    emit sendRemoveMap(m_mapItemArray[index]);
}

void MapManagerPage::installMap(int index)
{
    m_mapItemArray[index]->downloadProcessed = true;
    QModelIndex needIndex = QAbstractItemModel::createIndex(index,0);
    emit dataChanged(needIndex, needIndex);

    emit sendInstallMap(m_mapItemArray[index]);
}

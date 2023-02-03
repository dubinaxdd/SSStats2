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
    else if (role == Size)
        return mapItem->size;
    else if (role == Tags)
    {
        QString tags;

        for(int i = 0; i < mapItem->tags.count(); i++ )
            tags += mapItem->tags.at(i) + ", ";

        return tags;
    }
    else if (role == NeedInstall)
        return mapItem->needInstall;
    else if (role == NeedUpdate)
        return mapItem->needUpdate;

    return QVariant();
}

int MapManagerPage::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_mapItemArray.count();
}

void MapManagerPage::receiveMapItem(MapItem *mapItem)
{

}

QHash<int, QByteArray> MapManagerPage::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[MapName] = "mapName";
    roles[Authors] = "authors";
    roles[Description] = "description";
    roles[Size] = "size";
    roles[Tags] = "tags";
    roles[NeedInstall] = "needInstall";
    roles[NeedUpdate] = "needUpdate";

    return roles;
}

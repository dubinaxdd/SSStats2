#include "mapManagerPage.h"
#include <QDebug>
#include <QFile>

MapManagerPage::MapManagerPage(ImageProvider* imageProvider, QObject *parent)
    : QAbstractListModel(parent)
    , m_imageProvider(imageProvider)
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
    else if (role == Selected)
        return mapItem->selected;

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

    //selectMap(0);
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
    roles[Selected] = "selected";

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

const QString &MapManagerPage::currentMapTags() const
{
    return m_currentMapTags;
}

void MapManagerPage::setCurrentMapTags(const QString &newCurrentMapTags)
{
    if (m_currentMapTags == newCurrentMapTags)
        return;
    m_currentMapTags = newCurrentMapTags;
    emit currentMapTagsChanged();
}

const QString &MapManagerPage::currentMapDescription() const
{
    return m_currentMapDescription;
}

void MapManagerPage::setCurrentMapDescription(const QString &newCurrentMapDescription)
{
    if (m_currentMapDescription == newCurrentMapDescription)
        return;
    m_currentMapDescription = newCurrentMapDescription;
    emit currentMapDescriptionChanged();
}

const QString &MapManagerPage::currentMapAuthors() const
{
    return m_currentMapAuthors;
}

void MapManagerPage::setCurrentMapAuthors(const QString &newCurrentMapAuthors)
{
    if (m_currentMapAuthors == newCurrentMapAuthors)
        return;
    m_currentMapAuthors = newCurrentMapAuthors;
    emit currentMapAuthorsChanged();
}

const QString &MapManagerPage::currentMapName() const
{
    return m_currentMapName;
}

void MapManagerPage::setCurrentMapName(const QString &newCurrentMapName)
{
    if (m_currentMapName == newCurrentMapName)
        return;
    m_currentMapName = newCurrentMapName;
    emit currentMapNameChanged();
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

void MapManagerPage::selectMap(int index)
{
    for(int i = 0; i < m_mapItemArray.count(); i++)
    {
        if (i == index)
            m_mapItemArray[i]->selected = true;
        else
            m_mapItemArray[i]->selected = false;
    }

    QModelIndex first = QAbstractItemModel::createIndex(0,0);
    QModelIndex last = QAbstractItemModel::createIndex(m_mapItemArray.count() - 1, 0);

    emit dataChanged(first, last);


    QImage minimap;
    for(int i = 0; i < m_mapItemArray[index]->filesList.count(); i++)
    {
        if(m_mapItemArray[index]->filesList.at(i).fileName.contains("_icon.tga"))
        {
            minimap = loadMiniMapImage(m_mapItemArray[index]->filesList.at(i).fileName);
            break;
        }
    }

    m_imageProvider->setCurrentMiniMap(std::move(minimap));

    setCurrentMapName(m_mapItemArray[index]->mapName);
    setCurrentMapAuthors(m_mapItemArray[index]->authors);
    setCurrentMapDescription(m_mapItemArray[index]->description);

    QString tags;

    for(int i = 0; i < m_mapItemArray[index]->tags.count(); i++ )
    {
        tags += m_mapItemArray[index]->tags.at(i);

        if (i != m_mapItemArray[index]->tags.count()-1)
             tags += ", ";
    }

    setCurrentMapTags(tags);
}


QImage MapManagerPage::loadMiniMapImage(QString fileName)
{
    QFile miniMapImageFile(m_ssPath + "\\DXP2\\Data\\Scenarios\\mp\\" + fileName);

    if (!miniMapImageFile.open(QIODevice::ReadOnly))
            return QImage();

    QByteArray imageBytes = miniMapImageFile.readAll();

    QByteArray widthBytes;
    widthBytes.append(imageBytes[13]);
    widthBytes.append(imageBytes[12]);

    QByteArray heightBytes;
    heightBytes.append(imageBytes[15]);
    heightBytes.append(imageBytes[14]);

    bool ok;
    int width = widthBytes.toHex().toInt(&ok, 16);
    int height = heightBytes.toHex().toInt(&ok, 16);


    qDebug() << width << height;

    int byteIndex = 18;

    QImage newImage(width, height, QImage::Format::Format_ARGB32);

    for (int y = height - 1; y >= 0 ; y--)
    {
        for (int x = 0; x < width; x++)
        {
            QByteArray colorBytes = imageBytes.mid(byteIndex, 4);

            int r = (quint8)colorBytes[2];
            int g = (quint8)colorBytes[1];
            int b = (quint8)colorBytes[0];
            int a = 255/*(quint8)colorBytes[3]*/;

            QColor color = QColor(r, g, b, a); //Возможно нужно инвертировать порядок байтов
            newImage.setPixelColor(x,y, color);

            byteIndex += 4;
        }
    }

    return newImage;
}

void MapManagerPage::setSsPath(const QString &newSsPath)
{
    m_ssPath = newSsPath;
}

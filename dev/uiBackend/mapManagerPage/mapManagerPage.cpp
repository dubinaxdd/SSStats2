#include "mapManagerPage.h"
#include <QDebug>
#include <QFile>

MapManagerPage::MapManagerPage(SettingsController* settingsController, ImageProvider* imageProvider, QObject *parent)
    : QAbstractListModel(parent)
    , m_settingsController(settingsController)
    , m_imageProvider(imageProvider)
{
    QObject::connect(m_settingsController, &SettingsController::settingsLoaded, this, &MapManagerPage::onSettingsLoaded, Qt::QueuedConnection );
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

            QString tags1 = consolidateTags(m_mapItemArray.at(j)->tags);
            QString tags2 = consolidateTags(m_mapItemArray.at(j + 1)->tags);

            QString name1 = m_mapItemArray.at(j)->mapName.right(3) + (tags1.contains("default-map") ? "0" : "") + m_mapItemArray.at(j)->mapName;
            QString name2 = m_mapItemArray.at(j+1)->mapName.right(3) + (tags2.contains("default-map") ? "0" : "") + m_mapItemArray.at(j+1)->mapName;

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

void MapManagerPage::receiveDownloadingProgress(int downloadedCount, int fullCount, bool downloadedProcessed)
{
    setDownloadedCount(downloadedCount);
    setFullCount(fullCount);
    setDownloadedProcessed(downloadedProcessed);
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

void MapManagerPage::onSettingsLoaded()
{
    setAutoinstallDefaultMaps(m_settingsController->getSettings()->autoinstallDefaultMaps);
    setAutoinstallAllMaps(m_settingsController->getSettings()->autoinstallAllMaps);
}

void MapManagerPage::checkUpdates()
{
    m_updatesAvailable = false;

    for (int i = 0; i < m_mapItemArray.count(); i++)
    {
        if ((m_mapItemArray.at(i)->needInstall || m_mapItemArray.at(i)->needUpdate) && consolidateTags(m_mapItemArray.at(i)->tags).contains("default-map"))
        {
            m_updatesAvailable = true;
            break;
        }
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
    setCurrentMapTags(consolidateTags(m_mapItemArray[index]->tags));
    setCurrentMapNeedInstall(m_mapItemArray[index]->needInstall);
}

void MapManagerPage::installAllMaps()
{
    for(int i = 0; i < m_mapItemArray.count(); i++)
    {
        if (m_mapItemArray.at(i)->needInstall || m_mapItemArray.at(i)->needUpdate)
            m_mapItemArray[i]->downloadProcessed = true;
    }

    QModelIndex first = QAbstractItemModel::createIndex(0,0);
    QModelIndex last = QAbstractItemModel::createIndex(m_mapItemArray.count() - 1, 0);

    emit dataChanged(first, last);

    emit sendInstallAllMaps();
}

void MapManagerPage::installDefaultMaps()
{
    for(int i = 0; i < m_mapItemArray.count(); i++)
    {
        QString tags = consolidateTags(m_mapItemArray.at(i)->tags);

        if ((m_mapItemArray.at(i)->needInstall || m_mapItemArray.at(i)->needUpdate) && tags.contains("default-map"))
            m_mapItemArray[i]->downloadProcessed = true;
    }

    QModelIndex first = QAbstractItemModel::createIndex(0,0);
    QModelIndex last = QAbstractItemModel::createIndex(m_mapItemArray.count() - 1, 0);

    emit dataChanged(first, last);

    emit sendInstallDefaultMaps();
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

    int quality = imageBytes.mid(16,1).toHex().toInt(&ok, 16);

    int byteIndex = 18;

    QImage newImage(width, height, QImage::Format::Format_ARGB32);

    if (quality == 32)
    {
        for (int y = height - 1; y >= 0 ; y--)
        {
            for (int x = 0; x < width; x++)
            {
                QByteArray colorBytes = imageBytes.mid(byteIndex, 4);

                int r = (quint8)colorBytes[2];
                int g = (quint8)colorBytes[1];
                int b = (quint8)colorBytes[0];
                int a = (quint8)colorBytes[3];

                QColor color = QColor(r, g, b, a);
                newImage.setPixelColor(x,y, color);

                byteIndex += 4;
            }
        }
    }
    else
    {
        for (int y = height - 1; y >= 0 ; y--)
        {
            for (int x = 0; x < width; x++)
            {
                QByteArray colorBytes = imageBytes.mid(byteIndex, 3);

                int r = (quint8)colorBytes[2];
                int g = (quint8)colorBytes[1];
                int b = (quint8)colorBytes[0];
                int a = 255;//(quint8)colorBytes[3];

                QColor color = QColor(r, g, b, a);
                newImage.setPixelColor(x,y, color);

                byteIndex += 3;
            }
        }
    }

    return newImage;
}

QString MapManagerPage::consolidateTags(QList<QString> tags)
{
    QString tagsString;

    for(int i = 0; i < tags.count(); i++ )
    {
        tagsString += tags.at(i);

        if (i != tags.count()-1)
             tagsString += ", ";
    }

    return tagsString;
}

bool MapManagerPage::autoinstallAllMaps() const
{
    return m_autoinstallAllMaps;
}

void MapManagerPage::setAutoinstallAllMaps(bool newAutoinstallAllMaps)
{
    if (m_autoinstallAllMaps == newAutoinstallAllMaps)
        return;
    m_autoinstallAllMaps = newAutoinstallAllMaps;
    emit autoinstallAllMapsChanged();

    m_settingsController->getSettings()->autoinstallAllMaps = m_autoinstallAllMaps;
    m_settingsController->saveSettings();
}

bool MapManagerPage::autoinstallDefaultMaps() const
{
    return m_autoinstallDefaultMaps;
}

void MapManagerPage::setAutoinstallDefaultMaps(bool newAutoinstallDefaultMaps)
{
    if (m_autoinstallDefaultMaps == newAutoinstallDefaultMaps)
        return;

    m_autoinstallDefaultMaps = newAutoinstallDefaultMaps;
    emit autoinstallDefaultMapsChanged();

    m_settingsController->getSettings()->autoinstallDefaultMaps = m_autoinstallDefaultMaps;
    m_settingsController->saveSettings();
}

bool MapManagerPage::downloadedProcessed() const
{
    return m_downloadedProcessed;
}

void MapManagerPage::setDownloadedProcessed(bool newDownloadedProcessed)
{
    if (m_downloadedProcessed == newDownloadedProcessed)
        return;
    m_downloadedProcessed = newDownloadedProcessed;
    emit downloadedProcessedChanged();
}

int MapManagerPage::fullCount() const
{
    return m_fullCount;
}

void MapManagerPage::setFullCount(int newFullCount)
{
    if (m_fullCount == newFullCount)
        return;
    m_fullCount = newFullCount;
    emit fullCountChanged();
}

int MapManagerPage::downloadedCount() const
{
    return m_downloadedCount;
}

void MapManagerPage::setDownloadedCount(int newDownloadedCount)
{
    if (m_downloadedCount == newDownloadedCount)
        return;
    m_downloadedCount = newDownloadedCount;
    emit downloadedCountChanged();
}

bool MapManagerPage::currentMapNeedInstall() const
{
    return m_currentMapNeedInstall;
}

void MapManagerPage::setCurrentMapNeedInstall(bool newCurrentMapNeedInstall)
{
    if (m_currentMapNeedInstall == newCurrentMapNeedInstall)
        return;
    m_currentMapNeedInstall = newCurrentMapNeedInstall;
    emit currentMapNeedInstallChanged();
}

void MapManagerPage::setSsPath(const QString &newSsPath)
{
    m_ssPath = newSsPath;
}

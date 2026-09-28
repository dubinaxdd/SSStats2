#include "mapManager.h"
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <zlib.h>
#include <zconf.h>
#include <QtZlib/zlib.h>
#include "JlCompress.h"
#include <QCryptographicHash>
#include <QtConcurrent/QtConcurrent>
#include <QDir>

#define GZIP_WINDOWS_BIT 15 + 16
#define GZIP_CHUNK_SIZE 32 * 1024

MapManager::MapManager(SettingsController *settingsController, GamePath *currentGame, QObject *parent)
    : QObject(parent)
    , m_currentGame(currentGame)
    , m_settingsController(settingsController)
    , m_networkManager (new QNetworkAccessManager(this))
    , m_fileHashReader( new FileHashReader())
    , m_fileHashReaderThread(new QThread(this))
{
    QObject::connect(m_settingsController, &SettingsController::settingsLoaded, this, &MapManager::onSettingsLoaded, Qt::QueuedConnection);

    QObject::connect(this, &MapManager::requsetLocalMapFilesList, m_fileHashReader, &FileHashReader::getLocalMapFilesList, Qt::QueuedConnection);
    QObject::connect(m_fileHashReader, &FileHashReader::sendLocalMapFilesList, this, &MapManager::receiveLocalMapFilesList,  Qt::QueuedConnection);

    m_fileHashReader->moveToThread(m_fileHashReaderThread);
    m_fileHashReaderThread->start();
}

MapManager::~MapManager()
{
    m_fileHashReaderThread->quit();
    m_fileHashReaderThread->wait();

    delete m_fileHashReader;
}

void MapManager::requestMapList()
{
    if(m_blockInfoUpdate)
        return;

    m_blockInfoUpdate = true;

    QString url = "https://dowonline.ru/api/Mods/list?ModType=2";

    QNetworkRequest newRequest = QNetworkRequest(QUrl(url));

    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveMapList(reply);
    });
}

void MapManager::receiveMapList(QNetworkReply *reply)
{
    if(!reply)
    {
        m_blockInfoUpdate = false;
        return;
    }

    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "MapManager::receiveMapList Connection error:" << reply->errorString();

        reply->deleteLater();

        m_blockInfoUpdate = false;
        m_checkUpdatesProcessed = false;

        emit mapsInfoLoaded();

        return;
    }

    QByteArray replyByteArray = reply->readAll();

    reply->deleteLater();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyByteArray);

    if(!jsonDoc.isArray())
    {
        m_blockInfoUpdate = false;
        return;
    }

    QJsonArray replyJsonArray = jsonDoc.array();

    m_mapItemArray.clear();
    m_requestetMapInfoCount = 0;

    for (int i = 0; i < replyJsonArray.count(); i++)
    {
        if(!replyJsonArray.at(i).isObject())
            continue;

        QJsonObject newObject = replyJsonArray.at(i).toObject();

        MapItem newMapItem;

        newMapItem.id = QString::number(newObject.value("id").toInt());
        newMapItem.authors = newObject.value("authors").toString();
        newMapItem.description = newObject.value("description").toString().replace("\n", " ");
        newMapItem.webPage = newObject.value("webPage").toString();
        newMapItem.mapName = newObject.value("modName").toString();
        newMapItem.modContentHash = newObject.value("modContentHash").toString();
        newMapItem.unpackedSize = newObject.value("unpackedSize").toInt();
        newMapItem.packedSize = newObject.value("packedSize").toInt();
        newMapItem.type = newObject.value("type").toInt();

        if(!newObject.value("tags").isArray())
            continue;

        QJsonArray tagsJsonArray = newObject.value("tags").toArray();

        for (int j = 0; j < tagsJsonArray.count(); j++)
            newMapItem.tags.append(tagsJsonArray.at(j).toString());

        m_mapItemArray.append(std::move(newMapItem));
    }

    if(m_mapItemArray.isEmpty())
    {
        m_blockInfoUpdate = false;
        m_checkUpdatesProcessed = false;
        m_mapListLoaded = true;

        emit mapsInfoLoaded();

        return;
    }

    for(int i = 0; i < m_mapItemArray.count(); i++)
    {
        requestMapInfo(
            m_mapItemArray.at(i).id,
            m_mapItemArray.at(i).modContentHash);

        requestMapImage(m_mapItemArray.at(i).id);
    }

    m_checkUpdatesProcessed = false;
    m_mapListLoaded = true;
}

void MapManager::updateMapList()
{
    m_requestetMapInfoCount = 0;

    for(int i = 0; i < m_mapItemArray.count(); i++)
    {
        MapItem *mapItem = &m_mapItemArray[i];

        checkLocalFilesState(mapItem);

        if ((mapItem->needInstall || mapItem->needUpdate) )
        {
            if (m_settingsController->getSettings()->autoinstallAllMaps
                || (m_settingsController->getSettings()->autoinstallDefaultMaps
                    && consolidateTags(mapItem->tags).contains("default-map"))
                )
                mapItem->downloadProcessed = true;
        }

        m_requestetMapInfoCount++;

        emit sendMapItem(mapItem);

        if(m_requestetMapInfoCount == m_mapItemArray.count())
        {
            m_blockInfoUpdate = false;
            emit mapsInfoLoaded();

            if( m_settingsController->getSettings()->autoinstallAllMaps)
            {
                receiveInstallAllMaps();
                return;
            }

            if( m_settingsController->getSettings()->autoinstallDefaultMaps)
            {
                receiveInstallDefaultMaps();
                return;
            }
        }
    }

    m_checkUpdatesProcessed = false;
    m_mapListLoaded = true;

    emit mapsInfoLoaded();
}

void MapManager::requestMapInfo(QString mapId, QString mapContentHash)
{
    if(mapId.isEmpty() || mapContentHash.isEmpty())
        return;

    QString url = getUrl(mapContentHash);

    QNetworkRequest newRequest = QNetworkRequest(QUrl(url));
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveMapInfo(reply, mapId);
    });
}

void MapManager::receiveMapInfo(QNetworkReply *reply, QString mapId)
{
    if(!reply)
        return;

    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "MapManager::receiveMapInfo Connection error:" << reply->errorString();

        reply->deleteLater();

        m_requestetMapInfoCount++;

        if(m_requestetMapInfoCount >= m_mapItemArray.count())
        {
            m_blockInfoUpdate = false;
            emit mapsInfoLoaded();
        }

        return;
    }

    QByteArray replyByteArray = reply->readAll();

    reply->deleteLater();

    QByteArray uncompressedByteArray;

    if (!uncompressGz(replyByteArray, uncompressedByteArray))
    {
        qWarning(logWarning()) << "MapManager::receiveMapInfo Can't uncompress map info:" << mapId;

        m_requestetMapInfoCount++;

        if(m_requestetMapInfoCount >= m_mapItemArray.count())
        {
            m_blockInfoUpdate = false;
            emit mapsInfoLoaded();
        }

        return;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(uncompressedByteArray);

    if(!jsonDoc.isObject())
    {
        qWarning(logWarning()) << "MapManager::receiveMapInfo Invalid JSON:" << mapId;

        m_requestetMapInfoCount++;

        if(m_requestetMapInfoCount >= m_mapItemArray.count())
        {
            m_blockInfoUpdate = false;
            emit mapsInfoLoaded();
        }

        return;
    }

    MapItem *mapItem = getMapItemById(mapId);

    if(!mapItem)
    {
        qWarning(logWarning()) << "MapManager::receiveMapInfo Map item not found:" << mapId;

        m_requestetMapInfoCount++;

        if(m_requestetMapInfoCount >= m_mapItemArray.count())
        {
            m_blockInfoUpdate = false;
            emit mapsInfoLoaded();
        }

        return;
    }

    QJsonObject jsonObj = jsonDoc.object();
    QJsonObject filesInfoObject = jsonObj.value("files").toObject();

    QStringList keys = filesInfoObject.keys();

    QList<MapFileHash> newFileHashArray;

    for(int i = 0; i < keys.count(); i++)
    {
        QJsonObject fileObject = filesInfoObject.value(keys.at(i)).toObject();
        QString hash = fileObject.value("fullFileHash").toString();

        MapFileHash newFileHash;

        newFileHash.fileName = keys.at(i);
        newFileHash.hash = hash;

        newFileHashArray.append(newFileHash);
    }

    mapItem->filesList = newFileHashArray;

    checkLocalFilesState(mapItem);

    if ((mapItem->needInstall || mapItem->needUpdate) )
    {
        if (m_settingsController->getSettings()->autoinstallAllMaps
            || (m_settingsController->getSettings()->autoinstallDefaultMaps
                && consolidateTags(mapItem->tags).contains("default-map"))
            )
            mapItem->downloadProcessed = true;
    }

    m_requestetMapInfoCount++;

    emit sendMapItem(mapItem);

    if(m_requestetMapInfoCount >= m_mapItemArray.count())
    {
        m_blockInfoUpdate = false;
        emit mapsInfoLoaded();

        if( m_settingsController->getSettings()->autoinstallAllMaps)
        {
            receiveInstallAllMaps();
            return;
        }

        if( m_settingsController->getSettings()->autoinstallDefaultMaps)
        {
            receiveInstallDefaultMaps();
            return;
        }
    }
}

void MapManager::requestFile(QString fileName, QString fileHash, QString mapId)
{
    if(fileName.isEmpty() || fileHash.isEmpty() || mapId.isEmpty())
        return;

    m_blockInfoUpdate = true;

    QString url = getUrl(fileHash);

    QNetworkRequest newRequest = QNetworkRequest(QUrl(url));
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveFile(reply, fileName, mapId);
    });
}

void MapManager::receiveFile(QNetworkReply *reply, QString fileName, QString mapId)
{
    if(!reply)
        return;

    MapItem *mapItem = getMapItemById(mapId);

    if(!mapItem)
    {
        qWarning(logWarning()) << "MapManager::receiveFile Map item not found:" << mapId;
        reply->deleteLater();
        return;
    }

    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "MapManager::receiveFile Connection error:" << reply->errorString();
        reply->deleteLater();

        mapItem->needInstall = mapItem->downloadedFiles < 0;
        mapItem->needUpdate = mapItem->downloadedFiles != mapItem->filesList.count();
        mapItem->downloadProcessed = false;

        emit sendMapItem(mapItem);

        updateBlockInfoUpdate();

        if (m_allMapsDownloadingProcessed)
            downloadNextMap();

        return;
    }

    QByteArray replyByteArray = reply->readAll();

    reply->deleteLater();

    QByteArray uncompressedByteArray;

    if (!uncompressGz(replyByteArray, uncompressedByteArray))
    {
        qWarning(logWarning()) << "MapManager::receiveFile Can't uncompress file:" << fileName;

        mapItem->needInstall = true;
        mapItem->needUpdate = true;
        mapItem->downloadProcessed = false;

        emit sendMapItem(mapItem);

        updateBlockInfoUpdate();

        return;
    }

    if(!m_currentGame)
    {
        qWarning(logWarning()) << "MapManager::receiveFile Current game is null";
        return;
    }

    QFile newFile;

    if (m_currentGame->gameType == GameType::GameTypeEnum::DefinitiveEdition)
    {
        QString path = m_currentGame->gamePath + "\\DXP3\\Data\\Scenarios\\mp";

        QDir dir(path);

        if(!dir.exists())
            dir.mkpath(path);

        newFile.setFileName(path + QDir::separator() + fileName);
    }
    else
    {
        QString path = m_currentGame->gamePath + "\\DXP2\\Data\\Scenarios\\mp";

        QDir dir(path);

        if(!dir.exists())
            dir.mkpath(path);

        newFile.setFileName(m_currentGame->gamePath + "\\DXP2\\Data\\Scenarios\\mp" + QDir::separator() + fileName);
    }

    if(!newFile.open(QIODevice::WriteOnly))
    {
        qWarning(logWarning()) << "MapManager::receiveFile Can't open file:"
                               << newFile.fileName();

        mapItem->needInstall = true;
        mapItem->needUpdate = true;
        mapItem->downloadProcessed = false;

        emit sendMapItem(mapItem);

        updateBlockInfoUpdate();

        return;
    }

    newFile.write(uncompressedByteArray);
    newFile.close();

    mapItem->downloadedFiles++;

    if(mapItem->downloadedFiles >= mapItem->filesList.count())
    {
        mapItem->needInstall = false;
        mapItem->needUpdate = false;
        mapItem->downloadProcessed = false;

        emit sendMapItem(mapItem);

        updateBlockInfoUpdate();

        if (m_allMapsDownloadingProcessed)
            downloadNextMap();
    }
    else
    {
        requestFile(
            mapItem->filesList.at(mapItem->downloadedFiles).fileName,
            mapItem->filesList.at(mapItem->downloadedFiles).hash,
            mapItem->id);
    }
}

void MapManager::requestMapImage(QString id)
{
    QString url = "https://dowonline.ru/Storage/ModIcons/" + id + ".jpg";

    QNetworkRequest newRequest = QNetworkRequest(QUrl(url));
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveMapImage(reply, id);
    });
}

void MapManager::receiveMapImage(QNetworkReply *reply, QString id)
{
    if(!reply)
        return;

    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "MapManager::receiveMapImage Connection error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray replyByteArray = reply->readAll();

    reply->deleteLater();

    QImage mapImage = QImage::fromData(replyByteArray);

    if (mapImage.isNull())
        return;

    emit sendMapImage(std::move(mapImage), "mapImage" + id);
}

void MapManager::downloadNextMap()
{
    if(m_downloadedMapsCount < 0)
        m_downloadedMapsCount = 0;

    if(m_downloadedMapsCount >= m_mapItemArray.count())
    {
        m_allMapsDownloadingProcessed = false;

        emit sendDownloadingProgress(
            m_downloadedMapsCount,
            m_mapItemArray.count(),
            false);

        return;
    }

    MapItem *mapItem = &m_mapItemArray[m_downloadedMapsCount];

    if(!(mapItem->needInstall || mapItem->needUpdate))
    {
        m_downloadedMapsCount++;
        downloadNextMap();

        return;
    }

    if(m_downloadOnlyDefaultMaps)
    {
        if(!consolidateTags(mapItem->tags).contains("default-map"))
        {
            m_downloadedMapsCount++;
            downloadNextMap();

            return;
        }
    }

    emit sendDownloadingProgress(
        m_downloadedMapsCount,
        m_mapItemArray.count(),
        true);

    installMap(mapItem);

    m_downloadedMapsCount++;
}

void MapManager::installMap(MapItem *mapItem)
{
    if(!mapItem)
        return;

    if(mapItem->filesList.isEmpty())
    {
        qWarning(logWarning()) << "MapManager::installMap Files list is empty:"
                               << mapItem->id;

        mapItem->needInstall = true;
        mapItem->needUpdate = true;
        mapItem->downloadProcessed = false;

        emit sendMapItem(mapItem);

        return;
    }

    mapItem->downloadedFiles = 0;

    requestFile(
        mapItem->filesList.at(0).fileName,
        mapItem->filesList.at(0).hash,
        mapItem->id);
}

void MapManager::receiveRemoveMap(MapItem *mapItem)
{
    if(!mapItem)
        return;

    if(!m_currentGame)
        return;

    for (int i = 0; i < mapItem->filesList.count(); i++)
    {
        QString path;

        if (m_currentGame->gameType == GameType::GameTypeEnum::DefinitiveEdition)
            path = (m_currentGame->gamePath + "\\DXP3\\Data\\Scenarios\\mp\\" + mapItem->filesList.at(i).fileName);
        else
            path = (m_currentGame->gamePath + "\\DXP2\\Data\\Scenarios\\mp\\" + mapItem->filesList.at(i).fileName);

        QFile tempfile(path);
        tempfile.remove();

        qInfo(logInfo()) << "Map file uninstalled from " << path;

        mapItem->needInstall = true;
        mapItem->needUpdate = true;

        emit sendMapItem(mapItem);
    }
}

void MapManager::receiveInstallMap(MapItem *mapItem)
{
    if(m_allMapsDownloadingProcessed)
        return;

    if(!mapItem)
        return;

    installMap(mapItem);
}

void MapManager::receiveInstallAllMaps()
{
    if(m_allMapsDownloadingProcessed)
        return;

    m_downloadedMapsCount = 0;
    m_allMapsDownloadingProcessed = true;

    m_downloadOnlyDefaultMaps = false;

    downloadNextMap();
}

void MapManager::receiveInstallDefaultMaps()
{
    if(m_allMapsDownloadingProcessed)
        return;

    m_downloadedMapsCount = 0;
    m_allMapsDownloadingProcessed = true;

    m_downloadOnlyDefaultMaps = true;

    downloadNextMap();
}

void MapManager::receiveLoadMapsInfo()
{
    if(m_checkUpdatesProcessed)
        return;

    m_checkUpdatesProcessed = true;

    if(!m_currentGame)
    {
        qWarning(logWarning()) << "MapManager::receiveLoadMapsInfo Current game is null";
        return;
    }

    QString path;

    if(m_currentGame->gameType == GameType::GameTypeEnum::DefinitiveEdition)
        path = m_currentGame->gamePath + "\\DXP3\\Data\\Scenarios\\mp";
    else
        path = m_currentGame->gamePath + "\\DXP2\\Data\\Scenarios\\mp";

    emit requsetLocalMapFilesList(path);
}

void MapManager::onSettingsLoaded()
{
    qInfo(logInfo()) << "MapManager::onSettingsLoaded()" << "load started";

    receiveLoadMapsInfo();

    qInfo(logInfo()) << "MapManager::onSettingsLoaded()" << "load finished";
}

void MapManager::receiveLocalMapFilesList(QList<MapFileHash> localMapFilesList)
{
    m_localMapFilesHashes = localMapFilesList;

    if(!m_mapListLoaded)
        requestMapList();
    else
        updateMapList();
}

QString MapManager::getUrl(QString mapHash)
{
    QByteArray bytes = QByteArray::fromBase64((mapHash.toUtf8()));
    QByteArray hex = bytes.toHex();

    QString hexStr = QString::fromUtf8(hex).toUpper();

    QString url = "https://dowonline.ru/Storage/" + hexStr.mid(0, 2) + "/" + hexStr.mid(2, 2) + "/" + hexStr + ".gz";

    return url;
}

void MapManager::checkLocalFilesState(MapItem *mapItem)
{
    if(!mapItem)
        return;

    bool needInstall = true;
    bool needUpdate = false;
    int foundedFiles = 0;

    mapItem->downloadedFiles = 0;

    if(mapItem->filesList.isEmpty())
    {
        mapItem->needInstall = true;
        mapItem->needUpdate = false;

        return;
    }

    for(int i = 0; i < mapItem->filesList.count(); i++)
    {
        for(int j = 0; j < m_localMapFilesHashes.count(); j++)
        {
            if(mapItem->filesList.at(i).fileName == m_localMapFilesHashes.at(j).fileName)
            {
                needInstall = false;
                foundedFiles++;

                if(mapItem->filesList.at(i).hash != m_localMapFilesHashes.at(j).hash)
                    needUpdate = true;

                break;
            }
        }
    }

    if(foundedFiles < mapItem->filesList.count())
    {
        needInstall = true;
        needUpdate = true;
    }

    mapItem->needUpdate = needUpdate;
    mapItem->needInstall = needInstall;
}

QString MapManager::consolidateTags(QList<QString> tags)
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

void MapManager::updateBlockInfoUpdate()
{
    bool downloadProcessed = false;

    for (int i = 0; i < m_mapItemArray.count(); i++)
    {
        if (m_mapItemArray.at(i).downloadProcessed)
        {
            downloadProcessed = true;
            break;
        }
    }

    m_blockInfoUpdate = downloadProcessed;
}

MapItem* MapManager::getMapItemById(QString mapId)
{
    if(mapId.isEmpty())
        return nullptr;

    for(int i = 0; i < m_mapItemArray.count(); i++)
    {
        if(m_mapItemArray.at(i).id == mapId)
            return &m_mapItemArray[i];
    }

    return nullptr;
}

bool MapManager::uncompressGz(QByteArray input, QByteArray &output)
{
    output.clear();

    if(input.length() > 0)
    {
        z_stream strm;

        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;
        strm.avail_in = 0;
        strm.next_in = Z_NULL;

        int ret = inflateInit2(&strm, GZIP_WINDOWS_BIT);

        if(ret != Z_OK)
            return(false);

        char *input_data = input.data();
        int input_data_left = input.length();

        do {
            int chunk_size = qMin(GZIP_CHUNK_SIZE, input_data_left);

            if(chunk_size <= 0)
                break;

            strm.next_in = (unsigned char*)input_data;
            strm.avail_in = chunk_size;

            input_data += chunk_size;
            input_data_left -= chunk_size;

            do {
                char out[GZIP_CHUNK_SIZE];

                strm.next_out = (unsigned char*)out;
                strm.avail_out = GZIP_CHUNK_SIZE;

                ret = inflate(&strm, Z_NO_FLUSH);

                switch (ret) {
                case Z_NEED_DICT:
                case Z_DATA_ERROR:
                case Z_MEM_ERROR:
                case Z_STREAM_ERROR:
                    inflateEnd(&strm);
                    return(false);
                }

                int have = GZIP_CHUNK_SIZE - strm.avail_out;

                if(have > 0)
                    output.append((char*)out, have);

            } while (strm.avail_out == 0);

        } while (ret != Z_STREAM_END);

        inflateEnd(&strm);

        return(ret == Z_STREAM_END);
    }
    else
        return(true);
}

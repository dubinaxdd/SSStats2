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

#define GZIP_WINDOWS_BIT 15 + 16
#define GZIP_CHUNK_SIZE 32 * 1024

MapManager::MapManager(SettingsController *settingsController, QString ssPath, QObject *parent)
    : QObject(parent)
    , m_settingsController(settingsController)
    , m_ssPath(ssPath)
    , m_fileHashReader( new FileHashReader(m_ssPath))
    , m_fileHashReaderThread(new QThread(this))
{
     m_networkManager = new QNetworkAccessManager(this);

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
    m_fileHashReader->deleteLater();
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
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "MapManager::receiveMapList Connection error:" << reply->errorString();
        delete reply;
        m_checkUpdatesProcessed = false;
        emit mapsInfoLoaded();
        return;
    }

    QByteArray replyByteArray = reply->readAll();

    delete reply;

    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyByteArray);

    if(!jsonDoc.isArray())
        return;

    QJsonArray replyJsonArray = jsonDoc.array();

    //TODO: Возможен баг так как другие методы могут обращаться по указателям к уже не существующим элементам массива
    //m_blockInfoUpdate должен помочь

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

    for(int i = 0; i < m_mapItemArray.count(); i++)
    {
        requestMapInfo( &m_mapItemArray[i] );
        requestMapImage(m_mapItemArray[i].id);
    }

    m_checkUpdatesProcessed = false;
}

void MapManager::requestMapInfo(MapItem *mapItem)
{
    QString url = getUrl(mapItem->modContentHash);

    QNetworkRequest newRequest = QNetworkRequest(QUrl(url));
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveMapInfo(reply, mapItem);
    });
}

void MapManager::receiveMapInfo(QNetworkReply *reply, MapItem *mapItem)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "MapManager::receiveMapInfo Connection error:" << reply->errorString();
        delete reply;

        m_requestetMapInfoCount++;

        if(m_requestetMapInfoCount == m_mapItemArray.count())
        {
            m_blockInfoUpdate = false;
            emit mapsInfoLoaded();
        }

        return;
    }

    QByteArray replyByteArray = reply->readAll();
    delete reply;

    QByteArray uncompressedByteArray;

    if (!uncompressGz(replyByteArray, uncompressedByteArray))
        return;

    QJsonDocument jsonDoc = QJsonDocument::fromJson(uncompressedByteArray);

    if(!jsonDoc.isObject())
        return;

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

void MapManager::requestFile(QString fileName, QString fileHash, MapItem *mapItem)
{
    m_blockInfoUpdate = true;

    QString url = getUrl(fileHash);

    QNetworkRequest newRequest = QNetworkRequest(QUrl(url));
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveFile(reply, fileName, mapItem);
    });
}

void MapManager::receiveFile(QNetworkReply *reply, QString fileName, MapItem *mapItem)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "MapManager::receiveFile Connection error:" << reply->errorString();
        delete reply;

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
    delete reply;

    QByteArray uncompressedByteArray;

    if (!uncompressGz(replyByteArray, uncompressedByteArray))
        return;

    QFile newFile( m_ssPath + "\\DXP2\\Data\\Scenarios\\mp" + QDir::separator() + fileName );
    if( newFile.open( QIODevice::WriteOnly ) ) {
        newFile.write(uncompressedByteArray);
        newFile.close();
    }

    mapItem->downloadedFiles++;

    if(mapItem->downloadedFiles == mapItem->filesList.count())
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
        requestFile(mapItem->filesList.at(mapItem->downloadedFiles).fileName, mapItem->filesList.at(mapItem->downloadedFiles).hash, mapItem);
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
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "MapManager::receiveMapImage Connection error:" << reply->errorString();
        delete reply;
        return;
    }

    QByteArray replyByteArray = reply->readAll();

    delete reply;

    QImage mapImage = QImage::fromData(replyByteArray);

    if (mapImage.isNull())
        return;

    emit sendMapImage(std::move(mapImage), "mapImage" + id);
}

void MapManager::downloadNextMap()
{
    if (m_downloadedMapsCount == m_mapItemArray.count())
    {
        m_allMapsDownloadingProcessed = false;
        emit sendDownloadingProgress(m_downloadedMapsCount, m_mapItemArray.count(), false);
        return;
    }

    if( !(m_mapItemArray[m_downloadedMapsCount].needInstall || m_mapItemArray[m_downloadedMapsCount].needUpdate) )
    {
        m_downloadedMapsCount++;
        downloadNextMap();
        return;
    }

    if(m_downloadOnlyDefaultMaps)
    {
        if( !consolidateTags(m_mapItemArray[m_downloadedMapsCount].tags).contains("default-map"))
        {
            m_downloadedMapsCount++;
            downloadNextMap();
            return;
        }
    }

    emit sendDownloadingProgress(m_downloadedMapsCount, m_mapItemArray.count(), true);
    installMap(&(m_mapItemArray[m_downloadedMapsCount]));
    m_downloadedMapsCount++;
}

void MapManager::installMap(MapItem *mapItem)
{
    mapItem->downloadedFiles = 0;

    requestFile(mapItem->filesList.at(0).fileName, mapItem->filesList.at(0).hash, mapItem);
}

void MapManager::receiveRemoveMap(MapItem *mapItem)
{
    for (int i = 0; i < mapItem->filesList.count(); i++)
    {
        QString path = (m_ssPath + "\\DXP2\\Data\\Scenarios\\mp\\" + mapItem->filesList.at(i).fileName);

        QFile tempfile(path);
        tempfile.remove();

        qInfo(logInfo()) <<  "Map file uninstalled from " << path;

        mapItem->needInstall = true;
        mapItem->needUpdate = true;

        emit sendMapItem( mapItem );
    }
}

void MapManager::receiveInstallMap(MapItem *mapItem)
{
    if (m_allMapsDownloadingProcessed)
        return;

    installMap(mapItem);
}

void MapManager::receiveInstallAllMaps()
{
    if (m_allMapsDownloadingProcessed)
        return;

    m_downloadedMapsCount = 0;
    m_allMapsDownloadingProcessed = true;

    m_downloadOnlyDefaultMaps = false;

    downloadNextMap();
}

void MapManager::receiveInstallDefaultMaps()
{
    if (m_allMapsDownloadingProcessed)
        return;

    m_downloadedMapsCount = 0;
    m_allMapsDownloadingProcessed = true;

    m_downloadOnlyDefaultMaps = true;
    downloadNextMap();
}

void MapManager::receiveLoadMapsInfo()
{
    if (m_checkUpdatesProcessed)
        return;

    m_checkUpdatesProcessed = true;
    emit requsetLocalMapFilesList();
}

void MapManager::onSettingsLoaded()
{
    m_checkUpdatesProcessed = true;
    emit requsetLocalMapFilesList();
}

void MapManager::receiveLocalMapFilesList(QList<MapFileHash> localMapFilesList)
{
    m_localMapFilesHashes = localMapFilesList;
    requestMapList();
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

    bool needInstall = true;
    bool needUpdate = false;
    int foundedFiles = 0;

    for(int i = 0; i < mapItem->filesList.count(); i++)
    {
        for(int j = 0; j < m_localMapFilesHashes.count(); j++)
        {
            if (mapItem->filesList.at(i).fileName == m_localMapFilesHashes.at(j).fileName)
            {
                needInstall = false;
                foundedFiles++;

                if (mapItem->filesList.at(i).hash != m_localMapFilesHashes.at(j).hash)
                {
                    needUpdate = true;
                    mapItem->downloadedFiles++;
                }

                continue;
            }
        }
    }

    if (foundedFiles < mapItem->filesList.count())
        needUpdate = true;

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

bool MapManager::uncompressGz(QByteArray input, QByteArray &output)
{

    //TODO: Source https://stackoverflow.com/questions/2690328/qt-quncompress-gzip-data

    // Prepare output
    output.clear();

    // Is there something to do?
    if(input.length() > 0)
    {
        // Prepare inflater status
        z_stream strm;
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;
        strm.avail_in = 0;
        strm.next_in = Z_NULL;

        // Initialize inflater
        int ret = inflateInit2(&strm, GZIP_WINDOWS_BIT);

        if (ret != Z_OK)
            return(false);

        // Extract pointer to input data
        char *input_data = input.data();
        int input_data_left = input.length();

        // Decompress data until available
        do {
            // Determine current chunk size
            int chunk_size = qMin(GZIP_CHUNK_SIZE, input_data_left);

            // Check for termination
            if(chunk_size <= 0)
                break;

            // Set inflater references
            strm.next_in = (unsigned char*)input_data;
            strm.avail_in = chunk_size;

            // Update interval variables
            input_data += chunk_size;
            input_data_left -= chunk_size;

            // Inflate chunk and cumulate output
            do {

                // Declare vars
                char out[GZIP_CHUNK_SIZE];

                // Set inflater references
                strm.next_out = (unsigned char*)out;
                strm.avail_out = GZIP_CHUNK_SIZE;

                // Try to inflate chunk
                ret = inflate(&strm, Z_NO_FLUSH);

                switch (ret) {
                case Z_NEED_DICT:
                    ret = Z_DATA_ERROR;
                case Z_DATA_ERROR:
                case Z_MEM_ERROR:
                case Z_STREAM_ERROR:
                    // Clean-up
                    inflateEnd(&strm);

                    // Return
                    return(false);
                }

                // Determine decompressed size
                int have = (GZIP_CHUNK_SIZE - strm.avail_out);

                // Cumulate result
                if(have > 0)
                    output.append((char*)out, have);

            } while (strm.avail_out == 0);

        } while (ret != Z_STREAM_END);

        // Clean-up
        inflateEnd(&strm);

        // Return
        return (ret == Z_STREAM_END);
    }
    else
        return(true);

}

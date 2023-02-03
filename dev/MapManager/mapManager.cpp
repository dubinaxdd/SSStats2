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

#define GZIP_WINDOWS_BIT 15 + 16
#define GZIP_CHUNK_SIZE 32 * 1024

MapManager::MapManager(QString ssPath, QObject *parent)
    : QObject(parent)
    , m_ssPath(ssPath)
{
     m_networkManager = new QNetworkAccessManager(this);

     getLocalMapFilesList();
     requestMapList();
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

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
        qWarning(logWarning()) << "Connection error:" << reply->errorString();
        reply->deleteLater();
    });
}

void MapManager::receiveMapList(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "Connection error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray replyByteArray = reply->readAll();

    reply->deleteLater();

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

        newMapItem.id = newObject.value("id").toString();
        newMapItem.authors = newObject.value("authors").toString();
        newMapItem.description = newObject.value("description").toString();
        newMapItem.size = newObject.value("size").toString();
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

   /* qDebug() << m_mapItemArray.at(0).id << "\n"
            << m_mapItemArray.at(0).authors << "\n"
            << m_mapItemArray.at(0).description << "\n"
            << m_mapItemArray.at(0).size << "\n"
            << m_mapItemArray.at(0).webPage << "\n"
            << m_mapItemArray.at(0).mapName << "\n"
            << m_mapItemArray.at(0).modContentHash << "\n"
            << m_mapItemArray.at(0).unpackedSize << "\n"
            << m_mapItemArray.at(0).packedSize << "\n"
            << m_mapItemArray.at(0).type << "\n";*/

    for(int i = 0; i < m_mapItemArray.count(); i++)
        requestMapInfo( &m_mapItemArray[i] );
}

void MapManager::requestMapInfo(MapItem *mapItem)
{
    QString url = getUrl(mapItem->modContentHash);

    QNetworkRequest newRequest = QNetworkRequest(QUrl(url));
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveMapInfo(reply, mapItem);
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
        qWarning(logWarning()) << "Connection error:" << reply->errorString();
        reply->deleteLater();
        m_requestetMapInfoCount++;
    });
}

void MapManager::receiveMapInfo(QNetworkReply *reply, MapItem *mapItem)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "Connection error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray replyByteArray = reply->readAll();
    reply->deleteLater();

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

    m_requestetMapInfoCount++;

    emit sendMapItem(mapItem);

    if(m_requestetMapInfoCount == m_mapItemArray.count())
        m_blockInfoUpdate = false;
}

void MapManager::requestFile(QString fileName, QString fileHash)
{
    QString url = getUrl(fileHash);

    QNetworkRequest newRequest = QNetworkRequest(QUrl(url));
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveFile(reply, fileName);
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
        qWarning(logWarning()) << "Connection error:" << reply->errorString();
        reply->deleteLater();
    });
}

void MapManager::receiveFile(QNetworkReply *reply, QString fileName)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "Connection error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QByteArray replyByteArray = reply->readAll();
    reply->deleteLater();

    QByteArray uncompressedByteArray;

    if (!uncompressGz(replyByteArray, uncompressedByteArray))
        return;

    QFile newFile( m_ssPath + "\\DXP2\\Data\\Scenarios\\mp" + QDir::separator() + fileName );
    if( newFile.open( QIODevice::WriteOnly ) ) {
        newFile.write(uncompressedByteArray);
        newFile.close();
    }
}

void MapManager::getLocalMapFilesList()
{
    QDir dir(m_ssPath + "\\DXP2\\Data\\Scenarios\\mp");
    QFileInfoList dirContent = dir.entryInfoList(QDir::Files);

    for (int i = 0; i < dirContent.count(); i++)
    {

        MapFileHash newMapFileHash;
        newMapFileHash.fileName = dirContent.at(i).fileName();

        QFile file(dirContent.at(i).filePath());

        if (file.open(QFile::ReadOnly))
        {
            QCryptographicHash hash(QCryptographicHash::Algorithm::Sha256);

            if (hash.addData(&file))
                newMapFileHash.hash = hash.result().toBase64();
        }

        m_localMapFilesHashes.append(newMapFileHash);
    }
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
                    needUpdate = true;

                continue;
            }
        }
    }

    if (foundedFiles < mapItem->filesList.count())
        needUpdate = true;

    mapItem->needUpdate = needUpdate;
    mapItem->needInstall = needInstall;
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

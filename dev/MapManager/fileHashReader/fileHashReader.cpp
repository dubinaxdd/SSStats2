#include "fileHashReader.h"
#include <QDir>
#include <QCryptographicHash>
#include <QDebug>

FileHashReader::FileHashReader(QObject *parent)
    : QObject(parent)
{

}

void FileHashReader::getLocalMapFilesList(QString gamePath)
{
    QList<MapFileHash> localMapFilesHashes;

    if(gamePath.isEmpty())
        return;

    QDir dir(gamePath);
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

        localMapFilesHashes.append(newMapFileHash);
    }

    emit sendLocalMapFilesList(localMapFilesHashes);
}

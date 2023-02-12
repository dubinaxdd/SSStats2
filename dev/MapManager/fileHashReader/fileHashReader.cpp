#include "fileHashReader.h"
#include <QDir>
#include <QCryptographicHash>

FileHashReader::FileHashReader(QString ssPath, QObject *parent)
    : QObject(parent)
    , m_ssPath(ssPath)
{

}

void FileHashReader::getLocalMapFilesList()
{
    QList<MapFileHash> localMapFilesHashes;

    if(m_ssPath.isEmpty())
        return;

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

        localMapFilesHashes.append(newMapFileHash);
    }

    emit sendLocalMapFilesList(localMapFilesHashes);
}

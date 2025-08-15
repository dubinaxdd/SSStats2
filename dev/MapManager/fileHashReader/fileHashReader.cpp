#include "fileHashReader.h"
#include <QDir>
#include <QCryptographicHash>

FileHashReader::FileHashReader(GamePath *currentGame, QObject *parent)
    : QObject(parent)
    , m_currentGame(currentGame)
{

}

void FileHashReader::getLocalMapFilesList()
{
    QList<MapFileHash> localMapFilesHashes;

    if(m_currentGame->gamePath.isEmpty())
        return;

    QString path;

    if (m_currentGame->gameType == DefinitiveEdition)
        path = m_currentGame->gamePath + "\\DoWDE\\Data\\Scenarios\\mp";
    else
        path = m_currentGame->gamePath + "\\DXP2\\Data\\Scenarios\\mp";

    QDir dir(path);
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

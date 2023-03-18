#include "asyncReplayReader.h"
#include <QFile>
#include <QDir>
#include <repreader.h>

using namespace ReplayReader;

AsyncReplayReader::AsyncReplayReader(QObject *parent)
    : QObject(parent)
{

}

void AsyncReplayReader::readReplaysList(QString playbackFolder, QString findText)
{
    QDir dir(playbackFolder);
    QString path = dir.absolutePath();
    QFileInfoList dirContent = dir.entryInfoList(QStringList() << "*.rec", QDir::Files | QDir::NoDotAndDotDot);

    if (dirContent.count() == 0)
        return;

    QVector<ReplayListInfo> replaysInfo;

    for (int i = 0; i < dirContent.count(); i++)
    {
        if (m_abort)
            return;

        QString fileName = dirContent.at(i).fileName();

        if(fileName == "temp.rec")
            continue;

        RepReader newRepReader(path + QDir::separator() + fileName);

        if (!newRepReader.isValide())
            continue;

        ReplayListInfo newReplayInfo;

        newReplayInfo.name = newRepReader.replay.Name;

        QString mapSourceUrl = "qrc:/maps/resources/maps/" + newRepReader.replay.Map.toLower() + ".jpg";;
        QFile checkFile(mapSourceUrl.right(mapSourceUrl.count() - 3));

        if(!checkFile.exists())
            mapSourceUrl = "qrc:/maps/resources/maps/default.jpg";

        newReplayInfo.map = newRepReader.replay.Map.toUpper();
        newReplayInfo.mapUrl = mapSourceUrl;
        newReplayInfo.mod = newRepReader.replay.MOD;
        newReplayInfo.fileName = fileName;

        QFileInfo replayFile(path + QDir::separator() + fileName);

        if(!replayFile.exists())
            continue;

        newReplayInfo.time = replayFile.birthTime();

        bool isContainsFindText = false;

        if(newReplayInfo.name.toUpper().contains(findText.toUpper())
                || newReplayInfo.map.toUpper().contains(findText.toUpper())
                || newReplayInfo.mod.toUpper().contains(findText.toUpper())
                || newReplayInfo.fileName.toUpper().contains(findText.toUpper())
                || newReplayInfo.time.toString("dd.MM.yyyy hh:mm").contains(findText.toUpper())
                )
            isContainsFindText = true;


        QList<Player*> players = newRepReader.replay.Players;

        for(int i = 0; i < players.count(); i++)
        {
            if (players[i]->Name.toUpper().contains(findText.toUpper()))
                isContainsFindText = true;
        }

        if(isContainsFindText)
            replaysInfo.append(newReplayInfo);
    }

    emit sendReplaysInfo(replaysInfo);
}

void AsyncReplayReader::abort()
{
    m_mutex.lock();
    m_abort = true;
    m_mutex.unlock();
}

#include "replayManager.h"
#include <QDir>
#include <QDebug>
#include <baseTypes.h>
#include <repreader.h>

using namespace ReplayReader;

ReplayManager::ReplayManager(QObject *parent)
    : QObject(parent)
    , m_replaysListModel( new ReplaysListModel(this))
{
    emit replaysListModelSetded();
}

void ReplayManager::setSsPath(const QString &newSsPath)
{
    m_ssPath = newSsPath;

    getReplaysDirs();
}

void ReplayManager::getReplaysDirs()
{
    QDir dir(m_ssPath + QDir::separator() + "playback");

    QString path = dir.absolutePath();

    QFileInfoList dirContent = dir.entryInfoList(QStringList() << "*.rec", QDir::Files | QDir::NoDotAndDotDot);

    QVector<ReplayListInfo> replaysInfo;

    for (int i = 0; i < dirContent.count(); i++)
    {
        QString folderName = dirContent.at(i).fileName();

        RepReader newRepReader(path + QDir::separator() + folderName);

        ReplayListInfo newReplayInfo;

        newReplayInfo.name = newRepReader.replay.Name;
        newReplayInfo.map = newRepReader.replay.Map;
        newReplayInfo.mod = newRepReader.replay.MOD;
        newReplayInfo.folderName = folderName;

        replaysInfo.append(newReplayInfo);
    }

    m_replaysListModel->setReplaysList(std::move(replaysInfo));
}

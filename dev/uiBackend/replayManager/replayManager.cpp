#include "replayManager.h"
#include <QDir>
#include <QDebug>
#include <baseTypes.h>
#include <repreader.h>
#include <QDesktopServices>
#include <QUrl>
#include <QFile>

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

void ReplayManager::openPlaybackFolder()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_ssPath + QDir::separator() + "Playback"));
}

void ReplayManager::openPlayback(QString fileName)
{
    QDir dir(m_ssPath + QDir::separator() + "Playback");
    QString path = dir.absolutePath();

    QFileInfo replayFile(path + QDir::separator() + fileName);

    if(!replayFile.exists())
        return;

    m_currentReplayTime = replayFile.birthTime().toString("dd.MM.yyyy hh:mm");

    RepReader newRepReader(path + QDir::separator() + fileName);

    m_currentReplayName = newRepReader.replay.Name;
    m_currentFileName = fileName;
    m_currentMod = newRepReader.replay.MOD;
    m_currentModVersion = newRepReader.replay.Version;
    m_currentMap = newRepReader.replay.Map;

    int duration = newRepReader.replay.Duration;

    QString minutes = QString::number(duration / 60);
    QString secs = QString::number(duration - ((duration / 60) * 60));

    if(secs.count() == 1)
        secs = "0" + secs;

    m_currentDuration = minutes + ":" + secs;
    m_currentPlayerCount = newRepReader.replay.PlayerCount;
    m_currentTeamsCount = newRepReader.replay.getTeamsCount();
    m_currentMapSize = newRepReader.replay.MapSize;

    m_mapSourceUrl = "qrc:/maps/resources/maps/" + m_currentMap.toLower() + ".jpg";

    QFile checkFile(m_mapSourceUrl.right(m_mapSourceUrl.count() - 3));

    if(!checkFile.exists())
        m_mapSourceUrl = "qrc:/maps/resources/maps/default.jpg";


    emit updateReplayInfo();
}

void ReplayManager::getReplaysDirs()
{
    QDir dir(m_ssPath + QDir::separator() + "Playback");
    QString path = dir.absolutePath();

    QFileInfoList dirContent = dir.entryInfoList(QStringList() << "*.rec", QDir::Files | QDir::NoDotAndDotDot);

    if (dirContent.count() == 0)
        return;

    QVector<ReplayListInfo> replaysInfo;

    for (int i = 0; i < dirContent.count(); i++)
    {
        QString fileName = dirContent.at(i).fileName();

        RepReader newRepReader(path + QDir::separator() + fileName);

        ReplayListInfo newReplayInfo;

        newReplayInfo.name = newRepReader.replay.Name;

        QString mapSourceUrl = "qrc:/maps/resources/maps/" + newRepReader.replay.Map.toLower() + ".jpg";

        QFile checkFile(m_mapSourceUrl.right(m_mapSourceUrl.count() - 3));

        if(!checkFile.exists())
            m_mapSourceUrl = "qrc:/maps/resources/maps/default.jpg";

        newReplayInfo.mapUrl = mapSourceUrl;
        newReplayInfo.mod = newRepReader.replay.MOD;
        newReplayInfo.fileName = fileName;

        replaysInfo.append(newReplayInfo);

        if (i == 0)
            openPlayback(fileName);
    }

    m_replaysListModel->setReplaysList(std::move(replaysInfo));
}

#include "replayManager.h"
#include <QDir>
#include <QDebug>
#include <baseTypes.h>
#include <repreader.h>

using namespace ReplayReader;

ReplayManager::ReplayManager(QObject *parent) : QObject(parent)
{

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

    for (int i = 0; i < dirContent.count(); i++)
    {
        qDebug() << dirContent.at(i).fileName() << dirContent.at(i);

      //  RepReader(dirContent.at(i));
    }
}

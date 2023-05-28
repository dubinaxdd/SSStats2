#include "BalanceModInstaller.h"
#include <QFile>
#include <QDataStream>
#include <JlCompress.h>
#include <QDebug>

BalanceModInstaller::BalanceModInstaller(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<InstallModData>("InstallModData");
}

void BalanceModInstaller::installMod(InstallModData data)
{
    QFile newFile(data.filePath);

    if( newFile.open( QIODevice::WriteOnly ) ) {
        QDataStream stream( &newFile );
        stream << data.modByteArray;
        newFile.close();

        JlCompress::extractDir(data.filePath, data.decompressPath);
        newFile.remove();

        QFile hotKeyPath(data.hotKeysPath);

        if (hotKeyPath.exists())
        {
            QDir dir(data.ssPath + "\\Profiles\\");
            QFileInfoList dirContent = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

            for (int i = 0; i < dirContent.count(); i++)
            {
                QDir().mkdir(dirContent.at(i).absoluteFilePath() + QDir::separator() + data.modTechnicalName.toLower());
                QString profilePath = dirContent.at(i).absoluteFilePath() + QDir::separator() + data.modTechnicalName.toLower() + QDir::separator() + "KEYDEFAULTS.LUA";
                QFile::copy(data.hotKeysPath, profilePath);
            }
        }

        QDir schemesDir = data.schemesPath;

        if (schemesDir.exists())
        {
             QFileInfoList schemesContent = schemesDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
             QDir dir(data.ssPath + "\\Profiles\\");

             QFileInfoList dirContent = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

             for (int i = 0; i < schemesContent.count(); i++)
             {
                  for (int j = 0; j < dirContent.count(); j++)
                  {
                      QDir().mkdir(dirContent.at(j).absoluteFilePath() + QDir::separator() + data.modTechnicalName.toLower() + QDir::separator() + "schemes");
                      QString copyPath = dirContent.at(j).absoluteFilePath() + QDir::separator() + data.modTechnicalName.toLower() + QDir::separator() + "schemes" + QDir::separator() + schemesContent.at(i).fileName();
                      QFile::copy(schemesContent.at(i).absoluteFilePath(), copyPath);
                  }
             }
        }

        emit modInstalled(data.modTechnicalName);
    }
}

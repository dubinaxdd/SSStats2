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

        emit modInstalled(data.modTechnicalName);
    }
}

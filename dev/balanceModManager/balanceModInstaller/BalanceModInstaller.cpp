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

        QString hotKeyPath = data.templateProfilePath + QDir::separator() + "KEYDEFAULTS.LUA";
        QString schemesPath = data.templateProfilePath + QDir::separator() + "schemes";

        QFile hotKeyFile(hotKeyPath);

        //Копируем хоткеи
        if (hotKeyFile.exists())
        {
            QDir dir(data.ssPath + "\\Profiles\\");
            QFileInfoList dirContent = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

            for (int i = 0; i < dirContent.count(); i++)
            {
                QDir().mkdir(dirContent.at(i).absoluteFilePath() + QDir::separator() + data.modTechnicalName.toLower());
                QString profilePath = dirContent.at(i).absoluteFilePath() + QDir::separator() + data.modTechnicalName.toLower() + QDir::separator() + "KEYDEFAULTS.LUA";
                QFile::copy(hotKeyPath, profilePath);
            }
        }

        //Копируем раскраски
        QDir schemesDir = schemesPath;

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

        //Отключаем туториал
        QDir dir(data.ssPath + "\\Profiles\\");
        QFileInfoList dirContent = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

        for (int i = 0; i < dirContent.count(); i++)
        {
            QFile playercfgFile(dirContent.at(i).absoluteFilePath() + QDir::separator() + "playercfg.lua");

            if (playercfgFile.open(QFile::ReadWrite | QIODevice::Text))
            {
                QString configString = QString::fromStdString(playercfgFile.readAll().toStdString());

                if (configString. contains("Tutorial_" + data.modTechnicalName.toLower()))
                {
                    playercfgFile.close();
                    continue;
                }

                configString.replace("\0", "");

                configString.append("\nTutorial_" + data.modTechnicalName.toLower() + "  =  \n"
                                    "{\n"
                                    "	HasClickedMulti = true,\n"
                                    "	HasClickedSkirmish = true,\n"
                                    "	HasPlayedTutorial = true,\n"
                                    "}\0");

                playercfgFile.resize(0);
                playercfgFile.write(configString.toLatin1());
                playercfgFile.close();
            }
        }

        emit modInstalled(data.modTechnicalName);
    }
}

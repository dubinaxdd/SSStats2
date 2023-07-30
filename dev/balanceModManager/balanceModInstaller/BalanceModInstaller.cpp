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

    if( !newFile.open( QIODevice::WriteOnly ) )
    {
        newFile.remove();
        emit modInstallError(data.modTechnicalName);
        return;
    }

    QDataStream stream( &newFile );
    stream << data.modByteArray;
    newFile.close();

    if (JlCompress::getFileList(data.filePath).isEmpty())
    {
        newFile.remove();
        emit modInstallError(data.modTechnicalName);
        return;
    }

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

            if (configString.contains("Tutorial_" + data.modTechnicalName.toLower()))
            {
                playercfgFile.close();
                continue;
            }

            configString.replace("\0", "");

            if (!configString.contains("ai_preferences1"))
            {
                configString.append(
                            "\nai_preferences1 =  \n"
                            "{\n"
                            "}\n"
                                );

            }

            configString.append("\nTutorial_" + data.modTechnicalName.toLower() + "  =  \n"
                                "{\n"
                                "	HasClickedMulti = true,\n"
                                "	HasClickedSkirmish = true,\n"
                                "	HasPlayedTutorial = true,\n"
                                "}\n"
                                + data.modTechnicalName.toLower() + "_game_preferences =  \n"
                                "{\n"
                                "	num_teams = 2,\n"
                                "	options = \n"
                                "	{\n"
                                "		option0 =  \n"
                                "		{\n"
                                "			choice = \"4\",\n"
                                "			id = \"1095320646\",\n"
                                "		},\n"
                                "		option1 = \n"
                                "		{\n"
                                "			choice = \"0\",\n"
                                "			id = \"1381192532\",\n"
                                "		},\n"
                                "		option2 =  \n"
                                "		{\n"
                                "			choice = \"0\",\n"
                                "			id = \"1280005197\",\n"
                                "		},\n"
                                "		option3 = \n"
                                "		{\n"
                                "			choice = \"1\",\n"
                                "			id = \"1128809793\",\n"
                                "		},\n"
                                "		option4 =  \n"
                                "		{\n"
                                "			choice = \"0\",\n"
                                "			id = \"1397509955\",\n"
                                "		},\n"
                                "		option5 =\n"
                                "		{\n"
                                "			choice = \"2\",\n"
                                "			id = \"1196642372\",\n"
                                "		},\n"
                                "		option6 = \n"
                                "		{\n"
                                "			choice = \"1\",\n"
                                "			id = \"1381192520\",\n"
                                "		},\n"
                                "		option7 = \n"
                                "		{\n"
                                "			choice = \"1\",\n"
                                "			id = \"1381192276\",\n"
                                "		},\n"
                                "	},\n"
                                "	team_type = 3,\n"
                                "	win_conditions =  \n"
                                "	{\n"
                                "		win_condition1 =  \n"
                                "		{\n"
                                "			id = \"1003066394\",\n"
                                "		},\n"
                                "		win_condition2 =  \n"
                                "		{\n"
                                "			id = \"735076042\",\n"
                                "		},\n"
                                "		win_condition3 =  \n"
                                "		{\n"
                                "			id = \"69421273\",\n"
                                "		},\n"
                                "		win_condition4 =  \n"
                                "		{\n"
                                "			id = \"1959084950\",\n"
                                "		},\n"
                                "	},\n"
                                "}\n"
                                "\0");

            playercfgFile.resize(0);
            playercfgFile.write(configString.toLatin1());
            playercfgFile.close();
        }
    }

    emit modInstalled(data.modTechnicalName);

}

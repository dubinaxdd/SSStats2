#include "BalanceModInstaller.h"
#include <QFile>
#include <QDataStream>
#include <JlCompress.h>
#include <QDebug>
#include <logger.h>

BalanceModInstaller::BalanceModInstaller(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<InstallModData>("InstallModData");
}

void BalanceModInstaller::installMod(InstallModData data, bool overwriteProfiles)
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

    QString templateHotKeyPath = data.templateProfilePath + QDir::separator() + "KEYDEFAULTS.LUA";
    QFile hotKeyFile(templateHotKeyPath);

    if (overwriteProfiles)
    {
        QDir dir(data.profilePath + "\\Profiles\\");
        QFileInfoList dirContent = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

        for (int i = 0; i < dirContent.count(); i++)
        {
            QDir removeDir(dirContent.at(i).absoluteFilePath() + QDir::separator() + data.modTechnicalName.toLower());
            removeDir.removeRecursively();
        }

        //Копируем хоткеи
        if (hotKeyFile.exists())
        {
            QDir dir(data.profilePath + "\\Profiles\\");
            QFileInfoList dirContent = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

            for (int i = 0; i < dirContent.count(); i++)
                installHotKeys(dirContent.at(i).absoluteFilePath() + QDir::separator() + data.modTechnicalName.toLower()
                               , templateHotKeyPath
                               , data.profilePath + QDir::separator() + data.modTechnicalName + QDir::separator() + "missing_hotkeys.lua");
        }

        //Копируем раскраски
        QDir templateSchemesDir = data.templateProfilePath + QDir::separator() + "schemes";

        if (templateSchemesDir.exists())
        {
             QFileInfoList schemesContent = templateSchemesDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
             QDir dir(data.profilePath + "\\Profiles\\");

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
    }

    //Отключаем туториал
    QDir dir(data.profilePath + "\\Profiles\\");
    QFileInfoList dirContent = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (int i = 0; i < dirContent.count(); i++)
    {
        QFile playercfgFile(dirContent.at(i).absoluteFilePath() + QDir::separator() + "playercfg.lua");

        if (playercfgFile.open(QFile::ReadWrite | QIODevice::Text))
        {
            QString configString = QString::fromStdString(playercfgFile.readAll().toStdString());

            configString.replace("\0", "");

            QString modName;

            if (data.gameType == GameType::GameTypeEnum::DefinitiveEdition)
                modName = data.modTechnicalName;
            else
                data.modTechnicalName.toLower();

            //Удалем предыдущую запись в настройках о туториале
            if (configString.contains("Tutorial_" + modName))
            {
                int startIndex = configString.indexOf("Tutorial_" + modName);

                for (int j = startIndex; j < configString.count(); j++)
                {
                    if(configString.at(j) == '}')
                    {
                        configString.remove(startIndex, j - startIndex + 1);
                        break;
                    }
                }
            }

            //Устанавливаем новые настройки туториала
            configString.append("\nTutorial_" + modName + "  =  \n"
                                "{\n"
                                "    HasClickedCampaign = true,\n"
                                "	HasClickedMulti = true,\n"
                                "	HasClickedSkirmish = true,\n"
                                "	HasPlayedTutorial = true,\n"
                                "}\n");

            if (!configString.contains("ai_preferences1"))
            {
                configString.append(
                            "\nai_preferences1 =  \n"
                            "{\n"
                            "}\n"
                                );

            }

            //Удаляем предыдущю запись в настрйоках о настройках пати
            if (configString.contains(modName + "_game_preferences ="))
            {
                int startIndex = configString.indexOf(modName + "_game_preferences =");
                int needCounter = 0;
                for (int j = startIndex; j < configString.count(); j++)
                {
                    if (configString.at(j) == '}')
                        needCounter++;

                    if (configString.at(j) == '{')
                        needCounter = 0;

                    if (needCounter == 3)
                    {
                        configString.remove(startIndex, j - startIndex + 1);
                        break;
                    }
                }
            }




            configString.append("\n" + modName + "_game_preferences =  \n"
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

void BalanceModInstaller::uninstallMod(QString gamePath, QString modTechnicalName)
{
    QDir modDir(gamePath + QDir::separator() + modTechnicalName);
    modDir.removeRecursively();

    QFile moduleFile(gamePath + QDir::separator() + modTechnicalName + ".module");
    moduleFile.remove();

    emit modUninstalled(modTechnicalName);
}

void BalanceModInstaller::updateHotKeysOnMod(QString modTechnicalName, QString gameSettingsPath)
{
    QDir dir(gameSettingsPath + "\\Profiles\\");
    QFileInfoList dirContent = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    QString missingHotkeysPath = gameSettingsPath + QDir::separator() + modTechnicalName + QDir::separator() + "missing_hotkeys.lua";

    bool result = true;

    for (int i = 0; i < dirContent.count(); i++)
    {
        QString hotkeysPath = dirContent.at(i).absoluteFilePath() + QDir::separator() + modTechnicalName + QDir::separator() + "KEYDEFAULTS.LUA";
        if (!updateHotkeys(hotkeysPath, missingHotkeysPath))
            result = false;
    }

    emit hotKeysUpdated(modTechnicalName, result);
}

void BalanceModInstaller::installHotKeys(QString hotkeysDir, QString templateHotKeyPath,  QString missingHotKeysPath)
{
    //Копируем хоткеи в новый профиль из шаблонного профиля
    QDir().mkdir(hotkeysDir);
    QString hotkeysPath = hotkeysDir + QDir::separator() + "KEYDEFAULTS.LUA";
    QFile::copy(templateHotKeyPath, hotkeysPath);

    updateHotkeys(hotkeysPath, missingHotKeysPath);
}

bool BalanceModInstaller::updateHotkeys(QString hotkeysPath, QString missingHotKeysPath)
{
    //Модифицируем файл с хоткеями недостающими значениями для новой версии мода
    QFile hotkeyFile(hotkeysPath);
    QFile missingHotKeysFile(missingHotKeysPath);

    if (!hotkeyFile.open(QFile::OpenModeFlag::ReadWrite))
    {
        qWarning(logWarning()) << "BalanceModInstaller::installHotKeys" << hotkeysPath << "Hotkeys file not openned!";
        return false;
    }

    if (!missingHotKeysFile.open(QFile::OpenModeFlag::ReadOnly))
    {
        qWarning(logWarning()) << "BalanceModInstaller::installHotKeys" << missingHotKeysPath << "Missing_hotkeeys file not openned!";
        return false;
    }

    QString hotkeys = hotkeyFile.readAll();
    QString missingHotkeys = missingHotKeysFile.readAll();
    hotkeyFile.resize(0);
    missingHotKeysFile.close();

    QVector<QString> missingHotkeysVector;

    //Парсим новые хоткеи
    int startStringIndex = 0;
    for (int i = 0; i < missingHotkeys.count(); i++)
    {
        if (missingHotkeys.at(i) == '\n')
        {
            missingHotkeysVector.append(missingHotkeys.mid(startStringIndex, i - startStringIndex));
            startStringIndex = i+1;
        }

        if(i == missingHotkeys.count() - 1)
        {
            missingHotkeysVector.append(missingHotkeys.mid(startStringIndex, i - startStringIndex + 1));
            startStringIndex = i+2;
        }
    }

    int insertIndex = 0;

    //Вставляем хэдер
    QString headerString = "-- BALANCE MODE NEW HOTKEYS";


    if (hotkeys.contains(headerString))
        insertIndex = hotkeys.indexOf(headerString) + headerString.count();
    else
    {
        QString bindingString = "bindings =\r\n{";

        //Говнокодим хотфикс
        if(!hotkeys.contains(bindingString))
        {
            bindingString = "bindings =\n{";
            if(!hotkeys.contains(bindingString))
            {
                bindingString = "bindings=\n{";
                if(!hotkeys.contains(bindingString))
                {
                    bindingString = "bindings={";
                    if(!hotkeys.contains(bindingString))
                    {
                        bindingString = "bindings= {";
                        if(!hotkeys.contains(bindingString))
                        {
                            bindingString = "bindings= \n{";
                            if(!hotkeys.contains(bindingString))
                            {
                                bindingString = "bindings= \r\n{";
                                if(!hotkeys.contains(bindingString))
                                {
                                    bindingString = "bindings ={";
                                    if(!hotkeys.contains(bindingString))
                                    {
                                        qWarning(logWarning()) << "BalanceModInstaller::installHotKeys" << "Failed to find bindings ={";
                                        hotkeyFile.close();
                                        return  false;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        insertIndex = hotkeys.indexOf(bindingString) + bindingString.count();

        if (!(insertIndex < hotkeys.count()))
        {
            qWarning(logWarning()) << "BalanceModInstaller::installHotKeys" << "Failed to insert hotkey file header, insertion index out of file range.";
            hotkeyFile.close();
            return false;
        }

        headerString = "\n\t" + headerString;

        hotkeys.insert(insertIndex, headerString);
        insertIndex += headerString.count();
    }

    //Вставляем хоткеи
    bool oldModHotkeysFinded = false;

    for (auto& needHotkeyString : missingHotkeysVector)
    {
        QString hotKeyId = needHotkeyString.mid(0,needHotkeyString.indexOf(" = "));

        if (hotkeys.contains(hotKeyId))
        {
            oldModHotkeysFinded = true;
            continue;
        }

        needHotkeyString = "\n\t" + needHotkeyString;

        hotkeys.insert(insertIndex, needHotkeyString);
        insertIndex += needHotkeyString.count();
    }

    if (!oldModHotkeysFinded)
        hotkeys.insert(insertIndex, "\n");

    //Перезаписываем файл хоткеев
    hotkeyFile.write(hotkeys.toLatin1());
    hotkeyFile.close();

    return true;
}

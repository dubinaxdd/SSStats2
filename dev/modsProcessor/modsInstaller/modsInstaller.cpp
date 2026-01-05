#include "modsInstaller.h"
#include "QFile"
#include "JlCompress.h"
#include "QDir"
#include "QDebug"

ModsInstaller::ModsInstaller(GamePath* gamePath, QObject *parent)
    : QObject(parent)
    ,m_currentGame(gamePath)
{
}

void ModsInstaller::installMod(InstMod mod, QString path)
{
    switch(mod){
        case InstMod::RussianFonts : installRussianFonts(path); break;
        case InstMod::CameraMod : installCameraMod(path); break;
        case InstMod::CameraModDE : installCameraMod(path); break;
        case InstMod::GridHotkeys : installGridHotkeys(path); break;
        case InstMod::TransparentCameraTrapezoid : installTransparentCameraTrapezoid(path); break;
    }

    emit modInstalled(mod);
}

void ModsInstaller::uninstallMod(InstMod mod)
{
    switch (mod){
        case InstMod::RussianFonts : uninstallRussianFonts(); break;
        case InstMod::CameraMod : uninstallCameraMod(); break;
        case InstMod::CameraModDE : uninstallCameraMod(); break;
        case InstMod::GridHotkeys : uninstallGridHotkeys(); break;
        case InstMod::TransparentCameraTrapezoid : uninstallTransparentCameraTrapezoid(); break;
    }
}


void ModsInstaller::installRussianFonts(QString path)
{
    if (m_currentGame->gameType == GameType::GameTypeEnum::DefinitiveEdition)
        return;

    JlCompress::extractDir(path, m_currentGame->gamePath + QDir::separator());
    qInfo(logInfo()) <<  "Russian fonts installed from " << path << "to" << m_currentGame->gamePath;

    QFile tempfile(path);
    tempfile.remove();
}

void ModsInstaller::uninstallRussianFonts()
{
    if (m_currentGame->gameType == GameType::GameTypeEnum::DefinitiveEdition)
        return;

    QDir dir1(m_currentGame->gamePath + "\\Engine\\Locale\\English\\data\\art");
    dir1.removeRecursively();

    QDir dir2(m_currentGame->gamePath + "\\Engine\\Locale\\English\\data\\font");
    dir2.removeRecursively();

    QDir dir3(m_currentGame->gamePath + "\\Engine\\Locale\\English\\data\\sound");
    dir3.removeRecursively();


    qInfo(logInfo()) <<  "Russian fonts uninstalled";
}

void ModsInstaller::installCameraMod(QString path)
{
    JlCompress::extractDir(path, m_currentGame->gamePath + QDir::separator());
    qInfo(logInfo()) <<  "Camera mod installed from " << path << "to" << m_currentGame->gamePath;

    QFile tempfile(path);
    tempfile.remove();
}

void ModsInstaller::uninstallCameraMod()
{
    QFile tempfile1(m_currentGame->gamePath + "\\Engine\\Data\\camera_ed.lua");
    tempfile1.remove();

    QFile tempfile2(m_currentGame->gamePath + "\\Engine\\Data\\camera_high.lua");
    tempfile2.remove();

    QFile tempfile3(m_currentGame->gamePath + "\\Engine\\Data\\camera_low.lua");
    tempfile3.remove();

    QFile tempfile4(m_currentGame->gamePath + "\\Engine\\Data\\camera_me.lua");
    tempfile4.remove();
}

void ModsInstaller::installGridHotkeys(QString path)
{
    if (m_currentGame->gameType == GameType::GameTypeEnum::DefinitiveEdition)
        return;

    QDir dir(m_currentGame->gameSettingsPath + "\\Profiles\\");
    QFileInfoList dirContent = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (int i = 0; i < dirContent.count(); i++)
    {
        QString profilePath = dirContent.at(i).absoluteFilePath() + QDir::separator() + "dxp2" + QDir::separator();
        JlCompress::extractDir(path, profilePath);
        qInfo(logInfo()) <<  "Grid hotkeys installed from " << path << "to" << profilePath;
    }

    QFile tempfile(path);
    tempfile.remove();
}

void ModsInstaller::uninstallGridHotkeys()
{
    if (m_currentGame->gameType == GameType::GameTypeEnum::DefinitiveEdition)
        return;

    QDir dir(m_currentGame->gameSettingsPath + "\\Profiles\\");
    QFileInfoList dirContent = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (int i = 0; i < dirContent.count(); i++)
    {
        QString path = dirContent.at(i).absoluteFilePath() + "\\dxp2\\KEYDEFAULTS.LUA";

        QFile tempfile1(path);
        tempfile1.remove();

        qInfo(logInfo()) <<  "Grid hotkeys uninstalled from " << path;
    }
}

void ModsInstaller::installTransparentCameraTrapezoid(QString path)
{
    JlCompress::extractDir(path, m_currentGame->gamePath + QDir::separator());

    qInfo(logInfo()) <<  "Transparent camera trapezoid installed from " << path << "to" << m_currentGame->gameSettingsPath;

    QFile tempfile(path);
    tempfile.remove();
}

void ModsInstaller::uninstallTransparentCameraTrapezoid()
{
    QFile tempfile1(m_currentGame->gamePath + "\\DXP2\\Data\\Art\\ui\\minimap\\camera.tga");
    tempfile1.remove();
    qInfo(logInfo()) <<  "Transparent camera trapezoid uninstalled";
}

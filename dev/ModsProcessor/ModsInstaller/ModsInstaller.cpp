#include "modsInstaller.h"
#include "QFile"
#include "JlCompress.h"
#include "QDir"
#include "QDebug"

ModsInstaller::ModsInstaller(QString ssPath, QObject *parent) : QObject(parent)
{
    m_ssPath = ssPath;

    //QuaZip zip("zipFile.zip");
   // zip.open(QuaZip::mdUnzip);


    //QuaZipFile file(&zip);

   // JlCompress::extractDir("C:/test/test.zip", "C:/test/");
/*
    for(bool f=zip.goToFirstFile(); f; f=zip.goToNextFile()) {
        file.open(QIODevice::ReadOnly);
        //same functionality as QIODevice::readData() -- data is a char*, maxSize is qint64
        file.read(data,maxSize);
        //do something with the data
        file.close();
    }*/

    /*  zip.close();*/
}

void ModsInstaller::installMod(InstMod mod, QString path)
{
    switch(mod){
        case InstMod::RussianFonts : installRussianFonts(path); break;
        case InstMod::CameraMod : installCameraMod(path); break;
        case InstMod::GridHotkeys : installGridHotkeys(path); break;
    }

    emit modInstalled(mod);
}

void ModsInstaller::uninstallMod(InstMod mod)
{
    switch (mod){
        case InstMod::RussianFonts : uninstallRussianFonts();
        case InstMod::CameraMod : uninstallCameraMod();
        case InstMod::GridHotkeys : uninstallGridHotkeys();
    }
}


void ModsInstaller::installRussianFonts(QString path)
{
    JlCompress::extractDir(path, m_ssPath + QDir::separator());
    qInfo(logInfo()) <<  "Russian fonts installed from " << path << "to" << m_ssPath;

    QFile tempfile(path);
    tempfile.remove();
}

void ModsInstaller::uninstallRussianFonts()
{
    QDir dir1(m_ssPath + "\\Engine\\Locale\\English\\data\\art");
    dir1.removeRecursively();

    QDir dir2(m_ssPath + "\\Engine\\Locale\\English\\data\\font");
    dir2.removeRecursively();

    QDir dir3(m_ssPath + "\\Engine\\Locale\\English\\data\\sound");
    dir3.removeRecursively();


    qInfo(logInfo()) <<  "Russian fonts uninstalled";
}

void ModsInstaller::installCameraMod(QString path)
{
    JlCompress::extractDir(path, m_ssPath + QDir::separator());
    qInfo(logInfo()) <<  "Camera mod installed from " << path << "to" << m_ssPath;

    QFile tempfile(path);
    tempfile.remove();
}

void ModsInstaller::uninstallCameraMod()
{
    QFile tempfile1(m_ssPath + "\\Engine\\Data\\camera_ed.lua");
    tempfile1.remove();

    QFile tempfile2(m_ssPath + "\\Engine\\Data\\camera_high.lua");
    tempfile2.remove();

    QFile tempfile3(m_ssPath + "\\Engine\\Data\\camera_low.lua");
    tempfile3.remove();
}

void ModsInstaller::installGridHotkeys(QString path)
{

    QDir dir(m_ssPath + "\\Profiles\\");
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
    QDir dir(m_ssPath + "\\Profiles\\");
    QFileInfoList dirContent = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (int i = 0; i < dirContent.count(); i++)
    {
        QString path = dirContent.at(i).absoluteFilePath() + "\\dxp2\\KEYDEFAULTS.LUA";

        QFile tempfile1(path);
        tempfile1.remove();

        qInfo(logInfo()) <<  "Grid hotkeys uninstalled from " << path;
    }
}

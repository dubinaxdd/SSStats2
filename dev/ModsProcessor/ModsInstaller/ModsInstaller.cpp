#include "ModsInstaller.h"
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

void ModsInstaller::uninstalRussianFonts()
{
    QDir dir1(m_ssPath + "\\Engine\\Locale\\English\\data\\art");
    dir1.removeRecursively();

    QDir dir2(m_ssPath + "\\Engine\\Locale\\English\\data\\font");
    dir2.removeRecursively();

    QDir dir3(m_ssPath + "\\Engine\\Locale\\English\\data\\sound");
    dir3.removeRecursively();


    qInfo(logInfo()) <<  "Russian fonts uninstalled";
}

void ModsInstaller::installRussianFonts(QString path)
{
    JlCompress::extractDir(path, m_ssPath + QDir::separator());

    emit russianFontsInstalled();

    qInfo(logInfo()) <<  "Russian fonts installed from " << path << "to" << m_ssPath;

    QFile tempfile(path);
    tempfile.remove();
}

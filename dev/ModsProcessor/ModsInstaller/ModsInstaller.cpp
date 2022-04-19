#include "ModsInstaller.h"
#include "quazip.h"
#include "quazipfile.h"
#include "JlCompress.h"
#include "QDebug"

ModsInstaller::ModsInstaller(QObject *parent) : QObject(parent)
{
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

#include "BalanceModInstaller.h"
#include <QFile>
#include <QDataStream>
#include <JlCompress.h>

BalanceModInstaller::BalanceModInstaller(QObject *parent) : QObject(parent)
{

}

void BalanceModInstaller::installMod(QByteArray modByteArray, QString filePath, QString decompressPath, QString modTechnicalName)
{
    QFile newFile(filePath);

    if( newFile.open( QIODevice::WriteOnly ) ) {
        QDataStream stream( &newFile );
        stream << modByteArray;
        newFile.close();

        JlCompress::extractDir(filePath, decompressPath);
        newFile.remove();

        emit modInstalled(modTechnicalName);
    }
}

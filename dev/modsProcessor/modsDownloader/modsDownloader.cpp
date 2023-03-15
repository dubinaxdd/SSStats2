#include "modsDownloader.h"
#include <QNetworkReply>
#include <QDir>
#include <QDataStream>

#include "JlCompress.h"

ModsDownloader::ModsDownloader(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{

}

void ModsDownloader::downloadMod(InstMod mod)
{
    switch (mod){
        case InstMod::RussianFonts  : requestRussianFonts(); break;
        case InstMod::CameraMod     : requestCameraMod(); break;
        case InstMod::GridHotkeys   : requestGridHotkeys(); break;
        case InstMod::TransparentCameraTrapezoid   : requestTransparentCameraTrapezoid(); break;
    }
}

void ModsDownloader::requestRussianFonts()
{
    QNetworkRequest newRequest;

    QString urlString = "https://dowstats.ru/ssstats/Downloads/RussianFonts.zip";

    newRequest.setUrl(QUrl(urlString));
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveRussinFonts(reply);
    });

    QObject::connect(reply, &QNetworkReply::downloadProgress, this, [=](qint64 bytesReceived, qint64 bytesTotal){

        if(bytesTotal != 0)
        {
            emit downladProgress(InstMod::RussianFonts, bytesReceived * 100 / bytesTotal);
            qInfo(logInfo()) << "Russian fonts download progress:" << bytesReceived << "/" << bytesTotal;
        }
    });
}

void ModsDownloader::receiveRussinFonts(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "Connection error:" << reply->errorString();
        delete reply;
        emit downloadError(InstMod::RussianFonts);
        return;
    }

    QByteArray replyByteArray = reply->readAll();

    delete reply;

    saveRussianFonts(std::move(replyByteArray));

    qInfo(logInfo()) <<  "Russian fonts downloaded to " << QDir::currentPath() + QDir::separator() + "RussianFonts.zip";

    emit modDownloaded(InstMod::RussianFonts, QDir::currentPath() + QDir::separator() + "RussianFonts.zip");
}

void ModsDownloader::saveRussianFonts(QByteArray russianFontsByteArray)
{
    QFile newFile( QDir::currentPath() + QDir::separator() + "RussianFonts.zip" );
    if( newFile.open( QIODevice::WriteOnly ) ) {

        QDataStream stream( &newFile );
        stream << russianFontsByteArray;
        newFile.close();
    }
}

void ModsDownloader::requestCameraMod()
{
    QNetworkRequest newRequest;

    QString urlString = "https://dowstats.ru/ssstats/Downloads/CameraMod.zip";

    newRequest.setUrl(QUrl(urlString));
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveCameraMod(reply);
    });

    QObject::connect(reply, &QNetworkReply::downloadProgress, this, [=](qint64 bytesReceived, qint64 bytesTotal){

        if(bytesTotal != 0)
        {
            emit downladProgress(InstMod::CameraMod, bytesReceived * 100 / bytesTotal);
            qInfo(logInfo()) << "Camera mod download progress:" << bytesReceived << "/" << bytesTotal;
        }
    });
}

void ModsDownloader::receiveCameraMod(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "Connection error:" << reply->errorString();
        delete reply;
        emit downloadError(InstMod::CameraMod);
        return;
    }

    QByteArray replyByteArray = reply->readAll();

    delete reply;

    saveCameraMod(std::move(replyByteArray));

    qInfo(logInfo()) <<  "Camera mod downloaded to " << QDir::currentPath() + QDir::separator() + "CameraMod.zip";

    emit modDownloaded(InstMod::CameraMod, QDir::currentPath() + QDir::separator() + "CameraMod.zip");
}

void ModsDownloader::saveCameraMod(QByteArray cameraModByteArray)
{
    QFile newFile( QDir::currentPath() + QDir::separator() + "CameraMod.zip" );
    if( newFile.open( QIODevice::WriteOnly ) ) {

        QDataStream stream( &newFile );
        stream << cameraModByteArray;
        newFile.close();
    }
}

void ModsDownloader::requestGridHotkeys()
{
    QNetworkRequest newRequest;

    QString urlString = "https://dowstats.ru/ssstats/Downloads/Gridkeys.zip";

    newRequest.setUrl(QUrl(urlString));
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveGridHotkeys(reply);
    });

    QObject::connect(reply, &QNetworkReply::downloadProgress, this, [=](qint64 bytesReceived, qint64 bytesTotal){

        if(bytesTotal != 0)
        {
            emit downladProgress(InstMod::GridHotkeys, bytesReceived * 100 / bytesTotal);
            qInfo(logInfo()) << "Russian fonts download progress:" << bytesReceived << "/" << bytesTotal;
        }
    });
}

void ModsDownloader::receiveGridHotkeys(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "Connection error:" << reply->errorString();
        delete reply;
        emit downloadError(InstMod::GridHotkeys);
        return;
    }

    QByteArray replyByteArray = reply->readAll();

    delete reply;

    saveGridHotkeys(std::move(replyByteArray));

    qInfo(logInfo()) <<  "Grid hotkeys downloaded to " << QDir::currentPath() + QDir::separator() + "Gridkeys.zip";

    emit modDownloaded(InstMod::GridHotkeys, QDir::currentPath() + QDir::separator() + "Gridkeys.zip");
}

void ModsDownloader::saveGridHotkeys(QByteArray gridHotkeysByteArray)
{
    QFile newFile( QDir::currentPath() + QDir::separator() + "Gridkeys.zip" );
    if( newFile.open( QIODevice::WriteOnly ) ) {

        QDataStream stream( &newFile );
        stream << gridHotkeysByteArray;
        newFile.close();
    }
}

void ModsDownloader::requestTransparentCameraTrapezoid()
{
    QNetworkRequest newRequest;

    QString urlString = "https://dowstats.ru/ssstats/Downloads/TransparentCameraTrapezoid.zip";

    newRequest.setUrl(QUrl(urlString));
    QNetworkReply *reply = m_networkManager->get(newRequest);

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        receiveTransparentCameraTrapezoid(reply);
    });

    QObject::connect(reply, &QNetworkReply::downloadProgress, this, [=](qint64 bytesReceived, qint64 bytesTotal){

        if(bytesTotal != 0)
        {
            emit downladProgress(InstMod::TransparentCameraTrapezoid, bytesReceived * 100 / bytesTotal);
            qInfo(logInfo()) << "Transparent camera trapezoid mod download progress:" << bytesReceived << "/" << bytesTotal;
        }
    });
}

void ModsDownloader::receiveTransparentCameraTrapezoid(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "ModsDownloader::receiveTransparentCameraTrapezoid Connection error:" << reply->errorString();
        delete reply;
        emit downloadError(InstMod::TransparentCameraTrapezoid);
        return;
    }

    QByteArray replyByteArray = reply->readAll();

    delete reply;

    saveTransparentCameraTrapezoid(std::move(replyByteArray));

    qInfo(logInfo()) <<  "Transparent camera trapezoid to " << QDir::currentPath() + QDir::separator() + "TransparentCameraTrapezoid.zip";

    emit modDownloaded(InstMod::TransparentCameraTrapezoid, QDir::currentPath() + QDir::separator() + "TransparentCameraTrapezoid.zip");
}

void ModsDownloader::saveTransparentCameraTrapezoid(QByteArray transparentCameraTrapezoidByteArray)
{
    QFile newFile( QDir::currentPath() + QDir::separator() + "TransparentCameraTrapezoid.zip" );
    if( newFile.open( QIODevice::WriteOnly ) ) {

        QDataStream stream( &newFile );
        stream << transparentCameraTrapezoidByteArray;
        newFile.close();
    }
}

#include "ModsDownloader.h"
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

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
        reply->deleteLater();
    });
}

void ModsDownloader::receiveRussinFonts(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "Connection error:" << reply->errorString();
        reply->deleteLater();
        emit downloadError(InstMod::RussianFonts);
        return;
    }

    QByteArray replyByteArray = reply->readAll();

    reply->deleteLater();

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

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [=](){
        reply->deleteLater();
    });
}

void ModsDownloader::receiveCameraMod(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning(logWarning()) << "Connection error:" << reply->errorString();
        reply->deleteLater();
        emit downloadError(InstMod::CameraMod);
        return;
    }

    QByteArray replyByteArray = reply->readAll();

    reply->deleteLater();

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

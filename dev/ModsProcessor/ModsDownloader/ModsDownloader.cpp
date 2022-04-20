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

void ModsDownloader::downloadRussinFonts()
{
    requestRussianFonts();
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
        emit russianFontsDownladProgress(bytesReceived * 100 / bytesTotal);
        qInfo(logInfo()) << "Russian fonts download progress:" << bytesReceived << "/" << bytesTotal;
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
        return;
    }

    QByteArray replyByteArray = reply->readAll();

    reply->deleteLater();

    saveRussianFonts(std::move(replyByteArray));

    qInfo(logInfo()) <<  "Russian fonts downloaded to " << QDir::currentPath() + QDir::separator() + "RussianFonts.zip";

    emit russianFontsDownloaded(QDir::currentPath() + QDir::separator() + "RussianFonts.zip");
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

#ifndef MODSDOWNLOADER_H
#define MODSDOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <logger.h>

class ModsDownloader : public QObject
{
    Q_OBJECT
public:
    explicit ModsDownloader(QObject *parent = nullptr);

public:
    void downloadRussinFonts();

signals:
    void russianFontsDownladProgress(int);
    void russianFontsDownloaded(QString path);
    void russianFontsDownloadError();

private:
    void requestRussianFonts();
    void receiveRussinFonts(QNetworkReply* reply);
    void saveRussianFonts(QByteArray russianFontsByteArray);


private:
    QNetworkAccessManager *m_networkManager;

};

#endif // MODSDOWNLOADER_H

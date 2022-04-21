#ifndef MODSDOWNLOADER_H
#define MODSDOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <logger.h>
#include <baseTypes.h>

class ModsDownloader : public QObject
{
    Q_OBJECT
public:
    explicit ModsDownloader(QObject *parent = nullptr);

public:
    void downloadMod(InstMod mod);

signals:
    void downladProgress(InstMod, int);
    void modDownloaded(InstMod, QString path);
    void downloadError(InstMod);

private:
    void requestRussianFonts();
    void receiveRussinFonts(QNetworkReply* reply);
    void saveRussianFonts(QByteArray russianFontsByteArray);

    void requestCameraMod();
    void receiveCameraMod(QNetworkReply* reply);
    void saveCameraMod(QByteArray cameraModByteArray);


private:
    QNetworkAccessManager *m_networkManager;

};

#endif // MODSDOWNLOADER_H

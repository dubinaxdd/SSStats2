#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <logger.h>
#include <baseTypes.h>

class MapManager : public QObject
{
    Q_OBJECT
public:
    explicit MapManager(QObject *parent = nullptr);

    void requestMapList();
    void receiveMapList(QNetworkReply *reply);

    void requestMapInfo(MapItem mapItem);
    void receiveMapInfo(QNetworkReply *reply, MapItem mapItem);

    void requestFile(QString fileName, QString fileHash);
    void receiveFile(QNetworkReply *reply, QString fileName);

signals:

private:
    QString getUrl(QString mapHash);


    bool uncompressGz(QByteArray input, QByteArray &output);

private:
    QNetworkAccessManager *m_networkManager;

};

#endif // MAPMANAGER_H

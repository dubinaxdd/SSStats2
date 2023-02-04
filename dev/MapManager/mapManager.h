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
    explicit MapManager(QString ssPath, QObject *parent = nullptr);

    void requestMapList();
    void receiveMapList(QNetworkReply *reply);

    void requestMapInfo(MapItem *mapItem);
    void receiveMapInfo(QNetworkReply *reply, MapItem *mapItem);

    void requestFile(QString fileName, QString fileHash, MapItem *mapItem);
    void receiveFile(QNetworkReply *reply, QString fileName, MapItem *mapItem);

    void getLocalMapFilesList();

public slots:
    void receiveRemoveMap(MapItem *mapItem);
    void receiveInstallMap(MapItem *mapItem);

signals:
    void sendMapItem(MapItem *mapItem);

private:
    QString getUrl(QString mapHash);
    bool uncompressGz(QByteArray input, QByteArray &output);
    void checkLocalFilesState(MapItem *mapItem);

private:
    QNetworkAccessManager *m_networkManager;
    QString m_ssPath;

    QList<MapFileHash> m_localMapFilesHashes;
    QList<MapItem> m_mapItemArray;

    bool m_blockInfoUpdate = false;
    int m_requestetMapInfoCount = 0;

};

#endif // MAPMANAGER_H

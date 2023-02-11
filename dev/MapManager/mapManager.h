#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <logger.h>
#include <baseTypes.h>
#include <settingsController.h>

class MapManager : public QObject
{
    Q_OBJECT
public:
    explicit MapManager(SettingsController* settingsController, QString ssPath, QObject *parent = nullptr);

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
    void receiveInstallAllMaps();
    void receiveInstallDefaultMaps();
    void receiveLoadMapsInfo();

private slots:
    void onSettingsLoaded();

signals:
    void sendMapItem(MapItem *mapItem);
    void sendDownloadingProgress(int downloadedCount, int fullCount, bool downloadedProcessed);

private:
    QString getUrl(QString mapHash);
    bool uncompressGz(QByteArray input, QByteArray &output);
    void checkLocalFilesState(MapItem *mapItem);

    void downloadNextMap();
    void installMap(MapItem *mapItem);

    QString consolidateTags(QList<QString> tags);

private:
    SettingsController* m_settingsController;
    QNetworkAccessManager *m_networkManager;
    QString m_ssPath;

    QList<MapFileHash> m_localMapFilesHashes;
    QList<MapItem> m_mapItemArray;

    bool m_blockInfoUpdate = false;
    int m_requestetMapInfoCount = 0;

    bool m_allMapsDownloadingProcessed = false;
    int m_downloadedMapsCount = 0;
    int m_downloadOnlyDefaultMaps = false;
};

#endif // MAPMANAGER_H

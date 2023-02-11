#ifndef MAPMANAGERPAGE_H
#define MAPMANAGERPAGE_H

#include <QObject>
#include <QAbstractListModel>
#include <baseTypes.h>
#include <imageProvider.h>
#include <settingsController.h>

class MapManagerPage : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool updatesAvailable READ updatesAvailable WRITE setUpdatesAvailable NOTIFY updatesAvailableChanged)

    Q_PROPERTY(QString currentMapName READ currentMapName WRITE setCurrentMapName NOTIFY currentMapNameChanged)
    Q_PROPERTY(QString currentMapAuthors READ currentMapAuthors WRITE setCurrentMapAuthors NOTIFY currentMapAuthorsChanged)
    Q_PROPERTY(QString currentMapDescription READ currentMapDescription WRITE setCurrentMapDescription NOTIFY currentMapDescriptionChanged)
    Q_PROPERTY(QString currentMapTags READ currentMapTags WRITE setCurrentMapTags NOTIFY currentMapTagsChanged)
    Q_PROPERTY(bool currentMapNeedInstall READ currentMapNeedInstall WRITE setCurrentMapNeedInstall NOTIFY currentMapNeedInstallChanged)
    Q_PROPERTY(int downloadedCount READ downloadedCount WRITE setDownloadedCount NOTIFY downloadedCountChanged)
    Q_PROPERTY(int fullCount READ fullCount WRITE setFullCount NOTIFY fullCountChanged)
    Q_PROPERTY(bool downloadedProcessed READ downloadedProcessed WRITE setDownloadedProcessed NOTIFY downloadedProcessedChanged)
    Q_PROPERTY(bool autoinstallDefaultMaps READ autoinstallDefaultMaps WRITE setAutoinstallDefaultMaps NOTIFY autoinstallDefaultMapsChanged)
    Q_PROPERTY(bool autoinstallAllMaps READ autoinstallAllMaps WRITE setAutoinstallAllMaps NOTIFY autoinstallAllMapsChanged)


public:
    explicit MapManagerPage(SettingsController* settingsController, ImageProvider* imageProvider, QObject *parent = nullptr);

    enum DataRoles {
            MapName = Qt::UserRole + 1,
            Authors = Qt::UserRole + 2,
            Description = Qt::UserRole + 3,
            Tags = Qt::UserRole + 4,
            NeedInstall = Qt::UserRole + 5,
            NeedUpdate = Qt::UserRole + 6,
            DownloadingProcessed = Qt::UserRole + 7,
            Selected = Qt::UserRole
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount( const QModelIndex& parent ) const override;

    bool updatesAvailable() const;
    void setUpdatesAvailable(bool newUpdatesAvailable);

    Q_INVOKABLE void removeMap(int index);
    Q_INVOKABLE void installMap(int index);
    Q_INVOKABLE void selectMap(int index);
    Q_INVOKABLE void installAllMaps();
    Q_INVOKABLE void installDefaultMaps();


    const QString &currentMapName() const;
    void setCurrentMapName(const QString &newCurrentMapName);

    const QString &currentMapAuthors() const;
    void setCurrentMapAuthors(const QString &newCurrentMapAuthors);

    const QString &currentMapDescription() const;
    void setCurrentMapDescription(const QString &newCurrentMapDescription);

    const QString &currentMapTags() const;
    void setCurrentMapTags(const QString &newCurrentMapTags);

    void setSsPath(const QString &newSsPath);

    bool currentMapNeedInstall() const;
    void setCurrentMapNeedInstall(bool newCurrentMapNeedInstall);

    int downloadedCount() const;
    void setDownloadedCount(int newDownloadedCount);

    int fullCount() const;
    void setFullCount(int newFullCount);

    bool downloadedProcessed() const;
    void setDownloadedProcessed(bool newDownloadedProcessed);

    bool autoinstallDefaultMaps() const;
    void setAutoinstallDefaultMaps(bool newAutoinstallDefaultMaps);

    bool autoinstallAllMaps() const;
    void setAutoinstallAllMaps(bool newAutoinstallAllMaps);

signals:
    void updatesAvailableChanged();
    void sendRemoveMap(MapItem *mapItem);
    void sendInstallMap(MapItem *mapItem);
    void sendInstallAllMaps();
    void sendInstallDefaultMaps();

    void currentMapNameChanged();
    void currentMapAuthorsChanged();
    void currentMapDescriptionChanged();
    void currentMapTagsChanged();

    void currentMapNeedInstallChanged();
    void downloadedCountChanged();
    void fullCountChanged();
    void downloadedProcessedChanged();
    void autoinstallDefaultMapsChanged();
    void autoinstallAllMapsChanged();

public slots:
    void receiveMapItem(MapItem *mapItem);
    void receiveDownloadingProgress(int downloadedCount, int fullCount, bool downloadedProcessed);

protected:
   QHash<int, QByteArray> roleNames() const override;


private slots:
    void onSettingsLoaded();

private:
    void checkUpdates();
    QImage loadMiniMapImage(QString fileName);
    QString consolidateTags(QList<QString> tags);

private:
    SettingsController* m_settingsController;
    ImageProvider* m_imageProvider;
    QString m_ssPath = "";

    QList<MapItem*> m_mapItemArray;
    bool m_updatesAvailable = false;

    QString m_currentMapName = "";
    QString m_currentMapAuthors = "";
    QString m_currentMapDescription  = "";
    QString m_currentMapTags = "";
    bool m_currentMapNeedInstall = true;

    int m_downloadedCount = 0;
    int m_fullCount = 0;
    bool m_downloadedProcessed = false;

    bool m_autoinstallDefaultMaps = true;
    bool m_autoinstallAllMaps = false;
};

#endif // MAPMANAGERPAGE_H

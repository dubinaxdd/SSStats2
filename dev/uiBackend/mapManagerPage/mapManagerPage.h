#ifndef MAPMANAGERPAGE_H
#define MAPMANAGERPAGE_H

#include <QObject>
#include <QAbstractListModel>
#include <baseTypes.h>
#include <imageProvider.h>

class MapManagerPage : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool updatesAvailable READ updatesAvailable WRITE setUpdatesAvailable NOTIFY updatesAvailableChanged)

    Q_PROPERTY(QString currentMapName READ currentMapName WRITE setCurrentMapName NOTIFY currentMapNameChanged)
    Q_PROPERTY(QString currentMapAuthors READ currentMapAuthors WRITE setCurrentMapAuthors NOTIFY currentMapAuthorsChanged)
    Q_PROPERTY(QString currentMapDescription READ currentMapDescription WRITE setCurrentMapDescription NOTIFY currentMapDescriptionChanged)
    Q_PROPERTY(QString currentMapTags READ currentMapTags WRITE setCurrentMapTags NOTIFY currentMapTagsChanged)

public:
    explicit MapManagerPage(ImageProvider* imageProvider, QObject *parent = nullptr);

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


    const QString &currentMapName() const;
    void setCurrentMapName(const QString &newCurrentMapName);

    const QString &currentMapAuthors() const;
    void setCurrentMapAuthors(const QString &newCurrentMapAuthors);

    const QString &currentMapDescription() const;
    void setCurrentMapDescription(const QString &newCurrentMapDescription);

    const QString &currentMapTags() const;
    void setCurrentMapTags(const QString &newCurrentMapTags);

    void setSsPath(const QString &newSsPath);

signals:
    void updatesAvailableChanged();
    void sendRemoveMap(MapItem *mapItem);
    void sendInstallMap(MapItem *mapItem);

    void currentMapNameChanged();
    void currentMapAuthorsChanged();
    void currentMapDescriptionChanged();
    void currentMapTagsChanged();

public slots:
    void receiveMapItem(MapItem *mapItem);

protected:
   QHash<int, QByteArray> roleNames() const override;

private:
    void checkUpdates();
    QImage loadMiniMapImage(QString fileName);

private:
    ImageProvider* m_imageProvider;
    QString m_ssPath = "";

    QList<MapItem*> m_mapItemArray;
    bool m_updatesAvailable = false;

    QString m_currentMapName = "";
    QString m_currentMapAuthors = "";
    QString m_currentMapDescription  = "";
    QString m_currentMapTags = "";
};

#endif // MAPMANAGERPAGE_H

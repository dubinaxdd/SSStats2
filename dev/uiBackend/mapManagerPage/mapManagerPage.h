#ifndef MAPMANAGERPAGE_H
#define MAPMANAGERPAGE_H

#include <QObject>
#include <QAbstractListModel>
#include <baseTypes.h>

class MapManagerPage : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool updatesAvailable READ updatesAvailable WRITE setUpdatesAvailable NOTIFY updatesAvailableChanged)

public:
    explicit MapManagerPage(QObject *parent = nullptr);

    enum DataRoles {
            MapName = Qt::UserRole + 1,
            Authors = Qt::UserRole + 2,
            Description = Qt::UserRole + 3,
            Tags = Qt::UserRole + 4,
            NeedInstall = Qt::UserRole + 5,
            NeedUpdate = Qt::UserRole + 6,
            DownloadingProcessed = Qt::UserRole
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount( const QModelIndex& parent ) const override;

    bool updatesAvailable() const;
    void setUpdatesAvailable(bool newUpdatesAvailable);

    Q_INVOKABLE void removeMap(int index);
    Q_INVOKABLE void installMap(int index);

signals:
    void updatesAvailableChanged();
    void sendRemoveMap(MapItem *mapItem);
    void sendInstallMap(MapItem *mapItem);

public slots:
    void receiveMapItem(MapItem *mapItem);

protected:
   QHash<int, QByteArray> roleNames() const override;

private:
    void checkUpdates();

private:
    QList<MapItem*> m_mapItemArray;
    bool m_updatesAvailable = false;

 };

#endif // MAPMANAGERPAGE_H

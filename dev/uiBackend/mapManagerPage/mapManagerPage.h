#ifndef MAPMANAGERPAGE_H
#define MAPMANAGERPAGE_H

#include <QObject>
#include <QAbstractListModel>
#include <baseTypes.h>

class MapManagerPage : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit MapManagerPage(QObject *parent = nullptr);

    enum DataRoles {
            MapName = Qt::UserRole + 1,
            Authors = Qt::UserRole + 2,
            Description = Qt::UserRole + 3,
            Size = Qt::UserRole + 4,
            Tags = Qt::UserRole + 5,
            NeedInstall = Qt::UserRole + 6,
            NeedUpdate = Qt::UserRole
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount( const QModelIndex& parent ) const override;

signals:

public slots:
    void receiveMapItem(MapItem *mapItem);

protected:
   QHash<int, QByteArray> roleNames() const override;


private:
    QList<MapItem*> m_mapItemArray;

};

#endif // MAPMANAGERPAGE_H

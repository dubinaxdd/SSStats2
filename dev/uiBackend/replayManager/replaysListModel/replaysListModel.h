#ifndef REPLAYSLISTMODEL_H
#define REPLAYSLISTMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <baseTypes.h>

class ReplaysListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ReplaysListModel(QObject *parent = nullptr);

    enum DataRoles {
            FolderName = Qt::UserRole + 1,
            Name = Qt::UserRole + 2,
            Mod = Qt::UserRole + 3,
            Map
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount( const QModelIndex& parent ) const override;

    void setReplaysList(const QVector<ReplayListInfo> &newReplaysList);

protected:
   QHash<int, QByteArray> roleNames() const override;

signals:

private:
    QVector<ReplayListInfo> replaysList;

};

#endif // REPLAYSLISTMODEL_H

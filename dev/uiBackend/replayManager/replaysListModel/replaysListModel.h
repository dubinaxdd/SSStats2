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
            FileName = Qt::UserRole + 1,
            Name = Qt::UserRole + 2,
            Mod = Qt::UserRole + 3,
            Selected = Qt::UserRole + 4,
            Map
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount( const QModelIndex& parent ) const override;

    void setReplaysList(const QVector<ReplayListInfo> &newReplaysList);

    void sort();

    Q_INVOKABLE void setSortType(int sortType);
    Q_INVOKABLE void setSelected(int index);

protected:
   QHash<int, QByteArray> roleNames() const override;

signals:
   void select(QString fileName);

private:
   void replace(int index);

private:
    QVector<ReplayListInfo> replaysList;
    SortType m_sortType = SortType::SortByFileName;

};

#endif // REPLAYSLISTMODEL_H

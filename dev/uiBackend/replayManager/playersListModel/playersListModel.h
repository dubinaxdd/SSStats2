#ifndef PLAYERSLISTMODEL_H
#define PLAYERSLISTMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <baseTypes.h>

class PlayersListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit PlayersListModel(QObject *parent = nullptr);

    enum DataRoles {
            Name = Qt::UserRole + 1,
            Team = Qt::UserRole + 2,
            Type = Qt::UserRole + 3,
            Race
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount( const QModelIndex& parent ) const override;

    void setPlayersList(QVector<ReplayPlayer> players);


protected:
   QHash<int, QByteArray> roleNames() const override;

signals:

private:

    QVector<ReplayPlayer> m_players;

};

#endif // PLAYERSLISTMODEL_H

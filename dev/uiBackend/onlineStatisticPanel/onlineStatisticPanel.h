#ifndef ONLINESTATISTICPANEL_H
#define ONLINESTATISTICPANEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QMap>
#include <baseTypes.h>

class OnlineStatisticPanel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int uniquePlayersOnDay   READ getUniquePlayersOnDay      NOTIFY uniquePlayersOnlineStatisticChanged)
    Q_PROPERTY(int uniquePlayersOnMonth READ getUniquePlayersOnMonth    NOTIFY uniquePlayersOnlineStatisticChanged)
    Q_PROPERTY(int uniquePlayersOnYear  READ getUniquePlayersOnYear     NOTIFY uniquePlayersOnlineStatisticChanged)
    Q_PROPERTY(int uniquePlayersOnTotal READ getUniquePlayersOnTotal    NOTIFY uniquePlayersOnlineStatisticChanged)

public:
    explicit OnlineStatisticPanel(QObject *parent = nullptr);

    enum DataRoles {
        ModName = Qt::UserRole + 1,
        OnlineCount = Qt::UserRole
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount( const QModelIndex& parent ) const override;

    int getUniquePlayersOnDay();
    int getUniquePlayersOnMonth();
    int getUniquePlayersOnYear();
    int getUniquePlayersOnTotal();

protected:
    QHash<int, QByteArray> roleNames() const override;

signals:
    void uniquePlayersOnlineStatisticChanged();

public slots:
    void receiveModsOnlineCountMap(QMap<QString, int> modsOnlineCountMap);
    void receiveUniquePlayersOnlineStatistic(UniqueOnlineStatistic uniqueOnlineStatistic);

private:
    QMap<QString, int> m_modsOnlineCountMap;
    UniqueOnlineStatistic m_uniqueOnlineStatistic;

};

#endif // ONLINESTATISTICPANEL_H

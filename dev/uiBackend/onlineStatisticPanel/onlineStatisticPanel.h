#ifndef ONLINESTATISTICPANEL_H
#define ONLINESTATISTICPANEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QMap>

class OnlineStatisticPanel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit OnlineStatisticPanel(QObject *parent = nullptr);


    enum DataRoles {
        ModName = Qt::UserRole + 1,
        OnlineCount = Qt::UserRole
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount( const QModelIndex& parent ) const override;

protected:
    QHash<int, QByteArray> roleNames() const override;



public slots:
    void receiveModsOnlineCountMap(QMap<QString, int> modsOnlineCountMap);

private:
    QMap<QString, int> m_modsOnlineCountMap;

};

#endif // ONLINESTATISTICPANEL_H

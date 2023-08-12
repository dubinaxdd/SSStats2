#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QAbstractListModel>
#include <baseTypes.h>

class NotificationManager : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit NotificationManager(QObject *parent = nullptr);

    enum DataRoles {
        Text = Qt::UserRole + 1,
        Type
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount( const QModelIndex& parent ) const override;

    Q_INVOKABLE void removeItem(int index);

protected:
   QHash<int, QByteArray> roleNames() const override;

public slots:
    void receiveNotification(NotificationInfo notificationInfo);

signals:

private:
    QList<NotificationInfo> m_notificationInfo;

};

#endif // NOTIFICATIONMANAGER_H

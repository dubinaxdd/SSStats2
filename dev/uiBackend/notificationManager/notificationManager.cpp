#include "notificationManager.h"
#include <QDebug>

NotificationManager::NotificationManager(QObject *parent) : QAbstractListModel(parent)
{

}

QVariant NotificationManager::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_notificationInfo.count())
        return QVariant();

    switch (role) {
        case Text: return m_notificationInfo.at(index.row()).text;
        case Type: return m_notificationInfo.at(index.row()).type;
    }

    return QVariant();
}

int NotificationManager::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_notificationInfo.count();
}

void NotificationManager::removeItem(int index)
{
    beginResetModel();
    m_notificationInfo.removeAt(index);
    endResetModel();
}

QHash<int, QByteArray> NotificationManager::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Text] = "text";
    roles[Type] = "type";

    return roles;
}

void NotificationManager::receiveNotification(NotificationInfo notificationInfo)
{
    for (int i = 0; i < m_notificationInfo.count(); i++)
    {
        if (m_notificationInfo.at(i).uuid == notificationInfo.uuid)
        {
            m_notificationInfo[i] = notificationInfo;
            QModelIndex index = QAbstractItemModel::createIndex(i, 0);
            emit dataChanged(index,index);
            return;
        }
    }

    beginResetModel();
    m_notificationInfo.push_front(notificationInfo);
    endResetModel();

}

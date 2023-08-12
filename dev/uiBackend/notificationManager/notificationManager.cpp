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
    beginResetModel();
    m_notificationInfo.push_front(notificationInfo);
    endResetModel();
}

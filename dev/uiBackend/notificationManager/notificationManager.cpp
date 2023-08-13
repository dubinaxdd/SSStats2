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
    /*for (NotificationInfo &needNotifyInfo : m_notificationInfo)
    {
        if (needNotifyInfo.uuid == notificationInfo.uuid)
        {
            beginResetModel();
            needNotifyInfo = notificationInfo;
            endResetModel();
            return;
        }
    }*/


    for (int i = 0; i < m_notificationInfo.count(); i++)
    {
        if (m_notificationInfo.at(i).uuid == notificationInfo.uuid)
        {
            //beginResetModel();
            m_notificationInfo[i] = notificationInfo;
            //endResetModel();


            qDebug() << "ASDASDASDADASDASDASD" + notificationInfo.text;

            QModelIndex index = QAbstractItemModel::createIndex(i, 0);

            emit dataChanged(index,index);
            return;
        }
    }

    beginResetModel();
    m_notificationInfo.push_front(notificationInfo);
    endResetModel();

}

#include "NewsPage.h"
#include <QDebug>

NewsPage::NewsPage(QObject *parent) : QAbstractListModel(parent)
{

}

QVariant NewsPage::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_news.count())
        return QVariant();

    const DiscordMessage &message = m_news[index.row()];
    if (role == UserName)
        return message.userName;
    else if (role == Content)
        return message.content;
    else if (role == TimesTamp)
        return message.timestamp.toString("dd.MM.yyyy");
    return QVariant();
}

int NewsPage::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_news.count();
}

QHash<int, QByteArray> NewsPage::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[UserName] = "userName";
    roles[Content] = "content";
    roles[TimesTamp] = "timesTamp";
    return roles;
}

void NewsPage::receiveNews(QList<DiscordMessage> news)
{
    beginInsertRows(QModelIndex(), 0, news.count() - 1);
    m_news = news;
    endInsertRows();

}

#include "MessagesPage.h"
#include <QDebug>
#include <QAbstractItemModel>
MessagesPage::MessagesPage(QObject *parent) : QAbstractListModel(parent)
{

}

QVariant MessagesPage::data(const QModelIndex &index, int role) const
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
    else if (role == AvatarId)
        return message.avatarId;
    else if (role == AttachmentId)
        return message.attachmentId;
    else if (role == AttachmentImageWidth)
        return message.attachmentImageWidth;
    else if (role == AttachmentImageHeight)
        return message.attachmentImageHeight;
    return QVariant();
}

int MessagesPage::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_news.count();
}

QHash<int, QByteArray> MessagesPage::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[UserName] = "userName";
    roles[Content] = "content";
    roles[TimesTamp] = "timesTamp";
    roles[AvatarId] = "avatarId";
    roles[AttachmentId] = "attachmentId";
    roles[AttachmentImageWidth] = "attachmentImageWidth";
    roles[AttachmentImageHeight] = "attachmentImageHeight";

    return roles;
}

void MessagesPage::receiveMessages(QList<DiscordMessage> news)
{
    beginRemoveRows(QModelIndex(), 0, m_news.count() - 1);
    m_news.clear();
    endRemoveRows();

    beginInsertRows(QModelIndex(), 0, news.count() - 1);

    m_news = news;

    for (int i = 0; i < m_news.count(); i++)
    {
        QString newText = m_news.at(i).content;

        newText.append('\0');

        for (int j = 0; j < newText.count() - 8; j++)
        {
            if(newText.mid(j, 7) == "http://" || newText.mid(j, 8) == "https://")
            {
                for(int k = j; k < newText.count(); k++)
                {
                    if(  k == newText.count() - 1 ||
                         newText.at(k) == ' ' ||
                         newText.at(k) == '\n' ||
                         newText.at(k) == '\0')
                    {
                        QString url = newText.mid(j, k - j);
                        QString formattedUrl = "<a href=\"" + url + "\">" + url + "</a>";

                        newText.replace(j, k-j, formattedUrl);
                        j += formattedUrl.count();
                        break;
                    }
                }
            }
        }

        for (int j = 0; j < newText.count(); j++)
        {
            if(newText.at(j) == '\n')
                newText.replace(j, 1, "<br> ");
        }

        m_news[i].content = newText;
    }




    endInsertRows();
}

void MessagesPage::onAvatarUpdate()
{
    for(int i = 0; i < m_news.count(); i++)
    {
        QString temp = m_news.at(i).avatarId;

        m_news[i].avatarId = "";

        QModelIndex first = QAbstractItemModel::createIndex(i, 0);
        QModelIndex last = QAbstractItemModel::createIndex(i, 0);

        emit dataChanged(first, last);

        m_news[i].avatarId = temp;
        emit dataChanged(first, last);
    }
}

void MessagesPage::onAttachmetImagesUpdate()
{
    for(int i = 0; i < m_news.count(); i++)
    {
        QString temp = m_news.at(i).attachmentId;

        m_news[i].attachmentId = "";

        QModelIndex first = QAbstractItemModel::createIndex(i, 0);
        QModelIndex last = QAbstractItemModel::createIndex(i, 0);

        emit dataChanged(first, last);

        m_news[i].attachmentId = temp;
        emit dataChanged(first, last);
    }
}

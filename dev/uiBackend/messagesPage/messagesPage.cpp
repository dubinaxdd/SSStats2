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
    else if (role == IsNew)
        return message.isNew;
    else if (role == YoutubeId)
        return message.youtubeId;
    else if (role == YoutubeImageWidth)
        return 480;
    else if (role == YoutubeImageHeight)
        return 360;

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
    roles[IsNew] = "isNew";
    roles[YoutubeId] = "youtubeId";
    roles[YoutubeImageWidth] = "youtubeImageWidth";
    roles[YoutubeImageHeight] = "youtubeImageHeight";

    return roles;
}

void MessagesPage::receiveMessages(QList<DiscordMessage> news)
{
    beginInsertRows(QModelIndex(), 0, news.count() - 1);
    m_news = formatingMessagesText(news) + m_news;
    endInsertRows();
}

void MessagesPage::receiveNextMessages(QList<DiscordMessage> news)
{
    beginInsertRows(QModelIndex(), m_news.count(), m_news.count() + news.count() - 1);

    m_news = m_news + formatingMessagesText(news);

    endInsertRows();

    m_requestingNextMessagesProcessed = false;
}

QList<DiscordMessage> MessagesPage::formatingMessagesText(QList<DiscordMessage> messages)
{
    for (int i = 0; i < messages.count(); i++)
    {
        QString newText = messages.at(i).content;

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

        //Жирный шрифт
        formate(&newText, "**", "<b>", "</b>");

        //Подчеркнутый шрифт
        formate(&newText, "__", "<u>", "</u>");

        //Зачеркнутый шрифт
        formate(&newText, "~~", "<s>", "</s>");

        //Курсивный шрифт
        formate(&newText, "*", "<i>", "</i>");
        //formate(&newText, "_", "<i>", "</i>");

        if ( messages.at(i).isNew)
        {
            m_newsAvailable = true;
            emit newsAvailableChanged(m_newsAvailable);
        }

        messages[i].content = newText;
    }

    return messages;
}

void MessagesPage::formate(QString *text, QString discordTag, QString openTag, QString closeTag)
{
    //Подчеркнутый шрифт
    bool endString = false;
    int firstIndex = 0;
    int tagLength = discordTag.count() - 1;

    for (int j = 0; j < text->count() - tagLength; j++)
    {
        if(text->mid(j, discordTag.count()) == discordTag)
        {
            if (endString)
                text->replace(j, discordTag.count(), closeTag);
            else
            {
                text->replace(j, discordTag.count(), openTag);
                firstIndex = j;
            }

            endString = !endString;
        }
    }

    if (endString)
        text->replace(firstIndex, 3, discordTag);
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

void MessagesPage::onYoutubeImagesUpdate()
{
    for(int i = 0; i < m_news.count(); i++)
    {
        QString temp = m_news.at(i).youtubeId;

        m_news[i].youtubeId = "";

        QModelIndex first = QAbstractItemModel::createIndex(i, 0);
        QModelIndex last = QAbstractItemModel::createIndex(i, 0);

        emit dataChanged(first, last);

        m_news[i].youtubeId = temp;
        emit dataChanged(first, last);
    }
}

void MessagesPage::messagesReaded()
{
    for(int i = 0; i < m_news.count(); i++)
    {
        QModelIndex first = QAbstractItemModel::createIndex(i, 0);
        QModelIndex last = QAbstractItemModel::createIndex(i, 0);
        m_news[i].isNew = false;
        emit dataChanged(first, last);
    }

    m_newsAvailable = false;
    emit newsAvailableChanged(m_newsAvailable);

    if(m_news.count() > 0)
        emit sendLastReadedMessageId(m_news[0].messageId);
}

void MessagesPage::requestNextMessages()
{
    if (m_news.count() > 0 && !m_requestingNextMessagesProcessed)
    {
        m_requestingNextMessagesProcessed = true;
        emit sendNextMessagesRequest(m_news.last().messageId);
    }
}

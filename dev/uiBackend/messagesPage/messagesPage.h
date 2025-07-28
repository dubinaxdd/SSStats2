#ifndef MESSAGESPAGE_H
#define MESSAGESPAGE_H

#include <QAbstractListModel>
#include <baseTypes.h>

class MessagesPage : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY( bool newsAvailable MEMBER m_newsAvailable NOTIFY newsAvailableChanged)

public:
    explicit MessagesPage(QObject *parent = nullptr);

    enum DataRoles {
            UserName = Qt::UserRole + 1,
            Content = Qt::UserRole + 2,
            AvatarId = Qt::UserRole + 3,
            AttachmentId = Qt::UserRole + 4,
            AttachmentImageWidth = Qt::UserRole + 5,
            AttachmentImageHeight = Qt::UserRole + 6,
            IsNew = Qt::UserRole + 7,
            YoutubeId = Qt::UserRole + 8,
            YoutubeImageWidth = Qt::UserRole + 9,
            YoutubeImageHeight = Qt::UserRole + 10,
            TimesTamp
    };


   QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
   int rowCount( const QModelIndex& parent ) const override;

   Q_INVOKABLE void messagesReaded();
   Q_INVOKABLE void requestNextMessages();

protected:
   QHash<int, QByteArray> roleNames() const override;

public slots:
    void receiveMessages(QList<DiscordMessage> news);
    //void receiveNextMessages(QList<DiscordMessage> news);
    void onAvatarUpdate();
    void onAttachmetImagesUpdate();
    void onYoutubeImagesUpdate();

signals:
    void newsAvailableChanged(bool);
    void sendLastReadedMessageId(QString id);
    void sendNextMessagesRequest(QString messgaesBeforeId);

private:
    QList<DiscordMessage> formatingMessagesText(QList<DiscordMessage> messages);

    void formate(QString* text, QString discordTag, QString openTag, QString closeTag);

private:
    QList<DiscordMessage> m_news;
    bool m_newsAvailable = false;
    bool m_requestingNextMessagesProcessed = false;

};

#endif // MESSAGESPAGE_H

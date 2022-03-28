#ifndef MESSAGESPAGE_H
#define MESSAGESPAGE_H

#include <QAbstractListModel>
#include <baseTypes.h>

class MessagesPage : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit MessagesPage(QObject *parent = nullptr);

    enum DataRoles {
            UserName = Qt::UserRole + 1,
            Content = Qt::UserRole + 2,
            AvatarId = Qt::UserRole + 3,
            TimesTamp
    };


   QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
   int rowCount( const QModelIndex& parent ) const override;

protected:
   QHash<int, QByteArray> roleNames() const override;

public slots:
    void receiveMessages(QList<DiscordMessage> news);
    void onAvatarUpdate();

signals:

private:
    QList<DiscordMessage> m_news;

};

#endif // MESSAGESPAGE_H

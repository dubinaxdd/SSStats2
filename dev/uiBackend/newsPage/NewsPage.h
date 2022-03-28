#ifndef NEWSPAGE_H
#define NEWSPAGE_H

#include <QAbstractListModel>
#include <baseTypes.h>

class NewsPage : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit NewsPage(QObject *parent = nullptr);

    enum DataRoles {
            UserName = Qt::UserRole + 1,
            Content = Qt::UserRole + 2,
            TimesTamp
    };


   QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
   int rowCount( const QModelIndex& parent ) const override;

protected:
   QHash<int, QByteArray> roleNames() const override;

public slots:
    void receiveNews(QList<DiscordMessage> news);

signals:

private:
    QList<DiscordMessage> m_news;

};

#endif // NEWSPAGE_H

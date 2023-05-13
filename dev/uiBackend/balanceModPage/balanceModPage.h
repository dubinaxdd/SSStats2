#ifndef BALANCEMODPAGE_H
#define BALANCEMODPAGE_H

#include <QAbstractListModel>

class BalanceModPage : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString currentModName READ currentModName WRITE setCurrentModName NOTIFY currentModNameChanged)

public:
    explicit BalanceModPage(QObject *parent = nullptr);

    enum DataRoles {
        Selected = Qt::UserRole + 1,
        Installed = Qt::UserRole + 1,
        Version
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount( const QModelIndex& parent ) const override;

    Q_INVOKABLE void slectItem(int itemIndex);

    const QString &currentModName() const;
    void setCurrentModName(const QString &newCurrentModName);

protected:
   QHash<int, QByteArray> roleNames() const override;

public slots:
   void receiveVersions(QStringList versions);

signals:
   void currentModNameChanged();

private:
   QStringList m_versions;
   QString m_currentModName;
   int m_selectedItemIndex = 0;
};

#endif // BALANCEMODPAGE_H

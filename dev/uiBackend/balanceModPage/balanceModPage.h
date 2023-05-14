#ifndef BALANCEMODPAGE_H
#define BALANCEMODPAGE_H

#include <QAbstractListModel>
#include <baseTypes.h>

class BalanceModPage : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString currentModName READ currentModName WRITE setCurrentModName NOTIFY currentModNameChanged)

public:
    explicit BalanceModPage(QObject *parent = nullptr);

    enum DataRoles {
        Selected = Qt::UserRole + 1,
        IsInstalled = Qt::UserRole + 2,
        Version = Qt::UserRole + 3,
        UiName
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount( const QModelIndex& parent ) const override;

    Q_INVOKABLE void slectItem(int itemIndex);

    const QString &currentModName() const;
    void setCurrentModName(const QString &newCurrentModName);

protected:
   QHash<int, QByteArray> roleNames() const override;

public slots:
   void receiveVersions(QList<ModInfo> modsInfo);

signals:
   void currentModNameChanged();

private:
   QList<ModInfo> m_modsInfo;
   QString m_currentModName;
   int m_selectedItemIndex = 0;
};

#endif // BALANCEMODPAGE_H

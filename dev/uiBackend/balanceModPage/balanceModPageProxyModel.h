#ifndef BALANCEMODPAGEPROXYMODEL_H
#define BALANCEMODPAGEPROXYMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>

class BalanceModPageProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit BalanceModPageProxyModel(QObject *parent = nullptr);
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

    void setBetVersionsVisible(bool newBetVersionsVisible);

private:
    bool m_betaVersionsVisible = true;
};

#endif // BALANCEMODPAGEPROXYMODEL_H

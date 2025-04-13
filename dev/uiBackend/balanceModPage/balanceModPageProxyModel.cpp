#include "balanceModPageProxyModel.h"
#include "balanceModPage.h"

BalanceModPageProxyModel::BalanceModPageProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{

}
bool BalanceModPageProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (m_betaVersionsVisible)
        return true;

    if (!sourceModel())
        return false;

    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
    auto value = !sourceModel()->data(index, BalanceModPage::IsBeta).toBool();

    return value;
}

void BalanceModPageProxyModel::setBetVersionsVisible(bool newBetVersionsVisible)
{
    m_betaVersionsVisible = newBetVersionsVisible;
    invalidateFilter();
}

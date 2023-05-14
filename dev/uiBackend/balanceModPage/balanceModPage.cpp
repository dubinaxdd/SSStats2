#include "balanceModPage.h"
#include <QDebug>

BalanceModPage::BalanceModPage(QObject *parent)
    : QAbstractListModel(parent)
{

}

QVariant BalanceModPage::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_modsInfo.count())
        return QVariant();

    switch (role) {
        case UiName: return m_modsInfo.at(index.row()).uiName;
        case Version: return m_modsInfo.at(index.row()).version;
        case IsInstalled: return m_modsInfo.at(index.row()).isInstalled;
        case Selected: return index.row() == m_selectedItemIndex;
    }

    return QVariant();
}

int BalanceModPage::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_modsInfo.count();
}

void BalanceModPage::slectItem(int itemIndex)
{
    beginResetModel();
    m_selectedItemIndex = itemIndex;
    endResetModel();

    setCurrentModName(m_modsInfo.at(m_selectedItemIndex).uiName);
}

QHash<int, QByteArray> BalanceModPage::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[UiName] = "uiName";
    roles[Version] = "version";
    roles[IsInstalled] = "isInstalled";
    roles[Selected] = "selected";

    return roles;
}

void BalanceModPage::receiveVersions(QList <ModInfo> modsInfo)
{
    if(modsInfo.count() < 1)
        return;

    m_selectedItemIndex = 0;

    beginResetModel();
    m_modsInfo = modsInfo;
    endResetModel();

    setCurrentModName(m_modsInfo.at(m_selectedItemIndex).uiName);
}

const QString &BalanceModPage::currentModName() const
{
    return m_currentModName;
}

void BalanceModPage::setCurrentModName(const QString &newCurrentModName)
{
    if (m_currentModName == newCurrentModName)
        return;
    m_currentModName = newCurrentModName;
    emit currentModNameChanged();
}

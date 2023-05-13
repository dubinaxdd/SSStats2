#include "balanceModPage.h"
#include <QDebug>

BalanceModPage::BalanceModPage(QObject *parent)
    : QAbstractListModel(parent)
{

}

QVariant BalanceModPage::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_versions.count())
        return QVariant();

    const QString version = m_versions[index.row()];

    if (role == Version)
        return version;

    if (role == Selected)
        return index.row() == m_selectedItemIndex;

    return QVariant();
}

int BalanceModPage::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_versions.count();
}

void BalanceModPage::slectItem(int itemIndex)
{
    beginResetModel();
    m_selectedItemIndex = itemIndex;
    endResetModel();

    setCurrentModName(m_versions.at(m_selectedItemIndex));
}

QHash<int, QByteArray> BalanceModPage::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Version] = "version";
    roles[Selected] = "selected";

    return roles;
}

void BalanceModPage::receiveVersions(QStringList versions)
{
    beginResetModel();
    m_versions = versions;
    endResetModel();

    setCurrentModName(m_versions.at(m_selectedItemIndex));
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

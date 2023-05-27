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
    case Version: return m_modsInfo.at(index.row()).version + (m_modsInfo.at(index.row()).isActual ? " (Latest)" : "");
        case IsInstalled: return m_modsInfo.at(index.row()).isInstalled;
        case Selected: return index.row() == m_selectedItemIndex;
        case IsCurrentMod: return m_modsInfo.at(index.row()).isCurrentMod;
        case DownladingProcessed: return m_modsInfo.at(index.row()).downloadingProcessed;
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

    emit selectedModInfoChanged();

    if(m_modsInfo.at(m_selectedItemIndex).changelog.isEmpty())
        emit requestChangeLog(m_modsInfo.at(m_selectedItemIndex).technicalName);
}

void BalanceModPage::downloadCurrentMod()
{
    emit requestDownloadMod(m_modsInfo.at(m_selectedItemIndex).technicalName);

    m_modsInfo[m_selectedItemIndex].downloadingProcessed = true;

    emit selectedModInfoChanged();

    QModelIndex index = QAbstractItemModel::createIndex(m_selectedItemIndex, 0);;
    emit dataChanged(index, index);
}

void BalanceModPage::uninstallCurrentMod()
{
    emit requestUninstallMod(m_modsInfo.at(m_selectedItemIndex).technicalName);

    m_modsInfo[m_selectedItemIndex].isInstalled = false;

    emit selectedModInfoChanged();

    QModelIndex index = QAbstractItemModel::createIndex(m_selectedItemIndex, 0);;
    emit dataChanged(index, index);
}

const QString BalanceModPage::selectedModName() const
{
    if(m_modsInfo.count() > m_selectedItemIndex)
        return m_modsInfo.at(m_selectedItemIndex).uiName;

    return "";
}

const QString BalanceModPage::selectedModVersion() const
{
    if(m_modsInfo.count() > m_selectedItemIndex)
        return m_modsInfo.at(m_selectedItemIndex).version + (m_modsInfo.at(m_selectedItemIndex).isActual ? " (Latest)" : "");

    return "";
}

const QString BalanceModPage::selectedChangeLog() const
{
    if(m_modsInfo.count() > m_selectedItemIndex)
        return m_modsInfo.at(m_selectedItemIndex).changelog;

    return "";
}

const bool BalanceModPage::selectedModIsCurrent() const
{
    if(m_modsInfo.count() > m_selectedItemIndex)
        return m_modsInfo.at(m_selectedItemIndex).isCurrentMod;

    return false;
}

const bool BalanceModPage::selectedModIsActual() const
{
    if(m_modsInfo.count() > m_selectedItemIndex)
        return m_modsInfo.at(m_selectedItemIndex).isActual;

    return false;
}

const bool BalanceModPage::selectedModIsInstalled() const
{
    if(m_modsInfo.count() > m_selectedItemIndex)
        return m_modsInfo.at(m_selectedItemIndex).isInstalled;

    return false;
}

const bool BalanceModPage::selectedModDownladingProcessed() const
{
    if(m_modsInfo.count() > m_selectedItemIndex)
        return m_modsInfo.at(m_selectedItemIndex).downloadingProcessed;

    return false;
}

QHash<int, QByteArray> BalanceModPage::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[UiName] = "uiName";
    roles[Version] = "version";
    roles[IsInstalled] = "isInstalled";
    roles[Selected] = "selected";
    roles[IsCurrentMod] = "isCurrentMod";
    roles[DownladingProcessed] = "downladingProcessed";

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

    emit selectedModInfoChanged();
}

void BalanceModPage::receiveCurrentModInGame(QString modName)
{
    setCurrentModInGame(modName);

    beginResetModel();

    for(int i = 0; i < m_modsInfo.count(); i++)
        m_modsInfo[i].isCurrentMod = modName.toLower() == m_modsInfo[i].technicalName.toLower();

    endResetModel();
}

void BalanceModPage::receiveChangeLog(QString modTechnicalName, QString changeLog)
{
    for(int i = 0; i < m_modsInfo.count(); i++)
    {
        if (modTechnicalName == m_modsInfo.at(i).technicalName)
        {
            m_modsInfo[i].changelog = changeLog;
            emit selectedModInfoChanged();
        }
    }
}

void BalanceModPage::receiveModDownloadProgress(int progress, QString modTechnicalName)
{
    m_downloadingProgress = QString::number(progress);
    emit downloadingProgressChanged();
}

void BalanceModPage::receiveModDownloaded(QString modTechnicalName)
{
    m_downloadingProgress = "";
    emit downloadingProgressChanged();

    for (int i = 0; i < m_modsInfo.count(); i++)
    {
        if ( m_modsInfo.at(i).technicalName == modTechnicalName)
        {
            m_modsInfo[i].isInstalled = true;
            m_modsInfo[i].downloadingProcessed = false;

            if (m_selectedItemIndex == i)
                emit selectedModInfoChanged();

            QModelIndex index = QAbstractItemModel::createIndex(i, 0);;
            emit dataChanged(index, index);

            break;
        }
    }
}

const QString &BalanceModPage::currentModInGame() const
{
    return m_currentModInGame;
}

void BalanceModPage::setCurrentModInGame(const QString &newCurrentModInGame)
{
    if (m_currentModInGame == newCurrentModInGame)
        return;
    m_currentModInGame = newCurrentModInGame;
    emit currentModInGameChanged();
}

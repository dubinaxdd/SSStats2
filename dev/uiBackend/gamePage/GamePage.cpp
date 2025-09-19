#include "GamePage.h"
#include <QDebug>

GamePage::GamePage(SettingsController *settingsController, QObject *parent)
    : QAbstractListModel{parent}
    , p_settingsController(settingsController)
{}

QVariant GamePage::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= p_gamePathArray->count())
        return QVariant();

    switch (role) {
        case GameNameRole: return p_gamePathArray->at(index.row()).uiGameName;
        case GameTypeRole: return p_gamePathArray->at(index.row()).gameType;
    }

    return QVariant();
}

int GamePage::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return p_gamePathArray->count();
}

void GamePage::setCurrentGame(GamePath *newCurrentGame)
{
    m_currentGame = newCurrentGame;
}

void GamePage::setGamePathArray(QVector<GamePath> *newGamePathArray)
{
    beginResetModel();
    p_gamePathArray = newGamePathArray;
    endResetModel();

    bool finded = false;

    for (int i = 0; i < p_gamePathArray->count(); i++)
    {
        if (p_settingsController->getSettings()->lastGamePath == p_gamePathArray->at(i).gamePath)
        {
            finded = true;
            setCurrentItem(i);
        }
    }

    if (!finded && !p_gamePathArray->isEmpty())
    {
        p_settingsController->getSettings()->lastGamePath = p_gamePathArray->first().gamePath;
        p_settingsController->saveSettings();
    }
}

void GamePage::updateCurrentGame(int itemIndex)
{
    *m_currentGame = p_gamePathArray->at(itemIndex);
    emit currentGameChanged();

    p_settingsController->getSettings()->lastGamePath = m_currentGame->gamePath;
    p_settingsController->saveSettings();
}

GameType::GameTypeEnum GamePage::getCurrentGameType()
{
    return m_currentGame->gameType;
}

int GamePage::getCurrentItem()
{
    return m_currentItem;
}

QHash<int, QByteArray> GamePage::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[GameNameRole] = "gameName";
    roles[GameTypeRole] = "gameType";

    return roles;
}

void GamePage::setCurrentItem(int newCurrentItem)
{
    m_currentItem = newCurrentItem;
    emit currentItemChanged();

    updateCurrentGame(m_currentItem);
}

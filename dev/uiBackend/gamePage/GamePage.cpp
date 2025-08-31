#include "GamePage.h"
#include <QDebug>

GamePage::GamePage(QObject *parent)
    : QAbstractListModel{parent}
{}

QVariant GamePage::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= p_gamePathArray->count())
        return QVariant();

    switch (role) {
        case GameName: return p_gamePathArray->at(index.row()).uiGameName;
        case GameType: return p_gamePathArray->at(index.row()).gameType;
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
}

void GamePage::updateCurrentGame(int itemIndex)
{
    *m_currentGame = p_gamePathArray->at(itemIndex);
    emit currentGameChanged();
}

QHash<int, QByteArray> GamePage::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[GameName] = "gameName";
    roles[GameType] = "gameType";

    return roles;
}

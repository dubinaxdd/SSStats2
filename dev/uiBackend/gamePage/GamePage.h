#ifndef GAMEPAGE_H
#define GAMEPAGE_H

#include <QAbstractListModel>
#include <baseTypes.h>

class GamePage : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit GamePage(QObject *parent = nullptr);

    enum DataRoles {
        GameName = Qt::UserRole + 1,
        GameType
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount( const QModelIndex& parent ) const override;

    void setCurrentGame(GamePath *newCurrentGame);
    void setGamePathArray(QVector<GamePath> *newGamePathArray);

    Q_INVOKABLE void updateCurrentGame(int itemIndex);

signals:
    void currentGameChanged();

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    GamePath* m_currentGame;
    QVector<GamePath> *p_gamePathArray;
};

#endif // GAMEPAGE_H

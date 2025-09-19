#ifndef GAMEPAGE_H
#define GAMEPAGE_H

#include <QAbstractListModel>
#include <baseTypes.h>
#include <settingsController.h>

class GamePage : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(GameType::GameTypeEnum currentGameType READ getCurrentGameType NOTIFY currentGameChanged)
    Q_PROPERTY(int currentItem READ getCurrentItem WRITE setCurrentItem NOTIFY currentItemChanged FINAL)
public:
    explicit GamePage(SettingsController *settingsController, QObject *parent = nullptr);

    enum DataRoles {
        GameNameRole = Qt::UserRole + 1,
        GameTypeRole
    };

    Q_INVOKABLE QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount( const QModelIndex& parent ) const override;

    void setCurrentGame(GamePath *newCurrentGame);
    void setGamePathArray(QVector<GamePath> *newGamePathArray);

    Q_INVOKABLE void updateCurrentGame(int itemIndex);
    Q_INVOKABLE GameType::GameTypeEnum getCurrentGameType();
    Q_INVOKABLE int getCurrentItem();

    void setCurrentItem(int newCurrentItem);

signals:
    void currentGameChanged();
    void currentItemChanged();

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    GamePath* m_currentGame;
    QVector<GamePath> *p_gamePathArray;
    SettingsController *p_settingsController;
    int m_currentItem = 0;

};

#endif // GAMEPAGE_H

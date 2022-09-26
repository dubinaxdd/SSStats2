#ifndef STATISTICPANEL_H
#define STATISTICPANEL_H

#include <baseTypes.h>
#include <QObject>
#include <QImage>
#include <imageProvider.h>
#include <statisticPanelItem.h>
#include <QAbstractListModel>

class StatisticPanel :  public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool expandPatyStatistic MEMBER m_expandPatyStatistic NOTIFY expandPatyStatisticChanged)
    Q_PROPERTY(StatisticPanelItem* curentPlayerStatsItem READ getCurentPlayerStatsItem NOTIFY playersItemsInitialized)

public:
    explicit StatisticPanel(ImageProvider *imageProvider, QObject *parent = nullptr);


    enum DataRoles {
        PlayerName = Qt::UserRole + 1,
        PlayerMmr = Qt::UserRole + 2,
        PlayerMmr1v1 = Qt::UserRole + 3,
        PlayerGamesCount = Qt::UserRole + 4,
        PlayerRace = Qt::UserRole + 5,
        PlayerWinRate = Qt::UserRole + 6,
        PlayerApm = Qt::UserRole + 7,
        PlayerIsBanned = Qt::UserRole + 8,
        PlayerAvatarId = Qt::UserRole + 9,
        SteamId = Qt::UserRole + 10,
        CalibrateGamesLeft = Qt::UserRole + 11,
        IsOnline = Qt::UserRole + 12,
        IsRanked = Qt::UserRole + 13,
        PlayerVisible = Qt::UserRole
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount( const QModelIndex& parent ) const override;

protected:
   QHash<int, QByteArray> roleNames() const override;

public:
    void setExpandPatyStatistic(bool newExpandPatyStatistic);
    void setBlockUpdate(bool newBlockUpdate);

signals:
    void currentPlayerStatsChanged();
    void expandPatyStatisticChanged();
    void playersItemsInitialized();

public slots:
    void receiveServerPlayerStats(ServerPlayerStats serverPlayerStats);
    void receivePlayersInfoMapFromScanner(QList<PlayerInfoFromDowServer> playersInfo, int playersCount);
    void receiveCurrentPlayerHostState(bool isHost);
    void receivePlyersRankedState(QVector<PlyersRankedState> plyersRankedState);

    void onQuitParty();

private:
    StatisticPanelItem *getCurentPlayerStatsItem();
    void sortStatsBySoloMmr();

private:
    StatisticPanelItem* m_curentPlayerStatsItem;
    QVector<StatisticPanelItem*> m_playersStatsItems;

    PlayerInfoFromDowServer m_crrentPlayerInfo;
    QList<PlayerInfoFromDowServer> m_playersInfo;

    ImageProvider *m_imageProvider;
    int m_playersCount = 0;
    bool m_expandPatyStatistic = false;
    bool currentPlayerIsHost = false;

    bool m_blockUpdate = false;
};

#endif // STATISTICPANEL_H

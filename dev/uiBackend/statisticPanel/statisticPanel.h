#ifndef STATISTICPANEL_H
#define STATISTICPANEL_H

#include <baseTypes.h>
#include <QObject>
#include <QImage>
#include <imageProvider.h>
#include <statisticPanelItem.h>
#include <QAbstractListModel>
#include <core.h>

class Core;

class StatisticPanel :  public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool expandPatyStatistic MEMBER m_expandPatyStatistic NOTIFY expandPatyStatisticChanged)
    Q_PROPERTY(StatisticPanelItem* curentPlayerStatsItem READ getCurentPlayerStatsItem NOTIFY playersItemsInitialized)

public:
    explicit StatisticPanel(Core* core, ImageProvider *imageProvider, QObject *parent = nullptr);


    enum DataRoles {
        PlayerName = Qt::UserRole,
        PlayerMmr,
        PlayerMmr1v1,
        CustomGamesMmr,
        PlayerGamesCount,
        PlayerRace,
        PlayerWinRate,
        PlayerApm,
        PlayerIsBanned,
        PlayerAvatarId,
        SteamId,
        CalibrateGamesLeft,
        IsOnline,
        IsRanked,
        BanType,
        PlayerRank,
        PlayerVisible
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount( const QModelIndex& parent ) const override;

protected:
   QHash<int, QByteArray> roleNames() const override;

public:
    void setExpandPatyStatistic(bool newExpandPatyStatistic);
    void setBlockUpdate(bool newBlockUpdate);
    Q_INVOKABLE void updateStatistic();

signals:
    void currentPlayerStatsChanged();
    void expandPatyStatisticChanged();
    void playersItemsInitialized();

public slots:
    void receiveServerPlayerStats(ServerPlayerStats serverPlayerStats);
    void receivePlayresInfoFromDowServer(QList<PlayerInfoFromDowServer> playersInfo);
    void receivePlyersRankedState(QVector<PlyersRankedState> plyersRankedState);

    void onQuitParty();

private:
    StatisticPanelItem *getCurentPlayerStatsItem();
    void sortStatsBySoloMmr();

private:
    Core* m_corePtr;
    StatisticPanelItem* m_curentPlayerStatsItem;
    QVector<StatisticPanelItem*> m_playersStatsItems;

    PlayerInfoFromDowServer m_crrentPlayerInfo;

    ImageProvider *m_imageProvider;
    int m_playersCount = 0;
    bool m_expandPatyStatistic = false;

    bool m_blockUpdate = false;
};

#endif // STATISTICPANEL_H

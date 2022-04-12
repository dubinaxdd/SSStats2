#ifndef STATISTICPANEL_H
#define STATISTICPANEL_H

#include <baseTypes.h>
#include <QObject>
#include <QImage>
#include <imageprovider.h>
#include <statisticpanelitem.h>
#include <QAbstractListModel>

class StatisticPanel :  public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool expandPatyStatistic MEMBER m_expandPatyStatistic NOTIFY expandPatyStatisticChanged)

    Q_PROPERTY(StatisticPanelItem* curentPlayerStatsItem READ getCurentPlayerStatsItem NOTIFY playersItemsInitialized)
   /* Q_PROPERTY(StatisticPanelItem* player2StatsItem READ getPlayer2StatsItem NOTIFY playersItemsInitialized)
    Q_PROPERTY(StatisticPanelItem* player3StatsItem READ getPlayer3StatsItem NOTIFY playersItemsInitialized)
    Q_PROPERTY(StatisticPanelItem* player4StatsItem READ getPlayer4StatsItem NOTIFY playersItemsInitialized)
    Q_PROPERTY(StatisticPanelItem* player5StatsItem READ getPlayer5StatsItem NOTIFY playersItemsInitialized)
    Q_PROPERTY(StatisticPanelItem* player6StatsItem READ getPlayer6StatsItem NOTIFY playersItemsInitialized)
    Q_PROPERTY(StatisticPanelItem* player7StatsItem READ getPlayer7StatsItem NOTIFY playersItemsInitialized)
    Q_PROPERTY(StatisticPanelItem* player8StatsItem READ getPlayer8StatsItem NOTIFY playersItemsInitialized)*/
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
    void playersStatsChanged();
    void expandPatyStatisticChanged();
    void playersItemsInitialized();

public slots:
    void receiveServerPlayerStats(ServerPlayerStats serverPlayerStats);
    void receivePlayersCount(int playersCount);
   // void receivePlayersInfoMapFromScanner(QList<SearchStemIdPlayerInfo> playersInfo, int playersCount);
    void receiveCurrentPlayerHostState(bool isHost);
    void onQuitParty();

private:
    StatisticPanelItem *getCurentPlayerStatsItem();
   /* StatisticPanelItem *getPlayer2StatsItem();
    StatisticPanelItem *getPlayer3StatsItem();
    StatisticPanelItem *getPlayer4StatsItem();
    StatisticPanelItem *getPlayer5StatsItem();
    StatisticPanelItem *getPlayer6StatsItem();
    StatisticPanelItem *getPlayer7StatsItem();
    StatisticPanelItem *getPlayer8StatsItem();*/

private:
    StatisticPanelItem* m_curentPlayerStatsItem;
    QVector<StatisticPanelItem*> m_playersStatsItems;

    SearchStemIdPlayerInfo m_crrentPlayerInfo;
    //QList<SearchStemIdPlayerInfo> m_playersInfo;

    ImageProvider *m_imageProvider;
    int m_playersCount = 0;
    bool m_expandPatyStatistic = false;
    bool currentPlayerIsHost = false;

    bool m_blockUpdate = false;
};

#endif // STATISTICPANEL_H

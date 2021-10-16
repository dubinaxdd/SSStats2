#ifndef STATISTICPANEL_H
#define STATISTICPANEL_H

#include "../../baseTypes/baseTypes.h"
#include <QObject>
#include <QImage>
#include "../imageProvider/imageprovider.h"
#include "statisticpanelitem.h"

class StatisticPanel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool expandPatyStatistic MEMBER m_expandPatyStatistic NOTIFY expandPatyStatisticChanged)

    Q_PROPERTY(StatisticPanelItem* curentPlayerStatsItem READ getCurentPlayerStatsItem NOTIFY playersItemsInitialized)
    Q_PROPERTY(StatisticPanelItem* player2StatsItem READ getPlayer2StatsItem NOTIFY playersItemsInitialized)
    Q_PROPERTY(StatisticPanelItem* player3StatsItem READ getPlayer3StatsItem NOTIFY playersItemsInitialized)
    Q_PROPERTY(StatisticPanelItem* player4StatsItem READ getPlayer4StatsItem NOTIFY playersItemsInitialized)
    Q_PROPERTY(StatisticPanelItem* player5StatsItem READ getPlayer5StatsItem NOTIFY playersItemsInitialized)
    Q_PROPERTY(StatisticPanelItem* player6StatsItem READ getPlayer6StatsItem NOTIFY playersItemsInitialized)
    Q_PROPERTY(StatisticPanelItem* player7StatsItem READ getPlayer7StatsItem NOTIFY playersItemsInitialized)
    Q_PROPERTY(StatisticPanelItem* player8StatsItem READ getPlayer8StatsItem NOTIFY playersItemsInitialized)
public:
    explicit StatisticPanel(ImageProvider *imageProvider, QObject *parent = nullptr);

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
    void receivePlayersInfoMapFromScanner(QList<SearchStemIdPlayerInfo> playersInfo, int playersCount);
    void receiveCurrentPlayerHostState(bool isHost);

private:
    StatisticPanelItem *getCurentPlayerStatsItem();
    StatisticPanelItem *getPlayer2StatsItem();
    StatisticPanelItem *getPlayer3StatsItem();
    StatisticPanelItem *getPlayer4StatsItem();
    StatisticPanelItem *getPlayer5StatsItem();
    StatisticPanelItem *getPlayer6StatsItem();
    StatisticPanelItem *getPlayer7StatsItem();
    StatisticPanelItem *getPlayer8StatsItem();

private:
    StatisticPanelItem *m_curentPlayerStatsItem;
    StatisticPanelItem *m_playersStatsItems[7];
    ImageProvider *m_imageProvider;
    int m_playersCount = 0;
    bool m_expandPatyStatistic = false;
    bool currentPlayerIsHost = false;

    bool m_blockUpdate = false;
};

#endif // STATISTICPANEL_H

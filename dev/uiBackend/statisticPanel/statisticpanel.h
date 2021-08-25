#ifndef STATISTICPANEL_H
#define STATISTICPANEL_H

#include "../../baseTypes/baseTypes.h"
#include <QObject>
#include <QImage>
#include "../imageProvider/imageprovider.h"

class StatisticPanel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString currentPlayerApm MEMBER m_currentPlayerApm NOTIFY currentPlayerStatsChanged)
    Q_PROPERTY(QString currentPlayerGamesCount MEMBER m_currentPlayerGamesCount NOTIFY currentPlayerStatsChanged)
    Q_PROPERTY(QString currentPlayerMmr MEMBER m_currentPlayerMmr NOTIFY currentPlayerStatsChanged)
    Q_PROPERTY(QString currentPlayerMmr1v1 MEMBER m_currentPlayerMmr1v1 NOTIFY currentPlayerStatsChanged)
    Q_PROPERTY(QString currentPlayerName MEMBER m_currentPlayerName NOTIFY currentPlayerStatsChanged)
    Q_PROPERTY(QString currentPlayerRace MEMBER m_currentPlayerRace NOTIFY currentPlayerStatsChanged)
    Q_PROPERTY(QString currentPlayerWinRate MEMBER m_currentPlayerWinRate NOTIFY currentPlayerStatsChanged)
    Q_PROPERTY(QString currentPlayerWinsCount MEMBER m_currentPlayerWinsCount NOTIFY currentPlayerStatsChanged)

    Q_PROPERTY(bool player2StatsVisible MEMBER m_player2StatsVisible NOTIFY playersStatsChanged)
    Q_PROPERTY(bool player3StatsVisible MEMBER m_player3StatsVisible NOTIFY playersStatsChanged)
    Q_PROPERTY(bool player4StatsVisible MEMBER m_player4StatsVisible NOTIFY playersStatsChanged)
    Q_PROPERTY(bool player5StatsVisible MEMBER m_player5StatsVisible NOTIFY playersStatsChanged)
    Q_PROPERTY(bool player6StatsVisible MEMBER m_player6StatsVisible NOTIFY playersStatsChanged)
    Q_PROPERTY(bool player7StatsVisible MEMBER m_player7StatsVisible NOTIFY playersStatsChanged)
    Q_PROPERTY(bool player8StatsVisible MEMBER m_player8StatsVisible NOTIFY playersStatsChanged)

public:
    explicit StatisticPanel(ImageProvider *imageProvider, QObject *parent = nullptr);

signals:
    void currentPlayerStatsChanged();
    void playersStatsChanged();

public slots:
    void receiveServerPlayerStats(ServerPlayerStats serverPlayerStats);
    void receivePlayersCount(int playersCount);

private:
    ImageProvider *m_imageProvider;

    int m_playersCount;

    QString m_currentPlayerApm;
    QString m_currentPlayerGamesCount;
    QString m_currentPlayerMmr;
    QString m_currentPlayerMmr1v1;
    QString m_currentPlayerName;
    QString m_currentPlayerRace;
    QString m_currentPlayerWinRate;
    QString m_currentPlayerWinsCount;


    bool m_player2StatsVisible = false;
    bool m_player3StatsVisible = false;
    bool m_player4StatsVisible = false;
    bool m_player5StatsVisible = false;
    bool m_player6StatsVisible = false;
    bool m_player7StatsVisible = false;
    bool m_player8StatsVisible = false;

};

#endif // STATISTICPANEL_H

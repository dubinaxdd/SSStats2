#ifndef STATISTICPANELITEM_H
#define STATISTICPANELITEM_H

#include <QObject>
#include <baseTypes.h>

class StatisticPanelItem : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool     playerIsBanned   READ getPlayerIsBanned     NOTIFY playersStatsChanged)
    Q_PROPERTY(QString  playerName       READ getPlayerName         NOTIFY playersStatsChanged)
    Q_PROPERTY(QString  playerMmr        READ getPlayerMmr          NOTIFY playersStatsChanged)
    Q_PROPERTY(QString  playerMmr1v1     READ getPlayerMmr1v1       NOTIFY playersStatsChanged)
    Q_PROPERTY(QString  playerGamesCount READ getPlayerGamesCount   NOTIFY playersStatsChanged)
    Q_PROPERTY(QString  playerRace       READ getPlayerRace         NOTIFY playersStatsChanged)
    Q_PROPERTY(QString  playerWinRate    READ getPlayerWinRate      NOTIFY playersStatsChanged)
    Q_PROPERTY(QString  playerApm        READ getPlayerApm          NOTIFY playersStatsChanged)
    Q_PROPERTY(bool     itemVisible      READ getVisible            NOTIFY itemVisibleChanged)

public:
    explicit StatisticPanelItem(QObject *parent = nullptr);
    void setPlayersStats(ServerPlayerStats newPlayersStats);
    void setVisible(bool newVisible);
    void setPlayerSteamId(QString steamId);

    ServerPlayerStats playersStats();

signals:
    void playersStatsChanged();
    void itemVisibleChanged();

public:
    QString getPlayerName() const;
    QString getPlayerMmr() const;
    QString getPlayerMmr1v1() const;
    QString getPlayerGamesCount() const;
    QString getPlayerRace() const;
    QString getPlayerWinRate() const;
    QString getPlayerApm() const;
    bool    getPlayerIsBanned() const;
    bool    getVisible() const;
    int     getPosition() const;

    QString getRaceFromNumber(int raceNumber) const;

private:
    ServerPlayerStats m_playersStats;
    bool m_visible = false;


};

#endif // STATISTICPANELITEM_H

#ifndef STATISTICPANELITEM_H
#define STATISTICPANELITEM_H

#include <QObject>
#include <baseTypes.h>
#include <QUrl>

class StatisticPanelItem : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool     playerIsBanned   READ getPlayerIsBanned     NOTIFY playersStatsChanged)
    Q_PROPERTY(QString  playerName       READ getPlayerName         NOTIFY playersStatsChanged)
    Q_PROPERTY(QString  playerMmr        READ getPlayerMmr          NOTIFY playersStatsChanged)
    Q_PROPERTY(QString  playerMmr1v1     READ getPlayerMmr1v1       NOTIFY playersStatsChanged)
    Q_PROPERTY(QString  customGamesMmr   READ getCustomGamesMmr     NOTIFY playersStatsChanged)
    Q_PROPERTY(QString  playerGamesCount READ getPlayerGamesCount   NOTIFY playersStatsChanged)
    Q_PROPERTY(int      playerRank       READ getPlayerRank         NOTIFY playersStatsChanged)
    Q_PROPERTY(QString  playerRace       READ getPlayerRace         NOTIFY playersStatsChanged)
    Q_PROPERTY(QString  playerWinRate    READ getPlayerWinRate      NOTIFY playersStatsChanged)
    Q_PROPERTY(QString  playerApm        READ getPlayerApm          NOTIFY playersStatsChanged)
    Q_PROPERTY(bool     itemVisible      READ getVisible            NOTIFY itemVisibleChanged)
    Q_PROPERTY(QUrl     playerAvatarId   READ avatarId              NOTIFY playersStatsChanged)
    Q_PROPERTY(QString  steamId          READ getSteamId            NOTIFY playersStatsChanged)
    Q_PROPERTY(int  calibrateGamesLeft   READ getCalibrateGamesLeft NOTIFY playersStatsChanged)
    Q_PROPERTY(BanType::BanTypeEnum  banType        READ banType    NOTIFY playersStatsChanged)

    Q_PROPERTY(bool isRanked READ getIsRanked WRITE setIsRanked NOTIFY isRankedChanged)
    Q_PROPERTY(bool isOnline READ getIsOnline WRITE setIsOnline NOTIFY isOnlineChanged)

    Q_PROPERTY(bool relicStatsAvailable READ relicStatsAvailable NOTIFY playersStatsChanged)
    Q_PROPERTY(QString  relicCountry   READ getRelicCountry NOTIFY playersStatsChanged)
    Q_PROPERTY(int  relicGamesCount   READ getRelicGamesCount NOTIFY playersStatsChanged)
    Q_PROPERTY(int  relicWinCount   READ getRelicWinCount NOTIFY playersStatsChanged)
    Q_PROPERTY(int  rating_1x1   READ getRating_1x1 NOTIFY playersStatsChanged)
    Q_PROPERTY(int  rating_2x2   READ getRating_2x2 NOTIFY playersStatsChanged)
    Q_PROPERTY(int  rating_3x3   READ getRrating_3x3 NOTIFY playersStatsChanged)
    Q_PROPERTY(QString  race_1x1   READ getRace_1x1 NOTIFY playersStatsChanged)
    Q_PROPERTY(QString  race_2x2   READ getRace_2x2 NOTIFY playersStatsChanged)
    Q_PROPERTY(QString  race_3x3   READ getRace_3x3 NOTIFY playersStatsChanged)

public:
    explicit StatisticPanelItem(QObject *parent = nullptr);
    void setPlayersStats(ServerPlayerStats newPlayersStats);
    void setVisible(bool newVisible);
    void setPlayerSteamId(QString steamId);
    void setPlayerName(QString name);
    void setRelicStats(RelicStats relicStats);

    ServerPlayerStats playersStats();

signals:
    void playersStatsChanged();
    void itemVisibleChanged();
    void isRankedChanged();
    void isOnlineChanged();

public:
    QString getPlayerName() const;
    QString getPlayerMmr() const;
    QString getPlayerMmr1v1() const;
    QString getCustomGamesMmr() const;
    QString getPlayerGamesCount() const;
    int     getPlayerRank() const;
    QString getPlayerRace() const;
    QString getPlayerWinRate() const;
    QString getPlayerApm() const;
    bool    getPlayerIsBanned() const;
    bool    getVisible() const;
    int     getPosition() const;
    QImage  getAvatar() const;
    QString getSteamId() const;    
    int getCalibrateGamesLeft() const;
    BanType::BanTypeEnum banType() const;

    QString getRelicCountry() const;
    int getRelicGamesCount() const;
    int getRelicWinCount() const;
    int getRating_1x1() const;
    int getRating_2x2() const;
    int getRrating_3x3() const;
    QString getRace_1x1() const;
    QString getRace_2x2() const;
    QString getRace_3x3() const;

    QString getRaceFromNumber(int raceNumber) const;

    const QString &avatarId() const;
    void setAvatarId(const QString &newAvatarId);

    bool getIsRanked() const;
    void setIsRanked(bool newIsRanked);

    bool getIsOnline() const;
    void setIsOnline(bool newIsOnline);

    bool relicStatsAvailable() const;

private:
    ServerPlayerStats m_playersStats;
    RelicStats m_relicStats;
    bool m_relicStatsAvailable = false;
    QString m_avatarId = "";
    bool m_visible = false;
    bool isRanked = true;
    bool isOnline = false;
};

#endif // STATISTICPANELITEM_H

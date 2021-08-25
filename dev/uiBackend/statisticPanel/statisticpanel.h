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

    Q_PROPERTY(QString player2Name READ getPlayer2Name NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player3Name READ getPlayer3Name NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player4Name READ getPlayer4Name NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player5Name READ getPlayer5Name NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player6Name READ getPlayer6Name NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player7Name READ getPlayer7Name NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player8Name READ getPlayer8Name NOTIFY playersStatsChanged)

    Q_PROPERTY(QString player2Mmr READ getPlayer2Mmr NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player3Mmr READ getPlayer3Mmr NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player4Mmr READ getPlayer4Mmr NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player5Mmr READ getPlayer5Mmr NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player6Mmr READ getPlayer6Mmr NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player7Mmr READ getPlayer7Mmr NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player8Mmr READ getPlayer8Mmr NOTIFY playersStatsChanged)

    Q_PROPERTY(QString player2Mmr1v1 READ getPlayer2Mmr1v1 NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player3Mmr1v1 READ getPlayer3Mmr1v1 NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player4Mmr1v1 READ getPlayer4Mmr1v1 NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player5Mmr1v1 READ getPlayer5Mmr1v1 NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player6Mmr1v1 READ getPlayer6Mmr1v1 NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player7Mmr1v1 READ getPlayer7Mmr1v1 NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player8Mmr1v1 READ getPlayer8Mmr1v1 NOTIFY playersStatsChanged)

    Q_PROPERTY(QString player2GamesCount READ getPlayer2GamesCount NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player3GamesCount READ getPlayer3GamesCount NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player4GamesCount READ getPlayer4GamesCount NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player5GamesCount READ getPlayer5GamesCount NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player6GamesCount READ getPlayer6GamesCount NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player7GamesCount READ getPlayer7GamesCount NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player8GamesCount READ getPlayer8GamesCount NOTIFY playersStatsChanged)

    Q_PROPERTY(QString player2Race READ getPlayer2Race NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player3Race READ getPlayer3Race NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player4Race READ getPlayer4Race NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player5Race READ getPlayer5Race NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player6Race READ getPlayer6Race NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player7Race READ getPlayer7Race NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player8Race READ getPlayer8Race NOTIFY playersStatsChanged)

    Q_PROPERTY(QString player2WinRate READ getPlayer2WinRate NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player3WinRate READ getPlayer3WinRate NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player4WinRate READ getPlayer4WinRate NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player5WinRate READ getPlayer5WinRate NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player6WinRate READ getPlayer6WinRate NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player7WinRate READ getPlayer7WinRate NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player8WinRate READ getPlayer8WinRate NOTIFY playersStatsChanged)

    Q_PROPERTY(QString player2Apm READ getPlayer2Apm NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player3Apm READ getPlayer3Apm NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player4Apm READ getPlayer4Apm NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player5Apm READ getPlayer5Apm NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player6Apm READ getPlayer6Apm NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player7Apm READ getPlayer7Apm NOTIFY playersStatsChanged)
    Q_PROPERTY(QString player8Apm READ getPlayer8Apm NOTIFY playersStatsChanged)

public:
    explicit StatisticPanel(ImageProvider *imageProvider, QObject *parent = nullptr);

signals:
    void currentPlayerStatsChanged();
    void playersStatsChanged();

public slots:
    void receiveServerPlayerStats(ServerPlayerStats serverPlayerStats);
    void receivePlayersCount(int playersCount);
    void receivePlayersInfoMapFromScanner(QList<SearchStemIdPlayerInfo> playersInfo);


private:
    QString getPlayer2Name();
    QString getPlayer3Name();
    QString getPlayer4Name();
    QString getPlayer5Name();
    QString getPlayer6Name();
    QString getPlayer7Name();
    QString getPlayer8Name();

    QString getPlayer2Mmr();
    QString getPlayer3Mmr();
    QString getPlayer4Mmr();
    QString getPlayer5Mmr();
    QString getPlayer6Mmr();
    QString getPlayer7Mmr();
    QString getPlayer8Mmr();

    QString getPlayer2Mmr1v1();
    QString getPlayer3Mmr1v1();
    QString getPlayer4Mmr1v1();
    QString getPlayer5Mmr1v1();
    QString getPlayer6Mmr1v1();
    QString getPlayer7Mmr1v1();
    QString getPlayer8Mmr1v1();

    QString getPlayer2GamesCount();
    QString getPlayer3GamesCount();
    QString getPlayer4GamesCount();
    QString getPlayer5GamesCount();
    QString getPlayer6GamesCount();
    QString getPlayer7GamesCount();
    QString getPlayer8GamesCount();

    QString getPlayer2Race();
    QString getPlayer3Race();
    QString getPlayer4Race();
    QString getPlayer5Race();
    QString getPlayer6Race();
    QString getPlayer7Race();
    QString getPlayer8Race();

    QString getPlayer2WinRate();
    QString getPlayer3WinRate();
    QString getPlayer4WinRate();
    QString getPlayer5WinRate();
    QString getPlayer6WinRate();
    QString getPlayer7WinRate();
    QString getPlayer8WinRate();

    QString getPlayer2Apm();
    QString getPlayer3Apm();
    QString getPlayer4Apm();
    QString getPlayer5Apm();
    QString getPlayer6Apm();
    QString getPlayer7Apm();
    QString getPlayer8Apm();

    QString getRaceFromNumber(int raceNumber);

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

    QVector <ServerPlayerStats> m_playersStats;


};

#endif // STATISTICPANEL_H

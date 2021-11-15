#ifndef GAMEINFOREADER_H
#define GAMEINFOREADER_H

#include <QObject>
#include <QTimer>
#include "../../baseTypes/baseTypes.h"
#include "../../core/logger/logger.h"



class GameInfoReader : public QObject
{ 
    Q_OBJECT
public:
    explicit GameInfoReader(QString sspath, QObject *parent = nullptr);
    void ssWindowClosed();
    bool getGameInitialized();
    void checkGameInitialize();

    void setCurrentProfile(const QString &newCurrentProfile);
    void setGameLounched(bool newGameLounched);

private slots:
    void readGameInfo();
    void readGameParametresAfterStop();

public slots:
    void receiveAverrageApm(int apm);
    void receivePlayresStemIdFromScanner(QList<SearchStemIdPlayerInfo> playersInfoFromScanner , int playersCount);
    void receivePlayerStemIdForHostedGame(SearchStemIdPlayerInfo playerInfoFromScanner);



signals:
    void loadStarted(SsGameState gameCurrentState);         //Сигнал о начале загрузки игры/реплея/сохраненки
    void startingMission(SsGameState gameCurrentState);     //Сигнал о старте миссии после загрузки
    void gameOver();                                        //Сигнал о победе какой-то из сторон
    void gameStopped();                                     //Сигнал о завершении игры и выхода в меню
    void gameInitialized();                                 //Сигнал о инициализации игры, когда игра включилась и появилось меню
    void ssShutdown();                                      //Сигнал о выключении игры


    void sendReplayToServer(SendingReplayInfo replayInfo);

private:
    void checkCurrentMode();
    bool checkMissionSettingsValide(int gameType);

private:
    QTimer* m_gameInfoReadTimer;
    QString m_ssPath;
    QString m_currentProfile;
    QString m_currentMode;

    QVector<WinCondition> m_winCoditionsVector;

    SsState m_ssCurrentState = SsState::ssShutdowned;
    SsGameState m_gameCurrentState = SsGameState::unknown;

    int m_lastAverrageApm = 0;

    bool m_gameLounched = false;
    bool m_lastGameSettingsValide = false;
    bool m_gameWillBePlayed = false;

    int m_playersCountFromScanner;
    QList<SearchStemIdPlayerInfo> m_playersInfoFromScanner;
    QList<SearchStemIdPlayerInfo> m_allPlayersInfoFromScanner;
};

#endif // GAMEINFOREADER_H

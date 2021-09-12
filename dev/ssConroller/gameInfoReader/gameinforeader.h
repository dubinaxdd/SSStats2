#ifndef GAMEINFOREADER_H
#define GAMEINFOREADER_H

#include <QObject>
#include <QTimer>
#include "../../baseTypes/baseTypes.h"



class GameInfoReader : public QObject
{ 
    Q_OBJECT
public:
    explicit GameInfoReader(QString sspath, QObject *parent = nullptr);
    void ssWindowClosed();
    bool getGameInitialized();
    void checkGameInitialize();

private slots:
    void readGameInfo();
    void readGameParametresAfterStop();

signals:
    void loadStarted(SsGameState gameCurrentState);         //Сигнал о начале загрузки игры/реплея/сохраненки
    void startingMission(SsGameState gameCurrentState);     //Сигнал о старте миссии после загрузки
    void gameOver();                                        //Сигнал о победе какой-то из сторон
    void gameStopped();                                     //Сигнал о завершении игры и выхода в меню
    void gameInitialized();                                 //Сигнал о инициализации игры, когда игра включилась и появилось меню
    void ssShutdown();                                      //Сигнал о выключении игры

private:
    QTimer* m_gameInfoReadTimer;
    QString m_ssPath;

    SsState m_ssCurrentState = SsState::ssShutdowned;
    SsGameState m_gameCurrentState = SsGameState::unknown;
};

#endif // GAMEINFOREADER_H

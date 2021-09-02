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
    void gameStarted();         //Сигнал о начале загрузки игры
    void gameStopped();         //Сигнал о завершении игры и выхода в меню
    void gameLoaded();          //Сигнал о запуске загруженной из сохранения игры
    void playbackStarted();     //Сигнал о начале реплея
    void gameInitialized();     //Сигнал о инициализации игры, когда игра включилась и появилось меню
    void ssShutdown();          //Сигнал о выключении игры
    void startingMission(SsGameState gameCurrentState);     //Сигнал о старте миссии после загрузки

private:
    QTimer* m_gameInfoReadTimer;
    QString m_ssPath;

    SsState m_ssCurrentState = SsState::ssShutdowned;
    SsGameState m_gameCurrentState = SsGameState::unknown;
};

#endif // GAMEINFOREADER_H

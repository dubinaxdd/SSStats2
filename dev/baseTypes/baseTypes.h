#ifndef BASETYPES_H
#define BASETYPES_H


#include <QString>
#include <QImage>

enum SsState
{
    ssShutdowned = 0,           //Сс выключен
    ssInitialized = 1,          //Сс загружен
};


enum SsGameState
{
    unknown = 0,
    gameStoped = 1,             //Игра завершена
    gameLoadStarted = 2,        //Начало загрузки Игры
    playbackLoadStarted = 3,    //Начало загрузки Реплея
    savedGameLoadStarted = 4,   //Начало загрузки сохраненной игры
    gameStarted = 5,            //Начало игры
    playbackStarted = 6,        //Начало реплея
    savedGameStarted = 7,       //Начало загруженной игры
    unknownGameStarted= 8,      //Начало хрен пойми какой игры (работает когда включили статку посреди игры)
    gameOver = 9                //Победа какой-то команды
};

struct PlayerStats
{
    QString name = "";
    QString race = "";
    QString team = "";
};

struct ServerPlayerStats
{
    QString steamId;
    int apm;
    int gamesCount;
    int mmr;
    int mmr1v1;
    QString name;
    int race;
    int winRate;
    int winsCount;
    bool isBanned;
    QImage avatar;
    bool isCurrentPlayer = false;
    bool avatarAvailable = false;
    bool statsAvailable = false;
    int position = 0;
};

struct SearchStemIdPlayerInfo
{
    QString steamId;
    int position;
    QString name;
    bool closeConnection = false;
};

#endif // BASETYPES_H

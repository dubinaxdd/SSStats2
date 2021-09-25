#ifndef BASETYPES_H
#define BASETYPES_H


#include <QString>
#include <QImage>

enum WinCondition
{
    ANNIHILATE,
    ASSASSINATE,
    CONTROLAREA,
    DESTROYHQ,
    ECONOMICVICTORY,
    GAMETIMER,
    STRATEGICOBJECTIVE,
    SUDDENDEATH
};

enum FinalState
{
    unknown0 = 0,
    inGame = 1,
    unknown1 = 2,
    loser = 3,
    unknown2 = 4,
    winner = 5,

};

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

enum Race
{
    SpaceMarines = 0,
    ChaosMarines = 1,
    Orks = 2,
    Eldar = 3,
    ImperialGuard = 4,
    Necrons = 5,
    TauEmpire = 6,
    SistersOfBattle = 7,
    DarkEldar = 8
};

enum GameTypeForReplaySending
{
    GameType1x1 = 1,
    GameType2x2 = 2,
    GameType3x3 = 3,
    GameType4x4 = 4
};

struct PlayerStats
{
    QString name = "";
    QString race = "";
    QString team = "";
    FinalState finalState;
    bool pHuman = true;
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


//p1=Mr%20P&sid1=76561198005134194&r1=1&
//p2=El%20Patr%25C3%25B3n&sid2=76561197990989159&r2=3&
//p3=NeukenindeKeuken&sid3=76561197988617798&r3=1&w1=3&
//p4=Thanatos&sid4=76561199101288861&r4=4&w2=4&
//apm=72.8824&
//type=2&
//map=4P_DOOM_SPIRAL&
//gtime=340&
//sid=76561198005134194&
//mod=dxp2&
//winby=ANNIHILATE&
//version=108&
//key=LPzltucVp5Rd0xgs

struct PlayerInfoforReplaySendong
{
    QString playerName;
    QString playerSid;
    Race playerRace;
    bool isWinner;
};

struct SendingReplayInfo
{
    QList<PlayerInfoforReplaySendong> playersInfo;
    int apm;
    GameTypeForReplaySending gameType;
    QString mapName;
    int gameTime;
    QString mod;
    WinCondition winBy;
};

#endif // BASETYPES_H

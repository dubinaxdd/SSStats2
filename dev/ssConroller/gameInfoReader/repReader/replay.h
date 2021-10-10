#ifndef Replay_H
#define Replay_H
#include <QString>
#include <QMap>
#include <QVector>
#include "game_action.h"
#include "player.h"
#include "gamesettings.h"

class GameAction;

class Replay
{
public:
    Replay();
    ~Replay();

    void setFileName(QString name);
    int getPlayerApm(int id);
    QString getShortMapName() const;
    int getVersion() const;
    uint getTeamsCount() const;
    bool isAnyAverageAPMAbove(uint apm);
    bool isStandart(int game_type);

    GameSettings settings;
    enum ConditionValues
    {
        Annihilate = 1003066394, //767227721
        SuddenDeath = -1826760460,
        Assassinate = 200405640,
        EconomicVictory = -242444938,
        ControlArea = 735076042,
        DestroyHQ = 1509920563,
        TakeAndHold = 1959084950,
        GameTimer = 69421273
    };

    bool hasTakeAndHold;
    bool hasDestroyHQ;
    bool hasControlArea;
    bool hasEconomicVictory;
    bool hasSuddenDeath;
    bool hasAnnihilate;
    bool hasAssassinate;
    bool hasGameTimer;

    QString Hash;

    bool ReadedFully;
    ulong BeginVersion;
    uint BeginNAME;
    ulong BeginFOLDINFO;
    uint LengthFOLDINFO;
    ulong BeginDATABASE;
    uint LengthDATABASE;
    ulong BeginFOLDWMAN;
    uint LengthFOLDWMAN;

    uint DataBaseChunkSize;
    ulong BeginDataBaseChunkSize;

    QVector<ulong> BeginPlayerDiffirences;
    QMap<ulong, uint> FOLDGPLYz;
    QMap<ulong, uint> DATAINFOz;
    QList<Player *> Players;
    QList<GameAction *> Actions;

    QString fileName;
    QString Name;
    uint Duration;
    uint Slots;
    long PlayerStart;
    int PlayerCount;
    uint TeamsCount;
    uint MapSize;
    QString MapLocale;
    QString Map;
    uint MapCRC32;
    int Version;
    QString MOD;
    long ActionDBSize;
    uint TotalTicks;
    ulong ActionStart;
    uint ActionCount;
};

#endif // Replay_H

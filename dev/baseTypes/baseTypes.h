#ifndef BASETYPES_H
#define BASETYPES_H


#include <QString>

struct PlayerStats
{
    QString name = "";
    QString race = "";
    QString team = "";
};

struct ServerPlayrStats
{
    int apm;
    int gamesCount;
    int mmr;
    int mmr1v1;
    QString name;
    int race;
    int winRate;
    int winsCount;
    bool isCurrentPlayer;
};

#endif // BASETYPES_H

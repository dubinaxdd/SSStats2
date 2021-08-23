#ifndef BASETYPES_H
#define BASETYPES_H


#include <QString>
#include <QImage>

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
    QImage avatar;
    bool isCurrentPlayer = false;
    bool avatarAvailable = false;
    bool statsAvailable = false;
};

struct SearchStemIdPlaerInfo
{
    QString steamId;
    QString name;
    bool closeConnection = false;
};

#endif // BASETYPES_H

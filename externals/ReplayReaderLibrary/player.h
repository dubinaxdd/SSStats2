#ifndef PLAYER_H
#define PLAYER_H

#include <QString>

class Player
{
public:
    Player();
    ~Player();

    enum RaceEnum
    {
        Unknown = 0,
        SpaceMarines,
        Orks,
        Eldar,
        ChaosSpaceMarines,
        DarkEldar,
        Necrons,
        SistersOfBattle,
        Tau,
        ImperialGuard
    } ResolvedRace;

    enum PlayerTypeEnum
    {
        EmptySlot = 0,
        Host,
        Computer,
        Spectator,
        OtherPlayer
    };

    QString Name;
    QString Race;
    uint Type;
    uint Team;
    ushort ActionCount;
    uint LastActionTick;

    QString Bytes;
    ulong LenghtFOLDGPLY;

    QString getShortRaceName() const;
    QString getVeryShortRaceName() const;
    QString getResolvedTypeStr() const;
    int getResolvedRace() const;
    int getResolvedType() const;
    bool isEmpty() const;
    bool isSpectator() const;
};

#endif // PLAYER_H

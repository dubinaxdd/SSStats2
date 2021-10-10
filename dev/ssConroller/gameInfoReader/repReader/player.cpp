#include "player.h"

Player::Player()
{

}

Player::~Player()
{

}
bool Player::isSpectator() const
{
    return getResolvedType() == Spectator;
}

bool Player::isEmpty() const
{
    return Race.isNull() || Name.isNull()|| Team == 0;
}


int Player::getResolvedRace() const
{
    if (Race.isNull())
        return Unknown;
    if(Race.toLower()=="space_marine_race")
        return SpaceMarines;
    if(Race.toLower()=="ork_race")
        return Orks;
    if(Race.toLower()=="eldar_race")
        return Eldar;
    if(Race.toLower()=="chaos_marine_race")
        return ChaosSpaceMarines;
    if(Race.toLower()=="dark_eldar_race")
        return DarkEldar;
    if(Race.toLower()=="necron_race")
        return Necrons;
    if(Race.toLower()=="sisters_race")
        return SistersOfBattle;
    if(Race.toLower()=="tau_race")
        return Tau;
    if(Race.toLower()=="guard_race")
        return ImperialGuard;
    return Unknown;
}
QString Player::getShortRaceName() const
{
    switch (getResolvedRace()) {
    case 1:
        return QString("SM");
        break;
    case 2:
        return QString("OR");
        break;
    case 3:
        return QString("EL");
        break;
    case 4:
        return QString("CM");
        break;
    case 5:
        return QString("DE");
        break;
    case 6:
        return QString("NE");
        break;
    case 7:
        return QString("SB");
        break;
    case 8:
        return QString("TE");
        break;
    case 9:
        return QString("IG");
        break;
    default:
        return QString("BD");
        break;
    }
}

QString Player::getVeryShortRaceName() const
{
    switch (getResolvedRace()) {
    case 1:
        return QString("M");
        break;
    case 2:
        return QString("O");
        break;
    case 3:
        return QString("E");
        break;
    case 4:
        return QString("C");
        break;
    case 5:
        return QString("D");
        break;
    case 6:
        return QString("N");
        break;
    case 7:
        return QString("S");
        break;
    case 8:
        return QString("T");
        break;
    case 9:
        return QString("I");
        break;
    default:
        return QString("B");
        break;
    }
}

int Player::getResolvedType() const
{
    if (Name.isNull() || Race.isNull())
        return EmptySlot;

    switch (Type)
    {
        case 0: return Host;
        case 2: return OtherPlayer;
        case 4: return Spectator;
        case 7: return EmptySlot;
        case 1: return Computer;
        case 3: return Computer;
        case 11: return Computer;
        default: return EmptySlot;

    }
}

QString Player::getResolvedTypeStr() const
{
    if (Name.isNull() || Race.isNull())
        return QString("EmptySlot");

    switch (Type)
    {
        case 0: return QString("Host");
        case 2: return QString("OtherPlayer");
        case 4: return QString("Spectator");
        case 7: return QString("EmptySlot");
        case 1: return QString("Computer");
        case 3: return QString("Computer");
        case 11: return QString("Computer");
        default: return QString("EmptySlot");

    }
}

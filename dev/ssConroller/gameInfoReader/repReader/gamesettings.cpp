#include "gamesettings.h"
#include <QDebug>

GameSettings::GameSettings()
{
    StartRes      = 0;
    LockTeams     = 0;
    CheatsON      = 1;
    StartLocation = 0;
    GameSpeed     = 2;
    ResShare      = 1;
    ResRate       = 1;
}

GameSettings::~GameSettings()
{

}

bool GameSettings::isStandart(int game_type)
{
    if(StartRes     !=0){
        qDebug() << "Starting resources:" << getResolvedResources();
        return false;
    }
    if(game_type!=1&&LockTeams    !=0){
        qDebug() << "Lock teams:" << getResolvedLockTeams();
        return false;
    }
    if(CheatsON     !=1){
        qDebug() << "Enable cheats:" << getResolvedCheats();
        return false;
    }
    if(game_type!=1&&StartLocation!=0){
        qDebug() << "Start location:" << getResolvedPositions();
        return false;
    }
    if(GameSpeed    !=2){
        qDebug() << "Game speed:" << getResolvedGameSpeed();
        return false;
    }
    if(game_type!=1&&ResShare     !=1){
        qDebug() << "Resource Sharing:" << getResolvedResourceSharing();
        return false;
    }
    if(ResRate      !=1){
        qDebug() << "Resource Rate:" << getResolvedResourceSpeed();
        return false;
    }
    return true;
}


QString GameSettings::getResolvedResources() const
{
    switch (StartRes)
    {
        case 0: return "Standard";
        case 1: return "Quick-start";
        default: return "Unknown";
    }
}

QString GameSettings::getResolvedLockTeams() const
{
    switch (LockTeams)
    {
        case 0: return "Lock teams";
        case 1: return "Unlock teams";
        default: return "Unknown";
    }
}

QString GameSettings::getResolvedPositions() const
{
    switch (StartLocation)
    {
        case 0: return "Random";
        case 1: return "Fixed";
        default: return "Unknown";
    }
}


QString GameSettings::getResolvedCheats() const
{
    switch (CheatsON)
    {
        case 0: return "Yes";
        case 1: return "No";
        default: return "Unknown";
    }
}


QString GameSettings::getResolvedGameSpeed() const
{
    switch (GameSpeed)
    {
        case 0: return "Very slow";
        case 1: return "Slow";
        case 2: return "Normal";
        case 3: return "Fast";
        default: return "Unknown";
    }
}

QString GameSettings::getResolvedResourceSharing() const
{
    switch (ResShare)
    {
        case 0: return "Yes";
        case 1: return "No";
        default: return "Unknown";
    }
}

QString GameSettings::getResolvedResourceSpeed() const
{
    switch (ResRate)
    {
        case 0: return "Low";
        case 1: return "Standard";
        case 2: return "High";
        default: return "Unknown";
    }
}

QString GameSettings::getResolvedAIDiff() const
{
    switch (AIDiff)
    {
        case 0: return "Hard";
        case 1: return "Insane";
        case 2: return "Easy";
        case 3: return "Hard";
        case 4: return "Insane";
        default: return "Unknown";
    }
}





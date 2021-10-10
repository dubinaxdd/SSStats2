#ifndef GAMESETTINGS_H
#define GAMESETTINGS_H

#include <QString>

class GameSettings
{
public:
    GameSettings();
    ~GameSettings();

    int AIDiff;
    int StartRes;
    int LockTeams;
    int CheatsON;
    int StartLocation;
    int GameSpeed;
    int ResShare;
    int ResRate;

    QString getResolvedAIDiff() const;
    QString getResolvedResourceSpeed() const;
    QString getResolvedResourceSharing() const;
    QString getResolvedGameSpeed() const;
    QString getResolvedCheats() const;
    QString getResolvedResources() const;
    QString getResolvedLockTeams() const;
    QString getResolvedPositions() const;
    bool isStandart(int game_type);
};

#endif // GAMESETTINGS_H

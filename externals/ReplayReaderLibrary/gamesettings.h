#ifndef GAMESETTINGS_H
#define GAMESETTINGS_H

#include <QString>

namespace ReplayReader{

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
    int NoFlyers = 0;

    QString getResolvedAIDiff() const;
    QString getResolvedResourceSpeed() const;
    QString getResolvedResourceSharing() const;
    QString getResolvedGameSpeed() const;
    QString getResolvedCheats() const;
    QString getResolvedResources() const;
    QString getResolvedLockTeams() const;
    QString getResolvedPositions() const;
    QString getResolvedNoFlayers() const;
    bool isStandart(int game_type);
};
}
#endif // GAMESETTINGS_H

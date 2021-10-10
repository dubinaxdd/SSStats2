#ifndef REPREADER_H
#define REPREADER_H
#include "replay.h"
#include "player.h"
#include "extendedbinreader.h"
#include "game_action.h"
#include <QDataStream>
#include <QFile>

class RepReader
{
public:
    RepReader(QString fileName);
    ~RepReader();

    Replay replay;
    QFile *pfile;

    bool ReadReplayFully();
    bool ReadHeader();
    QByteArray getReplayData();
    bool convertReplayToSteamVersion();
    bool convertReplayTo1_2Version();
    QString RenameReplay();
    bool isStandart(int game_type);
    bool playerIsObserver(QString name);
    int GetAverageAPM(int id);
    void close();
    static QString getReplayMod(QString replay);
    static QString getReplayMap(QString replay);

private:

    Player *ReadPlayer();
    void ReadActionDetail();

    int _lastTick;
    ExtendedBinReader *BinReader;
};
#endif //REPREADER_H

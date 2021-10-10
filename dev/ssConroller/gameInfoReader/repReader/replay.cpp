#include "replay.h"
#include <QStringList>
#include <QDebug>

Replay::Replay()
{
    ReadedFully = false;
    hasAnnihilate       = true;
    hasDestroyHQ        = false;
    hasEconomicVictory  = false;
    hasSuddenDeath      = false;
    hasAssassinate      = false;
}

void Replay::setFileName(QString name)
{
    fileName = name;
}

Replay::~Replay()
{
    for(int i=0;i<Players.size();++i)
        delete Players[i];
}

QString Replay::getShortMapName() const
{
    QString name="";
    QStringList lst = Map.right(Map.size()-2).split('_');
    foreach (QString word, lst)
        name += word.left(1).toUpper();
    return name;
}

int Replay::getVersion() const
{
    return Version;
}

uint Replay::getTeamsCount() const
{
    if (Players.isEmpty())
        return 0;

    uint count = 0;
    foreach(Player *item, Players)
        count = qMax(count, item->Team);

    return count;
}


int Replay::getPlayerApm(int id)
{
//    Player *pl = Players.at(id);


    if (/*pl == 0 || pl->isEmpty() || pl->isSpectator() || */id >= PlayerCount || Actions.isEmpty() || Actions.size() == 0)
    {
//        qDebug() << Players[id]->getResolvedTypeStr();
        qDebug() << "Bad id or Actions is empty";
        return 0;
    }

    GameAction *last = Actions.last();
    auto ticksCount = last->Tick;

    int acts_count = 0;
    for(int i=0; i<Actions.size(); ++i)
        if(Actions[i]->PlayerNumber == id)
            ++acts_count;

    int apm = (int)(((double)acts_count/ ticksCount) * 8 * 60);

    return apm;
}

bool Replay::isAnyAverageAPMAbove(uint apm)
{
    if (PlayerCount == 0 || TotalTicks == 0)
        return false;
    return (((ActionDBSize * 8 * 60 / 80) / PlayerCount) / TotalTicks) >= apm;
}

bool Replay::isStandart(int game_type)
{
    if(!settings.isStandart(game_type))
        return false;

    if(!hasAnnihilate){
        qDebug() << "Win conditions do not contain \"Annihilate\"";
        return false;
    }
    if(hasDestroyHQ){
        qDebug() << "Win conditions contain \"Destroy HQ\"";
        return false;
    }
    if(hasEconomicVictory){
        qDebug() << "Win conditions contain \"Economic Victory\"";
        return false;
    }
    if(hasSuddenDeath){
        qDebug() << "Win conditions contain \"Sudden Death\"";
        return false;
    }
    if(hasAssassinate){
        qDebug() << "Win conditions contain \"Assassinate\"";
        return false;
    }
    return true;
}

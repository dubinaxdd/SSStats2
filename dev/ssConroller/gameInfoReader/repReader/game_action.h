#ifndef GAME_ACTION_H
#define GAME_ACTION_H

#include <QString>

class GameAction
{
public:
    GameAction(){}
    ~GameAction();

    int ActionLen;
    int Kind;
    int KindValue;
    int Tick;
    int Time;
    int PlayerId;
    qint16 PlayerNumber;
    qint16 PlayerNumber2;
    qint16 PlayerActionNumber;
    qint16 ActionType;
    qint16 Value;
    QByteArray AdditionalInfo;
    QByteArray ActionData2;
    bool IsForced;
    QString PlayerIdBytes;
    bool Lag;
    bool Chat;
    bool Empty;
};

#endif // GAME_ACTION_H

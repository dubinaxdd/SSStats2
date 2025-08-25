#ifndef SOULSTORMMEMORYREADER_H
#define SOULSTORMMEMORYREADER_H

#include <QObject>
#include <QCoreApplication>
#include <QMap>
#include <QTimer>
#include <windows.h>
#include <baseTypes.h>
#include <QMutex>

class GameMemoryReader : public QObject
{
    Q_OBJECT
public:
    explicit GameMemoryReader(QObject *parent = nullptr);

    void setSoulstormHwnd(HWND newSoulstormHwnd);

    QTimer *scanTimer() const;


    void setGameType(GameType newGameType);

signals:
    void sendSteamPlayersInfoMap(QList<PlayerInfoFromDowServer> playersInfo, int playersCount);
    void sendSteamPlayerInfoForHostedGame(PlayerInfoFromDowServer playersInfo);
    void sendSessionId(QString sessionId);
    void sendAuthKey(QString authKey);

public slots:
    void refreshSteamPlayersInfo();
    void findPlayerBySsId(int ssId, int playerPosititon);
    void findSessionId();
    void findAuthKey();
    void abort();

private:
    QString findSteamSoulstormSessionId();
    QString findDefinitiveEditionSessionId();

private:
    unsigned char steamHeader[18] =  { 0x18, 0x0, 0x0, 0x0, 0x2F, 0x0, 0x73, 0x0, 0x74, 0x0, 0x65, 0x0, 0x61, 0x0, 0x6D, 0x0, 0x2F, 0x0 };
    unsigned char playresCountPostfix[8] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    unsigned char playresCountPostfix2[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    unsigned char sessionHeader[10] = { 0x73, 0x65, 0x73, 0x73, 0x69, 0x6F, 0x6E, 0x49, 0x44, 0x3D};
    unsigned char authHeader[20] = {0x70, 0x6C, 0x61, 0x74, 0x66, 0x6F, 0x72, 0x6D, 0x6C, 0x6F, 0x67, 0x69, 0x6E, 0x3F, 0x61, 0x75, 0x74, 0x68, 0x3D, 0x43};

    //QString authHeader = "platformlogin?auth=";
   // QByteArray sessionHeader = {"sessionID="};

    QTimer *m_scanTimer;
    HWND m_gameHwnd = NULL;
    QMutex m_playersSteamScannerMutex;
    bool m_abort = false;

    GameType m_gameType;
};
#endif // SOULSTORMMEMORYREADER_H

#ifndef SOULSTORMMEMORYREADER_H
#define SOULSTORMMEMORYREADER_H

#include <QObject>
#include <QCoreApplication>
#include <QMap>
#include <QTimer>
#include <windows.h>
#include <baseTypes.h>
#include <QMutex>

class SoulstormMemoryReader : public QObject
{
    Q_OBJECT
public:
    explicit SoulstormMemoryReader(QObject *parent = nullptr);

    void setSoulstormHwnd(HWND newSoulstormHwnd);

    QTimer *scanTimer() const;

signals:
    void sendSteamPlayersInfoMap(QList<SearchStemIdPlayerInfo> playersInfo, int playersCount);

    void sendSteamPlayerInfoForHostedGame(SearchStemIdPlayerInfo playersInfo);

    void sendSessionId(QString sessionId);

public slots:
    void refreshSteamPlayersInfo();
    void findPlayerBySsId(int ssId, int playerPosititon);
    void findSessionId();

private:
    unsigned char steamHeader[18] =  { 0x18, 0x0, 0x0, 0x0, 0x2F, 0x0, 0x73, 0x0, 0x74, 0x0, 0x65, 0x0, 0x61, 0x0, 0x6D, 0x0, 0x2F, 0x0 };
    unsigned char playresCountPostfix[8] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    unsigned char playresCountPostfix2[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    unsigned char sessionHeader[10] = { 0x73, 0x65, 0x73, 0x73, 0x69, 0x6F, 0x6E, 0x49, 0x44, 0x3D};
   // QByteArray sessionHeader = {"sessionID="};



    QTimer *m_scanTimer;
    HWND m_soulstormHwnd = NULL;
    QMutex m_playersSteamScannerMutex;
};
#endif // SOULSTORMMEMORYREADER_H

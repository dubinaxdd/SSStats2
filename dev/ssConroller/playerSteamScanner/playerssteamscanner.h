#ifndef PLAYERSSTEAMSCANNER_H
#define PLAYERSSTEAMSCANNER_H


#include <QObject>
#include <QCoreApplication>
#include <QMap>
#include <QTimer>
#include <windows.h>
#include "../../baseTypes/baseTypes.h"
#include <QMutex>


class PlayersSteamScanner : public QObject
{
    Q_OBJECT
public:
    explicit PlayersSteamScanner(QObject *parent = nullptr);

    void setSoulstormHwnd(HWND newSoulstormHwnd);

    QTimer *scanTimer() const;

signals:
    void sendSteamPlayersInfoMap(QList<SearchStemIdPlayerInfo> playersInfo, int playersCount);

private slots:
    void refreshSteamPlayersInfo();

private:
    unsigned char patyBlockHeader[4] = { 0x5B, 0x8C, 0x15, 0x77};
    unsigned char steamHeader[18] =  { 0x18, 0x0, 0x0, 0x0, 0x2F, 0x0, 0x73, 0x0, 0x74, 0x0, 0x65, 0x0, 0x61, 0x0, 0x6D, 0x0, 0x2F, 0x0 };
    unsigned char playresCountPostfix[8] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    unsigned char playresCountPostfix2[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


    QTimer *m_scanTimer;
    HWND m_soulstormHwnd = NULL;
    QMutex m_playersSteamScannerMutex;
};
#endif // PLAYERSSTEAMSCANNER_H

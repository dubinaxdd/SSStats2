#ifndef PLAYERSSTEAMSCANNER_H
#define PLAYERSSTEAMSCANNER_H


#include <QObject>
#include <QCoreApplication>
#include <QMap>
#include <QTimer>
#include <windows.h>
#include "../../baseTypes/baseTypes.h"

class PlayersSteamScanner : public QObject
{
    Q_OBJECT
public:
    explicit PlayersSteamScanner(QObject *parent = nullptr);

    void setSoulstormHwnd(HWND newSoulstormHwnd);

signals:
    void sendSteamPlayersInfoMap(QMap<QString, SearchStemIdPlaerInfo> infoMap);

private slots:
    void refreshSteamPlayersInfo();

private:
//0x5B, 0x8C, 0x15, 0x77, 0xC2, 0xEA, 0xC2, 0x32, 0x0, 0x0, 0xFE, 0x03, 0x00, 0x04, 0x0, 0x0, 0xD8, 0xF7, 0x78

    unsigned char patyBlockHeader[4] = { 0x5B, 0x8c, 0x15, 0x77};
    unsigned char steamHeader[18] =  { 0x18, 0x0, 0x0, 0x0, 0x2F, 0x0, 0x73, 0x0, 0x74, 0x0, 0x65, 0x0, 0x61, 0x0, 0x6D, 0x0, 0x2F, 0x0 };
    unsigned char playresCountPostfix[8] =  { 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    QTimer *m_scanTimer;
    HWND m_soulstormHwnd = NULL;
};
#endif // PLAYERSSTEAMSCANNER_H

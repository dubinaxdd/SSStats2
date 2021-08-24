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
    unsigned char steamHeader[18] =  { 0x18, 0x0, 0x0, 0x0, 0x2F, 0x0, 0x73, 0x0, 0x74, 0x0, 0x65, 0x0, 0x61, 0x0, 0x6D, 0x0, 0x2F, 0x0 };

    QTimer *m_scanTimer;
    HWND m_soulstormHwnd = NULL;
};
#endif // PLAYERSSTEAMSCANNER_H

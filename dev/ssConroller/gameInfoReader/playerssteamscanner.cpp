#include "playerssteamscanner.h"
//#include <QFile>
//#include <QDir>
#include <QVariantList>
//#include <QCoreApplication>
//#include <QSettings>
#include <QDebug>
#include <iostream>
#include <fstream>

#define SCAN_STEAM_PLAYERS_INTERVAL 2000

using namespace std;


PlayersSteamScanner::PlayersSteamScanner(QObject *parent)
    : QObject(parent)
{
    m_scanTimer = new QTimer(this);
    m_scanTimer->setInterval(SCAN_STEAM_PLAYERS_INTERVAL);
    QObject::connect(m_scanTimer, &QTimer::timeout, this, &PlayersSteamScanner::refreshSteamPlayersInfo, Qt::QueuedConnection );
    m_scanTimer->start();
}

void PlayersSteamScanner::refreshSteamPlayersInfo()
{

   // qDebug() << "Start scanning steam addresses";
    //HWND hWnd = FindWindow(nullptr, "Dawn of War: Soulstorm");



    if(!m_soulstormHwnd)
        return;

    DWORD PID;
    GetWindowThreadProcessId(m_soulstormHwnd, &PID);
    qDebug() << "PID = " << PID;

    // Получение дескриптора процесса
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, PID);
    if(hProcess==nullptr)
        return;

    QMap<QString, QString> allPlayersInfo;

    allPlayersInfo.insert("asdasd", "baneblade");

    int size = 0;

    QByteArray buffer(2000000, 0);
    //QByteArray buffer(30400, 0);

    sidsAddr[0] = (PVOID)0x155740B6;
    sidsAddr[1] = (PVOID)0x156B2DF6;
    sidsAddr[2] = (PVOID)0x15033F16;
    sidsAddr[3] = (PVOID)0x155B6A0E;
    sidsAddr[4] = (PVOID)0x15671A2E;
    sidsAddr[5] = (PVOID)0x155FE566;

    for(int k=0; k < 6; ++k)
    {
        PVOID readAddr = sidsAddr[k];
        SIZE_T bytesRead = 0;

        // Если функция вернула не ноль, то продолжим цикл
        if(!ReadProcessMemory(hProcess, readAddr, buffer.data(), /*30400*/2000000, &bytesRead))
        {
            if(GetLastError()!=299)
            {
                qDebug() << "Could not read process memory" << readAddr << GetLastError();
                continue;
            }
        }
         //qDebug() << k << "address of steam IDs:" << readAddr;
        for (int i = 0; i < static_cast<int>(bytesRead - 200); i++)
        {
            bool match = false;

            for (int j = 0; j < static_cast<int>(sizeof(steamHeader)); j++)
            {
                if (buffer.at(i + j) != steamHeader[j])
                {

                    match = false;
                    qDebug() << "FALSE" << j;
                    break;
                }
                else
                {
                    qDebug() << "TRUE" << j;
                    match = true;
                }
            }

            if (!match)
                continue;

            //qDebug() << "BANEBLADE";

            int nickPos = i + 56;

            if (buffer.at(nickPos) < 50 &&
                    buffer.at(nickPos) > 0 &&
                    buffer.at(nickPos + 1) == 0 &&
                    buffer.at(nickPos + 2) == 0 &&
                    buffer.at(nickPos + 3) == 0 &&
                    buffer.at(nickPos - 1) == 0 &&
                    buffer.at(nickPos - 2) == 0 &&
                    buffer.at(nickPos - 3) == 0 &&
                    buffer.at(nickPos - 4) == 0 &&
                    buffer.at(nickPos+4+buffer.at(nickPos)*2)   == 0 &&
                    buffer.at(nickPos+4+buffer.at(nickPos)*2+1) == 0 &&
                    buffer.at(nickPos+4+buffer.at(nickPos)*2+2) == 0 &&
                    buffer.at(nickPos+4+buffer.at(nickPos)*2+3) == 0)
            {

                qDebug() << "BANEBLADE";

                QString steamIdStr = QString::fromUtf16((ushort*)buffer.mid(i + 18, 34).data()).left(17);

                if(!steamIdStr.contains(QRegExp("^[0-9]{17}$")))
                    continue;
                //*if(get_stats&&!PlayersInfo.contains(steamIdStr))
                //*    PlayersInfo.append(steamIdStr);
                QString nick = QString::fromUtf16((ushort*)buffer.mid(nickPos + 4, buffer.at(nickPos) * 2).data()).left(buffer.at(nickPos));

                if(!allPlayersInfo.contains(steamIdStr)){
                    qDebug() << "Player found:" << nick << QString("http://steamcommunity.com/profiles/"+steamIdStr) << "at address" << k;
                    allPlayersInfo.insert(steamIdStr, nick);
                }
                else if(allPlayersInfo.value(steamIdStr)!=nick){
                    qDebug() << "Сейчас упаду как!";
                    allPlayersInfo[steamIdStr]=nick;
                }
                qDebug() << allPlayersInfo.keys();

            }
            else
                qDebug() << "отвали";
        }
    }
    //qDebug() << "BANEBLADE";
    qDebug() << allPlayersInfo.keys();
    emit sendSteamPlayersInfoMap(allPlayersInfo);
}

void PlayersSteamScanner::setSoulstormHwnd(HWND newSoulstormHwnd)
{
    m_soulstormHwnd = newSoulstormHwnd;
}

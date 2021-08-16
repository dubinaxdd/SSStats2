#include "playerssteamscanner.h"
#include <QFile>
#include <QDir>
#include <QVariantList>
#include <QCoreApplication>
#include <QSettings>
#include <QDebug>
#include <windows.h>
#include <iostream>
#include <fstream>

using namespace std;


void PlayersSteamScanner::refreshSteamPlayersInfo()
{
    AllPlayersInfo.clear();
   // qDebug() << "Start scanning steam addresses";
    HWND hWnd = FindWindow(nullptr, "Dawn of War: Soulstorm");
    DWORD PID;
    if(hWnd==nullptr){
        //qDebug() << "Could not find soulstorm process" << GetLastError();
        return;
    }
    GetWindowThreadProcessId(hWnd, &PID);
    qDebug() << "PID = " << PID;

    // Получение дескриптора процесса
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
                                       PROCESS_VM_READ, FALSE, PID);
    if(hProcess==nullptr){
        //qDebug() << "Could not open process" << GetLastError();
        return;
    }

    int size = 0;
//    if(!useOldSIDSearch){
    QByteArray buffer(2000000, 0);
    sidsAddr[0] = (PVOID)0x155740B6;
    sidsAddr[1] = (PVOID)0x156B2DF6;
    sidsAddr[2] = (PVOID)0x15033F16;
    sidsAddr[3] = (PVOID)0x155B6A0E;
    sidsAddr[4] = (PVOID)0x15671A2E;
    sidsAddr[5] = (PVOID)0x155FE566;
    for(int k=0; k<6; ++k){

        PVOID readAddr = sidsAddr[k];
        SIZE_T bytesRead = 0;

        // если функция вернула не ноль, то продолжим цикл
        if(!ReadProcessMemory(hProcess, readAddr, buffer.data(), 2000000, &bytesRead)){
            if(GetLastError()!=299)
                //qDebug() << "Could not read process memory" << readAddr << GetLastError();
            continue;
        }
        // qDebug() << i << "address of steam IDs:" << readAddr;
        for (int i = 0; i < static_cast<int>(bytesRead - 200); i++) {
            bool match = false;
            for (int j = 0; j < static_cast<int>(sizeof(steamHeader)); j++)
                if (buffer.at(i + j) != steamHeader[j]){
                    match = false;
                    break;
                }else match = true;
            if (!match)continue;
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
                buffer.at(nickPos+4+buffer.at(nickPos)*2+3) == 0) {
                QString steamIdStr = QString::fromUtf16((ushort*)buffer.mid(i + 18, 34).data()).left(17);
                if(!steamIdStr.contains(QRegExp("^[0-9]{17}$")))
                    continue;
                //*if(get_stats&&!PlayersInfo.contains(steamIdStr))
                //*    PlayersInfo.append(steamIdStr);
                QString nick = QString::fromUtf16((ushort*)buffer.mid(nickPos + 4, buffer.at(nickPos) * 2).data()).left(buffer.at(nickPos));

                if(!AllPlayersInfo.contains(steamIdStr)){
                    qDebug() << "Player found:" << nick << QString("http://steamcommunity.com/profiles/"+steamIdStr) << "at address" << k;
                    AllPlayersInfo.insert(steamIdStr, nick);
                }
                else if(AllPlayersInfo.value(steamIdStr)!=nick){
                    qDebug() << "Сейчас упаду как!";
                    AllPlayersInfo[steamIdStr]=nick;
                }
                //qDebug() << AllPlayersInfo.keys();

            }
        }
    }
}

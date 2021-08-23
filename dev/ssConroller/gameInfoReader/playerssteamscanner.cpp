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

    QMap<QString, SearchStemIdPlaerInfo> allPlayersInfo;

    //allPlayersInfo.insert("asdasd", "baneblade");

    int size = 0;

    QByteArray buffer(30400, 0);
    //QByteArray buffer(30400, 0);
    //QByteArray buffer(34200, 0);

    //CloseNetSession
//00 00 00 00 00 8B BA 0D 00 00 00 00 00 D4 CA 00 00 FF FF FF FF 05 0A 00 00 00 0F 00 00 00 43 6C 6F 73 65 4E 65 74 53 65 73 73 69 6F 6E 00 00 00 22

    //30 байт до строки CloseNetSession от конуа никнейма

    //sidsAddr[0] = (PVOID)0x155740B6;
    //sidsAddr[1] = (PVOID)0x156B2DF6;
   // sidsAddr[2] = (PVOID)0x15033F16;
    //sidsAddr[3] = (PVOID)0x155B6A0E;
    //sidsAddr[4] = (PVOID)0x15671A2E;
   // sidsAddr[5] = (PVOID)0x155FE566;
    //новые
    //sidsAddr[6] = (PVOID)0x1A40879D;

   // sidsAddr[6] = (PVOID)0x1AA3AD8A; //этот вродеб получше робит
   // sidsAddr[7] = (PVOID)0x1A882BCE;

    //sidsAddr[7] = (PVOID)0x1AA6792F;
    //sidsAddr[9] = (PVOID)0x1A8851F6;
    //sidsAddr[7] = (PVOID)0x1A3DF8BA;
    //sidsAddr[7] = (PVOID)0x1AA36430; //Этот вроде всех находит но не забывает предыдущие найденные



    //76561198041477216
//5B 8C 0C 77 C6 A6 4C ED 00 00 0D 04 30 25 00 00 C8 F6 6D 09 9E

   // 0x096DF488

   //unsigned long ptr1Count = 160116800;
    unsigned long ptr1Count = 0x00000000;
    while (ptr1Count < 0x7FFE0000)
    {

        //PVOID readAddr = sidsAddr[k];
        SIZE_T bytesRead = 0;

       // allPlayersInfo.clear();

        // Если функция вернула не ноль, то продолжим цикл
        if(!ReadProcessMemory(hProcess, (LPCVOID)ptr1Count/*readAddr*/, buffer.data(), 30400 , &bytesRead))
        {
            if(GetLastError()!=299)
            {
                qDebug() << "Could not read process memory" << ptr1Count << GetLastError();
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
                    //qDebug() << "FALSE" << j;
                    break;
                }
                else
                {
                    //qDebug() << "TRUE" << j;
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

               // qDebug() << "BANEBLADE";

                QString steamIdStr = QString::fromUtf16((ushort*)buffer.mid(i + 18, 34).data()).left(17);

                if(!steamIdStr.contains(QRegExp("^[0-9]{17}$")))
                    continue;
                //if(get_stats&&!PlayersInfo.contains(steamIdStr))
                //    PlayersInfo.append(steamIdStr);
                QString nick = QString::fromUtf16((ushort*)buffer.mid(nickPos + 4, buffer.at(nickPos) * 2).data()).left(buffer.at(nickPos));

                //qDebug() << "Player found:" << nick << QString("http://steamcommunity.com/profiles/"+steamIdStr) << "at address" << ptr1Count;

                QString fullString = QString::fromUtf8((char*)buffer.mid(nickPos + 4 + (nick.length()*2), 44).data(), 44);
               // qDebug() << Qt::hex << fullString.toLocal8Bit().toHex();

                QString closeConnectionFlag = QString::fromUtf8((char*)buffer.mid(nickPos + 4 + (nick.length()*2) + 29, 15).data(), 15);
                QString postfixFlag = QString::fromStdString(closeConnectionFlag.toLocal8Bit().toHex().toStdString());

               // qDebug() << postfixFlag;
               // qDebug() << postfixFlag.mid(2,6);
              //  qDebug() << postfix;
               // qDebug() << postfix2;

                if(!allPlayersInfo.contains(steamIdStr)){

                    SearchStemIdPlaerInfo newPlayerInPaty;

                    newPlayerInPaty.steamId = steamIdStr;
                    newPlayerInPaty.name = nick;

                    if(closeConnectionFlag == "CloseNetSession" /*|| postfixFlag == postfix || postfixFlag == postfix2*/ /*|| postfixFlag.mid(2,6) == postfix3*/)
                        newPlayerInPaty.closeConnection = true;

                   // qDebug() << "Новый";

                    allPlayersInfo.insert(steamIdStr, newPlayerInPaty);


                }
                else
                {
                    //qDebug() << "Уже есть";

                    if(allPlayersInfo.value(steamIdStr).name !=nick)
                    {
                   //     qDebug() << "Сейчас упаду как!";
                        allPlayersInfo[steamIdStr].name = nick;
                    }

                    if(closeConnectionFlag == "CloseNetSession" /*|| postfixFlag == postfix || postfixFlag == postfix2*/ /*|| postfixFlag.mid(2,6) == postfix3*/)
                        allPlayersInfo[steamIdStr].closeConnection = true;

                }
            }
        }

        ptr1Count += 30400;
    }



    for(int i = 0; i < allPlayersInfo.values().size(); i++)
    {
        if (allPlayersInfo.values().at(i).closeConnection)
        {
            qDebug() << allPlayersInfo.values().at(i).name;
            allPlayersInfo.remove(allPlayersInfo.values().at(i).steamId); //Потому что ключ в мапе совпадает со стим Ид
        }
    }

    qDebug() << "==============================================================";
    for(int i = 0; i < allPlayersInfo.values().size(); i++)
        qDebug() << allPlayersInfo.values().at(i).name;

    emit sendSteamPlayersInfoMap(allPlayersInfo);
}

void PlayersSteamScanner::setSoulstormHwnd(HWND newSoulstormHwnd)
{
    m_soulstormHwnd = newSoulstormHwnd;
}

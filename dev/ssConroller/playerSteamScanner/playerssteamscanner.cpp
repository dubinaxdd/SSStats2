#include "playerssteamscanner.h"
#include <QVariantList>
#include <QDebug>
#include <iostream>
#include <fstream>
#include <QTextCodec>
#include "../../core/logger/logger.h"

//#define SCAN_STEAM_PLAYERS_INTERVAL 3000

#define SCAN_STEAM_PLAYERS_INTERVAL 1000

using namespace std;


PlayersSteamScanner::PlayersSteamScanner(QObject *parent)
    : QObject(parent)
{
    m_scanTimer = new QTimer(this);
    m_scanTimer->setInterval(SCAN_STEAM_PLAYERS_INTERVAL);

    m_scanTimer->setSingleShot(true);

    QObject::connect(m_scanTimer, &QTimer::timeout, this, &PlayersSteamScanner::refreshSteamPlayersInfo, Qt::QueuedConnection );
   // m_scanTimer->start();
}

void PlayersSteamScanner::refreshSteamPlayersInfo()
{

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString ss = codec->toUnicode("Dawn of War: Soulstorm");
    LPCWSTR lps = (LPCWSTR)ss.utf16();

    m_soulstormHwnd = FindWindowW(NULL, lps);

    if(!m_soulstormHwnd)
        return;

    DWORD PID;
    GetWindowThreadProcessId(m_soulstormHwnd, &PID);
    //qDebug() << "PID = " << PID;

    // Получение дескриптора процесса
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, PID);
    if(hProcess==nullptr)
        return;

    //qInfo(logInfo()) << "Scan started";


    QMap<QString, SearchStemIdPlayerInfo> allPlayersInfo;

    int playersCount = 0;

    QByteArray buffer(/*30400*/100000, 0);


   //unsigned long ptr1Count = 160116800;
    unsigned long ptr1Count = /*100000000*/0x00000000;
    while (ptr1Count < /*200000000*/0x7FFE0000)
    {

        SIZE_T bytesRead = 0;

        // Если функция вернула не ноль, то продолжим цикл
        if(!ReadProcessMemory(hProcess, (LPCVOID)ptr1Count, buffer.data(), /*30400*/100000 , &bytesRead))
        {
            if(GetLastError()!=299)
            {
                qDebug() << "Could not read process memory" << ptr1Count << GetLastError();
                continue;
            }
        }

        int playerPosition = 0;

        //Ищем игроков в патиблоке
        for (int i = 100; i < static_cast<int>(bytesRead) - 300; i++)
        {
            bool match = false;

            for (int j = 0; j < static_cast<int>(sizeof(steamHeader)); j++)
            {
                if (buffer.at(i + j) != steamHeader[j])
                {
                    match = false;
                    break;
                }
                else
                    match = true;
            }

            if (!match)
                continue;


            bool match2 = false;

            //Ищем постфикс байта с количеством игроков по первой маске
            if (allPlayersInfo.count() == 0)
            {

                for (int k = i - 100; k < i; k++)
                {

                    for (int t = 0; t < static_cast<int>(sizeof(playresCountPostfix)); t++)
                    {
                        if (buffer.at(k + t) != playresCountPostfix[t])
                        {
                            match2= false;
                            break;
                        }
                        else
                            match2 = true;

                    }

                    if (match2)
                    {
                        //Дергаем байт с количеством игроков
                        playersCount = QString::fromUtf8((char*)buffer.mid(k - 1, 1).data(), 1).toLocal8Bit().toHex().toInt();
                        //qDebug() << Qt::hex << ptr1Count + k - 1;
                        //qDebug() << playersCount;
                        break;
                    }
                    else
                        playersCount = 0;
                }
            }
            //Ищем постфикс байта с количеством игроков по второй маске
           if(!match2)
            {
                if (allPlayersInfo.count() == 0)
                {

                    for (int k = i - 100; k < i; k++)
                    {

                        for (int t = 0; t < static_cast<int>(sizeof(playresCountPostfix2)); t++)
                        {
                            if (buffer.at(k + t) != playresCountPostfix2[t])
                            {
                                match2= false;
                                break;
                            }
                            else
                                match2 = true;
                        }

                        if (match2)
                        {
                            if(buffer.at(k + sizeof(playresCountPostfix2)) == 0)
                            {
                                match2 = false;
                                playersCount = 0;

                            }
                            else
                            {
                                //Дергаем байт с количеством игроков
                                playersCount = QString::fromUtf8((char*)buffer.mid(k - 1, 1).data(), 1).toLocal8Bit().toHex().toInt();
                                //qDebug() << Qt::hex << ptr1Count + k - 1;
                                //qDebug() << playersCount;
                                break;
                            }
                        }
                        else
                            playersCount = 0;
                    }
                }
            }

            if(playersCount == 0 || playersCount > 8)
                continue;

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

                QString steamIdStr = QString::fromUtf16((ushort*)buffer.mid(i + 18, 34).data()).left(17);

                if(!steamIdStr.contains(QRegExp("^[0-9]{17}$")))
                    continue;

                QString nick = QString::fromUtf16((ushort*)buffer.mid(nickPos + 4, buffer.at(nickPos) * 2).data()).left(buffer.at(nickPos));

                //qDebug() << "Player found:" << nick << QString("http://steamcommunity.com/profiles/"+steamIdStr) << "at address" << ptr1Count;

                //QString fullString = QString::fromUtf8((char*)buffer.mid(nickPos + 4 + (nick.length()*2), 100).data(), 100);
                //qDebug() << Qt::hex << fullString.toLocal8Bit().toHex();

                QString closeConnectionFlag = QString::fromUtf8((char*)buffer.mid(nickPos + 4 + (nick.length()*2) + 29, 15).data(), 15);
                //QString postfixFlag = QString::fromStdString(closeConnectionFlag.toLocal8Bit().toHex().toStdString());
                //qDebug() << closeConnectionFlag;

                //TODO: это тоже можно использовать как фильтр
                //4E 6F 20 6C 6F 6E 67 65 72 20 63 6F 6E 6E 65 63 74 65 64 20 74 6F 20 68 6F 73 74 00
                //No longer connection to host

                if(!allPlayersInfo.contains(steamIdStr)){

                    SearchStemIdPlayerInfo newPlayerInPaty;

                    newPlayerInPaty.steamId = steamIdStr;
                    newPlayerInPaty.name = nick;
                    newPlayerInPaty.position = playerPosition;
                    playerPosition++;

                    if(closeConnectionFlag == "CloseNetSession" || closeConnectionFlag == "No longer conne")
                        //newPlayerInPaty.closeConnection = true;
                        continue;

                    allPlayersInfo.insert(steamIdStr, newPlayerInPaty);
                }
                else
                {
                    if(allPlayersInfo.value(steamIdStr).name != nick)
                        allPlayersInfo[steamIdStr].name = nick;

                    if(closeConnectionFlag == "CloseNetSession" || closeConnectionFlag == "No longer conne")
                        //allPlayersInfo[steamIdStr].closeConnection = true;
                        continue;
                }
            }
        }

        if (allPlayersInfo.count() > 0 && playersCount > 1 && playersCount <= 8)
            break;


        playersCount = 0;
        allPlayersInfo.clear();
        ptr1Count += /*30400*/100000;
    }


    QList<SearchStemIdPlayerInfo> playersList;

    for(int i = 0; i < allPlayersInfo.values().size(); i++)
    {
        if (!allPlayersInfo.values().at(i).closeConnection)
            playersList.append(allPlayersInfo.values().at(i));
    }

    //qDebug() << "==============================================================";
    //for(int i = 0; i < allPlayersInfo.values().size(); i++)
        //qDebug() << playersList.name;


    emit sendSteamPlayersInfoMap(playersList, playersCount);

    // qInfo(logInfo()) << "Scan finished";
}


QTimer *PlayersSteamScanner::scanTimer() const
{
    return m_scanTimer;
}

void PlayersSteamScanner::setSoulstormHwnd(HWND newSoulstormHwnd)
{
    m_soulstormHwnd = newSoulstormHwnd;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlayersSteamScanner::findPlayerBySsId(int ssId, int playerPosititon)
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString ss = codec->toUnicode("Dawn of War: Soulstorm");
    LPCWSTR lps = (LPCWSTR)ss.utf16();

    m_soulstormHwnd = FindWindowW(NULL, lps);

    if(!m_soulstormHwnd)
        return;

    DWORD PID;
    GetWindowThreadProcessId(m_soulstormHwnd, &PID);

    // Получение дескриптора процесса
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, PID);
    if(hProcess==nullptr)
        return;

    SearchStemIdPlayerInfo playerInfo;

    int playersCount = 0;

    QByteArray buffer(/*30400*/100000, 0);


   //unsigned long ptr1Count = 160116800;
    unsigned long ptr1Count = /*100000000*/0x00000000;
    while (ptr1Count < /*200000000*/0x7FFE0000)
    {

        SIZE_T bytesRead = 0;

        // Если функция вернула не ноль, то продолжим цикл
        if(!ReadProcessMemory(hProcess, (LPCVOID)ptr1Count, buffer.data(), /*30400*/100000 , &bytesRead))
        {
            if(GetLastError()!=299)
            {
                qDebug() << "Could not read process memory" << ptr1Count << GetLastError();
                continue;
            }
        }

        int playerPosition = 0;

        //Ищем игроков в патиблоке
        for (int i = 100; i < static_cast<int>(bytesRead) - 300; i++)
        {
            bool match = false;

            for (int j = 0; j < static_cast<int>(sizeof(steamHeader)); j++)
            {
                if (buffer.at(i + j) != steamHeader[j])
                {
                    match = false;
                    break;
                }
                else
                    match = true;
            }

            if (!match)
                continue;

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

                QByteArray readedSsIdByteArray = buffer.mid(i - 8, 4).data();

                int readedSsId = *reinterpret_cast<int*>(readedSsIdByteArray.data());

                if (readedSsId != ssId)
                    continue;

                QString steamIdStr = QString::fromUtf16((ushort*)buffer.mid(i + 18, 34).data()).left(17);

                if(!steamIdStr.contains(QRegExp("^[0-9]{17}$")))
                    continue;

                QString nick = QString::fromUtf16((ushort*)buffer.mid(nickPos + 4, buffer.at(nickPos) * 2).data()).left(buffer.at(nickPos));

                playerInfo.steamId = steamIdStr;
                playerInfo.name = nick;
                playerInfo.position = playerPosititon;

                emit sendSteamPlayerInfoForHostedGame(playerInfo);

                return;

            }
        }

        ptr1Count += /*30400*/100000;
    }



}


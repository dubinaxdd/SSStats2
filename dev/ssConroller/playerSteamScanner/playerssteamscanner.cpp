#include "playerssteamscanner.h"
//#include <QFile>
//#include <QDir>
#include <QVariantList>
//#include <QCoreApplication>
//#include <QSettings>
#include <QDebug>
#include <iostream>
#include <fstream>
#include <QTextCodec>

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
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString ss = codec->toUnicode("Dawn of War: Soulstorm");
    LPCWSTR lps = (LPCWSTR)ss.utf16();

    m_soulstormHwnd = FindWindowW(NULL, lps);

    if(!m_soulstormHwnd)
        return;

    DWORD PID;
    GetWindowThreadProcessId(m_soulstormHwnd, &PID);
   // qDebug() << "PID = " << PID;

    // Получение дескриптора процесса
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, PID);
    if(hProcess==nullptr)
        return;

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

        bool patyMatch = false;

        //Ищем пати блок
        for (int x = 0; x < static_cast<int>(bytesRead - 10); x++)
        {
            for (int y = 0; y < static_cast<int>(sizeof(patyBlockHeader)); y++)
            {
                if (y == 1) //Потому что блять 0x8C нихера не ищется
                    y += 2;

                if (buffer.at(x + y) != patyBlockHeader[y])
                {
                    patyMatch = false;
                    break;
                }
                else
                {
                    patyMatch = true;
                }
            }

            if (patyMatch)
               break;

        }

        if(!patyMatch)
        {
            ptr1Count += 30400;
            continue;
        }

        //qDebug() << "Info: Paty blok in " << ptr1Count;

        int playerPosition = 0;

        //Ищем игроков в патиблоке
        for (int i = 0; i < static_cast<int>(bytesRead - 200); i++)
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


                if(!allPlayersInfo.contains(steamIdStr)){

                    SearchStemIdPlayerInfo newPlayerInPaty;

                    newPlayerInPaty.steamId = steamIdStr;
                    newPlayerInPaty.name = nick;
                    newPlayerInPaty.position = playerPosition;
                    playerPosition++;

                    //TODO: это тоже можно использовать как фильтр
                    //4E 6F 20 6C 6F 6E 67 65 72 20 63 6F 6E 6E 65 63 74 65 64 20 74 6F 20 68 6F 73 74 00
                    //No longer connection to host

                    if(closeConnectionFlag == "CloseNetSession")
                        newPlayerInPaty.closeConnection = true;

                    allPlayersInfo.insert(steamIdStr, newPlayerInPaty);
                }
                else
                {
                    if(allPlayersInfo.value(steamIdStr).name !=nick)
                    {
                   //     qDebug() << "Сейчас упаду как!";
                        allPlayersInfo[steamIdStr].name = nick;
                    }

                    if(closeConnectionFlag == "CloseNetSession")
                        allPlayersInfo[steamIdStr].closeConnection = true;
                }
            }
        }

        if (allPlayersInfo.count() > 0 && playersCount > 1 && playersCount <= 8)
        {
            break;
        }

        playersCount = 0;
        allPlayersInfo.clear();
        ptr1Count += 30400;
    }



    for(int i = 0; i < allPlayersInfo.values().size(); i++)
    {
        if (allPlayersInfo.values().at(i).closeConnection /*|| allPlayersInfo.values().at(i).position >= playersCount*/)
        {
            //qDebug() << allPlayersInfo.values().at(i).name;
            allPlayersInfo.remove(allPlayersInfo.values().at(i).steamId); //Потому что ключ в мапе совпадает со стим Ид
        }
    }

    //qDebug() << "==============================================================";
    //for(int i = 0; i < allPlayersInfo.values().size(); i++)
        //qDebug() << allPlayersInfo.values().at(i).name;


    emit sendSteamPlayersInfoMap(allPlayersInfo.values(), playersCount);
}

void PlayersSteamScanner::setSoulstormHwnd(HWND newSoulstormHwnd)
{
    m_soulstormHwnd = newSoulstormHwnd;
}

void PlayersSteamScanner::startScan()
{
    m_scanTimer->start();
}

void PlayersSteamScanner::stopScan()
{
    m_scanTimer->stop();
}

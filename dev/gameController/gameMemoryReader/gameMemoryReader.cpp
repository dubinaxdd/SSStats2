#include <gameMemoryReader.h>
#include <QVariantList>
#include <QDebug>
#include <iostream>
#include <fstream>
#include <QTextCodec>
#include <logger.h>
#include <ppl.h>

//#define SCAN_STEAM_PLAYERS_INTERVAL 3000

#define SCAN_STEAM_PLAYERS_INTERVAL 1000

using namespace std;

GameMemoryReader::GameMemoryReader(QObject *parent)
    : QObject(parent)
{
    m_scanTimer = new QTimer(this);
    m_scanTimer->setInterval(SCAN_STEAM_PLAYERS_INTERVAL);

    m_scanTimer->setSingleShot(true);

    QObject::connect(m_scanTimer, &QTimer::timeout, this, &GameMemoryReader::refreshSteamPlayersInfo, Qt::QueuedConnection );
    // m_scanTimer->start();
}

void GameMemoryReader::refreshSteamPlayersInfo()
{

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString ss = codec->toUnicode("Dawn of War: Soulstorm");
    LPCWSTR lps = (LPCWSTR)ss.utf16();

    m_gameHwnd = FindWindowW(NULL, lps);

    if(!m_gameHwnd)
        return;

    DWORD PID;
    GetWindowThreadProcessId(m_gameHwnd, &PID);
    //qDebug() << "PID = " << PID;

    // Получение дескриптора процесса
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, PID);
    if(hProcess==nullptr)
        return;

    //qInfo(logInfo()) << "Scan started";


    QMap<QString, PlayerInfoFromDowServer> allPlayersInfo;

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

                    PlayerInfoFromDowServer newPlayerInPaty;

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


    QList<PlayerInfoFromDowServer> playersList;

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


QTimer *GameMemoryReader::scanTimer() const
{
    return m_scanTimer;
}

void GameMemoryReader::setSoulstormHwnd(HWND newSoulstormHwnd)
{
    m_gameHwnd = newSoulstormHwnd;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GameMemoryReader::findPlayerBySsId(int ssId, int playerPosititon)
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString ss = codec->toUnicode("Dawn of War: Soulstorm");
    LPCWSTR lps = (LPCWSTR)ss.utf16();

    m_gameHwnd = FindWindowW(NULL, lps);

    if(!m_gameHwnd)
        return;

    DWORD PID;
    GetWindowThreadProcessId(m_gameHwnd, &PID);

    // Получение дескриптора процесса
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, PID);
    if(hProcess==nullptr)
        return;

    PlayerInfoFromDowServer playerInfo;

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

void GameMemoryReader::findSessionId()
{
    qDebug() << "GameMemoryReader::findSessionId() - Start find SessionId";

    m_dataFinded = false;
    DowServerRequestParametres parametres;
    QString gameName = "";
    DWORD64 step;

    if (m_gameType == GameType::GameTypeEnum::DefinitiveEdition)
    {
        QVector<DWORD64> numbers;
        step = 0x1000000000;
        gameName = "Warhammer 40,000: Dawn of War";

        for (DWORD64 i = 0x00000000000; i < 0x30000000000; i += step)
            numbers.append(i);

        HANDLE hProcess = getProcessHandle(gameName);

        if(hProcess == nullptr)
            return;

        concurrency::parallel_for_each(numbers.begin(), numbers.end(), [&](DWORD64 n) {
            parametres = findDefinitiveEditionSessionId(n, n+step, hProcess);

            if (!m_dataFinded && !parametres.sesionId.isEmpty() && !parametres.appBinaryChecksum.isEmpty() && !parametres.dataChecksum.isEmpty() && !parametres.modDLLChecksum.isEmpty())
            {
                emit sendDowServerRequestParametres(parametres);
                m_dataFinded = true;
                return;
            }
        });

        if (!m_dataFinded)
        {
            qWarning(logWarning()) << "Dow server request parametres not finded!!! sesionId:" << parametres.sesionId << "appBinaryChecksum:" << parametres.appBinaryChecksum
                               << "dataChecksum:" << parametres.dataChecksum << "modDLLChecksum:" << parametres.modDLLChecksum;

            emit sendDowServerRequestParametresError();
        }
    }
    else if (m_gameType == GameType::GameTypeEnum::SoulstormSteam)
    {
        parametres = findSteamSoulstormSessionId();

        if (!parametres.sesionId.isEmpty())
            emit sendDowServerRequestParametres(parametres);
        else
            qWarning(logWarning()) << "sessionId not finded!!!";
    }
    else
        return;
}

void GameMemoryReader::findAuthKey()
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString ss = codec->toUnicode("Dawn of War: Soulstorm");
    LPCWSTR lps = (LPCWSTR)ss.utf16();

    m_gameHwnd = FindWindowW(NULL, lps);

    if(!m_gameHwnd)
        return;

    DWORD PID;
    GetWindowThreadProcessId(m_gameHwnd, &PID);

    // Получение дескриптора процесса
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, PID);
    if(hProcess==nullptr)
        return;

    QByteArray buffer(10000, 0);

    unsigned long ptr1Count = 0x00000000;
    while (ptr1Count < 0x29000000)
    {
        if (m_abort)
            return;

        SIZE_T bytesRead = 0;

        // Если функция вернула не ноль, то продолжим цикл
        if(!ReadProcessMemory(hProcess, (LPCVOID)ptr1Count, buffer.data(), 10000 , &bytesRead))
        {
            if(GetLastError()!=299)
            {
                qDebug() << "Could not read process memory" << ptr1Count << GetLastError();
                continue;
            }
        }

        for (int i = 0; i < static_cast<int>(bytesRead) - 20; i++)
        {
            if (m_abort)
                return;

            bool match = false;

            for (int j = 0; j < 20; j++)
            {
                if (buffer.at(i + j) != authHeader[j])
                {
                    match = false;
                    break;
                }
                else
                    match = true;
            }

            if (!match)
                continue;

            //qDebug() << "MATCHED" <<  QString::fromUtf8((char*)buffer.mid(i, 1000).data());

            i = i + 19;

            QString authStr = "";

            int j = i;

            while (authStr.right(12) != "&accountType" && authStr.count() != 1000)
            {
                authStr += QString::fromUtf8((char*)buffer.mid(j, 1).data());
                j++;
            }

            //qDebug() << authStr;

            if (authStr.right(12) != "&accountType")
                continue;

            authStr = authStr.left(authStr.count() - 12);

            emit sendAuthKey(authStr);

            //qDebug() << authStr;

            return;
        }

        ptr1Count += 10000;
    }
}

void GameMemoryReader::abort()
{
    m_playersSteamScannerMutex.lock();
    m_abort = true;
    m_playersSteamScannerMutex.unlock();
}

void GameMemoryReader::findIgnoredPlayersId(QStringList playersIdList)
{
    m_ignoredPlayersIdFinded = false;
    QStringList findedPlayersIdList;

    QVector<DWORD64> numbers;

    DWORD64 step;
    QString gameName;

    if (m_gameType == GameType::GameTypeEnum::DefinitiveEdition)
    {
        step = 0x1000000000;
        gameName = "Warhammer 40,000: Dawn of War";

        for (DWORD64 i = 0x30000000000; i > 0x00000000000; i -= step)
            numbers.append(i);
    }
    else if (m_gameType == GameType::GameTypeEnum::SoulstormSteam)
    {
        step = 0x10000000;
        gameName = "Dawn of War: Soulstorm";

        for (DWORD64 i = 0x00000000; i < 0x80000000; i += step)
            numbers.append(i);
    }

    HANDLE hProcess = getProcessHandle(gameName);

    if(hProcess == nullptr)
        return;

    concurrency::parallel_for_each(numbers.begin(), numbers.end(), [&](DWORD64 n) {
        findedPlayersIdList = findIgnoredPlayersIdInMemorySection(n, n + step, playersIdList, hProcess);

        if (!findedPlayersIdList.isEmpty() && !m_ignoredPlayersIdFinded)
        {
            qInfo(logInfo()) << "Finded full list of players ID" << findedPlayersIdList;
            m_ignoredPlayersIdFinded = true;
            emit sendPlayersIdList(findedPlayersIdList);
        }
    });

    if (!m_ignoredPlayersIdFinded)
        qWarning(logWarning()) << "GameMemoryReader::findIgnoredPlaersId: players id not finded";
}

DowServerRequestParametres GameMemoryReader::findSteamSoulstormSessionId()
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");

    QString ss = codec->toUnicode("Dawn of War: Soulstorm");
    LPCWSTR lps = (LPCWSTR)ss.utf16();

    m_gameHwnd = FindWindowW(NULL, lps);

    if(!m_gameHwnd)
        return DowServerRequestParametres();

    DWORD PID;
    GetWindowThreadProcessId(m_gameHwnd, &PID);

    // Получение дескриптора процесса
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, PID);
    if(hProcess==nullptr)
        return DowServerRequestParametres();

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

        for (int i = 100; i < static_cast<int>(bytesRead) - 44; i++)
        {
            bool match = false;
            for (int j = 0; j < static_cast<int>(sizeof(sessionHeader)); j++)
            {
                if (buffer.at(i + j) != sessionHeader[j])
                {
                    match = false;
                    break;
                }
                else
                    match = true;
            }

            if (!match)
                continue;

            QString sessionIdStr = QString::fromUtf8((char*)buffer.mid(i, 44).data()).right(34);

            if (sessionIdStr.right(4) != "&ack")
                continue;

            sessionIdStr = sessionIdStr.left(30);


            DowServerRequestParametres parametres;
            parametres.sesionId = sessionIdStr;

            return parametres;
        }

        ptr1Count += 100000;
    }

    return DowServerRequestParametres();
}

DowServerRequestParametres GameMemoryReader::findDefinitiveEditionSessionId(DWORD64 startAdress, DWORD64 endAdress, HANDLE hProcess)
{
    int bufferSize = 2000000;
    QByteArray buffer(bufferSize, 0);
    DWORD64 ptr1Count = startAdress;
    DWORD64 ptr2Count = endAdress;

    QByteArray sesionIdHead = "sessionID=";
    QByteArray appBinaryChecksumHead = "appBinaryChecksum=";
    QByteArray dataChecksumHead = "dataChecksum=";
    QByteArray modDLLChecksumHead = "modDLLChecksum=";

    DowServerRequestParametres parametres;

    while (ptr1Count < ptr2Count)
    {
        if(m_dataFinded)
            return DowServerRequestParametres();

        DWORD64  bytesRead = 0;

        if(!ReadProcessMemory(hProcess, (LPCVOID)ptr1Count, buffer.data(), bufferSize , &bytesRead))
        {
                ptr1Count += bufferSize;
                continue;
        }

        //Как только входим в читабельную зону памяти уменьшаем размер буфера, для того что бы больше данных можно было прочесть
        bufferSize = 100000;

        if (parametres.sesionId.isEmpty() && buffer.contains( sesionIdHead ))
        {
            QString sessionIdStr = findParameter(&buffer, sesionIdHead, 30);

            if (!sessionIdStr.isEmpty())
                parametres.sesionId = sessionIdStr;
        }

        if (parametres.appBinaryChecksum.isEmpty() && buffer.contains( appBinaryChecksumHead ))
        {
            QString appBinaryChecksum = findChecksummParameter(&buffer, appBinaryChecksumHead);

            if (!appBinaryChecksum.isEmpty())
                parametres.appBinaryChecksum = appBinaryChecksum;
        }

        if (parametres.dataChecksum.isEmpty() && buffer.contains( dataChecksumHead ))
        {
            QString dataChecksum = findChecksummParameter(&buffer, dataChecksumHead);

            if (!dataChecksum.isEmpty())
                parametres.dataChecksum = dataChecksum;
        }

        if (parametres.modDLLChecksum.isEmpty() && buffer.contains( modDLLChecksumHead ))
        {
            QString modDLLChecksum = findChecksummParameter(&buffer, modDLLChecksumHead);

            if (!modDLLChecksum.isEmpty())
                parametres.modDLLChecksum = modDLLChecksum;
        }

        if (parametres.sesionId.isEmpty() || parametres.appBinaryChecksum.isEmpty() || parametres.dataChecksum.isEmpty() || parametres.modDLLChecksum.isEmpty())
        {
            ptr1Count += bufferSize;
            continue;
        }

        return parametres;
    }

    return DowServerRequestParametres();
}

QString GameMemoryReader::findParameter(QByteArray *buffer, QByteArray head, int length)
{
    int index = buffer->indexOf(head);

    QString parameterStr = QString::fromUtf8((char*)buffer->mid(index, head.length() + length).data() + 1);

    if(parameterStr.at(0) == "-")
    {
        length++;
        parameterStr = parameterStr.right(length);
    }
    else
        parameterStr = parameterStr.left(length + head.length()).right(length);

    if (parameterStr.count() != length)
        return "";

    return parameterStr;
}

QString GameMemoryReader::findChecksummParameter(QByteArray *buffer, QByteArray head)
{
    int index = buffer->indexOf(head) + head.length();

    QString symbolsString = "-0123456789";
    QString parameterStr = "";

    while (buffer->size() > index && symbolsString.contains(buffer->at(index)))
    {
        parameterStr.append(buffer->at(index));
        index++;

        if(parameterStr.size() > 30)
            break;
    }

    return parameterStr;
}

QStringList GameMemoryReader::findIgnoredPlayersIdInMemorySection(DWORD64 startAdress, DWORD64 endAdress, QStringList playersId, HANDLE hProcess)
{
    int bufferSize = 2000000;
    QByteArray buffer(bufferSize, 0);
    DWORD64 ptr1Count = startAdress;
    DWORD64 ptr2Count = endAdress;

    QByteArray searchedID = playersId.first().toLocal8Bit();
    QString maskString = "0123456789";

    while (ptr1Count < ptr2Count)
    {
        if(m_ignoredPlayersIdFinded)
            return QStringList();

        DWORD64  bytesRead = 0;

        if(!ReadProcessMemory(hProcess, (LPCVOID)ptr1Count, buffer.data(), bufferSize , &bytesRead))
        {
            //bufferSize = 2000000;

            ptr1Count += bufferSize;
            continue;

        }

        //Как только входим в читабельную зону памяти уменьшаем размер буфера, для того что бы больше данных можно было прочесть
        bufferSize = 200000;

        for (int i = 201; i < bytesRead/*buffer.size()*/ - 201; i++)
        {
            if(m_ignoredPlayersIdFinded)
                return QStringList();

            bool match = true;

            for (int j = 0; j < searchedID.size(); j++)
            {
                if (buffer.at(i + j) != searchedID[j])
                {
                    match = false;
                    break;
                }
            }

            if (!match)
                continue;

            auto temp2 = buffer.mid(i - 200, 400);

            bool allIdFinded = true;

            for(auto& needName2 : playersId)
            {
                if (!temp2.contains(needName2.toLocal8Bit()))
                {
                    allIdFinded = false;
                    i+=151;
                    break;
                }
            }

            if(allIdFinded)
            {
                if (playersId.count() > 2 && (temp2.contains("\"global\"")|| temp2.contains("[" + searchedID + ",") || temp2.contains("," + searchedID + ",") || temp2.contains("," + searchedID + "]") ) ||
                    playersId.count() == 1 && (temp2.contains("\"global\"")|| temp2.contains("[" + searchedID + ",") || temp2.contains("," + searchedID + "]") )
                    )
                {
                    qDebug() << "Second matched" << temp2;

                    QStringList idList;
                    QString currentId;

                    for (auto& symbol : temp2)
                    {
                        if (maskString.contains(symbol))
                        {
                            currentId.append(symbol);
                        }
                        else
                        {
                            if(currentId.size() == 8 && !idList.contains(currentId))
                                idList.append(currentId);

                            currentId.clear();
                        }
                    }

                    if(idList.count() == playersId.count() + 1)
                        return idList;

                }
            }
        }

        ptr1Count += bufferSize;
    }

    return QStringList();
}

HANDLE GameMemoryReader::getProcessHandle(QString gameName)
{

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    LPCWSTR lps = (LPCWSTR)gameName.utf16();
    m_gameHwnd = FindWindowW(NULL, lps);

    if(!m_gameHwnd)
    {
        qWarning(logWarning()) << "GameMemoryReader::getProcessHandle - Process Not Openned";
        return nullptr;
    }

    DWORD PID;
    GetWindowThreadProcessId(m_gameHwnd, &PID);

    // Получение дескриптора процесса
    HANDLE hProcess= OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, PID);

    if(hProcess==nullptr)
        qWarning(logWarning()) << "GameMemoryReader::getProcessHandle - Process handle not finded";

    return hProcess;

}

void GameMemoryReader::setGameType(GameType::GameTypeEnum newGameType)
{
    m_playersSteamScannerMutex.lock();
    m_gameType = newGameType;
    m_playersSteamScannerMutex.unlock();
}

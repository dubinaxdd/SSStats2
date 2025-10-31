#include <gameMemoryReader.h>
#include <QVariantList>
#include <QDebug>

#include <QTextCodec>
#include <logger.h>
#include <ppl.h>


using namespace std;

GameMemoryReader::GameMemoryReader(QObject *parent)
    : QObject(parent)
{
    m_ignoredPlayersIdFinded = false;
    m_dataFinded = false;
    m_abort = false;
}

void GameMemoryReader::findSessionId()
{
    qInfo(logInfo()) << "GameMemoryReader::findSessionId() - Start find SessionId";

    m_dataFinded = false;
    QString sessionId;
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
            sessionId = findDefinitiveEditionSessionId(n, n+step, hProcess);

            if(m_abort == true)
                return;

            if (!m_dataFinded && !sessionId.isEmpty())
            {
                qInfo(logInfo()) << "Session id finded:" << sessionId;
                emit sendSessionId(sessionId);
                m_dataFinded = true;
                return;
            }
        });

        if (!m_dataFinded)
        {
            qWarning(logWarning()) << "Session id not finded!!!";
            emit sendSessionIdError();
        }
    }
    else if (m_gameType == GameType::GameTypeEnum::SoulstormSteam)
    {
        sessionId = findSteamSoulstormSessionId();

        if (!sessionId.isEmpty())
        {
            qInfo(logInfo()) << "Session id finded:" << sessionId;
            emit sendSessionId(sessionId);
        }
        else
            qWarning(logWarning()) << "SessionId not finded!!!";
    }
    else
        return;
}

void GameMemoryReader::abort()
{
    m_abort = true;
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

        for (DWORD64 i = 0x30000000000; i > 0x00001000000; i -= step)
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

        if(m_abort == true)
            return;

        if (!findedPlayersIdList.isEmpty() && !m_ignoredPlayersIdFinded)
        {
            qInfo(logInfo()) << "Finded full list of players ID" << findedPlayersIdList;
            m_ignoredPlayersIdFinded = true;
            emit sendPlayersIdList(findedPlayersIdList);
        }
    });

    if (!m_ignoredPlayersIdFinded)
    {
        qWarning(logWarning()) << "GameMemoryReader::findIgnoredPlaersId: players id not finded";
        if (m_firstIgnoredPlayersSearch)
        {
            m_firstIgnoredPlayersSearch = false;
            findIgnoredPlayersId(playersIdList);
            qInfo(logInfo()) << "GameMemoryReader::findIgnoredPlaersId - start second find ignored players";
        }
    }

    m_firstIgnoredPlayersSearch = true;
}

QString GameMemoryReader::findSteamSoulstormSessionId()
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");

    QString ss = codec->toUnicode("Dawn of War: Soulstorm");
    LPCWSTR lps = (LPCWSTR)ss.utf16();

    m_gameHwnd = FindWindowW(NULL, lps);

    if(!m_gameHwnd)
        return QString();

    DWORD PID;
    GetWindowThreadProcessId(m_gameHwnd, &PID);

    // Получение дескриптора процесса
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, PID);
    if(hProcess==nullptr)
        return QString();

    QByteArray buffer(100000, 0);

    unsigned long ptr1Count = 0x00000000;
    while (ptr1Count < 0x7FFE0000)
    {
        if(m_abort)
            return QString();


        SIZE_T bytesRead = 0;

        // Если функция вернула не ноль, то продолжим цикл
        if(!ReadProcessMemory(hProcess, (LPCVOID)ptr1Count, buffer.data(), 100000 , &bytesRead))
        {
            if(GetLastError()!=299)
            {
                qDebug() << "Could not read process memory" << ptr1Count << GetLastError();
                continue;
            }
        }

        if(m_abort)
            return QString();

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

            return sessionIdStr;
        }

        ptr1Count += 100000;
    }

    return QString();
}

QString GameMemoryReader::findDefinitiveEditionSessionId(DWORD64 startAdress, DWORD64 endAdress, HANDLE hProcess)
{
    int bufferSize = 2000000;
    QByteArray buffer(bufferSize, 0);
    DWORD64 ptr1Count = startAdress;
    DWORD64 ptr2Count = endAdress;

    QByteArray sesionIdHead = "sessionID=";

    while (ptr1Count < ptr2Count)
    {
        if(m_dataFinded || m_abort)
            return QString();

        DWORD64  bytesRead = 0;

        if(!ReadProcessMemory(hProcess, (LPCVOID)ptr1Count, buffer.data(), bufferSize , &bytesRead))
        {
                ptr1Count += bufferSize;
                continue;
        }

        //Как только входим в читабельную зону памяти уменьшаем размер буфера, для того что бы больше данных можно было прочесть
        bufferSize = 100000;

        if (buffer.contains( sesionIdHead ))
        {
            QString sessionIdStr = findParameter(&buffer, sesionIdHead, 30);

            if (!sessionIdStr.isEmpty())
                return sessionIdStr;
        }

        ptr1Count += bufferSize;
    }

    return QString();
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

QStringList GameMemoryReader::findIgnoredPlayersIdInMemorySection(DWORD64 startAdress, DWORD64 endAdress, QStringList playerIdList, HANDLE hProcess)
{
    int bufferSize = 1000000;
    QByteArray buffer(bufferSize + 2000, 0);
    DWORD64 ptr1Count = startAdress;
    DWORD64 ptr2Count = endAdress;

    QByteArray searchedID = playerIdList.first().toLocal8Bit();
    QString maskString = "0123456789";

    QByteArray matchStartMessageHeader = "MatchStartMessage\",";

    while (ptr1Count < ptr2Count)
    {
        if(m_ignoredPlayersIdFinded || m_abort)
            return QStringList();

        DWORD64  bytesRead = 0;

        if(!ReadProcessMemory(hProcess, (LPCVOID)ptr1Count, buffer.data(), bufferSize + 2000 , &bytesRead))
        {
            ptr1Count += bufferSize;
            continue;
        }

        //Как только входим в читабельную зону памяти уменьшаем размер буфера, для того что бы больше данных можно было прочесть
        bufferSize = 200000;

        for (int i = 401; i < bytesRead - 401; i++)
        {
            if(m_ignoredPlayersIdFinded || m_abort)
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

            auto temp2 = buffer.mid(i - 400, 800);

            bool allIdFinded = true;

            for(auto& needName2 : playerIdList)
            {
                if (!temp2.contains(needName2.toLocal8Bit()))
                {
                    allIdFinded = false;
                    i+=200;
                    break;
                }
            }

            if(allIdFinded)
            {
                if (playerIdList.count() > 2 && (temp2.contains("\"global\"")|| temp2.contains("[" + searchedID + ",") || temp2.contains("," + searchedID + ",") || temp2.contains("," + searchedID + "]") ) ||
                    playerIdList.count() == 1 && (temp2.contains("\"global\"")|| temp2.contains("[" + searchedID + ",") || temp2.contains("," + searchedID + "]") )
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

                    if(idList.count() == playerIdList.count() + 1)
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
    m_mutex.lock();
    m_gameType = newGameType;
    m_mutex.unlock();
}

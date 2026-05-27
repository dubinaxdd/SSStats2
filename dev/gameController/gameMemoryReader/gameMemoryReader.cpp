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
    // 1. Поиск окна игры (современный и лаконичный способ для Qt)
    QString ss = QString::fromUtf8("Dawn of War: Soulstorm");
    HWND gameHwnd = FindWindowW(NULL, (LPCWSTR)ss.utf16());

    if (!gameHwnd)
        return QString();

    DWORD PID;
    GetWindowThreadProcessId(gameHwnd, &PID);

    // 2. Открытие процесса
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, PID);
    if (!hProcess)
        return QString();

    // Автоматическое закрытие дескриптора при выходе из функции
    struct HandleGuard { HANDLE h; ~HandleGuard() { if (h) CloseHandle(h); } } guard{ hProcess };

    // Подготовка сигнатуры для поиска (предполагаем, что sessionHeader определен в классе)
    // Если sessionHeader — это массив/вектор, берем его границы
    const char* sigStart = reinterpret_cast<const char*>(sessionHeader);
    const char* sigEnd = sigStart + sizeof(sessionHeader);

    std::vector<char> buffer;
    MEMORY_BASIC_INFORMATION mbi;

    // Границы поиска для 32-битного процесса Soulstorm
    DWORD64 currentAddress = 0x00000000;
    DWORD64 endAddress = 0x7FFE0000;

    // 3. Быстрый проход по регионам памяти
    while (currentAddress < endAddress)
    {
        if (m_abort)
            return QString();

        if (VirtualQueryEx(hProcess, (LPCVOID)currentAddress, &mbi, sizeof(mbi)) == 0)
        {
            currentAddress += 4096;
            continue;
        }

        DWORD64 regionEnd = (DWORD64)mbi.BaseAddress + mbi.RegionSize;
        if (regionEnd > endAddress)
            regionEnd = endAddress;

        // Проверяем только выделенную память с правами на чтение
        bool isReadable = (mbi.State == MEM_COMMIT) &&
                          ((mbi.Protect & PAGE_READONLY) ||
                           (mbi.Protect & PAGE_READWRITE) ||
                           (mbi.Protect & PAGE_EXECUTE_READ) ||
                           (mbi.Protect & PAGE_EXECUTE_READWRITE));

        if (isReadable && currentAddress < regionEnd)
        {
            DWORD64 bytesToRead = regionEnd - currentAddress;

            if (buffer.size() < bytesToRead)
                buffer.resize(bytesToRead);

            SIZE_T bytesRead = 0;
            if (ReadProcessMemory(hProcess, (LPCVOID)currentAddress, buffer.data(), bytesToRead, &bytesRead) && bytesRead > 0)
            {
                auto bufferStart = buffer.begin();
                auto bufferEnd = buffer.begin() + bytesRead;

                // 4. Векторизованный поиск сигнатуры вместо вложенных циклов
                auto it = std::search(bufferStart, bufferEnd, sigStart, sigEnd);

                while (it != bufferEnd)
                {
                    int offset = std::distance(bufferStart, it);

                    // Проверяем, достаточно ли данных осталось в буфере для извлечения ID (44 байта)
                    if (offset + 44 <= static_cast<int>(bytesRead))
                    {
                        // Извлекаем нужную подстроку напрямую из памяти без лишних аллокаций
                        QByteArray rawId(buffer.data() + offset, 44);
                        QString sessionIdStr = QString::fromUtf8(rawId).right(34);

                        if (sessionIdStr.right(4) == "&ack")
                        {
                            return sessionIdStr.left(30);
                        }
                    }

                    // Если нашли заголовок, но проверка не прошла, ищем следующий в этом же регионе
                    if (it + 1 < bufferEnd)
                        it = std::search(it + 1, bufferEnd, sigStart, sigEnd);
                    else
                        break;
                }
            }
        }

        currentAddress = regionEnd;
    }

    return QString();
}

QString GameMemoryReader::findDefinitiveEditionSessionId(DWORD64 startAdress, DWORD64 endAdress, HANDLE hProcess)
{
    // Сигнатуры для поиска
    const std::string head1 = "sessionID=";
    const std::string head2 = "\"sessionToken\":\"";

    // Временный буфер для чтения регионов памяти
    std::vector<char> buffer;

    MEMORY_BASIC_INFORMATION mbi;
    DWORD64 currentAddress = startAdress;

    // Перебираем страницы памяти с помощью VirtualQueryEx
    while (currentAddress < endAdress)
    {
        if (m_dataFinded || m_abort)
            return QString();

        // Запрашиваем информацию о текущем регионе памяти
        if (VirtualQueryEx(hProcess, (LPCVOID)currentAddress, &mbi, sizeof(mbi)) == 0)
        {
            // Если не удалось получить инфо, перешагиваем стандартную страницу 4KB
            currentAddress += 4096;
            continue;
        }

        DWORD64 regionEnd = (DWORD64)mbi.BaseAddress + mbi.RegionSize;

        // Ограничиваем поиск конечным адресом, заданным пользователем
        if (regionEnd > endAdress)
            regionEnd = endAdress;

        // Проверяем, что регион выделен (State == MEM_COMMIT)
        // и доступен для чтения (не PAGE_NOACCESS, не PAGE_GUARD)
        bool isReadable = (mbi.State == MEM_COMMIT) &&
                          ((mbi.Protect & PAGE_READONLY) ||
                           (mbi.Protect & PAGE_READWRITE) ||
                           (mbi.Protect & PAGE_EXECUTE_READ) ||
                           (mbi.Protect & PAGE_EXECUTE_READWRITE));

        if (isReadable && currentAddress < regionEnd)
        {
            DWORD64 bytesToRead = regionEnd - currentAddress;

            // Выделяем память под размер региона, если он изменился
            if (buffer.size() < bytesToRead)
                buffer.resize(bytesToRead);

            SIZE_T bytesRead = 0;
            // Читаем весь регион за один системный вызов
            if (ReadProcessMemory(hProcess, (LPCVOID)currentAddress, buffer.data(), bytesToRead, &bytesRead) && bytesRead > 0)
            {
                // Используем std::search (он оптимизирован на уровне компилятора)
                auto it = std::search(buffer.begin(), buffer.begin() + bytesRead, head1.begin(), head1.end());
                if (it != buffer.begin() + bytesRead)
                {
                    // Нашли первую сигнатуру. Преобразуем локальный срез в QByteArray
                    int offset = std::distance(buffer.begin(), it);
                    QByteArray subBuffer(buffer.data() + offset, min((int)(bytesRead - offset), 100));
                    QString sessionIdStr = findParameter(&subBuffer, QByteArray::fromStdString(head1), 30);
                    if (!sessionIdStr.isEmpty()) return sessionIdStr;
                }

                it = std::search(buffer.begin(), buffer.begin() + bytesRead, head2.begin(), head2.end());
                if (it != buffer.begin() + bytesRead)
                {
                    // Нашли вторую сигнатуру
                    int offset = std::distance(buffer.begin(), it);
                    QByteArray subBuffer(buffer.data() + offset, min((int)(bytesRead - offset), 100));
                    QString sessionIdStr = findParameter(&subBuffer, QByteArray::fromStdString(head2), 30);
                    if (!sessionIdStr.isEmpty()) return sessionIdStr;
                }
            }
        }

        // Переходим к следующему региону памяти Windows
        currentAddress = regionEnd;
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

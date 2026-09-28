#include <gameMemoryReader.h>
#include <QVariantList>
#include <QDebug>

#include <QTextCodec>
#include <logger.h>
#include <ppl.h>
#include <algorithm>

using namespace std;

GameMemoryReader::GameMemoryReader(QObject *parent)
    : QObject(parent)
{
    m_abort = false;
}

void GameMemoryReader::findSessionId()
{
    qInfo(logInfo()) << "GameMemoryReader::findSessionId() - Start find SessionId";

    if (m_gameType == GameType::GameTypeEnum::DefinitiveEdition)
    {
        QString sessionId = findDefinitiveEditionSessionId();

        if (!sessionId.isEmpty())
        {
            qInfo(logInfo()) << "Session ID found:" << sessionId;
            emit sendSessionId(sessionId);
        }
        else if (!m_abort)
        {
            qWarning(logWarning()) << "Session ID not found!";
            emit sendSessionIdError();
        }
    }
    else if (m_gameType == GameType::GameTypeEnum::SoulstormSteam)
    {
        QString sessionId = findSteamSoulstormSessionId();

        if (!sessionId.isEmpty())
        {
            qInfo(logInfo()) << "Session id finded:" << sessionId;
            emit sendSessionId(sessionId);
        }
        else
        {
            qWarning(logWarning()) << "SessionId not finded!!!";
            emit sendSessionIdError();
        }
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
    if (playersIdList.isEmpty())
        return;

    QStringList findedPlayersIdList = findIgnoredPlayersIdInMemory(playersIdList);

    if (!findedPlayersIdList.isEmpty())
    {
        emit sendPlayersIdList(findedPlayersIdList);
        return;
    }

    if (m_abort)
        return;

    qWarning(logWarning())<< "GameMemoryReader::findIgnoredPlayersId:" << "players ID not found";
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

QString GameMemoryReader::findDefinitiveEditionSessionId()
{
    const QString gameName = "Warhammer 40,000: Dawn of War";
    const QByteArray head1 = "sessionID=";
    const QByteArray head2 = "\"sessionToken\":\"";

    constexpr SIZE_T CHUNK_SIZE = 1024 * 1024; // 1 MB
    constexpr SIZE_T OVERLAP = 128;

    HANDLE hProcess = getProcessHandle(gameName);

    if (hProcess == nullptr)
    {
        qWarning(logWarning()) << "Cannot open process:" << gameName;
        return QString();
    }

    SYSTEM_INFO systemInfo{};
    GetNativeSystemInfo(&systemInfo);

    DWORD64 currentAddress = reinterpret_cast<DWORD64>(systemInfo.lpMinimumApplicationAddress);

    const DWORD64 maxAddress = reinterpret_cast<DWORD64>(systemInfo.lpMaximumApplicationAddress);
    MEMORY_BASIC_INFORMATION mbi{};

    QByteArray buffer;
    buffer.reserve(CHUNK_SIZE);

    while (currentAddress < maxAddress)
    {
        if (m_abort)
            return {};

        SIZE_T queryResult = VirtualQueryEx(
            hProcess,
            reinterpret_cast<LPCVOID>(currentAddress),
            &mbi,
            sizeof(mbi));

        if (queryResult == 0)
        {
            // Не получилось получить информацию о регионе.
            // Переходим на следующую страницу.
            currentAddress += 0x1000;
            continue;
        }

        const DWORD64 regionBase = reinterpret_cast<DWORD64>(mbi.BaseAddress);
        const DWORD64 regionEnd = regionBase + mbi.RegionSize;

        // Защита от переполнения/зацикливания.
        if (regionEnd <= currentAddress)
            break;

        // Не выходим за пределы адресного пространства.
        const DWORD64 scanEnd = min(regionEnd, maxAddress);

        // Проверяем, что регион действительно выделен и его можно читать.
        const DWORD protection = mbi.Protect & 0xFF;

        const bool isReadable =
            mbi.State == MEM_COMMIT &&
            !(mbi.Protect & PAGE_GUARD) &&
            protection != PAGE_NOACCESS &&
            (
                protection == PAGE_READONLY ||
                protection == PAGE_READWRITE ||
                protection == PAGE_WRITECOPY ||
                protection == PAGE_EXECUTE_READ ||
                protection == PAGE_EXECUTE_READWRITE ||
                protection == PAGE_EXECUTE_WRITECOPY
            );

        if (!isReadable)
        {
            currentAddress = scanEnd;
            continue;
        }

        DWORD64 address = currentAddress;

        // Храним конец предыдущего блока.
        // Это позволяет найти сигнатуру, которая находится на границе двух блоков.
        QByteArray tail;

        while (address < scanEnd)
        {
            if (m_abort)
                return {};

            const SIZE_T bytesToRead =
                static_cast<SIZE_T>(
                    std::min<DWORD64>(
                        CHUNK_SIZE,
                        scanEnd - address));

            buffer.resize(
                static_cast<qsizetype>(bytesToRead));

            SIZE_T bytesRead = 0;

            const BOOL result = ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(address), buffer.data(), bytesToRead, &bytesRead);
            Q_UNUSED(result);

            if (bytesRead == 0)
            {
                // Если весь блок прочитать не удалось,
                // пробуем перейти на следующую страницу.
                address += 0x1000;
                tail.clear();
                continue;
            }

            buffer.resize(static_cast<qsizetype>(bytesRead));

            // Добавляем хвост предыдущего блока.
            QByteArray data;

            if (!tail.isEmpty())
                data = tail + buffer;
            else
                data = buffer;

            auto findSessionIdBySignature = [&](const QByteArray& signature) -> QString
            {
                constexpr qsizetype TOKEN_LENGTH = 30;
                qsizetype searchPosition = 0;

                while (true)
                {
                    const qsizetype position = data.indexOf(signature, searchPosition);

                    if (position < 0)
                        return QString();

                    const qsizetype valueStart = position + signature.size();

                    // Проверяем, что все 30 байт доступны.
                    if (valueStart + TOKEN_LENGTH <= data.size())
                    {
                        bool valid = true;

                        for (qsizetype i = 0; i < TOKEN_LENGTH; ++i)
                        {
                            const char c = data.at(valueStart + i);

                            const bool isDigit = c >= '0' && c <= '9';
                            const bool isLower = c >= 'a' && c <= 'z';
                            const bool isUpper = c >= 'A' && c <= 'Z';

                            if (!isDigit && !isLower && !isUpper)
                            {
                                valid = false;
                                break;
                            }
                        }

                        if (valid)
                            return QString::fromLatin1(data.constData() + valueStart, TOKEN_LENGTH);
                    }

                    searchPosition = position + 1;
                }
            };

            QString sessionId = findSessionIdBySignature(head1);

            if (!sessionId.isEmpty())
                return sessionId;

            sessionId = findSessionIdBySignature(head2);

            if (!sessionId.isEmpty())
                return sessionId;

            // Сохраняем хвост блока.
            const qsizetype tailSize = std::min<qsizetype>(data.size(), static_cast<qsizetype>(OVERLAP));
            tail = data.right(tailSize);
            address += bytesRead;
        }

        currentAddress = scanEnd;
    }

    return QString();
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

QStringList GameMemoryReader::findIgnoredPlayersIdInMemory(const QStringList& playerIdList)
{
    if (playerIdList.isEmpty())
        return QStringList();

    QString gameName;

    if (m_gameType == GameType::GameTypeEnum::DefinitiveEdition)
        gameName = "Warhammer 40,000: Dawn of War";
    else if (m_gameType == GameType::GameTypeEnum::SoulstormSteam)
        gameName = "Dawn of War: Soulstorm";
    else
        return QStringList();

    HANDLE hProcess = getProcessHandle(gameName);

    if (hProcess == nullptr)
    {
        qWarning(logWarning()) << "Cannot open process:" << gameName;
        return QStringList();
    }

    // Подготавливаем искомые ID
    const QByteArray searchedID = playerIdList.first().toLocal8Bit();
    const char* sigStart = searchedID.constData();
    const size_t sigSize = static_cast<size_t>(searchedID.size());

    if (sigSize == 0)
        return QStringList();

    std::vector<std::vector<char>> targetIds;

    targetIds.reserve(static_cast<size_t>(playerIdList.size()));

    for (const QString& playerId : playerIdList)
    {
        QByteArray ba = playerId.toLocal8Bit();
        targetIds.emplace_back(ba.begin(), ba.end());
    }

    const int idCount = playerIdList.size();

    // Определяем диапазон адресов
    SYSTEM_INFO systemInfo{};
    GetNativeSystemInfo(&systemInfo);

    DWORD64 currentAddress = reinterpret_cast<DWORD64>(systemInfo.lpMinimumApplicationAddress);
    const DWORD64 maxAddress = reinterpret_cast<DWORD64>(systemInfo.lpMaximumApplicationAddress);

    // Буфер чтения
    constexpr SIZE_T CHUNK_SIZE = 1024 * 1024;
    constexpr SIZE_T OVERLAP = 512;

    QByteArray buffer;
    buffer.reserve(CHUNK_SIZE);

    MEMORY_BASIC_INFORMATION mbi{};

    // Проходим реальные регионы памяти процесса
    while (currentAddress < maxAddress)
    {
        if (m_abort)
            return QStringList();

        if (VirtualQueryEx(hProcess, reinterpret_cast<LPCVOID>(currentAddress), &mbi, sizeof(mbi)) == 0)
        {
            currentAddress += 0x1000;
            continue;
        }

        const DWORD64 regionBase = reinterpret_cast<DWORD64>(mbi.BaseAddress);

        const DWORD64 regionEnd = regionBase + mbi.RegionSize;

        // Защита от зацикливания.
        if (regionEnd <= currentAddress)
            break;

        const DWORD64 scanEnd = min(regionEnd, maxAddress);


        // Проверяем доступность региона
        const DWORD protection = mbi.Protect & 0xFF;

        const bool isReadable =
            mbi.State == MEM_COMMIT &&
            !(mbi.Protect & PAGE_GUARD) &&
            protection != PAGE_NOACCESS &&
            (
                protection == PAGE_READONLY ||
                protection == PAGE_READWRITE ||
                protection == PAGE_WRITECOPY ||
                protection == PAGE_EXECUTE_READ ||
                protection == PAGE_EXECUTE_READWRITE ||
                protection == PAGE_EXECUTE_WRITECOPY
                );

        if (!isReadable)
        {
            currentAddress = scanEnd;
            continue;
        }


        // Читаем регион кусками
        DWORD64 address = currentAddress;
        QByteArray tail;

        while (address < scanEnd)
        {
            if (m_abort)
                return QStringList();

            const SIZE_T bytesToRead = static_cast<SIZE_T>(std::min<DWORD64>(CHUNK_SIZE, scanEnd - address));
            buffer.resize(static_cast<qsizetype>(bytesToRead));

            SIZE_T bytesRead = 0;

            ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(address), buffer.data(), bytesToRead, &bytesRead);

            if (bytesRead == 0)
            {
                address += 0x1000;
                tail.clear();
                continue;
            }

            buffer.resize(static_cast<qsizetype>(bytesRead));


            // Объединяем хвост прошлого блока с текущим блоком.
            QByteArray data;

            if (!tail.isEmpty())
                data = tail + buffer;
            else
                data = buffer;

            const char* bufStart = data.constData();
            const char* bufEnd = bufStart + data.size();
            const char* currentPtr = bufStart;


            // Ищем первый Player ID
            while ((currentPtr = std::search( currentPtr, bufEnd,sigStart, sigStart + sigSize)) != bufEnd)
            {
                if (m_abort)
                    return QStringList();

                const qsizetype offset = currentPtr - bufStart;
                const qsizetype windowStart = std::max<qsizetype>(0, offset - 400);
                const qsizetype windowEnd =std::min<qsizetype>(data.size(), offset + 400);
                const char* wData = bufStart + windowStart;
                const char* wEnd = bufStart + windowEnd;


                // Проверяем наличие всех ID
                bool allIdFound = true;

                for (size_t t = 0; t < targetIds.size(); ++t)
                {
                    if (std::search(wData, wEnd, targetIds[t].begin(), targetIds[t].end()) == wEnd)
                    {
                        allIdFound = false;
                        break;
                    }
                }

                if (allIdFound)
                {

                    // Дополнительные признаки структуры данных
                    auto matchPattern = [wData, wEnd](const char* pattern,size_t length) -> bool
                    {
                        return std::search(wData, wEnd, pattern, pattern + length) != wEnd;
                    };

                    const bool hasGlobal = matchPattern("\"global\"", 8);


                    // Формируем паттерны:
                    std::vector<char> p1;
                    std::vector<char> p2;
                    std::vector<char> p3;

                    p1.reserve(sigSize + 2);
                    p2.reserve(sigSize + 2);
                    p3.reserve(sigSize + 2);

                    p1.push_back('[');
                    p1.insert(p1.end(), sigStart, sigStart + sigSize);
                    p1.push_back(',');

                    p2.push_back(',');
                    p2.insert(p2.end(), sigStart, sigStart + sigSize);
                    p2.push_back(',');

                    p3.push_back(',');
                    p3.insert(p3.end(), sigStart, sigStart + sigSize);
                    p3.push_back(']');

                    const bool hasP1 = std::search(wData,wEnd,p1.begin(), p1.end()) != wEnd;
                    const bool hasP2 = std::search(wData, wEnd,p2.begin(),p2.end()) != wEnd;
                    const bool hasP3 = std::search(wData, wEnd,p3.begin(),p3.end()) != wEnd;

                    const bool validStructure = (idCount > 2 && (hasGlobal || hasP1 || hasP2 || hasP3)) || (idCount == 1 && (hasGlobal || hasP1 || hasP3));

                    if (validStructure)
                    {
                        // Извлекаем 8-значные ID
                        QStringList idList;

                        QString currentId;
                        currentId.reserve(8);

                        for (const char* p = wData; p < wEnd; ++p)
                        {
                            const char symbol = *p;

                            if (symbol >= '0' && symbol <= '9')
                                currentId.append(symbol);
                            else
                            {
                                if (currentId.size() == 8)
                                {
                                    if (!idList.contains(currentId))
                                        idList.append(currentId);
                                }

                                currentId.clear();
                            }
                        }

                        // Не забываем проверить число в самом конце окна.
                        if (currentId.size() == 8)
                        {
                            if (!idList.contains(currentId))
                                idList.append(currentId);
                        }

                        if (idList.size() == idCount + 1)
                            return idList;
                    }
                }

                // Продолжаем поиск следующего вхождения.
                currentPtr += sigSize;
            }

            // -------------------------------------------------
            // Сохраняем хвост блока.
            // -------------------------------------------------

            const qsizetype tailSize = std::min<qsizetype>(data.size(), static_cast<qsizetype>(OVERLAP));
            tail = data.right(tailSize);
            address += bytesRead;
        }

        currentAddress = scanEnd;
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

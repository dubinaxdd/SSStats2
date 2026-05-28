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
    if (playerIdList.isEmpty())
        return QStringList();

    // 1. Предварительное кеширование данных (выполняется один раз)
    QByteArray searchedID = playerIdList.first().toLocal8Bit();
    const char* sigStart = searchedID.constData();
    int sigSize = searchedID.size();

    // Переводим все искомые ID в сырые векторы байт
    std::vector<std::vector<char> > targetIds;
    targetIds.reserve(playerIdList.size());
    for (int i = 0; i < playerIdList.size(); ++i) {
        QByteArray ba = playerIdList.at(i).toLocal8Bit();
        targetIds.push_back(std::vector<char>(ba.begin(), ba.end()));
    }

    std::vector<char> buffer;
    MEMORY_BASIC_INFORMATION mbi;
    DWORD64 currentAddress = startAdress;

    while (currentAddress < endAdress)
    {
        if (m_ignoredPlayersIdFinded || m_abort)
            return QStringList();

        if (VirtualQueryEx(hProcess, (LPCVOID)currentAddress, &mbi, sizeof(mbi)) == 0)
        {
            currentAddress += 4096;
            continue;
        }

        DWORD64 regionEnd = (DWORD64)mbi.BaseAddress + mbi.RegionSize;
        if (regionEnd > endAdress)
            regionEnd = endAdress;

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
                const char* bufStart = buffer.data();
                const char* bufEnd = bufStart + bytesRead;
                const char* currentPtr = bufStart;

                // 2. Поиск основного ID стандартным, но быстрым std::search
                while ((currentPtr = std::search(currentPtr, bufEnd, sigStart, sigStart + sigSize)) != bufEnd)
                {
                    if (m_ignoredPlayersIdFinded || m_abort)
                        return QStringList();

                    int offset = currentPtr - bufStart;
                    int windowStart = offset - 400;
                    int windowEnd = offset + 400;

                    // Проверяем, укладывается ли окно [-400, 400] в прочитанный буфер
                    if (windowStart >= 0 && windowEnd <= static_cast<int>(bytesRead))
                    {
                        const char* wData = bufStart + windowStart;
                        const char* wEnd = bufStart + windowEnd;

                        // 3. Быстрая проверка всех остальных ID без Qt-оберток
                        bool allIdFinded = true;
                        for (size_t t = 0; t < targetIds.size(); ++t)
                        {
                            if (std::search(wData, wEnd, targetIds[t].begin(), targetIds[t].end()) == wEnd)
                            {
                                allIdFinded = false;
                                break;
                            }
                        }

                        if (allIdFinded)
                        {
                            int idCount = playerIdList.count();

                            // 4. Оптимизированный поиск паттернов («сырые» вызовы std::search)
                            auto matchPattern = [wData, wEnd](const char* pat, size_t len) -> bool {
                                return std::search(wData, wEnd, pat, pat + len) != wEnd;
                            };

                            bool hasGlobal = matchPattern("\"global\"", 8);

                            // Собираем паттерны со скобками и запятыми динамически на стеке (без выделения памяти в куче)
                            std::vector<char> p1, p2, p3;
                            p1.reserve(sigSize + 2); p2.reserve(sigSize + 2); p3.reserve(sigSize + 2);

                            p1.push_back('['); p1.insert(p1.end(), sigStart, sigStart + sigSize); p1.push_back(',');
                            p2.push_back(','); p2.insert(p2.end(), sigStart, sigStart + sigSize); p2.push_back(',');
                            p3.push_back(','); p3.insert(p3.end(), sigStart, sigStart + sigSize); p3.push_back(']');

                            bool hasP1 = std::search(wData, wEnd, p1.begin(), p1.end()) != wEnd;
                            bool hasP2 = std::search(wData, wEnd, p2.begin(), p2.end()) != wEnd;
                            bool hasP3 = std::search(wData, wEnd, p3.begin(), p3.end()) != wEnd;

                            if ((idCount > 2 && (hasGlobal || hasP1 || hasP2 || hasP3)) ||
                                (idCount == 1 && (hasGlobal || hasP1 || hasP3)))
                            {
                                QStringList idList;
                                QString currentId;
                                currentId.reserve(8);

                                // 5. Извлечение ID по маске цифр
                                for (const char* p = wData; p < wEnd; ++p)
                                {
                                    char symbol = *p;
                                    if (symbol >= '0' && symbol <= '9')
                                    {
                                        currentId.append(symbol);
                                    }
                                    else
                                    {
                                        if (currentId.size() == 8)
                                        {
                                            // Быстрый поиск дубликатов в QStringList
                                            if (std::find(idList.begin(), idList.end(), currentId) == idList.end())
                                            {
                                                idList.append(currentId);
                                            }
                                        }
                                        currentId.clear();
                                    }
                                }

                                if (idList.count() == idCount + 1)
                                    return idList;
                            }
                        }
                    }

                    // Сдвигаем указатель вперед для продолжения поиска
                    currentPtr += sigSize;
                }
            }
        }
        currentAddress = regionEnd;
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

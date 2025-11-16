#include <soulstormMemoryController.h>
#include <TlHelp32.h>
#include <windows.h>
#include <psapi.h>
//#include "dbghelp.h"
#include <QtEndian>

#pragma comment(lib, "dbghelp.lib")

PVOID FogAddr = (PVOID)0x008282F0;
PVOID MapSkyDistanceAddr = (PVOID)0x0082A33A;
PVOID Float512Addr = (PVOID)0x00AF54C8;

DWORD Float512OldProtect;

BYTE CodeMapSkyDistance[6] = {0xD9, 0x9B, 0x70, 0x0C, 0x00, 0x00};
BYTE CodeF512[4] = {0x00, 0x00, 0xC0, 0x42};
BYTE Float512[4] = {0x00, 0x00, 0x00, 0x44};
BYTE CodeFog[6] = {0xD9, 0x81, 0x60, 0x0C, 0x00, 0x00};

BYTE nop_array6[6] = {0x90,0x90,0x90,0x90,0x90,0x90};
BYTE temp6[6] = {0}; // Буфер инициализирован изначально нулями
BYTE temp6_2[6] = {0};

BYTE temp4[6] = {0};

//Функция написана нейросетью, получает список функций с адресам в выбранной dll библиотеки выбранного процесса
bool ListRemoteDllExports(HANDLE hProcess, HMODULE remoteHModule, QMap<QString, DWORD_PTR> *functionsAddresses) {

    BYTE buffer[1024]; // Buffer to read headers into locally

    // --- Read the DOS Header ---
    IMAGE_DOS_HEADER dosHeader;
    if (!ReadProcessMemory(hProcess, remoteHModule, &dosHeader, sizeof(dosHeader), NULL) ||
        dosHeader.e_magic != IMAGE_DOS_SIGNATURE) {
        qInfo(logInfo()) << "Failed to read remote DOS header or invalid signature.";
        return false;
    }

    // --- Read the NT Headers (assuming 64-bit for modern systems) ---
    // You must read the offset from the DOS header to find where NT headers start.
    PIMAGE_NT_HEADERS64 pNtHeaders64 = reinterpret_cast<PIMAGE_NT_HEADERS64>(buffer);
    DWORD ntHeadersOffset = dosHeader.e_lfanew;

    if (!ReadProcessMemory(hProcess,
                           reinterpret_cast<BYTE*>(remoteHModule) + ntHeadersOffset,
                           pNtHeaders64,
                           sizeof(IMAGE_NT_HEADERS64),
                           NULL) ||
        pNtHeaders64->Signature != IMAGE_NT_SIGNATURE) {
        qInfo(logInfo()) << "Failed to read remote NT headers or invalid signature.";
        return false;
    }

    // --- Read the Export Directory Table ---
    DWORD exportDirRVA = pNtHeaders64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    if (exportDirRVA == 0) {
        qInfo(logInfo()) << "Remote DLL has no named exports.";
        return true;
    }

    IMAGE_EXPORT_DIRECTORY exportDir;
    if (!ReadProcessMemory(hProcess,
                           reinterpret_cast<BYTE*>(remoteHModule) + exportDirRVA,
                           &exportDir,
                           sizeof(exportDir),
                           NULL)) {
        qInfo(logInfo()) << "Failed to read remote Export Directory.";
        return false;
    }

    qInfo(logInfo()) << "Total exported names found: " << exportDir.NumberOfNames;

    // --- Read the arrays of names, ordinals, and function RVAs into local buffers ---
    std::vector<DWORD> nameRVAs(exportDir.NumberOfNames);
    std::vector<WORD> ordinals(exportDir.NumberOfNames);
    std::vector<DWORD> functionRVAs(exportDir.NumberOfFunctions);

    ReadProcessMemory(hProcess, reinterpret_cast<BYTE*>(remoteHModule) + exportDir.AddressOfNames, nameRVAs.data(), nameRVAs.size() * sizeof(DWORD), NULL);
    ReadProcessMemory(hProcess, reinterpret_cast<BYTE*>(remoteHModule) + exportDir.AddressOfNameOrdinals, ordinals.data(), ordinals.size() * sizeof(WORD), NULL);
    ReadProcessMemory(hProcess, reinterpret_cast<BYTE*>(remoteHModule) + exportDir.AddressOfFunctions, functionRVAs.data(), functionRVAs.size() * sizeof(DWORD), NULL);

    // --- Enumerate and print names and addresses ---
    for (DWORD i = 0; i < exportDir.NumberOfNames; ++i) {
        char funcName[MAX_PATH];
        // Read the actual function name string
        ReadProcessMemory(hProcess, reinterpret_cast<BYTE*>(remoteHModule) + nameRVAs[i], funcName, sizeof(funcName), NULL);

        WORD ordinalIndex = ordinals[i];
        // The address in the *remote* process
        DWORD_PTR remoteAddress = reinterpret_cast<DWORD_PTR>(remoteHModule) + functionRVAs[ordinalIndex];

        functionsAddresses->insert(funcName, remoteAddress);
    }

    return true;
}


uintptr_t GetModuleBaseAddress(HANDLE hProcess, const std::string& moduleName) {
    HMODULE hMods[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded)) {
        for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
            TCHAR szModName[MAX_PATH];
            if (GetModuleFileNameEx(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR))) {
                std::string currentModuleName(szModName);
                if (currentModuleName.find(moduleName) != std::string::npos) {
                    MODULEINFO mi;
                    if (GetModuleInformation(hProcess, hMods[i], &mi, sizeof(mi))) {
                        return reinterpret_cast<uintptr_t>(mi.lpBaseOfDll);
                    }
                }
            }
        }
    } else {
        qInfo(logInfo()) << "Failed to enumerate modules. Error: " << GetLastError();
    }

    return 0; // Module not found
}

SoulstormMemoryController::SoulstormMemoryController(SettingsController* settingsController, QObject *parent)
    : QObject(parent)
    , m_settingsController(settingsController)
{
    QObject::connect(m_settingsController, &SettingsController::settingsLoaded, this, &SoulstormMemoryController::onSettingsLoaded,Qt::QueuedConnection);
}

void SoulstormMemoryController::onGameLaunchStateChanged(bool state)
{
    m_gameLaunchState = state;

    if (!m_gameLaunchState)
        return;

    onNoFogStateChanged(currentNoFog && m_gameLaunchState);
}

void SoulstormMemoryController::onSettingsLoaded()
{
    qInfo(logInfo()) << "SoulstormMemoryController::onSettingsLoaded()" << "load started";

    currentNoFog = m_settingsController->getSettings()->noFog;
    onNoFogStateChanged(currentNoFog && m_gameLaunchState);

    qInfo(logInfo()) << "SoulstormMemoryController::onSettingsLoaded()" << "load finished";
}

void SoulstormMemoryController::setCurrentGame(GamePath *newCurrentGame)
{
    m_currentGame = newCurrentGame;
}

void SoulstormMemoryController::receiveCurrentMissionState(GameMissionState missionCurrentState)
{
    if (missionCurrentState == GameMissionState::gameStarted ||
        missionCurrentState == GameMissionState::playbackStarted ||
        missionCurrentState == GameMissionState::savedGameStarted ||
        missionCurrentState == GameMissionState::unknownGameStarted
    )
    {
        if (m_currentGame->gameType == GameType::DefinitiveEdition)
            disableFogDE(m_targetNoFog);
    }
}

void SoulstormMemoryController::disableFogDE(bool disableFog)
{
    //Получаем ID процесса игры
    QString gameName = "Warhammer 40,000: Dawn of War";

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    LPCWSTR lps = (LPCWSTR)gameName.utf16();
    HWND m_gameHwnd = NULL;

    m_gameHwnd = FindWindowW(NULL, lps);

    if(!m_gameHwnd)
        qInfo(logInfo()) << "Process Not Openned";

    DWORD PID;
    GetWindowThreadProcessId(m_gameHwnd, &PID);

    //Получаем базовый адрес сим енжина
    std::string targetModuleName = "SimEngine.dll";

    // Получение дескриптора процесса игры
    HANDLE hProcess= OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, 0, PID);

    if(hProcess==nullptr)
    {
        qInfo(logInfo()) << "Process handle not finded";
        CloseHandle(hProcess);
        return;
    }

    uintptr_t baseAddress = GetModuleBaseAddress( hProcess/*PID*/, targetModuleName);

    if (baseAddress != 0) {
        qInfo(logInfo()) << "Base address of SimEngine.dll" << QString::fromStdString(targetModuleName) << "in game process: 0x" << hex << baseAddress;
    } else {
        qInfo(logInfo()) << "Module " << QString::fromStdString(targetModuleName) << " not found or error occurred.";
    }

    //Получаем список функций симэнжина
    QMap<QString, DWORD_PTR> functionsAddresses;
    ListRemoteDllExports(hProcess, (HMODULE)baseAddress, &functionsAddresses);

    //Ищем функцию s_instance@TurningBehaviorTemplateManager
    uintptr_t needFunctionAddress;

    for(auto& key : functionsAddresses.keys())
    {
        if (key.contains("s_instance@TurningBehaviorTemplateManager"))
        {
            needFunctionAddress = functionsAddresses.value(key);
            break;
        }
    }

    qInfo(logInfo()) << "TurningBehaviorTemplateManager function adress:" << hex << needFunctionAddress;

    //Получаем адрес области памяти в которую пишет сименжин
    QByteArray simEngineMemoryAddressByteArray(8, 0);

    //uintptr_t indent = 0x138398; //отступ от начала сименжина
    //ReadProcessMemory(hProcess, (PVOID)(baseAddress + indent), simEngineMemoryAddressByteArray.data(), 8, nullptr);
    //qDebug() << "SimEngineMemoryAddressByteArray: " << hex << (PVOID)(baseAddress + indent) << simEngineMemoryAddressByteArray.toHex().data();

    uintptr_t func_indent = 0x98; //отступ от начала сименжина

    qInfo(logInfo()) << hex << (PVOID)(needFunctionAddress + func_indent);

    ReadProcessMemory(hProcess, (PVOID)(needFunctionAddress + func_indent), simEngineMemoryAddressByteArray.data(), 8, nullptr);
    qInfo(logInfo()) << "SimEngineMemoryAddressByteArray: " << hex << (PVOID)(needFunctionAddress + func_indent) << simEngineMemoryAddressByteArray.toHex().data();

    quint64 simEngineMemoryAddress = qFromLittleEndian<quint64>((uchar*)simEngineMemoryAddressByteArray.data());
    qInfo(logInfo()) << "SimEngineMemoryAddress" << hex << simEngineMemoryAddress;

    if (simEngineMemoryAddress == 0)
        return;

    //Получаем адрес объекта террейна
    uintptr_t terrainIndent = 0x40; //отступ от начала памяти сименжина
    QByteArray terrainAddressByteArray(8, 0);

    ReadProcessMemory(hProcess, (PVOID)(simEngineMemoryAddress + terrainIndent), terrainAddressByteArray.data(), 8, nullptr);
    qInfo(logInfo()) << "TerrainAddressByteArray: " << hex << (PVOID)(simEngineMemoryAddress + terrainIndent) << terrainAddressByteArray.toHex().data();

    quint64 terrainAddress = qFromLittleEndian<quint64>((uchar*)terrainAddressByteArray.data());
    qInfo(logInfo()) << "terrainAddressByteArray" << hex << terrainAddress;


    //Получаем адреса память в которых будем менять
    uintptr_t fogMinAddress = terrainAddress + 0xC44;
    qInfo(logInfo()) << "fogMinAdress" << hex << fogMinAddress;

    uintptr_t fogMaxAddress = terrainAddress + 0xC48;
    qInfo(logInfo()) << "fogMaxAddress" << hex << fogMaxAddress;

    uintptr_t skyDistanceAddress = terrainAddress + 0xC60;
    qInfo(logInfo()) << "skyDistanceAddress" << hex << skyDistanceAddress;

    uintptr_t skyRadius1_Address = terrainAddress + 0xC90;
    qInfo(logInfo()) << "skyRadius1_Address" << hex << skyRadius1_Address;

    uintptr_t skyRadius2_Address = terrainAddress + 0xC68;
    qInfo(logInfo()) << "skyRadius2_Address" << hex << skyRadius2_Address;

    uintptr_t skyRadius3_Address = terrainAddress + 0xC7C;
    qInfo(logInfo()) << "skyRadius3_Address" << hex << skyRadius3_Address;

    if (disableFog)
    {
        BYTE fogMinValue[4] = {0x00, 0x00, 0xCE, 0x43};
        BYTE fogMaxValue[4] = {0x00, 0x00, 0x00, 0x44};
        BYTE skyDistanceValue[4] = {0x00, 0x00, 0x80, 0x44};
        BYTE skyRadius1_Value[4] = {0x00, 0x00, 0x7A, 0x44};
        BYTE skyRadius2_Value[4] = {0x00, 0x00, 0x7A, 0x44};
        BYTE skyRadius3_Value[4] = {0x00, 0x00, 0x7A, 0x44};

        WriteProcessMemory(hProcess, (PVOID)fogMinAddress, fogMinValue, 4, nullptr); //default 50 [00 00 48 42], new 412  [00 00 CE 43]
        WriteProcessMemory(hProcess, (PVOID)fogMaxAddress, fogMaxValue, 4, nullptr); //default 150 [00 00 16 43], new 512  [00 00 00 44]
        WriteProcessMemory(hProcess, (PVOID)skyDistanceAddress, skyDistanceValue, 4, nullptr); //default 160 [00 00 20 43], new 1024  [00 00 80 44]
        WriteProcessMemory(hProcess, (PVOID)skyRadius1_Address, skyRadius1_Value, 4, nullptr); //default 246.9650116 [0B F7 76 43], new 1000 [00 00 7A 44]
        WriteProcessMemory(hProcess, (PVOID)skyRadius2_Address, skyRadius2_Value, 4, nullptr); //default 246.9650116 [0B F7 76 43], new 1000 [00 00 7A 44]
        WriteProcessMemory(hProcess, (PVOID)skyRadius3_Address, skyRadius3_Value, 4, nullptr); //default 247.9805298 [04 FB 77 43], new 1000 [00 00 7A 44]
    }
    else
    {
        BYTE fogMinValue[4] = {0x00, 0x00, 0x48, 0x42};
        BYTE fogMaxValue[4] = {0x00, 0x00, 0x16, 0x43};
        BYTE skyDistanceValue[4] = {0x00, 0x00, 0x20, 0x43};
        BYTE skyRadius1_Value[4] = {0x0B, 0xF7, 0x76, 0x43};
        BYTE skyRadius2_Value[4] = {0x0B, 0xF7, 0x76, 0x43};
        BYTE skyRadius3_Value[4] = {0x04, 0xFB, 0x77, 0x43};

        WriteProcessMemory(hProcess, (PVOID)fogMinAddress, fogMinValue, 4, nullptr); //default 50 [00 00 48 42], new 412  [00 00 CE 43]
        WriteProcessMemory(hProcess, (PVOID)fogMaxAddress, fogMaxValue, 4, nullptr); //default 150 [00 00 16 43], new 512  [00 00 00 44]
        WriteProcessMemory(hProcess, (PVOID)skyDistanceAddress, skyDistanceValue, 4, nullptr); //default 160 [00 00 20 43], new 1024  [00 00 80 44]
        WriteProcessMemory(hProcess, (PVOID)skyRadius1_Address, skyRadius1_Value, 4, nullptr); //default 246.9650116 [0B F7 76 43], new 1000 [00 00 7A 44]
        WriteProcessMemory(hProcess, (PVOID)skyRadius2_Address, skyRadius2_Value, 4, nullptr); //default 246.9650116 [0B F7 76 43], new 1000 [00 00 7A 44]
        WriteProcessMemory(hProcess, (PVOID)skyRadius3_Address, skyRadius3_Value, 4, nullptr); //default 247.9805298 [04 FB 77 43], new 1000 [00 00 7A 44]
    }

    CloseHandle(hProcess);
}

void SoulstormMemoryController::disableFogSS()
{
    //qInfo(logInfo()) << "Fog state: " <<  state;

    if(m_gameHwnd == nullptr)
        return; // Процесс DoW не обнаружен


    DWORD PID;
    GetWindowThreadProcessId(m_gameHwnd, &PID);
    HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, 0, PID);
    if(hProcess==nullptr){
        qWarning(logWarning()) << "Could not open process" << GetLastError();
        return;
    }

    //if(targetNoFog!=currentNoFog||force){
    ReadProcessMemory(hProcess, FogAddr, temp6, 6, nullptr); // Читаем данные из памяти процесса "Dawn of War: Soulstorm" по адресу "Тумана" (FogAddr) и записываем в буфер temp6 в количестве 6 байт
    ReadProcessMemory(hProcess, Float512Addr, temp4, 4, nullptr);
    ReadProcessMemory(hProcess, MapSkyDistanceAddr, temp6_2, 6, nullptr);

    if(m_targetNoFog&&memcmp(temp6, CodeFog, 6)==0){ // Проверяем совпали ли прочитанные ранее данные "Тумана" с базовыми оригинальными данными (заведомо нам известны).
        qInfo(logInfo()) << "Enable NoFOG";
        WriteProcessMemory(hProcess, FogAddr, nop_array6, 6, nullptr); // Записываем данные в память процесса "Dawn of War: Soulstorm" по адресу "Тумана" (FogAddr) из буфера nop_array6 в количестве 6 байт
    } else if(!m_targetNoFog){ // Если данные не совпали и/или необходимо вернуть оригинальные данные
        qInfo(logInfo()) << "Disable NoFOG";
        WriteProcessMemory(hProcess, FogAddr, CodeFog, 6, nullptr);
    }
    if(m_targetNoFog&&memcmp(temp4, CodeF512, 4)==0){
        VirtualProtectEx(hProcess, Float512Addr, 4, PAGE_EXECUTE_READWRITE, &Float512OldProtect);
        if(!WriteProcessMemory(hProcess, Float512Addr, Float512, 4, nullptr))
            qWarning(logWarning()) << "Could not write CodeF512 to memory";
        VirtualProtectEx(hProcess, Float512Addr, 4, Float512OldProtect, nullptr);
    } else if(!m_targetNoFog){
        //qDebug() << "temp4 is not equal to CodeF512";
        VirtualProtectEx(hProcess, Float512Addr, 4, PAGE_EXECUTE_READWRITE, &Float512OldProtect);
        WriteProcessMemory(hProcess, Float512Addr, CodeF512, 4, nullptr);
        VirtualProtectEx(hProcess, Float512Addr, 4, Float512OldProtect, nullptr);
    }
    if(m_targetNoFog&&memcmp(temp6_2, CodeMapSkyDistance, 6)==0){
        if(!WriteProcessMemory(hProcess, MapSkyDistanceAddr, nop_array6, 6, nullptr))
            qWarning(logWarning()) << "Could not write CodeMapSkyDistance to memory";
    } else if(!m_targetNoFog){
        //qDebug() << "temp6 is not equal to CodeMapSkyDistance";
        WriteProcessMemory(hProcess, MapSkyDistanceAddr, CodeMapSkyDistance, 6, nullptr);
    }
    //QByteArray array4((const char*)temp4, 4);
    //qDebug() << QString(array4.toHex());
    //QByteArray array6((const char*)temp6_2, 6);
    //qDebug() << QString(array6.toHex());
    //currentNoFog = targetNoFog;
    //}
    CloseHandle(hProcess);
}

void SoulstormMemoryController::setGameHwnd(HWND newGameHwnd)
{
    m_gameHwnd = newGameHwnd;
}

void SoulstormMemoryController::onNoFogStateChanged(bool state)
{
    m_targetNoFog = state;

    if (m_currentGame->gameType == GameType::SoulstormSteam)
        disableFogSS();
    else if (m_currentGame->gameType == GameType::DefinitiveEdition)
        disableFogDE(m_targetNoFog);
}


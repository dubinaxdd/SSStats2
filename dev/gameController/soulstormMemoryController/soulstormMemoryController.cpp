#include <soulstormMemoryController.h>
#include <TlHelp32.h>
#include <windows.h>
#include <psapi.h>
#include <QtEndian>

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

/*uintptr_t GetModuleBaseAddress(DWORD processID, const std::string& moduleName) {
    uintptr_t moduleBaseAddress = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processID);

    if (hSnapshot != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 moduleEntry;
        moduleEntry.dwSize = sizeof(MODULEENTRY32);

        if (Module32First(hSnapshot, &moduleEntry)) {
            do {
                // Convert moduleEntry.szModule to std::string for comparison
                std::string currentModuleName(moduleEntry.szModule);
                if (currentModuleName == moduleName) {
                    moduleBaseAddress = reinterpret_cast<uintptr_t>(moduleEntry.modBaseAddr);
                    break;
                }
            } while (Module32Next(hSnapshot, &moduleEntry));
        }
        CloseHandle(hSnapshot);
    }
    return moduleBaseAddress;
}*/

uintptr_t GetModuleBaseAddress(DWORD processId, const std::string& moduleName) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (hProcess == NULL) {
        qDebug() << "Failed to open process. Error: " << GetLastError();
        return 0;
    }

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
                        CloseHandle(hProcess);
                        return reinterpret_cast<uintptr_t>(mi.lpBaseOfDll);
                    }
                }
            }
        }
    } else {
        qDebug() << "Failed to enumerate modules. Error: " << GetLastError();
    }

    CloseHandle(hProcess);
    return 0; // Module not found
}


SoulstormMemoryController::SoulstormMemoryController(SettingsController* settingsController, QObject *parent)
    : QObject(parent)
    , m_settingsController(settingsController)
{
    QObject::connect(m_settingsController, &SettingsController::settingsLoaded, this, &SoulstormMemoryController::onSettingsLoaded,Qt::QueuedConnection);

    //Получаем ID процесса игры
    QString gameName = "Warhammer 40,000: Dawn of War";

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    LPCWSTR lps = (LPCWSTR)gameName.utf16();
    HWND m_gameHwnd = NULL;

    m_gameHwnd = FindWindowW(NULL, lps);

    if(!m_gameHwnd)
        qDebug() << "Process Not Openned";

    DWORD PID;
    GetWindowThreadProcessId(m_gameHwnd, &PID);

    //Получаем базовый адрес сим енжина
    std::string targetModuleName = "SimEngine.dll";

    uintptr_t baseAddress = GetModuleBaseAddress( PID, targetModuleName);

    if (baseAddress != 0) {
        qDebug() << "Base address of " << QString::fromStdString(targetModuleName) << " in current process: 0x" << hex << baseAddress;
    } else {
        qDebug() << "Module " << QString::fromStdString(targetModuleName) << " not found or error occurred.";
    }


    // Получение дескриптора процесса игры
    HANDLE hProcess= OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, 0, PID);

    if(hProcess==nullptr)
        qDebug() << "Process handle not finded";


    //Получаем адрес области памяти в которую пишет сименжин
    uintptr_t indent = 0x138398; //отступ от начала сименжина
    QByteArray simEngineMemoryAddressByteArray(8, 0);

    ReadProcessMemory(hProcess, (PVOID)(baseAddress + indent), simEngineMemoryAddressByteArray.data(), 8, nullptr);
    qDebug() << "SimEngineMemoryAddressByteArray: " << hex << (PVOID)(baseAddress + indent) << simEngineMemoryAddressByteArray.toHex().data();

    quint64 simEngineMemoryAddress = qFromLittleEndian<quint64>((uchar*)simEngineMemoryAddressByteArray.data());
    qDebug() << "SimEngineMemoryAddress" << hex << simEngineMemoryAddress;


    //Получаем адрес объекта террейна
    uintptr_t terrainIndent = 0x40; //отступ от начала памяти сименжина
    QByteArray terrainAddressByteArray(8, 0);

    ReadProcessMemory(hProcess, (PVOID)(simEngineMemoryAddress + terrainIndent), terrainAddressByteArray.data(), 8, nullptr);
    qDebug() << "TerrainAddressByteArray: " << hex << (PVOID)(baseAddress + indent) << terrainAddressByteArray.toHex().data();

    quint64 terrainAddress = qFromLittleEndian<quint64>((uchar*)terrainAddressByteArray.data());
    qDebug() << "terrainAddressByteArray" << hex << terrainAddress;


    //Получаем адреса память в которых будем менять
    uintptr_t fogMinAddress = terrainAddress + 0xC44;
    qDebug() << "fogMinAdress" << hex << fogMinAddress;

    uintptr_t fogMaxAddress = terrainAddress + 0xC48;
    qDebug() << "fogMaxAddress" << hex << fogMaxAddress;

    uintptr_t skyDistanceAddress = terrainAddress + 0xC60;
    qDebug() << "skyDistanceAddress" << hex << skyDistanceAddress;

    uintptr_t skyRadius1_Address = terrainAddress + 0xC90;
    qDebug() << "skyRadius1_Address" << hex << skyRadius1_Address;

    uintptr_t skyRadius2_Address = terrainAddress + 0xC68;
    qDebug() << "skyRadius2_Address" << hex << skyRadius2_Address;

    BYTE fogMinValue[4] = {0x00, 0x00, 0xCE, 0x43};
    WriteProcessMemory(hProcess, (PVOID)fogMinAddress, fogMinValue, 4, nullptr); //default 50, new 412  [00 00 CE 43]

    BYTE fogMaxValue[4] = {0x00, 0x00, 0x00, 0x44};
    WriteProcessMemory(hProcess, (PVOID)fogMaxAddress, fogMaxValue, 4, nullptr); //default 50, new 512  [00 00 00 44]

    BYTE skyDistanceValue[4] = {0x00, 0x00, 0x80, 0x44};
    WriteProcessMemory(hProcess, (PVOID)skyDistanceAddress, skyDistanceValue, 4, nullptr); //default 160, new 1024  [00 00 80 44]

    BYTE skyRadius1_Value[4] = {0x00, 0x00, 0x7A, 0x44};
    WriteProcessMemory(hProcess, (PVOID)skyRadius1_Address, skyRadius1_Value, 4, nullptr); //default 246.9650116, new 1000 [00 00 7A 44]

    BYTE skyRadius2_Value[4] = {0x00, 0x00, 0x7A, 0x44};
    WriteProcessMemory(hProcess, (PVOID)skyRadius2_Address, skyRadius2_Value, 4, nullptr); //default 246.9650116, new 1000 [00 00 7A 44]
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

void SoulstormMemoryController::setGameHwnd(HWND newSoulstormHwnd)
{
    m_soulstormHwnd = newSoulstormHwnd;
}

void SoulstormMemoryController::onNoFogStateChanged(bool state)
{
    //qInfo(logInfo()) << "Fog state: " <<  state;

    targetNoFog = state;

    if(m_soulstormHwnd == nullptr)
        return; // Процесс DoW не обнаружен


    DWORD PID;
    GetWindowThreadProcessId(m_soulstormHwnd, &PID);
    HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, 0, PID);
    if(hProcess==nullptr){
        qWarning(logWarning()) << "Could not open process" << GetLastError();
        return;
    }

    //if(targetNoFog!=currentNoFog||force){
        ReadProcessMemory(hProcess, FogAddr, temp6, 6, nullptr); // Читаем данные из памяти процесса "Dawn of War: Soulstorm" по адресу "Тумана" (FogAddr) и записываем в буфер temp6 в количестве 6 байт
        ReadProcessMemory(hProcess, Float512Addr, temp4, 4, nullptr);
        ReadProcessMemory(hProcess, MapSkyDistanceAddr, temp6_2, 6, nullptr);

        if(targetNoFog&&memcmp(temp6, CodeFog, 6)==0){ // Проверяем совпали ли прочитанные ранее данные "Тумана" с базовыми оригинальными данными (заведомо нам известны).
            qInfo(logInfo()) << "Enable NoFOG";
            WriteProcessMemory(hProcess, FogAddr, nop_array6, 6, nullptr); // Записываем данные в память процесса "Dawn of War: Soulstorm" по адресу "Тумана" (FogAddr) из буфера nop_array6 в количестве 6 байт
        } else if(!targetNoFog){ // Если данные не совпали и/или необходимо вернуть оригинальные данные
            qInfo(logInfo()) << "Disable NoFOG";
            WriteProcessMemory(hProcess, FogAddr, CodeFog, 6, nullptr);
        }
        if(targetNoFog&&memcmp(temp4, CodeF512, 4)==0){
            VirtualProtectEx(hProcess, Float512Addr, 4, PAGE_EXECUTE_READWRITE, &Float512OldProtect);
            if(!WriteProcessMemory(hProcess, Float512Addr, Float512, 4, nullptr))
                qWarning(logWarning()) << "Could not write CodeF512 to memory";
            VirtualProtectEx(hProcess, Float512Addr, 4, Float512OldProtect, nullptr);
        } else if(!targetNoFog){
            //qDebug() << "temp4 is not equal to CodeF512";
            VirtualProtectEx(hProcess, Float512Addr, 4, PAGE_EXECUTE_READWRITE, &Float512OldProtect);
            WriteProcessMemory(hProcess, Float512Addr, CodeF512, 4, nullptr);
            VirtualProtectEx(hProcess, Float512Addr, 4, Float512OldProtect, nullptr);
        }
        if(targetNoFog&&memcmp(temp6_2, CodeMapSkyDistance, 6)==0){
            if(!WriteProcessMemory(hProcess, MapSkyDistanceAddr, nop_array6, 6, nullptr))
                qWarning(logWarning()) << "Could not write CodeMapSkyDistance to memory";
        } else if(!targetNoFog){
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


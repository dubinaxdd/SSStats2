#include <soulstormMemoryController.h>

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

SoulstormMemoryController::SoulstormMemoryController(SettingsController* settingsController, QObject *parent)
    : QObject(parent)
    , m_settingsController(settingsController)
{
    QObject::connect(m_settingsController, &SettingsController::settingsLoaded, this, &SoulstormMemoryController::onSettingsLoaded,Qt::QueuedConnection);
}

void SoulstormMemoryController::onSsLaunchStateChanged(bool state)
{
    m_ssLaunchState = state;

    if (!m_ssLaunchState)
        return;

    onNoFogStateChanged(currentNoFog && m_ssLaunchState);
}

void SoulstormMemoryController::onSettingsLoaded()
{
    qInfo(logInfo()) << "SoulstormMemoryController::onSettingsLoaded()" << "load started";

    currentNoFog = m_settingsController->getSettings()->noFog;
    onNoFogStateChanged(currentNoFog && m_ssLaunchState);

    qInfo(logInfo()) << "SoulstormMemoryController::onSettingsLoaded()" << "load finished";
}

void SoulstormMemoryController::setSoulstormHwnd(HWND newSoulstormHwnd)
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

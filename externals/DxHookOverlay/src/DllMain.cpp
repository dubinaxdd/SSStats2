#include <Windows.h>
#include "DX9Hook.h"

namespace
{
DWORD WINAPI InitializeThread(LPVOID)
{
    const bool hookInstalled =
        InstallDX9Hook();

    // Shutdown thread должен быть запущен
    // даже если установка hook не удалась.
    StartDX9HookShutdownThread();

    return hookInstalled ? 0 : 1;
}
}

BOOL APIENTRY DllMain(
    HMODULE hModule,
    DWORD reason,
    LPVOID
    )
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);

        SetDX9HookModule(hModule);

        HANDLE thread = CreateThread(
            nullptr,
            0,
            InitializeThread,
            nullptr,
            0,
            nullptr
            );

        if (thread)
        {
            CloseHandle(thread);
        }
    }

    return TRUE;
}

#include <Windows.h>
#include "DX9Hook.h"

namespace
{
DWORD WINAPI InitializeThread(LPVOID)
{
    InstallDX9Hook();
    return 0;
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

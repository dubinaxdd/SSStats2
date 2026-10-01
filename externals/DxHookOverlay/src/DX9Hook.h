#pragma once

#include <Windows.h>

bool InstallDX9Hook();

// Передаём DLL handle из DllMain.
// Нужен для FreeLibraryAndExitThread.
void SetDX9HookModule(HMODULE module);

// Создаёт shutdown event/thread.
bool StartDX9HookShutdownThread();

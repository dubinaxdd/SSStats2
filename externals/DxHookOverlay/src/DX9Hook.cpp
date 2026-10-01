#include "DX9Hook.h"

#include <Windows.h>
#include <d3d9.h>

#include <atomic>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <thread>

#pragma comment(lib, "d3d9.lib")

// ============================================================
// Configuration
// ============================================================

namespace
{
constexpr SIZE_T STOLEN_BYTES = 18;

constexpr wchar_t SHARED_IMAGE_NAME[] =
    L"Local\\DX9OverlayImage";

constexpr wchar_t SHUTDOWN_EVENT_NAME[] =
    L"Local\\DX9OverlayShutdown";

// ========================================================
// Types
// ========================================================

using PresentFn = HRESULT(WINAPI*)(
    IDirect3DDevice9*,
    const RECT*,
    const RECT*,
    HWND,
    const RGNDATA*
    );

struct SharedImage
{
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t format;

    uint64_t frame;

    LONG activeBuffer;

    unsigned char pixels[3840 * 2160 * 4 * 2];
};

// ========================================================
// Globals
// ========================================================

HMODULE g_module = nullptr;

PresentFn OriginalPresent = nullptr;

void* PresentAddress = nullptr;
void* Trampoline = nullptr;

IDirect3DTexture9* g_texture = nullptr;

HANDLE g_mapping = nullptr;
SharedImage* g_sharedImage = nullptr;

std::mutex g_sharedImageMutex;
std::mutex g_textureMutex;

std::atomic<bool> g_shutdownRequested{ false };
std::atomic<bool> g_hookInstalled{ false };

// Количество Present, которые сейчас выполняются.
std::atomic<LONG> g_activePresentCalls{ 0 };

// Initialization thread выставляет true после InstallDX9Hook().
std::atomic<bool> g_initializationFinished{ false };

HANDLE g_shutdownEvent = nullptr;
HANDLE g_shutdownThread = nullptr;

// Оригинальные байты Present.
uint8_t g_originalBytes[STOLEN_BYTES]{};

// ========================================================
// Forward declarations
// ========================================================

void ShutdownDX9HookInternal();

DWORD WINAPI ShutdownThread(LPVOID);

// ========================================================
// Helpers
// ========================================================

void SetMemoryProtection(
    void* address,
    SIZE_T size,
    DWORD newProtection,
    DWORD& oldProtection
    )
{
    VirtualProtect(
        address,
        size,
        newProtection,
        &oldProtection
        );
}

bool WriteAbsoluteJump(
    void* source,
    void* destination
    )
{
    if (!source || !destination)
    {
        return false;
    }

    DWORD oldProtection = 0;

    if (!VirtualProtect(
            source,
            14,
            PAGE_EXECUTE_READWRITE,
            &oldProtection))
    {
        return false;
    }

    uint8_t patch[14]{};

    // mov rax, imm64
    patch[0] = 0x48;
    patch[1] = 0xB8;

    std::uint64_t address =
        reinterpret_cast<std::uint64_t>(destination);

    std::memcpy(
        &patch[2],
        &address,
        sizeof(address)
        );

    // jmp rax
    patch[10] = 0xFF;
    patch[11] = 0xE0;

    // padding
    patch[12] = 0x90;
    patch[13] = 0x90;

    std::memcpy(
        source,
        patch,
        sizeof(patch)
        );

    FlushInstructionCache(
        GetCurrentProcess(),
        source,
        sizeof(patch)
        );

    DWORD dummy = 0;

    VirtualProtect(
        source,
        14,
        oldProtection,
        &dummy
        );

    return true;
}

// ========================================================
// Resolve possible JMP
// ========================================================

void* ResolveJump(void* address)
{
    if (!address)
    {
        return nullptr;
    }

    auto* bytes =
        reinterpret_cast<uint8_t*>(address);

    // E9 rel32
    if (bytes[0] == 0xE9)
    {
        int32_t relative = 0;

        std::memcpy(
            &relative,
            bytes + 1,
            sizeof(relative)
            );

        return
            reinterpret_cast<uint8_t*>(address)
            + 5
            + relative;
    }

    // FF 25 [rip+rel32]
    if (bytes[0] == 0xFF &&
        bytes[1] == 0x25)
    {
        int32_t relative = 0;

        std::memcpy(
            &relative,
            bytes + 2,
            sizeof(relative)
            );

        auto** absoluteAddress =
            reinterpret_cast<void**>(
                reinterpret_cast<uint8_t*>(address)
                + 6
                + relative
                );

        return *absoluteAddress;
    }

    return address;
}

// ========================================================
// Trampoline
// ========================================================

void* CreateTrampoline(
    void* target,
    SIZE_T stolenBytes
    )
{
    if (!target || stolenBytes < 14)
    {
        return nullptr;
    }

    constexpr SIZE_T TRAMPOLINE_SIZE = 64;

    auto* trampoline =
        reinterpret_cast<uint8_t*>(
            VirtualAlloc(
                nullptr,
                TRAMPOLINE_SIZE,
                MEM_COMMIT | MEM_RESERVE,
                PAGE_EXECUTE_READWRITE
                )
            );

    if (!trampoline)
    {
        return nullptr;
    }

    std::memcpy(
        trampoline,
        target,
        stolenBytes
        );

    auto* jumpBack =
        trampoline + stolenBytes;

    uint8_t patch[14]{};

    // mov rax, target + stolenBytes
    patch[0] = 0x48;
    patch[1] = 0xB8;

    std::uint64_t returnAddress =
        reinterpret_cast<std::uint64_t>(
            reinterpret_cast<uint8_t*>(target)
            + stolenBytes
            );

    std::memcpy(
        &patch[2],
        &returnAddress,
        sizeof(returnAddress)
        );

    // jmp rax
    patch[10] = 0xFF;
    patch[11] = 0xE0;

    patch[12] = 0x90;
    patch[13] = 0x90;

    std::memcpy(
        jumpBack,
        patch,
        sizeof(patch)
        );

    FlushInstructionCache(
        GetCurrentProcess(),
        trampoline,
        TRAMPOLINE_SIZE
        );

    return trampoline;
}

// ========================================================
// Shared memory
// ========================================================

bool OpenSharedImage()
{
    std::lock_guard<std::mutex> lock(
        g_sharedImageMutex
        );

    if (g_sharedImage)
    {
        return true;
    }

    g_mapping = OpenFileMappingW(
        FILE_MAP_READ,
        FALSE,
        SHARED_IMAGE_NAME
        );

    if (!g_mapping)
    {
        return false;
    }

    g_sharedImage =
        reinterpret_cast<SharedImage*>(
            MapViewOfFile(
                g_mapping,
                FILE_MAP_READ,
                0,
                0,
                sizeof(SharedImage)
                )
            );

    if (!g_sharedImage)
    {
        CloseHandle(g_mapping);
        g_mapping = nullptr;

        return false;
    }

    return true;
}

void CloseSharedImage()
{
    std::lock_guard<std::mutex> lock(
        g_sharedImageMutex
        );

    if (g_sharedImage)
    {
        UnmapViewOfFile(g_sharedImage);
        g_sharedImage = nullptr;
    }

    if (g_mapping)
    {
        CloseHandle(g_mapping);
        g_mapping = nullptr;
    }
}

// ========================================================
// Texture
// ========================================================

void ReleaseTexture()
{
    std::lock_guard<std::mutex> lock(
        g_textureMutex
        );

    if (g_texture)
    {
        g_texture->Release();
        g_texture = nullptr;
    }
}

bool CreateImageTexture(
    IDirect3DDevice9* device,
    uint32_t width,
    uint32_t height
    )
{
    if (!device ||
        width == 0 ||
        height == 0)
    {
        return false;
    }

    std::lock_guard<std::mutex> lock(
        g_textureMutex
        );

    if (g_texture)
    {
        D3DSURFACE_DESC desc{};

        if (SUCCEEDED(
                g_texture->GetLevelDesc(
                    0,
                    &desc
                    )))
        {
            if (desc.Width == width &&
                desc.Height == height)
            {
                return true;
            }
        }

        g_texture->Release();
        g_texture = nullptr;
    }

    HRESULT hr =
        device->CreateTexture(
            width,
            height,
            1,
            D3DUSAGE_DYNAMIC,
            D3DFMT_A8R8G8B8,
            D3DPOOL_DEFAULT,
            &g_texture,
            nullptr
            );

    return SUCCEEDED(hr);
}

bool UpdateTextureFromSharedMemory()
{
    if (!g_sharedImage ||
        !g_texture)
    {
        return false;
    }

    D3DLOCKED_RECT locked{};

    if (FAILED(
            g_texture->LockRect(
                0,
                &locked,
                nullptr,
                D3DLOCK_DISCARD
                )))
    {
        return false;
    }

    const uint32_t width =
        g_sharedImage->width;

    const uint32_t height =
        g_sharedImage->height;

    const uint32_t pitch =
        g_sharedImage->pitch;

    if (width == 0 ||
        height == 0 ||
        width > 3840 ||
        height > 2160)
    {
        g_texture->UnlockRect(0);
        return false;
    }

    const uint8_t* source =
        g_sharedImage->pixels;

    auto* destination =
        reinterpret_cast<uint8_t*>(
            locked.pBits
            );

    for (uint32_t y = 0; y < height; ++y)
    {
        std::memcpy(
            destination + y * locked.Pitch,
            source + y * pitch,
            width * 4
            );
    }

    g_texture->UnlockRect(0);

    return true;
}

// ========================================================
// Drawing
// ========================================================

void DrawOverlayTexture(IDirect3DDevice9* device, IDirect3DTexture9* texture)
{
    if (!device || !texture)
        return;

    struct Vertex
    {
        float x;
        float y;
        float z;
        float rhw;
        float u;
        float v;
    };

    D3DVIEWPORT9 viewport = {};
    if (FAILED(device->GetViewport(&viewport)))
        return;

    const float width = static_cast<float>(viewport.Width);
    const float height = static_cast<float>(viewport.Height);

    // Смещение -0.5f для точного пиксельного соответствия в DX9
    Vertex vertices[] =
        {
            { -0.5f,        -0.5f,         0.0f, 1.0f, 0.0f, 0.0f },
            { width - 0.5f, -0.5f,         0.0f, 1.0f, 1.0f, 0.0f },
            { -0.5f,        height - 0.5f, 0.0f, 1.0f, 0.0f, 1.0f },
            { width - 0.5f, height - 0.5f, 0.0f, 1.0f, 1.0f, 1.0f }
        };

    // Принудительно отключаем программируемый конвейер игры (шейдеры)
    device->SetVertexShader(nullptr);
    device->SetPixelShader(nullptr);

    // Настройка текстуры и формата вершин
    device->SetTexture(0, texture);
    device->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);

    // Изолируем 2D-отрисовку от 3D-мира игры
    device->SetRenderState(D3DRS_LIGHTING, FALSE);
    device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    device->SetRenderState(D3DRS_ZENABLE, FALSE);
    device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    device->SetRenderState(D3DRS_FOGENABLE, FALSE); // Отключаем игровой туман

    // Настройка прозрачности (Alpha Blending)
    device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    // Блокируем влияние цвета задника: берем строго цвет из текстуры
    device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

    device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

    // Фильтрация
    device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
    device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

    // Отрисовка
    device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(Vertex));

    device->SetTexture(0, nullptr);
}

void DrawRedSquare(
    IDirect3DDevice9* device
    )
{
    if (!device)
    {
        return;
    }

    struct Vertex
    {
        float x;
        float y;
        float z;
        float rhw;
        DWORD color;
    };

    constexpr DWORD FVF =
        D3DFVF_XYZRHW | D3DFVF_DIFFUSE;

    Vertex vertices[4]{};

    vertices[0] = {
        20.0f,
        20.0f,
        0.0f,
        1.0f,
        0xFFFF0000
    };

    vertices[1] = {
        120.0f,
        20.0f,
        0.0f,
        1.0f,
        0xFFFF0000
    };

    vertices[2] = {
        120.0f,
        120.0f,
        0.0f,
        1.0f,
        0xFFFF0000
    };

    vertices[3] = {
        20.0f,
        120.0f,
        0.0f,
        1.0f,
        0xFFFF0000
    };

    device->SetTexture(
        0,
        nullptr
        );

    device->SetFVF(FVF);

    device->DrawPrimitiveUP(
        D3DPT_TRIANGLEFAN,
        2,
        vertices,
        sizeof(Vertex)
        );
}

// ========================================================
// Hooked Present
// ========================================================

HRESULT WINAPI HookedPresent(
    IDirect3DDevice9* device,
    const RECT* sourceRect,
    const RECT* destRect,
    HWND destWindowOverride,
    const RGNDATA* dirtyRegion
    )
{
    // Очень важно: счётчик увеличивается сразу при входе в hook.
    g_activePresentCalls.fetch_add(
        1,
        std::memory_order_acq_rel
        );

    HRESULT result = D3D_OK;

    // После начала shutdown новый overlay больше не выполняем.
    if (!g_shutdownRequested.load(
            std::memory_order_acquire))
    {
        if (device)
        {
            // 1. Создаем стейтблок и захватываем состояние ДО открытия нашей сцены
            IDirect3DStateBlock9* stateBlock = nullptr;
            if (SUCCEEDED(device->CreateStateBlock(D3DSBT_ALL, &stateBlock)))
            {
                stateBlock->Capture();
            }

            // 2. Открываем сцену (Present всегда идет ПОСЛЕ игрового EndScene)
            bool sceneOpenedByUs = false;
            if (SUCCEEDED(device->BeginScene()))
            {
                sceneOpenedByUs = true;
            }

            // 3. Выполняем логику работы с памятью и рендера
            if (OpenSharedImage())
            {
                const uint32_t width = g_sharedImage->width;
                const uint32_t height = g_sharedImage->height;

                if (width > 0 &&
                    height > 0 &&
                    width <= 3840 &&
                    height <= 2160)
                {
                    if (CreateImageTexture(device, width, height))
                    {
                        UpdateTextureFromSharedMemory();
                        DrawOverlayTexture(device, g_texture);
                    }
                }
            }

            // На всякий случай гасим шейдеры и перед квадратом, если функция DrawRedSquare их не сбрасывает
            device->SetVertexShader(nullptr);
            device->SetPixelShader(nullptr);
            DrawRedSquare(device);

            // 4. Закрываем сцену
            if (sceneOpenedByUs)
            {
                device->EndScene();
            }

            // 5. Возвращаем игре её стейты и шейдеры в исходное состояние
            if (stateBlock)
            {
                stateBlock->Apply();
                stateBlock->Release();
            }
        }
    }

    if (OriginalPresent)
    {
        result = OriginalPresent(
            device,
            sourceRect,
            destRect,
            destWindowOverride,
            dirtyRegion
            );
    }

    g_activePresentCalls.fetch_sub(
        1,
        std::memory_order_acq_rel
        );

    return result;
}



// ========================================================
// Restore original Present
// ========================================================

bool RestoreOriginalPresent()
{
    if (!PresentAddress)
    {
        return true;
    }

    DWORD oldProtection = 0;

    if (!VirtualProtect(
            PresentAddress,
            STOLEN_BYTES,
            PAGE_EXECUTE_READWRITE,
            &oldProtection))
    {
        return false;
    }

    std::memcpy(
        PresentAddress,
        g_originalBytes,
        STOLEN_BYTES
        );

    FlushInstructionCache(
        GetCurrentProcess(),
        PresentAddress,
        STOLEN_BYTES
        );

    DWORD dummy = 0;

    VirtualProtect(
        PresentAddress,
        STOLEN_BYTES,
        oldProtection,
        &dummy
        );

    return true;
}

// ========================================================
// Dummy D3D9 device
// ========================================================

IDirect3DDevice9* CreateDummyDevice(
    IDirect3D9** outD3D
    )
{
    if (!outD3D)
    {
        return nullptr;
    }

    *outD3D = nullptr;

    IDirect3D9* d3d =
        Direct3DCreate9(D3D_SDK_VERSION);

    if (!d3d)
    {
        return nullptr;
    }

    HWND window =
        GetDesktopWindow();

    D3DPRESENT_PARAMETERS pp{};

    pp.Windowed = TRUE;
    pp.SwapEffect =
        D3DSWAPEFFECT_DISCARD;

    pp.hDeviceWindow = window;

    IDirect3DDevice9* device = nullptr;

    HRESULT hr =
        d3d->CreateDevice(
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            window,
            D3DCREATE_SOFTWARE_VERTEXPROCESSING,
            &pp,
            &device
            );

    if (FAILED(hr))
    {
        d3d->Release();
        return nullptr;
    }

    *outD3D = d3d;

    return device;
}

// ========================================================
// Install hook
// ========================================================

bool InstallHook()
{
    IDirect3D9* d3d = nullptr;

    IDirect3DDevice9* dummyDevice =
        CreateDummyDevice(&d3d);

    if (!dummyDevice)
    {
        return false;
    }

    void** vtable =
        *reinterpret_cast<void***>(
            dummyDevice
            );

    // IDirect3DDevice9::Present = index 17
    void* present =
        vtable[17];

    PresentAddress =
        ResolveJump(present);

    if (!PresentAddress)
    {
        dummyDevice->Release();
        d3d->Release();

        return false;
    }

    // Сохраняем оригинальные байты ДО патча.
    std::memcpy(
        g_originalBytes,
        PresentAddress,
        STOLEN_BYTES
        );

    Trampoline =
        CreateTrampoline(
            PresentAddress,
            STOLEN_BYTES
            );

    if (!Trampoline)
    {
        dummyDevice->Release();
        d3d->Release();

        PresentAddress = nullptr;

        return false;
    }

    OriginalPresent =
        reinterpret_cast<PresentFn>(
            Trampoline
            );

    if (!WriteAbsoluteJump(
            PresentAddress,
            reinterpret_cast<void*>(
                &HookedPresent
                )))
    {
        VirtualFree(
            Trampoline,
            0,
            MEM_RELEASE
            );

        Trampoline = nullptr;
        OriginalPresent = nullptr;
        PresentAddress = nullptr;

        dummyDevice->Release();
        d3d->Release();

        return false;
    }

    g_hookInstalled.store(
        true,
        std::memory_order_release
        );

    dummyDevice->Release();
    d3d->Release();

    return true;
}

// ========================================================
// Shutdown
// ========================================================

void ShutdownDX9HookInternal()
{
    g_shutdownRequested.store(
        true,
        std::memory_order_release
        );

    // ----------------------------------------------------
    // 1. Restore original Present.
    // ----------------------------------------------------

    if (g_hookInstalled.load(
            std::memory_order_acquire))
    {
        RestoreOriginalPresent();

        g_hookInstalled.store(
            false,
            std::memory_order_release
            );
    }

    // ----------------------------------------------------
    // 2. Ждём, пока уже выполняющиеся HookedPresent
    //    полностью закончатся.
    // ----------------------------------------------------

    for (;;)
    {
        LONG active =
            g_activePresentCalls.load(
                std::memory_order_acquire
                );

        if (active == 0)
        {
            break;
        }

        Sleep(1);
    }

    // ----------------------------------------------------
    // 3. После этого HookedPresent больше не использует
    //    texture/shared memory/trampoline.
    // ----------------------------------------------------

    ReleaseTexture();

    CloseSharedImage();

    // ----------------------------------------------------
    // 4. Освобождаем trampoline.
    // ----------------------------------------------------

    if (Trampoline)
    {
        VirtualFree(
            Trampoline,
            0,
            MEM_RELEASE
            );

        Trampoline = nullptr;
    }

    OriginalPresent = nullptr;
    PresentAddress = nullptr;
}

// ========================================================
// Shutdown thread
// ========================================================

DWORD WINAPI ShutdownThread(
    LPVOID
    )
{
    HANDLE event = g_shutdownEvent;

    if (!event)
    {
        return 0;
    }

    // Ждём команды от внешнего injector/uninjector.
    WaitForSingleObject(
        event,
        INFINITE
        );

    // Если shutdown пришёл во время InstallDX9Hook(),
    // ждём окончания initialization thread.
    while (!g_initializationFinished.load(
        std::memory_order_acquire))
    {
        Sleep(1);
    }

    ShutdownDX9HookInternal();

    // Закрываем event ДО выгрузки DLL.
    if (g_shutdownEvent)
    {
        CloseHandle(g_shutdownEvent);
        g_shutdownEvent = nullptr;
    }

    HMODULE module = g_module;

    // Здесь НЕЛЬЗЯ делать обычный return после FreeLibrary.
    //
    // FreeLibraryAndExitThread() атомарно уменьшает
    // refcount DLL и завершает этот thread.
    //
    // Поэтому после этой функции управление обратно
    // в код DLL не возвращается.
    if (module)
    {
        FreeLibraryAndExitThread(
            module,
            0
            );
    }

    return 0;
}

} // anonymous namespace


// ============================================================
// Public API
// ============================================================

void SetDX9HookModule(
    HMODULE module
    )
{
    g_module = module;
}


bool InstallDX9Hook()
{
    return InstallHook();
}


bool StartDX9HookShutdownThread()
{
    if (g_shutdownEvent)
    {
        return true;
    }

    g_shutdownEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            SHUTDOWN_EVENT_NAME
            );

    if (!g_shutdownEvent)
    {
        return false;
    }

    g_shutdownThread =
        CreateThread(
            nullptr,
            0,
            ShutdownThread,
            nullptr,
            0,
            nullptr
            );

    if (!g_shutdownThread)
    {
        CloseHandle(g_shutdownEvent);
        g_shutdownEvent = nullptr;

        return false;
    }

    // Handle shutdown thread нам больше не нужен.
    //
    // Сам thread продолжает работать.
    CloseHandle(g_shutdownThread);
    g_shutdownThread = nullptr;

    g_initializationFinished.store(
        true,
        std::memory_order_release
        );

    return true;
}

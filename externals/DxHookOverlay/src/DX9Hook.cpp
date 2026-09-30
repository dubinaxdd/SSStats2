#include "DX9Hook.h"

#include <Windows.h>
#include <d3d9.h>
#include <cstdint>
#include <cstring>
#include <cstdio>

#pragma comment(lib, "d3d9.lib")

namespace
{
constexpr SIZE_T STOLEN_BYTES = 18;
constexpr SIZE_T ABSOLUTE_JUMP_SIZE = 14;

using PresentFn = HRESULT(WINAPI*)(
    IDirect3DDevice9* device,
    const RECT* sourceRect,
    const RECT* destRect,
    HWND destWindowOverride,
    const RGNDATA* dirtyRegion
    );

PresentFn OriginalPresent = nullptr;

void* PresentAddress = nullptr;
void* Trampoline = nullptr;


// ------------------------------------------------------------
// Resolve a possible jump/thunk
// ------------------------------------------------------------

void* ResolveJump(void* address)
{
    if (!address)
        return nullptr;

    auto* p = reinterpret_cast<uint8_t*>(address);

    // E9 rel32
    if (p[0] == 0xE9)
    {
        int32_t relative =
            *reinterpret_cast<int32_t*>(p + 1);

        return p + 5 + relative;
    }

    // FF 25 rel32
    // jmp qword ptr [rip + rel32]
    if (p[0] == 0xFF && p[1] == 0x25)
    {
        int32_t relative =
            *reinterpret_cast<int32_t*>(p + 2);

        auto** target =
            reinterpret_cast<void**>(p + 6 + relative);

        return *target;
    }

    return address;
}


// ------------------------------------------------------------
// Write:
//
// mov rax, destination
// jmp rax
// nop
// nop
//
// 14 bytes total
// ------------------------------------------------------------

bool WriteAbsoluteJump(
    void* source,
    void* destination)
{
    if (!source || !destination)
        return false;

    auto* p =
        reinterpret_cast<uint8_t*>(source);

    DWORD oldProtect = 0;

    if (!VirtualProtect(
            source,
            ABSOLUTE_JUMP_SIZE,
            PAGE_EXECUTE_READWRITE,
            &oldProtect))
    {
        return false;
    }

    // mov rax, imm64
    p[0] = 0x48;
    p[1] = 0xB8;

    *reinterpret_cast<uint64_t*>(p + 2) =
        reinterpret_cast<uint64_t>(destination);

    // jmp rax
    p[10] = 0xFF;
    p[11] = 0xE0;

    // padding
    p[12] = 0x90;
    p[13] = 0x90;

    FlushInstructionCache(
        GetCurrentProcess(),
        source,
        ABSOLUTE_JUMP_SIZE
        );

    DWORD temp = 0;

    VirtualProtect(
        source,
        ABSOLUTE_JUMP_SIZE,
        oldProtect,
        &temp
        );

    return true;
}


// ------------------------------------------------------------
// Create trampoline
//
// Original:
//
// target:
//   [18 stolen bytes]
//
// trampoline:
//   [18 original bytes]
//   jmp target + 18
// ------------------------------------------------------------

void* CreateTrampoline(void* target)
{
    if (!target)
        return nullptr;

    constexpr SIZE_T trampolineSize =
        STOLEN_BYTES + ABSOLUTE_JUMP_SIZE;

    auto* trampoline =
        reinterpret_cast<uint8_t*>(
            VirtualAlloc(
                nullptr,
                trampolineSize,
                MEM_COMMIT | MEM_RESERVE,
                PAGE_EXECUTE_READWRITE
                )
            );

    if (!trampoline)
        return nullptr;

    // Copy the first 18 bytes.
    //
    // Based on the Present bytes you showed:
    //
    // 40 53             2
    // 55                1
    // 56                1
    // 57                1
    // 41 56             3
    // 41 57             3
    // 48 81 EC ...      7
    //
    // Total = 18 bytes.
    //
    // The next instruction is RIP-relative,
    // so we deliberately do NOT copy it.

    std::memcpy(
        trampoline,
        target,
        STOLEN_BYTES
        );

    // Jump from trampoline back to:
    //
    // target + 18
    //
    if (!WriteAbsoluteJump(
            trampoline + STOLEN_BYTES,
            reinterpret_cast<uint8_t*>(target) + STOLEN_BYTES))
    {
        VirtualFree(
            trampoline,
            0,
            MEM_RELEASE
            );

        return nullptr;
    }

    FlushInstructionCache(
        GetCurrentProcess(),
        trampoline,
        trampolineSize
        );

    return trampoline;
}


// ------------------------------------------------------------
// Our Present hook
// ------------------------------------------------------------

/*HRESULT WINAPI HookedPresent(
    IDirect3DDevice9* device,
    const RECT* sourceRect,
    const RECT* destRect,
    HWND destWindowOverride,
    const RGNDATA* dirtyRegion)
{
    static bool shown = false;

    if (!shown)
    {
        shown = true;

        char buffer[256];

        std::snprintf(
            buffer,
            sizeof(buffer),
            "GAME Present reached\n"
            "device = %p\n"
            "OriginalPresent = %p",
            device,
            reinterpret_cast<void*>(OriginalPresent)
            );

        MessageBoxA(
            nullptr,
            buffer,
            "DX9 HOOK",
            MB_OK
            );
    }

    // IMPORTANT:
    //
    // Do NOT return D3D_OK here.
    //
    // Do NOT restore the original bytes here.
    //
    // Do NOT modify the vtable here.
    //
    // We simply continue into the original Present.

    if (!OriginalPresent)
    {
        return E_FAIL;
    }

    return OriginalPresent(
        device,
        sourceRect,
        destRect,
        destWindowOverride,
        dirtyRegion
        );
}*/

void DrawRedSquare(IDirect3DDevice9* device)
{
    struct Vertex
    {
        float x;
        float y;
        float z;
        float rhw;
        DWORD color;
    };

    Vertex vertices[] =
        {
            { 20.0f,  20.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(255, 0, 0) },
            {120.0f,  20.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(255, 0, 0) },
            { 20.0f, 120.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(255, 0, 0) },
            {120.0f, 120.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(255, 0, 0) }
        };

    IDirect3DStateBlock9* stateBlock = nullptr;

    if (SUCCEEDED(
            device->CreateStateBlock(
                D3DSBT_ALL,
                &stateBlock)))
    {
        stateBlock->Capture();
    }

    device->SetFVF(
        D3DFVF_XYZRHW | D3DFVF_DIFFUSE
        );

    device->SetRenderState(
        D3DRS_LIGHTING,
        FALSE
        );

    device->SetRenderState(
        D3DRS_ZENABLE,
        FALSE
        );

    device->SetRenderState(
        D3DRS_ALPHABLENDENABLE,
        FALSE
        );

    device->DrawPrimitiveUP(
        D3DPT_TRIANGLESTRIP,
        2,
        vertices,
        sizeof(Vertex)
        );

    if (stateBlock)
    {
        stateBlock->Apply();
        stateBlock->Release();
    }
}

HRESULT WINAPI HookedPresent(
    IDirect3DDevice9* device,
    const RECT* sourceRect,
    const RECT* destRect,
    HWND destWindowOverride,
    const RGNDATA* dirtyRegion)
{
    static bool shown = false;

    if (!shown)
    {
        shown = true;

        MessageBoxA(
            nullptr,
            "GAME Present reached",
            "DX9 HOOK",
            MB_OK
            );
    }

    if (device)
    {
        DrawRedSquare(device);
    }

    return OriginalPresent(
        device,
        sourceRect,
        destRect,
        destWindowOverride,
        dirtyRegion
        );
}


// ------------------------------------------------------------
// Create dummy DX9 device
// ------------------------------------------------------------

IDirect3DDevice9* CreateDummyDevice(
    IDirect3D9** outD3D)
{
    if (!outD3D)
        return nullptr;

    *outD3D = nullptr;

    IDirect3D9* d3d =
        Direct3DCreate9(D3D_SDK_VERSION);

    if (!d3d)
        return nullptr;

    HWND hwnd =
        GetForegroundWindow();

    if (!hwnd)
    {
        d3d->Release();
        return nullptr;
    }

    D3DPRESENT_PARAMETERS pp = {};

    pp.Windowed = TRUE;
    pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    pp.hDeviceWindow = hwnd;

    IDirect3DDevice9* device = nullptr;

    HRESULT hr =
        d3d->CreateDevice(
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            hwnd,
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


// ------------------------------------------------------------
// Install hook
// ------------------------------------------------------------

bool InstallHook()
{
    IDirect3D9* d3d = nullptr;

    IDirect3DDevice9* dummyDevice =
        CreateDummyDevice(&d3d);

    if (!dummyDevice)
    {
        MessageBoxA(
            nullptr,
            "Failed to create dummy D3D9 device",
            "DX9 HOOK",
            MB_OK
            );

        return false;
    }

    // IDirect3DDevice9::Present is vtable index 17.
    void** vtable =
        *reinterpret_cast<void***>(dummyDevice);

    void* presentThunk =
        vtable[17];

    // Resolve possible E9 / FF 25 thunk.
    void* resolvedPresent =
        ResolveJump(presentThunk);

    if (!resolvedPresent)
    {
        dummyDevice->Release();
        d3d->Release();

        MessageBoxA(
            nullptr,
            "Failed to resolve Present",
            "DX9 HOOK",
            MB_OK
            );

        return false;
    }

    PresentAddress = resolvedPresent;

    // Show what we found.
    {
        char buffer[512];

        std::snprintf(
            buffer,
            sizeof(buffer),
            "D3D9 Present found\n\n"
            "vtable[17] = %p\n"
            "resolved   = %p",
            presentThunk,
            resolvedPresent
            );

        MessageBoxA(
            nullptr,
            buffer,
            "DX9 HOOK",
            MB_OK
            );
    }

    // --------------------------------------------------------
    // Create trampoline BEFORE modifying Present.
    // --------------------------------------------------------

    Trampoline =
        CreateTrampoline(resolvedPresent);

    if (!Trampoline)
    {
        dummyDevice->Release();
        d3d->Release();

        MessageBoxA(
            nullptr,
            "Failed to create Present trampoline",
            "DX9 HOOK",
            MB_OK
            );

        return false;
    }

    OriginalPresent =
        reinterpret_cast<PresentFn>(Trampoline);

    // --------------------------------------------------------
    // Install permanent inline hook.
    //
    // We DO NOT restore these bytes later.
    // --------------------------------------------------------

    if (!WriteAbsoluteJump(
            resolvedPresent,
            reinterpret_cast<void*>(&HookedPresent)))
    {
        VirtualFree(
            Trampoline,
            0,
            MEM_RELEASE
            );

        Trampoline = nullptr;
        OriginalPresent = nullptr;

        dummyDevice->Release();
        d3d->Release();

        MessageBoxA(
            nullptr,
            "Failed to patch Present",
            "DX9 HOOK",
            MB_OK
            );

        return false;
    }

    // We no longer need the dummy device.
    dummyDevice->Release();
    d3d->Release();

    MessageBoxA(
        nullptr,
        "Bootstrap Present hook installed.\n\n"
        "Waiting for GAME Present...",
        "DX9 HOOK",
        MB_OK
        );

    return true;
}
}


// ------------------------------------------------------------
// Public entry
// ------------------------------------------------------------

bool InstallDX9Hook()
{
    return InstallHook();
}

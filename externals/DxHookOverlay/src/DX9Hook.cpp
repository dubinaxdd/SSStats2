#include "DX9Hook.h"

#include <Windows.h>
#include <d3d9.h>

#include <cstdint>
#include <cstring>
#include <cstdio>

#pragma comment(lib, "d3d9.lib")

#include <cstddef>



namespace
{

// ============================================================
// НЕ ТРОГАЕМ РАБОЧУЮ ЧАСТЬ HOOK
// ============================================================

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

constexpr wchar_t SHARED_MEMORY_NAME[] = L"Local\\DX9OverlayImage";

constexpr uint32_t MAX_IMAGE_WIDTH = 3840;
constexpr uint32_t MAX_IMAGE_HEIGHT = 2160;
constexpr uint32_t BYTES_PER_PIXEL = 4;

constexpr uint32_t MAX_IMAGE_SIZE = MAX_IMAGE_WIDTH * MAX_IMAGE_HEIGHT * BYTES_PER_PIXEL;

struct SharedImage
{
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t format;
    uint64_t frame;
    volatile LONG activeBuffer;
    uint8_t pixels[2][MAX_IMAGE_SIZE];
};

HANDLE g_sharedMemory = nullptr;
SharedImage* g_sharedImage = nullptr;
uint8_t* g_sharedPixels = nullptr;

IDirect3DTexture9* g_texture = nullptr;
uint32_t g_textureWidth = 0;
uint32_t g_textureHeight = 0;
uint64_t g_lastFrame = 0;

static_assert(offsetof(SharedImage, pixels) == 28, "SharedImage layout mismatch");
static_assert(sizeof(SharedImage) == 66355232, "SharedImage size mismatch");

/*bool UpdateTextureFromSharedMemory(
    IDirect3DDevice9* device)
{
    if (!device || !g_sharedMemory)
        return false;

    if (!g_texture)
        return false;

    constexpr UINT WIDTH = 3840;
    constexpr UINT HEIGHT = 2160;

    constexpr SIZE_T PIXELS_OFFSET = 28;

    constexpr SIZE_T IMAGE_SIZE =
        static_cast<SIZE_T>(WIDTH) *
        static_cast<SIZE_T>(HEIGHT) *
        4;

    uint8_t* view =
        reinterpret_cast<uint8_t*>(
            MapViewOfFile(
                g_sharedMemory,
                FILE_MAP_READ,
                0,
                0,
                PIXELS_OFFSET + IMAGE_SIZE
                )
            );

    if (!view)
        return false;

    bool success = false;

    D3DLOCKED_RECT locked = {};

    HRESULT hr = g_texture->LockRect(0, &locked, nullptr, 0);

    if (SUCCEEDED(hr))
    {
        for (UINT y = 0; y < HEIGHT; ++y)
        {
            std::memcpy(
                reinterpret_cast<uint8_t*>(locked.pBits) +
                    static_cast<SIZE_T>(y) *
                        locked.Pitch,

                view +
                    PIXELS_OFFSET +
                    static_cast<SIZE_T>(y) *
                        WIDTH *
                        4,

                WIDTH * 4
                );
        }

        g_texture->UnlockRect(0);
        success = true;
    }

    UnmapViewOfFile(view);

    return success;
}*/

bool UpdateTextureFromSharedMemory(IDirect3DDevice9* device)
{
    if (!device || !g_sharedMemory || !g_texture)
        return false;

    constexpr SIZE_T PIXELS_OFFSET = 28;

    const UINT width = g_textureWidth;
    const UINT height = g_textureHeight;

    if (width == 0 || height == 0)
        return false;

    const SIZE_T imageSize =
        static_cast<SIZE_T>(width) *
        static_cast<SIZE_T>(height) *
        4;

    uint8_t* view =
        reinterpret_cast<uint8_t*>(
            MapViewOfFile(
                g_sharedMemory,
                FILE_MAP_READ,
                0,
                0,
                PIXELS_OFFSET + imageSize
                )
            );

    if (!view)
        return false;

    D3DLOCKED_RECT locked = {};

    HRESULT hr =
        g_texture->LockRect(
            0,
            &locked,
            nullptr,
            0
            );

    if (FAILED(hr))
    {
        UnmapViewOfFile(view);
        return false;
    }

    for (UINT y = 0; y < height; ++y)
    {
        std::memcpy(
            reinterpret_cast<uint8_t*>(locked.pBits) +
                static_cast<SIZE_T>(y) * locked.Pitch,

            view +
                PIXELS_OFFSET +
                static_cast<SIZE_T>(y) * width * 4,

            static_cast<SIZE_T>(width) * 4
            );
    }

    g_texture->UnlockRect(0);

    UnmapViewOfFile(view);

    return true;
}


void* ResolveJump(void* address)
{
    if (!address)
        return nullptr;

    auto* p = reinterpret_cast<uint8_t*>(address);

    if (p[0] == 0xE9)
    {
        int32_t relative = *reinterpret_cast<int32_t*>(p + 1);
        return p + 5 + relative;
    }

    if (p[0] == 0xFF && p[1] == 0x25)
    {
        int32_t relative = *reinterpret_cast<int32_t*>(p + 2);
        auto** target = reinterpret_cast<void**>(p + 6 + relative);
        return *target;
    }

    return address;
}


bool WriteAbsoluteJump(
    void* source,
    void* destination)
{
    if (!source || !destination)
        return false;

    auto* p =
        reinterpret_cast<uint8_t*>(source);

    DWORD oldProtect = 0;

    if (!VirtualProtect(source, ABSOLUTE_JUMP_SIZE, PAGE_EXECUTE_READWRITE, &oldProtect))
        return false;

    p[0] = 0x48;
    p[1] = 0xB8;

    *reinterpret_cast<uint64_t*>(p + 2) = reinterpret_cast<uint64_t>(destination);

    p[10] = 0xFF;
    p[11] = 0xE0;

    p[12] = 0x90;
    p[13] = 0x90;

    FlushInstructionCache(GetCurrentProcess(), source, ABSOLUTE_JUMP_SIZE);

    DWORD temp = 0;
    VirtualProtect( source, ABSOLUTE_JUMP_SIZE, oldProtect,&temp);
    return true;
}


void* CreateTrampoline(void* target)
{
    if (!target)
        return nullptr;

    constexpr SIZE_T trampolineSize = STOLEN_BYTES + ABSOLUTE_JUMP_SIZE;

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

    std::memcpy(trampoline, target, STOLEN_BYTES);

    if (!WriteAbsoluteJump(trampoline + STOLEN_BYTES, reinterpret_cast<uint8_t*>(target) + STOLEN_BYTES))
    {
        VirtualFree(trampoline, 0, MEM_RELEASE);
        return nullptr;
    }

    FlushInstructionCache(GetCurrentProcess(), trampoline, trampolineSize);
    return trampoline;
}

bool OpenSharedImage()
{
    if (g_sharedImage)
        return true;

    g_sharedMemory = OpenFileMappingW(FILE_MAP_READ, FALSE, SHARED_MEMORY_NAME );

    if (!g_sharedMemory)
        return false;

    g_sharedImage = reinterpret_cast<SharedImage*>(
            MapViewOfFile(
                g_sharedMemory,
                FILE_MAP_READ,
                0,
                0,
                sizeof(SharedImage)
                )
            );

    if (!g_sharedImage)
    {
        CloseHandle(g_sharedMemory);
        g_sharedMemory = nullptr;
        return false;
    }

    // Только после успешного MapViewOfFile.
    MEMORY_BASIC_INFORMATION mbi = {};

    SIZE_T queried = VirtualQuery(g_sharedImage, &mbi, sizeof(mbi));
    return true;
}


void CloseSharedImage()
{
    if (g_sharedImage)
    {
        UnmapViewOfFile(g_sharedImage);
        g_sharedImage = nullptr;
    }

    if (g_sharedMemory)
    {
        CloseHandle(g_sharedMemory);
        g_sharedMemory = nullptr;
    }
}


bool IsValidImageSize(uint32_t width, uint32_t height)
{
    if (width == 0 || height == 0)
        return false;

    if (width > MAX_IMAGE_WIDTH || height > MAX_IMAGE_HEIGHT)
        return false;

    return true;
}

void ReleaseTexture()
{
    if (g_texture)
    {
        g_texture->Release();
        g_texture = nullptr;
    }

    g_textureWidth = 0;
    g_textureHeight = 0;
    g_lastFrame = 0;
}


bool CreateImageTexture(IDirect3DDevice9* device, uint32_t width, uint32_t height)
{
    if (!device)
        return false;

    if (width == 0 || height == 0)
        return false;

    if (width > MAX_IMAGE_WIDTH || height > MAX_IMAGE_HEIGHT)
        return false;

    if (g_texture &&
        g_textureWidth == width &&
        g_textureHeight == height)
    {
        return true;
    }

    if (g_texture)
    {
        g_texture->Release();
        g_texture = nullptr;
    }

    g_textureWidth = 0;
    g_textureHeight = 0;

    HRESULT hr =
        device->CreateTexture(
            width,
            height,
            1,
            0,
            D3DFMT_A8R8G8B8,
            D3DPOOL_MANAGED,
            &g_texture,
            nullptr
            );

    if (FAILED(hr))
    {
        g_texture = nullptr;
        return false;
    }

    g_textureWidth = width;
    g_textureHeight = height;

    return true;
}



bool UpdateImageTexture()
{
    if (!g_sharedImage)
        return false;

    if (!g_texture)
        return false;

    const uint32_t width = g_sharedImage->width;
    const uint32_t height = g_sharedImage->height;
    const uint32_t pitch = g_sharedImage->pitch;

    if (width == 0 || height == 0)
        return false;

    if (width > MAX_IMAGE_WIDTH || height > MAX_IMAGE_HEIGHT)
        return false;

    const uint32_t requiredPitch = width * BYTES_PER_PIXEL;

    if (pitch < requiredPitch)
        return false;


    // Текущий опубликованный буфер.
    const LONG activeBuffer = InterlockedCompareExchange(&g_sharedImage->activeBuffer, 0, 0);

    if (activeBuffer != 0 && activeBuffer != 1)
        return false;

    const uint64_t frame = g_sharedImage->frame;


    // Этот кадр уже загружали.
    if (frame == g_lastFrame)
        return true;


    const uint8_t* source = g_sharedImage->pixels[activeBuffer];

    D3DLOCKED_RECT lockedRect{};

    HRESULT hr = g_texture->LockRect(0, &lockedRect, nullptr, 0);

    if (FAILED(hr))
        return false;

    uint8_t* destination = reinterpret_cast<uint8_t*>(lockedRect.pBits);

    for (uint32_t y = 0; y < height; ++y)
    {
        const uint8_t* srcRow = source + static_cast<size_t>(y) * pitch;
        uint8_t* dstRow = destination + static_cast<size_t>(y) * lockedRect.Pitch;
        std::memcpy(dstRow, srcRow, requiredPitch);
    }

    g_texture->UnlockRect(0);


    // Проверяем, не переключил ли Qt буфер, пока мы копировали.
    const LONG activeBufferAfter = InterlockedCompareExchange(&g_sharedImage->activeBuffer, 0, 0);

    if (activeBufferAfter != activeBuffer)
        return true;

    g_lastFrame = frame;

    return true;
}

// СТАРЫЙ КРАСНЫЙ КВАДРАТ
// ОСТАВЛЯЕМ ДЛЯ АВАРИЙНОЙ ПРОВЕРКИ
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

    if (SUCCEEDED(device->CreateStateBlock(D3DSBT_ALL, &stateBlock)))
        stateBlock->Capture();

    device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
    device->SetRenderState(D3DRS_LIGHTING, FALSE);
    device->SetRenderState(D3DRS_ZENABLE, FALSE);
    device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(Vertex));

    if (stateBlock)
    {
        stateBlock->Apply();
        stateBlock->Release();
    }
}

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

    Vertex vertices[] =
        {
            { -0.5f,     -0.5f,      0.0f, 1.0f, 0.0f, 0.0f },
            { width - 0.5f, -0.5f,   0.0f, 1.0f, 1.0f, 0.0f },
            { -0.5f, height - 0.5f,  0.0f, 1.0f, 0.0f, 1.0f },
            { width - 0.5f, height - 0.5f,
             0.0f, 1.0f, 1.0f, 1.0f }
        };

    IDirect3DStateBlock9* stateBlock = nullptr;

    if (SUCCEEDED(device->CreateStateBlock(D3DSBT_ALL, &stateBlock)))
        stateBlock->Capture();


    device->SetTexture(0, texture);
    device->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
    device->SetRenderState(D3DRS_LIGHTING, FALSE);
    device->SetRenderState( D3DRS_ZENABLE,FALSE);
    device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
    device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    device->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
    device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    device->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    device->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    device->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
    device->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
    device->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(Vertex) );

    device->SetTexture(0, nullptr);

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
    if (device)
    {
        static uint64_t lastFrame = 0;

        if (OpenSharedImage())
        {
            const uint32_t width = g_sharedImage->width;
            const uint32_t height = g_sharedImage->height;
            const uint64_t frame = g_sharedImage->frame;

            if (width != 0 && height != 0 && frame != 0)
            {
                if (!g_texture)
                    CreateImageTexture(device, width, height );

                if (g_texture)
                {
                    if (frame != lastFrame)
                    {
                        if (UpdateTextureFromSharedMemory(device))
                            lastFrame = frame;
                    }

                    DrawOverlayTexture(device, g_texture);
                }
            }
        }

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


IDirect3DDevice9* CreateDummyDevice(
    IDirect3D9** outD3D)
{
    if (!outD3D)
        return nullptr;

    *outD3D = nullptr;

    IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);

    if (!d3d)
        return nullptr;

    HWND hwnd = GetForegroundWindow();

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

    HRESULT hr = d3d->CreateDevice(
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

bool InstallHook()
{
    IDirect3D9* d3d = nullptr;

    IDirect3DDevice9* dummyDevice = CreateDummyDevice(&d3d);

    void** vtable =  *reinterpret_cast<void***>(dummyDevice);
    void* presentThunk = vtable[17];
    void* resolvedPresent = ResolveJump(presentThunk);

    PresentAddress = resolvedPresent;

    Trampoline = CreateTrampoline(resolvedPresent);

    if (!Trampoline)
    {
        dummyDevice->Release();
        d3d->Release();
        return false;
    }

    OriginalPresent = reinterpret_cast<PresentFn>(Trampoline);

    if (!WriteAbsoluteJump(resolvedPresent, reinterpret_cast<void*>(&HookedPresent)))
    {
        VirtualFree(Trampoline, 0, MEM_RELEASE);

        Trampoline = nullptr;
        OriginalPresent = nullptr;

        dummyDevice->Release();
        d3d->Release();

        return false;
    }

    dummyDevice->Release();
    d3d->Release();

    return true;
}

} // namespace

bool InstallDX9Hook()
{
    return InstallHook();
}

#pragma once

#include "../Core/Core.hpp"
#include "../Global.hpp"
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Xinput.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <emmintrin.h> // For CPU cache prefetching (_mm_prefetch)
#include "GUIBase.hpp"
#include "../Core/Core_ImGui.hpp"
#include "../CrimsonGUI.hpp"
#include "../Config.hpp"
#include "../DebugDrawDX11.hpp"
#include "../../ThirdParty/ImGui/imgui.h"
#include "../../ThirdParty/ImGui/Backend/imgui_impl_win32.h"
#include "../../ThirdParty/ImGui/Backend/imgui_impl_dx11.h"
#include "SwapChainWrapper.hpp"
#include "../StyleSwitchFX.hpp"
#include "../CrimsonSDL.hpp"
#include "../CrimsonEfk.hpp"

void FPSLimiter_Init(double fps);
void FPSLimiter_Apply();

void UpdateMousePositionMultiplier();

typedef void (*UpdateKeyboard_func_t)(DI8::DIKEYBOARDSTATE* stateAddr);

extern UpdateKeyboard_func_t UpdateKeyboard_func;

typedef void (*UpdateGamepad_func_t)(DIJOYSTATE* stateAddr);

extern UpdateGamepad_func_t UpdateGamepad_func;

namespace XI {
void UpdateGamepad();
}

// @Research: template- and reference-less solution. Use void **.
template <typename T, typename T2> void Install(void* dest, T& baseFuncAddr, T2& hookFuncAddr) {
    auto& funcAddr = *reinterpret_cast<T*>(dest);

    // @Research: Prefer dest and move up.
    protectionHelper.Push(&funcAddr, 8);

    baseFuncAddr = funcAddr;

    funcAddr = hookFuncAddr;

    protectionHelper.Pop();
}

void UpdateKeyboard();
void UpdateMouse();
void UpdateGamepad();


struct CrimsonWindow {
    HWND  hWnd   = nullptr;
    bool  active = true;

    // game = what the game thinks; actual = what we've really set.
    struct {
        RECT  window = {};
        RECT  client = {};
        DWORD style  = 0;
    } game, actual;

    // Borderless toggle state.
    struct {
        bool  active     = false;
        RECT  windowRect = {};
        DWORD style      = 0;
        DWORD exStyle    = 0;
        bool  altPressed = false;
    } borderless;

    // Stuck keys — snapshot on focus loss, release on regain.
    BYTE  lastKeyState[256] = {};

    bool  wantBackgroundRender() const { return !active; }
};

extern CrimsonWindow g_window;

// g_window.active is canonical; g_appInactive mirrors it for compat.
extern bool g_appInactive;

// Low-latency flip model optimization globals
extern HANDLE g_frameLatencyWaitableObject;
extern bool g_flipModelLatencyOptimized;

extern long g_flipSkip;

#pragma region Windows
void UpdateShow();

namespace Windows {
typedef WNDPROC WindowProc_t;
typedef decltype(RegisterClassExW)* RegisterClassExW_t;
typedef decltype(CreateWindowExW)* CreateWindowExW_t;
typedef decltype(CreateFileA)* CreateFileA_t;
typedef decltype(CreateFileW)* CreateFileW_t;
}; // namespace Windows

namespace Base::Windows {
extern ::Windows::WindowProc_t WindowProc;
extern ::Windows::RegisterClassExW_t RegisterClassExW;
extern ::Windows::CreateWindowExW_t CreateWindowExW;
extern ::Windows::CreateFileA_t CreateFileA;
extern ::Windows::CreateFileW_t CreateFileW;
}; // namespace Base::Windows

namespace Hook::Windows {
LRESULT WindowProc(HWND windowHandle, UINT message, WPARAM wParameter, LPARAM lParameter);

ATOM RegisterClassExW(const WNDCLASSEXW* windowClassAddr);
HWND CreateWindowExW(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);

typedef void (*CreateFileA_func_t)(LPCSTR lpFileName);

extern CreateFileA_func_t CreateFileA_func;

HANDLE CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);

HANDLE CreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
} // namespace Hook::Windows
#pragma endregion

inline void CreateRenderTarget() {
    ID3D11Texture2D* backBuffer = nullptr;
    ::DXGI::swapChain->GetBuffer(0, IID_ID3D11Texture2D, reinterpret_cast<void**>(&backBuffer));
    if (backBuffer) {
        ::D3D11::device->CreateRenderTargetView(backBuffer, nullptr, &::D3D11::renderTargetView);
        backBuffer->Release();
    }
}

inline void RemoveRenderTarget() {
    if (!::D3D11::renderTargetView)
        return;
    float clearColor[4] = {};
    ::D3D11::deviceContext->ClearRenderTargetView(::D3D11::renderTargetView, clearColor);
    ::D3D11::renderTargetView->Release();
    ::D3D11::renderTargetView = nullptr;
}

void Timestep();

#pragma region DXGI
// Old Present / ResizeBuffers hooks are gone — SwapChainWrapper handles
// all of that now. Kept the Present_func typedef for backwards compat.
typedef void (*Present_func_t)();
extern Present_func_t Present_func;
#pragma endregion

#pragma region D3D11

namespace D3D11 {
typedef decltype(D3D11CreateDeviceAndSwapChain)* D3D11CreateDeviceAndSwapChain_t;
};

namespace Base::D3D11 {
extern ::D3D11::D3D11CreateDeviceAndSwapChain_t D3D11CreateDeviceAndSwapChain;
};

namespace Hook::D3D11 {
typedef void (*D3D11CreateDeviceAndSwapChain_func_t)();

extern D3D11CreateDeviceAndSwapChain_func_t D3D11CreateDeviceAndSwapChain_func;

HRESULT D3D11CreateDeviceAndSwapChain(IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags,
    const D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
    IDXGISwapChain** ppSwapChain, ID3D11Device** ppDevice, D3D_FEATURE_LEVEL* pFeatureLevel, ID3D11DeviceContext** ppImmediateContext);
} // namespace Hook::D3D11
#pragma endregion

#pragma region DI8
namespace DI8 {
typedef HRESULT (*GetDeviceStateA_t)(IDirectInputDevice8A* pDevice, DWORD BufferSize, LPVOID Buffer);
};

namespace Base::DI8 {
extern ::DI8::GetDeviceStateA_t GetDeviceStateA;
};

namespace Hook::DI8 {
HRESULT GetDeviceStateA(IDirectInputDevice8A* pDevice, DWORD BufferSize, LPVOID Buffer);
}
#pragma endregion

#pragma region XI
namespace XI {
typedef decltype(XInputGetState)* XInputGetState_t;
};


namespace Base::XI {

extern ::XI::XInputGetState_t XInputGetState;

};

namespace Hook::XI {
DWORD XInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState);
};
#pragma endregion

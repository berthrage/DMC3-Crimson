#pragma once

#include "../Core/Core.hpp"
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Xinput.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <d3d10.h>
#include <emmintrin.h> // For CPU cache prefetching (_mm_prefetch)
#include "GUIBase.hpp"
#include "../Core/Core_ImGui.hpp"
#include "../CrimsonGUI.hpp"
#include "../Config.hpp"
#include "../DebugDrawDX11.hpp"
#include "../../ThirdParty/ImGui/imgui.h"
#include "../../ThirdParty/ImGui/Backend/imgui_impl_win32.h"
#include "../../ThirdParty/ImGui/Backend/imgui_impl_dx10.h"
#include "../../ThirdParty/ImGui/Backend/imgui_impl_dx11.h"
#include "../StyleSwitchFX.hpp"
#include "../CrimsonSDL.hpp"

namespace API {
enum {
    D3D10,
    D3D11,
};
};

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

// Low-latency flip model optimization globals
extern HANDLE g_frameLatencyWaitableObject;
extern bool g_flipModelLatencyOptimized;

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

template <new_size_t api> void CreateRenderTarget() {
    LogFunction();

    if constexpr (api == API::D3D10) {
        ID3D10Texture2D* backBuffer = 0;
        ::DXGI::swapChain->GetBuffer(0, IID_ID3D10Texture2D, reinterpret_cast<void**>(&backBuffer));
        ::D3D10::device->CreateRenderTargetView(backBuffer, 0, &::D3D10::renderTargetView);
        backBuffer->Release();

    } else if constexpr (api == API::D3D11) {
        ID3D11Texture2D* backBuffer = 0;
        ::DXGI::swapChain->GetBuffer(0, IID_ID3D11Texture2D, reinterpret_cast<void**>(&backBuffer));
        ::D3D11::device->CreateRenderTargetView(backBuffer, 0, &::D3D11::renderTargetView);
        backBuffer->Release();
    }
}

template <new_size_t api> void RemoveRenderTarget() {
    LogFunction();

    if constexpr (api == API::D3D10) {
        if (!::D3D10::renderTargetView) {
            return;
        }
        float clearColor[4] = {};
        ::D3D10::device->ClearRenderTargetView(::D3D10::renderTargetView, clearColor);
        ::D3D10::renderTargetView->Release();
        ::D3D10::renderTargetView = 0;

    } else if constexpr (api == API::D3D11) {
        if (!::D3D11::renderTargetView) {
            return;
        }
        float clearColor[4] = {};
        ::D3D11::deviceContext->ClearRenderTargetView(::D3D11::renderTargetView, clearColor);
        ::D3D11::renderTargetView->Release();
        ::D3D11::renderTargetView = 0;
    }
}

void Timestep();

#pragma region DXGI
namespace DXGI {

typedef HRESULT (*Present_t)(IDXGISwapChain* SwapChain, UINT SyncInterval, UINT Flags);
typedef HRESULT (*ResizeBuffers_t)(
    IDXGISwapChain* SwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
}; // namespace DXGI


namespace Base::DXGI {
extern ::DXGI::Present_t Present;
extern ::DXGI::ResizeBuffers_t ResizeBuffers;
}; // namespace Base::DXGI

namespace Hook::DXGI {
typedef void (*Present_func_t)();
extern Present_func_t Present_func;

template <new_size_t api> HRESULT Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    static bool run = false;
    if (!run) {
        run = true;

        Log(
#ifdef _WIN64
            "%s "
            "%llX "
            "%u "
            "%X",
#else
            "%s "
            "%X "
            "%u "
            "%X",
#endif
            FUNC_NAME, pSwapChain, SyncInterval, Flags);
            
        // Log the actual swap chain properties to verify flip model
        DXGI_SWAP_CHAIN_DESC actualDesc = {};
        if (SUCCEEDED(pSwapChain->GetDesc(&actualDesc))) {
            Log("Actual swap chain - BufferCount: %u, SwapEffect: %u, Windowed: %s", 
                actualDesc.BufferCount, 
                actualDesc.SwapEffect,
                actualDesc.Windowed ? "TRUE" : "FALSE");
        }
        
        // For flip model latency optimization, set device frame latency
        if constexpr (api == API::D3D11) {
            IDXGIDevice1* dxgiDevice = nullptr;
            if (SUCCEEDED(::D3D11::device->QueryInterface(IID_PPV_ARGS(&dxgiDevice)))) {
                dxgiDevice->SetMaximumFrameLatency(1);
                dxgiDevice->Release();
                Log("Set device frame latency to 1 for reduced input lag");
            }
        }
    }

    // Note: Advanced frame latency waitable object optimization is implemented
    // in the D3D11CreateDeviceAndSwapChain function for DXGI 1.2+ compatibility

    if (activeConfig.vSync != 0) {
        SyncInterval = (activeConfig.vSync - 1);
    }
	static uint32 prevWidth = 0;
	static uint32 prevHeight = 0;

    UpdateShow();


    UpdateKeyboard();
    UpdateMouse();
    UpdateGamepad();

    XI::UpdateGamepad();


    if constexpr (api == API::D3D10) {
        ImGui_ImplDX10_NewFrame();
        ImGui_ImplWin32_NewFrame();
    } else if constexpr (api == API::D3D11) {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
    }

    DXGI_SWAP_CHAIN_DESC swapDesc = {};
    pSwapChain->GetDesc(&swapDesc);

	uint32 width = swapDesc.BufferDesc.Width;
	uint32 height = swapDesc.BufferDesc.Height;

    auto& io = ImGui::GetIO();
	// Only update if the size has changed
	if (width != prevWidth || height != prevHeight) {
		prevWidth = width;
		prevHeight = height;

		UpdateGlobalRenderSize(width, height);
        UpdateGlobalClientSize();
        UpdateGlobalWindowSize();

	}
    io.DisplaySize = ImVec2((float)width, (float)height);

    ImGui_ImplWin32_GetDpiScaleForHwnd(swapDesc.OutputWindow);

    io.MousePos.x *= (swapDesc.BufferDesc.Width / g_clientSize.x);
    io.MousePos.y *= (swapDesc.BufferDesc.Height / g_clientSize.y);

    Timestep();

    ImGui::NewFrame();

    CrimsonSDL::InitSDL();
    GUI_Render(pSwapChain);

    DrawStyleSwitchFxTexture();

    ImGui::Render();

    if constexpr (api == API::D3D10) {
        ::D3D10::device->OMSetRenderTargets(1, &::D3D10::renderTargetView, 0);

        ImGui_ImplDX10_RenderDrawData(ImGui::GetDrawData());
    } else if constexpr (api == API::D3D11) {
        ::D3D11::deviceContext->OMSetRenderTargets(1, &::D3D11::renderTargetView, 0);

        // Low-latency optimization: Prefetch render target for better cache performance
        if (activeCrimsonConfig.System.flipModelPresentation) {
            _mm_prefetch((const char*)&::D3D11::renderTargetView, _MM_HINT_T0);
        }

        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        debug_draw_update(ImGui::GetIO().DeltaTime);
    }

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    [&]() {
        auto func = Present_func;
        if (!func) {
            return;
        }

        func();
    }();

    // Ultra-low latency optimization: Wait for GPU frame completion before presenting
    if (activeCrimsonConfig.System.flipModelPresentation && g_frameLatencyWaitableObject != nullptr) {
        // Boost thread priority for rendering thread during frame presentation
        HANDLE currentThread = GetCurrentThread();
        int originalPriority = GetThreadPriority(currentThread);
        SetThreadPriority(currentThread, THREAD_PRIORITY_TIME_CRITICAL);
        
        // Wait for the GPU to signal that it's ready for the next frame
        // This ensures minimal CPU-GPU pipeline stalls and reduces input lag by 1-2 frames
        DWORD waitResult = WaitForSingleObjectEx(g_frameLatencyWaitableObject, 16, TRUE); // 16ms timeout (1 frame at 60fps)
        if (waitResult == WAIT_OBJECT_0) {
            // GPU is ready - proceed with minimal latency
            // Force immediate command buffer flush for D3D11
            if constexpr (api == API::D3D11) {
                ::D3D11::deviceContext->Flush(); // Ensure all GPU commands are submitted immediately
            }
        } else if (waitResult == WAIT_TIMEOUT) {
            Log("Frame latency wait timeout - GPU may be overloaded");
        }
        
        // Restore original thread priority
        SetThreadPriority(currentThread, originalPriority);
    }

    // Use optimal present flags for flip model low latency
    UINT presentFlags = Flags;
    if (activeCrimsonConfig.System.flipModelPresentation) {
        if (SyncInterval == 0) {
            // For immediate presentation (VSync off), use tearing for lowest latency
            presentFlags = DXGI_PRESENT_ALLOW_TEARING;
        } else {
            // For VSync on, still optimize for flip model
            presentFlags = 0; 
        }
    }

    return ::Base::DXGI::Present(pSwapChain, SyncInterval, presentFlags);
}

template <new_size_t api>
HRESULT ResizeBuffers(IDXGISwapChain* SwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
    Log(
#ifdef _WIN64
        "%s "
        "%llX "
        "%u "
        "%u "
        "%u "
        "%d "
        "%X",
#else
        "%s "
        "%X "
        "%u "
        "%u "
        "%u "
        "%d "
        "%X",
#endif
        FUNC_NAME, SwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);

    RemoveRenderTarget<api>();


    auto result = ::Base::DXGI::ResizeBuffers(SwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);

    auto error = GetLastError();

    CreateRenderTarget<api>();


    UpdateGlobalWindowSize();
    UpdateGlobalClientSize();
    UpdateGlobalRenderSize(Width, Height);


    SetLastError(error);

    return result;
}
} // namespace Hook::DXGI
#pragma endregion

#pragma region D3D10

namespace D3D10 {
typedef decltype(D3D10CreateDeviceAndSwapChain)* D3D10CreateDeviceAndSwapChain_t;
};

namespace Base::D3D10 {
extern ::D3D10::D3D10CreateDeviceAndSwapChain_t D3D10CreateDeviceAndSwapChain;
};

namespace Hook::D3D10 {
typedef void (*D3D10CreateDeviceAndSwapChain_func_t)();

extern D3D10CreateDeviceAndSwapChain_func_t D3D10CreateDeviceAndSwapChain_func;

HRESULT D3D10CreateDeviceAndSwapChain(IDXGIAdapter* pAdapter, D3D10_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, UINT SDKVersion,
    DXGI_SWAP_CHAIN_DESC* pSwapChainDesc, IDXGISwapChain** ppSwapChain, ID3D10Device** ppDevice);


} // namespace Hook::D3D10
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
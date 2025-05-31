// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "HooksBase.hpp"
#include <Unknwn.h>
#include "../Core/Input.hpp"
#include "../Core/Core_ImGui.hpp"
#include "../DMC3Input.hpp"
#include "../Global.hpp"
#include "../Vars.hpp"

#include "../Core/Macros.h"

#include "../Core/DebugSwitch.hpp"
#include "../StyleSwitchFX.hpp"
#include "../CrimsonHUD.hpp"

void UpdateMousePositionMultiplier() {
    using namespace CoreImGui::DI8;

    if ((g_clientSize.x < 1) || (g_clientSize.y < 1) || (g_renderSize.x < 1) || (g_renderSize.y < 1)) {
        mousePositionMultiplier.x = 1;
        mousePositionMultiplier.y = 1;
    } else {
        mousePositionMultiplier.x = (g_renderSize.x / g_clientSize.x);
        mousePositionMultiplier.y = (g_renderSize.y / g_clientSize.y);
    }

    Log("%s %g %g", FUNC_NAME, mousePositionMultiplier.x, mousePositionMultiplier.y);
}

// namespace DI8 {

void CreateKeyboard() {
    LogFunction();

    if (!keyboard.Create()) {
        Log("keyboard.Create failed.");

        return;
    }
}

UpdateKeyboard_func_t UpdateKeyboard_func = 0;
UpdateGamepad_func_t UpdateGamepad_func   = 0;

#if debug

void WindowSize1() {
    LogFunction();

    SetWindowPos(appWindow, 0, 0, 0, 640, 360, 0);
}

void WindowSize2() {
    LogFunction();

    SetWindowPos(appWindow, 0, 0, 0, 1920, 1080, 0);
}

void WindowSize3() {
    LogFunction();

    SetWindowPos(appWindow, 0, 1920, 0, 1920, 1080, 0);
}

#endif

// @Update

void UpdateKeyboard() {
    keyboard.Update();

    CoreImGui::DI8::UpdateKeyboard(&keyboard.state);


    auto& state = keyboard.state;


    for_all(index, keyBindings.size()) {
        auto& keyBinding = keyBindings[index];

        keyBinding.UpdateKeyData(state.keys);
        keyBinding.Check(state.keys);
    }


    // @Move

    [&]() {
        auto func = UpdateKeyboard_func;
        if (!func) {
            return;
        }

        func(&keyboard.state);
    }();

#if debug

    static KeyData keyData[] = {
        {{
             ::DI8::KEY::LEFT_CONTROL,
             ::DI8::KEY::ONE,
         },
            2, false, false, WindowSize1},
        {{
             ::DI8::KEY::LEFT_CONTROL,
             ::DI8::KEY::TWO,
         },
            2, false, false, WindowSize2},
        {{
             ::DI8::KEY::LEFT_CONTROL,
             ::DI8::KEY::THREE,
         },
            2, false, false, WindowSize3},
    };

    for_all(index, countof(keyData)) {
        keyData[index].Check(keyboard.state.keys);
    }

#endif
}

void CreateMouse() {
    LogFunction();

    if (!mouse.Create()) {
        Log("mouse.Create failed.");

        return;
    }
}

void UpdateMouse() {
    mouse.Update();

    CoreImGui::DI8::UpdateMouse(appWindow, &mouse.state);
}

BOOL CreateGamepad_EnumFunction(LPCDIDEVICEINSTANCEA deviceInstanceAddr, LPVOID pvRef) {
    DebugLogFunction();

    if (!deviceInstanceAddr) {
        Log("!deviceInstanceAddr");

        return DIENUM_CONTINUE;
    }

    auto& deviceInstance = *deviceInstanceAddr;


    // Log    ("dwSize          %X", deviceInstance.dwSize                  );
    // LogGUID("guidInstance    "  , deviceInstance.guidInstance            );
    // LogGUID("guidProduct     "  , deviceInstance.guidProduct             );
    // Log    ("dwDevType       %X", deviceInstance.dwDevType               );
    // Log    ("dwDevType[0]    %X", (deviceInstance.dwDevType & 0xFF));
    // Log    ("dwDevType[1]    %X", ((deviceInstance.dwDevType >> 8) & 0xFF));


    // Log    ("tszInstanceName %s", deviceInstance.tszInstanceName         );
    // Log    ("tszProductName  %s", deviceInstance.tszProductName          );
    // LogGUID("guidFFDriver    "  , deviceInstance.guidFFDriver            );
    // Log    ("wUsagePage      %X", deviceInstance.wUsagePage              );
    // Log    ("wUsage          %X", deviceInstance.wUsage                  );

    // Log<false>("");

    if (strcmp(deviceInstance.tszInstanceName, activeConfig.gamepadName) != 0) {
        // Log("No Match $%s$ $%s$", deviceInstance.tszInstanceName, activeConfig.gamepadName);
        // Log<false>("");

        return DIENUM_CONTINUE;
    }

    CopyMemory(&gamepad.deviceInstance, &deviceInstance, sizeof(gamepad.deviceInstance));

    gamepad.match = true;

    return DIENUM_STOP;
}

void CreateGamepad() {
    LogFunction();

    gamepad.enumFunc = CreateGamepad_EnumFunction;

    if (!gamepad.Create()) {
        Log("gamepad.Create failed.");

        return;
    }
}

void UpdateGamepad() {
    if ((GetForegroundWindow() != appWindow) && !activeConfig.forceWindowFocus) {
        return;
    }

    gamepad.Update();

    auto& state = gamepad.state;

    auto button = activeConfig.gamepadButton;
    if (button > countof(state.rgbButtons)) {
        button = 0;
    }

    static bool execute = false;

    if (state.rgbButtons[button]) {
        if (execute) {
            execute = false;

            ToggleCrimsonGUI();
        }
    } else {
        execute = true;
    }

    [&]() {
        auto func = UpdateGamepad_func;
        if (!func) {
            return;
        }

        func(&gamepad.state);
    }();
}

//};

namespace XI {
void UpdateGamepad() {
    // @Merge
    if ((GetForegroundWindow() != appWindow) && !activeConfig.forceWindowFocus) {
        return;
    }

    if (!g_show) {
        return;
    }

    new_XInputGetState(0, &state);


    ::CoreImGui::XI::UpdateGamepad(&state);
}
}; // namespace XI

#pragma region Windows

namespace Base::Windows {
::Windows::WindowProc_t WindowProc             = 0;
::Windows::RegisterClassExW_t RegisterClassExW = 0;
::Windows::CreateWindowExW_t CreateWindowExW   = 0;
::Windows::CreateFileA_t CreateFileA           = 0;
::Windows::CreateFileW_t CreateFileW           = 0;
}; // namespace Base::Windows

namespace Hook::Windows {
LRESULT WindowProc(HWND windowHandle, UINT message, WPARAM wParameter, LPARAM lParameter) {
    auto result = ::Base::Windows::WindowProc(windowHandle, message, wParameter, lParameter);

    auto error = GetLastError();


    static bool run = false;
    if (!run) {
        run = true;

        Log(
#ifdef _WIN64
            "%s "
            "%llX "
            "%X "
            "%llX "
            "%llX",
#else
            "%s "
            "%X "
            "%X "
            "%X "
            "%X",
#endif
            FUNC_NAME, windowHandle, message, wParameter, lParameter);
    }


    switch (message) {
    case WM_SIZE: {
        if (!appWindow) {
            break;
        }

        auto width  = static_cast<uint16>(lParameter);
        auto height = static_cast<uint16>(lParameter >> 16);

        Log("WM_SIZE %u %u", width, height);

        UpdateGlobalWindowSize();
        UpdateGlobalClientSize();
        UpdateMousePositionMultiplier();

        break;
    }
    case WM_STYLECHANGED: {
        if (!appWindow) {
            break;
        }

        auto style = GetWindowLongA(appWindow, GWL_STYLE);

        Log("WM_STYLECHANGED %X", style);

        UpdateGlobalWindowSize();
        UpdateGlobalClientSize();
        UpdateMousePositionMultiplier();

        break;
    }
    case WM_SETCURSOR: {
        CoreImGui::UpdateMouseCursor(windowHandle);

        break;
    }
    case WM_CHAR: {
        auto character = static_cast<uint16>(wParameter);

        auto& io = ImGui::GetIO();

        io.AddInputCharacter(character);

        break;
    }
    }


    SetLastError(error);

    return result;
}

ATOM RegisterClassExW(const WNDCLASSEXW* windowClassAddr) {
    Log(
#ifdef _WIN64
        "%s "
        "%llX",
#else
        "%s "
        "%X",
#endif
        FUNC_NAME, windowClassAddr);


    if (!windowClassAddr) {
        goto Return;
    }


    {
        auto& windowClass = *const_cast<WNDCLASSEXW*>(windowClassAddr);

        static bool run = false;
        if (!run) {
            run = true;

            ImGui::CreateContext();

            auto& io = ImGui::GetIO();

            io.IniFilename = 0;


            io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

            io.BackendFlags |= ImGuiBackendFlags_HasGamepad;


            CoreImGui::DI8::Init();

            GUI_Init();


            ::Base::Windows::WindowProc = windowClass.lpfnWndProc;

            windowClass.lpfnWndProc = ::Hook::Windows::WindowProc;
        }
    }


Return:;
    { return ::Base::Windows::RegisterClassExW(windowClassAddr); }
}

HWND CreateWindowExW(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    X = activeConfig.windowPosX;
    Y = activeConfig.windowPosY;


    Log(
#ifdef _WIN64
        "%s "
        "%X "
        "%llX "
        "%llX "
        "%X "
        "%d "
        "%d "
        "%d "
        "%d "
        "%llX "
        "%llX "
        "%llX "
        "%llX",
#else
        "%s "
        "%X "
        "%X "
        "%X "
        "%X "
        "%d "
        "%d "
        "%d "
        "%d "
        "%X "
        "%X "
        "%X "
        "%X",
#endif
        FUNC_NAME, dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

    ToggleCursor(); // CreateWindowExW

    return ::Base::Windows::CreateWindowExW(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

CreateFileA_func_t CreateFileA_func = 0;

HANDLE CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
    [&]() {
        auto func = CreateFileA_func;
        if (!func) {
            return;
        }

        func(lpFileName);
    }();


    return ::Base::Windows::CreateFileA(
        lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

HANDLE CreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
    return ::Base::Windows::CreateFileW(
        lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

}; // namespace Hook::Windows

#pragma endregion

void UpdateShow() {
    g_show = (
        // activeConfig.welcome ||
        g_showMain || g_showShop);

    if (g_lastShow != g_show) {
        g_lastShow = g_show;

        ToggleCursor(); // g_show
    }
}

void Timestep() {
    static uint64 frequency   = 0;
    static uint64 mainCounter = 0;
    static bool run           = false;

    uint64 currentCounter = 0;

    if (!run) {
        run = true;

        QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&frequency));
        QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&mainCounter));
    }

    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCounter));

    ImGui::GetIO().DeltaTime = (static_cast<float>(currentCounter - mainCounter) / static_cast<float>(frequency));

    mainCounter = currentCounter;
}

#pragma region DXGI
namespace Base::DXGI {
::DXGI::Present_t Present             = 0;
::DXGI::ResizeBuffers_t ResizeBuffers = 0;
}; // namespace Base::DXGI

namespace Hook::DXGI {

typedef void (*Present_func_t)();

Present_func_t Present_func = 0;

}; // namespace Hook::DXGI

#pragma endregion


#pragma region D3D10
namespace Base::D3D10 {

::D3D10::D3D10CreateDeviceAndSwapChain_t D3D10CreateDeviceAndSwapChain = 0;

};

namespace Hook::D3D10 {
D3D10CreateDeviceAndSwapChain_func_t D3D10CreateDeviceAndSwapChain_func = 0;

HRESULT D3D10CreateDeviceAndSwapChain(IDXGIAdapter* pAdapter, D3D10_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, UINT SDKVersion,
    DXGI_SWAP_CHAIN_DESC* pSwapChainDesc, IDXGISwapChain** ppSwapChain, ID3D10Device** ppDevice) {
    Log(
#ifdef _WIN64
        "%s "
        "%llX "
        "%X "
        "%llX "
        "%X "
        "%X "
        "%llX "
        "%llX "
        "%llX",
#else
        "%s "
        "%X "
        "%X "
        "%X "
        "%X "
        "%X "
        "%X "
        "%X "
        "%X",
#endif
        FUNC_NAME, pAdapter, DriverType, Software, Flags, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice);

    auto result = ::Base::D3D10::D3D10CreateDeviceAndSwapChain(
        pAdapter, DriverType, Software, Flags, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice);

    auto error = GetLastError();

    ::D3D10::device   = *ppDevice;
    ::DXGI::swapChain = *ppSwapChain;

    appWindow = pSwapChainDesc->OutputWindow;

    UpdateGlobalWindowSize();
    UpdateGlobalClientSize();
    UpdateGlobalRenderSize(pSwapChainDesc->BufferDesc.Width, pSwapChainDesc->BufferDesc.Height);

    CoreImGui::UpdateDisplaySize(pSwapChainDesc->BufferDesc.Width, pSwapChainDesc->BufferDesc.Height);

    UpdateMousePositionMultiplier();

    DXGI_SWAP_CHAIN_DESC swapDesc{};
    ::DXGI::swapChain->GetDesc(&swapDesc);

    if (!ImGui_ImplWin32_Init(swapDesc.OutputWindow)) {
        Log("%s Failed to initialize ImGui on D3D10 -> ImGui_ImplWin32_Init.", FUNC_NAME);
        return result;
    }

    if (!ImGui_ImplDX10_Init(::D3D10::device)) {
        Log("%s Failed to initialize ImGui on D3D10 -> ImGui_ImplDX10_Init.", FUNC_NAME);
        return result;
    }

    CreateRenderTarget<API::D3D10>();


    [&]() {
        auto func = D3D10CreateDeviceAndSwapChain_func;
        if (!func) {
            return;
        }

        func();
    }();


    [&]() {
        if ((result != 0) || !ppSwapChain || !*ppSwapChain) {
            return;
        }

        auto funcAddrs = *reinterpret_cast<byte8***>(*ppSwapChain);
        if (!funcAddrs) {
            return;
        }

        //Install(&funcAddrs[8], ::Base::DXGI::Present, ::Hook::DXGI::Present<API::D3D10>);

        Install(&funcAddrs[13], ::Base::DXGI::ResizeBuffers, ::Hook::DXGI::ResizeBuffers<API::D3D10>);
    }();


    CreateKeyboard();
    CreateMouse();
    CreateGamepad();


    SetLastError(error);

    return result;
}

}; // namespace Hook::D3D10

#pragma endregion


#pragma region D3D11
namespace Base::D3D11 {

::D3D11::D3D11CreateDeviceAndSwapChain_t D3D11CreateDeviceAndSwapChain = 0;

};


namespace Hook::D3D11 {

D3D11CreateDeviceAndSwapChain_func_t D3D11CreateDeviceAndSwapChain_func = 0;

HRESULT D3D11CreateDeviceAndSwapChain(IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags,
    const D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
    IDXGISwapChain** ppSwapChain, ID3D11Device** ppDevice, D3D_FEATURE_LEVEL* pFeatureLevel, ID3D11DeviceContext** ppImmediateContext) {
    Log(
#ifdef _WIN64
        "%s "
        "%llX "
        "%X "
        "%llX "
        "%X "
        "%llX "
        "%X "
        "%X "
        "%llX "
        "%llX "
        "%llX "
        "%llX "
        "%llX",
#else
        "%s "
        "%X "
        "%X "
        "%X "
        "%X "
        "%X "
        "%X "
        "%X "
        "%X "
        "%X "
        "%X "
        "%X "
        "%X",
#endif
        FUNC_NAME, pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice,
        pFeatureLevel, ppImmediateContext);
    DXGI_SWAP_CHAIN_DESC swapChainDesc = *pSwapChainDesc;
    D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_1 };
    UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
    deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif             
    IDXGIFactory1* pFactory;
    CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&pFactory));    
    for (int i = 0; pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; i++)
    {
        DXGI_ADAPTER_DESC adapterDesc{};
        pAdapter->GetDesc(&adapterDesc);
        Log("Found device %d: %S", i, (wchar_t*)adapterDesc.Description);
        break;
    }
    HRESULT hr = D3D11CreateDevice(
        pAdapter,
        D3D_DRIVER_TYPE_UNKNOWN,
        0,                
        deviceFlags,      
        levels,           
        ARRAYSIZE(levels),
        D3D11_SDK_VERSION,
        ppDevice,         
        NULL,             
        ppImmediateContext
    );
    assert(SUCCEEDED(hr) && "D3D11 device creation failure.");
    IDXGIOutput* pOutput = nullptr;
    hr = pAdapter->EnumOutputs(0, &pOutput);
    assert(SUCCEEDED(hr) && "Failed to enumerate DXGI Output.");
    DXGI_MODE_DESC desiredMode{}, matchingMode{};
    desiredMode.Width = swapChainDesc.BufferDesc.Width;
    desiredMode.Height = swapChainDesc.BufferDesc.Height;
    desiredMode.Format = swapChainDesc.BufferDesc.Format;
    // Match highest available refresh rates
    desiredMode.RefreshRate.Numerator = 1000;
    desiredMode.RefreshRate.Denominator = 1;
    hr = pOutput->FindClosestMatchingMode(&desiredMode, &matchingMode, *ppDevice);
    assert(SUCCEEDED(hr) && "Failed to match display mode!");
	Log("Display: %dx%d@%fHz", matchingMode.Width, matchingMode.Height,
		static_cast<float>(matchingMode.RefreshRate.Numerator) / static_cast<float>(matchingMode.RefreshRate.Denominator));    
    swapChainDesc.BufferDesc = matchingMode;    
    // Create the Swapchain
    auto result = pFactory->CreateSwapChain(
        *ppDevice,
        &swapChainDesc,
        ppSwapChain
    );
    assert(SUCCEEDED(result) && "Failed to create the Swapchain!");
    auto error = GetLastError();       
    pFactory->Release();
    pOutput->Release();
    
    ::D3D11::device        = *ppDevice;
    ::D3D11::deviceContext = *ppImmediateContext;
    ::DXGI::swapChain      = *ppSwapChain;

    appWindow = pSwapChainDesc->OutputWindow;

    UpdateGlobalWindowSize();
    UpdateGlobalClientSize();
    UpdateGlobalRenderSize(pSwapChainDesc->BufferDesc.Width, pSwapChainDesc->BufferDesc.Height);

    CoreImGui::UpdateDisplaySize(pSwapChainDesc->BufferDesc.Width, pSwapChainDesc->BufferDesc.Height);

    UpdateMousePositionMultiplier();

    DXGI_SWAP_CHAIN_DESC swapDesc{};
    ::DXGI::swapChain->GetDesc(&swapDesc);

    if (!ImGui_ImplWin32_Init(swapDesc.OutputWindow)) {
        Log("%s Failed to initialize ImGui on D3D11 -> ImGui_ImplWin32_Init.", FUNC_NAME);
        return result;
    }

    if (!ImGui_ImplDX11_Init(::D3D11::device, ::D3D11::deviceContext)) {
        Log("%s Failed to initialize ImGui on D3D11 -> ImGui_ImplDX11_Init.", FUNC_NAME);
        return result;
    }

    CreateRenderTarget<API::D3D11>();

    CrimsonHUD::InitTextures(::D3D11::device);
    InitStyleSwitchFxTexture(::D3D11::device);
    debug_draw_init(
        (void*)::D3D11::device, (void*)::D3D11::deviceContext, pSwapChainDesc->BufferDesc.Width, pSwapChainDesc->BufferDesc.Height);

    [&]() {
        auto func = D3D11CreateDeviceAndSwapChain_func;
        if (!func) {
            return;
        }

        func();
    }();


    [&]() {
        if ((result != 0) || !ppSwapChain || !*ppSwapChain) {
            return;
        }

        auto funcAddrs = *reinterpret_cast<byte8***>(*ppSwapChain);
        if (!funcAddrs) {
            return;
        }

        Install(&funcAddrs[8], ::Base::DXGI::Present, ::Hook::DXGI::Present<API::D3D11>);

        //Install(&funcAddrs[13], ::Base::DXGI::ResizeBuffers, ::Hook::DXGI::ResizeBuffers<API::D3D11>);
    }();


    CreateKeyboard();
    CreateMouse();
    CreateGamepad();


    SetLastError(error);

    return result;
}

}; // namespace Hook::D3D11

#pragma endregion


#pragma region DI8
namespace Base::DI8 {

::DI8::GetDeviceStateA_t GetDeviceStateA = 0;

};

namespace Hook::DI8 {

HRESULT GetDeviceStateA(IDirectInputDevice8A* pDevice, DWORD BufferSize, LPVOID Buffer) {
    // Blocks DI8 Keyboard Input while GUI is Open
    if (g_show || GetForegroundWindow() != appWindow) {

        SetMemory(Buffer, 0, BufferSize);
    }

    return 0;
}

}; // namespace Hook::DI8
#pragma endregion


#pragma region XI
namespace Base::XI {

::XI::XInputGetState_t XInputGetState = 0;

};


namespace Hook::XI {

DWORD XInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState) {
	// Blocks XInput Gamepad Input while GUI is Open
    if (g_show) {
        SetMemory(pState, 0, sizeof(XINPUT_STATE));
    }

    SwapXInputButtonsCoop(dwUserIndex, pState);

    return 0;
}

}; // namespace Hook::XI

#pragma endregion

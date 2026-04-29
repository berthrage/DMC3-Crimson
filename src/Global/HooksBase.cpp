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
#include "../CrimsonEfk.hpp"
#include "../CrimsonHUD.hpp"
#include <dxgi1_3.h>

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

    // Only update ImGui keyboard state from DirectInput if ImGui doesn't want keyboard capture
    // When WantCaptureKeyboard is true, ImGui_ImplWin32_WndProcHandler handles input via Windows messages
    if (!ImGui::GetIO().WantCaptureKeyboard) {
        CoreImGui::DI8::UpdateKeyboard(&keyboard.state);
    }

    auto& state = keyboard.state;

    UpdateKeyboardConfigCapture(state.keys);

    // Block game input when ImGui wants keyboard input
    if (ImGui::GetIO().WantCaptureKeyboard) {
        return;
    }


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

static double g_targetFrameTime = 0.0; // seconds (0 = uncapped)
static LARGE_INTEGER g_qpcFreq = {};
static LARGE_INTEGER g_lastPresentTime = {};
static bool g_fpsLimiterInitialized = false;

void FPSLimiter_Init(double fps) {
	if (fps <= 0.0) {
		g_targetFrameTime = 0.0;
		g_fpsLimiterInitialized = false;
		return;
	}

	QueryPerformanceFrequency(&g_qpcFreq);
	QueryPerformanceCounter(&g_lastPresentTime);

	g_targetFrameTime = 1.0 / fps;
	g_fpsLimiterInitialized = true;
}


void FPSLimiter_Apply() {
	if (!g_fpsLimiterInitialized || g_targetFrameTime <= 0.0)
		return;

	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);

	double elapsed = double(now.QuadPart - g_lastPresentTime.QuadPart) / double(g_qpcFreq.QuadPart);
	double remaining = g_targetFrameTime - elapsed;

	if (remaining > 0.0) {
		// Coarse sleep (RTSS-like)
		if (remaining > 0.002) { // > 2ms
			DWORD sleepMs = (DWORD)((remaining - 0.001) * 1000.0);
			if (sleepMs > 0)
				Sleep(sleepMs);
		}

		// Fine spin wait
		do {
			QueryPerformanceCounter(&now);
			elapsed = double(now.QuadPart - g_lastPresentTime.QuadPart) / double(g_qpcFreq.QuadPart);
		} while (elapsed < g_targetFrameTime);
	}

	g_lastPresentTime = now;
}



namespace Base::Windows {
::Windows::WindowProc_t WindowProc             = 0;
::Windows::RegisterClassExW_t RegisterClassExW = 0;
::Windows::CreateWindowExW_t CreateWindowExW   = 0;
::Windows::CreateFileA_t CreateFileA           = 0;
::Windows::CreateFileW_t CreateFileW           = 0;
}; // namespace Base::Windows

static bool g_isBorderlessFullscreen = false;
static RECT g_windowedRect = {};
static DWORD g_windowedStyle = 0;
static DWORD g_windowedExStyle = 0;

// Key state tracking for Alt+Enter (proper keydown detection)
static bool g_altEnterPressed = false;

// Low-latency flip model optimization globals
static HANDLE g_frameLatencyWaitableObject = nullptr;
static bool g_flipModelLatencyOptimized = false;

void ToggleBorderlessFullscreen() {
    // Only allow borderless fullscreen if flip model presentation is enabled
    if (!activeCrimsonConfig.System.flipModelPresentation) {
        Log("ToggleBorderlessFullscreen: Disabled - flip model presentation is not enabled in configuration");
        return;
    }

    if (!appWindow) {
        Log("ToggleBorderlessFullscreen: appWindow is null");
        return;
    }

    // Detect initial state on first call
    static bool firstCall = true;
    if (firstCall) {
        firstCall = false;
        
        DWORD currentStyle = GetWindowLongA(appWindow, GWL_STYLE);
        
        // Check if we're already in borderless fullscreen mode
        // Borderless fullscreen typically has WS_POPUP style and covers the monitor
        bool isBorderless = (currentStyle & WS_POPUP) && !(currentStyle & WS_CAPTION);
        
        if (isBorderless) {
            RECT windowRect;
            GetWindowRect(appWindow, &windowRect);
            
            HMONITOR hMonitor = MonitorFromWindow(appWindow, MONITOR_DEFAULTTOPRIMARY);
            MONITORINFO monitorInfo = {};
            monitorInfo.cbSize = sizeof(MONITORINFO);
            GetMonitorInfo(hMonitor, &monitorInfo);
            
            // Check if window covers the entire monitor
            bool coversMonitor = (windowRect.left <= monitorInfo.rcMonitor.left) &&
                               (windowRect.top <= monitorInfo.rcMonitor.top) &&
                               (windowRect.right >= monitorInfo.rcMonitor.right) &&
                               (windowRect.bottom >= monitorInfo.rcMonitor.bottom);
            
            if (coversMonitor) {
                g_isBorderlessFullscreen = true;
                // Set default windowed state (will be overridden when user switches to windowed)
                g_windowedStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
                g_windowedExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
                
                // Use a reasonable default windowed size (92% of monitor)
                int defaultWidth = (monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left) * 0.92f;
                int defaultHeight = (monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top) * 0.92f;
                int centerX = monitorInfo.rcMonitor.left + (monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left - defaultWidth) / 2;
                int centerY = monitorInfo.rcMonitor.top + (monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top - defaultHeight) / 2;
                
                g_windowedRect.left = centerX;
                g_windowedRect.top = centerY;
                g_windowedRect.right = centerX + defaultWidth;
                g_windowedRect.bottom = centerY + defaultHeight;
                
                Log("Detected initial borderless fullscreen state - set default windowed size: %dx%d", defaultWidth, defaultHeight);
            } else {
                g_isBorderlessFullscreen = false;
                Log("Detected initial windowed state");
            }
        } else {
            g_isBorderlessFullscreen = false;
            Log("Detected initial windowed state");
        }
    }

    Log("Starting window transition - current state: %s", g_isBorderlessFullscreen ? "borderless" : "windowed");

    if (!g_isBorderlessFullscreen) {
        // Save current windowed state
        g_windowedStyle = GetWindowLongA(appWindow, GWL_STYLE);
        g_windowedExStyle = GetWindowLongA(appWindow, GWL_EXSTYLE);
        GetWindowRect(appWindow, &g_windowedRect);

        // Get primary monitor info
        HMONITOR hMonitor = MonitorFromWindow(appWindow, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO monitorInfo = {};
        monitorInfo.cbSize = sizeof(MONITORINFO);
        GetMonitorInfo(hMonitor, &monitorInfo);

        Log("Transitioning to borderless fullscreen...");

        // Set borderless fullscreen style
        SetWindowLongA(appWindow, GWL_STYLE, WS_POPUP | WS_VISIBLE);
        SetWindowLongA(appWindow, GWL_EXSTYLE, WS_EX_APPWINDOW);

        // Move window to cover entire monitor
        SetWindowPos(appWindow, HWND_TOP, 
            monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top,
            monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
            monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
            SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

        g_isBorderlessFullscreen = true;
        
        Log("Switched to borderless fullscreen: %dx%d", 
            monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
            monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top);
    } else {
        Log("Transitioning to windowed mode...");

        // Validate saved windowed state
        if (g_windowedStyle == 0) {
            g_windowedStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
            g_windowedExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
            Log("Using default windowed style (saved state was invalid)");
        }
        
        if (g_windowedRect.right <= g_windowedRect.left || g_windowedRect.bottom <= g_windowedRect.top) {
            // Invalid saved rect, create a reasonable default
            HMONITOR hMonitor = MonitorFromWindow(appWindow, MONITOR_DEFAULTTOPRIMARY);
            MONITORINFO monitorInfo = {};
            monitorInfo.cbSize = sizeof(MONITORINFO);
            GetMonitorInfo(hMonitor, &monitorInfo);
            
            int defaultWidth = (monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left) * 0.92f;
            int defaultHeight = (monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top) * 0.92f;
            int centerX = monitorInfo.rcMonitor.left + (monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left - defaultWidth) / 2;
            int centerY = monitorInfo.rcMonitor.top + (monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top - defaultHeight) / 2;
            
            g_windowedRect.left = centerX;
            g_windowedRect.top = centerY;
            g_windowedRect.right = centerX + defaultWidth;
            g_windowedRect.bottom = centerY + defaultHeight;
            
            Log("Using default windowed rect (saved rect was invalid): %dx%d", defaultWidth, defaultHeight);
        }

        // Restore windowed mode
        SetWindowLongA(appWindow, GWL_STYLE, g_windowedStyle);
        SetWindowLongA(appWindow, GWL_EXSTYLE, g_windowedExStyle);

        SetWindowPos(appWindow, HWND_NOTOPMOST,
            g_windowedRect.left, g_windowedRect.top,
            g_windowedRect.right - g_windowedRect.left,
            g_windowedRect.bottom - g_windowedRect.top,
            SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

        g_isBorderlessFullscreen = false;
        
        Log("Switched to windowed mode: %dx%d", 
            g_windowedRect.right - g_windowedRect.left,
            g_windowedRect.bottom - g_windowedRect.top);
    }
}

// Forward declaration for ImGui Win32 backend handler
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Hook::Windows {
LRESULT WindowProc(HWND windowHandle, UINT message, WPARAM wParameter, LPARAM lParameter) {
    // Let ImGui's Win32 backend handle the message first - this is CRITICAL for proper text input
    // This handles WM_CHAR, WM_KEYDOWN, WM_KEYUP, WM_MOUSEMOVE, etc. internally
    if (ImGui_ImplWin32_WndProcHandler(windowHandle, message, wParameter, lParameter)) {
        // ImGui handled the message and consumed it - don't pass it to the game
        return true;
    }

    // Only handle Alt+Enter if flip model presentation is enabled
    if (activeCrimsonConfig.System.flipModelPresentation) {
        // Handle Alt+Enter 
        if (message == WM_SYSKEYDOWN && wParameter == VK_RETURN && (lParameter & (1 << 29))) {
            if (GetForegroundWindow() == windowHandle && !g_altEnterPressed) {
                // Key pressed down for the first time
                g_altEnterPressed = true;
                Log("Alt+Enter pressed via WindowProc - triggering fullscreen toggle");
                ToggleBorderlessFullscreen();
                return 0; // Consume the keydown message completely
            }
        } else if (message == WM_SYSKEYUP && wParameter == VK_RETURN && (lParameter & (1 << 29))) {
            if (g_altEnterPressed) {
                g_altEnterPressed = false;

            }
            // Don't return 0 here - let the keyup message pass through to the base handler
        }
    }

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

    // Disable DXGI's default Alt+Enter handling since we handle it ourselves
    if (::DXGI::swapChain) {
        IDXGIFactory* factory = nullptr;
        if (SUCCEEDED(::DXGI::swapChain->GetParent(IID_PPV_ARGS(&factory)))) {
            factory->MakeWindowAssociation(appWindow, DXGI_MWA_NO_ALT_ENTER);
            factory->Release();
        }
    }


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

	// Create a modifiable copy of the swap chain description
	DXGI_SWAP_CHAIN_DESC modifiedSwapChainDesc = *pSwapChainDesc;
	bool flipModelEnabled = false;

	if (activeCrimsonConfig.System.flipModelPresentation && 
		modifiedSwapChainDesc.SampleDesc.Count == 1 && modifiedSwapChainDesc.SampleDesc.Quality == 0) {
		flipModelEnabled = true;
		SetProcessDPIAware();
		Log("SetProcessDPIAware() called for consistent DPI scaling");
	} else if (!activeCrimsonConfig.System.flipModelPresentation) {
		Log("Flip model presentation disabled by configuration - using original swap chain settings");
	} else {
		Log("Cannot enable flip model - MSAA detected (Count: %u, Quality: %u)", 
			modifiedSwapChainDesc.SampleDesc.Count, modifiedSwapChainDesc.SampleDesc.Quality);
	}

	HRESULT result = S_OK;

	// Use DXGI 1.2 SwapChain upgrade strictly for Flip Model scaling support.
	if (flipModelEnabled && ppSwapChain) {
		Log("Flip Model requested: Executing DXGI 1.2 CreateSwapChainForHwnd upgrade...");

		ID3D11Device* localDevice = nullptr;
		ID3D11DeviceContext* localContext = nullptr;

		// 1. Create Device ONLY (pass nullptr for SwapChain details)
		result = ::Base::D3D11::D3D11CreateDeviceAndSwapChain(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels,
			SDKVersion, nullptr, nullptr, &localDevice, pFeatureLevel, &localContext);

		if (SUCCEEDED(result)) {
			// 2. Extract factories and build DXGI 1.2 chain
			IDXGIDevice* dxgiDevice = nullptr;
			localDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
			if (dxgiDevice) {
				IDXGIAdapter* dxgiAdapter = nullptr;
				dxgiDevice->GetAdapter(&dxgiAdapter);
				if (dxgiAdapter) {
					IDXGIFactory2* factory2 = nullptr;
					dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)&factory2);
					if (factory2) {
						DXGI_SWAP_CHAIN_DESC1 desc1 = {};
						desc1.Width = modifiedSwapChainDesc.BufferDesc.Width;
						desc1.Height = modifiedSwapChainDesc.BufferDesc.Height;
						desc1.Format = modifiedSwapChainDesc.BufferDesc.Format;
						desc1.SampleDesc = modifiedSwapChainDesc.SampleDesc;
						desc1.BufferUsage = modifiedSwapChainDesc.BufferUsage;
						desc1.BufferCount = 2; // Flip model needs 2+ buffers
						desc1.Scaling = DXGI_SCALING_STRETCH; // Enables decoupled window/render resolution
						desc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
						desc1.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED; 
						desc1.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

						DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc = {};
						fsDesc.Windowed = TRUE;

						IDXGISwapChain1* swapChain1 = nullptr;
						HRESULT hr = factory2->CreateSwapChainForHwnd(
							localDevice, pSwapChainDesc->OutputWindow, 
							&desc1, &fsDesc, nullptr, &swapChain1);

						if (SUCCEEDED(hr)) {
							*ppSwapChain = swapChain1;
							Log("Successfully created DXGI 1.2 SwapChain with STRETCH scaling.");
						} else {
							Log("Failed to create DXGI 1.2 SwapChain: %X", hr);
						}
						factory2->Release();
					}
					dxgiAdapter->Release();
				}
				dxgiDevice->Release();
			}
		}

// 		IDXGIFactory* factory = nullptr;
// 		if (SUCCEEDED(::DXGI::swapChain->GetParent(IID_PPV_ARGS(&factory)))) {
// 			// Add DXGI_MWA_NO_WINDOW_CHANGES to prevent DXGI from messing with Alt-Tab focus
// 			factory->MakeWindowAssociation(appWindow, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);
// 			factory->Release();
// 			Log("Disabled DXGI Alt+Enter & Window Change handling - using custom borderless fullscreen");
// 		}

		if (ppDevice) *ppDevice = localDevice;
		if (ppImmediateContext) *ppImmediateContext = localContext;

	} else {
		Log("Enabling Standard D3D11 SwapChain presentation");
		result = ::Base::D3D11::D3D11CreateDeviceAndSwapChain(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels,
			SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice, pFeatureLevel, ppImmediateContext);
	}

	auto error = GetLastError();

	::D3D11::device        = *ppDevice;
	::D3D11::deviceContext = *ppImmediateContext;
	::DXGI::swapChain      = *ppSwapChain;

	appWindow = pSwapChainDesc->OutputWindow;

	UpdateGlobalWindowSize();
	UpdateGlobalClientSize();
	// Make sure we inform rendering logic of our potentially higher/mutated resolution so drawing math adapts:
	UpdateGlobalRenderSize(modifiedSwapChainDesc.BufferDesc.Width, modifiedSwapChainDesc.BufferDesc.Height);

	CoreImGui::UpdateDisplaySize(modifiedSwapChainDesc.BufferDesc.Width, modifiedSwapChainDesc.BufferDesc.Height);

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
		(void*)::D3D11::device, (void*)::D3D11::deviceContext, modifiedSwapChainDesc.BufferDesc.Width, modifiedSwapChainDesc.BufferDesc.Height);

	bool efk = CrimsonEfk::EffekInit(::D3D11::device, ::D3D11::deviceContext, modifiedSwapChainDesc.BufferDesc.Width, modifiedSwapChainDesc.BufferDesc.Height);
    assert(efk);

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

    // Only disable DXGI's default Alt+Enter handling if flip model is enabled
    // When flip model is disabled, let DXGI handle Alt+Enter normally
    if (flipModelEnabled && ::DXGI::swapChain) {
        IDXGIFactory* factory = nullptr;
        if (SUCCEEDED(::DXGI::swapChain->GetParent(IID_PPV_ARGS(&factory)))) {
            factory->MakeWindowAssociation(appWindow, DXGI_MWA_NO_ALT_ENTER);
            factory->Release();
            Log("Disabled DXGI Alt+Enter handling - using custom borderless fullscreen");
        }
        
        // Apply low-latency optimizations for flip model
        // Try to use DXGI 1.2+ features if available, fallback to basic optimization
        IDXGISwapChain2* swapChain2 = nullptr;
        HRESULT hr = ::DXGI::swapChain->QueryInterface(__uuidof(IDXGISwapChain2), (void**)&swapChain2);
        if (SUCCEEDED(hr) && swapChain2) {
            // Set maximum frame latency to 1 for lowest possible latency
            hr = swapChain2->SetMaximumFrameLatency(1);
            if (SUCCEEDED(hr)) {
                Log("Set swap chain maximum frame latency to 1 for reduced input lag");
                
                // Get the waitable object for precise frame timing
                HANDLE waitableObject = swapChain2->GetFrameLatencyWaitableObject();
                if (waitableObject != nullptr) {
                    g_frameLatencyWaitableObject = waitableObject;
                    g_flipModelLatencyOptimized = true;
                    Log("Frame latency waitable object obtained and stored for ultra-low latency rendering");
                    Log("Latency optimization: CPU-GPU synchronization enabled to minimize input lag");
                } else {
                    Log("Warning: Failed to obtain frame latency waitable object");
                }
            } else {
                Log("Warning: Failed to set swap chain maximum frame latency (HRESULT: 0x%X)", hr);
            }
            swapChain2->Release();
        } else {
            Log("DXGI 1.2+ not available - using basic latency optimization");
        }
        
        // Set device-level maximum frame latency for additional latency reduction
        IDXGIDevice1* dxgiDevice = nullptr;
        if (SUCCEEDED((*ppDevice)->QueryInterface(IID_PPV_ARGS(&dxgiDevice)))) {
            hr = dxgiDevice->SetMaximumFrameLatency(1);
            if (SUCCEEDED(hr)) {
                Log("Set device maximum frame latency to 1");
            } else {
                Log("Warning: Failed to set device maximum frame latency (HRESULT: 0x%X)", hr);
            }
            dxgiDevice->Release();
        }
    } else if (!flipModelEnabled) {
        Log("Flip model disabled - DXGI will handle Alt+Enter normally");
    }


    CreateKeyboard();
    CreateMouse();
    CreateGamepad();

    FPSLimiter_Init(activeCrimsonConfig.System.fpsCap);


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
    HRESULT result = ::Base::DI8::GetDeviceStateA ? ::Base::DI8::GetDeviceStateA(pDevice, BufferSize, Buffer) : S_OK;
    
    // Only intercept Alt+Enter if flip model presentation is enabled
    if (BufferSize == 256 && Buffer && result == S_OK && activeCrimsonConfig.System.flipModelPresentation) {
        BYTE* keys = static_cast<BYTE*>(Buffer);
        
        // DirectInput scan codes 
        #define DIK_LMENU 0x38   // Left Alt
        #define DIK_RMENU 0xB8   // Right Alt 
        #define DIK_RETURN 0x1C  // Enter
        
        // Check for Alt+Enter combination and clear the keys to prevent game from seeing them
        // Note: We don't trigger the toggle here anymore - Windows message handler does that
        bool leftAltPressed = (keys[DIK_LMENU] & 0x80) != 0;
        bool rightAltPressed = (keys[DIK_RMENU] & 0x80) != 0;
        bool enterPressed = (keys[DIK_RETURN] & 0x80) != 0;
        bool altEnterCurrentlyPressed = (leftAltPressed || rightAltPressed) && enterPressed;
        
        // Clear the Alt and Enter keys to prevent the game from seeing them if they were pressed
        if (altEnterCurrentlyPressed) {
            keys[DIK_LMENU] = 0;
            keys[DIK_RMENU] = 0; 
            keys[DIK_RETURN] = 0;
        }
    }

    // Blocks DI8 Keyboard Input while GUI is Open or ImGui wants keyboard input
    if (g_show || ImGui::GetIO().WantCaptureKeyboard || GetForegroundWindow() != appWindow) {
        SetMemory(Buffer, 0, BufferSize);
    }

    return result;
}

}; // namespace Hook::DI8
#pragma endregion


#pragma region XI
namespace Base::XI {

::XI::XInputGetState_t XInputGetState = 0;

};


namespace Hook::XI {

DWORD XInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState) {
	SwapXInputButtonsCoop(dwUserIndex, pState);

	// Blocks XInput Gamepad Input while GUI is Open
	if (g_show) {
		SetMemory(pState, 0, sizeof(XINPUT_STATE));
	}

	return 0;
}

}; // namespace Hook::XI

#pragma endregion

#include "CrimsonResolution.hpp"
#include "Core/Core.hpp"
#include "Core/Core_ImGui.hpp"
#include "Global.hpp"
#include "Vars.hpp"
#include "CrimsonConfig.hpp"
#include "CrimsonEfk.hpp"
#include "DebugDrawDX11.hpp"
#include "Global/HooksBase.hpp"
#include "Core/GUI.hpp"

#include <string>
#include <dxgi1_3.h>#include <vector>
#include <map>
#include <algorithm>
#include <tuple>
#include "Core/Macros.h"

namespace CrimsonResolution {

namespace {

uint32 g_gameWidth  = 0; // the game's own configured resolution
uint32 g_gameHeight = 0;
bool g_pending      = false;

bool g_flipActive = true;                                 // the swap chain in use is a flip model one
int32 g_appliedMode = DISPLAYMODE::BORDERLESS_FULLSCREEN; // the display mode last applied successfully
int g_exclusiveMonitor = 0; // monitor Legacy Fullscreen is currently on

struct Monitor {
    std::wstring device;
    std::string name; // the monitor's own name, e.g. "DELL U2723QE"
    RECT rect = {};
    bool primary = false;
};

std::vector<Monitor> g_monitors;
std::vector<std::string> g_monitorNames;

int g_resolutionMonitor = -1; // monitor g_resolutions was built for
std::vector<std::pair<uint32, uint32>> g_resolutions;
std::vector<std::string> g_resolutionNames; // entry 0 is the game's own resolution, the rest match g_resolutions

constexpr uint32 uiBaseHeight = 1080;

// sUserConfig_1405EA130: +0x670 width, +0x674 height, +0x678 fullscreen, +0x679 borderless
constexpr uintptr_t userConfigRva = 0x5EA130;

// Style CreateGameWindow_sub_140047B40 uses for its windowed window.
constexpr DWORD windowedStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

// RenderWidth_140C0B3FC, RenderHeight_140C0B3F8, sRender
constexpr uintptr_t renderWidthRva  = 0xC0B3FC;
constexpr uintptr_t renderHeightRva = 0xC0B3F8;
constexpr uintptr_t renderRva       = 0xC0B410;

// Render target objects inside sRender (SwapChainCreation_sub_140042890).
constexpr size_t fullDepthTargets[] = {0x20, 0x2C0};
constexpr size_t halfDepthTargets[] = {0x410, 0x4B8};
constexpr size_t fullColorTargets[] = {0xC8, 0x170, 0x218, 0x368};
constexpr size_t scaledTargetsBase  = 0x560; // 5 sizes (w >> 1 .. w >> 5) x 4 targets, 0xA8 each
constexpr size_t scaledTargetStride = 0xA8;
constexpr size_t scaledTargetCount  = 20;
constexpr size_t backBufferTarget   = 0x1320;
constexpr size_t copyTexture        = 0x1380;

constexpr size_t targetTextureOffset = 0x28;
constexpr size_t targetViewOffset    = 0x30;

constexpr uint32 colorFormat = 0x1C;
constexpr uint32 depthFormat = 0x2D;

using CreateColorTarget_t = char(__fastcall*)(uint8* target, int width, int height, uint32 format);                // sub_140045950
using CreateDepthTarget_t = char(__fastcall*)(uint8* target, uint32 width, uint32 height, uint32 format, uint32 depth); // sub_140045A30
using CreateTexture_t     = char(__fastcall*)(uint8* target, int width, int height, int format);                    // sub_140046E40
using SetBackBuffer_t     = char(__fastcall*)(uint8* target, uint16 width, uint16 height, void* texture);          // sub_140046F90

void ReleaseCom(uint8* target, size_t offset) {
    auto& object = *reinterpret_cast<IUnknown**>(target + offset);

    if (object) {
        object->Release();
        object = nullptr;
    }
}

void AttachBackBuffer(uint8* render, IDXGISwapChain* swapChain, uint32 width, uint32 height) {
    ID3D11Texture2D* backBuffer = nullptr;

    if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer))) || !backBuffer) {
        Log("%s GetBuffer failed", FUNC_NAME);
        return;
    }

    auto setBackBuffer = reinterpret_cast<SetBackBuffer_t>(appBaseAddr + 0x46F90);

    // The game keeps the reference returned by GetBuffer.
    setBackBuffer(render + backBufferTarget, static_cast<uint16>(width), static_cast<uint16>(height), backBuffer);
}

void RebuildGameTargets(uint8* render, uint32 width, uint32 height) {
    auto createColor   = reinterpret_cast<CreateColorTarget_t>(appBaseAddr + 0x45950);
    auto createDepth   = reinterpret_cast<CreateDepthTarget_t>(appBaseAddr + 0x45A30);
    auto createTexture = reinterpret_cast<CreateTexture_t>(appBaseAddr + 0x46E40);

    // The create functions overwrite the texture and view without releasing them.
    auto releaseTarget = [&](size_t offset) {
        ReleaseCom(render + offset, targetTextureOffset);
        ReleaseCom(render + offset, targetViewOffset);
    };

    for (auto offset : fullDepthTargets) {
        releaseTarget(offset);
        createDepth(render + offset, width, height, colorFormat, depthFormat);
    }

    for (auto offset : halfDepthTargets) {
        releaseTarget(offset);
        createDepth(render + offset, width / 2, height / 2, colorFormat, depthFormat);
    }

    for (auto offset : fullColorTargets) {
        releaseTarget(offset);
        createColor(render + offset, width, height, colorFormat);
    }

    for (size_t index = 0; index < scaledTargetCount; index++) {
        auto offset = scaledTargetsBase + scaledTargetStride * index;
        auto shift  = 1 + index / 4;

        releaseTarget(offset);
        createColor(render + offset, std::max<uint32>(width >> shift, 1), std::max<uint32>(height >> shift, 1), colorFormat);
    }

    releaseTarget(copyTexture);
    createTexture(render + copyTexture, width, height, colorFormat);
}

std::string ToUtf8(const wchar_t* text) {
    int size = WideCharToMultiByte(CP_UTF8, 0, text, -1, nullptr, 0, nullptr, nullptr);

    if (size <= 1) {
        return {};
    }

    std::string result(size - 1, '\0');
    WideCharToMultiByte(CP_UTF8, 0, text, -1, result.data(), size, nullptr, nullptr);

    return result;
}

// Maps GDI device names (\\.\DISPLAY1) to the names monitors report themselves.
std::map<std::wstring, std::string> QueryMonitorNames() {
    std::map<std::wstring, std::string> names;

    UINT32 pathCount = 0;
    UINT32 modeCount = 0;

    if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &pathCount, &modeCount) != ERROR_SUCCESS) {
        return names;
    }

    std::vector<DISPLAYCONFIG_PATH_INFO> paths(pathCount);
    std::vector<DISPLAYCONFIG_MODE_INFO> modes(modeCount);

    if (QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathCount, paths.data(), &modeCount, modes.data(), nullptr) != ERROR_SUCCESS) {
        return names;
    }

    for (UINT32 index = 0; index < pathCount; index++) {
        auto& path = paths[index];

        DISPLAYCONFIG_SOURCE_DEVICE_NAME source = {};
        source.header.type                      = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
        source.header.size                      = sizeof(source);
        source.header.adapterId                 = path.sourceInfo.adapterId;
        source.header.id                        = path.sourceInfo.id;

        DISPLAYCONFIG_TARGET_DEVICE_NAME target = {};
        target.header.type                      = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
        target.header.size                      = sizeof(target);
        target.header.adapterId                 = path.targetInfo.adapterId;
        target.header.id                        = path.targetInfo.id;

        if (DisplayConfigGetDeviceInfo(&source.header) == ERROR_SUCCESS && DisplayConfigGetDeviceInfo(&target.header) == ERROR_SUCCESS) {
            auto name = ToUtf8(target.monitorFriendlyDeviceName);

            if (!name.empty()) {
                names[source.viewGdiDeviceName] = name;
            }
        }
    }

    return names;
}

void RefreshMonitors() {
    g_monitors.clear();

    EnumDisplayMonitors(nullptr, nullptr,
        [](HMONITOR handle, HDC, LPRECT, LPARAM) -> BOOL {
            MONITORINFOEXW info = {};
            info.cbSize         = sizeof(info);

            if (GetMonitorInfoW(handle, &info)) {
                Monitor monitor;
                monitor.device  = info.szDevice;
                monitor.rect    = info.rcMonitor;
                monitor.primary = (info.dwFlags & MONITORINFOF_PRIMARY) != 0;
                g_monitors.push_back(monitor);
            }

            return TRUE;
        },
        0);

    if (g_monitors.empty()) {
        Monitor monitor;
        monitor.rect    = {0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
        monitor.primary = true;
        g_monitors.push_back(monitor);
    }

    auto monitorNames = QueryMonitorNames();

    for (auto& monitor : g_monitors) {
        auto found   = monitorNames.find(monitor.device);
        monitor.name = (found != monitorNames.end()) ? found->second : "Generic Monitor";
    }

    // Primary first, so index 0 is always the primary monitor.
    std::sort(g_monitors.begin(), g_monitors.end(), [](const Monitor& a, const Monitor& b) {
        if (a.primary != b.primary) {
            return a.primary;
        }
        return std::tie(a.rect.left, a.rect.top) < std::tie(b.rect.left, b.rect.top);
    });

    g_monitorNames.clear();

    for (size_t index = 0; index < g_monitors.size(); index++) {
        auto& monitor = g_monitors[index];

        char name[160] = {};
        snprintf(name, sizeof(name), "%u: %s - %ux%u%s", static_cast<unsigned>(index + 1), monitor.name.c_str(),
            static_cast<unsigned>(monitor.rect.right - monitor.rect.left), static_cast<unsigned>(monitor.rect.bottom - monitor.rect.top),
            monitor.primary ? " (Primary)" : "");
        g_monitorNames.push_back(name);
    }

    g_resolutionMonitor = -1;
}

int ClampMonitor(int32 index) {
    return std::clamp<int>(index, 0, static_cast<int>(g_monitors.size()) - 1);
}

RECT MonitorRect(int32 index) {
    if (g_monitors.empty()) {
        RefreshMonitors();
    }

    return g_monitors[ClampMonitor(index)].rect;
}

void BuildResolutions(int monitorIndex) {
    if (g_monitors.empty()) {
        RefreshMonitors();
    }

    monitorIndex = ClampMonitor(monitorIndex);

    if (g_resolutionMonitor == monitorIndex && !g_resolutionNames.empty()) {
        return;
    }

    g_resolutionMonitor = monitorIndex;
    g_resolutions.clear();

    const wchar_t* device = g_monitors[monitorIndex].device.empty() ? nullptr : g_monitors[monitorIndex].device.c_str();

    DEVMODEW mode = {};
    mode.dmSize   = sizeof(mode);

    for (DWORD index = 0; EnumDisplaySettingsW(device, index, &mode); index++) {
        if (mode.dmBitsPerPel >= 32 && mode.dmPelsWidth >= 640 && mode.dmPelsHeight >= 360) {
            g_resolutions.emplace_back(mode.dmPelsWidth, mode.dmPelsHeight);
        }
    }

    std::sort(g_resolutions.begin(), g_resolutions.end(), std::greater<>());
    g_resolutions.erase(std::unique(g_resolutions.begin(), g_resolutions.end()), g_resolutions.end());

    g_resolutionNames.clear();

    char name[48] = {};

    for (auto& [width, height] : g_resolutions) {
        snprintf(name, sizeof(name), "%ux%u", width, height);
        g_resolutionNames.push_back(name);
    }

    // Never leave the list empty, the GUI always shows an entry.
    if (g_resolutions.empty()) {
        auto& rect = g_monitors[monitorIndex].rect;

        g_resolutions.emplace_back(rect.right - rect.left, rect.bottom - rect.top);
        snprintf(name, sizeof(name), "%ux%u", g_resolutions[0].first, g_resolutions[0].second);
        g_resolutionNames.push_back(name);
    }
}

// The DXGI output of a detected monitor, on the swap chain's adapter. Null when it belongs to another adapter.
IDXGIOutput* FindOutput(IDXGISwapChain* swapChain, int monitorIndex) {
    IDXGIDevice* dxgiDevice = nullptr;

    if (FAILED(swapChain->GetDevice(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice))) || !dxgiDevice) {
        return nullptr;
    }

    IDXGIAdapter* adapter = nullptr;
    dxgiDevice->GetAdapter(&adapter);
    dxgiDevice->Release();

    if (!adapter) {
        return nullptr;
    }

    IDXGIOutput* found = nullptr;
    auto& device       = g_monitors[ClampMonitor(monitorIndex)].device;

    for (UINT index = 0; !found; index++) {
        IDXGIOutput* output = nullptr;

        if (adapter->EnumOutputs(index, &output) == DXGI_ERROR_NOT_FOUND) {
            break;
        }

        DXGI_OUTPUT_DESC desc = {};

        if (SUCCEEDED(output->GetDesc(&desc)) && device == desc.DeviceName) {
            found = output;
        } else {
            output->Release();
        }
    }

    adapter->Release();

    return found;
}

// Leaves exclusive fullscreen, moves the window and enters it again on the other monitor. Best effort: on failure it
// tries to go back to the old monitor.
bool MoveExclusiveToMonitor(IDXGISwapChain* swapChain, int newMonitor, int oldMonitor) {
    auto output = FindOutput(swapChain, newMonitor);

    if (!output) {
        Log("%s no DXGI output for monitor %d", FUNC_NAME, newMonitor);
        return false;
    }

    auto place = [](int monitor) {
        auto rect = MonitorRect(monitor);
        SetWindowPos(appWindow, HWND_TOP, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOOWNERZORDER);
    };

    swapChain->SetFullscreenState(FALSE, nullptr);
    place(newMonitor);

    HRESULT result = swapChain->SetFullscreenState(TRUE, output);
    output->Release();

    if (SUCCEEDED(result)) {
        return true;
    }

    Log("%s SetFullscreenState on monitor %d failed: %X", FUNC_NAME, newMonitor, result);

    if (auto previous = FindOutput(swapChain, oldMonitor)) {
        place(oldMonitor);
        swapChain->SetFullscreenState(TRUE, previous);
        previous->Release();
    }

    return false;
}

} // namespace

bool UsesFlipModel() {
    return g_flipActive;
}

std::vector<std::string>& MonitorNames() {
    if (g_monitors.empty()) {
        RefreshMonitors();
    }

    return g_monitorNames;
}

std::vector<std::string>& ResolutionNames(int32 monitorIndex) {
    BuildResolutions(monitorIndex);

    return g_resolutionNames;
}

// The exact match, or the closest resolution in the list.
int32 ResolutionIndex(int32 monitorIndex, int32 width, int32 height) {
    BuildResolutions(monitorIndex);

    int32 closest      = 0;
    int64 closestError = INT64_MAX;

    for (size_t index = 0; index < g_resolutions.size(); index++) {
        const int64 errorX = static_cast<int64>(g_resolutions[index].first) - width;
        const int64 errorY = static_cast<int64>(g_resolutions[index].second) - height;
        const int64 error  = errorX * errorX + errorY * errorY;

        if (error < closestError) {
            closestError = error;
            closest      = static_cast<int32>(index);
        }
    }

    return closest;
}

void ResolutionFromIndex(int32 monitorIndex, int32 index, int32& width, int32& height) {
    BuildResolutions(monitorIndex);

    index  = std::clamp<int32>(index, 0, static_cast<int32>(g_resolutions.size()) - 1);
    width  = static_cast<int32>(g_resolutions[index].first);
    height = static_cast<int32>(g_resolutions[index].second);
}

std::string MonitorResolutionName(int32 monitorIndex) {
    auto rect = MonitorRect(monitorIndex);

    char name[32] = {};
    snprintf(name, sizeof(name), "%ux%u", static_cast<unsigned>(rect.right - rect.left), static_cast<unsigned>(rect.bottom - rect.top));

    return name;
}

// Resolution the display mode renders at before the internal resolution percent: the monitor's for Borderless Fullscreen,
// otherwise the chosen resolution or the game's own.
static void GetBaseSize(uint32& width, uint32& height) {
    auto& system = activeCrimsonConfig.System;

    if (system.Mode() == DISPLAYMODE::BORDERLESS_FULLSCREEN) {
        auto rect = MonitorRect(system.monitorIndex);

        width  = rect.right - rect.left;
        height = rect.bottom - rect.top;
        return;
    }

    width  = g_gameWidth;
    height = g_gameHeight;

    if (system.resolutionWidth > 0 && system.resolutionHeight > 0) {
        width  = std::clamp<int32>(system.resolutionWidth, 320, 16384);
        height = std::clamp<int32>(system.resolutionHeight, 180, 16384);
    }
}

void GetRenderSize(uint32& width, uint32& height) {
    auto& system = activeCrimsonConfig.System;

    uint32 baseWidth  = 0;
    uint32 baseHeight = 0;
    GetBaseSize(baseWidth, baseHeight);

    // Exclusive Fullscreen renders at the display mode's resolution.
    auto percent = (system.Mode() == DISPLAYMODE::EXCLUSIVE_FULLSCREEN) ? 100 : std::clamp<int32>(system.internalResolutionPercent, 25, 400);

    width  = std::clamp<uint32>(baseWidth * percent / 100, 160, 16384);
    height = std::clamp<uint32>(baseHeight * percent / 100, 90, 16384);
}

// Lays the window out for the active display mode on the selected monitor.
static void ApplyWindowLayout(HWND window) {
    auto& system = activeCrimsonConfig.System;

    auto monitor       = MonitorRect(system.monitorIndex);
    const int monitorW = monitor.right - monitor.left;
    const int monitorH = monitor.bottom - monitor.top;

    if (system.Mode() == DISPLAYMODE::WINDOWED) {
        uint32 clientWidth  = 0;
        uint32 clientHeight = 0;
        GetBaseSize(clientWidth, clientHeight);

        const DWORD style = windowedStyle | WS_VISIBLE;
        SetWindowLongA(window, GWL_STYLE, style);
        SetWindowLongA(window, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);

        RECT rect = {0, 0, static_cast<LONG>(clientWidth), static_cast<LONG>(clientHeight)};
        AdjustWindowRect(&rect, style, FALSE);

        const int width  = rect.right - rect.left;
        const int height = rect.bottom - rect.top;

        SetWindowPos(window, HWND_NOTOPMOST, monitor.left + std::max(0, (monitorW - width) / 2),
            monitor.top + std::max(0, (monitorH - height) / 2), width, height, SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
    } else {
        // Fullscreen and Legacy Fullscreen are both a monitor sized popup.
        // WS_SYSMENU is needed for Alt+F4 to close the window.
        SetWindowLongA(window, GWL_STYLE, WS_POPUP | WS_SYSMENU | WS_VISIBLE);
        SetWindowLongA(window, GWL_EXSTYLE, WS_EX_APPWINDOW);

        SetWindowPos(window, HWND_TOP, monitor.left, monitor.top, monitorW, monitorH, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }

    g_window.borderless.active = (system.Mode() == DISPLAYMODE::BORDERLESS_FULLSCREEN);
}

void InitStartup() {
    // Physical pixels from here on, so monitor and mode sizes match what the game and DXGI use.
    SetProcessDPIAware();

    RefreshMonitors();

    auto& system = activeCrimsonConfig.System;

    system.windowMode = std::clamp<int32>(system.windowMode, 0, WINDOWMODE::COUNT - 1);
    g_appliedMode      = system.Mode();
    g_flipActive       = (g_appliedMode != DISPLAYMODE::EXCLUSIVE_FULLSCREEN);
    g_exclusiveMonitor = ClampMonitor(system.monitorIndex);

    auto userConfig    = appBaseAddr + userConfigRva;
    auto& configWidth  = *reinterpret_cast<uint32*>(userConfig + 0x670);
    auto& configHeight = *reinterpret_cast<uint32*>(userConfig + 0x674);
    auto& fullscreen   = *reinterpret_cast<uint8*>(userConfig + 0x678);
    auto& borderless   = *reinterpret_cast<uint8*>(userConfig + 0x679);

    g_gameWidth  = configWidth ? configWidth : 1920;
    g_gameHeight = configHeight ? configHeight : 1080;

    // No resolution chosen yet: start at the entry of the monitor's list closest to the game's own.
    if (system.resolutionWidth <= 0 || system.resolutionHeight <= 0) {
        ResolutionFromIndex(system.monitorIndex, ResolutionIndex(system.monitorIndex, g_gameWidth, g_gameHeight), system.resolutionWidth,
            system.resolutionHeight);

        queuedCrimsonConfig.System.resolutionWidth  = system.resolutionWidth;
        queuedCrimsonConfig.System.resolutionHeight = system.resolutionHeight;
    }

    uint32 baseWidth  = 0;
    uint32 baseHeight = 0;
    GetBaseSize(baseWidth, baseHeight);

    // The game creates a monitor sized popup for fullscreen, Windowed gets its window from ApplyWindowLayout.
    fullscreen = (system.Mode() != DISPLAYMODE::WINDOWED);
    borderless = 0;

    configWidth  = baseWidth;
    configHeight = baseHeight;

    if (system.Mode() == DISPLAYMODE::WINDOWED) {
        RECT rect = {0, 0, static_cast<LONG>(baseWidth), static_cast<LONG>(baseHeight)};
        AdjustWindowRect(&rect, windowedStyle, FALSE);

        configWidth  = rect.right - rect.left;
        configHeight = rect.bottom - rect.top;
    }

    uint32 width  = 0;
    uint32 height = 0;
    GetRenderSize(width, height);

    auto renderWidth  = reinterpret_cast<uint32*>(appBaseAddr + renderWidthRva);
    auto renderHeight = reinterpret_cast<uint32*>(appBaseAddr + renderHeightRva);

    // The swap chain and every render target are created from these.
    *renderWidth  = width;
    *renderHeight = height;

    Log("%s mode %d monitor %d resolution %ux%u render %ux%u (%d%%)", FUNC_NAME, system.Mode(), system.monitorIndex, baseWidth, baseHeight,
        width, height, system.internalResolutionPercent);
}

void PlaceWindowAtStartup(HWND window) {
    ApplyWindowLayout(window);
}

void ApplyDisplaySettings() {
    auto mode = activeCrimsonConfig.System.Mode();

    if (!appWindow) {
        return;
    }

    // Moving the window only applies to the swap chain in use. Switching between flip model and Legacy Fullscreen
    // happens in ApplyPending, which lays the window out for the new mode itself.
    if (mode != DISPLAYMODE::EXCLUSIVE_FULLSCREEN && g_flipActive) {
        ApplyWindowLayout(appWindow);
        g_appliedMode = mode;
    }

    RequestApply();
}

void ToggleBorderless() {
    auto& active = activeCrimsonConfig.System;

    if (!g_flipActive || active.Mode() == DISPLAYMODE::EXCLUSIVE_FULLSCREEN) {
        return;
    }

    active.windowMode                       = (active.windowMode == WINDOWMODE::FULLSCREEN) ? WINDOWMODE::WINDOWED : WINDOWMODE::FULLSCREEN;
    queuedCrimsonConfig.System.windowMode   = active.windowMode;
    ::GUI::save                             = true;

    ApplyDisplaySettings();
}

void GetUiSize(uint32 bufferWidth, uint32 bufferHeight, uint32& width, uint32& height) {
    // Fixed 1080p height with the buffer's aspect ratio, so the overlay looks the same at any window or render resolution.
    height = uiBaseHeight;
    width  = bufferHeight ? static_cast<uint32>(std::lround(static_cast<double>(uiBaseHeight) * bufferWidth / bufferHeight)) : 1920;
}

void UpdateUiSize(uint32 bufferWidth, uint32 bufferHeight) {
    uint32 width  = 0;
    uint32 height = 0;
    GetUiSize(bufferWidth, bufferHeight, width, height);

    UpdateGlobalRenderSize(bufferWidth, bufferHeight);
    UpdateGlobalUiSize(width, height);
    CoreImGui::UpdateDisplaySize(width, height);
}

void RequestApply() {
    g_pending = true;
}

enum class SwitchResult { Switched, Recovered, Failed };

static IDXGIAdapter* GetAdapter(ID3D11Device* device) {
    IDXGIDevice* dxgiDevice = nullptr;

    if (FAILED(device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice))) || !dxgiDevice) {
        return nullptr;
    }

    IDXGIAdapter* adapter = nullptr;
    dxgiDevice->GetAdapter(&adapter);
    dxgiDevice->Release();

    return adapter;
}

// A flip model swap chain for Fullscreen and Windowed.
static IDXGISwapChain* CreateFlipSwapChain(ID3D11Device* device, HWND window, uint32 width, uint32 height) {
    auto adapter = GetAdapter(device);

    if (!adapter) {
        return nullptr;
    }

    IDXGIFactory2* factory = nullptr;
    adapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&factory));
    adapter->Release();

    if (!factory) {
        return nullptr;
    }

    DXGI_SWAP_CHAIN_DESC1 desc = {};
    desc.Width                 = width;
    desc.Height                = height;
    desc.Format                = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count      = 1;
    desc.BufferUsage           = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount           = 2;
    desc.Scaling               = DXGI_SCALING_STRETCH;
    desc.SwapEffect            = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.AlphaMode             = DXGI_ALPHA_MODE_UNSPECIFIED;
    desc.Flags                 = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreen = {};
    fullscreen.Windowed                        = TRUE;

    IDXGISwapChain1* swapChain = nullptr;
    HRESULT result             = factory->CreateSwapChainForHwnd(device, window, &desc, &fullscreen, nullptr, &swapChain);

    if (SUCCEEDED(result)) {
        factory->MakeWindowAssociation(window, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);
    }

    factory->Release();

    if (FAILED(result) || !swapChain) {
        Log("%s CreateSwapChainForHwnd failed: %X", FUNC_NAME, result);
        return nullptr;
    }

    // Same low latency setup as when the game starts in a flip model mode.
    IDXGISwapChain2* swapChain2 = nullptr;

    if (SUCCEEDED(swapChain->QueryInterface(__uuidof(IDXGISwapChain2), reinterpret_cast<void**>(&swapChain2))) && swapChain2) {
        swapChain2->SetMaximumFrameLatency(1);

        g_frameLatencyWaitableObject = swapChain2->GetFrameLatencyWaitableObject();
        g_flipModelLatencyOptimized  = (g_frameLatencyWaitableObject != nullptr);

        swapChain2->Release();
    }

    return swapChain;
}

// The game's own kind of swap chain, taken into exclusive fullscreen on the selected monitor.
static IDXGISwapChain* CreateLegacySwapChain(ID3D11Device* device, HWND window, uint32 width, uint32 height, int monitor) {
    auto adapter = GetAdapter(device);

    if (!adapter) {
        return nullptr;
    }

    IDXGIFactory* factory = nullptr;
    adapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&factory));
    adapter->Release();

    if (!factory) {
        return nullptr;
    }

    DXGI_SWAP_CHAIN_DESC desc         = {};
    desc.BufferDesc.Width             = width;
    desc.BufferDesc.Height            = height;
    desc.BufferDesc.Format            = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BufferDesc.RefreshRate       = {60, 1};
    desc.BufferUsage                  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.SampleDesc.Count             = 1;
    desc.BufferCount                  = 1;
    desc.OutputWindow                 = window;
    desc.Windowed                     = TRUE;
    desc.SwapEffect                   = DXGI_SWAP_EFFECT_DISCARD;

    IDXGISwapChain* swapChain = nullptr;
    HRESULT result            = factory->CreateSwapChain(device, &desc, &swapChain);

    if (SUCCEEDED(result)) {
        factory->MakeWindowAssociation(window, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);
    }

    factory->Release();

    if (FAILED(result) || !swapChain) {
        Log("%s CreateSwapChain failed: %X", FUNC_NAME, result);
        return nullptr;
    }

    auto output = FindOutput(swapChain, monitor);

    result = swapChain->SetFullscreenState(TRUE, output);

    if (output) {
        output->Release();
    }

    if (FAILED(result)) {
        Log("%s SetFullscreenState failed: %X", FUNC_NAME, result);
        swapChain->Release();
        return nullptr;
    }

    // Switch the monitor to the resolution of the buffers.
    DXGI_MODE_DESC mode = desc.BufferDesc;
    mode.RefreshRate    = {0, 0};

    swapChain->ResizeTarget(&mode);
    swapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_UNKNOWN, 0);

    return swapChain;
}

// The mode the display settings were applied in last, put back into the active config after a failed switch.
static void RevertActiveMode() {
    auto& active = activeCrimsonConfig.System;

    active.windowMode       = (g_appliedMode == DISPLAYMODE::WINDOWED) ? WINDOWMODE::WINDOWED : WINDOWMODE::FULLSCREEN;
    active.legacyFullscreen = (g_appliedMode == DISPLAYMODE::EXCLUSIVE_FULLSCREEN);
}

// Replaces the wrapper's swap chain with one of the other kind. The window can only have one swap chain, so the old
// one goes first. If the new one can't be created the old kind is created again.
static SwitchResult SwitchSwapChain(SwapChainWrapper* wrapper, bool wantFlip, uint32 width, uint32 height, uint32 oldWidth, uint32 oldHeight) {
    auto device = wrapper->GetD3D11Device();
    auto window = wrapper->GetHWND();
    auto old    = wrapper->GetReal();

    BOOL fullscreen = FALSE;
    old->GetFullscreenState(&fullscreen, nullptr);

    if (fullscreen) {
        old->SetFullscreenState(FALSE, nullptr);
    }

    g_frameLatencyWaitableObject = nullptr;
    g_flipModelLatencyOptimized  = false;

    wrapper->ReleaseReal();

    auto create = [&](bool flip, uint32 createWidth, uint32 createHeight) -> IDXGISwapChain* {
        // The window follows the active display mode.
        ApplyWindowLayout(window);

        if (flip) {
            return CreateFlipSwapChain(device, window, createWidth, createHeight);
        }

        return CreateLegacySwapChain(device, window, createWidth, createHeight, activeCrimsonConfig.System.monitorIndex);
    };

    auto swapChain = create(wantFlip, width, height);
    auto result    = SwitchResult::Switched;

    if (!swapChain) {
        Log("%s switching to %s failed, going back", FUNC_NAME, wantFlip ? "flip model" : "Legacy Fullscreen");

        RevertActiveMode();

        swapChain = create(!wantFlip, oldWidth, oldHeight);
        result    = SwitchResult::Recovered;
    }

    if (!swapChain) {
        Log("%s could not create a swap chain", FUNC_NAME);
        return SwitchResult::Failed;
    }

    wrapper->SetReal(swapChain);

    return result;
}

void ApplyPending(SwapChainWrapper* wrapper) {
    if (!g_pending) {
        return;
    }

    g_pending = false;

    auto context   = wrapper->GetD3D11Context();
    auto swapChain = wrapper->GetReal();
    auto mode      = activeCrimsonConfig.System.Mode();

    if (!g_gameWidth || !g_gameHeight) {
        return;
    }

    auto renderWidth  = reinterpret_cast<uint32*>(appBaseAddr + renderWidthRva);
    auto renderHeight = reinterpret_cast<uint32*>(appBaseAddr + renderHeightRva);
    auto render       = reinterpret_cast<uint8*>(appBaseAddr + renderRva);

    uint32 oldWidth  = *renderWidth;
    uint32 oldHeight = *renderHeight;

    uint32 width  = 0;
    uint32 height = 0;
    GetRenderSize(width, height);

    const bool wantFlip     = (mode != DISPLAYMODE::EXCLUSIVE_FULLSCREEN);
    const bool switchChain  = (wantFlip != g_flipActive);

    // Legacy Fullscreen can be moved to another monitor while playing.
    const bool monitorChange = !switchChain && (mode == DISPLAYMODE::EXCLUSIVE_FULLSCREEN) &&
                               ClampMonitor(activeCrimsonConfig.System.monitorIndex) != g_exclusiveMonitor;

    if (width == oldWidth && height == oldHeight && !monitorChange && !switchChain) {
        return;
    }

    Log("%s %ux%u -> %ux%u%s%s", FUNC_NAME, oldWidth, oldHeight, width, height, monitorChange ? " (monitor change)" : "",
        switchChain ? " (swap chain change)" : "");

    // Nothing may reference the old buffers when the swap chain is resized.
    context->OMSetRenderTargets(0, nullptr, nullptr);

    ID3D11ShaderResourceView* nullViews[16] = {};
    context->PSSetShaderResources(0, 16, nullViews);
    context->VSSetShaderResources(0, 16, nullViews);
    context->Flush();

    RemoveRenderTarget();
    ReleaseCom(render + backBufferTarget, targetTextureOffset);

    if (switchChain) {
        auto outcome = SwitchSwapChain(wrapper, wantFlip, width, height, oldWidth, oldHeight);

        if (outcome == SwitchResult::Failed) {
            return;
        }

        swapChain = wrapper->GetReal();

        if (outcome == SwitchResult::Recovered) {
            width  = oldWidth;
            height = oldHeight;
        } else {
            g_flipActive       = wantFlip;
            g_appliedMode      = mode;
            g_exclusiveMonitor = ClampMonitor(activeCrimsonConfig.System.monitorIndex);
        }
    } else {
        DXGI_SWAP_CHAIN_DESC desc = {};
        swapChain->GetDesc(&desc);

        if (monitorChange) {
            const int newMonitor = ClampMonitor(activeCrimsonConfig.System.monitorIndex);

            if (MoveExclusiveToMonitor(swapChain, newMonitor, g_exclusiveMonitor)) {
                g_exclusiveMonitor = newMonitor;
            } else {
                // Back on the old monitor, the change waits for a restart.
                activeCrimsonConfig.System.monitorIndex = g_exclusiveMonitor;
            }
        }

        HRESULT result = S_OK;

        // In Exclusive Fullscreen the monitor's display mode has to change first.
        if (mode == DISPLAYMODE::EXCLUSIVE_FULLSCREEN) {
            DXGI_MODE_DESC target = desc.BufferDesc;
            target.Width          = width;
            target.Height         = height;
            target.RefreshRate    = {0, 0};

            result = swapChain->ResizeTarget(&target);

            if (FAILED(result)) {
                Log("%s ResizeTarget failed: %X", FUNC_NAME, result);
            }
        }

        if (SUCCEEDED(result)) {
            result = swapChain->ResizeBuffers(desc.BufferCount, width, height, DXGI_FORMAT_UNKNOWN, desc.Flags);
        }

        if (FAILED(result)) {
            Log("%s resize failed: %X", FUNC_NAME, result);

            if (mode == DISPLAYMODE::EXCLUSIVE_FULLSCREEN) {
                DXGI_MODE_DESC previous = desc.BufferDesc;
                swapChain->ResizeTarget(&previous);
            }

            swapChain->ResizeBuffers(desc.BufferCount, oldWidth, oldHeight, DXGI_FORMAT_UNKNOWN, desc.Flags);
            AttachBackBuffer(render, swapChain, oldWidth, oldHeight);
            CreateRenderTarget();

            return;
        }
    }

    *renderWidth  = width;
    *renderHeight = height;

    AttachBackBuffer(render, swapChain, width, height);
    RebuildGameTargets(render, width, height);

    CreateRenderTarget();

    UpdateUiSize(width, height);
    UpdateGlobalClientSize();
    UpdateGlobalWindowSize();
    UpdateMousePositionMultiplier();

    CrimsonEfk::EffekSetRenderSize(width, height);
    debug_draw_set_size(g_uiSize.x, g_uiSize.y);
}

} // namespace CrimsonResolution

#pragma once

#include <string>
#include <vector>

// Display mode, monitor and resolution handling. The game sizes its swap chain and every render target from
// RenderWidth_140C0B3FC / RenderHeight_140C0B3F8, copied from its config at CRT init.
//
// Exclusive Fullscreen keeps the game's own swap chain. Borderless Fullscreen and Windowed use a DXGI flip model
// swap chain that is stretched to the window, which lets the internal resolution differ from the window resolution
// and be changed while playing.

namespace CrimsonResolution {

// True unless the game started in Exclusive Fullscreen. The swap chain type is fixed for the session.
bool StartedWithFlipModel();

// Applies the display settings to the game's config before it creates its window. Call from the RegisterClassExW hook.
void InitStartup();

// Moves the window to the selected monitor before the swap chain is created.
void PlaceWindowAtStartup(HWND window);

// Display settings are applied by the GUI right away, see ApplyDisplayChange.
// Exclusive Fullscreen changes its display mode with the resolution and does not move a window.
void ApplyDisplaySettings();

// Alt+Enter: switches between Borderless Fullscreen and Windowed.
void ToggleBorderless();

// Detected monitors and the resolutions of one of them. ResolutionIndex returns the exact match or the closest entry.
std::vector<std::string>& MonitorNames();
std::vector<std::string>& ResolutionNames(int32 monitorIndex);
int32 ResolutionIndex(int32 monitorIndex, int32 width, int32 height);
void ResolutionFromIndex(int32 monitorIndex, int32 index, int32& width, int32& height);

// Monitor, resolution (for example 3840x2160) of the selected monitor.
std::string MonitorResolutionName(int32 monitorIndex);

// Render size of the active settings.
void GetRenderSize(uint32& width, uint32& height);

// Size the overlay and HUD are laid out in: 1080 tall with the buffer's aspect ratio, whatever the window or internal resolution.
void GetUiSize(uint32 bufferWidth, uint32 bufferHeight, uint32& width, uint32& height);

// Updates g_renderSize (buffer), g_uiSize and the ImGui display size from the swap chain buffer size.
void UpdateUiSize(uint32 bufferWidth, uint32 bufferHeight);

// Queues a render resolution change, executed at the end of the next Present.
void RequestApply();

// Rebuilds the swap chain buffers and the game's render targets when a change was requested.
void ApplyPending(IDXGISwapChain* swapChain, ID3D11DeviceContext* context);

} // namespace CrimsonResolution

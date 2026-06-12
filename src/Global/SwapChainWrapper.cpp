// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "SwapChainWrapper.hpp"
#include <dxgi1_4.h>
#include "HooksBase.hpp"
#include "../Core/Macros.h"
#include "../CrimsonHUD.hpp"
#include "../CrimsonEfkPreload.hpp"
#include "../NvApiReflex.hpp"

namespace {
    constexpr double kMaxFpsCap = 500.0;
    inline double ClampFpsCap(double cap) { return cap > kMaxFpsCap ? kMaxFpsCap : cap; }
}

// Construction

SwapChainWrapper::SwapChainWrapper(IDXGISwapChain* real, ID3D11Device* device, ID3D11DeviceContext* context, HWND hWnd)
    : m_real(real)
    , m_device(device)
    , m_context(context)
    , m_hWnd(hWnd)
    , m_refCount(1)
    , m_version(0) {
    // Keep the real chain alive as long as we're around.
    if (m_real)
        m_real->AddRef();
}

SwapChainWrapper::~SwapChainWrapper() {
    if (m_real)
        m_real->Release();
}

// IUnknown

HRESULT STDMETHODCALLTYPE SwapChainWrapper::QueryInterface(REFIID riid, void** ppv) {
    if (!ppv)
        return E_POINTER;

    // For the base swap chain / IUnknown stuff, hand out ourselves
    // so the game always hits our Present(). For anything else
    // (IDXGISwapChain1/2/3/4 etc.) just pass through — DMC3 never
    // asks for those anyway.
    if (riid == __uuidof(IDXGISwapChain) || riid == __uuidof(IUnknown) || riid == __uuidof(IDXGIDeviceSubObject) || riid == __uuidof(IDXGIObject)) {
        AddRef();
        *ppv = static_cast<IDXGISwapChain*>(this);
        return S_OK;
    }

    return m_real->QueryInterface(riid, ppv);
}

ULONG STDMETHODCALLTYPE SwapChainWrapper::AddRef() {
    return InterlockedIncrement(&m_refCount);
}

ULONG STDMETHODCALLTYPE SwapChainWrapper::Release() {
    LONG ref = InterlockedDecrement(&m_refCount);
    if (ref == 0)
        delete this;
    return ref;
}

// IDXGIObject

HRESULT STDMETHODCALLTYPE SwapChainWrapper::SetPrivateData(REFGUID Name, UINT DataSize, const void* pData) {
    return m_real->SetPrivateData(Name, DataSize, pData);
}

HRESULT STDMETHODCALLTYPE SwapChainWrapper::SetPrivateDataInterface(REFGUID Name, const IUnknown* pUnknown) {
    return m_real->SetPrivateDataInterface(Name, pUnknown);
}

HRESULT STDMETHODCALLTYPE SwapChainWrapper::GetPrivateData(REFGUID Name, UINT* pDataSize, void* pData) {
    return m_real->GetPrivateData(Name, pDataSize, pData);
}

HRESULT STDMETHODCALLTYPE SwapChainWrapper::GetParent(REFIID riid, void** ppParent) {
    return m_real->GetParent(riid, ppParent);
}

// IDXGIDeviceSubObject

HRESULT STDMETHODCALLTYPE SwapChainWrapper::GetDevice(REFIID riid, void** ppDevice) {
    return m_real->GetDevice(riid, ppDevice);
}

// IDXGISwapChain — boring forwards (Present is at the bottom)

HRESULT STDMETHODCALLTYPE SwapChainWrapper::GetBuffer(UINT Buffer, REFIID riid, void** ppSurface) {
    return m_real->GetBuffer(Buffer, riid, ppSurface);
}

HRESULT STDMETHODCALLTYPE SwapChainWrapper::SetFullscreenState(BOOL Fullscreen, IDXGIOutput* pTarget) {
    return m_real->SetFullscreenState(Fullscreen, pTarget);
}

HRESULT STDMETHODCALLTYPE SwapChainWrapper::GetFullscreenState(BOOL* pFullscreen, IDXGIOutput** ppTarget) {
    return m_real->GetFullscreenState(pFullscreen, ppTarget);
}

HRESULT STDMETHODCALLTYPE SwapChainWrapper::GetDesc(DXGI_SWAP_CHAIN_DESC* pDesc) {
    return m_real->GetDesc(pDesc);
}

HRESULT STDMETHODCALLTYPE SwapChainWrapper::ResizeBuffers(UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
    RemoveRenderTarget();

    HRESULT hr = m_real->ResizeBuffers(BufferCount, Width, Height, NewFormat, SwapChainFlags);

    if (SUCCEEDED(hr)) {
        CreateRenderTarget();
        UpdateGlobalWindowSize();
        UpdateGlobalClientSize();
        UpdateGlobalRenderSize(Width, Height);
    }

    return hr;
}

HRESULT STDMETHODCALLTYPE SwapChainWrapper::ResizeTarget(const DXGI_MODE_DESC* pNewTargetParameters) {
    return m_real->ResizeTarget(pNewTargetParameters);
}

HRESULT STDMETHODCALLTYPE SwapChainWrapper::GetContainingOutput(IDXGIOutput** ppOutput) {
    return m_real->GetContainingOutput(ppOutput);
}

HRESULT STDMETHODCALLTYPE SwapChainWrapper::GetFrameStatistics(DXGI_FRAME_STATISTICS* pStats) {
    return m_real->GetFrameStatistics(pStats);
}

HRESULT STDMETHODCALLTYPE SwapChainWrapper::GetLastPresentCount(UINT* pLastPresentCount) {
    return m_real->GetLastPresentCount(pLastPresentCount);
}

// Present

HRESULT STDMETHODCALLTYPE SwapChainWrapper::Present(UINT SyncInterval, UINT Flags) {
    // First-frame init — deferring this here means we don't need to
    // worry about the device context being ready during creation.
    static bool s_firstFrameInitDone = false;
    if (!s_firstFrameInitDone) {
        s_firstFrameInitDone = true;

        if (!ImGui_ImplDX11_Init(m_device, m_context)) {
            Log("%s Failed to initialize ImGui on D3D11 -> ImGui_ImplDX11_Init.", FUNC_NAME);
        }

        CreateRenderTarget();

        CrimsonHUD::InitTextures(m_device);
        InitStyleSwitchFxTexture(m_device);

        DXGI_SWAP_CHAIN_DESC initDesc = {};
        if (SUCCEEDED(m_real->GetDesc(&initDesc))) {
            debug_draw_init((void*)m_device, (void*)m_context, initDesc.BufferDesc.Width, initDesc.BufferDesc.Height);

            bool efk = CrimsonEfk::EffekInit(m_device, m_context, initDesc.BufferDesc.Width, initDesc.BufferDesc.Height);
            if (efk) {
                CrimsonEfkPreload::PreloadAll();
            }
        }

        FPSLimiter_Init(500.0);

        // Always initialize NVAPI on first frame — needed for PCL Stats
        // (NVIDIA App overlay) regardless of Reflex toggle state.
        if (GetNvApiReflex().Initialize(m_device)) {
            Log("%s NVAPI loaded — PCL Stats active", FUNC_NAME);

            // Configure Reflex low-latency mode if enabled in config.
            // fpsUnlocked is now capped at kMaxFpsCap (500) instead of unlimited.
            if (activeCrimsonConfig.System.nvidiaReflex) {
                double reflexCap = activeCrimsonConfig.System.fpsUnlocked
                    ? kMaxFpsCap : ClampFpsCap(activeCrimsonConfig.System.fpsCap);
                if (GetNvApiReflex().Configure(
                        true,
                        reflexCap,
                        activeCrimsonConfig.System.nvidiaReflexBoost,
                        true)) {
                    Log("%s NVIDIA Reflex low-latency mode enabled", FUNC_NAME);
                } else {
                    Log("%s NVIDIA Reflex unavailable — falling back to CPU limiter", FUNC_NAME);
                }
            }
        } else {
            Log("%s NVAPI not available — PCL Stats and Reflex disabled", FUNC_NAME);
        }
    }

    // ── Frame pacing
    // NVIDIA Reflex: Sleep() at frame start lets the driver pace frames
    // at the GPU-optimal point. Only active when Reflex is toggled on AND
    // successfully configured. Falls back to CPU FPS limiter otherwise.
    // Always runs regardless of fpsUnlocked, the cap is clamped to kMaxFpsCap.
    if (activeCrimsonConfig.System.nvidiaReflex && !GetNvApiReflex().Sleep()) {
        FPSLimiter_Apply();
    } else if (!activeCrimsonConfig.System.nvidiaReflex) {
        FPSLimiter_Apply();
    }

    // NVIDIA Reflex / PCL Stats: begin frame marker (SIMULATION_START)
    static uint64_t s_reflexFrameID = 0;
    if (GetNvApiReflex().IsNvApiLoaded()) {
        GetNvApiReflex().BeginFrame(s_reflexFrameID);
    }

    // VSync
    if (activeConfig.vSync != 0)
        SyncInterval = activeConfig.vSync - 1;

    // Size
    static uint32 prevWidth  = 0;
    static uint32 prevHeight = 0;

    DXGI_SWAP_CHAIN_DESC swapDesc = {};
    m_real->GetDesc(&swapDesc);
    uint32 width  = swapDesc.BufferDesc.Width;
    uint32 height = swapDesc.BufferDesc.Height;

    if (width != prevWidth || height != prevHeight) {
        prevWidth  = width;
        prevHeight = height;
        UpdateGlobalRenderSize(width, height);
        UpdateGlobalClientSize();
        UpdateGlobalWindowSize();
    }

    // Input
    UpdateShow();
    UpdateKeyboard();
    UpdateMouse();
    UpdateGamepad();

    // ImGui
    CrimsonEfk::CaptureDepthStencilForPresent(m_context);
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();

    auto& io   = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)width, (float)height);

    ImGui_ImplWin32_GetDpiScaleForHwnd(m_hWnd);

    if (g_clientSize.x > 0 && g_clientSize.y > 0) {
        io.MousePos.x *= ((float)width  / g_clientSize.x);
        io.MousePos.y *= ((float)height / g_clientSize.y);
    }

    Timestep();
    ImGui::NewFrame();

    // Overlay
    CrimsonSDL::InitSDL();
    GUI_Render(m_real);
    DrawStyleSwitchFxTexture();
    ImGui::Render();

    // NVIDIA Reflex / PCL Stats: end simulation, begin render submission
    if (GetNvApiReflex().IsNvApiLoaded()) {
        GetNvApiReflex().SetLatencyMarker(SIMULATION_END, s_reflexFrameID);
        GetNvApiReflex().SetLatencyMarker(RENDERSUBMIT_START, s_reflexFrameID);
    }

    // Render
    m_context->OMSetRenderTargets(1, &::D3D11::renderTargetView, nullptr);

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    CrimsonEfk::EffekIncFrames();
    CrimsonEfk::EffekRenderOnPresent(m_context);
    debug_draw_update(CrimsonClock::DeltaTime());

    // ImGui viewport extension (docking etc.)
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    // PresentFlags
    UINT presentFlags = Flags;
    if (activeCrimsonConfig.System.flipModelPresentation) {
        if (SyncInterval == 0)
            presentFlags = DXGI_PRESENT_ALLOW_TEARING;
    }

    // Poll the frame latency waitable — non-blocking, just lets the
    // FPS limiter naturally back-pressure if the GPU is behind.
    if (activeCrimsonConfig.System.flipModelPresentation && g_frameLatencyWaitableObject != nullptr) {
        WaitForSingleObject(g_frameLatencyWaitableObject, 0);
    }

    // Callback — let anyone hooked into the old Present_func still fire.
    [&]() {
        auto func = Present_func;
        if (!func)
            return;
        func();
    }();

    // NVIDIA Reflex / PCL Stats: end render submission, mark present start
    if (GetNvApiReflex().IsNvApiLoaded()) {
        GetNvApiReflex().SetLatencyMarker(RENDERSUBMIT_END, s_reflexFrameID);
        GetNvApiReflex().SetLatencyMarker(PRESENT_START, s_reflexFrameID);
    }

    // Present — the one that actually hits the GPU.
    HRESULT hr = m_real->Present(SyncInterval, presentFlags);

    // NVIDIA Reflex / PCL Stats: mark present end
    if (GetNvApiReflex().IsNvApiLoaded()) {
        GetNvApiReflex().SetLatencyMarker(PRESENT_END, s_reflexFrameID);
    }

    // If Present blew up with a mode-change error, try fixing the swap
    // chain. DXGI_PRESENT_TEST tells us what's wrong, then we walk a
    // ResizeBuffers → ResizeTarget → toggle-fullscreen recovery chain.
    if (FAILED(hr) && hr != 0x087A0001) {
        HRESULT probe = m_real->Present(SyncInterval, DXGI_PRESENT_TEST);

        if (probe == DXGI_STATUS_MODE_CHANGE_IN_PROGRESS) {
            DXGI_SWAP_CHAIN_DESC desc = {};
            m_real->GetDesc(&desc);

            if (FAILED(m_real->ResizeBuffers(desc.BufferCount,
                desc.BufferDesc.Width, desc.BufferDesc.Height,
                desc.BufferDesc.Format, desc.Flags))) {

                DXGI_MODE_DESC md = desc.BufferDesc;
                m_real->ResizeTarget(&md);

                if (FAILED(m_real->ResizeBuffers(desc.BufferCount,
                    desc.BufferDesc.Width, desc.BufferDesc.Height,
                    desc.BufferDesc.Format, desc.Flags))) {

                    BOOL fs = FALSE;
                    m_real->GetFullscreenState(&fs, nullptr);
                    if (SUCCEEDED(m_real->SetFullscreenState(!fs, nullptr)))
                        m_real->ResizeBuffers(desc.BufferCount,
                            desc.BufferDesc.Width, desc.BufferDesc.Height,
                            desc.BufferDesc.Format, desc.Flags);
                }
            }

            RemoveRenderTarget();
            CreateRenderTarget();
            UpdateGlobalWindowSize();
            UpdateGlobalClientSize();
            UpdateGlobalRenderSize(desc.BufferDesc.Width, desc.BufferDesc.Height);

            hr = m_real->Present(SyncInterval, presentFlags);
        }
    }

    // Occluded
    if (hr == 0x087A0001) {
        static bool loggedOccluded = false;
        if (!loggedOccluded) {
            loggedOccluded = true;
            Log("Present: swap chain occluded");
        }
        return hr;
    }

    // FPS
    static double g_currentCap = -1.0;
    static bool   g_wasUnlocked = false;
    static bool   g_wasReflexOn = false;
    static bool   s_reflexJustToggled = false;
    double newCap = activeCrimsonConfig.System.fpsCap;
    bool   newUnlocked = activeCrimsonConfig.System.fpsUnlocked;
    bool   newReflexOn = activeCrimsonConfig.System.nvidiaReflex;
    bool   newReflexBoost = activeCrimsonConfig.System.nvidiaReflexBoost;

    if (g_currentCap != newCap || g_wasUnlocked != newUnlocked || g_wasReflexOn != newReflexOn) {
        // Log what changed
        if (g_wasReflexOn != newReflexOn) {
            Log("Reflex toggle: %s -> %s (user changed setting)",
                g_wasReflexOn ? "ON" : "OFF",
                newReflexOn ? "ON" : "OFF");
            s_reflexJustToggled = true;
        }

        g_currentCap = newCap;
        g_wasUnlocked = newUnlocked;
        g_wasReflexOn = newReflexOn;

        // Cap at kMaxFpsCap (500) — even "unlocked" mode is now clamped.
        double reflexCap = newUnlocked ? kMaxFpsCap : ClampFpsCap(newCap);

        // Update Reflex configuration when FPS cap, unlock state, or Reflex toggle changes
        if (newReflexOn && GetNvApiReflex().IsNvApiLoaded()) {
            GetNvApiReflex().Configure(
                true,
                reflexCap,
                newReflexBoost,
                true);
        } else if (!newReflexOn && GetNvApiReflex().IsAvailable()) {
            // Reflex toggled off — disable low-latency mode but keep PCL Stats active
            GetNvApiReflex().Configure(false, 0.0f, false, false);
        }

        // Keep CPU-side limiter in sync when Reflex isn't handling pacing
        if (!newReflexOn || !GetNvApiReflex().IsAvailable()) {
            FPSLimiter_Init(reflexCap);
        }
    }

    {
        static LARGE_INTEGER s_verifyTime = {};
        static LARGE_INTEGER s_verifyFreq = {};
        static bool          s_verifyArmed = false;
        if (s_reflexJustToggled) {
            s_reflexJustToggled = false;
            QueryPerformanceFrequency(&s_verifyFreq);
            QueryPerformanceCounter(&s_verifyTime);
            s_verifyTime.QuadPart += s_verifyFreq.QuadPart * 2; // +2s
            s_verifyArmed = true;
        }
        if (s_verifyArmed) {
            LARGE_INTEGER now;
            QueryPerformanceCounter(&now);
            if (now.QuadPart >= s_verifyTime.QuadPart) {
                s_verifyArmed = false;
                bool     driverLL = false, driverBoost = false, driverGS = false;
                uint32_t driverInterval = 0;
                if (GetNvApiReflex().GetSleepStatus(driverLL, driverBoost, driverInterval, driverGS)) {
                    Log("Reflex driver state: LowLatency=%d Interval=%uus GameSleep=%d",
                        driverLL, driverInterval, driverGS);
                }
            }
        }
    }

    // Increment frame counter (for PCL Stats / Reflex markers)
    if (GetNvApiReflex().IsNvApiLoaded()) {
        ++s_reflexFrameID;
    }

    return hr;
}

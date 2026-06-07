// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "SwapChainWrapper.hpp"
#include "HooksBase.hpp"
#include "../Core/Macros.h"
#include "../CrimsonHUD.hpp"
#include "../CrimsonEfkPreload.hpp"

// Construction

SwapChainWrapper::SwapChainWrapper(IDXGISwapChain* real, ID3D11Device* device, ID3D11DeviceContext* context, HWND hWnd)
    : m_real(real)
    , m_device(device)
    , m_context(context)
    , m_hWnd(hWnd)
    , m_refCount(1) {
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
    // Bail immediately when we're not the active window.
    // If we don't, DWM blocks on our ImGui work during alt-tab.
//     if (g_appInactive)
//         return m_real->Present(SyncInterval, Flags);

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

    // Render
    m_context->OMSetRenderTargets(1, &::D3D11::renderTargetView, nullptr);

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    CrimsonEfk::EffekIncFrames();
    CrimsonEfk::EffekRenderOnPresent(m_context);
    debug_draw_update(io.DeltaTime);

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

    // Present — the one that actually hits the GPU.
    HRESULT hr = m_real->Present(SyncInterval, presentFlags);

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
    double newCap = activeCrimsonConfig.System.fpsCap;
    if (g_currentCap != newCap) {
        g_currentCap = newCap;
        FPSLimiter_Init(g_currentCap);
    }

    if (!activeCrimsonConfig.System.fpsUnlocked)
        FPSLimiter_Apply();

    return hr;
}

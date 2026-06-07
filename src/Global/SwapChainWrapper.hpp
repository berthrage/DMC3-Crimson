#pragma once

#include <dxgi.h>
#include <d3d11.h>

// Wraps the real swap chain so we own Present() without touching its vtable.
// Everything else forwards straight to the real thing.
// DWM gets the unwrapped chain — we get to intercept every game frame.

class SwapChainWrapper : public IDXGISwapChain {
public:
    SwapChainWrapper(IDXGISwapChain* real, ID3D11Device* device, ID3D11DeviceContext* context, HWND hWnd);
    virtual ~SwapChainWrapper();

    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override;
    ULONG   STDMETHODCALLTYPE AddRef() override;
    ULONG   STDMETHODCALLTYPE Release() override;

    // IDXGIObject
    HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID Name, UINT DataSize, const void* pData) override;
    HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(REFGUID Name, const IUnknown* pUnknown) override;
    HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID Name, UINT* pDataSize, void* pData) override;
    HRESULT STDMETHODCALLTYPE GetParent(REFIID riid, void** ppParent) override;

    // IDXGIDeviceSubObject
    HRESULT STDMETHODCALLTYPE GetDevice(REFIID riid, void** ppDevice) override;

    // IDXGISwapChain — Present() is the only one with our code.
    HRESULT STDMETHODCALLTYPE Present(UINT SyncInterval, UINT Flags) override;

    HRESULT STDMETHODCALLTYPE GetBuffer(UINT Buffer, REFIID riid, void** ppSurface) override;
    HRESULT STDMETHODCALLTYPE SetFullscreenState(BOOL Fullscreen, IDXGIOutput* pTarget) override;
    HRESULT STDMETHODCALLTYPE GetFullscreenState(BOOL* pFullscreen, IDXGIOutput** ppTarget) override;
    HRESULT STDMETHODCALLTYPE GetDesc(DXGI_SWAP_CHAIN_DESC* pDesc) override;
    HRESULT STDMETHODCALLTYPE ResizeBuffers(UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) override;
    HRESULT STDMETHODCALLTYPE ResizeTarget(const DXGI_MODE_DESC* pNewTargetParameters) override;
    HRESULT STDMETHODCALLTYPE GetContainingOutput(IDXGIOutput** ppOutput) override;
    HRESULT STDMETHODCALLTYPE GetFrameStatistics(DXGI_FRAME_STATISTICS* pStats) override;
    HRESULT STDMETHODCALLTYPE GetLastPresentCount(UINT* pLastPresentCount) override;

    // Accessors
    IDXGISwapChain*      GetReal()          const { return m_real; }
    ID3D11Device*        GetD3D11Device()   const { return m_device; }
    ID3D11DeviceContext* GetD3D11Context()  const { return m_context; }
    HWND                 GetHWND()          const { return m_hWnd; }

private:
    IDXGISwapChain*      m_real;
    ID3D11Device*        m_device;
    ID3D11DeviceContext* m_context;
    HWND                 m_hWnd;
    LONG                 m_refCount;
};

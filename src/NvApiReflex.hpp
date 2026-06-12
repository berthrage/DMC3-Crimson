// NvApiReflex.hpp — NVIDIA Reflex Integration via NVAPI
// 
// Dynamically loads nvapi64.dll and exposes Reflex low-latency
// functions using NVIDIA's public NVAPI function IDs.
//
// Reference: NVIDIA NVAPI SDK (public), nvapi_interface.h
// Function IDs verified against github.com/NVIDIA/nvapi

#pragma once

#include <Unknwn.h>
#include <cstdint>

// NVAPI structures expect 8-byte packing to match the driver's ABI.
#pragma pack(push, 8)

// ── NVAPI Status Codes (public) ────────────────────────────────────────────
using NvAPI_Status = int32_t;

constexpr NvAPI_Status NVAPI_OK                 =  0;
constexpr NvAPI_Status NVAPI_ERROR              = -1;
constexpr NvAPI_Status NVAPI_NOT_SUPPORTED      = -2;
constexpr NvAPI_Status NVAPI_NO_IMPLEMENTATION  = -3;
constexpr NvAPI_Status NVAPI_INVALID_ARGUMENT   = -5;
constexpr NvAPI_Status NVAPI_INCOMPATIBLE_STRUCT_VERSION = -9;
constexpr NvAPI_Status NVAPI_INVALID_POINTER    = -174;

// ── NVAPI version helper ───────────────────────────────────────────────────
// NVAPI encodes the struct size into the lower 16 bits of the version field
// and a user-visible version number into the upper 16 bits.
#define NVAPI_MAKE_VERSION(typeName, ver) \
    static_cast<uint32_t>(sizeof(typeName) | ((ver) << 16))

// ── Reflex Latency Marker Types (public) ───────────────────────────────────
enum NV_LATENCY_MARKER_TYPE : uint32_t {
    SIMULATION_START    = 0,
    SIMULATION_END      = 1,
    RENDERSUBMIT_START  = 2,
    RENDERSUBMIT_END    = 3,
    PRESENT_START       = 4,
    PRESENT_END         = 5,
    INPUT_SAMPLE        = 6,
    TRIGGER_FLASH       = 7,
    PC_LATENCY_PING     = 8,
    OUT_OF_BAND_RENDERSUBMIT_START = 9,
    OUT_OF_BAND_RENDERSUBMIT_END   = 10,
    OUT_OF_BAND_PRESENT_START      = 11,
    OUT_OF_BAND_PRESENT_END        = 12,
};

// ── Reflex Parameter Structures ───────────────────────────────────────────
// Field layouts verified against NVIDIA's official MIT-licensed nvapi.h
// https://github.com/NVIDIA/nvapi

// NV_LATENCY_MARKER_PARAMS_V1 — 88 bytes
//   +0:  version    (NvU32, 4)
//   +4:  (padding to 8-byte-align frameID)
//   +8:  frameID    (NvU64, 8)
//   +16: markerType (NvU32, 4)
//   +20: (padding to 8-byte-align rsvd0)
//   +24: rsvd0      (NvU64, 8)
//   +32: rsvd[56]   (NvU8[56])
struct NV_LATENCY_MARKER_PARAMS {
    uint32_t              version;
    uint32_t              _pad0;    // alignment padding
    uint64_t              frameID;
    NV_LATENCY_MARKER_TYPE markerType;
    uint32_t              _pad1;    // alignment padding
    uint64_t              rsvd0;
    uint8_t               rsvd[56];
};

static_assert(sizeof(NV_LATENCY_MARKER_PARAMS) == 88,
    "NV_LATENCY_MARKER_PARAMS must be 88 bytes");

// NV_SET_SLEEP_MODE_PARAMS_V1 — 44 bytes
//   +0:  version              (NvU32, 4)
//   +4:  bLowLatencyMode       (NvBool/NvU8, 1)
//   +5:  bLowLatencyBoost      (NvBool/NvU8, 1)
//   +6:  (padding 2 bytes to 4-byte-align minimumIntervalUs)
//   +8:  minimumIntervalUs     (NvU32, 4)
//   +12: bUseMarkersToOptimize (NvBool/NvU8, 1)
//   +13: rsvd[31]             (NvU8[31])
struct NV_SET_SLEEP_MODE_PARAMS {
    uint32_t version;
    uint8_t  bLowLatencyMode;
    uint8_t  bLowLatencyBoost;
    uint8_t  _pad0[2];               // alignment padding
    uint32_t minimumIntervalUs;
    uint8_t  bUseMarkersToOptimize;
    uint8_t  rsvd[31];
};

static_assert(sizeof(NV_SET_SLEEP_MODE_PARAMS) == 44,
    "NV_SET_SLEEP_MODE_PARAMS must be 44 bytes");

// NV_GET_SLEEP_STATUS_PARAMS_V1 — 136 bytes
struct NV_GET_SLEEP_STATUS_PARAMS {
    uint32_t version;
    uint8_t  bLowLatencyMode;      // (OUT) Is low latency mode enabled?
    uint8_t  bFsVrr;               // (OUT) Is fullscreen VRR enabled?
    uint8_t  bCplVsyncOn;          // (OUT) Is Control Panel overriding VSYNC ON?
    uint8_t  _pad0;                // alignment padding
    uint32_t sleepIntervalUs;      // (OUT) Latest sleep interval in microseconds
    uint8_t  bUseGameSleep;        // (OUT) Is NvAPI_D3D_Sleep() being called?
    uint8_t  rsvd[123];            // Reserved

    uint32_t versionVal() const { return version; }
};

static_assert(sizeof(NV_GET_SLEEP_STATUS_PARAMS) == 136,
    "NV_GET_SLEEP_STATUS_PARAMS must be 136 bytes");

#pragma pack(pop)

// ── Function Pointer Types ─────────────────────────────────────────────────
using NvAPI_QueryInterface_t = void* (*)(unsigned int ordinal);

using NvAPI_D3D_SetLatencyMarker_t = NvAPI_Status (__cdecl *)(
    IUnknown*                  pDevice,
    NV_LATENCY_MARKER_PARAMS*  pParams);

using NvAPI_D3D_SetSleepMode_t = NvAPI_Status (__cdecl *)(
    IUnknown*                  pDevice,
    NV_SET_SLEEP_MODE_PARAMS*  pParams);

using NvAPI_D3D_Sleep_t = NvAPI_Status (__cdecl *)(
    IUnknown*                  pDevice);

using NvAPI_D3D_GetSleepStatus_t = NvAPI_Status (__cdecl *)(
    IUnknown*                  pDevice,
    NV_GET_SLEEP_STATUS_PARAMS* pParams);

// ── NVAPI Reflex Manager ───────────────────────────────────────────────────
// Singleton that handles NVAPI lifecycle and provides Reflex functions.
//
// Usage:
//   NvApiReflex::Get().Initialize();          // call once after device creation
//   NvApiReflex::Get().Configure(fps, boost); // set low-latency mode
//   NvApiReflex::Get().SetLatencyMarker(...); // per-frame markers
//   NvApiReflex::Get().Sleep();               // GPU-optimal frame pacing
//   NvApiReflex::Get().Shutdown();            // on device teardown

class NvApiReflex {
public:
    static NvApiReflex& Get();

    // One-time NVAPI initialization. Must be called after D3D11 device is ready.
    // Always call this on first frame — it loads NVAPI and starts PCL Stats.
    // Returns true if NVAPI loaded successfully (even if Reflex functions unavailable).
    bool Initialize(IUnknown* d3d11Device);

    // Release NVAPI resources. Safe to call multiple times.
    void Shutdown();

    // NVAPI library loaded and functions resolved (PCL Stats active).
    bool IsNvApiLoaded() const { return m_initialized; }

    // Reflex fully active: NVAPI loaded AND sleep mode configured.
    bool IsAvailable() const { return m_initialized && m_sleepModeSet; }

    // PC Latency Stats — reports frame timing to NVIDIA App / GFE overlay
    // via ETW TraceLogging. Call alongside SetLatencyMarker for each marker.
    void PclStatsMarker(NV_LATENCY_MARKER_TYPE type, uint64_t frameID);

    // Query the driver for the actual Reflex sleep status.
    // Returns true if the driver reports low-latency mode is active.
    bool GetSleepStatus(bool& outLowLatency, bool& outBoost, uint32_t& outIntervalUs, bool& outGameSleep);

    // Configure Reflex low-latency mode.
    //   enable:  turn Reflex low-latency mode on/off.
    //   fpsCap:  target frame rate (e.g. 120.0). Pass 0 for unlocked.
    //   boost:   enable Reflex Boost (disables GPU power savings).
    //   optimize: allow Reflex to use latency markers for optimization.
    // Returns true on success.
    bool Configure(bool enable, float fpsCap, bool boost, bool optimize);

    // Submit a latency marker for the current frame.
    void SetLatencyMarker(NV_LATENCY_MARKER_TYPE type, uint64_t frameID);

    // Let the GPU/driver sleep until the optimal present time.
    // Should be called instead of or before a CPU busy-wait.
    // Returns true if sleep was handled by Reflex.
    bool Sleep();

    // Call once per frame at the very start of the frame logic.
    void BeginFrame(uint64_t frameID);

    // Call once per frame just before Present().
    void EndFrame(uint64_t frameID);

private:
    NvApiReflex() = default;
    ~NvApiReflex();
    NvApiReflex(const NvApiReflex&) = delete;
    NvApiReflex& operator=(const NvApiReflex&) = delete;

    // Dynamically resolve an NVAPI function by ordinal.
    template<typename T>
    T ResolveFunction(unsigned int ordinal, const char* name);

    // ── PCL Stats (ETW TraceLogging for NVIDIA App overlay) ─────────────
    void InitPclStats();
    void ShutdownPclStats();
    static DWORD WINAPI PclStatsPingThreadProc(LPVOID param);

    // The NVAPI function IDs (ordinals) — from NVIDIA public nvapi_interface.h
    // github.com/NVIDIA/nvapi
    static constexpr unsigned int ID_NvAPI_D3D_SetLatencyMarker = 0xd9984c05;
    static constexpr unsigned int ID_NvAPI_D3D_SetSleepMode     = 0xac1ca9e0;
    static constexpr unsigned int ID_NvAPI_D3D_Sleep            = 0x852cd1d2;
    static constexpr unsigned int ID_NvAPI_D3D_GetSleepStatus   = 0xaef96ca1;

    HMODULE  m_nvapiModule    = nullptr;
    IUnknown* m_device        = nullptr;
    bool     m_initialized    = false;
    bool     m_sleepModeSet   = false;
    uint64_t m_currentFrameID = 0;

    // Resolved NVAPI function pointers
    NvAPI_QueryInterface_t          m_NvAPI_QueryInterface          = nullptr;
    NvAPI_D3D_SetLatencyMarker_t   m_NvAPI_D3D_SetLatencyMarker   = nullptr;
    NvAPI_D3D_SetSleepMode_t       m_NvAPI_D3D_SetSleepMode       = nullptr;
    NvAPI_D3D_Sleep_t              m_NvAPI_D3D_Sleep              = nullptr;
    NvAPI_D3D_GetSleepStatus_t     m_NvAPI_D3D_GetSleepStatus     = nullptr;

    // ── PCL Stats state ────────────────────────────────────────────────
    // NVIDIA's ETW provider GUID for PC Latency Stats:
    //   {0x0d216f06, 0x82a6, 0x4d49, 0xbc, 0x4f, 0x8f, 0x38, 0xae, 0x56, 0xef, 0xab}
    // This is a well-known public constant used by all Reflex-enabled titles.
    HANDLE  m_pclQuitEvent    = nullptr;
    HANDLE  m_pclPingThread   = nullptr;
    UINT    m_pclWindowMsg    = 0;       // "PC_Latency_Stats_Ping" registered message
    DWORD   m_pclIdThread     = 0;       // thread ID for PCLStatsInput events
    bool    m_pclInitialized  = false;
    volatile LONG  m_pclPingSignal = 0;  // set by ping thread, cleared by main thread
};

// ── Global convenience accessor ────────────────────────────────────────────
inline NvApiReflex& GetNvApiReflex() { return NvApiReflex::Get(); }

// NvApiReflex.cpp — NVIDIA Reflex Integration via NVAPI
//
// Dynamically loads nvapi64.dll, resolves Reflex function pointers
// via nvapi_QueryInterface, and provides a clean C++ wrapper for
// latency markers, sleep mode, and Reflex-optimized frame pacing.
//
// Architecture note:
//   NVAPI is loaded dynamically (not linked) so the mod works on
//   non-NVIDIA hardware without missing symbol errors. All Reflex
//   calls are guarded — if NVAPI isn't available, they become no-ops.

#include "NvApiReflex.hpp"
#include "Core/Core.hpp"
#include "Core/Macros.h"
#include <windows.h>
#include <cstdio>
#include <TraceLoggingProvider.h>
#include <evntrace.h>

#pragma comment(lib, "advapi32.lib")

// ── Singleton ──────────────────────────────────────────────────────────────

NvApiReflex& NvApiReflex::Get() {
    static NvApiReflex instance;
    return instance;
}

NvApiReflex::~NvApiReflex() {
    Shutdown();
}

// ── PCL Stats ETW Provider ─────────────────────────────────────────────────
// NVIDIA's well-known PC Latency Stats TraceLogging provider GUID.
// The NVIDIA driver / NVIDIA App subscribes to events from this provider
// to display real-time average PC latency in the performance overlay.
//
// GUID: {0x0d216f06, 0x82a6, 0x4d49, 0xbc, 0x4f, 0x8f, 0x38, 0xae, 0x56, 0xef, 0xab}
// This is a public constant — the same GUID used by all Reflex-enabled titles.

TRACELOGGING_DEFINE_PROVIDER(
    g_hPclStatsProvider,
    "PCLStatsTraceLoggingProvider",
    (0x0d216f06, 0x82a6, 0x4d49, 0xbc, 0x4f, 0x8f, 0x38, 0xae, 0x56, 0xef, 0xab));

// ── Template: Resolve NVAPI function by ordinal ────────────────────────────

template<typename T>
T NvApiReflex::ResolveFunction(unsigned int ordinal, const char* name) {
    if (!m_NvAPI_QueryInterface) return nullptr;
    T func = reinterpret_cast<T>(m_NvAPI_QueryInterface(ordinal));
    if (func) {
        Log("%s Resolved NVAPI function '%s' (ordinal 0x%08X)", FUNC_NAME, name, ordinal);
    } else {
        Log("%s NVAPI function '%s' (ordinal 0x%08X) not available on this driver", FUNC_NAME, name, ordinal);
    }
    return func;
}

// ── Initialize ─────────────────────────────────────────────────────────────

bool NvApiReflex::Initialize(IUnknown* d3d11Device) {
    if (m_initialized) {
        Log("%s Already initialized", FUNC_NAME);
        return true;
    }

    if (!d3d11Device) {
        Log("%s No D3D11 device provided", FUNC_NAME);
        return false;
    }

    m_device = d3d11Device;

    // ── Load nvapi64.dll ───────────────────────────────────────────────
    m_nvapiModule = LoadLibraryW(L"nvapi64.dll");
    if (!m_nvapiModule) {
        Log("%s nvapi64.dll not found — NVIDIA driver not installed or not an NVIDIA GPU", FUNC_NAME);
        return false;
    }

    // Pin the module so it can't be unloaded while we hold function pointers.
    HMODULE pinned = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_PIN, L"nvapi64.dll", &pinned);

    // ── Resolve nvapi_QueryInterface ────────────────────────────────────
    m_NvAPI_QueryInterface = reinterpret_cast<NvAPI_QueryInterface_t>(
        GetProcAddress(m_nvapiModule, "nvapi_QueryInterface"));
    if (!m_NvAPI_QueryInterface) {
        Log("%s Failed to resolve nvapi_QueryInterface", FUNC_NAME);
        Shutdown();
        return false;
    }

    // ── Initialize NVAPI ────────────────────────────────────────────────
    // NvAPI_Initialize is ordinal 0x0150E828 (public)
    using NvAPI_Initialize_t = NvAPI_Status(__cdecl*)();
    auto NvAPI_Init = reinterpret_cast<NvAPI_Initialize_t>(
        m_NvAPI_QueryInterface(0x0150E828));
    if (!NvAPI_Init) {
        Log("%s Failed to resolve NvAPI_Initialize", FUNC_NAME);
        Shutdown();
        return false;
    }

    NvAPI_Status status = NvAPI_Init();
    if (status != NVAPI_OK) {
        Log("%s NvAPI_Initialize failed with status %d — no NVIDIA GPU or driver issue", FUNC_NAME, status);
        Shutdown();
        return false;
    }
    Log("%s NvAPI_Initialize succeeded", FUNC_NAME);

    // ── Resolve Reflex function pointers ────────────────────────────────
    m_NvAPI_D3D_SetLatencyMarker = ResolveFunction<NvAPI_D3D_SetLatencyMarker_t>(
        ID_NvAPI_D3D_SetLatencyMarker, "NvAPI_D3D_SetLatencyMarker");
    m_NvAPI_D3D_SetSleepMode = ResolveFunction<NvAPI_D3D_SetSleepMode_t>(
        ID_NvAPI_D3D_SetSleepMode, "NvAPI_D3D_SetSleepMode");
    m_NvAPI_D3D_Sleep = ResolveFunction<NvAPI_D3D_Sleep_t>(
        ID_NvAPI_D3D_Sleep, "NvAPI_D3D_Sleep");
    m_NvAPI_D3D_GetSleepStatus = ResolveFunction<NvAPI_D3D_GetSleepStatus_t>(
        ID_NvAPI_D3D_GetSleepStatus, "NvAPI_D3D_GetSleepStatus");

    if (!m_NvAPI_D3D_SetLatencyMarker || !m_NvAPI_D3D_SetSleepMode || !m_NvAPI_D3D_Sleep) {
        Log("%s One or more Reflex functions unavailable — Reflex low-latency will be disabled, but PCL Stats still active", FUNC_NAME);
        // Don't fail — NVAPI is still loaded, PCL Stats can work.
    }

    m_initialized = true;
    Log("%s NVIDIA Reflex successfully initialized", FUNC_NAME);

    // Initialize PC Latency Stats for NVIDIA App overlay reporting (always on)
    InitPclStats();

    return true;
}

// ── Shutdown ───────────────────────────────────────────────────────────────

void NvApiReflex::Shutdown() {
    ShutdownPclStats();

    m_initialized    = false;
    m_sleepModeSet   = false;
    m_device         = nullptr;
    m_currentFrameID = 0;

    m_NvAPI_D3D_SetLatencyMarker = nullptr;
    m_NvAPI_D3D_SetSleepMode     = nullptr;
    m_NvAPI_D3D_Sleep            = nullptr;
    m_NvAPI_D3D_GetSleepStatus   = nullptr;
    m_NvAPI_QueryInterface       = nullptr;

    // Don't call FreeLibrary — the module is pinned and shared with the driver.
    m_nvapiModule = nullptr;
}

// ── Configure ──────────────────────────────────────────────────────────────

bool NvApiReflex::Configure(bool enable, float fpsCap, bool boost, bool optimize) {
    if (!m_initialized) {
        Log("%s Configure failed: NVAPI not initialized", FUNC_NAME);
        return false;
    }
    if (!m_device) {
        Log("%s Configure failed: no device", FUNC_NAME);
        return false;
    }
    if (!m_NvAPI_D3D_SetSleepMode) {
        Log("%s Configure failed: NvAPI_D3D_SetSleepMode not resolved", FUNC_NAME);
        return false;
    }

    NV_SET_SLEEP_MODE_PARAMS params = {};
    params.version               = NVAPI_MAKE_VERSION(NV_SET_SLEEP_MODE_PARAMS, 1);
    params.bLowLatencyMode       = enable ? 1u : 0u;
    params.bLowLatencyBoost      = (enable && boost) ? 1u : 0u;
    params.bUseMarkersToOptimize = (enable && optimize) ? 1u : 0u;

    if (enable && fpsCap > 0.0f && fpsCap < 1000.0f) {
        params.minimumIntervalUs = static_cast<uint32_t>(1000000.0f / fpsCap);
    } else {
        params.minimumIntervalUs = 0;
    }

    NvAPI_Status status = m_NvAPI_D3D_SetSleepMode(m_device, &params);
    const char*  statusStr = (status == NVAPI_OK) ? "OK" :
                              (status == NVAPI_INCOMPATIBLE_STRUCT_VERSION) ? "INCOMPATIBLE_STRUCT_VERSION" :
                              (status == NVAPI_INVALID_ARGUMENT) ? "INVALID_ARGUMENT" :
                              (status == NVAPI_NO_IMPLEMENTATION) ? "NO_IMPLEMENTATION" :
                              (status == NVAPI_NOT_SUPPORTED) ? "NOT_SUPPORTED" : "UNKNOWN";

    Log("%s Reflex %s | LowLatency=%d Boost=%d Optimize=%d Interval=%uus Cap=%.1f | NvAPI: %s (%d)",
        FUNC_NAME,
        enable ? "ENABLED" : "DISABLED",
        params.bLowLatencyMode, params.bLowLatencyBoost, params.bUseMarkersToOptimize,
        params.minimumIntervalUs, fpsCap,
        statusStr, status);

    if (status == NVAPI_OK) {
        m_sleepModeSet = enable;
        return true;
    }

    return false;
}

// ── GetSleepStatus ───────────────────────────────────────────────────────

bool NvApiReflex::GetSleepStatus(bool& outLowLatency, bool& outBoost, uint32_t& outIntervalUs, bool& outGameSleep) {
    if (!m_initialized || !m_device || !m_NvAPI_D3D_GetSleepStatus)
        return false;

    NV_GET_SLEEP_STATUS_PARAMS statusParams = {};
    statusParams.version = NVAPI_MAKE_VERSION(NV_GET_SLEEP_STATUS_PARAMS, 1);

    NvAPI_Status status = m_NvAPI_D3D_GetSleepStatus(m_device, &statusParams);
    if (status == NVAPI_OK) {
        outLowLatency  = statusParams.bLowLatencyMode != 0;
        outBoost       = false;
        outIntervalUs  = statusParams.sleepIntervalUs;
        outGameSleep   = statusParams.bUseGameSleep != 0;
        Log("%s Driver reports: LowLatency=%d VRR=%d CPvsync=%d SleepInterval=%uus GameSleep=%d",
            FUNC_NAME,
            statusParams.bLowLatencyMode, statusParams.bFsVrr, statusParams.bCplVsyncOn,
            statusParams.sleepIntervalUs, statusParams.bUseGameSleep);
        return true;
    }
    Log("%s NvAPI_D3D_GetSleepStatus failed: %d", FUNC_NAME, status);
    return false;
}

// ── SetLatencyMarker ───────────────────────────────────────────────────────

void NvApiReflex::SetLatencyMarker(NV_LATENCY_MARKER_TYPE type, uint64_t frameID) {
    // Works whenever NVAPI is loaded (even without Reflex sleep mode).
    // This allows PCL Stats reporting regardless of Reflex toggle state.
    if (!m_initialized) return;
    if (!m_device) return;

    NV_LATENCY_MARKER_PARAMS params = {};
    params.version    = NVAPI_MAKE_VERSION(NV_LATENCY_MARKER_PARAMS, 1);
    params.frameID    = frameID;
    params.markerType = type;
    params.rsvd0      = 0;

    if (m_NvAPI_D3D_SetLatencyMarker) {
        m_NvAPI_D3D_SetLatencyMarker(m_device, &params);
    }

    // Always report to PCL Stats for NVIDIA App overlay
    PclStatsMarker(type, frameID);
}

// ── Sleep ──────────────────────────────────────────────────────────────────

bool NvApiReflex::Sleep() {
    // Only works when Reflex sleep mode is configured.
    if (!m_sleepModeSet) return false;
    if (!m_NvAPI_D3D_Sleep) return false;
    if (!m_device) return false;

    NvAPI_Status status = m_NvAPI_D3D_Sleep(m_device);
    return (status == NVAPI_OK);
}

// ── BeginFrame / EndFrame ──────────────────────────────────────────────────

void NvApiReflex::BeginFrame(uint64_t frameID) {
    // Works whenever NVAPI is loaded (markers + PCL Stats always active).
    if (!m_initialized) return;
    m_currentFrameID = frameID;
    SetLatencyMarker(SIMULATION_START, frameID);

    // Check if the PCL ping thread has signalled and send a PC_LATENCY_PING
    if (m_pclInitialized && InterlockedCompareExchange(&m_pclPingSignal, 0, 1)) {
        SetLatencyMarker(PC_LATENCY_PING, frameID);
    }
}

void NvApiReflex::EndFrame(uint64_t frameID) {
    if (!m_initialized) return;
    SetLatencyMarker(PRESENT_START, frameID);
    // PRESENT_END is set after the actual Present call in SwapChainWrapper
}

// ── PCL Stats: ETW TraceLogging for NVIDIA App / GFE overlay ──────────────

// ETW provider callback — the driver enables/disables us via ETW control codes.
static void WINAPI PclStatsProviderCallback(
    LPCGUID, ULONG ControlCode, UCHAR, ULONGLONG, ULONGLONG,
    PEVENT_FILTER_DESCRIPTOR, PVOID)
{
    // The driver sends EVENT_CONTROL_CODE_ENABLE_PROVIDER when the overlay
    // wants stats, and DISABLE_PROVIDER when it stops. We don't need to
    // track this explicitly — we always write events; the driver simply
    // ignores them when not subscribed.
}

void NvApiReflex::PclStatsMarker(NV_LATENCY_MARKER_TYPE type, uint64_t frameID) {
    if (!m_pclInitialized)
        return;

    // Write the latency marker via ETW. The NVIDIA driver subscribes to the
    // "PCLStatsEvent" event from our provider and extracts Marker + FrameID.
    TraceLoggingWrite(
        g_hPclStatsProvider,
        "PCLStatsEvent",
        TraceLoggingUInt32(static_cast<UINT32>(type), "Marker"),
        TraceLoggingUInt64(frameID, "FrameID"));
}

DWORD WINAPI NvApiReflex::PclStatsPingThreadProc(LPVOID param) {
    auto* self = static_cast<NvApiReflex*>(param);

    // Random ping interval between 100-300ms to avoid exact periodicity.
    const DWORD minPingMs = 100;
    const DWORD maxPingMs = 300;

    while (WaitForSingleObject(self->m_pclQuitEvent,
           minPingMs + (rand() % (maxPingMs - minPingMs))) == WAIT_TIMEOUT)
    {
        if (!self->m_pclInitialized)
            continue;

        // Send the PCLStatsInput event — tells the driver how to correlate
        // input events for latency measurement.
        // When m_pclIdThread is set (non-zero), we report the thread ID;
        // otherwise we report the registered window message.
        if (self->m_pclIdThread != 0) {
            TraceLoggingWrite(
                g_hPclStatsProvider,
                "PCLStatsInput",
                TraceLoggingUInt32(self->m_pclIdThread, "IdThread"));
        } else if (self->m_pclWindowMsg != 0) {
            TraceLoggingWrite(
                g_hPclStatsProvider,
                "PCLStatsInput",
                TraceLoggingUInt32(self->m_pclWindowMsg, "MsgId"));
        }

        // Signal the main thread to send a PC_LATENCY_PING marker.
        InterlockedExchange(&self->m_pclPingSignal, 1);
    }

    return 0;
}

void NvApiReflex::InitPclStats() {
    if (m_pclInitialized)
        return;

    // Register for the "PC_Latency_Stats_Ping" window message — the driver
    // uses this to request a ping/flash indicator from the application.
    if (m_pclWindowMsg == 0) {
        m_pclWindowMsg = RegisterWindowMessageW(L"PC_Latency_Stats_Ping");
    }

    // Set thread ID to -1 (DWORD max) — tells the driver to use the
    // calling thread for input event correlation.
    m_pclIdThread = (DWORD)-1;

    // Create quit event for the ping thread.
    m_pclQuitEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!m_pclQuitEvent)
        return;

    // Register the ETW provider. The driver enables it when the overlay
    // is active and disables it when the overlay closes.
    TraceLoggingRegisterEx(g_hPclStatsProvider, PclStatsProviderCallback, nullptr);

    // Write an init event — signals to the driver that we support PCL Stats.
    TraceLoggingWrite(g_hPclStatsProvider, "PCLStatsInit");

    // Start the ping thread — periodically wakes to signal a ping marker.
    m_pclPingThread = CreateThread(nullptr, 0, PclStatsPingThreadProc, this, 0, nullptr);

    m_pclInitialized = true;
    Log("%s PC Latency Stats initialized for NVIDIA App overlay", FUNC_NAME);
}

void NvApiReflex::ShutdownPclStats() {
    if (!m_pclInitialized)
        return;

    m_pclInitialized = false;

    // Signal the ping thread to quit and wait for it.
    if (m_pclQuitEvent) {
        SetEvent(m_pclQuitEvent);
    }
    if (m_pclPingThread) {
        WaitForSingleObject(m_pclPingThread, 1000);
        CloseHandle(m_pclPingThread);
        m_pclPingThread = nullptr;
    }

    // Write shutdown event and unregister the provider.
    TraceLoggingWrite(g_hPclStatsProvider, "PCLStatsShutdown");
    TraceLoggingUnregister(g_hPclStatsProvider);

    if (m_pclQuitEvent) {
        CloseHandle(m_pclQuitEvent);
        m_pclQuitEvent = nullptr;
    }

    Log("%s PC Latency Stats shutdown", FUNC_NAME);
}

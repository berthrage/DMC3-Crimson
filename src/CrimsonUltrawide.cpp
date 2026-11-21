#include "CrimsonUltrawide.hpp"

#include <windows.h>
#include <winternl.h>
#include <tlhelp32.h>

#include <algorithm>

#include "Core/Core.hpp"
#include "Core/Macros.h"
#include "CrimsonConfig.hpp"
#include "CrimsonDetours.hpp"
#include "CrimsonReversedCalls.hpp"

namespace CrimsonUltrawide {

namespace {

// RenderWidth_140C0B3FC, RenderHeight_140C0B3F8
constexpr uintptr_t RENDER_WIDTH_RVA  = 0xC0B3FC;
constexpr uintptr_t RENDER_HEIGHT_RVA = 0xC0B3F8;

// pRenderFrameState_140D6D300, +0x64 = horizontal projection scale (written by RenderFrameStateSetup_sub_140332F00)
constexpr uintptr_t RENDER_FRAME_STATE_RVA = 0xD6D300;
constexpr size_t FRAME_PROJECTION_SCALE    = 0x64;

// Camera2DSetup_sub_140325B90: dmc3.exe+325C0C - C7 41 40 0000803F - mov [rcx+40],3F800000 (2D projection x scale)
constexpr uintptr_t HUD_SCALE_IMM_RVA = 0x325C0F;
// sCWorkManager_140CF2520 + 0x400, built once by WorkManagerCameraInit_sub_140326410
constexpr uintptr_t CAMERA_2D_RVA     = 0xCF2920;
constexpr size_t CAMERA_PROJECTION    = 0x40;
constexpr size_t CAMERA_VIEW          = 0x100;
// CameraSetup_sub_140325D90 takes the debug camera path while this is set
constexpr uintptr_t DEBUG_CAMERA_FLAG_RVA = 0xCA8804;

// Frustum culling scale read by dmc3.exe+3254C8, shared with dmc3.exe+F8CD9. It's the horizontal / vertical plane
// ratio, sized for 4:3 plus a margin.
constexpr uintptr_t CULL_SCALE_RVA = 0x4D0B6C;

constexpr float ASPECT_4_3 = 4.0f / 3.0f;
constexpr float ASPECT_16_9 = 16.0f / 9.0f;
constexpr float ASPECT_EPSILON = 0.01f;

bool s_active = false;
float s_multiplier = 1.0f;
bool s_cullingActive = false;
float s_cullingScale = 0.0f;
bool s_standaloneChecked = false;
bool s_standaloneLoaded = false;
bool s_standaloneDisabled = false;

constexpr wchar_t STANDALONE_FIX_NAME[] = L"DMCHDFix.asi";

// LdrRegisterDllNotification (ntdll)
struct DllNotificationData {
	ULONG flags;
	const UNICODE_STRING* fullDllName;
	const UNICODE_STRING* baseDllName;
	void* dllBase;
	ULONG sizeOfImage;
};
using DllNotificationFn = void(CALLBACK*)(ULONG reason, const DllNotificationData* data, void* context);
using LdrRegisterDllNotificationFn = NTSTATUS(NTAPI*)(ULONG flags, DllNotificationFn callback, void* context, void** cookie);
using NtQueryInformationThreadFn = NTSTATUS(NTAPI*)(HANDLE thread, ULONG infoClass, void* info, ULONG length, ULONG* returnLength);
constexpr ULONG DLL_NOTIFICATION_LOADED = 1;
constexpr ULONG THREAD_QUERY_SET_WIN32_START_ADDRESS = 9;
void* s_dllNotificationCookie = nullptr;

// Replaces DMCHDFix's DllMain with "return TRUE", so it never starts the thread that does its patching.
void StubEntryPoint(byte8* base) {
	auto dos = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
	auto nt  = reinterpret_cast<IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
	auto entry = base + nt->OptionalHeader.AddressOfEntryPoint;
	constexpr byte8 returnTrue[] = {0xB8, 0x01, 0x00, 0x00, 0x00, 0xC3}; // mov eax,1; ret
	DWORD protection = 0;
	if (VirtualProtect(entry, sizeof(returnTrue), PAGE_EXECUTE_READWRITE, &protection)) {
		memcpy(entry, returnTrue, sizeof(returnTrue));
		VirtualProtect(entry, sizeof(returnTrue), protection, &protection);
		FlushInstructionCache(GetCurrentProcess(), entry, sizeof(returnTrue));
		s_standaloneDisabled = true;
	}
}

// Already loaded: we're still inside the loader lock, so the thread its DllMain created hasn't run yet.
void TerminateModuleThreads(byte8* base) {
	auto nt = reinterpret_cast<IMAGE_NT_HEADERS*>(base + reinterpret_cast<IMAGE_DOS_HEADER*>(base)->e_lfanew);
	const auto start = reinterpret_cast<uintptr_t>(base);
	const auto end   = start + nt->OptionalHeader.SizeOfImage;
	auto queryThread = reinterpret_cast<NtQueryInformationThreadFn>(GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationThread"));
	if (!queryThread) {
		return;
	}

	auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (snapshot == INVALID_HANDLE_VALUE) {
		return;
	}
	THREADENTRY32 entry = {};
	entry.dwSize = sizeof(entry);
	const auto processId = GetCurrentProcessId();
	for (auto ok = Thread32First(snapshot, &entry); ok; ok = Thread32Next(snapshot, &entry)) {
		if (entry.th32OwnerProcessID != processId || entry.th32ThreadID == GetCurrentThreadId()) {
			continue;
		}
		auto thread = OpenThread(THREAD_QUERY_INFORMATION | THREAD_TERMINATE, FALSE, entry.th32ThreadID);
		if (!thread) {
			continue;
		}
		uintptr_t threadStart = 0;
		if (queryThread(thread, THREAD_QUERY_SET_WIN32_START_ADDRESS, &threadStart, sizeof(threadStart), nullptr) >= 0 &&
			threadStart >= start && threadStart < end && TerminateThread(thread, 0)) {
			s_standaloneDisabled = true;
		}
		CloseHandle(thread);
	}
	CloseHandle(snapshot);
}

void CALLBACK OnDllNotification(ULONG reason, const DllNotificationData* data, void*) {
	if (reason != DLL_NOTIFICATION_LOADED || !data || !data->baseDllName || !data->baseDllName->Buffer) {
		return;
	}
	const auto& name = *data->baseDllName;
	constexpr size_t nameLength = (sizeof(STANDALONE_FIX_NAME) / sizeof(wchar_t)) - 1;
	if (name.Length / sizeof(wchar_t) != nameLength || _wcsnicmp(name.Buffer, STANDALONE_FIX_NAME, nameLength) != 0) {
		return;
	}
	StubEntryPoint(reinterpret_cast<byte8*>(data->dllBase));
}

uintptr_t FrameState() {
	const auto frameState = *reinterpret_cast<const uintptr_t*>(appBaseAddr + RENDER_FRAME_STATE_RVA);
	if (frameState <= 0x10000 || (frameState & 0xFFF0000000000000ull)) {
		return 0;
	}
	return frameState;
}

void ApplyHudScale(float inv) {
	Write<float>(appBaseAddr + HUD_SCALE_IMM_RVA, inv);

	// The 2D camera is only built at boot and on GameDataInit, rebuild its matrices for runtime changes.
	auto camera = appBaseAddr + CAMERA_2D_RVA;
	auto& projectionX = *reinterpret_cast<float*>(camera + CAMERA_PROJECTION);
	if (projectionX == 0.0f) {
		return;
	}
	projectionX = inv;
	if (*reinterpret_cast<uint8*>(appBaseAddr + DEBUG_CAMERA_FLAG_RVA) == 0) {
		CrimsonReversedCalls::CameraSetView_sub_140325810(camera, camera + CAMERA_VIEW);
	}
}

// CameraSetup_sub_140325D90 only runs when the camera is set up, so switching the fix on mid-mission has to write
// the value itself; the hook covers the setups that come after.
void ApplyCameraAspect(float multiplier) {
	using namespace CrimsonDetours;
	if (auto frameState = FrameState()) {
		auto& scale = *reinterpret_cast<float*>(frameState + FRAME_PROJECTION_SCALE);
		if (g_UltrawideCameraWritten == 0.0f || scale != g_UltrawideCameraWritten) {
			g_UltrawideCameraBase = scale;
		}
		scale = g_UltrawideCameraBase * multiplier;
		g_UltrawideCameraWritten = scale;
	}
}

void RestoreCameraAspect() {
	using namespace CrimsonDetours;
	if (auto frameState = FrameState()) {
		auto& scale = *reinterpret_cast<float*>(frameState + FRAME_PROJECTION_SCALE);
		if (g_UltrawideCameraWritten != 0.0f && scale == g_UltrawideCameraWritten) {
			scale = g_UltrawideCameraBase;
		}
	}
	g_UltrawideCameraWritten = 0.0f;
}

void UpdateCulling(float aspect) {
	using namespace CrimsonDetours;

	// The standalone DMCHDFix.asi hooks the same instruction.
	const bool active = activeCrimsonConfig.System.cullingFix && !s_standaloneLoaded;
	static const float cullBase = *reinterpret_cast<const float*>(appBaseAddr + CULL_SCALE_RVA);
	const float scale = cullBase * (std::max)(1.0f, aspect / ASPECT_4_3);
	if (active == s_cullingActive && scale == s_cullingScale) {
		return;
	}

	g_CullingFixScale = scale;
	ToggleCullingFix(active);
	Log("%s %s, scale %.4f", FUNC_NAME, active ? "on" : "off", scale);

	s_cullingActive = active;
	s_cullingScale = scale;
}

}

void DisableStandaloneFix() {
	if (auto module = GetModuleHandleW(STANDALONE_FIX_NAME)) {
		TerminateModuleThreads(reinterpret_cast<byte8*>(module));
		Log("%s DMCHDFix.asi was already loaded, %s", FUNC_NAME, s_standaloneDisabled ? "stopped it" : "its thread already ran");
		return;
	}

	auto registerNotification = reinterpret_cast<LdrRegisterDllNotificationFn>(
		GetProcAddress(GetModuleHandleA("ntdll.dll"), "LdrRegisterDllNotification"));
	if (!registerNotification || registerNotification(0, &OnDllNotification, nullptr, &s_dllNotificationCookie) < 0) {
		Log("%s LdrRegisterDllNotification failed", FUNC_NAME);
	}
}

void Update() {
	using namespace CrimsonDetours;

	if (!s_standaloneChecked) {
		s_standaloneChecked = true;
		s_standaloneLoaded = !s_standaloneDisabled && GetModuleHandleW(STANDALONE_FIX_NAME) != nullptr;
		if (s_standaloneDisabled) {
			Log("%s DMCHDFix.asi is disabled in dmc3, Crimson's fix replaces it", FUNC_NAME);
		} else if (s_standaloneLoaded) {
			Log("%s DMCHDFix.asi already patched dmc3, Crimson's ultrawide and culling fixes stay off", FUNC_NAME);
		}
	}

	const auto width  = *reinterpret_cast<const uint32*>(appBaseAddr + RENDER_WIDTH_RVA);
	const auto height = *reinterpret_cast<const uint32*>(appBaseAddr + RENDER_HEIGHT_RVA);
	float aspect = ASPECT_16_9;
	if (width > 0 && height > 0) {
		aspect = static_cast<float>(width) / static_cast<float>(height);
	}

	UpdateCulling(aspect);

	float multiplier = aspect / ASPECT_16_9;
	const bool active = activeCrimsonConfig.System.ultrawideFix && !s_standaloneLoaded && multiplier > 1.0f + ASPECT_EPSILON;
	if (!active) {
		multiplier = 1.0f;
	}
	if (active == s_active && multiplier == s_multiplier) {
		if (s_active) {
			ApplyCameraAspect(s_multiplier);
		}
		return;
	}

	g_UltrawideAspect    = multiplier;
	g_UltrawideInvAspect = 1.0f / multiplier;

	if (active) {
		ApplyCameraAspect(multiplier);
		ToggleUltrawideFix(true);
	} else {
		ToggleUltrawideFix(false);
		RestoreCameraAspect();
	}
	ApplyHudScale(g_UltrawideInvAspect);

	Log("%s %s, render %ux%u multiplier %.4f", FUNC_NAME, active ? "on" : "off", width, height, multiplier);

	s_active = active;
	s_multiplier = multiplier;
}

bool Active() {
	return s_active;
}

float Multiplier() {
	return s_multiplier;
}

bool StandaloneFixLoaded() {
	return s_standaloneLoaded;
}

}

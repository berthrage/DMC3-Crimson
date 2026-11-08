#include "CrimsonSplitscreen.hpp"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include <d3d11.h>
#include <wrl/client.h>

#include "../Core/Core.hpp"
#include "ImGui/imgui.h"
#include "Global.hpp"
#include "Utility/Detour.hpp"
#include "Config.hpp"
#include "HUD.hpp"
#include "Vars.hpp"
#include "CrimsonEnemyAITarget.hpp"

// Splitscreen works by multiplexing in time: each frame is a normal single-camera
// frame, built for one player, and stored. The most recent frame of every player is
// composited after the scene is drawn: 2 players as side-by-side centre crops, 3-4 players
// as quadrants (the frame downscaled to half size; a 3rd-player layout leaves the fourth blank).

namespace {

struct RenderTarget {
	char pad0[0x28];
	ID3D11Texture2D* texture; // 0x28
	char pad1[0x78];
};
static_assert(sizeof(RenderTarget) == 0xA8);

struct Render {
	char pad0[8];
	ID3D11Device* device;               // 0x08
	ID3D11DeviceContext* deviceContext; // 0x10
	IDXGISwapChain* swapChain;          // 0x18
	RenderTarget renderTargets[28];     // 0x20
};

constexpr uintptr_t RENDER_OFFSET           = 0xC0B410;
constexpr uintptr_t CAMERA_CTRL_PTR_OFFSET  = 0xC8FBD0;
constexpr uintptr_t SCENE_STATE_PTR_OFFSET  = 0xCF2330;
constexpr uintptr_t SUBMIT_DRAW_LIST_OFFSET = 0x32D8C0;
constexpr uintptr_t UPDATE_PLAYER_INPUT_OFFSET = 0x1EBCF0;
constexpr uintptr_t UPDATE_MIST_OFFSET         = 0x31AE10;
constexpr size_t SCENE_CAMERA_MATRIX = 0x100; // pSceneStateCursor_140CF2330: 4x4 view matrix of the current tick
constexpr size_t SCENE_CAMERA_MATRIX_SIZE = 0x40;
constexpr size_t SCENE_PROJECTION_MATRIX = 0x140; // 4x4 matrix the HUD projects world points with (session +0x160)
constexpr uintptr_t PLAYER_POOL_OFFSET         = 0xC90E28;
constexpr uintptr_t EVENT_POOL_OFFSET          = 0xC90E10;
constexpr uintptr_t COCKPIT_PTR_OFFSET         = 0xCF2680; // the game's CUIDCockpit00 object (HUD.cpp draws it)
constexpr uintptr_t COCKPIT_TOP_UPDATE_OFFSET  = 0x27C0F0;
constexpr uintptr_t LOCKON_CURSOR_UPDATE_OFFSET = 0x296DB0; // CUIDLockOnCursorUpdate_sub_140296DB0 (vtable 0x1404E9CE0 slot 0)
constexpr uintptr_t UPDATE_CAMERA_ORBIT_OFFSET = 0x57520;
constexpr uintptr_t UPDATE_CAMERA_BOSS_OFFSET  = 0x50920;
constexpr uintptr_t UPDATE_CAMERA_CTRL_OFFSET  = 0x55B90;
constexpr uintptr_t BOSS_CAMERA_MODE_OFFSET    = 0xC91830; // int8: 0 off, 1..0xE boss camera type, 0x80 = ending
constexpr uintptr_t PAD_BLOCK_OFFSET           = 0xD54A10 + 0x550; // ENGINE_GAMEPAD[PLAYER_COUNT]
static_assert(sizeof(ENGINE_GAMEPAD) == 0x2C);

// CUIDCockpit00 per-frame state that follows the element ModelData array (HP/DT smoothing, alpha timers).
constexpr size_t COCKPIT_STATE_BEGIN = 0x6880;
constexpr size_t COCKPIT_STATE_END   = 0x7040;
constexpr size_t COCKPIT_TOP_LEFT_ALPHA = 0x6920; // fade (0..127) of the top-left group: HP / DT / style icon / flux
// CUIDLockOnCursor: locked target (+0x800), then the marker target lists and their scale animation. The lock-on
// display toggle timer after them (+0x1188 / +0x118C) belongs to pad 0 and stays shared.
constexpr size_t LOCKON_TARGET_PTR  = 0x800;
constexpr size_t LOCKON_STATE_BEGIN = 0x10C0;
constexpr size_t LOCKON_STATE_END   = 0x1188;

// Per-layout HUD tuning (the cockpit draw hooks live in HUD.cpp and read the layout in use through
// CrimsonSplitscreen::HudLayoutInUse). Each group's elements are drawn at position * posScale + offset, with the
// model scaled by sizeScale. Tuned live from the "Splitscreen HUD" window (CrimsonSplitscreen::HudTuningWindow).
struct HudTuning {
	HudGroupTuning topLeft;  // HP / DT / style icon / flux
	HudGroupTuning topRight; // red orb counter group
	HudGroupTuning bottom;   // weapon icons
	bool showGuides;
};

enum HudLayout { HUD_LAYOUT_HALVES = 0, HUD_LAYOUT_QUADS = 1, HUD_LAYOUT_COUNT };

// Halves show a centre crop of the frame, so the HUD is pulled in to its edges.
constexpr HudTuning HUD_TUNING_HALVES = {
	{ 0.500f, 0.500f, 6.0f, 93.5f },
	{ 0.500f, 0.500f, 39.0f, -112.0f },
	{ 0.500f, 0.500f, 0.0f, 112.0f },
	false,
};
// Quadrants show the whole downscaled frame, so only the top-left group needs a nudge.
constexpr HudTuning HUD_TUNING_QUADS = {
	{ 1.000f, 1.000f, -16.0f, -2.0f },
	{ 1.000f, 1.000f, 0.0f, 0.0f },
	{ 1.000f, 1.000f, 0.0f, 0.0f },
	false,
};
HudTuning s_hudTuning[HUD_LAYOUT_COUNT] = { HUD_TUNING_HALVES, HUD_TUNING_QUADS };

// General adjustment (scale / x / y, see HUD.hpp) per layout, applied on top of that layout's HudTuning.topLeft in
// frame units (the frame is cropped / downscaled into the views afterwards).
constexpr HudGeneral HUD_GENERAL_HALVES = { 1.0f, 0.0f, 0.0f };
constexpr HudGeneral HUD_GENERAL_QUADS  = { 1.0f, 0.0f, 0.0f };
constexpr HudPreset HUD_PRESETS_HALVES[] = {
	{ "Default", { 1.000f, 0.0f, 0.0f } },
};
constexpr HudPreset HUD_PRESETS_QUADS[] = {
	{ "Default", { 1.000f, 0.0f, 0.0f } },
};
HudGeneral s_splitHudGeneral[HUD_LAYOUT_COUNT] = { HUD_GENERAL_HALVES, HUD_GENERAL_QUADS };

float s_frameAspect = 16.0f / 9.0f;

constexpr size_t CAM_EYE_OFFSET = 0x70; // eye at 0x70, lookat at 0x80

// Camera state per player: the camera control object (the final eye/lookat), the CCameraPlayer it owns
// (third person camera, lock-on camera is one of its modes) and the CCameraBoss (boss camera).
constexpr size_t CAM_REGION_COUNT = 3;
constexpr size_t CAM_REGION_SIZE[CAM_REGION_COUNT] = { 0x200, 0x200, 0x2C0 };
constexpr size_t CAM_MAX_REGION_SIZE = 0x2C0;
constexpr size_t CAM_CTRL_PLAYER_CAMERA = 0x4A0; // CCameraCtrl: CCameraPlayer*
constexpr size_t CAM_CTRL_BOSS_CAMERA = 0x4A8;   // CCameraCtrl: CCameraBoss*
constexpr size_t CAM_CTRL_OVERRIDE_CAMERA = 0x4B0; // CCameraCtrl: fixed camera in use when set
constexpr size_t CAM_FOLLOW_TARGET = 0xB0;       // CCameraPlayer: actor the camera follows
constexpr size_t CAM_BOSS_STATE = 0x2B7;         // CCameraBoss: 0 = init, 1 = running, 2 = ending

// The fixed camera side of CCameraCtrl: the selected camera pointers (0x498 current / 0x4A0 player / 0x4A8 boss /
// 0x4B0 override), the storage the rail/pan/relative camera objects are built in, and the current/previous camera
// index with its transition blend. The room picks the fixed camera from the zone the player stands in, so each
// player needs their own copy of this or every view shows the one camera chosen for player 1.
constexpr size_t CAM_CTRL_FIXED      = 0x498;
constexpr size_t CAM_CTRL_FIXED_SIZE = 0x2728 - CAM_CTRL_FIXED;
constexpr size_t CAM_CTRL_CAM_INDEX  = 0x26F1; // CCameraCtrl: currentCamIndex, 0xFF = no room camera

// Cells are half the frame wide; half tall too in the quadrant layout.
struct Cells {
	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex[PLAYER_COUNT];
	Microsoft::WRL::ComPtr<ID3D11Texture2D> blank;
	// Frame copy with a mip chain; mip 1 is the half-size frame.
	Microsoft::WRL::ComPtr<ID3D11Texture2D> mipSource;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mipView;
	bool valid[PLAYER_COUNT] = {};
	UINT frameWidth = 0;
	UINT frameHeight = 0;
	DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
	bool quads = false;
};
Cells s_cells;
int s_lastViewCount = 0;

// Saved camera state per player, see CAM_REGION_SIZE.
struct CameraSlots {
	uint8_t state[PLAYER_COUNT][CAM_REGION_COUNT][CAM_MAX_REGION_SIZE] = {};
	uint8_t fixed[PLAYER_COUNT][CAM_CTRL_FIXED_SIZE] = {};
	bool valid[PLAYER_COUNT] = {};
};
CameraSlots s_camSlots;

bool CameraObjectValid(const uint8_t* p) {
	const auto address = reinterpret_cast<uintptr_t>(p);
	return address > 0x10000 && !(address & 0xFFF0000000000000ull);
}

void CameraRegions(uint8_t* cameraControl, uint8_t* regions[CAM_REGION_COUNT]) {
	regions[0] = cameraControl;
	regions[1] = cameraControl ? *reinterpret_cast<uint8_t**>(cameraControl + CAM_CTRL_PLAYER_CAMERA) : nullptr;
	regions[2] = cameraControl ? *reinterpret_cast<uint8_t**>(cameraControl + CAM_CTRL_BOSS_CAMERA) : nullptr;
	for (int r = 1; r < CAM_REGION_COUNT; ++r) {
		if (!CameraObjectValid(regions[r])) {
			regions[r] = nullptr;
		}
	}
}

typedef __int64(__fastcall* SubmitDrawListFn)(int);
typedef __int64(__fastcall* UpdatePlayerInputFn)(__int64);
typedef void(__fastcall* UpdateMistFn)(__int64, uint8_t*);
typedef void(__fastcall* CockpitFn)(uint8_t*);
typedef __int64(__fastcall* UpdateCameraFn)(__int64);
typedef __int64(__fastcall* UpdateCameraCtrlFn)(uint8_t*);
std::unique_ptr<Utility::Detour_t> s_submitDrawListHook;
std::unique_ptr<Utility::Detour_t> s_updatePlayerInputHook;
std::unique_ptr<Utility::Detour_t> s_updateMistHook;
std::unique_ptr<Utility::Detour_t> s_cockpitUpdateHook;
std::unique_ptr<Utility::Detour_t> s_lockOnCursorUpdateHook;
std::unique_ptr<Utility::Detour_t> s_updateCameraOrbitHook;
std::unique_ptr<Utility::Detour_t> s_updateCameraBossHook;
std::unique_ptr<Utility::Detour_t> s_updateCameraCtrlHook;

// g_viewedCameraPlayer is advanced right after every drawn frame, so while a
// frame is being drawn it still holds the player that frame was built for.
int FramePlayer() {
	return (g_viewedCameraPlayer < CrimsonSplitscreen::ViewCount()) ? g_viewedCameraPlayer : 0;
}

bool SplitscreenActive() {
	return CrimsonSplitscreen::ViewCount() >= 2;
}

uint8_t* PlayerMainActor(int player);

int ActiveHudLayout() {
	return (CrimsonSplitscreen::ViewCount() > 2) ? HUD_LAYOUT_QUADS : HUD_LAYOUT_HALVES;
}

const HudTuning& ActiveHudTuning() {
	return s_hudTuning[ActiveHudLayout()];
}

// Camera matrix of player 1's last tick, used by the mist blending effect (see UpdateMistHook).
uint8_t s_mistCameraMatrix[SCENE_CAMERA_MATRIX_SIZE] = {};
bool s_mistCameraMatrixValid = false;

// The HUD projection matrix each player's last completed frame ended with (see LockOnCursorUpdateHook).
struct ProjectionSlots {
	uint8_t matrix[PLAYER_COUNT][SCENE_CAMERA_MATRIX_SIZE] = {};
	bool valid[PLAYER_COUNT] = {};
};
ProjectionSlots s_projectionSlots;

// Last room camera index logged per player, see LogCameraIndex.
int s_lastCameraIndex[PLAYER_COUNT] = { -1, -1, -1, -1 };

void ResetSplitState() {
	s_mistCameraMatrixValid = false;
	for (int p = 0; p < PLAYER_COUNT; ++p) {
		s_camSlots.valid[p]        = false;
		s_cells.valid[p]           = false;
		g_splitscreenCamCopyValid[p] = false;
		s_projectionSlots.valid[p] = false;
		s_lastCameraIndex[p]       = -1;
	}
}

// After a room change, load or cutscene the game rebuilds the camera and HUD objects over a few
// frames. Splitscreen stays off for this long so none of our saved per-player state is written
// over half-initialised objects.
constexpr double SETTLE_SECONDS = 0.05;
bool s_settling = true;
bool s_wasOpen = false;
uint32_t s_lastRoom = UINT32_MAX;
uint8_t* s_lastCameraPtrs[2] = {};
std::chrono::steady_clock::time_point s_settleUntil;

void StartSettling(double seconds = SETTLE_SECONDS) {
	s_settling = true;
	s_settleUntil = std::chrono::steady_clock::now() + std::chrono::milliseconds(static_cast<int>(seconds * 1000.0));
}

uint32_t CurrentRoom() {
	auto pool = *reinterpret_cast<uint8_t***>(appBaseAddr + EVENT_POOL_OFFSET);
	if (!pool || !pool[8]) {
		return UINT32_MAX;
	}
	return reinterpret_cast<EventData*>(pool[8])->room;
}

// Runs once per drawn frame; returns true when splitscreen can run this frame.
bool UpdateSplitGate(uint8_t* cameraControl) {
	static bool s_loggedOpen = false;
	if (!CrimsonSplitscreen::GateOpen()) {
		if (s_loggedOpen) {
			s_loggedOpen = false;
			Log("Splitscreen gate closed (room %u)", CurrentRoom());
		}
		ResetSplitState();
		s_lastViewCount = 0;
		s_wasOpen = false;
		StartSettling();
		return false;
	}

	uint8_t* regions[CAM_REGION_COUNT];
	CameraRegions(cameraControl, regions);
	// The player camera object is the room's own; the current camera pointer also switches to the boss camera.
	uint8_t* cameraPtrs[2] = { regions[0], regions[1] };
	const uint32_t room = CurrentRoom();
	const bool changed = !s_wasOpen || room != s_lastRoom ||
		cameraPtrs[0] != s_lastCameraPtrs[0] || cameraPtrs[1] != s_lastCameraPtrs[1];
	if (changed) {
		Log("Splitscreen reset: opened %d, room %u -> %u, camera %p %p -> %p %p", !s_wasOpen ? 1 : 0, s_lastRoom, room,
			s_lastCameraPtrs[0], s_lastCameraPtrs[1], cameraPtrs[0], cameraPtrs[1]);
	}
	s_loggedOpen = true;
	s_wasOpen = true;
	s_lastRoom = room;
	s_lastCameraPtrs[0] = cameraPtrs[0];
	s_lastCameraPtrs[1] = cameraPtrs[1];
	if (changed) {
		ResetSplitState();
		s_lastViewCount = 0;
		StartSettling();
	} else if (s_settling && std::chrono::steady_clock::now() >= s_settleUntil) {
		s_settling = false;
	}
	return !s_settling;
}

// A camera that is not a number, or is far from its own player, shows the void (black view) and
// nothing in the per-player cycle brings it back. Detect it and recover like switching the camera mode does:
// drop the saved state and let the game's own camera re-converge on the player.
constexpr float CAM_MAX_PLAYER_DISTANCE = 4000.0f;
constexpr int   CAM_BROKEN_FRAMES       = 12;
constexpr double CAM_RECOVER_SETTLE_SECONDS = 1.0;

bool CameraLooksBroken(int player, const uint8_t* cameraControlState) {
	const float* eye    = reinterpret_cast<const float*>(cameraControlState + CAM_EYE_OFFSET);
	const float* lookat = eye + 4;
	for (int i = 0; i < 3; ++i) {
		if (!std::isfinite(eye[i]) || !std::isfinite(lookat[i])) {
			return true;
		}
	}
	// A room camera sits wherever the room put it, so only the player camera is held to the distance limit.
	if (*reinterpret_cast<const uintptr_t*>(s_camSlots.fixed[player] + (CAM_CTRL_OVERRIDE_CAMERA - CAM_CTRL_FIXED)) != 0) {
		return false;
	}
	const uint8_t* actor = PlayerMainActor(player);
	if (!actor) {
		return false;
	}
	const auto& position = reinterpret_cast<const PlayerActorData*>(actor)->position;
	const float dx = eye[0] - position.x;
	const float dy = eye[1] - position.y;
	const float dz = eye[2] - position.z;
	return (dx * dx + dy * dy + dz * dz) > CAM_MAX_PLAYER_DISTANCE * CAM_MAX_PLAYER_DISTANCE;
}

// Returns true when it reset the splitscreen state.
bool RecoverBrokenCamera(int player, const uint8_t* cameraControlState) {
	static int s_brokenFrames[PLAYER_COUNT] = {};
	static std::chrono::steady_clock::time_point s_recoverAllowedAt;
	if (!CameraLooksBroken(player, cameraControlState)) {
		s_brokenFrames[player] = 0;
		return false;
	}
	const auto now = std::chrono::steady_clock::now();
	if (++s_brokenFrames[player] < CAM_BROKEN_FRAMES || now < s_recoverAllowedAt) {
		return false;
	}
	const float* eye = reinterpret_cast<const float*>(cameraControlState + CAM_EYE_OFFSET);
	Log("Splitscreen: camera of player %d is broken (eye %.1f %.1f %.1f), resetting", player, eye[0], eye[1], eye[2]);
	for (int& frames : s_brokenFrames) {
		frames = 0;
	}
	s_recoverAllowedAt = now + std::chrono::seconds(3);
	ResetSplitState();
	s_lastViewCount = 0;
	StartSettling(CAM_RECOVER_SETTLE_SECONDS);
	return true;
}

// The boss camera is sticky once it has ended (state 2), so every player's copy restarts when a new boss camera begins.
void RestartBossCameraOnStart(uint8_t* bossCamera) {
	static bool s_bossCameraWasActive = false;
	const bool active = *reinterpret_cast<int8_t*>(appBaseAddr + BOSS_CAMERA_MODE_OFFSET) > 0;
	if (active && !s_bossCameraWasActive) {
		for (int p = 0; p < PLAYER_COUNT; ++p) {
			s_camSlots.state[p][2][CAM_BOSS_STATE] = 0;
		}
		if (bossCamera) {
			bossCamera[CAM_BOSS_STATE] = 0;
		}
	}
	s_bossCameraWasActive = active;
}

// Logs the room camera each player ends a frame with, so a fixed camera room can be checked per view.
void LogCameraIndex(int player) {
	const int index = s_camSlots.fixed[player][CAM_CTRL_CAM_INDEX - CAM_CTRL_FIXED];
	if (s_lastCameraIndex[player] != index) {
		s_lastCameraIndex[player] = index;
		Log("Splitscreen: player %d camera index %d", player, index);
	}
}

void SwapCameraState(uint8_t* cameraControl, int framePlayer, int viewCount) {
	uint8_t* regions[CAM_REGION_COUNT];
	CameraRegions(cameraControl, regions);
	RestartBossCameraOnStart(regions[2]);

	for (int r = 0; r < CAM_REGION_COUNT; ++r) {
		if (regions[r]) {
			std::memcpy(s_camSlots.state[framePlayer][r], regions[r], CAM_REGION_SIZE[r]);
		}
	}
	std::memcpy(s_camSlots.fixed[framePlayer], cameraControl + CAM_CTRL_FIXED, CAM_CTRL_FIXED_SIZE);
	s_camSlots.valid[framePlayer] = true;
	LogCameraIndex(framePlayer);
	std::memcpy(g_splitscreenCamCopy[framePlayer], s_camSlots.state[framePlayer][0], SPLITSCREEN_CAM_COPY_SIZE);
	g_splitscreenCamCopyValid[framePlayer] = true;

	if (RecoverBrokenCamera(framePlayer, s_camSlots.state[framePlayer][0])) {
		return;
	}

	const int next = (framePlayer + 1) % viewCount;
	if (!s_camSlots.valid[next]) {
		// First use: copy the other camera and shift it onto this player.
		const float dx = g_splitscreenPlayerPos[next][0] - g_splitscreenPlayerPos[framePlayer][0];
		const float dy = g_splitscreenPlayerPos[next][1] - g_splitscreenPlayerPos[framePlayer][1];
		const float dz = g_splitscreenPlayerPos[next][2] - g_splitscreenPlayerPos[framePlayer][2];
		for (int r = 0; r < CAM_REGION_COUNT; ++r) {
			std::memcpy(s_camSlots.state[next][r], s_camSlots.state[framePlayer][r], CAM_REGION_SIZE[r]);
			auto* eyeAndLookAt = reinterpret_cast<float*>(s_camSlots.state[next][r] + CAM_EYE_OFFSET);
			for (int v = 0; v < 2; ++v) {
				eyeAndLookAt[v * 4 + 0] += dx;
				eyeAndLookAt[v * 4 + 1] += dy;
				eyeAndLookAt[v * 4 + 2] += dz;
			}
		}
		// Copied as-is: the first tick of that player re-selects its camera from its own zones.
		std::memcpy(s_camSlots.fixed[next], s_camSlots.fixed[framePlayer], CAM_CTRL_FIXED_SIZE);
		s_camSlots.valid[next] = true;
	}

	for (int r = 0; r < CAM_REGION_COUNT; ++r) {
		if (regions[r]) {
			std::memcpy(regions[r], s_camSlots.state[next][r], CAM_REGION_SIZE[r]);
		}
	}
	std::memcpy(cameraControl + CAM_CTRL_FIXED, s_camSlots.fixed[next], CAM_CTRL_FIXED_SIZE);
	g_viewedCameraPlayer = static_cast<uint8>(next);
	g_customCameraPos[0] = g_splitscreenPlayerPos[next][0];
	g_customCameraPos[1] = g_splitscreenPlayerPos[next][1];
	g_customCameraPos[2] = g_splitscreenPlayerPos[next][2];
	g_customCameraPos[3] = 1.0f;
}

DXGI_FORMAT TypedFormat(DXGI_FORMAT format) {
	switch (format) {
	case DXGI_FORMAT_R8G8B8A8_TYPELESS: return DXGI_FORMAT_R8G8B8A8_UNORM;
	case DXGI_FORMAT_B8G8R8A8_TYPELESS: return DXGI_FORMAT_B8G8R8A8_UNORM;
	case DXGI_FORMAT_B8G8R8X8_TYPELESS: return DXGI_FORMAT_B8G8R8X8_UNORM;
	default: return format;
	}
}

bool CreateCells(Render* render, const D3D11_TEXTURE2D_DESC& frameDesc, bool quads) {
	s_cells = Cells{};
	auto* device = render->device;
	const UINT cellW = frameDesc.Width / 2;
	const UINT cellH = quads ? frameDesc.Height / 2 : frameDesc.Height;

	D3D11_TEXTURE2D_DESC cellDesc{};
	cellDesc.Width            = cellW;
	cellDesc.Height           = cellH;
	cellDesc.MipLevels        = 1;
	cellDesc.ArraySize        = 1;
	cellDesc.Format           = frameDesc.Format;
	cellDesc.SampleDesc.Count = 1;
	cellDesc.Usage            = D3D11_USAGE_DEFAULT;
	for (int i = 0; i < (quads ? PLAYER_COUNT : 2); ++i) {
		if (FAILED(device->CreateTexture2D(&cellDesc, nullptr, s_cells.tex[i].GetAddressOf()))) {
			return false;
		}
	}

	if (quads) {
		// 16 bytes per pixel covers every format the frame could use.
		const UINT pitch = cellW * 16;
		std::vector<uint8_t> zeros(static_cast<size_t>(pitch) * cellH);
		D3D11_SUBRESOURCE_DATA init{ zeros.data(), pitch, 0 };
		if (FAILED(device->CreateTexture2D(&cellDesc, &init, s_cells.blank.GetAddressOf()))) {
			return false;
		}

		// Mip 1 of the frame is a filtered half-size copy; without it fall back to a centre crop.
		D3D11_TEXTURE2D_DESC mipDesc = frameDesc;
		mipDesc.MipLevels      = 2;
		mipDesc.ArraySize      = 1;
		mipDesc.Usage          = D3D11_USAGE_DEFAULT;
		mipDesc.BindFlags      = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		mipDesc.CPUAccessFlags = 0;
		mipDesc.MiscFlags      = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc{};
		viewDesc.Format                    = TypedFormat(frameDesc.Format);
		viewDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MostDetailedMip = 0;
		viewDesc.Texture2D.MipLevels       = 2;
		g_splitscreenQuadScaled =
			SUCCEEDED(device->CreateTexture2D(&mipDesc, nullptr, s_cells.mipSource.GetAddressOf())) &&
			SUCCEEDED(device->CreateShaderResourceView(s_cells.mipSource.Get(), &viewDesc, s_cells.mipView.GetAddressOf()));
		if (!g_splitscreenQuadScaled) {
			s_cells.mipSource.Reset();
			s_cells.mipView.Reset();
		}
	}

	s_cells.frameWidth  = frameDesc.Width;
	s_cells.frameHeight = frameDesc.Height;
	s_cells.format      = frameDesc.Format;
	s_cells.quads       = quads;
	return true;
}

void Composite(Render* render, int player, int viewCount) {
	ID3D11Texture2D* frame = render->renderTargets[0].texture;
	D3D11_TEXTURE2D_DESC desc{};
	frame->GetDesc(&desc);
	if (desc.SampleDesc.Count != 1 || desc.Width < 8 || desc.Height < 8) {
		return;
	}
	s_frameAspect = static_cast<float>(desc.Width) / static_cast<float>(desc.Height);

	const bool quads = viewCount > 2;
	if (!s_cells.tex[0] || s_cells.frameWidth != desc.Width || s_cells.frameHeight != desc.Height ||
		s_cells.format != desc.Format || s_cells.quads != quads) {
		if (!CreateCells(render, desc, quads)) {
			s_cells.frameWidth = 0;
			return;
		}
	}

	auto* ctx = render->deviceContext;
	const UINT cellW = desc.Width / 2;
	const UINT cellH = quads ? desc.Height / 2 : desc.Height;

	if (quads && s_cells.mipSource) {
		ctx->CopySubresourceRegion(s_cells.mipSource.Get(), 0, 0, 0, 0, frame, 0, nullptr);
		ctx->GenerateMips(s_cells.mipView.Get());
		ctx->CopySubresourceRegion(s_cells.tex[player].Get(), 0, 0, 0, 0, s_cells.mipSource.Get(), 1, nullptr);
	} else {
		// Cropping the centre keeps the aspect ratio.
		const UINT left = desc.Width / 4;
		const UINT top  = quads ? desc.Height / 4 : 0;
		const D3D11_BOX centre = { left, top, 0, left + cellW, top + cellH, 1 };
		ctx->CopySubresourceRegion(s_cells.tex[player].Get(), 0, 0, 0, 0, frame, 0, &centre);
	}
	s_cells.valid[player] = true;

	for (int i = 0; i < (quads ? PLAYER_COUNT : 2); ++i) {
		ID3D11Texture2D* src = nullptr;
		if (i < viewCount) {
			src = s_cells.valid[i] ? s_cells.tex[i].Get() : nullptr;
		} else {
			src = s_cells.blank.Get(); // unused quadrant
		}
		if (src) {
			ctx->CopySubresourceRegion(frame, 0, static_cast<UINT>(i & 1) * cellW, static_cast<UINT>(i >> 1) * cellH, 0, src, 0, nullptr);
		}
	}
}

// SubmitDrawList_sub_14032D8C0
__int64 __fastcall SubmitDrawListHook(int a1) {
	const auto original = s_submitDrawListHook->GetTrampoline<SubmitDrawListFn>();
	const __int64 result = original(a1);

	auto* render = reinterpret_cast<Render*>(appBaseAddr + RENDER_OFFSET);
	auto* cameraCtrl = *reinterpret_cast<uint8_t**>(appBaseAddr + CAMERA_CTRL_PTR_OFFSET);
	const bool ready = UpdateSplitGate(cameraCtrl);
	const int viewCount = ready ? CrimsonSplitscreen::ViewCount() : 0;
	if (viewCount < 2) {
		if (s_lastViewCount != 0) {
			ResetSplitState();
		}
		s_lastViewCount = 0;
	} else if (render->device && render->deviceContext && render->renderTargets[0].texture && cameraCtrl) {
		if (viewCount != s_lastViewCount) {
			ResetSplitState();
			s_lastViewCount = viewCount;
		}
		const int player = FramePlayer();
		// The matrix the frame just finished with is the HUD projection of that player's camera.
		if (const auto scene = *reinterpret_cast<uint8_t**>(appBaseAddr + SCENE_STATE_PTR_OFFSET)) {
			std::memcpy(s_projectionSlots.matrix[player], scene + SCENE_PROJECTION_MATRIX, SCENE_CAMERA_MATRIX_SIZE);
			s_projectionSlots.valid[player] = true;
		}
		Composite(render, player, viewCount);
		SwapCameraState(cameraCtrl, player, viewCount);
	}
	return result;
}

// UpdateBlendEffect_Mist_sub_14031AE10 scrolls the mist by how far the previous tick's camera point
// moved in the current camera's space. The camera alternates between players every tick, so it
// would see the gap between two cameras each tick and the mist would race. Every player's tick
// therefore uses player 1's camera matrix, which keeps the mist stepping like a single camera.
void __fastcall UpdateMistHook(__int64 effectSystem, uint8_t* effect) {
	const auto original = s_updateMistHook->GetTrampoline<UpdateMistFn>();

	const auto scene = *reinterpret_cast<uint8_t**>(appBaseAddr + SCENE_STATE_PTR_OFFSET);
	if (!scene || !SplitscreenActive()) {
		s_mistCameraMatrixValid = false;
		return original(effectSystem, effect);
	}

	uint8_t* liveMatrix = scene + SCENE_CAMERA_MATRIX;
	if (FramePlayer() == 0) {
		std::memcpy(s_mistCameraMatrix, liveMatrix, SCENE_CAMERA_MATRIX_SIZE);
		s_mistCameraMatrixValid = true;
		return original(effectSystem, effect);
	}
	if (!s_mistCameraMatrixValid) {
		return original(effectSystem, effect);
	}

	uint8_t backup[SCENE_CAMERA_MATRIX_SIZE];
	std::memcpy(backup, liveMatrix, SCENE_CAMERA_MATRIX_SIZE);
	std::memcpy(liveMatrix, s_mistCameraMatrix, SCENE_CAMERA_MATRIX_SIZE);
	original(effectSystem, effect);
	std::memcpy(liveMatrix, backup, SCENE_CAMERA_MATRIX_SIZE);
}

// UpdatePlayerInput_sub_1401EBCF0 derives actor.cameraDirection from the live
// camera. Actors that belong to another player's camera get that camera's
// eye/lookat for the duration of the call, otherwise movement flips between
// diverging yaws every tick.
__int64 __fastcall UpdatePlayerInputHook(__int64 actor) {
	const auto original = s_updatePlayerInputHook->GetTrampoline<UpdatePlayerInputFn>();

	const int owner = *reinterpret_cast<uint8_t*>(actor + 0x1CACC); // newPlayerIndex
	uint8_t* camObj = nullptr;
	if (owner < CrimsonSplitscreen::ViewCount() && owner != g_viewedCameraPlayer && s_camSlots.valid[owner]) {
		const auto scene = *reinterpret_cast<uintptr_t*>(appBaseAddr + SCENE_STATE_PTR_OFFSET);
		if (scene) {
			camObj = *reinterpret_cast<uint8_t**>(scene + 0x240);
		}
	}
	if (!camObj) {
		return original(actor);
	}

	int region = 0;
	uint8_t* regions[CAM_REGION_COUNT];
	CameraRegions(*reinterpret_cast<uint8_t**>(appBaseAddr + CAMERA_CTRL_PTR_OFFSET), regions);
	for (int r = 1; r < CAM_REGION_COUNT; ++r) {
		if (regions[r] == camObj) {
			region = r;
		}
	}
	constexpr size_t vecBlockSize = 0x20;
	uint8_t backup[vecBlockSize];
	std::memcpy(backup, camObj + CAM_EYE_OFFSET, vecBlockSize);
	std::memcpy(camObj + CAM_EYE_OFFSET, s_camSlots.state[owner][region] + CAM_EYE_OFFSET, vecBlockSize);
	const __int64 result = original(actor);
	std::memcpy(camObj + CAM_EYE_OFFSET, backup, vecBlockSize);
	return result;
}

struct CockpitSlots {
	uint8_t state[PLAYER_COUNT][COCKPIT_STATE_END - COCKPIT_STATE_BEGIN] = {};
	bool valid[PLAYER_COUNT] = {};
};
CockpitSlots s_cockpitSlots;

// What each player's HUD icon models show; -1 = unknown. The models are per player images (HUD_ActivatePlayerModels).
struct HudIcons {
	int vergil  = -1;
	int lockOn  = -1;
	int style   = -1;
	int melee   = -1;
	int ranged  = -1;
};
HudIcons s_hudIcons[PLAYER_COUNT];
uint32 s_hudSerial = 0; // g_hudIconSerial after our last sync; anything else means a loader ran elsewhere
bool s_hudWasSplit = false;

void ResetHudIcons() {
	for (auto& icons : s_hudIcons) {
		icons = {};
	}
}

// What the swap of the live models found empty has to be loaded again.
void ApplyModelFlags(unsigned flags, int player) {
	if (flags & HUD_MODELS_REBUILT) {
		ResetHudIcons();
		return;
	}
	auto& icons = s_hudIcons[player];
	if (flags & HUD_MODELS_TOP_BLANK) {
		icons.vergil = -1;
		icons.style  = -1;
	}
	if (flags & HUD_MODELS_BOTTOM_BLANK) {
		icons.melee  = -1;
		icons.ranged = -1;
	}
	if (flags & HUD_MODELS_CURSOR_BLANK) {
		icons.lockOn = -1;
	}
}

uint8_t* PlayerMainActor(int player) {
	auto& playerData = GetPlayerData(static_cast<uint8>(player));
	return reinterpret_cast<uint8_t*>(GetNewActorData(static_cast<uint8>(player), playerData.activeCharacterIndex, ENTITY::MAIN).baseAddr);
}

// The cameras read the right stick from pad 0 (GetPadStickDirection_sub_14032CC30 /
// GetPadRightStickDirection_sub_14032CC90) and the target actor from pool_10222[3] (reset-behind-the-player
// angle, lock-on state, stand-still check). Each frame is built for one player, so lend the update
// that player's pad and actor. followsActor: the player camera also keeps its target actor in +0xB0.
__int64 UpdateCameraForPlayer(UpdateCameraFn original, __int64 camera, bool followsActor) {
	auto* pool = *reinterpret_cast<uintptr_t**>(appBaseAddr + PLAYER_POOL_OFFSET);
	uint8_t* actor = (SplitscreenActive() && pool) ? PlayerMainActor(FramePlayer()) : nullptr;
	if (!actor) {
		return original(camera);
	}

	auto* pads = reinterpret_cast<ENGINE_GAMEPAD*>(appBaseAddr + PAD_BLOCK_OFFSET);
	const uint8 pad = reinterpret_cast<PlayerActorData*>(actor)->newGamepad;
	const bool swapPad = pad != 0 && pad < PLAYER_COUNT;
	ENGINE_GAMEPAD savedPad = pads[0];
	if (swapPad) {
		pads[0] = pads[pad];
	}
	const uintptr_t mainPlayerActor = pool[3];
	pool[3] = reinterpret_cast<uintptr_t>(actor);
	auto* followTarget = reinterpret_cast<uintptr_t*>(camera + CAM_FOLLOW_TARGET);
	const uintptr_t savedTarget = *followTarget;
	const bool swapTarget = followsActor && savedTarget != 0;
	if (swapTarget) {
		*followTarget = reinterpret_cast<uintptr_t>(actor);
	}

	const __int64 result = original(camera);

	if (swapTarget) {
		*followTarget = savedTarget;
	}
	pool[3] = mainPlayerActor;
	if (swapPad) {
		pads[0] = savedPad;
	}
	return result;
}

// UpdateCameraOrbit_sub_140057520: third person camera, includes the lock-on camera mode.
__int64 __fastcall UpdateCameraOrbitHook(__int64 cameraData) {
	return UpdateCameraForPlayer(s_updateCameraOrbitHook->GetTrampoline<UpdateCameraFn>(), cameraData, true);
}

// UpdateCameraBoss_sub_140050920
__int64 __fastcall UpdateCameraBossHook(__int64 cameraData) {
	return UpdateCameraForPlayer(s_updateCameraBossHook->GetTrampoline<UpdateCameraFn>(), cameraData, false);
}

// UpdateCameraCtrl_sub_140055B90 is the camera control's tick: it walks the room's camera switches and keeps the
// one whose zone holds the player main actor in pool_10222[3]. Fixed cameras are room data, so the only way to
// get each view its own is to run the selection as the player the frame is for.
__int64 __fastcall UpdateCameraCtrlHook(uint8_t* cameraControl) {
	const auto original = s_updateCameraCtrlHook->GetTrampoline<UpdateCameraCtrlFn>();
	auto* pool = *reinterpret_cast<uintptr_t**>(appBaseAddr + PLAYER_POOL_OFFSET);
	uint8_t* actor = (SplitscreenActive() && pool) ? PlayerMainActor(FramePlayer()) : nullptr;
	if (!actor) {
		return original(cameraControl);
	}

	const uintptr_t mainPlayerActor = pool[3];
	pool[3] = reinterpret_cast<uintptr_t>(actor);
	const __int64 result = original(cameraControl);
	pool[3] = mainPlayerActor;
	return result;
}

// Makes the cockpit re-read HP/DT from the actor instead of smoothing from stale values.
void ResetCockpitSmoothing(uint8_t* state) {
	auto at = [state](size_t offset) { return state + (offset - COCKPIT_STATE_BEGIN); };
	*at(0x6888) = 0;
	*at(0x68A9) = 0;
	*at(0x690F) = 0;
	*reinterpret_cast<uint64_t*>(at(0x6970)) = 0;
	*reinterpret_cast<uint64_t*>(at(0x6980)) = 0;
	*reinterpret_cast<uint64_t*>(at(0x6988)) = 0;
}

// The loaders are expensive-ish, so only reload what differs from what is shown. topOnly: only the cockpit top
// models are the player's (shared camera cockpits), the weapon icons and the lock-on cursor stay player 1's.
void SyncHudIcons(uint8_t* actorAddr, int player, bool topOnly = false) {
	auto& actor         = *reinterpret_cast<PlayerActorData*>(actorAddr);
	auto& playerData    = GetPlayerData(static_cast<uint8>(player));
	auto& characterData = GetCharacterData(static_cast<uint8>(player), playerData.activeCharacterIndex, ENTITY::MAIN);

	if (s_hudSerial != g_hudIconSerial) {
		ResetHudIcons();
	}
	auto& icons = s_hudIcons[player];

	const uint8 character = actor.character;
	const int vergil      = (character == CHARACTER::BOB || character == CHARACTER::VERGIL) ? 1 : 0;
	const int style       = static_cast<int>(actor.style);
	const int melee       = (characterData.meleeWeaponIndex < MELEE_WEAPON_COUNT) ? characterData.meleeWeapons[characterData.meleeWeaponIndex] : -1;
	const int ranged      = (characterData.rangedWeaponIndex < RANGED_WEAPON_COUNT) ? characterData.rangedWeapons[characterData.rangedWeaponIndex] : -1;

	if (vergil != icons.vergil) {
		HUD_UpdateHPBar(character);
		HUD_UpdateDevilTriggerGauge(character);
		HUD_UpdateDevilTriggerLightning(character);
		HUD_UpdateDevilTriggerExplosion(character);
		icons.style = -1;
	}
	if (!topOnly && vergil != icons.lockOn) {
		HUD_UpdateLockOn(character);
	}
	if (style != icons.style && style < STYLE::MAX) {
		HUD_UpdateStyleIcon(static_cast<uint8>(style), character);
	}
	if (!topOnly && melee != icons.melee && melee >= 0) {
		HUD_UpdateWeaponIcon(HUD_BOTTOM::MELEE_WEAPON_1, static_cast<uint8>(melee));
	}
	if (!topOnly && ranged != icons.ranged && ranged >= 0) {
		HUD_UpdateWeaponIcon(HUD_BOTTOM::RANGED_WEAPON_1, static_cast<uint8>(ranged));
	}

	icons.vergil  = vergil;
	icons.style   = style;
	if (!topOnly) {
		icons.lockOn = vergil;
		icons.melee  = melee;
		icons.ranged = ranged;
	}
	s_hudSerial   = g_hudIconSerial;
}

// UpdateCUIDCockpit00_sub_14027C0F0
// The cockpit reads HP/DT/style from pool_10222[3], i.e. always player 1. Each frame is
// built for one player, so point it at that player's actor and keep a cockpit state
// (HP/DT smoothing, alpha timers) per player.

// Shared / single player camera: one cockpit per player, all drawn in the same frame (HUD.cpp). The game's cockpit
// object is player 1's; every tick it is also updated for the other players, and what that produced (state, element
// matrices, top models) is kept per player and swapped in for their draw (SeatCockpitBegin).
constexpr size_t COCKPIT_MATRIX_BEGIN = 0xB0;                 // 13 elements, 0x80 apart
constexpr size_t COCKPIT_MATRIX_SIZE  = 13 * 0x80;
constexpr size_t COCKPIT_STATE_SIZE   = COCKPIT_STATE_END - COCKPIT_STATE_BEGIN;

struct SeatSlots {
	uint8_t matrices[PLAYER_COUNT][COCKPIT_MATRIX_SIZE] = {};
	bool ready[PLAYER_COUNT] = {}; // updated at least once since the cockpit object appeared
};
SeatSlots s_seatSlots;
uint8_t s_seatLiveState[COCKPIT_STATE_SIZE];
uint8_t s_seatLiveMatrices[COCKPIT_MATRIX_SIZE];
bool s_seatLoaded = false;

void ClearSeatReady() {
	for (bool& ready : s_seatSlots.ready) {
		ready = false;
	}
}

void UpdateSeatCockpits(CockpitFn original, uint8_t* cockpit, uintptr_t* pool, int seats) {
	s_hudWasSplit = true;
	uint8_t* live = cockpit + COCKPIT_STATE_BEGIN;
	uint8_t* liveMatrices = cockpit + COCKPIT_MATRIX_BEGIN;

	ApplyModelFlags(HUD_ActivatePlayerModels(0, HUD_GROUP_TOP), 0);
	original(cockpit);

	uint8_t leadState[COCKPIT_STATE_SIZE];
	uint8_t leadMatrices[COCKPIT_MATRIX_SIZE];
	std::memcpy(leadState, live, COCKPIT_STATE_SIZE);
	std::memcpy(leadMatrices, liveMatrices, COCKPIT_MATRIX_SIZE);
	// Player 1's state is what the cockpit object holds once the seats are done, so keep it for CockpitTopLeftAlpha.
	std::memcpy(s_cockpitSlots.state[0], leadState, COCKPIT_STATE_SIZE);
	s_cockpitSlots.valid[0] = true;

	for (int player = 1; player < seats; ++player) {
		uint8_t* actor = PlayerMainActor(player);
		if (!actor) {
			s_seatSlots.ready[player] = false;
			continue;
		}
		if (s_hudSerial != g_hudIconSerial) {
			ResetHudIcons();
		}
		ApplyModelFlags(HUD_ActivatePlayerModels(player, HUD_GROUP_TOP), player);
		if (!s_cockpitSlots.valid[player]) {
			std::memcpy(s_cockpitSlots.state[player], leadState, COCKPIT_STATE_SIZE);
			ResetCockpitSmoothing(s_cockpitSlots.state[player]);
			s_cockpitSlots.valid[player] = true;
		}
		std::memcpy(live, s_cockpitSlots.state[player], COCKPIT_STATE_SIZE);
		// After the state is back, see CockpitUpdateHook.
		SyncHudIcons(actor, player, true);

		const uintptr_t mainPlayerActor = pool[3];
		pool[3] = reinterpret_cast<uintptr_t>(actor);
		original(cockpit);
		pool[3] = mainPlayerActor;

		std::memcpy(s_cockpitSlots.state[player], live, COCKPIT_STATE_SIZE);
		std::memcpy(s_seatSlots.matrices[player], liveMatrices, COCKPIT_MATRIX_SIZE);
		s_seatSlots.ready[player] = true;
	}
	for (int player = seats; player < PLAYER_COUNT; ++player) {
		s_seatSlots.ready[player] = false;
	}

	const unsigned flags = HUD_ActivatePlayerModels(0, HUD_GROUP_TOP);
	ApplyModelFlags(flags, 0);
	std::memcpy(live, leadState, COCKPIT_STATE_SIZE);
	std::memcpy(liveMatrices, leadMatrices, COCKPIT_MATRIX_SIZE);
	if (flags & HUD_MODELS_TOP_BLANK) {
		if (uint8_t* lead = PlayerMainActor(0)) {
			SyncHudIcons(lead, 0, true);
		}
	}
}

void __fastcall CockpitUpdateHook(uint8_t* cockpit) {
	const auto original = s_cockpitUpdateHook->GetTrampoline<CockpitFn>();
	HUD_RunRepair();
	auto* pool = *reinterpret_cast<uintptr_t**>(appBaseAddr + PLAYER_POOL_OFFSET);

	// A new cockpit object (new room / mission) starts from clean state and icon models.
	static uint8_t* s_lastCockpit = nullptr;
	if (cockpit != s_lastCockpit) {
		s_lastCockpit = cockpit;
		for (bool& valid : s_cockpitSlots.valid) {
			valid = false;
		}
		ClearSeatReady();
		ResetHudIcons();
		s_hudWasSplit = false;
	}

	if (!SplitscreenActive() || !cockpit || !pool) {
		if (cockpit && pool && HUD_SeatCount() >= 2) {
			UpdateSeatCockpits(original, cockpit, pool, HUD_SeatCount());
			return;
		}
		ClearSeatReady();
		if (s_hudWasSplit && cockpit && pool && !CrimsonSplitscreen::GateOpen() && InGame() && !g_inGameCutscene) {
			s_hudWasSplit = false;
			ResetCockpitSmoothing(cockpit + COCKPIT_STATE_BEGIN);
			ApplyModelFlags(HUD_ReleasePlayerModels(), 0);
			if (auto* mainActor = PlayerMainActor(0)) {
				SyncHudIcons(mainActor, 0);
			}
		}
		for (bool& valid : s_cockpitSlots.valid) {
			valid = false;
		}
		original(cockpit);
		return;
	}

	const int player = FramePlayer();
	uint8_t* actor = PlayerMainActor(player);
	if (!actor) {
		original(cockpit);
		return;
	}
	s_hudWasSplit = true;

	if (s_hudSerial != g_hudIconSerial) {
		ResetHudIcons();
	}
	ApplyModelFlags(HUD_ActivatePlayerModels(player), player);

	constexpr size_t stateSize = COCKPIT_STATE_END - COCKPIT_STATE_BEGIN;
	uint8_t* live = cockpit + COCKPIT_STATE_BEGIN;
	if (!s_cockpitSlots.valid[player]) {
		std::memcpy(s_cockpitSlots.state[player], live, stateSize);
		ResetCockpitSmoothing(s_cockpitSlots.state[player]);
		s_cockpitSlots.valid[player] = true;
	}
	std::memcpy(live, s_cockpitSlots.state[player], stateSize);

	// After the state is back: the style icon loader sets the icon effect byte (+0x690E) in this state, which the
	// Quicksilver and Doppelganger icons need and which the restore above would overwrite with the old style's value.
	SyncHudIcons(actor, player);

	const uintptr_t mainPlayerActor = pool[3];
	pool[3] = reinterpret_cast<uintptr_t>(actor);
	original(cockpit);
	pool[3] = mainPlayerActor;

	std::memcpy(s_cockpitSlots.state[player], live, stateSize);
}

struct LockOnCursorSlots {
	uint8_t state[PLAYER_COUNT][LOCKON_STATE_END - LOCKON_STATE_BEGIN] = {};
	uint64_t target[PLAYER_COUNT] = {};
	bool valid[PLAYER_COUNT] = {};
};
LockOnCursorSlots s_lockOnSlots;

// CUIDLockOnCursorUpdate_sub_140296DB0
// The original lock-on cursor reads the lock-on button and the target list from pool_10222[3], i.e. always
// player 1, and keeps the marker lists in the object. Each frame is built for one player, so point it at that
// player's actor and keep the lists per player (the draw, vtable slot 2, only submits what the update just set).
void __fastcall LockOnCursorUpdateHook(uint8_t* cursor) {
	const auto original = s_lockOnCursorUpdateHook->GetTrampoline<CockpitFn>();
	HUD_AuditModels("lock-on update");
	auto* pool = *reinterpret_cast<uintptr_t**>(appBaseAddr + PLAYER_POOL_OFFSET);

	static uint8_t* s_lastCursor = nullptr;
	if (cursor != s_lastCursor) {
		s_lastCursor = cursor;
		for (bool& valid : s_lockOnSlots.valid) {
			valid = false;
		}
	}

	uint8_t* actor = (SplitscreenActive() && cursor && pool) ? PlayerMainActor(FramePlayer()) : nullptr;
	if (!actor) {
		for (bool& valid : s_lockOnSlots.valid) {
			valid = false;
		}
		CrimsonEnemyAITarget::RunWithActorLockState(pool ? pool[3] : 0, [&]() { original(cursor); });
		return;
	}

	const int player = FramePlayer();
	constexpr size_t stateSize = LOCKON_STATE_END - LOCKON_STATE_BEGIN;
	uint8_t* live = cursor + LOCKON_STATE_BEGIN;
	auto* liveTarget = reinterpret_cast<uint64_t*>(cursor + LOCKON_TARGET_PTR);
	if (!s_lockOnSlots.valid[player]) {
		std::memset(s_lockOnSlots.state[player], 0, stateSize);
		s_lockOnSlots.target[player] = 0;
		s_lockOnSlots.valid[player] = true;
	}
	std::memcpy(live, s_lockOnSlots.state[player], stateSize);
	*liveTarget = s_lockOnSlots.target[player];

	const uintptr_t mainPlayerActor = pool[3];
	pool[3] = reinterpret_cast<uintptr_t>(actor);

	// The cursor projects its targets with the scene's projection matrix, which at this point still belongs to
	// whichever player's camera ran last (the other player's, every other tick). Use this player's own.
	const auto scene = *reinterpret_cast<uint8_t**>(appBaseAddr + SCENE_STATE_PTR_OFFSET);
	uint8_t* liveProjection = scene ? scene + SCENE_PROJECTION_MATRIX : nullptr;
	uint8_t projectionBackup[SCENE_CAMERA_MATRIX_SIZE];
	const bool swapProjection = liveProjection && s_projectionSlots.valid[player];
	if (swapProjection) {
		std::memcpy(projectionBackup, liveProjection, SCENE_CAMERA_MATRIX_SIZE);
		std::memcpy(liveProjection, s_projectionSlots.matrix[player], SCENE_CAMERA_MATRIX_SIZE);
	}

	CrimsonEnemyAITarget::RunWithActorLockState(reinterpret_cast<uintptr_t>(actor), [&]() { original(cursor); });

	if (swapProjection) {
		std::memcpy(liveProjection, projectionBackup, SCENE_CAMERA_MATRIX_SIZE);
	}
	pool[3] = mainPlayerActor;

	std::memcpy(s_lockOnSlots.state[player], live, stateSize);
	s_lockOnSlots.target[player] = *liveTarget;
}

} // namespace

namespace CrimsonSplitscreen {

// Only while playing: scene GAME + EVENT::MAIN, outside of cutscenes. The camera and HUD objects
// are being rebuilt otherwise.
bool GateOpen() {
	if (g_multiplayerCameraMode != MULTIPLAYERCAMERA::SPLITSCREEN || !activeConfig.Actor.enable || activeConfig.Actor.playerCount < 2) {
		return false;
	}
	return InGame() && !g_inGameCutscene && !InCutscene();
}

// Splitscreen is the active multiplayer camera (2+ players), whether or not it is currently running.
bool Enabled() {
	return g_multiplayerCameraMode == MULTIPLAYERCAMERA::SPLITSCREEN && activeConfig.Actor.enable && activeConfig.Actor.playerCount >= 2;
}

int ViewCount() {
	if (!GateOpen() || s_settling) {
		return 0;
	}
	return (activeConfig.Actor.playerCount > PLAYER_COUNT) ? PLAYER_COUNT : activeConfig.Actor.playerCount;
}

int CameraPlayer() {
	return SplitscreenActive() ? FramePlayer() : 0;
}

uint8_t* CameraActor() {
	return SplitscreenActive() ? PlayerMainActor(FramePlayer()) : nullptr;
}

const uint8_t* CameraCopy(int playerIndex) {
	if (playerIndex < 0 || playerIndex >= ViewCount() || !g_splitscreenCamCopyValid[playerIndex]) {
		return nullptr;
	}
	return g_splitscreenCamCopy[playerIndex];
}

bool CameraEye(int playerIndex, float out[3]) {
	const uint8_t* cam = CameraCopy(playerIndex);
	if (!cam) {
		return false;
	}
	const float* eye = reinterpret_cast<const float*>(cam + CAM_EYE_OFFSET);
	out[0] = eye[0];
	out[1] = eye[1];
	out[2] = eye[2];
	return true;
}

const uint8_t* ProjectionMatrix(int playerIndex) {
	if (playerIndex < 0 || playerIndex >= PLAYER_COUNT || !s_projectionSlots.valid[playerIndex]) {
		return nullptr;
	}
	return s_projectionSlots.matrix[playerIndex];
}

void MapPoint(int playerIndex, float frameW, float frameH, float& x, float& y) {
	const int count = ViewCount();
	if (count < 2) {
		return;
	}
	if (count == 2) {
		// Halves show the centre of the frame, 1:1.
		x = x - frameW * 0.25f + static_cast<float>(playerIndex) * frameW * 0.5f;
		return;
	}
	const float cellX = static_cast<float>(playerIndex & 1) * frameW * 0.5f;
	const float cellY = static_cast<float>(playerIndex >> 1) * frameH * 0.5f;
	if (g_splitscreenQuadScaled) {
		x = x * 0.5f + cellX;
		y = y * 0.5f + cellY;
	} else {
		x = x - frameW * 0.25f + cellX;
		y = y - frameH * 0.25f + cellY;
	}
}

bool ViewRect(int playerIndex, float frameW, float frameH, float& minX, float& minY, float& maxX, float& maxY) {
	const int count = ViewCount();
	if (count < 2 || playerIndex < 0 || playerIndex >= count) {
		return false;
	}
	if (count == 2) {
		minX = static_cast<float>(playerIndex) * frameW * 0.5f;
		minY = 0.0f;
		maxX = minX + frameW * 0.5f;
		maxY = frameH;
		return true;
	}
	minX = static_cast<float>(playerIndex & 1) * frameW * 0.5f;
	minY = static_cast<float>(playerIndex >> 1) * frameH * 0.5f;
	maxX = minX + frameW * 0.5f;
	maxY = minY + frameH * 0.5f;
	return true;
}

float ViewScale(int playerIndex) {
	const int count = ViewCount();
	if (count < 2 || playerIndex < 0 || playerIndex >= count) {
		return 1.0f;
	}
	return (count > 2 && g_splitscreenQuadScaled) ? 0.5f : 1.0f;
}

bool HudLayoutInUse(HudSplitLayout& out) {
	if (!SplitscreenActive()) {
		return false;
	}
	const int layout = ActiveHudLayout();
	const HudTuning& tuning = s_hudTuning[layout];
	out = { tuning.topLeft, tuning.topRight, tuning.bottom, s_splitHudGeneral[layout] };
	return true;
}

// Swaps player's cockpit (state, element matrices, top models) in for the draw that follows; SeatCockpitEnd gives the
// cockpit back to player 1. False when that player has not been updated yet (nothing to draw).
bool SeatCockpitBegin(uint8_t* cockpit, int player) {
	if (!cockpit || player <= 0 || player >= PLAYER_COUNT || s_seatLoaded || !s_seatSlots.ready[player] || !s_cockpitSlots.valid[player]) {
		return false;
	}
	std::memcpy(s_seatLiveState, cockpit + COCKPIT_STATE_BEGIN, COCKPIT_STATE_SIZE);
	std::memcpy(s_seatLiveMatrices, cockpit + COCKPIT_MATRIX_BEGIN, COCKPIT_MATRIX_SIZE);
	s_seatLoaded = true;

	const unsigned flags = HUD_ActivatePlayerModels(player, HUD_GROUP_TOP);
	if (flags) {
		// The player's images are gone (the game rebuilt the HUD): the tick update loads them again.
		ApplyModelFlags(flags, player);
		s_seatSlots.ready[player] = false;
		SeatCockpitEnd(cockpit);
		return false;
	}
	std::memcpy(cockpit + COCKPIT_STATE_BEGIN, s_cockpitSlots.state[player], COCKPIT_STATE_SIZE);
	std::memcpy(cockpit + COCKPIT_MATRIX_BEGIN, s_seatSlots.matrices[player], COCKPIT_MATRIX_SIZE);
	return true;
}

void SeatCockpitEnd(uint8_t* cockpit) {
	if (!s_seatLoaded) {
		return;
	}
	s_seatLoaded = false;
	std::memcpy(cockpit + COCKPIT_STATE_BEGIN, s_seatLiveState, COCKPIT_STATE_SIZE);
	std::memcpy(cockpit + COCKPIT_MATRIX_BEGIN, s_seatLiveMatrices, COCKPIT_MATRIX_SIZE);
	ApplyModelFlags(HUD_ActivatePlayerModels(0, HUD_GROUP_TOP), 0);
}

uint8_t CockpitTopLeftAlpha(int player) {
	if (player >= 0 && player < PLAYER_COUNT && s_cockpitSlots.valid[player]) {
		return s_cockpitSlots.state[player][COCKPIT_TOP_LEFT_ALPHA - COCKPIT_STATE_BEGIN];
	}
	auto cockpit = *reinterpret_cast<uint8_t**>(appBaseAddr + COCKPIT_PTR_OFFSET);
	return cockpit ? cockpit[COCKPIT_TOP_LEFT_ALPHA] : 0;
}

void HudTuningWindow(bool* open) {
	ImGui::SetNextWindowSize(ImVec2(460, 0), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Splitscreen HUD", open)) {
		ImGui::End();
		return;
	}

	static int s_editLayout = HUD_LAYOUT_HALVES;
	static int s_lastActiveLayout = -1;
	const int viewCount = ViewCount();
	if (viewCount >= 2) {
		const int activeLayout = (viewCount > 2) ? HUD_LAYOUT_QUADS : HUD_LAYOUT_HALVES;
		if (activeLayout != s_lastActiveLayout) {
			s_editLayout = activeLayout;
			s_lastActiveLayout = activeLayout;
		}
	}

	if (viewCount < 2) {
		ImGui::TextWrapped("Splitscreen is off (not in a multiplayer session or another multiplayer camera is selected). Values still apply once it is on.");
	}
	ImGui::RadioButton("Halves (2 players)", &s_editLayout, HUD_LAYOUT_HALVES);
	ImGui::SameLine();
	ImGui::RadioButton("Quadrants (3-4 players)", &s_editLayout, HUD_LAYOUT_QUADS);
	HudTuning& tuning = s_hudTuning[s_editLayout];
	HudGeneral& general = s_splitHudGeneral[s_editLayout];
	const bool quads = (s_editLayout == HUD_LAYOUT_QUADS);

	ImGui::Text("Frame aspect: %.3f", s_frameAspect);
	if (quads && !g_splitscreenQuadScaled) {
		ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "Downscale unavailable: quadrants show a centre crop.");
	}

	ImGui::Separator();
	ImGui::Text("General (top left + style addons)");
	ImGui::TextWrapped("Same controls as the single player window: scale, x and y move the top-left cockpit group and the style addons "
		"as one, on top of the layout base below (x/y in HUD units, %.0f tall, y grows downward, measured on the full frame before it is "
		"cropped / downscaled into the views). The calibration is shared with the single player window.", HUD_HEIGHT);
	HUD_GeneralControls(general, quads ? HUD_GENERAL_QUADS : HUD_GENERAL_HALVES,
		quads ? HUD_PRESETS_QUADS : HUD_PRESETS_HALVES, quads ? std::size(HUD_PRESETS_QUADS) : std::size(HUD_PRESETS_HALVES));
	HUD_CalibrationControls();
	HUD_DrawPivotMarker();

	ImGui::Separator();
	HUD_SplitWheelControls();
	HUD_RedOrbControls();
	HUD_StyleMeterControls(quads ? HUD_STYLE_QUADS : HUD_STYLE_HALVES);
	HUD_VanillaStyleMeterControls(quads ? HUD_STYLE_QUADS : HUD_STYLE_HALVES);

	ImGui::Separator();
	ImGui::Text("Layout groups (raw)");
	ImGui::TextWrapped("Raw matrix values: position = original * posScale + offset. Half of the frame width is %.1f units if the HUD is %.0f tall. "
		"The top left group is the base the general controls adjust.", HUD_HEIGHT * s_frameAspect * 0.5f, HUD_HEIGHT);

	auto group = [](const char* label, HudGroupTuning& g, ImGuiTreeNodeFlags flags) {
		if (ImGui::CollapsingHeader(label, flags)) {
			ImGui::PushID(label);
			ImGui::DragFloat("sizeScale", &g.sizeScale, 0.005f, 0.05f, 2.0f, "%.3f");
			ImGui::DragFloat("posScale", &g.posScale, 0.005f, 0.0f, 2.0f, "%.3f");
			ImGui::DragFloat("offsetX", &g.offsetX, 0.5f, -800.0f, 800.0f, "%.1f");
			ImGui::DragFloat("offsetY", &g.offsetY, 0.5f, -800.0f, 800.0f, "%.1f");
			ImGui::PopID();
		}
	};
	group("Top left base (HP / DT / style)", tuning.topLeft, ImGuiTreeNodeFlags_None);
	group("Top right (red orbs)", tuning.topRight, ImGuiTreeNodeFlags_DefaultOpen);
	group("Bottom (weapons)", tuning.bottom, ImGuiTreeNodeFlags_DefaultOpen);

	ImGui::Checkbox("Show view guides", &tuning.showGuides);
	if (tuning.showGuides) {
		auto* draw = ImGui::GetForegroundDrawList();
		const ImVec2 size = ImGui::GetIO().DisplaySize;
		const ImU32 colour = IM_COL32(255, 0, 255, 200);
		const int cells = quads ? PLAYER_COUNT : 2;
		for (int i = 0; i < cells; ++i) {
			const float x0 = static_cast<float>(i & 1) * size.x * 0.5f;
			const float y0 = quads ? static_cast<float>(i >> 1) * size.y * 0.5f : 0.0f;
			const float w  = size.x * 0.5f;
			const float h  = quads ? size.y * 0.5f : size.y;
			draw->AddRect(ImVec2(x0 + 1, y0 + 1), ImVec2(x0 + w - 1, y0 + h - 1), colour);
		}
	}

	if (ImGui::Button("Reset all")) {
		tuning = quads ? HUD_TUNING_QUADS : HUD_TUNING_HALVES;
		general = quads ? HUD_GENERAL_QUADS : HUD_GENERAL_HALVES;
	}
	ImGui::SameLine();
	if (ImGui::Button("Copy as code")) {
		char text[1024];
		auto fmt = [](const HudGroupTuning& g) {
			char line[160];
			snprintf(line, sizeof(line), "\t{ %.3ff, %.3ff, %.1ff, %.1ff },\n", g.sizeScale, g.posScale, g.offsetX, g.offsetY);
			return std::string(line);
		};
		snprintf(text, sizeof(text), "constexpr HudTuning %s = {\n%s%s%s\tfalse,\n};\nconstexpr HudGeneral %s = { %.3ff, %.1ff, %.1ff };\n",
			quads ? "HUD_TUNING_QUADS" : "HUD_TUNING_HALVES",
			fmt(tuning.topLeft).c_str(), fmt(tuning.topRight).c_str(), fmt(tuning.bottom).c_str(),
			quads ? "HUD_GENERAL_QUADS" : "HUD_GENERAL_HALVES", general.scale, general.offsetX, general.offsetY);
		ImGui::SetClipboardText(text);
	}

	ImGui::End();
}

void InitHooks() {
	s_submitDrawListHook = std::make_unique<Utility::Detour_t>(
		(uintptr_t)appBaseAddr + SUBMIT_DRAW_LIST_OFFSET, &SubmitDrawListHook, 0, "SubmitDrawList_sub_14032D8C0");
	s_submitDrawListHook->Toggle(true);

	s_updatePlayerInputHook = std::make_unique<Utility::Detour_t>(
		(uintptr_t)appBaseAddr + UPDATE_PLAYER_INPUT_OFFSET, &UpdatePlayerInputHook, 0, "UpdatePlayerInput_sub_1401EBCF0");
	s_updatePlayerInputHook->Toggle(true);

	s_updateMistHook = std::make_unique<Utility::Detour_t>(
		(uintptr_t)appBaseAddr + UPDATE_MIST_OFFSET, &UpdateMistHook, 0, "UpdateBlendEffect_Mist_sub_14031AE10");
	s_updateMistHook->Toggle(true);

	s_cockpitUpdateHook = std::make_unique<Utility::Detour_t>(
		(uintptr_t)appBaseAddr + COCKPIT_TOP_UPDATE_OFFSET, &CockpitUpdateHook, 0, "UpdateCUIDCockpit00_sub_14027C0F0");
	s_cockpitUpdateHook->Toggle(true);

	s_lockOnCursorUpdateHook = std::make_unique<Utility::Detour_t>(
		(uintptr_t)appBaseAddr + LOCKON_CURSOR_UPDATE_OFFSET, &LockOnCursorUpdateHook, 0, "CUIDLockOnCursorUpdate_sub_140296DB0");
	s_lockOnCursorUpdateHook->Toggle(true);

	s_updateCameraOrbitHook = std::make_unique<Utility::Detour_t>(
		(uintptr_t)appBaseAddr + UPDATE_CAMERA_ORBIT_OFFSET, &UpdateCameraOrbitHook, 0, "UpdateCameraOrbit_sub_140057520");
	s_updateCameraOrbitHook->Toggle(true);

	s_updateCameraBossHook = std::make_unique<Utility::Detour_t>(
		(uintptr_t)appBaseAddr + UPDATE_CAMERA_BOSS_OFFSET, &UpdateCameraBossHook, 0, "UpdateCameraBoss_sub_140050920");
	s_updateCameraBossHook->Toggle(true);

	s_updateCameraCtrlHook = std::make_unique<Utility::Detour_t>(
		(uintptr_t)appBaseAddr + UPDATE_CAMERA_CTRL_OFFSET, &UpdateCameraCtrlHook, 0, "UpdateCameraCtrl_sub_140055B90");
	s_updateCameraCtrlHook->Toggle(true);
}

} // namespace CrimsonSplitscreen

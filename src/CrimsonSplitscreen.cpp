#include "CrimsonSplitscreen.hpp"

#include <algorithm>
#include <cfloat>
#include <cstdio>
#include <cstring>
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
constexpr uintptr_t PLAYER_POOL_OFFSET         = 0xC90E28;
constexpr uintptr_t COCKPIT_TOP_UPDATE_OFFSET  = 0x27C0F0;
constexpr uintptr_t COCKPIT_TOP_DRAW_OFFSET    = 0x27F1F0;
constexpr uintptr_t COCKPIT_BOTTOM_DRAW_OFFSET = 0x281120;

// CUIDCockpit00: 13 element matrices, 0x80 apart. The first is the upper HP frame.
constexpr size_t COCKPIT_TOP_MATRIX_BASE   = 0xB0;
constexpr size_t COCKPIT_TOP_MATRIX_STRIDE = 0x80;
constexpr size_t COCKPIT_TOP_MATRIX_COUNT  = 13;
// Per-frame state that follows the element ModelData array (HP/DT smoothing, alpha timers).
constexpr size_t COCKPIT_STATE_BEGIN = 0x6880;
constexpr size_t COCKPIT_STATE_END   = 0x7040;
// CUIDCockpit02: two consecutive matrices.
constexpr size_t COCKPIT_BOTTOM_MATRIX_BASE  = 0x26E0;
constexpr size_t COCKPIT_BOTTOM_MATRIX_COUNT = 2;

// HUD space: y grows downward, x is centred on 0, 640x448 design. Each group's elements are
// drawn at position * posScale + offset, with the model scaled by sizeScale. Tuned live from
// the "Splitscreen HUD" window (CrimsonSplitscreen::HudTuningWindow).
struct HudGroupTuning {
	float sizeScale;
	float posScale;
	float offsetX;
	float offsetY;
};

struct HudTuning {
	HudGroupTuning topLeft;  // HP / DT / style icon / flux
	HudGroupTuning topRight; // red orb counter group
	HudGroupTuning bottom;   // weapon icons
	bool showGuides;
};

enum HudLayout { HUD_LAYOUT_HALVES = 0, HUD_LAYOUT_QUADS = 1, HUD_LAYOUT_COUNT };

// Halves show a centre crop of the frame, so the HUD is pulled in to its edges.
constexpr HudTuning HUD_TUNING_HALVES = {
	{ 0.500f, 0.500f, -21.5f, 50.0f },
	{ 0.500f, 0.500f, 39.0f, -112.0f },
	{ 0.500f, 0.500f, 0.0f, 112.0f },
	false,
};
// Quadrants show the whole downscaled frame, so only the top-left group needs a nudge.
constexpr HudTuning HUD_TUNING_QUADS = {
	{ 1.000f, 1.000f, -41.0f, 13.5f },
	{ 1.000f, 1.000f, 0.0f, 0.0f },
	{ 1.000f, 1.000f, 0.0f, 0.0f },
	false,
};
HudTuning s_hudTuning[HUD_LAYOUT_COUNT] = { HUD_TUNING_HALVES, HUD_TUNING_QUADS };

float s_frameAspect = 16.0f / 9.0f;

constexpr size_t CAM_STATE_SIZE = 0x200;
constexpr size_t CAM_EYE_OFFSET = 0x70; // eye at 0x70, lookat at 0x80

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

// Saved camera state per player, in two regions: the camera control object and
// the CameraData at pool[147].
struct CameraSlots {
	uint8_t state[PLAYER_COUNT][2][CAM_STATE_SIZE] = {};
	bool valid[PLAYER_COUNT] = {};
};
CameraSlots s_camSlots;

typedef __int64(__fastcall* SubmitDrawListFn)(int);
typedef __int64(__fastcall* UpdatePlayerInputFn)(__int64);
typedef void(__fastcall* CockpitFn)(uint8_t*);
std::unique_ptr<Utility::Detour_t> s_submitDrawListHook;
std::unique_ptr<Utility::Detour_t> s_updatePlayerInputHook;
std::unique_ptr<Utility::Detour_t> s_cockpitUpdateHook;
std::unique_ptr<Utility::Detour_t> s_cockpitTopDrawHook;
std::unique_ptr<Utility::Detour_t> s_cockpitBottomDrawHook;

// g_viewedCameraPlayer is advanced right after every drawn frame, so while a
// frame is being drawn it still holds the player that frame was built for.
int FramePlayer() {
	return (g_viewedCameraPlayer < GetSplitscreenViewCount()) ? g_viewedCameraPlayer : 0;
}

bool SplitscreenActive() {
	return GetSplitscreenViewCount() >= 2;
}

const HudTuning& ActiveHudTuning() {
	return s_hudTuning[(GetSplitscreenViewCount() > 2) ? HUD_LAYOUT_QUADS : HUD_LAYOUT_HALVES];
}

void ResetSplitState() {
	for (int p = 0; p < PLAYER_COUNT; ++p) {
		s_camSlots.valid[p]        = false;
		s_cells.valid[p]           = false;
		g_splitscreenCamCopyValid[p] = false;
	}
}

void SwapCameraState(uint8_t* cameraControl, int framePlayer, int viewCount) {
	uint8_t* regions[2] = { cameraControl, nullptr };
	auto pool = *reinterpret_cast<uint8_t***>(appBaseAddr + CAMERA_CTRL_PTR_OFFSET);
	if (pool && pool[147] && !(reinterpret_cast<uintptr_t>(pool[147]) & 0xFFF0000000000000ull) && pool[147] != regions[0]) {
		regions[1] = pool[147];
	}

	for (int r = 0; r < 2; ++r) {
		if (regions[r]) {
			std::memcpy(s_camSlots.state[framePlayer][r], regions[r], CAM_STATE_SIZE);
		}
	}
	s_camSlots.valid[framePlayer] = true;
	std::memcpy(g_splitscreenCamCopy[framePlayer], s_camSlots.state[framePlayer][0], SPLITSCREEN_CAM_COPY_SIZE);
	g_splitscreenCamCopyValid[framePlayer] = true;

	const int next = (framePlayer + 1) % viewCount;
	if (!s_camSlots.valid[next]) {
		// First use: copy the other camera and shift it onto this player.
		const float dx = g_splitscreenPlayerPos[next][0] - g_splitscreenPlayerPos[framePlayer][0];
		const float dy = g_splitscreenPlayerPos[next][1] - g_splitscreenPlayerPos[framePlayer][1];
		const float dz = g_splitscreenPlayerPos[next][2] - g_splitscreenPlayerPos[framePlayer][2];
		for (int r = 0; r < 2; ++r) {
			std::memcpy(s_camSlots.state[next][r], s_camSlots.state[framePlayer][r], CAM_STATE_SIZE);
			auto* eyeAndLookAt = reinterpret_cast<float*>(s_camSlots.state[next][r] + CAM_EYE_OFFSET);
			for (int v = 0; v < 2; ++v) {
				eyeAndLookAt[v * 4 + 0] += dx;
				eyeAndLookAt[v * 4 + 1] += dy;
				eyeAndLookAt[v * 4 + 2] += dz;
			}
		}
		s_camSlots.valid[next] = true;
	}

	for (int r = 0; r < 2; ++r) {
		if (regions[r]) {
			std::memcpy(regions[r], s_camSlots.state[next][r], CAM_STATE_SIZE);
		}
	}
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
	const int viewCount = GetSplitscreenViewCount();
	if (viewCount < 2) {
		ResetSplitState();
		s_lastViewCount = 0;
	} else if (render->device && render->deviceContext && render->renderTargets[0].texture && cameraCtrl) {
		if (viewCount != s_lastViewCount) {
			ResetSplitState();
			s_lastViewCount = viewCount;
		}
		const int player = FramePlayer();
		Composite(render, player, viewCount);
		SwapCameraState(cameraCtrl, player, viewCount);
	}
	return result;
}

// UpdatePlayerInput_sub_1401EBCF0 derives actor.cameraDirection from the live
// camera. Actors that belong to another player's camera get that camera's
// eye/lookat for the duration of the call, otherwise movement flips between
// diverging yaws every tick.
__int64 __fastcall UpdatePlayerInputHook(__int64 actor) {
	const auto original = s_updatePlayerInputHook->GetTrampoline<UpdatePlayerInputFn>();

	const int owner = *reinterpret_cast<uint8_t*>(actor + 0x1CACC); // newPlayerIndex
	uint8_t* camObj = nullptr;
	if (owner < GetSplitscreenViewCount() && owner != g_viewedCameraPlayer && s_camSlots.valid[owner]) {
		const auto scene = *reinterpret_cast<uintptr_t*>(appBaseAddr + SCENE_STATE_PTR_OFFSET);
		if (scene) {
			camObj = *reinterpret_cast<uint8_t**>(scene + 0x240);
		}
	}
	if (!camObj) {
		return original(actor);
	}

	int region = 0;
	auto pool = *reinterpret_cast<uint8_t***>(appBaseAddr + CAMERA_CTRL_PTR_OFFSET);
	if (pool && pool[147] == camObj) {
		region = 1;
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

// What the HUD icon models currently show; -1 = unknown.
struct HudIcons {
	int vergil  = -1;
	int style   = -1;
	int melee   = -1;
	int ranged  = -1;
	uint32 serial = 0;
};
HudIcons s_hudIcons;
bool s_hudWasSplit = false;

uint8_t* PlayerMainActor(int player) {
	auto& playerData = GetPlayerData(static_cast<uint8>(player));
	return reinterpret_cast<uint8_t*>(GetNewActorData(static_cast<uint8>(player), playerData.activeCharacterIndex, ENTITY::MAIN).baseAddr);
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

// The loaders are expensive-ish, so only reload what differs from what is shown.
void SyncHudIcons(uint8_t* actorAddr, int player) {
	auto& actor         = *reinterpret_cast<PlayerActorData*>(actorAddr);
	auto& playerData    = GetPlayerData(static_cast<uint8>(player));
	auto& characterData = GetCharacterData(static_cast<uint8>(player), playerData.activeCharacterIndex, ENTITY::MAIN);

	if (s_hudIcons.serial != g_hudIconSerial) {
		s_hudIcons = {};
	}

	const uint8 character = actor.character;
	const int vergil      = (character == CHARACTER::BOB || character == CHARACTER::VERGIL) ? 1 : 0;
	const int style       = static_cast<int>(actor.style);
	const int melee       = (characterData.meleeWeaponIndex < MELEE_WEAPON_COUNT) ? characterData.meleeWeapons[characterData.meleeWeaponIndex] : -1;
	const int ranged      = (characterData.rangedWeaponIndex < RANGED_WEAPON_COUNT) ? characterData.rangedWeapons[characterData.rangedWeaponIndex] : -1;

	if (vergil != s_hudIcons.vergil) {
		HUD_UpdateHPBar(character);
		HUD_UpdateDevilTriggerGauge(character);
		HUD_UpdateDevilTriggerLightning(character);
		HUD_UpdateDevilTriggerExplosion(character);
		HUD_UpdateLockOn(character);
		s_hudIcons.style = -1;
	}
	if (style != s_hudIcons.style && style < STYLE::MAX) {
		HUD_UpdateStyleIcon(static_cast<uint8>(style), character);
	}
	if (melee != s_hudIcons.melee && melee >= 0) {
		HUD_UpdateWeaponIcon(HUD_BOTTOM::MELEE_WEAPON_1, static_cast<uint8>(melee));
	}
	if (ranged != s_hudIcons.ranged && ranged >= 0) {
		HUD_UpdateWeaponIcon(HUD_BOTTOM::RANGED_WEAPON_1, static_cast<uint8>(ranged));
	}

	s_hudIcons.vergil  = vergil;
	s_hudIcons.style   = style;
	s_hudIcons.melee   = melee;
	s_hudIcons.ranged  = ranged;
	s_hudIcons.serial  = g_hudIconSerial;
}

// UpdateCUIDCockpit00_sub_14027C0F0
// The cockpit reads HP/DT/style from pool_10222[3], i.e. always player 1. Each frame is
// built for one player, so point it at that player's actor and keep a cockpit state
// (HP/DT smoothing, alpha timers) per player.
void __fastcall CockpitUpdateHook(uint8_t* cockpit) {
	const auto original = s_cockpitUpdateHook->GetTrampoline<CockpitFn>();
	auto* pool = *reinterpret_cast<uintptr_t**>(appBaseAddr + PLAYER_POOL_OFFSET);

	if (!SplitscreenActive() || !cockpit || !pool) {
		if (s_hudWasSplit && cockpit && pool) {
			s_hudWasSplit = false;
			ResetCockpitSmoothing(cockpit + COCKPIT_STATE_BEGIN);
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

	SyncHudIcons(actor, player);

	constexpr size_t stateSize = COCKPIT_STATE_END - COCKPIT_STATE_BEGIN;
	uint8_t* live = cockpit + COCKPIT_STATE_BEGIN;
	if (!s_cockpitSlots.valid[player]) {
		std::memcpy(s_cockpitSlots.state[player], live, stateSize);
		ResetCockpitSmoothing(s_cockpitSlots.state[player]);
		s_cockpitSlots.valid[player] = true;
	}
	std::memcpy(live, s_cockpitSlots.state[player], stateSize);

	const uintptr_t mainPlayerActor = pool[3];
	pool[3] = reinterpret_cast<uintptr_t>(actor);
	original(cockpit);
	pool[3] = mainPlayerActor;

	std::memcpy(s_cockpitSlots.state[player], live, stateSize);
}

// Row-vector matrix: output x/y are columns 0/1, translation is the last row.
void ShrinkHudMatrix(float* m, const HudGroupTuning& group) {
	for (int row = 0; row < 3; ++row) {
		m[row * 4 + 0] *= group.sizeScale;
		m[row * 4 + 1] *= group.sizeScale;
	}
	m[12] = m[12] * group.posScale + group.offsetX;
	m[13] = m[13] * group.posScale + group.offsetY;
}

// The red orb counter group is anchored to the right of the screen, the rest to the left.
bool IsRightHudElement(size_t i) {
	return i >= 8 && i <= 10;
}

// The draw copies the matrix, so it can be restored right after.
// CUIDCockpit00 draw (vtable +0x20)
void __fastcall CockpitTopDrawHook(uint8_t* cockpit) {
	const auto original = s_cockpitTopDrawHook->GetTrampoline<CockpitFn>();
	if (!SplitscreenActive() || !cockpit) {
		original(cockpit);
		return;
	}

	auto matrix = [cockpit](size_t i) {
		return reinterpret_cast<float*>(cockpit + COCKPIT_TOP_MATRIX_BASE + COCKPIT_TOP_MATRIX_STRIDE * i);
	};
	const HudTuning& tuning = ActiveHudTuning();
	float backup[COCKPIT_TOP_MATRIX_COUNT][16];
	for (size_t i = 0; i < COCKPIT_TOP_MATRIX_COUNT; ++i) {
		std::memcpy(backup[i], matrix(i), sizeof(backup[i]));
		ShrinkHudMatrix(matrix(i), IsRightHudElement(i) ? tuning.topRight : tuning.topLeft);
	}
	original(cockpit);
	for (size_t i = 0; i < COCKPIT_TOP_MATRIX_COUNT; ++i) {
		std::memcpy(matrix(i), backup[i], sizeof(backup[i]));
	}
}

// CUIDCockpit02 draw (vtable +0x20)
// Its matrices are identity (the layout lives in the model).
void __fastcall CockpitBottomDrawHook(uint8_t* cockpit) {
	const auto original = s_cockpitBottomDrawHook->GetTrampoline<CockpitFn>();
	if (!SplitscreenActive() || !cockpit) {
		original(cockpit);
		return;
	}

	auto matrix = [cockpit](size_t i) {
		return reinterpret_cast<float*>(cockpit + COCKPIT_BOTTOM_MATRIX_BASE + 0x40 * i);
	};
	float backup[COCKPIT_BOTTOM_MATRIX_COUNT][16];
	for (size_t i = 0; i < COCKPIT_BOTTOM_MATRIX_COUNT; ++i) {
		std::memcpy(backup[i], matrix(i), sizeof(backup[i]));
		ShrinkHudMatrix(matrix(i), ActiveHudTuning().bottom);
	}
	original(cockpit);
	for (size_t i = 0; i < COCKPIT_BOTTOM_MATRIX_COUNT; ++i) {
		std::memcpy(matrix(i), backup[i], sizeof(backup[i]));
	}
}

} // namespace

namespace CrimsonSplitscreen {

void HudTuningWindow(bool* open) {
	ImGui::SetNextWindowSize(ImVec2(460, 0), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Splitscreen HUD", open)) {
		ImGui::End();
		return;
	}

	static int s_editLayout = HUD_LAYOUT_HALVES;
	static int s_lastActiveLayout = -1;
	const int viewCount = GetSplitscreenViewCount();
	if (viewCount >= 2) {
		const int activeLayout = (viewCount > 2) ? HUD_LAYOUT_QUADS : HUD_LAYOUT_HALVES;
		if (activeLayout != s_lastActiveLayout) {
			s_editLayout = activeLayout;
			s_lastActiveLayout = activeLayout;
		}
	}

	if (viewCount < 2) {
		ImGui::TextWrapped("Splitscreen is off (F6 with 2+ players). Values still apply once it is on.");
	}
	ImGui::RadioButton("Halves (2 players)", &s_editLayout, HUD_LAYOUT_HALVES);
	ImGui::SameLine();
	ImGui::RadioButton("Quadrants (3-4 players)", &s_editLayout, HUD_LAYOUT_QUADS);
	HudTuning& tuning = s_hudTuning[s_editLayout];
	const bool quads = (s_editLayout == HUD_LAYOUT_QUADS);

	ImGui::Text("Frame aspect: %.3f", s_frameAspect);
	if (quads && !g_splitscreenQuadScaled) {
		ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "Downscale unavailable: quadrants show a centre crop.");
	}
	ImGui::TextWrapped("Position = original * posScale + offset (HUD units, y grows downward). "
		"Half of the frame width is %.1f units if the HUD is 448 tall.", 448.0f * s_frameAspect * 0.5f);

	auto group = [](const char* label, HudGroupTuning& g) {
		if (ImGui::CollapsingHeader(label, ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::PushID(label);
			ImGui::DragFloat("sizeScale", &g.sizeScale, 0.005f, 0.05f, 2.0f, "%.3f");
			ImGui::DragFloat("posScale", &g.posScale, 0.005f, 0.0f, 2.0f, "%.3f");
			ImGui::DragFloat("offsetX", &g.offsetX, 0.5f, -800.0f, 800.0f, "%.1f");
			ImGui::DragFloat("offsetY", &g.offsetY, 0.5f, -800.0f, 800.0f, "%.1f");
			ImGui::PopID();
		}
	};
	group("Top left (HP / DT / style)", tuning.topLeft);
	group("Top right (red orbs)", tuning.topRight);
	group("Bottom (weapons)", tuning.bottom);

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

	if (ImGui::Button("Reset")) {
		tuning = quads ? HUD_TUNING_QUADS : HUD_TUNING_HALVES;
	}
	ImGui::SameLine();
	if (ImGui::Button("Copy as code")) {
		char text[768];
		auto fmt = [](const HudGroupTuning& g) {
			char line[160];
			snprintf(line, sizeof(line), "\t{ %.3ff, %.3ff, %.1ff, %.1ff },\n", g.sizeScale, g.posScale, g.offsetX, g.offsetY);
			return std::string(line);
		};
		snprintf(text, sizeof(text), "constexpr HudTuning %s = {\n%s%s%s\tfalse,\n};\n",
			quads ? "HUD_TUNING_QUADS" : "HUD_TUNING_HALVES",
			fmt(tuning.topLeft).c_str(), fmt(tuning.topRight).c_str(), fmt(tuning.bottom).c_str());
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

	s_cockpitUpdateHook = std::make_unique<Utility::Detour_t>(
		(uintptr_t)appBaseAddr + COCKPIT_TOP_UPDATE_OFFSET, &CockpitUpdateHook, 0, "UpdateCUIDCockpit00_sub_14027C0F0");
	s_cockpitUpdateHook->Toggle(true);

	s_cockpitTopDrawHook = std::make_unique<Utility::Detour_t>(
		(uintptr_t)appBaseAddr + COCKPIT_TOP_DRAW_OFFSET, &CockpitTopDrawHook, 0, "DrawCUIDCockpit00_sub_14027F1F0");
	s_cockpitTopDrawHook->Toggle(true);

	s_cockpitBottomDrawHook = std::make_unique<Utility::Detour_t>(
		(uintptr_t)appBaseAddr + COCKPIT_BOTTOM_DRAW_OFFSET, &CockpitBottomDrawHook, 0, "DrawCUIDCockpit02_sub_140281120");
	s_cockpitBottomDrawHook->Toggle(true);
}

} // namespace CrimsonSplitscreen

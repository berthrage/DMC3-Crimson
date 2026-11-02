#include "CrimsonSplitscreen.hpp"

#include <algorithm>
#include <cfloat>
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>

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
// frame, built for one player, and the centre half of it is stored. The two most
// recent halves are composited side by side after the scene is drawn.

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

constexpr HudTuning HUD_TUNING_DEFAULT = {
	{ 0.500f, 0.500f, -21.5f, 50.0f },
	{ 0.500f, 0.500f, 39.0f, -112.0f },
	{ 0.500f, 0.500f, 0.0f, 112.0f },
	false,
};
HudTuning s_hudTuning = HUD_TUNING_DEFAULT;

float s_frameAspect = 16.0f / 9.0f;

constexpr size_t CAM_STATE_SIZE = 0x200;
constexpr size_t CAM_EYE_OFFSET = 0x70; // eye at 0x70, lookat at 0x80

struct Halves {
	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex[2];
	bool valid[2] = {};
	UINT width = 0;
	UINT height = 0;
	DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
};
Halves s_halves;

// Saved camera state per player, in two regions: the camera control object and
// the CameraData at pool[147].
struct CameraSlots {
	uint8_t state[2][2][CAM_STATE_SIZE] = {};
	bool valid[2] = {};
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
	return (g_viewedCameraPlayer == 1) ? 1 : 0;
}

bool SplitscreenActive() {
	return g_splitscreenRenderTest && activeConfig.Actor.playerCount >= 2;
}

void SwapCameraState(uint8_t* cameraControl, int framePlayer) {
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

	const int next = 1 - framePlayer;
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

void Composite(Render* render) {
	ID3D11Texture2D* frame = render->renderTargets[0].texture;
	D3D11_TEXTURE2D_DESC desc{};
	frame->GetDesc(&desc);
	if (desc.SampleDesc.Count != 1 || desc.Width < 8) {
		return;
	}
	const UINT halfW = desc.Width / 2;
	s_frameAspect = static_cast<float>(desc.Width) / static_cast<float>(desc.Height);

	if (s_halves.width != halfW || s_halves.height != desc.Height || s_halves.format != desc.Format || !s_halves.tex[0]) {
		D3D11_TEXTURE2D_DESC halfDesc{};
		halfDesc.Width            = halfW;
		halfDesc.Height           = desc.Height;
		halfDesc.MipLevels        = 1;
		halfDesc.ArraySize        = 1;
		halfDesc.Format           = desc.Format;
		halfDesc.SampleDesc.Count = 1;
		halfDesc.Usage            = D3D11_USAGE_DEFAULT;
		for (int i = 0; i < 2; ++i) {
			s_halves.tex[i].Reset();
			s_halves.valid[i] = false;
			if (FAILED(render->device->CreateTexture2D(&halfDesc, nullptr, s_halves.tex[i].GetAddressOf()))) {
				return;
			}
		}
		s_halves.width  = halfW;
		s_halves.height = desc.Height;
		s_halves.format = desc.Format;
	}

	auto* ctx = render->deviceContext;
	const int player = FramePlayer();

	// Cropping the centre keeps the aspect ratio.
	const D3D11_BOX centre = { desc.Width / 4, 0, 0, desc.Width / 4 + halfW, desc.Height, 1 };
	ctx->CopySubresourceRegion(s_halves.tex[player].Get(), 0, 0, 0, 0, frame, 0, &centre);
	s_halves.valid[player] = true;

	for (int i = 0; i < 2; ++i) {
		if (s_halves.valid[i]) {
			ctx->CopySubresourceRegion(frame, 0, static_cast<UINT>(i) * halfW, 0, 0, s_halves.tex[i].Get(), 0, nullptr);
		}
	}
}

// SubmitDrawList_sub_14032D8C0
__int64 __fastcall SubmitDrawListHook(int a1) {
	const auto original = s_submitDrawListHook->GetTrampoline<SubmitDrawListFn>();
	const __int64 result = original(a1);

	auto* render = reinterpret_cast<Render*>(appBaseAddr + RENDER_OFFSET);
	auto* cameraCtrl = *reinterpret_cast<uint8_t**>(appBaseAddr + CAMERA_CTRL_PTR_OFFSET);
	if (!g_splitscreenRenderTest || activeConfig.Actor.playerCount < 2) {
		s_camSlots.valid[0] = s_camSlots.valid[1] = false;
		g_splitscreenCamCopyValid[0] = g_splitscreenCamCopyValid[1] = false;
	} else if (render->device && render->deviceContext && render->renderTargets[0].texture && cameraCtrl) {
		Composite(render);
		SwapCameraState(cameraCtrl, FramePlayer());
	}
	return result;
}

// UpdatePlayerInput_sub_1401EBCF0 derives actor.cameraDirection from the live
// camera. Actors that belong to the other player's camera get that camera's
// eye/lookat for the duration of the call, otherwise movement flips between
// two diverging yaws every tick.
__int64 __fastcall UpdatePlayerInputHook(__int64 actor) {
	const auto original = s_updatePlayerInputHook->GetTrampoline<UpdatePlayerInputFn>();

	const int owner = *reinterpret_cast<uint8_t*>(actor + 0x1CACC); // newPlayerIndex
	uint8_t* camObj = nullptr;
	if (g_splitscreenRenderTest && activeConfig.Actor.playerCount >= 2 && owner < 2 && owner != g_viewedCameraPlayer && s_camSlots.valid[owner]) {
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
	uint8_t state[2][COCKPIT_STATE_END - COCKPIT_STATE_BEGIN] = {};
	bool valid[2] = {};
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
		s_cockpitSlots.valid[0] = s_cockpitSlots.valid[1] = false;
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
	float backup[COCKPIT_TOP_MATRIX_COUNT][16];
	for (size_t i = 0; i < COCKPIT_TOP_MATRIX_COUNT; ++i) {
		std::memcpy(backup[i], matrix(i), sizeof(backup[i]));
		ShrinkHudMatrix(matrix(i), IsRightHudElement(i) ? s_hudTuning.topRight : s_hudTuning.topLeft);
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
		ShrinkHudMatrix(matrix(i), s_hudTuning.bottom);
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

	if (!SplitscreenActive()) {
		ImGui::TextWrapped("Splitscreen is off (F6 with 2+ players). Values still apply once it is on.");
	}
	ImGui::Text("Frame aspect: %.3f", s_frameAspect);
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
	group("Top left (HP / DT / style)", s_hudTuning.topLeft);
	group("Top right (red orbs)", s_hudTuning.topRight);
	group("Bottom (weapons)", s_hudTuning.bottom);

	ImGui::Checkbox("Show half guides", &s_hudTuning.showGuides);
	if (s_hudTuning.showGuides) {
		auto* draw = ImGui::GetForegroundDrawList();
		const ImVec2 size = ImGui::GetIO().DisplaySize;
		const ImU32 colour = IM_COL32(255, 0, 255, 200);
		draw->AddLine(ImVec2(size.x * 0.5f, 0), ImVec2(size.x * 0.5f, size.y), colour);
		draw->AddRect(ImVec2(1, 1), ImVec2(size.x * 0.5f - 1, size.y - 1), colour);
		draw->AddRect(ImVec2(size.x * 0.5f + 1, 1), ImVec2(size.x - 1, size.y - 1), colour);
	}

	if (ImGui::Button("Reset")) {
		s_hudTuning = HUD_TUNING_DEFAULT;
	}
	ImGui::SameLine();
	if (ImGui::Button("Copy as code")) {
		char text[768];
		auto fmt = [](const HudGroupTuning& g) {
			char line[160];
			snprintf(line, sizeof(line), "\t{ %.3ff, %.3ff, %.1ff, %.1ff },\n", g.sizeScale, g.posScale, g.offsetX, g.offsetY);
			return std::string(line);
		};
		snprintf(text, sizeof(text), "constexpr HudTuning HUD_TUNING_DEFAULT = {\n%s%s%s\tfalse,\n};\n",
			fmt(s_hudTuning.topLeft).c_str(), fmt(s_hudTuning.topRight).c_str(), fmt(s_hudTuning.bottom).c_str());
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

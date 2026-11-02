#include "CrimsonSplitscreen.hpp"

#include <cstring>
#include <memory>

#include <d3d11.h>
#include <wrl/client.h>

#include "../Core/Core.hpp"
#include "Global.hpp"
#include "Utility/Detour.hpp"
#include "Config.hpp"
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
std::unique_ptr<Utility::Detour_t> s_submitDrawListHook;
std::unique_ptr<Utility::Detour_t> s_updatePlayerInputHook;

// g_viewedCameraPlayer is advanced right after every drawn frame, so while a
// frame is being drawn it still holds the player that frame was built for.
int FramePlayer() {
	return (g_viewedCameraPlayer == 1) ? 1 : 0;
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

} // namespace

namespace CrimsonSplitscreen {

void InitHooks() {
	s_submitDrawListHook = std::make_unique<Utility::Detour_t>(
		(uintptr_t)appBaseAddr + SUBMIT_DRAW_LIST_OFFSET, &SubmitDrawListHook, 0, "SubmitDrawList_sub_14032D8C0");
	s_submitDrawListHook->Toggle(true);

	s_updatePlayerInputHook = std::make_unique<Utility::Detour_t>(
		(uintptr_t)appBaseAddr + UPDATE_PLAYER_INPUT_OFFSET, &UpdatePlayerInputHook, 0, "UpdatePlayerInput_sub_1401EBCF0");
	s_updatePlayerInputHook->Toggle(true);
}

} // namespace CrimsonSplitscreen

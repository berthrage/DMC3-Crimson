#pragma once

#include <vector>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Xinput.h>
#include <dxgi.h>
#include <d3d11.h>
#include <d3d10.h>

#include "Core/DataTypes.hpp"
#include "Core/Input.hpp"
#include "Vars.hpp"

namespace DXGI {

	extern IDXGISwapChain* swapChain;

};

namespace D3D10 {

	extern ID3D10Device* device;
	extern ID3D10RenderTargetView* renderTargetView;

};

namespace D3D11 {

	extern ID3D11Device* device;
	extern ID3D11DeviceContext* deviceContext;
	extern ID3D11RenderTargetView* renderTargetView;

};

extern Keyboard keyboard;
extern Mouse    mouse;
extern Gamepad  gamepad;

namespace XI {

	extern XINPUT_STATE state;

};

extern bool g_show;
extern bool g_lastShow;
extern bool g_showMain;
extern bool g_showShop;

extern uint8 g_scene;

extern bool g_eventRun[EVENT::COUNT];

extern bool g_quicksilver;
extern bool g_disableCameraRotation;
extern bool g_haywireNeoGenerator;

#pragma region Actor

extern Vector<byte8*> g_playerActorBaseAddrs;

extern NewActorData g_defaultNewActorData[ENTITY_COUNT];

extern NewActorData g_newActorData[PLAYER_COUNT][CHARACTER_COUNT][ENTITY_COUNT];

void ClearActorData();

NewActorData& GetNewActorData
(
	uint8 playerIndex,
	uint8 characterIndex,
	uint8 entityIndex
);

#pragma endregion

extern uint8 g_helperIndices[CHANNEL::MAX];

extern float g_shopTimer;
extern float g_shopTimeout; // in ms

extern bool g_missionSelectForceConfirm;

extern float g_hitPoints[PLAYER_COUNT];
extern float g_magicPoints[PLAYER_COUNT];

void ToggleSkipIntro(bool enable);
void ToggleSkipCutscenes(bool enable);
bool InGame();

bool& IsTurbo();
bool& InCutscene();
bool InCredits();

extern new_size_t g_saveIndex;
extern new_size_t g_lastSaveIndex;

extern bool g_noTeleport;

#pragma region SecretMission

extern uint32 g_secretMission;

void ClearGlobalSecretMission();
void SetGlobalSecretMission();

#pragma endregion
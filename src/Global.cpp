// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "Global.hpp"
#include "Core/Core.hpp"
#include <Unknwn.h>

#include "Core/Macros.h"

#include "Core/DebugSwitch.hpp"

namespace DXGI {

IDXGISwapChain* swapChain = 0;

};

namespace D3D10 {

ID3D10Device* device                     = 0;
ID3D10RenderTargetView* renderTargetView = 0;

}; // namespace D3D10

namespace D3D11 {

ID3D11Device* device                     = 0;
ID3D11DeviceContext* deviceContext       = 0;
ID3D11RenderTargetView* renderTargetView = 0;

}; // namespace D3D11

Keyboard keyboard = {};
Mouse mouse       = {};
Gamepad gamepad   = {};

namespace XI {

XINPUT_STATE state = {};

};

bool g_show     = false;
bool g_lastShow = false;
bool g_showMain = false;
bool g_showShop = false;

uint8 g_scene = 0;

bool g_eventRun[EVENT::COUNT] = {};

bool g_quicksilver           = false;
bool g_disableCameraRotation = false;
bool g_haywireNeoGenerator   = false;


#pragma region Actor

Vector<byte8*> g_playerActorBaseAddrs = {};

NewActorData g_defaultNewActorData[ENTITY_COUNT] = {};

NewActorData g_newActorData[PLAYER_COUNT][CHARACTER_COUNT][ENTITY_COUNT] = {};


void ClearActorData() {
    LogFunction();

    g_playerActorBaseAddrs.Clear();

    SetMemory(g_defaultNewActorData, 0, sizeof(g_defaultNewActorData));

    SetMemory(g_newActorData, 0, sizeof(g_newActorData));
}

NewActorData& GetNewActorData(uint8 playerIndex, uint8 characterIndex, uint8 entityIndex) {
    return g_newActorData[playerIndex][characterIndex][entityIndex];
}

#pragma endregion


uint8 g_helperIndices[CHANNEL::MAX] = {};

float g_shopTimer   = 0;
float g_shopTimeout = 100; // in ms

bool g_missionSelectForceConfirm = false;

float g_hitPoints[PLAYER_COUNT]   = {};
float g_magicPoints[PLAYER_COUNT] = {};

// @Update
void ToggleSkipIntro(bool enable) {
    LogFunction(enable);

    WriteAddress((appBaseAddr + 0x2383F2), (enable) ? (appBaseAddr + 0x2383F8) : (appBaseAddr + 0x238527), 6); // Skip Message
    WriteAddress((appBaseAddr + 0x241789), (enable) ? (appBaseAddr + 0x24178B) : (appBaseAddr + 0x2417A6), 2); // Skip Video
    Write<uint8>((appBaseAddr + 0x238704), (enable) ? 0 : 1);                                                  // Hide Rebellion

    // Disable Video Timer
    {
        auto dest = (appBaseAddr + 0x243531);
        if (enable) {
            SetMemory(dest, 0x90, 2, MemoryFlags_VirtualProtectDestination);
        } else {
            constexpr byte8 buffer[] = {
                0xFF, 0xC8, // dec eax
            };
            CopyMemory(dest, buffer, sizeof(buffer), MemoryFlags_VirtualProtectDestination);
        }
    }
}

void ToggleSkipCutscenes(bool enable) {
    LogFunction(enable);

    WriteAddress((appBaseAddr + 0x238CCA), (enable) ? (appBaseAddr + 0x238CD0) : (appBaseAddr + 0x238E62), 6);
    /*
    dmc3.exe+238CCA - 0F85 92010000 - jne dmc3.exe+238E62
    dmc3.exe+238CD0 - 8B 43 1C      - mov eax,[rbx+1C]
    */

    WriteAddress((appBaseAddr + 0x23918A), (enable) ? (appBaseAddr + 0x23918C) : (appBaseAddr + 0x2391A4), 2);
    /*
    dmc3.exe+23918A - 75 18    - jne dmc3.exe+2391A4
    dmc3.exe+23918C - 8B 43 1C - mov eax,[rbx+1C]
    */
}

bool InGame() {
    if (g_scene != SCENE::GAME) {
        return false;
    }

    auto pool_205 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
    if (!pool_205 || !pool_205[8]) {
        return false;
    }
    auto& eventData = *reinterpret_cast<EventData*>(pool_205[8]);


    if (eventData.event != EVENT::MAIN) {
        return false;
    }

    return true;
}

bool& IsTurbo() {
    return *reinterpret_cast<bool*>(appBaseAddr + 0xD6CEA9);
}

bool& InCutscene() {
    return *reinterpret_cast<bool*>(appBaseAddr + 0x5D113D);
}

bool InCredits() {
    auto pool = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E28);
    if (!pool) {
        return false;
    }

    return *reinterpret_cast<bool*>(pool + 0x11);
}

new_size_t g_saveIndex     = 0;
new_size_t g_lastSaveIndex = 0;

bool g_noTeleport = false;

#pragma region SecretMission

uint32 g_secretMission = 0;

void ClearGlobalSecretMission() {
    LogFunction();

    g_secretMission = 0;
}

void SetGlobalSecretMission() {
    auto pool_260 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
    if (!pool_260 || !pool_260[8]) {
        return;
    }
    auto& eventData = *reinterpret_cast<EventData*>(pool_260[8]);

    auto pool_261 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
    if (!pool_261 || !pool_261[3]) {
        return;
    }
    auto& actorData = *reinterpret_cast<PlayerActorData*>(pool_261[3]);

    LogFunction();


    auto& room = eventData.room;
    auto& x    = actorData.position.x;
    auto& y    = actorData.position.y;
    auto& z    = actorData.position.z;


    g_secretMission = ((room == 5) && (x >= 5300) && (x <= 5500) && (z >= 2700) && (z <= 3000))       ? 1
                      : ((room == 113) && (x >= 2700) && (x <= 3000) && (z >= 1500) && (z <= 1700))   ? 2
                      : ((room == 127) && (x >= 2900) && (x <= 3100) && (z >= 1200) && (z <= 1400))   ? 3
                      : ((room == 300) && (x >= 5100) && (x <= 5300) && (z >= -200) && (z <= 0))      ? 4
                      : ((room == 209) && (x >= 1800) && (x <= 2200) && (z >= 2600) && (z <= 3000))   ? 5
                      : ((room == 206) && (x >= 10200) && (x <= 10400) && (z >= 7000) && (z <= 7200)) ? 6
                      : ((room == 212) && (x >= 2200) && (x <= 2500) && (z >= 3000) && (z <= 3300))   ? 7
                      : ((room == 230) && (x >= 3500) && (x <= 3800) && (z >= 3400) && (z <= 3600))   ? 8
                      : ((room == 233) && (x >= 2800) && (x <= 3100) && (z >= 1000) && (z <= 1300))   ? 9
                      : ((room == 146) && (x >= 3500) && (x <= 4000) && (z >= 2400) && (z <= 2600))   ? 10
                      : ((room == 140) && (x >= 2300) && (x <= 2600) && (z >= 2300) && (z <= 2600))   ? 11
                      : ((room == 402) && (x >= 2200) && (x <= 2500) && (z >= -3300) && (z <= -3000)) ? 12
                                                                                                      : 0;
}

#pragma endregion

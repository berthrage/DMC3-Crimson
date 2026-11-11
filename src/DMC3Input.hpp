#pragma once

#include <cstddef>

#include "Core/Core.hpp"
#include "Vars.hpp"
#include "Xinput.h"
#include "CrimsonConfig.hpp"
#include "CrimsonInput.hpp"

namespace TILT_DIRECTION {
enum {
    NEUTRAL,
    UP,
    RIGHT,
    DOWN,
    LEFT,
    MAX,
};
};

void ToggleCursor();

inline ENGINE_GAMEPAD& GetGamepad(uint8 index) {
    return (reinterpret_cast<ENGINE_GAMEPAD*>(appBaseAddr + 0xD54A10 + 0x550))[index];
}

#if 0
inline byte16 GetBinding(uint8 index) {
    return (reinterpret_cast<byte16*>(appBaseAddr + 0xD6CE80 + 0xA))[index];
}
#else
// The game's own bind table is its first 15 entries (up .. taunt) at dmc3.exe+D6CE8A. The two bytes right after it
// (dmc3.exe+D6CEA8 / +D6CEA9) are the game's Lock On Display and Turbo settings, so the Crimson-only entries
// (START, switch button) live in g_crimsonExtraBinds instead of the game's memory.
constexpr uint8 GAME_BIND_TABLE_ENTRIES = 15;
extern uint16 g_crimsonExtraBinds[2]; // START, SWITCH_BUTTON

inline byte16 GetBinding(uint8 index) {
    if (index >= GAME_BIND_TABLE_ENTRIES) {
        return g_crimsonExtraBinds[index - GAME_BIND_TABLE_ENTRIES];
    }
    return (reinterpret_cast<byte16*>(appBaseAddr + 0xD6CE80 + 0xA))[index];
}
// Created with ReClass.NET 1.2 by KN4CK3R
struct BindTable
{
public:
    uint16_t up; //0x0000
    uint16_t down; //0x0002
    uint16_t right; //0x0004
    uint16_t left; //0x0006
    uint16_t melee_atk; //0x0008
    uint16_t jump; //0x000A
    uint16_t style; //0x000C
    uint16_t shoot; //0x000E
    uint16_t dt; //0x0010
    uint16_t change_gun; //0x0012
    uint16_t change_target; //0x0014
    uint16_t lock_on; //0x0016
    uint16_t change_sword; //0x0018
    uint16_t default_camera; //0x001A
    uint16_t taunt; //0x001C
	uint16_t start; //0x001E
	uint16_t switch_button; //0x0020
}; //Size: 0x0022
static_assert(offsetof(BindTable, start) == GAME_BIND_TABLE_ENTRIES * sizeof(uint16_t));
// Bytes of the table the game owns; everything after is Crimson-only (see GAME_BIND_TABLE_ENTRIES).
constexpr size_t GAME_BIND_TABLE_SIZE = offsetof(BindTable, start);
struct CUIDControl;

extern CUIDControl* g_control_ui;

void InitBindings();
void SwapXInputButtonsCoop(uint8 index, XINPUT_STATE* gamepad);
void ShowButtonConfigWindow();
void CloseButtonConfigWindow();
void UpdateKeyboardConfigCapture(byte8* state);
void UpdateGamepadConfigCapture();
void StoreHDCKeybinds();
void OverrideHDCKeybinds();
extern bool g_showControllerRemap;
extern bool g_showKeyboardConfig;
extern uint32 g_hdcKeybinds[NUM_KEYBINDS];
extern CrimsonInput::BindPair(*activeConfigInputs[PLAYER_COUNT][2])[NUM_GAMEPADBINDS];

#endif

// The Doppelganger runs on delayed input, so its stick comes from its own (replayed) input block;
// the live pad is only right for the main actor.
inline uint16 GetActorLeftStickPosition(PlayerActorData& actorData) {
    if (actorData.newEntityIndex == ENTITY::CLONE) {
        return actorData.leftStickPosition;
    }
    return static_cast<uint16>(GetGamepad(actorData.newGamepad).leftStickPosition);
}

inline uint16 GetActorLeftStickRadius(PlayerActorData& actorData) {
    if (actorData.newEntityIndex == ENTITY::CLONE) {
        return actorData.leftStickRadius;
    }
    return static_cast<uint16>(GetGamepad(actorData.newGamepad).leftStickRadius);
}

inline uint16 GetRelativeTilt(PlayerActorData& actorData) {
    uint16 relativeTilt = 0;

    relativeTilt = (actorData.actorCameraDirection - GetActorLeftStickPosition(actorData));

    return relativeTilt;
}

inline uint8 GetRelativeTiltDirection(PlayerActorData& actorData) {
    uint16 relativeTilt = 0;
    using namespace ACTION_DANTE;

    if (GetActorLeftStickRadius(actorData) < LEFT_STICK_DEADZONE) {
        return TILT_DIRECTION::NEUTRAL;
    }

    relativeTilt = (actorData.actorCameraDirection - GetActorLeftStickPosition(actorData));

    {
        uint16 value = (relativeTilt - 0x6000);
        if (value <= 0x4000) {
            return TILT_DIRECTION::UP;
        }
    }
    {
        uint16 value = (relativeTilt + 0x6000);
        if (value <= 0x4000) {
            return TILT_DIRECTION::RIGHT;
        }
    }
    {
        uint16 value = (relativeTilt + 0x2000);
        if (value <= 0x4000) {
            return TILT_DIRECTION::DOWN;
        }
    }
    {
        uint16 value = (relativeTilt - 0x2000);
        if (value <= 0x4000) {
            return TILT_DIRECTION::LEFT;
        }
    }


    return TILT_DIRECTION::NEUTRAL;
}
void ShowCoopControllerRemapWindow();

void StoreHDCKeybinds();

void OverrideHDCKeybinds();

void ShowKeyboardConfigWindow();

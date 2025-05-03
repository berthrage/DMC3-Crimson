// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "DMC3Input.hpp"
#include "Config.hpp"
#include "Global.hpp"

#include <array>
#include <vector>
#include <memory>
#include "Core/DataTypes.hpp"
#include "Utility/Detour.hpp"
#include "Xinput.h"

static std::unique_ptr<Utility::Detour_t> s_GetBindingHook;

struct CrimsonXinputBinding {
    uint16 source, target;
    const char* name;
};

static BindTable s_defaultBinds = {
    .up = 0x1000,
    .down = 0x4000,
    .right = 0x2000,
    .left = 0x8000,
    .melee_atk = 0x10,
    .jump = 0x40,
    .style = 0x20,
    .dt = 0x4,
    .change_gun = 0x1,
    .change_target = 0x200,
    .lock_on = 0x8,
    .change_sword = 0x2,
    .default_camera = 0x400,
    .taunt = 0x100
};

static std::array<std::vector<CrimsonXinputBinding>, 4> s_bindsPlayer = {{

    // first player 
    {
        { XINPUT_GAMEPAD_DPAD_UP,        XINPUT_GAMEPAD_DPAD_UP,        "ITEM SCREEN (UP)" },
        { XINPUT_GAMEPAD_DPAD_DOWN,      XINPUT_GAMEPAD_DPAD_DOWN,      "EQUIP SCREEN (DOWN)" },
        { XINPUT_GAMEPAD_DPAD_RIGHT,     XINPUT_GAMEPAD_DPAD_RIGHT,     "MAP SCREEN (RIGHT)" },
        { XINPUT_GAMEPAD_DPAD_LEFT,      XINPUT_GAMEPAD_DPAD_LEFT,      "FILE SCREEN (LEFT)" },
        { XINPUT_GAMEPAD_B,              XINPUT_GAMEPAD_B,              "MEELE ATTACK"},
        { XINPUT_GAMEPAD_A,              XINPUT_GAMEPAD_A,              "JUMP" },
        { XINPUT_GAMEPAD_Y,              XINPUT_GAMEPAD_Y,              "STYLE ACTION" },
        { XINPUT_GAMEPAD_X,              XINPUT_GAMEPAD_X,              "SHOOT" },
        { XINPUT_GAMEPAD_LEFT_SHOULDER,  XINPUT_GAMEPAD_LEFT_SHOULDER,  "DEVIL TRIGGER" },
        { XINPUT_GAMEPAD_LEFT_THUMB,     XINPUT_GAMEPAD_LEFT_THUMB,     "CHANGE TARGET" },
        { XINPUT_GAMEPAD_RIGHT_SHOULDER, XINPUT_GAMEPAD_RIGHT_SHOULDER, "LOCK ON" },
        { XINPUT_GAMEPAD_RIGHT_THUMB,    XINPUT_GAMEPAD_RIGHT_THUMB,    "DEFAULT CAMERA" },
        { XINPUT_GAMEPAD_BACK,           XINPUT_GAMEPAD_BACK,           "TAUNT" }
    },
    // second player
    {
        { XINPUT_GAMEPAD_DPAD_UP,        XINPUT_GAMEPAD_DPAD_UP,        "ITEM SCREEN (UP)" },
        { XINPUT_GAMEPAD_DPAD_DOWN,      XINPUT_GAMEPAD_DPAD_DOWN,      "EQUIP SCREEN (DOWN)" },
        { XINPUT_GAMEPAD_DPAD_RIGHT,     XINPUT_GAMEPAD_DPAD_RIGHT,     "MAP SCREEN (RIGHT)" },
        { XINPUT_GAMEPAD_DPAD_LEFT,      XINPUT_GAMEPAD_DPAD_LEFT,      "FILE SCREEN (LEFT)" },
        { XINPUT_GAMEPAD_Y,              XINPUT_GAMEPAD_Y,              "MEELE ATTACK"},
        { XINPUT_GAMEPAD_A,              XINPUT_GAMEPAD_A,              "JUMP" },
        { XINPUT_GAMEPAD_B,              XINPUT_GAMEPAD_B,              "STYLE ACTION" },
        { XINPUT_GAMEPAD_X,              XINPUT_GAMEPAD_X,              "SHOOT" },
        { XINPUT_GAMEPAD_LEFT_SHOULDER,  XINPUT_GAMEPAD_LEFT_SHOULDER,  "DEVIL TRIGGER" },
        { XINPUT_GAMEPAD_LEFT_THUMB,     XINPUT_GAMEPAD_LEFT_THUMB,     "CHANGE TARGET" },
        { XINPUT_GAMEPAD_RIGHT_SHOULDER, XINPUT_GAMEPAD_RIGHT_SHOULDER, "LOCK ON" },
        { XINPUT_GAMEPAD_RIGHT_THUMB,    XINPUT_GAMEPAD_RIGHT_THUMB,    "DEFAULT CAMERA" },
        { XINPUT_GAMEPAD_BACK,           XINPUT_GAMEPAD_BACK,           "TAUNT" }
    },
    // third player
    {
        { XINPUT_GAMEPAD_DPAD_UP,        XINPUT_GAMEPAD_DPAD_UP,        "ITEM SCREEN (UP)" },
        { XINPUT_GAMEPAD_DPAD_DOWN,      XINPUT_GAMEPAD_DPAD_DOWN,      "EQUIP SCREEN (DOWN)" },
        { XINPUT_GAMEPAD_DPAD_RIGHT,     XINPUT_GAMEPAD_DPAD_RIGHT,     "MAP SCREEN (RIGHT)" },
        { XINPUT_GAMEPAD_DPAD_LEFT,      XINPUT_GAMEPAD_DPAD_LEFT,      "FILE SCREEN (LEFT)" },
        { XINPUT_GAMEPAD_Y,              XINPUT_GAMEPAD_Y,              "MEELE ATTACK"},
        { XINPUT_GAMEPAD_A,              XINPUT_GAMEPAD_A,              "JUMP" },
        { XINPUT_GAMEPAD_B,              XINPUT_GAMEPAD_B,              "STYLE ACTION" },
        { XINPUT_GAMEPAD_X,              XINPUT_GAMEPAD_X,              "SHOOT" },
        { XINPUT_GAMEPAD_LEFT_SHOULDER,  XINPUT_GAMEPAD_LEFT_SHOULDER,  "DEVIL TRIGGER" },
        { XINPUT_GAMEPAD_LEFT_THUMB,     XINPUT_GAMEPAD_LEFT_THUMB,     "CHANGE TARGET" },
        { XINPUT_GAMEPAD_RIGHT_SHOULDER, XINPUT_GAMEPAD_RIGHT_SHOULDER, "LOCK ON" },
        { XINPUT_GAMEPAD_RIGHT_THUMB,    XINPUT_GAMEPAD_RIGHT_THUMB,    "DEFAULT CAMERA" },
        { XINPUT_GAMEPAD_BACK,           XINPUT_GAMEPAD_BACK,           "TAUNT" }
    },
    // fourth player
    {
        { XINPUT_GAMEPAD_DPAD_UP,        XINPUT_GAMEPAD_DPAD_UP,        "ITEM SCREEN (UP)" },
        { XINPUT_GAMEPAD_DPAD_DOWN,      XINPUT_GAMEPAD_DPAD_DOWN,      "EQUIP SCREEN (DOWN)" },
        { XINPUT_GAMEPAD_DPAD_RIGHT,     XINPUT_GAMEPAD_DPAD_RIGHT,     "MAP SCREEN (RIGHT)" },
        { XINPUT_GAMEPAD_DPAD_LEFT,      XINPUT_GAMEPAD_DPAD_LEFT,      "FILE SCREEN (LEFT)" },
        { XINPUT_GAMEPAD_Y,              XINPUT_GAMEPAD_Y,              "MEELE ATTACK"},
        { XINPUT_GAMEPAD_A,              XINPUT_GAMEPAD_A,              "JUMP" },
        { XINPUT_GAMEPAD_B,              XINPUT_GAMEPAD_B,              "STYLE ACTION" },
        { XINPUT_GAMEPAD_X,              XINPUT_GAMEPAD_X,              "SHOOT" },
        { XINPUT_GAMEPAD_LEFT_SHOULDER,  XINPUT_GAMEPAD_LEFT_SHOULDER,  "DEVIL TRIGGER" },
        { XINPUT_GAMEPAD_LEFT_THUMB,     XINPUT_GAMEPAD_LEFT_THUMB,     "CHANGE TARGET" },
        { XINPUT_GAMEPAD_RIGHT_SHOULDER, XINPUT_GAMEPAD_RIGHT_SHOULDER, "LOCK ON" },
        { XINPUT_GAMEPAD_RIGHT_THUMB,    XINPUT_GAMEPAD_RIGHT_THUMB,    "DEFAULT CAMERA" },
        { XINPUT_GAMEPAD_BACK,           XINPUT_GAMEPAD_BACK,           "TAUNT" }
    }
}};

void ToggleCursor() {
    if (g_showMain || g_showShop) {
        Windows_ToggleCursor(true);
    } else {
        Windows_ToggleCursor(!activeConfig.hideMouseCursor);
    }
}

void InitBindings()
{
    BindTable* mainBinds = (BindTable*)(appBaseAddr + 0xD6CE80 + 0xA);
    //TODO(): check if coop is on
    //if (coopEnabled) {
    SetMemory((void*)mainBinds, (byte8)&s_defaultBinds, sizeof(BindTable));
    //}
}

void SwapXInputButtonsCoop(uint8 plindex, XINPUT_STATE* state) {
    // TODO(): idk how to tell if coop mode is on
    //if(!coopEnabled) { return; }
    std::vector<CrimsonXinputBinding>& playerBinds = s_bindsPlayer[plindex];
    std::vector<uint16> buttons(playerBinds.size(), 0x0);
    size_t btnIndex = 0;
    uint16 xinput = state->Gamepad.wButtons;
    for (CrimsonXinputBinding& bind : playerBinds) {
        if (xinput & bind.source) {
            buttons[btnIndex] = bind.target;
        }
        btnIndex += 1;
    }
    uint16 result = 0;
    for (uint16 button : buttons) {
        result |= button;
    }
    state->Gamepad.wButtons = result;
}


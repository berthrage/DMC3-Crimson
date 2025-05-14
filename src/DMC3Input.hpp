#pragma once

#include "Core/Core.hpp"
#include "Vars.hpp"
#include "Xinput.h"

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
inline byte16 GetBinding(uint8 index) {
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
}; //Size: 0x0020
void InitBindings();
void SwapXInputButtonsCoop(uint8 index, XINPUT_STATE* gamepad);

#endif

inline uint16 GetRelativeTilt(PlayerActorData& actorData) {
    auto& gamepad       = GetGamepad(0);
    uint16 relativeTilt = 0;


    relativeTilt = (actorData.actorCameraDirection - gamepad.leftStickPosition);

    return relativeTilt;
}

inline uint8 GetRelativeTiltDirection(PlayerActorData& actorData) {
    auto& gamepad       = GetGamepad(actorData.newPlayerIndex);
    uint16 relativeTilt = 0;
    using namespace ACTION_DANTE;

    if (gamepad.leftStickRadius < LEFT_STICK_DEADZONE) {
        return TILT_DIRECTION::NEUTRAL;
    }

    relativeTilt = (actorData.actorCameraDirection - gamepad.leftStickPosition);

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
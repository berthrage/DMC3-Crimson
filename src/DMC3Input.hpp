#pragma once

#include "Core/Core.hpp"
#include "Vars.hpp"

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

inline byte16 GetBinding(uint8 index) {
    return (reinterpret_cast<byte16*>(appBaseAddr + 0xD6CE80 + 0xA))[index];
}

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
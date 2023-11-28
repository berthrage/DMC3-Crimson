#pragma once

#include "Core.hpp"
#include <Xinput.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

struct InputDevice {
    IDirectInput8A* deviceInterface;
    IDirectInputDevice8A* device;
    bool init;


    bool CreateInterface();

    bool Create(const GUID& guid, const DIDATAFORMAT* dataFormat, DWORD setCooperativeLevelFlags);

    void Update(DWORD size, void* addr);
};

struct Keyboard : InputDevice {
    DI8::DIKEYBOARDSTATE state;

    bool Create();

    void Update();
};

struct Mouse : InputDevice {
    DIMOUSESTATE2 state;

    bool Create();

    void Update();
};

struct Gamepad : InputDevice {
    LPDIENUMDEVICESCALLBACKA enumFunc;
    DIDEVICEINSTANCEA deviceInstance;
    bool match;
    DIJOYSTATE state;

    bool Create();

    void Update();
};

namespace XI {
namespace GAMEPAD {
enum {
    UP             = 0x1,
    DOWN           = 0x2,
    LEFT           = 0x4,
    RIGHT          = 0x8,
    START          = 0x10,
    BACK           = 0x20,
    LEFT_THUMB     = 0x40,
    RIGHT_THUMB    = 0x80,
    LEFT_SHOULDER  = 0x100,
    RIGHT_SHOULDER = 0x200,
    A              = 0x1000,
    B              = 0x2000,
    X              = 0x4000,
    Y              = 0x8000,
};
};

typedef DWORD (*new_XInputGetState_t)(DWORD, XINPUT_STATE*);

extern new_XInputGetState_t new_XInputGetState;
void new_Init(const char* libName);
} // namespace XI
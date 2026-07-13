// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "DMC3Input.hpp"
#include "Config.hpp"
#include "CrimsonInput.hpp"
#include "Global.hpp"
#include "Vars.hpp"

#include <array>
#include <vector>
#include <memory>
#include <cstring>
#include <string>
#include "Core/DataTypes.hpp"
#include "Utility/Detour.hpp"
#include "Xinput.h"
#include "Core/GUI.hpp"
#include "Core/Input.hpp"
#include "ImGuiExtra.hpp"
#include "CrimsonGUI.hpp"
#include "CrimsonConfig.hpp"
#include "CrimsonSDL.hpp"
#include "Actor.hpp"

#define NUM_BINDS_WITHOUT_START 18

// NOTE(): writing bind table at that particular place seems to work fine for me... idk, needs testing?
static std::unique_ptr<Utility::Detour_t> s_ButtonToActionHook;
// NOTE(): to show custom imgoo menu when opening settings, idk if there is a better way tbqh
static std::unique_ptr<Utility::Detour_t> s_CUIDControlConsHook; // Constructor
static std::unique_ptr<Utility::Detour_t> s_CUIDControlDestHook; // Destructor
// Function-level detour on the game's XInput wrapper, covering call sites not patched by Hooks.cpp
static std::unique_ptr<Utility::Detour_t> s_XInputWrapperHook;
static std::unique_ptr<Utility::Detour_t> s_XInputGetStateHook;

// Per-player defaults (historical behavior). These are copied into per-character slots at runtime.
static BindTable s_PlayerDefaultBinds[PLAYER_COUNT] = {
    {
        .up = GAMEPAD::UP,
        .down = GAMEPAD::DOWN,
        .right = GAMEPAD::RIGHT,
        .left = GAMEPAD::LEFT,
        .melee_atk = GAMEPAD::Y,
        .jump = GAMEPAD::A,
        .style = GAMEPAD::B,
        .shoot = GAMEPAD::X,
        .dt = GAMEPAD::LEFT_SHOULDER,
        .change_gun = GAMEPAD::LEFT_TRIGGER,
        .change_target = GAMEPAD::LEFT_STICK_CLICK,
        .lock_on = GAMEPAD::RIGHT_SHOULDER,
        .change_sword = GAMEPAD::RIGHT_TRIGGER,
        .default_camera = GAMEPAD::RIGHT_STICK_CLICK,
        .taunt = GAMEPAD::BACK,
        .start = GAMEPAD::START,
        .switch_button = GAMEPAD::RIGHT_STICK_CLICK
    },
#ifdef MY_BINDS_TEST
    {
        .up = GAMEPAD::UP,
        .down = GAMEPAD::DOWN,
        .right = GAMEPAD::RIGHT,
        .left = GAMEPAD::LEFT,
        .melee_atk = GAMEPAD::A,
        .jump = GAMEPAD::B,
        .style = GAMEPAD::Y,
        .shoot = GAMEPAD::X,
        .dt = GAMEPAD::LEFT_SHOULDER,
        .change_gun = GAMEPAD::LEFT_TRIGGER,
        .change_target = GAMEPAD::LEFT_STICK_CLICK,
        .lock_on = GAMEPAD::RIGHT_SHOULDER,
        .change_sword = GAMEPAD::RIGHT_TRIGGER,
        .default_camera = GAMEPAD::RIGHT_STICK_CLICK,
        .taunt = GAMEPAD::BACK,
        .start = GAMEPAD::START,
        .switch_button = GAMEPAD::RIGHT_STICK_CLICK
    },
#else
    {
        .up = GAMEPAD::UP,
        .down = GAMEPAD::DOWN,
        .right = GAMEPAD::RIGHT,
        .left = GAMEPAD::LEFT,
        .melee_atk = GAMEPAD::Y,
        .jump = GAMEPAD::A,
        .style = GAMEPAD::B,
        .shoot = GAMEPAD::X,
        .dt = GAMEPAD::LEFT_SHOULDER,
        .change_gun = GAMEPAD::LEFT_TRIGGER,
        .change_target = GAMEPAD::LEFT_STICK_CLICK,
        .lock_on = GAMEPAD::RIGHT_SHOULDER,
        .change_sword = GAMEPAD::RIGHT_TRIGGER,
        .default_camera = GAMEPAD::RIGHT_STICK_CLICK,
        .taunt = GAMEPAD::BACK,
        .start = GAMEPAD::START,
        .switch_button = GAMEPAD::RIGHT_STICK_CLICK
    },
#endif
    {
        .up = GAMEPAD::UP,
        .down = GAMEPAD::DOWN,
        .right = GAMEPAD::RIGHT,
        .left = GAMEPAD::LEFT,
        .melee_atk = GAMEPAD::Y,
        .jump = GAMEPAD::A,
        .style = GAMEPAD::B,
        .shoot = GAMEPAD::X,
        .dt = GAMEPAD::LEFT_SHOULDER,
        .change_gun = GAMEPAD::LEFT_TRIGGER,
        .change_target = GAMEPAD::LEFT_STICK_CLICK,
        .lock_on = GAMEPAD::RIGHT_SHOULDER,
        .change_sword = GAMEPAD::RIGHT_TRIGGER,
        .default_camera = GAMEPAD::RIGHT_STICK_CLICK,
        .taunt = GAMEPAD::BACK,
        .start = GAMEPAD::START,
        .switch_button = GAMEPAD::RIGHT_STICK_CLICK
    },
    {
        .up = GAMEPAD::UP,
        .down = GAMEPAD::DOWN,
        .right = GAMEPAD::RIGHT,
        .left = GAMEPAD::LEFT,
        .melee_atk = GAMEPAD::Y,
        .jump = GAMEPAD::A,
        .style = GAMEPAD::B,
        .shoot = GAMEPAD::X,
        .dt = GAMEPAD::LEFT_SHOULDER,
        .change_gun = GAMEPAD::LEFT_TRIGGER,
        .change_target = GAMEPAD::LEFT_STICK_CLICK,
        .lock_on = GAMEPAD::RIGHT_SHOULDER,
        .change_sword = GAMEPAD::RIGHT_TRIGGER, 
        .default_camera = GAMEPAD::RIGHT_STICK_CLICK,
        .taunt = GAMEPAD::BACK,
        .start = GAMEPAD::START,
        .switch_button = GAMEPAD::RIGHT_STICK_CLICK
    },
};


// References to active/queued config inputs for each player and character.
// Each bind is a BindPair (slotA, slotB).
CrimsonInput::BindPair(*activeConfigInputs[PLAYER_COUNT][2])[NUM_GAMEPADBINDS] = {
	{ &activeCrimsonInput.ButtonConfig.dante1P,
	  &activeCrimsonInput.ButtonConfig.vergil1P },
	{ &activeCrimsonInput.ButtonConfig.dante2P,
	  &activeCrimsonInput.ButtonConfig.vergil2P },
	{ &activeCrimsonInput.ButtonConfig.dante3P,
	  &activeCrimsonInput.ButtonConfig.vergil3P },
	{ &activeCrimsonInput.ButtonConfig.dante4P,
	  &activeCrimsonInput.ButtonConfig.vergil4P }
};

CrimsonInput::BindPair(*queuedConfigInputs[PLAYER_COUNT][2])[NUM_GAMEPADBINDS] = {
	{ &queuedCrimsonInput.ButtonConfig.dante1P,
	  &queuedCrimsonInput.ButtonConfig.vergil1P },
	{ &queuedCrimsonInput.ButtonConfig.dante2P,
	  &queuedCrimsonInput.ButtonConfig.vergil2P },
	{ &queuedCrimsonInput.ButtonConfig.dante3P,
	  &queuedCrimsonInput.ButtonConfig.vergil3P },
	{ &queuedCrimsonInput.ButtonConfig.dante4P,
	  &queuedCrimsonInput.ButtonConfig.vergil4P }
};

// Default bindings in the new dual-slot format (BindPair).
// Slot A = traditional GAMEPAD value, Slot B = 0 unbound (except TAUNT: TOUCHPAD_RIGHT).
static const CrimsonInput::BindPair s_defaultBinds[NUM_GAMEPADBINDS] = {
	{ GAMEPAD::UP,                  0 },                          // UP
	{ GAMEPAD::DOWN,                0 },                          // DOWN
	{ GAMEPAD::RIGHT,               0 },                          // RIGHT
	{ GAMEPAD::LEFT,                0 },                          // LEFT
	{ GAMEPAD::Y,                   0 },                          // MELEE ATK
	{ GAMEPAD::A,                   0 },                          // JUMP
	{ GAMEPAD::B,                   0 },                          // STYLE
	{ GAMEPAD::X,                   0 },                          // SHOOT
	{ GAMEPAD::LEFT_SHOULDER,       0 },                          // DT
	{ GAMEPAD::LEFT_TRIGGER,        0 },                          // CHANGE GUN
	{ GAMEPAD::LEFT_STICK_CLICK,    0 },                          // CHANGE TARGET
	{ GAMEPAD::RIGHT_SHOULDER,      0 },                          // LOCK ON
	{ GAMEPAD::RIGHT_TRIGGER,       0 },                          // CHANGE SWORD
	{ GAMEPAD::RIGHT_STICK_CLICK,   0 },                          // DEFAULT CAMERA
	{ GAMEPAD::BACK,                GAMEPAD::TOUCHPAD_RIGHT },    // TAUNT
	{ GAMEPAD::START,               0 },                          // START
	{ GAMEPAD::RIGHT_STICK_CLICK,   0 },                          // DOPPEL_SWITCH_BUTTON (16)
	{ GAMEPAD::BACK,                GAMEPAD::TOUCHPAD_RIGHT },    // BACKWARDS SWITCH (17)
};

// Action names for the new UI (matches BTImGuiCtx old action names)
static const char* s_gamepadActionNames[NUM_BINDS_WITHOUT_START] = {
	"UP", "DOWN", "RIGHT", "LEFT",
	"MELEE ATK", "JUMP", "STYLE", "SHOOT",
	"DT", "CHANGE GUN", "CHANGE TARGET", "LOCK ON",
	"CHANGE SWORD", "DEFAULT CAMERA", "TAUNT", "START",
	"DOPPEL SWITCH", "BACKWARDS SWITCH",
};

static uint8_t GetCharacterBindSlot(const PlayerActorData* actorData) {
    if (!actorData) {
        return 0;
    }

    const auto characterIndex = static_cast<uint8_t>(actorData->character);

    // Only Dante/Vergil are relevant for activeButtonConfig switching.
    if (characterIndex == static_cast<uint8_t>(CHARACTER::VERGIL)) {
        return 1;
    }
    if (characterIndex == static_cast<uint8_t>(CHARACTER::DANTE)) {
        return 0;
    }

    // Fallback: if the game uses raw 0/1 slots, honor them.
    if (characterIndex == 1) {
        return 1;
    }

    // If CHARACTER_COUNT is a constexpr/enum (not a macro), #if won't work. Use a runtime check.
    if (CHARACTER_COUNT > 1 && characterIndex < CHARACTER_COUNT) {
        return characterIndex;
    }

    return 0;
}

static uint8_t GetCharacterBindSlotFromPlayerIndex(uint8 playerIndex) {
	PlayerActorData* actorDataPtr = nullptr;
	auto& playerData = GetPlayerData(playerIndex);

	if (activeConfig.Actor.enable) {
		auto& activeNewActorData = GetNewActorData(playerIndex, playerData.activeCharacterIndex, ENTITY::MAIN);
		if (activeNewActorData.baseAddr) {
			actorDataPtr = reinterpret_cast<PlayerActorData*>(activeNewActorData.baseAddr);
		}
	}
	else {
		actorDataPtr = GetVanillaPlayerActor();
	}
	if (!actorDataPtr) {
		return 0;
	}
	auto& actorData = *actorDataPtr;
	uint8 character = actorData.character;


	const auto characterIndex = static_cast<uint8_t>(actorData.character);

	// Only Dante/Vergil are relevant for activeButtonConfig switching.
	if (characterIndex == static_cast<uint8_t>(CHARACTER::VERGIL)) {
		return 1;
	}
	if (characterIndex == static_cast<uint8_t>(CHARACTER::DANTE)) {
		return 0;
	}

	// Fallback: if the game uses raw 0/1 slots, honor them.
	if (characterIndex == 1) {
		return 1;
	}

	// If CHARACTER_COUNT is a constexpr/enum (not a macro), #if won't work. Use a runtime check.
	if (CHARACTER_COUNT > 1 && characterIndex < CHARACTER_COUNT) {
		return characterIndex;
	}

	return 0;
}

static void __fastcall sub_1401EB170(PlayerActorData* a1) {
    if (!a1) {
        s_ButtonToActionHook->GetTrampoline<decltype(&sub_1401EB170)>()(a1);
        return;
    }

    BindTable* mainBinds = (BindTable*)(appBaseAddr + 0xD6CE80 + 0xA);

    const auto playerIndex = (static_cast<uint32_t>(a1->newPlayerIndex) < PLAYER_COUNT) ? a1->newPlayerIndex : 0;
    const auto characterSlot = GetCharacterBindSlot(a1);

    if (characterSlot >= 2) {
        s_ButtonToActionHook->GetTrampoline<decltype(&sub_1401EB170)>()(a1);
        return;
    }

    const CrimsonInput::BindPair* configBinds = (*activeConfigInputs[playerIndex][characterSlot]);

    // Combine both slots. Touchpad zones (> 0xFFFF) are excluded from BindTable
    // and handled via gamepad state injection in Hooked_XInputGetState.
    #define BINDVAL(idx) (uint16_t)( \
        ((configBinds[idx].slotA <= 0xFFFF ? configBinds[idx].slotA : 0) | \
         (configBinds[idx].slotB <= 0xFFFF ? configBinds[idx].slotB : 0)) & 0xFFFF)

    mainBinds->up              = BINDVAL(0);
    mainBinds->down            = BINDVAL(1);
    mainBinds->right           = BINDVAL(2);
    mainBinds->left            = BINDVAL(3);
    mainBinds->melee_atk       = BINDVAL(4);
    mainBinds->jump            = BINDVAL(5);
    mainBinds->style           = BINDVAL(6);
    mainBinds->shoot           = BINDVAL(7);
    mainBinds->dt              = BINDVAL(8);
    mainBinds->change_gun      = BINDVAL(9);
    mainBinds->change_target   = BINDVAL(10);
    mainBinds->lock_on         = BINDVAL(11);
    mainBinds->change_sword    = BINDVAL(12);
    mainBinds->default_camera  = BINDVAL(13);
    mainBinds->taunt           = BINDVAL(14);
    mainBinds->start           = BINDVAL(15);
    mainBinds->switch_button   = BINDVAL(16);

    // START-rebound BindTable correction 
    // When the START action (index 15) no longer includes GAMEPAD::START,
    // any other action bound ONLY to START would break: Hooked_XInputGetState
    // strips 0x0010 from wButtons, leaving nothing to match the BindTable.
    // Fix: replace those entries with the action's default button.
    if ((mainBinds->start & GAMEPAD::START) == 0) {
        uint16_t* fields = &mainBinds->up;
        for (int a = 0; a < 17; a++) {
            if (a == 15) continue;
            if (fields[a] == GAMEPAD::START) {
                fields[a] = (uint16_t)s_defaultBinds[a].slotA;
            }
        }
    }

    // Touchpad-only BindTable correction 
    // When an action's only bindings are touchpad zones (>0xFFFF), the
    // BindTable entry is 0. Fix by setting a non-conflicting standard
    // button so the game's comparison works when Hooked_XInputGetState
    // injects that button into wButtons. Runs here (not in the XInput
    // hook) so sub_1401EB170 never overwrites the correction.
    {
        uint16_t* fields = &mainBinds->up;

        // Read touchpad state for this player's physical slot
        int physSlot = (int)activeCrimsonInput.xinputSlots[playerIndex];
        SDL_Gamepad* pad = NULL;
        if (physSlot >= 0 && physSlot < 4)
            pad = CrimsonSDL::GetControllerByPhysicalSlot(physSlot);
        else if (physSlot >= 4) {
            size_t ei = (size_t)(physSlot - 4);
            if (ei < CrimsonSDL::sdlGamepadsExtra.size())
                pad = CrimsonSDL::sdlGamepadsExtra[ei];
        }
        uint32_t touchZone = pad ? CrimsonSDL::GetTouchpadZone(pad) : 0;

        // Snapshot physical buttons for suppression gating
        XINPUT_STATE xiState = {};
        bool xiValid = false;
        if (physSlot >= 0 && physSlot < 4) {
            if (CrimsonSDL::PopulateXInputStateFromSdlSlot(physSlot, &xiState))
                xiValid = true;
        }
        if (!xiValid && physSlot >= 0 && physSlot < 4) {
            if (XI::new_XInputGetState && XI::new_XInputGetState((DWORD)physSlot, &xiState) == ERROR_SUCCESS)
                xiValid = true;
        }
        uint32_t physicalButtons = xiValid ? GAMEPAD::FromXInput(xiState.Gamepad.wButtons) : 0;

        for (int a = 0; a < 17; a++) {
            if (fields[a] != 0) continue;

            uint32_t rawA = configBinds[a].slotA;
            uint32_t rawB = configBinds[a].slotB;
            bool hasTouchpad = (rawA > 0xFFFF) || (rawB > 0xFFFF);
            bool hasStandard = (rawA > 0 && rawA <= 0xFFFF) || (rawB > 0 && rawB <= 0xFFFF);
            if (hasStandard || !hasTouchpad) continue;

            // Only correct when the matching touchpad zone is pressed
            // (touchZone==0 means no touchpad press — leave BindTable at 0).
            if (touchZone == 0) continue;
            bool zoneMatches = (rawA > 0xFFFF && GAMEPAD::TouchpadZoneMatches(rawA, touchZone))
                            || (rawB > 0xFFFF && GAMEPAD::TouchpadZoneMatches(rawB, touchZone));
            if (!zoneMatches) continue;

            // Resolve a non-conflicting fallback button
            uint32_t btn = s_defaultBinds[a].slotA;

            bool conflicts = false;
            for (int other = 0; other < 17; other++) {
                if (other == a) continue;
                if (fields[other] & btn) { conflicts = true; break; }
            }

            if (conflicts) {
                static constexpr uint32_t candidates[] = {
                    GAMEPAD::LEFT_TRIGGER, GAMEPAD::RIGHT_TRIGGER,
                    GAMEPAD::LEFT_SHOULDER, GAMEPAD::RIGHT_SHOULDER,
                    GAMEPAD::Y, GAMEPAD::B, GAMEPAD::A, GAMEPAD::X,
                    GAMEPAD::BACK, GAMEPAD::LEFT_STICK_CLICK, GAMEPAD::RIGHT_STICK_CLICK,
                    GAMEPAD::UP, GAMEPAD::RIGHT, GAMEPAD::DOWN, GAMEPAD::LEFT,
                };
                bool found = false;
                for (uint32_t bit : candidates) {
                    if (bit == GAMEPAD::START) continue;
                    bool bitOk = true;
                    for (int other = 0; other < 17; other++) {
                        if (other == a) continue;
                        if (fields[other] & bit) { bitOk = false; break; }
                    }
                    if (bitOk) { btn = bit; found = true; break; }
                }

                if (!found && physicalButtons == 0) {
                    // Suppress conflicting button from other actions'
                    // BindTable entries (touchpad is sole input).
                    for (int other = 0; other < 17; other++) {
                        if (other == a) continue;
                        if (fields[other] & btn)
                            fields[other] &= ~((uint16_t)btn);
                    }
                }
            }

            fields[a] = (uint16_t)btn;
        }
    }

    #undef BINDVAL

    s_ButtonToActionHook->GetTrampoline<decltype(&sub_1401EB170)>()(a1);
}

class CUIDControl
{
public:
	char pad_0000[8]; //0x0000
	uint32_t someFlag; //0x0008
	uint32_t N000020E2; //0x000C
	char pad_0010[8]; //0x0010
	float N00000052; //0x0018
	char pad_001C[11812]; //0x001C
	uint8_t N0000062C; //0x2E40
	uint8_t cursor; //0x2E41
	uint8_t N000020DC; //0x2E42
	uint8_t N000020DF; //0x2E43
}; //Size: 0x2E44

CUIDControl* g_control_ui = nullptr;
bool g_showControllerRemap = false;
uint32 g_hdcKeybinds[NUM_KEYBINDS];

static CUIDControl* __fastcall CUIDControl__CUIDControl_sub_1402817C0(__int64 a1) {
    auto result = s_CUIDControlConsHook->GetTrampoline<decltype(&CUIDControl__CUIDControl_sub_1402817C0)>()(a1);
    g_control_ui = result;
    return result;
}

static uintptr_t __fastcall CUIDControl_Destructor_sub_140281840(uintptr_t block, char a2) {
    g_control_ui = nullptr;
    return s_CUIDControlDestHook->GetTrampoline<decltype(&CUIDControl_Destructor_sub_140281840)>()(block, a2);
}

static void CUIDControl_Close() {
    assert(g_control_ui && "CUIDControl instance is null!");
    g_control_ui->cursor = 15; // CUID exit button index
    g_control_ui->someFlag = 2; // idk some cWork thing?
}

static const char* GetXInputControllerName(DWORD userIndex) {
    // Sentinel values ≥ 4 indicate SDL slots
    if (userIndex >= 4) {
        static std::string s_sdlNames[8];
        size_t extrasIdx = (size_t)(userIndex - 4);
        const char* name = CrimsonSDL::GetControllerNameForXInputSlot((int)userIndex);
        if (name && name[0]) {
            s_sdlNames[extrasIdx] = name;
        } else {
            s_sdlNames[extrasIdx] = "SDL Controller";
        }
        return s_sdlNames[extrasIdx].c_str();
    }

    static std::string s_names[PLAYER_COUNT];
    static bool        s_isSdlName[PLAYER_COUNT] = { false };
    static DWORD       s_lastCheck[PLAYER_COUNT] = {};
    static DWORD       s_tick = 0;
    ++s_tick;

    if (userIndex >= (DWORD)PLAYER_COUNT) return "Unknown";

    // Refresh every ~300 calls (UI frames) to catch connect/disconnect
    if ((s_tick - s_lastCheck[userIndex]) <= 300u && !s_names[userIndex].empty())
        return s_names[userIndex].c_str();
    s_lastCheck[userIndex] = s_tick;

    XINPUT_CAPABILITIES caps = {};
    if (XInputGetCapabilities(userIndex, XINPUT_FLAG_GAMEPAD, &caps) != ERROR_SUCCESS) {
        s_names[userIndex] = "Disconnected";
        s_isSdlName[userIndex] = false;
        return s_names[userIndex].c_str();
    }

    // Prefer the real controller name from SDL (bypasses Steam Input emulated names).
    // Once we get an SDL name, stick with it — never fall back to HID names
    // for this slot, which prevents flickering between real/emulated names.
    const char* sdlName = CrimsonSDL::GetControllerNameForXInputSlot((int)userIndex);
    if (sdlName && sdlName[0]) {
        s_names[userIndex] = sdlName;
        s_isSdlName[userIndex] = true;
        return s_names[userIndex].c_str();
    }

    // If we previously had an SDL name, keep using it even if SDL temporarily
    // can't provide one (e.g. during controller remap). It will refresh next cycle.
    if (s_isSdlName[userIndex] && !s_names[userIndex].empty()) {
        return s_names[userIndex].c_str();
    }

    // Attempt to read the HID product string via Raw Input + dynamically loaded hid.dll
    auto tryGetProductName = [&]() -> std::string {
        UINT count = 0;
        if (GetRawInputDeviceList(nullptr, &count, sizeof(RAWINPUTDEVICELIST)) != 0 || count == 0)
            return {};

        std::vector<RAWINPUTDEVICELIST> list(count);
        if (GetRawInputDeviceList(list.data(), &count, sizeof(RAWINPUTDEVICELIST)) == (UINT)-1)
            return {};

        // Load hid.dll at runtime so hid.lib is not required
        static HMODULE hHid = LoadLibraryA("hid.dll");
        typedef BOOLEAN(WINAPI* PFN_HidD_GetProductString)(HANDLE, PVOID, ULONG);
        static auto pfnGetProductString = hHid
            ? (PFN_HidD_GetProductString)GetProcAddress(hHid, "HidD_GetProductString")
            : nullptr;

        // XInput HID devices contain "IG_" in their Raw Input path.
        // Their enumeration order corresponds to XInput user indices.
        DWORD xiSlot = 0;
        for (const auto& dev : list) {
            if (dev.dwType != RIM_TYPEHID) continue;

            UINT len = 0;
            GetRawInputDeviceInfoA(dev.hDevice, RIDI_DEVICENAME, nullptr, &len);
            if (len == 0) continue;

            std::string path(len, '\0');
            GetRawInputDeviceInfoA(dev.hDevice, RIDI_DEVICENAME, path.data(), &len);

            if (path.find("IG_") == std::string::npos) continue;
            if (xiSlot++ != userIndex) continue;

            if (pfnGetProductString) {
                HANDLE hDev = CreateFileA(path.c_str(), 0,
                    FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
                if (hDev != INVALID_HANDLE_VALUE) {
                    WCHAR ws[128] = {};
                    if (pfnGetProductString(hDev, ws, sizeof(ws))) {
                        char mb[128] = {};
                        WideCharToMultiByte(CP_UTF8, 0, ws, -1, mb, sizeof(mb), nullptr, nullptr);
                        CloseHandle(hDev);
                        return std::string(mb);
                    }
                    CloseHandle(hDev);
                }
            }
            break;
        }
        return {};
    };

    std::string name = tryGetProductName();
    if (!name.empty()) {
        s_names[userIndex] = name;
        return s_names[userIndex].c_str();
    }

    // Fallback: use capability subtype when the product string is unavailable
    switch (caps.SubType) {
        case XINPUT_DEVSUBTYPE_GAMEPAD:      s_names[userIndex] = "Gamepad";      break;
        case XINPUT_DEVSUBTYPE_WHEEL:        s_names[userIndex] = "Wheel";        break;
        case XINPUT_DEVSUBTYPE_ARCADE_STICK: s_names[userIndex] = "Arcade Stick"; break;
        case XINPUT_DEVSUBTYPE_FLIGHT_STICK: s_names[userIndex] = "Flight Stick"; break;
        case XINPUT_DEVSUBTYPE_DANCE_PAD:    s_names[userIndex] = "Dance Pad";    break;
        case XINPUT_DEVSUBTYPE_GUITAR:       s_names[userIndex] = "Guitar";       break;
        case XINPUT_DEVSUBTYPE_DRUM_KIT:     s_names[userIndex] = "Drum Kit";     break;
        default:                             s_names[userIndex] = "Controller";   break;
    }
    return s_names[userIndex].c_str();
}

static const char* s_keybindActionNames[NUM_KEYBINDS] = {
    "SELECT / TAUNT",
    "LB / DEVIL TRIGGER",
    "LS / CHANGE TARGET",
    "DPAD UP / ITEM SCREEN",
    "DPAD RIGHT / MAP SCREEN",
    "DPAD DOWN / EQUIP SCREEN",
    "DPAD LEFT / FILE SCREEN",
    "START",
    "RB / LOCK ON",
    "RS / DEFAULT CAMERA",
    "Y / MELEE ATK",
    "B / STYLE",
    "A / JUMP",
    "X / SHOOT",
    "LEFT ANALOG UP",
    "LEFT ANALOG RIGHT",
    "LEFT ANALOG DOWN",
    "LEFT ANALOG LEFT",
    "RIGHT ANALOG UP",
    "RIGHT ANALOG RIGHT",
    "RIGHT ANALOG DOWN",
    "RIGHT ANALOG LEFT",
    "LT / CHANGE GUN",
    "RT / CHANGE DEVIL ARM",
};

static const char* s_directWeaponActionNames[NUM_DIRECT_WEAPON_BINDS] = {
    "MELEE SLOT 1",
    "MELEE SLOT 2",
    "MELEE SLOT 3",
    "MELEE SLOT 4",
    "MELEE SLOT 5",
    "GUN SLOT 1",
    "GUN SLOT 2",
    "GUN SLOT 3",
    "GUN SLOT 4",
    "GUN SLOT 5",
};

struct KBCaptureState {
    bool   open        = false;
    int    index       = -1;
    uint32 previewKey  = 0;
    bool   executes[256] = {};
    bool   isDirectWeapon = false;
    bool   hasConflict = false;
    char   conflictText[96] = {};
};

static KBCaptureState s_kbCapture;
bool g_showKeyboardConfig = false;

static bool FindKeyboardBindConflict(uint32 key, bool isDirectWeapon, int index, const char** outName) {
    if (key == 0 || key >= 256) {
        return false;
    }

    const uint32* keybinds = activeCrimsonInput.KeyboardConfig.keybinds;
    for (int i = 0; i < NUM_KEYBINDS; i++) {
        if (!isDirectWeapon && i == index) {
            continue;
        }
        if (keybinds[i] == key) {
            *outName = s_keybindActionNames[i];
            return true;
        }
    }

    const uint32* directBinds = activeCrimsonInput.KeyboardConfig.directWeaponKeybinds;
    for (int i = 0; i < NUM_DIRECT_WEAPON_BINDS; i++) {
        if (isDirectWeapon && i == index) {
            continue;
        }
        if (directBinds[i] == key) {
            *outName = s_directWeaponActionNames[i];
            return true;
        }
    }

    return false;
}

struct GPCaptureState {
    bool   open          = false;
    int    playerIndex   = -1;
    int    actionIndex   = -1;
    int    slotIndex     = 0;     // 0 = A (primary), 1 = B (secondary)
    uint32_t previewButton = 0;
    bool   waitingForRelease = false; // skip the button that opened the popup
    std::chrono::steady_clock::time_point bHoldStart;
    bool   bHoldTracking = false;       // B-track is being held; updated every frame
};

static GPCaptureState s_gpCapture;

static std::array<int, PLAYER_COUNT> s_selectedCharacterSlotByPlayer = {};

void ShowButtonConfigWindow() {
	static bool s_wasOpen = false;
	bool isOpen = (g_control_ui || g_showControllerRemap || g_showKeyboardConfig);

	if (isOpen != s_wasOpen) {
		s_wasOpen = isOpen;
		ToggleCursor();
	}

	if (!isOpen) {
		return;
	}

	// Poll gamepad capture state while window is open
	UpdateGamepadConfigCapture();

	// If opened specifically via g_showKeyboardConfig, start on the keyboard tab.
	static int s_tab = 0; // 0 = Controller, 1 = Keyboard
	if (g_showKeyboardConfig && !g_showControllerRemap && !g_control_ui) {
		s_tab = 1;
	}

	const auto nplayers = queuedConfig.Actor.playerCount;
	bool shouldClose = false;

	static std::array<int, PLAYER_COUNT> s_comboSelection = {};

	const float scaleY      = CrimsonGUI::scaleFactorY;
	const float scaleF      = (CrimsonGUI::scaleFactorX + CrimsonGUI::scaleFactorY) * 0.5f;
	float width             = g_renderSize.x / 1.40f;
	float height            = g_renderSize.y / 1.10f;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,  ImVec2(20.0f * scaleF, 20.0f * scaleF));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 20.0f * scaleF);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.85f));

	ImGui::SetNextWindowSize(ImVec2(width, height));
	ImGui::SetNextWindowPos(ImVec2(g_renderSize.x * 0.5f, g_renderSize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

    auto& io = ImGui::GetIO();
    int keyboardBackKey = activeCrimsonInput.KeyboardConfig.keybinds[11];

	ImGui::Begin("Button Configuration", &shouldClose, ImGuiWindowFlags_NoTitleBar); {
		ImGui::SetWindowFontScale(scaleY);

		// Background faded text (like the "Divinity Statue" text in the shop)
		{
			const char* fadedText = "Button Configuration";
			ImFont* fadedFont = UI::g_ImGuiFont_Benguiat256;
			float fadedFontSize = CrimsonGUI::scaledFontSize * 4.8f;
			ImVec2 fadedTextSize = ImGui::CalcTextSize(fadedText, nullptr, false, fadedFontSize);
			ImVec2 winPos = ImGui::GetWindowPos();
			ImVec2 winSize = ImGui::GetWindowSize();
			float rightMargin = 780.0f * scaleY;
			float x = winPos.x + winSize.x - fadedTextSize.x - rightMargin;
			float y = winPos.y + 15.0f * scaleY;
			ImGui::GetWindowDrawList()->AddText(
				fadedFont, fadedFontSize,
				ImVec2(x, y),
				UI::SwapColorEndianness(0xFFFFFF10),
				fadedText
			);
		}

		if (GUI_CloseX() || (io.KeysDown[keyboardBackKey] && (io.KeysDownDuration[keyboardBackKey] == 0.0f))
            || (io.KeysDown[DI8::KEY::ESCAPE] && (io.KeysDownDuration[DI8::KEY::ESCAPE] == 0.0f))) {
			if (g_control_ui) {
				CUIDControl_Close();
			}
			g_showControllerRemap = false;
			g_showKeyboardConfig  = false;
		}

		ImGui::Text("");

		// Tab buttons
		const auto defaultFontSize  = UI::g_UIContext.DefaultFontSize;
		const ImVec4 tabActiveColor = ImVec4(0.79f, 0.06f, 0.21f, 1.0f);
		ImGui::PushFont(UI::g_ImGuiFont_Benguiat[defaultFontSize * 1.1f]);
		ImGui::PushStyleColor(ImGuiCol_Button, s_tab == 0 ? tabActiveColor : ImGui::GetStyle().Colors[ImGuiCol_Button]);
		if (ImGui::Button("CONTROLLERS")) { s_tab = 0; }
		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, s_tab == 1 ? tabActiveColor : ImGui::GetStyle().Colors[ImGuiCol_Button]);
		if (ImGui::Button("KEYBOARD - 1P")) { s_tab = 1; }
		ImGui::PopStyleColor();
		ImGui::PopFont();

		ImGui::Separator();
		ImGui::Text("");

		if (s_tab == 0) {
			// ======================== CONTROLLER TAB ========================
			char buffer[64] = {};
			const float gpRowLabelX = 170.0f * scaleY;
			const float gpSlotButtonW = 172.5f * scaleY;
			const float gpClearButtonW = 22.0f * scaleY;
			const float gpListHeight = height - 280.0f * scaleY;

			ImGui::BeginChild("##gp_scroll", ImVec2(0, gpListHeight), false); {

		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(20.0f * scaleF, 20.0f * scaleF));
			if (ImGui::BeginTable("##players", 2, ImGuiTableFlags_BordersInnerV)) {

			for (int i = 0; i < nplayers && i < PLAYER_COUNT; i++) {
				ImGui::TableNextColumn();
				sprintf(buffer, "%dP", i + 1);
				ImGui::Separator();
				ImGui::Text(buffer);
				ImGui::SameLine();
				ImGui::TextDisabled("[%s]", GetXInputControllerName((DWORD)activeCrimsonInput.xinputSlots[i]));

				// XInput/SDL slot selector (cached)
				{
					static char s_slotLabels[4 + 8][64];
					static const char* s_slotPtrs[4 + 8];
					static int s_slotCount = 0;
					static size_t s_cachedSdlExtrasSize = (size_t)-1;

					if (s_cachedSdlExtrasSize != CrimsonSDL::sdlGamepadsExtra.size()) {
						s_cachedSdlExtrasSize = CrimsonSDL::sdlGamepadsExtra.size();
						s_slotCount = 0;
						for (int s = 0; s < 4; s++) {
							snprintf(s_slotLabels[s_slotCount], sizeof(s_slotLabels[s_slotCount]),
								"XInput %d  [%s]", s, GetXInputControllerName((DWORD)s));
							s_slotPtrs[s_slotCount] = s_slotLabels[s_slotCount];
							s_slotCount++;
						}
						for (size_t s = 0; s < CrimsonSDL::sdlGamepadsExtra.size() && s_slotCount < 12; s++) {
							const char* name = "Unknown";
							if (CrimsonSDL::sdlGamepadsExtra[s] != NULL) {
								name = CrimsonSDL::GetControllerNameForXInputSlot((int)(s + 4));
								if (!name || !name[0]) name = "SDL Controller";
							}
							snprintf(s_slotLabels[s_slotCount], sizeof(s_slotLabels[s_slotCount]),
								"SDL %zu  [%s]", s, name);
							s_slotPtrs[s_slotCount] = s_slotLabels[s_slotCount];
							s_slotCount++;
						}
					}

					int slotCount = s_slotCount;
					uint8 cfgSlot = activeCrimsonInput.xinputSlots[i];
					int currentSlot = (cfgSlot >= 4) ? (int)(cfgSlot - 4 + 4) : (int)cfgSlot;
					sprintf(buffer, "##ctrl%d", i);
					if (ImGui::Combo(buffer, &currentSlot, s_slotPtrs, slotCount)) {
						if (currentSlot < 4) {
							activeCrimsonInput.xinputSlots[i] = (uint8)currentSlot;
							queuedCrimsonInput.xinputSlots[i] = (uint8)currentSlot;
						} else {
							uint8 sentinel = (uint8)(4 + (currentSlot - 4));
							activeCrimsonInput.xinputSlots[i] = sentinel;
							queuedCrimsonInput.xinputSlots[i] = sentinel;
						}
						GUI::save = true;
					}
					ImGui::SameLine();
					sprintf(buffer, "<##prev%d", i);
					if (ImGui::Button(buffer)) {
						currentSlot = (currentSlot == 0) ? (slotCount - 1) : (currentSlot - 1);
						s_comboSelection[i] = currentSlot;
						if (currentSlot < 4) {
							activeCrimsonInput.xinputSlots[i] = (uint8)currentSlot;
							queuedCrimsonInput.xinputSlots[i] = (uint8)currentSlot;
						} else {
							uint8 sentinel = (uint8)(4 + (currentSlot - 4));
							activeCrimsonInput.xinputSlots[i] = sentinel;
							queuedCrimsonInput.xinputSlots[i] = sentinel;
						}
						GUI::save = true;
					}
					ImGui::SameLine();
					sprintf(buffer, ">##next%d", i);
					if (ImGui::Button(buffer)) {
						currentSlot = (currentSlot + 1) % slotCount;
						s_comboSelection[i] = currentSlot;
						if (currentSlot < 4) {
							activeCrimsonInput.xinputSlots[i] = (uint8)currentSlot;
							queuedCrimsonInput.xinputSlots[i] = (uint8)currentSlot;
						} else {
							uint8 sentinel = (uint8)(4 + (currentSlot - 4));
							activeCrimsonInput.xinputSlots[i] = sentinel;
							queuedCrimsonInput.xinputSlots[i] = sentinel;
						}
						GUI::save = true;
					}
				}


				// Dante/Vergil character tabs
				int& selectedSlot = s_selectedCharacterSlotByPlayer[i];
				if (selectedSlot < 0 || selectedSlot >= CHARACTER_COUNT) {
					selectedSlot = 0;
				}
				if (CHARACTER_COUNT > 1) {
					ImGui::Spacing();
					ImGui::PushID(i);
					ImGui::Text("Character:");
					ImGui::SameLine();
					if (ImGui::BeginTabBar("##CharacterTabs")) {
						if (ImGui::BeginTabItem("Dante")) {
							selectedSlot = 0;
							ImGui::EndTabItem();
						}
						if (ImGui::BeginTabItem("Vergil")) {
							selectedSlot = 1;
							ImGui::EndTabItem();
						}
						ImGui::EndTabBar();
					}
					ImGui::PopID();
				}

				CrimsonInput::BindPair* activeButtonConfig  = (*activeConfigInputs[i][selectedSlot]);
				CrimsonInput::BindPair* queuedButtonConfig  = (*queuedConfigInputs[i][selectedSlot]);

				for (int j = 0; j < NUM_BINDS_WITHOUT_START; j++) {
					const bool isSharedAction = (j >= 16); // DOPPEL_SWITCH_BUTTON and BACKWARDS_SWITCH

					if (j == 16) { // separator before DOPPEL_SWITCH_BUTTON
						ImGui::Spacing();
						ImGui::Separator();
						ImGui::TextDisabled("Per-Player (shared across characters)");
					}

					ImGui::PushID(i * 100 + j);

					// Action label
					ImGui::Text("%s", s_gamepadActionNames[j]);
					if (j == 16 && ImGui::IsItemHovered()) {
						ImGui::SetTooltip("Hold the button while pressing CHANGE GUN/CHANGE SWORD to switch Doppelganger's weapons while it's active.\n"
							"Double Tap D-pad Up to switch characters. The Doppel Switch Button can also be set to Switch Characters with a toggle.");
					}
					if (j == 17 && ImGui::IsItemHovered()) {
						ImGui::SetTooltip("Hold this button while switching weapons to reverse direction.");
					}

					// Slot A button
					ImGui::SameLine(gpRowLabelX);
					ImGui::PushID(0); // slot 0
					uint32* slotAPtr = isSharedAction
						? &(*activeConfigInputs[i][0])[j].slotA  // shared binds always read Dante
						: &activeButtonConfig[j].slotA;
					const char* slotAName = GAMEPAD::ButtonName(*slotAPtr);
					sprintf(buffer, "%s##A", slotAName);
					if (GUI_Button(buffer, ImVec2(gpSlotButtonW, 0))) {
						if (!s_gpCapture.open) {
							s_gpCapture.open          = true;
							s_gpCapture.playerIndex   = i;
							s_gpCapture.actionIndex   = j;
							s_gpCapture.slotIndex     = 0;
							s_gpCapture.previewButton = *slotAPtr;
							s_gpCapture.waitingForRelease = true;
						}
					}
					if (j == 16 && ImGui::IsItemHovered()) {
						ImGui::SetTooltip("Hold the button while pressing CHANGE GUN/CHANGE SWORD to switch Doppelganger's weapons while it's active.\n"
							"Double Tap D-pad Up to switch characters. The Doppel Switch Button can also be set to Switch Characters with a toggle.");
					}
					if (j == 17 && ImGui::IsItemHovered()) {
						ImGui::SetTooltip("Hold this button while switching weapons to reverse direction.");
					}
					ImGui::SameLine();
					// Clear slot A
					sprintf(buffer, "X##clrA");
					if (GUI_Button(buffer, ImVec2(gpClearButtonW, 0))) {
						*slotAPtr = 0;
						if (isSharedAction) {
							(*activeConfigInputs[i][1])[j].slotA = 0;
							(*queuedConfigInputs[i][1])[j].slotA = 0;
						}
						queuedButtonConfig[j].slotA = 0;
						GUI::save = true;
					}
					ImGui::PopID();

					// Slot B button
					ImGui::SameLine(gpRowLabelX + gpSlotButtonW + gpClearButtonW + 10.0f * scaleF);
					ImGui::PushID(1); // slot 1
					uint32* slotBPtr = isSharedAction
						? &(*activeConfigInputs[i][0])[j].slotB
						: &activeButtonConfig[j].slotB;
					const char* slotBName = GAMEPAD::ButtonName(*slotBPtr);
					sprintf(buffer, "%s##B", slotBName);
					if (GUI_Button(buffer, ImVec2(gpSlotButtonW, 0))) {
						if (!s_gpCapture.open) {
							s_gpCapture.open          = true;
							s_gpCapture.playerIndex   = i;
							s_gpCapture.actionIndex   = j;
							s_gpCapture.slotIndex     = 1;
							s_gpCapture.previewButton = *slotBPtr;
							s_gpCapture.waitingForRelease = true;
						}
					}
					if (j == 16 && ImGui::IsItemHovered()) {
						ImGui::SetTooltip("Hold the button while pressing CHANGE GUN/CHANGE SWORD to switch Doppelganger's weapons while it's active.\n"
							"Double Tap D-pad Up to switch characters. The Doppel Switch Button can also be set to Switch Characters with a toggle.");
					}
					if (j == 17 && ImGui::IsItemHovered()) {
						ImGui::SetTooltip("Hold this button while switching weapons to reverse direction.");
					}
					ImGui::SameLine();
					// Clear slot B
					sprintf(buffer, "X##clrB");
					if (GUI_Button(buffer, ImVec2(gpClearButtonW, 0))) {
						*slotBPtr = 0;
						if (isSharedAction) {
							(*activeConfigInputs[i][1])[j].slotB = 0;
							(*queuedConfigInputs[i][1])[j].slotB = 0;
						}
						queuedButtonConfig[j].slotB = 0;
						GUI::save = true;
					}
					ImGui::PopID();

					ImGui::PopID(); // i*100 + j
				}

				// Doppel Switch character switch toggle
				GUI_Checkbox2("Use Doppel Switch Button for Character Switching",
					activeCrimsonInput.switchButtonCharSwitch[i],
					queuedCrimsonInput.switchButtonCharSwitch[i]);

				// Vibration intensity
			GUI_Slider2Float("Vibration Intensity",
				activeCrimsonInput.vibrationIntensity[i],
				queuedCrimsonInput.vibrationIntensity[i],
				0.0f, 100.0f, 1.0f, "%.0f%%");
				// Restore Defaults
				if (GUI_Button("Restore Defaults")) {
					for (int j = 0; j < NUM_BINDS_WITHOUT_START; j++) {
						activeButtonConfig[j].slotA = s_defaultBinds[j].slotA;
						activeButtonConfig[j].slotB = s_defaultBinds[j].slotB;
						queuedButtonConfig[j].slotA = s_defaultBinds[j].slotA;
						queuedButtonConfig[j].slotB = s_defaultBinds[j].slotB;
					}
					// Sync shared actions (DOPPEL_SWITCH_BUTTON and BACKWARDS_SWITCH) to Vergil
					for (int j = 16; j < NUM_GAMEPADBINDS; j++) {
						(*activeConfigInputs[i][1])[j].slotA = s_defaultBinds[j].slotA;
						(*activeConfigInputs[i][1])[j].slotB = s_defaultBinds[j].slotB;
						(*queuedConfigInputs[i][1])[j].slotA = s_defaultBinds[j].slotA;
						(*queuedConfigInputs[i][1])[j].slotB = s_defaultBinds[j].slotB;
					}

					activeCrimsonInput.switchButtonCharSwitch[i] = defaultCrimsonInput.switchButtonCharSwitch[i];
					queuedCrimsonInput.switchButtonCharSwitch[i] = defaultCrimsonInput.switchButtonCharSwitch[i];
					activeCrimsonInput.vibrationIntensity[i] = defaultCrimsonInput.vibrationIntensity[i];
					queuedCrimsonInput.vibrationIntensity[i] = defaultCrimsonInput.vibrationIntensity[i];
					GUI::save = true;
				}
			}

			ImGui::EndTable();
			ImGui::PopStyleVar();
			}
			}
			ImGui::EndChild();

		} else {
			// ======================== KEYBOARD TAB ========================
			uint32* activeKeybinds  = activeCrimsonInput.KeyboardConfig.keybinds;
			uint32* queuedKeybinds  = queuedCrimsonInput.KeyboardConfig.keybinds;
			uint32* defaultKeybinds = defaultCrimsonInput.KeyboardConfig.keybinds;

            uint32* dwActiveKeybinds = activeCrimsonInput.KeyboardConfig.directWeaponKeybinds;
            uint32* dwQueuedKeybinds = queuedCrimsonInput.KeyboardConfig.directWeaponKeybinds;

            const float kbRowLabelX = 210.0f * scaleY;
            const float kbButtonW   = 150.0f * scaleY;
            const float kbListHeight = height - 230.0f * scaleY;

            if (ImGui::BeginTable("##kb_table", 2)) {
                ImGui::TableSetupColumn("KeyboardBinds", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("DirectWeaponBinds", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableNextRow();

                ImGui::TableNextColumn();
                ImGui::BeginChild("##kb_scroll", ImVec2(0, kbListHeight), false); {
                    for (int i = 0; i < NUM_KEYBINDS; i++) {
                        ImGui::PushID(i);
                        ImGui::Text(s_keybindActionNames[i]);
                        ImGui::SameLine(kbRowLabelX);
                        const char* keyName = (activeKeybinds[i] > 0 && activeKeybinds[i] < 256) ? DI8::keyNames[activeKeybinds[i]] : "UNBOUND";
                        if (GUI_Button(keyName, ImVec2(kbButtonW, 0))) {
                            s_kbCapture.open       = true;
                            s_kbCapture.index      = i;
                            s_kbCapture.previewKey = activeKeybinds[i];
                            s_kbCapture.hasConflict = false;
                            s_kbCapture.conflictText[0] = '\0';
                            memset(s_kbCapture.executes, 0, sizeof(s_kbCapture.executes));
                        }
                        ImGui::SameLine();
                        if (GUI_Button("X", ImVec2(22.0f * scaleY, 0))) {
                            activeKeybinds[i] = 0;
                            queuedKeybinds[i] = 0;
                            byte8* addr = (appBaseAddr + 0x5611A0) + (i * 4);
                            protectionHelper.Push(addr, 4);
                            *(uint32_t*)addr = 0;
                            protectionHelper.Pop();
                            GUI::save = true;
                        }
                        ImGui::PopID();
                    }
                }
                ImGui::EndChild();

                ImGui::TableNextColumn();
                ImGui::BeginChild("##dw_scroll", ImVec2(0, kbListHeight), false); {
                    ImGui::Text("DIRECT WEAPON SELECTION");
                    ImGui::Separator();
                    for (int i = 0; i < NUM_DIRECT_WEAPON_BINDS; i++) {
                        ImGui::PushID(1000 + i);
                        ImGui::Text(s_directWeaponActionNames[i]);
                        ImGui::SameLine(kbRowLabelX);
                        const char* keyName = (dwActiveKeybinds[i] > 0 && dwActiveKeybinds[i] < 256) ? DI8::keyNames[dwActiveKeybinds[i]] : "UNBOUND";
                        if (GUI_Button(keyName, ImVec2(kbButtonW, 0))) {
                            s_kbCapture.open          = true;
                            s_kbCapture.index         = i;
                            s_kbCapture.isDirectWeapon = true;
                            s_kbCapture.previewKey    = dwActiveKeybinds[i];
                            s_kbCapture.hasConflict = false;
                            s_kbCapture.conflictText[0] = '\0';
                            memset(s_kbCapture.executes, 0, sizeof(s_kbCapture.executes));
                        }
                        ImGui::SameLine();
                        if (GUI_Button("X", ImVec2(22.0f * scaleY, 0))) {
                            dwActiveKeybinds[i] = 0;
                            dwQueuedKeybinds[i] = 0;
                            GUI::save = true;
                        }
                        ImGui::PopID();
                    }
                }
                ImGui::EndChild();

                ImGui::EndTable();
            }

			ImGui::Separator();

			if (GUI_Button("Reset All to Crimson Defaults")) {
				memcpy(activeKeybinds,  defaultKeybinds, sizeof(uint32) * NUM_KEYBINDS);
				memcpy(queuedKeybinds, defaultKeybinds, sizeof(uint32) * NUM_KEYBINDS);
				byte8* baseAddr = appBaseAddr + 0x5611A0;
				protectionHelper.Push(baseAddr, NUM_KEYBINDS * 4);
				for (int i = 0; i < NUM_KEYBINDS; i++) {
					*(uint32_t*)(baseAddr + (i * 4)) = defaultKeybinds[i];
				}
				protectionHelper.Pop();
				memset(dwActiveKeybinds, 0, sizeof(uint32) * NUM_DIRECT_WEAPON_BINDS);
				memset(dwQueuedKeybinds, 0, sizeof(uint32) * NUM_DIRECT_WEAPON_BINDS);
				GUI::save = true;
			}
			ImGui::SameLine();
			if (GUI_Button("Reset All to HDC Launcher Configs")) {
				memcpy(activeKeybinds,  g_hdcKeybinds, sizeof(uint32) * NUM_KEYBINDS);
				memcpy(queuedKeybinds, g_hdcKeybinds, sizeof(uint32) * NUM_KEYBINDS);
				byte8* baseAddr = appBaseAddr + 0x5611A0;
				protectionHelper.Push(baseAddr, NUM_KEYBINDS * 4);
				for (int i = 0; i < NUM_KEYBINDS; i++) {
					*(uint32_t*)(baseAddr + (i * 4)) = g_hdcKeybinds[i];
				}
				protectionHelper.Pop();
				GUI::save = true;
			}
		}
	}
	ImGui::End();
	ImGui::PopStyleVar(4);
	ImGui::PopStyleColor();

	// Keyboard capture popup (shown above the main window when capturing a key)
	if (s_kbCapture.open && s_kbCapture.index >= 0) {
		const float popupScale = scaleF;
		float popupWidth  = 500.0f * popupScale;
		float popupHeight = 260.0f * popupScale;

		ImGui::SetNextWindowSize(ImVec2(popupWidth, popupHeight));
		ImGui::SetNextWindowPos(ImVec2((g_renderSize.x - popupWidth) * 0.5f, (g_renderSize.y - popupHeight) * 0.5f));

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,  ImVec2(20.0f * popupScale, 20.0f * popupScale));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 20.0f * popupScale);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.92f));

		bool popupOpen = s_kbCapture.open;
		if (ImGui::Begin("KBRebind", &popupOpen, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize)) {
			ImGui::SetWindowFontScale(popupScale);

			auto kbFontSize = UI::g_UIContext.DefaultFontSize;
			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[kbFontSize * 1.2f]);
			const char* rebindName = s_kbCapture.isDirectWeapon
				? s_directWeaponActionNames[s_kbCapture.index]
				: s_keybindActionNames[s_kbCapture.index];
			ImGui::Text("Rebinding: %s", rebindName);
			ImGui::SameLine();
			if (GUI_CloseX()) {
				s_kbCapture.open          = false;
				s_kbCapture.index         = -1;
				s_kbCapture.isDirectWeapon = false;
			}
			ImGui::PopFont();

			ImGui::Text("");
			ImGui::Text("");

			const char* previewName = (s_kbCapture.previewKey < 256) ? DI8::keyNames[s_kbCapture.previewKey] : "---";
			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[kbFontSize * 1.3f]);
			CenterText(previewName);
			ImGui::PopFont();

            if (s_kbCapture.hasConflict) {
                ImGui::Text("");
                ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.35f, 1.0f), "%s", s_kbCapture.conflictText);
            }

			ImGui::Text("");
			ImGui::Text("");

			CenterText("Press a key, then ENTER to confirm or ESCAPE to cancel.");
		}
		ImGui::End();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(4);

		if (!popupOpen) {
			s_kbCapture.open          = false;
			s_kbCapture.index         = -1;
			s_kbCapture.isDirectWeapon = false;
		}
	}

	// Gamepad capture popup (shown above the main window when capturing a button)
	if (s_gpCapture.open && s_gpCapture.actionIndex >= 0 && s_gpCapture.playerIndex >= 0) {
		const float popupScale = scaleF;
		float popupWidth  = 420.0f * popupScale;
		float popupHeight = 200.0f * popupScale;

		ImGui::SetNextWindowSize(ImVec2(popupWidth, popupHeight));
		ImGui::SetNextWindowPos(ImVec2((g_renderSize.x - popupWidth) * 0.5f, (g_renderSize.y - popupHeight) * 0.5f));

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,  ImVec2(20.0f * popupScale, 20.0f * popupScale));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 20.0f * popupScale);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.92f));

		bool popupOpen = s_gpCapture.open;
		char gpPopupTitle[32];
		snprintf(gpPopupTitle, sizeof(gpPopupTitle), "GPRebind##%d", s_gpCapture.playerIndex);
		if (ImGui::Begin(gpPopupTitle, &popupOpen, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize)) {
			ImGui::SetWindowFontScale(popupScale);

			auto gpFontSize = UI::g_UIContext.DefaultFontSize;
			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[gpFontSize * 1.2f]);
			const char* actionName = (s_gpCapture.actionIndex >= 0 && s_gpCapture.actionIndex < NUM_BINDS_WITHOUT_START)
				? s_gamepadActionNames[s_gpCapture.actionIndex] : "???";
			ImGui::Text("Rebinding: %s (Slot %c) for %dP", actionName,
				s_gpCapture.slotIndex == 0 ? 'A' : 'B', s_gpCapture.playerIndex + 1);
			ImGui::SameLine();
			if (GUI_CloseX()) {
				s_gpCapture.open = false;
				s_gpCapture.actionIndex = -1;
			}
			ImGui::PopFont();

			ImGui::Text("");
			ImGui::Text("");

			const char* previewName = GAMEPAD::ButtonName(s_gpCapture.previewButton);
			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[gpFontSize * 1.3f]);
			CenterText(previewName);
			ImGui::PopFont();

			ImGui::Text("");
			ImGui::Text("");

			if (s_gpCapture.waitingForRelease) {
				CenterText("Release all buttons, then press any button to capture...");
			} else {
				CenterText("Press any button to capture.  Hold B for 1s to cancel.");
			}
		}
		ImGui::End();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(4);

		if (!popupOpen) {
			s_gpCapture.open          = false;
			s_gpCapture.actionIndex   = -1;
			s_gpCapture.playerIndex   = -1;
		}
	}

	if (shouldClose) {
		if (g_control_ui) {
			CUIDControl_Close();
		}
		g_showControllerRemap = false;
		g_showKeyboardConfig  = false;
	}
}

void ToggleCursor() {
    if (g_showMain || g_showShop || g_control_ui != nullptr) {
        Windows_ToggleCursor(true);
    } else {
        Windows_ToggleCursor(!activeConfig.hideMouseCursor);
    }
}

void UpdateGamepadConfigCapture() {
	if (!s_gpCapture.open || s_gpCapture.actionIndex < 0 || s_gpCapture.playerIndex < 0) {
		return;
	}

	static uint32_t s_prevButtons = 0;
	static uint32_t s_prevTouchpad = 0;

	const int pi = s_gpCapture.playerIndex;
	const int physSlot = (int)activeCrimsonInput.xinputSlots[pi];

	// Poll gamepad state: try SDL first (handles SDL controllers and XInput via SDL),
	// then fall back to raw XInput for pure XInput controllers.
	XINPUT_STATE xiState = {};
	bool xiValid = false;

	// SDL covers both SDL-native controllers and XInput controllers visible via SDL.
	if (physSlot >= 0 && physSlot < 4) {
		if (CrimsonSDL::PopulateXInputStateFromSdlSlot(physSlot, &xiState)) {
			xiValid = true;
		}
	} else if (physSlot >= 4) {
		// Sentinel SDL slot: look up in sdlGamepadsExtra
		size_t extrasIdx = (size_t)(physSlot - 4);
		if (extrasIdx < CrimsonSDL::sdlGamepadsExtra.size()) {
			SDL_Gamepad* pad = CrimsonSDL::sdlGamepadsExtra[extrasIdx];
			if (pad && CrimsonSDL::PopulateXInputStateFromSDL(pad, &xiState)) {
				xiValid = true;
			}
		}
	}
	// Fall back to raw XInput for slots 0-3 not covered by SDL.
	if (!xiValid && physSlot >= 0 && physSlot < 4) {
		if (XI::new_XInputGetState && XI::new_XInputGetState((DWORD)physSlot, &xiState) == ERROR_SUCCESS) {
			xiValid = true;
		}
	}

	// SDL gamepad for touchpad
	SDL_Gamepad* sdlPad = NULL;
	if (physSlot >= 0 && physSlot < 4) {
		sdlPad = CrimsonSDL::GetControllerByPhysicalSlot(physSlot);
	} else if (physSlot >= 4) {
		size_t extrasIdx = (size_t)(physSlot - 4);
		if (extrasIdx < CrimsonSDL::sdlGamepadsExtra.size()) {
			sdlPad = CrimsonSDL::sdlGamepadsExtra[extrasIdx];
		}
	}

	// Convert standard XInput wButtons to the game's internal GAMEPAD format
	uint32_t curButtons = xiValid ? GAMEPAD::FromXInput(xiState.Gamepad.wButtons) : 0;
	uint32_t curTouchpad = 0;
	if (sdlPad) {
		curTouchpad = CrimsonSDL::GetTouchpadZone(sdlPad);
	}

	// ── Confirm helper: apply the captured binding and close the popup ──
	auto ApplyBinding = [&](uint32_t capturedButton) {
		int j = s_gpCapture.actionIndex;
		int slot = s_gpCapture.slotIndex;

		int charSlot = s_selectedCharacterSlotByPlayer[pi];
		if (charSlot < 0 || charSlot >= 2) charSlot = 0;
		CrimsonInput::BindPair* activeCfg = (*activeConfigInputs[pi][charSlot]);
		CrimsonInput::BindPair* queuedCfg = (*queuedConfigInputs[pi][charSlot]);

		if (j >= 16) {
			if (slot == 0) {
				(*activeConfigInputs[pi][0])[j].slotA = capturedButton;
				(*activeConfigInputs[pi][1])[j].slotA = capturedButton;
				(*queuedConfigInputs[pi][0])[j].slotA = capturedButton;
				(*queuedConfigInputs[pi][1])[j].slotA = capturedButton;
			} else {
				(*activeConfigInputs[pi][0])[j].slotB = capturedButton;
				(*activeConfigInputs[pi][1])[j].slotB = capturedButton;
				(*queuedConfigInputs[pi][0])[j].slotB = capturedButton;
				(*queuedConfigInputs[pi][1])[j].slotB = capturedButton;
			}
		} else {
			if (slot == 0) {
				activeCfg[j].slotA = capturedButton;
				queuedCfg[j].slotA = capturedButton;
			} else {
				activeCfg[j].slotB = capturedButton;
				queuedCfg[j].slotB = capturedButton;
			}
		}
		GUI::save = true;
		s_gpCapture.open = false;
		s_gpCapture.actionIndex = -1;
		s_gpCapture.bHoldTracking = false;
	};

	// ── B-hold cancel tracking ──
	// B is capturable, but uses hold-to-cancel: release before 1s = capture,
	// hold for 1s = cancel.
	bool bDownNow = (curButtons & GAMEPAD::B) != 0;
	bool bWasDown = (s_prevButtons & GAMEPAD::B) != 0;
	if (bDownNow && !bWasDown) {
		// B just pressed — start hold timer
		s_gpCapture.bHoldTracking = true;
		s_gpCapture.bHoldStart    = std::chrono::steady_clock::now();
	} else if (!bDownNow && bWasDown && s_gpCapture.bHoldTracking) {
		// B just released — capture it if still within the tracking window
		s_gpCapture.bHoldTracking = false;
		if (!s_gpCapture.waitingForRelease) {
			auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::steady_clock::now() - s_gpCapture.bHoldStart).count();
			if (elapsed < 1000) {
				ApplyBinding(GAMEPAD::B);
				s_prevButtons  = curButtons;
				s_prevTouchpad = curTouchpad;
				return;
			}
		}
	}

	if (s_gpCapture.bHoldTracking && !s_gpCapture.waitingForRelease) {
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now() - s_gpCapture.bHoldStart).count();
		if (elapsed >= 1000) {
			// Cancel: B held for 1 second
			s_gpCapture.open = false;
			s_gpCapture.actionIndex = -1;
			s_gpCapture.bHoldTracking = false;
			s_prevButtons  = curButtons;
			s_prevTouchpad = curTouchpad;
			return;
		}
	}

	// ── Waiting-for-release phase ──
	if (s_gpCapture.waitingForRelease) {
		if (curButtons == 0 && curTouchpad == 0) {
			s_gpCapture.waitingForRelease = false;
			s_prevButtons  = 0;
			s_prevTouchpad = 0;
		} else {
			s_prevButtons  = curButtons;
			s_prevTouchpad = curTouchpad;
		}
		return;
	}

	// Detect touchpad press (rising edge) → capture immediately 
	if (curTouchpad != 0 && s_prevTouchpad == 0) {
		ApplyBinding(curTouchpad);
		s_prevButtons  = curButtons;
		s_prevTouchpad = curTouchpad;
		return;
	}

	// ── Detect XInput button press (rising edges) → capture immediately ──
	// B is handled by the hold-to-cancel tracker above; all other buttons
	// (including START) are captured on the first rising edge.
	uint32_t newButtons = curButtons & ~s_prevButtons;
	uint32_t captureButtons = newButtons & ~(uint32_t)GAMEPAD::B;
	if (captureButtons != 0) {
		ApplyBinding(captureButtons);
	}

	s_prevButtons  = curButtons;
	s_prevTouchpad = curTouchpad;
}

// Remapping Xinput Slots is applied inside this hook at the XInputGetState level, so dmc3_XInputWrapper's
// internal per-player state is populated with the correct physical controller data before
// the game ever reads from it.
static DWORD WINAPI Hooked_XInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState) {
	if (!pState) return ERROR_INVALID_PARAMETER;

	const DWORD physSlot = (dwUserIndex < (DWORD)PLAYER_COUNT)
		? (DWORD)activeCrimsonInput.xinputSlots[dwUserIndex]
		: dwUserIndex;

	DWORD result;
	if (CrimsonSDL::PopulateXInputStateFromSdlSlot((int)physSlot, pState)) {
		result = ERROR_SUCCESS;
	} else {
		result = s_XInputGetStateHook->GetTrampoline<decltype(&XInputGetState)>()(physSlot, pState);
	}

	// Touchpad injection: when touchpad is pressed and any character slot has a
	// matching binding, inject the other slot's standard button into wButtons.
	if (result == ERROR_SUCCESS && dwUserIndex < (DWORD)PLAYER_COUNT) {
		SDL_Gamepad* sdlPad = NULL;
		if (physSlot >= 0 && physSlot < 4) {
			sdlPad = CrimsonSDL::GetControllerByPhysicalSlot((int)physSlot);
		} else if (physSlot >= 4) {
			size_t extrasIdx = (size_t)(physSlot - 4);
			if (extrasIdx < CrimsonSDL::sdlGamepadsExtra.size())
				sdlPad = CrimsonSDL::sdlGamepadsExtra[extrasIdx];
		}

		if (sdlPad) {
			uint32_t touchZone = CrimsonSDL::GetTouchpadZone(sdlPad);

			if (touchZone != 0) {
				int pi = (int)dwUserIndex;
				// Check character slot
				const auto cs = GetCharacterBindSlotFromPlayerIndex(pi);
				const CrimsonInput::BindPair* binds = (*activeConfigInputs[pi][cs]);

				// BindTable for temporary corrections when touchpad-only actions
				// need a non-zero entry to match against injected defaults.
				BindTable* mainBinds = (BindTable*)(appBaseAddr + 0xD6CE80 + 0xA);
				uint16_t* btFields = &mainBinds->up;

				for (int a = 0; a < 17; a++) {
					// Skip custom actions handled by Crimson directly (e.g. Doppel Switch Button, Backwards Switch).
					// These read touchpad zones from activeConfigInputs in their own code
					// and don't need XInput injection into the game's BindTable.
					if (a >= 16) continue; 

					uint32_t injectBtn = 0;
					if (GAMEPAD::TouchpadZoneMatches(binds[a].slotA, touchZone)) {
						if (binds[a].slotB > 0 && binds[a].slotB <= 0xFFFF)
							injectBtn = binds[a].slotB;
						else
							injectBtn = btFields[a]; // BindTable pre-resolved by sub_1401EB170
					} else if (GAMEPAD::TouchpadZoneMatches(binds[a].slotB, touchZone)) {
						if (binds[a].slotA > 0 && binds[a].slotA <= 0xFFFF)
							injectBtn = binds[a].slotA;
						else
							injectBtn = btFields[a]; // BindTable pre-resolved by sub_1401EB170
					}

					if (injectBtn != 0) {
						uint16_t xiBit = GAMEPAD::ToXInput(injectBtn);
						if (xiBit)
							pState->Gamepad.wButtons |= xiBit;
					}
				}
				
			}
		}

		// Bidirectional START remapping 
		// The game's pause menu reads raw gamepad state (System B), bypassing
		// the BindTable (System A). We bridge both directions:
		//   1. Non-START button → START action: inject 0x0010 into wButtons.
		//   2. Physical START → other action: strip 0x0010 from wButtons and
		//      re-inject the other action's button bits so System A still fires.
		{
			int pi = (int)dwUserIndex;
			const auto cs = GetCharacterBindSlotFromPlayerIndex(pi);
			const CrimsonInput::BindPair* binds = (*activeConfigInputs[pi][cs]);

			// START action (index 15) — what the user configured for "pause"
			uint32_t startA = (binds[15].slotA <= 0xFFFF) ? binds[15].slotA : 0;
			uint32_t startB = (binds[15].slotB <= 0xFFFF) ? binds[15].slotB : 0;
			uint32_t startBinding = startA | startB;

			bool physicalStartDown = (pState->Gamepad.wButtons & 0x0010) != 0;

			// All standard GAMEPAD bits that have XInput equivalents (excl. combos/touchpad).
			static constexpr uint32_t kAllGpBits[] = {
				GAMEPAD::LEFT_TRIGGER, GAMEPAD::RIGHT_TRIGGER,
				GAMEPAD::LEFT_SHOULDER, GAMEPAD::RIGHT_SHOULDER,
				GAMEPAD::Y, GAMEPAD::B, GAMEPAD::A, GAMEPAD::X,
				GAMEPAD::BACK, GAMEPAD::LEFT_STICK_CLICK, GAMEPAD::RIGHT_STICK_CLICK,
				GAMEPAD::START,
				GAMEPAD::UP, GAMEPAD::RIGHT, GAMEPAD::DOWN, GAMEPAD::LEFT,
			};

			if (physicalStartDown) {
				// Physical START button pressed on the controller 
				if ((startBinding & GAMEPAD::START) == 0) {
					// START action no longer includes physical START — suppress pause.
					pState->Gamepad.wButtons &= ~((WORD)0x0010);

					// Re-inject alternate buttons for any other action whose binding
					// includes GAMEPAD::START, so the BindTable (System A) still fires.
					for (int a = 0; a < NUM_GAMEPADBINDS; a++) {
						if (a == 15) continue; // skip START action itself

						uint32_t bA = (binds[a].slotA <= 0xFFFF) ? binds[a].slotA : 0;
						uint32_t bB = (binds[a].slotB <= 0xFFFF) ? binds[a].slotB : 0;
						uint32_t actionBinding = bA | bB;

						if ((actionBinding & GAMEPAD::START) == 0) continue;

						// Extract non-START bits from this action's binding and
						// convert each to its XInput equivalent.
						uint32_t altBits = actionBinding & ~GAMEPAD::START;
						if (altBits != 0) {
							for (uint32_t bit : kAllGpBits) {
								if (altBits & bit) {
									uint16_t xiBit = GAMEPAD::ToXInput(bit);
									if (xiBit)
										pState->Gamepad.wButtons |= xiBit;
								}
							}
						} else {
							// Action is bound ONLY to START — fall back to the default
							// button (coordinated with BindTable correction in
							// sub_1401EB170 which already replaced START with the default).
							uint16_t xiBit = GAMEPAD::ToXInput(s_defaultBinds[a].slotA);
							if (xiBit)
								pState->Gamepad.wButtons |= xiBit;
						}
					}
				}
				// else: START action still includes physical START — let it pass natively.
			} else {
				// Physical START NOT pressed 
				// If a non-START button is bound to START action, inject 0x0010
				// when that button is pressed.
				if (startBinding != 0 && startBinding != GAMEPAD::START) {
					uint32_t gpButtons = GAMEPAD::FromXInput(pState->Gamepad.wButtons);

					bool boundPressed = false;
					if (startA != 0 && (gpButtons & startA)) boundPressed = true;
					if (startB != 0 && (gpButtons & startB)) boundPressed = true;

					if (boundPressed) {
						pState->Gamepad.wButtons |= 0x0010;
					}
				}
			}
		}
	}

	return result;
}

static DWORD __fastcall Hooked_dmc3_XInputWrapper(DWORD dwUserIndex, XINPUT_STATE* pState) {
    if (g_control_ui) {
        SetMemory(pState, 0, sizeof(XINPUT_STATE));
        return ERROR_SUCCESS;
    }
    // No remapping needed here: XInputGetState is hooked and applies xinputSlots remapping
    // before the game's internal per-player state is populated.
    return s_XInputWrapperHook->GetTrampoline<decltype(&Hooked_dmc3_XInputWrapper)>()(dwUserIndex, pState);
}

void InitBindings() {

    s_ButtonToActionHook = std::make_unique<Utility::Detour_t>((uintptr_t)appBaseAddr + 0x1EB170, &sub_1401EB170);
    bool res = s_ButtonToActionHook->Toggle();
    assert(res);

    s_CUIDControlConsHook = std::make_unique<Utility::Detour_t>((uintptr_t)appBaseAddr + 0x2817C0, &CUIDControl__CUIDControl_sub_1402817C0);
    res = s_CUIDControlConsHook->Toggle();
    assert(res);

    s_CUIDControlDestHook = std::make_unique<Utility::Detour_t>((uintptr_t)appBaseAddr + 0x281840, &CUIDControl_Destructor_sub_140281840);
    res = s_CUIDControlDestHook->Toggle();
    assert(res);

    s_XInputWrapperHook = std::make_unique<Utility::Detour_t>((uintptr_t)appBaseAddr + 0x3453F6, &Hooked_dmc3_XInputWrapper);
    res = s_XInputWrapperHook->Toggle();
    assert(res);

    s_XInputGetStateHook = std::make_unique<Utility::Detour_t>((uintptr_t)&XInputGetState, &Hooked_XInputGetState);
    res = s_XInputGetStateHook->Toggle();
    assert(res);

#if 0
    BindTable* mainBinds = (BindTable*)(appBaseAddr + 0xD6CE80 + 0xA);
    if (activeConfig.Actor.playerCount > 1) {
        CopyMemory(mainBinds, &s_defaultBinds, sizeof(BindTable));
    }
#endif
}

void SwapXInputButtonsCoop(uint8 plindex, XINPUT_STATE* state) {
    if (g_control_ui) {
        SetMemory(state, 0, sizeof(XINPUT_STATE));
        return;
    }
    // Pass the raw player index — the hooked XInputGetState applies xinputSlots remapping.
    // Do NOT pre-remap here; doing so would double-remap through the hook.
    if (XInputGetState(plindex, state) != ERROR_SUCCESS) {
        SetMemory(state, 0, sizeof(XINPUT_STATE));
    }
}

void StoreHDCKeybinds() {
    static bool runOnce = false;
    if (!runOnce) {
        Log("=== Storing HDC Keybinds from dmc3.exe+5611A0 ===");

        for (int i = 0; i < NUM_KEYBINDS; i++) {
            byte8* currentAddr = appBaseAddr + 0x5611A0 + (i * 4);  // Recalculate each time
            g_hdcKeybinds[i] = *(uint32_t*)currentAddr;             // Store uint32 content

            Log("  keybind[%2d] = addr %p = 0x%08X (int: %d)",
                i,
                currentAddr,               // Address being read
                g_hdcKeybinds[i],          // Full DWORD stored
                g_hdcKeybinds[i]);  // Int
        }
    }
	runOnce = true;
}

void OverrideHDCKeybinds() {
	static bool runOnce = false;
	if (!runOnce) {
		Log("=== Overriding HDC Keybinds with CrimsonConfig values ===");

		byte8* baseAddr = appBaseAddr + 0x5611A0;
		protectionHelper.Push(baseAddr, NUM_KEYBINDS * 4); 

		for (int i = 0; i < NUM_KEYBINDS; i++) {
			byte8* currentAddr = baseAddr + (i * 4);
			*(uint32_t*)currentAddr = activeCrimsonInput.KeyboardConfig.keybinds[i];
			Log("  keybind[%2d] = 0x%08X = int: %d", i, *(uint32_t*)currentAddr,
                *(uint32_t*)currentAddr);
		}

		protectionHelper.Pop();  
	}
    runOnce = true;
}

void UpdateKeyboardConfigCapture(byte8* state) {
    if (!s_kbCapture.open || s_kbCapture.index < 0) {
        return;
    }

    auto& exec    = s_kbCapture.executes;
    auto& preview = s_kbCapture.previewKey;

    // Escape = cancel
    if (state[DI8::KEY::ESCAPE] & 0x80) {
        if (exec[DI8::KEY::ESCAPE]) {
            exec[DI8::KEY::ESCAPE] = false;
            s_kbCapture.open          = false;
            s_kbCapture.index         = -1;
            s_kbCapture.isDirectWeapon = false;
            s_kbCapture.hasConflict = false;
            s_kbCapture.conflictText[0] = '\0';
        }
    } else {
        exec[DI8::KEY::ESCAPE] = true;
    }

    // Enter = confirm
    if (state[DI8::KEY::ENTER] & 0x80) {
        if (exec[DI8::KEY::ENTER] && preview != 0) {
            exec[DI8::KEY::ENTER] = false;

            const char* conflictName = nullptr;
            if (FindKeyboardBindConflict(preview, s_kbCapture.isDirectWeapon, s_kbCapture.index, &conflictName)) {
                s_kbCapture.hasConflict = true;
                snprintf(s_kbCapture.conflictText, sizeof(s_kbCapture.conflictText),
                    "Keybind already assigned to %s", conflictName);
                return;
            }

            if (s_kbCapture.isDirectWeapon) {
                activeCrimsonInput.KeyboardConfig.directWeaponKeybinds[s_kbCapture.index] = preview;
                queuedCrimsonInput.KeyboardConfig.directWeaponKeybinds[s_kbCapture.index] = preview;
            } else {
                activeCrimsonInput.KeyboardConfig.keybinds[s_kbCapture.index] = preview;
                queuedCrimsonInput.KeyboardConfig.keybinds[s_kbCapture.index] = preview;

                byte8* currentAddr = (appBaseAddr + 0x5611A0) + (s_kbCapture.index * 4);
                protectionHelper.Push(currentAddr, 4);
                *(uint32_t*)currentAddr = preview;
                protectionHelper.Pop();
            }

            s_kbCapture.open  = false;
            s_kbCapture.index = -1;
            s_kbCapture.isDirectWeapon = false;
            s_kbCapture.hasConflict = false;
            s_kbCapture.conflictText[0] = '\0';
            GUI::save = true;
  
        }
    } else {
        exec[DI8::KEY::ENTER] = true;
    }

    // Any other key = preview
    for (int i = 1; i < 256; i++) {
        if (i == DI8::KEY::ESCAPE || i == DI8::KEY::ENTER) {
            continue;
        }
        if (state[i] & 0x80) {
            if (exec[i]) {
                exec[i]  = false;
                preview  = static_cast<uint32>(i);
                s_kbCapture.hasConflict = false;
                s_kbCapture.conflictText[0] = '\0';
            }
        } else {
            exec[i] = true;
        }
    }
}


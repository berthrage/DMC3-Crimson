// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "DMC3Input.hpp"
#include "Config.hpp"
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
#include "ImGuiExtra.hpp"
#include "CrimsonGUI.hpp"
#include "CrimsonConfig.hpp"
#include "CrimsonSDL.hpp"

#define NUM_BINDS_WITHOUT_START 15

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
        .up = 0x1000,
        .down = 0x4000,
        .right = 0x2000,
        .left = 0x8000,
        .melee_atk = 0x10,
        .jump = 0x40,
        .style = 0x20,
        .shoot = 0x80,
        .dt = 0x4,
        .change_gun = 0x1,
        .change_target = 0x200,
        .lock_on = 0x8,
        .change_sword = 0x2,
        .default_camera = 0x400,
        .taunt = 0x100,
        .start = 0x800
    },
#ifdef MY_BINDS_TEST
    {
        .up = 0x1000,
        .down = 0x4000,
        .right = 0x2000,
        .left = 0x8000,
        .melee_atk = 0x40,
        .jump = 0x20,
        .style = 0x10,
        .shoot = 0x80,
        .dt = 0x4,
        .change_gun = 0x1,
        .change_target = 0x200,
        .lock_on = 0x8,
        .change_sword = 0x2,
        .default_camera = 0x400,
        .taunt = 0x100,
        .start = 0x800
    },
#else
    {
        .up = 0x1000,
        .down = 0x4000,
        .right = 0x2000,
        .left = 0x8000,
        .melee_atk = 0x10,
        .jump = 0x40,
        .style = 0x20,
        .shoot = 0x80,
        .dt = 0x4,
        .change_gun = 0x1,
        .change_target = 0x200,
        .lock_on = 0x8,
        .change_sword = 0x2,
        .default_camera = 0x400,
        .taunt = 0x100,
        .start = 0x800
    },
#endif
    {
        .up = 0x1000,
        .down = 0x4000,
        .right = 0x2000,
        .left = 0x8000,
        .melee_atk = 0x10,
        .jump = 0x40,
        .style = 0x20,
        .shoot = 0x80,
        .dt = 0x4,
        .change_gun = 0x1,
        .change_target = 0x200,
        .lock_on = 0x8,
        .change_sword = 0x2,
        .default_camera = 0x400,
        .taunt = 0x100,
        .start = 0x800
    },
    {
        .up = 0x1000,
        .down = 0x4000,
        .right = 0x2000,
        .left = 0x8000,
        .melee_atk = 0x10,
        .jump = 0x40,
        .style = 0x20,
        .shoot = 0x80,
        .dt = 0x4,
        .change_gun = 0x1,
        .change_target = 0x200,
        .lock_on = 0x8,
        .change_sword = 0x2, 
        .default_camera = 0x400,
        .taunt = 0x100,
        .start = 0x800
    },
};


// References to active/queued config inputs for each player and character.
uint16_t(*activeConfigInputs[PLAYER_COUNT][2])[NUM_GAMEPADBINDS] = {
	{ &activeCrimsonConfig.System.ButtonConfig.dante1P,
	  &activeCrimsonConfig.System.ButtonConfig.vergil1P },
	{ &activeCrimsonConfig.System.ButtonConfig.dante2P,
	  &activeCrimsonConfig.System.ButtonConfig.vergil2P },
	{ &activeCrimsonConfig.System.ButtonConfig.dante3P,
	  &activeCrimsonConfig.System.ButtonConfig.vergil3P },
	{ &activeCrimsonConfig.System.ButtonConfig.dante4P,
	  &activeCrimsonConfig.System.ButtonConfig.vergil4P }
};

uint16_t(*queuedConfigInputs[PLAYER_COUNT][2])[NUM_GAMEPADBINDS] = {
	{ &queuedCrimsonConfig.System.ButtonConfig.dante1P,
	  &queuedCrimsonConfig.System.ButtonConfig.vergil1P },
	{ &queuedCrimsonConfig.System.ButtonConfig.dante2P,
	  &queuedCrimsonConfig.System.ButtonConfig.vergil2P },
	{ &queuedCrimsonConfig.System.ButtonConfig.dante3P,
	  &queuedCrimsonConfig.System.ButtonConfig.vergil3P },
	{ &queuedCrimsonConfig.System.ButtonConfig.dante4P,
	  &queuedCrimsonConfig.System.ButtonConfig.vergil4P }
};

struct BTImGuiCtx {
    const char* actions[NUM_BINDS_WITHOUT_START] = {
        "UP",
        "DOWN",
        "RIGHT",
        "LEFT",
        "MELEE ATK",
        "JUMP",
        "STYLE",
        "SHOOT",
        "DT",
        "CHANGE GUN",
        "CHANGE TARGET",
        "LOCK ON",
        "CHANGE SWORD",
        "DEFAULT CAMERA",
        "TAUNT",
    };
    const char* items[NUM_BINDS_WITHOUT_START] = {
        "DPAD UP",
        "DPAD DOWN",
        "DPAD RIGHT",
        "DPAD LEFT",
        "Y",
        "A",
        "B",
        "X",
        "LEFT SHOULDER",
        "LEFT TRIGGER",
        "LEFT THUMB",
        "RIGHT SHOULDER",
        "RIGHT TRIGGER",
        "RIGHT THUMB",
        "BACK",
    };
    uint16_t values[NUM_BINDS_WITHOUT_START] = {
        0x1000,
        0x4000,
        0x2000,
        0x8000,
        0x10,
        0x40,
        0x20,
        0x80,
        0x4,
        0x1,
        0x200,
        0x8,
        0x2,
        0x400,
        0x100,
    };
};

static BindTable s_defaultBinds = {
    .up = 0x1000,
    .down = 0x4000,
    .right = 0x2000,
    .left = 0x8000,
    .melee_atk = 0x10,
    .jump = 0x40,
    .style = 0x20,
    .shoot = 0x80,
    .dt = 0x4,
    .change_gun = 0x1,
    .change_target = 0x200,
    .lock_on = 0x8,
    .change_sword = 0x2,
    .default_camera = 0x400,
    .taunt = 0x100,
    .start = 0x800
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

    const uint16_t* configBinds = (*activeConfigInputs[playerIndex][characterSlot]);

    mainBinds->up = configBinds[0];
    mainBinds->down = configBinds[1];
    mainBinds->right = configBinds[2];
    mainBinds->left = configBinds[3];
    mainBinds->melee_atk = configBinds[4];
    mainBinds->jump = configBinds[5];
    mainBinds->style = configBinds[6];
    mainBinds->shoot = configBinds[7];
    mainBinds->dt = configBinds[8];
    mainBinds->change_gun = configBinds[9];
    mainBinds->change_target = configBinds[10];
    mainBinds->lock_on = configBinds[11];
    mainBinds->change_sword = configBinds[12];
    mainBinds->default_camera = configBinds[13];
    mainBinds->taunt = configBinds[14];
    //mainBinds->start = configBinds[15]; // Start is actually handled somewhere else, 
    // otherwise we overwrite to D6CEA9 (turbo mode boolean) which isn't ideal.

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

static CUIDControl* g_control_ui = nullptr;
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

    const uint32* keybinds = activeCrimsonConfig.System.KeyboardConfig.keybinds;
    for (int i = 0; i < NUM_KEYBINDS; i++) {
        if (!isDirectWeapon && i == index) {
            continue;
        }
        if (keybinds[i] == key) {
            *outName = s_keybindActionNames[i];
            return true;
        }
    }

    const uint32* directBinds = activeCrimsonConfig.System.KeyboardConfig.directWeaponKeybinds;
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

void ShowButtonConfigWindow() {
	if (!g_control_ui && !g_showControllerRemap && !g_showKeyboardConfig) {
		return;
	}

	// If opened specifically via g_showKeyboardConfig, start on the keyboard tab.
	static int s_tab = 0; // 0 = Controller, 1 = Keyboard
	if (g_showKeyboardConfig && !g_showControllerRemap && !g_control_ui) {
		s_tab = 1;
	}

	ToggleCursor();
	BTImGuiCtx ctxControl{};
	const auto nplayers = queuedConfig.Actor.playerCount;
	bool shouldClose = false;

	static std::array<int, PLAYER_COUNT> s_selectedCharacterSlotByPlayer = {};
	static std::array<int, PLAYER_COUNT> s_comboSelection = {};

	const float scaleY      = CrimsonGUI::scaleFactorY;
	const float scaleF      = (CrimsonGUI::scaleFactorX + CrimsonGUI::scaleFactorY) * 0.5f;
	float width             = g_renderSize.x / 1.40f;
	float height            = g_renderSize.y / 1.10f;
	const float columnWidth = 0.5f  * queuedConfig.globalScale;
	const float rowWidth    = 40.0f * queuedConfig.globalScale;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,  ImVec2(20.0f * scaleF, 20.0f * scaleF));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 20.0f * scaleF);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.85f));

	ImGui::SetNextWindowSize(ImVec2(width, height));
	ImGui::SetNextWindowPos(ImVec2(g_renderSize.x * 0.5f, g_renderSize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

    auto& io = ImGui::GetIO();
    int keyboardBackKey = activeCrimsonConfig.System.KeyboardConfig.keybinds[11];

	ImGui::Begin("Button Configuration", &shouldClose, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar); {
		ImGui::SetWindowFontScale(scaleY);

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
			char buffer[33] = {};
			ImGui::BeginTable(buffer, 2); {
				ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
				ImGui::TableNextRow(0, rowWidth);
				ImGui::TableNextColumn();

				for (int i = 0; i < nplayers && i < PLAYER_COUNT; i++) {
					sprintf(buffer, "%dP", i + 1);
					ImGui::Text(buffer);
					ImGui::SameLine();
					ImGui::TextDisabled("[%s]", GetXInputControllerName((DWORD)activeCrimsonConfig.System.xinputSlots[i]));

					{
						// Build a unified slot list: XInput 0-3 + SDL
						char slotLabels[4 + 8][64];
						const char* slotPtrs[4 + 8];
						int slotCount = 0;

						for (int s = 0; s < 4; s++) {
							snprintf(slotLabels[slotCount], sizeof(slotLabels[slotCount]),
								"XInput %d  [%s]", s, GetXInputControllerName((DWORD)s));
							slotPtrs[slotCount] = slotLabels[slotCount];
							slotCount++;
						}
						for (size_t s = 0; s < CrimsonSDL::sdlGamepadsExtra.size() && slotCount < 12; s++) {
							const char* name = "Unknown";
							if (CrimsonSDL::sdlGamepadsExtra[s] != NULL) {
								name = CrimsonSDL::GetControllerNameForXInputSlot((int)(s + 4));
								if (!name || !name[0]) name = "SDL Controller";
							}
							snprintf(slotLabels[slotCount], sizeof(slotLabels[slotCount]),
								"SDL %zu  [%s]", s, name);
							slotPtrs[slotCount] = slotLabels[slotCount];
							slotCount++;
						}

						// Read current selection from config (handle sentinel values)
						uint8 cfgSlot = activeCrimsonConfig.System.xinputSlots[i];
						int currentSlot = (cfgSlot >= 4) ? (int)(cfgSlot - 4 + 4) : (int)cfgSlot;
						sprintf(buffer, "##ctrl%d", i);
						if (ImGui::Combo(buffer, &currentSlot, slotPtrs, slotCount)) {
							if (currentSlot < 4) {
								activeCrimsonConfig.System.xinputSlots[i] = (uint8)currentSlot;
								queuedCrimsonConfig.System.xinputSlots[i] = (uint8)currentSlot;
							} else {
								uint8 sentinel = (uint8)(4 + (currentSlot - 4));
								activeCrimsonConfig.System.xinputSlots[i] = sentinel;
								queuedCrimsonConfig.System.xinputSlots[i] = sentinel;
							}
							GUI::save = true;
						}
						ImGui::SameLine();
						sprintf(buffer, "<##prev%d", i);
						if (ImGui::Button(buffer)) {
							currentSlot = (currentSlot == 0) ? (slotCount - 1) : (currentSlot - 1);
							s_comboSelection[i] = currentSlot;
							if (currentSlot < 4) {
								activeCrimsonConfig.System.xinputSlots[i] = (uint8)currentSlot;
								queuedCrimsonConfig.System.xinputSlots[i] = (uint8)currentSlot;
							} else {
								uint8 sentinel = (uint8)(4 + (currentSlot - 4));
								activeCrimsonConfig.System.xinputSlots[i] = sentinel;
								queuedCrimsonConfig.System.xinputSlots[i] = sentinel;
							}
							GUI::save = true;
						}
						ImGui::SameLine();
						sprintf(buffer, ">##next%d", i);
						if (ImGui::Button(buffer)) {
							currentSlot = (currentSlot + 1) % slotCount;
							s_comboSelection[i] = currentSlot;
							if (currentSlot < 4) {
								activeCrimsonConfig.System.xinputSlots[i] = (uint8)currentSlot;
								queuedCrimsonConfig.System.xinputSlots[i] = (uint8)currentSlot;
							} else {
								uint8 sentinel = (uint8)(4 + (currentSlot - 4));
								activeCrimsonConfig.System.xinputSlots[i] = sentinel;
								queuedCrimsonConfig.System.xinputSlots[i] = sentinel;
							}
							GUI::save = true;
						}
					}

					int& selectedSlot = s_selectedCharacterSlotByPlayer[i];
					if (selectedSlot < 0 || selectedSlot >= CHARACTER_COUNT) {
						selectedSlot = 0;
					}

					if (CHARACTER_COUNT > 1) {
						ImGui::PushID(i);
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

					uint16_t* activeButtonConfig  = activeConfigInputs[i][selectedSlot][0];
					uint16_t* queuedButtonConfig  = queuedConfigInputs[i][selectedSlot][0];
					uint16_t* defaultButtonConfig = (&s_defaultBinds.up);
					for (int j = 0; j < NUM_BINDS_WITHOUT_START; j++) {
						ImGui::PushID(i);
						ImGui::PushID(j);
						UI::ComboMapValue2<uint16_t, NUM_BINDS_WITHOUT_START>(ctxControl.actions[j], ctxControl.items,
							ctxControl.values, activeButtonConfig[j], queuedButtonConfig[j]);
						ImGui::PopID();
						ImGui::PopID();
					}

					if (GUI_Button("Restore Defaults")) {
						memcpy(queuedButtonConfig,  defaultButtonConfig, sizeof(BindTable));
						memcpy(activeButtonConfig,  defaultButtonConfig, sizeof(BindTable));
					}

					ImGui::TableNextColumn();
					if (i == 1 || i == 2) {
						ImGui::Text("");
					}
				}
			}
			ImGui::EndTable();

		} else {
			// ======================== KEYBOARD TAB ========================
			uint32* activeKeybinds  = activeCrimsonConfig.System.KeyboardConfig.keybinds;
			uint32* queuedKeybinds  = queuedCrimsonConfig.System.KeyboardConfig.keybinds;
			uint32* defaultKeybinds = defaultCrimsonConfig.System.KeyboardConfig.keybinds;

            uint32* dwActiveKeybinds = activeCrimsonConfig.System.KeyboardConfig.directWeaponKeybinds;
            uint32* dwQueuedKeybinds = queuedCrimsonConfig.System.KeyboardConfig.directWeaponKeybinds;

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

// Remapping Xinput Slots is applied inside this hook at the XInputGetState level, so dmc3_XInputWrapper's
// internal per-player state is populated with the correct physical controller data before
// the game ever reads from it.
static DWORD WINAPI Hooked_XInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState) {
	if (!pState) return ERROR_INVALID_PARAMETER;

	const DWORD physSlot = (dwUserIndex < (DWORD)PLAYER_COUNT)
		? (DWORD)activeCrimsonConfig.System.xinputSlots[dwUserIndex]
		: dwUserIndex;

	// Try SDL first — this enables pure-SDL controllers (PS4/PS5/Switch)
	// to control the game without XInput emulation.
	if (CrimsonSDL::PopulateXInputStateFromSdlSlot((int)physSlot, pState)) {
		return ERROR_SUCCESS;
	}

	// Fall back to real XInput for controllers not visible to SDL
	return s_XInputGetStateHook->GetTrampoline<decltype(&XInputGetState)>()(physSlot, pState);
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
			*(uint32_t*)currentAddr = activeCrimsonConfig.System.KeyboardConfig.keybinds[i];
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
                activeCrimsonConfig.System.KeyboardConfig.directWeaponKeybinds[s_kbCapture.index] = preview;
                queuedCrimsonConfig.System.KeyboardConfig.directWeaponKeybinds[s_kbCapture.index] = preview;
            } else {
                activeCrimsonConfig.System.KeyboardConfig.keybinds[s_kbCapture.index] = preview;
                queuedCrimsonConfig.System.KeyboardConfig.keybinds[s_kbCapture.index] = preview;

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


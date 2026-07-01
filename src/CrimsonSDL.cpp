#define SDL_MAIN_HANDLED
#include "../ThirdParty/SDL3/SDL.h"
/* SDL3 renamed SDL_RWops to SDL_IOStream. Alias for SDL2_mixer compatibility. */
typedef SDL_IOStream SDL_RWops;
/* SDL3 removed SDL_bool; SDL2_mixer.h still uses it. */
#define SDL_bool bool
/* SDL3 removed SDL_MIX_MAXVOLUME; SDL2_mixer.h uses it. */
#define SDL_MIX_MAXVOLUME 128
/* Skip SDL2's SDL_rwops.h (its SDL_ReadU8/SDL_WriteU8 conflict with SDL3's SDL_iostream.h).
   The typedef above provides all SDL_mixer.h needs from it. */
#define SDL_rwops_h_
/* SDL3 removed the SDL_version struct; SDL2_mixer.h uses it as a pointer. Forward-declare. */
struct SDL_version;
#include "../ThirdParty/SDL2/SDL_mixer.h"
#include <string>
#include <thread>
#include <chrono>

// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "Core/Core.hpp"
#include "Config.hpp"
#include "SDL.hpp"
#include "Vars.hpp"
#include "CrimsonFileHandling.hpp"
#include "CrimsonUtil.hpp"
#include "Sound.hpp"
#include <iostream>
#include <unordered_set>
#include <deque>
#include <cctype>
#include <Xinput.h>
#include "Core/Input.hpp"
#include "Global.hpp"
#include "DMC3Input.hpp"

namespace CrimsonSDL {

SDL_Gamepad* mainController = NULL;
std::vector<SDL_Gamepad*> controllers(4, NULL);
std::unordered_set<SDL_JoystickID> currentlyConnected;
SDL_Gamepad* sdlGamepadByXiSlot[4] = {NULL, NULL, NULL, NULL};
std::vector<SDL_Gamepad*> sdlGamepadsExtra;
std::string SDL3Initialization   = "";
std::string MixerInitialization  = "";
std::string MixerInitialization2 = "";
bool SDL3Init                    = false;
bool cacheAudioFiles             = false;
Mix_Chunk* changeGun;
Mix_Chunk* changeDevilArm;
Mix_Chunk* changeWeaponDMC3;
Mix_Chunk* styleChange;
Mix_Chunk* tricksterVO;
Mix_Chunk* swordmasterVO;
Mix_Chunk* gunslingerVO;
Mix_Chunk* royalguardVO;
Mix_Chunk* quicksilverVO;
Mix_Chunk* doppelganger1VO;
Mix_Chunk* doppelganger2VO;
Mix_Chunk* sprintL1;
Mix_Chunk* sprintL2;
Mix_Chunk* devilTriggerInL1;
Mix_Chunk* devilTriggerInL2;
Mix_Chunk* devilTriggerOut;
Mix_Chunk* devilTriggerLoop;
Mix_Chunk* devilTriggerReady;
Mix_Chunk* dtExplosionStart;
Mix_Chunk* dtExplosionLoop;
Mix_Chunk* dtExplosionFinish;
Mix_Chunk* dtExplosionRelease;
Mix_Chunk* doppelgangerIn;
Mix_Chunk* doppelgangerOut;
Mix_Chunk* quicksilverIn;
Mix_Chunk* quicksilverOut;
Mix_Chunk* styleRankD1;
Mix_Chunk* styleRankD2;
Mix_Chunk* styleRankC1;
Mix_Chunk* styleRankC2;
Mix_Chunk* styleRankB1;
Mix_Chunk* styleRankB2;
Mix_Chunk* styleRankA1;
Mix_Chunk* styleRankA2;
Mix_Chunk* styleRankS1;
Mix_Chunk* styleRankS2;
Mix_Chunk* styleRankSS1;
Mix_Chunk* styleRankSS2;
Mix_Chunk* styleRankSSS1;
Mix_Chunk* styleRankSSS2;
Mix_Chunk* delayedCombo1;
Mix_Chunk* delayedCombo2;
Mix_Chunk* delayedDrive;
Mix_Chunk* guard;
Mix_Chunk* royalBlock;
Mix_Chunk* normalBlock;
Mix_Chunk* jdc;
Mix_Chunk* jdcJustFrame;
Mix_Chunk* jdcCharge;
Mix_Chunk* driveStart;
Mix_Chunk* driveLoop;
Mix_Chunk* driveLevelUp;
Mix_Chunk* snap;
Mix_Music* missionClearSong;
Mix_Music* divinityStatueSong;
Mix_Music* battleOfBrothersSong;

// Mix Channels used
namespace CHANNEL {
    constexpr int initialDevilArm = 0; // to 79, 20 channels per player
    constexpr int initialChangeGun = 80; // to 159, 20 channels per player
    constexpr int initialStyleChange = 160; // to 239, 20 channels per player
    constexpr int initialStyleChangeVO = 240; // to 319, 20 channels per player
    constexpr int initialStyleRank = 320; // to 326
    constexpr int initialSprint = 327; // to 334, 2 channels per player
    constexpr int initialDTIn = 335; // to 342, 2 channels per player
    constexpr int initialDTOut = 343; // to 346, 1 channel per player
    constexpr int initialDTLoop = 347; // to 350, 1 channel per player
    constexpr int initialDoppIn = 351; // to 354, 1 channel per player
    constexpr int initialDoppOut = 355; // to 358, 1 channel per player
    constexpr int quickIn = 359;
    constexpr int quickOut = 360;
    constexpr int initialDTReady = 361; // to 364, 1 channel per player
    constexpr int initialDelayedCombo1 = 365; // to 368, 1 channel per player
    constexpr int initialDelayedCombo2 = 369; // to 372, 1 channel per player
    constexpr int initialDTEStart = 373; // to 376, 1 channel per player
    constexpr int initialDTELoop = 377; // to 380, 1 channel per player
    constexpr int initialDTEFinish = 381; // to 384, 1 channel per player
    constexpr int initialDTERelease = 385; // to 388, 1 channel per player
    constexpr int initialGuard = 389; // to 396, 2 channels per player
    constexpr int initialRoyalBlock = 397; // to 421, 5 channels per player 
    constexpr int initialBlock = 422; // to 441, 5 channels per player
	constexpr int initialJDC = 442; // to 481, 10 channels per player
    constexpr int initialJDCCharge = 482; // to 485, 1 channel per player
    constexpr int initialSnap = 486; // to 525, 10 channels per player
	constexpr int initialDrive = 526; // to 529, 1 channels per player
	constexpr int initialDriveLevelUp = 530; // to 537, 2 channels per player
	constexpr int initialDriveLoop = 538; // to 541, 1 channel per player
	constexpr int initialDriveClone = 542; // to 545, 1 channel per player
	constexpr int initialDriveLevelUpClone = 546; // to 553, 2 channels per player
	constexpr int initialDriveLoopClone = 554; // to 557, 1 channel per player
}

#define SDL_FUNCTION_DECLRATION(X) decltype(X)* fn_##X
#define LOAD_SDL_FUNCTION(X) fn_##X = GetSDLFunction<decltype(X)*>(#X)
#define LOAD_MIXER_FUNCTION(X) fn_##X = GetSDLMixerFunction<decltype(X)*>(#X)

SDL_FUNCTION_DECLRATION(SDL_Init)                         = NULL;
SDL_FUNCTION_DECLRATION(SDL_PollEvent)                    = NULL;
SDL_FUNCTION_DECLRATION(Mix_OpenAudioDevice)              = NULL;
SDL_FUNCTION_DECLRATION(Mix_OpenAudio)                    = NULL;
SDL_FUNCTION_DECLRATION(Mix_Init)                         = NULL;
SDL_FUNCTION_DECLRATION(SDL_GetGamepads)                 = NULL;
SDL_FUNCTION_DECLRATION(SDL_OpenGamepad)                  = NULL;
SDL_FUNCTION_DECLRATION(SDL_CloseGamepad)                 = NULL;
SDL_FUNCTION_DECLRATION(SDL_GetGamepadPlayerIndex)        = NULL;
SDL_FUNCTION_DECLRATION(SDL_GetGamepadJoystick)           = NULL;
SDL_FUNCTION_DECLRATION(Mix_AllocateChannels)             = NULL;
SDL_FUNCTION_DECLRATION(Mix_ReserveChannels)              = NULL;
SDL_FUNCTION_DECLRATION(Mix_LoadWAV)                      = NULL;
SDL_FUNCTION_DECLRATION(Mix_LoadMUS)                      = NULL;
SDL_FUNCTION_DECLRATION(Mix_Playing)                      = NULL;
SDL_FUNCTION_DECLRATION(Mix_Pause)                        = NULL;
SDL_FUNCTION_DECLRATION(Mix_Resume)                       = NULL;
SDL_FUNCTION_DECLRATION(Mix_Volume)                       = NULL;
SDL_FUNCTION_DECLRATION(Mix_SetPosition)                  = NULL;
SDL_FUNCTION_DECLRATION(Mix_PlayChannel)                  = NULL;
SDL_FUNCTION_DECLRATION(Mix_HaltChannel)                  = NULL;
SDL_FUNCTION_DECLRATION(Mix_FadeOutChannel)				  = NULL;
SDL_FUNCTION_DECLRATION(Mix_FadeInChannel)                = NULL;
SDL_FUNCTION_DECLRATION(Mix_VolumeMusic)                  = NULL;
SDL_FUNCTION_DECLRATION(Mix_GetMusicVolume)               = NULL;
SDL_FUNCTION_DECLRATION(Mix_FadeInMusic)                  = NULL;
SDL_FUNCTION_DECLRATION(Mix_FadeOutMusic)                 = NULL;
SDL_FUNCTION_DECLRATION(Mix_PlayingMusic)                 = NULL;
SDL_FUNCTION_DECLRATION(SDL_GetGamepadButton)             = NULL;
SDL_FUNCTION_DECLRATION(SDL_GetGamepadID)        = NULL;
SDL_FUNCTION_DECLRATION(SDL_UpdateGamepads)              = NULL;
SDL_FUNCTION_DECLRATION(SDL_GetGamepadName)               = NULL;
SDL_FUNCTION_DECLRATION(SDL_IsGamepad)                    = NULL;
SDL_FUNCTION_DECLRATION(SDL_RumbleGamepad)                = NULL;
SDL_FUNCTION_DECLRATION(SDL_GetError)                     = NULL;
SDL_FUNCTION_DECLRATION(SDL_free)                         = NULL;
SDL_FUNCTION_DECLRATION(SDL_GetGamepadPath)               = NULL;
SDL_FUNCTION_DECLRATION(SDL_GetGamepadPathForID)          = NULL;
SDL_FUNCTION_DECLRATION(SDL_GetGamepadAxis)               = NULL;
SDL_FUNCTION_DECLRATION(SDL_GetNumGamepadTouchpads)       = NULL;
SDL_FUNCTION_DECLRATION(SDL_GetGamepadTouchpadFinger)     = NULL;

void LoadAllSFX() {
	if (!cacheAudioFiles) {

		changeGun = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\changegun.wav").c_str());
		changeDevilArm = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\changedevilarm.wav").c_str());
		changeWeaponDMC3 = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\changeweapondmc3.wav").c_str());
		styleChange = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\stylechange.wav").c_str());
		tricksterVO = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\trickster1.wav").c_str());
		swordmasterVO = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\swordmaster1.wav").c_str());
		gunslingerVO = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\gunslinger1.wav").c_str());
		royalguardVO = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\royalguard1.wav").c_str());
		quicksilverVO = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\quicksilver1.wav").c_str());
		doppelganger1VO = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\doppelganger1.wav").c_str());
        doppelganger2VO = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\doppelganger2.wav").c_str());
		sprintL1 = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\sprint_l1.wav").c_str());
		sprintL2 = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\sprint_l2.wav").c_str());
		devilTriggerInL1 = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\dt_activation_l1.wav").c_str());
		devilTriggerInL2 = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\dt_activation_l2.wav").c_str());
		devilTriggerOut = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\dt_deactivation.wav").c_str());
		devilTriggerLoop = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\dt_loop.wav").c_str());
		dtExplosionStart = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\dte_start.wav").c_str());
		dtExplosionLoop = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\dte_loop.wav").c_str());
		dtExplosionFinish = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\dte_finish.wav").c_str());
		dtExplosionRelease = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\dte_release.wav").c_str());
		doppelgangerIn = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\dopp_activation.wav").c_str());
		doppelgangerOut = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\dopp_deactivation.wav").c_str());
		quicksilverIn = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\qs_activation.wav").c_str());
        quicksilverOut = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\qs_deactivation.wav").c_str());
		devilTriggerReady = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\dt_ready.wav").c_str());
		styleRankD1 = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\styleranks\\d1.wav").c_str());
		styleRankD2 = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\styleranks\\d2.wav").c_str());
		styleRankC1 = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\styleranks\\c1.wav").c_str());
		styleRankC2 = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\styleranks\\c2.wav").c_str());
		styleRankB1 = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\styleranks\\b1.wav").c_str());
		styleRankB2 = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\styleranks\\b2.wav").c_str());
		styleRankA1 = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\styleranks\\a1.wav").c_str());
		styleRankA2 = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\styleranks\\a2.wav").c_str());
		styleRankS1 = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\styleranks\\s1.wav").c_str());
		styleRankS2 = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\styleranks\\s2.wav").c_str());
		styleRankSS1 = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\styleranks\\ss1.wav").c_str());
		styleRankSS2 = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\styleranks\\ss2.wav").c_str());
		styleRankSSS1 = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\styleranks\\sss1.wav").c_str());
		styleRankSSS2 = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\styleranks\\sss2.wav").c_str());
		delayedCombo1 = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\delayedcombo1.wav").c_str());
		delayedCombo2 = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\delayedcombo2.wav").c_str());
        guard = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\guard.wav").c_str());
        royalBlock = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\blockroyal.wav").c_str());
        normalBlock = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\block.wav").c_str());
		jdc = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\jdc.wav").c_str());
		jdcJustFrame = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\jdc_justframe.wav").c_str());
		jdcCharge = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\jdc_charge.wav").c_str());
		driveStart = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\drive_start.wav").c_str());
		driveLoop = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\drive_loop.wav").c_str());
		driveLevelUp = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\drive_levelup.wav").c_str());
		snap = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\snap.wav").c_str());


		missionClearSong = fn_Mix_LoadMUS(((std::string)Paths::sounds + "\\music\\missionclear.mp3").c_str());
        divinityStatueSong = fn_Mix_LoadMUS(((std::string)Paths::sounds + "\\music\\divinitystatue.mp3").c_str());
		battleOfBrothersSong = fn_Mix_LoadMUS(((std::string)Paths::sounds + "\\music\\battleofbrothers.mp3").c_str());


		cacheAudioFiles = true;
	}
}

// Forward declarations for controller management functions
void RemapSdlControllers();

void AddController(SDL_JoystickID instanceID) {
	if (fn_SDL_IsGamepad == NULL || fn_SDL_OpenGamepad == NULL) return;
	if (fn_SDL_IsGamepad(instanceID)) {
		SDL_Gamepad* gamepad = fn_SDL_OpenGamepad(instanceID);
		if (gamepad) {
			currentlyConnected.insert(instanceID);
			bool assigned = false;
			for (auto& ctrl : controllers) {
				if (ctrl == NULL) {
					ctrl = gamepad;
					assigned = true;
					break;
				}
			}
			if (!assigned) {
				std::cerr << "Too many controllers connected. Ignoring controller " << instanceID << "\n";
				fn_SDL_CloseGamepad(gamepad);
			}
			else {
				const char* name = (fn_SDL_GetGamepadName != NULL) ? fn_SDL_GetGamepadName(gamepad) : "Unknown";
				std::cout << "Opened controller " << instanceID << ": " << name << std::endl;
			}
			RemapSdlControllers();
		}
		else {
			const char* err = (fn_SDL_GetError != NULL) ? fn_SDL_GetError() : "Unknown error";
			std::cerr << "Could not open controller " << instanceID << ": " << err << std::endl;
		}
	}
}

void RemoveController(SDL_JoystickID instanceID) {
	if (fn_SDL_GetGamepadID == NULL || fn_SDL_CloseGamepad == NULL) return;
	for (int i = 0; i < controllers.size(); ++i) {
		if (controllers[i] && fn_SDL_GetGamepadID(controllers[i]) == instanceID) {
			fn_SDL_CloseGamepad(controllers[i]);
			controllers[i] = NULL;
			currentlyConnected.erase(instanceID);
			std::cout << "Closed controller at index " << i << "  instanceID: " << instanceID << "\n";
			break;
		}
	}
	RemapSdlControllers();
}

void InitControllers() {
	if (fn_SDL_GetGamepads == NULL || fn_SDL_free == NULL) return;
	int count;
	SDL_JoystickID* joysticks = fn_SDL_GetGamepads(&count);
	if (joysticks) {
		for (int i = 0; i < count; ++i) {
			AddController(joysticks[i]);
		}
		fn_SDL_free(joysticks);
	}
}

// Normalize a HID device path for cross-API comparison.
// Lowercases and strips ALL HID collection filter segments (e.g. "&IG_00",
// "&MI_03", "&COL01") between the PID and the instance ID separator.
// This makes paths from different SDL backends (XInput vs HIDAPI vs WGI)
// comparable, while preserving the unique device instance ID.
static std::string NormalizeHidPath(const char* path) {
	if (!path || !path[0]) return {};

	std::string result(path);
	// Lowercase
	for (auto& c : result) c = (char)tolower((unsigned char)c);

	// Find "pid_" to locate the PID segment
	size_t pidPos = result.find("pid_");
	if (pidPos == std::string::npos) return result;

	// PID is 4 hex digits: "pid_XXXX" → end at pidPos + 8
	size_t pidEnd = pidPos + 8;
	if (pidEnd > result.size()) return result;

	// Find the next '#' after the PID (start of instance container ID)
	size_t hashPos = result.find('#', pidEnd);
	if (hashPos == std::string::npos) return result;

	// Strip all filter metadata between PID digits and the '#'
	// e.g. "...&pid_09cc&ig_00&mi_03#container..." → "...&pid_09cc#container..."
	result.erase(pidEnd, hashPos - pidEnd);

	return result;
}

// Returns the XInput slot (0-3) that corresponds to the given SDL gamepad path,
// or -1 if the path doesn't match any XInput device.
//
// SDL3 returns different path formats depending on the backend:
//   XInput backend:  "XInput#0", "XInput#1", etc.
//   HIDAPI backend:  "\\?\HID#VID_XXXX&PID_XXXX&...#instance#..."
// This function handles both.
int GetXInputSlotForHIDPath(const char* sdlPath) {
	if (!sdlPath || !sdlPath[0]) return -1;

	// --- Fast path: SDL XInput backend returns "XInput#N" ---
	{
		std::string lower(sdlPath);
		for (auto& c : lower) c = (char)tolower((unsigned char)c);

		// Match "xinput#N" where N is a digit 0-3
		const char* prefix = "xinput#";
		size_t prefixLen = 7;
		if (lower.compare(0, prefixLen, prefix) == 0 && lower.size() > prefixLen) {
			char digit = lower[prefixLen];
			if (digit >= '0' && digit <= '3') {
				return (int)(digit - '0');
			}
		}
		// Match "xinput controller N" (alternative format)
		const char* prefixAlt = "xinput controller ";
		size_t prefixAltLen = 18;
		if (lower.compare(0, prefixAltLen, prefixAlt) == 0 && lower.size() > prefixAltLen) {
			char digit = lower[prefixAltLen];
			if (digit >= '0' && digit <= '3') {
				return (int)(digit - '0');
			}
		}
	}

	// --- Slow path: HIDAPI backend returns HID device paths ---
	// Normalize the SDL path (strips vendor filter segments)
	std::string sdlPathNormalized = NormalizeHidPath(sdlPath);
	// If the path doesn't look like a HID path (no "vid_"), skip RawInput matching
	if (sdlPathNormalized.find("vid_") == std::string::npos)
		return -1;

	UINT count = 0;
	if (GetRawInputDeviceList(nullptr, &count, sizeof(RAWINPUTDEVICELIST)) != 0 || count == 0)
		return -1;

	std::vector<RAWINPUTDEVICELIST> list(count);
	if (GetRawInputDeviceList(list.data(), &count, sizeof(RAWINPUTDEVICELIST)) == (UINT)-1)
		return -1;

	DWORD xiSlot = 0;
	for (const auto& dev : list) {
		if (dev.dwType != RIM_TYPEHID) continue;

		UINT len = 0;
		GetRawInputDeviceInfoA(dev.hDevice, RIDI_DEVICENAME, nullptr, &len);
		if (len == 0) continue;

		std::string path(len, '\0');
		if (GetRawInputDeviceInfoA(dev.hDevice, RIDI_DEVICENAME, path.data(), &len) == (UINT)-1)
			continue;

		if (!path.empty() && path.back() == '\0') path.pop_back();

		// Only XInput HID devices contain "IG_" in their path
		if (path.find("IG_") == std::string::npos) continue;

		std::string xiPathNormalized = NormalizeHidPath(path.c_str());

		if (sdlPathNormalized == xiPathNormalized) {
			return (int)xiSlot;
		}

		if (++xiSlot >= 4) break;
	}

	return -1;
}

// Returns a normalized HID path that uniquely identifies the physical device
// behind a gamepad, enabling cross-backend deduplication.
// - XInput-backend pads (path "XInput#N") are resolved via RawInput to their
//   underlying HID path, then normalized.
// - HIDAPI / WGI pads are normalized directly via NormalizeHidPath.
// Returns empty string if the device cannot be identified.
static std::string GetNormalizedPathForPad(SDL_Gamepad* pad) {
	if (!pad || !fn_SDL_GetGamepadPath) return {};

	const char* path = fn_SDL_GetGamepadPath(pad);
	if (!path || !path[0]) return {};

	std::string lower(path);
	for (auto& c : lower) c = (char)tolower((unsigned char)c);

	// --- XInput backend: resolve virtual slot to underlying HID path ---
	if (lower.compare(0, 7, "xinput#") == 0 && lower.size() > 7) {
		int xiSlot = lower[7] - '0';
		if (xiSlot < 0 || xiSlot >= 4) return {};

		UINT count = 0;
		if (GetRawInputDeviceList(nullptr, &count, sizeof(RAWINPUTDEVICELIST)) != 0 || count == 0)
			return {};

		std::vector<RAWINPUTDEVICELIST> list(count);
		if (GetRawInputDeviceList(list.data(), &count, sizeof(RAWINPUTDEVICELIST)) == (UINT)-1)
			return {};

		DWORD slot = 0;
		for (const auto& dev : list) {
			if (dev.dwType != RIM_TYPEHID) continue;

			UINT len = 0;
			GetRawInputDeviceInfoA(dev.hDevice, RIDI_DEVICENAME, nullptr, &len);
			if (len == 0) continue;

			std::string xiPath(len, '\0');
			if (GetRawInputDeviceInfoA(dev.hDevice, RIDI_DEVICENAME, xiPath.data(), &len) == (UINT)-1)
				continue;

			if (!xiPath.empty() && xiPath.back() == '\0') xiPath.pop_back();
			if (xiPath.find("IG_") == std::string::npos) continue;

			if (slot == (DWORD)xiSlot) {
				return NormalizeHidPath(xiPath.c_str());
			}

			if (++slot >= 4) break;
		}
		return {};
	}

	// "xinput controller N" alternative format
	if (lower.compare(0, 18, "xinput controller ") == 0 && lower.size() > 18) {
		int xiSlot = lower[18] - '0';
		if (xiSlot < 0 || xiSlot >= 4) return {};
		// Same resolution as above; inline for simplicity
		UINT count = 0;
		if (GetRawInputDeviceList(nullptr, &count, sizeof(RAWINPUTDEVICELIST)) != 0 || count == 0)
			return {};

		std::vector<RAWINPUTDEVICELIST> list(count);
		if (GetRawInputDeviceList(list.data(), &count, sizeof(RAWINPUTDEVICELIST)) == (UINT)-1)
			return {};

		DWORD slot = 0;
		for (const auto& dev : list) {
			if (dev.dwType != RIM_TYPEHID) continue;

			UINT len = 0;
			GetRawInputDeviceInfoA(dev.hDevice, RIDI_DEVICENAME, nullptr, &len);
			if (len == 0) continue;

			std::string xiPath(len, '\0');
			if (GetRawInputDeviceInfoA(dev.hDevice, RIDI_DEVICENAME, xiPath.data(), &len) == (UINT)-1)
				continue;

			if (!xiPath.empty() && xiPath.back() == '\0') xiPath.pop_back();
			if (xiPath.find("IG_") == std::string::npos) continue;

			if (slot == (DWORD)xiSlot) {
				return NormalizeHidPath(xiPath.c_str());
			}

			if (++slot >= 4) break;
		}
		return {};
	}

	// --- HIDAPI / WGI / other backends: normalize directly ---
	return NormalizeHidPath(path);
}

void RemapSdlControllers() {
	// Clear existing mapping
	for (int i = 0; i < 4; ++i) sdlGamepadByXiSlot[i] = NULL;
	sdlGamepadsExtra.clear();

	if (fn_SDL_GetGamepadPath == NULL) return;

	// First pass: place XInput-matching controllers into their slots
	for (SDL_Gamepad* pad : controllers) {
		if (pad == NULL) continue;

		const char* path = fn_SDL_GetGamepadPath(pad);
		if (!path) continue;

		int xiSlot = GetXInputSlotForHIDPath(path);

		if (xiSlot >= 0 && xiSlot < 4) {
			if (sdlGamepadByXiSlot[xiSlot] == NULL) {
				sdlGamepadByXiSlot[xiSlot] = pad;
			} else {
				// Slot already occupied; store as extra
				sdlGamepadsExtra.push_back(pad);
			}
		}
	}

	// Second pass: all remaining pads go to extras.
	// Duplicates (same physical device via different SDL backends) are
	// filtered out by the deduplication pass below.
	for (SDL_Gamepad* pad : controllers) {
		if (pad == NULL) continue;

		const char* path = fn_SDL_GetGamepadPath(pad);
		if (!path) continue;

		int xiSlot = GetXInputSlotForHIDPath(path);
		if (xiSlot >= 0 && xiSlot < 4 && sdlGamepadByXiSlot[xiSlot] == pad) {
			continue; // Already placed in XInput slot
		}
		if (xiSlot >= 0 && xiSlot < 4 && sdlGamepadByXiSlot[xiSlot] != NULL) {
			continue; // XInput slot occupied by another pad; already pushed to extras
		}

		// Non-XInput controller (or native duplicate of an XInput controller)
		sdlGamepadsExtra.push_back(pad);
	}

	// === Deduplication pass ===
	// Remove controllers from sdlGamepadsExtra that represent the same
	// physical device as one already mapped in sdlGamepadByXiSlot, or that
	// appear multiple times within extras itself.
	// Uses normalized HID paths (preserving device instance ID) so two
	// genuinely different controllers of the same model are NOT conflated.
	{
		std::unordered_set<std::string> seenPaths;

		// Seed with normalized paths of XInput-slot controllers
		for (int i = 0; i < 4; ++i) {
			if (sdlGamepadByXiSlot[i] != NULL) {
				std::string np = GetNormalizedPathForPad(sdlGamepadByXiSlot[i]);
				if (!np.empty()) seenPaths.insert(np);
			}
		}

		// Filter extras: keep first occurrence, skip duplicates
		std::vector<SDL_Gamepad*> deduped;
		for (SDL_Gamepad* pad : sdlGamepadsExtra) {
			if (pad == NULL) continue;
			std::string np = GetNormalizedPathForPad(pad);
			if (np.empty()) {
				// Cannot identify; keep it (safer than losing a controller)
				deduped.push_back(pad);
			} else if (seenPaths.find(np) == seenPaths.end()) {
				seenPaths.insert(np);
				deduped.push_back(pad);
			}
			// else: duplicate — skip
		}
		sdlGamepadsExtra = std::move(deduped);
	}

	// === Player auto-assignment ===
	// For each player whose configured physical XInput slot has no
	// controller, assign the next available SDL controller (sentinel ≥ 4).
	// Pre-scans already-assigned sentinels so that multiple invocations of
	// RemapSdlControllers (e.g. during init) never give two players the
	// same SDL controller.
	// Does NOT move SDL controllers into XInput slots.
	{
		size_t sdlCount = sdlGamepadsExtra.size();

		// First, detect which XInput slots (0-3) have physical controllers
		// connected — either via SDL or the real XInput API. This prevents
		// SDL controllers from stealing slots that belong to XInput
		// controllers the game hasn't assigned to players yet.
		// Uses the unhooked XI::new_XInputGetState so that subsequent
		// hotplug calls (when the XInputGetState hook is live) still
		// query real hardware instead of being remapped through
		// xinputSlots[] sentinels.
		bool xiSlotOccupied[4] = {};
		for (int slot = 0; slot < 4; ++slot) {
			if (sdlGamepadByXiSlot[slot] != NULL) {
				xiSlotOccupied[slot] = true;
			} else if (XI::new_XInputGetState != NULL) {
				XINPUT_STATE state;
				if (XI::new_XInputGetState((DWORD)slot, &state) == ERROR_SUCCESS) {
					xiSlotOccupied[slot] = true;
				}
			}
		}

		// Track which SDL indices are already claimed by any player
		bool sentinelUsed[8] = {}; // up to 8 SDL extras
		for (int p = 0; p < PLAYER_COUNT; ++p) {
			uint8 slot = activeCrimsonInput.xinputSlots[p];
			if (slot >= 4 && (size_t)(slot - 4) < sdlCount) {
				sentinelUsed[slot - 4] = true; // still valid
			} else if (slot >= 4) {
				// Sentinel points to a removed SDL controller; reset to dead
				activeCrimsonInput.xinputSlots[p] = 0;
				queuedCrimsonInput.xinputSlots[p] = 0;
			}
		}

		// Assign unused SDL controllers to players with dead XInput slots
		int nextSdl = 0;
		for (int p = 0; p < PLAYER_COUNT; ++p) {
			uint8 slot = activeCrimsonInput.xinputSlots[p];
			bool dead = (slot < 4 && !xiSlotOccupied[slot]);
			if (dead) {
				while (nextSdl < (int)sdlCount && sentinelUsed[nextSdl]) ++nextSdl;
				if (nextSdl < (int)sdlCount) {
					activeCrimsonInput.xinputSlots[p] = (uint8)(4 + nextSdl);
					queuedCrimsonInput.xinputSlots[p] = (uint8)(4 + nextSdl);
					sentinelUsed[nextSdl] = true;
					++nextSdl;
				}
			}
		}
	}

	// Log controller mapping summary (after all processing)
	{
		int xiCount = 0;
		for (int i = 0; i < 4; ++i) {
			if (sdlGamepadByXiSlot[i] != NULL) {
				++xiCount;
				const char* name = (fn_SDL_GetGamepadName != NULL)
					? fn_SDL_GetGamepadName(sdlGamepadByXiSlot[i]) : "?";
				Log("RemapSdlControllers: XInput slot %d -> \"%s\"", i, name);
			}
		}
		for (size_t i = 0; i < sdlGamepadsExtra.size(); ++i) {
			const char* name = (fn_SDL_GetGamepadName != NULL)
				? fn_SDL_GetGamepadName(sdlGamepadsExtra[i]) : "?";
			Log("RemapSdlControllers: SDL[%zu]        -> \"%s\"", i, name);
		}
		Log("RemapSdlControllers: %d XInput + %zu SDL = %zu total",
			xiCount, sdlGamepadsExtra.size(), (size_t)xiCount + sdlGamepadsExtra.size());
	}
}

SDL_Gamepad* GetControllerForPlayer(int playerIndex) {
	if (playerIndex < 0 || playerIndex >= 4) return NULL;
	int xiSlot = (int)activeCrimsonInput.xinputSlots[playerIndex];
	// Sentinel values ≥ 4 indicate SDL slot N (where N = xiSlot - 4)
	if (xiSlot >= 4) {
		size_t extrasIdx = (size_t)(xiSlot - 4);
		if (extrasIdx < sdlGamepadsExtra.size())
			return sdlGamepadsExtra[extrasIdx];
		return NULL;
	}
	if (xiSlot >= 0 && xiSlot < 4 && sdlGamepadByXiSlot[xiSlot] != NULL) {
		return sdlGamepadByXiSlot[xiSlot];
	}
	return NULL;
}

SDL_Gamepad* GetControllerByPhysicalSlot(int xiSlot) {
	if (xiSlot < 0 || xiSlot >= 4) return NULL;
	return sdlGamepadByXiSlot[xiSlot];
}

const char* GetControllerNameForXInputSlot(int xiSlot) {
	if (xiSlot >= 0 && xiSlot < 4) {
		SDL_Gamepad* pad = sdlGamepadByXiSlot[xiSlot];
		if (pad != NULL && fn_SDL_GetGamepadName != NULL) {
			return fn_SDL_GetGamepadName(pad);
		}
		return NULL;
	}
	// Sentinel values ≥ 4 indicate SDL slot N (where N = xiSlot - 4)
	if (xiSlot >= 4 && !sdlGamepadsExtra.empty()) {
		size_t extrasIdx = (size_t)(xiSlot - 4);
		if (extrasIdx < sdlGamepadsExtra.size() && fn_SDL_GetGamepadName != NULL) {
			return fn_SDL_GetGamepadName(sdlGamepadsExtra[extrasIdx]);
		}
	}
	return NULL;
}

bool IsNativeControllerConnected() {
	return !sdlGamepadsExtra.empty();
}

bool IsNativeControllerButtonDown(int button) {
	if (fn_SDL_GetGamepadButton == NULL) return false;
	for (SDL_Gamepad* pad : sdlGamepadsExtra) {
		if (pad != NULL && fn_SDL_GetGamepadButton(pad, (SDL_GamepadButton)button)) {
			return true;
		}
	}
	return false;
}

// Populate an XINPUT_STATE from an SDL gamepad.
// Returns true if the state was successfully populated.
bool PopulateXInputStateFromSDL(SDL_Gamepad* pad, XINPUT_STATE* pState) {
	if (pad == NULL || pState == NULL || fn_SDL_GetGamepadButton == NULL) return false;

	ZeroMemory(pState, sizeof(XINPUT_STATE));

	static DWORD s_packetNumber = 0;
	pState->dwPacketNumber = ++s_packetNumber;

	WORD& btns = pState->Gamepad.wButtons;
	auto btn = [&](SDL_GamepadButton b) { return fn_SDL_GetGamepadButton(pad, b); };

	if (btn(SDL_GAMEPAD_BUTTON_DPAD_UP))         btns |= XINPUT_GAMEPAD_DPAD_UP;
	if (btn(SDL_GAMEPAD_BUTTON_DPAD_DOWN))       btns |= XINPUT_GAMEPAD_DPAD_DOWN;
	if (btn(SDL_GAMEPAD_BUTTON_DPAD_LEFT))       btns |= XINPUT_GAMEPAD_DPAD_LEFT;
	if (btn(SDL_GAMEPAD_BUTTON_DPAD_RIGHT))      btns |= XINPUT_GAMEPAD_DPAD_RIGHT;
	if (btn(SDL_GAMEPAD_BUTTON_START))           btns |= XINPUT_GAMEPAD_START;
	if (btn(SDL_GAMEPAD_BUTTON_BACK))            btns |= XINPUT_GAMEPAD_BACK;
	if (btn(SDL_GAMEPAD_BUTTON_LEFT_STICK))      btns |= XINPUT_GAMEPAD_LEFT_THUMB;
	if (btn(SDL_GAMEPAD_BUTTON_RIGHT_STICK))     btns |= XINPUT_GAMEPAD_RIGHT_THUMB;
	if (btn(SDL_GAMEPAD_BUTTON_LEFT_SHOULDER))   btns |= XINPUT_GAMEPAD_LEFT_SHOULDER;
	if (btn(SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER))  btns |= XINPUT_GAMEPAD_RIGHT_SHOULDER;
	if (btn(SDL_GAMEPAD_BUTTON_SOUTH))           btns |= XINPUT_GAMEPAD_A;
	if (btn(SDL_GAMEPAD_BUTTON_EAST))            btns |= XINPUT_GAMEPAD_B;
	if (btn(SDL_GAMEPAD_BUTTON_WEST))            btns |= XINPUT_GAMEPAD_X;
	if (btn(SDL_GAMEPAD_BUTTON_NORTH))           btns |= XINPUT_GAMEPAD_Y;

	// Axes (thumb sticks and triggers)
	if (fn_SDL_GetGamepadAxis != NULL) {
		pState->Gamepad.sThumbLX = fn_SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_LEFTX);
		// SDL Y-axis is inverted vs XInput (SDL: up=-, XInput: up=+).
		// Negate, but handle -32768 which can't be negated in Sint16.
		Sint16 ly = fn_SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_LEFTY);
		Sint16 ry = fn_SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_RIGHTY);
		pState->Gamepad.sThumbLY = (ly == -32768) ? 32767 : -ly;
		pState->Gamepad.sThumbRY = (ry == -32768) ? 32767 : -ry;
		pState->Gamepad.sThumbRX = fn_SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_RIGHTX);

		// SDL triggers: 0-32767, XInput triggers: 0-255
		Sint16 lt = fn_SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER);
		Sint16 rt = fn_SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER);
		pState->Gamepad.bLeftTrigger  = (BYTE)((lt > 0) ? (lt * 255 / 32767) : 0);
		pState->Gamepad.bRightTrigger = (BYTE)((rt > 0) ? (rt * 255 / 32767) : 0);
	}

	return true;
}

// Populate an XINPUT_STATE from the SDL gamepad mapped to a physical XInput slot.
// Returns true if state was populated, false if no SDL controller is available
// for this slot (caller should fall back to real XInput).
bool PopulateXInputStateFromSdlSlot(int xiSlot, XINPUT_STATE* pState) {
	if (xiSlot < 0 || pState == NULL) return false;

	SDL_Gamepad* pad = NULL;

	// Sentinel values ≥ 4 indicate SDL slot N (where N = xiSlot - 4)
	if (xiSlot >= 4) {
		size_t extrasIdx = (size_t)(xiSlot - 4);
		if (extrasIdx < sdlGamepadsExtra.size()) {
			pad = sdlGamepadsExtra[extrasIdx];
		}
	} else if (xiSlot < 4) {
		pad = sdlGamepadByXiSlot[xiSlot];
	}

	return PopulateXInputStateFromSDL(pad, pState);
}

void InitSDL() {
    if (!SDL3Init) {
        // --- Phase 1: Load SDL3 core functions and init SDL3 ---
        LOAD_SDL_FUNCTION(SDL_Init);

		if (fn_SDL_Init == NULL) {
			SDL3Initialization = "SDL3 Error: SDL_Init not found";
			MessageBoxA(NULL,
				"SDL3.dll could not be loaded.\n\nPlease ensure SDL3.dll is present in the game folder.\n\nThe application will now close.",
				"Crimson - Missing Dependency", MB_OK | MB_ICONERROR);
			exit(1);
		}

		// SDL3: SDL_Init returns bool; SDL_INIT_HAPTIC removed (merged into GAMEPAD)
		if (!fn_SDL_Init(SDL_INIT_GAMEPAD)) {
			SDL3Initialization = std::string("SDL3 Error: ") + (fn_SDL_GetError ? fn_SDL_GetError() : "unknown");
			MessageBoxA(NULL,
				"SDL3.dll could not be loaded.\n\nPlease ensure SDL3.dll is present in the game folder.\n\nThe application will now close.",
				"Crimson - Missing Dependency", MB_OK | MB_ICONERROR);
			exit(1);
		}
		SDL3Initialization = "SDL3 Success";
		Log((SDL3Initialization).c_str());
        LOAD_SDL_FUNCTION(SDL_PollEvent);
        LOAD_SDL_FUNCTION(SDL_GetGamepads);
        LOAD_SDL_FUNCTION(SDL_OpenGamepad);
		LOAD_SDL_FUNCTION(SDL_CloseGamepad);
		LOAD_SDL_FUNCTION(SDL_GetGamepadPlayerIndex);
        LOAD_SDL_FUNCTION(SDL_GetGamepadJoystick);
        LOAD_SDL_FUNCTION(SDL_GetGamepadButton);
        LOAD_SDL_FUNCTION(SDL_IsGamepad);
        LOAD_SDL_FUNCTION(SDL_GetGamepadName);
        LOAD_SDL_FUNCTION(SDL_RumbleGamepad);
        LOAD_SDL_FUNCTION(SDL_GetGamepadID);
        LOAD_SDL_FUNCTION(SDL_UpdateGamepads);
        LOAD_SDL_FUNCTION(SDL_GetError);
        LOAD_SDL_FUNCTION(SDL_free);
        LOAD_SDL_FUNCTION(SDL_GetGamepadPath);
        LOAD_SDL_FUNCTION(SDL_GetGamepadPathForID);
        LOAD_SDL_FUNCTION(SDL_GetGamepadAxis);
        LOAD_SDL_FUNCTION(SDL_GetNumGamepadTouchpads);
        LOAD_SDL_FUNCTION(SDL_GetGamepadTouchpadFinger);

       

        // --- Phase 2: Load SDL2_mixer (only after SDL3 is initialized) ---
        LOAD_MIXER_FUNCTION(Mix_AllocateChannels);
        LOAD_MIXER_FUNCTION(Mix_ReserveChannels);
        LOAD_MIXER_FUNCTION(Mix_LoadWAV);
        LOAD_MIXER_FUNCTION(Mix_LoadMUS);
        LOAD_MIXER_FUNCTION(Mix_FadeOutChannel);
        LOAD_MIXER_FUNCTION(Mix_FadeInChannel);
        LOAD_MIXER_FUNCTION(Mix_Playing);
        LOAD_MIXER_FUNCTION(Mix_Pause);
        LOAD_MIXER_FUNCTION(Mix_Resume);
        LOAD_MIXER_FUNCTION(Mix_Volume);
        LOAD_MIXER_FUNCTION(Mix_PlayChannel);
        LOAD_MIXER_FUNCTION(Mix_SetPosition);
        LOAD_MIXER_FUNCTION(Mix_HaltChannel);
        LOAD_MIXER_FUNCTION(Mix_VolumeMusic);
        LOAD_MIXER_FUNCTION(Mix_GetMusicVolume);
        LOAD_MIXER_FUNCTION(Mix_FadeInMusic);
        LOAD_MIXER_FUNCTION(Mix_FadeOutMusic);
        LOAD_MIXER_FUNCTION(Mix_PlayingMusic);
		LOAD_MIXER_FUNCTION(Mix_OpenAudio);
        LOAD_MIXER_FUNCTION(Mix_OpenAudioDevice);
        LOAD_MIXER_FUNCTION(Mix_Init);

		Log("Mix_OpenAudioDevice: %p", fn_Mix_OpenAudioDevice);
		Log("Mix_OpenAudio: %p", fn_Mix_OpenAudio);
		Log("Mix_Init: %p", fn_Mix_Init);

        // SDL2_mixer: returns 0 on success, -1 on error
        if (fn_Mix_OpenAudioDevice != NULL) {
            if (fn_Mix_OpenAudioDevice(44100, SDL_AUDIO_S16LE, 2, 512, NULL, 0) == -1) {
                MixerInitialization = "Mixer Error";
            } else {
                MixerInitialization = "Mixer Success";
            }
        } else {
            
            if (fn_Mix_OpenAudio != NULL) {
                if (fn_Mix_OpenAudio(44100, SDL_AUDIO_S16LE, 2, 512) == -1) {
                    MixerInitialization = "Mixer Error (OpenAudio fallback)";
                } else {
                    MixerInitialization = "Mixer Success (OpenAudio fallback)";
                }
            } else {
                MixerInitialization = "Mixer Error: OpenAudioDevice/OpenAudio not found";
            }
        }

		Log((MixerInitialization).c_str());

        // Mix_Init returns bitmask of loaded codecs (0 = nothing loaded = failure)
        if (fn_Mix_Init != NULL) {
            int flags = MIX_INIT_OGG | MIX_INIT_MP3;
            int result = fn_Mix_Init(flags);
            if (!result) {
                MixerInitialization2 = "Mixer2 Error";
            } else if ((result & flags) != flags) {
                MixerInitialization2 = "Mixer2 Partial";
            } else {
                MixerInitialization2 = "Mixer2 Succes";
            }
        } else {
            MixerInitialization2 = "Mixer2 Error: Mix_Init not found";
        }

        mainController = NULL;
        if (fn_SDL_GetGamepads != NULL && fn_SDL_IsGamepad != NULL && fn_SDL_OpenGamepad != NULL && fn_SDL_free != NULL) {
            int joyCount;
            SDL_JoystickID* joysticks = fn_SDL_GetGamepads(&joyCount);
            if (joysticks) {
                for (int i = 0; i < joyCount && i < 4; ++i) {
                    if (fn_SDL_IsGamepad(joysticks[i])) {
                        mainController = fn_SDL_OpenGamepad(joysticks[i]);
                        if (mainController) {
                            break;
                        }
                    }
                }
                fn_SDL_free(joysticks);
            }
        }


        int controllerIndex = (fn_SDL_GetGamepadPlayerIndex != NULL && mainController != NULL)
            ? fn_SDL_GetGamepadPlayerIndex(mainController) : -1;

        InitControllers();
        RemapSdlControllers();

        // Install XInput hooks now that SDL controllers are mapped.
        // This must happen here (not earlier in Crimson.cpp) because
        // Hooked_XInputGetState depends on sdlGamepadByXiSlot/sdlGamepadsExtra
        // which are only populated after InitControllers/RemapSdlControllers.
        InitBindings();

        if (!g_SDL3Mixer) {
            MessageBoxA(NULL,
                "SDL2.dll / SDL2_mixer.dll could not be loaded.\n\nPlease ensure both SDL2.dll and SDL2_mixer.dll are present in the game folder.\n\nThe application will now close.",
                "Crimson - Missing Dependency", MB_OK | MB_ICONERROR);
            exit(1);
        }

        SDL3Init = true;
    }


    // CHUNKS OF SOUND
    if (fn_Mix_AllocateChannels != NULL) {
        fn_Mix_AllocateChannels(1000);
    }

    // RESERVES SELECT EFFECT SOUND FOR CHANNELS 100 AND ABOVE
    if (fn_Mix_ReserveChannels != NULL) {
        fn_Mix_ReserveChannels(100);
    }

    if (fn_Mix_LoadWAV != NULL && fn_Mix_LoadMUS != NULL) {
        LoadAllSFX();
    }
}

void PlayOnChannels(int initialChannel, int finalChannel, Mix_Chunk* sfx, int volume) {
    if (!SDL3Init || fn_Mix_Playing == NULL || fn_Mix_Volume == NULL || fn_Mix_PlayChannel == NULL) return;

	for (int i = initialChannel; i <= finalChannel; i++) {
		if (!fn_Mix_Playing(i)) {
			fn_Mix_Volume(i, volume);
			fn_Mix_PlayChannel(i, sfx, 0);
			break;
		}
	}
}

void InterruptChannels(int initialChannel, int finalChannel) {
    if (!SDL3Init || fn_Mix_Playing == NULL || fn_Mix_HaltChannel == NULL) return;

	for (int i = initialChannel; i <= finalChannel; i++) {
		if (fn_Mix_Playing(i)) {
			fn_Mix_HaltChannel(i);
			break;
		}
	}
}

struct SnapEvent {
	byte8* actorBaseAddr;
	uint32       initialStyle;
	int         playerIndex;
	int         volume;
	bool        playDouble;
	bool        secondSnapPending;
	std::chrono::steady_clock::time_point fireAt;
};

static std::deque<SnapEvent> snapQueue;

void TickSnapQueue() {
	const auto now = std::chrono::steady_clock::now();

	for (auto it = snapQueue.begin(); it != snapQueue.end(); ) {
		if (now < it->fireAt) {
			++it;
			continue;
		}

		auto& ev = *it;
		auto& actorData = *reinterpret_cast<PlayerActorData*>(ev.actorBaseAddr);
		const auto style = actorData.style;
		const bool canPlaySnap = (actorData.eventData[0].event == 1 && actorData.character == CHARACTER::DANTE);
		const auto initialChannel = CHANNEL::initialSnap + (10 * ev.playerIndex);

		if (!ev.secondSnapPending) {
			// First snap
			if (canPlaySnap && ev.initialStyle == style)
				PlayOnChannels(initialChannel, initialChannel + 9, snap, ev.volume);
			else
				ev.playDouble = false;

			if (style != STYLE::GUNSLINGER && style != STYLE::ROYALGUARD)
				ev.playDouble = false;

			if (ev.playDouble) {
				// Re-queue for the second snap
				ev.secondSnapPending = true;
				ev.fireAt = now + std::chrono::milliseconds(40);
				++it;
				continue;
			}
		}
		else {
			// Second snap
			if (canPlaySnap && (style == STYLE::GUNSLINGER || style == STYLE::ROYALGUARD))
				PlayOnChannels(initialChannel, initialChannel + 9, snap, ev.volume);
		}

		it = snapQueue.erase(it);
	}
}


bool IsGamepadButtonDown(SDL_Gamepad* gamepad, int button) {
	if (fn_SDL_GetGamepadButton == NULL || gamepad == NULL) return false;
	return fn_SDL_GetGamepadButton(gamepad, (SDL_GamepadButton)button);
}

bool IsControllerButtonDown(int controllerIndex, int button) {
	SDL_Gamepad* pad = GetControllerForPlayer(controllerIndex);
	return IsGamepadButtonDown(pad, button);
}

uint32_t GetTouchpadZone(SDL_Gamepad* gamepad) {
	if (fn_SDL_GetNumGamepadTouchpads == NULL || fn_SDL_GetGamepadTouchpadFinger == NULL || gamepad == NULL)
		return 0;
	if (fn_SDL_GetGamepadButton == NULL)
		return 0;

	int numTouchpads = fn_SDL_GetNumGamepadTouchpads(gamepad);
	if (numTouchpads <= 0) return 0;

	// ── Per-gamepad state: lock zone at the instant of press ──
	struct TouchState {
		bool     buttonWasPressed = false;
		uint32_t lockedHalf       = 0;
	};
	static std::unordered_map<SDL_Gamepad*, TouchState> s_state;

	auto& state        = s_state[gamepad];
	bool buttonPressed  = fn_SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_TOUCHPAD);
	bool wasPressed     = state.buttonWasPressed;
	state.buttonWasPressed = buttonPressed;

	// Rising edge — scan ALL fingers, pick the one with highest pressure.
	// The finger actively pressing the pad exerts more capacitive pressure
	// than a finger that is merely resting.
	if (buttonPressed && !wasPressed) {
		float bestX = 0.0f, bestY = 0.0f, bestPressure = -1.0f;
		bool  found = false;
		for (int finger = 0; finger < 16; ++finger) {
			bool  down = false;
			float x = 0.0f, y = 0.0f, pressure = 0.0f;
			if (!fn_SDL_GetGamepadTouchpadFinger(gamepad, 0, finger, &down, &x, &y, &pressure))
				break;
			if (down && pressure > bestPressure) {
				bestPressure = pressure;
				bestX = x;
				bestY = y;
				found = true;
			}
		}
		if (found) {
			bool left = (bestX < 0.5f);
			state.lockedHalf = left ? GAMEPAD::TOUCHPAD_LEFT : GAMEPAD::TOUCHPAD_RIGHT;
		}
	}

	// Falling edge — clear locked zone
	if (!buttonPressed) {
		state.lockedHalf = 0;
		return 0;
	}

	// Return the previously locked zone (always halves)
	return state.lockedHalf;
}

bool IsTouchpadBindingActive(int playerIndex, uint32_t slotA, uint32_t slotB) {
	SDL_Gamepad* sdlPad = GetControllerForPlayer(playerIndex);
	if (!sdlPad) return false;

	uint32_t touchZone = GetTouchpadZone(sdlPad);
	if (touchZone == 0) return false;

	return GAMEPAD::TouchpadZoneMatches(slotA, touchZone)
		|| GAMEPAD::TouchpadZoneMatches(slotB, touchZone);
}

void CheckAndOpenControllers() {
	if (fn_SDL_PollEvent == NULL) return;
	SDL_Event event;

	while (fn_SDL_PollEvent(&event)) {
		if (event.type == SDL_EVENT_GAMEPAD_ADDED) {
			AddController(event.gdevice.which);
		} else if (event.type == SDL_EVENT_GAMEPAD_REMOVED) {
			RemoveController(event.gdevice.which); 
		}
	}
}

void UpdateJoysticks() {
    if (fn_SDL_UpdateGamepads != NULL) {
        fn_SDL_UpdateGamepads();
    }
    TickSnapQueue();
}

void VibrateController(int controllerIndex, Uint16 rumbleStrengthLowFreq, Uint16 rumbleStrengthHighFreq, int rumbleDuration) {
	uint8* disableFlag = reinterpret_cast<uint8*>(appBaseAddr + 0xCA8800);
	if (*disableFlag) return;
    if (fn_SDL_RumbleGamepad == NULL) return;
    SDL_Gamepad* pad = GetControllerForPlayer(controllerIndex);
    if (pad != NULL) {
		if (fn_SDL_RumbleGamepad(pad, rumbleStrengthLowFreq, rumbleStrengthHighFreq, rumbleDuration)) {
			// rumble started successfully
		}
		else {
			const char* err = (fn_SDL_GetError != NULL) ? fn_SDL_GetError() : "Unknown";
			std::cerr << "Vibration ERROR " << controllerIndex << ": " << err << std::endl;
		}
    }
}

void VibrateControllerByPhysicalSlot(int xiSlot, Uint16 rumbleLow, Uint16 rumbleHigh) {
    if (fn_SDL_RumbleGamepad == NULL) return;
    Uint32 duration = (rumbleLow == 0 && rumbleHigh == 0) ? 0 : 5000;

    // Reverse-lookup: find which player (if any) is mapped to this physical
    // XInput slot via xinputSlots[]. This handles SDL-only controllers
    // (PS4/PS5/Switch) that live in sdlGamepadsExtra and are mapped to
    // a physical slot through a sentinel value (≥4).
    for (int p = 0; p < PLAYER_COUNT; ++p) {
        if ((int)activeCrimsonInput.xinputSlots[p] == xiSlot) {
            SDL_Gamepad* pad = GetControllerForPlayer(p);
            if (pad != NULL) {
                fn_SDL_RumbleGamepad(pad, rumbleLow, rumbleHigh, duration);
            }
            return;
        }
    }

    // Fallback: no player explicitly maps to this physical slot — try the
    // direct sdlGamepadByXiSlot[] entry (XInput controller at this slot).
    SDL_Gamepad* pad = GetControllerByPhysicalSlot(xiSlot);
    if (pad != NULL) {
        fn_SDL_RumbleGamepad(pad, rumbleLow, rumbleHigh, duration);
        return;
    }

    // Second fallback: all controllers are SDL-only (in sdlGamepadsExtra,
    // mapped via sentinel values). Vibrate player 0's controller since
    // the game always targets the active player for vibration.
    pad = GetControllerForPlayer(0);
    if (pad != NULL) {
        fn_SDL_RumbleGamepad(pad, rumbleLow, rumbleHigh, duration);
    }
}

void FadeOutChannels(int channelException, int initialChannel, int numChannels, int fadeOutms) {

    for (int i = initialChannel; i < numChannels; i++) {

        if (i != channelException) {

            fn_Mix_FadeOutChannel(i, fadeOutms);
        }
    }
}

void PlayOnChannelsFadeOut(int initialChannel, int finalChannel, Mix_Chunk* sfx, int volume, int fadeOutms) {
    if (!SDL3Init) return;
    int channelBeingPlayed = 0;

    for (int i = initialChannel; i <= finalChannel; i++) {
        if (!fn_Mix_Playing(i)) {
            fn_Mix_Volume(i, volume);
            fn_Mix_PlayChannel(i, sfx, 0);
            channelBeingPlayed = i;
            break;
        }
    }

    FadeOutChannels(channelBeingPlayed, initialChannel, finalChannel, fadeOutms);
}

void PlayOnChannelsFadeOutPosition(int initialChannel, int finalChannel, Mix_Chunk* sfx, int volume, int fadeOutms, int angle, int distance) {
    if (!SDL3Init) return;
	int channelBeingPlayed = 0;


	for (int i = initialChannel; i <= finalChannel; i++) {
		if (!fn_Mix_Playing(i)) {
			fn_Mix_Volume(i, volume);
			fn_Mix_SetPosition(i, angle, distance);
			fn_Mix_PlayChannel(i, sfx, 0);
			channelBeingPlayed = i;
			break;
		}
	}

	FadeOutChannels(channelBeingPlayed, initialChannel, finalChannel, fadeOutms);
}

void PlayChangeDevilArm() {
	float slider = activeCrimsonConfig.SFX.changeWeaponEffectVolume / 100.0f;
	int volume = (int)(30.0f * slider);
    PlayOnChannelsFadeOutPosition(CHANNEL::initialDevilArm, CHANNEL::initialDevilArm + 19, changeDevilArm, volume, 400, 90, 0);
}

void PlayChangeGun() {
	float slider = activeCrimsonConfig.SFX.changeWeaponEffectVolume / 100.0f;
	int volume = (int)(30.0f * slider);
    PlayOnChannelsFadeOutPosition(CHANNEL::initialChangeGun, CHANNEL::initialChangeGun + 19, changeGun, volume, 400, 270, 0);
}


void PlayChangeDevilArmMP() {
	float slider = activeCrimsonConfig.SFX.changeWeaponEffectVolume / 100.0f;
	int volume = (int)(20.0f * slider);
	PlayOnChannelsFadeOutPosition(CHANNEL::initialDevilArm, CHANNEL::initialDevilArm + 19, changeDevilArm, volume, 400, 0, 0);
}

void PlayChangeGunMP() {
	float slider = activeCrimsonConfig.SFX.changeWeaponEffectVolume / 100.0f;
	int volume = (int)(20.0f * slider);
	PlayOnChannelsFadeOutPosition(CHANNEL::initialChangeGun, CHANNEL::initialChangeGun + 19, changeGun, volume, 400, 0, 0);
}

void PlayChangeWeaponDMC3MP() {
	float slider = activeCrimsonConfig.SFX.changeWeaponEffectVolume / 100.0f;
	int volume = (int)(20.0f * slider);
	PlayOnChannelsFadeOutPosition(CHANNEL::initialChangeGun, CHANNEL::initialChangeGun + 19, changeWeaponDMC3, volume, 400, 0, 0);
}

void PlayStyleChange(int playerIndex) {
	float slider = activeCrimsonConfig.SFX.styleChangeVolume / 100.0f; 
	int volume = (int)(10.0f * slider); 
    auto initialChannel = CHANNEL::initialStyleChange + (20 * playerIndex);
   
    PlayOnChannelsFadeOut(initialChannel, initialChannel + 19, styleChange, volume, 150);
}

void PlayStyleChangeVO(int playerIndex, int style, bool doppActive) {
	float slider = activeCrimsonConfig.SFX.styleChangeVoiceOverVolume / 100.0f;
	int volume = (int)(46.08f * slider);
    auto initialChannel = CHANNEL::initialStyleChangeVO + (20 * playerIndex);

    if (style == 2) {
        PlayOnChannelsFadeOut(initialChannel, initialChannel + 19, tricksterVO, volume, 150);
    } else if (style == 0) {
        PlayOnChannelsFadeOut(initialChannel, initialChannel + 19, swordmasterVO, volume, 150);
    } else if (style == 1) {
        PlayOnChannelsFadeOut(initialChannel, initialChannel + 19, gunslingerVO, volume, 150);
    } else if (style == 3) {
        PlayOnChannelsFadeOut(initialChannel, initialChannel + 19, royalguardVO, volume, 150);
    } else if (style == 4) {
        PlayOnChannelsFadeOut(initialChannel, initialChannel + 19, quicksilverVO, volume, 150);
    } else if (style == 5) {

        if (doppActive) {
            PlayOnChannelsFadeOut(initialChannel, initialChannel + 19, doppelganger1VO, volume, 150);
        }
        else {
            PlayOnChannelsFadeOut(initialChannel, initialChannel + 19, doppelganger2VO, volume, 150);
        }
    }
}

void PlaySnap(byte8* actorBaseAddr) {
	if (!actorBaseAddr) return;

	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
	const auto initialStyle = actorData.style;
	const auto playerIndex = actorData.newPlayerIndex;
	const auto actorSpeed = actorData.speed;  // adjust field name as needed

	if (actorData.eventData[0].event != 1 || actorData.character != CHARACTER::DANTE)
		return;
	float slider = (activeCrimsonConfig.SFX.styleChangeSnapVolume / 100.0f);
	//float slider = 5.0f / 100.0f;
	int   volume = (int)(255.0f * 5.0f / 100.0f * slider);

	std::chrono::milliseconds delay{ 0 };
	bool playDouble = false;

	switch (initialStyle) {
	case STYLE::SWORDMASTER:  delay = std::chrono::milliseconds(300); break;
	case STYLE::GUNSLINGER:   delay = std::chrono::milliseconds(300); playDouble = true; break;
	case STYLE::TRICKSTER:    delay = std::chrono::milliseconds(300); break;
	case STYLE::ROYALGUARD:   delay = std::chrono::milliseconds(300); playDouble = true; break;
	case STYLE::QUICKSILVER:  delay = std::chrono::milliseconds(300); break;
	case STYLE::DOPPELGANGER: delay = std::chrono::milliseconds(300); break;
	}

	if (actorSpeed > 0.0f) {
		delay = std::chrono::milliseconds(
			static_cast<long long>(delay.count() / (actorData.speed / g_FrameRateTimeMultiplier))
		);
	}

	snapQueue.push_back({
		.actorBaseAddr = actorBaseAddr,
		.initialStyle = initialStyle,
		.playerIndex = playerIndex,
		.volume = volume,
		.playDouble = playDouble,
		.secondSnapPending = false,
		.fireAt = std::chrono::steady_clock::now() + delay,
		});
}

void SetSFXDistanceMultipleChannels(int playerIndex, int initialChannel, int numberChannelsPerPlayer, int angle, int distance) {
    if (!SDL3Init || fn_Mix_SetPosition == NULL) return;
    auto initialChannelPlayer = initialChannel + (numberChannelsPerPlayer * playerIndex);

    for (int i = initialChannelPlayer; i <= (initialChannelPlayer + (numberChannelsPerPlayer - 1)); i++) {
        fn_Mix_SetPosition(i, angle, distance);
    }

}

void SetAllSFXDistance(int playerIndex, int angle, int distance) {
    // This will simulate a pseudo 3D effect for the SFX

    if (activeConfig.Actor.playerCount > 1) {
		SetSFXDistanceMultipleChannels(playerIndex, CHANNEL::initialDevilArm, 20, angle, distance);
		SetSFXDistanceMultipleChannels(playerIndex, CHANNEL::initialChangeGun, 20, angle, distance);
    }

	SetSFXDistanceMultipleChannels(playerIndex, CHANNEL::initialStyleChange, 20, angle, distance);
	SetSFXDistanceMultipleChannels(playerIndex, CHANNEL::initialStyleChangeVO, 20, angle, distance);
	
    fn_Mix_SetPosition(CHANNEL::initialSprint + playerIndex, angle, distance);
    fn_Mix_SetPosition(CHANNEL::initialSprint + playerIndex + 4, angle, distance); // L2
    fn_Mix_SetPosition(CHANNEL::initialDTIn + playerIndex, angle, distance);
    fn_Mix_SetPosition(CHANNEL::initialDTIn + playerIndex + 4, angle, distance); // L2
    fn_Mix_SetPosition(CHANNEL::initialDTOut + playerIndex, angle, distance);
    fn_Mix_SetPosition(CHANNEL::initialDTLoop + playerIndex, angle, distance);
    fn_Mix_SetPosition(CHANNEL::initialDoppIn + playerIndex, angle, distance);
    fn_Mix_SetPosition(CHANNEL::initialDoppOut + playerIndex, angle, distance);
    fn_Mix_SetPosition(CHANNEL::quickIn, angle, distance);
    fn_Mix_SetPosition(CHANNEL::quickOut, angle, distance);
    fn_Mix_SetPosition(CHANNEL::initialDTReady + playerIndex, angle, distance);
    fn_Mix_SetPosition(CHANNEL::initialDelayedCombo1 + playerIndex, angle, distance);
    fn_Mix_SetPosition(CHANNEL::initialDelayedCombo2 + playerIndex, angle, distance);
    fn_Mix_SetPosition(CHANNEL::initialDTEStart + playerIndex, angle, distance);
    fn_Mix_SetPosition(CHANNEL::initialDTELoop + playerIndex, angle, distance);
    fn_Mix_SetPosition(CHANNEL::initialDTEFinish + playerIndex, angle, distance);
    fn_Mix_SetPosition(CHANNEL::initialDTERelease + playerIndex, angle, distance);
	fn_Mix_SetPosition(CHANNEL::initialDTEStart + playerIndex, angle, distance);
	fn_Mix_SetPosition(CHANNEL::initialDTELoop + playerIndex, angle, distance);
	fn_Mix_SetPosition(CHANNEL::initialDTEFinish + playerIndex, angle, distance);

    SetSFXDistanceMultipleChannels(playerIndex, CHANNEL::initialGuard, 2, angle, distance);
    SetSFXDistanceMultipleChannels(playerIndex, CHANNEL::initialRoyalBlock, 5, angle, distance);
    SetSFXDistanceMultipleChannels(playerIndex, CHANNEL::initialBlock, 5, angle, distance);
    SetSFXDistanceMultipleChannels(playerIndex, CHANNEL::initialJDC, 10, angle, distance);
	SetSFXDistanceMultipleChannels(playerIndex, CHANNEL::initialJDCCharge, 1, angle, distance);
	SetSFXDistanceMultipleChannels(playerIndex, CHANNEL::initialSnap, 10, angle, distance);
	SetSFXDistanceMultipleChannels(playerIndex, CHANNEL::initialDrive, 1, angle, distance);
	SetSFXDistanceMultipleChannels(playerIndex, CHANNEL::initialDriveLoop, 1, angle, distance);
	SetSFXDistanceMultipleChannels(playerIndex, CHANNEL::initialDriveLevelUp, 2, angle, distance);
	SetSFXDistanceMultipleChannels(playerIndex, CHANNEL::initialDriveClone, 1, angle, distance);
	SetSFXDistanceMultipleChannels(playerIndex, CHANNEL::initialDriveLoopClone, 1, angle, distance);
	SetSFXDistanceMultipleChannels(playerIndex, CHANNEL::initialDriveLevelUpClone, 2, angle, distance);
}

void StyleRankCooldownTracker(int rank) {
    rankAnnouncer[rank].trackerRunning = true;
    rankAnnouncer[rank].offCooldown    = false;
    std::this_thread::sleep_for(std::chrono::seconds(activeCrimsonConfig.SFX.styleRankAnnouncerCooldownSec));
    rankAnnouncer[rank].offCooldown    = true;
    rankAnnouncer[rank].trackerRunning = false;
}

void SetCurrentStyleRank(int currentStyleRank) {

    for (int i = 0; i < 7; i++) {

        if (i <= currentStyleRank) {
            rankAnnouncer[i].count = 1;
        } else {
            rankAnnouncer[i].count = 0;
        }
    }
}

void PlayStyleRank(Mix_Chunk* styleRankWAV, Mix_Chunk* styleRankWAVAlt, int rank) {
	float slider = activeCrimsonConfig.SFX.announcerVolume / 100.0f;
	int volume = (int)(255.0f * slider);

    if (activeCrimsonConfig.SFX.onlyResetAnnouncerWhenHit && !rankAnnouncer[rank - 1].wasHit) {
        return;
    }

    if (rankAnnouncer[rank - 1].turn == 0 && rankAnnouncer[rank - 1].count == 0 && rankAnnouncer[rank - 1].offCooldown) {
        fn_Mix_Volume(CHANNEL::initialStyleRank + (rank - 1), volume);
        fn_Mix_PlayChannel(CHANNEL::initialStyleRank + (rank - 1), styleRankWAV, 0);
        rankAnnouncer[rank - 1].turn++;

        rankAnnouncer[rank - 1].timer = activeCrimsonConfig.SFX.styleRankAnnouncerCooldownSec;


    } else if (rankAnnouncer[rank - 1].turn == 1 && rankAnnouncer[rank - 1].count == 0 && rankAnnouncer[rank - 1].offCooldown) {
        fn_Mix_Volume(CHANNEL::initialStyleRank + (rank - 1), volume);
        fn_Mix_PlayChannel(CHANNEL::initialStyleRank + (rank - 1), styleRankWAVAlt, 0);
        rankAnnouncer[rank - 1].turn = 0;

        rankAnnouncer[rank - 1].timer = activeCrimsonConfig.SFX.styleRankAnnouncerCooldownSec;
    }

    rankAnnouncer[rank - 1].wasHit = false;
    SetCurrentStyleRank(rank - 1);
}

void StyleRankAnnouncerController(int rank) {

    if (rank == 1) {

        PlayStyleRank(styleRankD1, styleRankD2, 1);

    } else if (rank == 2) {
        PlayStyleRank(styleRankC1, styleRankC2, 2);
    } else if (rank == 3) {
        PlayStyleRank(styleRankB1, styleRankB2, 3);
    } else if (rank == 4) {
        PlayStyleRank(styleRankA1, styleRankA2, 4);
    } else if (rank == 5) {
        PlayStyleRank(styleRankS1, styleRankS2, 5);
    } else if (rank == 6) {
        PlayStyleRank(styleRankSS1, styleRankSS2, 6);
    } else if (rank == 7) {
        PlayStyleRank(styleRankSSS1, styleRankSSS2, 7);
    }

    if (rank == 0) {
        rankAnnouncer[0].count = 0;
    }
}


void PlaySprint(int playerIndex) {
	float slider = activeCrimsonConfig.SFX.sprintEffectVolume / 100.0f;
	int volume = (int)(20.0f * slider);

    fn_Mix_Volume(CHANNEL::initialSprint + playerIndex, volume);
    fn_Mix_Volume(CHANNEL::initialSprint + 4 + playerIndex, volume);
    fn_Mix_PlayChannel(CHANNEL::initialSprint + playerIndex, sprintL1, 0);
    fn_Mix_PlayChannel(CHANNEL::initialSprint + 4 + playerIndex, sprintL2, 0);
}

void PlayDevilTriggerIn(int playerIndex) {
	float sliderL1 = activeCrimsonConfig.SFX.dTInL1Volume / 100.0f;
	int volumeL1 = (int)(40.0f * sliderL1);
	float sliderL2 = activeCrimsonConfig.SFX.dTInL2Volume / 100.0f;
	int volumeL2 = (int)(20.0f * sliderL2);

    fn_Mix_Volume(CHANNEL::initialDTIn + playerIndex, volumeL1);
    fn_Mix_Volume(CHANNEL::initialDTIn +  4 + playerIndex, volumeL2);
    fn_Mix_PlayChannel(CHANNEL::initialDTIn + playerIndex, devilTriggerInL1, 0);
    fn_Mix_PlayChannel(CHANNEL::initialDTIn + 4 + playerIndex, devilTriggerInL2, 0);
}

void PlayDevilTriggerOut(int playerIndex) {
	float slider = activeCrimsonConfig.SFX.dTOutVolume / 100.0f;
	int volume = (int)(50.0f * slider);

    fn_Mix_Volume(CHANNEL::initialDTOut + playerIndex, volume);
    fn_Mix_PlayChannel(CHANNEL::initialDTOut + playerIndex, devilTriggerOut, 0);
}

void PlayDevilTriggerLoop(int playerIndex) {
    // Currently unused. - Mia

    fn_Mix_Volume(CHANNEL::initialDTLoop + playerIndex, 30);
    fn_Mix_PlayChannel(CHANNEL::initialDTLoop + playerIndex, devilTriggerLoop, -1);
}

void StopDevilTriggerLoop(int playerIndex) {

    fn_Mix_HaltChannel(CHANNEL::initialDTLoop + playerIndex);
}

void PlayDoppelgangerIn(int playerIndex) {
	float slider = activeCrimsonConfig.SFX.doppelInVolume / 100.0f;
	int volume = (int)(50.0f * slider);

    fn_Mix_Volume(CHANNEL::initialDoppIn + playerIndex, volume);
    fn_Mix_PlayChannel(CHANNEL::initialDoppIn + playerIndex, doppelgangerIn, 0);
}

void PlayDoppelgangerOut(int playerIndex) {
	float slider = activeCrimsonConfig.SFX.doppelOutVolume / 100.0f;
	int volume = (int)(50.0f * slider);

    fn_Mix_Volume(CHANNEL::initialDoppOut + playerIndex, volume);
    fn_Mix_PlayChannel(CHANNEL::initialDoppOut + playerIndex, doppelgangerOut, 0);
}

void PlayQuicksilverIn() {
	float slider = activeCrimsonConfig.SFX.quickInVolume / 100.0f;
	int volume = (int)(50.0f * slider);

    fn_Mix_Volume(CHANNEL::quickIn, volume);
    fn_Mix_PlayChannel(CHANNEL::quickIn, quicksilverIn, 0);
}

void PlayQuicksilverOut() {
	float slider = activeCrimsonConfig.SFX.quickOutVolume / 100.0f;
	int volume = (int)(50.0f * slider);

	fn_Mix_Volume(CHANNEL::quickOut, volume);
	fn_Mix_PlayChannel(CHANNEL::quickOut, quicksilverOut, 0);
}

void PlayJDC(int playerIndex, bool justFrame, float delay) {

	auto initialChannel = CHANNEL::initialJDC + (10 * playerIndex);

	auto playSound = [=]() {
		float slider = 90 / 100.0f;
		int volume = (int)(72.0f * slider);
		if (justFrame) {
			PlayOnChannels(initialChannel, initialChannel + 9, jdcJustFrame, volume);
		} else {
			PlayOnChannels(initialChannel, initialChannel + 9, jdc, volume);
		}
	};

	if (delay > 0) {
		std::thread([=]() {
			std::this_thread::sleep_for(std::chrono::milliseconds((int)delay));
			playSound();
		}).detach();
	} else {
		playSound();
	}
}

void PlayJDCCharge(int playerIndex) {
    auto initialChannel = CHANNEL::initialJDCCharge + (playerIndex);
    float slider = 10 / 100.0f;
    int volume = (int)(65.0f * slider);
    fn_Mix_Volume(CHANNEL::initialJDCCharge + playerIndex, volume);
    fn_Mix_PlayChannel(CHANNEL::initialJDCCharge + playerIndex, jdcCharge, 0);
}

void PlayDevilTriggerReady(int playerIndex) {
	float slider = activeCrimsonConfig.SFX.dTReadyVolume / 100.0f;
	int volume = (int)(110.0f * slider);

    fn_Mix_Volume(CHANNEL::initialDTReady + playerIndex, volume);
    fn_Mix_PlayChannel(CHANNEL::initialDTReady + playerIndex, devilTriggerReady, 0);
}

void PlayDelayedCombo1(int playerIndex) {
    // TYPE B - DmC ping
	float slider = activeCrimsonConfig.SFX.delayedComboIndicatorVolume / 100.0f; // 1.0 to 0.0
	int volumeDelayed1 = (int)(11.0f * slider); // 18 at 100%, 0 at 0%
	fn_Mix_Volume(CHANNEL::initialDelayedCombo1 + playerIndex, volumeDelayed1);
	fn_Mix_PlayChannel(CHANNEL::initialDelayedCombo1 + playerIndex, delayedCombo1, 0);
}

void PlayDelayedCombo2(int playerIndex) {
    // TYPE A - Less Intrusive
	float slider = activeCrimsonConfig.SFX.delayedComboIndicatorVolume / 100.0f; 
	int volumeDelayed2 = (int)(50.0f * slider); 
    fn_Mix_Volume(CHANNEL::initialDelayedCombo2 + playerIndex, volumeDelayed2);
    fn_Mix_PlayChannel(CHANNEL::initialDelayedCombo2 + playerIndex, delayedCombo2, 0);
}

bool ChannelIsPlaying(int channel) {
    if (!SDL3Init || fn_Mix_Playing == NULL) return false;
    return fn_Mix_Playing(channel);
}

bool DTEStartIsPlaying(int playerIndex) {
    return ChannelIsPlaying(CHANNEL::initialDTEStart + playerIndex);
}

void PlayDTExplosionStart(int playerIndex) {
    // starts at channel 312, to 315 for 4P
	float slider = activeCrimsonConfig.SFX.dTEStartLoopVolume / 100.0f;
	int volume = (int)(50.0f * slider);
    fn_Mix_Volume(CHANNEL::initialDTEStart + playerIndex, volume);
    fn_Mix_PlayChannel(CHANNEL::initialDTEStart + playerIndex, dtExplosionStart, 0);
}

void PlayDTExplosionLoop(int playerIndex) {
	// starts at channel 316, to 319 for 4P
	float slider = activeCrimsonConfig.SFX.dTEStartLoopVolume / 100.0f;
	int volume = (int)(50.0f * slider);
	fn_Mix_Volume(CHANNEL::initialDTELoop + playerIndex, volume);
	fn_Mix_PlayChannel(CHANNEL::initialDTELoop + playerIndex, dtExplosionLoop, -1);
}

void PlayDTExplosionFinish(int playerIndex) {
    // starts at channel 320, to 323 for 4P
	float slider = activeCrimsonConfig.SFX.dTEFinishVolume / 100.0f;
	int volume = (int)(130.0f * slider);
    fn_Mix_Volume(CHANNEL::initialDTEFinish + playerIndex, volume);
    fn_Mix_PlayChannel(CHANNEL::initialDTEFinish + playerIndex, dtExplosionFinish, 0);
}

void PlayDTEExplosionRelease(int playerIndex, float volumeMultiplier) {
    // starts at channel 324, to 327 for 4P
	float slider = activeCrimsonConfig.SFX.dTEReleaseVolume / 100.0f;
	int volume = (int)((85.0f * volumeMultiplier) * slider);
	fn_Mix_Volume(CHANNEL::initialDTERelease + playerIndex, volume);
	fn_Mix_PlayChannel(CHANNEL::initialDTERelease + playerIndex, dtExplosionRelease, 0);
}

void InterruptDTExplosionSFX(int playerIndex) {
    fn_Mix_HaltChannel(CHANNEL::initialDTEStart + playerIndex);
    fn_Mix_HaltChannel(CHANNEL::initialDTELoop + playerIndex);
}

void PauseDTExplosionSFX(int playerIndex) {
    fn_Mix_Pause(CHANNEL::initialDTEStart + playerIndex);
	fn_Mix_Pause(CHANNEL::initialDTELoop + playerIndex);
}

void ResumeDTExplosionSFX(int playerIndex) {
	fn_Mix_Resume(CHANNEL::initialDTEStart + playerIndex);
	fn_Mix_Resume(CHANNEL::initialDTELoop + playerIndex);
}


void PlayGuard(int playerIndex) {
	auto initialChannel = CHANNEL::initialGuard + (2 * playerIndex);

	PlayOnChannels(initialChannel, initialChannel + 1, guard, 50);
}

void PlayRoyalBlock(int playerIndex) {
	auto initialChannel = CHANNEL::initialRoyalBlock + (5 * playerIndex);
	float slider = activeCrimsonConfig.SFX.royalBlockVolume / 100.0f;
	int volume = (int)(44.0f * slider);

	PlayOnChannels(initialChannel, initialChannel + 4, royalBlock, volume);
}

void PlayNormalBlock(int playerIndex) {
	auto initialChannel = CHANNEL::initialBlock + (5 * playerIndex);
	float slider = activeCrimsonConfig.SFX.normalBlockVolume / 100.0f;
	int volume = (int)(20.0f * slider);

	PlayOnChannels(initialChannel, initialChannel + 4, normalBlock, volume);
}

void PlayDriveStart(int playerIndex, int entityIndex) {
	auto initialChannel = (entityIndex == 0) ? CHANNEL::initialDrive + playerIndex : CHANNEL::initialDriveClone + playerIndex;
	float slider = 7.0f / 100.0f;
	int   volume = (int)(255.0f * slider);
	fn_Mix_Volume(initialChannel, volume);
	fn_Mix_FadeInChannel(initialChannel, driveStart, 0, 400);
}

void PlayDriveLoop(int playerIndex, int entityIndex) {
	auto initialChannel = (entityIndex == 0) ? CHANNEL::initialDriveLoop + playerIndex : CHANNEL::initialDriveLoopClone + playerIndex;
	float slider = 7.0f / 100.0f;
	int   volume = (int)(255.0f * slider);
	fn_Mix_Volume(initialChannel, volume);
	fn_Mix_PlayChannel(initialChannel, driveLoop, -1);
}

void PlayDriveLevelUp(int playerIndex, int entityIndex) {
	auto initialChannel = (entityIndex == 0) ? CHANNEL::initialDriveLevelUp + (2 * playerIndex) : 
		CHANNEL::initialDriveLevelUpClone + (2 * playerIndex);
	float slider = 10.0f / 100.0f;
	int   volume = (int)(255.0f * slider);
	PlayOnChannels(initialChannel, initialChannel + 1, driveLevelUp, volume);
}

void InterruptDriveSFX(int playerIndex, int entityIndex) {
	auto initialChannelStart = (entityIndex == 0) ? CHANNEL::initialDrive + playerIndex : CHANNEL::initialDriveClone + playerIndex;
	auto initialChannelLoop = (entityIndex == 0) ? CHANNEL::initialDriveLoop + playerIndex : CHANNEL::initialDriveLoopClone + playerIndex;
	fn_Mix_FadeOutChannel(initialChannelStart, 100);
	fn_Mix_FadeOutChannel(initialChannelLoop, 100);
}

bool DriveStartIsPlaying(int playerIndex, int entityIndex) {
	auto initialChannel = (entityIndex == 0) ? CHANNEL::initialDrive + playerIndex : CHANNEL::initialDriveClone + playerIndex;
	return ChannelIsPlaying(initialChannel);
}

void PlayNewMissionClearSong() {
    if (!SDL3Init || fn_Mix_VolumeMusic == NULL || fn_Mix_FadeInMusic == NULL) return;
    fn_Mix_VolumeMusic(60 * (activeCrimsonConfig.Sound.channelVolumes[9] / 100.0f));
    fn_Mix_FadeInMusic(missionClearSong, -1, 500);
}

void PlayDivinityStatueSong() {
    if (!SDL3Init || fn_Mix_VolumeMusic == NULL || fn_Mix_FadeInMusic == NULL) return;
	fn_Mix_VolumeMusic(57 * (activeCrimsonConfig.Sound.channelVolumes[9] / 100.0f));
	fn_Mix_FadeInMusic(divinityStatueSong, -1, 500);
}

void FadeOutMusic(int delayMs) {
    if (!SDL3Init || fn_Mix_FadeOutMusic == NULL) return;
    fn_Mix_FadeOutMusic(delayMs);
}

int IsMusicPlaying() {
    if (!SDL3Init || fn_Mix_PlayingMusic == NULL) return 0;
    return fn_Mix_PlayingMusic();
}

void PlayBattleOfBrothersSong() {
    if (!SDL3Init || fn_Mix_VolumeMusic == NULL || fn_Mix_FadeInMusic == NULL) return;
	fn_Mix_VolumeMusic(31 * (activeCrimsonConfig.Sound.channelVolumes[9] / 100.0f));
	fn_Mix_FadeInMusic(battleOfBrothersSong, -1, 500);
}

void FadeOutMusic(float fadeoutTime) {
    if (!SDL3Init || fn_Mix_FadeOutMusic == NULL) return;
    fn_Mix_FadeOutMusic(fadeoutTime);
}

void ReduceMusicVolumeInPause() {
    if (!SDL3Init || fn_Mix_GetMusicVolume == NULL || fn_Mix_VolumeMusic == NULL) return;
	auto pool_19315 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
	if (!pool_19315 || !pool_19315[8]) {
		return;
	}
	auto& eventData = *reinterpret_cast<EventData*>(pool_19315[8]);
	static int previousVolume = -1; 

	if (g_scene == SCENE::GAME) {
		if (eventData.event == EVENT::PAUSE) {

			// Save current volume if not already saved
			if (previousVolume == -1) {
				previousVolume = fn_Mix_GetMusicVolume(battleOfBrothersSong);
			}
			// Reduce volume to 30% of configured value
			int reducedVolume = static_cast<int>(previousVolume * 0.5f);
			fn_Mix_VolumeMusic(reducedVolume);
		} else if (eventData.event == EVENT::MAIN) {
			// Restore previous volume if it was changed
			if (previousVolume != -1) {
				fn_Mix_VolumeMusic(previousVolume);
				previousVolume = -1;
			}
		}
	}
}

}

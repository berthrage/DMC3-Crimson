#define SDL_MAIN_HANDLED
#include "../ThirdParty/SDL2/SDL.h"
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

namespace CrimsonSDL {

SDL_GameController* mainController = NULL;
std::vector<SDL_GameController*> controllers(4, NULL);
std::unordered_set<SDL_JoystickID> currentlyConnected;
std::string SDL2Initialization   = "";
std::string MixerInitialization  = "";
std::string MixerInitialization2 = "";
bool SDL2Init                    = false;
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
Mix_Music* missionClearSong;
Mix_Music* divinityStatueSong;

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
}

#define SDL_FUNCTION_DECLRATION(X) decltype(X)* fn_##X
#define LOAD_SDL_FUNCTION(X) fn_##X = GetSDLFunction<decltype(X)*>(#X)
#define LOAD_MIXER_FUNCTION(X) fn_##X = GetSDLMixerFunction<decltype(X)*>(#X)

SDL_FUNCTION_DECLRATION(SDL_Init)                         = NULL;
SDL_FUNCTION_DECLRATION(SDL_PollEvent)                    = NULL;
SDL_FUNCTION_DECLRATION(Mix_OpenAudio)                    = NULL;
SDL_FUNCTION_DECLRATION(Mix_Init)                         = NULL;
SDL_FUNCTION_DECLRATION(SDL_NumJoysticks)                 = NULL;
SDL_FUNCTION_DECLRATION(SDL_GameControllerOpen)           = NULL;
SDL_FUNCTION_DECLRATION(SDL_GameControllerClose)          = NULL;
SDL_FUNCTION_DECLRATION(SDL_GameControllerGetPlayerIndex) = NULL;
SDL_FUNCTION_DECLRATION(SDL_GameControllerGetJoystick)    = NULL;
SDL_FUNCTION_DECLRATION(SDL_HapticOpenFromJoystick)       = NULL;
SDL_FUNCTION_DECLRATION(Mix_AllocateChannels)             = NULL;
SDL_FUNCTION_DECLRATION(Mix_ReserveChannels)              = NULL;
SDL_FUNCTION_DECLRATION(Mix_LoadWAV)                      = NULL;
SDL_FUNCTION_DECLRATION(Mix_LoadMUS)                      = NULL;
SDL_FUNCTION_DECLRATION(Mix_FadeOutChannel)               = NULL;
SDL_FUNCTION_DECLRATION(Mix_Playing)                      = NULL;
SDL_FUNCTION_DECLRATION(Mix_Pause)                        = NULL;
SDL_FUNCTION_DECLRATION(Mix_Resume)                       = NULL;
SDL_FUNCTION_DECLRATION(Mix_Volume)                       = NULL;
SDL_FUNCTION_DECLRATION(Mix_SetPosition)                  = NULL;
SDL_FUNCTION_DECLRATION(Mix_PlayChannel)                  = NULL;
SDL_FUNCTION_DECLRATION(Mix_HaltChannel)                  = NULL;
SDL_FUNCTION_DECLRATION(Mix_VolumeMusic)                  = NULL;
SDL_FUNCTION_DECLRATION(Mix_FadeInMusic)                  = NULL;
SDL_FUNCTION_DECLRATION(Mix_FadeOutMusic)                 = NULL;
SDL_FUNCTION_DECLRATION(Mix_PlayingMusic)                 = NULL;
SDL_FUNCTION_DECLRATION(SDL_GameControllerGetButton)      = NULL;
SDL_FUNCTION_DECLRATION(SDL_JoystickInstanceID)           = NULL;
SDL_FUNCTION_DECLRATION(SDL_JoystickGetDeviceInstanceID)  = NULL;
SDL_FUNCTION_DECLRATION(SDL_JoystickUpdate)               = NULL;
SDL_FUNCTION_DECLRATION(SDL_GameControllerName)           = NULL;
SDL_FUNCTION_DECLRATION(SDL_IsGameController)             = NULL;
SDL_FUNCTION_DECLRATION(SDL_GameControllerRumble)         = NULL;
SDL_FUNCTION_DECLRATION(SDL_GetError)                     = NULL;

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



		missionClearSong = fn_Mix_LoadMUS(((std::string)Paths::sounds + "\\music\\missionclear.mp3").c_str());
        divinityStatueSong = fn_Mix_LoadMUS(((std::string)Paths::sounds + "\\music\\divinitystatue.mp3").c_str());


		cacheAudioFiles = true;
	}
}

void AddController(int index) {
	if (fn_SDL_IsGameController(index)) {
		SDL_GameController* controller = fn_SDL_GameControllerOpen(index);
		if (controller) {
			SDL_JoystickID instanceID = fn_SDL_JoystickInstanceID(fn_SDL_GameControllerGetJoystick(controller));
			currentlyConnected.insert(instanceID);
			bool assigned = false;
			for (auto& ctrl : controllers) {
				if (ctrl == NULL) {
					ctrl = controller;
					assigned = true;
					break;
				}
			}
			if (!assigned) {
				std::cerr << "Too many controllers connected. Ignoring controller " << index << "\n";
				fn_SDL_GameControllerClose(controller);
			}
			else {
				std::cout << "Opened controller " << index << ": " << fn_SDL_GameControllerName(controller) << std::endl;
			}
		}
		else {
			std::cerr << "Could not open controller " << index << ": " << fn_SDL_GetError() << std::endl;
		}
	}
}

void RemoveController(SDL_JoystickID instanceID) {
	for (int i = 0; i < controllers.size(); ++i) {
		if (controllers[i] && fn_SDL_JoystickInstanceID(fn_SDL_GameControllerGetJoystick(controllers[i])) == instanceID) {
			fn_SDL_GameControllerClose(controllers[i]);
			controllers[i] = NULL;
			currentlyConnected.erase(instanceID);
			std::cout << "Closed controller at index " << i << "  instanceID: " << instanceID << "\n";
			break;
		}
	}
}

void InitControllers() {
	for (int i = 0; i < fn_SDL_NumJoysticks(); ++i) {
        AddController(i);
	}
}

void InitSDL() {
    if (!SDL2Init) {
        // Get the function addresses
        LOAD_SDL_FUNCTION(SDL_Init);
        LOAD_SDL_FUNCTION(SDL_PollEvent);
        LOAD_SDL_FUNCTION(SDL_NumJoysticks);
        LOAD_SDL_FUNCTION(SDL_GameControllerOpen);
        LOAD_SDL_FUNCTION(SDL_GameControllerClose);
        LOAD_SDL_FUNCTION(SDL_GameControllerGetPlayerIndex);
        LOAD_SDL_FUNCTION(SDL_GameControllerGetJoystick);
        LOAD_SDL_FUNCTION(SDL_GameControllerGetButton);
        LOAD_SDL_FUNCTION(SDL_IsGameController);
        LOAD_SDL_FUNCTION(SDL_GameControllerName);
        LOAD_SDL_FUNCTION(SDL_GameControllerRumble);
        LOAD_SDL_FUNCTION(SDL_JoystickInstanceID);
        LOAD_SDL_FUNCTION(SDL_JoystickGetDeviceInstanceID);
        LOAD_SDL_FUNCTION(SDL_JoystickUpdate);
        LOAD_SDL_FUNCTION(SDL_GetError);
        LOAD_MIXER_FUNCTION(Mix_AllocateChannels);
        LOAD_MIXER_FUNCTION(Mix_ReserveChannels);
        LOAD_MIXER_FUNCTION(Mix_LoadWAV);
        LOAD_MIXER_FUNCTION(Mix_LoadMUS);
        LOAD_MIXER_FUNCTION(Mix_FadeOutChannel);
        LOAD_MIXER_FUNCTION(Mix_Playing);
        LOAD_MIXER_FUNCTION(Mix_Pause);
        LOAD_MIXER_FUNCTION(Mix_Resume);
        LOAD_MIXER_FUNCTION(Mix_Volume);
        LOAD_MIXER_FUNCTION(Mix_PlayChannel);
        LOAD_MIXER_FUNCTION(Mix_SetPosition);
        LOAD_MIXER_FUNCTION(Mix_HaltChannel);
        LOAD_MIXER_FUNCTION(Mix_VolumeMusic);
        LOAD_MIXER_FUNCTION(Mix_FadeInMusic);
        LOAD_MIXER_FUNCTION(Mix_FadeOutMusic);
        LOAD_MIXER_FUNCTION(Mix_PlayingMusic);
        LOAD_MIXER_FUNCTION(Mix_OpenAudio);
        LOAD_MIXER_FUNCTION(Mix_Init);
        

        if (fn_SDL_NumJoysticks == NULL) {
            // TODO: Handle the error
        }

        if (fn_SDL_GameControllerOpen == NULL) {
            // TODO: Handle the error
        }

        if (fn_SDL_GameControllerGetPlayerIndex == NULL) {
            // TODO: Handle the error
        }

        if (fn_SDL_GameControllerGetJoystick == NULL) {
            // TODO: Handle the error
        }

        if (fn_SDL_HapticOpenFromJoystick == NULL) {
            // TODO: Handle the error
        }

        if (fn_Mix_AllocateChannels == NULL) {
            // TODO: Handle the error
        }

        if (fn_Mix_ReserveChannels == NULL) {
            // TODO: Handle the error
        }
        if (fn_Mix_LoadWAV == NULL) {
            // TODO: Handle the error
        }

        if (fn_Mix_LoadMUS == NULL) {
            // TODO: Handle the error
        }

        if (fn_SDL_Init == NULL) {
            // TODO: Handle the error
        }

        if (fn_Mix_OpenAudio == NULL) {
            // TODO: Handle the error
        }

        if (fn_Mix_Init == NULL) {
            // TODO: Handle the error
        }

        if (fn_SDL_Init(SDL_INIT_AUDIO | SDL_INIT_HAPTIC | SDL_INIT_GAMECONTROLLER) == -1) {
            SDL2Initialization = "SDL2 Error";
        } else {
            SDL2Initialization = "SDL2 Success";
        }

        if (fn_Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 512) == -1) {
            MixerInitialization = "Mixer Error";
        } else {
            MixerInitialization = "Mixer Success";
        }

        int flags = MIX_INIT_OGG | MIX_INIT_MP3;
        if (!fn_Mix_Init(flags)) {
            MixerInitialization2 = "Mixer2 Error";
        } else {
            MixerInitialization2 = "Mixer2 Success";
        }

        mainController = NULL;
        for (int i = 0; i < 4; ++i) {
            mainController = fn_SDL_GameControllerOpen(i);
            if (mainController) {
                break;
            }
        }


        int controllerIndex = fn_SDL_GameControllerGetPlayerIndex(mainController);

        InitControllers();
        CrimsonUtil::ReverseNonNull(controllers);

        SDL2Init = true;
    }


    // CHUNKS OF SOUND
    fn_Mix_AllocateChannels(500);

    // RESERVES SELECT EFFECT SOUND FOR CHANNELS 100 AND ABOVE
    fn_Mix_ReserveChannels(100);

    LoadAllSFX();
}


bool IsControllerButtonDown(int controllerIndex, int button) {
   return fn_SDL_GameControllerGetButton(controllers[controllerIndex], (SDL_GameControllerButton)button);
}


void CheckAndOpenControllers() {
	SDL_Event event;

	while (fn_SDL_PollEvent(&event)) {
		if (event.type == SDL_CONTROLLERDEVICEADDED) {
			AddController(event.cdevice.which);
		} else if (event.type == SDL_CONTROLLERDEVICEREMOVED) {
			RemoveController(event.cdevice.which); 
		}
	}
}

void UpdateJoysticks() {
    fn_SDL_JoystickUpdate();
}

void VibrateController(int controllerIndex, Uint16 rumbleStrengthLowFreq, Uint16 rumbleStrengthHighFreq, int rumbleDuration) {
    if (controllers[controllerIndex] != NULL) {
		if (fn_SDL_GameControllerRumble(controllers[controllerIndex], rumbleStrengthLowFreq, rumbleStrengthHighFreq, rumbleDuration) == 0) {
			//std::cout << "SWEET VIBRATION " << controllerIndex << std::endl;
		}
		else {
			std::cerr << "Vibration ERROR " << 0 << ": " << fn_SDL_GetError() << std::endl;
		}
    }
}

void FadeOutChannels(int channelException, int initialChannel, int numChannels, int fadeOutms) {

    for (int i = initialChannel; i < numChannels; i++) {

        if (i != channelException) {

            fn_Mix_FadeOutChannel(i, fadeOutms);
        }
    }
}

void PlayOnChannels(int initialChannel, int finalChannel, Mix_Chunk* sfx, int volume) {

    for (int i = initialChannel; i <= finalChannel; i++) {
        if (!fn_Mix_Playing(i)) {
            fn_Mix_Volume(i, volume);
            fn_Mix_PlayChannel(i, sfx, 0);
            break;
        } else {
            i++;
        }
    }
}

void PlayOnChannelsFadeOut(int initialChannel, int finalChannel, Mix_Chunk* sfx, int volume, int fadeOutms) {
    int channelBeingPlayed = 0;

    for (int i = initialChannel; i <= finalChannel; i++) {
        if (!fn_Mix_Playing(i)) {
            fn_Mix_Volume(i, volume);
            fn_Mix_PlayChannel(i, sfx, 0);
            channelBeingPlayed = i;
            break;
        } else {
            i++;
        }
    }

    FadeOutChannels(channelBeingPlayed, initialChannel, finalChannel, fadeOutms);
}

void PlayOnChannelsFadeOutPosition(int initialChannel, int finalChannel, Mix_Chunk* sfx, int volume, int fadeOutms, int angle, int distance) {
	int channelBeingPlayed = 0;


	for (int i = initialChannel; i <= finalChannel; i++) {
		if (!fn_Mix_Playing(i)) {
			fn_Mix_Volume(i, volume);
            fn_Mix_SetPosition(i, angle, distance);
			fn_Mix_PlayChannel(i, sfx, 0);
			channelBeingPlayed = i;
			break;
		}
		else {
			i++;
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
	int volume = (int)(80.0f * slider);
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

void SetSFXDistanceMultipleChannels(int playerIndex, int initialChannel, int numberChannelsPerPlayer, int angle, int distance) {
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
}

void StyleRankCooldownTracker(int rank) {
    rankAnnouncer[rank].trackerRunning = true;
    rankAnnouncer[rank].offCooldown    = false;
    std::this_thread::sleep_for(std::chrono::seconds(activeCrimsonConfig.SFX.styleRankAnnouncerCooldownSeconds));
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

    if (rankAnnouncer[rank - 1].turn == 0 && rankAnnouncer[rank - 1].count == 0 && rankAnnouncer[rank - 1].offCooldown) {
        fn_Mix_Volume(CHANNEL::initialStyleRank + (rank - 1), volume);
        fn_Mix_PlayChannel(CHANNEL::initialStyleRank + (rank - 1), styleRankWAV, 0);
        rankAnnouncer[rank - 1].turn++;

        if (!rankAnnouncer[rank - 1].trackerRunning) {
            std::thread stylerankcooldowntracker(StyleRankCooldownTracker, rank - 1);
            stylerankcooldowntracker.detach();
        }


    } else if (rankAnnouncer[rank - 1].turn == 1 && rankAnnouncer[rank - 1].count == 0 && rankAnnouncer[rank - 1].offCooldown) {
        fn_Mix_Volume(CHANNEL::initialStyleRank + (rank - 1), volume);
        fn_Mix_PlayChannel(CHANNEL::initialStyleRank + (rank - 1), styleRankWAVAlt, 0);
        rankAnnouncer[rank - 1].turn = 0;

        if (!rankAnnouncer[rank - 1].trackerRunning) {
            std::thread stylerankcooldowntracker(StyleRankCooldownTracker, rank - 1);
            stylerankcooldowntracker.detach();
        }
    }


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

void PlayNewMissionClearSong() {
    fn_Mix_VolumeMusic(60 * (activeCrimsonConfig.Sound.channelVolumes[9] / 100.0f));
    fn_Mix_FadeInMusic(missionClearSong, -1, 500);
}

void PlayDivinityStatueSong() {
	fn_Mix_VolumeMusic(57 * (activeCrimsonConfig.Sound.channelVolumes[9] / 100.0f));
	fn_Mix_FadeInMusic(divinityStatueSong, -1, 500);
}

void FadeOutMusic() {
    fn_Mix_FadeOutMusic(500);
}

int IsMusicPlaying() {
    return fn_Mix_PlayingMusic();
}

}
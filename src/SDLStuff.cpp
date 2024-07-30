#define SDL_MAIN_HANDLED
#include "../ThirdParty/SDL2/SDL.h"
#include "../ThirdParty/SDL2/SDL_mixer.h"
#include <string>
#include <thread>
#include <chrono>

// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "SDLStuff.hpp"
#include "Core/Core.hpp"
#include "Config.hpp"
#include "SDL.hpp"
#include "Vars.hpp"
#include "CrimsonFileHandling.hpp"

SDL_GameController* controller = NULL;
SDL_Joystick* joystick;
SDL_Haptic* styleHaptic;
std::string SDL2Initialization   = "";
std::string MixerInitialization  = "";
std::string MixerInitialization2 = "";
bool SDL2Init                    = false;
bool cacheAudioFiles             = false;
Mix_Chunk* changeGun;
Mix_Chunk* changeDevilArm;
Mix_Chunk* styleChange;
Mix_Chunk* tricksterVO;
Mix_Chunk* swordmasterVO;
Mix_Chunk* gunslingerVO;
Mix_Chunk* royalguardVO;
Mix_Chunk* quicksilverVO;
Mix_Chunk* doppelgangerVO;
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
Mix_Music* missionClearSong;

// Mix Channels used
namespace CHANNEL {
    constexpr int initialDevilArm = 0; // to 19
    constexpr int initialChangeGun = 20; // to 39
    constexpr int initialStyleChange = 40; // to 119, 20 channels per player
    constexpr int initialStyleChangeVO = 120; // to 199, 20 channels per player
    constexpr int initialStyleRank = 200; // to 206
    constexpr int initialSprint = 300; // to 307, 2 channel per player
    constexpr int initialDTIn = 307; // to 314, 2 channels per player
    constexpr int initialDTOut = 315; // to 318, 1 channel per player
    constexpr int initialDTLoop = 319; // to 322, 1 channel per player
    constexpr int initialDoppIn = 323; // to 326, 1 channel per player
    constexpr int initialDoppOut = 327; // to 330, 1 channel per player
	constexpr int quickIn = 328;
	constexpr int quickOut = 329;
	constexpr int initialDTReady = 330; // to 333, 1 channel per player
	constexpr int initialDelayedCombo1 = 334; // to 337, 1 channel per player
    constexpr int initialDelayedCombo2 = 338; // to 341, 1 channel per player
    constexpr int initialDTEStart = 342; // to 345, 1 channel per player
    constexpr int initialDTELoop = 346; // to 349, 1 channel per player
    constexpr int initialDTEFinish = 350; // to 353, 1 channel per player
    constexpr int initialDTERelease = 354; // to 357, 1 channel per player

}

#define SDL_FUNCTION_DECLRATION(X) decltype(X)* fn_##X
#define LOAD_SDL_FUNCTION(X) fn_##X = GetSDLFunction<decltype(X)*>(#X)
#define LOAD_MIXER_FUNCTION(X) fn_##X = GetSDLMixerFunction<decltype(X)*>(#X)

SDL_FUNCTION_DECLRATION(SDL_Init)                         = NULL;
SDL_FUNCTION_DECLRATION(Mix_OpenAudio)                    = NULL;
SDL_FUNCTION_DECLRATION(Mix_Init)                         = NULL;
SDL_FUNCTION_DECLRATION(SDL_NumJoysticks)                 = NULL;
SDL_FUNCTION_DECLRATION(SDL_GameControllerOpen)           = NULL;
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
SDL_FUNCTION_DECLRATION(Mix_Volume)                       = NULL;
SDL_FUNCTION_DECLRATION(Mix_SetPosition)                  = NULL;
SDL_FUNCTION_DECLRATION(Mix_PlayChannel)                  = NULL;
SDL_FUNCTION_DECLRATION(Mix_HaltChannel)                  = NULL;
SDL_FUNCTION_DECLRATION(Mix_VolumeMusic)                  = NULL;
SDL_FUNCTION_DECLRATION(Mix_FadeInMusic)                  = NULL;
SDL_FUNCTION_DECLRATION(Mix_FadeOutMusic)                 = NULL;
SDL_FUNCTION_DECLRATION(Mix_PlayingMusic)                 = NULL;
SDL_FUNCTION_DECLRATION(SDL_JoystickGetButton) = NULL;

void LoadAllSFX() {
	if (!cacheAudioFiles) {

		changeGun = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\changegun.wav").c_str());
		changeDevilArm = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\changedevilarm.wav").c_str());
		styleChange = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\stylechange.wav").c_str());
		tricksterVO = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\trickster1.wav").c_str());
		swordmasterVO = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\swordmaster1.wav").c_str());
		gunslingerVO = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\gunslinger1.wav").c_str());
		royalguardVO = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\royalguard1.wav").c_str());
		quicksilverVO = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\quicksilver1.wav").c_str());
		doppelgangerVO = fn_Mix_LoadWAV(((std::string)Paths::sounds + "\\doppelganger2.wav").c_str());
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



		missionClearSong = fn_Mix_LoadMUS(((std::string)Paths::sounds + "\\music\\missionclear.mp3").c_str());


		cacheAudioFiles = true;
	}
}

void InitSDL() {
    if (!SDL2Init) {
        // Get the function addresses
        LOAD_SDL_FUNCTION(SDL_Init);
        LOAD_SDL_FUNCTION(SDL_NumJoysticks);
        LOAD_SDL_FUNCTION(SDL_GameControllerOpen);
        LOAD_SDL_FUNCTION(SDL_GameControllerGetPlayerIndex);
        LOAD_SDL_FUNCTION(SDL_GameControllerGetJoystick);
        LOAD_SDL_FUNCTION(SDL_HapticOpenFromJoystick);
        LOAD_SDL_FUNCTION(SDL_JoystickGetButton);
        LOAD_MIXER_FUNCTION(Mix_AllocateChannels);
        LOAD_MIXER_FUNCTION(Mix_ReserveChannels);
        LOAD_MIXER_FUNCTION(Mix_LoadWAV);
        LOAD_MIXER_FUNCTION(Mix_LoadMUS);
        LOAD_MIXER_FUNCTION(Mix_FadeOutChannel);
        LOAD_MIXER_FUNCTION(Mix_Playing);
        LOAD_MIXER_FUNCTION(Mix_Pause);
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

        controller = NULL;
        for (int i = 0; i < fn_SDL_NumJoysticks(); ++i) {
            controller = fn_SDL_GameControllerOpen(i);
            if (controller) {
                break;
            }
        }

        int controllerIndex = fn_SDL_GameControllerGetPlayerIndex(controller);


        joystick  = fn_SDL_GameControllerGetJoystick(controller);
        styleHaptic = fn_SDL_HapticOpenFromJoystick(joystick);

        SDL2Init = true;
    }


    // CHUNKS OF SOUND
    fn_Mix_AllocateChannels(500);

    // RESERVES SELECT EFFECT SOUND FOR CHANNELS 100 AND ABOVE
    fn_Mix_ReserveChannels(100);

    LoadAllSFX();
}


bool IsJoystickButtonDown(SDL_Joystick* joystick, int button) {
   return fn_SDL_JoystickGetButton(joystick, button);
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
    PlayOnChannelsFadeOutPosition(CHANNEL::initialDevilArm, CHANNEL::initialDevilArm + 19, changeDevilArm, activeConfig.SFX.changeWeaponVolume, 400, 90, 0);
}

void PlayChangeGun() {
    PlayOnChannelsFadeOutPosition(CHANNEL::initialChangeGun, CHANNEL::initialChangeGun + 19, changeGun, activeConfig.SFX.changeWeaponVolume, 400, 270, 0);
}

void PlayStyleChange(int playerIndex) {
    auto initialChannel = CHANNEL::initialStyleChange + (20 * playerIndex);
   
    PlayOnChannelsFadeOut(initialChannel, initialChannel + 19, styleChange, activeConfig.SFX.styleChangeEffectVolume, 150);
}

void PlayStyleChangeVO(int playerIndex, int style) {
    auto initialChannel = CHANNEL::initialStyleChangeVO + (20 * playerIndex);

    if (style == 2) {
        PlayOnChannelsFadeOut(initialChannel, initialChannel + 19, tricksterVO, activeConfig.SFX.styleChangeVOVolume, 150);
    } else if (style == 0) {
        PlayOnChannelsFadeOut(initialChannel, initialChannel + 19, swordmasterVO, activeConfig.SFX.styleChangeVOVolume, 150);
    } else if (style == 1) {
        PlayOnChannelsFadeOut(initialChannel, initialChannel + 19, gunslingerVO, activeConfig.SFX.styleChangeVOVolume, 150);
    } else if (style == 3) {
        PlayOnChannelsFadeOut(initialChannel, initialChannel + 19, royalguardVO, activeConfig.SFX.styleChangeVOVolume, 150);
    } else if (style == 4) {
        PlayOnChannelsFadeOut(initialChannel, initialChannel + 19, quicksilverVO, activeConfig.SFX.styleChangeVOVolume, 150);
    } else if (style == 5) {
        PlayOnChannelsFadeOut(initialChannel, initialChannel + 19, doppelgangerVO, activeConfig.SFX.styleChangeVOVolume, 150);
    }
}

void SetAllSFXDistance(int playerIndex, int distance) {
    // This will simulate a pseudo 3D effect for the SFX

    auto initialChannelStyleChange = CHANNEL::initialStyleChange + (20 * playerIndex);
    auto initialChannelStyleChangeVO = CHANNEL::initialStyleChangeVO + (20 * playerIndex);

    for (int i = initialChannelStyleChange; i <= initialChannelStyleChange + 19; i++) {
        fn_Mix_SetPosition(i, 0, distance);
    }

	for (int i = initialChannelStyleChangeVO; i <= initialChannelStyleChangeVO + 19; i++) {
		fn_Mix_SetPosition(i, 0, distance);
	}

    fn_Mix_SetPosition(CHANNEL::initialSprint + playerIndex, 0, distance);
    fn_Mix_SetPosition(CHANNEL::initialSprint + playerIndex + 4, 0, distance); // L2
    fn_Mix_SetPosition(CHANNEL::initialDTIn + playerIndex, 0, distance);
    fn_Mix_SetPosition(CHANNEL::initialDTIn + playerIndex + 4, 0, distance); // L2
    fn_Mix_SetPosition(CHANNEL::initialDTOut + playerIndex, 0, distance);
    fn_Mix_SetPosition(CHANNEL::initialDTLoop + playerIndex, 0, distance);
    fn_Mix_SetPosition(CHANNEL::initialDoppIn + playerIndex, 0, distance);
    fn_Mix_SetPosition(CHANNEL::initialDoppOut + playerIndex, 0, distance);
    fn_Mix_SetPosition(CHANNEL::quickIn, 0, distance);
    fn_Mix_SetPosition(CHANNEL::quickOut, 0, distance);
    fn_Mix_SetPosition(CHANNEL::initialDTReady + playerIndex, 0, distance);
    fn_Mix_SetPosition(CHANNEL::initialDelayedCombo1 + playerIndex, 0, distance);
    fn_Mix_SetPosition(CHANNEL::initialDelayedCombo2 + playerIndex, 0, distance);
    fn_Mix_SetPosition(CHANNEL::initialDTEStart + playerIndex, 0, distance);
    fn_Mix_SetPosition(CHANNEL::initialDTELoop + playerIndex, 0, distance);
    fn_Mix_SetPosition(CHANNEL::initialDTEFinish + playerIndex, 0, distance);
    fn_Mix_SetPosition(CHANNEL::initialDTERelease + playerIndex, 0, distance);
	fn_Mix_SetPosition(CHANNEL::initialDTEStart + playerIndex, 0, distance);
	fn_Mix_SetPosition(CHANNEL::initialDTELoop + playerIndex, 0, distance);
	fn_Mix_SetPosition(CHANNEL::initialDTEFinish + playerIndex, 0, distance);
}

void StyleRankCooldownTracker(int rank) {
    rankAnnouncer[rank].trackerRunning = true;
    rankAnnouncer[rank].offCooldown    = false;
    std::this_thread::sleep_for(std::chrono::seconds(activeConfig.SFX.styleRankAnnouncerCooldownSeconds));
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


    if (rankAnnouncer[rank - 1].turn == 0 && rankAnnouncer[rank - 1].count == 0 && rankAnnouncer[rank - 1].offCooldown) {
        fn_Mix_Volume(CHANNEL::initialStyleRank + (rank - 1), activeConfig.SFX.styleRankAnnouncerVolume);
        fn_Mix_PlayChannel(CHANNEL::initialStyleRank + (rank - 1), styleRankWAV, 0);
        rankAnnouncer[rank - 1].turn++;

        if (!rankAnnouncer[rank - 1].trackerRunning) {
            std::thread stylerankcooldowntracker(StyleRankCooldownTracker, rank - 1);
            stylerankcooldowntracker.detach();
        }


    } else if (rankAnnouncer[rank - 1].turn == 1 && rankAnnouncer[rank - 1].count == 0 && rankAnnouncer[rank - 1].offCooldown) {
        fn_Mix_Volume(CHANNEL::initialStyleRank + (rank - 1), activeConfig.SFX.styleRankAnnouncerVolume);
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

    fn_Mix_Volume(CHANNEL::initialSprint + playerIndex, activeConfig.SFX.sprintVolume);
    fn_Mix_Volume(CHANNEL::initialSprint + 4 + playerIndex, activeConfig.SFX.sprintVolume);
    fn_Mix_PlayChannel(CHANNEL::initialSprint + playerIndex, sprintL1, 0);
    fn_Mix_PlayChannel(CHANNEL::initialSprint + 4 + playerIndex, sprintL2, 0);
}

void PlayDevilTriggerIn(int playerIndex) {

    fn_Mix_Volume(CHANNEL::initialDTIn + playerIndex, activeConfig.SFX.devilTriggerInL1Volume);
    fn_Mix_Volume(CHANNEL::initialDTIn +  4 + playerIndex, activeConfig.SFX.devilTriggerInL2Volume);
    fn_Mix_PlayChannel(CHANNEL::initialDTIn + playerIndex, devilTriggerInL1, 0);
    fn_Mix_PlayChannel(CHANNEL::initialDTIn + 4 + playerIndex, devilTriggerInL2, 0);
}

void PlayDevilTriggerOut(int playerIndex) {

    fn_Mix_Volume(CHANNEL::initialDTOut + playerIndex, activeConfig.SFX.devilTriggerOutVolume);
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
    fn_Mix_Volume(CHANNEL::initialDoppIn + playerIndex, activeConfig.SFX.doppelgangerInVolume);
    fn_Mix_PlayChannel(CHANNEL::initialDoppIn + playerIndex, doppelgangerIn, 0);
}

void PlayDoppelgangerOut(int playerIndex) {
    fn_Mix_Volume(CHANNEL::initialDoppOut + playerIndex, activeConfig.SFX.doppelgangerOutVolume);
    fn_Mix_PlayChannel(CHANNEL::initialDoppOut + playerIndex, doppelgangerOut, 0);
}

void PlayQuicksilverIn() {

    fn_Mix_Volume(CHANNEL::quickIn, activeConfig.SFX.quicksilverInVolume);
    fn_Mix_PlayChannel(CHANNEL::quickIn, quicksilverIn, 0);
}

void PlayDevilTriggerReady(int playerIndex) {

    fn_Mix_Volume(CHANNEL::initialDTReady + playerIndex, activeConfig.SFX.devilTriggerReadyVolume);
    fn_Mix_PlayChannel(CHANNEL::initialDTReady + playerIndex, devilTriggerReady, 0);
}

void PlayDelayedCombo1(int playerIndex) {
    fn_Mix_Volume(CHANNEL::initialDelayedCombo1 + playerIndex, 25);
    fn_Mix_PlayChannel(CHANNEL::initialDelayedCombo1 + playerIndex, delayedCombo1, 0);
}

void PlayDelayedCombo2(int playerIndex) {
    fn_Mix_Volume(CHANNEL::initialDelayedCombo2 + playerIndex, 100);
    fn_Mix_PlayChannel(CHANNEL::initialDelayedCombo2 + playerIndex, delayedCombo2, 0);
}

bool ChannelIsPlaying(int channel) {
    return fn_Mix_Playing(channel);
}

bool DTEStartIsPlaying(int playerIndex) {
    return ChannelIsPlaying(CHANNEL::initialDTEStart + playerIndex);
}

void PlayDTExplosionStart(int playerIndex, int volume) {
    // starts at channel 312, to 315 for 4P
    fn_Mix_Volume(CHANNEL::initialDTEStart + playerIndex, volume);
    fn_Mix_PlayChannel(CHANNEL::initialDTEStart + playerIndex, dtExplosionStart, 0);
}

void PlayDTExplosionLoop(int playerIndex, int volume) {
	// starts at channel 316, to 319 for 4P
	fn_Mix_Volume(CHANNEL::initialDTELoop + playerIndex, volume);
	fn_Mix_PlayChannel(CHANNEL::initialDTELoop + playerIndex, dtExplosionLoop, -1);
}

void PlayDTExplosionFinish(int playerIndex, int volume) {
    // starts at channel 320, to 323 for 4P
    fn_Mix_Volume(CHANNEL::initialDTEFinish + playerIndex, volume);
    fn_Mix_PlayChannel(CHANNEL::initialDTEFinish + playerIndex, dtExplosionFinish, 0);
}

void PlayDTEExplosionRelease(int playerIndex, int volume) {
    // starts at channel 324, to 327 for 4P
	fn_Mix_Volume(CHANNEL::initialDTERelease + playerIndex, volume);
	fn_Mix_PlayChannel(CHANNEL::initialDTERelease + playerIndex, dtExplosionRelease, 0);
}

void InterruptDTExplosionSFX(int playerIndex) {
    fn_Mix_HaltChannel(CHANNEL::initialDTEStart + playerIndex);
    fn_Mix_HaltChannel(CHANNEL::initialDTELoop + playerIndex);
}

void PlayNewMissionClearSong() {
    fn_Mix_VolumeMusic(128 * activeConfig.channelVolumes[9]);
    fn_Mix_FadeInMusic(missionClearSong, -1, 500);
}

void FadeOutNewMissionClearSong() {
    fn_Mix_FadeOutMusic(500);
}

int IsMusicPlaying() {
    return fn_Mix_PlayingMusic();
}
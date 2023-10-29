module;

#define SDL_MAIN_HANDLED
#include "../ThirdParty/SDL2/SDL.h"
#include "../ThirdParty/SDL2/SDL_mixer.h"
#include <string>
#include <thread>
#include <chrono>
export module ExtraSound;

import SDL;
import Config;
import Vars;
import Core;

export std::string SDL2Initialization = "";
export std::string MixerInitialization = "";
export std::string MixerInitialization2 = "";
export bool SDL2Init = false;
export bool cacheAudioFiles = false;
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
Mix_Music* missionClearSong;

#define SDL_FUNCTION_DECLRATION(X) decltype(X)* fn_##X
#define LOAD_SDL_FUNCTION(X) fn_##X = GetSDLFunction<decltype(X)*>(#X)
#define LOAD_MIXER_FUNCTION(X) fn_##X = GetSDLMixerFunction<decltype(X)*>(#X)

SDL_FUNCTION_DECLRATION(SDL_Init) = NULL;
SDL_FUNCTION_DECLRATION(Mix_OpenAudio) = NULL;
SDL_FUNCTION_DECLRATION(Mix_Init) = NULL;
SDL_FUNCTION_DECLRATION(SDL_NumJoysticks) = NULL;
SDL_FUNCTION_DECLRATION(SDL_GameControllerOpen) = NULL;
SDL_FUNCTION_DECLRATION(SDL_GameControllerGetPlayerIndex) = NULL;
SDL_FUNCTION_DECLRATION(SDL_GameControllerGetJoystick) = NULL;
SDL_FUNCTION_DECLRATION(SDL_HapticOpenFromJoystick) = NULL;
SDL_FUNCTION_DECLRATION(Mix_AllocateChannels) = NULL;
SDL_FUNCTION_DECLRATION(Mix_ReserveChannels) = NULL;
SDL_FUNCTION_DECLRATION(Mix_LoadWAV) = NULL;
SDL_FUNCTION_DECLRATION(Mix_LoadMUS) = NULL;
SDL_FUNCTION_DECLRATION(Mix_FadeOutChannel) = NULL;
SDL_FUNCTION_DECLRATION(Mix_Playing) = NULL;
SDL_FUNCTION_DECLRATION(Mix_Volume) = NULL;
SDL_FUNCTION_DECLRATION(Mix_PlayChannel) = NULL;
SDL_FUNCTION_DECLRATION(Mix_HaltChannel) = NULL;
SDL_FUNCTION_DECLRATION(Mix_VolumeMusic) = NULL;
SDL_FUNCTION_DECLRATION(Mix_FadeInMusic) = NULL;
SDL_FUNCTION_DECLRATION(Mix_FadeOutMusic) = NULL;
SDL_FUNCTION_DECLRATION(Mix_PlayingMusic) = NULL;

export void initSDL() {
    if (!SDL2Init) {
        // Get the function addresses
		LOAD_SDL_FUNCTION(SDL_Init);
		LOAD_SDL_FUNCTION(SDL_NumJoysticks);
		LOAD_SDL_FUNCTION(SDL_GameControllerOpen);
		LOAD_SDL_FUNCTION(SDL_GameControllerGetPlayerIndex);
		LOAD_SDL_FUNCTION(SDL_GameControllerGetJoystick);
		LOAD_SDL_FUNCTION(SDL_HapticOpenFromJoystick);
		LOAD_MIXER_FUNCTION(Mix_AllocateChannels);
		LOAD_MIXER_FUNCTION(Mix_ReserveChannels);
		LOAD_MIXER_FUNCTION(Mix_LoadWAV);
		LOAD_MIXER_FUNCTION(Mix_LoadMUS);
		LOAD_MIXER_FUNCTION(Mix_FadeOutChannel);
		LOAD_MIXER_FUNCTION(Mix_Playing);
		LOAD_MIXER_FUNCTION(Mix_Volume);
		LOAD_MIXER_FUNCTION(Mix_PlayChannel);
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
        }
        else {
            SDL2Initialization = "SDL2 Success";
        }

        if (fn_Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 512) == -1) {
            MixerInitialization = "Mixer Error";
        }
        else {
            MixerInitialization = "Mixer Success";
        }

        int flags = MIX_INIT_OGG | MIX_INIT_MP3;
        if (!fn_Mix_Init(flags)) {
            MixerInitialization2 = "Mixer2 Error";
        }
        else {
            MixerInitialization2 = "Mixer2 Success";
        }

        SDL_GameController* controller = NULL;
        for (int i = 0; i < fn_SDL_NumJoysticks(); ++i) {
            controller = fn_SDL_GameControllerOpen(i);
            if (controller) {
                break;
            }
        }

        int controllerIndex = fn_SDL_GameControllerGetPlayerIndex(controller);


        SDL_Joystick* joystick = fn_SDL_GameControllerGetJoystick(controller);
        SDL_Haptic* styleHaptic = fn_SDL_HapticOpenFromJoystick(joystick);

        SDL2Init = true;
    }

    // CHUNKS OF SOUND
    fn_Mix_AllocateChannels(500);

    // RESERVES SELECT EFFECT SOUND FOR CHANNELS 100 AND ABOVE
    fn_Mix_ReserveChannels(100);

    if (!cacheAudioFiles) {

        changeGun = fn_Mix_LoadWAV("sound/changegun.wav");
        changeDevilArm = fn_Mix_LoadWAV("sound/changedevilarm.wav");
        styleChange = fn_Mix_LoadWAV("sound/stylechange.wav");
        tricksterVO = fn_Mix_LoadWAV("sound/trickster1.wav");
        swordmasterVO = fn_Mix_LoadWAV("sound/swordmaster1.wav");
        gunslingerVO = fn_Mix_LoadWAV("sound/gunslinger1.wav");
        royalguardVO = fn_Mix_LoadWAV("sound/royalguard1.wav");
        quicksilverVO = fn_Mix_LoadWAV("sound/quicksilver1.wav");
        doppelgangerVO = fn_Mix_LoadWAV("sound/doppelganger2.wav");
        sprintL1 = fn_Mix_LoadWAV("sound/sprint_l1.wav");
        sprintL2 = fn_Mix_LoadWAV("sound/sprint_l2.wav");
        devilTriggerInL1 = fn_Mix_LoadWAV("sound/dt_activation_l1.wav");
        devilTriggerInL2 = fn_Mix_LoadWAV("sound/dt_activation_l2.wav");
        devilTriggerOut = fn_Mix_LoadWAV("sound/dt_deactivation.wav");
        devilTriggerLoop = fn_Mix_LoadWAV("sound/dt_loop.wav");
        doppelgangerIn = fn_Mix_LoadWAV("sound/dopp_activation.wav");
        doppelgangerOut = fn_Mix_LoadWAV("sound/dopp_deactivation.wav");
        quicksilverIn = fn_Mix_LoadWAV("sound/qs_activation.wav");
        devilTriggerReady = fn_Mix_LoadWAV("sound/dt_ready.wav");
        styleRankD1 = fn_Mix_LoadWAV("sound/styleranks/d1.wav");
        styleRankD2 = fn_Mix_LoadWAV("sound/styleranks/d2.wav");
        styleRankC1 = fn_Mix_LoadWAV("sound/styleranks/c1.wav");
        styleRankC2 = fn_Mix_LoadWAV("sound/styleranks/c2.wav");
        styleRankB1 = fn_Mix_LoadWAV("sound/styleranks/b1.wav");
        styleRankB2 = fn_Mix_LoadWAV("sound/styleranks/b2.wav");
        styleRankA1 = fn_Mix_LoadWAV("sound/styleranks/a1.wav");
        styleRankA2 = fn_Mix_LoadWAV("sound/styleranks/a2.wav");
        styleRankS1 = fn_Mix_LoadWAV("sound/styleranks/s1.wav");
        styleRankS2 = fn_Mix_LoadWAV("sound/styleranks/s2.wav");
        styleRankSS1 = fn_Mix_LoadWAV("sound/styleranks/ss1.wav");
        styleRankSS2 = fn_Mix_LoadWAV("sound/styleranks/ss2.wav");
        styleRankSSS1 = fn_Mix_LoadWAV("sound/styleranks/sss1.wav");
        styleRankSSS2 = fn_Mix_LoadWAV("sound/styleranks/sss2.wav");
        missionClearSong = fn_Mix_LoadMUS("sound/music/missionclear.mp3");


        cacheAudioFiles = true;
    }
}



/*void Shout(int channel, int initialChannel, int numChannels, Mix_Chunk* shout, int fadeOutmsStyle) {
    int volume;

    if (asVergil == 0) {
        if (inDevilTrigger == 0 || playDTShoutsWhenDT == 0) {
            volume = shoutVolume;

        }
        else if (inDevilTrigger == 1 && playDTShoutsWhenDT == 1) {
            volume = shoutDTVolume;
        }
    }
    else if (asVergil == 1) {
        if (vergilShoutLinesEnabled == 1) {
            if (inDevilTrigger == 0 || playDTShoutsWhenDT == 0) {
                volume = shoutVergilVolume;

            }
            else if (inDevilTrigger == 1 && playDTShoutsWhenDT == 1) {
                volume = shoutVergilDTVolume;
            }
        }

    }

    Mix_Volume(channel, volume);
    Mix_PlayChannel(channel, shout, 0);
    FadeOutChannels(channel, initialChannel, numChannels, 150);
}*/

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
        }
        else {
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
        }
        else {
            i++;
        }
    }

    FadeOutChannels(channelBeingPlayed, initialChannel, finalChannel, fadeOutms);
}

export void playChangeDevilArm() {
    PlayOnChannelsFadeOut(0, 19, changeDevilArm, activeConfig.SFX.changeWeaponVolume, 400);
}

export void playChangeGun() {
    PlayOnChannelsFadeOut(20, 39, changeGun, activeConfig.SFX.changeWeaponVolume, 400);
}

export void playStyleChange() {
    PlayOnChannelsFadeOut(40, 59, styleChange, activeConfig.SFX.styleChangeEffectVolume, 150);
}

export void playStyleChangeVO(int style) {
    if (style == 2) {
        PlayOnChannelsFadeOut(60, 99, tricksterVO, activeConfig.SFX.styleChangeVOVolume, 150);
    }
    else if (style == 0) {
        PlayOnChannelsFadeOut(60, 99, swordmasterVO, activeConfig.SFX.styleChangeVOVolume, 150);
    }
    else if (style == 1) {
        PlayOnChannelsFadeOut(60, 99, gunslingerVO, activeConfig.SFX.styleChangeVOVolume, 150);
    }
    else if (style == 3) {
        PlayOnChannelsFadeOut(60, 99, royalguardVO, activeConfig.SFX.styleChangeVOVolume, 150);
    }
    else if (style == 4) {
        PlayOnChannelsFadeOut(60, 99, quicksilverVO, activeConfig.SFX.styleChangeVOVolume, 150);
    }
    else if (style == 5) {
        PlayOnChannelsFadeOut(60, 99, doppelgangerVO, activeConfig.SFX.styleChangeVOVolume, 150);
    }

}

void StyleRankCooldownTracker(int rank) {
    rankAnnouncer[rank].trackerRunning = true;
    rankAnnouncer[rank].offCooldown = false;
    std::this_thread::sleep_for(std::chrono::seconds(activeConfig.styleRankAnnouncerCooldownSeconds));
    rankAnnouncer[rank].offCooldown = true;
    rankAnnouncer[rank].trackerRunning = false;
}

void SetCurrentStyleRank(int currentStyleRank) {

    for (int i = 0; i < 7; i++) {

        if (i <= currentStyleRank) {
            rankAnnouncer[i].count = 1;
        }
        else {
            rankAnnouncer[i].count = 0;
        }

    }

}

void PlayStyleRank(Mix_Chunk* styleRankWAV, Mix_Chunk* styleRankWAVAlt, int rank) {


    if (rankAnnouncer[rank - 1].turn == 0 && rankAnnouncer[rank - 1].count == 0 && rankAnnouncer[rank - 1].offCooldown) {
        fn_Mix_Volume(100 + (rank - 1), activeConfig.SFX.styleRankAnnouncerVolume);
        fn_Mix_PlayChannel(100 + (rank - 1), styleRankWAV, 0);
        rankAnnouncer[rank - 1].turn++;

        if (!rankAnnouncer[rank - 1].trackerRunning) {
            std::thread stylerankcooldowntracker(StyleRankCooldownTracker, rank - 1);
            stylerankcooldowntracker.detach();
        }


    }
    else if (rankAnnouncer[rank - 1].turn == 1 && rankAnnouncer[rank - 1].count == 0 && rankAnnouncer[rank - 1].offCooldown) {
        fn_Mix_Volume(100 + (rank - 1), activeConfig.SFX.styleRankAnnouncerVolume);
        fn_Mix_PlayChannel(100 + (rank - 1), styleRankWAVAlt, 0);
        rankAnnouncer[rank - 1].turn = 0;

        if (!rankAnnouncer[rank - 1].trackerRunning) {
            std::thread stylerankcooldowntracker(StyleRankCooldownTracker, rank - 1);
            stylerankcooldowntracker.detach();
        }


    }




    SetCurrentStyleRank(rank - 1);


}

export void StyleRankAnnouncerController(int rank) {

    if (rank == 1) {

        PlayStyleRank(styleRankD1, styleRankD2, 1);

    }
    else if (rank == 2) {
        PlayStyleRank(styleRankC1, styleRankC2, 2);
    }
    else if (rank == 3) {
        PlayStyleRank(styleRankB1, styleRankB2, 3);
    }
    else if (rank == 4) {
        PlayStyleRank(styleRankA1, styleRankA2, 4);
    }
    else if (rank == 5) {
        PlayStyleRank(styleRankS1, styleRankS2, 5);
    }
    else if (rank == 6) {
        PlayStyleRank(styleRankSS1, styleRankSS2, 6);
    }
    else if (rank == 7) {
        PlayStyleRank(styleRankSSS1, styleRankSSS2, 7);
    }

    if (rank == 0) {
        rankAnnouncer[0].count = 0;
    }

}


export void playSprint() {

    fn_Mix_Volume(300, activeConfig.SFX.sprintVolume);
    fn_Mix_Volume(301, activeConfig.SFX.sprintVolume);
    fn_Mix_PlayChannel(300, sprintL1, 0);
    fn_Mix_PlayChannel(301, sprintL2, 0);
}

export void PlayDevilTriggerIn() {

    fn_Mix_Volume(302, activeConfig.SFX.devilTriggerInL1Volume);
    fn_Mix_Volume(303, activeConfig.SFX.devilTriggerInL2Volume);
    fn_Mix_PlayChannel(302, devilTriggerInL1, 0);
    fn_Mix_PlayChannel(303, devilTriggerInL2, 0);
}

export void PlayDevilTriggerOut() {

    fn_Mix_Volume(304, activeConfig.SFX.devilTriggerOutVolume);
    fn_Mix_PlayChannel(304, devilTriggerOut, 0);
}

export void playDevilTriggerLoop() {

    fn_Mix_Volume(305, 30);
    fn_Mix_PlayChannel(305, devilTriggerLoop, -1);
}

export void stopDevilTriggerLoop() {

    fn_Mix_HaltChannel(305);
}

export void playDoppelgangerIn() {
    fn_Mix_Volume(306, activeConfig.SFX.doppelgangerInVolume);
    fn_Mix_PlayChannel(306, doppelgangerIn, 0);
}

export void playDoppelgangerOut() {
    fn_Mix_Volume(307, activeConfig.SFX.doppelgangerOutVolume);
    fn_Mix_PlayChannel(307, doppelgangerOut, 0);
}

export void playQuicksilverIn() {

    fn_Mix_Volume(308, activeConfig.SFX.quicksilverInVolume);
    fn_Mix_PlayChannel(308, quicksilverIn, 0);

}

export void PlayDevilTriggerReady() {

    fn_Mix_Volume(309, activeConfig.SFX.devilTriggerReadyVolume);
    fn_Mix_PlayChannel(309, devilTriggerReady, 0);

}

export void PlayNewMissionClearSong() {
    fn_Mix_VolumeMusic(128 * activeConfig.channelVolumes[9]);
    fn_Mix_FadeInMusic(missionClearSong, -1, 500);
}

export void FadeOutNewMissionClearSong() {
    fn_Mix_FadeOutMusic(500);
}

export int isMusicPlaying() {
    return fn_Mix_PlayingMusic();
}
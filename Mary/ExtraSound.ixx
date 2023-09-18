module;

#define SDL_MAIN_HANDLED
#include "../ThirdParty/SDL2/SDL.h"
#include "../ThirdParty/SDL2/SDL_mixer.h"
#include "../ThirdParty/Simple-SDL2/audio.c"
#include "../ThirdParty/Simple-SDL2/audio.h"
#include <string>
export module ExtraSound;

import Config;
import Vars;
import Core;

export std::string SDL2Initialization = "";
export std::string MixerInitialization = "";
export std::string MixerInitialization2 = "";
export int SSDL2AudioInitialization = 0;
export bool SDL2Init = false;
export bool cacheAudioFiles = false;
Mix_Chunk* changeGun;
Mix_Chunk* changeDevilArm;
Mix_Chunk* styleChange;
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







export void initSDL() {
    if(!SDL2Init) {
		if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_HAPTIC | SDL_INIT_GAMECONTROLLER) == -1) {
        SDL2Initialization = "SDL2 Error";
		}
		else {
			SDL2Initialization = "SDL2 Success";
		}
		
        SSDL2AudioInitialization = initAudio();

        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 512) == -1) {
            MixerInitialization = "Mixer Error";
        }
        else {
            MixerInitialization = "Mixer Success";
        }

        int flags = MIX_INIT_OGG | MIX_INIT_MP3;
        if(!Mix_Init(flags)) {
            MixerInitialization2 = "Mixer2 Error";
        }
        else {
            MixerInitialization2 = "Mixer2 Success";
        }
        
        SDL_GameController* controller = NULL;
        for (int i = 0; i < SDL_NumJoysticks(); ++i) {
            controller = SDL_GameControllerOpen(i);
            if (controller) {
                break;
            }
        }
        int controllerIndex = SDL_GameControllerGetPlayerIndex(controller);


        SDL_Joystick* joystick = SDL_GameControllerGetJoystick(controller);
        SDL_Haptic* styleHaptic = SDL_HapticOpenFromJoystick(joystick);

        SDL2Init = true;
	}

    // CHUNKS OF SOUND
    Mix_AllocateChannels(500);

    // RESERVES SELECT EFFECT SOUND FOR CHANNELS 100 AND ABOVE
    Mix_ReserveChannels(100);

	if(!cacheAudioFiles) {

        changeGun = Mix_LoadWAV("sound/changegun.wav");
        changeDevilArm = Mix_LoadWAV("sound/changedevilarm.wav");
        styleChange = Mix_LoadWAV("sound/stylechange.wav");
        sprintL1 = Mix_LoadWAV("sound/sprint_l1.wav");
        sprintL2 = Mix_LoadWAV("sound/sprint_l2.wav");
        devilTriggerInL1 = Mix_LoadWAV("sound/dt_activation_l1.wav");
        devilTriggerInL2 = Mix_LoadWAV("sound/dt_activation_l2.wav");
        devilTriggerOut = Mix_LoadWAV("sound/dt_deactivation.wav");
        devilTriggerLoop = Mix_LoadWAV("sound/dt_loop.wav");
        doppelgangerIn = Mix_LoadWAV("sound/dopp_activation.wav");
        doppelgangerOut = Mix_LoadWAV("sound/dopp_deactivation.wav");
        quicksilverIn = Mix_LoadWAV("sound/qs_activation.wav");

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

            Mix_FadeOutChannel(i, fadeOutms);

        }
    }
}

void PlayOnChannels(int initialChannel, int finalChannel, Mix_Chunk* sfx, int volume) {

    for(int i = initialChannel; i <= finalChannel; i++) {
        if(!Mix_Playing(i)) {
            Mix_Volume(i, volume);
            Mix_PlayChannel(i, sfx, 0);
            break;
        }
        else {
            i++;
        }
    }
}

void PlayOnChannelsFadeOut(int initialChannel, int finalChannel, Mix_Chunk* sfx, int volume, int fadeOutms) {
    int channelBeingPlayed = 0;

    for(int i = initialChannel; i <= finalChannel; i++) {
        if(!Mix_Playing(i)) {
            Mix_Volume(i, volume);
            Mix_PlayChannel(i, sfx, 0);
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
    PlayOnChannels(0, 19, changeDevilArm, activeConfig.SFX.changeWeaponVolume);
}

export void playChangeGun() {
    PlayOnChannels(20, 39, changeGun, activeConfig.SFX.changeWeaponVolume);
}

export void playStyleChange() {
    PlayOnChannelsFadeOut(40, 59, styleChange, activeConfig.SFX.styleChangeVolume, 150);
}

export void playSprint() {
    
    Mix_Volume(300, activeConfig.SFX.sprintVolume);
    Mix_Volume(301, activeConfig.SFX.sprintVolume);
    Mix_PlayChannel(300, sprintL1, 0);
    Mix_PlayChannel(301, sprintL2, 0);
}

export void playDevilTriggerIn() {
    
    Mix_Volume(302, 30);
    Mix_Volume(303, 50);
    Mix_PlayChannel(302, devilTriggerInL1, 0);
    Mix_PlayChannel(303, devilTriggerInL2, 0);
}

export void playDevilTriggerOut() {
    
    Mix_Volume(304, 50);
    Mix_PlayChannel(304, devilTriggerOut, 0);
}

export void playDevilTriggerLoop() {
    
    Mix_Volume(305, 30);
    Mix_PlayChannel(305, devilTriggerLoop, -1);
}

export void stopDevilTriggerLoop() {
    
    Mix_HaltChannel(305);
}

export void playDoppelgangerIn() {
    Mix_Volume(306, 50);
    Mix_PlayChannel(306, doppelgangerIn, 0);
}

export void playDoppelgangerOut() {
    Mix_Volume(307, 50);
    Mix_PlayChannel(307, doppelgangerOut, 0);
}

export void playQuicksilverIn() {
    
    Mix_Volume(308, 50);
    Mix_PlayChannel(308, quicksilverIn, 0);

}
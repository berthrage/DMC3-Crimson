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

void PlayChangeWeapon(int initialChannel, int finalChannel, Mix_Chunk* sfx, int volume) {

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

export void playChangeDevilArm() {
    PlayChangeWeapon(0, 19, changeDevilArm, activeConfig.SFX.changeWeaponVolume);
}

export void playChangeGun() {
    PlayChangeWeapon(20, 39, changeGun, activeConfig.SFX.changeWeaponVolume);
}
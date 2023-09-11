module;

#define SDL_MAIN_HANDLED
#include "../ThirdParty/SDL2/SDL.h"
#include "../ThirdParty/Simple-SDL2/audio.c"
#include "../ThirdParty/Simple-SDL2/audio.h"
#include <string>
export module ExtraSound;

import Config;
import Vars;
import Core;

export std::string SDL2Initialization = "";
export int SSDL2AudioInitialization = 0;
export bool SDL2Init = false;
export bool cacheAudioFiles = false;
export Audio * changeGun;
export Audio * changeDevilArm;



export void initSDL() {
    if(!SDL2Init) {
		if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_HAPTIC | SDL_INIT_GAMECONTROLLER) == -1) {
        SDL2Initialization = "SDL2 Error";
		}
		else {
			SDL2Initialization = "SDL2 Success";
		}
		
        SSDL2AudioInitialization = initAudio();
	
        SDL2Init = true;
	}

	if(!cacheAudioFiles) {

		changeGun = createAudio("sound/changegun.wav", 0, 128);
		changeDevilArm = createAudio("sound/changedevilarm.wav", 0, 128);

		cacheAudioFiles = true;
	}
}

export void playChangeDevilArm() {
    playSoundFromMemory(changeDevilArm, 128);
}
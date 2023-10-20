#include <SDL2/SDL.h>
#include "audio.h"

int main(void)
{
    /* Initialize only SDL Audio on default device */
    if(SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        return 1; 
    }

    /* Init Simple-SDL2-Audio */
    initAudio();

    /* Play music and a sound */
    playMusic("music/highlands.wav", SDL_MIX_MAXVOLUME);
    playSound("sounds/door1.wav", SDL_MIX_MAXVOLUME / 2);

    /* While using delay for showcase, don't actually do this in your project */
    SDL_Delay(1000);

    /* Override music, play another sound */
    playMusic("music/road.wav", SDL_MIX_MAXVOLUME);
    SDL_Delay(1000);

    /* Pause audio test */
    pauseAudio();
    SDL_Delay(1000);
    unpauseAudio();

    playSound("sounds/door2.wav", SDL_MIX_MAXVOLUME / 2);
    SDL_Delay(2000);

    /* Caching sound example, create, play from Memory, clear */

    Audio * sound = createAudio("sounds/door1.wav", 0, SDL_MIX_MAXVOLUME / 2);
    playSoundFromMemory(sound, SDL_MIX_MAXVOLUME);
    SDL_Delay(2000);

    Audio * music = createAudio("music/highlands.wav", 1, SDL_MIX_MAXVOLUME);
    playMusicFromMemory(music, SDL_MIX_MAXVOLUME);
    SDL_Delay(2000);

    /* End Simple-SDL2-Audio */
    endAudio();

    /* Important to free audio after ending Simple-SDL2-Audio because they might be referenced still */
    freeAudio(sound);
    freeAudio(music);

    SDL_Quit();

    return 0;
}

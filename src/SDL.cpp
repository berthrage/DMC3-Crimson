#include "SDL.hpp"

#pragma region SDL

HMODULE g_SDL2      = NULL;
HMODULE g_SDL2Mixer = NULL;

bool LoadSDLDLL() {
    g_SDL2 = LoadLibrary("SDL2.dll");

    return g_SDL2 == NULL ? false : true;
}

bool LoadSDLMixerDLL() {
    g_SDL2Mixer = LoadLibrary("SDL2_mixer.dll");

    return g_SDL2Mixer == NULL ? false : true;
}

#pragma endregion
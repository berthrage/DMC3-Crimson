#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "../ThirdParty/SDL2/SDL.h"
#include "../ThirdParty/SDL2/SDL_mixer.h"

#pragma region SDL

extern HMODULE g_SDL2;
extern HMODULE g_SDL2Mixer;

bool LoadSDLDLL();
bool LoadSDLMixerDLL();

template <typename F> F GetSDLFunction(const char* name) {
    if (g_SDL2 == NULL) {
        if (!LoadSDLDLL()) {
            return static_cast<F>(NULL);
        }
    }

    return reinterpret_cast<F>(GetProcAddress(g_SDL2, name));
}

template <typename F> F GetSDLMixerFunction(const char* name) {
    if (g_SDL2Mixer == NULL) {
        if (!LoadSDLMixerDLL()) {
            return static_cast<F>(NULL);
        }
    }

    return reinterpret_cast<F>(GetProcAddress(g_SDL2Mixer, name));
}

#pragma endregion
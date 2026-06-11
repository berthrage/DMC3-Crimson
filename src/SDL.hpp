#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
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

#pragma region SDL

extern HMODULE g_SDL3;
extern HMODULE g_SDL3Mixer;
extern bool    g_SDL3LoadAttempted;
extern bool    g_SDL3MixerLoadAttempted;

bool LoadSDLDLL();
bool LoadSDLMixerDLL();

template <typename F> F GetSDLFunction(const char* name) {
    if (g_SDL3 == NULL) {
        if (!LoadSDLDLL()) {
            return static_cast<F>(NULL);
        }
    }

    F func = reinterpret_cast<F>(GetProcAddress(g_SDL3, name));
    if (!func) {
        DWORD err = GetLastError();
        OutputDebugStringA((std::string("GetProcAddress(SDL3, ") + name + ") failed: code " + std::to_string(err) + "\n").c_str());
    }
    return func;
}

template <typename F> F GetSDLMixerFunction(const char* name) {
    if (g_SDL3Mixer == NULL) {
        if (!LoadSDLMixerDLL()) {
            return static_cast<F>(NULL);
        }
    }

    F func = reinterpret_cast<F>(GetProcAddress(g_SDL3Mixer, name));
    if (!func) {
        DWORD err = GetLastError();
        OutputDebugStringA((std::string("GetProcAddress(SDL2_mixer, ") + name + ") failed: code " + std::to_string(err) + "\n").c_str());
    }
    return func;
}

#pragma endregion
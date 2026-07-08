#pragma once

// SDL3 + SDL2_mixer headers and their compat shims are now provided by the
// precompiled header (pch.hpp). This file only declares the Crimson-specific
// SDL runtime-loading layer.

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
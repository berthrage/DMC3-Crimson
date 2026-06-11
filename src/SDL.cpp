#include "SDL.hpp"
#include "Core/Core.hpp"

#pragma region SDL

HMODULE g_SDL3      = NULL;
HMODULE g_SDL3Mixer = NULL;
bool    g_SDL3LoadAttempted      = false;
bool    g_SDL3MixerLoadAttempted = false;

bool LoadSDLDLL() {
    if (g_SDL3LoadAttempted) return g_SDL3 != NULL;
    g_SDL3LoadAttempted = true;
    g_SDL3 = LoadLibrary("SDL3.dll");
    if (!g_SDL3) {
        DWORD err = GetLastError();
        LPSTR msgBuf = nullptr;
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       nullptr, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&msgBuf, 0, nullptr);
        Log("LoadLibrary(\"SDL3.dll\") failed: %s (code %lu)", msgBuf ? msgBuf : "Unknown error", err);
        if (msgBuf) LocalFree(msgBuf);
    }
    return g_SDL3 != NULL;
}

bool LoadSDLMixerDLL() {
    if (g_SDL3MixerLoadAttempted) return g_SDL3Mixer != NULL;
    g_SDL3MixerLoadAttempted = true;
    g_SDL3Mixer = LoadLibrary("SDL2_mixer.dll");
    if (!g_SDL3Mixer) {
        DWORD err = GetLastError();
        LPSTR msgBuf = nullptr;
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       nullptr, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&msgBuf, 0, nullptr);
        Log("LoadLibrary(\"SDL2_mixer.dll\") failed: %s (code %lu)", msgBuf ? msgBuf : "Unknown error", err);
        if (msgBuf) LocalFree(msgBuf);
    }
    return g_SDL3Mixer != NULL;
}

#pragma endregion
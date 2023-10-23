module;

#include <windows.h>

#include "../ThirdParty/SDL2/SDL.h"
#include "../ThirdParty/SDL2/SDL_mixer.h"

export module SDL;

#pragma region SDL

HMODULE g_SDL2 = NULL;
HMODULE g_SDL2Mixer = NULL;

bool LoadSDLDLL() {
	g_SDL2 = LoadLibrary("SDL2.dll");

	return g_SDL2 == NULL ? false : true;
}

bool LoadSDLMixerDLL() {
	g_SDL2Mixer = LoadLibrary("SDL2_mixer.dll");

	return g_SDL2Mixer == NULL ? false : true;
}

export template<typename F>
F GetSDLFunction(const char* name)
{
	if (g_SDL2 == NULL) {
		if (!LoadSDLDLL()) {
			return static_cast<F>(NULL);
		}
	}

	return reinterpret_cast<F>(GetProcAddress(g_SDL2, name));
}

export template<typename F>
F GetSDLMixerFunction(const char* name)
{
	if (g_SDL2Mixer == NULL) {
		if (!LoadSDLMixerDLL()) {
			return static_cast<F>(NULL);
		}
	}

	return reinterpret_cast<F>(GetProcAddress(g_SDL2Mixer, name));
}
#pragma endregion
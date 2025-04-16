// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "Core/Core.hpp"
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tlhelp32.h>

#include "Core/Macros.h"


#include <stdio.h>
#include <string.h>

#include "Core/DebugSwitch.hpp"


namespace DI8 {

typedef decltype(DirectInput8Create)* DirectInput8Create_t;

};


namespace Base::DI8 {

::DI8::DirectInput8Create_t DirectInput8Create = 0;

};


namespace Hook::DI8 {

HRESULT DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, const IID& riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter) {
#pragma comment(linker, "/EXPORT:DirectInput8Create=" DECORATED_FUNCTION_NAME)


    return ::Base::DI8::DirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);
}

}; // namespace Hook::DI8


void Init() {
    LogFunction();


    byte32 error = 0;

    const char* libName = "dinput8.dll";

    char directory[128];
    char location[512];


    GetSystemDirectoryA(directory, sizeof(directory));

    snprintf(location, sizeof(location), "%s\\%s", directory, libName);


    SetLastError(0);

    auto lib = LoadLibraryA(location);
    if (!lib) {
        error = GetLastError();

        Log("LoadLibraryA failed. %s %X", location, error);

        return;
    }

    // DirectInput8Create
    {
        const char* funcName = "DirectInput8Create";

        SetLastError(0);

        auto funcAddr = GetProcAddress(lib, funcName);
        if (!funcAddr) {
            error = GetLastError();

            Log("GetProcAddress failed. %s %X", funcName, error);

            return;
        }

        ::Base::DI8::DirectInput8Create = reinterpret_cast<::DI8::DirectInput8Create_t>(funcAddr);

        Log("DirectInput8Create %X", DirectInput8Create);
    }
}

void Load() {
    LogFunction();

	// Early memory limit patch
	uintptr_t base = reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr));
    // ENEMIES
// 	uintptr_t target = base + 0x2C6030; 
// 
// 	BYTE patch[] = { 0xBA, 0x00, 0x00, 0x00, 0x0A }; // 160MB
// 	DWORD oldProtect;
// 	if (VirtualProtect(reinterpret_cast<void*>(target), sizeof(patch), PAGE_EXECUTE_READWRITE, &oldProtect)) {
// 		memcpy(reinterpret_cast<void*>(target), patch, sizeof(patch));
// 		VirtualProtect(reinterpret_cast<void*>(target), sizeof(patch), oldProtect, &oldProtect);
// 		Log("Patched memory limit at %p", reinterpret_cast<void*>(target));
// 	}

    // not it
// 	uintptr_t target = base + 0x2C6064;
// 	BYTE patch[] = { 0xBA, 0x00, 0x00, 0x00, 0x0A }; // 160MB
// 	DWORD oldProtect;
// 	if (VirtualProtect(reinterpret_cast<void*>(target), sizeof(patch), PAGE_EXECUTE_READWRITE, &oldProtect)) {
// 		memcpy(reinterpret_cast<void*>(target), patch, sizeof(patch));
// 		VirtualProtect(reinterpret_cast<void*>(target), sizeof(patch), oldProtect, &oldProtect);
// 		Log("Patched memory limit at %p", reinterpret_cast<void*>(target));
// 	}

    // not it
// 	uintptr_t target = base + 0x2C6098;
// 	BYTE patch[] = { 0xBA, 0x00, 0x00, 0x00, 0x0A }; // 160MB
// 	DWORD oldProtect;
// 	if (VirtualProtect(reinterpret_cast<void*>(target), sizeof(patch), PAGE_EXECUTE_READWRITE, &oldProtect)) {
// 		memcpy(reinterpret_cast<void*>(target), patch, sizeof(patch));
// 		VirtualProtect(reinterpret_cast<void*>(target), sizeof(patch), oldProtect, &oldProtect);
// 		Log("Patched memory limit at %p", reinterpret_cast<void*>(target));
// 	}

    // not it
// 	uintptr_t target = base + 0x2C604C;
// 	BYTE patch[] = { 0x41, 0xB9, 0x00, 0x00, 0x0A }; // 160MB
// 	DWORD oldProtect;
// 	if (VirtualProtect(reinterpret_cast<void*>(target), sizeof(patch), PAGE_EXECUTE_READWRITE, &oldProtect)) {
// 		memcpy(reinterpret_cast<void*>(target), patch, sizeof(patch));
// 		VirtualProtect(reinterpret_cast<void*>(target), sizeof(patch), oldProtect, &oldProtect);
// 		Log("Patched memory limit at %p", reinterpret_cast<void*>(target));
// 	}

	uintptr_t target = base + 0x2C6080;
	BYTE patch[] = { 0x41, 0xB9, 0x00, 0x00, 0x0A }; // 160MB
	DWORD oldProtect;
	if (VirtualProtect(reinterpret_cast<void*>(target), sizeof(patch), PAGE_EXECUTE_READWRITE, &oldProtect)) {
		memcpy(reinterpret_cast<void*>(target), patch, sizeof(patch));
		VirtualProtect(reinterpret_cast<void*>(target), sizeof(patch), oldProtect, &oldProtect);
		Log("Patched memory limit at %p", reinterpret_cast<void*>(target));
	}

    byte32 error = 0;

    MODULEENTRY32 me32 = {};
    me32.dwSize        = sizeof(MODULEENTRY32);

    auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);

    Module32First(snapshot, &me32);


    const char* names[][4] = {{
                                  "dmc1.exe",
                                  "Eva.dll",
                              },
        {
            "dmc2.exe",
            "Lucia.dll",
        },
        {"dmc3.exe", "Crimson.dll", "SDL2.dll", "SDL2_mixer.dll"},
        {
            "dmc4.exe",
            "Kyrie.dll",
        },
        {
            "DevilMayCry4SpecialEdition.exe",
            "Kyrie.dll",
        }};


    for_all(index, countof(names)) {
        auto appName = names[index][0];
        auto libName = names[index][1];

        if (strncmp(me32.szModule, appName,
                sizeof(appName) // @Research: Pretty sure that should be strlen. sizeof includes the terminating zero, strlen doesn't.
                ) == 0) {
            Log("%s %s", appName, libName);


            SetLastError(0);

            auto lib = LoadLibraryA(libName);
            if (!lib) {
                error = GetLastError();

                Log("LoadLibraryA failed. %s %X", libName, error);
            }


            return;
        }
    }


    Log("No Match");
}


byte32 DllMain(HINSTANCE instance, byte32 reason, void* reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        InitLog("logs", "dinput8.txt");

        Log("Session started.");

        Init();
        Load();
    }

    return 1;
}

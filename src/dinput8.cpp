// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "Core/Core.hpp"
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tlhelp32.h>

#include "Core/Macros.h"


#include <stdio.h>
#include <string.h>

#include "Core/DebugSwitch.hpp"

#include <utility>
#include <vector>
#include <stdlib.h>
#include <cstring>
#include "CrashHandler.hpp"

namespace DI8 {

typedef decltype(DirectInput8Create)* DirectInput8Create_t;

};

static std::vector<std::pair<ptrdiff_t, std::vector<uint8_t>>> s_CrimsonOffsets = {
    { 0x1E0BB2 , { 0xF3, 0x0F, 0x58, 0x89, 0xB8, 0x3E, 0x00, 0x00 } },
    { 0x1E0B8E , { 0x80, 0xB9, 0x9B, 0x3E, 0x00, 0x00, 0x01 } },
    { 0x215DE7 , { 0x44, 0x0F, 0x2F, 0xC1 } },
    { 0x215DEB , { 0x0F, 0x86, 0x95, 0x01, 0x00, 0x00 } },
    { 0x215DF1 , { 0x8B, 0x8F, 0x38, 0x63, 0x00, 0x00 } },
    { 0x215E3E , { 0x44, 0x0F, 0x2F, 0xC0 } },
    { 0x215EAA , { 0x74, 0x20 } },
    { 0x2BB194 , { 0x0F, 0x85, 0x18, 0x02, 0x00, 0x00 } },
    { 0x1EC467 , { 0x0F, 0x8E, 0x1E, 0x01, 0x00, 0x00 } },
    { 0x1FC5D5 , { 0xE8, 0xD6, 0x13, 0x13, 0x00 } },
    { 0x1FC5DA , { 0x44, 0x0F, 0xB7, 0x0F } },
    { 0x1FC5DE , { 0x44, 0x0F, 0xB7, 0xD0 } },
    { 0x1FA509 , { 0x66, 0x41, 0x03, 0xCB } },
    { 0x1FA50D , { 0x66, 0x41, 0x89, 0x88, 0xC0, 0x00, 0x00, 0x00 } },
    { 0x1EBF42 , { 0x66, 0x89, 0x83, 0x0C, 0x75, 0x00, 0x00 } },
    { 0x20BCF8 , { 0xC6, 0x83, 0x10, 0x3E, 0x00, 0x00, 0x01 } },
    { 0x20BD57 , { 0xC7, 0x83, 0x84, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
    { 0x881F1  , { 0xF3, 0x44, 0x0F, 0x10, 0x4A, 0x0C } },
    { 0x10B7B6 , { 0xF3, 0x0F, 0x10, 0x7A, 0x0C } },
    { 0x27E850 , { 0x66, 0x89, 0x86, 0x3C, 0x69, 0x00, 0x00 } },
    { 0x27E85C , { 0x66, 0x89, 0x9E, 0x3C, 0x69, 0x00, 0x00 } },
    { 0x27E874 , { 0x66, 0x89, 0x86, 0x3C, 0x69, 0x00, 0x00 } },
    { 0x27E830 , { 0x0F, 0xB7, 0x86, 0x3C, 0x69, 0x00, 0x00 } },
};

namespace Base::DI8 {

::DI8::DirectInput8Create_t DirectInput8Create = 0;

};


namespace Hook::DI8 {

HRESULT DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, const IID& riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter) {
#pragma comment(linker, "/EXPORT:DirectInput8Create=" DECORATED_FUNCTION_NAME)

    InitLog("logs", "Crash.txt");
    InstallCrashHandler(".\\logs\\crimson_crash.dmp");
    if (!InitializeDbgHelp()) {
        Log("Init_DbgHelp failed.");

        return 0;
    }

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

    uintptr_t base = (uintptr_t)GetModuleHandleA(NULL);
    for (const auto& entry : s_CrimsonOffsets) {
        uintptr_t address = base + entry.first;
        int result = memcmp((void*)address, entry.second.data(), entry.second.size());
        if (result != 0) {
            Log("Check dmc3.exe + %X FAILED", address);
            MessageBoxA(NULL, "Executable checksum is wrong. Startup cannot continue", "DMC3 Crimson", MB_ICONERROR);
            std::exit(1);
        }
    }
}

void Load() {
    LogFunction();

	// Early memory limit patch
// 	uintptr_t base = reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr));
//     // ENEMIES
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

    // not it
// 	uintptr_t target = base + 0x2C6080;
// 	BYTE patch[] = { 0x41, 0xB9, 0x00, 0x00, 0x0A }; // 160MB
// 	DWORD oldProtect;
// 	if (VirtualProtect(reinterpret_cast<void*>(target), sizeof(patch), PAGE_EXECUTE_READWRITE, &oldProtect)) {
// 		memcpy(reinterpret_cast<void*>(target), patch, sizeof(patch));
// 		VirtualProtect(reinterpret_cast<void*>(target), sizeof(patch), oldProtect, &oldProtect);
// 		Log("Patched memory limit at %p", reinterpret_cast<void*>(target));
// 	}

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
        MessageBoxA(NULL, "DEBUG", "DEBUG", MB_ICONINFORMATION);
        InitLog("logs", "dinput8.txt");

        Log("Session started.");

        Init();
        Load();
    }
    if (reason == DLL_PROCESS_DETACH) {
        UninstallCrashHandler();
    }

    return 1;
}

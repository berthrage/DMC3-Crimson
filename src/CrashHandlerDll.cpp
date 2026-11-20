// CrashHandler.dll. Kept out of the injector so the handler always matches the installed
// Crimson.dll, whatever dinput8.dll is sitting next to the game.
#include <Windows.h>
#include "Core/Core.hpp"
#include "CrashHandler.hpp"

extern "C" __declspec(dllexport) void __cdecl CrashHandlerInstall(const char* crashFilePath) {
    if (!crashFilePath) {
        return;
    }

    // Own log file: InitLog truncates it, so it must not point at Crimson.txt or at the Crash.txt
    // an older dinput8.dll still uses.
    InitLog("logs", "CrashHandler.txt");

    if (!InitializeDbgHelp()) {
        Log("CrashHandlerInstall: dbghelp unavailable\n");
    }

    InstallCrashHandler(crashFilePath);
}

BOOL APIENTRY DllMain(HMODULE instance, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_DETACH) {
        UninstallCrashHandler();
    }

    return TRUE;
}

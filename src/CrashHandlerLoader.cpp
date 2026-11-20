#include <Windows.h>
#include <string.h>

#include "CrashHandlerLoader.hpp"
#include "Core/Core.hpp"

namespace {

typedef void(__cdecl* CrashHandlerInstallFn)(const char*);

const char* CRASH_HANDLER_DLL_NAME = "CrashHandler.dll";

HMODULE g_crashHandlerDll = nullptr;

bool BuildCrashHandlerPath(char* path) {
    HMODULE self = nullptr;
    if (!GetModuleHandleExA(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCSTR>(&BuildCrashHandlerPath), &self)) {
        return false;
    }

    DWORD length = GetModuleFileNameA(self, path, MAX_PATH);
    if ((length == 0) || (length >= MAX_PATH)) {
        return false;
    }

    char* fileName = strrchr(path, '\\');
    if (!fileName) {
        return false;
    }

    strcpy_s(fileName + 1, MAX_PATH - (fileName + 1 - path), CRASH_HANDLER_DLL_NAME);

    return true;
}

} // namespace

bool InstallCrashHandlerFromDll(const char* crashFilePath) {
    if (g_crashHandlerDll) {
        return true;
    }

    char path[MAX_PATH] = {};
    if (!BuildCrashHandlerPath(path)) {
        Log("InstallCrashHandlerFromDll: module path unavailable");

        return false;
    }

    g_crashHandlerDll = LoadLibraryA(path);
    if (!g_crashHandlerDll) {
        Log("InstallCrashHandlerFromDll: LoadLibraryA failed. %s %X", path, GetLastError());

        // Fall back to the loader search path, which covers the game folder.
        g_crashHandlerDll = LoadLibraryA(CRASH_HANDLER_DLL_NAME);
        if (!g_crashHandlerDll) {
            Log("InstallCrashHandlerFromDll: LoadLibraryA failed. %s %X", CRASH_HANDLER_DLL_NAME, GetLastError());

            return false;
        }
    }

    auto install = reinterpret_cast<CrashHandlerInstallFn>(GetProcAddress(g_crashHandlerDll, "CrashHandlerInstall"));
    if (!install) {
        Log("InstallCrashHandlerFromDll: GetProcAddress failed. %s %X", path, GetLastError());

        FreeLibrary(g_crashHandlerDll);
        g_crashHandlerDll = nullptr;

        return false;
    }

    install(crashFilePath);

    return true;
}

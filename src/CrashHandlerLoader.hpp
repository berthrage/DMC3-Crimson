#pragma once

// Installs the crash handler from CrashHandler.dll. The dll is loaded from next to Crimson.dll,
// so an outdated dinput8.dll does not decide which handler runs.
bool InstallCrashHandlerFromDll(const char* crashFilePath);

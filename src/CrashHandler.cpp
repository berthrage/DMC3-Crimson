#pragma warning(disable : 4668)

#include <signal.h>
#include <eh.h>

#include <Windows.h>
#include <string>
#include <DbgHelp.h>
#include <tlhelp32.h>
#include <cstdio>
#include <processthreadsapi.h>
#include <process.h>
#include "Core/Core.hpp"

#define USE_THREAD 1

#pragma warning(push)
#pragma warning(disable : 6011)
inline void CrashMeDaddy() {
    char* p = nullptr;
    *p = 0; // NOLINT
}
#pragma warning(pop)

static char gCrashMemory[2 * 1024 * 1024] {0};
static size_t crashMemoryOffset { 0 };

char* gCrashFilePath = nullptr;
static HANDLE gDumpEvent = nullptr;
static HANDLE gDumpThread = nullptr;

//static HANDLE gDumpEvent = nullptr;
// not working in DLL_PROCESS_ATTACH :(
//static HANDLE gDumpThread = nullptr;
static bool gCrashed = false;

static MINIDUMP_EXCEPTION_INFORMATION gMei{};
static LPTOP_LEVEL_EXCEPTION_FILTER gPrevExceptionFilter = nullptr;

#define MSGBOX_TITLE "Crimson Crash Handler"

template <typename... Args> void Append(const char* format, Args... args) {
    int ret = snprintf(gCrashMemory + crashMemoryOffset, sizeof(gCrashMemory), format, args...);
    if (ret == -1) {
        MessageBoxA(NULL, "Bro... we crashed... in a crash handler :facepalm:", MSGBOX_TITLE, MB_ICONERROR);
        return;
    }
    crashMemoryOffset += ret;
}

static const char* ExceptionFromCode(UINT code) {
    switch (code) {
    case EXCEPTION_ACCESS_VIOLATION:
        return "The thread tried to read from or write to a virtual address for which it does not have the appropriate access.";
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        return "The thread tried to access an array element that is out of bounds and the underlying hardware supports bounds checking.";
    case EXCEPTION_BREAKPOINT:
        return "A breakpoint was encountered.";
    case EXCEPTION_DATATYPE_MISALIGNMENT:
        return "The thread tried to read or write data that is misaligned on hardware that does not provide alignment. For example, 16-bit "
            "values must be aligned on 2-byte boundaries; 32-bit values on 4-byte boundaries, and so on.";
    case EXCEPTION_FLT_DENORMAL_OPERAND:
        return "One of the operands in a floating-point operation is denormal. A denormal value is one that is too small to represent as a "
            "standard floating-point value.";
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        return "The thread tried to divide a floating-point value by a floating-point divisor of zero.";
    case EXCEPTION_FLT_INEXACT_RESULT:
        return "The result of a floating-point operation cannot be represented exactly as a decimal fraction.";
    case EXCEPTION_FLT_INVALID_OPERATION:
        return "This exception represents any floating-point exception not included in this list.";
    case EXCEPTION_FLT_OVERFLOW:
        return "The exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type.";
    case EXCEPTION_FLT_STACK_CHECK:
        return "The stack overflowed or underflowed as the result of a floating-point operation.";
    case EXCEPTION_FLT_UNDERFLOW:
        return "The exponent of a floating-point operation is less than the magnitude allowed by the corresponding type.";
    case EXCEPTION_ILLEGAL_INSTRUCTION:
        return "The thread tried to execute an invalid instruction.";
    case EXCEPTION_IN_PAGE_ERROR:
        return "The thread tried to access a page that was not present, and the system was unable to load the page. For example, this "
            "exception might occur if a network connection is lost while running a program over the network.";
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
        return "The thread tried to divide an integer value by an integer divisor of zero.";
    case EXCEPTION_INT_OVERFLOW:
        return "The result of an integer operation caused a carry out of the most significant bit of the result.";
    case EXCEPTION_INVALID_DISPOSITION:
        return "An exception handler returned an invalid disposition to the exception dispatcher. Programmers using a high-level language "
            "such as C should never encounter this exception.";
    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
        return "The thread tried to continue execution after a noncontinuable exception occurred.";
    case EXCEPTION_PRIV_INSTRUCTION:
        return "The thread tried to execute an instruction whose operation is not allowed in the current machine mode.";
    case EXCEPTION_SINGLE_STEP:
        return "A trace trap or other single-instruction mechanism signaled that one instruction has been executed.";
    case EXCEPTION_STACK_OVERFLOW:
        return "The thread used up its stack.";
    default:
        return "Unknown exception";
    }
}

void GetExceptionInfo(EXCEPTION_POINTERS* excPointers) {
    if (!excPointers) {
        return;
    }
    Append("\n-------- Exception information start ----------\n\n");

    EXCEPTION_RECORD* excRecord = excPointers->ExceptionRecord;
    DWORD excCode = excRecord->ExceptionCode;
    Append("Exception: %08X %s\n", (int)excCode, ExceptionFromCode(excCode));

    Append("Faulting IP: %08X", (DWORD64)excRecord->ExceptionAddress);
    if ((EXCEPTION_ACCESS_VIOLATION == excCode) || (EXCEPTION_IN_PAGE_ERROR == excCode)) {
        int readWriteFlag = (int)excRecord->ExceptionInformation[0];
        DWORD64 dataVirtAddr = (DWORD64)excRecord->ExceptionInformation[1];
        if (0 == readWriteFlag) {
            Append("Fault reading address %08X", dataVirtAddr);
        } else if (1 == readWriteFlag) {
            Append("Fault writing address %08X", dataVirtAddr);
        } else if (8 == readWriteFlag) {
            Append("DEP violation at address ", dataVirtAddr);
        } else {
            Append("unknown readWriteFlag: %d", readWriteFlag);
        }
        Append("\n");
    }

    PCONTEXT ctx = excPointers->ContextRecord;
    Append("\nRegisters:\n");

    Append(
        "RAX:%016I64X  RBX:%016I64X  RCX:%016I64X\nRDX:%016I64X  RSI:%016I64X  RDI:%016I64X\n"
        "R8: %016I64X\nR9: "
        "%016I64X\nR10:%016I64X\nR11:%016I64X\nR12:%016I64X\nR13:%016I64X\nR14:%016I64X\nR15:%016I64X\n",
        ctx->Rax, ctx->Rbx, ctx->Rcx, ctx->Rdx, ctx->Rsi, ctx->Rdi, ctx->R8, ctx->R9, ctx->R10, ctx->R11, ctx->R12,
        ctx->R13, ctx->R14, ctx->R15);
    Append("CS:RIP:%04X:%016I64X\n", ctx->SegCs, ctx->Rip);
    Append("SS:RSP:%04X:%016X  RBP:%08X\n", ctx->SegSs, (unsigned int)ctx->Rsp, (unsigned int)ctx->Rbp);
    Append("DS:%04X  ES:%04X  FS:%04X  GS:%04X\n", ctx->SegDs, ctx->SegEs, ctx->SegFs, ctx->SegGs);
    Append("Flags:%08X\n", ctx->EFlags);

    Append("\nCrashed thread: %08X\n", GetCurrentThread());

    Append("\n-------- Exception information end ----------\n\n");
}

// returns true if running on wine (winex11.drv is present)
// it's not a logical, but convenient place to do it
static bool GetModules() {
    bool isWine = false;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
    if (snap == INVALID_HANDLE_VALUE) {
        return true;
    }

    MODULEENTRY32 mod;
    mod.dwSize = sizeof(mod);
    BOOL cont = Module32First(snap, &mod);
    while (cont) {
        auto nameA = mod.szModule;
        isWine = std::strncmp(nameA, "winex11.drv", sizeof(mod.szModule)) == 0;
        auto pathA = mod.szExePath;
        Append("Module: %p %06X %-16s %s\n", mod.modBaseAddr, mod.modBaseSize, nameA, pathA);
        cont = Module32Next(snap, &mod);
    }
    CloseHandle(snap);

    return isWine;
}

static void BuildCrashInfoText() {

    if (gMei.ExceptionPointers) {
        // those are only set when we capture exception
        GetExceptionInfo(gMei.ExceptionPointers);
        Append("\n");
    }

    Append("\n-------- Modules during crash start ----------\n\n");
    GetModules();
    Append("\n-------- Modules after crash end    ----------\n\n");

}

void ShowCrashHandlerMessage() {
    Log("ShowCrashHandlerMessage()\n");

    const char* msg = "We're sorry, DMC3 Crimson has crashed.\n\nPress 'Ok' to open log folder\nSend crimson_crash.dmp and Crash.txt files to the developers and describe what you were doing!";
    UINT flags = MB_ICONERROR | MB_OK | MB_OKCANCEL | MB_SETFOREGROUND | MB_TOPMOST;

    int res = MessageBoxA(NULL, msg, MSGBOX_TITLE, flags);
    if (IDOK != res) {
        return;
    }
    if (!gCrashFilePath) {
        Log("ShowCrashHandlerMessage: !gCrashFilePath\n");
        return;
    }
    ShellExecuteA(NULL, "open", "explorer.exe", ".\\logs\\", NULL, SW_SHOWNORMAL);
}

decltype(&MiniDumpWriteDump) minidump_write_dump = nullptr;

bool InitializeDbgHelp() {

    auto dbghelp = LoadLibrary("dbghelp.dll");
    if(!dbghelp) {
        return false;
    }

    minidump_write_dump = (decltype(MiniDumpWriteDump)*)GetProcAddress(dbghelp, "MiniDumpWriteDump");
    if (!minidump_write_dump) {
        return false;
    }

    return true;
}

static BOOL CALLBACK OpenMiniDumpCallback(void*, PMINIDUMP_CALLBACK_INPUT input, PMINIDUMP_CALLBACK_OUTPUT output) {
    if (!input || !output) {
        return FALSE;
    }

    switch (input->CallbackType) {
    case ModuleCallback:
        if (!(output->ModuleWriteFlags & ModuleReferencedByMemory)) {
            output->ModuleWriteFlags &= ~ModuleWriteModule;
        }
        return TRUE;
    case IncludeModuleCallback:
    case IncludeThreadCallback:
    case ThreadCallback:
    case ThreadExCallback:
        return TRUE;
    default:
        return FALSE;
    }
}

#if USE_THREAD
static DWORD WINAPI CrashDumpThread(LPVOID param) {
    WaitForSingleObject(gDumpEvent, INFINITE);
#else
static DWORD WINAPI CrashDumpProc() {
#endif
    if (!gCrashed) {
        return 0;
    }

    BuildCrashInfoText();

    Log("CrashDumpThread\n");

    DWORD n = GetEnvironmentVariableA("CRIMSON_FULLDUMP", nullptr, 0);
    bool fullDump = (0 != n);

    HANDLE hFile = CreateFile(gCrashFilePath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, nullptr);
    if (INVALID_HANDLE_VALUE == hFile) {
        return 0;
    }

    MINIDUMP_TYPE type = (MINIDUMP_TYPE)(MiniDumpNormal | MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory);
    type = (MINIDUMP_TYPE)(type | MiniDumpWithDataSegs | MiniDumpWithHandleData);
    if (fullDump) {
        type =
            (MINIDUMP_TYPE)(type | MiniDumpWithPrivateReadWriteMemory);
    }
    MINIDUMP_CALLBACK_INFORMATION mci = {OpenMiniDumpCallback, nullptr};

    minidump_write_dump(GetCurrentProcess(), GetCurrentProcessId(), hFile, type, &gMei, nullptr, &mci);

    CloseHandle(hFile);
    // Log function cuts off symbolls if string exceeds 2048 chars... sigh
    size_t length = ::strlen(gCrashMemory);
    for (int i = 0; i < length; i += 2048) {
        Log<false>(gCrashMemory + i);
    }
    return 0;
}

static LONG WINAPI CrashDumpVectoredExceptionHandler(EXCEPTION_POINTERS* exceptionInfo) {
#if 1
    if (exceptionInfo->ExceptionRecord->ExceptionCode != STATUS_HEAP_CORRUPTION) {
        return EXCEPTION_CONTINUE_SEARCH;
    }
#endif

    //    gReducedLogging = true;
    Log("CrashDumpVectoredExceptionHandler\n");

    static bool wasHere = false;
    if (wasHere) {
        Log("CrashDumpVectoredExceptionHandler: wasHere set\n");
        return EXCEPTION_CONTINUE_SEARCH; // Note: or should TerminateProcess()?
    }

    wasHere = true;
    gCrashed = true;

    gMei.ThreadId = GetCurrentThreadId();
    gMei.ExceptionPointers = exceptionInfo;
    // DOES NOT WORK IN DLL_PROCESS_ATTACH
#if USE_THREAD
    // per msdn (which is backed by my experience), MiniDumpWriteDump() doesn't
    // write callstack for the calling thread correctly. We use msdn-recommended
    // work-around of spinning a thread to do the writing
    SetEvent(gDumpEvent);
    WaitForSingleObject(gDumpThread, INFINITE);
#else
    CrashDumpProc();
#endif

    ShowCrashHandlerMessage();
    TerminateProcess(GetCurrentProcess(), 1);

    return EXCEPTION_CONTINUE_SEARCH;
}

static LONG WINAPI CrashDumpExceptionHandler(EXCEPTION_POINTERS* exceptionInfo) {
    if (!exceptionInfo || (EXCEPTION_BREAKPOINT == exceptionInfo->ExceptionRecord->ExceptionCode)) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    Log("CrashDumpExceptionHandler\n");

    static bool wasHere = false;
    if (wasHere) {
        Log("CrashDumpExceptionHandler: wasHere set\n");
        return EXCEPTION_CONTINUE_SEARCH; // Note: or should TerminateProcess()?
    }

    wasHere = true;
    gCrashed = true;

    gMei.ThreadId = GetCurrentThreadId();
    gMei.ExceptionPointers = exceptionInfo;
#if USE_THREAD
    // per msdn (which is backed by my experience), MiniDumpWriteDump() doesn't
    // write callstack for the calling thread correctly. We use msdn-recommended
    // work-around of spinning a thread to do the writing

    SetEvent(gDumpEvent);
    WaitForSingleObject(gDumpThread, INFINITE);
#else
    CrashDumpProc();
#endif

    ShowCrashHandlerMessage();
    TerminateProcess(GetCurrentProcess(), 1);

    return EXCEPTION_CONTINUE_SEARCH;
}


static void GetOsVersion() {
    OSVERSIONINFOEX ver{};
    ZeroMemory(&ver, sizeof(ver));
    ver.dwOSVersionInfoSize = sizeof(ver);
    BOOL ok = GetVersionEx((OSVERSIONINFO*)&ver);
    if (!ok) {
        return;
    }

    const char* os_ver = "unk";
    if (VER_PLATFORM_WIN32_NT != ver.dwPlatformId) {
        os_ver = "9x";
    }
    if (ver.dwMajorVersion == 6 && ver.dwMinorVersion == 3) {
        os_ver = "8.1"; // or Server 2012 R2
    }
    if (ver.dwMajorVersion == 6 && ver.dwMinorVersion == 2) {
        os_ver = "8"; // or Server 2012
    }
    if (ver.dwMajorVersion == 6 && ver.dwMinorVersion == 1) {
        os_ver = "7"; // or Server 2008 R2
    }
    if (ver.dwMajorVersion == 6 && ver.dwMinorVersion == 0) {
        os_ver = "Vista"; // or Server 2008
    }
    if (ver.dwMajorVersion == 5 && ver.dwMinorVersion == 2) {
        os_ver = "Server 2003";
    }
    if (ver.dwMajorVersion == 5 && ver.dwMinorVersion == 1) {
        os_ver = "XP";
    }
    if (ver.dwMajorVersion == 5 && ver.dwMinorVersion == 0) {
        os_ver = "2000";
    }
    if (ver.dwMajorVersion == 10) {
        os_ver = "10";
    }
    int servicePackMajor = ver.wServicePackMajor;
    int servicePackMinor = ver.wServicePackMinor;
    int buildNumber = ver.dwBuildNumber & 0xFFFF;
    if (0 == servicePackMajor) {
        Append("OS: Windows %s build %d\n", os_ver, buildNumber);
    } else if (0 == servicePackMinor) {
        Append("OS: Windows %s SP%d build %d\n", os_ver, servicePackMajor, buildNumber);
    } else {
        Append("OS: Windows %s %d.%d build %d\n", os_ver, servicePackMajor, servicePackMinor, buildNumber);
    }
}

static void GetProcessorName() {
    HKEY hKey;
    const char* keyPath = "HARDWARE\\DESCRIPTION\\System\\CentralProcessor";
    const char* valueName = "ProcessorNameString";

    // Try the base key first
    LONG result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, keyPath, 0, KEY_READ, &hKey);

    if (result != ERROR_SUCCESS) {
        // If that fails, try with "\\0" suffix
        keyPath = "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0";
        result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, keyPath, 0, KEY_READ, &hKey);
    }

    if (result == ERROR_SUCCESS) {
        DWORD type = 0;
        DWORD size = 0;

        // First call to get required buffer size
        result = RegQueryValueExA(hKey, valueName, nullptr, &type, nullptr, &size);
        if (result == ERROR_SUCCESS && type == REG_SZ) {
            char buffer[MAX_PATH] = {0};
            result = RegQueryValueExA(hKey, valueName, nullptr, nullptr,
                (LPBYTE)buffer, &size);
            if (result == ERROR_SUCCESS) {
                Append("Processor: %s\n", buffer);
            }
        }

        RegCloseKey(hKey);
    }
}

#define GFX_DRIVER_KEY_FMT "SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}\\%04d"

static void GetGraphicsDriverInfo() {
    char buffer[512] = {0};
    size_t bufferSize = 512;
    const char* baseKey = "SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}";
    const DWORD MAX_INSTANCE = 16; // Arbitrary limit on number of GPU drivers to read
    const DWORD VALUE_BUF_SIZE = 256;

    for (DWORD i = 0; i < MAX_INSTANCE; ++i) {
        char keyPath[256] = {0};
        _snprintf(keyPath, sizeof(keyPath), "%s\\%04lu", baseKey, i);

        HKEY hKey{};
        LONG result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, keyPath, 0, KEY_READ, &hKey);
        if (result != ERROR_SUCCESS) {
            break; // No more devices
        }

        char valueBuf[VALUE_BUF_SIZE] = {0};

        // Read DriverDesc
        DWORD valueType;
        DWORD valueSize = sizeof(valueBuf);
        result = RegQueryValueExA(hKey, "DriverDesc", nullptr, &valueType, reinterpret_cast<LPBYTE>(valueBuf), &valueSize);
        if (result == ERROR_SUCCESS && valueType == REG_SZ) {
            _snprintf(buffer + strlen(buffer), bufferSize - strlen(buffer),
                "Graphics driver %lu\n  DriverDesc:         %s\n", i, valueBuf);
        }

        // Read DriverVersion
        valueSize = sizeof(valueBuf);
        result = RegQueryValueExA(hKey, "DriverVersion", nullptr, &valueType, reinterpret_cast<LPBYTE>(valueBuf), &valueSize);
        if (result == ERROR_SUCCESS && valueType == REG_SZ) {
            _snprintf(buffer + strlen(buffer), bufferSize - strlen(buffer),
                "  DriverVersion:      %s\n", valueBuf);
        }

        // Read UserModeDriverName
        valueSize = sizeof(valueBuf);
        result = RegQueryValueExA(hKey, "UserModeDriverName", nullptr, &valueType, reinterpret_cast<LPBYTE>(valueBuf), &valueSize);
        if (result == ERROR_SUCCESS && valueType == REG_MULTI_SZ) {
            // Just print first string in MULTI_SZ
            if (valueBuf[0] != '\0') {
                snprintf(buffer + strlen(buffer), bufferSize - strlen(buffer),
                    "  UserModeDriverName: %s\n", valueBuf);
            }
        }

        RegCloseKey(hKey);
        Append(buffer);
    }
}

static void GetSystemInfo() {
    Append("-------- System info start ----------");
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    Append("Number Of Processors: %d\n", si.dwNumberOfProcessors);
    GetProcessorName();

    {
        MEMORYSTATUSEX ms;
        ms.dwLength = sizeof(ms);
        GlobalMemoryStatusEx(&ms);

        float physMemGB = (float)ms.ullTotalPhys / (float)(1024 * 1024 * 1024);
        float totalPageGB = (float)ms.ullTotalPageFile / (float)(1024 * 1024 * 1024);
        DWORD usedPerc = ms.dwMemoryLoad;
        Append("Physical Memory: %.2f GB\nCommit Charge Limit: %.2f GB\nMemory Used: %d%%\n", physMemGB,
            totalPageGB, usedPerc);
    }
    {
        // get language
        char country[32] = {}, lang[32]{};
        GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_SISO3166CTRYNAME, country, sizeof(country) - 1);
        GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_SISO639LANGNAME, lang, sizeof(lang) - 1);
        Append("Lang: %s %s\n", lang, country);
    }
    GetGraphicsDriverInfo();

    Append("-------- System info end ----------");
}

// returns true if running on wine
static bool BuildModulesInfo() {
    Append("-------- Modules during startup ----------");
    bool isWine = GetModules();
    Append("-------- Modules during startup ----------");
    return isWine;
}

static void BuildSystemInfo() {
    GetOsVersion();
    GetSystemInfo();
}

void __cdecl onSignalAbort(int) {
    // put the signal back because can be called many times
    // (from multiple threads) and raise() resets the handler
    signal(SIGABRT, onSignalAbort);
    CrashMeDaddy();
}

void onTerminate() {
    CrashMeDaddy();
}

void onUnexpected() {
    CrashMeDaddy();
}

// shadow crt's _purecall() so that we're called instead of CRT
int __cdecl _purecall() {
    CrashMeDaddy();
    return 0;
}

void InstallCrashHandler(const char* crashFilePath) {
    gCrashFilePath = (char*)crashFilePath;

    Log("InstallCrashHandler:\n  crashFilePath: '%s'\n", crashFilePath );

    // don't bother sending crash reports when running under Wine
    // as they're not helpful
    bool isWine = BuildModulesInfo();
    if (isWine) {
        return;
    }

    BuildSystemInfo();

    // DOES NOT WORK IN DLL_PROCESS_ATTACH!
#if USE_THREAD
    gDumpEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!gDumpEvent) {
        Log("InstallCrashHandler: skipping because !gDumpEvent\n");
        return;
    }
    gDumpThread = CreateThread(nullptr, 0, CrashDumpThread, nullptr, 0, nullptr);
    if (!gDumpThread) {
        Log("InstallCrashHandler: skipping because !gDumpThread\n");
        return;
    }
#endif
    gPrevExceptionFilter = SetUnhandledExceptionFilter(CrashDumpExceptionHandler);
    // 1 means that our handler will be called first, 0 would be: last
    AddVectoredExceptionHandler(1, CrashDumpVectoredExceptionHandler);

    signal(SIGABRT, onSignalAbort);
    ::set_terminate(onTerminate);
    // TODO: breaks starting in 17.3. Requires _HAS_EXCEPTION
    // but it is disabled by _HAS_CXX17 because P0003R5
    // https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0003r5.html
    ::set_unexpected(onUnexpected);
}

void UninstallCrashHandler() {
#if USE_THREAD
    if (!gDumpEvent || !gDumpThread) {
        return;
    }
#endif

    if (gPrevExceptionFilter) {
        SetUnhandledExceptionFilter(gPrevExceptionFilter);
    }

#if USE_THREAD
    SetEvent(gDumpEvent);
    WaitForSingleObject(gDumpThread, 1000); // 1 sec

    CloseHandle(gDumpThread);
    CloseHandle(gDumpEvent);
#endif
}

// Tests that various ways to crash will generate crash report.
// Commented-out because they are ad-hoc. Left in code because
// I don't want to write them again if I ever need to test crash reporting
#if 0
#include <signal.h>
static void TestCrashAbort()
{
    raise(SIGABRT);
}

struct Base;
void foo(Base* b);

struct Base {
    Base() {
        foo(this);
    }
    virtual ~Base() = 0;
    virtual void pure() = 0;
};
struct Derived : public Base {
    void pure() { }
};

void foo(Base* b) {
    b->pure();
}

static void TestCrashPureCall()
{
    Derived d; // should crash
}

// tests that making a big allocation with new raises an exception
static int TestBigNew()
{
    size_t size = 1024*1024*1024*1;  // 1 GB should be out of reach
    char *mem = (char*)1;
    while (mem) {
        mem = new char[size];
    }
    // just some code so that compiler doesn't optimize this code to null
    for (size_t i = 0; i < 1024; i++) {
        mem[i] = i & 0xff;
    }
    int res = 0;
    for (size_t i = 0; i < 1024; i++) {
        res += mem[i];
    }
    return res;
}
#endif

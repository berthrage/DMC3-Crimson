#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef DELETE

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DataTypes.hpp"

#undef CopyMemory

extern bool noLog;
void _nop(char* dst, unsigned int size);
void _patch(char* dst, char* src, int size);

extern char logLocation[64];

template <bool timestamp = true, typename... Args> void Log(const char* format, Args... args);

void LogFunctionHelper(const char* funcName);

#pragma region Utility

template <typename T1, typename T2> struct TypeMatch {
    static constexpr bool value = false;
};

template <typename T> struct TypeMatch<T, T> {
    static constexpr bool value = true;
};

template <new_size_t T> struct TypeValue {
    static constexpr auto value = T;
};

template <typename T, new_size_t count> constexpr auto countof(T (&)[count]) {
    return count;
}

// struct SizeData
// {
// 	uint32 width;
// 	uint32 height;
// 	vec2 size;

// 	void Update
// 	(
// 		uint32 newWidth,
// 		uint32 newHeight
// 	)
// 	{
// 		width = newWidth;
// 		height = newHeight;
// 		size =
// 		{
// 			static_cast<float>(width),
// 			static_cast<float>(height)
// 		};
// 	}
// };

struct TimeData {
    uint32 milliseconds;
    uint32 seconds;
    uint32 minutes;
    uint32 hours;

    TimeData(float frameCount, float frameRate) {
        constexpr uint32 oneSecond = 1000;
        constexpr uint32 oneMinute = (60 * oneSecond);
        constexpr uint32 oneHour   = (60 * oneMinute);

        auto time = static_cast<uint32>((frameCount / frameRate) * 1000.0f);

        hours = (time / oneHour);
        time -= (hours * oneHour);

        minutes = (time / oneMinute);
        time -= (minutes * oneMinute);

        seconds = (time / oneSecond);
        time -= (seconds * oneSecond);

        milliseconds = time;
    }
};

template <typename T, uint8 count> bool SignatureMatch(byte8* signature, T (&array)[count]) {
    uint8 signatureIndex = 0;

    while (signatureIndex < count) {
        if (signature[signatureIndex] != array[signatureIndex]) {
            break;
        }

        signatureIndex++;
    }

    return (signatureIndex == count);
}

template <uint32 count> constexpr uint32 ctstrlen(const char (&name)[count]) {
    return (count - 1);
}

// @Research: Consider reference.
template <typename T> T Reverse(T* varAddr) {
    constexpr uint8 size = static_cast<uint8>(sizeof(T));

    T var = 0;

    for (uint8 index = 0; index < size; index++) {
        reinterpret_cast<byte8*>(&var)[index] = reinterpret_cast<byte8*>(varAddr)[(size - 1 - index)];
    }

    return var;
}

template <typename varType, uint8 mapItemCount> void UpdateMapIndex(const varType (&map)[mapItemCount], uint8& index, varType& var) {
    // LogFunction();

    for (uint8 mapIndex = 0; mapIndex < mapItemCount; mapIndex++) {
        auto& mapItem = map[mapIndex];
        if (mapItem == var) {
            index = mapIndex;
            return;
        }
    }

    Log("No match.");
}

bool Match(const void* addr, const void* addr2, uint32 count);

#pragma endregion

#pragma region Memory
enum {
    RAX,
    RCX,
    RDX,
    RBX,
    RSP,
    RBP,
    RSI,
    RDI,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15,
    MAX_REGISTER,
};

extern byte8* appBaseAddr;
extern uint32 appSize;
extern HWND appWindow;

void SetMemory(void* addr, byte8 value, new_size_t size);

void CopyMemory(void* destination, const void* source, new_size_t size);

byte8* Alloc(new_size_t size, byte8* dest = 0);

byte8* AllocEx(new_size_t size, offset_t start, offset_t end);

byte8* LowAlloc(new_size_t size);

byte8* HighAlloc(new_size_t size);

bool Free(byte8* addr);

template <typename T> T Align(T& pos, T boundary, byte8* addr = 0, byte8 padValue = 0) {
    T remainder = (pos % boundary);

    if (remainder) {
        T size = (boundary - remainder);

        if (addr) {
            SetMemory((addr + pos), padValue, size);
        }

        pos += size;
    }

    return remainder;
}

#pragma endregion

#pragma region File

// @Research: bufferSize should be uint32.
enum {
    FileFlags_Read   = 1 << 0,
    FileFlags_Write  = 1 << 1,
    FileFlags_Append = 1 << 2,
};

HANDLE OpenFile(const char* location, byte32 flags);

bool CloseFile(HANDLE fileHandle);

new_size_t GetFileSize(HANDLE fileHandle);

bool LoadFile(HANDLE fileHandle, new_size_t size, void* dest,
    offset_t start // Default value unnecessarily complicates overload resolution.
);

byte8* LoadFile(HANDLE fileHandle, new_size_t size,
    offset_t start // Default value unnecessarily complicates overload resolution.
);

byte8* LoadFile(const char* location);

bool SaveFile(HANDLE fileHandle, const void* addr, new_size_t size);

bool SaveFile(const char* location, const void* addr, new_size_t size, byte32 flags = FileFlags_Write);

#pragma endregion

#pragma region Log

template <bool timestamp, typename... Args> void Log(const char* format, Args... args) {
    if (noLog) {
        return;
    }


    char buffer[2048];
    constexpr new_size_t bufferSize = sizeof(buffer);
    offset_t pos                    = 0;


    auto BufferDest = [&]() { return (buffer + pos); };

    auto RemainingBufferSize = [&]() { return (bufferSize - pos); };


    if (timestamp) {
        SYSTEMTIME st = {};

        GetLocalTime(&st);

        snprintf(BufferDest(), RemainingBufferSize(), "%.2u:%.2u:%.2u.%.3u ", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

        pos = strlen(buffer);
    }


    snprintf(BufferDest(), RemainingBufferSize(), format, args...);

    pos = strlen(buffer);

    /*
    At this point pos is less than bufferSize.

    snprintf guarantees that we don't overflow and that the buffer is zero terminated.

    strlen excludes the zero.

    So pos can at most be (bufferSize - 1).

    We don't need the zero when saving the file. So we can use the full buffer.
    */

    buffer[pos] = 0xA;
    pos++;


    if (!SaveFile(logLocation, buffer, pos, FileFlags_Append)) {
        SaveFile(logLocation, buffer, pos, FileFlags_Write);
    }
}

void LogFunctionHelper(const char* funcName);

template <typename T> const char* LogFunctionHelper_GetFormat() {
    if constexpr (TypeMatch<T, bool>::value) {
        return "%s %u";
    } else if constexpr (TypeMatch<T, uint8>::value) {
        return "%s %u";
    } else if constexpr (TypeMatch<T, void*>::value || TypeMatch<T, byte8*>::value) {
#ifdef _WIN64
        return "%s %llX";
#else
        return "%s %X";
#endif
    }
}

template <typename T> void LogFunctionHelper(const char* funcName, T value) {
    Log(LogFunctionHelper_GetFormat<T>(), funcName, value);
}

void InitLog(const char* directoryName, const char* fileName);

#pragma endregion

#pragma region Containers

template <typename...> struct Container;

template <> struct Container<> {
    struct Metadata {
        offset_t off;
        new_size_t size;
    };

    byte8* dataAddr;
    new_size_t dataSize;

    byte8* metadataAddr;
    new_size_t metadataSize;

    offset_t pos;
    new_size_t count;

    // @Todo: Add alloc func arg.
    bool InitData(new_size_t size);
    bool InitMetadata(new_size_t size);
    bool Init(new_size_t dataSize2, new_size_t metadataSize2);
    void Clear();
    byte8* Next(new_size_t size = 0);


    void Push(void* addr, new_size_t size);
    void Pop();
    byte8* operator[](new_size_t index);


    byte8* Last();
};

// @Todo: Add Extend.
// @Todo: Add += and -- operator I guess.
template <typename T> struct Container<T> {
    T* dataAddr;
    new_size_t dataSize;
    new_size_t count;
    new_size_t capacity;

    bool Init(new_size_t size);
    void Push(const T& var);
    void Pop();


    void Remove(new_size_t index) {
        if ((count == 0) || (index >= count)) {
            return;
        }

        auto remainingCount = (count - index - 1);

        if (remainingCount == 0) {
            SetMemory(&dataAddr[index], 0, sizeof(T));

            count--;

            return;
        }

        auto remainingSize = (sizeof(T) * remainingCount);

        CopyMemory(&dataAddr[index], &dataAddr[(index + 1)], remainingSize);

        SetMemory(&dataAddr[(count - 1)], 0, sizeof(T));

        count--;
    }


    void Clear();
    T& operator[](new_size_t index);
};

template <typename T> bool Container<T>::Init(new_size_t size) {
    if (size == 0) {
        return false;
    }

    dataAddr = reinterpret_cast<T*>(HighAlloc(size));
    if (!dataAddr) {
        return false;
    }

    dataSize = size;

    capacity = (dataSize / sizeof(T));

    return true;
}

template <typename T> void Container<T>::Push(const T& var) {
    if (!dataAddr || (count >= capacity)) {
        return;
    }

    dataAddr[count] = var;

    count++;
}

template <typename T> void Container<T>::Pop() {
    if (!dataAddr || (count < 1)) {
        return;
    }

    dataAddr[(count - 1)] = 0;

    count--;
}

template <typename T> void Container<T>::Clear() {
    if (dataAddr) {
        SetMemory(dataAddr, 0, dataSize);
    }

    count = 0;
}

template <typename T> T& Container<T>::operator[](new_size_t index) {
    return dataAddr[index];
}

template <typename T> using Vector = Container<T>;

template <typename T, typename T2> struct Container<T, T2> {
    T dataAddr[T2::value];
    new_size_t dataSize;
    new_size_t count;
    new_size_t capacity;

    Container();

    void Push(const T& var);
    void Pop();
    void Clear();
    T& operator[](new_size_t index);
    // @Remove
    template <typename U> void ForEach(new_size_t start, new_size_t end, U& func);
    template <typename U> void ForAll(U& func);
};

template <typename T, typename T2> Container<T, T2>::Container() {
    dataSize = (sizeof(T) * T2::value);
    capacity = T2::value;
}

template <typename T, typename T2> void Container<T, T2>::Push(const T& var) {
    if (count >= capacity) {
        return;
    }

    dataAddr[count] = var;

    count++;
}

template <typename T, typename T2> void Container<T, T2>::Pop() {
    if (count < 1) {
        return;
    }

    dataAddr[(count - 1)] = 0;

    count--;
}

template <typename T, typename T2> void Container<T, T2>::Clear() {
    SetMemory(dataAddr, 0, dataSize);

    count = 0;
}

template <typename T, typename T2> T& Container<T, T2>::operator[](new_size_t index) {
    return dataAddr[index];
}

template <typename T, typename T2> template <typename U> void Container<T, T2>::ForEach(new_size_t start, new_size_t end, U& func) {
    for_each(index, start, end) {
        auto& data = dataAddr[index];

        func(data);
    }
}

template <typename T, typename T2> template <typename U> void Container<T, T2>::ForAll(U& func) {
    return ForEach(0, count, func);
}

template <typename T, new_size_t T2> using Array = Container<T, TypeValue<T2>>;

#pragma endregion

#pragma region Memory 2

extern Container<> memoryData;

struct ProtectionHelperData {
    void* addr;
    new_size_t size;
    byte32 protection;
};

struct ProtectionHelper : Container<ProtectionHelperData> {
    void Push(void* addr, new_size_t size);
    void Pop();
};

extern ProtectionHelper protectionHelper;

enum {
    MemoryFlags_VirtualProtectDestination = 1 << 0,
    MemoryFlags_VirtualProtectSource      = 1 << 1,
};

void SetMemory(void* addr, byte8 value, new_size_t size, byte32 flags);

void CopyMemory(void* destination, const void* source, new_size_t size, byte32 flags);

struct BackupHelper : Container<> {
    struct Metadata : Container<>::Metadata {
        void* addr;
    };

    void Save(void* addr, new_size_t size);
    void Restore(void* addr);
};

extern BackupHelper backupHelper;

template <typename T> void Write(void* addr, T value, uint32 padSize = 0, byte8 padValue = 0x90) {
    constexpr uint32 size = sizeof(T);

    protectionHelper.Push(addr, size);

    *reinterpret_cast<T*>(addr) = value;

    if (padSize) {
        SetMemory((reinterpret_cast<byte8*>(addr) + size), padValue, padSize);
    }

    protectionHelper.Pop();
}

void WriteAddress(byte8* addr, void* dest, uint32 size, byte8 value = 0, uint32 padSize = 0, byte8 padValue = 0x90, uint32 off = 0);

void WriteCall(byte8* addr, void* dest, uint32 padSize = 0, byte8 padValue = 0x90);

void WriteJump(byte8* addr, void* dest, uint32 padSize = 0, byte8 padValue = 0x90);

void WriteShortJump(byte8* addr, void* dest, uint32 padSize = 0, byte8 padValue = 0x90);

void WriteNop(void* addr, new_size_t size);

#pragma region CreateFunction

#ifdef _WIN64

struct Function {
    byte8* addr;
    byte8* sect0;
    byte8* sect1;
    byte8* sect2;
    byte8** cache;
};

// @Todo: Increase Memory Data count.

enum {
    FunctionFlags_SaveRegisters    = 1 << 0,
    FunctionFlags_NoResult         = 1 << 1,
    FunctionFlags_NoReturn         = 1 << 2,
    FunctionFlags_Jump             = 1 << 3,
    FunctionFlags_SaveXMMRegisters = 1 << 4,
    FunctionFlags_NoXMMResult      = 1 << 5,
};

Function CreateFunction(void* funcAddr = 0, byte8* jumpAddr = 0, byte64 flags = 0, new_size_t size0 = 0, new_size_t size1 = 0,
    new_size_t size2 = 0, new_size_t cacheSize = 0, new_size_t count = 0);

__declspec(deprecated) Function old_CreateFunction(void* funcAddr = 0, byte8* jumpAddr = 0, bool saveRegisters = true, bool noResult = true,
    new_size_t size0 = 0, new_size_t size1 = 0, new_size_t size2 = 0, new_size_t cacheSize = 0, new_size_t count = 0, bool noReturn = false,
    bool saveXMMRegisters = false, bool noXMMResult = true);

#else

enum {
    FunctionFlags_SaveRegisters = 1 << 0,
    FunctionFlags_NoResult      = 1 << 1,
    FunctionFlags_NoReturn      = 1 << 2,
    FunctionFlags_Jump          = 1 << 3,
};


struct Function {
    byte8* addr;
    byte8* sect0;
    byte8* sect1;
    byte8* sect2;
    byte8** cache;
};

// @Update
Function CreateFunction(void* funcAddr = 0, byte8* jumpAddr = 0, byte32 flags = 0, new_size_t size0 = 0, new_size_t size1 = 0,
    new_size_t size2 = 0, new_size_t cacheSize = 0);

// @Remove
_declspec(deprecated) Function old_CreateFunction(void* funcAddr = 0, byte8* jumpAddr = 0, bool saveRegisters = true, bool noResult = true,
    new_size_t size0 = 0, new_size_t size1 = 0, new_size_t size2 = 0, new_size_t cacheSize = 0, bool noReturn = true);

#endif

#pragma endregion

bool Core_Memory_Init();

#pragma endregion

#pragma region String Stuff

int32 IndexOf(const char* name, const char* name2);

#pragma endregion

#pragma region Windows

inline bool Windows_GetTicksPerSecond(uint64* var) {
    return QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(var));
}

inline bool Windows_GetTickCount(uint64* var) {
    return QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(var));
}

void Windows_ToggleCursor(bool enable);

// bool Windows_GetWindowPos(HWND window, POINT * point);
// bool IsBorderless(HWND windowHandle);

// @Todo: Update for RVO.
void GetWindowPos(HWND window, POINT& point);

#pragma endregion

enum {
    KeyFlags_AtLeastOneKey = 1 << 0,
};

struct KeyData {
    byte8 keys[4];
    new_size_t keyCount;
    bool allowEmpty;
    bool execute;
    // void * func;
	

	bool IsEmpty() const {
		return !allowEmpty && keyCount == 0;
	}

    typedef void (*func_t)();

    func_t func;

    void Clear() {
        SetMemory(keys, 0, sizeof(keys));

        keyCount = 0;
    }

    void AddKey(byte8 newKey) {
        if (keyCount >= countof(keys)) {
            return;
        }

        for (new_size_t keyIndex = 0; keyIndex < keyCount; keyIndex++) {
            if (keys[keyIndex] == newKey) {
                return;
            }
        }

        keys[keyCount] = newKey;

        keyCount++;
    }


    // @Todo: Use KEYBOARDSTATE.
    void Check(byte8* state) {
        // if (showPopup)
        // {
        // 	return;
        // }

        // auto & execute  = executes3[0];
        // auto & keys     = activeKeyData.keys;
        // auto & keyCount = activeKeyData.keyCount;

        new_size_t keysDown = 0;

        if (keyCount < 1) {
            return;
        }

        for (new_size_t keyIndex = 0; keyIndex < keyCount; keyIndex++) {
            auto& key = keys[keyIndex];

            if (state[key] & 0x80) {
                keysDown++;
            }
        }

        if (keysDown == keyCount) {
            if (execute) {
                execute = false;

                [&]() {
                    if (!func) {
                        return;
                    }

                    func();
                }();
            }
        } else {
            execute = true;
        }
    }
};

// @Research: New module (Please god fuck no)
namespace DI8 {
// @Todo: Add to create_modules as new.
struct DIKEYBOARDSTATE {
    byte8 keys[256];
};

// $KeyStart
namespace KEY {
enum {
    UNKNOWN_0,
    ESCAPE,
    ONE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    ZERO,
    MINUS,
    EQUALS,
    BACKSPACE,
    TAB,
    Q,
    W,
    E,
    R,
    T,
    Y,
    U = 22,
    I,
    O,
    P,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    ENTER,
    LEFT_CONTROL,
    A,
    S,
    D,
    F,
    G,
    H,
    J,
    K,
    L,
    SEMICOLON,
    APOSTROPHE,
    TILDE,
    LEFT_SHIFT = 42,
    BACKSLASH,
    Z,
    X,
    C,
    V,
    B,
    N,
    M,
    COMMA,
    PERIOD,
    SLASH,
    RIGHT_SHIFT,
    NUMPAD_MULTIPLY,
    LEFT_ALT,
    SPACE,
    CAPSLOCK,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    NUMLOCK,
    SCROLLLOCK,
    NUMPAD_7,
    NUMPAD_8,
    NUMPAD_9,
    NUMPAD_SUB,
    NUMPAD_4,
    NUMPAD_5,
    NUMPAD_6,
    NUMPAD_ADD,
    NUMPAD_1,
    NUMPAD_2,
    NUMPAD_3,
    NUMPAD_0,
    NUMPAD_DECIMAL,
    UNKNOWN_84,
    UNKNOWN_85,
    UNKNOWN_86,
    F11,
    F12,
    UNKNOWN_89,
    UNKNOWN_90,
    UNKNOWN_91,
    UNKNOWN_92,
    UNKNOWN_93,
    UNKNOWN_94,
    UNKNOWN_95,
    UNKNOWN_96,
    UNKNOWN_97,
    UNKNOWN_98,
    UNKNOWN_99,
    UNKNOWN_100,
    UNKNOWN_101,
    UNKNOWN_102,
    UNKNOWN_103,
    UNKNOWN_104,
    UNKNOWN_105,
    UNKNOWN_106,
    UNKNOWN_107,
    UNKNOWN_108,
    UNKNOWN_109,
    UNKNOWN_110,
    UNKNOWN_111,
    UNKNOWN_112,
    UNKNOWN_113,
    UNKNOWN_114,
    UNKNOWN_115,
    UNKNOWN_116,
    UNKNOWN_117,
    UNKNOWN_118,
    UNKNOWN_119,
    UNKNOWN_120,
    UNKNOWN_121,
    UNKNOWN_122,
    UNKNOWN_123,
    UNKNOWN_124,
    UNKNOWN_125,
    UNKNOWN_126,
    UNKNOWN_127,
    UNKNOWN_128,
    UNKNOWN_129,
    UNKNOWN_130,
    UNKNOWN_131,
    UNKNOWN_132,
    UNKNOWN_133,
    UNKNOWN_134,
    UNKNOWN_135,
    UNKNOWN_136,
    UNKNOWN_137,
    UNKNOWN_138,
    UNKNOWN_139,
    UNKNOWN_140,
    UNKNOWN_141,
    UNKNOWN_142,
    UNKNOWN_143,
    UNKNOWN_144,
    UNKNOWN_145,
    UNKNOWN_146,
    UNKNOWN_147,
    UNKNOWN_148,
    UNKNOWN_149,
    UNKNOWN_150,
    UNKNOWN_151,
    UNKNOWN_152,
    UNKNOWN_153,
    UNKNOWN_154,
    UNKNOWN_155,
    UNKNOWN_156,
    RIGHT_CONTROL,
    UNKNOWN_158,
    UNKNOWN_159,
    UNKNOWN_160,
    UNKNOWN_161,
    UNKNOWN_162,
    UNKNOWN_163,
    UNKNOWN_164,
    UNKNOWN_165,
    UNKNOWN_166,
    UNKNOWN_167,
    UNKNOWN_168,
    UNKNOWN_169,
    UNKNOWN_170,
    UNKNOWN_171,
    UNKNOWN_172,
    UNKNOWN_173,
    UNKNOWN_174,
    UNKNOWN_175,
    UNKNOWN_176,
    UNKNOWN_177,
    UNKNOWN_178,
    UNKNOWN_179,
    UNKNOWN_180,
    NUMPAD_DIVIDE,
    UNKNOWN_182,
    PRINT,
    RIGHT_ALT,
    UNKNOWN_185,
    UNKNOWN_186,
    UNKNOWN_187,
    UNKNOWN_188,
    UNKNOWN_189,
    UNKNOWN_190,
    UNKNOWN_191,
    UNKNOWN_192,
    UNKNOWN_193,
    UNKNOWN_194,
    UNKNOWN_195,
    UNKNOWN_196,
    PAUSE,
    UNKNOWN_198,
    HOME,
    UP,
    PAGE_UP,
    UNKNOWN_202,
    LEFT,
    UNKNOWN_204,
    RIGHT,
    UNKNOWN_206,
    END,
    DOWN,
    PAGE_DOWN,
    INSERT,
    DELETE,
    UNKNOWN_212,
    UNKNOWN_213,
    UNKNOWN_214,
    UNKNOWN_215,
    UNKNOWN_216,
    UNKNOWN_217,
    UNKNOWN_218,
    WIN,
    UNKNOWN_220,
    UNKNOWN_221,
    UNKNOWN_222,
    UNKNOWN_223,
    UNKNOWN_224,
    UNKNOWN_225,
    UNKNOWN_226,
    UNKNOWN_227,
    UNKNOWN_228,
    UNKNOWN_229,
    UNKNOWN_230,
    UNKNOWN_231,
    UNKNOWN_232,
    UNKNOWN_233,
    UNKNOWN_234,
    UNKNOWN_235,
    UNKNOWN_236,
    UNKNOWN_237,
    UNKNOWN_238,
    UNKNOWN_239,
    UNKNOWN_240,
    UNKNOWN_241,
    UNKNOWN_242,
    UNKNOWN_243,
    UNKNOWN_244,
    UNKNOWN_245,
    UNKNOWN_246,
    UNKNOWN_247,
    UNKNOWN_248,
    UNKNOWN_249,
    UNKNOWN_250,
    UNKNOWN_251,
    UNKNOWN_252,
    UNKNOWN_253,
    UNKNOWN_254,
    UNKNOWN_255,
    COUNT,
};
};

extern const char* keyNames[];

// $KeyEnd
static_assert(KEY::COUNT == 256);


// @Remove
namespace BUFFER_SIZE {
enum {
    KEYBOARD = 256,
};
};
} // namespace DI8

#pragma region Global

extern vec2 g_windowSize;
extern vec2 g_clientSize;
extern vec2 g_renderSize;

extern float g_frameRateMultiplier;

POINT GetWindowSize(HWND windowHandle);
POINT GetClientSize(HWND windowHandle);
void UpdateGlobalWindowSize();
void UpdateGlobalClientSize();
void UpdateGlobalRenderSize(uint32 width, uint32 height);

#pragma endregion
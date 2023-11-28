// @Review

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <xmmintrin.h>

// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "Core.hpp"
#include "DataTypes.hpp"

#include "tlhelp32.h"

#include "Macros.h"

#include "DebugSwitch.hpp"

bool noLog = false;

void _nop(char* dst, unsigned int size) {
    DWORD oldprotect;
    VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
    memset(dst, 0x90, size);
    VirtualProtect(dst, size, oldprotect, &oldprotect);
};

void _patch(char* dst, char* src, int size) {
    DWORD oldprotect;
    VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
    memcpy(dst, src, size);
    VirtualProtect(dst, size, oldprotect, &oldprotect);
};

#pragma region utility

bool Match(const void* addr, const void* addr2, uint32 count) {
    for (uint32 index = 0; index < count; index++) {
        if (reinterpret_cast<byte8*>(const_cast<void*>(addr))[index] != reinterpret_cast<byte8*>(const_cast<void*>(addr2))[index]) {
            return false;
        }
    }

    return true;
}

#pragma endregion

#pragma region Memory

byte8* appBaseAddr = 0;
uint32 appSize     = 0;
HWND appWindow     = 0;

SYSTEM_INFO systemInfo = {};

void SetMemory(void* addr, byte8 value, new_size_t size) {
    memset(addr, value, size);
}

void CopyMemory(void* destination, const void* source, new_size_t size) {
    memcpy(destination, source, size);
}

byte8* Alloc(new_size_t size, byte8* dest) {
    byte8* addr  = 0;
    byte32 error = 0;

    SetLastError(0);

    addr = reinterpret_cast<byte8*>(VirtualAlloc(dest, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));

    error = GetLastError();

    if (!addr) {
        Log("VirtualAlloc failed. %X", error);
    }

    return addr;
}

byte8* AllocEx(new_size_t size, offset_t start, offset_t end) {
    if constexpr (debug) {
        LogFunction();

        Log("size  %llX", size);
        Log("start %llX", start);
        Log("end   %llX", end);
    }

    byte8* addr = 0;

    auto pos = start;

    MEMORY_BASIC_INFORMATION mbi = {};
    bool match                   = false;
    byte32 error                 = 0;

    do {
        SetLastError(0);

        if (VirtualQuery(reinterpret_cast<void*>(pos), &mbi, sizeof(mbi)) == 0) {
            error = GetLastError();

            Log("VirtualQuery failed. %X", error);
        }

        if ((mbi.RegionSize >= size) && (mbi.State == MEM_FREE)) {
            if constexpr (debug) {
                Log("pos        %llX", pos);
                Log("regionSize %llX", mbi.RegionSize);
                Log("state      %X", mbi.State);
            }

            auto remainder = Align<offset_t>(pos, systemInfo.dwAllocationGranularity);
            if (!remainder) {
                match = true;

                break;
            }

            continue;
        }

        pos += mbi.RegionSize;
    } while (pos < end);

    if (!match) {
        return 0;
    }

    addr = reinterpret_cast<byte8*>(mbi.BaseAddress);

    addr = Alloc(size, addr);

    SetLastError(0);

    if (VirtualQuery(addr, &mbi, sizeof(mbi)) == 0) {
        error = GetLastError();

        Log("VirtualQuery failed. %X", error);
    }

    if constexpr (debug) {
        Log("addr  %llX", addr);
        Log("state %X", mbi.State);
    }

    return addr;
}

byte8* LowAlloc(new_size_t size) {
    return AllocEx(size, 0x10000, 0x7FFFFFFF);
}

byte8* HighAlloc(new_size_t size) {
    return AllocEx(size, reinterpret_cast<offset_t>(appBaseAddr + appSize), reinterpret_cast<offset_t>(appBaseAddr + 0x7FFFFFFF));
}

bool Free(byte8* addr) {
    if (!addr) {
        return false;
    }

    byte32 error = 0;

    SetLastError(0);

    if (!VirtualFree(addr, 0, MEM_RELEASE)) {
        error = GetLastError();

        Log("VirtualFree failed. %X", error);

        return false;
    }

    return true;
}

#pragma endregion

#pragma region File

HANDLE OpenFile(const char* location, byte32 flags) {
    if (flags & FileFlags_Read) {
        return CreateFileA(location, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    } else if (flags & FileFlags_Write) {
        return CreateFileA(location, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    } else if (flags & FileFlags_Append) {
        return CreateFileA(location, FILE_APPEND_DATA, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    }

    return INVALID_HANDLE_VALUE;
}

bool CloseFile(HANDLE fileHandle) {
    return (CloseHandle(fileHandle) != 0);
}

new_size_t GetFileSize(HANDLE fileHandle) {
    if (fileHandle == INVALID_HANDLE_VALUE) {
        return 0;
    }


    BY_HANDLE_FILE_INFORMATION fileInformation = {};

    if (!GetFileInformationByHandle(fileHandle, &fileInformation)) {
        return 0;
    }


#ifdef _WIN64

    new_size_t size = 0;

    auto sizeAddr = reinterpret_cast<byte8*>(&size);

    CopyMemory(sizeAddr, &fileInformation.nFileSizeLow, 4);

    CopyMemory((sizeAddr + 4), &fileInformation.nFileSizeHigh, 4);

    return size;


#else

    return fileInformation.nFileSizeLow;

#endif
}

bool LoadFile(HANDLE fileHandle, new_size_t size, void* dest,
    offset_t start // Default value unnecessarily complicates overload resolution.
) {
    if ((fileHandle == INVALID_HANDLE_VALUE) || !dest) {
        return false;
    }


    constexpr new_size_t bufferSize = (1 * 1024 * 1024); // @Research: Consider global bufferSize.
    auto pos                        = start;
    uint32 bytesRead                = 0;
    LARGE_INTEGER filePointer       = {};


    auto Function = [&](new_size_t size2) -> bool {
        LARGE_INTEGER newPos = {};

        *reinterpret_cast<offset_t*>(&newPos) = pos;


        if (!SetFilePointerEx(fileHandle, newPos, &filePointer, FILE_BEGIN)) {
            return false;
        }


        if (!ReadFile(fileHandle, (reinterpret_cast<byte8*>(dest) + pos), static_cast<uint32>(size2), &bytesRead, 0)) {
            return false;
        }


        pos += size2;
        size -= size2;


        return true;
    };


    while (size > bufferSize) {
        if (!Function(bufferSize)) {
            return false;
        }
    }


    if (size > 0) {
        if (!Function(size)) {
            return false;
        }
    }


    return true;
}

byte8* LoadFile(HANDLE fileHandle, new_size_t size,
    offset_t start // Default value unnecessarily complicates overload resolution.
) {
    auto dest = Alloc(size);
    if (!dest) {
        return 0;
    }


    if (!LoadFile(fileHandle, size, dest, start)) {
        Free(dest);

        return 0;
    }


    return dest;
}

byte8* LoadFile(const char* location) {
    if (!location) {
        return 0;
    }


    byte32 error      = 0;
    HANDLE fileHandle = 0;
    new_size_t size   = 0;
    byte8* dest       = 0;


    fileHandle = OpenFile(location, FileFlags_Read);
    if (fileHandle == INVALID_HANDLE_VALUE) {
        return 0;
    }


    SetLastError(0);

    size = GetFileSize(fileHandle); // File can exist, but be empty.

    error = GetLastError(); // @Research: Not sure if error checking is necessary here.

    if (error) {
        return 0;
    }


    dest = LoadFile(fileHandle,
        size, // Size is rounded up to next page boundary.
        0);


    CloseFile(fileHandle);


    return dest;
}

bool SaveFile(HANDLE fileHandle, const void* addr, new_size_t size) {
    if ((fileHandle == INVALID_HANDLE_VALUE) || !addr) {
        return false;
    }


    constexpr new_size_t bufferSize = (1 * 1024 * 1024);
    offset_t pos                    = 0;
    uint32 bytesWritten             = 0;
    LARGE_INTEGER filePointer       = {};


    auto Function = [&](new_size_t size2) -> bool {
        if (!WriteFile(
                fileHandle, (reinterpret_cast<byte8*>(const_cast<void*>(addr)) + pos), static_cast<uint32>(size2), &bytesWritten, 0)) {
            return false;
        }

        pos += size2;
        size -= size2;

        return true;
    };


    while (size > bufferSize) {
        if (!Function(bufferSize)) {
            return false;
        }
    }


    if (size > 0) {
        if (!Function(size)) {
            return false;
        }
    }


    return true;
}

bool SaveFile(const char* location, const void* addr, new_size_t size, byte32 flags) {
    if (!location || !addr) {
        return false;
    }


    HANDLE fileHandle = 0;


    fileHandle = OpenFile(location, flags);
    if (fileHandle == INVALID_HANDLE_VALUE) {
        return false;
    }


    SaveFile(fileHandle, addr, size);


    CloseFile(fileHandle);


    return true;
}

#pragma endregion

#pragma region Log

// @Research: Solution for situations where the file system is not available.
// Consider accumulation and later emit or fallback like OutputDebugStringA.

char logLocation[64] = {};

void LogFunctionHelper(const char* funcName) {
    Log(funcName);
}

void InitLog(const char* directoryName, const char* fileName) {
    CreateDirectoryA(directoryName, 0);

    snprintf(logLocation, sizeof(logLocation), "%s/%s", directoryName, fileName);

    DeleteFileA(logLocation);
}

#pragma endregion

#pragma region Containers

// @Todo: virtual Push & Pop.
bool Container<>::InitData(new_size_t size) {
    if (size == 0) {
        return false;
    }

    dataAddr = HighAlloc(size);
    if (!dataAddr) {
        return false;
    }

    dataSize = size;

    return true;
}

bool Container<>::InitMetadata(new_size_t size) {
    if (size == 0) {
        return false;
    }

    metadataAddr = HighAlloc(size);
    if (!metadataAddr) {
        return false;
    }

    metadataSize = size;

    return true;
}

bool Container<>::Init(new_size_t dataSize2, new_size_t metadataSize2) {
    if ((dataSize2 == 0) || !InitData(dataSize2) || (metadataSize2 == 0) || !InitMetadata(metadataSize2)) {
        return false;
    }

    return true;
}

void Container<>::Clear() {
    if (dataAddr) {
        SetMemory(dataAddr, 0, dataSize);
    }

    if (metadataAddr) {
        SetMemory(metadataAddr, 0, metadataSize);
    }

    pos   = 0;
    count = 0;
}

byte8* Container<>::Next(new_size_t size) {
    if (!dataAddr || (dataSize == 0) || (size > dataSize) || (pos > (dataSize - size))) {
        return 0;
    }

    return (dataAddr + pos);
}

void Container<>::Push(void* addr, new_size_t size) {
    if (!dataAddr || (dataSize == 0) || !metadataAddr || (metadataSize == 0) || !addr || (size == 0) || (pos > (dataSize - size))) {
        return;
    }

    CopyMemory((dataAddr + pos), addr, size);

    auto& metadata = reinterpret_cast<Metadata*>(metadataAddr)[count];

    metadata.off  = pos;
    metadata.size = size;

    pos += size;

    count++;
}

void Container<>::Pop() {
    if (!dataAddr || !metadataAddr || (count < 1)) {
        return;
    }

    auto& metadata = reinterpret_cast<Metadata*>(metadataAddr)[(count - 1)];

    SetMemory((dataAddr + metadata.off), 0, metadata.size);

    pos -= metadata.size;

    SetMemory(&metadata, 0, sizeof(metadata));

    count--;
}

byte8* Container<>::operator[](new_size_t index) {
    if (!dataAddr || !metadataAddr || (index >= count)) {
        return 0;
    }

    auto& metadata = reinterpret_cast<Metadata*>(metadataAddr)[index];

    return (dataAddr + metadata.off);
}


byte8* Container<>::Last() {
    if (!dataAddr || (dataSize == 0) || !metadataAddr || (metadataSize == 0) || (count < 1)) {
        return 0;
    }

    return (*this)[(count - 1)];
}

#pragma endregion

#pragma region Memory 2

Container<> memoryData = {};

void ProtectionHelper::Push(void* addr, new_size_t size) {
    if (!dataAddr || !addr || (size == 0) || (count >= capacity)) {
        return;
    }

    auto& data = dataAddr[count];

    byte32 error      = 0;
    byte32 protection = 0;

    SetLastError(0);

    if (VirtualProtect(addr, size, PAGE_EXECUTE_READWRITE, &protection) == 0) {
        error = GetLastError();

        Log("VirtualProtect failed. %X", error);
    }

    data.addr       = addr;
    data.size       = size;
    data.protection = protection;

    count++;
}

void ProtectionHelper::Pop() {
    if (!dataAddr || (count < 1)) {
        return;
    }

    auto& data = dataAddr[(count - 1)];

    byte32 error      = 0;
    byte32 protection = 0;

    SetLastError(0);

    if (VirtualProtect(data.addr, data.size, data.protection, &protection) == 0) {
        error = GetLastError();

        Log("VirtualProtect failed. %X", error);
    }

    SetMemory(&data, 0, sizeof(data));

    count--;
}

ProtectionHelper protectionHelper = {};

void SetMemory(void* addr, byte8 value, new_size_t size, byte32 flags) {
    if (flags & MemoryFlags_VirtualProtectDestination) {
        protectionHelper.Push(addr, size);
    }

    SetMemory(addr, value, size);

    if (flags & MemoryFlags_VirtualProtectDestination) {
        protectionHelper.Pop();
    }
}

void CopyMemory(void* destination, const void* source, new_size_t size, byte32 flags) {
    if (flags & MemoryFlags_VirtualProtectDestination) {
        protectionHelper.Push(destination, size);
    }

    if (flags & MemoryFlags_VirtualProtectSource) {
        protectionHelper.Push(const_cast<void*>(source), size);
    }

    CopyMemory(destination, source, size);

    if (flags & MemoryFlags_VirtualProtectSource) {
        protectionHelper.Pop();
    }

    if (flags & MemoryFlags_VirtualProtectDestination) {
        protectionHelper.Pop();
    }
}

void BackupHelper::Save(void* addr, new_size_t size) {
    if (!dataAddr || !metadataAddr || !addr || (size == 0) || (pos > (dataSize - size))) {
        return;
    }

    CopyMemory((dataAddr + pos), addr, size, MemoryFlags_VirtualProtectSource);

    auto& metadata = reinterpret_cast<Metadata*>(metadataAddr)[count];

    metadata.off  = pos;
    metadata.size = size;
    metadata.addr = addr;

    pos += size;

    count++;
}

void BackupHelper::Restore(void* addr) {
    for_all(index, count) {
        auto& metadata = reinterpret_cast<Metadata*>(metadataAddr)[index];

        if (metadata.addr != addr) {
            continue;
        }

        CopyMemory(metadata.addr, (dataAddr + metadata.off), metadata.size, MemoryFlags_VirtualProtectDestination);

        return;
    }

    auto off = static_cast<offset_t>(reinterpret_cast<byte8*>(addr) - appBaseAddr);

    Log("%s failed.", FUNC_NAME);

    Log("No Match dmc3.exe+%llX", off);
}

BackupHelper backupHelper = {};

void WriteAddress(byte8* addr, void* dest, uint32 size, byte8 value, uint32 padSize, byte8 padValue, uint32 off) {
    protectionHelper.Push(addr, (size + padSize));

    if (value) {
        addr[0] = value;
    }

    if (size == 2) {
        *reinterpret_cast<int8*>(addr + (size - 1 - off)) = static_cast<int8>(reinterpret_cast<byte8*>(dest) - addr - size);
    } else {
        *reinterpret_cast<int32*>(addr + (size - 4 - off)) = static_cast<int32>(reinterpret_cast<byte8*>(dest) - addr - size);
    }

    if (padSize) {
        SetMemory((addr + size), padValue, padSize);
    }

    protectionHelper.Pop();
}

void WriteCall(byte8* addr, void* dest, uint32 padSize, byte8 padValue) {
    return WriteAddress(addr, dest, 5, 0xE8, padSize, padValue);
}

void WriteJump(byte8* addr, void* dest, uint32 padSize, byte8 padValue) {
    return WriteAddress(addr, dest, 5, 0xE9, padSize, padValue);
}

void WriteShortJump(byte8* addr, void* dest, uint32 padSize, byte8 padValue) {
    return WriteAddress(addr, dest, 2, 0xEB, padSize, padValue);
}

void WriteNop(void* addr, new_size_t size) {
    SetMemory(addr, 0x90, size, MemoryFlags_VirtualProtectDestination);
}

#pragma region CreateFunction

#ifdef _WIN64

Function CreateFunction(void* funcAddr, byte8* jumpAddr, byte64 flags, new_size_t size0, new_size_t size1, new_size_t size2,
    new_size_t cacheSize, new_size_t count) {


    // @Remove
    const bool saveRegisters    = (flags & FunctionFlags_SaveRegisters);
    const bool noResult         = (flags & FunctionFlags_NoResult);
    const bool noReturn         = (flags & FunctionFlags_NoReturn);
    const bool saveXMMRegisters = (flags & FunctionFlags_SaveXMMRegisters);
    const bool noXMMResult      = (flags & FunctionFlags_NoXMMResult);


    Function func = {};
    offset_t pos  = 0;

    auto Feed = [&](const byte8* buffer, new_size_t bufferSize, bool adjustPosition = true) {
        // @Todo: Use CopyMemory.
        memcpy((func.addr + pos), buffer, bufferSize);

        if (!adjustPosition) {
            return;
        }

        pos += bufferSize;
    };

    Align<offset_t>(memoryData.pos, 0x10);

    func.addr = (memoryData.dataAddr + memoryData.pos);

    func.sect0 = (func.addr + pos);
    pos += size0;

    if (saveRegisters) {
        if (noResult) {
            constexpr byte8 buffer[] = {
                push_rax,
            };
            Feed(buffer, sizeof(buffer));
        }

        if (count) {
            if (noResult) {
                constexpr byte8 buffer[] = {
                    0x48, 0x8D, 0x44, 0x24, 0x08, // lea rax,[rsp+08]
                };
                Feed(buffer, sizeof(buffer));
            } else {
                constexpr byte8 buffer[] = {
                    0x48, 0x8B, 0xC4, // mov rax,rsp
                };
                Feed(buffer, sizeof(buffer));
            }
        }

        {
            // @Todo: Replace with macros.
            constexpr byte8 buffer[] = {
                0x51,       // push rcx
                0x52,       // push rdx
                0x53,       // push rbx
                0x54,       // push rsp
                0x55,       // push rbp
                0x56,       // push rsi
                0x57,       // push rdi
                0x41, 0x50, // push r8
                0x41, 0x51, // push r9
                0x41, 0x52, // push r10
                0x41, 0x53, // push r11
                0x41, 0x54, // push r12
                0x41, 0x55, // push r13
                0x41, 0x56, // push r14
                0x41, 0x57, // push r15
                0x9C,       // pushfq
            };
            Feed(buffer, sizeof(buffer));
        }

        if (saveXMMRegisters) {
            {
                constexpr byte8 buffer[] = {
                    0x48, 0x81, 0xEC, 0x00, 0x01, 0x00, 0x00, // sub rsp,00000100
                };
                Feed(buffer, sizeof(buffer));
            }
            if (noXMMResult) {
                constexpr byte8 buffer[] = {
                    0x0F, 0x11, 0x84, 0x24, 0xF0, 0x00, 0x00, 0x00, // movups [rsp+000000F0],xmm0
                };
                Feed(buffer, sizeof(buffer));
            }
            {
                constexpr byte8 buffer[] = {
                    0x0F, 0x11, 0x8C, 0x24, 0xE0, 0x00, 0x00, 0x00,       // movups [rsp+000000E0],xmm1
                    0x0F, 0x11, 0x94, 0x24, 0xD0, 0x00, 0x00, 0x00,       // movups [rsp+000000D0],xmm2
                    0x0F, 0x11, 0x9C, 0x24, 0xC0, 0x00, 0x00, 0x00,       // movups [rsp+000000C0],xmm3
                    0x0F, 0x11, 0xA4, 0x24, 0xB0, 0x00, 0x00, 0x00,       // movups [rsp+000000B0],xmm4
                    0x0F, 0x11, 0xAC, 0x24, 0xA0, 0x00, 0x00, 0x00,       // movups [rsp+000000A0],xmm5
                    0x0F, 0x11, 0xB4, 0x24, 0x90, 0x00, 0x00, 0x00,       // movups [rsp+00000090],xmm6
                    0x0F, 0x11, 0xBC, 0x24, 0x80, 0x00, 0x00, 0x00,       // movups [rsp+00000080],xmm7
                    0x44, 0x0F, 0x11, 0x84, 0x24, 0x70, 0x00, 0x00, 0x00, // movups [rsp+00000070],xmm8
                    0x44, 0x0F, 0x11, 0x8C, 0x24, 0x60, 0x00, 0x00, 0x00, // movups [rsp+00000060],xmm9
                    0x44, 0x0F, 0x11, 0x94, 0x24, 0x50, 0x00, 0x00, 0x00, // movups [rsp+00000050],xmm10
                    0x44, 0x0F, 0x11, 0x9C, 0x24, 0x40, 0x00, 0x00, 0x00, // movups [rsp+00000040],xmm11
                    0x44, 0x0F, 0x11, 0xA4, 0x24, 0x30, 0x00, 0x00, 0x00, // movups [rsp+00000030],xmm12
                    0x44, 0x0F, 0x11, 0xAC, 0x24, 0x20, 0x00, 0x00, 0x00, // movups [rsp+00000020],xmm13
                    0x44, 0x0F, 0x11, 0xB4, 0x24, 0x10, 0x00, 0x00, 0x00, // movups [rsp+00000010],xmm14
                    0x44, 0x0F, 0x11, 0xBC, 0x24, 0x00, 0x00, 0x00, 0x00, // movups [rsp+00000000],xmm15
                };
                Feed(buffer, sizeof(buffer));
            }
        }

        {
            constexpr byte8 buffer[] = {
                mov_rbp_rsp,
            };
            Feed(buffer, sizeof(buffer));
        }

        if (count) {
            constexpr byte8 buffer[] = {
                0x48, 0x81, 0xEC, 0x20, 0x00, 0x00, 0x00, // sub rsp,00000020
            };
            Feed(buffer, sizeof(buffer), false);
            *reinterpret_cast<uint32*>(func.addr + pos + 3) += (static_cast<uint32>(count) * 8);
            pos += sizeof(buffer);
        } else {
            constexpr byte8 buffer[] = {
                0x48, 0x83, 0xEC, 0x20, // sub rsp,20
            };
            Feed(buffer, sizeof(buffer));
        }

        {
            constexpr byte8 buffer[] = {
                0x40, 0x80, 0xE4, 0xF0, // and spl,F0
            };
            Feed(buffer, sizeof(buffer));
        }

        if (count) {
            constexpr byte8 buffer[] = {
                0x51,                         // push rcx
                0x56,                         // push rsi
                0x57,                         // push rdi
                0xB9, 0x00, 0x00, 0x00, 0x00, // mov ecx
                0x48, 0x8D, 0x70, 0x28,       // lea rsi,[rax+28] return addr + shadow space
                0x48, 0x8D, 0x7C, 0x24, 0x38, // lea rdi,[rsp+38] rdi + rsi + rcx + shadow space
                0xF3, 0x48, 0xA5,             // repe movsq
                0x5F,                         // pop rdi
                0x5E,                         // pop rsi
                0x59,                         // pop rcx
            };
            Feed(buffer, sizeof(buffer), false);
            *reinterpret_cast<uint32*>(func.addr + pos + 4) = static_cast<uint32>(count);
            pos += sizeof(buffer);
        }
    }

    func.sect1 = (func.addr + pos);
    pos += size1;

    if (funcAddr) {
        constexpr byte8 buffer[] = {
            0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rax
        };
        Feed(buffer, sizeof(buffer), false);
        *reinterpret_cast<void**>(func.addr + pos + 2) = funcAddr;
        pos += sizeof(buffer);


        if (flags & FunctionFlags_Jump) {
            constexpr byte8 buffer[] = {
                0xFF, 0xE0, // jmp rax
            };
            Feed(buffer, sizeof(buffer));
        } else {
            constexpr byte8 buffer[] = {
                0xFF, 0xD0, // call rax
            };
            Feed(buffer, sizeof(buffer));
        }
    }

    if (saveRegisters) {
        {
            constexpr byte8 buffer[] = {
                mov_rsp_rbp,
            };
            Feed(buffer, sizeof(buffer));
        }

        if (saveXMMRegisters) {
            {
                constexpr byte8 buffer[] = {
                    0x44, 0x0F, 0x10, 0xBC, 0x24, 0x00, 0x00, 0x00, 0x00, // movups xmm15,[rsp+00000000]
                    0x44, 0x0F, 0x10, 0xB4, 0x24, 0x10, 0x00, 0x00, 0x00, // movups xmm14,[rsp+00000010]
                    0x44, 0x0F, 0x10, 0xAC, 0x24, 0x20, 0x00, 0x00, 0x00, // movups xmm13,[rsp+00000020]
                    0x44, 0x0F, 0x10, 0xA4, 0x24, 0x30, 0x00, 0x00, 0x00, // movups xmm12,[rsp+00000030]
                    0x44, 0x0F, 0x10, 0x9C, 0x24, 0x40, 0x00, 0x00, 0x00, // movups xmm11,[rsp+00000040]
                    0x44, 0x0F, 0x10, 0x94, 0x24, 0x50, 0x00, 0x00, 0x00, // movups xmm10,[rsp+00000050]
                    0x44, 0x0F, 0x10, 0x8C, 0x24, 0x60, 0x00, 0x00, 0x00, // movups xmm9,[rsp+00000060]
                    0x44, 0x0F, 0x10, 0x84, 0x24, 0x70, 0x00, 0x00, 0x00, // movups xmm8,[rsp+00000070]
                    0x0F, 0x10, 0xBC, 0x24, 0x80, 0x00, 0x00, 0x00,       // movups xmm7,[rsp+00000080]
                    0x0F, 0x10, 0xB4, 0x24, 0x90, 0x00, 0x00, 0x00,       // movups xmm6,[rsp+00000090]
                    0x0F, 0x10, 0xAC, 0x24, 0xA0, 0x00, 0x00, 0x00,       // movups xmm5,[rsp+000000A0]
                    0x0F, 0x10, 0xA4, 0x24, 0xB0, 0x00, 0x00, 0x00,       // movups xmm4,[rsp+000000B0]
                    0x0F, 0x10, 0x9C, 0x24, 0xC0, 0x00, 0x00, 0x00,       // movups xmm3,[rsp+000000C0]
                    0x0F, 0x10, 0x94, 0x24, 0xD0, 0x00, 0x00, 0x00,       // movups xmm2,[rsp+000000D0]
                    0x0F, 0x10, 0x8C, 0x24, 0xE0, 0x00, 0x00, 0x00,       // movups xmm1,[rsp+000000E0]
                };
                Feed(buffer, sizeof(buffer));
            }
            if (noXMMResult) {
                constexpr byte8 buffer[] = {
                    0x0F, 0x10, 0x84, 0x24, 0xF0, 0x00, 0x00, 0x00, // movups xmm0,[rsp+000000F0]
                };
                Feed(buffer, sizeof(buffer));
            }
            {
                constexpr byte8 buffer[] = {
                    0x48, 0x81, 0xC4, 0x00, 0x01, 0x00, 0x00, // add rsp,00000100
                };
                Feed(buffer, sizeof(buffer));
            }
        }

        {
            // @Todo: Replace with macros.
            constexpr byte8 buffer[] = {
                0x9D,       // popfq
                0x41, 0x5F, // pop r15
                0x41, 0x5E, // pop r14
                0x41, 0x5D, // pop r13
                0x41, 0x5C, // pop r12
                0x41, 0x5B, // pop r11
                0x41, 0x5A, // pop r10
                0x41, 0x59, // pop r9
                0x41, 0x58, // pop r8
                0x5F,       // pop rdi
                0x5E,       // pop rsi
                0x5D,       // pop rbp
                0x5C,       // pop rsp
                0x5B,       // pop rbx
                0x5A,       // pop rdx
                0x59,       // pop rcx
            };
            Feed(buffer, sizeof(buffer));
        }

        if (noResult) {
            constexpr byte8 buffer[] = {
                0x58, // pop rax
            };
            Feed(buffer, sizeof(buffer));
        }
    }

    func.sect2 = (func.addr + pos);
    pos += size2;

    if (jumpAddr) {
        WriteJump((func.addr + pos), jumpAddr);

        pos += 5;
    } else {
        if (!noReturn) {
            constexpr byte8 buffer[] = {
                0xC3, // ret
            };
            Feed(buffer, sizeof(buffer));
        }
    }

    memoryData.pos += pos;

    if (cacheSize) {
        Align<offset_t>(memoryData.pos, 0x10);

        func.cache = reinterpret_cast<byte8**>(memoryData.dataAddr + memoryData.pos);

        memoryData.pos += cacheSize;
    }

    return func;
}

__declspec(deprecated) Function old_CreateFunction(void* funcAddr, byte8* jumpAddr, bool saveRegisters, bool noResult, new_size_t size0,
    new_size_t size1, new_size_t size2, new_size_t cacheSize, new_size_t count, bool noReturn, bool saveXMMRegisters, bool noXMMResult) {
    byte64 flags = 0;

    if (saveRegisters) {
        flags |= FunctionFlags_SaveRegisters;
    }

    if (noResult) {
        flags |= FunctionFlags_NoResult;
    }

    if (noReturn) {
        flags |= FunctionFlags_NoReturn;
    }

    if (saveXMMRegisters) {
        flags |= FunctionFlags_SaveXMMRegisters;
    }

    if (noXMMResult) {
        flags |= FunctionFlags_NoXMMResult;
    }

    return CreateFunction(funcAddr, jumpAddr, flags, size0, size1, size2, cacheSize, count);
}

#else

// @Update
Function CreateFunction(
    void* funcAddr, byte8* jumpAddr, byte32 flags, new_size_t size0, new_size_t size1, new_size_t size2, new_size_t cacheSize) {


    // @Remove
    const bool saveRegisters = (flags & FunctionFlags_SaveRegisters);
    const bool noResult      = (flags & FunctionFlags_NoResult);
    const bool noReturn      = (flags & FunctionFlags_NoReturn);


    Function func = {};
    offset_t pos  = 0;

    auto Feed = [&](const byte8* buffer, new_size_t bufferSize, bool adjustPosition = true) {
        CopyMemory((func.addr + pos), buffer, bufferSize);

        if (!adjustPosition) {
            return;
        }

        pos += bufferSize;
    };

    Align<offset_t>(memoryData.pos, 0x10);

    func.addr = (memoryData.dataAddr + memoryData.pos);

    func.sect0 = (func.addr + pos);
    pos += size0;

    if (saveRegisters) {
        if (noResult) {
            constexpr byte8 buffer[] = {
                0x50, // push eax
            };
            Feed(buffer, sizeof(buffer));
        }

        {
            constexpr byte8 buffer[] = {
                0x51, // push ecx
                0x52, // push edx
                0x53, // push ebx
                0x54, // push esp
                0x55, // push ebp
                0x56, // push esi
                0x57, // push edi
                0x9C, // pushfd
            };
            Feed(buffer, sizeof(buffer));
        }

        {
            constexpr byte8 buffer[] = {
                0x8B, 0xEC, // mov ebp,esp
            };
            Feed(buffer, sizeof(buffer));
        }

        {


            constexpr byte8 buffer[] = {
                0x81, 0xE4, 0xF0, 0xFF, 0xFF, 0xFF, // and esp,FFFFFFF0
            };


            // constexpr byte8 buffer[] =
            // {
            // 	0x81, 0xE4, 0xF0, 0x00, 0x00, 0x00, // and esp,000000F0
            // };
            Feed(buffer, sizeof(buffer));
        }
    }

    func.sect1 = (func.addr + pos);
    pos += size1;

    if (funcAddr) {
        constexpr byte8 buffer[] = {
            0xB8, 0x00, 0x00, 0x00, 0x00, // mov eax
        };
        Feed(buffer, sizeof(buffer), false);
        *reinterpret_cast<void**>(func.addr + pos + 1) = funcAddr;
        pos += sizeof(buffer);


        if (flags & FunctionFlags_Jump) {
            constexpr byte8 buffer[] = {
                0xFF, 0xE0, // jmp eax
            };
            Feed(buffer, sizeof(buffer));
        } else {
            constexpr byte8 buffer[] = {
                0xFF, 0xD0, // call eax
            };
            Feed(buffer, sizeof(buffer));
        }


        // // Not required!

        // if (argCount)
        // {
        // 	constexpr byte8 buffer[] =
        // 	{
        // 		0x83, 0xC4, 0x00, // add esp
        // 	};
        // 	Feed(buffer, sizeof(buffer), false);
        // 	*reinterpret_cast<uint8 *>(func.addr + pos + 2) = (argCount * 4);
        // 	pos += sizeof(buffer);
        // }
    }

    if (saveRegisters) {
        {
            constexpr byte8 buffer[] = {
                0x8B, 0xE5, // mov esp,ebp
            };
            Feed(buffer, sizeof(buffer));
        }

        {
            constexpr byte8 buffer[] = {
                0x9D, // popfd
                0x5F, // pop edi
                0x5E, // pop esi
                0x5D, // pop ebp
                0x5C, // pop esp
                0x5B, // pop ebx
                0x5A, // pop edx
                0x59, // pop ecx
            };
            Feed(buffer, sizeof(buffer));
        }

        if (noResult) {
            constexpr byte8 buffer[] = {
                0x58, // pop eax
            };
            Feed(buffer, sizeof(buffer));
        }
    }

    func.sect2 = (func.addr + pos);
    pos += size2;

    if (jumpAddr) {
        WriteJump((func.addr + pos), jumpAddr);

        pos += 5;
    } else {
        if (!noReturn) {
            constexpr byte8 buffer[] = {
                0xC3, // ret
            };
            Feed(buffer, sizeof(buffer));
        }
    }

    memoryData.pos += pos;

    if (cacheSize) {
        Align<offset_t>(memoryData.pos, 0x10);

        func.cache = reinterpret_cast<byte8**>(memoryData.dataAddr + memoryData.pos);

        memoryData.pos += cacheSize;
    }

    return func;
}

// @Remove
__declspec(deprecated) Function old_CreateFunction(void* funcAddr = 0, byte8* jumpAddr = 0, bool saveRegisters = true, bool noResult = true,
    new_size_t size0 = 0, new_size_t size1 = 0, new_size_t size2 = 0, new_size_t cacheSize = 0, bool noReturn = true) {
    byte32 flags = 0;

    if (saveRegisters) {
        flags |= FunctionFlags_SaveRegisters;
    }

    if (noResult) {
        flags |= FunctionFlags_NoResult;
    }

    if (noReturn) {
        flags |= FunctionFlags_NoReturn;
    }

    return CreateFunction(funcAddr, jumpAddr, flags, size0, size1, size2, cacheSize);
}

#endif

#pragma endregion

bool Core_Memory_Init() {
    LogFunction();

    MODULEENTRY32 moduleEntry = {};
    HANDLE snapshot           = 0;

    moduleEntry.dwSize = sizeof(moduleEntry);
    snapshot           = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
    Module32First(snapshot, &moduleEntry);

    appBaseAddr = moduleEntry.modBaseAddr;
    appSize     = moduleEntry.modBaseSize;

    Log("%u %s", moduleEntry.th32ProcessID, moduleEntry.szModule);


#ifdef _WIN64
    Log("appStart %llX", appBaseAddr);
    Log("appEnd   %llX", (appBaseAddr + appSize));
#else
    Log("appStart %X", appBaseAddr);
    Log("appEnd   %X", (appBaseAddr + appSize));
#endif


    Log("appSize %X", appSize);

    GetSystemInfo(&systemInfo);

    return true;
}

#pragma endregion

#pragma region String Stuff

int32 IndexOf(const char* name, const char* name2) {
    auto count  = static_cast<int32>(strlen(name));
    auto count2 = static_cast<int32>(strlen(name2));

    if ((count < 1) || (count2 < 1) || (count < count2)) {
        return -1;
    }


    int32 pos = 0;

    while (pos < count) {
        if ((count - pos) < count2) {
            break;
        }

        auto dest = (reinterpret_cast<byte8*>(const_cast<char*>(name)) + pos);

        if (memcmp(dest, name2, count2) == 0) {
            return pos;
        }

        pos++;
    }


    return -1;
}

#pragma endregion

#pragma region Windows

void Windows_ToggleCursor(bool enable) {
    int32 counter = ShowCursor(false);
    if (counter < 0) {
        do {
            continue;
        } while (ShowCursor(true) < 0);
    } else {
        do {
            continue;
        } while (ShowCursor(false) > 0);
    }
    if (enable) {
        ShowCursor(true);
    } else {
        ShowCursor(false);
    }
}

// export bool Windows_GetWindowPos(HWND window, POINT * point)
// {
// 	if (!window)
// 	{
// 		return false;
// 	}
// 	RECT rect = {};
// 	if (!GetWindowRect(window, &rect))
// 	{
// 		return false;
// 	}
// 	point->x = rect.left;
// 	point->y = rect.top;
// 	return true;
// }

// POINT Windows_GetWindowSize(HWND windowHandle)
// {
// 	RECT rect = {};

// 	GetWindowRect
// 	(
// 		windowHandle,
// 		&rect
// 	);

// 	POINT point = {};

// 	point.x = (rect.right - rect.left);
// 	point.y = (rect.bottom - rect.top);

// 	return point;
// }

// POINT Windows_GetClientSize(HWND windowHandle)
// {
// 	RECT rect = {};

// 	GetClientRect
// 	(
// 		windowHandle,
// 		&rect
// 	);

// 	POINT point = {};

// 	point.x = rect.right;
// 	point.y = rect.bottom;

// 	return point;
// }

// POINT Windows_GetWindowSize(HWND windowHandle)
// {
// 	POINT size = {};

// 	RECT rect = {};

// 	GetWindowRect
// 	(
// 		windowHandle,
// 		&rect
// 	);

// }

// export bool IsBorderless(HWND windowHandle)
// {
// 	auto style = GetWindowLongA
// 	(
// 		windowHandle,
// 		GWL_STYLE
// 	);

// 	if (style & WS_BORDER)
// 	{
// 		return false;
// 	}

// 	return true;
// }

// @Todo: Update for RVO.
void GetWindowPos(HWND window, POINT& point) {
    if (!window) {
        return;
    }

    RECT rect = {};

    GetWindowRect(window, &rect);

    point.x = rect.left;
    point.y = rect.top;
}

#pragma endregion

#pragma region Global

vec2 g_windowSize = {};
vec2 g_clientSize = {};
vec2 g_renderSize = {};

float g_frameRateMultiplier = 1.0f;

POINT GetWindowSize(HWND windowHandle) {
    POINT point = {};
    RECT rect   = {};

    if (!windowHandle) {
        return point;
    }

    GetWindowRect(windowHandle, &rect);

    point.x = (rect.right - rect.left);
    point.y = (rect.bottom - rect.top);

    return point;
}

POINT GetClientSize(HWND windowHandle) {
    POINT point = {};
    RECT rect   = {};
    RECT rect2  = {};

    if (!windowHandle) {
        return point;
    }

    GetWindowRect(appWindow, &rect);

    auto style = GetWindowLongA(appWindow, GWL_STYLE);

    AdjustWindowRect(&rect2, style, 0);

    rect.left -= rect2.left;
    rect.top -= rect2.top;
    rect.right -= rect2.right;
    rect.bottom -= rect2.bottom;

    point.x = (rect.right - rect.left);
    point.y = (rect.bottom - rect.top);

    return point;
}

void UpdateGlobalWindowSize() {
    if (!appWindow) {
        return;
    }

    auto size = GetWindowSize(appWindow);

    g_windowSize = {static_cast<float>(size.x), static_cast<float>(size.y)};

    Log("%s %g %g", FUNC_NAME, g_windowSize.x, g_windowSize.y);
}

void UpdateGlobalClientSize() {
    if (!appWindow) {
        return;
    }

    auto size = GetClientSize(appWindow);

    g_clientSize = {static_cast<float>(size.x), static_cast<float>(size.y)};

    Log("%s %g %g", FUNC_NAME, g_clientSize.x, g_clientSize.y);
}

void UpdateGlobalRenderSize(uint32 width, uint32 height) {
    g_renderSize = {static_cast<float>(width), static_cast<float>(height)};

    Log("%s %g %g", FUNC_NAME, g_renderSize.x, g_renderSize.y);
}

#pragma endregion

namespace DI8 {
const char* keyNames[] = {
    "UNKNOWN_0",
    "ESCAPE",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "0",
    "MINUS",
    "EQUALS",
    "BACKSPACE",
    "TAB",
    "Q",
    "W",
    "E",
    "R",
    "T",
    "Y",
    "U",
    "I",
    "O",
    "P",
    "LEFT_BRACKET",
    "RIGHT_BRACKET",
    "ENTER",
    "LEFT_CONTROL",
    "A",
    "S",
    "D",
    "F",
    "G",
    "H",
    "J",
    "K",
    "L",
    "SEMICOLON",
    "APOSTROPHE",
    "TILDE",
    "LEFT_SHIFT",
    "BACKSLASH",
    "Z",
    "X",
    "C",
    "V",
    "B",
    "N",
    "M",
    "COMMA",
    "PERIOD",
    "SLASH",
    "RIGHT_SHIFT",
    "NUMPAD_MULTIPLY",
    "LEFT_ALT",
    "SPACE",
    "CAPSLOCK",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "NUMLOCK",
    "SCROLLLOCK",
    "NUMPAD_7",
    "NUMPAD_8",
    "NUMPAD_9",
    "NUMPAD_SUB",
    "NUMPAD_4",
    "NUMPAD_5",
    "NUMPAD_6",
    "NUMPAD_ADD",
    "NUMPAD_1",
    "NUMPAD_2",
    "NUMPAD_3",
    "NUMPAD_0",
    "NUMPAD_DECIMAL",
    "UNKNOWN_84",
    "UNKNOWN_85",
    "UNKNOWN_86",
    "F11",
    "F12",
    "UNKNOWN_89",
    "UNKNOWN_90",
    "UNKNOWN_91",
    "UNKNOWN_92",
    "UNKNOWN_93",
    "UNKNOWN_94",
    "UNKNOWN_95",
    "UNKNOWN_96",
    "UNKNOWN_97",
    "UNKNOWN_98",
    "UNKNOWN_99",
    "UNKNOWN_100",
    "UNKNOWN_101",
    "UNKNOWN_102",
    "UNKNOWN_103",
    "UNKNOWN_104",
    "UNKNOWN_105",
    "UNKNOWN_106",
    "UNKNOWN_107",
    "UNKNOWN_108",
    "UNKNOWN_109",
    "UNKNOWN_110",
    "UNKNOWN_111",
    "UNKNOWN_112",
    "UNKNOWN_113",
    "UNKNOWN_114",
    "UNKNOWN_115",
    "UNKNOWN_116",
    "UNKNOWN_117",
    "UNKNOWN_118",
    "UNKNOWN_119",
    "UNKNOWN_120",
    "UNKNOWN_121",
    "UNKNOWN_122",
    "UNKNOWN_123",
    "UNKNOWN_124",
    "UNKNOWN_125",
    "UNKNOWN_126",
    "UNKNOWN_127",
    "UNKNOWN_128",
    "UNKNOWN_129",
    "UNKNOWN_130",
    "UNKNOWN_131",
    "UNKNOWN_132",
    "UNKNOWN_133",
    "UNKNOWN_134",
    "UNKNOWN_135",
    "UNKNOWN_136",
    "UNKNOWN_137",
    "UNKNOWN_138",
    "UNKNOWN_139",
    "UNKNOWN_140",
    "UNKNOWN_141",
    "UNKNOWN_142",
    "UNKNOWN_143",
    "UNKNOWN_144",
    "UNKNOWN_145",
    "UNKNOWN_146",
    "UNKNOWN_147",
    "UNKNOWN_148",
    "UNKNOWN_149",
    "UNKNOWN_150",
    "UNKNOWN_151",
    "UNKNOWN_152",
    "UNKNOWN_153",
    "UNKNOWN_154",
    "UNKNOWN_155",
    "UNKNOWN_156",
    "RIGHT_CONTROL",
    "UNKNOWN_158",
    "UNKNOWN_159",
    "UNKNOWN_160",
    "UNKNOWN_161",
    "UNKNOWN_162",
    "UNKNOWN_163",
    "UNKNOWN_164",
    "UNKNOWN_165",
    "UNKNOWN_166",
    "UNKNOWN_167",
    "UNKNOWN_168",
    "UNKNOWN_169",
    "UNKNOWN_170",
    "UNKNOWN_171",
    "UNKNOWN_172",
    "UNKNOWN_173",
    "UNKNOWN_174",
    "UNKNOWN_175",
    "UNKNOWN_176",
    "UNKNOWN_177",
    "UNKNOWN_178",
    "UNKNOWN_179",
    "UNKNOWN_180",
    "NUMPAD_DIVIDE",
    "UNKNOWN_182",
    "PRINT",
    "RIGHT_ALT",
    "UNKNOWN_185",
    "UNKNOWN_186",
    "UNKNOWN_187",
    "UNKNOWN_188",
    "UNKNOWN_189",
    "UNKNOWN_190",
    "UNKNOWN_191",
    "UNKNOWN_192",
    "UNKNOWN_193",
    "UNKNOWN_194",
    "UNKNOWN_195",
    "UNKNOWN_196",
    "PAUSE",
    "UNKNOWN_198",
    "HOME",
    "UP",
    "PAGE_UP",
    "UNKNOWN_202",
    "LEFT",
    "UNKNOWN_204",
    "RIGHT",
    "UNKNOWN_206",
    "END",
    "DOWN",
    "PAGE_DOWN",
    "INSERT",
    "DELETE",
    "UNKNOWN_212",
    "UNKNOWN_213",
    "UNKNOWN_214",
    "UNKNOWN_215",
    "UNKNOWN_216",
    "UNKNOWN_217",
    "UNKNOWN_218",
    "WIN",
    "UNKNOWN_220",
    "UNKNOWN_221",
    "UNKNOWN_222",
    "UNKNOWN_223",
    "UNKNOWN_224",
    "UNKNOWN_225",
    "UNKNOWN_226",
    "UNKNOWN_227",
    "UNKNOWN_228",
    "UNKNOWN_229",
    "UNKNOWN_230",
    "UNKNOWN_231",
    "UNKNOWN_232",
    "UNKNOWN_233",
    "UNKNOWN_234",
    "UNKNOWN_235",
    "UNKNOWN_236",
    "UNKNOWN_237",
    "UNKNOWN_238",
    "UNKNOWN_239",
    "UNKNOWN_240",
    "UNKNOWN_241",
    "UNKNOWN_242",
    "UNKNOWN_243",
    "UNKNOWN_244",
    "UNKNOWN_245",
    "UNKNOWN_246",
    "UNKNOWN_247",
    "UNKNOWN_248",
    "UNKNOWN_249",
    "UNKNOWN_250",
    "UNKNOWN_251",
    "UNKNOWN_252",
    "UNKNOWN_253",
    "UNKNOWN_254",
    "UNKNOWN_255",
};
}
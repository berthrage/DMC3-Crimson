// Deserpente
#include "Graphics.hpp"
#include "Core/Core.hpp"
#include "Config.hpp"
#include "Global.hpp"
#include "Vars.hpp"
#include "Global/HooksBase.hpp"
#include "Utility/Detour.hpp"

#include <memory>

#include "Core/Macros.h"

#include "Core/DebugSwitch.hpp"
#include "../ThirdParty/ImGui/imgui.h"


double defaultFrequency = 0;
double* frequencyAddr   = 0;
int32* frameRateAddr    = 0;


void ToggleFrameRateFixes(bool enable) {
    LogFunction(enable);

    static bool run = false;


#pragma region movsx eax

    {
        auto addr             = (appBaseAddr + 0x4F22E);
        constexpr uint64 size = 7;
        /*
        dmc3.exe+4F22E - 0FBE 83 8D020000 - movsx eax,byte ptr [rbx+0000028D]
        dmc3.exe+4F235 - FF 83 90020000   - inc [rbx+00000290]
        */

        constexpr byte8 sect0[] = {
            0x8B, 0x05, 0x00, 0x00, 0x00, 0x00, // mov eax,[]
        };

        if (!run) {
            backupHelper.Save(addr, size);
        }

        if (enable) {
            SetMemory(addr, 0x90, size, MemoryFlags_VirtualProtectDestination);
            CopyMemory(addr, sect0, sizeof(sect0), MemoryFlags_VirtualProtectDestination);
            WriteAddress(addr, frameRateAddr, 6);
        } else {
            backupHelper.Restore(addr);
        }
    }

    {
        auto addr             = (appBaseAddr + 0x23D39E);
        constexpr uint64 size = 7;
        /*
        dmc3.exe+23D39E - 0FBE 05 5828A500 - movsx eax,byte ptr [dmc3.exe+C8FBFD]
        dmc3.exe+23D3A5 - 48 8B 57 10      - mov rdx,[rdi+10]
        */

        constexpr byte8 sect0[] = {
            0x8B, 0x05, 0x00, 0x00, 0x00, 0x00, // mov eax,[]
        };

        if (!run) {
            backupHelper.Save(addr, size);
        }

        if (enable) {
            SetMemory(addr, 0x90, size, MemoryFlags_VirtualProtectDestination);
            CopyMemory(addr, sect0, sizeof(sect0), MemoryFlags_VirtualProtectDestination);
            WriteAddress(addr, frameRateAddr, 6);
        } else {
            backupHelper.Restore(addr);
        }
    }

#pragma endregion

#pragma region movsx ecx

    {
        auto addr             = (appBaseAddr + 0x2A284D);
        constexpr uint64 size = 7;
        /*
        dmc3.exe+2A284D - 0FBE 0D A9D39E00     - movsx ecx,byte ptr [dmc3.exe+C8FBFD]
        dmc3.exe+2A2854 - 4C 89 A4 24 A0000000 - mov [rsp+000000A0],r12
        */

        constexpr byte8 sect0[] = {
            0x8B, 0x0D, 0x00, 0x00, 0x00, 0x00, // mov ecx,[]
        };

        if (!run) {
            backupHelper.Save(addr, size);
        }

        if (enable) {
            SetMemory(addr, 0x90, size, MemoryFlags_VirtualProtectDestination);
            CopyMemory(addr, sect0, sizeof(sect0), MemoryFlags_VirtualProtectDestination);
            WriteAddress(addr, frameRateAddr, 6);
        } else {
            backupHelper.Restore(addr);
        }
    }

    {
        auto addr             = (appBaseAddr + 0x2A5504);
        constexpr uint64 size = 7;
        /*
        dmc3.exe+2A5504 - 0FBE 0D F2A69E00 - movsx ecx,byte ptr [dmc3.exe+C8FBFD]
        dmc3.exe+2A550B - 33 D2            - xor edx,edx
        */

        constexpr byte8 sect0[] = {
            0x8B, 0x0D, 0x00, 0x00, 0x00, 0x00, // mov ecx,[]
        };

        if (!run) {
            backupHelper.Save(addr, size);
        }

        if (enable) {
            SetMemory(addr, 0x90, size, MemoryFlags_VirtualProtectDestination);
            CopyMemory(addr, sect0, sizeof(sect0), MemoryFlags_VirtualProtectDestination);
            WriteAddress(addr, frameRateAddr, 6);
        } else {
            backupHelper.Restore(addr);
        }
    }

    {
        auto addr             = (appBaseAddr + 0x32B23D);
        constexpr uint64 size = 7;
        /*
        dmc3.exe+32B23D - 0FBE 0D B9499600 - movsx ecx,byte ptr [dmc3.exe+C8FBFD]
        dmc3.exe+32B244 - 8B 05 46932300   - mov eax,[dmc3.exe+564590]
        */

        constexpr byte8 sect0[] = {
            0x8B, 0x0D, 0x00, 0x00, 0x00, 0x00, // mov ecx,[]
        };

        if (!run) {
            backupHelper.Save(addr, size);
        }

        if (enable) {
            SetMemory(addr, 0x90, size, MemoryFlags_VirtualProtectDestination);
            CopyMemory(addr, sect0, sizeof(sect0), MemoryFlags_VirtualProtectDestination);
            WriteAddress(addr, frameRateAddr, 6);
        } else {
            backupHelper.Restore(addr);
        }
    }

#pragma endregion

#pragma region movsx edx

    {
        auto addr             = (appBaseAddr + 0x3297EB);
        constexpr uint64 size = 7;
        /*
        dmc3.exe+3297EB - 0FBE 15 0B649600 - movsx edx,byte ptr [dmc3.exe+C8FBFD]
        dmc3.exe+3297F2 - 4C 8B C0         - mov r8,rax
        */

        constexpr byte8 sect0[] = {
            0x8B, 0x15, 0x00, 0x00, 0x00, 0x00, // mov edx,[]
        };

        if (!run) {
            backupHelper.Save(addr, size);
        }

        if (enable) {
            SetMemory(addr, 0x90, size, MemoryFlags_VirtualProtectDestination);
            CopyMemory(addr, sect0, sizeof(sect0), MemoryFlags_VirtualProtectDestination);
            WriteAddress(addr, frameRateAddr, 6);
        } else {
            backupHelper.Restore(addr);
        }
    }

#pragma endregion

#pragma region movsx r10d
    // Serp was trying to fix Secret Mission timers here but it was not working. 
    // We fix it through our detours instead (which are prolly not even needed).
#pragma endregion


    run = true;
}


namespace Graphics {
void Toggle(bool enable) {
    LogFunction(enable);

    static bool run = false;


    if (!run) {
        defaultFrequency = *reinterpret_cast<double*>(appBaseAddr + 0x505E38);
        /*
        dmc3.exe+2C5E97 - F2 0F10 3D 99FF2300 - movsd xmm7,[dmc3.exe+505E38]
        dmc3.exe+2C5E9F - 48 89 05 4A2A9E00   - mov [dmc3.exe+CA88F0],rax
        */

        frequencyAddr = reinterpret_cast<double*>(HighAlloc(8));
        if (!frequencyAddr) {
            Log("HighAlloc failed.");

            return;
        }

        *frequencyAddr = defaultFrequency;

        frameRateAddr = reinterpret_cast<int32*>(HighAlloc(4));
        if (!frameRateAddr) {
            Log("HighAlloc failed.");

            return;
        }

        *frameRateAddr = 60;
    }


    // Remove framerate lock
    // Credits to mos9527 for this research
    {
        auto addr = (appBaseAddr + 0x2C5EB0);
        auto jumpAddr = (appBaseAddr + 0x2C5F1D);
        auto destAddr = (appBaseAddr + 0x2C5F29);
        constexpr uint64 size = 2;
        /*
        dmc3.exe+2C5F1D - 76 0A - jbe dmc3.exe+2C5F29
        */

        static Function func = {};

        constexpr byte8 sect0[] = {
            0xEB, 0x0A, // jmp dmc3.exe+2C5F29
        };

        if (!run) {
            backupHelper.Save(jumpAddr, size);
        }

        if (enable) {
            WriteShortJump(jumpAddr, destAddr);
        }
        else {
            backupHelper.Restore(jumpAddr);
        }
    }


    {
        auto addr             = (appBaseAddr + 0x2C5EB0);
        auto jumpAddr         = (appBaseAddr + 0x2C5EB5);
        constexpr uint64 size = 5;
        /*
        dmc3.exe+2C5EB0 - 48 8D 4C 24 20 - lea rcx,[rsp+20]
        dmc3.exe+2C5EB5 - FF 15 1D930800 - call qword ptr [dmc3.exe+34F1D8]
        */

        static Function func = {};

        constexpr byte8 sect0[] = {
            0xF2, 0x0F, 0x10, 0x3D, 0x00, 0x00, 0x00, 0x00, // movsd xmm7,[]
        };

        if (!run) {
            backupHelper.Save(addr, size);
            func = old_CreateFunction(0, jumpAddr, false, true, (sizeof(sect0) + size));
            CopyMemory(func.sect0, sect0, sizeof(sect0));
            CopyMemory((func.sect0 + sizeof(sect0)), addr, size, MemoryFlags_VirtualProtectSource);
            WriteAddress(func.sect0, frequencyAddr, 8);
        }

        if (enable) {
            WriteJump(addr, func.addr, (size - 5));
        } else {
            backupHelper.Restore(addr);
        }
    }


    ToggleFrameRateFixes(enable);


    run = true;
}

}; // namespace Graphics


namespace {

constexpr uintptr_t UPLOAD_VERTEX_STREAM_OFFSET = 0x43190;
constexpr uintptr_t VERTEX_RING_UNITS_IMM_OFFSET = 0x42DED;
constexpr uint8_t VERTEX_RING_UNITS_ORIGINAL = 0x28;
constexpr uint8_t VERTEX_RING_UNITS_PATCHED = 8;

typedef void(__fastcall* UploadVertexStreamFn)(__int64, int, int*, unsigned int);
std::unique_ptr<Utility::Detour_t> s_uploadVertexStreamHook;

// UploadVertexStream_sub_140043190: the vertex ring has no bounds check and is shrunk at init (see
// Graphics::InitVertexRing), so drop a draw that would run past the end instead of writing out of the mapping.
void __fastcall UploadVertexStreamHook(__int64 render, int flags, int* data, unsigned int count) {
	const auto original = s_uploadVertexStreamHook->GetTrampoline<UploadVertexStreamFn>();

	unsigned int stride = ((flags & 0x40000002) == 0x40000002) ? 0x10 : 0xC;
	if (flags & 0x100) stride += 8;
	if (flags & 0x40) stride += 0x10;
	if (flags & 0x20000) stride += 4;
	const unsigned long long capacity = 0x18000ull * *reinterpret_cast<unsigned long long*>(render + 0x1798);
	const unsigned long long used = *reinterpret_cast<unsigned long long*>(render + 0x17A0);
	if (used + (unsigned long long)stride * count > capacity) {
		static bool s_logged = false;
		if (!s_logged) {
			s_logged = true;
			Log("Vertex ring overflow: %llu + %u x %u > %llu, draw skipped", used, stride, count, capacity);
		}
		return;
	}

	if (!kPerfProbes) {
		original(render, flags, data, count);
		return;
	}

	const long long offsetBefore = (long long)used;
	LARGE_INTEGER start, end;
	QueryPerformanceCounter(&start);
	original(render, flags, data, count);
	QueryPerformanceCounter(&end);

	const long long ticks = end.QuadPart - start.QuadPart;
	g_perfUploadTicks += ticks;
	if (offsetBefore == 0) g_perfDiscardTicks += ticks;
	g_perfVerts += count;
	g_perfRingUnits = *reinterpret_cast<long long*>(render + 0x1798);
	const long long offsetAfter = *reinterpret_cast<long long*>(render + 0x17A0);
	if (offsetAfter > g_perfPeakBytes) g_perfPeakBytes = offsetAfter;
	++g_perfDrawCount;
}

} // namespace

namespace Graphics {

// Must run before the D3D11 device is created: the ring buffers are sized from this immediate.
void InitVertexRing() {
	// SwapChainCreation_sub_140042890: vertex ring size in 0x18000 units (mov qword [r14+1798h], 28h)
	auto* units = reinterpret_cast<uint8_t*>(appBaseAddr + VERTEX_RING_UNITS_IMM_OFFSET);
	if (*units == VERTEX_RING_UNITS_ORIGINAL) {
		uint8_t newUnits = VERTEX_RING_UNITS_PATCHED;
		_patch(reinterpret_cast<char*>(units), reinterpret_cast<char*>(&newUnits), 1);
	}

	s_uploadVertexStreamHook = std::make_unique<Utility::Detour_t>(
		(uintptr_t)appBaseAddr + UPLOAD_VERTEX_STREAM_OFFSET, &UploadVertexStreamHook, 0, "UploadVertexStream_sub_140043190");
	s_uploadVertexStreamHook->Toggle(true);
}

} // namespace Graphics


void UpdateFrameRate() {
	auto pool_10298 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
	if (!pool_10298 || !pool_10298[8]) {
		return;
	}
	auto& eventData = *reinterpret_cast<EventData*>(pool_10298[8]);

	// Use ONLY measured frame rate
	float runtimeFrameRate = g_FrameRate;

	if (runtimeFrameRate <= 1.0f) {
		runtimeFrameRate = 60.0f;
	}

	g_frameRateMultiplier = (60.0f / runtimeFrameRate);

	// Keep frequency neutral
    //*frequencyAddr = (defaultFrequency * static_cast<double>(60 / activeCrimsonConfig.System.fpsCap));
    *frequencyAddr = defaultFrequency;

	// Update mission frame counter rate
	*frameRateAddr = static_cast<int32>(runtimeFrameRate + 0.5f);
}

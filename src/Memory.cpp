// @Todo: Review.
// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "Memory.hpp"
#include <cstring>
#include "CrimsonConfig.hpp"
#include "File.hpp"
#include "Core/Core.hpp"
#include "Vars.hpp"

#include "Core/Macros.h"

constexpr uint32 vectorItemCount = 4096;

// @Todo: Rename.
byte8* Memory_main = 0;

byte8* effectManagerData = 0;
byte8* valueManagerData  = 0;

auto GetMemoryAddress() {
    return Memory_main;
}

// Game file slots (sub_140048B20): 100 slots of 0x20 bytes at dmc3.exe+C18E60, byte +0x18 = in use. When all are taken every
// file open fails, which makes menu resource requests return NULL.
void Memory_LogFileSlots(bool always) {
    static int32 lastUsed = -1;
    auto table            = appBaseAddr + 0xC18E60;
    int32 used            = 0;
    for (uint32 slot = 0; slot < 100; slot++) {
        if (table[(slot * 0x20) + 0x18]) {
            used++;
        }
    }
    if (!always && used == lastUsed) {
        return;
    }
    lastUsed = used;
    Log("File slots used %d/100", used);
    if (always) {
        for (uint32 slot = 0; slot < 100; slot++) {
            auto entry = table + (slot * 0x20);
            if (entry[0x18]) {
                Log("  slot %u handle %llX object %llX +10 %llX", slot, *reinterpret_cast<uint64*>(entry), *reinterpret_cast<uint64*>(entry + 8),
                    *reinterpret_cast<uint64*>(entry + 0x10));
            }
        }
    }
}

// Arena usage (CKernelManagedArena_t): used blocks and the largest free run, i.e. the biggest allocation that can still succeed.
static void LogArena(const char* name, byte8* arena) {
    auto memory     = *reinterpret_cast<byte8**>(arena);
    auto blockCount = *reinterpret_cast<int32*>(arena + 0x10);
    auto blockSize  = *reinterpret_cast<int32*>(arena + 0x14);
    if (!memory || blockCount <= 0) {
        Log("  %s: not initialised", name);
        return;
    }
    uint32 used = 0, run = 0, best = 0;
    for (int32 block = 0; block < blockCount; block++) {
        if (memory[block]) {
            used++;
            run = 0;
        } else if (++run > best) {
            best = run;
        }
    }
    Log("  %s: %u/%d blocks used, largest free run %u blocks = %llu bytes (block size %X)", name, used, blockCount, best,
        static_cast<uint64>(best) * blockSize, blockSize);
}

// File read queues (EnqueueFileRead_sub_1402EF4D0 / ProcessFileReadQueue_sub_1402EF790), table at +CAF820, 0x18 bytes per queue:
// +0x10 active, +0x11 progress, +0x14 head, +0x18 tail, +0x1C ring size, +0x20 ring of 0x88 byte entries (state dword at +0, path at +0x10).
// An enqueue fails when the entry at the tail still has state > 0.
static void LogReadQueues() {
    for (uint32 queueIndex = 0; queueIndex < 2; queueIndex++) {
        auto queue = appBaseAddr + 0xCAF820 + (queueIndex * 0x18);
        auto ring  = *reinterpret_cast<byte8**>(queue + 0x20);
        auto size  = *reinterpret_cast<int32*>(queue + 0x1C);
        auto head  = *reinterpret_cast<int32*>(queue + 0x14);
        auto tail  = *reinterpret_cast<int32*>(queue + 0x18);
        Log("  read queue %u: active %u progress %u head %d tail %d size %d ring %llX", queueIndex, queue[0x10], queue[0x11], head, tail, size,
            reinterpret_cast<uint64>(ring));
        if (!ring || size <= 0 || size > 0x1000) {
            continue;
        }
        uint32 busy = 0;
        for (int32 entry = 0; entry < size; entry++) {
            if (*reinterpret_cast<int32*>(ring + (entry * 0x88)) > 0) {
                busy++;
            }
        }
        Log("  read queue %u: %u/%d entries busy", queueIndex, busy, size);
        for (int32 entry : {head, tail}) {
            if (entry >= 0 && entry < size) {
                auto data = ring + (entry * 0x88);
                Log("    entry %d state %d path %.40s", entry, *reinterpret_cast<int32*>(data), reinterpret_cast<const char*>(data + 0x10));
            }
        }
    }
}

static void LogHeapArenas() {
    LogArena("heap arena 0", appBaseAddr + 0xCA8910);
    LogArena("heap arena 1", appBaseAddr + 0xCA8938);
    LogArena("heap arena 2", appBaseAddr + 0xCA8960);
    // Resource request table (dmc3.exe+C99D30): scratch arena at +6718, scratch mode flag at +6760.
    auto table = appBaseAddr + 0xC99D30;
    Log("  resource table scratch mode %u", *reinterpret_cast<uint8*>(table + 0x6760));
    LogArena("pac scratch arena", table + 0x6718);

    LogReadQueues();

    // Resource request slots (0x48 bytes, first index word at +581A2A, count word at +581A1A): state +4, entry +0x18 (path pointer at entry+8),
    // buffer +0x20, heap handle +0x28 (arena at +0x30, data +0x38, first block +0x40, block count +0x44).
    auto start = *reinterpret_cast<uint16*>(appBaseAddr + 0x581A2A);
    auto count = *reinterpret_cast<uint16*>(appBaseAddr + 0x581A1A);
    Log("  request slots start %u count %u", start, count);
    uint32 shown = 0;
    bool probed  = false;
    for (uint32 index = start; index < static_cast<uint32>(start + count) && shown < 40; index++) {
        auto slot  = table + (index * 0x48);
        auto state = *reinterpret_cast<uint32*>(slot + 4);
        auto entry = *reinterpret_cast<byte8**>(slot + 0x18);
        if (!state && !entry) {
            continue;
        }
        const char* name = "?";
        if (entry >= appBaseAddr && entry < (appBaseAddr + appSize)) {
            auto path = *reinterpret_cast<const char**>(entry + 8);
            if (reinterpret_cast<byte8*>(const_cast<char*>(path)) >= appBaseAddr && reinterpret_cast<byte8*>(const_cast<char*>(path)) < (appBaseAddr + appSize)) {
                name = path;
            }
        }
        Log("  slot %u state %u flags %X entry %llX %s buffer %llX handle arena %llX data %llX first %d count %d", index, state, *reinterpret_cast<uint16*>(slot + 8),
            reinterpret_cast<uint64>(entry), name, *reinterpret_cast<uint64*>(slot + 0x20), *reinterpret_cast<uint64*>(slot + 0x30),
            *reinterpret_cast<uint64*>(slot + 0x38), *reinterpret_cast<int32*>(slot + 0x40), *reinterpret_cast<int32*>(slot + 0x44));
        shown++;
        if (!state && name[0] != '?' && !probed) {
            // Ask the game's own file size lookup (GetFileSizeInSectors_sub_1402EF620) about the path that failed. 0 = it cannot open it.
            probed         = true;
            auto fileSize  = reinterpret_cast<uint32(__fastcall*)(void*, const char*)>(appBaseAddr + 0x2EF620)(appBaseAddr + 0xCAF820, name);
            Log("  probe: GetFileSizeInSectors('%s') = %u", name, fileSize);
        }
    }
}

// Option screens: a menu resource request (RequestMenuResource_sub_1401B8EA0) returned NULL and its result is dereferenced without a check.
// dmc3.exe+1C83B1 - mov eax,[rcx+04] (a1+0x58, id\id400\id450.pac) and dmc3.exe+1C8FF0 - mov ecx,[rax+04] (a1+0x40, id\id400\id430.pac)
// Log only: the crash itself is left to happen.
static LONG CALLBACK MenuResourceGuard(PEXCEPTION_POINTERS info) {
    auto ctx = info->ContextRecord;
    auto rip = ctx->Rip - reinterpret_cast<DWORD64>(appBaseAddr);
    if (info->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION || (rip != 0x1C83B1 && rip != 0x1C8FF0)) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    static bool logged = false;
    if (!logged) {
        logged = true;
        Log("MenuResourceGuard: dmc3.exe+%llX option screen %llX state %u has no resource", rip, ctx->Rbx, *reinterpret_cast<uint32*>(ctx->Rbx + 4));
        LogHeapArenas();
        Memory_LogFileSlots(true);
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

// SubmitResourceLoad_sub_1401B84E0 returning false makes AllocResourceRequestSlot_sub_1401B8DF0 return NULL (the option screen crash).
// This wrapper logs that moment: requested path, the allocation size the game computed, the slot's heap handle before and after, and the arenas.
// dmc3.exe+1B8E4A - E8 91 F6 FF FF - call dmc3.exe+1B84E0
static uint64 LoggedSubmitResourceLoad(byte8* table, byte8* slot, byte8* entry, uint64 arg4, uint64 arg5) {
    auto handleArenaBefore = *reinterpret_cast<uint64*>(slot + 0x30);
    auto result            = reinterpret_cast<uint64 (*)(byte8*, byte8*, byte8*, uint64, uint64)>(appBaseAddr + 0x1B84E0)(table, slot, entry, arg4, arg5);
    static uint32 count    = 0;
    if (!(result & 0xFF) && count++ < 16) {
        auto path = *reinterpret_cast<const char**>(entry + 8);
        auto size = reinterpret_cast<uint32 (*)(byte8*, byte8*)>(appBaseAddr + 0x1B7B90)(table, slot);
        auto exists = reinterpret_cast<int32 (*)(byte8*, byte8*)>(appBaseAddr + 0x1B79E0)(table, slot);
        Log("SubmitResourceLoad failed: '%s' alloc size %u exists %d scratch mode %u buffer %llX handle arena before %llX after %llX heap flags %u %u %u", path, size,
            exists, table[0x6760], *reinterpret_cast<uint64*>(slot + 0x20), handleArenaBefore, *reinterpret_cast<uint64*>(slot + 0x30), appBaseAddr[0xCA8AB0],
            appBaseAddr[0xCA8AB1], appBaseAddr[0xCA8AB2]);
        LogHeapArenas();
        LogReadQueues();
    }
    return result;
}

static void HookSubmitResourceLoad() {
    auto thunk = HighAlloc(16);
    if (!thunk) {
        Log("HookSubmitResourceLoad: HighAlloc failed.");
        return;
    }
    // mov rax,imm64 / jmp rax
    byte8 code[] = {0x48, 0xB8, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xE0};
    auto target  = reinterpret_cast<uint64>(&LoggedSubmitResourceLoad);
    CopyMemory(code + 2, &target, 8);
    CopyMemory(thunk, code, sizeof(code));
    WriteCall((appBaseAddr + 0x1B8E4A), thunk);
    Log("HookSubmitResourceLoad installed");
}

// Render handle dispatch (sub_14002CC10): table[(handle & 0x3FFF) >> 2] at dmc3.exe+5E1830 holds the object, then
// dmc3.exe+2CC95 - FF 50 08 - call qword ptr [rax+08]. A stale/poisoned object has [obj] = -1 and faults here.
static LONG CALLBACK RenderHandleGuard(PEXCEPTION_POINTERS info) {
    auto record = info->ExceptionRecord;
    auto ctx    = info->ContextRecord;

    if (record->ExceptionCode != EXCEPTION_ACCESS_VIOLATION || ctx->Rip != reinterpret_cast<DWORD64>(appBaseAddr + 0x2CC95)) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    static uint32 count = 0;
    if (count++ < 16) {
        auto obj    = ctx->Rdi;
        auto handle = ctx->Rbx;
        Log("RenderHandleGuard: handle %llX slot %llX obj %llX [obj] %llX mode %llX", handle, ((handle & 0x3FFF) >> 2), obj, ctx->Rax, ctx->Rbp);

        MEMORY_BASIC_INFORMATION mbi = {};
        if (VirtualQuery(reinterpret_cast<void*>(obj), &mbi, sizeof(mbi))) {
            Log("  region base %llX alloc %llX size %llX state %X type %X protect %X", mbi.BaseAddress, mbi.AllocationBase, mbi.RegionSize, mbi.State,
                mbi.Type, mbi.Protect);
        }

        auto Where = [&](const char* name, byte8* base, uint64 size) {
            if (base && obj >= reinterpret_cast<uint64>(base) && obj < (reinterpret_cast<uint64>(base) + size)) {
                Log("  inside %s +%llX", name, obj - reinterpret_cast<uint64>(base));
            }
        };
        Where("Memory_main", Memory_main, MEMORY_SIZE_ALL);
        Where("File_staticFiles", File_staticFiles.dataAddr, File_staticFiles.dataSize);
        Where("File_dynamicFiles", File_dynamicFiles.dataAddr, File_dynamicFiles.dataSize);

        // Managed heap arenas (g_HeapArena0/1/2 at dmc3.exe+CA8910, stride 0x28): map byte 0 = free block, 1 = allocated.
        for (uint32 arenaIndex = 0; arenaIndex < 3; arenaIndex++) {
            auto arena      = appBaseAddr + 0xCA8910 + (arenaIndex * 0x28);
            auto memory     = *reinterpret_cast<byte8**>(arena);
            auto data       = *reinterpret_cast<byte8**>(arena + 8);
            auto blockCount = *reinterpret_cast<int32*>(arena + 0x10);
            auto blockSize  = *reinterpret_cast<int32*>(arena + 0x14);
            auto totalSize  = *reinterpret_cast<uint32*>(arena + 0x18);
            if (!memory || !blockSize || obj < reinterpret_cast<uint64>(memory) || obj >= (reinterpret_cast<uint64>(memory) + totalSize)) {
                continue;
            }
            if (obj < reinterpret_cast<uint64>(data)) {
                Log("  heap arena %u: in the block map +%llX", arenaIndex, obj - reinterpret_cast<uint64>(memory));
                continue;
            }
            auto block = (obj - reinterpret_cast<uint64>(data)) / blockSize;
            auto used  = (block < static_cast<uint64>(blockCount)) ? memory[block] : 0xEE;
            Log("  heap arena %u: block %llu/%d (size %X) map %X allocCount %d", arenaIndex, block, blockCount, blockSize, used,
                *reinterpret_cast<int32*>(arena + 0x20));
        }

        // Render pools carved at the start of the root allocation (CKernelMemory_Initialize_sub_140030190).
        constexpr uint32 poolPtrOffsets[] = {0x5D9D38, 0x5D9D40, 0x5D9D48, 0x5D9D50, 0x5D9D58, 0x5D9D60, 0x5D9D68, 0x5D9D70, 0x5D9D78, 0x5D9D80,
            0x5D9D88, 0x5D9D90, 0x5D9D98, 0x5D9DA0, 0x5D9DA8, 0x5D9D10, 0x5D9E80, 0x5D9F00, 0x5D9E88, 0x5D9E90, 0x5D9DB0, 0x5D9E78, 0x5D9DB8};
        for (auto offset : poolPtrOffsets) {
            auto pool = *reinterpret_cast<uint64*>(appBaseAddr + offset);
            if (pool && obj >= pool && (obj - pool) < 0x300000) {
                Log("  near render pool ptr dmc3.exe+%X (%llX) +%llX", offset, pool, obj - pool);
            }
        }
    }

    ctx->Rip = reinterpret_cast<DWORD64>(appBaseAddr + 0x2CCEF);

    return EXCEPTION_CONTINUE_EXECUTION;
}

void PatchD3D() {
    //140042E26:
    //shl rax,13
    Write<uint32>((appBaseAddr + 0x42E26), 0x13E0C148);

    //140042E42:
    //shl eax,13
    Write<uint32>((appBaseAddr + 0x42E42), 0x8913E0C1);

    //140042EA7:
    //shl rcx,13
    Write<uint32>((appBaseAddr + 0x42EA7), 0x13E1C148);

    //140042EBE:
    //shl eax,13
    Write<uint32>((appBaseAddr + 0x42EBE), 0x8913E0C1);

    //140042EF4:
    //mov eax,004E0000
    //nop
    Write<uint32>((appBaseAddr + 0x42EF4), 0x4E0000B8);
    Write<uint32>((appBaseAddr + 0x42EF8), 0x33459000);

    //140042F04:
    //mov qword ptr [r14+000017C8], 4E0000
    Write<uint32>((appBaseAddr + 0x42F04), 0xC886C749);
    Write<uint32>((appBaseAddr + 0x42F08), 0x00000017);
    Write<uint32>((appBaseAddr + 0x42F0C), 0x89004E00);

    //140042F45:
    //mov eax,009C0000
    //nop
    Write<uint32>((appBaseAddr + 0x42F45), 0x9C0000B8);
    Write<uint32>((appBaseAddr + 0x42F49), 0x8D489000);

    //140042F60:
    //mov qword ptr [r14+00001800], 9C0000
    Write<uint32>((appBaseAddr + 0x42F60), 0x0086C749);
    Write<uint32>((appBaseAddr + 0x42F64), 0x00000018);
    Write<uint32>((appBaseAddr + 0x42F68), 0x89009C00);
}

// Startup-only switches from the DEBUG menu (CHEATS & DEBUG > MEMLIMIT INCREASE): nod3d (PatchD3D), noheap (heap/section/scratch
// arena sizes), noblocks (heap/scratch block sizes), nopools (render pools).
static bool MemTest(const char* token) {
    auto& config = activeCrimsonConfig.Memlimit;
    if (!decltype(activeCrimsonConfig.Memlimit)::available) {
        return false;
    }
    if (strcmp(token, "nod3d") == 0) {
        return config.disableD3DBuffers;
    } else if (strcmp(token, "noheap") == 0) {
        return config.disableHeapSizes;
    } else if (strcmp(token, "noblocks") == 0) {
        return config.disableBlockSizes;
    } else if (strcmp(token, "nopools") == 0) {
        return config.disableRenderPools;
    }
    return false;
}

void PatchArenaSizes()
{
	constexpr uint32 ROOT_ALLOC_SIZE = 0x20400000;
	constexpr uint32 MASTER_ARENA_SIZE = 0x20000000;
	constexpr uint32 MAIN_HEAP_SIZE = 0x08000000;
	constexpr uint32 MAIN_BLOCK_SIZE = 0x1000;

	Write<uint32>((appBaseAddr + 0x30195), ROOT_ALLOC_SIZE);
	//Write<uint32>((appBaseAddr + 0x301AB), ROOT_ALLOC_SIZE); 

	Write<uint32>((appBaseAddr + 0x2C60EF), MASTER_ARENA_SIZE);
	Write<uint32>((appBaseAddr + 0x2C6115), MASTER_ARENA_SIZE);

    if (!MemTest("noheap")) {
        Write<uint32>((appBaseAddr + 0x2C6031), MAIN_HEAP_SIZE);
        Write<uint32>((appBaseAddr + 0x2C604E), MAIN_HEAP_SIZE);
    }

    if (!MemTest("noblocks")) {
        Write<uint32>((appBaseAddr + 0x2C605B), MAIN_BLOCK_SIZE);
    }

    // .pac scratch buffers
	// dmc3.exe + 1B8343 - BA 00 00 00 01           - mov edx,01000000 { 16777216 } -- 16 MB
	// dmc3.exe + 1B835C - 41 B9 00 00 00 01       - mov r9d, 01000000 { 16777216 } -- 16 MB
	constexpr uint32 RESOURCE_HEAP_SIZE = 0x04000000; // 64 MB
	constexpr uint32 RESOURCE_BLOCK_SIZE = 0x1000;
    if (!MemTest("noheap")) {
        Write<uint32>((appBaseAddr + 0x1B8344), RESOURCE_HEAP_SIZE);
        Write<uint32>((appBaseAddr + 0x1B835E), RESOURCE_HEAP_SIZE);
    }
    if (!MemTest("noblocks")) {
        Write<uint32>((appBaseAddr + 0x1B8364), RESOURCE_BLOCK_SIZE);
    }

    if (MemTest("nopools")) {
        return;
    }

    // Patching individual pools 
    Write<uint32>((appBaseAddr + 0x301CD), 0x00020000);
    Write<uint32>((appBaseAddr + 0x301ED), 0x00100000);
    Write<uint32>((appBaseAddr + 0x3020D), 0x00020000);

	constexpr uint32 FRAME_POOL_SIZE = 0x00008000;

	Write<uint32>((appBaseAddr + 0x3026C), FRAME_POOL_SIZE);
	Write<uint32>((appBaseAddr + 0x302A8), FRAME_POOL_SIZE);
	Write<uint32>((appBaseAddr + 0x302E6), FRAME_POOL_SIZE);
	Write<uint32>((appBaseAddr + 0x30305), FRAME_POOL_SIZE);

    constexpr uint32 AUX_FRAME_POOL_SIZE = 0x00000800;

    Write<uint32>((appBaseAddr + 0x30324), AUX_FRAME_POOL_SIZE);
    Write<uint32>((appBaseAddr + 0x30376), AUX_FRAME_POOL_SIZE);
    Write<uint32>((appBaseAddr + 0x30395), AUX_FRAME_POOL_SIZE);

    Write<uint32>((appBaseAddr + 0x303F2), 0x00040000);
    Write<uint8>((appBaseAddr + 0x30412), 0x40);

	//Write<uint32>((appBaseAddr + 0x3022C), 0x00001000); // 0x500  -> 0x1000 -- makes 3d stuff glitch out more, scene start crash
	//Write<uint32>((appBaseAddr + 0x3024C), 0x00000800); // 0x418  -> 0x800 -- makes 2d stuff glitch out more, scene start crash
// 
 	Write<uint32>((appBaseAddr + 0x3026C), 0x00008000); // 0x1018 -> 0x8000 -- same stability
// 
 	//Write<uint32>((appBaseAddr + 0x3028A), 0x00000070); // 0x58   -> 0x100 -- makes 3d and 2d stuff glitch out, 3d stuff looks transparent, also affects blending effects,
    // actually more stable crash-wise
// 
    Write<uint32>((appBaseAddr + 0x302A8), 0x00010000); // 0x1018 -> 0x8000 -- actually seems to improve stability, can hold scene for longer without crashing
// 
 	//Write<uint32>((appBaseAddr + 0x302C7), 0x00000200); // 0xA8   -> 0x200 -- no apparent difference, if anything slightly worse stability crash-wise
// 
//  	 	Write<uint32>((appBaseAddr + 0x302E6), 0x00008000); // 0x1018 -> 0x8000 -- no apparent difference
//  	 	Write<uint32>((appBaseAddr + 0x30305), 0x00008000); // 0x1018 -> 0x8000 -- no apparent difference

// 	constexpr uint32 RENDER_PACKET_POOL = 0x00008000;
// 
// 	Write<uint32>((appBaseAddr + 0x3026C), RENDER_PACKET_POOL);
// 	Write<uint32>((appBaseAddr + 0x302A8), RENDER_PACKET_POOL);
// 	Write<uint32>((appBaseAddr + 0x302E6), RENDER_PACKET_POOL);
// 	Write<uint32>((appBaseAddr + 0x30305), RENDER_PACKET_POOL);
}

bool Memory_Init() {
    LogFunction();

    Memory_main = LowAlloc(MEMORY_SIZE_ALL);
    if (!Memory_main) {
        Log("LowAlloc failed.");
        return false;
    }

    Log("Memory_main %llX", Memory_main);

    {
        auto func = old_CreateFunction(GetMemoryAddress, (appBaseAddr + 0x3019E), true, false);
        WriteJump((appBaseAddr + 0x30199), func.addr);
        /*
        dmc3.exe+30199 - E8 328F0100 - call dmc3.exe+490D0
        dmc3.exe+3019E - 48 8B D0    - mov rdx,rax
        */
    }

    PatchArenaSizes();
    if (!MemTest("nod3d")) {
        PatchD3D();
    }
    Log("Memlimit: d3d %u heap %u blocks %u pools %u (1 = patch off)", MemTest("nod3d"), MemTest("noheap"), MemTest("noblocks"), MemTest("nopools"));
    AddVectoredExceptionHandler(1, RenderHandleGuard);
    AddVectoredExceptionHandler(1, MenuResourceGuard);
    HookSubmitResourceLoad();
    // 256 actually

    // Increase main memory from 260 MB to 512 MB.
//     {
//         constexpr uint32 size = (516 * 1024 * 1024);
//         Write<uint32>((appBaseAddr + 0x30195), size);
//         Write<uint32>((appBaseAddr + 0x301AB), size);
//     }
// 
	// Increase main memory from 260 MB to 512 MB.
	{
		constexpr uint32 size = (512 * 1024 * 1024);
//		Write<uint32>((appBaseAddr + 0x2C60ED), size);
// 		Write<uint32>((appBaseAddr + 0x2C6113), size);
	}

    // Increase section from 5 MB to 128 MB (alloc size @ +2C6065, managed arena size @ +2C6082, both immediates).
    if (!MemTest("noheap")) {
        constexpr uint32 size = (128 * 1024 * 1024);
        Write<uint32>((appBaseAddr + 0x2C6065), size);
        Write<uint32>((appBaseAddr + 0x2C6082), size);
    }

    // Small heap (arena 2) from 4 MB to 128 MB (alloc size @ +2C6099, managed arena size @ +2C60B6, both immediates).
    // CKernelHeap_Alloc_sub_1402C6150 routes every allocation to it while byte_140CA8AB1 is set, which hit the pause menu
    // loading its 10 MB id430.pac and failing.
    if (!MemTest("noheap")) {
        constexpr uint32 size = (128 * 1024 * 1024);
        Write<uint32>((appBaseAddr + 0x2C6099), size);
        Write<uint32>((appBaseAddr + 0x2C60B6), size);
    }

    {
        constexpr uint32 size = ((vectorItemCount * 0x10) + 8);
        effectManagerData     = HighAlloc(size);
        valueManagerData      = HighAlloc(size);
        if (!effectManagerData || !valueManagerData) {
            Log("HighAlloc failed.");
            return false;
        }
    }

    return true;
}

void Memory_ToggleExtendVectors(bool enable) {
    LogFunction(enable);

    // Effect Manager
    {
        byte8* addr  = (enable) ? effectManagerData : (appBaseAddr + 0xCAB230);
        uint32 count = (enable) ? vectorItemCount : 304;

        WriteAddress((appBaseAddr + 0x2C0387), addr, 7);
        WriteAddress((appBaseAddr + 0x2C0460), addr, 7);
        WriteAddress((appBaseAddr + 0x2C0639), addr, 7);
        WriteAddress((appBaseAddr + 0x2E3B2E), addr, 7);
        WriteAddress((appBaseAddr + 0x2E3BAE), addr, 7);
        WriteAddress((appBaseAddr + 0x2E826E), addr, 7);

        Write<uint32>((appBaseAddr + 0x2E86D2), count);
        Write<uint32>((appBaseAddr + 0x2E8712), count);
        Write<uint32>((appBaseAddr + 0x2E876E), count);
        Write<uint32>((appBaseAddr + 0x2E87AC), count);
    }

    // Value Manager
    {
        byte8* addr  = (enable) ? valueManagerData : (appBaseAddr + 0xCF1270);
        uint32 count = (enable) ? vectorItemCount : 256;

        WriteAddress((appBaseAddr + 0x2C0376), addr, 7);
        WriteAddress((appBaseAddr + 0x2C043E), addr, 7);
        WriteAddress((appBaseAddr + 0x2C0663), addr, 7);
        WriteAddress((appBaseAddr + 0x2E824A), addr, 7);
        WriteAddress((appBaseAddr + 0x32447E), addr, 7);
        WriteAddress((appBaseAddr + 0x3244FE), addr, 7);
        WriteAddress((appBaseAddr + 0x324804), addr, 7);

        Write<uint32>((appBaseAddr + 0x324F42), count);
        Write<uint32>((appBaseAddr + 0x324F83), count);
        Write<uint32>((appBaseAddr + 0x324FFE), count);
        Write<uint32>((appBaseAddr + 0x32503C), count);
    }
}

// @Todo: Remove.
byte8* GetMasterPointer() {
    auto dest = *reinterpret_cast<byte8**>(appBaseAddr + 0xC8F970 + 0x1478 + 0x10);
    if (!dest) {
        return 0;
    }
    return *reinterpret_cast<byte8**>(dest + 0x18 + 0x10);
}

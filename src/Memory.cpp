// @Todo: Review.
// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "Memory.hpp"
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

	Write<uint32>((appBaseAddr + 0x2C6031), MAIN_HEAP_SIZE);
	Write<uint32>((appBaseAddr + 0x2C604E), MAIN_HEAP_SIZE);

    Write<uint32>((appBaseAddr + 0x2C605B), MAIN_BLOCK_SIZE);

// 	constexpr uint32 RESOURCE_HEAP_SIZE = 0x02000000;
// 	constexpr uint32 RESOURCE_BLOCK_SIZE = 0x1000;
// 
// 	Write<uint32>((appBaseAddr + 0x1B8344), RESOURCE_HEAP_SIZE);
// 		Write<uint32>((appBaseAddr + 0x1B835E), RESOURCE_HEAP_SIZE);
// 		Write<uint32>((appBaseAddr + 0x1B8364), RESOURCE_BLOCK_SIZE);

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
    PatchD3D();
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

    // Increase section from 5 MB to 16 MB.
    {
        constexpr uint32 size = (128 * 1024 * 1024);
        Write<uint32>((appBaseAddr + 0x2C6065), size);
        Write<uint32>((appBaseAddr + 0x2C6082), size);
    }

    // Increase internal allocation pools (enemies, textures, models).
    {
        constexpr uint32 size = (512 * 1024 * 1024);
        
        // mov edx, size  (2nd arg — allocation cap)
        //Write<uint32>((appBaseAddr + 0x2C6030), size); -- crashes
        //Write<uint32>((appBaseAddr + 0x2C6064), size); -- crashes
        //Write<uint32>((appBaseAddr + 0x2C6098), size); -- crashes

        // mov r9d, size  (4th arg — different pool param)
        //Write<uint32>((appBaseAddr + 0x2C604C), size); // -- crashes
        Write<uint32>((appBaseAddr + 0x2C6080), size);
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

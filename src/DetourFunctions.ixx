module;


#include <stdio.h>
#include "Utility/Detour.hpp"
#include <intrin.h>
#include <string>



export module DetourFunctions;

extern "C" {
	std::uint64_t DetourBaseAddr;

	std::uint64_t g_SampleMod_ReturnAddr1;
	export void SampleModDetour1();

	std::uint64_t g_GuardGravity_ReturnAddr1;
	export void GuardGravityDetour();

	std::uint64_t createEffectRBXMov;
	std::uint64_t createEffectCallA;
	std::uint64_t createEffectCallB;
	export int createEffectBank = 3;
	export int createEffectID = 144;
	export void CreateEffectDetour();
}

import Core;


export void InitDetours() {
	using namespace Utility;
	DetourBaseAddr = (uintptr_t)appBaseAddr;

	//GuardGravity
	static std::unique_ptr<Detour_t> guardGravityHook = std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1EE121, &GuardGravityDetour, 7);
	g_GuardGravity_ReturnAddr1 = guardGravityHook->GetReturnAddress();
	guardGravityHook->Toggle(true);

	//EffectCall
	createEffectCallA = (uintptr_t)appBaseAddr + 0x2E7CA0;
	createEffectCallB = (uintptr_t)appBaseAddr + 0x1FAA50;
	createEffectRBXMov = (uintptr_t)appBaseAddr + 0xC18AF8;
}
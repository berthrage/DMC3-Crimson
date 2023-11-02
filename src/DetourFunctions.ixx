module;

#include <stdio.h>
#include "Utility/Detour.hpp"
#include <intrin.h>
#include <string>

export module DetourFunctions;

import Core;
import Vars;

extern "C" {
	std::uint64_t DetourBaseAddr;
	float* holdToCrazyComboActionTimer{};

	// SampleMod
	std::uint64_t g_SampleMod_ReturnAddr1;
	export void SampleModDetour1();

	// GuardGravity
	std::uint64_t g_GuardGravity_ReturnAddr;
	export void GuardGravityDetour();

	// CreateEffect
	std::uint64_t createEffectRBXMov;
	std::uint64_t createEffectCallA;
	std::uint64_t createEffectCallB;
	export int createEffectBank = 3;
	export int createEffectID = 144;
	export void CreateEffectDetour();

	// HoldToCrazyCombo
	std::uint64_t g_HoldToCrazyCombo_ReturnAddr;
	export void HoldToCrazyComboDetour();
	std::uint64_t g_holdToCrazyComboConditionalAddr;

	// HudHPSeparation
	std::uint64_t g_HudHPSeparation_ReturnAddr;
	export void HudHPSeparationDetour();

}

export void InitDetours() {
	using namespace Utility;
	DetourBaseAddr = (uintptr_t)appBaseAddr;

	// GuardGravity
	static std::unique_ptr<Detour_t> guardGravityHook = std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1EE121, &GuardGravityDetour, 7);
	g_GuardGravity_ReturnAddr = guardGravityHook->GetReturnAddress();
	guardGravityHook->Toggle(true);

	// CreateEffect
	createEffectCallA = (uintptr_t)appBaseAddr + 0x2E7CA0;
	createEffectCallB = (uintptr_t)appBaseAddr + 0x1FAA50;
	createEffectRBXMov = (uintptr_t)appBaseAddr + 0xC18AF8;

	// HoldToCrazyCombo
	static std::unique_ptr<Utility::Detour_t> HoldToCrazyComboHook = std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1EB7C5, &HoldToCrazyComboDetour, 12);
	g_HoldToCrazyCombo_ReturnAddr = HoldToCrazyComboHook->GetReturnAddress();
	g_holdToCrazyComboConditionalAddr = (uintptr_t)appBaseAddr + 0x1EB7FE;
	HoldToCrazyComboHook->Toggle(true);
	holdToCrazyComboActionTimer = &crimsonPlayer[0].actionTimer;


	// HudHPSeparation
	static std::unique_ptr<Utility::Detour_t> HudHPSeparationHook = std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x27DD64, &HudHPSeparationDetour, 8);
	g_HudHPSeparation_ReturnAddr = HudHPSeparationHook->GetReturnAddress();
	HudHPSeparationHook->Toggle(true);
}

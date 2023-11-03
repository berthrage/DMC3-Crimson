module;

#include <stdio.h>
#include "Utility/Detour.hpp"
#include <intrin.h>
#include <string>
#include <algorithm>

export module DetourFunctions;

import Core;
import Vars;
import Input; // tiltdirection

extern "C" {
	std::uint64_t DetourBaseAddr;

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
	void* holdToCrazyComboCall;

	// HudHPSeparation
	std::uint64_t g_HudHPSeparation_ReturnAddr;
	export void HudHPSeparationDetour();

}

bool g_HoldToCrazyComboFuncA(PlayerActorData& actorData) {
	// iterate through crimsonPlayers until finding actorData I guess
	/*int i;
	for (i = 0; i < sizeof(crimsonPlayer) / sizeof(crimsonPlayer[0]); i++) {
		if (actorData != (PlayerActorData&)crimsonPlayer[i]) { return; }
	}*/
	switch (actorData.action) { // from vars, namespaceStart(ACTION_DANTE); 
		case ACTION_DANTE::REBELLION_STINGER_LEVEL_1:
			if (std::clamp<float>(crimsonPlayer[0].actionTimer, 0.2f, 0.3f) == crimsonPlayer[0].actionTimer &&
				GetRelativeTiltDirection(actorData) == TILT_DIRECTION::NEUTRAL) { return true; }
			break;
		case ACTION_DANTE::REBELLION_STINGER_LEVEL_2:
			if (std::clamp<float>(crimsonPlayer[0].actionTimer, 0.2f, 0.3f) == crimsonPlayer[0].actionTimer &&
				GetRelativeTiltDirection(actorData) == TILT_DIRECTION::NEUTRAL) { return true; }
			break;
		case ACTION_DANTE::REBELLION_COMBO_2_PART_2:
			if (std::clamp<float>(crimsonPlayer[0].actionTimer, 0.0f, 0.85f) == crimsonPlayer[0].actionTimer &&
				GetRelativeTiltDirection(actorData) == TILT_DIRECTION::NEUTRAL) { return true; }
			break;
		default:
			break;
	}
	return false;
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
	holdToCrazyComboCall = &g_HoldToCrazyComboFuncA;


	// HudHPSeparation
	static std::unique_ptr<Utility::Detour_t> HudHPSeparationHook = std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x27DD64, &HudHPSeparationDetour, 8);
	g_HudHPSeparation_ReturnAddr = HudHPSeparationHook->GetReturnAddress();
	HudHPSeparationHook->Toggle(true);
}

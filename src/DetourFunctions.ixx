module;

#include <stdio.h>
#include "Utility/Detour.hpp"
#include <intrin.h>
#include <string>
#include <algorithm>

export module DetourFunctions;

import Core;
#include "Core/Macros.h" 

import ActorBase;
import Global;
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

	// DisableDriveHold
	std::uint64_t g_DisableDriveHold_ReturnAddr;
	export void DisableDriveHoldDetour();

	// HudHPSeparation
	std::uint64_t g_HudHPSeparation_ReturnAddr;
	export void HudHPSeparationDetour();

}

export bool g_HoldToCrazyComboFuncA(PlayerActorData& actorData) {
	using namespace ACTION_DANTE;

	auto playerIndex = GetPlayerIndexFromAddr((uintptr_t)actorData.baseAddr);
	
	auto inputException = !(crimsonPlayer[playerIndex].lockOn &&
		(crimsonPlayer[playerIndex].tiltDirection == TILT_DIRECTION::UP || crimsonPlayer[playerIndex].tiltDirection == TILT_DIRECTION::DOWN));

	auto inputExceptionNevanJamSession = !(crimsonPlayer[playerIndex].tiltDirection == TILT_DIRECTION::LEFT);



	switch (crimsonPlayer[playerIndex].action) { // from vars, namespaceStart(ACTION_DANTE); 

	case REBELLION_STINGER_LEVEL_1:
		if (std::clamp<float>(crimsonPlayer[playerIndex].actionTimer, 0.2f, 0.3f) == crimsonPlayer[playerIndex].actionTimer &&
			inputException) {
			return true;
		}
		break;
	case REBELLION_STINGER_LEVEL_2:
		if (std::clamp<float>(crimsonPlayer[playerIndex].actionTimer, 0.2f, 0.3f) == crimsonPlayer[playerIndex].actionTimer &&
			inputException) {
			return true;
		}
		break;
	case REBELLION_MILLION_STAB:
		if (std::clamp<float>(crimsonPlayer[playerIndex].actionTimer, 0.2f, 10.0f) == crimsonPlayer[playerIndex].actionTimer &&
			inputException) {
			return true;
		}
		break;
	case REBELLION_COMBO_2_PART_2:
		if (std::clamp<float>(crimsonPlayer[playerIndex].actionTimer, 0.0f, 0.85f) == crimsonPlayer[playerIndex].actionTimer &&
			inputException) {
			return true;
		}
		break;
	case BEOWULF_COMBO_2_PART_3:
		if (std::clamp<float>(crimsonPlayer[playerIndex].animTimer, 0.5f, 1.09f) == crimsonPlayer[playerIndex].animTimer &&
			inputException) {
			return true;
		}
		break;
	case NEVAN_COMBO_1:
		if (inputExceptionNevanJamSession) {
			return true;
		}
		break;
	case CERBERUS_COMBO_2_PART_4:
		if (std::clamp<float>(crimsonPlayer[playerIndex].actionTimer, 0.0f, 1.0f) == crimsonPlayer[playerIndex].actionTimer &&
			inputException) {
			return true;
		}
		break;
	case REBELLION_PROP:
		return true;

	case REBELLION_SHREDDER:
		return true;

	case REBELLION_DANCE_MACABRE_PART_8:
		return true;

	case REBELLION_CRAZY_DANCE:
		return true;

	case POLE_PLAY:
		return true;

	case CERBERUS_WINDMILL:
		return true;

	case CERBERUS_CRYSTAL:
		return true;

	case CERBERUS_MILLION_CARATS:
		return true;

	case AGNI_RUDRA_COMBO_3_PART_3:
		return true;

	case AGNI_RUDRA_MILLION_SLASH:
		return true;

	case AGNI_RUDRA_TWISTER:
		return true;

	case AGNI_RUDRA_TEMPEST:
		return true;

	case NEVAN_FEEDBACK:
		return true;

	case NEVAN_CRAZY_ROLL:
		return true;

	case BEOWULF_REAL_IMPACT:
		return true;

	case BEOWULF_TORNADO:
		return true;

	case BEOWULF_HYPER_FIST:
		return true;

	case SHOTGUN_GUN_STINGER:
		return true;

	case SHOTGUN_POINT_BLANK:
		return true;

	case ARTEMIS_SPHERE:
		return true;

	case ARTEMIS_ACID_RAIN:
		return true;

	case EBONY_IVORY_NORMAL_SHOT:
		return true;

	case EBONY_IVORY_CHARGED_SHOT:
		return true;

	case EBONY_IVORY_WILD_STOMP:
		return true;

	case SPIRAL_SNIPER:
		return true;

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
	
	// DisableDriveHold
	static std::unique_ptr<Utility::Detour_t> DisableDriveHoldHook = std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1EB6F2, &DisableDriveHoldDetour, 5);
	g_DisableDriveHold_ReturnAddr = DisableDriveHoldHook->GetReturnAddress();
	DisableDriveHoldHook->Toggle(true);

	// HudHPSeparation
	static std::unique_ptr<Utility::Detour_t> HudHPSeparationHook = std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x27DD64, &HudHPSeparationDetour, 8);
	g_HudHPSeparation_ReturnAddr = HudHPSeparationHook->GetReturnAddress();
	HudHPSeparationHook->Toggle(true);
}

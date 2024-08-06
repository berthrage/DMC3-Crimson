
// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include <algorithm>
#include "DetourFunctions.hpp"
#include "Core/Core.hpp"
#include <stdio.h>
#include "Utility/Detour.hpp"
#include <intrin.h>
#include <string>
#include "CrimsonUtil.hpp"
#include "DMC3Input.hpp"
#include "Global.hpp"

#include "Core/Macros.h"
#include "Config.hpp"
#include <iostream>

extern "C" {
std::uint64_t DetourBaseAddr;

// SampleMod
std::uint64_t g_SampleMod_ReturnAddr1;
void SampleModDetour1();

// GuardGravity
std::uint64_t g_GuardGravity_ReturnAddr;
void GuardGravityDetour();

// EnableAirTaunt
std::uint64_t g_EnableAirTaunt_ReturnAddr;
std::uint64_t g_EnableAirTaunt_ConditionalAddr;
std::uint64_t g_EnableAirTaunt_ConditionalAddr2;
void EnableAirTauntDetour();

// SetAirTaunt
std::uint64_t g_SetAirTaunt_ReturnAddr;
std::uint64_t g_SetAirTaunt_Call;
void SetAirTauntDetour();

// CreateEffect
std::uint64_t createEffectRBXMov;
std::uint64_t createEffectCallA;
std::uint64_t createEffectCallB;
int createEffectBank = 3;
int createEffectID   = 144;
int createEffectBone = 1;
std::uint64_t createEffectPlayerAddr = 0;
void CreateEffectDetour();

// HoldToCrazyCombo
std::uint64_t g_HoldToCrazyCombo_ReturnAddr;
void HoldToCrazyComboDetour();
std::uint64_t g_holdToCrazyComboConditionalAddr;
void* holdToCrazyComboCall;

// DisableStaggerRoyalguard
std::uint64_t g_DisableStagger_ReturnAddr;
std::uint64_t g_DisableStagger_ConditionalAddr;
void DisableStaggerRoyalguardDetour();
void* g_DisableStaggerCheckCall;

// ToggleTakeDamage
std::uint64_t g_ToggleTakeDamage_ReturnAddr;
void ToggleTakeDamageDetour();
void* g_ToggleTakeDamageCheckCall;

// DisableDriveHold
std::uint64_t g_DisableDriveHold_ReturnAddr;
void DisableDriveHoldDetour();

// HudHPSeparation
std::uint64_t g_HudHPSeparation_ReturnAddr;
void HudHPSeparationDetour();

// HudStyleBarPos
std::uint64_t g_HudStyleBarPos_ReturnAddr;
float g_HudStyleBarPosX;
float g_HudStyleBarPosY;
void HudStyleBarPosDetour();

// StyleRankHUDNoFadeout
std::uint64_t g_StyleRankHudNoFadeout_ReturnAddr;
void StyleRankHudNoFadeoutDetour();

// ShootRemapDown
std::uint64_t g_ShootRemapDown_ReturnAddr;
void ShootRemapDownDetour();
std::uint64_t g_ShootRemapHold_ReturnAddr;
void ShootRemapHoldDetour();
std::uint16_t g_ShootRemap_NewMap;

// VergilNeutralTrick
std::uint64_t g_VergilNeutralTrick_ReturnAddr;
void VergilNeutralTrickDetour();
}

bool g_HoldToCrazyComboFuncA(PlayerActorData& actorData) {
    using namespace ACTION_DANTE;

    auto playerIndex = GetPlayerIndexFromAddr((uintptr_t)actorData.baseAddr); // simply using actorData.newPlayerIndex also works here.

    auto tiltDirection = GetRelativeTiltDirection(actorData);

    auto inputException = !(actorData.lockOn && (tiltDirection == TILT_DIRECTION::UP || tiltDirection == TILT_DIRECTION::DOWN));

    auto inputExceptionNevanJamSession = !(tiltDirection == TILT_DIRECTION::LEFT);

    // if the player ptr we fetched is a Clone then we use action/animTimers Clone, if not then use the normal ones instead.
    auto actionTimer =
        (actorData.newEntityIndex == 1) ? crimsonPlayer[playerIndex].actionTimerClone : crimsonPlayer[playerIndex].actionTimer;
    auto animTimer = (actorData.newEntityIndex == 1) ? crimsonPlayer[playerIndex].animTimerClone : crimsonPlayer[playerIndex].animTimer;


    switch (actorData.action) { // from vars, namespace ACTION_DANTE {

    case REBELLION_STINGER_LEVEL_1:
        if (std::clamp<float>(actionTimer, 0.2f, 0.3f) == actionTimer && inputException) {
            return true;
        }
        break;
    case REBELLION_STINGER_LEVEL_2:
        if (std::clamp<float>(actionTimer, 0.2f, 0.3f) == actionTimer && inputException) {
            return true;
        }
        break;
    case REBELLION_MILLION_STAB:
        if (std::clamp<float>(actionTimer, 0.2f, 10.0f) == actionTimer && inputException) {
            return true;
        }
        break;
    case REBELLION_COMBO_2_PART_2:
        if (std::clamp<float>(actionTimer, 0.0f, 0.85f) == actionTimer && inputException) {
            return true;
        }
        break;
    case BEOWULF_COMBO_2_PART_3:
        if (std::clamp<float>(animTimer, 0.5f, 1.09f) == animTimer && inputException) {
            return true;
        }
        break;
    case NEVAN_COMBO_1:
        if (inputExceptionNevanJamSession) {
            return true;
        }
        break;
    case CERBERUS_COMBO_2_PART_4:
        if (std::clamp<float>(actionTimer, 0.0f, 1.0f) == actionTimer && inputException) {
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

bool DisableStaggerCheck(PlayerActorData& actorData) {
	auto playerIndex = actorData.newPlayerIndex;
    auto& playerData = GetPlayerData(actorData);
    bool ensureIsActiveActor = ((actorData.newCharacterIndex == playerData.activeCharacterIndex) && (actorData.newEntityIndex == ENTITY::MAIN));

	// All of this will feed into Royalguard Rebalanced (check CrimsonGameplay)
	if (ensureIsActiveActor) {
        if (actorData.character != CHARACTER::DANTE) {
            return false;
        }

        if (actorData.magicPoints >= 2000) {
			if (actorData.guard && actorData.eventData[0].event != 44) {
				return true;
			}
            else {
                return false;
            }
        }
        else if (actorData.magicPoints < 2000) {
            return false;
        }
        
		
	}
	return false;
}

bool TakeDamageCheck(std::uint64_t addr) {

    auto playerActorAddr = addr - 0x4114;
    auto& actorData = *reinterpret_cast<PlayerActorData*>(playerActorAddr);
	auto playerIndex = actorData.newPlayerIndex;
	auto& playerData = GetPlayerData(actorData);
	bool ensureIsActiveActor = ((actorData.newCharacterIndex == playerData.activeCharacterIndex) && (actorData.newEntityIndex == ENTITY::MAIN));

    for (int i = 0; i < PLAYER_COUNT; i++) {
        if (playerActorAddr == crimsonPlayer[i].playerPtr) {
			if (actorData.character != CHARACTER::DANTE) {
				return false;
			}
			if (ensureIsActiveActor) {
				if (actorData.guard && actorData.magicPoints >= 2000) {
					return true;
				}
			}
			
        }
    }
    return false;
	// All of this will feed into Royalguard Rebalanced (check CrimsonGameplay)
    
// 	if (ensureIsActiveActor) {
// 		if (actorData.character != CHARACTER::DANTE) {
// 			return false;
// 		}
// 
// 		if (actorData.magicPoints >= 2000) {
// 			if (actorData.guard && actorData.eventData[0].event != 44) {
// 				return true;
// 			}
// 			else {
// 				return false;
// 			}
// 		}
// 		else if (actorData.magicPoints < 2000) {
// 			return false;
// 		}
// 
// 
// 	}
// 	return false;


    
}

void InitDetours() {
    using namespace Utility;
    DetourBaseAddr = (uintptr_t)appBaseAddr;

    // GuardGravity
    static std::unique_ptr<Detour_t> guardGravityHook =
        std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1EE121, &GuardGravityDetour, 7);
    g_GuardGravity_ReturnAddr = guardGravityHook->GetReturnAddress();
    guardGravityHook->Toggle(true);

    // EnableAirTaunt
    static std::unique_ptr<Detour_t> enableAirTauntHook =
        std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1E99EB, &EnableAirTauntDetour, 9);
    g_EnableAirTaunt_ReturnAddr = enableAirTauntHook->GetReturnAddress();
    enableAirTauntHook->Toggle(true);
    g_EnableAirTaunt_ConditionalAddr  = (uintptr_t)appBaseAddr + 0x1E9A53;
    g_EnableAirTaunt_ConditionalAddr2 = (uintptr_t)appBaseAddr + 0x1E9A0F;

    // SetAirTaunt
    static std::unique_ptr<Detour_t> setAirTauntHook = std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1E9A46, &SetAirTauntDetour, 5);
    g_SetAirTaunt_ReturnAddr                         = setAirTauntHook->GetReturnAddress();
    g_SetAirTaunt_Call                               = (uintptr_t)appBaseAddr + 0x1E09D0;
    setAirTauntHook->Toggle(true);

    // CreateEffect
    createEffectCallA  = (uintptr_t)appBaseAddr + 0x2E7CA0;
    createEffectCallB  = (uintptr_t)appBaseAddr + 0x1FAA50;
    createEffectRBXMov = (uintptr_t)appBaseAddr + 0xC18AF8;

    // HoldToCrazyCombo
    static std::unique_ptr<Utility::Detour_t> HoldToCrazyComboHook =
        std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1EB7C5, &HoldToCrazyComboDetour, 12);
    g_HoldToCrazyCombo_ReturnAddr     = HoldToCrazyComboHook->GetReturnAddress();
    g_holdToCrazyComboConditionalAddr = (uintptr_t)appBaseAddr + 0x1EB7FE;
    HoldToCrazyComboHook->Toggle(true);
    holdToCrazyComboCall = &g_HoldToCrazyComboFuncA;


    // DisableStaggerRoyalguard
	static std::unique_ptr<Utility::Detour_t> DisableStaggerRoyalguardHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1EC464, &DisableStaggerRoyalguardDetour, 9);
	g_DisableStagger_ReturnAddr = DisableStaggerRoyalguardHook->GetReturnAddress();
	g_DisableStagger_ConditionalAddr = (uintptr_t)appBaseAddr + 0x1EC58B;
    DisableStaggerRoyalguardHook->Toggle(true);
	//dmc3.exe + 1EC464 - E9 22010000 - jmp dmc3.exe + 1EC58B
	//dmc3.exe + 1EC467 - 0F8E 1E010000 - jng dmc3.exe + 1EC58B - original code
	g_DisableStaggerCheckCall = &DisableStaggerCheck;

    // Toggle Take Damage
	static std::unique_ptr<Utility::Detour_t> takeDamageHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x8851C, &ToggleTakeDamageDetour, 5);
	g_ToggleTakeDamage_ReturnAddr = takeDamageHook->GetReturnAddress();
	takeDamageHook->Toggle(true);
    g_ToggleTakeDamageCheckCall = &TakeDamageCheck;

    // DisableDriveHold
    static std::unique_ptr<Utility::Detour_t> DisableDriveHoldHook =
        std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1EB6F2, &DisableDriveHoldDetour, 5);
    g_DisableDriveHold_ReturnAddr = DisableDriveHoldHook->GetReturnAddress();
    DisableDriveHoldHook->Toggle(true);

   
    
    // VergilNeutralTrick // func is already detoured, Crimson.MobilityFunction<27>+B1
    // static std::unique_ptr<Utility::Detour_t> VergilNeutralTrickHook = std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x0,
    // &VergilNeutralTrickDetour, 5); g_VergilNeutralTrick_ReturnAddr = VergilNeutralTrickHook->GetReturnAddress();
    // VergilNeutralTrickHook->Toggle(true);
}

void ToggleClassicHUDPositionings(bool enable) {
	using namespace Utility;
	DetourBaseAddr = (uintptr_t)appBaseAddr;

	// HudHPSeparation
	static std::unique_ptr<Utility::Detour_t> HudHPSeparationHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x27DD64, &HudHPSeparationDetour, 8);
	g_HudHPSeparation_ReturnAddr = HudHPSeparationHook->GetReturnAddress();
    if (enable) {
        HudHPSeparationHook->Toggle(false);
    }
    else {
        HudHPSeparationHook->Toggle(true);
    }
	

	// HudStyleBarPos
	static std::unique_ptr<Utility::Detour_t> HudStyleBarPosHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2BB357, &HudStyleBarPosDetour, 16);
	g_HudStyleBarPos_ReturnAddr = HudStyleBarPosHook->GetReturnAddress();
	g_HudStyleBarPosX = 575.0f;
	g_HudStyleBarPosY = 115.0f;
	
	if (enable) {
        HudStyleBarPosHook->Toggle(false);
	}
	else {
        HudStyleBarPosHook->Toggle(true);
	}
}

void ToggleStyleRankHudNoFadeout(bool enable) {
    using namespace Utility;
	DetourBaseAddr = (uintptr_t)appBaseAddr;

	// StyleRankHudNoFadeout 
	static std::unique_ptr<Utility::Detour_t> StyleRankHudNoFadeoutHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2BAE5D, &StyleRankHudNoFadeoutDetour, 10);
	g_StyleRankHudNoFadeout_ReturnAddr = StyleRankHudNoFadeoutHook->GetReturnAddress();

    if (enable) {
        StyleRankHudNoFadeoutHook->Toggle(true);
    }
    else {
        StyleRankHudNoFadeoutHook->Toggle(false);
    }
	
}

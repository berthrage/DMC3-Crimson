
// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include <algorithm>
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
#include "CrimsonPatches.hpp"

namespace CrimsonDetours {

extern "C" {
std::uint64_t DetourBaseAddr;

// SampleMod
std::uint64_t g_SampleMod_ReturnAddr1;
//void SampleModDetour1();

// GuardGravity
std::uint64_t g_GuardGravity_ReturnAddr;
void GuardGravityDetour();

// AddToMirageGauge
std::uint64_t g_AddToMirageGauge_ReturnAddr;
void AddToMirageGaugeDetour();
void* g_AddToMirageGaugeCall;

// EnableAirTaunt
std::uint64_t g_EnableAirTaunt_ReturnAddr;
std::uint64_t g_EnableAirTaunt_ConditionalAddr;
std::uint64_t g_EnableAirTaunt_ConditionalAddr2;
void EnableAirTauntDetour();

// SetAirTaunt
std::uint64_t g_SetAirTaunt_ReturnAddr;
std::uint64_t g_SetAirTaunt_Call;
void SetAirTauntDetour();

// Sky Launch Detours: (Dante Air Taunt)
// SkyLaunchForceRelease 
std::uint64_t g_SkyLaunchForceRelease_ReturnAddr;
void* g_skyLaunchForceReleaseCheckCall;
void SkyLaunchForceReleaseDetour();

// SkyLaunchKillRGConsumption
void* g_skyLaunchCheckCall;
std::uint64_t g_SkyLaunchKillRGConsumption_ReturnAddr;
void SkyLaunchKillRGConsumptionDetour();

// SkyLaunchKillReleaseLevel1
std::uint64_t g_SkyLaunchKillReleaseLevel1_ReturnAddr;
void SkyLaunchKillReleaseLevel1Detour();

// SkyLaunchKillReleaseLevel2
std::uint64_t g_SkyLaunchKillReleaseLevel2_ReturnAddr;
void SkyLaunchKillReleaseLevel2Detour();

// SkyLaunchKillReleaseLevel3
std::uint64_t g_SkyLaunchKillReleaseLevel3_ReturnAddr;
void SkyLaunchKillReleaseLevel3Detour();

// SkyLaunchKillDamage
std::uint64_t g_SkyLaunchKillDamage_ReturnAddr;
void SkyLaunchKillDamageDetour();

// SkyLaunchKillDamageToCerberus
std::uint64_t g_SkyLaunchKillDamageCerberus_ReturnAddr;
void SkyLaunchKillDamageCerberusDetour();

// SkyLaunchKillDamageToShieldNevan
void* g_skyLaunchCheckCall2;
std::uint64_t g_SkyLaunchKillDamageShieldNevan_ReturnAddr;
void SkyLaunchKillDamageShieldNevanDetour();

// CreateEffect
std::uint64_t createEffectRBXMov;
std::uint64_t createEffectCallA;
std::uint64_t createEffectCallB;
//void _fastcall CreateEffectDetour(void* pPlayer, int effectBank, int effectId, int effectBoneIdx, uint32_t colo_u_r);

// FixCrashCerberus
std::uint64_t g_FixCrashCerberus_ReturnAddr;
std::uint64_t g_FixCrashCerberus_PlayerStructAddr;
void FixCrashCerberusDetour();

// HoldToCrazyCombo
std::uint64_t g_HoldToCrazyCombo_ReturnAddr;
void HoldToCrazyComboDetour();
std::uint64_t g_holdToCrazyComboConditionalAddr;
void* holdToCrazyComboCall;

// DisableStaggerRoyalguard: for DT-Infused Royalguard
std::uint64_t g_DisableStagger_ReturnAddr;
std::uint64_t g_DisableStagger_ConditionalAddr;
void DisableStaggerRoyalguardDetour();
void* g_DisableStaggerCheckCall;

// ToggleTakeDamage: for DT-Infused Royalguard
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

// RerouteRedOrbsCounterAlpha
std::uint64_t g_RerouteRedOrbsCounterAlpha_ReturnAddr1;
std::uint64_t g_RerouteRedOrbsCounterAlpha_ReturnAddr2;
std::uint64_t g_RerouteRedOrbsCounterAlpha_ReturnAddr3;
std::uint64_t g_RerouteRedOrbsCounterAlpha_ReturnAddr4;
std::uint64_t g_RerouteRedOrbsCounterAlpha_VariableAddr;
void RerouteRedOrbsCounterAlphaDetour1();
void RerouteRedOrbsCounterAlphaDetour2();
void RerouteRedOrbsCounterAlphaDetour3();
void RerouteRedOrbsCounterAlphaDetour4();

// StyleRankHUDNoFadeout
std::uint64_t g_StyleRankHudNoFadeout_ReturnAddr;
void StyleRankHudNoFadeoutDetour();

// ShootRemapDown
std::uint64_t g_ShootRemapDown_ReturnAddr;
//void ShootRemapDownDetour();
std::uint64_t g_ShootRemapHold_ReturnAddr;
//void ShootRemapHoldDetour();
std::uint16_t g_ShootRemap_NewMap;

// VergilNeutralTrick
std::uint64_t g_VergilNeutralTrick_ReturnAddr;
//void VergilNeutralTrickDetour();
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

	// All of this will feed into DT Infused Royalguard (check CrimsonGameplay)
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

    // All of this will feed into DT Infused Royalguard (check CrimsonGameplay)

    for (int i = 0; i < PLAYER_COUNT; i++) {
        if (playerActorAddr == crimsonPlayer[i].playerPtr) {
			if (actorData.character != CHARACTER::DANTE) {
				return false;
			}
			if (ensureIsActiveActor) {
				if (actorData.guard && actorData.eventData[0].event != 44) {
					if (actorData.magicPoints >= 2000) {
                        return true;
					}
					else if (actorData.magicPoints < 2000) {
						return false;
					}
				}
				else {
					return false;
				}
				
				
			}
			
        }
    }
    return false;

}

void AddingToPlayersMirageGauge(PlayerActorData& actorData, std::uint64_t amountToAdduint64) {
    if (!activeConfig.Actor.enable) {
        return;
    }
    
    float amountToAdd = *reinterpret_cast<float*>(&amountToAdduint64);
    auto playerIndex = actorData.newPlayerIndex;
    auto& vergilDopp = crimsonPlayer[playerIndex].vergilDoppelganger;

	int newMiragePoints = vergilDopp.miragePoints + amountToAdd * 1.73f;
	if (newMiragePoints > vergilDopp.maxMiragePoints) {
        vergilDopp.miragePoints = vergilDopp.maxMiragePoints;
	}
	else {
        vergilDopp.miragePoints = newMiragePoints;
	}
    return;

}

bool CheckForceRoyalReleaseForSkyLaunch(PlayerActorData& actorData) {
    auto playerIndex = actorData.newPlayerIndex;
    auto gamepad = GetGamepad(playerIndex);

    if ((actorData.state & STATE::IN_AIR && gamepad.buttons[0] & GetBinding(BINDING::TAUNT))
            || activeConfig.Royalguard.forceJustFrameRelease) {
        return true;
    }

    return false;
}

bool DetectIfInSkyLaunch(PlayerActorData& actorData) {
	auto playerIndex = actorData.newPlayerIndex;
	auto gamepad = GetGamepad(playerIndex);

    if (activeConfig.infiniteHitPoints) {
        return true;
    }

    if (actorData.character == CHARACTER::DANTE) {
		if (actorData.state & STATE::IN_AIR && gamepad.buttons[0] & GetBinding(BINDING::TAUNT)) {
			return true;
		}
    }

	return false;
}

void InitDetours() {
    using namespace Utility;
    DetourBaseAddr = (uintptr_t)appBaseAddr;

	// AddToMirageGauge
	//dmc3.exe + 1E0BB2 - F3 0F58 89 B83E0000 - addss xmm1, [rcx + 00003EB8] 
	//dmc3.exe + 1E0B8E - 80 B9 9B3E0000 01 - cmp byte ptr[rcx + 00003E9B], 01 - original code, 
    // comparing if DT is on or not to add DT, but still holding the value to add at xmm1
	static std::unique_ptr<Utility::Detour_t> AddToMirageGaugeHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1E0B8E, &AddToMirageGaugeDetour, 7);
	g_AddToMirageGauge_ReturnAddr = AddToMirageGaugeHook->GetReturnAddress();
	AddToMirageGaugeHook->Toggle(true);
	g_AddToMirageGaugeCall = &AddingToPlayersMirageGauge;

    // GuardGravity
    static std::unique_ptr<Detour_t> guardGravityHook =
        std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1EE121, &GuardGravityDetour, 7);
    g_GuardGravity_ReturnAddr = guardGravityHook->GetReturnAddress();
    guardGravityHook->Toggle(true);

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

void SkyLaunchDetours(bool enable) {
	using namespace Utility;
	DetourBaseAddr = (uintptr_t)appBaseAddr;
	static bool run = false;

	if (run == enable) {
		return;
	}

	// SkyLaunchForceRelease
	// dmc3.exe + 20BCF8 - C6 83 10 3E 00 00 01 - mov byte ptr[rbx + 00003E10], 01 - original code
	static std::unique_ptr<Utility::Detour_t> SkyLaunchForceReleaseHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x20BCF8, &SkyLaunchForceReleaseDetour, 7);
	g_SkyLaunchForceRelease_ReturnAddr = SkyLaunchForceReleaseHook->GetReturnAddress();
	SkyLaunchForceReleaseHook->Toggle(enable);
	g_skyLaunchForceReleaseCheckCall = &CheckForceRoyalReleaseForSkyLaunch;

	// SkyLaunchKillRGConsumption
	// dmc3.exe + 20BD57 - C7 83 84 63 00 00 00 00 00 00 - mov[rbx + 00006384], 00000000
	static std::unique_ptr<Utility::Detour_t> SkyLaunchKillRGConsumptionHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x20BD57, &SkyLaunchKillRGConsumptionDetour, 10);
	g_SkyLaunchKillRGConsumption_ReturnAddr = SkyLaunchKillRGConsumptionHook->GetReturnAddress();
	SkyLaunchKillRGConsumptionHook->Toggle(enable);
	g_skyLaunchCheckCall = &DetectIfInSkyLaunch;

	// SkyLaunchKillReleaseLevel1
	// dmc3.exe+1EEDF7 - 66 C7 81 80 63 00 00 01 01 - mov word ptr [rcx+00006380], 257
	static std::unique_ptr<Utility::Detour_t> SkyLaunchKillReleaseLevel1Hook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1EEDF7, &SkyLaunchKillReleaseLevel1Detour, 9);
	g_SkyLaunchKillReleaseLevel1_ReturnAddr = SkyLaunchKillReleaseLevel1Hook->GetReturnAddress();
	SkyLaunchKillReleaseLevel1Hook->Toggle(enable);

	// SkyLaunchKillReleaseLevel2
	// dmc3.exe+1EEE27 - 66 C7 81 80 63 00 00 02 03 - mov word ptr [rcx+00006380], 770
	static std::unique_ptr<Utility::Detour_t> SkyLaunchKillReleaseLevel2Hook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1EEE27, &SkyLaunchKillReleaseLevel2Detour, 9);
	g_SkyLaunchKillReleaseLevel2_ReturnAddr = SkyLaunchKillReleaseLevel2Hook->GetReturnAddress();
	SkyLaunchKillReleaseLevel2Hook->Toggle(enable);

	// SkyLaunchKillReleaseLevel3
	// dmc3.exe+1EEE4A - 66 C7 81 80 63 00 00 03 05 - mov word ptr [rcx+00006380], 1283
	static std::unique_ptr<Utility::Detour_t> SkyLaunchKillReleaseLevel3Hook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1EEE4A, &SkyLaunchKillReleaseLevel3Detour, 9);
	g_SkyLaunchKillReleaseLevel3_ReturnAddr = SkyLaunchKillReleaseLevel3Hook->GetReturnAddress();
	SkyLaunchKillReleaseLevel3Hook->Toggle(enable);

	// SkyLaunchKillDamage
	// dmc3.exe + 881F1 - F3 44 0F 10 4A 0C - movss xmm9, [rdx + 0C]
	static std::unique_ptr<Utility::Detour_t> SkyLaunchKillDamageHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x881F1, &SkyLaunchKillDamageDetour, 6);
	g_SkyLaunchKillDamage_ReturnAddr = SkyLaunchKillDamageHook->GetReturnAddress();
	SkyLaunchKillDamageHook->Toggle(enable);

	// SkyLaunchKillDamageToCerberus
	// dmc3.exe + 10B7B6 - F3 0F 10 7A 0C - movss xmm7, [rdx + 0C]
	static std::unique_ptr<Utility::Detour_t> SkyLaunchKillDamageCerberusHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x10B7B6, &SkyLaunchKillDamageCerberusDetour, 5);
	g_SkyLaunchKillDamageCerberus_ReturnAddr = SkyLaunchKillDamageCerberusHook->GetReturnAddress();
	SkyLaunchKillDamageCerberusHook->Toggle(enable);

	// SkyLaunchKillDamageToShieldNevan
	// dmc3.exe+133D85 - F3 0F 11 83 24 02 00 00 - movss [rbx+00000224],xmm0
	// dmc3.exe+135097 - F3 0F11 8B 24020000   - movss [rbx+00000224],xmm1 -- also exists for damaging Shieded Nevan, but we won't use it here
	static std::unique_ptr<Utility::Detour_t> SkyLaunchKillDamageShieldNevanHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x133D85, &SkyLaunchKillDamageShieldNevanDetour, 8);
	g_SkyLaunchKillDamageShieldNevan_ReturnAddr = SkyLaunchKillDamageShieldNevanHook->GetReturnAddress();
	SkyLaunchKillDamageShieldNevanHook->Toggle(enable);

    run = enable;

}

void AirTauntDetours(bool enable) {
	using namespace Utility;
	static bool run = false;

	if (run == enable) {
		return;
	}

	// EnableAirTaunt
	static std::unique_ptr<Detour_t> enableAirTauntHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1E99EB, &EnableAirTauntDetour, 9);
	g_EnableAirTaunt_ReturnAddr = enableAirTauntHook->GetReturnAddress();
	enableAirTauntHook->Toggle(enable);
	g_EnableAirTaunt_ConditionalAddr = (uintptr_t)appBaseAddr + 0x1E9A53;
	g_EnableAirTaunt_ConditionalAddr2 = (uintptr_t)appBaseAddr + 0x1E9A0F;

	// SetAirTaunt
	static std::unique_ptr<Detour_t> setAirTauntHook = std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1E9A46, &SetAirTauntDetour, 5);
	g_SetAirTaunt_ReturnAddr = setAirTauntHook->GetReturnAddress();
	g_SetAirTaunt_Call = (uintptr_t)appBaseAddr + 0x1E09D0;
	setAirTauntHook->Toggle(enable);

    SkyLaunchDetours(enable);

    run = enable;
}

void RerouteRedOrbsCounterAlpha(bool enable, volatile uint16_t& alphaVar) {
    using namespace Utility;
    static bool run = false;
    
	if (run == enable) {
		return;
	}
	
    // dmc3.exe + 27E850 - 66 89 86 3C 69 00 00 - mov [rsi + 0000693C], ax { Red Orb Count Alpha Iterating on loop for fade out }
	// dmc3.exe + 27E85C - 66 89 9E 3C 69 00 00 - mov [rsi + 0000693C], bx { Red Orb Count Alpha Setting it to 0 }
	// dmc3.exe + 27E874 - 66 89 86 3C 69 00 00 - mov [rsi + 0000693C], ax { Red Orb Count Alpha Setting max alpha value (127) }
    // dmc3.exe + 27E830 - 0F B7 86 3C 69 00 00 - movzx eax, word ptr[rsi + 0000693C] { Red Orb Alpha Check if Fade Out is needed}

	static std::unique_ptr<Utility::Detour_t> RerouteRedOrbsCounterAlphaHook1 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x27E850, &RerouteRedOrbsCounterAlphaDetour1, 7);
	g_RerouteRedOrbsCounterAlpha_ReturnAddr1 = RerouteRedOrbsCounterAlphaHook1->GetReturnAddress();
    RerouteRedOrbsCounterAlphaHook1->Toggle(enable);

	static std::unique_ptr<Utility::Detour_t> RerouteRedOrbsCounterAlphaHook2 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x27E85C, &RerouteRedOrbsCounterAlphaDetour2, 7);
    g_RerouteRedOrbsCounterAlpha_ReturnAddr2 = RerouteRedOrbsCounterAlphaHook2->GetReturnAddress();
    RerouteRedOrbsCounterAlphaHook2->Toggle(enable);

	static std::unique_ptr<Utility::Detour_t> RerouteRedOrbsCounterAlphaHook3 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x27E874, &RerouteRedOrbsCounterAlphaDetour3, 7);
    g_RerouteRedOrbsCounterAlpha_ReturnAddr3 = RerouteRedOrbsCounterAlphaHook3->GetReturnAddress();
    RerouteRedOrbsCounterAlphaHook3->Toggle(enable);

	static std::unique_ptr<Utility::Detour_t> RerouteRedOrbsCounterAlphaHook4 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x27E830, &RerouteRedOrbsCounterAlphaDetour4, 7);
	g_RerouteRedOrbsCounterAlpha_ReturnAddr4 = RerouteRedOrbsCounterAlphaHook4->GetReturnAddress();
	RerouteRedOrbsCounterAlphaHook4->Toggle(enable);

    g_RerouteRedOrbsCounterAlpha_VariableAddr = (uintptr_t)&alphaVar;

    run = enable;
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

void ToggleCerberusCrashFix(bool enable) {
    using namespace Utility;
    static bool run = false;
    
	// If the function has already run in the current state, return early
	if (run == enable) {
		return;
	}

    //std::cout << (enable ? "Enabling CerberusFix\n" : "Disabling CerberusFix\n");

	// FixCrashCerberus
	// dmc3.exe+11793F - C6 40 05 01 - mov byte ptr [rax+05],01 - original code
	// we add test rax, rax here to ensure rax wont contain an invalid addr leading to a crash
	// when fighting Cerberus with multiple (4) actors
	static std::unique_ptr<Utility::Detour_t> FixCrashCerberusHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x11793F, &FixCrashCerberusDetour, 11);
	g_FixCrashCerberus_ReturnAddr = FixCrashCerberusHook->GetReturnAddress();
    g_FixCrashCerberus_PlayerStructAddr = (uintptr_t)appBaseAddr + 0xC90E28;

    FixCrashCerberusHook->Toggle(enable);
    CrimsonPatches::CerberusCrashFixPart2(enable);
  
	
    run = enable;
}

}
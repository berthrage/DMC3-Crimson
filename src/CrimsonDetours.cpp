// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include <algorithm>
#include "CrimsonEnemyAITarget.hpp"
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
#include "CrimsonLDK.hpp"
#include "CrimsonCameraController.hpp"
#include "Actor.hpp"


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

// FixCrashVergilM3
std::uint64_t g_FixCrashVergilM3_ReturnAddr;
std::uint64_t g_FixCrashVergilM3_JumpAddr;
void FixCrashVergilM3Detour();

// FixCrashM5
std::uint64_t g_FixCrashM5_ReturnAddr;
std::uint64_t g_FixCrashM5_JmpAddr;
void FixCrashM5Detour();

// HoldToCrazyCombo
std::uint64_t g_HoldToCrazyCombo_ReturnAddr;
void HoldToCrazyComboDetour();
std::uint64_t g_holdToCrazyComboConditionalAddr;
void* holdToCrazyComboCall;

// ConvertGuardBreakIntoNormalBlock: for DT-Infused Royalguard
std::uint64_t g_GuardBreakToNormalBlock_ReturnAddr;
void GuardBreakToNormalBlockDetour();
void* g_GuardBreakToNormalBlockCheckCall;

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

// HideStyleRankHUD
std::uint64_t g_HideStyleRankHUD_JumpAddr;
void HideStyleRankHUDDetour();

// HudHPSeparation
std::uint64_t g_HudHPSeparation_ReturnAddr;
void HudHPSeparationDetour();

// HudStyleBarPos
std::uint64_t g_HudStyleBarPos_ReturnAddr;
float g_HudStyleBarPosX;
float g_HudStyleBarPosY;
void HudStyleBarPosDetour();

// CustomCameraPositioning
std::uint64_t g_CustomCameraPos_ReturnAddr;
float* g_CustomCameraPos_NewPosAddr = nullptr;
void CustomCameraPositioningDetour();

// CustomCameraSensitivity
std::uint64_t g_CameraSensitivity_ReturnAddr1;
std::uint64_t g_CameraSensitivity_ReturnAddr2;
float* g_CameraSensitivity_NewSensAddr = nullptr;
void CameraSensitivityDetour1();
void CameraSensitivityDetour2();

// CamHittingWall
std::uint64_t g_CamHittingWall_ReturnAddr;
void CamHittingWallDetour();
bool* g_CamHittingWall_ConditionalAddr = nullptr;

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

// FreeformSoftLockHelper
std::uint64_t g_FreeformSoftLockHelper_ReturnAddr;
void FreeformSoftLockHelperDetour();

// DMC4/5LockOnDirection
std::uint64_t g_DMC4LockOnDirection_ReturnAddr;
void DMC4LockOnDirectionDetour();
void* g_DMC4LockOnDirectionCall;

// FasterTurnRate
std::uint64_t g_FasterTurnRate_ReturnAddr;
std::uint64_t g_FasterTurnRateCallAddr;
void FasterTurnRateDetour();

// FixFPSSpeedIssues
std::uint64_t g_FixFPSSpeedIssues_ReturnAddr;
void FixFPSSpeedIssuesDetour();
void* g_FixFPSSpeedIssuesCall;

// FixBallsHangHitSpeed
std::uint64_t g_FixBallsHangHitSpeed_ReturnAddr;
void FixBallsHangHitSpeedDetour();

// StyleRankHUDNoFadeout
std::uint64_t g_StyleRankHudNoFadeout_ReturnAddr;
void StyleRankHudNoFadeoutDetour();

// ShootRemapDown
std::uint64_t g_ShootRemapDown_ReturnAddr;
void ShootRemapDownDetour();
std::uint64_t g_ShootRemapHold_ReturnAddr;
//void ShootRemapHoldDetour();
std::uint16_t g_ShootRemap_NewMap;

// VergilNeutralTrick
std::uint64_t g_VergilNeutralTrick_ReturnAddr;
//void VergilNeutralTrickDetour();

// Artemis Instant Full Charge
void ArtemisInstantFullChargeDetour1();
std::uint64_t g_ArtemisReworkJumpAddr1;
void ArtemisInstantFullChargeDetour2();
std::uint64_t g_ArtemisReworkJumpAddr2;

// GreenOrbsMultiplayerRegen
void GreenOrbsMPRegenDetour();
std::uint64_t g_GreenOrbsMPRegen_ReturnAddr;
void* g_GreenOrbsMPRegen_Call;
}

bool g_HoldToCrazyComboFuncA(PlayerActorData& actorData) {
    using namespace ACTION_DANTE;

    auto playerIndex = CrimsonUtil::GetPlayerIndexFromAddr((uintptr_t)actorData.baseAddr); // simply using actorData.newPlayerIndex also works here.

    auto tiltDirection = GetRelativeTiltDirection(actorData);

    auto inputException = !(actorData.lockOn && (tiltDirection == TILT_DIRECTION::UP || tiltDirection == TILT_DIRECTION::DOWN));

    auto inputExceptionNevanJamSession = !(tiltDirection == TILT_DIRECTION::LEFT);

    // if the player ptr we fetched is a Clone then we use action/animTimers Clone, if not then use the normal ones instead.
    auto actionTimer =
        (actorData.newEntityIndex == 1) ? crimsonPlayer[playerIndex].actionTimerClone : crimsonPlayer[playerIndex].actionTimer;
    auto animTimer = (actorData.newEntityIndex == 1) ? crimsonPlayer[playerIndex].animTimerClone : crimsonPlayer[playerIndex].animTimer;


    switch (actorData.action) { // from vars, namespace ACTION_DANTE {

    case REBELLION_STINGER_LEVEL_1:
        if (std::clamp<float>(actionTimer, 0.22f, 0.3f) == actionTimer && inputException) {
            return true;
        }
        break;
    case REBELLION_STINGER_LEVEL_2:
        if (std::clamp<float>(actionTimer, 0.22f, 0.3f) == actionTimer && inputException) {
            return true;
        }
        break;
    case REBELLION_MILLION_STAB:
        if (std::clamp<float>(actionTimer, 0.22f, 10.0f) == actionTimer && inputException) {
            return true;
        }
        break;
    case REBELLION_COMBO_2_PART_2:
        if (std::clamp<float>(actionTimer, 0.0f, 0.90f) == actionTimer && inputException) {
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

	case CERBERUS_FLICKER:
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
				if (actorData.guard && actorData.eventData[0].event != ACTOR_EVENT::STAGGER) {
					if (actorData.magicPoints >= 1500) {
                        return true;
					}
					else {
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
            || activeCrimsonGameplay.Cheats.Dante.forceRoyalRelease) {
        return true;
    }

    return false;
}

bool DetectIfInSkyLaunch(PlayerActorData& actorData) {
	auto playerIndex = actorData.newPlayerIndex;
	auto gamepad = GetGamepad(playerIndex);

    if (activeCrimsonGameplay.Cheats.Training.infiniteHP) {
        return true;
    }

    if (actorData.character == CHARACTER::DANTE) {
		if (actorData.state & STATE::IN_AIR && gamepad.buttons[0] & GetBinding(BINDING::TAUNT)) {
			return true;
		}
    }

	return false;
}

uint16 ActorCameraDirectionToEnemyCameraDirection(PlayerActorData& actorData) {
	if (actorData.character != CHARACTER::DANTE && actorData.character != CHARACTER::VERGIL) return actorData.actorCameraDirection;
	if (!activeConfig.Actor.enable) return actorData.actorCameraDirection;
	auto playerIndex = actorData.newPlayerIndex;
	auto& rotationTowardsEnemy = (actorData.newEntityIndex == 1) ? crimsonPlayer[playerIndex].rotationCloneTowardsEnemy : crimsonPlayer[playerIndex].rotationTowardsEnemy;

	auto enemyCameraDirection = rotationTowardsEnemy - actorData.cameraDirection;
	return enemyCameraDirection;
}

float CalculateIdealTurnRateSpeed(PlayerActorData& actorData) {
	return actorData.speed * 1.9f;
}

void RegenerateMultiplayerPlayersHP(std::uint64_t hpRegen64) {
	auto hpRegen = *reinterpret_cast<float*>(&hpRegen64);

	for (uint8 playerIndex = 1; playerIndex < activeConfig.Actor.playerCount; playerIndex++) {
		auto& playerData = GetPlayerData(playerIndex);
		auto& characterData = GetCharacterData(playerIndex, playerData.characterIndex, ENTITY::MAIN);
		auto& newActorData = GetNewActorData(playerIndex, playerData.characterIndex, ENTITY::MAIN);

		if (!newActorData.baseAddr) {
			continue;
		}
		auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);

		if (!actorData.dead) {
			actorData.hitPoints += (hpRegen);
		}
	}
}

void InitDetours() {
    using namespace Utility;
    DetourBaseAddr = (uintptr_t)appBaseAddr;

	// cEnemySetCtrl__spawnGuy_sub_1401A4680
	//dmc3.exe+1A4680 - 40 57 - push rdi
	CameraCtrlInitDetour();
	CameraSwitchInitDetour();
	LdkInitDetour();

	// AddToMirageGauge
	//dmc3.exe + 1E0BB2 - F3 0F58 89 B83E0000 - addss xmm1, [rcx + 00003EB8] 
	//dmc3.exe + 1E0B8E - 80 B9 9B3E0000 01 - cmp byte ptr[rcx + 00003E9B], 01 - original code, 
    // comparing if DT is on or not to add DT, but still holding the value to add at xmm1
	static std::unique_ptr<Utility::Detour_t> AddToMirageGaugeHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1E0B8E, &AddToMirageGaugeDetour, 7);
	g_AddToMirageGauge_ReturnAddr = AddToMirageGaugeHook->GetReturnAddress();
	AddToMirageGaugeHook->Toggle(true);
	g_AddToMirageGaugeCall = &AddingToPlayersMirageGauge;

	// CameraHittingWallDetection
	// dmc3.exe+60301 - 45 8B F7              - mov r14d,r15d
	// dmc3.exe+60304 - 85 C0                 - test eax,eax { Camera Hitting Wall ? }
	static std::unique_ptr<Utility::Detour_t> CamHittingWallHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x60301, &CamHittingWallDetour, 5);
	g_CamHittingWall_ReturnAddr = CamHittingWallHook->GetReturnAddress();
	g_CamHittingWall_ConditionalAddr = &g_cameraHittingWall;
	CamHittingWallHook->Toggle(true);

    // CreateEffect
    createEffectCallA  = (uintptr_t)appBaseAddr + 0x2E7CA0;
    createEffectCallB  = (uintptr_t)appBaseAddr + 0x1FAA50;
    createEffectRBXMov = (uintptr_t)appBaseAddr + 0xC18AF8;

       
    // VergilNeutralTrick // func is already detoured, Crimson.MobilityFunction<27>+B1
    // static std::unique_ptr<Utility::Detour_t> VergilNeutralTrickHook = std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x0,
    // &VergilNeutralTrickDetour, 5); g_VergilNeutralTrick_ReturnAddr = VergilNeutralTrickHook->GetReturnAddress();
    // VergilNeutralTrickHook->Toggle(true);
}

void ToggleArtemisInstantFullCharge(bool enable) {
	using namespace Utility;
	static bool run = false;
	if (run == enable) {
		return;
	}
	// ArtemisInstantFullCharge

	// dmc3.exe + 215DE7 - 44 0F 2F C1 - comiss xmm8, xmm1
	// dmc3.exe + 215DEB - 0F 86 95 01 00 00 - jbe dmc3.exe + 215F86 --> jmp dmc3.exe + 215DF1
	// dmc3.exe + 215DF1 - 8B 8F 38 63 00 00 - mov ecx, [rdi + 00006338]
	static std::unique_ptr<Utility::Detour_t> ArtemisInstantFullChargeHook1 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x215DE7, &ArtemisInstantFullChargeDetour1, 4);
	g_ArtemisReworkJumpAddr1 = (uintptr_t)appBaseAddr + 0x215DF1;
	ArtemisInstantFullChargeHook1->Toggle(enable);

	// dmc3.exe + 215E3E - 44 0F 2F C0 - comiss xmm8,xmm0
	// dmc3.exe + 215E42 - 0F 82 00 00 00 00 - jb dmc3.exe + 215E48 --> jmp dmc3.exe + 215E51
	static std::unique_ptr<Utility::Detour_t> ArtemisInstantFullChargeHook2 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x215E3E, &ArtemisInstantFullChargeDetour2, 4);
	g_ArtemisReworkJumpAddr2 = (uintptr_t)appBaseAddr + 0x215E51;
	ArtemisInstantFullChargeHook2->Toggle(enable);

	// dmc3.exe + 215EA2 - 80 BC 38 E0C90100 07 - cmp byte ptr[rax + rdi + 0001C9E0], 07 { 7 }
	// dmc3.exe + 215EAA - 74 20 - je dmc3.exe+215ECC --> jmp dmc3.exe + 215EE2

	CrimsonPatches::ToggleIncreasedArtemisInstantChargeResponsiveness(enable);

	run = enable;
}

void ToggleGuardGravityAlteration(bool enable) {
	using namespace Utility;
	static bool run = false;
	if (run == enable) {
		return;
	}

	// GuardGravity
	static std::unique_ptr<Utility::Detour_t> guardGravityHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1EE121, &GuardGravityDetour, 7);
	g_GuardGravity_ReturnAddr = guardGravityHook->GetReturnAddress();
	guardGravityHook->Toggle(enable);

	run = enable;
}

void ToggleDisableDriveHold(bool enable) {
	using namespace Utility;
	static bool run = false;

	if (run == enable) {
		return;
	}

	// DisableDriveHold
	static std::unique_ptr<Utility::Detour_t> DisableDriveHoldHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1EB6F2, &DisableDriveHoldDetour, 5);
	g_DisableDriveHold_ReturnAddr = DisableDriveHoldHook->GetReturnAddress();
	DisableDriveHoldHook->Toggle(enable);

	run = enable;
}

void ToggleHideStyleRankHUD(bool enable) {
	using namespace Utility;
	static bool run = false;
	if (run == enable) {
		return;
	}
	// HideStyleRankHUD
	//dmc3.exe + 2BB194 - 0F 85 18 02 00 00 - jne dmc3.exe + 2BB3B2
	// goes to dmc3.exe+2BB195 - E9 18 02 00 00 - jmp dmc3.exe+2BB3B2
	static std::unique_ptr<Utility::Detour_t> HideStyleRankHUDHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2BB194, &HideStyleRankHUDDetour, 6);
	g_HideStyleRankHUD_JumpAddr = (uintptr_t)appBaseAddr + 0x2BB3B2;
	HideStyleRankHUDHook->Toggle(enable);
	run = enable;
}

void ToggleDTInfusedRoyalguardDetours(bool enable) {
	using namespace Utility;
	static bool run = false;

	if (run == enable) {
		return;
	}

	// DisableStaggerRoyalguard
	static std::unique_ptr<Utility::Detour_t> DisableStaggerRoyalguardHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1EC464, &DisableStaggerRoyalguardDetour, 9);
	g_DisableStagger_ReturnAddr = DisableStaggerRoyalguardHook->GetReturnAddress();
	g_DisableStagger_ConditionalAddr = (uintptr_t)appBaseAddr + 0x1EC58B;
	DisableStaggerRoyalguardHook->Toggle(false);
	//dmc3.exe + 1EC464 - E9 22010000 - jmp dmc3.exe + 1EC58B
	//dmc3.exe + 1EC467 - 0F8E 1E010000 - jng dmc3.exe + 1EC58B - original code
	g_DisableStaggerCheckCall = &DisableStaggerCheck;

	// GuardBreakToNormalBlock
	// dmc3.exe+1EC45A - 89 83 08 63 00 00 - mov [rbx+00006308],eax { Determine stagger dmg (on eax) to royal block }
	static std::unique_ptr<Utility::Detour_t> GuardBreakToNormalBlockHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1EC45A, &GuardBreakToNormalBlockDetour, 6);
	g_GuardBreakToNormalBlock_ReturnAddr = GuardBreakToNormalBlockHook->GetReturnAddress();
	GuardBreakToNormalBlockHook->Toggle(enable);

	// Toggle Take Damage
	static std::unique_ptr<Utility::Detour_t> takeDamageHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x8851C, &ToggleTakeDamageDetour, 5);
	g_ToggleTakeDamage_ReturnAddr = takeDamageHook->GetReturnAddress();
	takeDamageHook->Toggle(enable);
	g_ToggleTakeDamageCheckCall = &TakeDamageCheck;

	run = enable;
}

//void ToggleShootRemapDown(bool enable) {
//	using namespace Utility;
//	static bool run = false;
//
//	if (run == enable) {
//		return;
//	}
//
//	// ---------- INJECTING HERE ----------
//	//dmc3.exe + 1E6A62: 0F B7 44 45 0A - movzx eax, word ptr[rbp + rax * 2 + 0A]
//	// ---------- DONE INJECTING  ----------
//	//dmc3.exe + 1E6A67 : 66 85 83 E4 74 00 00 - test[rbx + 000074E4], ax
//	//dmc3.exe + 1E6A6E : 75 0A - jne dmc3.exe + 1E6A7A
//	static std::unique_ptr<Utility::Detour_t> ShootRemapDownHook =
//		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1E6A62, &ShootRemapDownDetour, 5);
//	g_ShootRemapDown_ReturnAddr = ShootRemapDownHook->GetReturnAddress();
//	ShootRemapDownHook->Toggle(enable);
//
//	run = enable;
//}

void ToggleFasterTurnRate(bool enable) {
	using namespace Utility;
	static bool run = false;

	if (run == enable) {
		return;
	}

	// FasterTurnRate
	// dmc3.exe + 1FC5D5 - E8 D6 13 13 00 - call dmc3.exe+32D9B0
	// dmc3.exe + 1FC5DA - 44 0F B7 0F - movzx r9d,word ptr [rdi]
	// dmc3.exe + 1FC5DE - 44 0F B7 D0 - movzx r10d, ax { value in ax (return from call) holds turn rate speed }
	static std::unique_ptr<Utility::Detour_t> FasterTurnRateHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1FC5D5, &FasterTurnRateDetour, 13);
	g_FasterTurnRate_ReturnAddr = FasterTurnRateHook->GetReturnAddress();
	g_FasterTurnRateCallAddr = (uintptr_t)appBaseAddr + 0x32D9B0;
	FasterTurnRateHook->Toggle(enable);

	run = enable;
}

void ToggleFreeformSoftLockHelper(bool enable) {
	using namespace Utility;
	static bool run = false;

	if (run == enable) {
		return;
	}

	// FreeformSoftLockHelper
	// dmc3.exe + 1FA509 - 66 41 03 CB - add cx,r11w { Final step to Set Rotation Towards Enemy }
	// dmc3.exe + 1FA50D - 66 41 89 88 C0 00 00 00 - mov[r8 + 000000C0], cx{ Set Character Rotation when doing directional move }
	static std::unique_ptr<Utility::Detour_t> FreeformSoftLockHelperHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1FA50D, &FreeformSoftLockHelperDetour, 8);
	g_FreeformSoftLockHelper_ReturnAddr = FreeformSoftLockHelperHook->GetReturnAddress();
	FreeformSoftLockHelperHook->Toggle(enable);

	run = enable;
}

void ToggleDMC4LockOnDirection(bool enable) {
	using namespace Utility;
	static bool run = false;

	if (run == enable) {
		return;
	}

	// DMC4/5LockOnDirection
	// relativeTilt = actorCameraDirection - leftStickPosition
	// actorCameraDirection = currentRotation - cameraDirection
	// dmc3.exe + 1EBF42 - 66 89 83 0C 75 00 00 - mov[rbx + 0000750C], ax{ Set actorCameraDirection }
	static std::unique_ptr<Utility::Detour_t> DMC4LockOnDirectionHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1EBF42, &DMC4LockOnDirectionDetour, 7);
	g_DMC4LockOnDirection_ReturnAddr = DMC4LockOnDirectionHook->GetReturnAddress();
	DMC4LockOnDirectionHook->Toggle(enable);
	g_DMC4LockOnDirectionCall = &ActorCameraDirectionToEnemyCameraDirection;

	run = enable;
}

void ToggleCustomCameraPositioning(bool enable) {
	using namespace Utility;
	static bool run = false;

	if (run == enable) {
		return;
	}

	// CustomCameraPositioning
	// dmc3.exe+571BC - 0F 28 82 80 00 00 00 - movaps xmm0,[rdx+00000080] { Determine camera's pos by player pos, player in rdx }
	static std::unique_ptr<Utility::Detour_t> CustomCameraPositioningHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x571BC, &CustomCameraPositioningDetour, 7);
	g_CustomCameraPos_ReturnAddr = CustomCameraPositioningHook->GetReturnAddress();
	g_CustomCameraPos_NewPosAddr = g_customCameraPos;
	CustomCameraPositioningHook->Toggle(enable);

    run = enable;
}

void ToggleCustomCameraSensitivity(bool enable) {
	using namespace Utility;
	static bool run = false;
	// This detour isn't used anymore due to causing issues with Low/Default FollowUpSpeed

	if (run == enable) {
		return;
	}

	// CameraSensitivity
	// dmc3.exe + 5772F - C7 87 D4 01 00 00 56 77 56 3D - mov[rdi + 000001D4], 3D567756{ (0) } // not default values, they are 'High' values
	// dmc3.exe + 5775B - C7 87 D4 01 00 00 56 77 56 3D - mov[rdi + 000001D4], 3D567756{ (0) }
	static std::unique_ptr<Utility::Detour_t> CameraSensitivityHook1 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x5772F, &CameraSensitivityDetour1, 10);
	static std::unique_ptr<Utility::Detour_t> CameraSensitivityHook2 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x5775B, &CameraSensitivityDetour2, 10);
	g_CameraSensitivity_ReturnAddr1 = CameraSensitivityHook1->GetReturnAddress();
	g_CameraSensitivity_ReturnAddr2 = CameraSensitivityHook2->GetReturnAddress();
	g_CameraSensitivity_NewSensAddr = &g_customCameraSensitivity;
	CameraSensitivityHook1->Toggle(enable);
	CameraSensitivityHook2->Toggle(enable);

	run = enable;
}


void ToggleHoldToCrazyCombo(bool enable) {
    using namespace Utility;
    static bool run = false;
	static uint8 previousCComboMashRequirement = activeCrimsonGameplay.Gameplay.General.crazyComboMashRequirement;

	if (run == enable) {
		return;
	}

	// HoldToCrazyCombo
	static std::unique_ptr<Utility::Detour_t> HoldToCrazyComboHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1EB7C5, &HoldToCrazyComboDetour, 12);
	g_HoldToCrazyCombo_ReturnAddr = HoldToCrazyComboHook->GetReturnAddress();
	g_holdToCrazyComboConditionalAddr = (uintptr_t)appBaseAddr + 0x1EB7FE;
	HoldToCrazyComboHook->Toggle(enable);
	holdToCrazyComboCall = &g_HoldToCrazyComboFuncA;

	if (enable) {
		activeCrimsonGameplay.Gameplay.General.crazyComboMashRequirement = 3;
		queuedCrimsonGameplay.Gameplay.General.crazyComboMashRequirement = 3;
		UpdateCrazyComboLevelMultiplier();
	} else {
		activeCrimsonGameplay.Gameplay.General.crazyComboMashRequirement = previousCComboMashRequirement;
		queuedCrimsonGameplay.Gameplay.General.crazyComboMashRequirement = previousCComboMashRequirement;
		UpdateCrazyComboLevelMultiplier();
	}

	previousCComboMashRequirement = activeCrimsonGameplay.Gameplay.General.crazyComboMashRequirement;
    run = enable;
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

void ToggleGreenOrbsMPRegen(bool enable) {
	using namespace Utility;
	static bool run = false;

	if (run == enable) {
		return;
	}

	// GreenOrbsMPRegen
	// Player ptr in RDX
	// dmc3.exe + 1B6E29 - F3 0F 58 82 1C 41 00 00   - addss xmm0,[rdx+0000411C] { Adding to player's hp value (green orb regen)
	// dmc3.exe + 1B6E34 - F3 0F 11 82 1C 41 00 00 - movss[rdx + 0000411C], xmm0{ Regenerated using green orbs }
	static std::unique_ptr<Utility::Detour_t> GreenOrbsMPRegenHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1B6E29, &GreenOrbsMPRegenDetour, 8);
	g_GreenOrbsMPRegen_ReturnAddr = GreenOrbsMPRegenHook->GetReturnAddress();
	g_GreenOrbsMPRegen_Call = &RegenerateMultiplayerPlayersHP;
	GreenOrbsMPRegenHook->Toggle(enable);

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

void ToggleFPSSpeedIssues(bool enable) {
	using namespace Utility;
	static bool run = false;
	if (run == enable) {
		return;
	}

	// FixFPSIssues
	// dmc3.exe+3261C3 - E8 98 07 00 00           - call dmc3.exe+326960
	// dmc3.exe+3261C8 - F3 0F 59 43 18         - mulss xmm0,[rbx+18] <-- Our detour here
	// dmc3.exe+3261CD - F3 0F 11 43 14         - movss [rbx+14],xmm0 <-- UpdateActorSpeed detours here (Actor.cpp)
// 	static std::unique_ptr<Utility::Detour_t> FixFPSSpeedIssuesHook =
// 		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x3261C8, &FixFPSSpeedIssuesDetour, 5);
// 	g_FixFPSSpeedIssues_ReturnAddr = FixFPSSpeedIssuesHook->GetReturnAddress();
// 	FixFPSSpeedIssuesHook->Toggle(enable);
// 	run = enable;
}

void ToggleFixBallsHangHitSpeed(bool enable) {
	using namespace Utility;
	static bool run = false;
	if (run == enable) {
		return;
	}
	// FixBallsHangHitSpeed
	// dmc3.exe + 25C2DD - F3 0F 10 5E B4 - movss xmm3, [rsi - 4C]

	static std::unique_ptr<Utility::Detour_t> FixBallsHangHitSpeedHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x25C2DD, &FixBallsHangHitSpeedDetour, 5);
	g_FixBallsHangHitSpeed_ReturnAddr = FixBallsHangHitSpeedHook->GetReturnAddress();
	FixBallsHangHitSpeedHook->Toggle(enable);
	run = enable;
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

void ToggleVergilM3CrashFix(bool enable) {
	using namespace Utility;
	static bool run = false;

	// If the function has already run in the current state, return early
	if (run == enable) {
		return;
	}

	// FixCrashVergilM3
	// dmc3.exe+324B45 - 66 44 3B 20 - cmp r12w,[rax]  <-- rax is getting null sometimes, so we do a null check before
	// dmc3.exe+324B49 - 7C 3E  - jl dmc3.exe+324B89
	static std::unique_ptr<Utility::Detour_t> FixCrashVergilM3Hook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x324B45, &FixCrashVergilM3Detour, 6);
	g_FixCrashVergilM3_ReturnAddr = FixCrashVergilM3Hook->GetReturnAddress();
	g_FixCrashVergilM3_JumpAddr = (uintptr_t)appBaseAddr + 0x324B89;

	FixCrashVergilM3Hook->Toggle(enable);

	run = enable;
}

void ToggleMission5CrashFix(bool enable) {
	using namespace Utility;
	static bool run = false;
	// If the function has already run in the current state, return early
	if (run == enable) {
		return;
	}

	// FixCrashM5Detour
	// dmc3.exe+5A422 - 39 42 04              - cmp [rdx+04],eax
	// dmc3.exe+5A425 - 72 0F                 - jb dmc3.exe+5A436
	static std::unique_ptr<Utility::Detour_t> FixMission5CrashHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x5A422, &FixCrashM5Detour, 5);
	g_FixCrashM5_ReturnAddr = FixMission5CrashHook->GetReturnAddress();
	g_FixCrashM5_JmpAddr = (uintptr_t)appBaseAddr + 0x5A436; // Jump to the next instruction after the detour
	FixMission5CrashHook->Toggle(enable);
	run = enable;
}

}
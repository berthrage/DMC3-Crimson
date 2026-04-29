// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include <algorithm>
#include <windows.h>
#include <thread>
#include <chrono>
#include "CrimsonEnemyAITarget.hpp"
#include "Core/Core.hpp"
#include <stdio.h>
#include "Utility/Detour.hpp"
#include <intrin.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
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
#include "CrimsonOnTick.hpp"
#include "CrimsonEfk.hpp"
#include "Internal.hpp"
#include "CrimsonDetours.hpp"

namespace CrimsonDetours {

extern "C" {
std::uint64_t g_appBaseAddr;

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
void* g_SetAirTauntSkyLaunchCheckCall;
void* g_SetAirTauntATRisingSunCheckCall;

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
std::uint64_t g_FixCrashM5_ReturnAddr2;
std::uint64_t g_FixCrashM5_JmpAddr2;
void FixCrashM5Detour();
void FixCrashM5Detour2();

//Cerb Damage fix
std::uint64_t g_CerbDamageFix_ReturnAddr;
std::uint64_t g_CerbDamageFix_JmpAddr;
void CerbDamageFixDetour();

// FixCrashArkhamPt2Grab
std::uint64_t g_FixCrashArkhamPt2Grab_ReturnAddr1;
void FixCrashArkhamPt2GrabDetour1();
std::uint64_t g_FixCrashArkhamPt2Grab_ReturnAddr2;
void FixCrashArkhamPt2GrabDetour2();
std::uint64_t g_FixCrashArkhamPt2Grab_ReturnAddr3;
void FixCrashArkhamPt2GrabDetour3();
std::uint64_t g_FixCrashArkhamPt2Grab_ReturnAddr4;
void FixCrashArkhamPt2GrabDetour4();
std::uint64_t g_FixCrashArkhamPt2Grab_ReturnAddr5;
void FixCrashArkhamPt2GrabDetour5();

// FixCrashArkhamPt2Doppel
std::uint64_t g_FixCrashArkhamPt2Doppel_ReturnAddr1;
std::uint64_t g_FixCrashArkhamPt2Doppel_CallAddr1;
void FixCrashArkhamPt2DoppelDetour1();
std::uint64_t g_FixCrashArkhamPt2Doppel_ReturnAddr2;
std::uint64_t g_FixCrashArkhamPt2Doppel_CallAddr2;
void FixCrashArkhamPt2DoppelDetour2();

// EnsureAirRisingDragonLaunch
std::uint64_t g_EnsureAirRisingDragonLaunch_ReturnAddr;
std::uint64_t g_EnsureAirRisingDragonLaunch_JmpAddr;
void EnsureAirRisingDragonLaunchDetour();

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

// ConfirmSetAction
std::uint64_t g_ConfirmSetAction_ReturnAddr;
std::uint64_t g_ConfirmSetAction_FuncCall;
void ConfirmSetActionDetour();
void* g_ConfirmSetActionCheckCall;

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

// InterceptCollisions
std::uint64_t g_InterceptCollisions_ReturnAddr;
void InterceptCollisionsDetour();
void* g_DrawCollisionsCall;
void* g_InterceptCollisionsCall;

// FasterTurnRate
std::uint64_t g_TurnRateFix_ReturnAddr;
void TurnRateFixDetour();

// FixFPSSpeedIssues
std::uint64_t g_FixFPSSpeedIssues_ReturnAddr;
void FixFPSSpeedIssuesDetour();
void* g_FixFPSSpeedIssuesCall;

// FixBallsHangHitSpeed
std::uint64_t g_FixBallsHangHitSpeed_ReturnAddr;
void FixBallsHangHitSpeedDetour();

// FixSecretMissionTimerFPS
std::uint64_t g_FixSecretMissionTimerFPS_ReturnAddr;
void FixSecretMissionTimerFPSDetour();
std::uint64_t g_FixSecretMissionTimerFPS_ReturnAddr2;
void FixSecretMissionTimerFPSDetour2();

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

// PortalsHide
void PortalsHideDetour();
std::uint64_t g_PortalsHide_ReturnAddr;

// PortalsMute
void PortalsMuteDetour();
std::uint64_t g_PortalsMute_ReturnAddr;
// StyleLevellingCCSFix
void StyleLevellingCCSFixDetour1();
std::uint64_t g_StyleLevellingCCSFix_ReturnAddr1;
void* g_StyleLevellingCCSFix_CheckCall1;
void StyleLevellingCCSFixDetour2();
std::uint64_t g_StyleLevellingCCSFix_ReturnAddr2;
void* g_StyleLevellingCCSFix_CheckCall2;

// DanteTrickAlterations
std::uint64_t g_DanteTrickAlter_ReturnAddr1;
void DanteTrickAlterationsDetour1();
std::uint64_t g_DanteTrickAlter_ReturnAddr2;
void DanteTrickAlterationsDetour2();
std::uint64_t g_DanteTrickAlter_ReturnAddr3;
void DanteTrickAlterationsDetour3();
std::uint64_t g_DanteTrickAlter_ReturnAddr4;
void DanteTrickAlterationsDetour4();

// ShotgunShlSpawnAnglePointBlank
std::uint64_t g_ShotgunShlSpawnAnglePointBlank_ReturnAddr1;
void ShotgunShlSpawnAnglePointBlankDetour();
std::uint64_t g_ShotgunShlSpawnAnglePointBlank_ReturnAddr2;
void ShotgunShlSpawnAnglePointBlankDetour2();
void* g_ShotgunShlSpawnAnglePointBlankCheckCall;
std::uint64_t g_ShotgunShlSpawnAnglePointBlankLockedOnEnemyJmp;

// PointBlankShotgunFire
std::uint64_t g_PointBlankShotgunFire_ReturnAddr;
void PointBlankShotgunFireDetour();
std::uint64_t g_PointBlankShotgunFireOgCall;
std::uint64_t g_PointBlankShotgunFireTailCall_ReturnAddr;
void PointBlankShotgunFireTailCallDetour();
void* g_PointBlankShotgunFireDelayCall;
std::uint64_t g_PointBlankShotgunCancelAnimTailCall;

// JudgementCutDetours
std::uint64_t g_JudgementCutSpeed_ReturnAddr;
void JudgementCutSpeedDetour();
std::uint64_t g_JudgementCutStartDelayCall;
std::uint64_t g_JudgementCutSpawnCollisionCall;
void* g_JudgementCutCheckJustFrameCall;

std::uint64_t g_JudgementCutVFX_ReturnAddr;
std::uint64_t g_JudgementCutVFX_ReturnAddr2;
void JudgementCutVFXDetour();
std::uint64_t g_JudgementCutRegularVFXCall;
void* g_JudgementCutJustFrameVFXCall;

std::uint64_t g_JudgementCutPosition_ReturnAddr;
void JudgementCutPositionDetour();
void* g_JudgementCutSetPositionCall;

std::uint64_t g_JudgementCutExtraShl_ReturnAddr;
void JudgementCutExtraShlDetour();
void* g_JudgementCutExtraShlCall;

// DTMustStyleArmor
std::uint64_t g_DTMustStyleArmor_ReturnAddr;
void DTMustStyleArmorDetour();
void* g_DTMustStyleArmor_CheckCall1;
void* g_DTMustStyleArmor_CheckCall2;
}

bool g_HoldToCrazyComboFuncA(PlayerActorData& actorData) {
    using namespace ACTION_DANTE;

    auto playerIndex = CrimsonUtil::GetPlayerIndexFromAddr((uintptr_t)actorData.baseAddr); // simply using actorData.newPlayerIndex also works here.
	auto entityIndex = actorData.newEntityIndex;

    auto tiltDirection = GetRelativeTiltDirection(actorData);

    auto inputException = !(actorData.lockOn && (tiltDirection == TILT_DIRECTION::UP || tiltDirection == TILT_DIRECTION::DOWN));

    auto inputExceptionNevanJamSession = !(tiltDirection == TILT_DIRECTION::LEFT);
	auto& gamepad = GetGamepad(playerIndex);

    // if the player ptr we fetched is a Clone then we use action/animTimers Clone, if not then use the normal ones instead.
	auto actionTimer =
		(entityIndex == ENTITY::MAIN) ? crimsonPlayer[playerIndex].actionTimer : crimsonPlayer[playerIndex].actionTimerClone;
	auto motionTimer =
		(entityIndex == ENTITY::MAIN) ? crimsonPlayer[playerIndex].motionTimer : crimsonPlayer[playerIndex].motionTimerClone;
	auto motionIndex = actorData.motionData[0].index;
	auto& stingerInput = (entityIndex == ENTITY::MAIN) ? crimsonPlayer[playerIndex].stingerInput : crimsonPlayer[playerIndex].stingerInputClone;
	auto& delayedComboFX = (entityIndex == ENTITY::MAIN) ? crimsonPlayer[playerIndex].delayedComboFX : crimsonPlayer[playerIndex].delayedComboFXClone;


    switch (actorData.action) { // from vars, namespace ACTION_DANTE {

    case REBELLION_STINGER_LEVEL_1:
        if (motionIndex == 16 && stingerInput.meleeButtonHold >= 0.1f && !stingerInput.meleeReleasedStinger && inputException) {
            return true;
        }
        break;
    case REBELLION_STINGER_LEVEL_2:
        if (motionIndex == 42 && stingerInput.meleeButtonHold >= 0.17f && !stingerInput.meleeReleasedStinger && inputException) {
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
        if (std::clamp<float>(motionTimer, 0.5f, 1.0f) == motionTimer && inputException) {
			if (motionIndex != 27) delayedComboFX.transitioningToHyperFist = true;
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

bool CheckSkyLaunchEnabled() {
	return activeCrimsonGameplay.Gameplay.General.extramoves && ExpConfig::missionExpDataDante.unlocks[UNLOCK_DANTE::SKY_LAUNCH_AIR_TAUNT];
}

bool CheckAirTauntRisingSunEnabled() {
	return activeCrimsonGameplay.Gameplay.General.extramoves && ExpConfig::missionExpDataVergil.unlocks[UNLOCK_VERGIL::RISING_SUN_AIR_TAUNT];
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

void StyleLevel1Fix(uintptr_t playerAddr) {
	auto& actorData = *reinterpret_cast<PlayerActorData*>(playerAddr);
	auto& character = actorData.character;
// 	ExpConfig& expData = (character == CHARACTER::DANTE) 
// 		? ExpConfig::missionExpDataDante
// 		: ExpConfig::missionExpDataVergil;
	HeldStyleExpData& heldStyleExpData = (character == CHARACTER::DANTE)
		? heldStyleExpDataDante
		: heldStyleExpDataVergil;
	//expData.styleLevels[helper.styleid] = helper.stylelevel;
	heldStyleExpData.missionStyleLevels[actorData.style] = 1;
}

void StyleLevel2Fix(uintptr_t playerAddr) {
	auto& actorData = *reinterpret_cast<PlayerActorData*>(playerAddr);
	auto& character = actorData.character;
	// 	ExpConfig& expData = (character == CHARACTER::DANTE) 
	// 		? ExpConfig::missionExpDataDante
	// 		: ExpConfig::missionExpDataVergil;
	HeldStyleExpData& heldStyleExpData = (character == CHARACTER::DANTE)
		? heldStyleExpDataDante
		: heldStyleExpDataVergil;
	//expData.styleLevels[helper.styleid] = helper.stylelevel;
	heldStyleExpData.missionStyleLevels[actorData.style] = 2;
}

bool CheckIfInMustStyle() {
	return activeCrimsonGameplay.Gameplay.ExtraDifficulty.mustStyleMode > STYLE_RANK::NONE;
}

void SetAnnouncerWasHit() {
	for (int rankId = 0; rankId < 7; rankId++) {
		rankAnnouncer[rankId].wasHit = true;
	}
}

bool CheckIfInBackslide(uintptr_t playerAddr) {
	auto& actorData = *reinterpret_cast<PlayerActorData*>(playerAddr);
	if (actorData.character != CHARACTER::DANTE) return false;
	auto playerIndex = actorData.newPlayerIndex;
	auto& backslide = (actorData.newEntityIndex == ENTITY::MAIN) ? crimsonPlayer[actorData.newPlayerIndex].backslide
		: crimsonPlayer[actorData.newPlayerIndex].backslideClone;

	if (backslide.performing) {
		return true;
	}
	return false;
}

bool CheckIfInJustFrameJDC(uintptr_t playerAddr) {
	auto& actorData = *reinterpret_cast<PlayerActorData*>(playerAddr);
	if (actorData.character != CHARACTER::VERGIL) return false;
	auto playerIndex = actorData.newPlayerIndex;
	auto& jCut = (actorData.newEntityIndex == ENTITY::MAIN) ? crimsonPlayer[actorData.newPlayerIndex].jCut
		: crimsonPlayer[actorData.newPlayerIndex].jCutClone;
	if (jCut.inJustFrameJDC) {
		return true;
	}
	return false;
}

static constexpr uintptr_t PLAYVFX_OFFSET() { return 0x2E7CA0; }

using PlayVFX_t = uintptr_t(__fastcall*)(int group, uint16 index, uintptr_t matrixPtr, int a4);

static uintptr_t PlayVFX_sub_1402E7CA0(int group, uint16 index, uintptr_t matrixPtr, int a4) {
    PlayVFX_t PlayVFXFunc = reinterpret_cast<PlayVFX_t>(appBaseAddr + PLAYVFX_OFFSET());
	if (!PlayVFXFunc) {
		return NULL;
	}

	return PlayVFXFunc(group, index, matrixPtr, a4);
}

static std::unordered_set<uintptr_t> s_suppressJustFrameVFXForShl;

uintptr_t PlayJustFrameJDCVFX(uintptr_t shlAddr) {
	auto& shlActorData = *reinterpret_cast<CPl021Shl02Actor*>(shlAddr - 0x60);
  auto shlActorBaseAddr = shlAddr - 0x60;

	if (s_suppressJustFrameVFXForShl.find(shlActorBaseAddr) != s_suppressJustFrameVFXForShl.end()) {
		float fakeVFXMatrix[16] = { 0 };
		std::memcpy(fakeVFXMatrix, shlActorData.matrix, sizeof(float) * 16);
		fakeVFXMatrix[13] -= 10000000.0f;
		shlActorData.CGeneratorPtr = PlayVFX_sub_1402E7CA0(2, 456, (uintptr_t)&fakeVFXMatrix, 0);
		return shlActorData.CGeneratorPtr;
	}

	static constexpr const wchar_t* justFrameVFXPath = L"Crimson\\vfx\\judgementcut\\justframejdc.efkefc";
	EffekseerRefHandle justFrameVFXRef = CrimsonEfk::LoadEffect(justFrameVFXPath, 40.0f);

	EffekseerHandle handle = CrimsonEfk::PlayEffectAtMatrix(justFrameVFXRef, shlActorData.matrix, NULL);

	// Spawn Default JDC VFX for the JDCs CGenerator not to be NULL
	// Default JDC VFX is group 2 index 456

	float fakeVFXMatrix[16] = { 0 };
	std::memcpy(fakeVFXMatrix, shlActorData.matrix, sizeof(float) * 16);
	if (!activeCrimsonConfig.VFX.originalJDCReference) fakeVFXMatrix[13] -= 10000000.0f;  // Make it go out of bounds
	shlActorData.CGeneratorPtr = PlayVFX_sub_1402E7CA0(2, 456, (uintptr_t)&fakeVFXMatrix, 0);
	if (shlActorData.CGeneratorPtr) {
		auto& cGenerator = *reinterpret_cast<CGenerator*>(shlActorData.CGeneratorPtr);
		cGenerator.color = 0xFF36EDFA;
		cGenerator.speed = 1.5f;
	}

	return shlActorData.CGeneratorPtr;
}

struct JdcSourceKey {
	uintptr_t ownerPlayerActorAddr = 0;
	uintptr_t sourceAddr = 0;

	bool operator==(const JdcSourceKey& other) const {
		return ownerPlayerActorAddr == other.ownerPlayerActorAddr && sourceAddr == other.sourceAddr;
	}
};

struct JdcSourceKeyHash {
	std::size_t operator()(const JdcSourceKey& key) const {
		return std::hash<uintptr_t>{}(key.ownerPlayerActorAddr) ^ (std::hash<uintptr_t>{}(key.sourceAddr) << 1);
	}
};

static std::unordered_map<JdcSourceKey, std::chrono::steady_clock::time_point, JdcSourceKeyHash> s_pendingSinceBySource;
static std::unordered_map<JdcSourceKey, uint8, JdcSourceKeyHash> s_spawnedExtraCountBySource;
static std::unordered_map<JdcSourceKey, vec3, JdcSourceKeyHash> s_primaryPosBySource;
static std::unordered_map<JdcSourceKey, vec3, JdcSourceKeyHash> s_fixedExtraPosBySource;
static std::unordered_map<uintptr_t, JdcSourceKey> s_extraAddrToSource;

void SetJDCPositionAtMatrix(uintptr_t shlAddr) {
	auto& shlActorData = *reinterpret_cast<CPl021Shl02Actor*>(shlAddr - 0x60);
	auto& actorData = *reinterpret_cast<PlayerActorData*>(shlActorData.playerActorAddr);
	auto shlActorBaseAddr = shlAddr - 0x60;
	const bool isJustFrameJdc = CheckIfInJustFrameJDC(shlActorData.playerActorAddr);

	if (isJustFrameJdc) {
		auto extraIt = s_extraAddrToSource.find(shlActorBaseAddr);
		if (extraIt != s_extraAddrToSource.end()) {
			auto fixedPosIt = s_fixedExtraPosBySource.find(extraIt->second);
			if (fixedPosIt != s_fixedExtraPosBySource.end()) {
				shlActorData.position.x = fixedPosIt->second.x;
				shlActorData.position.y = fixedPosIt->second.y;
				shlActorData.position.z = fixedPosIt->second.z;
				shlActorData.matrix[12] = shlActorData.position.x;
				shlActorData.matrix[13] = shlActorData.position.y;
				shlActorData.matrix[14] = shlActorData.position.z;
				return;
			}

			s_extraAddrToSource.erase(extraIt);
		}
	}

	auto SpawnJDCInFrontOfPlayer = [](CPl021Shl02Actor& shlActorData, PlayerActorData& actorData) {
		constexpr float jdcForwardOffset = 800.0f;
		constexpr float rotationToRadians = 6.28318530717958647692f / 65536.0f;

		const float yaw = static_cast<float>(actorData.rotation) * rotationToRadians;
		const float forwardX = std::sin(yaw);
		const float forwardZ = std::cos(yaw);

		shlActorData.position.x = actorData.position.x + forwardX * jdcForwardOffset;
		shlActorData.position.y = actorData.position.y + 120.0f;
		shlActorData.position.z = actorData.position.z + forwardZ * jdcForwardOffset;
		};

	if (actorData.lockOnData.targetBaseAddr60) {
		float heightOffset = 120.0f;
		if (actorData.state & STATE::IN_AIR) {
			heightOffset = 0.0f; // slightly higher spawn for airborne JDCs to avoid ground collision and look better visually
		}
		auto& enemyData = *reinterpret_cast<EnemyActorData*>(actorData.lockOnData.targetBaseAddr60 - 0x60);
		shlActorData.position.x = enemyData.position.x;
		shlActorData.position.y = enemyData.position.y + heightOffset;
		shlActorData.position.z = enemyData.position.z;
	}
	else {
		SpawnJDCInFrontOfPlayer(shlActorData, actorData);
	}

	shlActorData.matrix[12] = shlActorData.position.x;
	shlActorData.matrix[13] = shlActorData.position.y;
	shlActorData.matrix[14] = shlActorData.position.z;

	if (isJustFrameJdc) {
		JdcSourceKey sourceKey{ shlActorData.playerActorAddr, shlActorBaseAddr };
		s_primaryPosBySource[sourceKey] = {
			shlActorData.position.x,
			shlActorData.position.y,
			shlActorData.position.z
		};
	}
}

static constexpr uintptr_t SPAWNCOLLISION_OFFSET() { return 0x5C320; }

using SpawnCollision_t = uintptr_t(__fastcall*)(uintptr_t collisionDataStruct, uint8 a2);

static uintptr_t SpawnCollision_sub_14005C320(uintptr_t collisionDataStruct, uint8 a2) {
	SpawnCollision_t SpawnCollisionFunc = reinterpret_cast<SpawnCollision_t>(appBaseAddr + SPAWNCOLLISION_OFFSET());
	if (!SpawnCollisionFunc) {
		return NULL;
	}

	return SpawnCollisionFunc(collisionDataStruct, a2);
}

static constexpr uintptr_t SETJDCPOSITION_OFFSET() { return 0x1DC1A0; }

using SetJDCPosition_t = uintptr_t(__fastcall*)(uintptr_t posPtr, uintptr_t matrixPtr, uintptr_t playerActorAddr, uint8 a4);

static uintptr_t SetJDCPosition_sub_1401DC1A0(uintptr_t posPtr, uintptr_t matrixPtr, uintptr_t playerActorAddr, uint8 a4) {
	SetJDCPosition_t SetJDCPositionFunc = reinterpret_cast<SetJDCPosition_t>(appBaseAddr + SETJDCPOSITION_OFFSET());
	if (!SetJDCPositionFunc) {
		return NULL;
	}

	return SetJDCPositionFunc(posPtr, matrixPtr, playerActorAddr, a4);
}

static constexpr uintptr_t SPAWNJDCSHL_OFFSET() { return 0x1DC320; }

using SpawnJDCShl = uintptr_t(__fastcall*)(uintptr_t shlAddr);

static uintptr_t SpawnJDCShl_sub_1401DC320(uintptr_t shlAddr) {
	SpawnJDCShl SpawnJDCShlFunc = reinterpret_cast<SpawnJDCShl>(appBaseAddr + SPAWNJDCSHL_OFFSET());
	if (!SpawnJDCShlFunc) {
		return NULL;
	}

	return SpawnJDCShlFunc(shlAddr);
}


void SpawnExtraJDCs(uintptr_t shlActorAddr) {
	auto& shlActorData = *reinterpret_cast<CPl021Shl02Actor*>(shlActorAddr);
	auto& actorData = *reinterpret_cast<PlayerActorData*>(shlActorData.playerActorAddr);
	JdcSourceKey sourceKey{ shlActorData.playerActorAddr, shlActorAddr };
	constexpr uint8 targetExtraJdcCount = 3; // Total JDCs = 1 base + 2 extras = 3

	if (!shlActorData.aliveStatus) {
		auto extraIt = s_extraAddrToSource.find(shlActorAddr);
		if (extraIt != s_extraAddrToSource.end()) {
         s_suppressJustFrameVFXForShl.erase(shlActorAddr);
			s_extraAddrToSource.erase(extraIt);
			return;
		}

		s_pendingSinceBySource.erase(sourceKey);
		s_spawnedExtraCountBySource.erase(sourceKey);
		s_primaryPosBySource.erase(sourceKey);
		s_fixedExtraPosBySource.erase(sourceKey);

		for (auto it = s_extraAddrToSource.begin(); it != s_extraAddrToSource.end(); ) {
			if (it->second == sourceKey) {
             s_suppressJustFrameVFXForShl.erase(it->first);
				it = s_extraAddrToSource.erase(it);
			}
			else {
				++it;
			}
		}
		return;
	}

	if (!CheckIfInJustFrameJDC(shlActorData.playerActorAddr)) {
		return;
	}

	if (s_extraAddrToSource.find(shlActorAddr) != s_extraAddrToSource.end()) {
		return;
	}

	auto spawnedCountIt = s_spawnedExtraCountBySource.find(sourceKey);
	const uint8 spawnedExtraCount = (spawnedCountIt != s_spawnedExtraCountBySource.end()) ? spawnedCountIt->second : 0;
	if (spawnedExtraCount >= targetExtraJdcCount) {
		return;
	}

	vec3 newPosition = { shlActorData.position.x, shlActorData.position.y, shlActorData.position.z };
	auto primaryPosIt = s_primaryPosBySource.find(sourceKey);
	if (primaryPosIt != s_primaryPosBySource.end()) {
		newPosition = primaryPosIt->second;
	}
	float newMatrix[16] = { 0 };
	std::memcpy(newMatrix, shlActorData.matrix, sizeof(float) * 16);
	newMatrix[12] = newPosition.x;
	newMatrix[13] = newPosition.y;
	newMatrix[14] = newPosition.z;

	const float speedScale = actorData.speed / (std::max)(0.001f, g_FrameRateTimeMultiplier);
	const int scaledDelayMs = (std::max)(1, static_cast<int>(100.0f * speedScale));
	auto extraJdcDelay = std::chrono::milliseconds(scaledDelayMs);

	auto now = std::chrono::steady_clock::now();
	auto pendingIt = s_pendingSinceBySource.find(sourceKey);
	if (pendingIt == s_pendingSinceBySource.end()) {
		s_pendingSinceBySource[sourceKey] = now + extraJdcDelay;
		return;
	}

	if (now < pendingIt->second) {
		return;
	}

	uintptr_t newJDC = SetJDCPosition_sub_1401DC1A0((uintptr_t)&newPosition, (uintptr_t)newMatrix, shlActorData.playerActorAddr, 10);
	if (newJDC) {
		s_spawnedExtraCountBySource[sourceKey] = spawnedExtraCount + 1;
		s_pendingSinceBySource[sourceKey] = now + extraJdcDelay;
		s_extraAddrToSource[newJDC] = sourceKey;
       s_suppressJustFrameVFXForShl.insert(newJDC);
		s_fixedExtraPosBySource[sourceKey] = newPosition;
		auto& newShlActorData = *reinterpret_cast<CPl021Shl02Actor*>(newJDC);
		SpawnJDCShl_sub_1401DC320(newJDC + 0x60);
	}
}

static constexpr uintptr_t SHOTGUN_FIRE_OFFSET() { return 0x217FF0; }

using ShotgunFire_t = void(__fastcall*)(PlayerActorData* actorData, uint8 mode, uint32 unk3);

static void CallShotgunFire(PlayerActorData& actorData, uint8 mode = 8, uint32 unk3 = 0) {
	auto shotgunFire = reinterpret_cast<ShotgunFire_t>(appBaseAddr + SHOTGUN_FIRE_OFFSET());
	if (!shotgunFire) {
		return;
	}

	shotgunFire(&actorData, mode, unk3);
}

void QueueDelayPointBlankShotgunFire(uintptr_t playerAddr, uint8 fireMode, uint8 unk3) {
	auto& actorData = *reinterpret_cast<PlayerActorData*>(playerAddr);
	auto& backslide = (actorData.newEntityIndex == ENTITY::MAIN) ? crimsonPlayer[actorData.newPlayerIndex].backslide 
		: crimsonPlayer[actorData.newPlayerIndex].backslideClone;
	auto& actionTimer = (actorData.newEntityIndex == ENTITY::MAIN) ? crimsonPlayer[actorData.newPlayerIndex].actionTimer 
		: crimsonPlayer[actorData.newPlayerIndex].actionTimerClone;
	actionTimer = 0.0f; // this is needed for the repeated shots' delay
	backslide.pendingFire = true;

	// Actual delay and subsequent call happens from CrimsonGameplay::DanteShotgunBackslide
}

namespace DriveCol {
	// Track effect handles per collision instance
	enum class DriveFxPhase : uint8_t { Part1, Part2, Part3 };
	struct DriveInstanceState {
		EffekseerHandle handle{};
		DriveFxPhase phase = DriveFxPhase::Part1;

		// Enables custom vertical pathing (Y) for this projectile instance.
		bool hasLockedTargetHeight = false;
		// Enables custom horizontal pathing (XZ) for this projectile instance.
		bool hasLockedDirection = false;
		// Tracks whether we have previous projectile position for step accumulation.
		bool hasLastProjectilePos = false;

		// Spawn/start values used to build deterministic pathing.
		float startY = 0.0f;
		float lockedTargetY = 0.0f;
		float startX = 0.0f;
		float startZ = 0.0f;

		// Last frame projectile world position (for measuring traveled distance this frame).
		float lastProjectileX = 0.0f;
		float lastProjectileZ = 0.0f;

		// Horizontal distance to target sampled at spawn (for normalized slope math).
		float targetDistXZ = 1.0f;
		// Locked horizontal direction sampled at spawn.
		float dirX = 0.0f;
		float dirZ = 0.0f;

		// Monotonic traveled distance along the path; never decreases.
		float furthestTraveledXZ = 0.0f;
		// Constant Y increase per 1 unit of XZ travel (fixed at spawn time).
		float verticalSlopePerXZ = 0.0f;
	};

	struct DriveMetadataState {
		std::unordered_map<uint32, DriveInstanceState> effectsByInstanceId;
	};

	static std::unordered_map<uintptr_t, DriveMetadataState> s_driveEffectsByMetadata;
	static std::unordered_map<uintptr_t, float> s_lastDriveVerticalSlopeByShooter;

	void HandleDriveCollisionLogic(CollisionDataMetadata* collisionMeta, uintptr_t metadataKey) {
		if (!activeCrimsonGameplay.Gameplay.Dante.driveRework) {
			return;
		}
       // Keep effect loading lazy (first use) so initialization order remains identical to pre-refactor behavior.
		static constexpr const wchar_t* driveParticlePath = L"Crimson\\vfx\\drive.efkefc";
		static EffekseerRefHandle driveParticleRef = CrimsonEfk::LoadEffect(driveParticlePath, 40.0f);

		static constexpr const wchar_t* drive2ParticlePath = L"Crimson\\vfx\\drive2.efkefc";
		static EffekseerRefHandle drive2ParticleRef = CrimsonEfk::LoadEffect(drive2ParticlePath, 40.0f);

		static constexpr const wchar_t* drive3ParticlePath = L"Crimson\\vfx\\drive3.efkefc";
		static EffekseerRefHandle drive3ParticleRef = CrimsonEfk::LoadEffect(drive3ParticlePath, 40.0f);

		const bool isDriveCollision =
			reinterpret_cast<uintptr_t>(collisionMeta->dmgDataAddr) == reinterpret_cast<uintptr_t>(appBaseAddr + 0x5CB1E0);

		if (isDriveCollision) {
			auto& collisionData = *reinterpret_cast<CollisionData*>(collisionMeta->collisionDataAddr);
			auto& actorData = *reinterpret_cast<PlayerActorData*>(collisionData.playerBaseAddr);
			auto& drive = (actorData.newEntityIndex == 0) ? crimsonPlayer[actorData.newPlayerIndex].drive : crimsonPlayer[actorData.newPlayerIndex].driveClone;

			auto& metadataState = s_driveEffectsByMetadata[metadataKey];

			// Latch phase at spawn per instanceId inside this metadata stream.
			if (metadataState.effectsByInstanceId.find(collisionMeta->instanceId) == metadataState.effectsByInstanceId.end()) {
				const auto desiredPhase = drive.inPart3
					? DriveFxPhase::Part3
					: (drive.inPart2 ? DriveFxPhase::Part2 : DriveFxPhase::Part1);

				EffekseerHandle handle{};
				if (desiredPhase == DriveFxPhase::Part3) {
                 handle = CrimsonEfk::PlayEffectAtMatrix(drive3ParticleRef, collisionMeta->matrix1, NULL);
				}
				else if (desiredPhase == DriveFxPhase::Part2) {
                 handle = CrimsonEfk::PlayEffectAtMatrix(drive2ParticleRef, collisionMeta->matrix1, NULL);
				}
				else {
                  handle = CrimsonEfk::PlayEffectAtMatrix(driveParticleRef, collisionMeta->matrix1, NULL);
				}

				DriveInstanceState instanceState{};
				instanceState.handle = handle;
				instanceState.phase = desiredPhase;
				vec4& matrixPos = *reinterpret_cast<vec4*>(&collisionMeta->matrix1[12]);
				vec4& hitboxPos = *reinterpret_cast<vec4*>(&collisionMeta->hitboxPos);

				auto& projectileData = *reinterpret_cast<ActorDataBase*>(collisionData.baseAddr);
				instanceState.startX = hitboxPos.x;
				instanceState.startZ = hitboxPos.z;
				instanceState.startY = hitboxPos.y + 50.0f;
				instanceState.lastProjectileX = projectileData.position.x;
				instanceState.lastProjectileZ = projectileData.position.z;
				instanceState.hasLastProjectilePos = true;

				// Lock-on logic is evaluated once at projectile spawn.
				// The result is latched into instanceState so trajectory remains stable afterward.
				if (actorData.lockOnData.targetBaseAddr60 != 0) {
					const uintptr_t shooterKey = reinterpret_cast<uintptr_t>(collisionData.playerBaseAddr);
					auto& enemyActorData = *reinterpret_cast<EnemyActorData*>(actorData.lockOnData.targetBaseAddr60 - 0x60);
					const float dx = enemyActorData.position.x - instanceState.startX;
					const float dz = enemyActorData.position.z - instanceState.startZ;
					const float targetDistXZRaw = std::sqrt(dx * dx + dz * dz);
					instanceState.targetDistXZ = (std::max)(0.001f, targetDistXZRaw);

					// Cap vertical targeting by angle so very close lock-on targets don't force extreme upward arcs.
					constexpr float maxVerticalAimAngleDeg = 38.0f;
					constexpr float degToRad = 0.01745329251994329577f;
					const float maxVerticalDelta = std::tan(maxVerticalAimAngleDeg * degToRad) * instanceState.targetDistXZ;
					const float desiredVerticalDelta = (std::max)(0.0f, enemyActorData.position.y - instanceState.startY);

					constexpr float minTrackingDistanceXZ = 120.0f;
					const bool targetTooClose = targetDistXZRaw < minTrackingDistanceXZ;
					const bool hitsAngleCap = desiredVerticalDelta > maxVerticalDelta;

					// Decision gate:
					// - Normal case: lock to enemy (custom XZ + custom Y).
					// - Close/capped case: reuse previous valid vertical slope for this shooter,
					//   but keep default forward XZ (no custom direction lock).
					if (!targetTooClose && !hitsAngleCap) {
						instanceState.lockedTargetY = instanceState.startY + desiredVerticalDelta;
						instanceState.verticalSlopePerXZ = (instanceState.lockedTargetY - instanceState.startY) / instanceState.targetDistXZ;
						instanceState.hasLockedTargetHeight = true;
						instanceState.dirX = dx / instanceState.targetDistXZ;
						instanceState.dirZ = dz / instanceState.targetDistXZ;
						instanceState.hasLockedDirection = true;
						s_lastDriveVerticalSlopeByShooter[shooterKey] = instanceState.verticalSlopePerXZ;
					}
					else {
						auto previousSlopeIt = s_lastDriveVerticalSlopeByShooter.find(shooterKey);
						if (previousSlopeIt != s_lastDriveVerticalSlopeByShooter.end()) {
							instanceState.verticalSlopePerXZ = previousSlopeIt->second;
							instanceState.hasLockedTargetHeight = true;
							instanceState.hasLockedDirection = false; // keep default forward XZ travel
						}
					}
				}

				metadataState.effectsByInstanceId[collisionMeta->instanceId] = instanceState;
			}

			auto instanceIt = metadataState.effectsByInstanceId.find(collisionMeta->instanceId);
			const DriveFxPhase latchedPhase = (instanceIt != metadataState.effectsByInstanceId.end())
				? instanceIt->second.phase
				: DriveFxPhase::Part1;

			vec4& matrixPos = *reinterpret_cast<vec4*>(&collisionMeta->matrix1[12]);
			vec4& hitboxPos = *reinterpret_cast<vec4*>(&collisionMeta->hitboxPos);
			// When locked on to an enemy, apply custom trajectory logic to steer towards the target.
			if (instanceIt != metadataState.effectsByInstanceId.end() && instanceIt->second.hasLockedTargetHeight) {
				auto& projectileData = *reinterpret_cast<ActorDataBase*>(collisionData.baseAddr);
				auto& state = instanceIt->second;

				// Accumulate horizontal travel by per-frame XZ step length.
				// This avoids jitter from recomputing from spawn every frame.
				float traveledXZ = state.furthestTraveledXZ;
				if (state.hasLastProjectilePos) {
					const float stepDx = projectileData.position.x - state.lastProjectileX;
					const float stepDz = projectileData.position.z - state.lastProjectileZ;
					traveledXZ += std::sqrt(stepDx * stepDx + stepDz * stepDz);
				}
				state.lastProjectileX = projectileData.position.x;
				state.lastProjectileZ = projectileData.position.z;
				state.hasLastProjectilePos = true;

				// Keep progression monotonic so path never changes mid-flight.
				traveledXZ = (std::max)(state.furthestTraveledXZ, traveledXZ);
				state.furthestTraveledXZ = traveledXZ;

				

				// Custom vertical steering along a fixed slope from spawn.
				const float desiredY = state.startY + state.verticalSlopePerXZ * traveledXZ;
				matrixPos.y = desiredY;
				hitboxPos.y = desiredY;
			}
			else { // When no enemy is locked-on to, just apply a simple vertical offset to keep particles above the ground.
				matrixPos.y += 120.0f;
				hitboxPos.y += 120.0f;
			}

			if (latchedPhase == DriveFxPhase::Part2) {
				vec3 right = { collisionMeta->matrix1[0], collisionMeta->matrix1[1], collisionMeta->matrix1[2] };
				right.y = 0.0f;
				float len = std::sqrt(right.x * right.x + right.z * right.z);
				if (len > 0.0001f) { right.x /= len; right.z /= len; }

				constexpr float leftOffset = -80.0f;
				const float ox = -right.x * leftOffset;
				const float oz = -right.z * leftOffset;

				auto& projectileData = *reinterpret_cast<ActorDataBase*>(collisionData.baseAddr);
				matrixPos.x += ox; matrixPos.z += oz;
				hitboxPos.x += ox; hitboxPos.z += oz;
				//projectileData.position.x += ox; projectileData.position.z += oz;
				//projectileData.position.x += 200.0f;
			}

// 			auto& collisionFile = *reinterpret_cast<uint32*>(collisionMeta->files[0]);
// 			collisionFile = 3; // Allows Drive collision go through walls
			//collisionMeta->mode = 6; // Allows Drive collision go through walls
		}
		else {
			// Only stop effect tied to THIS instance (not globally)
			auto metadataIt = s_driveEffectsByMetadata.find(metadataKey);
			if (metadataIt != s_driveEffectsByMetadata.end()) {
				for (auto& kvp : metadataIt->second.effectsByInstanceId) {
					CrimsonEfk::StopEffect(kvp.second.handle);
				}
				s_driveEffectsByMetadata.erase(metadataIt);
			}
		}
	}
}

float InterceptingCollisions(byte8* metadataAddr, float radius) {
	auto collisionMeta = reinterpret_cast<CollisionDataMetadata*>(metadataAddr);

	if (!collisionMeta) {
		return radius;
	}

	const uintptr_t metadataKey = reinterpret_cast<uintptr_t>(metadataAddr);

	// Context: Every move seems to have a specific offset from PlayerAddr for its collision data.
	// We can use this to both id which move the collision pertains to and which player it belongs to / spawned it.
	// You can identify this offset by putting a breakpoint at dmc3.exe + 2CCC98 and looking at RBX, 
	// which points to the collisionMetadata struct. For attack hitboxes, it will usually end at ...0x420 (for Vergil) and 0x760 (for Dante).
	// so just look at the relation between sourceMatrixAddr (+0x20) and the playerAddr to get the hitbox offset for each move.
	// This detour call is placed right before the game writes the radius value for the hitbox, 
	// so we can check for specific moves and modify their hitbox size if we want to.
	// Conversely, we can also use the dmgDatastruct and playerAddr present in the Collision Structs to id them (more reliable). 
	// - Mia

	DriveCol::HandleDriveCollisionLogic(collisionMeta, metadataKey);

	// Yamato High Time hitbox increase
	uintptr_t yamatoHighTimeOffset = (uintptr_t)collisionMeta->sourceMatrixAddr - 0x66640;
	uintptr_t yamatoHighTimeOffsetDT = (uintptr_t)collisionMeta->sourceMatrixAddr - 0x7CA40; // +0x16400 from yamatoHighTimeOffset, for DT version of the move, DT has slightly larger radius?
	uintptr_t yamatoHighTimeOffsetClone = (uintptr_t)collisionMeta->sourceMatrixAddr - 0x17A640; // +0x114000 from yamatoHighTimeOffset

	// Checking for all Players and Clones
	for (uint8 playerIndex = 0; playerIndex < activeConfig.Actor.playerCount; playerIndex++) {
		for (uint8 entityIndex = 0; entityIndex < 2; entityIndex++) {
			auto& playerData = GetPlayerData(playerIndex);
			auto& characterData = GetCharacterData(playerIndex, playerData.activeCharacterIndex, entityIndex);
			auto& newActorData = GetNewActorData(playerIndex, playerData.activeCharacterIndex, entityIndex);

			if (!newActorData.baseAddr) {
				continue;
			}
			auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);
			auto& inYamatoHighTime = (entityIndex == ENTITY::MAIN) ? crimsonPlayer[playerIndex].inYamatoHighTime :
				crimsonPlayer[playerIndex].inYamatoHighTimeClone;

			if ((((yamatoHighTimeOffset == (uintptr_t)newActorData.baseAddr) && crimsonPlayer[playerIndex].inYamatoHighTime) ||
				(yamatoHighTimeOffsetClone == (uintptr_t)newActorData.baseAddr) && crimsonPlayer[playerIndex].inYamatoHighTimeClone) ||
				(yamatoHighTimeOffsetDT == (uintptr_t)newActorData.baseAddr) && crimsonPlayer[playerIndex].inYamatoHighTime) {

				return radius * 3.5f;
			}

		}
	}


	return radius;
}

void DebugDrawCollisions(byte8* metadataAddr) {
	if (!activeCrimsonGameplay.Debug.showHitboxes || !activeCrimsonGameplay.Debug.debugTools) {
		return;
	}

	auto& collisionMeta = *reinterpret_cast<CollisionDataMetadata*>(metadataAddr);

	vec3 right = { collisionMeta.matrix1[0], collisionMeta.matrix1[1], collisionMeta.matrix1[2] };
	vec3 up = { collisionMeta.matrix1[4], collisionMeta.matrix1[5], collisionMeta.matrix1[6] };
	vec3 forward = { collisionMeta.matrix1[8], collisionMeta.matrix1[9], collisionMeta.matrix1[10] };

	vec4 position = { collisionMeta.matrix1[12], collisionMeta.matrix1[13], collisionMeta.matrix1[14],  collisionMeta.matrix1[15] };

	dd::circle(dd_ctx(), *(ddVec3*)&collisionMeta.hitboxPos, *(ddVec3*)&up, dd::colors::Coral, collisionMeta.hitboxRadius, 8, 32);
	dd::circle(dd_ctx(), *(ddVec3*)&collisionMeta.hitboxPos, *(ddVec3*)&right, dd::colors::Chartreuse, collisionMeta.hitboxRadius, 8, 32);
	dd::circle(dd_ctx(), *(ddVec3*)&collisionMeta.hitboxPos, *(ddVec3*)&forward, dd::colors::Crimson, collisionMeta.hitboxRadius, 8, 32);
}

bool CheckIfCanExecuteAction(uintptr_t playerAddr, uint32 event) {
	auto& actorData = *reinterpret_cast<PlayerActorData*>(playerAddr);
	uint8 playerIndex = actorData.newPlayerIndex;
	uint8 entityIndex = actorData.newEntityIndex;
	auto& jCut = (entityIndex == ENTITY::MAIN) ? crimsonPlayer[playerIndex].jCut : crimsonPlayer[playerIndex].jCutClone;
	auto lockOn = (actorData.buttons[0] & GetBinding(BINDING::LOCK_ON));
	auto& gamepad = GetGamepad(playerIndex);
	auto tiltDirection = GetRelativeTiltDirection(actorData);

	// Here we can block certain actions from being performed.
	
// 	if (jCut.isJustFrameCharged || jCut.isAfterJustFrameCharged || actorData.action == ACTION_VERGIL::YAMATO_JUDGEMENT_CUT_LEVEL_2 || actorData.eventData[0].event == 33) {
// 		return false;
// 	}

// 	if (jCut.performing) {
// 		return false;
// 	}

	return true;
}

void InitDetours() {
    using namespace Utility;
    g_appBaseAddr = (uintptr_t)appBaseAddr;

	// cEnemySetCtrl__spawnGuy_sub_1401A4680
	//dmc3.exe+1A4680 - 40 57 - push rdi
	CameraCtrlInitDetour();
	CameraSwitchInitDetour();
	CameraWallCheckDetour();
	LdkInitDetour();
	CrimsonEfk::EffekInitRenderHook();
	CrimsonOnTick::ToggleOnTickFuncs(true);

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

	// InterceptCollisions
	// dmc3.exe + 2CCC98 - F3 0F 11 8B 40 01 00 00 - movss[rbx + 00000140], xmm1 { Assigning Hitbox Radius }
	static std::unique_ptr<Utility::Detour_t> InterceptCollisionsHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2CCC98, &InterceptCollisionsDetour, 8);
	g_InterceptCollisions_ReturnAddr = InterceptCollisionsHook->GetReturnAddress();
	g_DrawCollisionsCall = &DebugDrawCollisions;
	g_InterceptCollisionsCall = &InterceptingCollisions;
	InterceptCollisionsHook->Toggle(true);

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

void ToggleTurnRateFix(bool enable) {
	// Here we make Turn Rates behave consistently across all FramesPerSecond targets.
	// This also controls the Faster Turn Rate setting. - Berthrage
	using namespace Utility;
	static bool run = false;
	CrimsonPatches::KillTurnRateTruncation(enable);
	if (run == enable) {
		return;
	}

	// Alter Turn Rate, from ControlMovementRotation_sub_1401FC5B0:
	// dmc3.exe+1FC5C7 - 41 0F BF D9 - movsx ebx,r9w { Turn Rate }
	// dmc3.exe+1FC5CB - 41 0F BF 08  - movsx ecx,word ptr [r8]

	static std::unique_ptr<Utility::Detour_t> TurnRateFixHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1FC5C7, &TurnRateFixDetour, 8);
	g_TurnRateFix_ReturnAddr = TurnRateFixHook->GetReturnAddress();
	g_FasterTurnRateSettingAddr = &activeCrimsonGameplay.Gameplay.General.fasterTurnRate;
	TurnRateFixHook->Toggle(enable);

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
	g_SetAirTauntSkyLaunchCheckCall = &CheckSkyLaunchEnabled;
	g_SetAirTauntATRisingSunCheckCall = &CheckAirTauntRisingSunEnabled;

	SkyLaunchDetours(enable);

    run = enable;
}

void ToggleEnsureAirRisingDragonLaunch(bool enable) {
	using namespace Utility;
	static bool run = false;

	if (run == enable) {
		return;
	}

	// dmc3.exe+202588 - 80 BB A7 3F 00 00 00     - cmp byte ptr [rbx+00003FA7],00 { Checking TransitionMove? }
	// dmc3.exe + 20258F - 75 16 - jne dmc3.exe + 2025A7 { Transition from Rising Dragon Launch to Whirlwind }
	// PlayerPtr in RBX
	static std::unique_ptr<Detour_t> EnsureAirRisingDragonLaunchHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x202588, &EnsureAirRisingDragonLaunchDetour, 7);
	g_EnsureAirRisingDragonLaunch_ReturnAddr = EnsureAirRisingDragonLaunchHook->GetReturnAddress();
	g_EnsureAirRisingDragonLaunch_JmpAddr = (uintptr_t)appBaseAddr + 0x202591;
	EnsureAirRisingDragonLaunchHook->Toggle(enable);

	run = enable;
}

void ToggleConfirmSetAction(bool enable) {
	using namespace Utility;
	static bool run = false;

	if (run == enable) {
		return;
	}

	// dmc3.exe+1E6DAF - E8 4C9AFFFF           - call dmc3.exe+1E0800 -- TriggerEvent call
	// RCX is playerPtr, RDX is event ID (0x11)

	g_ConfirmSetAction_FuncCall = (uintptr_t)appBaseAddr + 0x1E0800;

	static std::unique_ptr<Utility::Detour_t> ConfirmSetActionHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1E6DAF, &ConfirmSetActionDetour, 5);
	g_ConfirmSetAction_ReturnAddr = ConfirmSetActionHook->GetReturnAddress();
	ConfirmSetActionHook->Toggle(enable);
	g_ConfirmSetActionCheckCall = &CheckIfCanExecuteAction;
	
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

void ToggleHideAndMutePortals(bool enable) {
	using namespace Utility;
	static bool run = false;
	if (run == enable) {
		return;
	}

	// HidePortals
	// dmc3.exe + 270B85 - 66 89 44 24 28        - mov [rsp+28],ax
	static std::unique_ptr<Utility::Detour_t> HidePortalsHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x270B85, &PortalsHideDetour, 5);
	g_PortalsHide_ReturnAddr = HidePortalsHook->GetReturnAddress();
	HidePortalsHook->Toggle(enable);

	// MutePortals
	// dmc3.exe + 26D07E - 89 03                 - mov [rbx],eax
	// dmc3.exe + 26D080 - 48 8D 5B 04           - lea rbx,[rbx+04]
	static std::unique_ptr<Utility::Detour_t> MutePortalsHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x26D07E, &PortalsMuteDetour, 6);
	g_PortalsMute_ReturnAddr = MutePortalsHook->GetReturnAddress();
	MutePortalsHook->Toggle(enable);

	run = enable;
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

void ToggleDanteTrickAlterations(bool enable) {
	// Used for Dante's Ground Trick (event 48)
	using namespace Utility;
	static bool run = false;
	if (run == enable) {
		return;
	}

	// DanteTrickAlterationsDetour1
	// AirTrickYInertia
	// dmc3.exe+1F218B - F3 0F 5E B7 34 3E 00 00   - divss xmm6,[rdi+00003E34]
	static std::unique_ptr<Utility::Detour_t> DanteTrickAlterationsHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1F218B, &DanteTrickAlterationsDetour1, 8);
	g_DanteTrickAlter_ReturnAddr1 = DanteTrickAlterationsHook->GetReturnAddress();
	DanteTrickAlterationsHook->Toggle(enable);

	// DanteTrickAlterationsDetour2
	// End of Air Trick Y inertia
	// dmc3.exe+1DFF07 - F3 0F 11 9B 94 00 00 00   - movss [rbx+00000094],xmm3
	static std::unique_ptr<Utility::Detour_t> DanteTrickAlterationsHook2 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1DFF07, &DanteTrickAlterationsDetour2, 8);
	g_DanteTrickAlter_ReturnAddr2 = DanteTrickAlterationsHook2->GetReturnAddress();
	DanteTrickAlterationsHook2->Toggle(enable);

	// DanteTrickAlterationsDetour3
	// Trick landing anim so you input grounded moves just before you land
	// dmc3.exe+1DFFF9 - BA 09 00 00 00           - mov edx, 9 { 00000009 }
	static std::unique_ptr<Utility::Detour_t> DanteTrickAlterationsHook3 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1DFFF9, &DanteTrickAlterationsDetour3, 5);
	g_DanteTrickAlter_ReturnAddr3 = DanteTrickAlterationsHook3->GetReturnAddress();
	DanteTrickAlterationsHook3->Toggle(enable);

	// DanteTrickAlterationsDetour4
	// Increase Air Trick Horizontal Inertia (xmm0) & MaxAirTrickDistance (xmm8)
	// dmc3.exe + 1F2128 - F3 44 0F 10 83 94 02 00 00 - movss xmm8, [rbx + 00000294]
	static std::unique_ptr<Utility::Detour_t> DanteTrickAlterationsHook4 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1F2128, &DanteTrickAlterationsDetour4, 10);
	g_DanteTrickAlter_ReturnAddr4 = DanteTrickAlterationsHook4->GetReturnAddress();
	DanteTrickAlterationsHook4->Toggle(enable);

	run = enable;
}

void ToggleShotgunShlSpawnAnglePointBlank(bool enable) {
	// Used for changing the Spawn Angle for Backslide.
	using namespace Utility;
	static bool run = false;
	if (run == enable) {
		return;
	}
	
	// dmc3.exe + 2180C6 - 75 53 - jne dmc3.exe+21811B { Check if locked on enemy is near} -- needs killing
	// dmc3.exe + 2180C8 - 0F B7 8B C0 00 00 00 - movzx ecx, word ptr[rbx + 000000C0]{ Spawn Angle taking Player Rotation for Normal Shots}
	// dmc3.exe+2180DC - 0F B7 8B C0 00 00 00 - movzx ecx,word ptr [rbx+000000C0] { Spawn Angle taking Player Rotation for Normal Shots 2 }
	static std::unique_ptr<Utility::Detour_t> ShotgunShlSpawnAngleHook1 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2180C6, &ShotgunShlSpawnAnglePointBlankDetour, 9);
	g_ShotgunShlSpawnAnglePointBlank_ReturnAddr1 = ShotgunShlSpawnAngleHook1->GetReturnAddress();
	g_ShotgunShlSpawnAnglePointBlankLockedOnEnemyJmp = (uintptr_t)appBaseAddr + 0x21811B;
	ShotgunShlSpawnAngleHook1->Toggle(enable);

	static std::unique_ptr<Utility::Detour_t> ShotgunShlSpawnAngleHook2 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2180DC, &ShotgunShlSpawnAnglePointBlankDetour2, 7);
	g_ShotgunShlSpawnAnglePointBlank_ReturnAddr2 = ShotgunShlSpawnAngleHook2->GetReturnAddress();
	ShotgunShlSpawnAngleHook2->Toggle(enable);

	g_ShotgunShlSpawnAnglePointBlankCheckCall = &CheckIfInBackslide;
	

	// dmc3.exe+20EDEB - 75 1C                 - jne dmc3.exe+20EE09
	// dmc3.exe+20EDED - 48 8D 8B 10 65 00 00  - lea rcx,[rbx+00006510]
	// dmc3.exe + 20EE11 - E8 DA 91 00 00 - call dmc3.exe + 217FF0{ Point Blank's ShotgunFire }
	// player in RCX, fireMode in RDX, R8 is 0
	static std::unique_ptr<Utility::Detour_t> PointBlankShotgunFireHook =
	std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x20EDEB, &PointBlankShotgunFireDetour, 9);
	g_PointBlankShotgunFire_ReturnAddr = PointBlankShotgunFireHook->GetReturnAddress();
	PointBlankShotgunFireHook->Toggle(enable);

	g_PointBlankShotgunFireDelayCall = &QueueDelayPointBlankShotgunFire;
	g_PointBlankShotgunFireOgCall = (uintptr_t)appBaseAddr + 0x217FF0;

	// dmc3.exe+20ED1A - E9 D1 92 00 00 - jmp dmc3.exe+217FF0 { Tail Func Call for PB's ShotgunFire(repeated shot) }
	static std::unique_ptr<Utility::Detour_t> PointBlankShotgunFireTailHook =
	std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x20ED1A, &PointBlankShotgunFireTailCallDetour, 5);
	g_PointBlankShotgunFireTailCall_ReturnAddr = PointBlankShotgunFireTailHook->GetReturnAddress();
	PointBlankShotgunFireTailHook->Toggle(enable);

	// In case we want to do Charged shots instead
	// dmc3.exe+21828B - 66 03 8B C0 00 00 00 - add cx,[rbx+000000C0] { Spawn Angle taking Player Rotation for Charged Shot } 
	// dmc3.exe+2182BD - 66 44 03 B3 C0 00 00 00 - add r14w,[rbx+000000C0] { Spawn Angle taking Player Rotation for Charged Shot 2 } 

	g_PointBlankShotgunCancelAnimTailCall = (uintptr_t)appBaseAddr + 0x1FC4F0;

	run = enable;
}


void ToggleJudgementCutDetours(bool enable) {
	using namespace Utility;
	static bool run = false;
	if (run == enable) {
		return;
	}
	// JudgementCutDetours
	// dmc3.exe+1DC678 - E9 B3 FA FF FF - jmp dmc3.StartupDelayJDC_sub_1401DC130
	// dmc3.exe+1DC66B - E9 70FEFFFF           - jmp dmc3.SpawnJDCCollision_sub_1401DC4E0

	static std::unique_ptr<Utility::Detour_t> JudgementCutSpeedHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1DC678, &JudgementCutSpeedDetour, 5);
	g_JudgementCutSpeed_ReturnAddr = JudgementCutSpeedHook->GetReturnAddress();
	g_JudgementCutVFX_ReturnAddr2 = (uintptr_t)appBaseAddr + 0x1DC3D7;
	g_JudgementCutStartDelayCall = (uintptr_t)appBaseAddr + 0x1DC130;
	g_JudgementCutSpawnCollisionCall = (uintptr_t)appBaseAddr + 0x1DC4E0;
	g_JudgementCutCheckJustFrameCall = &CheckIfInJustFrameJDC;
	JudgementCutSpeedHook->Toggle(false);


	// dmc3.exe+1DC3B5 - E8 E6 B8 10 00 - call dmc3.PlayVFX_sub_1402E7CA0 { JudgementCut PlayVFX Call }
	static std::unique_ptr<Utility::Detour_t> JudgementCutVFXHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1DC3B5, &JudgementCutVFXDetour, 5);
	g_JudgementCutVFX_ReturnAddr = JudgementCutVFXHook->GetReturnAddress();
	g_JudgementCutRegularVFXCall = (uintptr_t)appBaseAddr + 0x2E7CA0;
	g_JudgementCutJustFrameVFXCall = &PlayJustFrameJDCVFX;
	JudgementCutVFXHook->Toggle(enable);

	// dmc3.exe+1DC3AB - C7 83 7C 01 00 00 00 00 80 3F - mov [rbx+0000017C],3F800000 { Setting JDC W position at matrix }
	static std::unique_ptr<Utility::Detour_t> JudgementCutPositionHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1DC3AB, &JudgementCutPositionDetour, 10);
	g_JudgementCutPosition_ReturnAddr = JudgementCutPositionHook->GetReturnAddress();
	g_JudgementCutSetPositionCall = &SetJDCPositionAtMatrix;
	JudgementCutPositionHook->Toggle(enable);

	// dmc3.exe+1DC62E - 0F B6 53 08 - movzx edx,byte ptr [rbx+08] { Checking aliveStatus for comparisons }
	// dmc3.exe+1DC632 - 85 D2 - test edx,edx
	static std::unique_ptr<Utility::Detour_t> JudgementCutExtraShlHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1DC62E, &JudgementCutExtraShlDetour, 6);
	g_JudgementCutExtraShl_ReturnAddr = JudgementCutExtraShlHook->GetReturnAddress();
	g_JudgementCutExtraShlCall = &SpawnExtraJDCs;
	JudgementCutExtraShlHook->Toggle(enable);

	run = enable;
}


void ToggleFixSecretMissionTimerFPS(bool enable) {
	// This will untie the Secret Mission timer from FPS settings (ie no longer spawning with half the time when playing at 120 fps).
	using namespace Utility;
	static bool run = false;
	if (run == enable) {
		return;
	}
	
	// FixSecretMissionTimerFPS
	// dmc3.exe+27C0DD - F3 0F 11 81 48 69 00 00   - movss [rcx+00006948],xmm0 { Setting secret mission timer }
	// RCX is HUDPtr

	// dmc3.exe+27EF74 - F3 0F 5C C8            - subss xmm1,xmm0
	// dmc3.exe+27EF78 - 0F 57 C0               - xorps xmm0,xmm0
	// dmc3.exe + 27EF7E - F3 0F 11 8F 48 69 00 00 - movss[rdi + 00006948], xmm1{ Decrement Secret Mission Timer }
	// RDI is HUDPtr

	static std::unique_ptr<Utility::Detour_t> FixSecretMissionTimerFPSHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x27C0DD, &FixSecretMissionTimerFPSDetour, 8);
	g_FixSecretMissionTimerFPS_ReturnAddr = FixSecretMissionTimerFPSHook->GetReturnAddress();
	FixSecretMissionTimerFPSHook->Toggle(enable);

	static std::unique_ptr<Utility::Detour_t> FixSecretMissionTimerFPSHook2 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x27EF74, &FixSecretMissionTimerFPSDetour2, 7);
	g_FixSecretMissionTimerFPS_ReturnAddr2 = FixSecretMissionTimerFPSHook2->GetReturnAddress();
	FixSecretMissionTimerFPSHook2->Toggle(enable);

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
	
	// FixCrashM5Detour2
	// dmc3.exe+5A42B - 8B 04 82              - mov eax,[rdx+rax*4]
	// dmc3.exe+5A42E - 85 C0                 - test eax,eax
	static std::unique_ptr<Utility::Detour_t> FixMission5CrashHook2 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x5A42B, &FixCrashM5Detour2, 5);
	g_FixCrashM5_ReturnAddr2 = FixMission5CrashHook2->GetReturnAddress();
	g_FixCrashM5_JmpAddr2 = (uintptr_t)appBaseAddr + 0x5A436; 
	FixMission5CrashHook2->Toggle(enable);

	run = enable;
}

void ToggleCerbDamageFix(bool enable) {
	using namespace Utility;
	static bool run = false;
	//g_cerbDamageValue = 9999.0f;
	// If the function has already run in the current state, return early
	if (run == enable) {
		return;
	}

	//Cerb damage fix detour
	//dmc3.exe + 10B7DA: 0F 2F F0 - comiss xmm6, xmm0
	//dmc3.exe + 10B7DD : 76 08 - jna dmc3.exe + 10B7E7
	static std::unique_ptr<Utility::Detour_t> CerbDamageFixHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x10B7DA, &CerbDamageFixDetour, 5);
	g_CerbDamageFix_ReturnAddr = CerbDamageFixHook->GetReturnAddress();
	g_CerbDamageFix_JmpAddr = (uintptr_t)appBaseAddr + 0x10B7E7; // Jump to the next instruction after the detour
	CerbDamageFixHook->Toggle(enable);
	run = enable;
}

void ToggleArkhamPt2GrabCrashFix(bool enable) {
	using namespace Utility;
	static bool run = false;
	// If the function has already run in the current state, return early
	if (run == enable) {
		return;
	}

	// Detour1
	// dmc3.exe + 32E5F4 - 0F 10 01 - movups xmm0, [rcx] // actorPos in rcx
	// dmc3.exe + 32E5F7 - 0F 28 D0 - movaps xmm2,xmm0
	static std::unique_ptr<Utility::Detour_t> FixCrashArkhamPt2GrabHook1 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x32E5F4, &FixCrashArkhamPt2GrabDetour1, 6);
	g_FixCrashArkhamPt2Grab_ReturnAddr1 = FixCrashArkhamPt2GrabHook1->GetReturnAddress();
	FixCrashArkhamPt2GrabHook1->Toggle(enable);

	// Detour2
	// dmc3.exe + 2C6813 - 0F 10 01 - movups xmm0,[rcx] { Enigma Rotate to Target }
	// dmc3.exe + 2C6816 - 0F 5C C1               - subps xmm0,xmm1
	static std::unique_ptr<Utility::Detour_t> FixCrashArkhamPt2GrabHook2 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2C6813, &FixCrashArkhamPt2GrabDetour2, 6);
	g_FixCrashArkhamPt2Grab_ReturnAddr2 = FixCrashArkhamPt2GrabHook2->GetReturnAddress();
	FixCrashArkhamPt2GrabHook2->Toggle(enable);

	// Detour3
	// dmc3.exe + 16725C - F3 0F 58 83 80 00 00 00   - addss xmm0,[rbx+00000080] { Arkham Leeches Jump Attack 2 Target Position }
	static std::unique_ptr<Utility::Detour_t> FixCrashArkhamPt2GrabHook3 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x16725C, &FixCrashArkhamPt2GrabDetour3, 8);
	g_FixCrashArkhamPt2Grab_ReturnAddr3 = FixCrashArkhamPt2GrabHook3->GetReturnAddress();
	FixCrashArkhamPt2GrabHook3->Toggle(enable);

	// Detour4
	// dmc3.exe + 1672B9 - F3 0F 58 83 88 00 00 00   - addss xmm0,[rbx+00000088]
	static std::unique_ptr<Utility::Detour_t> FixCrashArkhamPt2GrabHook4 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1672B9, &FixCrashArkhamPt2GrabDetour4, 8);
	g_FixCrashArkhamPt2Grab_ReturnAddr4 = FixCrashArkhamPt2GrabHook4->GetReturnAddress();
	FixCrashArkhamPt2GrabHook4->Toggle(enable);

	// Detour5
	// dmc3.exe+1675B3 - 0F 28 82 80 00 00 00      - movaps xmm0,[rdx+00000080] { Arkham Leeches Jump Attack Target Position }
	static std::unique_ptr<Utility::Detour_t> FixCrashArkhamPt2GrabHook5 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1675B3, &FixCrashArkhamPt2GrabDetour5, 7);
	g_FixCrashArkhamPt2Grab_ReturnAddr5 = FixCrashArkhamPt2GrabHook5->GetReturnAddress();
	FixCrashArkhamPt2GrabHook5->Toggle(enable);

	run = enable;
}

void ToggleArkhamPt2DoppelCrashFix(bool enable) {
	using namespace Utility;
	static bool run = false;
	// If the function has already run in the current state, return early
	if (run == enable) {
		return;
	}

	// Detour1
	// dmc3.exe + 337744 - 48 03 08              - add rcx,[rax]
	// dmc3.exe + 337747 - E8 9E F4 00 00           - call dmc3.exe+346BEA { ->->VCRUNTIME140.memset }
	static std::unique_ptr<Utility::Detour_t> FixCrashArkhamPt2DoppelHook1 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x337744, &FixCrashArkhamPt2DoppelDetour1, 8);
	g_FixCrashArkhamPt2Doppel_ReturnAddr1 = FixCrashArkhamPt2DoppelHook1->GetReturnAddress();
	g_FixCrashArkhamPt2Doppel_CallAddr1 = (uintptr_t)appBaseAddr + 0x346BEA; // VCRUNTIME140.memset
	FixCrashArkhamPt2DoppelHook1->Toggle(enable);

	// Detour2
	// dmc3.exe + 337753 - FF 48 20 - dec[rax + 20]
	// dmc3.exe + 337756 - E8 45 1B FF FF           - call dmc3.exe+3292A0
	static std::unique_ptr<Utility::Detour_t> FixCrashArkhamPt2DoppelHook2 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x337753, &FixCrashArkhamPt2DoppelDetour2, 8);
	g_FixCrashArkhamPt2Doppel_ReturnAddr2 = FixCrashArkhamPt2DoppelHook2->GetReturnAddress();
	g_FixCrashArkhamPt2Doppel_CallAddr2 = (uintptr_t)appBaseAddr + 0x3292A0; 
	FixCrashArkhamPt2DoppelHook2->Toggle(true);

	run = enable;
}

void ToggleStyleLevellingCCSFix(bool enable) {
	using namespace Utility;
	static bool run = false;
	// If the function has already run in the current state, return early
	if (run == enable) {
		return;
	}

	// StyleLevellingCCSFixDetour1
	// dmc3.exe+1FA3D9 - C7 83 58630000 01000000 - mov [rbx+00006358],00000001 { Level up StyleLevel to lvl 2 }
	static std::unique_ptr<Utility::Detour_t> StyleLevellingCCSFixHook1 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1FA3D9, &StyleLevellingCCSFixDetour1, 10);
	g_StyleLevellingCCSFix_ReturnAddr1 = StyleLevellingCCSFixHook1->GetReturnAddress();
	g_StyleLevellingCCSFix_CheckCall1 = &StyleLevel1Fix; 
	StyleLevellingCCSFixHook1->Toggle(enable);

	// StyleLevellingCCSFixDetour2
	// dmc3.exe+1FA343 - C7 83 58630000 02000000 - mov [rbx+00006358],00000002 { Level up StyleLevel to lvl 3 }
	static std::unique_ptr<Utility::Detour_t> StyleLevellingCCSFixHook2 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1FA343, &StyleLevellingCCSFixDetour2, 10);
	g_StyleLevellingCCSFix_ReturnAddr2 = StyleLevellingCCSFixHook2->GetReturnAddress();
	g_StyleLevellingCCSFix_CheckCall2 = &StyleLevel2Fix; 
	StyleLevellingCCSFixHook2->Toggle(enable);

	run = enable;
}

void ToggleDTMustStyleArmor(bool enable) {
	using namespace Utility;
	static bool run = false;
	// If the function has already run in the current state, return early
	if (run == enable) {
		return;
	}
	// DTMustStyleArmorDetour
	// dmc3.exe+27A18C - 41 8B 94 80 D4 A1 27 00  - mov edx,[r8+rax*4+0027A1D4]
	static std::unique_ptr<Utility::Detour_t> DTMustStyleArmorHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x27A18C, &DTMustStyleArmorDetour, 8);
	g_DTMustStyleArmor_ReturnAddr = DTMustStyleArmorHook->GetReturnAddress();
	g_DTMustStyleArmor_CheckCall1 = &CheckIfInMustStyle;
	g_DTMustStyleArmor_CheckCall2 = &SetAnnouncerWasHit;
	DTMustStyleArmorHook->Toggle(enable);
	run = enable;
}

}
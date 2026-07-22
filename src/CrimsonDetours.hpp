#pragma once
#include <cstdint>
#include "Vars.hpp"

namespace CrimsonDetours {

extern "C" {
// SampleMod
void SampleModDetour1();

// GuardGravity
void GuardGravityDetour();

// EnableAirTaunt
void EnableAirTauntDetour();

// SetAirTaunt
void SetAirTauntDetour();

// Sky Launch Detours: (Dante Air Taunt)
// SkyLaunchForceRelease 
void SkyLaunchForceReleaseDetour();

// SkyLaunchKillRGConsumption1
void SkyLaunchKillRGConsumptionDetour();

// SkyLaunchKillReleaseLevelDetour
void SkyLaunchKillReleaseLevel1Detour();
void SkyLaunchKillReleaseLevel2Detour();
void SkyLaunchKillReleaseLevel3Detour();

// SkyLaunchKillDamage
void SkyLaunchKillDamageDetour();

// SkyLaunchKillDamageToCerberus
void SkyLaunchKillDamageCerberusDetour();

// SkyLaunchKillDamageToShieldNevan
void SkyLaunchKillDamageShieldNevanDetour();

// CreateEffect
//extern int createEffectBank;
//extern int createEffectID;
//extern int createEffectBone;
//extern std::uint64_t createEffectPlayerAddr;
void _fastcall CreateEffectDetour(void* pPlayer, int bank, int id, int boneIndex, bool enableCustomColor, uint32_t color, float speed);

// HoldToCrazyCombo
void HoldToCrazyComboDetour();

// DisableStaggerRoyalguard: for DT-Infused Royalguard
void DisableStaggerRoyalguardDetour();

// ToggleTakeDamage: for DT-Infused Royalguard
extern std::uint64_t toggleTakeDamageActorBaseAddr;
extern bool toggleTakeDamage;
void ToggleTakeDamageDetour();

// DisableDriveHold
void DisableDriveHoldDetour();

// HideStyleRankHUD
void HideStyleRankHUDDetour();

// HudHPSeparation
void HudHPSeparationDetour();

// HudStyleBarPos
void HudStyleBarPosDetour();

// MultiplayerCameraPositioning
void CustomCameraPositioningDetour();

// RerouteRedOrbsCounterAlpha
void RerouteRedOrbsCounterAlphaDetour1();
void RerouteRedOrbsCounterAlphaDetour2();
void RerouteRedOrbsCounterAlphaDetour3();

// FreeformSoftLockHelper
void FreeformSoftLockHelperDetour();

// DMC4/5LockOnDirection
void DMC4LockOnDirectionDetour();

// StyleRankHudNoFadeout
void StyleRankHudNoFadeoutDetour();

// ShootRemapDown
void ShootRemapDownDetour();
void ShootRemapHoldDetour();

// VergilNeutralTrick
void VergilNeutralTrickDetour();

// ArtemisInstantFullCharge
void ArtemisInstantFullChargeDetour1();
void ArtemisInstantFullChargeDetour2();

// CameraSensitivity
void CameraSensitivityDetour1();
void CameraSensitivityDetour2();

// CheckScreenBreak
void CheckScreenBreakDetour();
// CheckMissionResultScreen
void CheckMissionResultScreenDetour1();
// CheckMissionResultBPScreen
void CheckMissionResultBPScreenDetour1();
void CheckMissionResultBPScreenDetour2();
void CheckMissionResultBPScreenDetour3();
void CheckMissionResultBPScreenDetour4();
// NoAirLunarPhaseLift
void NoAirLunarPhaseLiftDetour();
// FixEnemyAttackCooldowns
void FixEnemyAttackCooldownsDetour1();
void FixEnemyAttackCooldownsDetour2();
// CheckTotalResultsScreen
void CheckTotalResultsScreenDetour();
// ChargeMechanicsCPlayer
void ChargeMechanicsCPlayerDetour();
// CItemOrbPickupAllPlayers
void CItemOrbPickupAllPlayersDetour();
// FasterSummonedSwords
void FasterSummonedSwordsDetour();
// SummonedSwordsFormationShortcuts
void SummonedSwordsFormationShortcutsDetour();
// StormSwordsDownedEnemyFix
void StormSwordsDownedEnemyFixDetour();
// FixMPXinputVibration
void FixMPXinputVibrationDetour();
// AdjustAirWhirlwindLaunchHeight
void AdjustAirWhirlwindLaunchHeightDetour();
// KillWeaponMotionState
void KillWeaponMotionStateDetour();
// SwingRoseAirTauntInertiaAndSpawnShl
void SwingRoseAirTauntInertiaAndSpawnShlDetour();
// FixNevanShlPlayerSpawn
void FixNevanShlPlayerSpawnDetour1();
void FixNevanShlPlayerSpawnDetour2();
// NevanShlMarkRoseMode
void NevanShlMarkRoseModeDetour();
// NevanShlSetToTravel
void NevanShlSetToTravelDetour();
// ScreenShakeDetours
void ScreenShakeDetoursDetour();}

bool g_HoldToCrazyComboFuncA(PlayerActorData& actorData);
bool DisableStaggerCheck(PlayerActorData& actorData);
void InitDetours();
void ToggleArtemisInstantFullCharge(bool enable);
void ToggleGuardGravityAlteration(bool enable);
void ToggleDisableDriveHold(bool enable);
void ToggleHideStyleRankHUD(bool enable);
void ToggleDTInfusedRoyalguardDetours(bool enable);
void ToggleTurnRateFix(bool enable);
void ToggleShootRemapDown(bool enable);
void ToggleFreeformSoftLockHelper(bool enable);
void ToggleDMC4LockOnDirection(bool enable);
void ToggleCustomCameraPositioning(bool enable);
void ToggleCustomCameraSensitivity(bool enable);	
void ToggleHoldToCrazyCombo(bool enable);
void AirTauntDetours(bool enable);
void RerouteRedOrbsCounterAlpha(bool enable, volatile uint16_t& alphaVar);
void ToggleEnsureAirRisingDragonLaunch(bool enable);
void ToggleAdjustAirRisingDragonLaunchHeight(bool enable);
void ToggleConfirmSetAction(bool enable);
void ToggleGreenOrbsMPRegen(bool enable);
void ToggleClassicHUDPositionings(bool enable);
void ToggleHideAndMutePortals(bool enable);
void ToggleStyleRankHudNoFadeout(bool enable);
void ToggleDanteTrickAlterations(bool enable);
void ToggleShotgunShlSpawnAnglePointBlank(bool enable);
void ToggleJudgementCutDetours(bool enable);
void ToggleFixBallsHangHitSpeed(bool enable);
void ToggleFixSecretMissionTimerFPS(bool enable);
void ToggleCerberusCrashFix(bool enable);
void ToggleVergilM3CrashFix(bool enable);
void ToggleMission5CrashFix(bool enable);
void ToggleCerbDamageFix(bool enable);
void ToggleArkhamPt2GrabCrashFix(bool enable);
void ToggleArkhamPt2DoppelCrashFix(bool enable);
void ToggleStyleLevellingCCSFix(bool enable);
void ToggleDTMustStyleArmor(bool enable);
void ToggleAllDetours(bool enable);
void CheckMissionResultBPScreen(bool enable);
void NoAirLunarPhaseLift(bool enable);
void FixEnemyAttackCooldowns(bool enable);
void CheckTotalResultsScreen(bool enable);
void ChargeMechanicsCPlayer(bool enable);
void CItemOrbPickupAllPlayers(bool enable);
void FasterSummonedSwords(bool enable);
void SummonedSwordsFormationShortcuts(bool enable);
void StormSwordsDownedEnemyFix(bool enable);
void FixMPXinputVibration(bool enable);
void AdjustAirWhirlwindLaunchHeight(bool enable);
void KillWeaponMotionState(bool enable);
void AirTauntRoseSwingDetours(bool enable);
void FixNevanShlPlayerSpawn(bool enable);
void NevanShlMarkRoseMode(bool enable);
void NevanShlSetToTravel(bool enable);
void ScreenShakeDetours(bool enable);
}
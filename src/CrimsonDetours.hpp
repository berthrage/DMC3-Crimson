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

// FasterTurnRate
void FasterTurnRateDetour();

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
}

bool g_HoldToCrazyComboFuncA(PlayerActorData& actorData);
bool DisableStaggerCheck(PlayerActorData& actorData);
void InitDetours();
void ToggleArtemisInstantFullCharge(bool enable);
void ToggleGuardGravityAlteration(bool enable);
void ToggleDisableDriveHold(bool enable);
void ToggleHideStyleRankHUD(bool enable);
void ToggleDTInfusedRoyalguardDetours(bool enable);
void ToggleFasterTurnRate(bool enable);
void ToggleShootRemapDown(bool enable);
void ToggleFreeformSoftLockHelper(bool enable);
void ToggleDMC4LockOnDirection(bool enable);
void ToggleCustomCameraPositioning(bool enable);
void ToggleCustomCameraSensitivity(bool enable);	
void ToggleHoldToCrazyCombo(bool enable);
void AirTauntDetours(bool enable);
void RerouteRedOrbsCounterAlpha(bool enable, volatile uint16_t& alphaVar);
void ToggleEnsureAirRisingDragonLaunch(bool enable);
void ToggleGreenOrbsMPRegen(bool enable);
void ToggleClassicHUDPositionings(bool enable);
void ToggleStyleRankHudNoFadeout(bool enable);
void ToggleDanteTrickAlterations(bool enable);

void DanteTeleportPlayerTrick();

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
}
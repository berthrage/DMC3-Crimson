#pragma once

#include "Core/DataTypes.hpp"
#include "Vars.hpp"

#pragma region GameplayImprovements

namespace CrimsonPatches {
void DisableHeightRestriction(bool enable);
void HoldToAutoFire(bool enable);
void EndBossFight(bool enable);
void IncreasedJCSpheres(bool enable);
void ImprovedBufferedReversals(bool enable);
void DisableJCRestriction(bool enable);
void BulletStop(bool enable);
void RainstormLift(bool enable);
void ToggleIncreasedEnemyJuggleTime(bool enable);
void DisableAirSlashKnockback(bool enable);
void ToggleIncreasedArtemisInstantChargeResponsiveness(bool enable);

#pragma endregion

#pragma region CameraStuff

void CameraSensController();
void CameraFollowUpSpeedController(CameraData& cameraData, CameraControlMetadata& cameraMetadata);
void CameraDistanceController(CameraControlMetadata& cameraMetadata);
void CameraLockOnDistanceController();
void CameraTiltController(CameraData* cameraData, CameraControlMetadata& cameraMetadata);
void ForceThirdPersonCamera(bool enable);
void DanteCoopTrick(bool enable);
void ToggleLockedOffCamera(bool enable);
void DisableLockOnCamera(bool enable);

#pragma endregion

#pragma region GraphicsStuff

void DisableBlendingEffects(bool enable);

#pragma endregion

#pragma region InertiaFixes

void InertiaFixes();

#pragma endregion

#pragma region AirTauntStuff

void ToggleRoyalguardForceJustFrameRelease(bool enable);
void OverrideTauntInAir(bool enable);
void AirTauntToggleController(byte8* actorBaseAddr);

#pragma endregion

#pragma region Fixing

void CerberusCrashFixPart2(bool enable);
void ToggleM6CrashFix(bool enable);

#pragma endregion

# pragma region Enemy

void DisableEnemyTargetting1PPosition(bool enable);
void ToggleDisableSoulEaterInvis(bool enable);
void SetEnemyDTMode(uint8 mode);
void DisableRegularEnemyAttacks(bool enable);

#pragma endregion

#pragma region Damage

void StopDamageToCerberus(bool enable);
void ReduceAirTornadoDamage(bool enable);
void ReduceArtemisProjectileDamage(bool enable);

#pragma endregion

#pragma region HudStuff

void SetRebOrbCounterDurationTillFadeOut(bool enable, float duration);
void ToggleHideLockOn(bool enable);

#pragma endregion

}

#pragma endregion
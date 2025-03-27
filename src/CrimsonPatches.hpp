#pragma once

#include "Core/DataTypes.hpp"
#include "Vars.hpp"

#pragma region GameplayImprovements

namespace CrimsonPatches {
void DisableHeightRestriction(bool enable);
void IncreasedJCSpheres(bool enable);
void ImprovedBufferedReversals(bool enable);
void DisableJCRestriction(bool enable);
void BulletStop(bool enable);
void RainstormLift(bool enable);
void ToggleIncreasedEnemyJuggleTime(bool enable);

#pragma endregion

#pragma region CameraStuff

void CameraSensController();
void CameraFollowUpSpeedController(CameraData* cameraData);
void CameraDistanceController(CameraData* cameraData);
void CameraLockOnDistanceController();
void CameraTiltController(CameraData* cameraData);
void ForceThirdPersonCamera(bool enable);
void ToggleLockedOffCamera(bool enable);
void DisableLockOnCamera(bool enable);

#pragma endregion

#pragma region GraphicsStuff

void DisableBlendingEffects(bool enable);

#pragma endregion

#pragma region InertiaFixes

void InertiaFixes();
void DisableAirSlashKnockback();

#pragma endregion

#pragma region AirTauntStuff

void ToggleRoyalguardForceJustFrameRelease(bool enable);
void OverrideTauntInAir(bool enable);
void AirTauntToggleController(byte8* actorBaseAddr);

#pragma endregion

#pragma region Fixing

void CerberusCrashFixPart2(bool enable);

#pragma endregion

#pragma region Damage

void StopDamageToCerberus(bool enable);
void ReduceAirTornadoDamage(bool enable);

#pragma endregion

#pragma region HudStuff

void SetRebOrbCounterDurationTillFadeOut(bool enable, float duration);

#pragma endregion

}

#pragma endregion
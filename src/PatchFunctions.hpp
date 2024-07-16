#pragma once

#include "Core/DataTypes.hpp"

#pragma region GameplayImprovements

void DisableHeightRestriction();
void IncreasedJCSpheres();
void ImprovedBufferedReversals();
void DisableJCRestriction();
void BulletStop();
void RainstormLift();

#pragma endregion

#pragma region CameraStuff

void CameraSensController();
void CameraFollowUpSpeedController();
void CameraDistanceController();
void CameraLockOnDistanceController();
void CameraTiltController();

#pragma endregion

#pragma region GraphicsStuff

void DisableBlendingEffects(bool enable);

#pragma endregion

#pragma region InertiaFixes

void LockedOffCameraToggle(bool enable);
void InertiaFixes();
void DisableAirSlashKnockback();

#pragma endregion

#pragma region AirTauntStuff

void ToggleRoyalguardForceJustFrameRelease(bool enable);
void OverrideTauntInAir(bool enable);
void AirTauntToggleController(byte8* actorBaseAddr);

#pragma endregion
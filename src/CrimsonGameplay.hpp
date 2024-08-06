#pragma once

#include "Core/DataTypes.hpp"

void ImprovedCancelsRoyalguardController(byte8* actorBaseAddr);
void ImprovedCancelsDanteController(byte8* actorBaseAddr);
void ImprovedCancelsVergilController(byte8* actorBaseAddr);
void VergilAdjustAirMovesPos(byte8* actorBaseAddr);
void CalculateAirStingerEndTime();
void SetAirStingerEnd(byte8* actorBaseAddr);
void FasterRapidSlashDevil(byte8* actorBaseAddr);
void FasterDarkslayerTricks();
void LastEventStateQueue(byte8* actorBaseAddr);
void FreeRotationSwordMoves(byte8* actorBaseAddr);
void StoreInertia(byte8* actorBaseAddr);
void InertiaController(byte8* actorBaseAddr);
void AerialRaveGravityTweaks(byte8* actorBaseAddr);
void AirFlickerGravityTweaks(byte8* actorBaseAddr);
void SkyDanceGravityTweaks(byte8* actorBaseAddr);
void StyleMeterDoppelganger(byte8* actorBaseAddr);
void inCombatDetection();
void DTReadySFX();
void BackToForwardInputs(byte8* actorBaseAddr);
void SprintAbility(byte8* actorBaseAddr);
void GunDTCharacterRemaps();
void CalculateCameraPlayerDistance(byte8* actorBaseAddr);
void DTExplosionFXController(byte8* actorBaseAddr);
void RoyalguardSFX(byte8* actorBaseAddr);
void RoyalguardRebalanced(byte8* actorBaseAddr);
void CorrectRoyalBlockUpdate(byte8* actorBaseAddr);
void CheckRoyalRelease(byte8* actorBaseAddr);
void CheckSkyLaunch(byte8* actorBaseAddr);
void SkyLaunchProperties(byte8* actorBaseAddr);
void DelayedComboFXController(byte8* actorBaseAddr);
void DriveTweaks(byte8* actorBaseAddr);
void StyleSwitchDrawText(byte8* actorBaseAddr);
void SetStyleSwitchDrawTextTime(int style, byte8* actorBaseAddr);
void StyleSwitchFlux(byte8* actorBaseAddr);
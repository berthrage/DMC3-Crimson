#pragma once

#include "Core/DataTypes.hpp"

namespace CrimsonGameplay {

void UpdateCrimsonPlayerData();
void ImprovedCancelsRoyalguardController(byte8* actorBaseAddr);
void ImprovedCancelsDanteController(byte8* actorBaseAddr);
void ImprovedCancelsVergilController(byte8* actorBaseAddr);
void VergilAdjustAirMovesPos(byte8* actorBaseAddr);
void CalculateAirStingerEndTime();
void SetAirStingerEnd(byte8* actorBaseAddr);
void FasterRapidSlashDevil(byte8* actorBaseAddr);
void FasterDarkslayerTricks();
float DrainDTBar(float currentDT, float maxDT, float elapsedTime, bool dtOn, float drainTimeDTOn, float drainTimeDTOff);
float DrainMirageGauge(float currentMP, float elapsedTime, float totalDrainTime);
void LastEventStateQueue(byte8* actorBaseAddr);
void FreeformSoftLockController(byte8* actorBaseAddr);
void StoreInertia(byte8* actorBaseAddr);
void InertiaController(byte8* actorBaseAddr);
void AerialRaveGravityTweaks(byte8* actorBaseAddr);
void AirFlickerGravityTweaks(byte8* actorBaseAddr);
void SkyDanceGravityTweaks(byte8* actorBaseAddr);
void StyleMeterDoppelganger(byte8* actorBaseAddr);
void InCombatDetection();
void BackToForwardInputs(byte8* actorBaseAddr);
void SprintAbility(byte8* actorBaseAddr);
void GunDTCharacterRemaps();
void DTInfusedRoyalguardController(byte8* actorBaseAddr);
void CalculateRotationTowardsEnemy(byte8* actorBaseAddr);
void SkyLaunchAirTauntController(byte8* actorBaseAddr);
void DriveTweaks(byte8* actorBaseAddr);

}
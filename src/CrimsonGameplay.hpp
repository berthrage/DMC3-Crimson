#pragma once

#include "Core/DataTypes.hpp"
#include "Vars.hpp"

namespace CrimsonGameplay {

extern bool startingFromGround[4];

void UpdateCrimsonPlayerData();
void FixAirStingerCancelTime(byte8* actorBaseAddr);
void ImprovedCancelsRoyalguardController(byte8* actorBaseAddr);
void ImprovedCancelsDanteController(byte8* actorBaseAddr);
void DarkslayerCancelsVergilController(byte8* actorBaseAddr);
void AirStingerJumpCancel(byte8* actorBaseAddr);
void VergilRisingStar(byte8* actorBaseAddr);
void VergilYamatoHighTime(byte8* actorBaseAddr);
void VergilAdjustAirMovesPos(byte8* actorBaseAddr);
void VergilAirTauntRisingSunDetection(byte8* actorBaseAddr);
void VergilAirRisingSun(byte8* actorBaseAddr);
void FasterDarkslayerTricks();
float DrainDTBar(float currentDT, float maxDT, float elapsedTime, bool dtOn, float drainTimeDTOn, float drainTimeDTOff);
float DrainMirageGauge(float currentMP, float elapsedTime, float totalDrainTime);
void LastEventStateQueue(byte8* actorBaseAddr);
void FreeformSoftLockController(byte8* actorBaseAddr);
void StoreInertia(byte8* actorBaseAddr);
void InertiaController(byte8* actorBaseAddr);
void GravityCorrections(byte8* actorBaseAddr);
void ShotgunAirShotTweaks(byte8* actorBaseAddr);
void AerialRaveGravityTweaks(byte8* actorBaseAddr);
void AirFlickerGravityTweaks(byte8* actorBaseAddr);
void SkyDanceGravityTweaks(byte8* actorBaseAddr);
void EbonyAndIvoryAerialTweaks(byte8* actorBaseAddr);
void DMC4Mobility(byte8* actorBaseAddr);
void AdjustDMC4MobilitySettings();
void StyleMeterDoppelganger(byte8* actorBaseAddr);
void InCombatDetection();
void BackToForwardInputs(byte8* actorBaseAddr);
void SprintAbility(byte8* actorBaseAddr);
void GunDTCharacterRemaps();
void DTInfusedRoyalguardController(byte8* actorBaseAddr);
void CalculateRotationTowardsEnemy(byte8* actorBaseAddr);
void GetLockedOnEnemyHitPoints(byte8* actorBaseAddr);
void GetLockedOnEnemyStunDisplacement(byte8* actorBaseAddr);
void CalculateLockedOnEnemyLastStunDisplacementValue(byte8* actorBaseAddr);
void GetLockedOnEnemyShield(byte8* actorBaseAddr);
void DetectCloseToEnemy(byte8* actorBaseAddr);
void SkyLaunchAirTauntController(byte8* actorBaseAddr);
void DriveTweaks(byte8* actorBaseAddr);
void GroundTrickFlagSet(byte8* actorBaseAddr);


}
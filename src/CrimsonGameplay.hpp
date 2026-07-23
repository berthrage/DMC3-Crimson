#pragma once

#include "Core/DataTypes.hpp"
#include "Vars.hpp"

namespace CrimsonGameplay {

template <typename T> bool CanQueueMeleeAttack(T& actorData) {
	using namespace NEXT_ACTION_REQUEST_POLICY;

	auto& policy = actorData.nextActionRequestPolicy[MELEE_ATTACK];

	if ((policy == BUFFER) || (policy == EXECUTE)) {
		return true;
	}

	return false;
}

template <typename T> bool CanQueueStyleAction(T& actorData) {
	using namespace NEXT_ACTION_REQUEST_POLICY;

	switch (actorData.style) {
	case STYLE::SWORDMASTER:
	case STYLE::GUNSLINGER: {
		auto& policy = actorData.nextActionRequestPolicy[SWORDMASTER_GUNSLINGER];

		if ((policy == BUFFER) || (policy == EXECUTE)) {
			return true;
		}

		break;
	}
	case STYLE::TRICKSTER: {
		auto& policy = actorData.nextActionRequestPolicy[TRICKSTER_DARK_SLAYER];

		if ((policy == BUFFER) || (policy == EXECUTE)) {
			return true;
		}

		break;
	}
	case STYLE::ROYALGUARD: {
		auto& policy = actorData.nextActionRequestPolicy[ROYALGUARD];

		if ((policy == BUFFER) || (policy == EXECUTE)) {
			return true;
		}

		break;
	}
	}

	return false;
}

extern bool startingFromGround[4];
bool IsActiveCharacterActor(byte8* actorBaseAddr);
void UpdateCrimsonPlayerData();
void FixAirStingerCancelTime(byte8* actorBaseAddr);
void DanteStingerInputCrazyCombo(byte8* actorBaseAddr);
void StoreAirCountsVergil(byte8* actorBaseAddr);
void AirCancelCountsTrackerVergil(byte8* actorBaseAddr);
void ImprovedCancelsRoyalguardController(byte8* actorBaseAddr);
void ImprovedCancelsDanteController(byte8* actorBaseAddr);
void DarkslayerCancelsVergilController(byte8* actorBaseAddr);
void AirStingerJumpCancel(byte8* actorBaseAddr);
void VergilTrackSwordFormationBuffer(byte8* actorBaseAddr);
void VergilRisingStar(byte8* actorBaseAddr);
void VergilYamatoHighTime(byte8* actorBaseAddr);
void VergilJudgementCutRework(byte8* actorBaseAddr);
void VergilAdjustAirMovesPos(byte8* actorBaseAddr);
void VergilAirTauntRisingSunDetection(byte8* actorBaseAddr);
void VergilAirRisingSun(byte8* actorBaseAddr);
void VergilDownertia(byte8* actorBaseAddr);
void FasterDarkslayerTricks();
float DrainDTBar(float currentDT, float maxDT, float elapsedTime, bool dtOn, float drainTimeDTOn, float drainTimeDTOff);
float DrainMirageGauge(float currentMP, float elapsedTime, float totalDrainTime);
void LastEventStateQueue(byte8* actorBaseAddr);
void FreeformSoftLockController(byte8* actorBaseAddr);
void ConsecutiveDirectionalMoves(byte8* actorBaseAddr);
void BulletMagnetism(byte8* actorBaseAddr);
void StoreInertia(byte8* actorBaseAddr);
void InertiaController(byte8* actorBaseAddr);
void GravityCorrections(byte8* actorBaseAddr);
void ShotgunAirShotTweaks(byte8* actorBaseAddr);
void AerialRaveGravityTweaks(byte8* actorBaseAddr);
void AirFlickerGravityTweaks(byte8* actorBaseAddr);
void SkyDanceGravityTweaks(byte8* actorBaseAddr);
void EbonyAndIvoryAerialTweaks(byte8* actorBaseAddr);
void DanteDownertia(byte8* actorBaseAddr);
void DMC4Mobility(byte8* actorBaseAddr);
void AdjustDMC4MobilitySettings();
void StyleMeterDoppelganger(byte8* actorBaseAddr);
void InCombatDetection();
void BackToForwardInputs(byte8* actorBaseAddr);
void SprintAbility(byte8* actorBaseAddr);
void StopSprintAbility(byte8* actorBaseAddr);
void DTInfusedRoyalguardController(byte8* actorBaseAddr);
void CalculateRotationTowardsEnemy(byte8* actorBaseAddr);
void GetLockedOnEnemyHitPoints(byte8* actorBaseAddr);
void GetLockedOnEnemyStunDisplacement(byte8* actorBaseAddr);
void CalculateLockedOnEnemyLastStunDisplacementValue(byte8* actorBaseAddr);
void GetLockedOnEnemyShield(byte8* actorBaseAddr);
void DetectCloseToEnemy(byte8* actorBaseAddr);
void SkyLaunchAirTauntController(byte8* actorBaseAddr);
void ToggleRebellionHoldDrive(bool enable);
void DanteDriveRework(byte8* actorBaseAddr);
void DanteShotgunBackslide(byte8* actorBaseAddr);
void GroundTrickFlagSet(byte8* actorBaseAddr);
bool CanBeRevived(uint8 playerIndex);
void RevivePlayer(uint8 playerIndex, bool ignoreGorbCost = true);
void UpdateRevivePlayer(byte8* actorBaseAddr);
void TeleportToPartyLeader(byte8* actorBaseAddr);
void BoBPartnerTeleport(byte8* actorBaseAddr);
void CharSwitchTeleportController(byte8* actorBaseAddr);

}
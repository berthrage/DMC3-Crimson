#include "../ThirdParty/glm/glm.hpp"
#include <algorithm>
#include "Core/Core.hpp"
#include <stdio.h>
#include "Utility/Detour.hpp"
#include <intrin.h>
#include <string>
#include "CrimsonUtil.hpp"
#include "DMC3Input.hpp"
#include "Global.hpp"

#include "Core/Macros.h"
#include "Config.hpp"
#include <iostream>
#include "CrimsonPatches.hpp"
#include "CrimsonLDK.hpp"
#include "Actor.hpp"

namespace CrimsonEnemyAITarget {

extern "C" {
	// StandardEnemyTarget
	std::uint64_t g_StandardEnemyTarget_ReturnAddr;
	void StandardEnemyTargetDetour();
	void* g_StandardEnemyTargetCheckCall;

	// EnigmaSetTargetAim
	std::uint64_t g_EngimaSetAim_ReturnAddr;
	void EnigmaSetTargetAimDetour();
	void* g_EnigmaSetAimCheckCall;

	// EnigmaSetRotationToTarget
	std::uint64_t g_EngimaSetRotation_ReturnAddr;
	void EnigmaSetRotationToTargetDetour();
	void* g_EnigmaSetRotationCheckCall;

	// BeowulfSetTargetAttack
	std::uint64_t g_BeowulfSetTarget_ReturnAddr;
	void BeowulfSetTargetAttackDetour();
	void* g_BeowulfSetTargetCheckCall;

	// FixMPLockOn
	std::uint64_t g_FixMPLockOn_ReturnAddr;
	void FixMPLockOnDetour();
	void* g_FixMPLockOnCheckCall;
}

uintptr_t EnemyTargetPlayerSelection(const glm::vec3& enemyPosition, bool hasValidEnemyPosition, uintptr_t defaultPlayerAddr) {
	// Returns the player address chosen by the enemy to target.
	glm::vec3 playerPosition[PLAYER_COUNT];
	float distanceToPlayer[PLAYER_COUNT];
	uintptr_t closestPlayerAddr = defaultPlayerAddr;
	float closestDistance = 9000.0f;

	for (uint8 playerIndex = 0; playerIndex < activeConfig.Actor.playerCount; playerIndex++) {
		auto& playerData = GetPlayerData(playerIndex);
		auto& characterData = GetCharacterData(playerIndex, playerData.characterIndex, ENTITY::MAIN);
		auto& newActorData = GetNewActorData(playerIndex, playerData.characterIndex, ENTITY::MAIN);

		if (!newActorData.baseAddr) {
			continue;
		}
		auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);

		playerPosition[playerIndex] = { actorData.position.x, actorData.position.y, actorData.position.z };

		if (!hasValidEnemyPosition)
			continue; // Skip distance check if we don't know the enemy position

		distanceToPlayer[playerIndex] = glm::distance(enemyPosition, playerPosition[playerIndex]);

		if (distanceToPlayer[playerIndex] < closestDistance) {
			closestDistance = distanceToPlayer[playerIndex];
			closestPlayerAddr = (uintptr_t)actorData.baseAddr;
		}
	}

	return closestPlayerAddr;
}

uintptr_t EnigmaTargetAimSwitch(uintptr_t ptrToLockedOnEnemyAddr) {
	auto pool_10222 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	while (!pool_10222 || !pool_10222[3]) {
		continue;
	}
	auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_10222[3]);

	if (!activeConfig.Actor.enable) {
		return (uintptr_t)mainActorData.baseAddr;
	}

	bool hasValidEnemyPos = false;
	glm::vec3 enemyPosition;

	if (ptrToLockedOnEnemyAddr && ptrToLockedOnEnemyAddr > 0x10000) {
		auto& enemy = *reinterpret_cast<EnemyActorData*>(ptrToLockedOnEnemyAddr - 0x60);
		if (enemy.enemy == ENEMY::GERYON) {
			return (uintptr_t)mainActorData.baseAddr;
		}
		enemyPosition = { enemy.position.x, enemy.position.y, enemy.position.z };
		hasValidEnemyPos = true;
	}

	return EnemyTargetPlayerSelection(enemyPosition, hasValidEnemyPos, (uintptr_t)mainActorData.baseAddr);
}

uintptr_t FetchEnemyForLockOnTargetting(uintptr_t playerAddr) {
	auto& actorData = *reinterpret_cast<PlayerActorData*>(playerAddr);
	auto pool_2128 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	while (!pool_2128 || !pool_2128[8]) continue;
	auto& enemyVectorData = *reinterpret_cast<EnemyVectorData*>(pool_2128[8]);

	uintptr_t enemyAddr;

	for (std::size_t enemyIndex = 0; enemyIndex < enemyVectorData.count; ++enemyIndex) {
		auto& enemyMeta = enemyVectorData.metadata[enemyIndex];
		if (!enemyMeta.baseAddr) continue;
		auto& enemyActorData = *reinterpret_cast<EnemyActorData*>(enemyMeta.baseAddr);
		if (!enemyActorData.baseAddr) continue;

		if (enemyIndex == 0) {
			enemyAddr = (uintptr_t)enemyActorData.baseAddr + 0x60;
			//actorData.lockOnData.targetPosition = enemyActorData.position;
		}
	}


	return enemyAddr;
}



void EnemyAIMultiplayerTargettingDetours(bool enable) {
	using namespace Utility;

	static bool run = false;
	if (run == enable) {
		return;
	}

// 	// StandardEnemyTarget
	// dmc3.exe + 616EB - 0F 28 82 80 00 00 00 - movaps xmm0, [rdx + 00000080] { Standard Enemy Set Target Position }
	// dmc3.exe + 616F2 - 0F 29 41 40 - movaps[rcx + 40], xmm0{ followup to standard target pos }
	// Works for all Hells enemies.
	static std::unique_ptr<Utility::Detour_t> StandardEnemyTargetHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x616EB, &StandardEnemyTargetDetour, 7);
	g_StandardEnemyTarget_ReturnAddr = StandardEnemyTargetHook->GetReturnAddress();
	g_StandardEnemyTargetCheckCall = &EnigmaTargetAimSwitch;
	StandardEnemyTargetHook->Toggle(false);

	// EngimaSetTargetAim
	// dmc3.exe + 1BB04C - 44 0F 28 89 80 00 00 00 - movaps xmm9, [rcx + 00000080] { Enigma Set Target Position to Aim 1, player in rcx, enemyaddr pointer is in r12+0x28 }
	// Fixes Lady Aiming on secondary targets. But not enigmas.
	static std::unique_ptr<Utility::Detour_t> EnigmaSetTargetAimHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1BB04C, &EnigmaSetTargetAimDetour, 8);
	g_EngimaSetAim_ReturnAddr = EnigmaSetTargetAimHook->GetReturnAddress();
	g_EnigmaSetAimCheckCall = &EnigmaTargetAimSwitch; 
	EnigmaSetTargetAimHook->Toggle(enable);

	// EngimaSetRotationToTarget
	// dmc3.exe + 2C6813 - 0F 10 01 - movups xmm0, [rcx] { Enigma Rotate to Target
	// dmc3.exe + 2C6816 - 0F 5C C1 - subps xmm0, xmm1 -- we have to replicate it in the detour
	// Geryon bugs out, Beowulf rotates to secondary players but doesn't attack them
	static std::unique_ptr<Utility::Detour_t> EnigmaSetRotationToTargetHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2C6813, &EnigmaSetRotationToTargetDetour, 6);
	g_EngimaSetRotation_ReturnAddr = EnigmaSetRotationToTargetHook->GetReturnAddress();
	g_EnigmaSetRotationCheckCall = &EnigmaTargetAimSwitch;
	EnigmaSetRotationToTargetHook->Toggle(false);

	// BeowulfSetTargetAttack
	// dmc3.exe + 2C688F - 0F 10 01 - movups xmm0, [rcx] { Dullahan Maybe This is used ? Also Judgement Cut }
	// dmc3.exe + 2C6892 - 48 8B F2 - mov rsi, rdx
	// Doesn't work. Crashes with Dullahans
	static std::unique_ptr<Utility::Detour_t> BeowulfSetTargetHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2C688F, &BeowulfSetTargetAttackDetour, 6);
	g_BeowulfSetTarget_ReturnAddr = BeowulfSetTargetHook->GetReturnAddress();
	g_BeowulfSetTargetCheckCall = &EnigmaTargetAimSwitch;
	BeowulfSetTargetHook->Toggle(false);

	// FixMPLockOn
	// dmc3.exe + 1E8A0E - 48 89 8B 28 63 00 00 - mov[rbx + 00006328], rcx{ Choose Target for Lock On without locking on }
	static std::unique_ptr<Utility::Detour_t> FixMPLockOnHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1E8A0E, &FixMPLockOnDetour, 7);
	g_FixMPLockOn_ReturnAddr = FixMPLockOnHook->GetReturnAddress();
	g_FixMPLockOnCheckCall = &FetchEnemyForLockOnTargetting;
	FixMPLockOnHook->Toggle(false);



	run = enable;
}

}
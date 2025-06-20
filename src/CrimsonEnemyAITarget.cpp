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

	// DullahanMaybeUsed
	std::uint64_t g_DullahanMaybeUsed_ReturnAddr;
	void DullahanMaybeUsedDetour();
	void* g_DullahanMaybeUsedCheckCall;

	// BeowulfAttackTargetPos
	std::uint64_t g_BeowulfAttackTarget_ReturnAddr;
	std::uint64_t g_BeowulfAttackTarget_CallAddr;
	void BeowulfAttackTargetDetour();
	void* g_BeowulfAttackTargetCheckCall;

	// BloodgoyleDiveTargetPos
	std::uint64_t g_BloodgoyleDiveTarget_ReturnAddr;
	void BloodgoyleDiveTargetDetour();
	void* g_BloodgoyleDiveTargetCheckCall;

	// BloodgoyleRotationTarget
	std::uint64_t g_BloodgoyleRotationTarget_ReturnAddr;
	std::uint64_t g_BloodgoyleRotationTarget_CallAddr;
	void BloodgoyleRotationTargetDetour();
	void* g_BloodgoyleRotationTargetCheckCall;	

	// ChessPawnAttackTarget
	std::uint64_t g_ChessPawnAttackTarget_ReturnAddr;
	std::uint64_t g_ChessPawnAttackTarget_CallAddr;
	void ChessPawnAttackTargetDetour();
	void* g_ChessPawnAttackTargetCheckCall;

	// ChessKnightAttackTarget
	std::uint64_t g_ChessKnightAttackTarget_ReturnAddr;
	std::uint64_t g_ChessKnightAttackTarget_CallAddr;
	void ChessKnightAttackTargetDetour();
	void* g_ChessKnightAttackTargetCheckCall;

	// ChessBishopRotationTarget
	std::uint64_t g_ChessBishopRotationTarget_ReturnAddr;
	std::uint64_t g_ChessBishopRotationTarget_CallAddr;
	void ChessBishopRotationTargetDetour();
	void* g_ChessBishopRotationTargetCheckCall;

	// ChessBishopAttackTarget
	std::uint64_t g_ChessBishopAttackTarget_ReturnAddr;
	std::uint64_t g_ChessBishopAttackTarget_CallAddr;
	void ChessBishopAttackTargetDetour();
	void* g_ChessBishopAttackTargetCheckCall;

	// ChessBishopAttack2Target
	std::uint64_t g_ChessBishopAttack2Target_ReturnAddr;
	std::uint64_t g_ChessBishopAttack2Target_CallAddr;
	void ChessBishopAttack2TargetDetour();
	void* g_ChessBishopAttack2TargetCheckCall;

	// ChessBishopAttack2TargetDetour2
	std::uint64_t g_ChessBishopAttack2Target2_ReturnAddr;
	std::uint64_t g_ChessBishopAttack2Target2_CallAddr;
	std::uint64_t g_ChessBishopAttack2Target2_JmpAddr;
	void ChessBishopAttack2TargetDetour2();

	// ChessBishopAttack2TargetDetour3
	std::uint64_t g_ChessBishopAttack2Target3_ReturnAddr;
	std::uint64_t g_ChessBishopAttack2Target3_CallAddr;
	std::uint64_t g_ChessBishopAttack2Target3_JmpAddr;
	void ChessBishopAttack2TargetDetour3();


	// ChessRookAttackTarget
	std::uint64_t g_ChessRookAttackTarget_ReturnAddr;
	std::uint64_t g_ChessRookAttackTarget_CallAddr;
	void ChessRookAttackTargetDetour();
	void* g_ChessRookAttackTargetCheckCall;

	// ChessRookAttackTargetDetour2
	std::uint64_t g_ChessRookAttackTarget2_ReturnAddr;
	std::uint64_t g_ChessRookAttackTarget2_CallAddr;
	std::uint64_t g_ChessRookAttackTarget2_JmpAddr;
	void ChessRookAttackTargetDetour2();

	// ChessRookAttackTargetDetour3
	std::uint64_t g_ChessRookAttackTarget3_ReturnAddr;
	std::uint64_t g_ChessRookAttackTarget3_CallAddr;
	std::uint64_t g_ChessRookAttackTarget3_JmpAddr;
	void ChessRookAttackTargetDetour3();

	// ChessKingAttackTarget
	std::uint64_t g_ChessKingAttackTarget_ReturnAddr;
	std::uint64_t g_ChessKingAttackTarget_CallAddr;
	std::uint64_t g_ChessKingAttackTarget_JmpAddr;
	void ChessKingAttackTargetDetour();
	void* g_ChessKingAttackTargetCheckCall;
	
	// ArachneCirclingAroundDetour
	std::uint64_t g_ArachneCirclingAround_ReturnAddr;
	void ArachneCirclingAroundDetour();
	void* g_ArachneCirclingAroundCheckCall;

	// ArachneJumpAttackDetour
	std::uint64_t g_ArachneJumpAttack_ReturnAddr;
	void ArachneJumpAttackDetour();
	void* g_ArachneJumpAttackCheckCall;

	// VergilBlinkPositionDetour
	std::uint64_t g_VergilBlinkPosition_ReturnAddr;
	void VergilBlinkPositionDetour();
	void* g_VergilBlinkPositionCheckCall;

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

		if (actorData.dead) {
			continue; // Skip dead players
		}

		distanceToPlayer[playerIndex] = glm::distance(enemyPosition, playerPosition[playerIndex]);

		if (distanceToPlayer[playerIndex] < closestDistance) {

			closestDistance = distanceToPlayer[playerIndex];
			closestPlayerAddr = (uintptr_t)actorData.baseAddr;
		}
	}

	return closestPlayerAddr;
}

uintptr_t EnemyTargetAimSwitchPlayerAddr(uintptr_t ptrToLockedOnEnemyAddr) {
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
	// Works for all Hells enemies
	// Hell Vanguard
	// Doppelganger
	// Lady
	// Vergil (basic attacks only)
	// Agni and Rudra
	// The Fallen
	// Dullahans (not sword attack)
	static std::unique_ptr<Utility::Detour_t> StandardEnemyTargetHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x616EB, &StandardEnemyTargetDetour, 7);
	g_StandardEnemyTarget_ReturnAddr = StandardEnemyTargetHook->GetReturnAddress();
	g_StandardEnemyTargetCheckCall = &EnemyTargetAimSwitchPlayerAddr;
	StandardEnemyTargetHook->Toggle(enable);

	// EngimaSetTargetAim
	// dmc3.exe + 1BB04C - 44 0F 28 89 80 00 00 00 - movaps xmm9, [rcx + 00000080] { Enigma Set Target Position to Aim 1, player in rcx, enemyaddr pointer is in r12+0x28 }
	// Fixes Lady Aiming on secondary targets. But not enigmas.
	static std::unique_ptr<Utility::Detour_t> EnigmaSetTargetAimHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1BB04C, &EnigmaSetTargetAimDetour, 8);
	g_EngimaSetAim_ReturnAddr = EnigmaSetTargetAimHook->GetReturnAddress();
	g_EnigmaSetAimCheckCall = &EnemyTargetAimSwitchPlayerAddr; 
	EnigmaSetTargetAimHook->Toggle(false);

	// EngimaSetRotationToTarget
	// dmc3.exe + 2C6813 - 0F 10 01 - movups xmm0, [rcx] { Enigma Rotate to Target
	// dmc3.exe + 2C6816 - 0F 5C C1 - subps xmm0, xmm1 
	// We use this to find out calls.
	static std::unique_ptr<Utility::Detour_t> EnigmaSetRotationToTargetHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2C6813, &EnigmaSetRotationToTargetDetour, 6);
	g_EngimaSetRotation_ReturnAddr = EnigmaSetRotationToTargetHook->GetReturnAddress();
	g_EnigmaSetRotationCheckCall = &EnemyTargetAimSwitchPlayerAddr;
	EnigmaSetRotationToTargetHook->Toggle(false);

	// DullahanMaybeUsed
	// dmc3.exe + 2C688F - 0F 10 01 - movups xmm0, [rcx] { Dullahan Maybe This is used ? Also Judgement Cut }
	// dmc3.exe + 2C6892 - 48 8B F2 - mov rsi, rdx
	// PlayerPos in RCX, EnemyPos in RDX
	// We use this to find out calls.
	static std::unique_ptr<Utility::Detour_t> DullahanMaybeUsedHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2C688F, &DullahanMaybeUsedDetour, 6);
	g_DullahanMaybeUsed_ReturnAddr = DullahanMaybeUsedHook->GetReturnAddress();
	g_DullahanMaybeUsedCheckCall = &EnemyTargetAimSwitchPlayerAddr;
	DullahanMaybeUsedHook->Toggle(false);

	// BeowulfAttackTargetPos
	// dmc3.exe+2C68C2 - E8 69 7D 06 00           - call dmc3.exe+32E630 { Beowulf Attack Distance Function }
	// PlayerPos in RCX, EnemyPos in RDX
	static std::unique_ptr<Utility::Detour_t> BeowulfAttackTargetHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2C68C2, &BeowulfAttackTargetDetour, 5);
	g_BeowulfAttackTarget_ReturnAddr = BeowulfAttackTargetHook->GetReturnAddress();
	g_BeowulfAttackTarget_CallAddr = (uintptr_t)appBaseAddr + 0x32E630;
	g_BeowulfAttackTargetCheckCall = &EnemyTargetAimSwitchPlayerAddr;
	BeowulfAttackTargetHook->Toggle(enable);


	// BloodgoyleDiveTargetPos
	// dmc3.exe+E7EC2 - 44 0F 10 02            - movups xmm8,[rdx] { Bloodgoyle Dive Target Position}
	// dmc3.exe+E7EC6 - 44 0F 5C C0            - subps xmm8,xmm0
	static std::unique_ptr<Utility::Detour_t> BloodgoyleDiveTargetHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0xE7EC2, &BloodgoyleDiveTargetDetour, 8);
	g_BloodgoyleDiveTarget_ReturnAddr = BloodgoyleDiveTargetHook->GetReturnAddress();
	g_BloodgoyleDiveTargetCheckCall = &EnemyTargetAimSwitchPlayerAddr;
	BloodgoyleDiveTargetHook->Toggle(enable);

	// BloodgoyleRotationTarget
	// dmc3.exe+E8142 - E8 C9 E6 1D 00           - call dmc3.exe+2C6810 { BloodgoyleRotationFunc }
	// PlayerPos in RCX, EnemyPos in RDX
	// Also works for ChessPawn and ChessKnight
	static std::unique_ptr<Utility::Detour_t> BloodgoyleRotationTargetHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0xE8142, &BloodgoyleRotationTargetDetour, 5);
	g_BloodgoyleRotationTarget_ReturnAddr = BloodgoyleRotationTargetHook->GetReturnAddress();
	g_BloodgoyleRotationTarget_CallAddr = (uintptr_t)appBaseAddr + 0x2C6810; // BloodgoyleRotationFunc
	g_BloodgoyleRotationTargetCheckCall = &EnemyTargetAimSwitchPlayerAddr;
	BloodgoyleRotationTargetHook->Toggle(enable);

	// ChessPawnAttackTargetDetour
	// dmc3.exe+F25B8 - E8 B3421D00           - call dmc3.exe+2C6870 { ChessPawnAttackTarget }
	// PlayerPos in RCX, EnemyPos in RDX
	static std::unique_ptr<Utility::Detour_t> ChessPawnAttackTargetHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0xF25B8, &ChessPawnAttackTargetDetour, 5);
	g_ChessPawnAttackTarget_ReturnAddr = ChessPawnAttackTargetHook->GetReturnAddress();
	g_ChessPawnAttackTarget_CallAddr = (uintptr_t)appBaseAddr + 0x2C6870; // ChessPawnAttackTarget
	g_ChessPawnAttackTargetCheckCall = &EnemyTargetAimSwitchPlayerAddr;
	ChessPawnAttackTargetHook->Toggle(enable);

	// ChessKnightAttackTargetDetour
	// dmc3.exe+F2C98 - E8 D33B1D00           - call dmc3.exe+2C6870 { ChessKnightAttackTarget }
	// PlayerPos in RCX, EnemyPos in RDX
	static std::unique_ptr<Utility::Detour_t> ChessKnightAttackTargetHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0xF2C98, &ChessKnightAttackTargetDetour, 5);
	g_ChessKnightAttackTarget_ReturnAddr = ChessKnightAttackTargetHook->GetReturnAddress();
	g_ChessKnightAttackTarget_CallAddr = (uintptr_t)appBaseAddr + 0x2C6870; // ChessKnightAttackTarget
	g_ChessKnightAttackTargetCheckCall = &EnemyTargetAimSwitchPlayerAddr;
	ChessKnightAttackTargetHook->Toggle(enable);


	// ChessBishopRotationTarget
	// dmc3.exe+EF871 - E8 9A 6F 1D 00           - call dmc3.exe+2C6810 { ChessBishopRotationTarget }
	// EnemyPos in RDX
	// Affects movement for every ChessPiece other than Pawns and Knights. Also affects Queen Attacks
	static std::unique_ptr<Utility::Detour_t> ChessBishopRotationTargetHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0xEF871, &ChessBishopRotationTargetDetour, 5);
	g_ChessBishopRotationTarget_ReturnAddr = ChessBishopRotationTargetHook->GetReturnAddress();
	g_ChessBishopRotationTarget_CallAddr = (uintptr_t)appBaseAddr + 0x2C6810; 
	g_ChessBishopRotationTargetCheckCall = &EnemyTargetAimSwitchPlayerAddr;
	ChessBishopRotationTargetHook->Toggle(enable);

	// ChessBishopAttackTargetDetour
	// dmc3.exe+F3DB3 - E8 B8 2A 1D 00           - call dmc3.exe+2C6870 { ChessBishopAttackTarget }
	// PlayerPos in RCX, EnemyPos in RDX
	// Works for JudgementCut
	static std::unique_ptr<Utility::Detour_t> ChessBishopAttackTargetHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0xF3DB3, &ChessBishopAttackTargetDetour, 5);
	g_ChessBishopAttackTarget_ReturnAddr = ChessBishopAttackTargetHook->GetReturnAddress();
	g_ChessBishopAttackTarget_CallAddr = (uintptr_t)appBaseAddr + 0x2C6870; // ChessBishopAttackTarget
	g_ChessBishopAttackTargetCheckCall = &EnemyTargetAimSwitchPlayerAddr;
	ChessBishopAttackTargetHook->Toggle(enable);

	// ChessBishopAttack2TargetDetour
	// dmc3.exe+F3423 - E8 18 DD FF FF           - call dmc3.exe+F1140 { ChessBishopAttack2Func }
	// PlayerPos in R8, EnemyPos in RDX
	static std::unique_ptr<Utility::Detour_t> ChessBishopAttack2Hook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0xF3423, &ChessBishopAttack2TargetDetour, 5);
	g_ChessBishopAttack2Target_ReturnAddr = ChessBishopAttack2Hook->GetReturnAddress();
	g_ChessBishopAttack2Target_CallAddr = (uintptr_t)appBaseAddr + 0xF1140; // ChessBishopAttack2Func
	g_ChessBishopAttack2TargetCheckCall = &EnemyTargetAimSwitchPlayerAddr;
	ChessBishopAttack2Hook->Toggle(enable);

	// ChessBishopAttack2TargetDetour2
	// dmc3.exe+F342D - E8 4E4DFFFF           - call dmc3.exe+E8180
	// dmc3.exe+E818F - E9 5C642400           - jmp dmc3.exe+32E5F0 < -- inside dmc3.exe+E8180
	// PlayerPos in R8
	static std::unique_ptr<Utility::Detour_t> ChessBishopAttack2TargetHook2 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0xF342D, &ChessBishopAttack2TargetDetour2, 5);
	g_ChessBishopAttack2Target2_ReturnAddr = ChessBishopAttack2TargetHook2->GetReturnAddress();
	g_ChessBishopAttack2Target2_CallAddr = (uintptr_t)appBaseAddr + 0xE8180; 
	g_ChessBishopAttack2Target2_JmpAddr = (uintptr_t)appBaseAddr + 0x32E5F0; // ChessBishopAttack2Func
	ChessBishopAttack2TargetHook2->Toggle(enable);

	// ChessBishopAttack2TargetDetour3
	// dmc3.exe+F343A - E8 414DFFFF           - call dmc3.exe+E8180
	// dmc3.exe+E818F - E9 5C642400           - jmp dmc3.exe+32E5F0 < -- inside dmc3.exe+E8180
	static std::unique_ptr<Utility::Detour_t> ChessBishopAttack2TargetHook3 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0xF343A, &ChessBishopAttack2TargetDetour3, 5);
	g_ChessBishopAttack2Target3_ReturnAddr = ChessBishopAttack2TargetHook3->GetReturnAddress();
	g_ChessBishopAttack2Target3_CallAddr = (uintptr_t)appBaseAddr + 0xE8180;
	g_ChessBishopAttack2Target3_JmpAddr = (uintptr_t)appBaseAddr + 0x32E5F0; 
	ChessBishopAttack2TargetHook3->Toggle(enable);


	// ChessRookAttackTargetDetour
	// dmc3.exe+F5037 - E8 E4 C1 FF FF           - call dmc3.exe+F1220 { ChessRookAttack }
	// PlayerPos in R8, EnemyPos in RDX
	static std::unique_ptr<Utility::Detour_t> ChessRookAttackHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0xF5037, &ChessRookAttackTargetDetour, 5);
	g_ChessRookAttackTarget_ReturnAddr = ChessRookAttackHook->GetReturnAddress();
	g_ChessRookAttackTarget_CallAddr = (uintptr_t)appBaseAddr + 0xF1220; // ChessRookAttack
	g_ChessRookAttackTargetCheckCall = &EnemyTargetAimSwitchPlayerAddr;
	ChessRookAttackHook->Toggle(enable);

	// ChessRookAttackTargetDetour2
	// dmc3.exe+F5041 - E8 3A31FFFF           - call dmc3.exe+E8180
	// dmc3.exe+E818F - E9 5C642400           - jmp dmc3.exe+32E5F0 < -- inside dmc3.exe+E8180
	static std::unique_ptr<Utility::Detour_t> ChessRookAttackTargetHook2 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0xF5041, &ChessRookAttackTargetDetour2, 5);
	g_ChessRookAttackTarget2_ReturnAddr = ChessRookAttackTargetHook2->GetReturnAddress();
	g_ChessRookAttackTarget2_CallAddr = (uintptr_t)appBaseAddr + 0xE8180;
	g_ChessRookAttackTarget2_JmpAddr = (uintptr_t)appBaseAddr + 0x32E5F0; 
	ChessRookAttackTargetHook2->Toggle(enable);

	// ChessRookAttackTargetDetour3
	// dmc3.exe+F504E - E8 2D31FFFF           - call dmc3.exe+E8180
	// dmc3.exe+E818F - E9 5C642400           - jmp dmc3.exe+32E5F0 < -- inside dmc3.exe+E8180
	static std::unique_ptr<Utility::Detour_t> ChessRookAttackTargetHook3 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0xF504E, &ChessRookAttackTargetDetour3, 5);
	g_ChessRookAttackTarget3_ReturnAddr = ChessRookAttackTargetHook3->GetReturnAddress();
	g_ChessRookAttackTarget3_CallAddr = (uintptr_t)appBaseAddr + 0xE8180;
	g_ChessRookAttackTarget3_JmpAddr = (uintptr_t)appBaseAddr + 0x32E5F0;
	ChessRookAttackTargetHook3->Toggle(enable);

	// ChessKingAttackTargetDetour
	// dmc3.exe+F6D89 - E8 F213FFFF           - call dmc3.exe+E8180
	// EnemyPos in RCX
	// dmc3.exe+E818F - E9 5C642400           - jmp dmc3.exe+32E5F0 < -- inside dmc3.exe+E8180
	// Gotta load the PlayerPos into RDX
	static std::unique_ptr<Utility::Detour_t> ChessKingAttackHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0xF6D89, &ChessKingAttackTargetDetour, 5);
	g_ChessKingAttackTarget_ReturnAddr = ChessKingAttackHook->GetReturnAddress();
	g_ChessKingAttackTarget_CallAddr = (uintptr_t)appBaseAddr + 0x2C6870; 
	g_ChessKingAttackTarget_JmpAddr = (uintptr_t)appBaseAddr + 0x32E5F0; 
	g_ChessKingAttackTargetCheckCall = &EnemyTargetAimSwitchPlayerAddr;
	ChessKingAttackHook->Toggle(enable);

	// ArachneCirclingAroundDetour
	// dmc3.exe+C5A40 - 0F 28 83 80 00 00 00      - movaps xmm0,[rbx+00000080] { Arachne Set Target Position for Circling Around }
	// Player in RBX, LockedOnEnemyAddr in RDI+60h
	static std::unique_ptr<Utility::Detour_t> ArachneCirclingAroundHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0xC5A40, &ArachneCirclingAroundDetour, 7);
	g_ArachneCirclingAround_ReturnAddr = ArachneCirclingAroundHook->GetReturnAddress();
	g_ArachneCirclingAroundCheckCall = &EnemyTargetAimSwitchPlayerAddr;
	ArachneCirclingAroundHook->Toggle(enable);

	// ArachneJumpAttackDetour
	// dmc3.exe+BF553 - 0F 28 82 80 00 00 00      - movaps xmm0,[rdx+00000080] { Arachne Set Target Position for Jump Attack }
	// Player in RDX, LockedOnEnemyAddr in RCX+60h
	static std::unique_ptr<Utility::Detour_t> ArachneJumpAttackHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0xBF553, &ArachneJumpAttackDetour, 7);
	g_ArachneJumpAttack_ReturnAddr = ArachneJumpAttackHook->GetReturnAddress();
	g_ArachneJumpAttackCheckCall = &EnemyTargetAimSwitchPlayerAddr;
	ArachneJumpAttackHook->Toggle(enable);

	// VergilBlinkPositionDetour
	// dmc3.exe+1871C5 - 0F 58 81 80 00 00 00      - addps xmm0,[rcx+00000080] { Vergil Blink/Trick Target Position }
	// Player in RCX, LockedOnEnemyAddr in RDI+60h
	static std::unique_ptr<Utility::Detour_t> VergilBlinkPositionHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1871C5, &VergilBlinkPositionDetour, 7);
	g_VergilBlinkPosition_ReturnAddr = VergilBlinkPositionHook->GetReturnAddress();
	g_VergilBlinkPositionCheckCall = &EnemyTargetAimSwitchPlayerAddr;
	VergilBlinkPositionHook->Toggle(enable);


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
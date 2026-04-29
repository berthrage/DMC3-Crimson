// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include <thread>
#include <chrono>
#include <math.h>
#include "../ThirdParty/glm/glm.hpp"
#include "../ThirdParty/ImGui/imgui.h"
#include <ctime>
#include <iostream>
#include <cstdio>
#include "Utility/Detour.hpp"
#include "DebugDrawDX11.hpp"
#define DEBUG_DRAW_EXPLICIT_CONTEXT
#include "debug_draw.hpp"
#include "CrimsonDetours.hpp"
#include "DMC3Input.hpp"
#include "File.hpp"
#include "Internal.hpp"
#include "ActorBase.hpp"
#include "CrimsonGameplay.hpp"
#include "Core/Core.hpp"
#include "CrimsonSDL.hpp"
#include "Memory.hpp"
#include "Model.hpp"
#include "CrimsonSDL.hpp"
#include "Config.hpp"
#include "Exp.hpp"
#include "Global.hpp"
#include "Vars.hpp"
#include "StyleSwitchFX.hpp"

#include "Core/Macros.h"
#include <deque>
#include "Training.hpp"
#include "CrimsonUtil.hpp"
#include "CrimsonEfk.hpp"

namespace CrimsonFX {

#pragma region GeneralFX


void DTReadySFX() {
    auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

    auto pool_12405 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
    if (!pool_12405 || !pool_12405[3]) {
        return;
    }
    auto& actorData = *reinterpret_cast<PlayerActorData*>(pool_12405[3]);
    auto playerIndex = actorData.newPlayerIndex;


    if (actorData.magicPoints >= 3000 && !devilTriggerReadyPlayed) {
        CrimsonSDL::PlayDevilTriggerReady(playerIndex);
        devilTriggerReadyPlayed = true;
    } else if (actorData.magicPoints < 3000) {
        devilTriggerReadyPlayed = false;
    }
}

void CalculateViewProperties(byte8* actorBaseAddr) {
	if (!actorBaseAddr) return;

	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
	if (!actorData.cloneActorBaseAddr) return;
	auto& cloneActorData = *reinterpret_cast<PlayerActorData*>(actorData.cloneActorBaseAddr);

	auto pool_10222 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_10222 || !pool_10222[3]) return;
	auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_10222[3]);

	auto pool_4449 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC8FBD0);
	if (!pool_4449 || !pool_4449[147]) return;
	auto& cameraData = *reinterpret_cast<CameraData*>(pool_4449[147]);
	
	auto pool_11962 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
	if (!pool_11962 || !pool_11962[8]) return;
	auto& eventData = *reinterpret_cast<EventData*>(pool_11962[8]);

	static bool doppelActive[PLAYER_COUNT] = { false };

    auto playerIndex = actorData.newPlayerIndex;
	auto indexToAssign = playerIndex * 2;
	auto cloneIndexToAssign = playerIndex * 2 + 1;
    
	glm::vec3 playerPosition = { actorData.position.x, actorData.position.y, actorData.position.z };
	glm::vec3 clonePosition = { cloneActorData.position.x, cloneActorData.position.y, cloneActorData.position.z };
	glm::vec3 mainActorPosition = { mainActorData.position.x, mainActorData.position.y, mainActorData.position.z };
	glm::vec3 cameraPosition = { cameraData.data[0].x, cameraData.data[0].y, cameraData.data[0].z };
	glm::vec3 lockedEnemyPosition = { actorData.lockOnData.targetPosition.x, actorData.lockOnData.targetPosition.y, actorData.lockOnData.targetPosition.z };
	glm::vec3 cloneLockedEnemyPosition = { cloneActorData.lockOnData.targetPosition.x, cloneActorData.lockOnData.targetPosition.y, cloneActorData.lockOnData.targetPosition.z };

	auto& playerTo1PDistance = crimsonPlayer[playerIndex].playerTo1PDistance;
	auto& cloneTo1PDistance = crimsonPlayer[playerIndex].cloneTo1PDistance;
	auto& playerScreenPosition = crimsonPlayer[playerIndex].playerScreenPosition;
	auto& cloneScreenPosition = crimsonPlayer[playerIndex].cloneScreenPosition;
	auto& lockedEnemyScreenPosition = crimsonPlayer[playerIndex].lockedEnemyScreenPosition;
	auto& cloneLockedEnemyScreenPosition = crimsonPlayer[playerIndex].cloneLockedEnemyScreenPosition;
    auto& cameraPlayerDistance = crimsonPlayer[playerIndex].cameraPlayerDistance;
	auto& cameraCloneDistance = crimsonPlayer[playerIndex].cameraCloneDistance;
	auto& cameraLockedEnemyDistance = crimsonPlayer[playerIndex].cameraLockedEnemyDistance;
	auto& cameraCloneLockedEnemyDistance = crimsonPlayer[playerIndex].cameraCloneLockedEnemyDistance;
	auto& playerOutOfView = crimsonPlayer[playerIndex].playerOutOfView;
	auto& cloneOutOfView = crimsonPlayer[playerIndex].cloneOutOfView;

	playerTo1PDistance = glm::distance(playerPosition, mainActorPosition);
	cloneTo1PDistance = glm::distance(clonePosition, mainActorPosition);
	g_plEntityTo1PDistances[indexToAssign] = playerTo1PDistance;
	g_plEntityTo1PDistances[cloneIndexToAssign] = cloneTo1PDistance;

	playerScreenPosition = debug_draw_world_to_screen((const float*)&actorData.position, 1.0f);
	cloneScreenPosition = debug_draw_world_to_screen((const float*)&cloneActorData.position, 1.0f);
	//lockedEnemyScreenPosition = debug_draw_world_to_screen((const float*)&actorData.lockOnData.targetPosition, 1.0f);
	cloneLockedEnemyScreenPosition = debug_draw_world_to_screen((const float*)&cloneActorData.lockOnData.targetPosition, 1.0f);

	g_plEntityScreenPositions[indexToAssign] = playerScreenPosition;
	g_plEntityScreenPositions[cloneIndexToAssign] = cloneScreenPosition;
	
    cameraPlayerDistance = glm::distance(playerPosition, cameraPosition);
	int distancePlayer = (int)cameraPlayerDistance / 20;
	crimsonPlayer[playerIndex].cameraPlayerDistanceClamped = glm::clamp(distancePlayer, 0, 255);
	g_plEntityCameraDistances[indexToAssign] = cameraPlayerDistance;

	cameraCloneDistance = glm::distance(clonePosition, cameraPosition);
	int distanceClone = (int)cameraCloneDistance / 20;
	crimsonPlayer[playerIndex].cameraCloneDistanceClamped = glm::clamp(distanceClone, 0, 255);
	g_plEntityCameraDistances[cloneIndexToAssign] = cameraCloneDistance;

// 	cameraLockedEnemyDistance = glm::distance(lockedEnemyPosition, cameraPosition);
// 	int distanceLockedEnemy = (int)cameraLockedEnemyDistance / 20;
// 	crimsonPlayer[playerIndex].cameraLockedEnemyDistanceClamped = glm::clamp(distanceLockedEnemy, 0, 255);

	cameraCloneLockedEnemyDistance = glm::distance(cloneLockedEnemyPosition, cameraPosition);
	int distanceCloneLockedEnemy = (int)cameraCloneLockedEnemyDistance / 20;
	crimsonPlayer[playerIndex].cameraCloneLockedEnemyDistanceClamped = glm::clamp(distanceCloneLockedEnemy, 0, 255);

	// Calculate screen-relative angle for audio positioning
	float screenCenterX = g_renderSize.x * 0.5f;
	float screenOffsetX = playerScreenPosition.x - screenCenterX;
	float normalizedOffset = screenOffsetX / (g_renderSize.x * 0.5f); // Range: -1.0 to 1.0

	// Convert to angle range suitable for SDL Mix_SetPosition (typically -180 to 180)
	float angle = normalizedOffset * 90.0f; // Scale to -90 to +90 degrees

	// Apply deadzone for center audio positioning
	const float AUDIO_DEADZONE_ANGLE = 15.0f;
	if (std::abs(angle) < AUDIO_DEADZONE_ANGLE) {
		angle = 0.0f; // Force center audio
	}

	// Same calculation for clone
	float cloneScreenOffsetX = cloneScreenPosition.x - screenCenterX;
	float cloneNormalizedOffset = cloneScreenOffsetX / (g_renderSize.x * 0.5f);
	float angleClone = cloneNormalizedOffset * 90.0f;

	if (std::abs(angleClone) < AUDIO_DEADZONE_ANGLE) {
		angleClone = 0.0f; 
	}

	crimsonPlayer[playerIndex].playerScreenAngle = static_cast<int>(std::round(angle));
	crimsonPlayer[playerIndex].cloneScreenAngle = static_cast<int>(std::round(angleClone));


	float screenWidth = g_renderSize.x;
	float screenHeight = g_renderSize.y;
	const float screenMargin = 50.0f;

	if (playerScreenPosition.x < screenMargin || playerScreenPosition.x > screenWidth - screenMargin ||
		playerScreenPosition.y < screenMargin || playerScreenPosition.y > screenHeight - screenMargin) {

		playerOutOfView = true;
		g_plEntityOutOfView[indexToAssign] = true;
	}
	else {
		playerOutOfView = false;
		g_plEntityOutOfView[indexToAssign] = false;
	}


	if (cloneScreenPosition.x < screenMargin || cloneScreenPosition.x > screenWidth - screenMargin ||
		cloneScreenPosition.y < screenMargin || cloneScreenPosition.y > screenHeight - screenMargin) {

		cloneOutOfView = true;
		g_plEntityOutOfView[cloneIndexToAssign] = true;
	}
	else {
		cloneOutOfView = false;
		g_plEntityOutOfView[cloneIndexToAssign] = false;
	}

	g_plEntityPositions[indexToAssign].x = actorData.position.x;
	g_plEntityPositions[indexToAssign].y = actorData.position.y;
	g_plEntityPositions[indexToAssign].z = actorData.position.z;

	g_plEntityPositions[cloneIndexToAssign].x = cloneActorData.position.x;
	g_plEntityPositions[cloneIndexToAssign].y = cloneActorData.position.y;
	g_plEntityPositions[cloneIndexToAssign].z = cloneActorData.position.z;

	if (actorData.doppelganger == 1 && !doppelActive[playerIndex]) {
		g_activeClonesCount++;
		doppelActive[playerIndex] = true;
	}
	
	
	if (doppelActive[playerIndex] && actorData.doppelganger == 0) {
		g_activeClonesCount--;
		doppelActive[playerIndex] = false;
	}
}

void DTExplosionFXController(byte8* actorBaseAddr) {
	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
	auto pool_10298 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
	if (!pool_10298 || !pool_10298[8]) {
		return;
	}
	auto& eventData = *reinterpret_cast<EventData*>(pool_10298[8]);

    auto playerIndex = actorData.newPlayerIndex;
    auto& maxDT = actorData.maxMagicPoints;
    auto&sfxStarted = crimsonPlayer[playerIndex].dTESFX.started;
    auto& sfxLooped = crimsonPlayer[playerIndex].dTESFX.looped;
    auto& sfxFinished = crimsonPlayer[playerIndex].dTESFX.finished;
    auto& releaseVolumeMult = crimsonPlayer[playerIndex].dTESFX.releaseVolumeMult;
    auto& vfxStarted = crimsonPlayer[playerIndex].dTEVFX.started;
    auto& vfxFinished = crimsonPlayer[playerIndex].dTEVFX.finished;
    auto& gamepad = GetGamepad(playerIndex);
    auto& distance = crimsonPlayer[playerIndex].cameraPlayerDistanceClamped;
	static bool pausedSFX = false;

	// INTERRUPT ON DEATH
	if (actorData.dead && sfxStarted) {
		CrimsonSDL::StopDevilTriggerLoop(playerIndex);
		return;
	}

    // SET RELEASE VOLUME MULTIPLIER
    if (actorData.dtExplosionCharge > 3000) {
        releaseVolumeMult = actorData.dtExplosionCharge / 10000;
    }
    else if (actorData.dtExplosionCharge > 200 &&  actorData.dtExplosionCharge < 3000) {
        releaseVolumeMult = 0;
    }
    
    // SFX START
	if (actorData.dtExplosionCharge > 500 && !sfxStarted) {
		CrimsonSDL::PlayDTExplosionStart(playerIndex);

        sfxStarted = true;
	}

    // SFX LOOP
    if (!CrimsonSDL::DTEStartIsPlaying(playerIndex) && sfxStarted && !sfxLooped) {
		CrimsonSDL::PlayDTExplosionLoop(playerIndex);
        sfxLooped = true;
    }

    // SFX FINISH
	if (actorData.dtExplosionCharge >= maxDT) {
		CrimsonSDL::InterruptDTExplosionSFX(playerIndex);
		if (!sfxFinished) {
			CrimsonSDL::PlayDTExplosionFinish(playerIndex);
		}
        
		sfxFinished = true;
	}

	// We handle SFX Loop Pause in CrimsonOnTick::PauseSFXWhenPaused().

	uint8 vfxColorDante[4] = { 48, 0, 10, 255 };
	uint8 vfxColorVergil[4] = { 2, 16, 43, 255 };
	uint32 actualColor = (actorData.character == CHARACTER::DANTE) ? CrimsonUtil::Uint8toAABBGGRR(activeCrimsonConfig.VFX.dtExplosionColorDante) :
		CrimsonUtil::Uint8toAABBGGRR(activeCrimsonConfig.VFX.dtExplosionColorVergil);
	// VFX START
	if (actorData.dtExplosionCharge > 2500 && !vfxStarted && !vfxFinished) {
        crimsonPlayer[playerIndex].dTEVFX.time = 0;
		auto pPlayer = (void*)crimsonPlayer[playerIndex].playerPtr;
		
		if (activeCrimsonConfig.VFX.dtExplosionVFX) CrimsonDetours::CreateEffectDetour(pPlayer, 3, 61, 1, true, actualColor, 0.4f);

		vfxStarted = true;
	}

	// VFX FINISH
	if (actorData.dtExplosionCharge >= maxDT && !vfxFinished) {
		auto pPlayer = (void*)crimsonPlayer[playerIndex].playerPtr;
		if (activeCrimsonConfig.VFX.dtExplosionVFX) CrimsonDetours::CreateEffectDetour(pPlayer, 3, 41, 1, true, actualColor, 1.0f);

		vfxFinished = true;
	}
    
    // RELEASE
    if (!(gamepad.buttons[0] & GetBinding(BINDING::DEVIL_TRIGGER)) && sfxStarted && !actorData.dead) {
		CrimsonSDL::InterruptDTExplosionSFX(playerIndex);
		if (actorData.dtExplosionCharge > 0) {
			CrimsonSDL::PlayDTEExplosionRelease(playerIndex, releaseVolumeMult);
		}

        if (releaseVolumeMult > 0.4f) {
            CrimsonSDL::VibrateController(actorData.newPlayerIndex, 0, 0x5555 * releaseVolumeMult, 800);
        }
        
        if (releaseVolumeMult > 0.4f && actorData.dtExplosionCharge > 0) {
			auto pPlayer = (void*)crimsonPlayer[playerIndex].playerPtr;
			if (activeCrimsonConfig.VFX.dtExplosionVFX) CrimsonDetours::CreateEffectDetour(pPlayer, 3, 61, 1,true, actualColor, 1.0f);
        }

        sfxStarted = false;
        sfxLooped = false;
        sfxFinished = false;
        vfxStarted = false;
        vfxFinished = false;
    }
}

void StyleRankHudFadeoutController() {
	// This function exists so that the fadeout still occurs if you're D Rank, even with "Disable Style Rank Fadeout" enabled.

	 // IntroduceMainActorData
	auto pool_12857 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_12857 || !pool_12857[3]) {
		return;
	}


	auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_12857[3]);


	if (activeConfig.disableStyleRankHudFadeout) {
		if (mainActorData.styleData.rank <= 0) {
			CrimsonDetours::ToggleStyleRankHudNoFadeout(false);
		}
		else {
			CrimsonDetours::ToggleStyleRankHudNoFadeout(true);
		}
	}

}

#pragma endregion

#pragma region DanteSpecificFX

void RoyalBlockFX(byte8* actorBaseAddr) {
	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
	auto playerIndex = actorData.newPlayerIndex;
	auto& event = actorData.eventData[0].event;
	auto& motionDataIndex = actorData.motionData[0].index;
	auto& playerData = GetPlayerData(actorData);
	bool inRoyalBlock = (actorData.royalBlock == 3);
	bool inNormalBlock = (actorData.royalBlock == 0 || actorData.royalBlock == 4);
	bool inStaggeredGuardBreak = (actorData.royalBlock == 2 || (actorData.royalBlock == 6 && event == 44 && actorData.guard));
	bool inGuardBreak = ((event == 20 && motionDataIndex == 2) && !inNormalBlock);
	bool ensureIsMainPlayer = ((actorData.newCharacterIndex == playerData.activeCharacterIndex) && (actorData.newEntityIndex == ENTITY::MAIN));

	// Convert static variables to arrays
	static bool guardPlayed[PLAYER_COUNT] = { false };
	static bool royalBlockPlayed[PLAYER_COUNT] = { false };
	static bool normalBlockPlayed[PLAYER_COUNT] = { false };

	// GUARD SFX -- SCRAPPED
// 	if (actorData.guard) {
// 		if (!guardPlayed) {
// 			PlayGuard(playerIndex);
// 			guardPlayed = true;
// 		}
// 		// Reset the debounce timer when guard is active
// 		guardLastChangedTime = now;
// 	}
// 	else {
// 		auto durationSinceGuardChanged = std::chrono::duration_cast<std::chrono::milliseconds>(now - guardLastChangedTime).count();
// 		if (durationSinceGuardChanged > guardDebounceTimeMs) {
// 			if (guardPlayed) {
// 				guardPlayed = false;
// 			}
// 		}
// 	}

	if (ensureIsMainPlayer) {

		// ROYAL BLOCK SFX
		if (inRoyalBlock) {
			if (!royalBlockPlayed[playerIndex] && activeCrimsonConfig.VFX.royalBlockVFX) {
				//std::cout << "royal block played" << std::endl;
				CrimsonSDL::PlayRoyalBlock(playerIndex);
				uint32 actualColor = CrimsonUtil::Uint8toAABBGGRR(activeCrimsonConfig.VFX.royalBlockColor);
				CrimsonDetours::CreateEffectDetour(actorBaseAddr, 3, 61, 15, true, actualColor, 1.7f);
				royalBlockPlayed[playerIndex] = true;
			}
		}
		else {
			royalBlockPlayed[playerIndex] = false;
		}

		if (actorData.magicPoints >= 2000) {
			// NORMAL BLOCK SFX
			if (inNormalBlock) {
				if (!normalBlockPlayed[playerIndex]) {
					
					normalBlockPlayed[playerIndex] = true;
				}
			}
			else {
				normalBlockPlayed[playerIndex] = false;
			}

		}

		if (actorData.magicPoints >= 2000) {
			// GUARD BREAK
			// for DT-Infused Royalguard only
			if (actorData.royalBlock == 1) {
				if (!guardPlayed[playerIndex]) {
					CrimsonSDL::PlayNormalBlock(playerIndex);
					guardPlayed[playerIndex] = true;
				}
			}
			else {
				guardPlayed[playerIndex] = false;
			}


		}



	}
}

template <typename T> auto GetMeleeWeapon(T& actorData) {
	auto& characterData = GetCharacterData(actorData);

	return characterData.meleeWeapons[characterData.meleeWeaponIndex];
}

void DelayedComboFXController(byte8* actorBaseAddr) {
	using namespace ACTION_DANTE;

	if (!actorBaseAddr) {
		return;
	}

	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
	if (!CrimsonGameplay::IsActiveCharacterActor(actorData)) return;
	if (!activeCrimsonConfig.VFX.delayedComboVFX) return;
	if (actorData.character != CHARACTER::DANTE) return;
	auto& motionIndex = actorData.motionData[0].index;

	auto playerIndex = actorData.newPlayerIndex;
	auto entityIndex = actorData.newEntityIndex;
	auto weapon = GetMeleeWeapon(actorData);
	auto& gamepad = GetGamepad(playerIndex);
	auto inAttack = (actorData.eventData[0].event == 17);
	auto inRebellionCombo1 = (actorData.action == REBELLION_COMBO_1_PART_1 && motionIndex == 3 && inAttack);
	auto inRebellionCombo2 = (actorData.action == REBELLION_COMBO_2_PART_2 && motionIndex == 6 && inAttack);
	auto inCerberusCombo2 = (actorData.action == CERBERUS_COMBO_1_PART_2 && motionIndex == 4 && inAttack);
	auto inAgniCombo1 = (actorData.action == AGNI_RUDRA_COMBO_1_PART_1 && motionIndex == 3 && inAttack);
	auto inAgniCombo2 = (actorData.action == AGNI_RUDRA_COMBO_2_PART_2 && motionIndex == 8 && inAttack);
	auto inBeoCombo1 = (actorData.action == BEOWULF_COMBO_1_PART_2 &&
		(motionIndex == 4 || motionIndex == 43 || motionIndex == 44) && inAttack);
	auto inBeoCombo2 = (actorData.action == BEOWULF_COMBO_2_PART_3 &&
		(motionIndex == 7 || motionIndex == 47 || motionIndex == 48) && inAttack);
	auto meleeWeapon = actorData.newWeapons[actorData.meleeWeaponIndex];
	auto& delayedComboFX = (entityIndex == ENTITY::MAIN) ? crimsonPlayer[playerIndex].delayedComboFX :
		crimsonPlayer[playerIndex].delayedComboFXClone;
	auto& actionTimer = (entityIndex == ENTITY::MAIN) ? crimsonPlayer[playerIndex].actionTimer :
		crimsonPlayer[playerIndex].actionTimerClone;
	auto& motionTimer = (entityIndex == ENTITY::MAIN) ? crimsonPlayer[playerIndex].motionTimer :
		crimsonPlayer[playerIndex].motionTimerClone;

	bool meleeDown = (gamepad.buttons[0] & GetBinding(BINDING::MELEE_ATTACK)) != 0;

	if (!actorData.newWeaponDataAddr[weapon]) {
		return;
	}
	static constexpr const wchar_t* delayedComboFXPath[6] = {
			L"Crimson\\vfx\\delayedcombo\\rebellion.efkefc",
			L"Crimson\\vfx\\delayedcombo\\cerberus.efkefc",
			L"Crimson\\vfx\\delayedcombo\\agni_rudra_1.efkefc",
			L"Crimson\\vfx\\delayedcombo\\agni_rudra_2.efkefc",
			L"Crimson\\vfx\\delayedcombo\\beowulf_arms.efkefc",
			L"Crimson\\vfx\\delayedcombo\\beowulf_legs.efkefc",
	};
	static EffekseerRefHandle delayedComboFXRef = CrimsonEfk::LoadEffect(delayedComboFXPath[weapon], 1.0f);

	cDrawReverse playerDanteCDraw = actorData.newModelData[actorData.activeModelIndexMirror]; // activeModelIndex == which DT or Non-DT model
	Matrix44Ptr* playerBoneMatrix = reinterpret_cast<Matrix44Ptr*>(playerDanteCDraw.bonesMatrixesPtr);

	if (inRebellionCombo1) {
		delayedComboFX.duration = 0.485f;
		delayedComboFX.weaponThatStartedMove = WEAPON::REBELLION;
	}
	else if (inRebellionCombo2) {
		delayedComboFX.duration = 0.85f;
		delayedComboFX.weaponThatStartedMove = WEAPON::REBELLION;
	}
	else if (inCerberusCombo2) {
		delayedComboFX.duration = 0.55f;
		delayedComboFX.weaponThatStartedMove = WEAPON::CERBERUS;
	}
	else if (inAgniCombo1) {
		delayedComboFX.duration = 0.53f;
		delayedComboFX.weaponThatStartedMove = WEAPON::AGNI_RUDRA;
	}
	else if (inAgniCombo2) {
		delayedComboFX.duration = 0.70f;
		delayedComboFX.weaponThatStartedMove = WEAPON::AGNI_RUDRA;
	}
	else if (inBeoCombo1) {
		delayedComboFX.duration = 0.5f; // Beowulf's time can be very inconsistent due to charge time (the more you charge the less you
		// need to wait between delays)
		delayedComboFX.weaponThatStartedMove = WEAPON::BEOWULF_DANTE;
	}
	else if (inBeoCombo2 && motionIndex == 7) {
		delayedComboFX.duration = 1.1f;
		delayedComboFX.weaponThatStartedMove = WEAPON::BEOWULF_DANTE;
	}
	else if (inBeoCombo2 && (motionIndex == 47 || motionIndex == 48)) {
		delayedComboFX.duration = 0.60f;
		delayedComboFX.weaponThatStartedMove = WEAPON::BEOWULF_DANTE;
	}

	if ((inRebellionCombo1 || inRebellionCombo2 || inCerberusCombo2 || inAgniCombo1 || inAgniCombo2 || inBeoCombo1 || inBeoCombo2) &&
		delayedComboFX.playCount == 0 && weapon == delayedComboFX.weaponThatStartedMove) {
		bool playedEffect = false;

		// VFX
		// uint32 actualColor = CrimsonUtil::Uint8toAABBGGRR(activeCrimsonConfig.VFX.delayedComboColor);
		// CrimsonDetours::CreateEffectDetour(pPlayer, delayedComboFX.bank, delayedComboFX.id, 1, true, actualColor, 1.2f);
		auto& danteWeapon = *reinterpret_cast<WeaponData*>(actorData.newWeaponDataAddr[weapon]);
		cDrawReverse* danteWeaponCDraw = reinterpret_cast<cDrawReverse*>(danteWeapon.weaponCDraw);
		Matrix44Ptr* weaponBoneMatrix = reinterpret_cast<Matrix44Ptr*>(danteWeaponCDraw[0].bonesMatrixesPtr);


		if (weapon == WEAPON::REBELLION && actionTimer >= delayedComboFX.duration) {
			delayedComboFXRef = CrimsonEfk::ReloadEffect(delayedComboFXRef, delayedComboFXPath[weapon], 1.0f);
			delayedComboFX.efkHandle = CrimsonEfk::PlayEffectAtMatrix(delayedComboFXRef, weaponBoneMatrix[0].matrix1, actorData);
			playedEffect = true;
		}
		else if (weapon == WEAPON::CERBERUS && actionTimer >= delayedComboFX.duration) {
			delayedComboFXRef = CrimsonEfk::ReloadEffect(delayedComboFXRef, delayedComboFXPath[weapon], 1.0f);
			delayedComboFX.efkHandle = CrimsonEfk::PlayEffectAtMatrix(delayedComboFXRef, weaponBoneMatrix[0].matrix2, actorData);
			delayedComboFX.efkHandle = CrimsonEfk::PlayEffectAtMatrix(delayedComboFXRef, weaponBoneMatrix[0].matrix9, actorData);
			delayedComboFX.efkHandle = CrimsonEfk::PlayEffectAtMatrix(delayedComboFXRef, weaponBoneMatrix[0].matrix10, actorData);
			delayedComboFX.efkHandle = CrimsonEfk::PlayEffectAtMatrix(delayedComboFXRef, weaponBoneMatrix[0].matrix11, actorData);

			// INCLUDE 2 (additional for extra brightness), 9 (held rod), 10 (rod 2), 11 (rod 3)
			playedEffect = true;
		}
		else if (weapon == WEAPON::AGNI_RUDRA && actionTimer >= delayedComboFX.duration) {
			delayedComboFXRef = CrimsonEfk::ReloadEffect(delayedComboFXRef, delayedComboFXPath[weapon], 1.0f);
			delayedComboFX.efkHandle = CrimsonEfk::PlayEffectAtMatrix(delayedComboFXRef, weaponBoneMatrix[0].matrix2, actorData);
			delayedComboFXRef = CrimsonEfk::ReloadEffect(delayedComboFXRef, delayedComboFXPath[weapon + 1], 1.0f);
			delayedComboFX.efkHandle = CrimsonEfk::PlayEffectAtMatrix(delayedComboFXRef, weaponBoneMatrix[0].matrix3, actorData);
			playedEffect = true;
		}
		else if (weapon == WEAPON::BEOWULF_DANTE && (inBeoCombo1 || inBeoCombo2) && motionTimer >= delayedComboFX.duration) {
			if (inBeoCombo2 && delayedComboFX.transitioningToHyperFist) {
				return;
			}
			delayedComboFXRef = CrimsonEfk::ReloadEffect(delayedComboFXRef, delayedComboFXPath[weapon], 1.0f);
			delayedComboFX.efkHandle = CrimsonEfk::PlayEffectAtMatrix(delayedComboFXRef, playerBoneMatrix[0].matrix10, actorData); // right hand
			delayedComboFX.efkHandle = CrimsonEfk::PlayEffectAtMatrix(delayedComboFXRef, playerBoneMatrix[0].matrix14, actorData); // left hand
			delayedComboFXRef = CrimsonEfk::ReloadEffect(delayedComboFXRef, delayedComboFXPath[weapon + 1], 1.0f);
			delayedComboFX.efkHandle = CrimsonEfk::PlayEffectAtMatrix(delayedComboFXRef, playerBoneMatrix[0].matrix19, actorData); // right foot
			delayedComboFX.efkHandle = CrimsonEfk::PlayEffectAtMatrix(delayedComboFXRef, playerBoneMatrix[0].matrix23, actorData); // left foot
			playedEffect = true;
		}

		if (playedEffect) {
			// SFX
			if (activeCrimsonConfig.SFX.delayedComboEffectType == DELAYEDCOMBOSFX::TYPE_A) {
				CrimsonSDL::PlayDelayedCombo2(actorData.newPlayerIndex);
			}
			else {
				CrimsonSDL::PlayDelayedCombo1(actorData.newPlayerIndex);
			}

			// VIBRATION
			CrimsonSDL::VibrateController(playerIndex, 0, 0x5555, 130);
			delayedComboFX.playCount++;
		}
	}
	else if (actionTimer < 0.485f) {
		delayedComboFX.playCount = 0;
		delayedComboFX.transitioningToHyperFist = false;
	}

	if (!(inRebellionCombo1 || inRebellionCombo2 || inCerberusCombo2 || inAgniCombo1 || inAgniCombo2 || inBeoCombo1 || inBeoCombo2)) {
		CrimsonEfk::StopEffect(delayedComboFX.efkHandle);
	}
}

void StyleSwitchFluxCrimson(byte8* actorBaseAddr, EffekseerHandle* styleSwitchHandles, EffekseerHandle* swooshHandles, 
	uint8 style, uint32_t color, uint32_t customSwooshColor) {
	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);

	static constexpr const wchar_t* styleSwitchEffectPath[7] = {
			L"Crimson\\vfx\\styleswitch\\crimsonvers\\swordmaster.efkefc",
			L"Crimson\\vfx\\styleswitch\\crimsonvers\\gunslinger.efkefc",
			L"Crimson\\vfx\\styleswitch\\crimsonvers\\trickster.efkefc",
			L"Crimson\\vfx\\styleswitch\\crimsonvers\\royalguard.efkefc",
			L"Crimson\\vfx\\styleswitch\\crimsonvers\\quicksilver.efkefc",
			L"Crimson\\vfx\\styleswitch\\crimsonvers\\doppelganger.efkefc",
			L"Crimson\\vfx\\styleswitch\\crimsonvers\\darkslayer.efkefc",
	};

	static constexpr const wchar_t* styleSwitchSwooshEffectPath[7] = {
			L"Crimson\\vfx\\styleswitch\\crimsonvers\\swordmaster_swoosh.efkefc",
			L"Crimson\\vfx\\styleswitch\\crimsonvers\\gunslinger_swoosh.efkefc",
			L"Crimson\\vfx\\styleswitch\\crimsonvers\\trickster_swoosh.efkefc",
			L"Crimson\\vfx\\styleswitch\\crimsonvers\\royalguard_swoosh.efkefc",
			L"Crimson\\vfx\\styleswitch\\crimsonvers\\quicksilver_swoosh.efkefc",
			L"Crimson\\vfx\\styleswitch\\crimsonvers\\doppelganger_swoosh.efkefc",
			L"Crimson\\vfx\\styleswitch\\crimsonvers\\darkslayer_swoosh.efkefc",
	};

	static EffekseerRefHandle styleSwitchRef = CrimsonEfk::LoadEffect(styleSwitchEffectPath[style], 40.0f);
	static EffekseerRefHandle styleSwitchSwooshRef = CrimsonEfk::LoadEffect(styleSwitchSwooshEffectPath[style], 40.0f);

	uint8 handleId = 0;

	for (uint8 i = 0; i < 10; i++) {
		if (CrimsonEfk::IsPlaying(styleSwitchHandles[i])) {
			if (i == 9) {
				handleId = 0;
			}
			else {
				handleId++;
			}
		}
	}

	cDrawReverse playerDantecDraw = actorData.newModelData[actorData.activeModelIndexMirror]; // activeModelIndex == which DT or Non-DT model
	Matrix44Ptr* boneMatrix = reinterpret_cast<Matrix44Ptr*>(playerDantecDraw.bonesMatrixesPtr); 

    
	styleSwitchRef = CrimsonEfk::ReloadEffect(styleSwitchRef, styleSwitchEffectPath[style], 40.0f);
	styleSwitchHandles[handleId] = CrimsonEfk::PlayEffectAtMatrix(styleSwitchRef, boneMatrix->matrix3, &actorData);

	styleSwitchSwooshRef = CrimsonEfk::ReloadEffect(styleSwitchSwooshRef, styleSwitchSwooshEffectPath[style], 40.0f);
	swooshHandles[handleId] = CrimsonEfk::PlayEffectAtMatrix(styleSwitchSwooshRef, boneMatrix->matrix3, &actorData);
	CrimsonEfk::SetAllColor(styleSwitchHandles[handleId], color);
	if (activeCrimsonConfig.StyleSwitchFX.Flux.customSwooshColors) {
		CrimsonEfk::SetAllColor(swooshHandles[handleId], customSwooshColor);
	}
	else {
		CrimsonEfk::SetAllColor(swooshHandles[handleId], color);
	}
}

void StyleSwitchFluxNS(byte8* actorBaseAddr, EffekseerHandle* styleSwitchHandles, EffekseerHandle* swooshHandles, uint8 style, uint32_t color) {
	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);

	static constexpr const wchar_t* styleSwitchEffectPath[7] = {
			L"Crimson\\vfx\\styleswitch\\nswitchvers\\swordmaster.efkefc",
			L"Crimson\\vfx\\styleswitch\\nswitchvers\\gunslinger.efkefc",
			L"Crimson\\vfx\\styleswitch\\nswitchvers\\trickster.efkefc",
			L"Crimson\\vfx\\styleswitch\\nswitchvers\\royalguard.efkefc",
			L"Crimson\\vfx\\styleswitch\\nswitchvers\\quicksilver.efkefc",
			L"Crimson\\vfx\\styleswitch\\nswitchvers\\doppelganger.efkefc",
			L"Crimson\\vfx\\styleswitch\\nswitchvers\\darkslayer.efkefc",
	};

	static constexpr const wchar_t* styleSwitchSwooshEffectPath[7] = {
			L"Crimson\\vfx\\styleswitch\\nswitchvers\\swordmaster_swoosh.efkefc",
			L"Crimson\\vfx\\styleswitch\\nswitchvers\\gunslinger_swoosh.efkefc",
			L"Crimson\\vfx\\styleswitch\\nswitchvers\\trickster_swoosh.efkefc",
			L"Crimson\\vfx\\styleswitch\\nswitchvers\\royalguard_swoosh.efkefc",
			L"Crimson\\vfx\\styleswitch\\nswitchvers\\quicksilver_swoosh.efkefc",
			L"Crimson\\vfx\\styleswitch\\nswitchvers\\doppelganger_swoosh.efkefc",
			L"Crimson\\vfx\\styleswitch\\nswitchvers\\darkslayer_swoosh.efkefc",
	};

	static EffekseerRefHandle styleSwitchRef = CrimsonEfk::LoadEffect(styleSwitchEffectPath[style], 40.0f);
	static EffekseerRefHandle styleSwitchSwooshRef = CrimsonEfk::LoadEffect(styleSwitchSwooshEffectPath[style], 40.0f);

	uint8 handleId = 0;

	for (uint8 i = 0; i < 10; i++) {
		if (CrimsonEfk::IsPlaying(styleSwitchHandles[i])) {
			if (i == 9) {
				handleId = 0;
			}
			else {
				handleId++;
			}
		}
	}

	cDrawReverse playerDantecDraw = actorData.newModelData[actorData.activeModelIndexMirror]; // activeModelIndex == which DT or Non-DT model
	Matrix44Ptr* boneMatrix = reinterpret_cast<Matrix44Ptr*>(playerDantecDraw.bonesMatrixesPtr);


	styleSwitchRef = CrimsonEfk::ReloadEffect(styleSwitchRef, styleSwitchEffectPath[style], 40.0f);
	styleSwitchHandles[handleId] = CrimsonEfk::PlayEffectAtMatrix(styleSwitchRef, boneMatrix->matrix3, &actorData);

	styleSwitchSwooshRef = CrimsonEfk::ReloadEffect(styleSwitchSwooshRef, styleSwitchSwooshEffectPath[style], 40.0f);
	swooshHandles[handleId] = CrimsonEfk::PlayEffectAtMatrix(styleSwitchSwooshRef, boneMatrix->matrix3, &actorData);
	CrimsonEfk::SetAllColor(styleSwitchHandles[handleId], color);
}

void StyleSwitchFlux(byte8* actorBaseAddr) {
	// Deprecated, goodbye buggy DevilFlux, we had good times. - Mia
    if (!actorBaseAddr) {   
        return;
    }
    auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    auto playerIndex = actorData.newPlayerIndex;
    auto* fluxtime = &crimsonPlayer[playerIndex].fluxtime;
    auto* style = &actorData.style;
	auto* canStart = &crimsonPlayer[playerIndex].fluxCanStart;
	auto* canEnd = &crimsonPlayer[playerIndex].fluxCanEnd;
    auto& gamepad = GetGamepad(playerIndex);
    
    if (*fluxtime > 0) {
		
		

		float delayTime1 = 0.006f;
		float delayTime2 = 0.05f;
		//auto speedValue = (IsTurbo()) ? activeConfig.Speed.turbo : activeConfig.Speed.mainSpeed;
        if (*canStart) {
			styleVFXCount++;
			styleChanged[*style] = true;
            DevilFluxVFX_1F94D0(actorData, DEVIL_FLUX::START);
            *canStart = false;
            *canEnd = true;
        }
		

		if (*fluxtime < 0.08f && *canEnd) {
			styleVFXCount--;
			DevilFluxVFX_1F94D0(actorData, 4);
			styleChanged[*style] = false;
			*canEnd = false;
            *fluxtime = 0;
            
		}

	}

    if (*fluxtime <= 0 && !*canStart) {
		for (int i = 0; i < 6; i++) {
			if (*fluxtime <= 0 && styleChanged[i] == true) {
				DevilFluxVFX_1F94D0(actorData, 4);
                // This guarantess FluxEffects color will be gone by the time the effect ends
				styleChanged[i] = false;
			}
		}

        *canStart = true;
    }


    // This guarantees FluxEffect won't 'leak' if it plays in conjunction with DT In/Out
    // but it can also fuck up with the "Sphere Out" vfx when exiting DT,
    // if the condition is fluxtime > 0, DTout plays normally, but 2P's flux will play on 1P
    // if the condition is fluxtime < 0.05f, DTOut doesn't play at all, but each player will have its flux play correctly.
    // fluxtime > 0.095f seems to be the most effective solution, both things work normally.

	// UPDATE: New checks to ensure leaks or fluxes playing on other players occur as least as possible.
	if (activeConfig.Actor.playerCount == 1){
		if (*fluxtime > 0) {

			if (actorData.devil == 0 && !(gamepad.buttons[2] & GetBinding(BINDING::DEVIL_TRIGGER))) {
				DevilFluxVFX_1F94D0(actorData, DEVIL_FLUX::GLOW_OFF);
			}

		}
	}
	else {
		if (actorData.character == CHARACTER::VERGIL && activeCrimsonGameplay.Cheats.Vergil.quicksilverStyle) {
			if (*fluxtime < 0.05f) {

				if (actorData.devil == 0 && !(gamepad.buttons[2] & GetBinding(BINDING::DEVIL_TRIGGER))) {
					DevilFluxVFX_1F94D0(actorData, DEVIL_FLUX::GLOW_OFF);
				}

			}
		}
		else {
			if (*fluxtime > 0.092f) {


				if (actorData.devil == 0 && !(gamepad.buttons[2] & GetBinding(BINDING::DEVIL_TRIGGER))) {
					DevilFluxVFX_1F94D0(actorData, DEVIL_FLUX::GLOW_OFF);
				}

			}
		}
	}
	

}

void StyleSwitchDrawText(byte8* actorBaseAddr) {
    if (!actorBaseAddr) {
        return;
    }
    auto& actorData  = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    auto playerIndex = actorData.newPlayerIndex;
    auto* sstext = &crimsonPlayer[playerIndex].styleSwitchText;
    auto distanceClamped = crimsonPlayer[playerIndex].cameraPlayerDistanceClamped;
	auto& worldSpaceBars = activeCrimsonConfig.MultiplayerBarsWorldSpace.show;
	auto worldSpace1PBar = activeCrimsonConfig.MultiplayerBarsWorldSpace.show1PBar == HUDELEMENTSHOWSTATE::OFF ? false : true;
	bool adjustRoyalWithWorldSpaceBar = playerIndex == 0 ? ((worldSpaceBars && worldSpace1PBar) ? true : false) : worldSpaceBars ?  true : false;
	float royalAdjustment = adjustRoyalWithWorldSpaceBar ? 70.0f : 10.0f;

    // This function draws the Style Switching Text near Dante when switching styles.

    float stylesWorldPos[9][3] = { { actorData.position.x, actorData.position.y + 200.f, actorData.position.z }, // trick
    { actorData.position.x, actorData.position.y + 130.f, actorData.position.z }, // sword
    { actorData.position.x, actorData.position.y + 130.f, actorData.position.z }, // gun
    { actorData.position.x, actorData.position.y - royalAdjustment, actorData.position.z }, // royal
    { actorData.position.x, actorData.position.y - royalAdjustment - 20.0f, actorData.position.z }, // quick
    { actorData.position.x, actorData.position.y + 130.f, actorData.position.z }, // dopp
    { actorData.position.x, actorData.position.y + 130.f, actorData.position.z }, // dt
    { actorData.position.x, actorData.position.y + 130.f, actorData.position.z }, // dte
    { actorData.position.x, actorData.position.y + 200.f, actorData.position.z } }; // ready
    
    // Color conversion from ImGui Color (255, 255, 255, 255) to ddColor (1, 1, 1)    
    for (int style = 0; style < 9; style++) {
        for (int j = 0; j < 3; j++) {
            sstext->color[style][j] = (float)activeCrimsonConfig.StyleSwitchFX.Text.color[style][j] / 255;
        }
    }

    // Adjusts size dynamically based on the distance between Camera and Player
    auto sizeDistance = sstext->animSize * (1.0f / ((float)distanceClamped / 20));

    for (int styleid = 0; styleid < 9; styleid++) {
		if (sstext->time[styleid] > 0) {
            float offset[2] = { 0 };

            // Offsets to the sides of danter
            if (styleid == 1) {
                offset[0] = 200.0f * sizeDistance;
            }
            else if (styleid == 2) {
                offset[0] = -200.0f * sizeDistance;
            }
            else if (styleid == STYLE::DOPPELGANGER) {
                offset[0] = -250.0f * sizeDistance;
            }
			SetStyleSwitchFxWork((SsFxType) styleid, stylesWorldPos[styleid], sstext->color[styleid], sstext->alpha[styleid], offset, sstext->time[styleid], sizeDistance);
		}
    }
    
}

void SetStyleSwitchDrawTextTime(int style, byte8* actorBaseAddr) {
    if (!actorBaseAddr) {
        return;
    }
    auto& actorData  = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    auto playerIndex = actorData.newPlayerIndex;
    auto* sstext = &crimsonPlayer[playerIndex].styleSwitchText;

    float* drawTextTimes[6] = { &sstext->time[1], &sstext->time[2],
        &sstext->time[0], &sstext->time[3],
        &sstext->time[4], &sstext->time[5] };

	float* drawTextAlphas[6] = { &sstext->alpha[1], &sstext->alpha[2],
		&sstext->alpha[0], &sstext->alpha[3],
		&sstext->alpha[4], &sstext->alpha[5] };


    if (actorData.character == CHARACTER::DANTE) {
        for (int i = 0; i < 6; i++) {
            if (i == style) {
                *drawTextTimes[i] = crimsonPlayer[playerIndex].styleSwitchText.duration;
                sstext->animSize = 1.2f * activeCrimsonConfig.StyleSwitchFX.Text.size;
                
            } else {
                *drawTextTimes[i] = 0;
				*drawTextAlphas[i] = activeCrimsonConfig.StyleSwitchFX.Text.maxAlpha;
          
            }
        }
    }
}

#pragma endregion

}
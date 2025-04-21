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

	auto& playerTo1PDistance = crimsonPlayer[playerIndex].playerTo1PDistance;
	auto& cloneTo1PDistance = crimsonPlayer[playerIndex].cloneTo1PDistance;
	auto& playerScreenPosition = crimsonPlayer[playerIndex].playerScreenPosition;
	auto& cloneScreenPosition = crimsonPlayer[playerIndex].cloneScreenPosition;
    auto& cameraPlayerDistance = crimsonPlayer[playerIndex].cameraPlayerDistance;
	auto& cameraCloneDistance = crimsonPlayer[playerIndex].cameraCloneDistance;
	auto& playerOutOfView = crimsonPlayer[playerIndex].playerOutOfView;
	auto& cloneOutOfView = crimsonPlayer[playerIndex].cloneOutOfView;

	playerTo1PDistance = glm::distance(playerPosition, mainActorPosition);
	cloneTo1PDistance = glm::distance(clonePosition, mainActorPosition);
	g_plEntityTo1PDistances[indexToAssign] = playerTo1PDistance;
	g_plEntityTo1PDistances[cloneIndexToAssign] = cloneTo1PDistance;

	playerScreenPosition = debug_draw_world_to_screen((const float*)&actorData.position, 1.0f);
	cloneScreenPosition = debug_draw_world_to_screen((const float*)&cloneActorData.position, 1.0f);

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

	// Calculate the angle
	glm::vec3 direction = glm::normalize(playerPosition - cameraPosition);
	float angle = glm::degrees(glm::atan(direction.y, direction.x)); // Angle in degrees

	glm::vec3 directionClone = glm::normalize(clonePosition - cameraPosition);
	float angleClone = glm::degrees(glm::atan(directionClone.y, directionClone.x)); 

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


    // SET RELEASE VOLUME MULTIPLIER
    if (actorData.dtExplosionCharge > 3000) {
        releaseVolumeMult = actorData.dtExplosionCharge / 10000;
    }
    else if (actorData.dtExplosionCharge > 200 &&  actorData.dtExplosionCharge < 3000) {
        releaseVolumeMult = 0;
    }
    
    // SFX START
	if (actorData.dtExplosionCharge > 500 && !sfxStarted) {
		CrimsonSDL::PlayDTExplosionStart(playerIndex, 50);

        sfxStarted = true;
	}

    // SFX LOOP
    if (!CrimsonSDL::DTEStartIsPlaying(playerIndex) && sfxStarted && !sfxLooped) {
		CrimsonSDL::PlayDTExplosionLoop(playerIndex, 50);

        sfxLooped = true;
    }

    // SFX FINISH
	if (actorData.dtExplosionCharge >= maxDT) {
		CrimsonSDL::InterruptDTExplosionSFX(playerIndex);
		if (!sfxFinished) {
			CrimsonSDL::PlayDTExplosionFinish(playerIndex, 150);
		}
        
		sfxFinished = true;
	}

	// We handle SFX Loop Pause in CrimsonOnTick::PauseSFXWhenPaused().

	uint8 vfxColorDante[4] = { 48, 0, 10, 255 };
	uint8 vfxColorVergil[4] = { 2, 16, 43, 255 };
	uint32 actualColor = (actorData.character == CHARACTER::DANTE) ? CrimsonUtil::Uint8toAABBGGRR(vfxColorDante) :
		CrimsonUtil::Uint8toAABBGGRR(vfxColorVergil);
	// VFX START
	if (actorData.dtExplosionCharge > 2500 && !vfxStarted && !vfxFinished) {
        crimsonPlayer[playerIndex].dTEVFX.time = 0;
		auto pPlayer = (void*)crimsonPlayer[playerIndex].playerPtr;
		
		CrimsonDetours::CreateEffectDetour(pPlayer, 3, 61, 1, true, actualColor, 0.4f);

		vfxStarted = true;
	}

	// VFX FINISH
	if (actorData.dtExplosionCharge >= maxDT && !vfxFinished) {
		auto pPlayer = (void*)crimsonPlayer[playerIndex].playerPtr;
		CrimsonDetours::CreateEffectDetour(pPlayer, 3, 41, 1, true, actualColor, 1.0f);

		vfxFinished = true;
	}
    
    // RELEASE
    if (!(gamepad.buttons[0] & GetBinding(BINDING::DEVIL_TRIGGER)) && sfxStarted) {
		CrimsonSDL::InterruptDTExplosionSFX(playerIndex);
		CrimsonSDL::PlayDTEExplosionRelease(playerIndex, 200 * releaseVolumeMult);

        if (releaseVolumeMult > 0.4f) {
            CrimsonSDL::VibrateController(actorData.newPlayerIndex, 0, 0x5555 * releaseVolumeMult, 800);
        }
        
        if (releaseVolumeMult > 0.4f) {
			auto pPlayer = (void*)crimsonPlayer[playerIndex].playerPtr;
			CrimsonDetours::CreateEffectDetour(pPlayer, 3, 61, 1,true, actualColor, 1.0f);
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
			if (!royalBlockPlayed[playerIndex]) {
				//std::cout << "royal block played" << std::endl;
				CrimsonSDL::PlayRoyalBlock(playerIndex);
				uint8 vfxColor[4] = { 226, 4, 50, 255 };
				uint32 actualColor = CrimsonUtil::Uint8toAABBGGRR(vfxColor);
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
			// for Royalguard Rebalanced only
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
    auto& motionDataIndex = actorData.motionData[0].index;

	auto playerIndex = actorData.newPlayerIndex;
	auto weapon = GetMeleeWeapon(actorData);

	auto inAttack = (actorData.eventData[0].event == 17);
	auto rebellionCombo1Anim = (actorData.motionData[0].index == 3);
	auto inRebellionCombo1 = (actorData.action == REBELLION_COMBO_1_PART_1 && motionDataIndex == 3 && inAttack);
    auto inRebellionCombo2 = (actorData.action == REBELLION_COMBO_2_PART_2 && motionDataIndex == 6 && inAttack);
	auto inCerberusCombo2 = (actorData.action == CERBERUS_COMBO_1_PART_2 && motionDataIndex == 4 && inAttack);
	auto inAgniCombo1 = (actorData.action == AGNI_RUDRA_COMBO_1_PART_1 && motionDataIndex == 3 && inAttack);
	auto inAgniCombo2 = (actorData.action == AGNI_RUDRA_COMBO_2_PART_2 && motionDataIndex == 8 && inAttack);
	auto inBeoCombo1 = (actorData.action == BEOWULF_COMBO_1_PART_2 && motionDataIndex == 4 && inAttack);
	auto meleeWeapon = actorData.newWeapons[actorData.meleeWeaponIndex];
	auto& delayedComboFX = crimsonPlayer[playerIndex].delayedComboFX;
    auto& actionTimer = crimsonPlayer[playerIndex].actionTimer;

	if (inRebellionCombo1) {
		delayedComboFX.duration = 0.485f;
		delayedComboFX.weaponThatStartedMove = 0;
	}
    else if (inRebellionCombo2) {
		delayedComboFX.duration = 0.85f;
		delayedComboFX.weaponThatStartedMove = 0;
    }
	else if (inCerberusCombo2) {
		delayedComboFX.duration = 0.55f;
		delayedComboFX.weaponThatStartedMove = 1;
	}
	else if (inAgniCombo1) {
		delayedComboFX.duration = 0.53f;
		delayedComboFX.weaponThatStartedMove = 2;
	}
	else if (inAgniCombo2) {
		delayedComboFX.duration = 0.70f;
		delayedComboFX.weaponThatStartedMove = 2;
	}
	else if (inBeoCombo1) {
		delayedComboFX.duration = 0.55f; // Beowulf's time can be very inconsistent due to charge time (the more you charge the less you
		// need to wait between delays)
		delayedComboFX.weaponThatStartedMove = 4;
	}


	if (actorData.character == CHARACTER::DANTE) {
		if (actionTimer >= delayedComboFX.duration &&
			(inRebellionCombo1 || inRebellionCombo2 || inCerberusCombo2 || inAgniCombo1 || inAgniCombo2 || inBeoCombo1) && delayedComboFX.playCount == 0 &&
			weapon == delayedComboFX.weaponThatStartedMove) {

            // SFX
			CrimsonSDL::PlayDelayedCombo1(actorData.newPlayerIndex);

            // VFX
			auto pPlayer = (void*)crimsonPlayer[playerIndex].playerPtr;
			uint8 vfxColor[4] = { 48, 0, 10, 255 };
			uint32 actualColor = CrimsonUtil::Uint8toAABBGGRR(vfxColor);
			CrimsonDetours::CreateEffectDetour(pPlayer, delayedComboFX.bank, delayedComboFX.id, 1, true, actualColor, 1.2f);

            // VIBRATION
			CrimsonSDL::VibrateController(playerIndex, 0, 0x5555, 130);


			delayedComboFX.playCount++;
		}
		else if (actionTimer < 0.485f) {
			delayedComboFX.playCount = 0;
		}

	}
   
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
            DevilFluxVFX(actorData, DEVIL_FLUX::START);
            *canStart = false;
            *canEnd = true;
        }
		

		if (*fluxtime < 0.08f && *canEnd) {
			styleVFXCount--;
			DevilFluxVFX(actorData, 4);
			styleChanged[*style] = false;
			*canEnd = false;
            *fluxtime = 0;
            
		}

	}

    if (*fluxtime <= 0 && !*canStart) {
		for (int i = 0; i < 6; i++) {
			if (*fluxtime <= 0 && styleChanged[i] == true) {
				DevilFluxVFX(actorData, 4);
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
				DevilFluxVFX(actorData, DEVIL_FLUX::GLOW_OFF);
			}

		}
	}
	else {
		if (actorData.character == CHARACTER::VERGIL && activeCrimsonConfig.Cheats.Vergil.quicksilverStyle) {
			if (*fluxtime < 0.05f) {

				if (actorData.devil == 0 && !(gamepad.buttons[2] & GetBinding(BINDING::DEVIL_TRIGGER))) {
					DevilFluxVFX(actorData, DEVIL_FLUX::GLOW_OFF);
				}

			}
		}
		else {
			if (*fluxtime > 0.092f) {


				if (actorData.devil == 0 && !(gamepad.buttons[2] & GetBinding(BINDING::DEVIL_TRIGGER))) {
					DevilFluxVFX(actorData, DEVIL_FLUX::GLOW_OFF);
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
    { actorData.position.x, actorData.position.y + 130.f, actorData.position.z }, // quick
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
            else if (styleid == 4) {
                offset[0] = 250.0f * sizeDistance;
            }
            else if (styleid == 5) {
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
                sstext->animSize = activeCrimsonConfig.StyleSwitchFX.Text.size;
                
            } else {
                *drawTextTimes[i] = 0;
				*drawTextAlphas[i] = activeCrimsonConfig.StyleSwitchFX.Text.maxAlpha;
          
            }
        }
    }
}

#pragma endregion

}
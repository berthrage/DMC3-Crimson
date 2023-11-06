module;
#include <thread>
#include <chrono>
#include "../ThirdParty/glm/glm.hpp"
#include "../ThirdParty/ImGui/imgui.h"

export module Timers;

import Core;

#include "Core/Macros.h" 

import Actor;
import ActorBase;
import ActorRelocations;
import Config;
import Exp;
import Global;
import Graphics;
import Vars;
import ExtraSound;



export void ActionTimersMain() {
	//IntroduceMainActorData
	auto pool_12857 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if
		(
			!pool_12857 ||
			!pool_12857[3]
			) {
		return;
	}


	auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_12857[3]);

	auto pool_10371 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
	if
		(
			!pool_10371 ||
			!pool_10371[8]
			) {
		return;
	}
	auto& eventData = *reinterpret_cast<EventData*>(pool_10371[8]);


	



// 	if (inAttack) {
// 		if (eventData.event != EVENT::PAUSE) {
// 			crimsonPlayer[0].actionTimer += (ImGui::GetIO().DeltaTime * mainActorData.speed) / g_frameRateMultiplier;
// 		}
// 	}
// 	else {
// 		crimsonPlayer[0].actionTimer = 0;
// 	}
// 
// 	// ACTIONS
// 	if (mainActorData.action != crimsonPlayer[0].currentAction) {
// 		crimsonPlayer[0].actionTimer = 0;
// 
// 		crimsonPlayer[0].currentAction = mainActorData.action;
// 	}


	old_for_all(uint8, playerIndex, PLAYER_COUNT) {


		auto& newActorData = GetNewActorData(playerIndex, 0, 0);

		auto actorBaseAddr = newActorData.baseAddr;
		

		if (!actorBaseAddr) {
			continue;
		}
		auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
		auto inAttack = (actorData.eventData[0].event == 17);

		if (inAttack) {
			if (eventData.event != EVENT::PAUSE) {
				crimsonPlayer[playerIndex].actionTimer += (ImGui::GetIO().DeltaTime * *crimsonPlayer[playerIndex].speed) / g_frameRateMultiplier;
			}
		}
		else {
			crimsonPlayer[playerIndex].actionTimer = 0;
		}

		// ACTIONS
		if (actorData.action != crimsonPlayer[playerIndex].currentAction) {
			crimsonPlayer[playerIndex].actionTimer = 0;

			crimsonPlayer[playerIndex].currentAction = actorData.action;
		}


	}
}


export void AnimTimersMain() {
	//IntroduceMainActorData
	auto pool_12857 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if
		(
			!pool_12857 ||
			!pool_12857[3]
			) {
		return;
	}


	auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_12857[3]);

	auto pool_10371 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
	if
		(
			!pool_10371 ||
			!pool_10371[8]
			) {
		return;
	}
	auto& eventData = *reinterpret_cast<EventData*>(pool_10371[8]);

	auto inAttack = (mainActorData.eventData[0].event == 17);

	old_for_all(uint8, playerIndex, PLAYER_COUNT) {
		

		auto& newActorData = GetNewActorData(playerIndex, 0, 0);

		auto actorBaseAddr = newActorData.baseAddr;

		if (!actorBaseAddr) {
			continue;
		}
		auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);

		if (actorData.motionData[0].index != crimsonPlayer[playerIndex].currentAnim) {
			crimsonPlayer[playerIndex].animTimer = 0;

			crimsonPlayer[playerIndex].currentAnim = actorData.motionData[0].index;
		}

		if (eventData.event != EVENT::PAUSE) {
			crimsonPlayer[playerIndex].animTimer += (ImGui::GetIO().DeltaTime * *crimsonPlayer[playerIndex].speed) / g_frameRateMultiplier;
		}

			
	}



	// ANIMATION IDS
	


}

void ActionTimersNonMain(byte8* actorBaseAddr) {

}

export void SprintTimer() {
	

	old_for_all(uint8, playerIndex, PLAYER_COUNT) {
		

		if (crimsonPlayer[playerIndex].sprint.timer > 0 && crimsonPlayer[playerIndex].sprint.runTimer) {
			crimsonPlayer[playerIndex].sprint.timer -= ImGui::GetIO().DeltaTime / g_frameRateMultiplier;
		}



		if (crimsonPlayer[playerIndex].sprint.timer <= 0 && !crimsonPlayer[playerIndex].sprint.canSprint) {
			//sprint.timer = sprint.timeToTrigger;
			crimsonPlayer[playerIndex].sprint.runTimer = false;
			crimsonPlayer[playerIndex].sprint.canSprint = true;
		}
	}

}

export void DriveTimer() {
	using namespace ACTION_DANTE;

	old_for_all(uint8, playerIndex, PLAYER_COUNT) {

		

		if (crimsonPlayer[playerIndex].drive.runTimer) {

			crimsonPlayer[playerIndex].drive.timer += (ImGui::GetIO().DeltaTime * *crimsonPlayer[playerIndex].speed) / g_frameRateMultiplier;
		}
		else {
			crimsonPlayer[playerIndex].drive.timer = 0;
		}
		
	}

}



export void BackToForwardTimers() {
	if (b2F.backCommand) {
		b2F.backBuffer -= ImGui::GetIO().DeltaTime / g_frameRateMultiplier;
	}

	if (b2F.forwardCommand) {
		b2F.forwardBuffer -= ImGui::GetIO().DeltaTime / g_frameRateMultiplier;
	}
}

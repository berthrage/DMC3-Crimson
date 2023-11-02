module;
#include <thread>
#include <chrono>
#include "../ThirdParty/glm/glm.hpp"
#include "../ThirdParty/ImGui/imgui.h"

export module Timers;

import Core;

import Actor;
import ActorBase;
import ActorRelocations;
import Config;
import Exp;
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


	auto inAttack = (mainActorData.eventData[0].event == 17);



	if (inAttack) {
		if (eventData.event != EVENT::PAUSE) {
			crimsonPlayer[0].actionTimer += ImGui::GetIO().DeltaTime * mainActorData.speed;
		}
	}
	else {
		crimsonPlayer[0].actionTimer = 0;
	}

	// ACTIONS
	if (mainActorData.action != crimsonPlayer[0].currentAction) {
		crimsonPlayer[0].actionTimer = 0;

		crimsonPlayer[0].currentAction = mainActorData.action;
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

	// ANIMATION IDS
	if (mainActorData.motionData[0].index != crimsonPlayer[0].currentAnim) {
		crimsonPlayer[0].animTimer = 0;

		crimsonPlayer[0].currentAnim = mainActorData.motionData[0].index;
	}

	if (eventData.event != EVENT::PAUSE) {
		crimsonPlayer[0].animTimer += ImGui::GetIO().DeltaTime * mainActorData.speed;
	}


}

void ActionTimersNonMain(byte8* actorBaseAddr) {

}

export void BackToForwardTimers() {
	if (b2F.backCommand) {
		b2F.backBuffer -= ImGui::GetIO().DeltaTime;
	}

	if (b2F.forwardCommand) {
		b2F.forwardBuffer -= ImGui::GetIO().DeltaTime;
	}
}

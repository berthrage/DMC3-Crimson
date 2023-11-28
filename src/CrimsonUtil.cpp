// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "Core/Core.hpp"
#include "DMC3Input.hpp"
#include "Config.hpp"
#include "Global.hpp"
#include "Vars.hpp"

#include <cstdio>

#include "Core/Macros.h"

void UpdateCrimsonPlayerData() {
	/*if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);*/

	old_for_all(uint8, playerIndex, PLAYER_COUNT) {
		auto& playerData = GetPlayerData(playerIndex);

		//auto& activeNewActorData = GetNewActorData(playerIndex, playerData.activeCharacterIndex, ENTITY::MAIN);

		auto& newActorData = GetNewActorData(playerIndex, playerData.activeCharacterIndex, 0);

		auto actorBaseAddr = newActorData.baseAddr;

		if (!actorBaseAddr) {
			continue;
		}
		auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
		auto& cloneActorData = *reinterpret_cast<PlayerActorData*>(actorData.cloneActorBaseAddr);

		auto& gamepad = GetGamepad(actorData.newPlayerIndex);
		auto tiltDirection = GetRelativeTiltDirection(actorData);
		auto tiltDirectionClone = GetRelativeTiltDirection(cloneActorData);
		auto inAir = (actorData.state & STATE::IN_AIR);
		auto lockOn = (actorData.buttons[0] & GetBinding(BINDING::LOCK_ON));

		crimsonPlayer[playerIndex].playerPtr = (uintptr_t)actorData.baseAddr;
		crimsonPlayer[playerIndex].action = actorData.action;
		crimsonPlayer[playerIndex].lastAction = actorData.lastAction;
		crimsonPlayer[playerIndex].event = actorData.eventData[0].event;
		crimsonPlayer[playerIndex].lastEvent = actorData.eventData[0].lastEvent;
		crimsonPlayer[playerIndex].state = actorData.state;
		crimsonPlayer[playerIndex].lastState = actorData.lastState;
		crimsonPlayer[playerIndex].motion = actorData.motionData[0].index;
		crimsonPlayer[playerIndex].character = actorData.character;
		crimsonPlayer[playerIndex].gamepad = gamepad;
		crimsonPlayer[playerIndex].tiltDirection = tiltDirection;
		crimsonPlayer[playerIndex].lockOn = lockOn;
		crimsonPlayer[playerIndex].speed = actorData.speed;
		crimsonPlayer[playerIndex].horizontalPull = actorData.horizontalPull;

		crimsonPlayer[playerIndex].clonePtr = (uintptr_t)actorData.cloneActorBaseAddr;
		crimsonPlayer[playerIndex].actionClone = cloneActorData.action;
		crimsonPlayer[playerIndex].lastActionClone = cloneActorData.lastAction;
		crimsonPlayer[playerIndex].motionClone = cloneActorData.motionData[0].index;
		crimsonPlayer[playerIndex].eventClone = cloneActorData.eventData[0].event;
		crimsonPlayer[playerIndex].lastEventClone = cloneActorData.eventData[0].lastEvent;
		crimsonPlayer[playerIndex].stateClone = cloneActorData.state;
		crimsonPlayer[playerIndex].lastStateClone = cloneActorData.lastState;
		crimsonPlayer[playerIndex].tiltDirectionClone = tiltDirectionClone;
		crimsonPlayer[playerIndex].speedClone = cloneActorData.speed;
		crimsonPlayer[playerIndex].horizontalPullClone = cloneActorData.horizontalPull;


	}

}

int GetPlayerIndexFromAddr(uintptr_t playerPtr) {
	int playerIndexFound = 10; // if none is found then it'll return 10.

	old_for_all(uint8, playerIndex, PLAYER_COUNT) {
		if (playerPtr == crimsonPlayer[playerIndex].playerPtr || playerPtr == crimsonPlayer[playerIndex].clonePtr) {
			playerIndexFound = playerIndex;
		}


	}

	return playerIndexFound;
}
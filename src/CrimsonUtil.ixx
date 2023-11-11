module;
#include <cstdio>

export module CrimsonUtil;


import Core;


#include "Core/Macros.h"

import Config;
import Global;
import Input;
import Vars;

export void UpdateCrimsonPlayerData() {
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
		crimsonPlayer[playerIndex].motion = actorData.motionData[0].index;
		crimsonPlayer[playerIndex].character = actorData.character;
		crimsonPlayer[playerIndex].gamepad = gamepad;
		crimsonPlayer[playerIndex].tiltDirection = tiltDirection;
		crimsonPlayer[playerIndex].lockOn = lockOn;
		crimsonPlayer[playerIndex].speed = actorData.speed;

		crimsonPlayer[playerIndex].clonePtr = (uintptr_t)actorData.cloneActorBaseAddr;
		crimsonPlayer[playerIndex].actionClone = cloneActorData.action;
		crimsonPlayer[playerIndex].motionClone = cloneActorData.motionData[0].index;
		crimsonPlayer[playerIndex].tiltDirectionClone = tiltDirectionClone;
		crimsonPlayer[playerIndex].speedClone = cloneActorData.speed;
	}

}

export int GetPlayerIndexFromAddr(uintptr_t playerPtr) {
	int playerIndexFound = 10; // if none is found then it'll return 10.

	old_for_all(uint8, playerIndex, PLAYER_COUNT) {
		if (playerPtr == crimsonPlayer[playerIndex].playerPtr || playerPtr == crimsonPlayer[playerIndex].clonePtr) {
			playerIndexFound = playerIndex;
		}


	}

	return playerIndexFound;
}
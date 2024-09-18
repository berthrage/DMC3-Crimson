#include <thread>
#include <chrono>
#include "../ThirdParty/glm/glm.hpp"
#include "../ThirdParty/ImGui/imgui.h"

// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "Core/Core.hpp"
#include "Graphics.hpp"
#include "Actor.hpp"
#include "ActorBase.hpp"
#include "ActorRelocations.hpp"
#include "Config.hpp"
#include "Exp.hpp"
#include "Global.hpp"
#include "Vars.hpp"

#include "Core/Macros.h"
#include "Sound.hpp"
#include "CrimsonSDL.hpp"
#include "CrimsonPatches.hpp"
#include "Camera.hpp"
#include "CrimsonDetours.hpp"
#include "CrimsonUtil.hpp"

namespace CrimsonOnTick {

void GameTrackDetection() {
	g_gameTrackPlaying = (std::string)reinterpret_cast<char*>(appBaseAddr + 0xD23906);
}

void CorrectFrameRateCutscenes() {
	// Changing frame rate to above or below 60 will alter cutscene speed, this function corrects this behavior
	// by forcing cutscenes to play at 60 fps. - Mia

	static bool changedFrameRateCorrection = false;
	float temp = queuedConfig.frameRate;

	if (g_scene == SCENE::CUTSCENE && !changedFrameRateCorrection) {
		activeConfig.frameRate = 60.0;

		UpdateFrameRate();
		changedFrameRateCorrection = true;
	}

	if (g_scene != SCENE::CUTSCENE && changedFrameRateCorrection) {
		activeConfig.frameRate = temp;

		UpdateFrameRate();
		changedFrameRateCorrection = false;
	}
}

void PreparePlayersDataBeforeSpawn() {
	auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);
	auto pool_10371 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
	if (!pool_10371 || !pool_10371[8]) {
		return;
	}
	auto& eventData = *reinterpret_cast<EventData*>(pool_10371[8]);

	if (g_scene != SCENE::GAME || (g_scene == SCENE::GAME && eventData.event == EVENT::DEATH)) {
		for (int playerIndex = 0; playerIndex < PLAYER_COUNT; ++playerIndex) {

			crimsonPlayer[playerIndex].hitPoints = sessionData.hitPoints;
			crimsonPlayer[playerIndex].maxHitPoints = sessionData.hitPoints;
			crimsonPlayer[playerIndex].style = sessionData.style;
			crimsonPlayer[playerIndex].magicPoints = sessionData.magicPoints;
			crimsonPlayer[playerIndex].maxMagicPoints = sessionData.magicPoints;
			crimsonPlayer[playerIndex].vergilDoppelganger.miragePoints = 2000;
			crimsonPlayer[playerIndex].vergilDoppelganger.maxMiragePoints = sessionData.magicPoints;
			crimsonPlayer[playerIndex].royalguardReleaseDamage = 0;
		}
	}

	// Reset all StyleData between missions
	if (g_scene != SCENE::GAME) {
		for (int playerIndex = 0; playerIndex < PLAYER_COUNT; ++playerIndex) {
			crimsonPlayer[playerIndex].styleData.rank = 0;
			crimsonPlayer[playerIndex].styleData.quotient = 0;
			crimsonPlayer[playerIndex].styleData.dividend = 0;
			crimsonPlayer[playerIndex].styleData.divisor = 0;

		}
	}
}

void NewMissionClearSong() {
	if (g_scene == SCENE::MISSION_RESULT && !missionClearSongPlayed) {
		// Mute Music Channel Volume
		SetVolume(9, 0);

		// Play song
		CrimsonSDL::PlayNewMissionClearSong();
		missionClearSongPlayed = true;
	}
	else if (g_scene != SCENE::MISSION_RESULT && missionClearSongPlayed) {
		// Fade it out
		CrimsonSDL::FadeOutNewMissionClearSong();

		// Restore original Channnel Volume
		SetVolume(9, activeConfig.channelVolumes[9]);

		missionClearSongPlayed = false;
	}
}

void DisableBlendingEffectsController() {
	// Disables PS2 Motion Blur among other PostProcessFX.

	auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);
	
	auto pool_10371 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
	if (!pool_10371 || !pool_10371[8]) {
		return;
	}
	auto& eventData = *reinterpret_cast<EventData*>(pool_10371[8]);
	if (g_scene != SCENE::GAME) {
		return;
	}
	
	if (activeConfig.disableBlendingEffects) {
		if (eventData.event == EVENT::MAIN || eventData.event == EVENT::PAUSE) {
			CrimsonPatches::DisableBlendingEffects(true);
		}
		else {
			CrimsonPatches::DisableBlendingEffects(false);
		}

	}
	else {
		CrimsonPatches::DisableBlendingEffects(false);
	}
}

void StyleMeterMultiplayer() {
	// Adjusts the Style Meter to take all players into account in MP.

	auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);
	auto pool_11962 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
	if (!pool_11962 || !pool_11962[8]) return;
	auto& eventData = *reinterpret_cast<EventData*>(pool_11962[8]);
	
	auto pool_10222 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_10222 || !pool_10222[3]) return;
	auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_10222[3]);

	if (g_scene != SCENE::GAME) {
		return;
	}

	float highestStyleRank = mainActorData.styleData.rank;
	float highestMeter = mainActorData.styleData.meter;

	for (uint8 playerIndex = 0; playerIndex < activeConfig.Actor.playerCount; ++playerIndex) {
		auto& playerData = GetPlayerData(playerIndex);
		auto& characterData = GetCharacterData(playerIndex, playerData.characterIndex, ENTITY::MAIN);
		auto& newActorData = GetNewActorData(playerIndex, playerData.characterIndex, ENTITY::MAIN);

		if (!newActorData.baseAddr) {
			return;
		}
		auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);
		auto& cloneActorData = *reinterpret_cast<PlayerActorData*>(actorData.cloneActorBaseAddr);

		if (actorData.styleData.rank > highestStyleRank) {
			highestStyleRank = actorData.styleData.rank;
		}
		if (actorData.styleData.meter > highestMeter) {
			highestMeter = actorData.styleData.meter;
		}
	}
	if (highestStyleRank > mainActorData.styleData.rank) {
		mainActorData.styleData.rank = highestStyleRank;
	}
	mainActorData.styleData.rank = highestStyleRank;
	if (highestMeter > mainActorData.styleData.meter) {
		mainActorData.styleData.meter = highestMeter;
	}
}

void DetermineActiveEntitiesCount() {
	auto pool_2128 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_2128 || !pool_2128[8]) return;
	auto& enemyVectorData = *reinterpret_cast<EnemyVectorData*>(pool_2128[8]);

	g_activePlayableEntitiesCount = activeConfig.Actor.playerCount + g_activeClonesCount;
	g_activeAllEntitiesCount = g_activePlayableEntitiesCount + enemyVectorData.count;
}


glm::vec3 currentCameraPos; // Stores current camera position for gradual transition

void MultiplayerCameraPositioningController() {
	auto pool_10222 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_10222 || !pool_10222[3]) return;
	auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_10222[3]);

	auto pool_2128 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_2128 || !pool_2128[8]) return;
	auto& enemyVectorData = *reinterpret_cast<EnemyVectorData*>(pool_2128[8]);

	g_customCameraPos[0] = 0.0f; // X
	g_customCameraPos[1] = 0.0f; // Y
	g_customCameraPos[2] = 0.0f; // Z
	g_customCameraPos[3] = 1.0f; // W

	for (uint8 playerIndex = 0; playerIndex < activeConfig.Actor.playerCount; ++playerIndex) {
		auto& playerData = GetPlayerData(playerIndex);
		auto& characterData = GetCharacterData(playerIndex, playerData.characterIndex, ENTITY::MAIN);
		auto& newActorData = GetNewActorData(playerIndex, playerData.characterIndex, ENTITY::MAIN);

		if (!newActorData.baseAddr) {
			return;
		}
		auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);
		auto& cloneActorData = *reinterpret_cast<PlayerActorData*>(actorData.cloneActorBaseAddr);

		g_customCameraPos[0] += actorData.position.x;
		g_customCameraPos[1] += actorData.position.y;
		g_customCameraPos[2] += actorData.position.z;

		if (actorData.doppelganger == 1) {
			g_customCameraPos[0] += cloneActorData.position.x;
			g_customCameraPos[1] += cloneActorData.position.y;
			g_customCameraPos[2] += cloneActorData.position.z;
		}
	}
	
	for (auto enemy : enemyVectorData.metadata) {
		if (!enemy.baseAddr) continue;
		auto& enemyData = *reinterpret_cast<EnemyActorData*>(enemy.baseAddr);
		if (!enemyData.baseAddr) continue;

		g_customCameraPos[0] += enemyData.position.x;
		g_customCameraPos[1] += enemyData.position.y;
		g_customCameraPos[2] += enemyData.position.z;

	}

	float minDistance = 100.0f; // Minimum distance between player and clone for MP cam to trigger in Single Player
	auto& cloneMainActorData = *reinterpret_cast<PlayerActorData*>(mainActorData.cloneActorBaseAddr);
	glm::vec3 playerPos;
	glm::vec3 clonePos;

	playerPos.x = mainActorData.position.x;
	playerPos.y = mainActorData.position.y;
	playerPos.z = mainActorData.position.z;

	clonePos.x = cloneMainActorData.position.x;
	clonePos.y = cloneMainActorData.position.y;
	clonePos.z = cloneMainActorData.position.z;

	bool triggerMPCam = activeCrimsonConfig.Camera.multiplayerCamera? true : false;
	for (int i = 0; i < activeConfig.Actor.playerCount * 2; i++) {
		float distanceTo1P = g_plEntityTo1PDistances[i];

		if (distanceTo1P >= 1800.0f) {
			triggerMPCam = false;
		}
	}

	bool triggerParanoramicCam = activeCrimsonConfig.Camera.panoramicCamera ? true : false;
	for (auto enemy : enemyVectorData.metadata) {
		if (!enemy.baseAddr) continue;
		auto& enemyData = *reinterpret_cast<EnemyActorData*>(enemy.baseAddr);
		if (!enemyData.baseAddr) continue;

		glm::vec3 enemyPos;
		enemyPos.x = enemyData.position.x;
		enemyPos.y = enemyData.position.y;
		enemyPos.z = enemyData.position.z;

		float distanceto1P = glm::distance(enemyPos, playerPos);
		if (distanceto1P >= 1000.0f) {
			triggerParanoramicCam = false;
		}

	}

	// Camera behavior based on player count and trigger status
	if (activeConfig.Actor.playerCount > 1 || mainActorData.doppelganger == 1) {
		// MULTIPLAYER

		if (triggerMPCam) {
			// MPCam mode: calculate average camera position
			g_customCameraPos[0] /= g_activeAllEntitiesCount;
			g_customCameraPos[1] /= g_activeAllEntitiesCount;
			g_customCameraPos[2] /= g_activeAllEntitiesCount;

			// If switching from normal cam to MPCam, initialize lerp transition
			if (!g_isMPCamActive) {
				g_isMPCamActive = true;
				currentCameraPos = glm::vec3(mainActorData.position.x, mainActorData.position.y, mainActorData.position.z);
			}

		}
		else {
			// Normal cam mode: focus on main actor position
			g_customCameraPos[0] = mainActorData.position.x;
			g_customCameraPos[1] = mainActorData.position.y;
			g_customCameraPos[2] = mainActorData.position.z;

			// If switching from MPCam to normal cam, initialize lerp transition
			if (g_isMPCamActive) {
				g_isMPCamActive = false;
				currentCameraPos = glm::vec3(g_customCameraPos[0], g_customCameraPos[1], g_customCameraPos[2]);
			}
		}

		// Gradual transition between MPCam and normal cam (if a transition is occurring)
		if (g_isMPCamActive) {
			float lerpFactor = 0.05f;  // Adjust this factor for smoother or faster transitions
			currentCameraPos.x = lerp(currentCameraPos.x, g_customCameraPos[0], lerpFactor);
			currentCameraPos.y = lerp(currentCameraPos.y, g_customCameraPos[1], lerpFactor);
			currentCameraPos.z = lerp(currentCameraPos.z, g_customCameraPos[2], lerpFactor);

			g_customCameraPos[0] = currentCameraPos.x;
			g_customCameraPos[1] = currentCameraPos.y;
			g_customCameraPos[2] = currentCameraPos.z;
		}
		else {
			g_customCameraPos[0] = mainActorData.position.x;
			g_customCameraPos[1] = mainActorData.position.y;
			g_customCameraPos[2] = mainActorData.position.z;
		}
	}
	else {
		// SINGLE PLAYER
		// Only average out camera position between clone and player if their distance
		// exceeds minDistance (to prevent bugging out when clone is spawning)
		float distanceBetweenClone = glm::distance(playerPos, clonePos);

		if (triggerParanoramicCam && g_inCombat) {
			// Panoramic Camera mode: calculate average camera position
			g_customCameraPos[0] /= g_activeAllEntitiesCount;
			g_customCameraPos[1] /= g_activeAllEntitiesCount;
			g_customCameraPos[2] /= g_activeAllEntitiesCount;

			// If switching from normal cam to PanoramicCam, initialize lerp transition
			if (!g_isParanoramicCamActive) {
				g_isParanoramicCamActive = true;
				currentCameraPos = glm::vec3(mainActorData.position.x, mainActorData.position.y, mainActorData.position.z);
			}
		}
		else {
			// Normal cam mode: focus on main actor position
			g_customCameraPos[0] = mainActorData.position.x;
			g_customCameraPos[1] = mainActorData.position.y;
			g_customCameraPos[2] = mainActorData.position.z;

			if (g_isParanoramicCamActive) {
				g_isParanoramicCamActive = false;
				currentCameraPos = glm::vec3(g_customCameraPos[0], g_customCameraPos[1], g_customCameraPos[2]);
			}
		}

		if (mainActorData.doppelganger == 0) g_isMPCamActive = false;

		// Gradual transition between MPCam and normal cam (if a transition is occurring)
		if (g_isParanoramicCamActive) {
			float lerpFactor = 0.05f;  // Adjust this factor for smoother or faster transitions
			currentCameraPos.x = lerp(currentCameraPos.x, g_customCameraPos[0], lerpFactor);
			currentCameraPos.y = lerp(currentCameraPos.y, g_customCameraPos[1], lerpFactor);
			currentCameraPos.z = lerp(currentCameraPos.z, g_customCameraPos[2], lerpFactor);

			g_customCameraPos[0] = currentCameraPos.x;
			g_customCameraPos[1] = currentCameraPos.y;
			g_customCameraPos[2] = currentCameraPos.z;
		}
		else {
			g_customCameraPos[0] = mainActorData.position.x;
			g_customCameraPos[1] = mainActorData.position.y;
			g_customCameraPos[2] = mainActorData.position.z;
		}
		
	}
	
	// Activate multiplayer camera positioning
	CrimsonDetours::ToggleCustomCameraPositioning(activeCrimsonConfig.Camera.multiplayerCamera || activeCrimsonConfig.Camera.panoramicCamera);
}


void ForceThirdPersonCameraController() {
	auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);
	auto pool_11962 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
	if (!pool_11962 || !pool_11962[8]) {
		return;
	}
	auto& eventData = *reinterpret_cast<EventData*>(pool_11962[8]);
	if (g_scene != SCENE::GAME) {
		return;
	}

	if (activeCrimsonConfig.Camera.forceThirdPerson) {
		CrimsonPatches::ForceThirdPersonCamera(true);
		if (!(eventData.room == 228 && eventData.position == 0)) { // Adding only Geryon Part 1 as an exception for now.
			Camera::ToggleDisableBossCamera(true);
		}
		else {
			Camera::ToggleDisableBossCamera(false);
		}
		
	}
	else {
		CrimsonPatches::ForceThirdPersonCamera(false);
		Camera::ToggleDisableBossCamera(activeCrimsonConfig.Camera.disableBossCamera);
	}
}

CameraData* GetSafeCameraData() {
	auto pool_4449 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC8FBD0);
	if (!pool_4449 || !pool_4449[147]) {
		return nullptr;
	}

	auto cameraDataPtr = reinterpret_cast<CameraData*>(pool_4449[147]);

	// Check for known invalid pointers
	if (!cameraDataPtr || reinterpret_cast<uintptr_t>(cameraDataPtr) & 0xFFF0000000000000) {
		return nullptr;
	}

	return cameraDataPtr;
}


void GeneralCameraOptionsController() {
	auto pool_10298 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
	if (!pool_10298 || !pool_10298[8]) {
		return;
	}
	auto& eventData = *reinterpret_cast<EventData*>(pool_10298[8]);
	CameraData* cameraData = GetSafeCameraData();
	if (!cameraData) {
		return;
	}

	auto pool_10222 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_10222 || !pool_10222[3]) {
		return;
	}
	auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_10222[3]);
	if (eventData.event != EVENT::MAIN) {
		return;
	}

	CrimsonPatches::CameraSensController();
	CrimsonPatches::CameraFollowUpSpeedController();
	CrimsonPatches::CameraDistanceController();
	CrimsonPatches::CameraTiltController();
	CrimsonPatches::LockedOffCameraToggle(activeCrimsonConfig.Camera.lockedOff);
	CrimsonPatches::CameraLockOnDistanceController();
}

void AirTauntDetoursController() {
	if (activeConfig.Actor.enable) {
		CrimsonDetours::AirTauntDetours(true);
	}
	else {
		CrimsonDetours::AirTauntDetours(false);
	}
}

}
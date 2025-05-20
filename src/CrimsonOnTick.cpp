#include <thread>
#include <chrono>
#include <algorithm>
#include "CrimsonEnemyAITarget.hpp"
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
#include "Speed.hpp"

#include "Core/Macros.h"
#include "Sound.hpp"
#include "CrimsonSDL.hpp"
#include "CrimsonPatches.hpp"
#include "Camera.hpp"
#include "CrimsonDetours.hpp"
#include "CrimsonUtil.hpp"
#include "CrimsonTimers.hpp"
#include "DMC3Input.hpp"


namespace CrimsonOnTick {

bool inputtingFPS = false;

void FrameResponsiveGameSpeed() {
	// Calculate Delta Time Manually
	static double lastTime = ImGui::GetTime();
	double currentTime = ImGui::GetTime();
	float deltaTime = static_cast<float>(currentTime - lastTime);
	lastTime = currentTime;

	// Compute frame rate and multiplier
	g_FrameRate = 1.0f / deltaTime;
	g_FrameRateTimeMultiplier = 60.0f / g_FrameRate;

	// Ignore deltaTime spikes that result from alt-tabbing, loading screens, etc.
	float freezeThreshold = 1.0f / 50.0f; // Skips <50 FPS frames
	if (deltaTime > freezeThreshold) {
		return;
	}

	const float gameSpeedBase = g_scene != SCENE::CUTSCENE ? IsTurbo() ? 1.2f : 1.0f : 1.0f;
	auto& activeValue = IsTurbo() ? activeConfig.Speed.turbo : activeConfig.Speed.mainSpeed;
	auto& queuedValue = IsTurbo() ? queuedConfig.Speed.turbo : queuedConfig.Speed.mainSpeed;


	if (activeConfig.framerateResponsiveGameSpeed) {
		// Cutscene audio is so timing sensitive that we can't truly sync the FPS to the game speed while in them.
		const float adjustedSpeed = g_scene != SCENE::CUTSCENE ? gameSpeedBase * g_FrameRateTimeMultiplier : gameSpeedBase * g_frameRateMultiplier;
		if (g_scene == SCENE::CUTSCENE) Speed::Toggle(true);

		activeConfig.Speed.turbo = adjustedSpeed;
		activeConfig.Speed.mainSpeed = adjustedSpeed;
		queuedConfig.Speed.turbo = adjustedSpeed;
		queuedConfig.Speed.mainSpeed = adjustedSpeed;

		UpdateFrameRate(); 

		// === Throttled Speed::Toggle(true) ===
		static double lastToggleTime = 0.0;
		constexpr double toggleInterval = 0.25; // seconds 

		if (currentTime - lastToggleTime >= toggleInterval) {
			Speed::Toggle(true);
			lastToggleTime = currentTime;
		}

		// === One-time enable/disable logic ===
		static bool speedWasEnabled = false;
		if (g_scene == SCENE::GAME && !speedWasEnabled) {
			Speed::Toggle(true);
			speedWasEnabled = true;
		} else if (g_inGameCutscene && speedWasEnabled) {
			speedWasEnabled = false;
		}
	} 
}

void GameTrackDetection() {
	g_gameTrackPlaying = (std::string)reinterpret_cast<char*>(appBaseAddr + 0xD23906);
}

void PreparePlayersDataBeforeSpawn() {
	auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);
	auto pool_10371 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
	if (!pool_10371 || !pool_10371[8]) {
		return;
	}
	auto& eventData = *reinterpret_cast<EventData*>(pool_10371[8]);

	auto missionDataPtr = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E30);
	if (!missionDataPtr) {
		return;
	}
	auto& missionData = *reinterpret_cast<MissionData*>(missionDataPtr);
	auto& queuedMissionActorData = *reinterpret_cast<QueuedMissionActorData*>(missionDataPtr + 0xC0);
	auto& activeMissionActorData = *reinterpret_cast<ActiveMissionActorData*>(missionDataPtr + 0x16C);

	//if we're in game
	if (g_scene == SCENE::GAME) {
		//see if we can grab chracter1 for actor1
		auto pool_10222 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
		if (!pool_10222 || !pool_10222[3]) {}
		else {

			if (!g_playerActorBaseAddrs[0]) {
				return;
			}
			//get the default character.
			auto& vanillaActorData = *reinterpret_cast<PlayerActorData*>(g_playerActorBaseAddrs[0]);
			//Log(vanillaActorData.maxHitPoints);
			//get the one actually used in game
			auto& actorData = *reinterpret_cast<PlayerActorData*>(pool_10222[3]);
			//if the actor's one exceeds the default, we picked up a blorb.
			//therefore, we update the default, along with active & queued mission data.
			//Don't write to session, that'll save when it shouldn't.
			if (actorData.maxHitPoints > vanillaActorData.maxHitPoints){
				vanillaActorData.maxHitPoints = actorData.maxHitPoints;
				//vanillaActorData.hitPoints = actorData.hitPoints;
				//not sure if these ones are necessary. 
				activeMissionActorData.maxHitPoints = actorData.maxHitPoints;
				queuedMissionActorData.hitPoints = actorData.maxHitPoints;
				//basically does the crimsonPlayer update when this orb collection happens
				for (int playerIndex = 0; playerIndex < PLAYER_COUNT; ++playerIndex) {
					crimsonPlayer[playerIndex].hitPoints = activeMissionActorData.maxHitPoints;
					crimsonPlayer[playerIndex].maxHitPoints = activeMissionActorData.maxHitPoints;
				}
			}
		}
	}

	if (g_scene != SCENE::GAME || (g_scene == SCENE::GAME && eventData.event == EVENT::DEATH)) {
		for (int playerIndex = 0; playerIndex < PLAYER_COUNT; ++playerIndex) {
			//write from active missiondata here instead of session so that we can use purchased but unsaved blorbs
			crimsonPlayer[playerIndex].hitPoints = activeMissionActorData.hitPoints;
			crimsonPlayer[playerIndex].maxHitPoints = activeMissionActorData.maxHitPoints;
			//stop breaking the style shop you bastards
			//by skipping the style override on secret mission start, we prevent dante's current style level from becoming desynced with its original style.
			if (g_secretMission == 0)
				crimsonPlayer[playerIndex].style = sessionData.style;
			//write from active missiondata here instead of session so that we can use purchased but unsaved porbs
			crimsonPlayer[playerIndex].magicPoints = activeMissionActorData.magicPoints;
			crimsonPlayer[playerIndex].maxMagicPoints = activeMissionActorData.maxMagicPoints;
			crimsonPlayer[playerIndex].vergilDoppelganger.miragePoints = 2000;
			//max mirage points should now use latest purchased porbs
			crimsonPlayer[playerIndex].vergilDoppelganger.maxMiragePoints = activeMissionActorData.magicPoints;
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

void CrimsonMissionClearSong() {
	if (g_scene == SCENE::MISSION_RESULT && !missionClearSongPlayed 
		&& (gameModeData.missionResultGameMode == GAMEMODEPRESETS::CRIMSON 
		|| gameModeData.missionResultGameMode == GAMEMODEPRESETS::CUSTOM)) {
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
		SetVolume(9, activeCrimsonConfig.Sound.channelVolumes[9] / 100.0f);

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

	auto pool_11962 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
	if (!pool_11962 || !pool_11962[8]) return;
	auto& eventData = *reinterpret_cast<EventData*>(pool_11962[8]);

	g_customCameraPos[0] = 0.0f; // X
	g_customCameraPos[1] = 0.0f; // Y
	g_customCameraPos[2] = 0.0f; // Z
	g_customCameraPos[3] = 1.0f; // W

	const float lerpFactorOutTransition = 0.2f; 
	const float lerpFactorInTransition = 0.01f;
	static float lerpFactor = lerpFactorOutTransition;
	static std::chrono::time_point<std::chrono::steady_clock> transitionToMPStartTime;
	static bool isTransitionTimerActive = false;
	bool triggerMPCam = activeCrimsonConfig.Camera.multiplayerCamera? true : false;

	int entityCount = 0; // Track valid entities for averaging
	float playerWeight = 5.0f;  // Weight for playable characters
	float enemyWeight = 1.0f;   // Weight for enemies
	float totalWeight = 0.0f;
	int alivePlayerCount = 0; // Track number of players still alive

	// Loop through player data
	for (uint8 playerIndex = 0; playerIndex < activeConfig.Actor.playerCount; ++playerIndex) {
		auto& playerData = GetPlayerData(playerIndex);
		auto& characterData = GetCharacterData(playerIndex, playerData.characterIndex, ENTITY::MAIN);
		auto& newActorData = GetNewActorData(playerIndex, playerData.characterIndex, ENTITY::MAIN);

		if (!newActorData.baseAddr) {
			return;
		}
		auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);
		auto& cloneActorData = *reinterpret_cast<PlayerActorData*>(actorData.cloneActorBaseAddr);
		if (!actorData.dead) {
			alivePlayerCount++;
		}
		// Apply player weight to their position
		g_customCameraPos[0] += actorData.position.x * playerWeight;
		g_customCameraPos[1] += actorData.position.y * playerWeight;
		g_customCameraPos[2] += actorData.position.z * playerWeight;
		totalWeight += playerWeight;
		entityCount++;

		// Include the clone if it exists
		if (actorData.doppelganger == 1) {
			g_customCameraPos[0] += cloneActorData.position.x * playerWeight;
			g_customCameraPos[1] += cloneActorData.position.y * playerWeight;
			g_customCameraPos[2] += cloneActorData.position.z * playerWeight;
			totalWeight += playerWeight;
			entityCount++;
		}
	}

	// Turn off multiplayer camera when active player count is > 1 and only one player is alive
	if (alivePlayerCount<=1 && activeConfig.Actor.playerCount > 1) {
		triggerMPCam = false;
	}

	// Loop through enemy data
	for (auto enemy : enemyVectorData.metadata) {
		if (!enemy.baseAddr) continue;
		auto& enemyData = *reinterpret_cast<EnemyActorData*>(enemy.baseAddr);
		if (!enemyData.baseAddr) continue;

		// Find the closest player to the enemy
		bool isWithinRange = false;
		for (uint8 playerIndex = 0; playerIndex < activeConfig.Actor.playerCount; ++playerIndex) {
			auto& playerData = GetPlayerData(playerIndex);
			auto& characterData = GetCharacterData(playerIndex, playerData.characterIndex, ENTITY::MAIN);
			auto& newActorData = GetNewActorData(playerIndex, playerData.characterIndex, ENTITY::MAIN);

			if (!newActorData.baseAddr) continue;

			auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);
			glm::vec3 playerPos(actorData.position.x, actorData.position.y, actorData.position.z);
			glm::vec3 enemyPos(enemyData.position.x, enemyData.position.y, enemyData.position.z);

			float distanceToPlayer = glm::distance(playerPos, enemyPos);

			if (distanceToPlayer <= 800.0f) {
				isWithinRange = true;
				break;
			}
		}

		// Only count enemy if it's within range of a player
		if (isWithinRange) {
			g_customCameraPos[0] += enemyData.position.x * enemyWeight;
			g_customCameraPos[1] += enemyData.position.y * enemyWeight;
			g_customCameraPos[2] += enemyData.position.z * enemyWeight;
			totalWeight += enemyWeight;
			entityCount++;
		}
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

	float cameraDistanceMP = (eventData.room >= ROOM::BLOODY_PALACE_1 && eventData.room <= ROOM::BLOODY_PALACE_10) || eventData.room? 2800.0f : 1900.0f;

	for (int i = 0; i < activeConfig.Actor.playerCount * 2; i++) {
		float distanceTo1P = g_plEntityTo1PDistances[i];

		if (distanceTo1P >= 2800.0) {
			triggerMPCam = false;
		}
	}

	bool triggerPanoramicCam = activeCrimsonConfig.Camera.panoramicCamera ? true : false;
	bool allEnemiesFarAway = true;

	for (std::size_t i = 0; i < enemyVectorData.count; ++i) {
		auto& enemy = enemyVectorData.metadata[i];
		if (!enemy.baseAddr) continue;
		auto& enemyData = *reinterpret_cast<EnemyActorData*>(enemy.baseAddr);
		if (!enemyData.baseAddr) continue;

		glm::vec3 enemyPos;
		enemyPos.x = enemyData.position.x;
		enemyPos.y = enemyData.position.y;
		enemyPos.z = enemyData.position.z;

		float distanceto1P = glm::distance(enemyPos, playerPos);

		g_enemiesTo1PDistances[i] = distanceto1P;

		// If any enemy is within 1000 units, set the flag to false
		if (distanceto1P < 1000.0f) {
			allEnemiesFarAway = false;
			break; // We can exit early since we know not all enemies are far away
		}
	}


	// Only set triggerPanoramicCam to false if all enemies are far enough
	triggerPanoramicCam = !allEnemiesFarAway && activeCrimsonConfig.Camera.panoramicCamera;

	// Camera behavior based on player count and trigger status
	if (activeConfig.Actor.playerCount > 1 || mainActorData.doppelganger == 1) {
		// MULTIPLAYER

		if (triggerMPCam) {
			// MPCam mode: calculate average camera position
			g_customCameraPos[0] /= totalWeight;
			g_customCameraPos[1] /= totalWeight;
			g_customCameraPos[2] /= totalWeight;

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

		g_isParanoramicCamActive = false;
		glm::vec3 currentCustomCamPos = { g_customCameraPos[0], g_customCameraPos[1], g_customCameraPos[2] };

		// Gradual transition between MPCam and normal cam (if a transition is occurring)
		if (g_isMPCamActive) {

			if (!isTransitionTimerActive) {
				transitionToMPStartTime = std::chrono::steady_clock::now();
				isTransitionTimerActive = true;
			}
			
			// Calculate elapsed time
			auto now = std::chrono::steady_clock::now();
			auto elapsedTime = std::chrono::duration<float>(now - transitionToMPStartTime).count();

			if (elapsedTime < 0.5f) {
				if (std::fabs(mainActorData.horizontalPull) < 30 && std::fabs(mainActorData.verticalPull) < 30) {
					lerpFactor = lerpFactorInTransition;
				}
				else {
					lerpFactor = 0.3f;
				}
				
			}
			else {
				lerpFactor = lerpFactorOutTransition;
			}

			currentCameraPos.x = CrimsonUtil::lerp(currentCameraPos.x, g_customCameraPos[0], lerpFactor);
			currentCameraPos.y = CrimsonUtil::lerp(currentCameraPos.y, g_customCameraPos[1], lerpFactor);
			currentCameraPos.z = CrimsonUtil::lerp(currentCameraPos.z, g_customCameraPos[2], lerpFactor);

			

			float distanceLerp = glm::distance(currentCameraPos, currentCustomCamPos);

			if (!guiPause.canPause) {
				currentCameraPos = currentCustomCamPos; // disable lerp when level isn't fully loaded
			}

			// apply lerp
			g_customCameraPos[0] = currentCameraPos.x;
			g_customCameraPos[1] = currentCameraPos.y;
			g_customCameraPos[2] = currentCameraPos.z;
			
		}
		else {
			currentCameraPos = currentCustomCamPos;
			isTransitionTimerActive = false;
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

		if (triggerPanoramicCam && g_inCombat) {
			// Panoramic Camera mode: calculate average camera position
// 			g_customCameraPos[0] /= totalWeight;
// 			g_customCameraPos[1] /= totalWeight;
// 			g_customCameraPos[2] /= totalWeight;
			g_customCameraPos[0] = mainActorData.position.x;
			g_customCameraPos[1] = mainActorData.position.y;
			g_customCameraPos[2] = mainActorData.position.z;

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

		g_isMPCamActive = false;

		// Gradual transition between MPCam and normal cam (if a transition is occurring)
		if (g_isParanoramicCamActive) {
			float lerpFactor = 0.05f;  // Adjust this factor for smoother or faster transitions
			currentCameraPos.x = CrimsonUtil::lerp(currentCameraPos.x, g_customCameraPos[0], lerpFactor);
			currentCameraPos.y = CrimsonUtil::lerp(currentCameraPos.y, g_customCameraPos[1], lerpFactor);
			currentCameraPos.z = CrimsonUtil::lerp(currentCameraPos.z, g_customCameraPos[2], lerpFactor);
// 
			g_customCameraPos[0] = mainActorData.position.x;
			g_customCameraPos[1] = mainActorData.position.y;
			g_customCameraPos[2] = mainActorData.position.z;

// 			g_customCameraPos[0] = currentCameraPos.x;
// 			g_customCameraPos[1] = currentCameraPos.y;
// 			g_customCameraPos[2] = currentCameraPos.z;
		}
		else {
			g_customCameraPos[0] = mainActorData.position.x;
			g_customCameraPos[1] = mainActorData.position.y;
			g_customCameraPos[2] = mainActorData.position.z;
		}
		
	}
	
	// Disable Lock On Cam when MP Cam is active
	CrimsonPatches::DisableLockOnCamera(g_isMPCamActive);

	// Activate multiplayer camera positioning
	CrimsonDetours::ToggleCustomCameraPositioning(g_isMPCamActive && activeCrimsonConfig.Camera.multiplayerCamera);
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

/// <summary>
/// Does logic for camera exceptions to handle intricacies of room transitions
/// </summary>
/// <param name="room">room the camera should be disabled in </param>
/// <param name="mission">mission the room should have the camera disabled for</param>
/// <param name="position">position you enter the room from that the camera should be disabled for</param>
/// <returns>true/false on tic depending on that camera exception</returns>
bool evaluateRoomCameraException(SessionData& sessionData, EventData& eventData, NextEventData& nextEventData, uint32 room, uint32 mission = 0, uint32 position = 0)
{
	////hackjob optimization 
	if (sessionData.mission != mission)
		return false;
	//true in states for which we are exiting the current room but haven't left yet.
	bool isRoomTransition = (eventData.event == EVENT::TELEPORT || eventData.event == EVENT::DELETE || eventData.event == EVENT::END);
	//basically this check will turn off the camera as we transition to the new room
	bool nextroom = (sessionData.mission == mission && nextEventData.room == room && nextEventData.position == position && isRoomTransition);
	//this will keep the camera off in the room its disabled for.
	bool currentroom = (sessionData.mission == mission && eventData.room == room && eventData.position == position) && !isRoomTransition;
	
	return nextroom || currentroom;
}

void ForceThirdPersonCameraController() {
	auto pool_10298 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
	if (!pool_10298 || !pool_10298[8]) {
		return;
	}
	auto& eventData = *reinterpret_cast<EventData*>(pool_10298[8]);

	if (eventData.event == EVENT::TELEPORT) {
		CrimsonPatches::ForceThirdPersonCamera(true);
	}


	auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

	//get event & nextevent
	auto pool_12959 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
	if (!pool_12959 || !pool_12959[12]) {
		return;
	}
	auto& nextEventData = *reinterpret_cast<NextEventData*>(pool_12959[12]);

	auto pool_10222 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	static bool checkIfGameHasAlreadyLoaded2 = false;
	

	if (!checkIfGameHasAlreadyLoaded2) {
		if (eventData.event == EVENT::MAIN && g_inGameDelayed) {
			checkIfGameHasAlreadyLoaded2 = true;
		}
	} else {
		if (!pool_10222 || !pool_10222[3]) {
			return;
		}
		auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_10222[3]);

		CameraData* cameraData = GetSafeCameraData();
		if (!cameraData) {
			return;
		}
	}

	if (activeCrimsonConfig.Camera.forceThirdPerson) {
		// Room Exceptions for TPS cam
		//reference room, position, mission. Check nextEventData 
		bool roomExceptions = (
			(eventData.room == ROOM::LOST_SOULS_NIRVANA && eventData.event != EVENT::TELEPORT)
			//Vergil approach 
			|| evaluateRoomCameraException(sessionData,eventData,nextEventData,ROOM::PEAK_OF_DARKNESS, 7, 0)
			//m8 exception
			|| evaluateRoomCameraException(sessionData, eventData, nextEventData, ROOM::LEVIATHANS_INTESTINES_2, 8, 0)
			|| evaluateRoomCameraException(sessionData, eventData, nextEventData, ROOM::LEVIATHANS_INTESTINES_5, 8, 0)
			//unfortunately forces boss cam in leviathan heart which we might not want
			//|| evaluateRoomCameraException(sessionData, eventData, nextEventData, ROOM::LEVIATHANS_HEARTCORE, 8, 0)
			// m9 exception
			//laser puzzle
			|| evaluateRoomCameraException(sessionData, eventData, nextEventData, ROOM::SUBTERRANEAN_GARDEN, 9, 0)
			//lake room (camera highlights progression + free cam gets stuck on a wall trying to reach a secret area
			|| evaluateRoomCameraException(sessionData, eventData, nextEventData, ROOM::SUBTERRANEAN_LAKE, 9, 0)
			|| evaluateRoomCameraException(sessionData, eventData, nextEventData, ROOM::SUBTERRANEAN_LAKE, 9, 1)
			|| evaluateRoomCameraException(sessionData, eventData, nextEventData, ROOM::SUBTERRANEAN_LAKE, 9, 2)
			//nevan?
			// || evaluateRoomCameraException(sessionData, eventData, nextEventData, ROOM::SUNKEN_OPERA_HOUSE, 9, 0)
			// m10 exception 
			//this should only happen in position 2 on the other side of the cave where you collect the m10 mask
			|| evaluateRoomCameraException(sessionData, eventData, nextEventData, ROOM::LIMESTONE_CAVERN, 10, 2)
			|| evaluateRoomCameraException(sessionData, eventData, nextEventData, ROOM::SUBTERRANEAN_LAKE, 10, 0)
			|| evaluateRoomCameraException(sessionData, eventData, nextEventData, ROOM::SUBTERRANEAN_LAKE, 10, 1)
			|| evaluateRoomCameraException(sessionData, eventData, nextEventData, ROOM::SUBTERRANEAN_LAKE, 10, 2)
			//
			//||(/* scenario 3*/)
			//||(/* scenario 4*/)
			);
		if (roomExceptions){
			CrimsonPatches::ForceThirdPersonCamera(false);
		} else {
			CrimsonPatches::ForceThirdPersonCamera(true);
			
		}

		// Disable Boss Camera Exceptions
		if (!(eventData.room == 228 && eventData.position == 0)) { // Adding only Geryon Part 1 as an exception for now.
			Camera::ToggleDisableBossCamera(true);
		} else {
			Camera::ToggleDisableBossCamera(false);
		}
	} else {
		CrimsonPatches::ForceThirdPersonCamera(false);
		Camera::ToggleDisableBossCamera(activeCrimsonConfig.Camera.disableBossCamera);
	}
}

void FixInitialCameraRotation(EventData& eventData, PlayerActorData& mainActorData, CameraData* cameraData, bool& setCamPos) {
	if (!setCamPos) {
		if (!g_inGameCutscene) {
			constexpr float TWO_PI = 6.283185307f;
			constexpr float PI = 3.1415926535f;
			float radius = 5.0f;
			float radiusZ = 5.0f;
			float verticalOffset = 140.0f;

			float angle = (mainActorData.rotation / 65535.0f) * TWO_PI;
			angle += PI;

			if (eventData.room != ROOM::HEAVENRISE_CHAMBER && eventData.room != ROOM::HIGH_FLY_ZONE) angle += PI;

			vec3 offset;
			offset.x = -sinf(angle) * radius;
			offset.z = -cosf(angle) * radiusZ;
			offset.y = verticalOffset;

			cameraData->data[0].x = mainActorData.position.x + offset.x;
			cameraData->data[0].y = mainActorData.position.y + offset.y;
			cameraData->data[0].z = mainActorData.position.z + offset.z;
			mainActorData.position.x = mainActorData.position.x + 1;

			setCamPos = true;
		}
	}
}

void VajuraBugFix(CameraData* cameraData, EventData& eventData) {
	static bool wasInCutscene = false;
	static bool restoreCamData1 = false;
	static bool restoreCamData2 = false;
	static vec3 fixCamData1Vec = { 0.0f, 0.0f, 0.0f };
	static vec3 fixCamData2Vec = { 0.0f, 0.0f, 0.0f };

	if (eventData.room == ROOM::LIVING_STATUE_ROOM) {
		if (g_inGameCutscene) {
			if (!wasInCutscene) {
				restoreCamData1 = true;
			}
			wasInCutscene = true;
		} else {
			if (restoreCamData1) {
				cameraData->data[1].x = fixCamData1Vec.x;
				cameraData->data[1].y = fixCamData1Vec.y;
				cameraData->data[1].z = fixCamData1Vec.z;
				restoreCamData1 = false;
			} else {
				fixCamData1Vec.x = cameraData->data[1].x;
				fixCamData1Vec.y = cameraData->data[1].y;
				fixCamData1Vec.z = cameraData->data[1].z;
			}
			wasInCutscene = false;
		}
	}
}

void ResetCameraToNearestSide(EventData& eventData, PlayerActorData& mainActorData, CameraData* cameraData) {
	if (activeCrimsonConfig.Camera.rightStickCameraCentering != RIGHTSTICKCENTERCAM::TO_NEAREST_SIDE) {
		return;
	}
    static bool defaultCamSet = false;
    constexpr float TWO_PI = 6.283185307f;
    constexpr float PI = 3.1415926535f;
    float radius = 200.0f;
    float radiusZ = 200.0f;
    float verticalOffset = 140.0f;

    if (mainActorData.buttons[0] & GetBinding(BINDING::DEFAULT_CAMERA) && !defaultCamSet) {
        if (!g_inGameCutscene) {
            // Character's forward angle in world space
            float charAngle = (mainActorData.rotation / 65535.0f) * TWO_PI;
           // charAngle += PI;

            // Use -dx, -dz so angle is from camera to character (matches forward logic)
            float dx = cameraData->data[0].x - mainActorData.position.x;
            float dz = cameraData->data[0].z - mainActorData.position.z;
            float camAngle = atan2f(-dx, -dz);

            // Normalize angles to [-PI, PI]
            auto NormalizeAngle = [](float angle) {
                constexpr float PI = 3.1415926535f;
                constexpr float TWO_PI = 6.283185307f;
                while (angle > PI) angle -= TWO_PI;
                while (angle < -PI) angle += TWO_PI;
                return angle;
            };

            float diff = NormalizeAngle(camAngle - charAngle);

            // If diff > 0, camera is on the left; snap to left. If diff < 0, snap to right.
            float targetAngle = (diff > 0.0f) ? (charAngle + (PI / 2.0f)) : (charAngle - (PI / 2.0f));

            vec3 offset;
            offset.x = -sinf(targetAngle) * radius;
            offset.z = -cosf(targetAngle) * radiusZ;
            offset.y = verticalOffset;

            cameraData->data[0].x = mainActorData.position.x + offset.x;
            cameraData->data[0].y = mainActorData.position.y + offset.y;
            cameraData->data[0].z = mainActorData.position.z + offset.z;
            mainActorData.position.x = mainActorData.position.x + 1; // Triggers camera orbit

            defaultCamSet = true;
        }
    } else if (!(mainActorData.buttons[0] & GetBinding(BINDING::DEFAULT_CAMERA))) {
        defaultCamSet = false;
    }
}

void GeneralCameraOptionsController() {
	static bool setCamPos = false;
	auto pool_10298 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
	if (!pool_10298 || !pool_10298[8]) {
		return;
	}
	auto& eventData = *reinterpret_cast<EventData*>(pool_10298[8]);
	CameraData* cameraData = GetSafeCameraData();
	auto pool_4449 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC8FBD0);
	if (!pool_4449) return;
	auto& cameraControlMetadata = *reinterpret_cast<CameraControlMetadata*>(pool_4449);

	auto pool_10222 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_10222 || !pool_10222[3]) {
		return;
	}
	auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_10222[3]);
	if (eventData.event != EVENT::MAIN && eventData.event != EVENT::PAUSE) {
		setCamPos = false;
		return;
	}
	if (g_inGameCutscene) {
		setCamPos = false;
	}

	g_disableRightStickCenterCamera = (activeCrimsonConfig.Camera.rightStickCameraCentering == RIGHTSTICKCENTERCAM::OFF ||
		activeCrimsonConfig.Camera.rightStickCameraCentering == RIGHTSTICKCENTERCAM::TO_NEAREST_SIDE) ? true : false;

	FixInitialCameraRotation(eventData, mainActorData, cameraData, setCamPos);
	VajuraBugFix(cameraData, eventData);
	ResetCameraToNearestSide(eventData, mainActorData, cameraData);

	CrimsonPatches::CameraSensController();
	
	//CrimsonDetours::ToggleCustomCameraSensitivity(activeConfig.Actor.playerCount == 1);

	if (cameraData != nullptr) {
		CrimsonPatches::CameraFollowUpSpeedController(*cameraData, cameraControlMetadata);
		CrimsonPatches::CameraDistanceController(cameraData, cameraControlMetadata);
		CrimsonPatches::CameraTiltController(cameraData, cameraControlMetadata);
	}

	CrimsonPatches::ToggleLockedOffCamera(g_disableCameraRotation ? false : activeCrimsonConfig.Camera.lockedOff);
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

void PauseSFXWhenPaused() {
	auto name_10723 = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E30);
	if (!name_10723) {
		return;
	}
	auto& missionData = *reinterpret_cast<MissionData*>(name_10723);
	auto pool_10298 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
	if (!pool_10298 || !pool_10298[8]) {
		return;
	}
	auto& eventData = *reinterpret_cast<EventData*>(pool_10298[8]);

	for (int i = 0; i < PLAYER_COUNT; ++i) {
		if (eventData.event == EVENT::PAUSE) {
			CrimsonSDL::PauseDTExplosionSFX(i);
		}
		else {
			CrimsonSDL::ResumeDTExplosionSFX(i);
		}
	}
}

void TrackMissionStyleLevels() {
	auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

	auto name_10723 = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E30);
	if (!name_10723) {
		return;
	}
	auto& missionData = *reinterpret_cast<MissionData*>(name_10723);
	
	if (missionData.frameCount <= 0 && g_scene != SCENE::MISSION_START && g_scene != SCENE::GAME && g_scene != SCENE::CUTSCENE) {
		for (int style = 0; style < STYLE::MAX; style++) {
			heldStyleExpDataDante.missionStyleLevels[style] = ExpConfig::sessionExpDataDante.styleLevels[style];
			heldStyleExpDataVergil.missionStyleLevels[style] = ExpConfig::sessionExpDataVergil.styleLevels[style];
			heldStyleExpDataDante.accumulatedStyleLevels[style] = ExpConfig::sessionExpDataDante.styleLevels[style];
			heldStyleExpDataVergil.accumulatedStyleLevels[style] = ExpConfig::sessionExpDataVergil.styleLevels[style];
		}
	}
	
}

#pragma endregion

#pragma region EnemyGameplay

void OverrideEnemyTargetPosition() {
	// Run only every 200ms (0.2 seconds)
	static double lastUpdateTime = 0.0;
	double currentTime = ImGui::GetTime();
	if (currentTime - lastUpdateTime < 0.2) {
		return;
	}
	lastUpdateTime = currentTime;

	if (g_scene != SCENE::GAME || g_inGameCutscene) {
		CrimsonPatches::DisableEnemyTargetting1PPosition(false);
		return;
	} else if (g_scene == SCENE::GAME &&
		activeConfig.Actor.enable) {
		CrimsonPatches::DisableEnemyTargetting1PPosition(activeConfig.Actor.playerCount > 1);
	}
	CrimsonEnemyAITarget::EnemyAIMultiplayerTargettingDetours(activeConfig.Actor.playerCount > 1);

	if (activeConfig.Actor.playerCount == 1 || g_inGameCutscene || g_scene != SCENE::GAME) return;

	auto pool_10222 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_10222 || !pool_10222[3]) {
		return;
	}
	auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_10222[3]);

	auto pool_2128 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_2128 || !pool_2128[8]) return;
	auto& enemyVectorData = *reinterpret_cast<EnemyVectorData*>(pool_2128[8]);

	for (auto enemyMeta : enemyVectorData.metadata) {
		if (!enemyMeta.baseAddr) continue;
		auto& enemy = *reinterpret_cast<EnemyActorData*>(enemyMeta.baseAddr);
		if (!enemy.baseAddr) continue;

		glm::vec3 enemyPosition = { enemy.position.x, enemy.position.y, enemy.position.z };

		glm::vec3 playerPosition[PLAYER_COUNT];
		float distanceToPlayer[PLAYER_COUNT];
		uintptr_t closestPlayerAddr = (uintptr_t)mainActorData.baseAddr;
		float closestDistance = 9000.0f;
		auto& enemyId = enemy.enemy;

		for (uint8 playerIndex = 0; playerIndex < activeConfig.Actor.playerCount; playerIndex++) {
// 			auto& playerData = GetPlayerData(playerIndex);
// 			auto& characterData = GetCharacterData(playerIndex, playerData.characterIndex, ENTITY::MAIN);
// 			auto& newActorData = GetNewActorData(playerIndex, playerData.characterIndex, ENTITY::MAIN);

			if (!crimsonPlayer[playerIndex].playerPtr) {
				continue;
			}
			auto& actorData = *reinterpret_cast<PlayerActorData*>(crimsonPlayer[playerIndex].playerPtr);

			// Skip dead players
			if (actorData.dead) {
				continue;
			}

			playerPosition[playerIndex] = { actorData.position.x, actorData.position.y, actorData.position.z };
			distanceToPlayer[playerIndex] = glm::distance(enemyPosition, playerPosition[playerIndex]);

			bool isAgniRudra = (enemyId >= ENEMY::AGNI_RUDRA_ALL && enemyId <= ENEMY::AGNI_RUDRA_BLUE);

			if (!enemy.baseAddr) {
				continue;
			}

			if (enemyId == ENEMY::BLOOD_GOYLE) {
				continue;
			}

			if (distanceToPlayer[playerIndex] < closestDistance) {
				closestDistance = distanceToPlayer[playerIndex];
				if ((enemyId >= ENEMY::PRIDE_1 && enemyId <= ENEMY::WRATH_4) && enemy.hitPointsHells > 20) {
					enemy.targetPosition = actorData.position;
				}

				if (enemyId >= ENEMY::GREED_1 && enemyId <= ENEMY::GREED_4 && enemy.hitPointsHells > 20) {
					enemy.targetPositionGreed = actorData.position;
				}

				if (enemyId == ENEMY::ABYSS && enemy.hitPointsHells > 20) {
					enemy.targetPositionAbyss = actorData.position;
				}

				if (enemyId == ENEMY::ENVY && enemy.hitPointsHells > 20) {
					enemy.targetPositionEnvy = actorData.position;
				}

				if (enemyId == ENEMY::HELL_VANGUARD && enemy.hitPointsHells > 20) {
					enemy.targetPositionHellVanguard = actorData.position;
				}

				if (enemyId == ENEMY::DOPPELGANGER && enemy.hitPointsDoppelganger > 20) {
					enemy.targetPositionDullahan = actorData.position;
				}

				if (enemyId == ENEMY::THE_FALLEN && enemy.hitPointsTheFallen > 20) {
					enemy.targetPositionDullahan = actorData.position;
				}

				if (enemyId == ENEMY::DULLAHAN && enemy.hitPointsDullahan > 20) {
					enemy.targetPositionDullahan = actorData.position;
				}

				if (enemyId == ENEMY::BEOWULF && enemy.hitPointsBeowulf > 20) {
					enemy.targetPositionDullahan = actorData.position;
				}

				if (enemyId == ENEMY::VERGIL && enemy.hitPointsVergil > 20) {
					enemy.targetPositionDullahan = actorData.position;
				}

				if (enemyId == ENEMY::LADY && enemy.hitPointsLady > 20) {
					enemy.targetPositionDullahan = actorData.position;
				}
			}
		}
	}
}

void UpdateDevilArmProgression(uint8 unlockId, uint8 gameData, uint8 weaponId, std::string weaponName) {
	weaponProgression.devilArmUnlocks[unlockId] = gameData;

	auto& names = weaponProgression.meleeWeaponNames;
	auto& ids = weaponProgression.meleeWeaponIds;

	auto nameIt = std::find(names.begin(), names.end(), weaponName);
	auto idIt = std::find(ids.begin(), ids.end(), weaponId);

	if (gameData) {
		if (nameIt == names.end()) {
			names.push_back(weaponName);
		}
		if (idIt == ids.end()) {
			ids.push_back(weaponId);
		}

	} else {
		if (nameIt != names.end()) {
			names.erase(nameIt);
		}
		if (idIt != ids.end()) {
			ids.erase(idIt);
		}

	}
}

void UpdateGunProgression(uint8 unlockId, uint8 gameData, uint8 weaponId, std::string weaponName) {
	weaponProgression.gunUnlocks[unlockId] = gameData;

	auto& names = weaponProgression.rangedWeaponNames;
	auto& ids = weaponProgression.rangedWeaponIds;

	auto nameIt = std::find(names.begin(), names.end(), weaponName);
	auto idIt = std::find(ids.begin(), ids.end(), weaponId);

	if (gameData) {
		if (nameIt == names.end()) {
			names.push_back(weaponName);
		}
		if (idIt == ids.end()) {
			ids.push_back(weaponId);
		}

	} else {
		if (nameIt != names.end()) {
			names.erase(nameIt);
		}
		if (idIt != ids.end()) {
			ids.erase(idIt);
		}
	}
}

bool CheckIfWeaponIdIsUnlocked(uint8 weaponId) {
	auto& names = weaponProgression.meleeWeaponNames;
	auto& ids = weaponProgression.meleeWeaponIds;
	auto idIt = std::find(ids.begin(), ids.end(), weaponId);
	if (idIt != ids.end()) {
		return true;
	}
	return false;
}

std::string GetWeaponNameById(uint8 weaponId) {
	switch (weaponId) {
	case WEAPON::CERBERUS:
		return "Cerberus";
	case WEAPON::AGNI_RUDRA:
		return "Agni & Rudra";
	case WEAPON::NEVAN:
		return "Nevan";
	case WEAPON::BEOWULF_DANTE:
		return "Beowulf";
	case WEAPON::SHOTGUN:
		return "Shotgun";
	case WEAPON::ARTEMIS:
		return "Artemis";
	case WEAPON::SPIRAL:
		return "Spiral";
	case WEAPON::KALINA_ANN:
		return "Kalina Ann";
	}

	return "";
}


void WeaponProgressionTracking() {
	auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

	if (g_scene == SCENE::GAME || g_scene == SCENE::MISSION_RESULT) {
		for (size_t i = 0; i < ITEM::COUNT; i++) {
			auto name_10723 = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E30);
			if (!name_10723) {
				return;
			}
			auto& missionData = *reinterpret_cast<MissionData*>(name_10723);
			switch (i) {
			case ITEM::CERBERUS:
				UpdateDevilArmProgression(DEVILARMUNLOCKS::CERBERUS, missionData.itemCounts[i], WEAPON::CERBERUS, "Cerberus");
				break;
			case ITEM::AGNI_RUDRA:
				UpdateDevilArmProgression(DEVILARMUNLOCKS::AGNI_RUDRA, missionData.itemCounts[i], WEAPON::AGNI_RUDRA, "Agni & Rudra");
				break;
			case ITEM::NEVAN:
				UpdateDevilArmProgression(DEVILARMUNLOCKS::NEVAN, missionData.itemCounts[i], WEAPON::NEVAN, "Nevan");
				break;
			case ITEM::BEOWULF:
				UpdateDevilArmProgression(DEVILARMUNLOCKS::BEOWULF, missionData.itemCounts[i], WEAPON::BEOWULF_DANTE, "Beowulf");
				break;
			case ITEM::SHOTGUN:
				UpdateGunProgression(GUNUNLOCKS::SHOTGUN, missionData.itemCounts[i], WEAPON::SHOTGUN, "Shotgun");
				break;
			case ITEM::ARTEMIS:
				UpdateGunProgression(GUNUNLOCKS::ARTEMIS, missionData.itemCounts[i], WEAPON::ARTEMIS, "Artemis");
				break;
			case ITEM::SPIRAL:
				UpdateGunProgression(GUNUNLOCKS::SPIRAL, missionData.itemCounts[i], WEAPON::SPIRAL, "Spiral");
				break;
			case ITEM::KALINA_ANN:
				UpdateGunProgression(GUNUNLOCKS::KALINA_ANN, missionData.itemCounts[i], WEAPON::KALINA_ANN, "Kalina Ann");
				break;
			}
		}
	} else if (g_scene == SCENE::MISSION_SELECT || g_scene == SCENE::MAIN || g_scene == SCENE::MISSION_START) {
		for (size_t i = 0; i < WEAPONANDSTYLEUNLOCKS::COUNT; i++) {
			switch (i) {
			case WEAPONANDSTYLEUNLOCKS::CERBERUS:
				UpdateDevilArmProgression(DEVILARMUNLOCKS::CERBERUS, sessionData.weaponAndStyleUnlocks[i], WEAPON::CERBERUS, "Cerberus");
				break;
			case WEAPONANDSTYLEUNLOCKS::AGNI_RUDRA:
				UpdateDevilArmProgression(DEVILARMUNLOCKS::AGNI_RUDRA, sessionData.weaponAndStyleUnlocks[i], WEAPON::AGNI_RUDRA, "Agni & Rudra");
				break;
			case WEAPONANDSTYLEUNLOCKS::NEVAN:
				UpdateDevilArmProgression(DEVILARMUNLOCKS::NEVAN, sessionData.weaponAndStyleUnlocks[i], WEAPON::NEVAN, "Nevan");
				break;
			case WEAPONANDSTYLEUNLOCKS::BEOWULF:
				UpdateDevilArmProgression(DEVILARMUNLOCKS::BEOWULF, sessionData.weaponAndStyleUnlocks[i], WEAPON::BEOWULF_DANTE, "Beowulf");
				break;
			case WEAPONANDSTYLEUNLOCKS::SHOTGUN:
				UpdateGunProgression(GUNUNLOCKS::SHOTGUN, sessionData.weaponAndStyleUnlocks[i], WEAPON::SHOTGUN, "Shotgun");
				break;
			case WEAPONANDSTYLEUNLOCKS::ARTEMIS:
				UpdateGunProgression(GUNUNLOCKS::ARTEMIS, sessionData.weaponAndStyleUnlocks[i], WEAPON::ARTEMIS, "Artemis");
				break;
			case WEAPONANDSTYLEUNLOCKS::SPIRAL:
				UpdateGunProgression(GUNUNLOCKS::SPIRAL, sessionData.weaponAndStyleUnlocks[i], WEAPON::SPIRAL, "Spiral");
				break;
			case WEAPONANDSTYLEUNLOCKS::KALINA_ANN:
				UpdateGunProgression(GUNUNLOCKS::KALINA_ANN, sessionData.weaponAndStyleUnlocks[i], WEAPON::KALINA_ANN, "Kalina Ann");
				break;
			}
		}
	}
	
	// Track Unlocked Weapon Quantity
	int previousDevilArmUnlockedQtt = weaponProgression.devilArmsUnlockedQtt;
	int previousGunsUnlockedQtt = weaponProgression.gunsUnlockedQtt;

	// Track Current Weapon Quantity
	weaponProgression.devilArmsUnlockedQtt = 0;
	for (size_t i = 0; i < DEVILARMUNLOCKS::COUNT; ++i) {
		if (weaponProgression.devilArmUnlocks[i]) {
			weaponProgression.devilArmsUnlockedQtt++;
		}
	}

	weaponProgression.gunsUnlockedQtt = 0;
	for (size_t i = 0; i < GUNUNLOCKS::COUNT; ++i) {
		if (weaponProgression.gunUnlocks[i]) {
			weaponProgression.gunsUnlockedQtt++;
		}
	}

	for (size_t playerIndex = 0; playerIndex < PLAYER_COUNT; playerIndex++) {
		for (size_t characterIndex = 0; characterIndex < CHARACTER_COUNT; characterIndex++) {
			auto& activeCharacterData = activeConfig.Actor.playerData[playerIndex].characterData[characterIndex][ENTITY::MAIN];
			auto& queuedCharacterData = queuedConfig.Actor.playerData[playerIndex].characterData[characterIndex][ENTITY::MAIN];

			auto& lastMaxMeleeWeaponCount = queuedCrimsonConfig.CachedSettings.lastMaxMeleeWeaponCount[playerIndex][characterIndex];
			auto& lastMaxRangedWeaponCount = queuedCrimsonConfig.CachedSettings.lastMaxRangedWeaponCount[playerIndex][characterIndex];
			auto& lastMaxMeleeWeaponCountVergil = queuedCrimsonConfig.CachedSettings.lastMaxMeleeWeaponCountVergil[playerIndex][characterIndex];
			auto& lastEquippedMeleeWeapons = queuedCrimsonConfig.CachedSettings.lastEquippedMeleeWeapons[playerIndex][characterIndex];
			auto& lastEquippedRangedWeapons = queuedCrimsonConfig.CachedSettings.lastEquippedRangedWeapons[playerIndex][characterIndex];
			auto& lastEquippedMeleeWeaponsVergil = queuedCrimsonConfig.CachedSettings.lastEquippedMeleeWeaponsVergil[playerIndex][characterIndex];

			// Caching lastMaxWeaponCounts and equippedWeapons for GUI usability and for maintaining the Weapon Progression System.
			if (activeCharacterData.character == CHARACTER::VERGIL) {
				if (activeCharacterData.meleeWeaponCount > 1) {
					lastMaxMeleeWeaponCountVergil = activeCharacterData.meleeWeaponCount;
					for (size_t i = 0; i < MELEE_WEAPON_COUNT_DANTE; i++) {
						lastEquippedMeleeWeaponsVergil[i] = activeCharacterData.meleeWeapons[i];
					}
				}
			}

			if (activeCharacterData.character != CHARACTER::DANTE) {
				break;
			}

			if (activeCharacterData.meleeWeaponCount > 1) {
				lastMaxMeleeWeaponCount = activeCharacterData.meleeWeaponCount;
				for (size_t i = 0; i < MELEE_WEAPON_COUNT_DANTE; i++) {
					lastEquippedMeleeWeapons[i] = activeCharacterData.meleeWeapons[i];
				}
			}

			if (activeCharacterData.rangedWeaponCount > 1) {
				lastMaxRangedWeaponCount = activeCharacterData.rangedWeaponCount;
				for (size_t i = 0; i < RANGED_WEAPON_COUNT_DANTE; i++) {
					lastEquippedRangedWeapons[i] = activeCharacterData.rangedWeapons[i];
				}
			}

			// Only update weaponCount when the unlocked quantity has changed
			// DEVIL ARMS
			if (weaponProgression.devilArmsUnlockedQtt != previousDevilArmUnlockedQtt) {

				if (g_scene == SCENE::GAME) {
					// If the player is in a mission, set the melee weapon count to the max unlocked quantity
					activeCharacterData.meleeWeaponCount = weaponProgression.devilArmsUnlockedQtt + 1;
					queuedCharacterData.meleeWeaponCount = weaponProgression.devilArmsUnlockedQtt + 1;
					lastMaxMeleeWeaponCount = weaponProgression.devilArmsUnlockedQtt + 1;
				} else {
					// If the player is not in a mission, set the melee weapon count to the last max melee weapon count
					if (lastMaxMeleeWeaponCount > weaponProgression.devilArmsUnlockedQtt + 1) {
						activeCharacterData.meleeWeaponCount = weaponProgression.devilArmsUnlockedQtt + 1;
						queuedCharacterData.meleeWeaponCount = weaponProgression.devilArmsUnlockedQtt + 1;
					} else {
						activeCharacterData.meleeWeaponCount = lastMaxMeleeWeaponCount;
						queuedCharacterData.meleeWeaponCount = lastMaxMeleeWeaponCount;
					}
				}

				// Replace invalid melee weapons
				for (size_t i = 0; i < activeCharacterData.meleeWeaponCount; ++i) {
					uint8_t equippedId = activeCharacterData.meleeWeapons[i];

					// If the equipped weapon is not in the unlocked list
					if (std::find(weaponProgression.meleeWeaponIds.begin(), weaponProgression.meleeWeaponIds.end(), equippedId) == weaponProgression.meleeWeaponIds.end()) {
						// Try to find a replacement weapon that is unlocked and not already equipped
						for (size_t j = 0; j < weaponProgression.meleeWeaponIds.size(); ++j) {
							uint8_t replacementId = weaponProgression.meleeWeaponIds[j];
							bool alreadyEquipped = false;

							// Skip if this replacement is already equipped
							for (size_t k = 0; k < activeCharacterData.meleeWeaponCount; ++k) {
								if (activeCharacterData.meleeWeapons[k] == replacementId) {
									alreadyEquipped = true;
									break;
								}
							}
							if (!alreadyEquipped) {
								activeCharacterData.meleeWeapons[i] = replacementId;
								queuedCharacterData.meleeWeapons[i] = replacementId;

								// Keep meleeWeaponNames in sync
								if (i < weaponProgression.meleeWeaponNames.size()) {
									weaponProgression.meleeWeaponNames[i] = GetWeaponNameById(replacementId); // j matches unlocked ID
								} else {
									// Pad the vector to make space if needed
									while (weaponProgression.meleeWeaponNames.size() <= i)
										weaponProgression.meleeWeaponNames.push_back("");
									weaponProgression.meleeWeaponNames[i] = GetWeaponNameById(replacementId);
								}
								break;
							}
						}
					}
				}
			}

			// Safety check: ensure count never becomes 0
			if (activeCharacterData.meleeWeaponCount == 0)
				activeCharacterData.meleeWeaponCount = 1;
			if (queuedCharacterData.meleeWeaponCount == 0)
				queuedCharacterData.meleeWeaponCount = 1;

			// GUNS
			if (weaponProgression.gunsUnlockedQtt != previousGunsUnlockedQtt) {

				if (g_scene == SCENE::GAME) {
					// If the player is in a mission, set the melee weapon count to the max unlocked quantity
					activeCharacterData.rangedWeaponCount = weaponProgression.gunsUnlockedQtt + 1;
					queuedCharacterData.rangedWeaponCount = weaponProgression.gunsUnlockedQtt + 1;
					lastMaxRangedWeaponCount = weaponProgression.gunsUnlockedQtt + 1;
				} else {
					// If the player is not in a mission, set the melee weapon count to the last max melee weapon count
					if (lastMaxRangedWeaponCount > weaponProgression.gunsUnlockedQtt + 1) {
						activeCharacterData.rangedWeaponCount = weaponProgression.gunsUnlockedQtt + 1;
						queuedCharacterData.rangedWeaponCount = weaponProgression.gunsUnlockedQtt + 1;
					} else {
						activeCharacterData.rangedWeaponCount = lastMaxRangedWeaponCount;
						queuedCharacterData.rangedWeaponCount = lastMaxRangedWeaponCount;
					}
				}

				// Replace invalid melee weapons
				for (size_t i = 0; i < activeCharacterData.rangedWeaponCount; ++i) {
					uint8_t equippedId = activeCharacterData.rangedWeapons[i];

					// If the equipped weapon is not in the unlocked list
					if (std::find(weaponProgression.rangedWeaponIds.begin(), weaponProgression.rangedWeaponIds.end(), equippedId) == weaponProgression.rangedWeaponIds.end()) {
						// Try to find a replacement weapon that is unlocked and not already equipped
						for (size_t j = 0; j < weaponProgression.rangedWeaponIds.size(); ++j) {
							uint8_t replacementId = weaponProgression.rangedWeaponIds[j];
							bool alreadyEquipped = false;

							// Skip if this replacement is already equipped
							for (size_t k = 0; k < activeCharacterData.rangedWeaponCount; ++k) {
								if (activeCharacterData.rangedWeapons[k] == replacementId) {
									alreadyEquipped = true;
									break;
								}
							}
							if (!alreadyEquipped) {
								activeCharacterData.rangedWeapons[i] = replacementId;
								queuedCharacterData.rangedWeapons[i] = replacementId;

								// Keep meleeWeaponNames in sync
								if (i < weaponProgression.rangedWeaponNames.size()) {
									weaponProgression.rangedWeaponNames[i] = GetWeaponNameById(replacementId); // j matches unlocked ID
								} else {
									// Pad the vector to make space if needed
									while (weaponProgression.rangedWeaponNames.size() <= i)
										weaponProgression.rangedWeaponNames.push_back("");
									weaponProgression.rangedWeaponNames[i] = GetWeaponNameById(replacementId);
								}
								break;
							}
						}
					}
				}
			}

			if (activeCharacterData.character == CHARACTER::DANTE) {
				if (activeCharacterData.rangedWeaponCount == 0)
					activeCharacterData.rangedWeaponCount = 1;
				if (queuedCharacterData.rangedWeaponCount == 0)
					queuedCharacterData.rangedWeaponCount = 1;
			}
		}
	}
}

void FixM7DevilTriggerUnlocking() {
	auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

	if ((g_scene == SCENE::MISSION_RESULT && sessionData.mission == 7) || 
		(g_scene == SCENE::MISSION_START && sessionData.mission == 8)) {
		if (!sessionData.unlockDevilTrigger && sessionData.magicPoints == 0) {
			sessionData.unlockDevilTrigger = true;
			sessionData.magicPoints = 3000;
		}
	}
}
}
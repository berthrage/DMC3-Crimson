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

}
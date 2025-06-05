#include "CrimsonBetterArkham2.hpp"

namespace CrimsonBetterArkham2 {
	static bool fightActive{ false };
	static bool fightEnding{ false };
	enum {
		PHASE_1, //arkham lobby 1
		PHASE_2, //cerberus
		PHASE_3,//arkham lobby 2
		PHASE_4, //agni rudra
		PHASE_5, //arkham lobby 3
		PHASE_6, //beowulf
		PHASE_7, //arkham lobby final
	};
	//when arkham drops below this point in a phase, he should move to next phase
	float arkhamHealthGates[] = {7500.0f,0.0f,5000.0f,0.0f,2500,0.0f,1.0f};

	//arkham shouldn't have more health than this in his respective phases. This simulates the boss phase damaging him.
	float arkhamHealthCheckpoints[] = { 9000.0f,0.0f,6000.0f,0.0f,3500.0f,0.0f,1000.0f };

	//current phase of the fight
	static int fightPhase{ CrimsonBetterArkham2::PHASE_1 };
	//the queued next phase of the fight.
	//due to the flow logic being done on tic, we need this variable to make sure we don't accidentally skip phases.
	static int nextFightPhase{ CrimsonBetterArkham2::PHASE_1 };

	vec4 spawnPosition{
		.x = 0.0f,
		.y = 0.0f,
		.z = 0.0f,
		.a = 0.0f
	};

	vec4 beowulfSpawnPosition{
	.x = 3000.0f,
	.y = 54.0f,
	.z = 2500.0f,
	.a = 0.0f
	};

	ConfigCreateEnemyActorData arkham2Data{
		.enemy = ENEMY::ARKHAM,
		.variant = 1,
		.position = spawnPosition,
		.rotation = 0,
		//to determine starting location
		.useMainActorData = true,
		.spawnMethod = 0
	};

	ConfigCreateEnemyActorData cerbData{
		.enemy = ENEMY::CERBERUS,
		.variant = 1,
		.position = spawnPosition,
		.rotation = 0,
		//to determine starting location
		.useMainActorData = true,
		.spawnMethod = 0
	};

	ConfigCreateEnemyActorData agniRudraData{
		.enemy = ENEMY::AGNI_RUDRA,
		.variant = 1,
		.position = spawnPosition,
		.rotation = 0,
		//to determine starting location
		.useMainActorData = true,
		.spawnMethod = 0
	};

	ConfigCreateEnemyActorData beowulfData{
		.enemy = ENEMY::BEOWULF,
		.variant = 1,
		.position = beowulfSpawnPosition,
		.rotation = 0,
		//to determine starting location
		.useMainActorData = false,
		.spawnMethod = 0
	};

	/// <summary>
	/// Sets the fight to not active when player hits continue on death screen.
	/// </summary>
	void Continue()
	{
		fightActive = false;
		fightEnding = false;
		CrimsonPatches::DisableDoorsInstancing(false);
		CrimsonDetours::ToggleHideAndMutePortals(false);
		return;
	};

	/// <summary>
	/// Overrides the music trigger when skipping the real arkham 2 boss fight
	/// </summary>
	/// <param name="filename"></param>
	/// <returns>false if we need to skip the arkham music</returns>
	bool SetTrack(const char* filename) {
		if (!activeCrimsonGameplay.Gameplay.ExtraDifficulty.betterArkham2) {
			return true;
		}

		LogFunction();

		auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

		auto pool_328 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
		if (!pool_328 || !pool_328[8]) {
			return true;
		}
		auto& eventData = *reinterpret_cast<EventData*>(pool_328[8]);

		auto pool_369 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
		if (!pool_369 || !pool_369[12]) {
			return true;
		}
		auto& nextEventData = *reinterpret_cast<NextEventData*>(pool_369[12]);

		auto pool_410 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E30);
		if (!pool_410 || !pool_410[1]) {
			return true;
		}
		auto eventFlags = reinterpret_cast<byte32*>(pool_410[1]);

		//arkham 2 audio skip
		//the logic here is weird, we check if fightActive is true even though we want to mute the music after the fight
		//because this call happens before that variable updates. So if it's true it's about to be false. Get it?
		if ((sessionData.mission == 19) 
			&& (eventFlags[20] == 1) && (fightActive == true) &&
			(strcmp(filename, bossHelpers[BOSS::ARKHAM_PART_2].track) == 0)) {
			return false;
		}

		return true;
	}

	/// <summary>
	/// Sets the fight to not active when the player:
	/// restarts the mission
	/// quits the mission
	/// finishes the mission
	/// game overs
	/// </summary>
	void SetNextScreen(EventData& eventData) {
		if ((eventData.nextScreen == SCREEN::MISSION_CLEAR)
			|| (eventData.nextScreen == SCREEN::GAME_OVER)
			|| (eventData.nextScreen == SCREEN::MISSION_SELECT)
			|| (eventData.nextScreen == SCREEN::MISSION_START)
		)

			CrimsonPatches::DisableDoorsInstancing(false);
			CrimsonDetours::ToggleHideAndMutePortals(false);
			fightActive = false;
			fightEnding = false;
		return;
	}


	/// <summary>
	/// return true when conditions are met for the arkham lobby phase to end.
	/// </summary>
	/// <param name="enemyVectorData"></param>
	/// <returns></returns>
	bool isEndArkhamLobby(EnemyVectorData& enemyVectorData) {
		for (auto enemy : enemyVectorData.metadata) {
			if (!enemy.baseAddr) continue;
			auto& enemyData = *reinterpret_cast<EnemyActorData*>(enemy.baseAddr);
			if (!enemyData.baseAddr) continue;

			//this decrements arkham's health so his HP bar goes down throughout the boss rush 
			if (enemyData.enemy == ENEMY::ARKHAM && enemyData.hitPointsArkham > arkhamHealthCheckpoints[fightPhase])
				enemyData.hitPointsArkham = arkhamHealthCheckpoints[fightPhase];

			//whenever arkham dives and sends out the dolphins, we don't fight them and advance to boss rush phase.
			//don't do this if phase 7 tho or the fight just ends
			if (enemyData.enemy == ENEMY::ARKHAM_LEECHES && fightActive && !(fightPhase == PHASE_7))
				return true;

			//this health gate stops people from skipping phases 
			if (enemyData.enemy == ENEMY::ARKHAM && enemyData.hitPointsArkham < arkhamHealthGates[fightPhase] && fightActive)
				return true;

		};
		return false;
	}

	/// <summary>
	/// Return true when the cerberus phase ends so the game knows to start transitioning 
	/// </summary>
	/// <param name="enemyVectorData"></param>
	/// <returns></returns>
	bool isEndCerberusPhase(EnemyVectorData& enemyVectorData) {
		for (auto enemy : enemyVectorData.metadata) {
			if (!enemy.baseAddr) continue;
			auto& enemyData = *reinterpret_cast<EnemyActorData*>(enemy.baseAddr);
			if (!enemyData.baseAddr) continue;

			//advance to phase2:
			if (enemyData.enemy == ENEMY::CERBERUS && enemyData.hitPointsCerberusTotal < 1.0f && fightActive)
				return true;
		};
		return false;
	}

	/// <summary>
	/// Return true when the beowulf phase ends so the game knows to start transitioning 
	/// </summary>
	/// <param name="enemyVectorData"></param>
	/// <returns></returns>
	bool isEndBeowulfPhase(EnemyVectorData& enemyVectorData) {
		for (auto enemy : enemyVectorData.metadata) {
			if (!enemy.baseAddr) continue;
			auto& enemyData = *reinterpret_cast<EnemyActorData*>(enemy.baseAddr);
			if (!enemyData.baseAddr) continue;

			//advance to phase2:
			if (enemyData.enemy == ENEMY::BEOWULF && enemyData.hitPointsBeowulf < 1.0f && fightActive)
				return true;
		};
		return false;
	}

/// <summary>
/// Return true when the beowulf phase ends so the game knows to start transitioning 
/// </summary>
/// <param name="enemyVectorData"></param>
/// <returns></returns>
	bool isEndAgniRudraPhase(EnemyVectorData& enemyVectorData) {
		if(enemyVectorData.count == 0 )
			return true;
		return false;
	}



	/// <summary>
	/// Manages the flow of the fight.
	/// </summary>
	void OnTick()
	{
		if (!InGame())
			return;

		//we're doing a lot here, let's not do it unless we need to.
		if (!activeCrimsonGameplay.Gameplay.ExtraDifficulty.betterArkham2)
			return;

		auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

		auto pool_19315 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
		if (!pool_19315 || !pool_19315[8]) {
			return;
		}
		auto& eventData = *reinterpret_cast<EventData*>(pool_19315[8]);
		auto pool_19326 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
		if (!pool_19326 || !pool_19326[12]) {
			return;
		}
		auto& nextEventData = *reinterpret_cast<NextEventData*>(pool_19326[12]);

		auto pool_19337 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E30);
		if (!pool_19337 || !pool_19337[1]) {
			return;
		}

		auto pool_10222 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
		if (!pool_10222 || !pool_10222[3]) return;
		auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_10222[3]);

		auto pool_2128 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
		if (!pool_2128 || !pool_2128[8]) return;
		auto& enemyVectorData = *reinterpret_cast<EnemyVectorData*>(pool_2128[8]);

		auto eventFlags = reinterpret_cast<byte32*>(pool_19337[1]);

		//Teleport when fight ends to go to real arkham2 fight (this was put here for ez gui testing)
		if (sessionData.mission == 19
			&& eventData.room == 421
			&& activeCrimsonGameplay.Gameplay.ExtraDifficulty.betterArkham2
			&& eventFlags[20] == 2
			&& CrimsonBetterArkham2::fightEnding)
		{
			nextEventData.room = 421;
			eventData.event = EVENT::TELEPORT;
		}

		//this ensures we don't accidentally get stuck in a teleport loop where we keep trying to exit a phase over and over
		if (fightPhase != nextFightPhase)
			return;
		if (!fightActive)
			return;
		//move through phases.
		switch (fightPhase) {
			//Fight begins, dante & vergil vs arkham
			case PHASE_1:
				if (isEndArkhamLobby(enemyVectorData))
					nextFightPhase = PHASE_2;
			break;
			//cerberus arena, dante & vergil vs cerberus
			case PHASE_2:
				if (isEndCerberusPhase(enemyVectorData))
					nextFightPhase = PHASE_3;
			break;
			//arkham lobby 2
			case PHASE_3:
				if (isEndArkhamLobby(enemyVectorData))
					nextFightPhase = PHASE_4;
				break;
			//agni rudra fight
			case PHASE_4:
				if (isEndAgniRudraPhase(enemyVectorData))
					nextFightPhase = PHASE_5;
				break;
			//arkham lobby 3
			case PHASE_5:
				if (isEndArkhamLobby(enemyVectorData))
					nextFightPhase = PHASE_6;
				break;
			//beowulf fight
			case PHASE_6:
				if (isEndBeowulfPhase(enemyVectorData))
					nextFightPhase = PHASE_7;
				break;
			//final arkham fight
			case PHASE_7:
				if (isEndArkhamLobby(enemyVectorData))
					CrimsonBetterArkham2::fightEnding = true;
				break;
		}




		

		if (fightPhase != nextFightPhase) {
			switch(nextFightPhase){
			//arkham phases
			case PHASE_3:
			case PHASE_5:
			case PHASE_7:
				nextEventData.room = ROOM::FORBIDDEN_NIRVANA_2;
				eventData.event = EVENT::TELEPORT;
				break;
			//cerb
			case PHASE_2:
				nextEventData.room = ROOM::ICE_GUARDIAN_REBORN;
				eventData.event = EVENT::TELEPORT;
				break;
			//agni rudra
			case PHASE_4:
				nextEventData.room = ROOM::FIRESTORM_REBORN;
				eventData.event = EVENT::TELEPORT;
				break;
			//beowulf
			case PHASE_6:
				nextEventData.room = ROOM::LIGHTBEAST_REBORN;
				eventData.event = EVENT::TELEPORT;
				break;

			default:
				DebugLog("invalid fight phase transition!")
				break;
			}
		}		
		//Teleport when fight ends to go to real arkham2 fight
		if (sessionData.mission == 19
			&& eventData.room == 421
			&& activeCrimsonGameplay.Gameplay.ExtraDifficulty.betterArkham2
			&& eventFlags[20] == 2
			&& CrimsonBetterArkham2::fightEnding)
		{
			nextEventData.room = 421;
			eventData.event = EVENT::TELEPORT;
		}
	}

	void DebugGui() {
		GUI_Checkbox("Debug fight ending",
			CrimsonBetterArkham2::fightEnding);
	}

	/// <summary>
	/// Logic for entering and exiting better arkham 2 fight
	/// </summary>
	void SceneGame() {
		if (!activeCrimsonGameplay.Gameplay.ExtraDifficulty.betterArkham2)
			return;
		auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

		auto pool_19315 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
		if (!pool_19315 || !pool_19315[8]) {
			return;
		}
		auto& eventData = *reinterpret_cast<EventData*>(pool_19315[8]);

		auto pool_19326 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
		if (!pool_19326 || !pool_19326[12]) {
			return;
		}
		auto& nextEventData = *reinterpret_cast<NextEventData*>(pool_19326[12]);

		auto pool_19337 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E30);
		if (!pool_19337 || !pool_19337[1]) {
			return;
		}
		auto eventFlags = reinterpret_cast<byte32*>(pool_19337[1]);


		DebugLog("room          %u", eventData.room);
		DebugLog("position      %u", eventData.position);
		DebugLog("next room     %u", nextEventData.room);
		DebugLog("next position %u", nextEventData.position);
		DebugLog("event flag address %u", &eventFlags[20])
		DebugLog("flags         %X", eventFlags[20]);

		//Let's say we didn't fight arkham 2 and pretend we did. -Hitch 2025
		if ((sessionData.mission == 19) && (nextEventData.room == 421)) {
			if (eventFlags[20] == 1) {
				eventFlags[20] = 2;
				fightActive = true;
				fightPhase = PHASE_1;
				nextFightPhase = PHASE_1;
				fightEnding = false;
				//spawn our own arkham

				//configure actors here
				//suprise dante/vergil time
				//if (queuedConfig.Actor.playerCount == 1) {
				//	activeConfig.Actor.playerCount = 2;
				//	auto playerdata = GetActivePlayerData(1);
				//	playerdata.activeCharacterIndex = 0;
				//	playerdata.characterCount = 1;
				//	auto& activeCharacterData = GetActiveCharacterData(1, 0, 0);
				//	activeCharacterData.meleeWeaponCount = 2;
				//	activeCharacterData.meleeWeapons[0] = WEAPON::YAMATO_VERGIL;
				//	activeCharacterData.meleeWeapons[0] = WEAPON::BEOWULF_VERGIL;
				//	ApplyDefaultCharacterData(activeCharacterData, CHARACTER::VERGIL, 1, 0);
				//}
				
				//this shouldn't need to be called, the skip is toggled off by default unless a scenechange needs it
				//CrimsonPatches::EndBossFight(false);

				//this is in a bunch of places right now, might be better to update on tic idk
 				CrimsonPatches::DisableDoorsInstancing(true);
 				CrimsonDetours::ToggleHideAndMutePortals(true);
			}

			//eventFlags[20] == 2 means the fight is still active, and fightEnding means we just beat arkham so do the sped up BoBfight now.
			//when fightEnding isn't called, we need to skip this as we're doing boss rush stuff
			else if (eventFlags[20] == 2 && fightEnding) {
				eventFlags[20] = 1;
				fightActive = false;
				fightEnding = false;
				CrimsonPatches::EndBossFight(true);
				CrimsonPatches::DisableDoorsInstancing(false);
				CrimsonDetours::ToggleHideAndMutePortals(false);
			}
		}
		return;
	}


	/// <summary>
	/// Spawns appropriate enemies based on phase of fight
	/// </summary>
	void EventMain()
	{
		if (!activeCrimsonGameplay.Gameplay.ExtraDifficulty.betterArkham2)
			return;
		//spawn arkham 2 in
		auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

		auto pool_19315 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
		if (!pool_19315 || !pool_19315[8]) {
			return;
		}
		auto& eventData = *reinterpret_cast<EventData*>(pool_19315[8]);

		auto pool_19326 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
		if (!pool_19326 || !pool_19326[12]) {
			return;
		}
		auto& nextEventData = *reinterpret_cast<NextEventData*>(pool_19326[12]);

		auto pool_19337 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E30);
		if (!pool_19337 || !pool_19337[1]) {
			return;
		}
		auto eventFlags = reinterpret_cast<byte32*>(pool_19337[1]);

		//Spawn Enemies for the phases.

		//Arkham
		if (sessionData.mission == 19 && (nextEventData.room == ROOM::FORBIDDEN_NIRVANA_2) && eventFlags[20] == 2 && nextFightPhase == PHASE_1) {
			fightPhase = PHASE_1;
			CreateEnemyActor(arkham2Data, 0);
		}
		if (sessionData.mission == 19 && (nextEventData.room == ROOM::FORBIDDEN_NIRVANA_2) && eventFlags[20] == 2 && nextFightPhase == PHASE_3) {
			fightPhase = PHASE_3;
			CreateEnemyActor(arkham2Data, 0);
		}
		if (sessionData.mission == 19 && (nextEventData.room == ROOM::FORBIDDEN_NIRVANA_2) && eventFlags[20] == 2 && nextFightPhase == PHASE_5) {
			fightPhase = PHASE_5;
			CreateEnemyActor(arkham2Data, 0);
		}
		if (sessionData.mission == 19 && (nextEventData.room == ROOM::FORBIDDEN_NIRVANA_2) && eventFlags[20] == 2 && nextFightPhase == PHASE_7) {
			fightPhase = PHASE_7;
			CreateEnemyActor(arkham2Data, 0);
		}

		//Cerb
		if (sessionData.mission == 19 && (nextEventData.room == ROOM::ICE_GUARDIAN_REBORN) && eventFlags[20] == 2 && nextFightPhase == PHASE_2) {
			fightPhase = PHASE_2;
			CreateEnemyActor(cerbData, 0);
		}
		//Agni Rudra
		if (sessionData.mission == 19 && (nextEventData.room == ROOM::FIRESTORM_REBORN) && eventFlags[20] == 2 && nextFightPhase == PHASE_4) {
			fightPhase = PHASE_4;
			CreateEnemyActor(agniRudraData, 0);
		}
		//Cerb
		if (sessionData.mission == 19 && (nextEventData.room == ROOM::LIGHTBEAST_REBORN) && eventFlags[20] == 2 && nextFightPhase == PHASE_6) {
			fightPhase = PHASE_6;
			CreateEnemyActor(beowulfData, 0);
		}
	}

	void BlackoutArkham2OriginalScene() {
		auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

		auto pool_19315 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
		if (!pool_19315 || !pool_19315[8]) {
			return;
		}
		auto& eventData = *reinterpret_cast<EventData*>(pool_19315[8]);

		auto pool_19326 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
		if (!pool_19326 || !pool_19326[12]) {
			return;
		}
		auto& nextEventData = *reinterpret_cast<NextEventData*>(pool_19326[12]);

		auto pool_19337 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E30);
		if (!pool_19337 || !pool_19337[1]) {
			return;
		}
		auto eventFlags = reinterpret_cast<byte32*>(pool_19337[1]);

		if ((sessionData.mission == 19) && activeCrimsonGameplay.Gameplay.ExtraDifficulty.betterArkham2 
			&& g_scene == SCENE::GAME) {
			if (eventFlags[20] == 1) {
				ImGuiViewport* viewport = ImGui::GetMainViewport();
				ImGui::SetNextWindowPos(viewport->Pos);
				ImGui::SetNextWindowSize(viewport->Size);

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

				ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoBackground |
					ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;

				ImGui::Begin("BlackoutArkham2Window", nullptr, windowFlags);

				ImDrawList* draw_list = ImGui::GetWindowDrawList();
				ImVec2 p0 = viewport->Pos;
				ImVec2 p1 = ImVec2(viewport->Pos.x + viewport->Size.x, viewport->Pos.y + viewport->Size.y);
				draw_list->AddRectFilled(p0, p1, IM_COL32(0, 0, 0, 255));

				ImGui::End();
				ImGui::PopStyleVar(3);
			}
		}
	}
};

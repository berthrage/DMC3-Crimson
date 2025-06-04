#include "CrimsonBetterArkham2.hpp"

namespace CrimsonBetterArkham2 {
	static bool fightActive{ false };
	static bool fightEnding{ false };
	vec4 arkhamposition{
		.x = 0.0f,
		.y = 0.0f,
		.z = 0.0f,
		.a = 0.0f
	};
	ConfigCreateEnemyActorData arkham2data{
		.enemy = 53,
		.variant = 1,
		.position = arkhamposition,
		.rotation = 0,
		//to determine starting location
		.useMainActorData = true,
		.spawnMethod = 0
	};
	/// <summary>
	/// Sets the fight to not active when player hits continue on death screen.
	/// </summary>
	void Continue()
	{
		fightActive = false;
		fightEnding = false;
		return;
	};
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
			fightActive = false;
			fightEnding = false;
		return;
	}

	/// <summary>
	/// used to end fight properly
	/// </summary>
	void OnTick()
	{
		if (!InGame())
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

		



		// Loop through enemy data
		for (auto enemy : enemyVectorData.metadata) {
			if (!enemy.baseAddr) continue;
			auto& enemyData = *reinterpret_cast<EnemyActorData*>(enemy.baseAddr);
			if (!enemyData.baseAddr) continue;

			//for now, we'll use this to end the arkham fight
			if (enemyData.enemy == 53 && enemyData.hitPointsArkham < 1.0f && fightActive)
				CrimsonBetterArkham2::fightEnding = true;
		}


		auto eventFlags = reinterpret_cast<byte32*>(pool_19337[1]);
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
		if ((sessionData.mission == 19) && (nextEventData.room == 421) && activeCrimsonGameplay.Gameplay.ExtraDifficulty.betterArkham2) {
			if (eventFlags[20] == 1) {
				eventFlags[20] = 2;
				fightActive = true;
				fightEnding = false;
				//spawn our own arkham

				//configure actors here
				//this shouldn't need to be called, the skip is toggled off by default unless a scenechange needs it
				//CrimsonPatches::EndBossFight(false);

// 				CrimsonPatches::DisableDoorsInstancing(true);
// 				CrimsonDetours::ToggleHideAndMutePortals(true);
			}
			else if (eventFlags[20] == 2 && fightEnding) {
				eventFlags[20] = 1;
				fightActive = false;
				fightEnding = false;
				CrimsonPatches::EndBossFight(true);
			}
		}
		return;
	}

	void EventMain()
	{
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
		if (sessionData.mission == 19 && (nextEventData.room == 421) && activeCrimsonGameplay.Gameplay.ExtraDifficulty.betterArkham2 && eventFlags[20] == 2) {
			CreateEnemyActor(arkham2data, 0);
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

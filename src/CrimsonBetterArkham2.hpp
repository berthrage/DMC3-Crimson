#pragma once
#include "Core/Core.hpp"
#include "CrimsonGUI.hpp"
#include "CrimsonPatches.hpp"
#include "Actor.hpp"
#include "Config.hpp"
#include "Global.hpp"
#include "Vars.hpp"
namespace CrimsonBetterArkham2 {
	void Continue();
	void SetNextScreen(EventData& eventData);
	void OnTick();
	void SceneGame();
	void EventMain();
	void DebugGui();
};
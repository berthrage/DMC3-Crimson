#include "Core/Core.hpp"
#include "CrimsonGUI.hpp"
#include "CrimsonUtil.hpp"
#include "CrimsonPatches.hpp"
#include "CrimsonDetours.hpp"
#include "Actor.hpp"
#include "Arcade.hpp"
#include "Config.hpp"
#include "Global.hpp"
#include "Sound.hpp"
#include "Vars.hpp"


namespace CrimsonTrainingRoom {
	void SetRoom();
	void SetNextScreen(EventData& eventData);
	void SetContinueRoom();
	bool SetTrack(const char* filename);
	void DrawImGuiWidget();
	void MenuInteraction(uint32 index);
	void DrawRoomSelect();
	
	bool DrawShopWidget();
	bool isInTrainingRoom();
	void ToggleTrainingRoom(bool enable);
};
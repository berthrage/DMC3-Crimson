#include "CrimsonTrainingRoom.hpp"

//using this while we test implementation




//redundant variable
static bool trainingRoomEnabled{ true };


static bool fromMissionStart{ false };
static bool fromInGame{ false };
static bool queueReset{ false };
static uint16 room{ ROOM::DEBUG_ROOM_5 };
//Tracks whether we are in training currently
static bool inTrainingRoom{ false };
const char* enterString{ "Enter Void" };
const char* exitString{ "Exit Void" };
CrimsonPlayerData preTrainingRoomCrimsonPlayer[20];
MissionData preTrainingRoomMissionData;
static EventData backupData;

//we may have to move this stuff to a separate file if we want to use it in other mods. 
static constexpr auto MENU_ACTION_OFFSET() { return 0xCA8968; }

namespace CrimsonTrainingRoom {

    void SetRoom() {

        //if (!trainingRoomEnabled) {
        //    return;
        //}
        LogFunction();

        if (!inTrainingRoom)
            return;

        auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

        auto pool_208 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
        if (!pool_208 || !pool_208[12]) {
            return;
        }
        auto& nextEventData = *reinterpret_cast<NextEventData*>(pool_208[12]);


        if ((sessionData.mission >= 1) && (sessionData.mission <= 20) && !activeConfig.BossRush.enable && !activeConfig.Arcade.enable) {
            nextEventData.room = room;
        }
        
    }

    void SetContinueRoom() {
        //if (!trainingRoomEnabled) {
        //    return;
        //}

        LogFunction();

        if (!inTrainingRoom)
            return;

        auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

        auto pool_309 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
        if (!pool_309 || !pool_309[8]) {
            return;
        }
        auto& eventData = *reinterpret_cast<EventData*>(pool_309[8]);

        auto pool_340 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
        if (!pool_340 || !pool_340[12]) {
            return;
        }
        auto& nextEventData = *reinterpret_cast<NextEventData*>(pool_340[12]);

        auto pool_371 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E30);
        if (!pool_371 || !pool_371[1]) {
            return;
        }
        auto eventFlags = reinterpret_cast<byte32*>(pool_371[1]);


        nextEventData.room = room;
        nextEventData.position = static_cast<uint16>(1);
    }


    /// <summary>
/// Resets void stuff when:
/// restarts the mission
/// quits the mission
/// finishes the mission
/// game overs
/// </summary>
    void SetNextScreen(EventData& eventData) {
        if ((eventData.nextScreen == SCREEN::MISSION_CLEAR)
            || (eventData.nextScreen == SCREEN::GAME_OVER)
            || (eventData.nextScreen == SCREEN::MISSION_SELECT)
            || (eventData.nextScreen == SCREEN::MISSION_START)) {
            if (eventData.nextScreen == SCREEN::MISSION_SELECT && inTrainingRoom)
                eventData.nextScreen = SCREEN::MISSION_START;
            ToggleTrainingRoom(false);
            fromMissionStart = false;
        }

        return;
    }

    /// <summary>
    /// Clear music when loading into void
    /// </summary>
    /// <param name="filename"></param>
    /// <returns></returns>
    bool SetTrack(const char* filename) {
        if (!inTrainingRoom) {
            return true;
        }

        LogFunction();
        return false;
    }
    void MenuInteraction(uint32 index) {
        auto& menuData = *reinterpret_cast<PauseMenuMetadata**>(appBaseAddr + 0xCA8968);
        menuData->menuIndex = index;
    }
    void DrawRoomSelect() {
        //ImGui::PushItemWidth(itemWidth * 1.3f);

        if (UI::ComboMapValue("", roomNames, roomsMap, room, 0))
            queueReset = false;
        //ImGui::PopItemWidth();
        if (InPauseMenu()) {
            if (GUI_Button("Return to mission start")) {
                MenuInteraction(3);
            }
        }
        GUI_Checkbox("Reset Training", queueReset);
        if (queueReset)
        {
            if (!InGame()) {
                return;
            }

            queueReset = false;
            auto pool_12898 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
            if (!pool_12898 || !pool_12898[8]) {
                return;
            }
            auto& eventData = *reinterpret_cast<EventData*>(pool_12898[8]);

            auto pool_12959 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
            if (!pool_12959 || !pool_12959[12]) {
                return;
            }
            auto& nextEventData = *reinterpret_cast<NextEventData*>(pool_12959[12]);


            nextEventData.room = static_cast<uint16>(room);
            nextEventData.position = static_cast<uint16>(eventData.position);


            eventData.event = EVENT::TELEPORT;
        }
    }
    
    /// <summary>
    /// Called in shop window to provide a button to enter training room. returns true when void is entered and we need to close the shop menu.
    /// </summary>
    /// <returns></returns>
    bool DrawShopWidget()
    {
        bool returnval = false;
        auto buttontext = enterString;

        if (InGame()) {
            //this is the implementation for accessing the training room from a mid mission shop.
            if (inTrainingRoom) {
                buttontext = exitString;
            }
        }else if(g_scene == SCENE::MISSION_START){
            //this is the implementation for accessing the training room from the mission start shop.
            if (GUI_Button(buttontext)) {
                fromMissionStart = true;
                ToggleTrainingRoom(true);
                returnval = true;
            }
        }
        return returnval;
    }

    void ToggleTrainingRoom(bool enable){
        //by including the arcade mode enable, we ensure that when we exit training mode we don't accidentally turn off arcade mode features.
        Arcade::ToggleMissionStart(enable || activeConfig.Arcade.enable);
        Arcade::ToggleOrbSkip(enable || activeConfig.Arcade.enable);
        ToggleSkipCutscenes(enable || activeConfig.skipCutscenes);
        GUI_Checkbox2("Auto Spawn", activeConfig.enemyAutoSpawn, queuedConfig.enemyAutoSpawn);
        CrimsonPatches::DisableDoorsInstancing(enable);
        CrimsonDetours::ToggleHideAndMutePortals(enable);
        inTrainingRoom = enable;
    }

    //old menu option, contains implementation for in-game training room that will need to be reworked.
    void DrawImGuiWidget()
    {
        //GUI_Checkbox("turn on void - debug, variable might be removed", trainingRoomEnabled);
        auto buttontext = enterString;
        if (inTrainingRoom) {
            buttontext = exitString;
        }
        if (!InGame()) {
            if (g_scene == SCENE::MISSION_START) {
                //when pressing the button on the mission start screen, we want to be taken straight into the game. 
                //going to need an exception for m1/places where we don't want it to work obviously.

                //also we might need to check whether collecting Worbs in training room will break the Worb collection message in m8.
                if (GUI_Button(buttontext)) {
                    Arcade::ToggleMissionStart(true);
                    Arcade::ToggleOrbSkip(true);
                    ToggleSkipCutscenes(true);
                    inTrainingRoom = !inTrainingRoom;
                }
            }
            return;
        }

        //code here allows instantly entering the void from a specific room. 

        if (GUI_Button(buttontext)) {
            
            inTrainingRoom = !inTrainingRoom;


            auto pool_11962 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
            if (!pool_11962 || !pool_11962[8]) {
                return;
            }
            auto& eventData = *reinterpret_cast<EventData*>(pool_11962[8]);

            auto pool_340 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
            if (!pool_340 || !pool_340[12]) {
                return;
            }
            auto& nextEventData = *reinterpret_cast<NextEventData*>(pool_340[12]);

            auto missionDataPtr = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E30);
            if (!missionDataPtr) {
                return;
            }
            auto& missionData = *reinterpret_cast<MissionData*>(missionDataPtr);


            
            if (inTrainingRoom) {
                backupData = eventData;
                preTrainingRoomMissionData = missionData;
                Arcade::ToggleOrbSkip(true);
                
                nextEventData.room = ROOM::DEBUG_ROOM_5;
                nextEventData.position = 0;
                for (int playerIndex = 0; playerIndex < PLAYER_COUNT; ++playerIndex) {
                    CrimsonUtil::CopyCrimsonPlayerData(&crimsonPlayer[playerIndex], &preTrainingRoomCrimsonPlayer[playerIndex]);
                }

            }
            else {
                Arcade::ToggleOrbSkip(false);
                missionData = preTrainingRoomMissionData;
                nextEventData.room = backupData.room;
                nextEventData.position = backupData.position;
                for (int playerIndex = 0; playerIndex < PLAYER_COUNT; ++playerIndex) {
                    CrimsonUtil::CopyCrimsonPlayerData(&preTrainingRoomCrimsonPlayer[playerIndex], &crimsonPlayer[playerIndex]);
                }
            }
            eventData.event = EVENT::TELEPORT;
        }
    }

    bool isInTrainingRoom()
    {
        return inTrainingRoom;
    }
}

#include "CrimsonTrainingRoom.hpp"
//using this while we test implementation





static bool trainingRoomEnabled{ false };
static bool inTrainingRoom{ false };
const char* enterString{ "Enter Void" };
const char* exitString{ "Exit Void" };
CrimsonPlayerData preTrainingRoomCrimsonPlayer[20];
MissionData preTrainingRoomMissionData;
static EventData backupData;

namespace CrimsonTrainingRoom {
    void SetRoom() {

        if (!trainingRoomEnabled) {
            return;
        }



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
            nextEventData.room = ROOM::DEBUG_ROOM_5;
        }
    }

    void SetContinueRoom() {
        if (!trainingRoomEnabled) {
            return;
        }



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


        nextEventData.room = ROOM::DEBUG_ROOM_5;
        nextEventData.position = static_cast<uint16>(1);
    }

    void DrawImGuiWidget()
    {
        GUI_Checkbox("turn on void - debug, variable might be removed", trainingRoomEnabled);
        auto buttontext = enterString;
        if (inTrainingRoom) {
            buttontext = exitString;
        }
        if (!InGame())
            return;

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
                
                
                nextEventData.room = ROOM::DEBUG_ROOM_5;
                nextEventData.position = 0;
                for (int playerIndex = 0; playerIndex < PLAYER_COUNT; ++playerIndex) {
                    CrimsonUtil::CopyCrimsonPlayerData(&crimsonPlayer[playerIndex], &preTrainingRoomCrimsonPlayer[playerIndex]);
                }

            }
            else {

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

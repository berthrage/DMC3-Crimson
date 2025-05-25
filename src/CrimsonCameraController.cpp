#include "CrimsonCameraController.hpp"
#include "Utility/Detour.hpp"
#include "../Core/Core.hpp"
#include <memory>
#include <array>
#include "imgui.h"
#include <cassert>
#include "Vars.hpp"
#include "CrimsonConfig.hpp"
#include "CrimsonPatches.hpp"

static std::unique_ptr<Utility::Detour_t> cameraControllerConstructionHook;
static std::unique_ptr<Utility::Detour_t> cameraSwitchAccessHook;
static constexpr auto CTRL_PROC_OFFSET() { return 0x23EEF0; }
static constexpr auto CAM_SWITCH_OFFSET() { return 0x055880; }

static bool s_cameraEnable{ true };


//this variable is to be removed
static bool s_cameraEnableOld{ true };

static bool s_tpsException{ false };

namespace CrimsonCameraController {
	uint32 g_currentCameraIndex = 0;
	uint32 g_currentCameraType = 0;
	static CAMERA_UPDATE_TYPE s_updateStatus{ ON_THIRD_PERSON };
	/// <summary>
	/// The function that determines whether the third person camera should have an exception.
	/// Controls logic for ANY exception. (or it will, we'll worry about the boss stuff later).
	/// If you want to toggle TPS off for something, PUT THE LOGIC HERE. DON'T PUT IT ANYWHERE ELSE.
	/// ESPECIALLY DON'T TRY TO TURN OFF THE TPS CAMERA IN THE GUI.
	/// </summary>
	/// <returns>true if we should ignore forced TPS cam at time of function call</returns>
	bool CheckInternalException() {

		//get event & nextevent
		auto pool_10298 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
		if (!pool_10298 || !pool_10298[8]) {
			return false;
		}
		auto& eventData = *reinterpret_cast<EventData*>(pool_10298[8]);

		auto pool_12959 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
		if (!pool_12959 || !pool_12959[12]) {
			return false;
		}
		auto& nextEventData = *reinterpret_cast<NextEventData*>(pool_12959[12]);

		auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);



		bool exceptions = ((false


			//MISSION 3:
			//|| (eventData.room == ROOM::SLUM_66_AVENUE)
			//|| (eventData.room == ROOM::CHAMBER_OF_ECHOES
			//MISSION 4:
			//fun little angle when you enter this room for the first time
			|| (eventData.room == ROOM::SILENCE_STATUARY && CrimsonCameraController::g_currentCameraIndex == 1)
			//fucking gigapede
			//that said boss cam overrides elsewhere unless we ALWAYS want this one to be fixed.
			//|| (eventData.room == ROOM::GIANTWALKER_CHAMBER && sessionData.mission == 4)
			|| (eventData.room == ROOM::INCANDESCENT_SPACE && CrimsonCameraController::g_currentCameraIndex == 1)
			//MISSION 5:
			//MISSION 6:
			//MISSION 7:
			
			//Elevator
			|| (eventData.room == ROOM::CHAMBER_OF_ECHOES && CrimsonCameraController::g_currentCameraIndex == 26)
			|| (eventData.room == ROOM::CHAMBER_OF_ECHOES && CrimsonCameraController::g_currentCameraIndex == 23)

			//camera kinda cramped
			|| (sessionData.mission == 7 && eventData.room == ROOM::CURSED_SKULL_CHAMBER && CrimsonCameraController::g_currentCameraIndex == 0)
			//vergil approach
			|| (sessionData.mission == 7 && eventData.room == ROOM::MOONLIGHT_MILE)
			|| (sessionData.mission == 7 && eventData.room == ROOM::PEAK_OF_DARKNESS)
			//MISSION 8:
			//half covered up shop statue in leviathan, might prevent people missing it idk
			|| (eventData.room == ROOM::LEVIATHANS_INTESTINES && CrimsonCameraController::g_currentCameraIndex == 2)
			
			//pre-fight boss room.
			//Camera goes back to third person after moving around a bit once the bossfight starts! Kinda jarring but maybe ok?
			|| (eventData.room == ROOM::LEVIATHANS_HEARTCORE && CrimsonCameraController::g_currentCameraIndex == 0)
			
			//gigapede chase 1
			|| (eventData.room == ROOM::LEVIATHANS_INTESTINES_2)
			//gigapede chase 2
			|| (eventData.room == ROOM::LEVIATHANS_INTESTINES_5)

			//half covered up shop statue in leviathan, might prevent people missing it idk (dark mode)
			|| (eventData.room == ROOM::LEVIATHANS_INTESTINES_6 && CrimsonCameraController::g_currentCameraIndex == 2)
			//MISSION 9:
			//laser puzzle gets fixed cam in mission
			|| eventData.room == ROOM::SUBTERRANEAN_GARDEN && CrimsonCameraController::g_currentCameraIndex == 2)
			//lake room (camera highlights progression + free cam gets stuck on a wall trying to reach a secret area
			//Weird behavior entering lake from the caverns where entire room is fixed cam.
			// Might be due to the fact that the room is just loading in?
			|| (eventData.room == ROOM::SUBTERRANEAN_LAKE && CrimsonCameraController::g_currentCameraIndex == 0)
			|| (eventData.room == ROOM::SUBTERRANEAN_LAKE && CrimsonCameraController::g_currentCameraIndex == 5)
			|| (eventData.room == ROOM::SUBTERRANEAN_LAKE && CrimsonCameraController::g_currentCameraIndex == 11)
			//added to make some platforming less goofy
			|| (eventData.room == ROOM::SUBTERRANEAN_LAKE && CrimsonCameraController::g_currentCameraIndex == 2)
			
			//nevan?
			// || evaluateRoomCameraException(sessionData, eventData, nextEventData, ROOM::SUNKEN_OPERA_HOUSE, 9, 0)
			//MISSION 10:
			//this should only happen in position 2 on the other side of the cave where you collect the m10 mask
			|| (eventData.room == ROOM::LIMESTONE_CAVERN && CrimsonCameraController::g_currentCameraIndex == 6)
			|| (eventData.room == ROOM::LIMESTONE_CAVERN && CrimsonCameraController::g_currentCameraIndex == 8)

			//MISSION 11:
			// This room has a weird puzzle. 
			//|| (eventData.room == ROOM::ALTAR_OF_EVIL)
			//room edges.
			|| (eventData.room == ROOM::ALTAR_OF_EVIL && CrimsonCameraController::g_currentCameraIndex == 7)
			|| (eventData.room == ROOM::ALTAR_OF_EVIL && CrimsonCameraController::g_currentCameraIndex == 8)
			//hey break this wall dumbass
			|| (sessionData.mission == 11 && eventData.room == ROOM::ALTAR_OF_EVIL && CrimsonCameraController::g_currentCameraIndex == 16)
			//Platform 1 of the train with the agni rudra adjudicator.
			//|| (eventData.room == ROOM::TEMPERANCE_WAGON_2)

			//I love hifi rush!
			|| (eventData.room == ROOM::TEMPERANCE_WAGON)
			//Beowulf approach (but only for m11 because you need to leave in a hurry)
			|| (sessionData.mission == 11 && eventData.room == ROOM::TEMPERANCE_WAGON_3 && CrimsonCameraController::g_currentCameraIndex == 0)
			|| (sessionData.mission == 11 && eventData.room == ROOM::TEMPERANCE_WAGON_3 && CrimsonCameraController::g_currentCameraIndex == 2)
			//MISSION 12:
			// 218 spiral corridor camera index 0
			|| (eventData.room == ROOM::SPIRAL_CORRIDOR && CrimsonCameraController::g_currentCameraIndex == 0)
			//MISSION 13:
			//230 spiral staircase camera index 0
			|| (eventData.room == ROOM::SPIRAL_STAIRCASE && CrimsonCameraController::g_currentCameraIndex == 0)
			//233 laser puzzle camera index 0
			//but only for this mission bc we come back here later
			|| (sessionData.mission == 13 && eventData.room == ROOM::VESTIBULE && CrimsonCameraController::g_currentCameraIndex == 0)
			//hidden room
			|| (sessionData.mission == 13 && eventData.room == ROOM::VESTIBULE && CrimsonCameraController::g_currentCameraIndex == 2)
			//MISSION 14:
			//back to the lake, we might want to indicate you can go behind the elevator so you can pick up spiral.
			|| (eventData.room == ROOM::SUBTERRAN_LAKE && CrimsonCameraController::g_currentCameraIndex == 0)
			//its honestly harder to see in this room with TPS.
			|| (eventData.room == ROOM::TOP_OBSIDIAN_PATH)
			
			//|| (eventData.room == ROOM::SUBTERRAN_LAKE && CrimsonCameraController::g_currentCameraIndex == 0)
			//MISSION 15:
			//there's one of those red orb crystals up there and you'll never see it unless the camera indicates it.
			|| (eventData.room == ROOM::FORBIDDEN_LAND_FRONT && CrimsonCameraController::g_currentCameraIndex == 0)
			
			//whoever decided you needed to go through this room sideways should go to hell
			|| (eventData.room == ROOM::ROUNDED_PATHWAY_4 && CrimsonCameraController::g_currentCameraIndex == 2)
			|| (eventData.room == ROOM::ROUNDED_PATHWAY_4 && CrimsonCameraController::g_currentCameraIndex == 4)
			//cool view
			|| (eventData.room == ROOM::TOP_SUBTERRIA_LACK && CrimsonCameraController::g_currentCameraIndex == 1)
			//thing you can get stuck on again
			|| (eventData.room == ROOM::TOP_SUBTERRIA_LACK && CrimsonCameraController::g_currentCameraIndex == 0)
			//blorb hint
			|| (sessionData.mission == 15 && eventData.room == ROOM::ALTAR_OF_EVIL_PATHWAY && CrimsonCameraController::g_currentCameraIndex == 0)
			|| (sessionData.mission == 15 && eventData.room == ROOM::ALTAR_OF_EVIL_PATHWAY && CrimsonCameraController::g_currentCameraIndex == 2)
			//elevator ride up
			|| (sessionData.mission == 15 && eventData.room == ROOM::DEVILSPROUT_LIFT && CrimsonCameraController::g_currentCameraIndex == 2)
			//MISSION 16:
			//lmao I got nothin
			//MISSION 17:
			//cool view at start
			|| (eventData.room == ROOM::THE_DARK_CORRIDOR_2 && CrimsonCameraController::g_currentCameraIndex == 2)
			|| (eventData.room == ROOM::TRIAL_OF_SKILL_2 && CrimsonCameraController::g_currentCameraIndex == 1)
			//jump pad outside tower
			|| (eventData.room == ROOM::PITCH_BLACK_VOID_2 && CrimsonCameraController::g_currentCameraIndex == 1)
			//moonlight mile happens again
			|| (sessionData.mission == 17 && eventData.room == ROOM::MOONLIGHT_MILE)
			|| (sessionData.mission == 17 && eventData.room == ROOM::DARK_PACT_CHAMBER)
			//doppelganger fight
			|| (sessionData.mission == 17 && eventData.room == ROOM::APPARITION_INCARNATE)
			
			//MISSION 18:
			//intro room has some cool cameras
			|| (sessionData.mission == 18 && eventData.room == ROOM::UNSACRED_HELLGATE)
			//feet (cam 0 only, bc we have to fight fallen in this room and we don't need to make that harder than it already is)
			|| (sessionData.mission == 18 && eventData.room == ROOM::ROAD_TO_DESPAIR && CrimsonCameraController::g_currentCameraIndex == 0)
			
			//OK. So in all seriousness. We could *try* and do something where we make each individual camera in the room a player tracker 
			// and see if it will orient properly, but like
			// it'd defeat the purpose of this room.
			//anyway exception given to the boss rush room because the camera is like that.
			|| (eventData.room == ROOM::LOST_SOULS_NIRVANA)
			//I am gigapeding out
			//|| (sessionData.mission == 18 && eventData.room == ROOM::GIANTWALKER_REBORN && CrimsonCameraController::g_currentCameraIndex == 0)
			//MISSION 19:
			//approach to arkham
			|| (eventData.room == ROOM::END_OF_THE_LINE)
			//MISSION 20:
			//It's literally the vergil fight
			);
		return exceptions;
	}

	/// <summary>
	/// This is basically a glorified wrapper for simplifying the camera change logic.
	/// It uses 3 variables:
	/// activeCrimsonConfig.Camera.forceThirdPerson, the state of the TPS mod in gui
	/// s_cameraEnable, whether this file is currently forcing the TPS camera
	/// and s_tpsException, whether something in game is overriding the TPS preferences to make the camera fixed
	/// 
	/// and returns an enum value which corresponds to 1 of 4 behaviors in the sub_140055880 function.
	/// 
	/// THIS LOGIC IS NOT WHERE YOU SHOULD EDIT SPECIAL CONDITIONS FOR OVERRIDING TPS. 
	/// THOSE SHOULD BE DONE IN CheckInternalException function above.
	/// Thank you. -Hitch
	/// </summary>
	/// <returns></returns>
	CAMERA_UPDATE_TYPE UpdateCrimsonCameraState() {

		//get the TPS camera gui state
		auto& cameraConfig = activeCrimsonConfig.Camera;

		//if TPS is off, we're either turning the camera off or it's already off.
		if (!cameraConfig.forceThirdPerson){

			if (!s_cameraEnable) {
				return CAMERA_UPDATE_TYPE::ON_FIXED;
			}

			//tps just turned off in options
			if (s_cameraEnable) {
				s_cameraEnable = false;
				return CAMERA_UPDATE_TYPE::THIRD_PERSON_TO_FIXED;
			}
		}

		//from this point, we are dealing with scenarios where cameraConfig.forceThirdPerson is true
		else {
			//this variable will have a value of true whenever we want to override third person camera without turning the mod off.
			s_tpsException = CheckInternalException();
			//if internal camera is enabled at this point, we know that we're either maintaining TPS or about to turn it off bc of exception. 
			if (s_cameraEnable) {
				//turning off bc of exception
				if (s_tpsException) {
					s_cameraEnable = false;
					return CAMERA_UPDATE_TYPE::THIRD_PERSON_TO_FIXED;
				}
				//maintaining tps camera on
				else {
					return CAMERA_UPDATE_TYPE::ON_THIRD_PERSON;
				}
			}
			//if internal camera disabled, we're either keeping it off due to an exception or about to turn it on. 
			else {
				//keeping TPS off due to exception
				if (s_tpsException) {
					return CAMERA_UPDATE_TYPE::ON_FIXED;
				}

				//no exception, about to turn on third person camera again.
				else {
					s_cameraEnable = true;
					return CAMERA_UPDATE_TYPE::FIXED_TO_THIRD_PERSON;
				}
			}
		}

		Log("Third Person Camera UpdateCrimsonCameraState() function broke horribly! invalid return!");
		//THIS SHOULD NEVER CALL
		return CAMERA_UPDATE_TYPE::ON_FIXED;
	}



}



uint32 SUBTERRANEAN_LAKECamNum = 13;
/// <summary>
/// A detour that occurs on the Camera change function.
/// This function is called when the game wants to update the current camera being used.
/// By adjusting the properties of CameraSwitchArrayData, we can change how cameras in the room behave,
/// or outright prevent the game from using any of them and forcing the game to use the default third person camera.
/// </summary>
/// <param name="cameraSwitchArrayDataAddress">ingame address of the CameraSwitchArrayData data type.</param>
/// <param name="a2">unknown</param>
/// <returns></returns>
static uintptr_t __fastcall sub_140055880(int64_t cameraSwitchArrayDataAddress, char a2) {
	typedef int64_t(__fastcall* sub_140055880)(int64_t, char);
	auto& cameraConfig = activeCrimsonConfig.Camera;

	uintptr_t trampoline_raw = cameraSwitchAccessHook->GetTrampoline();
	sub_140055880 trampoline = (sub_140055880)trampoline_raw;

	uintptr_t res; //will store function result when we calculate it

	//getting the camera switch info
	auto& cameraswitchInfo = *reinterpret_cast<CameraSwitchArrayData*>(cameraSwitchArrayDataAddress);

	//store the current index and type for the debug cam.
	CrimsonCameraController::g_currentCameraIndex = cameraswitchInfo.currentCamIndex;
	//only access the camera type if we have a valid camera index.
	if (CrimsonCameraController::g_currentCameraIndex != 255) {
		//we need to multiply the index by 4 to get the actual camera. 
		CrimsonCameraController::g_currentCameraType = cameraswitchInfo.switches[cameraswitchInfo.currentCamIndex * 4]->type;
	}
	//if camera index is 255 then perform vanilla behavior and bail.
	else {
		res = trampoline(cameraSwitchArrayDataAddress, a2);
		return res;
	}

	//Past this point, the camera is trying to change to valid camera.
	//That unfortunately means it's now our problem.

	/*
	Explaining what I'm doing here.
	The idea is simple: siyan figured out we could force the first person cam by saying that every attempt to transition to fixed cam was invalid.
	This worked but broke when trying to re-enable freecam. Once the game had fround a valid fixed cam, 
	it would no longer default to the third person camera it used when it couldn't find anything else.
	However, it wouldn't transition to any new fixed cameras either, so it would just be stuck on the current camera as you moved further and further away.
	To resolve this, we can access the new cam the game is trying to switch to, and make that a first person camera. 
	This works, but now we've permanently made that camera third person instead of fixed. Not good.
	So, let's try changing the camera type, running the camera switch function, then changing the camera type back
	*/


	//first we evaluate what behavior we want.

	CrimsonCameraController::s_updateStatus = CrimsonCameraController::UpdateCrimsonCameraState();

	//Then we perform one of four behaviors. 
	//First 2 should just be vanilla behavior,
	//third one makes the camera index invalid so the controller can't try swapping to a new camera, 
	//the fourth one is the complicated one and restores TPS mid room.
	switch (CrimsonCameraController::s_updateStatus) {
		//vanilla behavior, no changes required.
		case CrimsonCameraController::CAMERA_UPDATE_TYPE::ON_FIXED:
			res = trampoline(cameraSwitchArrayDataAddress, a2);
			break;

		//vanilla behavior, no changes required.
		case CrimsonCameraController::CAMERA_UPDATE_TYPE::THIRD_PERSON_TO_FIXED:
			res = trampoline(cameraSwitchArrayDataAddress, a2);
			break;

		//This scenario is used when TPS is already on. We just maintain the invalid camera pointer like so.
		case CrimsonCameraController::CAMERA_UPDATE_TYPE::ON_THIRD_PERSON:
			cameraswitchInfo.currentCamIndex = 255;
			res = trampoline(cameraSwitchArrayDataAddress, a2);
			break;

		//if camera was just turned on and there's a valid camera ID we're trying to swap to,
		case CrimsonCameraController::CAMERA_UPDATE_TYPE::FIXED_TO_THIRD_PERSON:
			//we make the new camera we're switching to third person.
			//Note: we already backed up the current camera type to the corresponding global variable, so we don't need to back it up here.

			cameraswitchInfo.switches[cameraswitchInfo.currentCamIndex * 4]->type = CAMERA_TYPE::THIRD_PERSON;
			//we now finally run the function
			res = trampoline(cameraSwitchArrayDataAddress, a2);
			//now that we've run the function, we swap back to the old camera type
			cameraswitchInfo.switches[cameraswitchInfo.currentCamIndex * 4]->type = CrimsonCameraController::g_currentCameraType;
			break;

		//should literally never call
		default:
			Log("Third Person Camera sub_140055880 function broke horribly! invalid case state!");
			res = trampoline(cameraSwitchArrayDataAddress, a2);
			break;
	};

	//We're currently running into an issue where dante's direction doesn't update as the camera turns. I'm not sure why, so we're going to try and restore camera index
	//see if that helps
	cameraswitchInfo.currentCamIndex = CrimsonCameraController::g_currentCameraIndex;

	//return the result of the run function, and boom! Dynamic camera swap between TPS and Fixed.
	return res;
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
	//technically all we need here is the nextEvent info. Make sure it matches and we're good.
	bool nextroom = (sessionData.mission == mission && nextEventData.room == room && nextEventData.position == position);
	return nextroom;
}

/// <summary>
/// This function is in the process of being phased out, it used to toggle tps patch based on room exceptions, 
/// but that logic is being moved to the detour above which is per camera.
/// </summary>
/// <param name="a1"></param>
/// <returns></returns>
static uintptr_t  __fastcall sub_14023EEF0(int64_t a1) {
	//new addition, only for scenario where the first part of this patch is removed
	//CrimsonPatches::ForceThirdPersonCamera(true);	
	typedef int64_t(__fastcall* sub_14023EEF0)(int64_t);
	uintptr_t trampoline_raw = cameraControllerConstructionHook->GetTrampoline();
	sub_14023EEF0 trampoline = (sub_14023EEF0)trampoline_raw;

	uintptr_t res = trampoline(a1);
	return res;
}

/// <summary>
/// Creates a hook I hope
/// </summary>
void CameraCtrlInitDetour() {
	cameraControllerConstructionHook =
		std::make_unique<Utility::Detour_t>(
			(uintptr_t)appBaseAddr + CTRL_PROC_OFFSET(),
			(uintptr_t)&sub_14023EEF0,
			NULL, "camera_ctr_detour");
	bool res = cameraControllerConstructionHook->Toggle();
	assert(res);
}

void CameraSwitchInitDetour() {
	cameraSwitchAccessHook =
		std::make_unique<Utility::Detour_t>(
			(uintptr_t)appBaseAddr + CAM_SWITCH_OFFSET(),
			(uintptr_t)&sub_140055880,
			NULL, "camera_switch_detour");
	bool res = cameraSwitchAccessHook->Toggle();
	assert(res);
}
#include "CrimsonCameraController.hpp"
#include "Utility/Detour.hpp"
#include "../Core/Core.hpp"
#include <memory>
#include <array>
#include "imgui.h"
#include <cassert>
#include "Vars.hpp"
#include "CrimsonConfig.hpp"

static std::unique_ptr<Utility::Detour_t> cameraControllerConstructionHook;
static std::unique_ptr<Utility::Detour_t> cameraSwitchAccessHook;
static constexpr auto CTRL_PROC_OFFSET() { return 0x23EEF0; }
static constexpr auto CAM_SWITCH_OFFSET() { return 0x055880; }
static bool s_cameraEnable{ true };

static uintptr_t __fastcall sub_140055880(int64_t a1, char a2) {
	typedef int64_t(__fastcall* sub_140055880)(int64_t,char);
	auto& cameraConfig = activeCrimsonConfig.Camera;

	uintptr_t trampoline_raw = cameraSwitchAccessHook->GetTrampoline();
	sub_140055880 trampoline = (sub_140055880)trampoline_raw;

	s_cameraEnable = cameraConfig.forceThirdPerson;

	uintptr_t res = trampoline(a1,a2);
	if (!res || !s_cameraEnable) {
		return res;
	}

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

static uintptr_t  __fastcall sub_14023EEF0(int64_t a1) {

	typedef int64_t(__fastcall* sub_14023EEF0)(int64_t);
	//typedef uintptr_t(__fastcall* spawGuyTrampoline)(uintptr_t, float*);
	//auto& ldkModeConfig = activeCrimsonGameplay.Gameplay.ExtraDifficulty.ldkMode;
	auto& cameraConfig = activeCrimsonConfig.Camera;

	uintptr_t trampoline_raw = cameraControllerConstructionHook->GetTrampoline();
	sub_14023EEF0 trampoline = (sub_14023EEF0)trampoline_raw;

	s_cameraEnable = cameraConfig.forceThirdPerson;


	uintptr_t res = trampoline(a1);
	if (!res || !s_cameraEnable) {
		return res;
	}



	//get event & nextevent
	auto pool_10298 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
	if (!pool_10298 || !pool_10298[8]) {
		return res;
	}
	auto& eventData = *reinterpret_cast<EventData*>(pool_10298[8]);

	auto pool_12959 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
	if (!pool_12959 || !pool_12959[12]) {
		return res;
	}
	auto& nextEventData = *reinterpret_cast<NextEventData*>(pool_12959[12]);

	auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

	bool roomExceptions = (
		(eventData.room == ROOM::LOST_SOULS_NIRVANA && eventData.event != EVENT::TELEPORT)
		//Vergil approach 
		|| evaluateRoomCameraException(sessionData, eventData, nextEventData, ROOM::PEAK_OF_DARKNESS, 7, 0)
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
	if (roomExceptions) {
		CrimsonPatches::ForceThirdPersonCamera(false);
	}
	else {
		CrimsonPatches::ForceThirdPersonCamera(true);

	}

	//custom code
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
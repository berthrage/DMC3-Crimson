#pragma once
#include "Core/Core.hpp"

void CameraCtrlInitDetour();
void CameraSwitchInitDetour();
void CameraWallCheckDetour();
void CameraSweepEyeDetour();
namespace CrimsonCameraController{
	extern uint32 g_currentCameraIndex;
	extern uint32 g_currentCameraType;
	struct WallPullInReadout {
		bool active = false;
		float wantedDistance = 0.0f;
		float hitDistance = -1.0f;
		float resolvedDistance = 0.0f;
	};
	extern WallPullInReadout g_wallPullIn;
	// Around UpdateCameraOrbit_sub_140057520: the orbit has to start from the eye the camera would have without walls.
	bool OrbitBegin(int64_t camera);
	void OrbitEnd(int64_t camera, bool restored);
	enum CAMERA_UPDATE_TYPE {
		ON_FIXED,
		ON_THIRD_PERSON,
		FIXED_TO_THIRD_PERSON,
		THIRD_PERSON_TO_FIXED
	};
};
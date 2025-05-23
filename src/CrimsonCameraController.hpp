#pragma once
#include "Core/Core.hpp"

void CameraCtrlInitDetour();
void CameraSwitchInitDetour();
namespace CrimsonCameraController{
	extern uint32 g_currentCameraIndex;
	extern uint32 g_currentCameraType;
	enum CAMERA_UPDATE_TYPE {
		ON_FIXED,
		ON_THIRD_PERSON,
		FIXED_TO_THIRD_PERSON,
		THIRD_PERSON_TO_FIXED
	};
};
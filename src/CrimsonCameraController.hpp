#pragma once
#include "Core/Core.hpp"

void CameraCtrlInitDetour();
void CameraSwitchInitDetour();
namespace CrimsonCameraController{
	extern uint32 g_currentCameraIndex;
	extern uint32 g_currentCameraType;
};
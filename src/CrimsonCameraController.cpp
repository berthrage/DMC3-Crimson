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
static constexpr auto CTRL_PROC_OFFSET() { return 0x23EEF0; }
static bool s_cameraEnable{ true };

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

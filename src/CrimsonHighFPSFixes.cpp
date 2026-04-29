#include "../ThirdParty/glm/glm.hpp"
#include <algorithm>
#include "Core/Core.hpp"
#include <stdio.h>
#include "Utility/Detour.hpp"
#include <intrin.h>
#include <string>
#include "CrimsonUtil.hpp"
#include "DMC3Input.hpp"
#include "Global.hpp"

#include "Core/Macros.h"
#include "Config.hpp"
#include <iostream>
#include "CrimsonPatches.hpp"
#include "CrimsonLDK.hpp"
#include "Actor.hpp"
#include "CrimsonHighFPSFixes.hpp"

namespace CrimsonHighFPSFixes {

extern "C" {
	// FixSecretMissionTimerFPS
// 	std::uint64_t g_FixSecretMissionTimerFPS_ReturnAddr;
// 	void FixSecretMissionTimerFPSDetour();
// 	std::uint64_t g_FixSecretMissionTimerFPS_ReturnAddr2;
// 	void FixSecretMissionTimerFPSDetour2();

	// FixBlendingEffects
	std::uint64_t g_FixBlendingEffects_Mist_ReturnAddr1;
	std::uint64_t g_FixBlendingEffects_Mist_ReturnAddr2;
	std::uint64_t g_FixBlendingEffects_Mist_ReturnAddr3;
	std::uint64_t g_MistBlendingEffect_SpdAddr1;
	std::uint64_t g_MistBlendingEffect_SpdAddr2;
	void FixBlendingEffect_Mist_Detour1();
	void FixBlendingEffect_Mist_Detour2();
	void FixBlendingEffect_Mist_Detour3();

	std::uint64_t g_FixBlendingEffects_Warp_ReturnAddr1;
	std::uint64_t g_FixBlendingEffects_Warp_ReturnAddr2;
	void FixBlendingEffect_Warp_Detour1();
	void FixBlendingEffect_Warp_Detour2();
}

void BlendingEffectsSpeedFixes(bool enable) {
	using namespace Utility;

	static bool run = false;
	if (run == enable) {
		return;
	}

	// From UpdateBlendEffect_Mist_sub_14031AE10:
	// dmc3.exe + 31AF16 - F3 0F 10 0D 8A CD 1E 00 - movss xmm1, [dmc3.exe + 507CA8] { (0.00) }
	static std::unique_ptr<Utility::Detour_t> FixMistHook1 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x31AF16, &FixBlendingEffect_Mist_Detour1, 8);
	g_FixBlendingEffects_Mist_ReturnAddr1 = FixMistHook1->GetReturnAddress();
	g_MistBlendingEffect_SpdAddr1 = (uintptr_t)appBaseAddr + 0x507CA8;
	FixMistHook1->Toggle(enable);

	// dmc3.exe+31AF0E - F3 0F 59 05 96 CD 1E 00   - mulss xmm0,[dmc3.exe+507CAC] { (0.00) }
	static std::unique_ptr<Utility::Detour_t> FixMistHook2 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x31AF0E, &FixBlendingEffect_Mist_Detour2, 8);
	g_FixBlendingEffects_Mist_ReturnAddr2 = FixMistHook2->GetReturnAddress();
	g_MistBlendingEffect_SpdAddr2 = (uintptr_t)appBaseAddr + 0x507CAC;
	FixMistHook2->Toggle(enable);

	// dmc3.exe+31AF1E - F3 0F 59 35 86 CD 1E 00   - mulss xmm6,[dmc3.exe+507CAC] { (0.00) }
	static std::unique_ptr<Utility::Detour_t> FixMistHook3 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x31AF1E, &FixBlendingEffect_Mist_Detour3, 8);
	g_FixBlendingEffects_Mist_ReturnAddr3 = FixMistHook3->GetReturnAddress();
	FixMistHook3->Toggle(enable);

	// From UpdateBlendEffect_Warp_sub_14031A830:
	// dmc3.exe+31A85A - 8B 46 20              - mov eax,[rsi+20]
	// dmc3.exe+31A85D - 8B D3                 - mov edx,ebx
	static std::unique_ptr<Utility::Detour_t> FixWarpHook1 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x31A85A, &FixBlendingEffect_Warp_Detour1, 5);
	g_FixBlendingEffects_Warp_ReturnAddr1 = FixWarpHook1->GetReturnAddress();
	FixWarpHook1->Toggle(enable);

	// From UpdateBlendEffect_Warp_sub_14031A830:
	// dmc3.exe+31A862 - 8B 46 24              - mov eax,[rsi+24]
	// dmc3.exe+31A865 - 01 46 74              - add [rsi+74],eax
	static std::unique_ptr<Utility::Detour_t> FixWarpHook2 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x31A862, &FixBlendingEffect_Warp_Detour2, 6);
	g_FixBlendingEffects_Warp_ReturnAddr2 = FixWarpHook2->GetReturnAddress();
	FixWarpHook2->Toggle(enable);

	run = enable;
}


void ToggleAllFixes(bool enable) {
	BlendingEffectsSpeedFixes(enable);
}


}
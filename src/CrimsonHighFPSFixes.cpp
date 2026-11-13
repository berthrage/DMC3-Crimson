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
#include "CrimsonMenuAnimation.hpp"

namespace CrimsonHighFPSFixes {

extern "C" {
	// FixSecretMissionTimerFPS
// 	std::uint64_t g_FixSecretMissionTimerFPS_ReturnAddr;
// 	void FixSecretMissionTimerFPSDetour();
// 	std::uint64_t g_FixSecretMissionTimerFPS_ReturnAddr2;
// 	void FixSecretMissionTimerFPSDetour2();

	// FixBlendingEffects -- Fixes the Speed which some Blending Effects play out.
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


	// FixClothPhysics -- Fixes Cloth Physics stiffening up at higher fps.
	void FixClothPhysicsDetour1();
	void FixClothPhysicsDetour2();
	void FixClothPhysicsDetour3();
	void FixClothPhysicsDetour4();
	void FixClothPhysicsDetour5();
	void FixClothPhysicsDetour6();
	void FixClothPhysicsDetour7();
	void FixClothPhysicsDetour8();
	void FixClothPhysicsDetour9();
	void FixClothPhysicsDetour10();
	void FixClothPhysicsDetour11();
	void FixClothPhysicsDetour12();
	void FixClothPhysicsDetour13();
	void FixClothPhysicsDetour14();
	void FixClothPhysicsDetour15();
	
	std::uint64_t g_FixClothPhysics_ReturnAddr1;
	std::uint64_t g_FixClothPhysics_ReturnAddr2;
	std::uint64_t g_FixClothPhysics_ReturnAddr3;
	std::uint64_t g_FixClothPhysics_ReturnAddr4;
	std::uint64_t g_FixClothPhysics_ReturnAddr5;
	std::uint64_t g_FixClothPhysics_ReturnAddr6;
	std::uint64_t g_FixClothPhysics_ReturnAddr7;
	std::uint64_t g_FixClothPhysics_ReturnAddr8;
	std::uint64_t g_FixClothPhysics_ReturnAddr9;
	std::uint64_t g_FixClothPhysics_ReturnAddr10;
	std::uint64_t g_FixClothPhysics_ReturnAddr11;
	std::uint64_t g_FixClothPhysics_ReturnAddr12;
	std::uint64_t g_FixClothPhysics_ReturnAddr13;
	std::uint64_t g_FixClothPhysics_ReturnAddr14;
	std::uint64_t g_FixClothPhysics_ReturnAddr15;

	std::uint64_t g_FixClothPhysics_Detour11_FloatConstAddr;
	std::uint64_t g_FixClothPhysics_RigidnessDanteCoatCall;

	// Flag read by Detour11 to conditionally skip velocity computation
	uint32_t g_ClothPhysicsEnhancementEnabled = 1;

	// FixBossCam
	void FixBossCamLookAtDetour(); // Fixes BossCam FollowUp Speed.
	std::uint64_t g_FixBossCamLookAt_ReturnAddr;

	void FixBossCamRefocusDetour1(); 
	std::uint64_t g_FixBossCamRefocus_ReturnAddr1;

	void BossCamFOVDetour();
	std::uint64_t g_BossCamFOV_ReturnAddr;
	std::uint64_t g_BossCamFOV_ConstAddr;

	void BossCamCustomPositioningDetour();
	std::uint64_t g_BossCamCustomPositioning_ReturnAddr;
	bool* g_isMPCamActiveAddr = nullptr;

	// FixStaggerGravityInertia -- Fixes Y inertia behaving weirdly when you get Staggered.
	void FixStaggerGravityDetour();
	std::uint64_t g_FixStaggerGravity_ReturnAddr;

	// FixCStageSetGateSpawn -- Fixes Mirror Gates taking way too long to fade in.
	void FixCStageSetGateSpawnDetour1();
	void FixCStageSetGateSpawnDetour2();
	void FixCStageSetGateSpawnDetour3();
	void FixCStageSetGateSpawnDetour4();
	std::uint64_t g_FixCStageSetGateSpawn_ReturnAddr1;
	std::uint64_t g_FixCStageSetGateSpawn_ReturnAddr2;
	std::uint64_t g_FixCStageSetGateSpawn_ReturnAddr3;
	std::uint64_t g_FixCStageSetGateSpawn_ReturnAddr4;

	// FixEnemyAttackCooldowns
	std::uint64_t g_FixEnemyAttackCooldowns_ReturnAddr1;
	std::uint64_t g_FixEnemyAttackCooldowns_ReturnAddr2;
	void FixEnemyAttackCooldownsDetour1();
	void FixEnemyAttackCooldownsDetour2();

	// FixBloodgoyleStormFormTime
	std::uint64_t g_FixBloodgoyleStormFormTime_ReturnAddr;
	void FixBloodgoyleStormFormTimeDetour();

	// FixCStaffRollCredits
	std::uint64_t g_CStaffRollCreditsFix_ReturnAddr1;
	std::uint64_t g_CStaffRollCreditsFix_ReturnAddr2;
	void CStaffRollCreditsFixDetour1();
	void CStaffRollCreditsFixDetour2();

	// FixCStageSetSeal
	std::uint64_t g_FixCStageSetSeal_ReturnAddr1;
	std::uint64_t g_FixCStageSetSeal_ReturnAddr2;
	std::uint64_t g_FixCStageSetSeal_ReturnAddr3;
	void FixCStageSetSealDetour1();
	void FixCStageSetSealDetour2();
	void FixCStageSetSealDetour3();

	// FixSummonedSwordsInitialTravel
	std::uint64_t g_FixSummonedSwordsInitialTravel_ReturnAddr;
	std::uint64_t g_FixSummonedSwordsInitialTravel_ReturnAddr2;
	std::uint64_t g_FixSummonedSwordsInitialTravel_ReturnAddr3;
	void FixSummonedSwordsInitialTravelDetour();
	void FixSummonedSwordsInitialTravelDetour2();
	void FixSummonedSwordsInitialTravelDetour3();

	// FixSecretMissionTimerFPS
	std::uint64_t g_FixSecretMissionTimerFPS_ReturnAddr;
	void FixSecretMissionTimerFPSDetour();
	std::uint64_t g_FixSecretMissionTimerFPS_ReturnAddr2;
	void FixSecretMissionTimerFPSDetour2();

	// FixWpNunchakuCerberusCollisions
	std::uint64_t g_FixWpNunchakuCerberusCollisions_ReturnAddr;
	void FixWpNunchakuCerberusCollisionsDetour();
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

void ClothPhysicsFixes(bool enable) {
	using namespace Utility;

	static bool run = false;
	if (run == enable) {
		return;
	}

	// All detours from ClothPhysicsUpdate_sub_1402C9450:

	// dmc3.exe+2C9CDB - F3 45 0F 59 C1         - mulss xmm8,xmm9 -- try7 - 4
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook1 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2C9CDB, &FixClothPhysicsDetour1, 5);
	g_FixClothPhysics_ReturnAddr1 = ClothPhysicsHook1->GetReturnAddress();
	ClothPhysicsHook1->Toggle(enable);

	// dmc3.exe+2C9572 - F3 0F 59 96 58 02 00 00   - mulss xmm2,[rsi+00000258] { wind simulation } -- try7 - 8
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook2 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2C9572, &FixClothPhysicsDetour2, 8);
	g_FixClothPhysics_ReturnAddr2 = ClothPhysicsHook2->GetReturnAddress();
	ClothPhysicsHook2->Toggle(enable);

	// dmc3.exe+2C96FA - F3 0F 5C C3              - subss xmm0,xmm3 { very important, cloth deformation update } -- try7 - 14
	// dmc3.exe+2C96FE - 4C 89 A4 24 50 02 00 00  - mov [rsp+00000250],r12
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook3 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2C96FA, &FixClothPhysicsDetour3, 12);
	g_FixClothPhysics_ReturnAddr3 = ClothPhysicsHook3->GetReturnAddress();
	ClothPhysicsHook3->Toggle(enable);

	// dmc3.exe+2C9985 - F3 41 0F10 8E A4000000  - movss xmm1,[r14+000000A4] -- try7 - 19
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook4 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2C98A2, &FixClothPhysicsDetour4, 8);
	g_FixClothPhysics_ReturnAddr4 = ClothPhysicsHook4->GetReturnAddress();
	ClothPhysicsHook4->Toggle(enable);

	// dmc3.exe+2C9993 - F3 41 0F 10 86 A8 00 00 00  - movss xmm0,[r14+000000A8] -- try7 - 20
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook5 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2C9993, &FixClothPhysicsDetour5, 9);
	g_FixClothPhysics_ReturnAddr5 = ClothPhysicsHook5->GetReturnAddress();
	ClothPhysicsHook5->Toggle(enable);

	// dmc3.exe+2C99D2 - F3 0F 10 4D 80         - movss xmm1,[rbp-80] { another wind simulation block 2 } -- try7 - 23
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook6 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2C99D2, &FixClothPhysicsDetour6, 5);
	g_FixClothPhysics_ReturnAddr6 = ClothPhysicsHook6->GetReturnAddress();
	ClothPhysicsHook6->Toggle(enable);

	// dmc3.exe+2C99DB - F3 0F 10 45 88         - movss xmm0,[rbp-78] -- try7 - 24
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook7 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2C99DB, &FixClothPhysicsDetour7, 5);
	g_FixClothPhysics_ReturnAddr7 = ClothPhysicsHook7->GetReturnAddress();
	ClothPhysicsHook7->Toggle(enable);

	// dmc3.exe+2C99E4 - F3 0F 10 55 84         - movss xmm2,[rbp-7C] -- try7 - 25
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook8 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2C99E4, &FixClothPhysicsDetour8, 5);
	g_FixClothPhysics_ReturnAddr8 = ClothPhysicsHook8->GetReturnAddress();
	ClothPhysicsHook8->Toggle(enable);

	// dmc3.exe+2C99F1 - F3 0F 11 4D 00         - movss [rbp+00],xmm1 -- try7 - 26
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook9 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2C99F1, &FixClothPhysicsDetour9, 5);
	g_FixClothPhysics_ReturnAddr9 = ClothPhysicsHook9->GetReturnAddress();
	ClothPhysicsHook9->Toggle(enable);

	// dmc3.exe+2C9A09 - F3 0F 11 4D F0         - movss [rbp-10],xmm1 -- try7 - 28
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook10 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2C9A09, &FixClothPhysicsDetour10, 5);
	g_FixClothPhysics_ReturnAddr10 = ClothPhysicsHook10->GetReturnAddress();
	ClothPhysicsHook10->Toggle(enable);

	// dmc3.exe+2C9A52 - F3 44 0F 59 0D E5 C4 1F 00  - mulss xmm9,[dmc3.exe+4C5F40] { (10.00) } -- try7 - 32
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook11 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2C9A52, &FixClothPhysicsDetour11, 9);
	g_FixClothPhysics_ReturnAddr11 = ClothPhysicsHook11->GetReturnAddress();
	g_FixClothPhysics_Detour11_FloatConstAddr = (uintptr_t)appBaseAddr + 0x4C5F40;
	ClothPhysicsHook11->Toggle(enable);

	run = enable;
}

void ClothPhysicsEnhancementFixes(bool enable) {
	using namespace Utility;

	static bool run = false;
	if (run == enable) {
		return;
	}

	// dmc3.exe+2C9ADA - F3 41 0F 58 F2         - addss xmm6,xmm10 { Cloth relaxation 1 } -- try7 - 35
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook12 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2C9ADA, &FixClothPhysicsDetour12, 5);
	g_FixClothPhysics_ReturnAddr12 = ClothPhysicsHook12->GetReturnAddress();
	ClothPhysicsHook12->Toggle(enable);

	// dmc3.exe+2C9AE5 - F3 41 0F 58 FB         - addss xmm7,xmm11 { cloth relaxation 2 } -- try7 - 36
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook13 =
		std::make_unique<Utility::Detour_t>((uintptr_t)appBaseAddr + 0x2C9AE5, &FixClothPhysicsDetour13, 5);
	g_FixClothPhysics_ReturnAddr13 = ClothPhysicsHook13->GetReturnAddress();
	ClothPhysicsHook13->Toggle(false);

	// dmc3.exe+2C9AF9 - F3 45 0F 58 C4         - addss xmm8,xmm12 { cloth relaxation 3 } -- try7 - 37
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook14 =
		std::make_unique<Utility::Detour_t>((uintptr_t)appBaseAddr + 0x2C9AF9, &FixClothPhysicsDetour14, 5);
	g_FixClothPhysics_ReturnAddr14 = ClothPhysicsHook14->GetReturnAddress();
	ClothPhysicsHook14->Toggle(enable);

	// From UpdateCPlDanteCoat_sub_140212070:
	// dmc3.exe+2120BF - E8 FC 7C 0B 00           - call dmc3.RigidnessCPlPlayerCoat_sub_1402C9DC0
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook15 =
		std::make_unique<Utility::Detour_t>((uintptr_t)appBaseAddr + 0x2120BF, &FixClothPhysicsDetour15, 5);
	g_FixClothPhysics_ReturnAddr15 = ClothPhysicsHook15->GetReturnAddress();
	g_FixClothPhysics_RigidnessDanteCoatCall = (uintptr_t)appBaseAddr + 0x2C9DC0;
	ClothPhysicsHook15->Toggle(enable);

	run = enable;
}

void ClothPhysicsFixesController() {
	auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);
	if (g_scene == SCENE::CUTSCENE && sessionData.mission == 1) { // We disable it at the cutscene at mission 1 to prevent coat clipping through Dante's head.
		ClothPhysicsFixes(false);
		g_ClothPhysicsEnhancementEnabled = 0;
		ClothPhysicsEnhancementFixes(false);
		return;
	}

	// Always enable base cloth physics FPS fixes (detours 1-11) when above the 60 FPS range
	if (g_FrameRate >= 65) {
		ClothPhysicsFixes(true);
	}
	else {
		ClothPhysicsFixes(false); // We disable it below the range because it makes cloth physics stiffen at lower frame rates.
	}

	const bool clothEnhancementEnabled = activeCrimsonConfig.Visual.clothPhysicsEnhancement;

	// Set the flag for Detour11's velocity computation
	g_ClothPhysicsEnhancementEnabled = clothEnhancementEnabled ? 1 : 0;

	// Toggle per-particle relaxation detours (12-14) based on config setting
	ClothPhysicsEnhancementFixes(clothEnhancementEnabled);
}

void LookAtBossCamFixes(bool enable) {
	using namespace Utility;
	static bool run = false;
	if (run == enable) {
		return;
	}

	// From CCameraBossLookAtTarget_sub_1402C8B80:
	// dmc3.exe+E7DDE - 48 89 44 24 40        - mov [rsp+40],rax
	static std::unique_ptr<Utility::Detour_t> BossCamLookAtHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0xE7DDE, &FixBossCamLookAtDetour, 5);
	g_FixBossCamLookAt_ReturnAddr = BossCamLookAtHook->GetReturnAddress();
	BossCamLookAtHook->Toggle(enable);
	run = enable;
}

void BossCamFixes(bool enable) {
	using namespace Utility;
	static bool run = false;
	if (run == enable) {
		return;
	}
	LookAtBossCamFixes(enable);

	// From CCameraBossVergilFightMovement_sub_140054490:
	// dmc3.exe + 544CD - F3 0F 5C 05 97 90 30 00 - subss xmm0, [dmc3.exe + 35D56C] { (1.00) }
	static std::unique_ptr<Utility::Detour_t> BossCamRefocusHook1 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x544CD, &FixBossCamRefocusDetour1, 8);
	g_FixBossCamRefocus_ReturnAddr1 = BossCamRefocusHook1->GetReturnAddress();
	BossCamRefocusHook1->Toggle(enable);

	// From sub_140050D70:
	// dmc3.exe+50D9F - F3 0F 59 05 79 0B 32 00   - mulss xmm0,[dmc3.exe + 371920] { fov Control }
	static std::unique_ptr<Utility::Detour_t> BossCamFOVHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x50D9F, &BossCamFOVDetour, 8);
	g_BossCamFOV_ReturnAddr = BossCamFOVHook->GetReturnAddress();
	g_BossCamFOV_ConstAddr = (uintptr_t)appBaseAddr + 0x371920;
	g_ActiveFOVMultSettingAddr = &activeCrimsonConfig.Camera.fovMultiplier;
	BossCamFOVHook->Toggle(enable);

	// From sub_140051A70:
	// dmc3.exe+51ADD - 0F 29 87 90 01 00 00      - movaps [rdi+00000190],xmm0 { DetermineBossCamPos }
	// g_CustomCameraPos_NewPosAddr
	static std::unique_ptr<Utility::Detour_t> BossCamCustomPositioningHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x51ADD, &BossCamCustomPositioningDetour, 7);
	g_BossCamCustomPositioning_ReturnAddr = BossCamCustomPositioningHook->GetReturnAddress();
	BossCamCustomPositioningHook->Toggle(enable);
	g_isMPCamActiveAddr = &g_isMPCamActive;

	run = enable;
}

void StaggerGravityInertiaFix(bool enable) {
	using namespace Utility;
	static bool run = false;
	if (run == enable) {
		return;
	}
	// From CPlayerStaggeGravity_sub_1401FBE20:
	// dmc3.exe+1FBEB9 - F3 0F 10 87 A4 00 00 00   - movss xmm0,[rdi+000000A4] // rdi == player, +0xA4 == verticalInertiaMultiplier
	static std::unique_ptr<Utility::Detour_t> StaggerGravityHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1FBEB9, &FixStaggerGravityDetour, 8);
	g_FixStaggerGravity_ReturnAddr = StaggerGravityHook->GetReturnAddress();
	StaggerGravityHook->Toggle(enable);

	run = enable;
}

void CStageSetGateSpawnFix(bool enable) {
	using namespace Utility;
	static bool run = false;
	if (run == enable) {
		return;
	}
	// From SpawnCStageSetGate_sub_1402561B0
	// These are all different occurrences of the Gate Portal Fading in:
	
	// dmc3.exe+2564B8 - F3 0F 10 4F 14         - movss xmm1,[rdi+14]
	static std::unique_ptr<Utility::Detour_t> CStageSetGateSpawnHook1 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2564B8, &FixCStageSetGateSpawnDetour1, 5);
	g_FixCStageSetGateSpawn_ReturnAddr1 = CStageSetGateSpawnHook1->GetReturnAddress();
	CStageSetGateSpawnHook1->Toggle(enable);

	// dmc3.exe+256298 - F3 0F10 47 14         - movss xmm0,[rdi+14]
	static std::unique_ptr<Utility::Detour_t> CStageSetGateSpawnHook2 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x256298, &FixCStageSetGateSpawnDetour2, 5);
	g_FixCStageSetGateSpawn_ReturnAddr2 = CStageSetGateSpawnHook2->GetReturnAddress();
	CStageSetGateSpawnHook2->Toggle(enable);

	// dmc3.exe+25636D - F3 0F10 47 14         - movss xmm0,[rdi+14]
	static std::unique_ptr<Detour_t> CStageSetGateSpawnHook3 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x25636D, &FixCStageSetGateSpawnDetour3, 5);
	g_FixCStageSetGateSpawn_ReturnAddr3 = CStageSetGateSpawnHook3->GetReturnAddress();
	CStageSetGateSpawnHook3->Toggle(enable);

	// dmc3.exe+2563B1 - F3 0F10 4F 14         - movss xmm1,[rdi+14]
	static std::unique_ptr<Utility::Detour_t> CStageSetGateSpawnHook4 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2563B1, &FixCStageSetGateSpawnDetour4, 5);
	g_FixCStageSetGateSpawn_ReturnAddr4 = CStageSetGateSpawnHook4->GetReturnAddress();
	CStageSetGateSpawnHook4->Toggle(enable);

	run = enable;
}

void FixEnemyAttackCooldowns(bool enable) {
	using namespace Utility;
	static bool run = false;
	if (run == enable) {
		return;
	}

	// From EnemySlottingControl_sub_1401C9A30:
	// dmc3.exe+1C9A56 - F3 0F 10 05 42 93 B2 00 - movss xmm0,[dmc3.exe+CF2DA0] { Enemy Slotting Cooldown }
	static std::unique_ptr<Utility::Detour_t> fixEnemyAttackCooldownsHook1 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1C9A56, &FixEnemyAttackCooldownsDetour1, 8);
	g_FixEnemyAttackCooldowns_ReturnAddr1 = fixEnemyAttackCooldownsHook1->GetReturnAddress();
	// g_FixEnemyAttackCooldownsCheckCall1 = &SomeCppFunction;  // Uncomment if calling C++ functions
	fixEnemyAttackCooldownsHook1->Toggle(enable);

	// From CEm013UpdateEnigmaAttackBehavior_sub_1400D8CB0:
	// dmc3.exe+D90C6 - F3 41 0F 5C C0         - subss xmm0,xmm8 { Enigma Time Between Charges for BlueandRedAttacks }
	static std::unique_ptr<Utility::Detour_t> fixEnemyAttackCooldownsHook2 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0xD90C6, &FixEnemyAttackCooldownsDetour2, 5);
	g_FixEnemyAttackCooldowns_ReturnAddr2 = fixEnemyAttackCooldownsHook2->GetReturnAddress();
	fixEnemyAttackCooldownsHook2->Toggle(enable);

	run = enable;
}

void FixBloodgoyleStormFormTime(bool enable) {
	using namespace Utility;

	static bool run = false;
	if (run == enable) {
		return;
	}

	// From CEm014BloodgoyleUpdate_sub_1400E2D70:
	// dmc3.exe+E2E2C - F3 0F 5C 84 C7 68 B5 56 00 - subss xmm0,[rdi+rax*8+0056B568] { Calculate time to turn back to blood from stone }
	static std::unique_ptr<Utility::Detour_t> fixBloodgoyleStormFormTimeHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0xE2E2C, &FixBloodgoyleStormFormTimeDetour, 9);
	g_FixBloodgoyleStormFormTime_ReturnAddr = fixBloodgoyleStormFormTimeHook->GetReturnAddress();
	fixBloodgoyleStormFormTimeHook->Toggle(enable);

	run = enable;
}



void FixCStaffRollCredits(bool enable) {
	using namespace Utility;
	static bool run = false;
	if (run == enable) {
		return;
	}

	// From CStaffRollUpdate_sub_1402454A0:
	// dmc3.exe+2454D3 - F3 0F 58 0D 91 80 11 00 - addss xmm1,[dmc3.exe+35D56C] { (1.00) -- CStaffRollSpeed }
	static std::unique_ptr<Utility::Detour_t> cStaffRollCreditsFixHook1 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2454D3, &CStaffRollCreditsFixDetour1, 8);
	g_CStaffRollCreditsFix_ReturnAddr1 = cStaffRollCreditsFixHook1->GetReturnAddress();
	// g_CStaffRollCreditsFixCheckCall1 = &SomeCppFunction;  // Uncomment if calling C++ functions
	cStaffRollCreditsFixHook1->Toggle(enable);

	// From ManageCEventMission_sub_1401A6510:
	// dmc3.exe+1A6CFF - F3 0F 5C 0D 65 68 1B 00 - subss xmm1,[dmc3.exe+35D56C] { (1.00) -- Mission20CreditsEndMissionTime }
	static std::unique_ptr<Utility::Detour_t> cStaffRollCreditsFixHook2 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1A6CFF, &CStaffRollCreditsFixDetour2, 8);
	g_CStaffRollCreditsFix_ReturnAddr2 = cStaffRollCreditsFixHook2->GetReturnAddress();
	cStaffRollCreditsFixHook2->Toggle(enable);

	run = enable;
}

void FixCStageSetSeal(bool enable) {
	using namespace Utility;
	static bool run = false;
	if (run == enable) {
		return;
	}

	// From CStageSetSealHandAndAnim_sub_14026E910:
	// dmc3.exe+26E9BD - F3 44 0F 2C C0 - cvttss2si r8d,xmm0 { fade logic truncation (bad) }
	static std::unique_ptr<Utility::Detour_t> fixCStageSetSealHook1 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x26E9BD, &FixCStageSetSealDetour1, 5);
	g_FixCStageSetSeal_ReturnAddr1 = fixCStageSetSealHook1->GetReturnAddress();
	fixCStageSetSealHook1->Toggle(enable);

	// dmc3.exe+26EB09 - F3 0F 10 73 14 - movss xmm6,[rbx+14] { setting speed for scrolling anim }
	static std::unique_ptr<Utility::Detour_t> fixCStageSetSealHook2 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x26EB09, &FixCStageSetSealDetour2, 5);
	g_FixCStageSetSeal_ReturnAddr2 = fixCStageSetSealHook2->GetReturnAddress();
	fixCStageSetSealHook2->Toggle(enable);

	// dmc3.exe+26EB46 - F3 44 0F 2C C8 - cvttss2si r9d,xmm0 { scrolling speed truncation (bad) }
	// dmc3.exe+26EB4B - F3 44 0F 2C C1 - cvttss2si r8d,xmm1
	static std::unique_ptr<Utility::Detour_t> fixCStageSetSealHook3 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x26EB46, &FixCStageSetSealDetour3, 10);
	g_FixCStageSetSeal_ReturnAddr3 = fixCStageSetSealHook3->GetReturnAddress();
	fixCStageSetSealHook3->Toggle(enable);

	run = enable;
}


void FixCPl021Shl01SummonedSwordsInitialTravel(bool enable) {
	using namespace Utility;

	static bool run = false;
	if (run == enable) {
		return;
	}

	// From CPl021Shl01ForwardStepTowardTargetYaw_sub_1401DA020:
	// dmc3.exe+1DA078 - F3 0F 11 4C 24 68 - movss [rsp+68],xmm1 { store initial summoned sword easing step distance }
	static std::unique_ptr<Utility::Detour_t> fixCPl021Shl01SummonedSwordsInitialTravelHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1DA078, &FixSummonedSwordsInitialTravelDetour, 6);
	g_FixSummonedSwordsInitialTravel_ReturnAddr = fixCPl021Shl01SummonedSwordsInitialTravelHook->GetReturnAddress();
	fixCPl021Shl01SummonedSwordsInitialTravelHook->Toggle(enable);

	// dmc3.exe+1DB8C2 - F3 0F 11 83 D4 0D 00 00 - movss [rbx+00000DD4],xmm0 { easing accumulation }

	static std::unique_ptr<Utility::Detour_t> fixCPl021Shl01SummonedSwordsInitialTravelHook2 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1DB8C2, &FixSummonedSwordsInitialTravelDetour2, 8);
	g_FixSummonedSwordsInitialTravel_ReturnAddr2 = fixCPl021Shl01SummonedSwordsInitialTravelHook2->GetReturnAddress();
	fixCPl021Shl01SummonedSwordsInitialTravelHook2->Toggle(enable);

	// dmc3.exe+1DA133 - 0F 28 4C 24 20 - movaps xmm1,[rsp+20] { easing displacement vector }
	static std::unique_ptr<Utility::Detour_t> fixCPl021Shl01SummonedSwordsInitialTravelHook3 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1DA133, &FixSummonedSwordsInitialTravelDetour3, 5);
	g_FixSummonedSwordsInitialTravel_ReturnAddr3 = fixCPl021Shl01SummonedSwordsInitialTravelHook3->GetReturnAddress();
	fixCPl021Shl01SummonedSwordsInitialTravelHook3->Toggle(enable);

	run = enable;
}

void FixSecretMissionTimerFPS(bool enable) {
	// This will untie the Secret Mission timer from FPS settings (ie no longer spawning with half the time when playing at 120 fps).
	using namespace Utility;
	static bool run = false;
	if (run == enable) {
		return;
	}

	// FixSecretMissionTimerFPS
	// From CUIDCockpit00_SetSecretMissionTimer_sub_14027C0A0:
	// dmc3.exe+27C0DD - F3 0F 11 81 48 69 00 00 - movss [rcx+00006948],xmm0 { Setting secret mission timer } // RCX is HUDPtr
	static std::unique_ptr<Utility::Detour_t> FixSecretMissionTimerFPSHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x27C0DD, &FixSecretMissionTimerFPSDetour, 8);
	g_FixSecretMissionTimerFPS_ReturnAddr = FixSecretMissionTimerFPSHook->GetReturnAddress();
	FixSecretMissionTimerFPSHook->Toggle(false); // Uneeded we were just trying to fix Serp's mistake.

	// From CUIDCockpit00_ControlSecretMissionTimer_sub_14027EEC0:
	// dmc3.exe+27EF97 - 44 0F BE 15 5E 0C A1 00 - movsx r10d,byte ptr [dmc3.exe+C8FBFD] { (60) } // holds locked 60 fps value
	static std::unique_ptr<Utility::Detour_t> FixSecretMissionTimerFPSHook2 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x27EF97, &FixSecretMissionTimerFPSDetour2, 8);
	g_FixSecretMissionTimerFPS_ReturnAddr2 = FixSecretMissionTimerFPSHook2->GetReturnAddress();
	FixSecretMissionTimerFPSHook2->Toggle(enable); // Might not be needed but we do it anyway

	run = enable;
}

void FixWpNunchakuCerberusCollisions(bool enable) {
	using namespace Utility;

	static bool run = false;
	if (run == enable) {
		return;
	}

	// From WpNunchakuCollision_sub_14022F4C0:
	// dmc3.exe+22FAFB - F3 0F 10 15 69 DA 12 00 - movss xmm2,[dmc3.exe+35D56C] { (1,00) }
	static std::unique_ptr<Utility::Detour_t> fixWpNunchakuCerberusCollisionsHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x22FAFB, &FixWpNunchakuCerberusCollisionsDetour, 8);
	g_FixWpNunchakuCerberusCollisions_ReturnAddr = fixWpNunchakuCerberusCollisionsHook->GetReturnAddress();
	fixWpNunchakuCerberusCollisionsHook->Toggle(enable);

	run = enable;
}

// Vanilla Doppelganger input history (CPlInputHistory, PlayerActorData+0x6410) is a ring indexed in
// ticks: {1, 30, 60} entries back. Ticks run once per rendered frame, so at higher frame rates the
// delay got shorter. The history is mirrored here with timestamps and read back by time instead.
namespace {

constexpr uint32 kInputSnapshotSize = 0x2E;
constexpr uint32 kInputHistoryCapacity = 2048;
constexpr uint32 kInputHistorySlotCount = 8;
constexpr double kVanillaFrameSeconds = 1.0 / 60.0;
constexpr double kInputHistoryMaxTickSeconds = 0.1;

struct InputHistoryEntry {
	double time;
	byte8 data[0x30];
};

struct InputHistorySlot {
	byte8* ring;
	byte8* buffer;
	uint32 head;
	uint32 count;
	double clock;
	int64 lastTick;
	InputHistoryEntry entries[kInputHistoryCapacity];
};

InputHistorySlot g_inputHistorySlots[kInputHistorySlotCount];
byte8 g_zeroInputSnapshot[0x30] = {};

byte8* RingBuffer(byte8* ring) {
	return *reinterpret_cast<byte8**>(ring + 0x28);
}

InputHistorySlot* FindInputHistorySlot(byte8* ring) {
	if (!ring) {
		return nullptr;
	}
	for (auto& slot : g_inputHistorySlots) {
		if (slot.ring == ring) {
			return &slot;
		}
	}
	return nullptr;
}

void ResetInputHistorySlot(InputHistorySlot& slot) {
	slot.head = 0;
	slot.count = 0;
	slot.lastTick = 0;
	slot.buffer = RingBuffer(slot.ring);
}

InputHistorySlot& AcquireInputHistorySlot(byte8* ring) {
	if (auto slot = FindInputHistorySlot(ring)) {
		return *slot;
	}
	InputHistorySlot* target = &g_inputHistorySlots[0];
	for (auto& slot : g_inputHistorySlots) {
		if (!slot.ring) {
			target = &slot;
			break;
		}
		if (slot.lastTick < target->lastTick) {
			target = &slot;
		}
	}
	target->ring = ring;
	target->clock = 0.0;
	ResetInputHistorySlot(*target);
	return *target;
}

} // namespace

// Replaces the calls to CPlInputHistory_Push_sub_1402EDFE0 in PreCPlController_sub_1401F83A0
// (dmc3.exe+1F8905, +1F8D8E).
uint64 DoppelgangerInputHistoryPush(byte8* ring, byte8* input) {
	using Push_t = uint64(__fastcall*)(byte8*, byte8*);
	auto result = reinterpret_cast<Push_t>(appBaseAddr + 0x2EDFE0)(ring, input);

	if (!ring || !input || !*(ring + 0x3C) || !RingBuffer(ring)) {
		return result;
	}

	auto& slot = AcquireInputHistorySlot(ring);
	if (slot.buffer != RingBuffer(ring)) {
		ResetInputHistorySlot(slot);
	}

	LARGE_INTEGER freq = {};
	LARGE_INTEGER now = {};
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&now);

	// The clock only advances while the game ticks: a pause or a long hitch counts as one vanilla frame.
	double dt = kVanillaFrameSeconds;
	if (slot.lastTick && freq.QuadPart) {
		dt = static_cast<double>(now.QuadPart - slot.lastTick) / static_cast<double>(freq.QuadPart);
		if (dt <= 0.0 || dt > kInputHistoryMaxTickSeconds) {
			dt = kVanillaFrameSeconds;
		}
	}
	slot.lastTick = now.QuadPart;
	slot.clock += dt;

	auto& entry = slot.entries[slot.head];
	entry.time = slot.clock;
	memcpy(entry.data, input, kInputSnapshotSize);
	slot.head = (slot.head + 1) % kInputHistoryCapacity;
	if (slot.count < kInputHistoryCapacity) {
		slot.count++;
	}

	return result;
}

// Replaces the calls to CPlInputHistory_GetDelayed_sub_1402EE040 (dmc3.exe+1F8922, +1F8DAB). The
// offset is -frames at 60 fps; the snapshot returned is the one recorded that long ago in real time.
byte8* DoppelgangerInputHistoryGetDelayed(byte8* ring, int32 offset) {
	using GetDelayed_t = byte8*(__fastcall*)(byte8*, int32);
	auto original = reinterpret_cast<GetDelayed_t>(appBaseAddr + 0x2EE040);

	auto slot = ring ? FindInputHistorySlot(ring) : nullptr;
	if (!slot || slot->count == 0 || offset >= 0 || slot->buffer != RingBuffer(ring)) {
		return original(ring, offset);
	}

	const double target = slot->clock - static_cast<double>(-offset) * kVanillaFrameSeconds;
	auto entryAt = [&](uint32 age) -> InputHistoryEntry& {
		return slot->entries[(slot->head + kInputHistoryCapacity - 1 - age) % kInputHistoryCapacity];
	};

	uint32 found = slot->count;
	for (uint32 age = 0; age < slot->count; age++) {
		if (entryAt(age).time <= target) {
			found = age;
			break;
		}
	}

	if (found == slot->count) {
		// Not enough history yet: the ring was just cleared, so vanilla hands back an empty input.
		return (slot->count == kInputHistoryCapacity) ? entryAt(slot->count - 1).data : g_zeroInputSnapshot;
	}

	// Pick whichever recorded tick lies closest to the target time.
	if (found > 0 && (entryAt(found - 1).time - target) < (target - entryAt(found).time)) {
		found--;
	}
	return entryAt(found).data;
}

// Replaces the memset in the ring clear of CPlStyleActionController_sub_1401E8F80 (dmc3.exe+1E92B5).
void* DoppelgangerInputHistoryClearBuffer(void* buffer, int value, size_t size) {
	auto result = memset(buffer, value, size);
	for (auto& slot : g_inputHistorySlots) {
		if (slot.ring && slot.buffer == buffer) {
			ResetInputHistorySlot(slot);
		}
	}
	return result;
}

void DoppelgangerInputHistoryReset(byte8* ring) {
	if (auto slot = FindInputHistorySlot(ring)) {
		ResetInputHistorySlot(*slot);
	}
}

void FixDoppelgangerInputDelay(bool enable) {
	static bool run = false;
	if (run == enable) {
		return;
	}

	/*
	dmc3.exe+1F8905 - call dmc3.exe+2EDFE0 { Push }
	dmc3.exe+1F8D8E - call dmc3.exe+2EDFE0 { Push }
	dmc3.exe+1F8922 - call dmc3.exe+2EE040 { GetDelayed }
	dmc3.exe+1F8DAB - call dmc3.exe+2EE040 { GetDelayed }
	dmc3.exe+1E92B5 - call memset { ring clear on activation }
	*/
	constexpr uint32 size = 5;
	static byte8* pushAddrs[2]       = {appBaseAddr + 0x1F8905, appBaseAddr + 0x1F8D8E};
	static byte8* getDelayedAddrs[2] = {appBaseAddr + 0x1F8922, appBaseAddr + 0x1F8DAB};
	static byte8* clearAddr          = appBaseAddr + 0x1E92B5;
	static Function pushFunc         = {};
	static Function getDelayedFunc   = {};
	static Function clearFunc        = {};

	if (!run) {
		for (auto addr : pushAddrs) {
			backupHelper.Save(addr, size);
		}
		for (auto addr : getDelayedAddrs) {
			backupHelper.Save(addr, size);
		}
		backupHelper.Save(clearAddr, size);
		pushFunc       = old_CreateFunction(DoppelgangerInputHistoryPush, 0, true, false);
		getDelayedFunc = old_CreateFunction(DoppelgangerInputHistoryGetDelayed, 0, true, false);
		clearFunc      = old_CreateFunction(DoppelgangerInputHistoryClearBuffer, 0, true, false);
	}

	if (enable) {
		for (auto addr : pushAddrs) {
			WriteCall(addr, pushFunc.addr, (size - 5));
		}
		for (auto addr : getDelayedAddrs) {
			WriteCall(addr, getDelayedFunc.addr, (size - 5));
		}
		WriteCall(clearAddr, clearFunc.addr, (size - 5));
	} else {
		for (auto addr : pushAddrs) {
			backupHelper.Restore(addr);
		}
		for (auto addr : getDelayedAddrs) {
			backupHelper.Restore(addr);
		}
		backupHelper.Restore(clearAddr);
	}

	run = enable;
}

// UpdateTeleportEvent_sub_14023C100 (EVENT::TELEPORT) counts its sub-state frames and ramps the
// transition effects (TeleportTransitionTick*) once per call, so the room transition plays faster
// the higher the frame rate. It is stepped at 60 Hz in real time instead.
namespace {

constexpr uint32 kTeleportEventOffset = 0x23C100;
constexpr uint32 kTeleportMaxCatchUpTicks = 4;
constexpr double kTeleportMaxFrameSeconds = 0.1;

using TeleportEventFn = uint64(__fastcall*)(byte8*);
std::unique_ptr<Utility::Detour_t> s_teleportEventHook;
int64 s_teleportLastFrame = 0;
double s_teleportTickDebt = 0.0;

uint64 __fastcall TeleportEventHook(byte8* scene) {
	const auto original = s_teleportEventHook->GetTrampoline<TeleportEventFn>();
	auto& eventData = *reinterpret_cast<EventData*>(scene);

	LARGE_INTEGER freq = {};
	LARGE_INTEGER now = {};
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&now);

	// Sub-state 0 is the first call of the event and kicks the animation off, so it always runs.
	if (*(scene + 0x24) == 0 || !s_teleportLastFrame || !freq.QuadPart) {
		s_teleportLastFrame = now.QuadPart;
		s_teleportTickDebt = 0.0;
		return original(scene);
	}

	double dt = static_cast<double>(now.QuadPart - s_teleportLastFrame) / static_cast<double>(freq.QuadPart);
	s_teleportLastFrame = now.QuadPart;
	if (dt <= 0.0 || dt > kTeleportMaxFrameSeconds) {
		dt = kVanillaFrameSeconds;
	}

	s_teleportTickDebt += dt;

	uint64 result = 0;
	for (uint32 i = 0; i < kTeleportMaxCatchUpTicks && s_teleportTickDebt >= kVanillaFrameSeconds; i++) {
		s_teleportTickDebt -= kVanillaFrameSeconds;
		result = original(scene);

		// The last tick hands over to another event, which must not be ticked as a teleport.
		if (eventData.event != EVENT::TELEPORT) {
			s_teleportTickDebt = 0.0;
			break;
		}
	}
	s_teleportTickDebt = (std::min)(s_teleportTickDebt, kVanillaFrameSeconds);

	return result;
}

} // namespace

void FixTeleportTransitionSpeed(bool enable) {
	using namespace Utility;

	static bool run = false;
	if (run == enable) {
		return;
	}

	if (!s_teleportEventHook) {
		s_teleportEventHook = std::make_unique<Detour_t>(
			(uintptr_t)appBaseAddr + kTeleportEventOffset, &TeleportEventHook, 0, "UpdateTeleportEvent_sub_14023C100");
	}
	s_teleportEventHook->Toggle(enable);

	run = enable;
}

void MenuAnimationFixes(bool enable) {
	static bool run = false;

	// The menu animation hooks install once and stay in; their behaviour follows the System config.
	if (!enable || run) {
		return;
	}

	CrimsonMenuAnimation::InitHooks();

	run = true;
}

void ToggleAllFixes(bool enable) {
	BlendingEffectsSpeedFixes(enable);
	BossCamFixes(enable);
	StaggerGravityInertiaFix(enable);
	CStageSetGateSpawnFix(enable);
	FixEnemyAttackCooldowns(enable);
	FixBloodgoyleStormFormTime(enable);
	FixCStaffRollCredits(enable);
	FixCStageSetSeal(enable);
	FixCPl021Shl01SummonedSwordsInitialTravel(enable);
	FixSecretMissionTimerFPS(enable);
	FixWpNunchakuCerberusCollisions(enable);
	FixDoppelgangerInputDelay(enable);
	FixTeleportTransitionSpeed(enable);
	MenuAnimationFixes(enable);
}



}
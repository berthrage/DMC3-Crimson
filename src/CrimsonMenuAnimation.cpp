// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include <algorithm>
#include <intrin.h>
#include <memory>
#include "Core/Core.hpp"
#include "CrimsonMenuAnimation.hpp"
#include "CrimsonClock.hpp"
#include "Config.hpp"
#include "Global.hpp"
#include "Vars.hpp"
#include "Utility/Detour.hpp"

namespace CrimsonMenuAnimation {

extern "C" {
	void MenuAnimationStepDetour();
	std::uint64_t g_MenuAnimationStepCall;

	void CreditsAlphaStepDetour();
	std::uint64_t g_CreditsAlphaStep_ReturnAddr;
	int g_creditsAlphaStep;

	void FrameCounterStepDetour();
	std::uint64_t g_FrameCounterStep_ReturnAddr;
	int g_frameCounterStep;
	int g_frameCounterCarry;
}

// The shared step helper already returns the speed channels scaled by 60 / fps, so it
// only needs the Menu Speed slider on top. Everything else here has no such scale.
static float s_menuStepScale = 1.0f;

// 60 / fps combined with the Menu Speed slider, for counters the game steps per frame.
static float s_menuFrameScale = 1.0f;

// The game advances every menu animation by adding or subtracting a literal once per
// rendered frame, so at 120 Hz they play twice as fast. Each counter reads a float
// constant through a RIP-relative operand; re-pointing that operand at a live scale
// turns the counter into real time without touching the instruction itself.
//
// The scale slots live in the global speed table (dmc3.exe+CF2D90). That table has 16
// float channels but ControlCFadeAmount_sub_140326960 only multiplies the first eight,
// so channels 8..11 are dead storage. Keeping them inside the game module also keeps
// every retargeted displacement within reach of a RIP-relative operand.
static constexpr uint32 kSpeedTableOffset = 0xCF2D90;
static constexpr uint32 kMenuTimerSlot = kSpeedTableOffset + 0x20; // channel 8:  menu 1.0 steps
static constexpr uint32 kMenuRateSlot = kSpeedTableOffset + 0x24;  // channel 9:  menu 0.05 ramps
static constexpr uint32 kFixedTimerSlot = kSpeedTableOffset + 0x28; // channel 10: fixed 1.0 steps
static constexpr uint32 kBlinkPhaseSlot = kSpeedTableOffset + 0x2C; // channel 11: fixed 3.0 phases

static float* Slot(uint32 offset) { return (float*)(appBaseAddr + offset); }

struct StepSite {
	uint32 offset;
	float base;   // the literal the instruction originally read, used to verify it
};

// Cockpit and credit timers: 1.0 per frame, always 100% speed.
static constexpr StepSite kFixedTimerSites[] = {
	{ 0x27D630, 1.0f },  // CUIDCockpit00_UpdateHP: hit flash timer
	{ 0x27D638, 1.0f },  // CUIDCockpit00_UpdateHP: red bar timer
	{ 0x27DF4B, 1.0f },  // CUIDCockpit00_Tick: top-left alpha timer
	{ 0x27E81B, 1.0f },  // CUIDCockpit00_UpdateRedOrbCounter: alpha timer
	{ 0x2A73FE, 1.0f },  // CUIDStaffRoll slot 1: credit panel timer
	{ 0x2956A8, 1.0f },  // CUIDItemGet: motion step
	{ 0x295751, 1.0f },  // CUIDItemGet: motion step
	{ 0x2BC360, 1.0f },  // CUIDTitle: logo / sword motion step
	{ 0x2BC405, 1.0f },  // CUIDTitle: logo / sword motion step
	{ 0x28153A, 1.0f },  // CUIDComnBg: background model motion step
	{ 0x2815AB, 1.0f },  // CUIDComnBg: background model motion step
	{ 0x28B6B8, 1.0f },  // sub_14028AF70: model motion step
};

// Sine blink phases advance 3 degrees per frame: the DT-ready pulse, the cockpit
// interact prompts, the style gauges and the result screen prompts.
static constexpr StepSite kBlinkSites[] = {
	{ 0x27C650, 3.0f },  // CUIDCockpit00_UpdateDT
	{ 0x27DF78, 3.0f },  // CUIDCockpit00_Tick
	{ 0x27E844, 3.0f },  // CUIDCockpit00_UpdateRedOrbCounter
	{ 0x27E929, 3.0f },  // CUIDCockpit00_UpdateStyle
	{ 0x27EA8A, 3.0f },  // CUIDCockpit00_UpdateStyle
	{ 0x27ECDA, 3.0f },  // CUIDCockpit00_UpdateStyle
	{ 0x2A34F8, 3.0f },  // sub_1402A3420: result screen prompt pulse
	{ 0x2A5ABB, 3.0f },  // sub_1402A59F0: result BP prompt pulse
};

// Menu screens: 1.0 per frame timers, follow the frame rate and the Menu Speed slider.
static constexpr StepSite kMenuTimerSites[] = {
	{ 0x2AA4FF, 1.0f },  // sub_1402AA2B0: result BP
	{ 0x244108, 1.0f },  // CScrnBreakUpdate_sub_1402440B0: shatter motion step
	{ 0x29FB13, 1.0f },  // sub_14029F900: motion step
	{ 0x29FC25, 1.0f },  // sub_14029F900: motion step
};

// Menu screens: 0.05 per frame ramps (option screen panel slide).
static constexpr StepSite kMenuRateSites[] = {
	{ 0x1C7C6E, 0.05f },  // sub_1401C7B20
	{ 0x1C7C8B, 0.05f },  // sub_1401C7B20
};

// Call sites of ControlCFadeAmount_sub_140326960 that time a menu screen. The helper
// returns the product of the speed channels, so it has to be told which caller asked.
static constexpr uint32 kMenuStepCallSites[] = {
	0x244187, 0x2442D1,                                          // CScrnBreakUpdate_sub_1402440B0
	0x2A0BA0, 0x2A0D6F,                                          // pause menu
	0x2A12B5, 0x2A1331, 0x2A153D, 0x2A15BC, 0x2A17A7, 0x2A1826,  // mission result
	0x2A19E5, 0x2A1A64, 0x2A1C23, 0x2A1CA2, 0x2A202C,
	0x2A313E, 0x2A32BE, 0x2A3396, 0x2A3A66, 0x2A3BE0, 0x2A3E80,  // result / result BP
	0x2A4AA0, 0x2A4D36, 0x2A573E, 0x2A5891, 0x2A5969,
};

static bool IsMenuStepCaller(uintptr_t caller) {
	if (caller < (uintptr_t)appBaseAddr + 5) {
		return false;
	}

	// The stub reads the return address, which is the address after the 5-byte `call`
	// instruction. The site table stores the `call` instruction addresses.
	const auto offset = static_cast<uint32>((caller - (uintptr_t)appBaseAddr) - 5);

	for (auto site : kMenuStepCallSites) {
		if (site == offset) {
			return true;
		}
	}

	return false;
}

// Stands in for ControlCFadeAmount_sub_140326960. Only menu callers get scaled, every
// other caller keeps the original product so gameplay timers are untouched.
extern "C" float MenuAnimationStep(float* speeds, uint32 mask, uintptr_t caller) {
	float step = 1.0f;

	for (uint32 channel = 0; channel < 8; ++channel) {
		if (mask & (1u << channel)) {
			step *= speeds[channel];
		}
	}

	return IsMenuStepCaller(caller) ? step * s_menuStepScale : step;
}

// The integer and float approach helpers (value moves toward a limit by a fixed step per
// call) drive the result screen fades and the rank slide-in. They are also used by input
// handling and gameplay, so only the listed call sites are scaled.
static constexpr uint32 kApproachCallSites[] = {
	0x2A12ED, 0x2A1575, 0x2A17DF, 0x2A1A1D, 0x2A1C5B,  // mission result: alpha fade-in
	0x2A3C1C, 0x2A3C3F, 0x2A3C5C,                      // mission result: rank slide-in
	0x29F970, 0x29FBAB,                                // sub_14029F900: alpha ramp
};

static float s_approachCarry[countof(kApproachCallSites)] = {};

static int ApproachSiteIndex(uintptr_t returnAddress) {
	const auto base = (uintptr_t)appBaseAddr;

	if (returnAddress < base + 5) {
		return -1;
	}

	const auto offset = static_cast<uint32>((returnAddress - base) - 5);

	for (int i = 0; i < (int)countof(kApproachCallSites); ++i) {
		if (kApproachCallSites[i] == offset) {
			return i;
		}
	}

	return -1;
}

// Integer steps keep their fraction between calls, otherwise a step of 4 at 0.12 would
// round down to nothing every frame.
static int ScaleIntStep(int site, int step) {
	if (site < 0) {
		return step;
	}

	const auto amount = (float)step * s_menuFrameScale + s_approachCarry[site];
	const auto whole = (int)amount;
	s_approachCarry[site] = amount - (float)whole;

	return whole;
}

// sub_1402C6860: value - step, not below limit.
static int __fastcall ApproachDownHook(int value, int limit, int step) {
	step = ScaleIntStep(ApproachSiteIndex((uintptr_t)_ReturnAddress()), step);
	const auto next = value - step;
	return next > limit ? next : limit;
}

// sub_1402C6850: value + step, not above limit.
static int __fastcall ApproachUpHook(int value, int limit, int step) {
	step = ScaleIntStep(ApproachSiteIndex((uintptr_t)_ReturnAddress()), step);
	const auto next = value + step;
	return next < limit ? next : limit;
}

// sub_1402C6840: value - step, not below limit.
static float __fastcall ApproachDownFloatHook(float value, float limit, float step) {
	if (ApproachSiteIndex((uintptr_t)_ReturnAddress()) >= 0) {
		step *= s_menuFrameScale;
	}

	const auto next = value - step;
	return next > limit ? next : limit;
}

// sub_1402A09D0 rolls the result digits by one step per call, with no timer. It is
// gated so it runs once per native frame, however many frames the game renders.
using CountUpFn = unsigned char(__fastcall*)(int64_t, int64_t);
static std::unique_ptr<Utility::Detour_t> s_countUpHook;
static float s_countUpAccumulator = 0.0f;
static unsigned char s_countUpResult = 0;

static unsigned char __fastcall CountUpHook(int64_t a1, int64_t a2) {
	const auto original = s_countUpHook->GetTrampoline<CountUpFn>();

	s_countUpAccumulator += s_menuFrameScale;

	int steps = 0;

	while (s_countUpAccumulator >= 1.0f && steps < 8) {
		s_countUpAccumulator -= 1.0f;
		s_countUpResult = original(a1, a2);
		steps++;

		if (s_countUpResult) {
			break;
		}
	}

	const auto result = s_countUpResult;

	if (result) {
		s_countUpResult = 0;
		s_countUpAccumulator = 1.0f;
	}

	return result;
}

// Re-points the RIP-relative float operand of one counter at our scale slot.
static bool Retarget(const StepSite& site, float* scale) {
	auto instruction = appBaseAddr + site.offset;

	if (instruction[0] != 0xF3 || instruction[1] != 0x0F) {
		return false;
	}

	const auto opcode = instruction[2];

	// movss, addss, subss, mulss, divss
	if (opcode != 0x10 && opcode != 0x58 && opcode != 0x5C && opcode != 0x59 && opcode != 0x5E) {
		return false;
	}

	if ((instruction[3] & 0xC7) != 0x05) {
		return false;
	}

	const auto readsBase = *(float*)(instruction + 8 + *(int32*)(instruction + 4));

	if (readsBase < site.base - 0.001f || readsBase > site.base + 0.001f) {
		return false;
	}

	Write<int32>(instruction + 4, (int32)((byte8*)scale - (instruction + 8)));

	return true;
}

static uint8 s_patchedSites = 0;
static uint8 s_expectedSites = 0;
static bool s_logged = false;

// Integer counters cannot carry a fractional step through an immediate, so the fraction
// is carried across frames and only the whole part is applied.
static float s_creditsAlphaCarry = 0.0f;

void InitHooks() {
	*Slot(kMenuTimerSlot) = 1.0f;
	*Slot(kMenuRateSlot) = 0.05f;
	*Slot(kFixedTimerSlot) = 1.0f;
	*Slot(kBlinkPhaseSlot) = 3.0f;
	g_creditsAlphaStep = 2;

	g_MenuAnimationStepCall = (std::uint64_t)&MenuAnimationStep;

	// dmc3.exe+326960 - F3 0F 10 05 F0 DD 03 00 - movss xmm0,[dmc3.exe+35D56C] { (1.00) }
	static std::unique_ptr<Utility::Detour_t> menuAnimationStepHook =
		std::make_unique<Utility::Detour_t>((uintptr_t)appBaseAddr + 0x326960, &MenuAnimationStepDetour);
	menuAnimationStepHook->Toggle(true);

	// dmc3.exe+337F15 - FF 40 54 - inc dword ptr [rax+54h]  (rax = pRenderFrameState)
	// The UI screens time their blink and pulse phases from this counter, which the
	// engine increments once per rendered frame.
	static std::unique_ptr<Utility::Detour_t> frameCounterStepHook =
		std::make_unique<Utility::Detour_t>((uintptr_t)appBaseAddr + 0x337F15, &FrameCounterStepDetour, 10);
	g_FrameCounterStep_ReturnAddr = frameCounterStepHook->GetReturnAddress();
	g_frameCounterStep = 0x10000;
	g_frameCounterCarry = 0;
	frameCounterStepHook->Toggle(true);

	// dmc3.exe+245636 - 83 C0 02 - add eax,2  /  89 87 84 00 00 00 - mov [rdi+84h],eax
	static std::unique_ptr<Utility::Detour_t> creditsAlphaStepHook =
		std::make_unique<Utility::Detour_t>((uintptr_t)appBaseAddr + 0x245636, &CreditsAlphaStepDetour, 9);
	g_CreditsAlphaStep_ReturnAddr = creditsAlphaStepHook->GetReturnAddress();
	creditsAlphaStepHook->Toggle(true);

	// dmc3.exe+2C6850/2C6860/2C6840 are three 11-13 byte leaf helpers, hooked by return address.
	static std::unique_ptr<Utility::Detour_t> approachUpHook =
		std::make_unique<Utility::Detour_t>((uintptr_t)appBaseAddr + 0x2C6850, &ApproachUpHook);
	approachUpHook->Toggle(true);

	static std::unique_ptr<Utility::Detour_t> approachDownHook =
		std::make_unique<Utility::Detour_t>((uintptr_t)appBaseAddr + 0x2C6860, &ApproachDownHook);
	approachDownHook->Toggle(true);

	static std::unique_ptr<Utility::Detour_t> approachDownFloatHook =
		std::make_unique<Utility::Detour_t>((uintptr_t)appBaseAddr + 0x2C6840, &ApproachDownFloatHook);
	approachDownFloatHook->Toggle(true);

	// dmc3.exe+2A09D0 - CUIDResult digit roll
	s_countUpHook = std::make_unique<Utility::Detour_t>((uintptr_t)appBaseAddr + 0x2A09D0, &CountUpHook);
	s_countUpHook->Toggle(true);

	s_expectedSites = 0;
	s_patchedSites = 0;

	auto apply = [](const StepSite* sites, size_t count, float* slot) {
		for (size_t i = 0; i < count; ++i) {
			s_expectedSites++;
			s_patchedSites += Retarget(sites[i], slot) ? 1 : 0;
		}
	};

	apply(kFixedTimerSites, countof(kFixedTimerSites), Slot(kFixedTimerSlot));
	apply(kBlinkSites, countof(kBlinkSites), Slot(kBlinkPhaseSlot));
	apply(kMenuTimerSites, countof(kMenuTimerSites), Slot(kMenuTimerSlot));
	apply(kMenuRateSites, countof(kMenuRateSites), Slot(kMenuRateSlot));
}

// 60 / deltaTime is how many frames fit in one native frame, so multiplying a per-frame
// step by it keeps that step worth one 60 Hz frame no matter how fast the game renders.
static float FrameScale() {
	const auto delta = CrimsonClock::DeltaTime();

	if (!(delta > 0.0f)) {
		return 1.0f;
	}

	return std::clamp(60.0f * delta, 0.05f, 4.0f);
}

void OnTick() {
	const auto& system = activeCrimsonConfig.System;
	const auto frameScale = FrameScale();

	// The UI frame counter is a 60 Hz counter in the original game, so only the part
	// above 60 fps is scaled back out of it.
	g_frameCounterStep = (int)(65536.0f * std::min(1.0f, frameScale) + 0.5f);

	const auto menuScale = system.menuAnimationFix ? frameScale * (system.menuAnimationSpeed / 100.0f) : 1.0f;
	const auto fixedScale = system.menuAnimationFix ? frameScale : 1.0f;

	*Slot(kMenuTimerSlot) = menuScale;
	*Slot(kMenuRateSlot) = 0.05f * menuScale;
	*Slot(kFixedTimerSlot) = fixedScale;
	*Slot(kBlinkPhaseSlot) = 3.0f * fixedScale;
	s_menuStepScale = system.menuAnimationFix ? system.menuAnimationSpeed / 100.0f : 1.0f;
	s_menuFrameScale = menuScale;

	// Credit text alpha fades 0 -> 0x80 by 2 per frame natively; credits stay at 100%.
	s_creditsAlphaCarry += 2.0f * fixedScale;
	auto alphaStep = (int)s_creditsAlphaCarry;
	s_creditsAlphaCarry -= (float)alphaStep;
	g_creditsAlphaStep = alphaStep > 127 ? 127 : alphaStep;

	if (!s_logged && CrimsonClock::Time() > 2.0) {
		s_logged = true;
		Log("MenuAnimation patched %u/%u sites, frameScale %.3f, UI counter %.3f/frame",
			s_patchedSites, s_expectedSites, frameScale, (float)g_frameCounterStep / 65536.0f);
	}
}

}

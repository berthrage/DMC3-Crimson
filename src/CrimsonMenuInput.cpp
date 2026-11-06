#include "CrimsonMenuInput.hpp"

#include <algorithm>
#include <cstdint>
#include <intrin.h>
#include <memory>

#include "Core/Core.hpp"
#include "Utility/Detour.hpp"
#include "Global.hpp"
#include "Config.hpp"
#include "Vars.hpp"

// Every UI reader asks the pad block for pad 0 (GetPad* accessors at dmc3.exe+32CC10..32CCF0, ENGINE_GAMEPAD
// array at PadInputHandle_140D54A10+0x550, 0x2C each). For callers outside gameplay code the accessors answer
// with all players' pads combined, so any player can advance messages, skip cutscenes and drive menus.
// Gameplay readers (camera, player input, interact) keep their own pad.

namespace CrimsonMenuInput {

namespace {

constexpr uintptr_t IMAGE_SIZE = 0xDAC000;

struct Range {
	uintptr_t begin;
	uintptr_t end;
};

// Pause check, sub_14023B060: START opens the menu, but its d-pad shortcuts must stay with player 1
// Alt Players having pause will disable the TeleportToPlayer Air Trick so I'm being mindful of this
constexpr Range PAUSE_RANGE = { 0x23B060, 0x23B220 };
constexpr uint64_t PAUSE_MASK = 0x800;
constexpr uint64_t ALL_BITS   = ~0ull;

// Not merged: gameplay code that is tied to one player.
constexpr Range GAMEPLAY_RANGES[] = {
	{ 0x0501F0, 0x058000 }, // camera (the Yes/No prompt right below it, sub_14004FED0, must be merged)
	{ 0x1E0000, 0x230000 }, // player actors: input update, controller, interact check
	{ 0x296DB0, 0x296E93 }, // lock-on cursor
};

using WordFn   = uint64_t(__fastcall*)(uintptr_t, int);
using StickFn  = uint64_t(__fastcall*)(uintptr_t, int, int);

enum Accessor { POSITION, DIRECTION, RADIUS, HELD, PREVIOUS, PRESSED, RELEASED_OR_REPEAT_A, COMBINED_A, COMBINED_B, COMBINED_C, COUNT };

struct Target {
	uintptr_t offset;
	const char* name;
};

constexpr Target TARGETS[COUNT] = {
	{ 0x32CC10, "GetPadStickPosition_sub_14032CC10" },
	{ 0x32CC30, "GetPadStickDirection_sub_14032CC30" },
	{ 0x32CC50, "GetPadStickRadius_sub_14032CC50" },
	{ 0x32CC70, "GetPadButtonsHeld_sub_14032CC70" },
	{ 0x32CC80, "GetPadButtonsPrevious_sub_14032CC80" },
	{ 0x32CC90, "GetPadRightStickDirection_sub_14032CC90" },
	{ 0x32CCA0, "GetPadButtonsRepeat_sub_14032CCA0" },
	{ 0x32CCB0, "GetPadHeldOrLeftStick_sub_14032CCB0" },
	{ 0x32CCD0, "GetPadPressedCombined_sub_14032CCD0" },
	{ 0x32CCF0, "GetPadRepeatCombined_sub_14032CCF0" },
};

std::unique_ptr<Utility::Detour_t> s_hooks[COUNT];

// Number of pads to combine, 0 when the call must keep answering for pad 0 only.
int MergeCount(uintptr_t returnAddress, int pad, uint64_t& mask) {
	mask = ALL_BITS;
	if (pad != 0 || !activeConfig.Actor.enable || activeConfig.Actor.playerCount < 2) {
		return 0;
	}
	const uintptr_t rva = returnAddress - reinterpret_cast<uintptr_t>(appBaseAddr);
	if (rva >= IMAGE_SIZE) {
		return 0;
	}
	for (const auto& range : GAMEPLAY_RANGES) {
		if (rva >= range.begin && rva < range.end) {
			return 0;
		}
	}
	if (rva >= PAUSE_RANGE.begin && rva < PAUSE_RANGE.end) {
		mask = PAUSE_MASK;
	}
	return (std::min)(static_cast<int>(activeConfig.Actor.playerCount), static_cast<int>(PLAYER_COUNT));
}

template <Accessor A> uint64_t __fastcall WordHook(uintptr_t pads, int pad) {
	const auto original = s_hooks[A]->GetTrampoline<WordFn>();
	uint64_t result = original(pads, pad);
	uint64_t mask;
	const int count = MergeCount(reinterpret_cast<uintptr_t>(_ReturnAddress()), pad, mask);
	for (int other = 1; other < count; other++) {
		result |= original(pads, other) & mask;
	}
	return result;
}

uint64_t __fastcall DirectionHook(uintptr_t pads, int pad, int kind) {
	const auto original = s_hooks[DIRECTION]->GetTrampoline<StickFn>();
	uint64_t result = original(pads, pad, kind);
	uint64_t mask;
	const int count = MergeCount(reinterpret_cast<uintptr_t>(_ReturnAddress()), pad, mask);
	for (int other = 1; other < count; other++) {
		result |= original(pads, other, kind) & mask;
	}
	return result;
}

// The stick that is pushed furthest wins; position (angle) and radius come from the same pad.
int StrongestStick(uintptr_t pads, int kind, int count, uint64_t& radius) {
	const auto radiusOf = s_hooks[RADIUS]->GetTrampoline<StickFn>();
	int best = 0;
	radius = radiusOf(pads, 0, kind);
	for (int other = 1; other < count; other++) {
		const uint64_t candidate = radiusOf(pads, other, kind);
		if (candidate > radius) {
			radius = candidate;
			best = other;
		}
	}
	return best;
}

// Sticks are merged everywhere except the pause check.
int StickMergeCount(uintptr_t returnAddress, int pad) {
	uint64_t mask;
	const int count = MergeCount(returnAddress, pad, mask);
	return mask == ALL_BITS ? count : 0;
}

uint64_t __fastcall RadiusHook(uintptr_t pads, int pad, int kind) {
	const int count = StickMergeCount(reinterpret_cast<uintptr_t>(_ReturnAddress()), pad);
	uint64_t radius = s_hooks[RADIUS]->GetTrampoline<StickFn>()(pads, pad, kind);
	if (count > 1) {
		StrongestStick(pads, kind, count, radius);
	}
	return radius;
}

uint64_t __fastcall PositionHook(uintptr_t pads, int pad, int kind) {
	const int count = StickMergeCount(reinterpret_cast<uintptr_t>(_ReturnAddress()), pad);
	int source = pad;
	if (count > 1) {
		uint64_t radius = 0;
		source = StrongestStick(pads, kind, count, radius);
	}
	return s_hooks[POSITION]->GetTrampoline<StickFn>()(pads, source, kind);
}

template <Accessor A, typename Hook> void Install(Hook hook) {
	s_hooks[A] = std::make_unique<Utility::Detour_t>(
		reinterpret_cast<uintptr_t>(appBaseAddr) + TARGETS[A].offset, hook, 0, TARGETS[A].name);
	s_hooks[A]->Toggle(true);
}

} // namespace

void InitHooks() {
	static bool run = false;
	if (run) {
		return;
	}
	run = true;

	Install<POSITION>(&PositionHook);
	Install<DIRECTION>(&DirectionHook);
	Install<RADIUS>(&RadiusHook);
	Install<HELD>(&WordHook<HELD>);
	Install<PREVIOUS>(&WordHook<PREVIOUS>);
	Install<PRESSED>(&WordHook<PRESSED>);
	Install<RELEASED_OR_REPEAT_A>(&WordHook<RELEASED_OR_REPEAT_A>);
	Install<COMBINED_A>(&WordHook<COMBINED_A>);
	Install<COMBINED_B>(&WordHook<COMBINED_B>);
	Install<COMBINED_C>(&WordHook<COMBINED_C>);
}

} // namespace CrimsonMenuInput

#include "CrimsonAnimation.hpp"

#include <array>

#include "Config.hpp"
#include "Core/Core.hpp"
#include "CrimsonReversedCalls.hpp"
#include "Vars.hpp"

namespace CrimsonAnimation {

static constexpr uint32 MOTION_FINISHED = 4;
static constexpr uint32 MOTION_GROUP_COUNT = sizeof(PlayerActorDataBase::motionArchives) / sizeof(byte8*);
static constexpr uint32 ANY_EVENT = 0xFFFFFFFF;
static constexpr uint8 MAX_SCOPE_ACTIONS = 4;
static constexpr uint8 MAX_UNARMED_TICKS = 5;

struct AnimationSlot {
	byte8* actor = nullptr;
	uint32 group = 0;
	uint32 index = 0;
	uint32 startEvent = 0;
	byte8* archive = nullptr;
	byte8* originalArchive = nullptr;
	PlayOptions options;
};

struct ArchiveSwap {
	byte8* actor = nullptr;
	uint32 group = 0;
	byte8* archive = nullptr;
	byte8* originalArchive = nullptr;
	std::array<uint8, MAX_SCOPE_ACTIONS> actions = {};
	uint8 actionCount = 0;
	uint32 event = ANY_EVENT;
	bool armed = false;
	uint8 unarmedTicks = 0;
	bool engaged = false;            // the move has started playing out of this group
	uint32 engagedEvent = ANY_EVENT; // event the move started in
};

static std::array<AnimationSlot, 16> s_slots;
static std::array<ArchiveSwap, 64> s_swaps;

// Only puts the archive back if nobody replaced it meanwhile.
static void RestoreIfOurs(byte8* actorBaseAddr, uint32 group, byte8* archive, byte8* originalArchive) {
	if (!actorBaseAddr || !archive || group >= MOTION_GROUP_COUNT) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorDataBase*>(actorBaseAddr);
	if (actorData.motionArchives[group] == archive) {
		actorData.motionArchives[group] = originalArchive;
	}
}

static AnimationSlot* FindSlot(byte8* actorBaseAddr) {
	for (auto& slot : s_slots) {
		if (slot.actor == actorBaseAddr) {
			return &slot;
		}
	}
	return nullptr;
}

static void ReleaseSlot(AnimationSlot& slot) {
	RestoreIfOurs(slot.actor, slot.group, slot.archive, slot.originalArchive);
	slot = {};
}

static ArchiveSwap* FindSwap(byte8* actorBaseAddr, uint32 group) {
	for (auto& swap : s_swaps) {
		if ((swap.actor == actorBaseAddr) && (swap.group == group)) {
			return &swap;
		}
	}
	return nullptr;
}

static void ReleaseSwap(ArchiveSwap& swap) {
	RestoreIfOurs(swap.actor, swap.group, swap.archive, swap.originalArchive);
	swap = {};
}

static bool InScope(const ArchiveSwap& swap) {
	auto& actorData = *reinterpret_cast<PlayerActorDataBase*>(swap.actor);
	if ((swap.event != ANY_EVENT) && (actorData.eventData[0].event != swap.event)) {
		return false;
	}
	if (!swap.actionCount) {
		return true;
	}
	for (uint8 i = 0; i < swap.actionCount; i++) {
		if (actorData.action == swap.actions[i]) {
			return true;
		}
	}
	return false;
}

static bool IsPlayingGroup(const PlayerActorDataBase& actorData, uint32 group) {
	return (actorData.motionData[0].group == group) || (actorData.motionData[1].group == group);
}

// Ends when the action leaves the scope, or when the event changes from the one the move started in (finished into
// idle, trick, jump, damage...), even if the group's clip is still the current motion.
// The start event is only recorded from Update, once the frame's event writes have settled.
static void CheckSwap(ArchiveSwap& swap, bool canEngage) {
	if (!InScope(swap)) {
		if (swap.armed) {
			ReleaseSwap(swap);
		}
		return;
	}
	swap.armed = true;

	auto& actorData = *reinterpret_cast<PlayerActorDataBase*>(swap.actor);
	uint32 event = actorData.eventData[0].event;
	if (swap.engaged) {
		if (event != swap.engagedEvent) {
			ReleaseSwap(swap);
		}
	} else if (canEngage && IsPlayingGroup(actorData, swap.group)) {
		swap.engaged = true;
		swap.engagedEvent = event;
	}
}

static void ReleaseActorSwaps(byte8* actorBaseAddr, bool armedOnly) {
	for (auto& swap : s_swaps) {
		if ((swap.actor == actorBaseAddr) && (!armedOnly || swap.armed)) {
			ReleaseSwap(swap);
		}
	}
}

static bool StartSwap(byte8* actorBaseAddr, uint32 group, byte8* archive, std::initializer_list<uint8> actions,
	uint32 event) {
	if (!actorBaseAddr || !archive || (group >= MOTION_GROUP_COUNT)) {
		return false;
	}
	auto& actorData = *reinterpret_cast<PlayerActorDataBase*>(actorBaseAddr);

	auto* swap = FindSwap(actorBaseAddr, group);
	byte8* originalArchive = swap ? swap->originalArchive : actorData.motionArchives[group];
	if (!swap) {
		swap = FindSwap(nullptr, 0);
		if (!swap) {
			return false;
		}
	}

	*swap = {};
	swap->actor = actorBaseAddr;
	swap->group = group;
	swap->archive = archive;
	swap->originalArchive = originalArchive;
	for (auto action : actions) {
		if (swap->actionCount == MAX_SCOPE_ACTIONS) {
			break;
		}
		swap->actions[swap->actionCount++] = action;
	}
	swap->event = event;
	swap->armed = InScope(*swap);

	actorData.motionArchives[group] = archive;
	return true;
}

static BodyPartData* GetBodyPart(byte8* actorBaseAddr, uint8 part) {
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
	auto model = actorData.activeModelIndex;
	if (activeConfig.Actor.enable) {
		return (model < 7) ? &actorData.newBodyPartData[model][part] : nullptr;
	}
	return (model < 3) ? &actorData.bodyPartData[model][part] : nullptr;
}

uint32 GetMotionStatus(byte8* actorBaseAddr, uint32 part) {
	if (!actorBaseAddr) {
		return 0;
	}
	return CrimsonReversedCalls::CPlGetMotionStatus_sub_1401EEF00((uintptr_t)actorBaseAddr, part);
}

bool IsMotionFinished(byte8* actorBaseAddr) {
	if (GetMotionStatus(actorBaseAddr, 0) == MOTION_FINISHED) {
		return true;
	}
	auto* bodyPart = GetBodyPart(actorBaseAddr, LOWER_BODY);
	return bodyPart && bodyPart->ended;
}

void ReturnToNeutral(byte8* actorBaseAddr, uint8 blendFrames) {
	if (!actorBaseAddr) {
		return;
	}
	CrimsonReversedCalls::CPlReturnToNeutral_sub_1401E0030((uintptr_t)actorBaseAddr, blendFrames);
}

bool Play(byte8* actorBaseAddr, uint32 group, uint32 index, byte8* archive, const PlayOptions& options) {
	if (!actorBaseAddr || group >= MOTION_GROUP_COUNT) {
		return false;
	}
	auto& actorData = *reinterpret_cast<PlayerActorDataBase*>(actorBaseAddr);

	if (auto* previous = FindSlot(actorBaseAddr)) {
		ReleaseSlot(*previous);
	}
	auto* slot = FindSlot(nullptr);
	if (!slot) {
		return false;
	}

	byte8* originalArchive = actorData.motionArchives[group];
	if (archive) {
		// The clip takes over a swap on the same group, so it restores the real original.
		if (auto* swap = FindSwap(actorBaseAddr, group)) {
			originalArchive = swap->originalArchive;
			*swap = {};
		}
		actorData.motionArchives[group] = archive;
	}

	uint8 blendFlag = options.blendFrames ? 2 : 0;
	uint8 blendFrames = options.blendFrames ? options.blendFrames : 0xFF;
	CrimsonReversedCalls::PlayAnimation_sub_1401EFB90((uintptr_t)actorBaseAddr, group, index, options.initialFrame,
		options.loopMode, blendFlag, blendFrames);

	// The idle event handler plays idle as soon as a leftover motionTimer runs out.
	if (options.returnToIdle) {
		actorData.motionTimer = 0.0f;
	}

	slot->actor = actorBaseAddr;
	slot->group = group;
	slot->index = index;
	slot->startEvent = actorData.eventData[0].event;
	slot->archive = archive;
	slot->originalArchive = originalArchive;
	slot->options = options;
	return true;
}

void Stop(byte8* actorBaseAddr) {
	if (auto* slot = FindSlot(actorBaseAddr)) {
		ReleaseSlot(*slot);
	}
	if (actorBaseAddr) {
		ReleaseActorSwaps(actorBaseAddr, false);
	}
}

void OnNewAction(byte8* actorBaseAddr) {
	// Swaps made after this point (Drive in SetAction) belong to the new move. Unarmed ones haven't started yet.
	if (actorBaseAddr) {
		ReleaseActorSwaps(actorBaseAddr, true);
	}
}

bool IsPlaying(byte8* actorBaseAddr) {
	return actorBaseAddr && FindSlot(actorBaseAddr);
}

bool SwapArchive(byte8* actorBaseAddr, uint32 group, byte8* archive, std::initializer_list<uint8> actions) {
	return StartSwap(actorBaseAddr, group, archive, actions, ANY_EVENT);
}

bool SwapArchiveForEvent(byte8* actorBaseAddr, uint32 group, byte8* archive) {
	if (!actorBaseAddr) {
		return false;
	}
	auto& actorData = *reinterpret_cast<PlayerActorDataBase*>(actorBaseAddr);
	return StartSwap(actorBaseAddr, group, archive, {}, actorData.eventData[0].event);
}

void RestoreArchive(byte8* actorBaseAddr, uint32 group) {
	if (!actorBaseAddr) {
		return;
	}
	if (auto* swap = FindSwap(actorBaseAddr, group)) {
		ReleaseSwap(*swap);
	}
}

void Reset() {
	s_slots.fill({});
	s_swaps.fill({});
}

static void UpdateClip(byte8* actorBaseAddr) {
	auto* slot = FindSlot(actorBaseAddr);
	if (!slot) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorDataBase*>(actorBaseAddr);

	auto& motion = actorData.motionData[0];
	if ((motion.group != slot->group) || (motion.index != slot->index) ||
		(actorData.eventData[0].event != slot->startEvent)) {
		ReleaseSlot(*slot);
		return;
	}

	if (!IsMotionFinished(actorBaseAddr)) {
		if (slot->options.returnToIdle) {
			actorData.motionTimer = 0.0f;
		}
		return;
	}

	bool returnToIdle = slot->options.returnToIdle;
	uint8 returnBlend = slot->options.returnBlend;
	ReleaseSlot(*slot);

	if (returnToIdle && (actorData.eventData[0].event == ACTOR_EVENT::IDLE)) {
		ReturnToNeutral(actorBaseAddr, returnBlend);
	}
}

void Update(byte8* actorBaseAddr) {
	if (!actorBaseAddr) {
		return;
	}
	for (auto& swap : s_swaps) {
		if (swap.actor != actorBaseAddr) {
			continue;
		}
		CheckSwap(swap, true);
		// A move that never started (the action never showed up) must not keep the bank.
		if (swap.actor && !swap.armed && (++swap.unarmedTicks > MAX_UNARMED_TICKS)) {
			ReleaseSwap(swap);
		}
	}
	UpdateClip(actorBaseAddr);
}

void OnPlayAnimation(byte8* actorBaseAddr, uint32 group, uint32 index) {
	if (!actorBaseAddr) {
		return;
	}
	// Anything else starting an animation interrupts our clip; restore the archive before the game reads it.
	if (auto* slot = FindSlot(actorBaseAddr)) {
		ReleaseSlot(*slot);
	}
	// A new move or an interrupting event must not bind its first chunk out of the previous move's archive.
	for (auto& swap : s_swaps) {
		if (swap.actor == actorBaseAddr) {
			CheckSwap(swap, false);
		}
	}
}

}

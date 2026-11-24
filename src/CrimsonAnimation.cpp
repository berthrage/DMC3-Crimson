#include "CrimsonAnimation.hpp"

#include <array>

#include "Config.hpp"
#include "Core/Core.hpp"
#include "CrimsonReversedCalls.hpp"
#include "Vars.hpp"

namespace CrimsonAnimation {

static constexpr uint32 MOTION_FINISHED = 4;
static constexpr uint32 MOTION_GROUP_COUNT = sizeof(PlayerActorDataBase::motionArchives) / sizeof(byte8*);

struct AnimationSlot {
	byte8* actor = nullptr;
	uint32 group = 0;
	uint32 index = 0;
	uint32 startEvent = 0;
	byte8* archive = nullptr;
	byte8* originalArchive = nullptr;
	PlayOptions options;
};

static std::array<AnimationSlot, 16> s_slots;

static AnimationSlot* FindSlot(byte8* actorBaseAddr) {
	for (auto& slot : s_slots) {
		if (slot.actor == actorBaseAddr) {
			return &slot;
		}
	}
	return nullptr;
}

static AnimationSlot* AllocSlot(byte8* actorBaseAddr) {
	if (auto* slot = FindSlot(actorBaseAddr)) {
		return slot;
	}
	return FindSlot(nullptr);
}

// Only puts the archive back if nobody replaced it meanwhile (the taunt bank detour swaps the same group).
static void ReleaseSlot(AnimationSlot& slot) {
	if (slot.actor && slot.archive && slot.group < MOTION_GROUP_COUNT) {
		auto& actorData = *reinterpret_cast<PlayerActorDataBase*>(slot.actor);
		if (actorData.motionArchives[slot.group] == slot.archive) {
			actorData.motionArchives[slot.group] = slot.originalArchive;
		}
	}
	slot = {};
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
	auto* slot = AllocSlot(actorBaseAddr);
	if (!slot) {
		return false;
	}

	byte8* originalArchive = actorData.motionArchives[group];
	if (archive) {
		actorData.motionArchives[group] = archive;
	}

	uint8 blendFlag = options.blendFrames ? 2 : 0;
	CrimsonReversedCalls::PlayAnimation_sub_1401EFB90((uintptr_t)actorBaseAddr, group, index, options.initialFrame,
		options.loopMode, blendFlag, options.blendFrames);

	// The idle event handler plays idle as soon as a leftover motionTimer runs out.
	actorData.motionTimer = 0.0f;

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
}

bool IsPlaying(byte8* actorBaseAddr) {
	return actorBaseAddr && FindSlot(actorBaseAddr);
}

void Update(byte8* actorBaseAddr) {
	if (!actorBaseAddr) {
		return;
	}
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
		actorData.motionTimer = 0.0f;
		return;
	}

	bool returnToIdle = slot->options.returnToIdle;
	uint8 returnBlend = slot->options.returnBlend;
	ReleaseSlot(*slot);

	if (returnToIdle && (actorData.eventData[0].event == ACTOR_EVENT::IDLE)) {
		ReturnToNeutral(actorBaseAddr, returnBlend);
	}
}

void OnPlayAnimation(byte8* actorBaseAddr, uint32 group, uint32 index) {
	if (!actorBaseAddr) {
		return;
	}
	// Anything else starting an animation interrupts ours; restore the archive before the game reads it.
	if (auto* slot = FindSlot(actorBaseAddr)) {
		ReleaseSlot(*slot);
	}
}

}

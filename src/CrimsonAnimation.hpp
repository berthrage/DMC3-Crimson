#pragma once
#include "Core/DataTypes.hpp"

// Plays custom animations on player actors, optionally out of a swapped motion archive.
// The swapped archive stays in place until the clip ends or something else interrupts it,
// because the game re-reads motion chunks from the archive mid-clip.
namespace CrimsonAnimation {

struct PlayOptions {
	float initialFrame = -1.0f; // -1 = start of the clip
	uint8 loopMode = 0xFF;      // 0xFF = the clip's own setting
	uint8 blendFrames = 0;      // 0 = snap, otherwise blend from the current pose over N frames
	bool returnToIdle = true;   // play the neutral idle when the clip ends while still in the idle event
	uint8 returnBlend = 10;
};

// Plays group/index on the actor. archive (optional) replaces motionArchives[group] for the whole clip.
bool Play(byte8* actorBaseAddr, uint32 group, uint32 index, byte8* archive = nullptr, const PlayOptions& options = {});
// Restores the archive and forgets the clip, without touching the current animation.
void Stop(byte8* actorBaseAddr);
bool IsPlaying(byte8* actorBaseAddr);
// Once per actor per frame. Finishes the clip (archive restore + return to idle) or drops it when interrupted.
void Update(byte8* actorBaseAddr);

// Game motion helpers.
uint32 GetMotionStatus(byte8* actorBaseAddr, uint32 part = 0); // 4 = finished
bool IsMotionFinished(byte8* actorBaseAddr);
void ReturnToNeutral(byte8* actorBaseAddr, uint8 blendFrames = 10);

// Called from the PlayAnimation_sub_1401EFB90 detour before the game function runs.
void OnPlayAnimation(byte8* actorBaseAddr, uint32 group, uint32 index);

}

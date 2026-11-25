#pragma once
#include <initializer_list>

#include "Core/DataTypes.hpp"

// Custom animations on player actors.
// - Play: one clip, optionally out of a swapped motion archive that is kept until the clip ends,
//   because the game re-reads motion chunks from the archive mid-clip.
// - SwapArchive: a swapped archive kept for a whole move, while the game's own events play the clips.
namespace CrimsonAnimation {

struct PlayOptions {
	float initialFrame = -1.0f; // -1 = start of the clip
	uint8 loopMode = 0xFF;      // 0xFF = the clip's own setting
	uint8 blendFrames = 0;      // 0 = hard cut, otherwise blend from the current pose over N frames
	bool returnToIdle = true;   // idle clip: holds the idle timer off and plays the neutral idle when it ends.
	                            // false for clips that run inside their own event (attacks).
	uint8 returnBlend = 10;
};

// Plays group/index on the actor. archive (optional) replaces motionArchives[group] for the whole clip.
bool Play(byte8* actorBaseAddr, uint32 group, uint32 index, byte8* archive = nullptr, const PlayOptions& options = {});
// Restores the archives (clip and swaps) and forgets them, without touching the current animation. For cutscenes.
void Stop(byte8* actorBaseAddr);
bool IsPlaying(byte8* actorBaseAddr);
// Once per actor per frame. Finishes the clip (archive restore + return to idle) or drops it when interrupted,
// and ends archive swaps whose move is over.
void Update(byte8* actorBaseAddr);

// Puts archive in motionArchives[group] for one move, restored automatically when it is over:
// - actor.action leaves actions (up to 4),
// - a new action starts (OnNewAction), even the same one again,
// - the event changes from the one the move started in (finished, trick, jump, damage...),
// - Stop (cutscene) or Reset.
// Swapping the same group again keeps the first original, so a multi-part move restores to vanilla.
// Scope checks only start once the actor has been seen in scope, so the action may be set after the swap.
bool SwapArchive(byte8* actorBaseAddr, uint32 group, byte8* archive, std::initializer_list<uint8> actions);
// Same, scoped to the actor's current event instead of the action.
bool SwapArchiveForEvent(byte8* actorBaseAddr, uint32 group, byte8* archive);
// Puts the original archive back early, if the swapped one is still in place. Only for odd cancels.
void RestoreArchive(byte8* actorBaseAddr, uint32 group);
// Forgets every clip and swap without touching any actor. For when the actors go away.
void Reset();

// Game motion helpers.
uint32 GetMotionStatus(byte8* actorBaseAddr, uint32 part = 0); // 4 = finished
bool IsMotionFinished(byte8* actorBaseAddr);
void ReturnToNeutral(byte8* actorBaseAddr, uint8 blendFrames = 10);

// Called from the PlayAnimation_sub_1401EFB90 detour before the game function runs.
void OnPlayAnimation(byte8* actorBaseAddr, uint32 group, uint32 index);
// Called right after the game writes a new melee/style action, before Crimson changes it.
void OnNewAction(byte8* actorBaseAddr);

}

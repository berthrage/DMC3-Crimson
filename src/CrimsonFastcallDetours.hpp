#pragma once
#include <cstdint>
namespace CrimsonFastcallDetours {
	void ModdedTauntDetour();
	void ModdedPlaySFXWithPosByTypeDetour();
	void InitDetours();
	// Super Judgement Cut
	bool SjcCanStart(uintptr_t actorBaseAddr); // on the ground, no Doppelganger, enough Devil Trigger gauge
	bool SjcIsActive();
	void SjcStart(uintptr_t actorBaseAddr);    // the taunt event carries the animations, the taunt hook plays the phases
	void SjcPoll(uintptr_t actorBaseAddr);     // every tick while the move is on: notices its end and puts everything back

}

//void LdkDrawImGuiWidget();

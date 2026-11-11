#include "../ThirdParty/glm/glm.hpp"
#pragma once
#include <cstdint>
#include "Vars.hpp"

namespace CrimsonHighFPSFixes {

	void ClothPhysicsFixesController();
	void ClothPhysicsEnhancementFixes(bool enable);
	void LookAtBossCamFixes(bool enable);
	void ToggleAllFixes(bool enable);
	void FixBloodgoyleStormFormTime(bool enable);
	void FixCPl021Shl01SummonedSwordsInitialTravel(bool enable);
	void FixWpNunchakuCerberusCollisions(bool enable);
	void FixDoppelgangerInputDelay(bool enable);
	uint64_t DoppelgangerInputHistoryPush(byte8* ring, byte8* input);
	byte8* DoppelgangerInputHistoryGetDelayed(byte8* ring, int32 offset);
	void DoppelgangerInputHistoryReset(byte8* ring);
}
#pragma once
#include <cstdint>
#include "Vars.hpp"

extern "C" {
	// SampleMod
	void SampleModDetour1();

	// GuardGravity
	void GuardGravityDetour();

	// EnableAirTaunt
	void EnableAirTauntDetour();

	// SetAirTaunt
	void SetAirTauntDetour();

	// CreateEffect
	extern int createEffectBank;
	extern int createEffectID;
	void CreateEffectDetour();

	// HoldToCrazyCombo
	void HoldToCrazyComboDetour();

	// DisableDriveHold
	void DisableDriveHoldDetour();

	// HudHPSeparation
	void HudHPSeparationDetour();

	// HudStyleBarPos
	void HudStyleBarPosDetour();

	// ShootRemapDown
	void ShootRemapDownDetour();
	void ShootRemapHoldDetour();

	// VergilNeutralTrick
	void VergilNeutralTrickDetour();
}

bool g_HoldToCrazyComboFuncA(PlayerActorData& actorData);
void InitDetours();

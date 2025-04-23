#include "CrimsonConfigGameplayMask.hpp"

CrimsonConfigGameplayMask VANILLA_PRESET_MASK;
CrimsonConfigGameplayMask STYLE_SWITCHER_PRESET_MASK;
CrimsonConfigGameplayMask CRIMSON_PRESET_MASK;

CrimsonConfigGameplayMask CreateVanillaPresetMask() {
	CrimsonConfigGameplayMask mask{};

	// For Vanilla, we'll check most gameplay settings but ignore some
	auto& general = mask.Gameplay.General;
	general.inertia = true;
	general.sprint = true;
	general.freeformSoftLock = true;
	general.bufferlessReversals = true;
	general.dmc4LockOnDirection = true;
	general.holdToCrazyCombo = false; // Optional
	general.crazyComboMashRequirement = false; // Optional
	general.disableHeightRestriction = true;
	general.improvedBufferedReversals = false; // Optional
	general.increasedJCSpheres = true;
	general.disableJCRestriction = true;
	general.increasedEnemyJuggleTime = true;
	general.fasterTurnRate = false; // Optional

	// Define similar settings for other sections
	// [...]

	return mask;
}

CrimsonConfigGameplayMask CreateStyleSwitcherPresetMask() {
	CrimsonConfigGameplayMask mask{};

	// Define which settings are required for Style Switcher
	// [...]

	return mask;
}

CrimsonConfigGameplayMask CreateCrimsonPresetMask() {
	CrimsonConfigGameplayMask mask{};

	// Define which settings are required for Crimson
	// [...]

	return mask;
}

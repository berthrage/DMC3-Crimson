#include <thread>
#include <chrono>
#include <algorithm>
#include "../ThirdParty/glm/glm.hpp"

#include "Core/Core.hpp"
#include "Graphics.hpp"
#include "Actor.hpp"
#include "ActorBase.hpp"
#include "ActorRelocations.hpp"
#include "Config.hpp"
#include "Exp.hpp"
#include "Global.hpp"
#include "Vars.hpp"
#include "Speed.hpp"

#include "Core/Macros.h"
#include "Sound.hpp"
#include "CrimsonSDL.hpp"
#include "CrimsonPatches.hpp"
#include "Camera.hpp"
#include "CrimsonDetours.hpp"
#include "CrimsonUtil.hpp"
#include "CrimsonTimers.hpp"
#include "CrimsonConfigGameplay.hpp"
#include "CrimsonGameModes.hpp"
#include "CrimsonConfigGameplayMask.hpp"

namespace CrimsonGameModes {

bool ConfigsMatch(const CrimsonConfigGameplay& config1, const CrimsonConfigGameplay& config2) {
	// Compare both Gameplay and Cheats structures fully
	// Note: We're excluding GameMode.preset itself from comparison as it's what we're determining
	bool gameplayMatch = CompareStructs(config1.Gameplay, config2.Gameplay);
	bool cheatsMatch = CompareStructs(config1.Cheats, config2.Cheats);


	return gameplayMatch && cheatsMatch;
}

bool ConfigsMatchMasked(const CrimsonConfigGameplay& config1, const CrimsonConfigGameplay& config2, const CrimsonConfigGameplayMask& mask) {
	return CompareStructsMasked(config1, config2, mask);
}

CrimsonConfigGameplay CreateVanillaPreset() {
	CrimsonConfigGameplay preset{};

	// Values matching expected vanilla behavior
	auto& general = preset.Gameplay.General;
	general.inertia = false;
	general.sprint = false;
	general.freeformSoftLock = false;
	general.bufferlessReversals = false;
	general.dmc4LockOnDirection = false;
	general.holdToCrazyCombo = false;
	general.disableHeightRestriction = false;
	general.improvedBufferedReversals = false;
	general.increasedJCSpheres = false;
	general.disableJCRestriction = false;
	general.increasedEnemyJuggleTime = false;
	general.fasterTurnRate = false;

	// Dante section
	auto& dante = preset.Gameplay.Dante;
	dante.improvedCancels = false;
	dante.bulletStop = false;
	dante.rainstormLift = false;
	dante.infiniteRainstorm = false;
	dante.foursomeTime = false;
	dante.aerialRaveTweaks = false;
	dante.airFlickerTweaks = false;
	dante.skyDanceTweaks = false;
	dante.shotgunAirShotTweaks = false;
	dante.driveTweaks = false;
	dante.disableAirSlashKnockback = false;
	dante.airStinger = false;
	dante.airRevolver = false;
	dante.airTornado = false;
	dante.airRisingDragonWhirlwind = false;
	dante.airAgniRudraWhirlwind = false;
	dante.dmc4Mobility = false;
	dante.dTInfusedRoyalguard = false;
	dante.airHikeCoreAbility = false;
	dante.altNevanVortex = false;
	dante.artemisRework = false;
	dante.swapDancePierceInputs = false;

	// Vergil section
	auto& vergil = preset.Gameplay.Vergil;
	vergil.darkslayerTrickCancels = false;
	vergil.fasterDTRapidSlash = false;
	vergil.roundTripTweaks = false;
	vergil.airStinger = false;
	vergil.airRisingSun = false;
	vergil.airLunarPhase = false;
	vergil.altJudgementCutInput = false;
	//vergil.adjustRisingSunPos = "Off"; // or whatever vanilla used
	//vergil.adjustLunarPhasePos = "Off";

	// Cheats: disabled
	preset.Cheats = defaultCrimsonGameplay.Cheats;

	// Game mode preset marker
	preset.GameMode.preset = GAMEMODEPRESETS::VANILLA;

	return preset;
}

CrimsonConfigGameplay CreateStyleSwitcherPreset() {
	CrimsonConfigGameplay preset{};

	auto& general = preset.Gameplay.General;
	general.inertia = false;
	general.sprint = true;
	general.freeformSoftLock = true;
	//general.bufferlessReversals = false;
	general.dmc4LockOnDirection = true;
	general.disableHeightRestriction = true;
	//general.improvedBufferedReversals = false;
	general.increasedJCSpheres = false;
	general.disableJCRestriction = false;
	general.increasedEnemyJuggleTime = false;
	//general.fasterTurnRate = false;

	// Dante section
	auto& dante = preset.Gameplay.Dante;
	dante.improvedCancels = true;
	dante.bulletStop = true;
	dante.rainstormLift = false;
	dante.infiniteRainstorm = true;
	dante.foursomeTime = true;
	dante.aerialRaveTweaks = false;
	dante.airFlickerTweaks = false;
	dante.skyDanceTweaks = false;
	dante.shotgunAirShotTweaks = false;
	dante.driveTweaks = false;
	dante.disableAirSlashKnockback = false;
	dante.airStinger = false;
	dante.airRevolver = true;
	dante.airTornado = true;
	dante.airRisingDragonWhirlwind = false;
	dante.airAgniRudraWhirlwind = false;
	dante.dmc4Mobility = false;
	dante.dTInfusedRoyalguard = false;
	dante.airHikeCoreAbility = true;
	dante.altNevanVortex = true;
	dante.artemisRework = true;
	//dante.swapDancePierceInputs = false;

	// Vergil section
	auto& vergil = preset.Gameplay.Vergil;
	vergil.darkslayerTrickCancels = true;
	vergil.fasterDTRapidSlash = false;
	vergil.roundTripTweaks = true;
	vergil.airStinger = true;
	vergil.airRisingSun = true;
	vergil.airLunarPhase = true;
	vergil.altJudgementCutInput = true;
	//vergil.adjustRisingSunPos = "Off"; // or whatever vanilla used
	//vergil.adjustLunarPhasePos = "Off";

	// Cheats section
	preset.Cheats.General = defaultCrimsonGameplay.Cheats.General;
	preset.Cheats.Damage = defaultCrimsonGameplay.Cheats.Damage;
	preset.Cheats.Training = defaultCrimsonGameplay.Cheats.Training;
	auto& cheatsVergil = preset.Cheats.Vergil;
	cheatsVergil.airRisingSunCount[0] = defaultCrimsonGameplay.Cheats.Vergil.airRisingSunCount[0];
	cheatsVergil.airRisingSunCount[1] = defaultCrimsonGameplay.Cheats.Vergil.airRisingSunCount[1];
	cheatsVergil.judgementCutCount[0] = defaultCrimsonGameplay.Cheats.Vergil.judgementCutCount[0];
	cheatsVergil.judgementCutCount[1] = defaultCrimsonGameplay.Cheats.Vergil.judgementCutCount[1];
	cheatsVergil.quicksilverStyle = defaultCrimsonGameplay.Cheats.Vergil.quicksilverStyle;
	cheatsVergil.infiniteRoundTrip = defaultCrimsonGameplay.Cheats.Vergil.infiniteRoundTrip;
	preset.Cheats.Misc = defaultCrimsonGameplay.Cheats.Misc;
	preset.Cheats.Speed = defaultCrimsonGameplay.Cheats.Speed;
	preset.Cheats.Dante = defaultCrimsonGameplay.Cheats.Dante;
	preset.Cheats.Mobility = defaultCrimsonGameplay.Cheats.Mobility;

	// Game mode preset marker
	preset.GameMode.preset = GAMEMODEPRESETS::STYLE_SWITCHER;

	return preset;
}

CrimsonConfigGameplay CreateCrimsonPreset() {
	CrimsonConfigGameplay preset{};

	auto& general = preset.Gameplay.General;
	general.inertia = true;
	general.sprint = true;
	general.freeformSoftLock = true;
	//general.bufferlessReversals = false;
	general.dmc4LockOnDirection = true;
	general.holdToCrazyCombo = true;
	general.crazyComboMashRequirement = 3;
	general.disableHeightRestriction = true;
	general.improvedBufferedReversals = true;
	general.increasedJCSpheres = true;
	general.disableJCRestriction = true;
	general.increasedEnemyJuggleTime = true;
	//general.fasterTurnRate = false;

	// Dante section
	auto& dante = preset.Gameplay.Dante;
	dante.improvedCancels = true;
	dante.bulletStop = true;
	dante.rainstormLift = true;
	dante.infiniteRainstorm = true;
	dante.foursomeTime = true;
	dante.aerialRaveTweaks = true;
	dante.airFlickerTweaks = true;
	dante.skyDanceTweaks = true;
	dante.shotgunAirShotTweaks = true;
	dante.driveTweaks = true;
	dante.disableAirSlashKnockback = true;
	dante.airStinger = true;
	dante.airRevolver = true;
	dante.airTornado = true;
	dante.airRisingDragonWhirlwind = true;
	dante.airAgniRudraWhirlwind = true;
	dante.dmc4Mobility = true;
	dante.dTInfusedRoyalguard = true;
	dante.airHikeCoreAbility = true;
	dante.altNevanVortex = true;
	dante.artemisRework = true;
	//dante.swapDancePierceInputs = false;

	// Vergil section
	auto& vergil = preset.Gameplay.Vergil;
	vergil.darkslayerTrickCancels = true;
	vergil.fasterDTRapidSlash = true;
	vergil.roundTripTweaks = true;
	vergil.airStinger = true;
	vergil.airRisingSun = true;
	vergil.airLunarPhase = true;
	vergil.altJudgementCutInput = true;
	//vergil.adjustRisingSunPos = "Off"; // or whatever vanilla used
	//vergil.adjustLunarPhasePos = "Off";

	preset.Cheats.General = defaultCrimsonGameplay.Cheats.General;
	preset.Cheats.Damage = defaultCrimsonGameplay.Cheats.Damage;
	preset.Cheats.Training = defaultCrimsonGameplay.Cheats.Training;
	auto& cheatsVergil = preset.Cheats.Vergil;
	cheatsVergil.airRisingSunCount[0] = defaultCrimsonGameplay.Cheats.Vergil.airRisingSunCount[0];
	cheatsVergil.airRisingSunCount[1] = defaultCrimsonGameplay.Cheats.Vergil.airRisingSunCount[1];
	cheatsVergil.judgementCutCount[0] = defaultCrimsonGameplay.Cheats.Vergil.judgementCutCount[0];
	cheatsVergil.judgementCutCount[1] = defaultCrimsonGameplay.Cheats.Vergil.judgementCutCount[1];
	cheatsVergil.quicksilverStyle = defaultCrimsonGameplay.Cheats.Vergil.quicksilverStyle;
	cheatsVergil.infiniteRoundTrip = defaultCrimsonGameplay.Cheats.Vergil.infiniteRoundTrip;
	preset.Cheats.Misc = defaultCrimsonGameplay.Cheats.Misc;
	preset.Cheats.Speed = defaultCrimsonGameplay.Cheats.Speed;
	preset.Cheats.Dante = defaultCrimsonGameplay.Cheats.Dante;
	auto& mobility = preset.Cheats.Mobility;
	mobility.airHikeCount[0] = 1;  mobility.airHikeCount[1] = 2;
	mobility.kickJumpCount[0] = 1; mobility.kickJumpCount[1] = 1;
	mobility.wallHikeCount[0] = 1; mobility.wallHikeCount[1] = 2;
	mobility.dashCount[0] = 3;     mobility.dashCount[1] = 3;
	mobility.skyStarCount[0] = 1;  mobility.skyStarCount[1] = 2;
	mobility.danteAirTrickCount[0] = 1;  mobility.danteAirTrickCount[1] = 2;
	mobility.vergilAirTrickCount[0] = 1; mobility.vergilAirTrickCount[1] = 1;
	mobility.trickUpCount[0] = 1;  mobility.trickUpCount[1] = 1;
	mobility.trickDownCount[0] = 1; mobility.trickDownCount[1] = 1;

	// Game mode preset marker
	preset.GameMode.preset = GAMEMODEPRESETS::CRIMSON;

	return preset;
}

static const CrimsonConfigGameplay VANILLA_PRESET = CreateVanillaPreset();
static const CrimsonConfigGameplay STYLE_SWITCHER_PRESET = CreateStyleSwitcherPreset();
static const CrimsonConfigGameplay CRIMSON_PRESET = CreateCrimsonPreset();

// Mask for Vanilla preset: all fields checked except these (set to false)
const CrimsonConfigGameplayMask VANILLA_MASK = [] {
	CrimsonConfigGameplayMask mask{};
	// Example: these fields are optional for Vanilla
	mask.Gameplay.General.crazyComboMashRequirement = false;
	mask.Gameplay.General.vanillaWeaponSwitchDelay = false;

	mask.GameMode.preset = false; // Ignore preset field in matching!
	return mask;
	}();

// Mask for Style Switcher preset: all fields checked except these
const CrimsonConfigGameplayMask STYLE_SWITCHER_MASK = [] {
	CrimsonConfigGameplayMask mask{};
	mask.Gameplay.General.holdToCrazyCombo = false;
	mask.Gameplay.General.crazyComboMashRequirement = false;
	mask.Gameplay.General.bufferlessReversals = false;
	mask.Cheats.Vergil.chronoSwords = false;

	mask.GameMode.preset = false; // Ignore preset field in matching!
	return mask;
	}();

// Mask for Crimson preset: all fields checked except these
const CrimsonConfigGameplayMask CRIMSON_MASK = [] {
	CrimsonConfigGameplayMask mask{};
	mask.Gameplay.General.crazyComboMashRequirement = false;
	mask.Gameplay.General.bufferlessReversals = false;
	mask.Cheats.Vergil.chronoSwords = false;

	
	mask.GameMode.preset = false; // Ignore preset field in matching!
	return mask;
	}();


void CrimsonGameModes::SetGameMode(uint8 mode) {
	switch (mode) {
	case GAMEMODEPRESETS::VANILLA:
		AssignMembersMasked(activeCrimsonGameplay, VANILLA_PRESET, VANILLA_MASK);
		AssignMembersMasked(queuedCrimsonGameplay, VANILLA_PRESET, VANILLA_MASK);
		queuedConfig.Actor.enable = false;
		break;
	case GAMEMODEPRESETS::STYLE_SWITCHER:
		AssignMembersMasked(activeCrimsonGameplay, STYLE_SWITCHER_PRESET, STYLE_SWITCHER_MASK);
		AssignMembersMasked(queuedCrimsonGameplay, STYLE_SWITCHER_PRESET, STYLE_SWITCHER_MASK);
		queuedConfig.Actor.enable = true;
		break;
	case GAMEMODEPRESETS::CRIMSON:
		AssignMembersMasked(activeCrimsonGameplay, CRIMSON_PRESET, CRIMSON_MASK);
		AssignMembersMasked(queuedCrimsonGameplay, CRIMSON_PRESET, CRIMSON_MASK);
		queuedConfig.Actor.enable = true;
		break;
	default:
		break;
	}
	// Set the preset field explicitly
	activeCrimsonGameplay.GameMode.preset = mode;
	queuedCrimsonGameplay.GameMode.preset = mode;
}

void CrimsonGameModes::TrackGameMode() {
	// Store the current preset value before any changes
	uint8 currentActivePreset = activeCrimsonGameplay.GameMode.preset;

	// Temporarily set to CUSTOM to prevent false matches on the preset field
	activeCrimsonGameplay.GameMode.preset = GAMEMODEPRESETS::CUSTOM;

	// Use masked matching for each preset
	bool matchesVanilla = ConfigsMatchMasked(activeCrimsonGameplay, VANILLA_PRESET, VANILLA_MASK);
	bool matchesStyleSwitcher = ConfigsMatchMasked(activeCrimsonGameplay, STYLE_SWITCHER_PRESET, STYLE_SWITCHER_MASK);
	bool matchesCrimson = ConfigsMatchMasked(activeCrimsonGameplay, CRIMSON_PRESET, CRIMSON_MASK);

	// Restore the original preset value
	activeCrimsonGameplay.GameMode.preset = currentActivePreset;

	// Determine which preset it matches, if any
	uint8 matchedPreset = GAMEMODEPRESETS::CUSTOM;
	if (matchesVanilla) {
		matchedPreset = GAMEMODEPRESETS::VANILLA;
	} else if (matchesStyleSwitcher) {
		matchedPreset = GAMEMODEPRESETS::STYLE_SWITCHER;
	} else if (matchesCrimson) {
		matchedPreset = GAMEMODEPRESETS::CRIMSON;
	}

	// If the matched preset differs from the current preset, update it
	if (matchedPreset != currentActivePreset) {
		if (matchedPreset != GAMEMODEPRESETS::CUSTOM) {
			SetGameMode(matchedPreset);
		} else {
			activeCrimsonGameplay.GameMode.preset = GAMEMODEPRESETS::CUSTOM;
			queuedCrimsonGameplay.GameMode.preset = GAMEMODEPRESETS::CUSTOM;
		}
	}
}

}
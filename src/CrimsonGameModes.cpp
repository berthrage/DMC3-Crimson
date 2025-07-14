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
	general.disableSoulEaterInvis = false;

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
	dante.downertiaFromAirFlickerSkyDance = false;
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
	dante.swapArtemisMultiLockNormalShot = false;

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
	general.holdToCrazyCombo = true;
	general.dmc4LockOnDirection = true;
	general.increasedJCSpheres = false;
	general.disableJCRestriction = true;
	general.increasedEnemyJuggleTime = false;
	general.disableSoulEaterInvis = false;
	general.disableHeightRestriction = true;
	general.fasterTurnRate = true;


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
	dante.downertiaFromAirFlickerSkyDance = false;
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
	vergil.yamatoRisingStar = false;
	vergil.mirageTrigger = false;
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
	general.disableSoulEaterInvis = true;
	general.fasterTurnRate = true;

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
	dante.downertiaFromAirFlickerSkyDance = true;
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
	dante.swapDancePierceInputs = true;

	// Vergil section
	auto& vergil = preset.Gameplay.Vergil;
	vergil.darkslayerTrickCancels = true;
	vergil.fasterDTRapidSlash = true;
	vergil.roundTripTweaks = true;
	vergil.airStinger = true;
	vergil.airRisingSun = true;
	vergil.airLunarPhase = true;
	vergil.altJudgementCutInput = true;
	vergil.yamatoRisingStar = true;
	vergil.mirageTrigger = true;
	vergil.yamatoHighTime = true;
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
	mask.Gameplay.General.charHotswap = false;
	mask.Gameplay.General.crazyComboMashRequirement = false;
	mask.Gameplay.General.holdToShoot = false;
	mask.Gameplay.General.vanillaWeaponSwitchDelay = false;
	mask.Gameplay.Dante.swapArtemisMultiLockNormalShot = false;

	mask.Gameplay.ExtraDifficulty.ldkMode = false;
	mask.Gameplay.ExtraDifficulty.mustStyleMode = false;
	mask.Gameplay.ExtraDifficulty.enemyDTMode = false;
	mask.Gameplay.ExtraDifficulty.forceDifficultyMode = false;

	mask.Cheats.Training.infiniteHP = false;
	mask.Cheats.Training.infiniteDT = false;
	mask.Cheats.Training.disableTimers = false;
	mask.Cheats.Training.infiniteBossLadyBullets = false;
	mask.Cheats.General.customDamage = false;
	mask.Cheats.Damage.playerReceivedDmgMult = false;
	mask.Cheats.Damage.enemyReceivedDmgMult = false;
	mask.Cheats.General.customSpeed = false;
	mask.Cheats.Speed.enemy = false;
	mask.Cheats.Speed.human = false;
	mask.Cheats.Speed.dTDante = false;
	mask.Cheats.Speed.dTVergil = false;
	mask.Cheats.Speed.quicksilverPlayer = false;
	mask.Cheats.Speed.quicksilverEnemy = false;
	mask.Cheats.General.customMobility = false;
	mask.Cheats.Mobility.airHikeCount = false;
	mask.Cheats.Mobility.kickJumpCount = false;
	mask.Cheats.Mobility.wallHikeCount = false;
	mask.Cheats.Mobility.dashCount = false;
	mask.Cheats.Mobility.skyStarCount = false;
	mask.Cheats.Mobility.danteAirTrickCount = false;
	mask.Cheats.Mobility.vergilAirTrickCount = false;
	mask.Cheats.Mobility.trickUpCount = false;
	mask.Cheats.Mobility.trickDownCount = false;
	mask.Debug.debugTools = false;
	mask.Cheats.General.enemySpawnerTool = false;
	mask.Cheats.General.teleporterTool = false;

	mask.GameMode.preset = false; // Ignore preset field in matching!
	return mask;
	}();

// Mask for Style Switcher preset: all fields checked except these
const CrimsonConfigGameplayMask STYLE_SWITCHER_MASK = [] {
	CrimsonConfigGameplayMask mask{};
	mask.Gameplay.General.charHotswap = false;
	mask.Gameplay.General.holdToCrazyCombo = false;
	mask.Gameplay.General.holdToShoot = false;
	mask.Gameplay.General.crazyComboMashRequirement = false;
	mask.Gameplay.General.bufferlessReversals = false;
	mask.Gameplay.General.dmc4LockOnDirection = false;
	mask.Gameplay.General.fasterTurnRate = false;
	mask.Gameplay.Dante.swapArtemisMultiLockNormalShot = false;
	
	// NEW MOVES are optional
	mask.Gameplay.Dante.airRevolver = false;
	mask.Gameplay.Dante.airTornado = false;
	mask.Gameplay.Dante.airRisingDragonWhirlwind = false;
	mask.Gameplay.Dante.airAgniRudraWhirlwind = false;
	mask.Gameplay.Dante.airStinger = false;
	mask.Gameplay.Vergil.airLunarPhase = false;
	mask.Gameplay.Vergil.airRisingSun = false;
	mask.Gameplay.Vergil.airStinger = false;
	mask.Gameplay.Vergil.yamatoRisingStar = false;
	mask.Gameplay.Vergil.yamatoHighTime = false;

	mask.Gameplay.ExtraDifficulty.ldkMode = false;
	mask.Gameplay.ExtraDifficulty.mustStyleMode = false;
	mask.Gameplay.ExtraDifficulty.enemyDTMode = false;
	mask.Gameplay.ExtraDifficulty.forceDifficultyMode = false;

	mask.Cheats.Vergil.chronoSwords = false;
	mask.Cheats.Training.infiniteHP = false;
	mask.Cheats.Training.infiniteDT = false;
	mask.Cheats.Training.disableTimers = false;
	mask.Cheats.Training.infiniteBossLadyBullets = false;
	mask.Cheats.General.customDamage = false;
	mask.Cheats.Damage.playerReceivedDmgMult = false;
	mask.Cheats.Damage.enemyReceivedDmgMult = false;
	mask.Cheats.General.customSpeed = false;
	mask.Cheats.Speed.enemy = false;
	mask.Cheats.Speed.human = false;
	mask.Cheats.Speed.dTDante = false;
	mask.Cheats.Speed.dTVergil = false;
	mask.Cheats.Speed.quicksilverPlayer = false;
	mask.Cheats.Speed.quicksilverEnemy = false;
	mask.Cheats.General.customMobility = false;
	mask.Cheats.Mobility.airHikeCount = false;
	mask.Cheats.Mobility.kickJumpCount = false;
	mask.Cheats.Mobility.wallHikeCount = false;
	mask.Cheats.Mobility.dashCount = false;
	mask.Cheats.Mobility.skyStarCount = false;
	mask.Cheats.Mobility.danteAirTrickCount = false;
	mask.Cheats.Mobility.vergilAirTrickCount = false;
	mask.Cheats.Mobility.trickUpCount = false;
	mask.Cheats.Mobility.trickDownCount = false;
	mask.Debug.debugTools = false;
	mask.Cheats.General.enemySpawnerTool = false;
	mask.Cheats.General.teleporterTool = false;

	mask.GameMode.preset = false; // Ignore preset field in matching!
	return mask;
	}();

// Mask for Crimson preset: all fields checked except these
const CrimsonConfigGameplayMask CRIMSON_MASK = [] {
	CrimsonConfigGameplayMask mask{};
	mask.Gameplay.General.charHotswap = false;
	mask.Gameplay.General.holdToShoot = false;
	mask.Gameplay.General.crazyComboMashRequirement = false;
	mask.Gameplay.General.bufferlessReversals = false;
	mask.Gameplay.General.fasterTurnRate = false;
	mask.Gameplay.Dante.swapArtemisMultiLockNormalShot = false;

	mask.Gameplay.Dante.downertiaFromAirFlickerSkyDance = false; // This should be optional
	// NEW MOVES are optional
	mask.Gameplay.Dante.airRevolver = false;
	mask.Gameplay.Dante.airTornado = false;
	mask.Gameplay.Dante.airRisingDragonWhirlwind = false;
	mask.Gameplay.Dante.airAgniRudraWhirlwind = false;
	mask.Gameplay.Dante.airStinger = false;
	mask.Gameplay.Vergil.airLunarPhase = false;
	mask.Gameplay.Vergil.airRisingSun = false;
	mask.Gameplay.Vergil.airStinger = false;
	mask.Gameplay.Vergil.yamatoRisingStar = false;
	mask.Gameplay.Vergil.yamatoHighTime = false;

	mask.Gameplay.ExtraDifficulty.ldkMode = false;
	mask.Gameplay.ExtraDifficulty.mustStyleMode = false;
	mask.Gameplay.ExtraDifficulty.enemyDTMode = false;
	mask.Gameplay.ExtraDifficulty.forceDifficultyMode = false;

	mask.Cheats.Vergil.chronoSwords = false;
	mask.Cheats.Training.infiniteHP = false;
	mask.Cheats.Training.infiniteDT = false;
	mask.Cheats.Training.disableTimers = false;
	mask.Cheats.Training.infiniteBossLadyBullets = false;
	mask.Cheats.General.customDamage = false;
	mask.Cheats.Damage.playerReceivedDmgMult = false;
	mask.Cheats.Damage.enemyReceivedDmgMult = false;
	mask.Cheats.General.customSpeed = false;
	mask.Cheats.Speed.enemy = false;
	mask.Cheats.Speed.human = false;
	mask.Cheats.Speed.dTDante = false;
	mask.Cheats.Speed.dTVergil = false;
	mask.Cheats.Speed.quicksilverPlayer = false;
	mask.Cheats.Speed.quicksilverEnemy = false;
	mask.Cheats.General.customMobility = false;
	mask.Cheats.Mobility.airHikeCount = false;
	mask.Cheats.Mobility.kickJumpCount = false;
	mask.Cheats.Mobility.wallHikeCount = false;
	mask.Cheats.Mobility.dashCount = false;
	mask.Cheats.Mobility.skyStarCount = false;
	mask.Cheats.Mobility.danteAirTrickCount = false;
	mask.Cheats.Mobility.vergilAirTrickCount = false;
	mask.Cheats.Mobility.trickUpCount = false;
	mask.Cheats.Mobility.trickDownCount = false;
	mask.Debug.debugTools = false;
	mask.Cheats.General.enemySpawnerTool = false;
	mask.Cheats.General.teleporterTool = false;

	
	mask.GameMode.preset = false; // Ignore preset field in matching!
	return mask;
	}();

const CrimsonConfigGameplay& GetCurrentPreset(uint8 mode) {
	switch (mode) {
	case GAMEMODEPRESETS::VANILLA:
	{
		static const CrimsonConfigGameplay vanilla = VANILLA_PRESET;
		return vanilla;
	}
	case GAMEMODEPRESETS::STYLE_SWITCHER:
	{
		static const CrimsonConfigGameplay styleSwitcher = STYLE_SWITCHER_PRESET;
		return styleSwitcher;
	}
	case GAMEMODEPRESETS::CRIMSON:
	{
		static const CrimsonConfigGameplay crimson = CRIMSON_PRESET;
		return crimson;
	}
	default:
	{
		static const CrimsonConfigGameplay fallback{};
		return fallback;
	}
	}
}

void CrimsonGameModes::SetGameModePreset(uint8 mode) {
	// This function sets the game mode using full assignment,
	// meaning it updates all fields in the Gameplay and Cheats structures,
	// regardless of whether they are obligatory for the mode or not.
	switch (mode) {
	case GAMEMODEPRESETS::VANILLA:
 		AssignMembersPreset(activeCrimsonGameplay, VANILLA_PRESET, VANILLA_MASK);
		AssignMembersPreset(queuedCrimsonGameplay, VANILLA_PRESET, VANILLA_MASK);
		queuedConfig.Actor.enable = false;
		break;
	case GAMEMODEPRESETS::STYLE_SWITCHER:
		AssignMembersPreset(activeCrimsonGameplay, STYLE_SWITCHER_PRESET, STYLE_SWITCHER_MASK);
		AssignMembersPreset(queuedCrimsonGameplay, STYLE_SWITCHER_PRESET, STYLE_SWITCHER_MASK);
		queuedConfig.Actor.enable = true;
		break;
	case GAMEMODEPRESETS::CRIMSON:
		AssignMembersPreset(activeCrimsonGameplay, CRIMSON_PRESET, CRIMSON_MASK);
		AssignMembersPreset(queuedCrimsonGameplay, CRIMSON_PRESET, CRIMSON_MASK);
		queuedConfig.Actor.enable = true;
		break;
	default:
		break;
	}
	// Set the preset field explicitly
	activeCrimsonGameplay.GameMode.preset = mode;
	queuedCrimsonGameplay.GameMode.preset = mode;
}

void CrimsonGameModes::SetGameModeMasked(uint8 mode) {
	// This function sets the game mode using masked assignment,
	// meaning it only updates fields that are obligatory,
	// while ignoring the options that aren't obligatory for the mode.
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
			SetGameModeMasked(matchedPreset);
		} else {
			activeCrimsonGameplay.GameMode.preset = GAMEMODEPRESETS::CUSTOM;
			queuedCrimsonGameplay.GameMode.preset = GAMEMODEPRESETS::CUSTOM;
		}
	}
}

void AdjustCustomPresetForComparison(CrimsonConfigGameplay& preset) {
    if (activeCrimsonGameplay.GameMode.preset == GAMEMODEPRESETS::CUSTOM) {
		auto& mobility = preset.Cheats.Mobility;
		auto& defMob = defaultCrimsonGameplay.Cheats.Mobility;

		if (activeCrimsonGameplay.Gameplay.Dante.dmc4Mobility) {
			mobility.airHikeCount[0] = 1;  mobility.airHikeCount[1] = 2;
			mobility.kickJumpCount[0] = 1; mobility.kickJumpCount[1] = 1;
			mobility.wallHikeCount[0] = 1; mobility.wallHikeCount[1] = 2;
			mobility.dashCount[0] = 3;     mobility.dashCount[1] = 3;
			mobility.skyStarCount[0] = 1;  mobility.skyStarCount[1] = 2;
			mobility.danteAirTrickCount[0] = 1;  mobility.danteAirTrickCount[1] = 2;
			mobility.vergilAirTrickCount[0] = 1; mobility.vergilAirTrickCount[1] = 1;
			mobility.trickUpCount[0] = 1;  mobility.trickUpCount[1] = 1;
			mobility.trickDownCount[0] = 1; mobility.trickDownCount[1] = 1;
		} else {
			mobility.airHikeCount[0] = defMob.airHikeCount[0];  mobility.airHikeCount[1] = defMob.airHikeCount[1];
			mobility.kickJumpCount[0] = defMob.kickJumpCount[0]; mobility.kickJumpCount[1] = defMob.kickJumpCount[1];
			mobility.wallHikeCount[0] = defMob.wallHikeCount[0]; mobility.wallHikeCount[1] = defMob.wallHikeCount[1];
			mobility.dashCount[0] = defMob.dashCount[0];     mobility.dashCount[1] = defMob.dashCount[1];
			mobility.skyStarCount[0] = defMob.skyStarCount[0];  mobility.skyStarCount[1] = defMob.skyStarCount[1];
			mobility.danteAirTrickCount[0] = defMob.danteAirTrickCount[0];  mobility.danteAirTrickCount[1] = defMob.danteAirTrickCount[1];
			mobility.vergilAirTrickCount[0] = defMob.vergilAirTrickCount[0]; mobility.vergilAirTrickCount[1] = defMob.vergilAirTrickCount[1];
			mobility.trickUpCount[0] = defMob.trickUpCount[0];  mobility.trickUpCount[1] = defMob.trickUpCount[1];
			mobility.trickDownCount[0] = defMob.trickDownCount[0]; mobility.trickDownCount[1] = defMob.trickDownCount[1];
		}
    }
}

void CrimsonGameModes::TrackCheats() {
	auto& activeCheats = activeCrimsonGameplay.Cheats;
	auto& currentPreset = GetCurrentPreset(activeCrimsonGameplay.GameMode.preset);
	auto& currentCheats = gameModeData.currentlyUsedCheats;
	auto& missionUsedCheats = gameModeData.missionUsedCheats;
	auto& initializedMission = gameModeData.isMissionInitializedCheatsUsedMission;

	auto missionDataAddr = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E30);
	if (!missionDataAddr) {
		initializedMission = false; // Reset for next mission
		return;
	}
	auto& missionData = *reinterpret_cast<MissionData*>(missionDataAddr);

	auto updateCheatFlag = [&](bool anyChanged, uint8 group) {
		auto it = std::find(currentCheats.begin(), currentCheats.end(), group);
		auto itMission = std::find(missionUsedCheats.begin(), missionUsedCheats.end(), group);
		if (anyChanged) {
			if (it == currentCheats.end()) {
				currentCheats.push_back(group);
				std::sort(currentCheats.begin(), currentCheats.end());
			}
			if (initializedMission && itMission == missionUsedCheats.end()) {
				missionUsedCheats.push_back(group);
				std::sort(missionUsedCheats.begin(), missionUsedCheats.end());
			}
		} else {
			if (it != currentCheats.end()) {
				currentCheats.erase(it);
				std::sort(currentCheats.begin(), currentCheats.end());
			}
		}
		};

	// === CHEATS::TRAINING ===
	bool trainingChanged =
		activeCheats.Training.infiniteHP != currentPreset.Cheats.Training.infiniteHP ||
		activeCheats.Training.infiniteDT != currentPreset.Cheats.Training.infiniteDT ||
		activeCheats.Training.disableTimers != currentPreset.Cheats.Training.disableTimers ||
		activeCheats.Training.infiniteBossLadyBullets != currentPreset.Cheats.Training.infiniteBossLadyBullets;

	updateCheatFlag(trainingChanged, CHEATS::TRAINING);

	// === CHEATS::DAMAGE ===
	bool damageChanged =
		activeCheats.General.customDamage != currentPreset.Cheats.General.customDamage ||
		activeCheats.Damage.playerReceivedDmgMult != currentPreset.Cheats.Damage.playerReceivedDmgMult ||
		activeCheats.Damage.enemyReceivedDmgMult != currentPreset.Cheats.Damage.enemyReceivedDmgMult;

	updateCheatFlag(damageChanged, CHEATS::DAMAGE);

	// === CHEATS::SPEED ===
	bool speedChanged =
		activeCheats.General.customSpeed != currentPreset.Cheats.General.customSpeed ||
		activeCheats.Speed.enemy != currentPreset.Cheats.Speed.enemy ||
		activeCheats.Speed.human != currentPreset.Cheats.Speed.human ||
		activeCheats.Speed.quicksilverPlayer != currentPreset.Cheats.Speed.quicksilverPlayer ||
		activeCheats.Speed.quicksilverEnemy != currentPreset.Cheats.Speed.quicksilverEnemy ||
		// Arrays: [6] and [5]
		std::memcmp(activeCheats.Speed.dTDante, currentPreset.Cheats.Speed.dTDante, sizeof(float) * 6) != 0 ||
		std::memcmp(activeCheats.Speed.dTVergil, currentPreset.Cheats.Speed.dTVergil, sizeof(float) * 5) != 0;

	updateCheatFlag(speedChanged, CHEATS::SPEED);

	// === CHEATS::MOBILITY ===
	bool mobilityChanged = [&]() {
		CrimsonConfigGameplay adjustedPreset = currentPreset;
		AdjustCustomPresetForComparison(adjustedPreset);

		return activeCheats.General.customMobility != adjustedPreset.Cheats.General.customMobility ||
			   activeCheats.Mobility.airHikeCount[0] != adjustedPreset.Cheats.Mobility.airHikeCount[0] ||
			   activeCheats.Mobility.airHikeCount[1] != adjustedPreset.Cheats.Mobility.airHikeCount[1] ||
			   activeCheats.Mobility.kickJumpCount[0] != adjustedPreset.Cheats.Mobility.kickJumpCount[0] ||
			   activeCheats.Mobility.kickJumpCount[1] != adjustedPreset.Cheats.Mobility.kickJumpCount[1] ||
			   activeCheats.Mobility.wallHikeCount[0] != adjustedPreset.Cheats.Mobility.wallHikeCount[0] ||
			   activeCheats.Mobility.wallHikeCount[1] != adjustedPreset.Cheats.Mobility.wallHikeCount[1] ||
			   activeCheats.Mobility.dashCount[0] != adjustedPreset.Cheats.Mobility.dashCount[0] ||
			   activeCheats.Mobility.dashCount[1] != adjustedPreset.Cheats.Mobility.dashCount[1] ||
			   activeCheats.Mobility.skyStarCount[0] != adjustedPreset.Cheats.Mobility.skyStarCount[0] ||
			   activeCheats.Mobility.skyStarCount[1] != adjustedPreset.Cheats.Mobility.skyStarCount[1] ||
			   activeCheats.Mobility.danteAirTrickCount[0] != adjustedPreset.Cheats.Mobility.danteAirTrickCount[0] ||
			   activeCheats.Mobility.danteAirTrickCount[1] != adjustedPreset.Cheats.Mobility.danteAirTrickCount[1] ||
			   activeCheats.Mobility.vergilAirTrickCount[0] != adjustedPreset.Cheats.Mobility.vergilAirTrickCount[0] ||
			   activeCheats.Mobility.vergilAirTrickCount[1] != adjustedPreset.Cheats.Mobility.vergilAirTrickCount[1] ||
			   activeCheats.Mobility.trickUpCount[0] != adjustedPreset.Cheats.Mobility.trickUpCount[0] ||
			   activeCheats.Mobility.trickUpCount[1] != adjustedPreset.Cheats.Mobility.trickUpCount[1] ||
			   activeCheats.Mobility.trickDownCount[0] != adjustedPreset.Cheats.Mobility.trickDownCount[0] ||
			   activeCheats.Mobility.trickDownCount[1] != adjustedPreset.Cheats.Mobility.trickDownCount[1];
	}();

	updateCheatFlag(mobilityChanged, CHEATS::MOBILITY);

	bool debugChanged =
		activeCrimsonGameplay.Debug.debugTools != currentPreset.Debug.debugTools ||
		activeCrimsonGameplay.Cheats.General.enemySpawnerTool != currentPreset.Cheats.General.enemySpawnerTool ||
		activeCrimsonGameplay.Cheats.General.teleporterTool != currentPreset.Cheats.General.teleporterTool;

	updateCheatFlag(debugChanged, CHEATS::DEBUG);

	bool rmsChanged = activeCrimsonGameplay.Cheats.Misc.resetMotionState != currentPreset.Cheats.Misc.resetMotionState;
	updateCheatFlag(rmsChanged, CHEATS::RMS);

	if (missionData.frameCount > 0) {
		if (!initializedMission) {
			initializedMission = true;
		}
	} else if (g_scene == SCENE::MISSION_SELECT || g_scene == SCENE::MISSION_START) {
		initializedMission = false;
	}

	if (!initializedMission && missionUsedCheats.size() > 0) {
		missionUsedCheats.clear();
	}

	if (initializedMission && activeConfig.Arcade.enable) {
		gameModeData.arcadeMissionEnabled = true;
	} else if (!initializedMission && !activeConfig.Arcade.enable) {
		gameModeData.arcadeMissionEnabled = false;
	}

	if (initializedMission && activeConfig.BossRush.enable) {
		gameModeData.bossRushMissionEnabled = true;
	} else if (!initializedMission && !activeConfig.BossRush.enable) {
		gameModeData.bossRushMissionEnabled = false;
	}
}

void CrimsonGameModes::TrackMissionResultGameMode() {
	bool& initializedMission = gameModeData.isMissionInitializedGameModeMResult;

	static bool presetChanged = false;
	static bool ldkChanged = false;
	static bool mustStyleChanged = false;
	static bool enemyDTChanged = false;
	static bool forceDifficultyChanged = false;

	auto name_10723 = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E30);
	if (!name_10723) {
		gameModeData.missionResultGameMode = presetChanged ? GAMEMODEPRESETS::UNRATED : activeCrimsonGameplay.GameMode.preset;
		gameModeData.ldkNissionResult = ldkChanged ? LDKMODE::OFF : activeCrimsonGameplay.Gameplay.ExtraDifficulty.ldkMode;
		gameModeData.mustStyleMissionResult = mustStyleChanged ? STYLE_RANK::NONE : activeCrimsonGameplay.Gameplay.ExtraDifficulty.mustStyleMode;
		gameModeData.enemyDTMissionResult = enemyDTChanged ? ENEMYDTMODE::DEFAULT : activeCrimsonGameplay.Gameplay.ExtraDifficulty.enemyDTMode;
		initializedMission = false; // Reset for next mission
		return;
	}
	auto& missionData = *reinterpret_cast<MissionData*>(name_10723);

	static uint8 initialPreset = GAMEMODEPRESETS::UNRATED;
	static uint8 initialLDKPreset = LDKMODE::OFF;
	static uint8 initialEnemyDTPreset = ENEMYDTMODE::DEFAULT;
	static uint32 initialMustStylePreset = STYLE_RANK::NONE;
	static bool enemyDTLockedNoDT = false; 
	static uint32 initialForceDifficulty = DIFFICULTY_MODE::FORCE_DIFFICULTY_OFF;

	if (missionData.frameCount > 0 && g_scene != SCENE::MISSION_RESULT) { // Mission is Running
		if (!initializedMission) {
			initialPreset = activeCrimsonGameplay.GameMode.preset;
			initialLDKPreset = activeCrimsonGameplay.Gameplay.ExtraDifficulty.ldkMode;
			initialEnemyDTPreset = activeCrimsonGameplay.Gameplay.ExtraDifficulty.enemyDTMode;
			initialMustStylePreset = activeCrimsonGameplay.Gameplay.ExtraDifficulty.mustStyleMode;
			initialForceDifficulty = activeCrimsonGameplay.Gameplay.ExtraDifficulty.forceDifficultyMode;
			initializedMission = true;
			presetChanged = false;
			ldkChanged = false;
			mustStyleChanged = false;
			enemyDTChanged = false;
			enemyDTLockedNoDT = false; // Reset lock at mission start
			forceDifficultyChanged = false;
		} else if (activeCrimsonGameplay.GameMode.preset != initialPreset) {
			presetChanged = true;
			gameModeData.missionResultGameMode = presetChanged ? GAMEMODEPRESETS::UNRATED : activeCrimsonGameplay.GameMode.preset;
		}

		if (activeCrimsonGameplay.Gameplay.ExtraDifficulty.ldkMode != initialLDKPreset) {
			ldkChanged = true;
			gameModeData.ldkNissionResult = ldkChanged ? LDKMODE::OFF : activeCrimsonGameplay.Gameplay.ExtraDifficulty.ldkMode;
		}

		if (activeCrimsonGameplay.Gameplay.ExtraDifficulty.mustStyleMode != initialMustStylePreset) {
			mustStyleChanged = true;
			gameModeData.mustStyleMissionResult = mustStyleChanged ? STYLE_RANK::NONE : activeCrimsonGameplay.Gameplay.ExtraDifficulty.mustStyleMode;
		}

		// Enemy DT Mode logic with lock
		if (!enemyDTLockedNoDT && activeCrimsonGameplay.Gameplay.ExtraDifficulty.enemyDTMode == ENEMYDTMODE::NO_ENEMY_DT) {
			enemyDTLockedNoDT = true;
			gameModeData.enemyDTMissionResult = ENEMYDTMODE::NO_ENEMY_DT;
		} else if (!enemyDTLockedNoDT && activeCrimsonGameplay.Gameplay.ExtraDifficulty.enemyDTMode != initialEnemyDTPreset) {
			enemyDTChanged = true;
			gameModeData.enemyDTMissionResult = enemyDTChanged ? ENEMYDTMODE::DEFAULT : activeCrimsonGameplay.Gameplay.ExtraDifficulty.enemyDTMode;
		} else if (enemyDTLockedNoDT) {
			// Keep it locked
			gameModeData.enemyDTMissionResult = ENEMYDTMODE::NO_ENEMY_DT;
		}

		if (activeCrimsonGameplay.Gameplay.ExtraDifficulty.forceDifficultyMode != initialForceDifficulty) {
			forceDifficultyChanged = true;
			if (initialForceDifficulty != DIFFICULTY_MODE::FORCE_DIFFICULTY_OFF || forceDifficultyChanged) {
				gameModeData.forceDifficultyResult = true;
			} 
		}
	} else if (g_scene == SCENE::MISSION_RESULT) { // Mission Result Screen
		gameModeData.missionResultGameMode = presetChanged ? GAMEMODEPRESETS::UNRATED : gameModeData.missionResultGameMode;
		gameModeData.ldkNissionResult = ldkChanged ? LDKMODE::OFF : activeCrimsonGameplay.Gameplay.ExtraDifficulty.ldkMode;
		gameModeData.mustStyleMissionResult = mustStyleChanged ? STYLE_RANK::NONE : activeCrimsonGameplay.Gameplay.ExtraDifficulty.mustStyleMode;
		gameModeData.enemyDTMissionResult = enemyDTLockedNoDT ? ENEMYDTMODE::NO_ENEMY_DT :
			(enemyDTChanged ? ENEMYDTMODE::DEFAULT : activeCrimsonGameplay.Gameplay.ExtraDifficulty.enemyDTMode);
		gameModeData.forceDifficultyResult = initialForceDifficulty != DIFFICULTY_MODE::FORCE_DIFFICULTY_OFF || forceDifficultyChanged ? true : false;
		initializedMission = false; // Reset for next mission
		enemyDTLockedNoDT = false;  // Reset lock after mission result
		
	}
}

}
#pragma once

#include <stdint.h>
#include "Core/Core.hpp"
#include "Core/Macros.h"
#include "Exp.hpp"
#include "Vars.hpp"

#include "Core/DebugSwitch.hpp"
#pragma optimize("", off) // Disable all optimizations
#pragma pack(push, 8)

namespace GAMEMODEPRESETS {
enum {
	VANILLA,
	STYLE_SWITCHER,
	CRIMSON,
	CUSTOM,

	UNRATED,
	};
}

struct CrimsonConfigGameplay {
	struct Gameplay {

        struct General {
			bool inertia = true;
			bool sprint = true;
			bool charHotswap = false;
			bool freeformSoftLock = true;
			bool bufferlessReversals = true;
			bool dmc4LockOnDirection = true;
            bool holdToCrazyCombo = true;
			bool holdToShoot = false;
			uint8 crazyComboMashRequirement = 6;
			bool disableHeightRestriction = true;
			bool improvedBufferedReversals = true;
			bool increasedJCSpheres = true;
			bool disableJCRestriction = true;
			bool increasedEnemyJuggleTime = true;
			bool fasterTurnRate = true;
			float vanillaWeaponSwitchDelay = 12;
			bool disableSoulEaterInvis = true;
            

			static constexpr auto Metadata() {
				return std::make_tuple(
					std::make_pair("inertia", &General::inertia),
					std::make_pair("sprint", &General::sprint),
					std::make_pair("charHotswap",&General::charHotswap),
					std::make_pair("freeformSoftLock", &General::freeformSoftLock),
					std::make_pair("bufferlessReversals", &General::bufferlessReversals),
					std::make_pair("dmc4LockOnDirection", &General::dmc4LockOnDirection),
                    std::make_pair("holdToCrazyCombo", &General::holdToCrazyCombo),
					std::make_pair("holdToShoot", &General::holdToShoot),
					std::make_pair("crazyComboMashRequirement", &General::crazyComboMashRequirement),
					std::make_pair("disableHeightRestriction", &General::disableHeightRestriction),
					std::make_pair("improvedBufferedReversals", &General::improvedBufferedReversals),
					std::make_pair("increasedJCSpheres", &General::increasedJCSpheres),
					std::make_pair("disableJCRestriction", &General::disableJCRestriction),
					std::make_pair("increasedEnemyJuggleTime", &General::increasedEnemyJuggleTime),
					std::make_pair("fasterTurnRate", &General::fasterTurnRate),
					std::make_pair("vanillaWeaponSwitchDelay", &General::vanillaWeaponSwitchDelay),
					std::make_pair("disableSoulEaterInvis", &General::disableSoulEaterInvis)
				);
			}
        } General;

        struct Dante {
			bool improvedCancels = true;
			bool bulletStop = true;
			bool rainstormLift = true;
			bool infiniteRainstorm = true;	
			bool foursomeTime = true;
			bool aerialMovesTweaks = true;
			bool downertiaD = false;
			bool shotgunAirShotTweaks = true;
			bool driveTweaks = true;
			bool disableAirSlashKnockback = true;
			bool airStinger = true;
			bool airRevolver = true;
			bool airTornado = true;
			bool airRisingDragonLaunch = true;
			bool airAgniRudraWhirlwind = true;
			bool skyLaunchAirTaunt = true;
			bool dmc4Mobility = true;
			bool dTInfusedRoyalguard = true;
			bool airHikeCoreAbility = true;
			bool altNevanVortex = true;
			bool artemisRework = true;
			bool groundTrick = true;
			bool swapArtemisMultiLockNormalShot = true;
			bool swapDancePierceInputs = true;
			bool swapHammerVocalnoInputs = true;

			static constexpr auto Metadata() {
                return std::make_tuple(
				    std::make_pair("improvedCancels", &Dante::improvedCancels),
					std::make_pair("bulletStop", &Dante::bulletStop),
					std::make_pair("rainstormLift", &Dante::rainstormLift),
					std::make_pair("infiniteRainstorm", &Dante::infiniteRainstorm),
					std::make_pair("foursomeTime", &Dante::foursomeTime),
					std::make_pair("aerialMovesTweaks", &Dante::aerialMovesTweaks),
					std::make_pair("downertiaD", &Dante::downertiaD),
					std::make_pair("shotgunAirShotTweaks", &Dante::shotgunAirShotTweaks),
					std::make_pair("driveTweaks", &Dante::driveTweaks),
					std::make_pair("disableAirSlashKnockback", &Dante::disableAirSlashKnockback),
					std::make_pair("airStinger", &Dante::airStinger),
					std::make_pair("airRevolver", &Dante::airRevolver),
					std::make_pair("airTornado", &Dante::airTornado),
					std::make_pair("airRisingDragonLaunch", &Dante::airRisingDragonLaunch),
					std::make_pair("airAgniRudraWhirlwind", &Dante::airAgniRudraWhirlwind),
					std::make_pair("skyLaunchAirTaunt", &Dante::skyLaunchAirTaunt),
					std::make_pair("dmc4Mobility", &Dante::dmc4Mobility),
					std::make_pair("dTInfusedRoyalguard", &Dante::dTInfusedRoyalguard),
					std::make_pair("airHikeCoreAbility", &Dante::airHikeCoreAbility),
					std::make_pair("altNevanVortex", &Dante::altNevanVortex),
					std::make_pair("artemisRework", &Dante::artemisRework),
					std::make_pair("groundTrick", &Dante::groundTrick),
					std::make_pair("swapArtemisMultiLockNormalShot", &Dante::swapArtemisMultiLockNormalShot),
					std::make_pair("swapDancePierceInputs", &Dante::swapDancePierceInputs),
					std::make_pair("swapHammerVocalnoInputs", &Dante::swapHammerVocalnoInputs)
				);
			}
        } Dante;

		struct Vergil {
			bool darkslayerTrickCancels = true;
			bool fasterDTRapidSlash = true;
			bool roundTripTweaks = true;
			bool airStinger = true;
			bool airRisingSun = true;
			bool airLunarPhase = true;
			bool altJudgementCutInput = false;
			bool yamatoRisingStar = true;
			bool yamatoHighTime = true;
			bool airTauntRisingSun = true;
			bool mirageTrigger = true;
			bool downertia = true;
			bool trickUpNoLockOn = true;
			std::string adjustLunarPhasePos = "From Air";

			static constexpr auto Metadata() {
				return std::make_tuple(
					std::make_pair("darkslayerTrickCancels", &Vergil::darkslayerTrickCancels),
					std::make_pair("fasterDTRapidSlash", &Vergil::fasterDTRapidSlash),
					std::make_pair("roundTripTweaks", &Vergil::roundTripTweaks),
					std::make_pair("airStinger", &Vergil::airStinger),
					std::make_pair("airRisingSun", &Vergil::airRisingSun),
					std::make_pair("airLunarPhase", &Vergil::airLunarPhase),
					std::make_pair("altJudgementCutInput", &Vergil::altJudgementCutInput),
					std::make_pair("yamatoRisingStar", &Vergil::yamatoRisingStar),
					std::make_pair("yamatoHighTime", &Vergil::yamatoHighTime),
					std::make_pair("airTauntRisingSun", &Vergil::airTauntRisingSun),
					std::make_pair("mirageTrigger", &Vergil::mirageTrigger),
					std::make_pair("downertia", &Vergil::downertia),
					std::make_pair("trickUpNoLockOn", &Vergil::trickUpNoLockOn),
					std::make_pair("adjustLunarPhasePos", &Vergil::adjustLunarPhasePos)
				);
			}
        } Vergil;

		struct ExtraDifficulty {
			uint8 ldkMode = 0;
			uint32 mustStyleMode = STYLE_RANK::NONE;
			uint8 enemyDTMode = ENEMYDTMODE::DEFAULT;
			uint32 forceDifficultyMode = DIFFICULTY_MODE::FORCE_DIFFICULTY_OFF;

			static constexpr auto Metadata() {
				return std::make_tuple(
					std::make_pair("ldkMode", &ExtraDifficulty::ldkMode),
					std::make_pair("mustStyleMode", &ExtraDifficulty::mustStyleMode),
					std::make_pair("enemyDTMode", &ExtraDifficulty::enemyDTMode),
					std::make_pair("forceDifficultyMode", &ExtraDifficulty::forceDifficultyMode)
				);
			}

		} ExtraDifficulty;


		static constexpr auto Metadata() {
			return std::make_tuple(
                std::make_pair("General", &Gameplay::General),
                std::make_pair("Dante", &Gameplay::Dante),
                std::make_pair("Vergil", &Gameplay::Vergil),
				std::make_pair("ExtraDifficulty", &Gameplay::ExtraDifficulty)
				
			);
		}

	} Gameplay;


	struct Cheats {

		struct General {
			bool customMobility = false;
			bool customDamage = false;
			bool customSpeed = false;
			bool miscCheats = false;
			bool legacyDDMKCharacters = false;
			bool teleporterTool = false;
			bool enemySpawnerTool = false;

			static constexpr auto Metadata() {
				return std::make_tuple(
					std::make_pair("customMobility", &General::customMobility),
					std::make_pair("customDamage", &General::customDamage),
					std::make_pair("customSpeed", &General::customSpeed),
					std::make_pair("miscCheats", &General::miscCheats),
					std::make_pair("legacyDDMKCharacters", &General::legacyDDMKCharacters),
					std::make_pair("teleporterTool", &General::teleporterTool),
					std::make_pair("enemySpawnerTool", &General::enemySpawnerTool)
				);
			}
		} General;

		struct Training {
			bool infiniteHP = false;
			bool infiniteDT = false;
			bool disableRegularEnemyAttacks = false;
			bool disableTimers = false;
			bool infiniteBossLadyBullets = false;

			static constexpr auto Metadata() {
				return std::make_tuple(
					std::make_pair("infiniteHP", &Training::infiniteHP),
					std::make_pair("infiniteDT", &Training::infiniteDT),
					std::make_pair("disableRegularEnemyAttacks", &Training::disableRegularEnemyAttacks),
					std::make_pair("disableTimers", &Training::disableTimers),
					std::make_pair("infiniteBossLadyBullets", &Training::infiniteBossLadyBullets)
				);
			}
		} Training;

		struct Damage {
			float playerReceivedDmgMult = 1.0f;
			float enemyReceivedDmgMult = 1.0f;

			static constexpr auto Metadata() {
				return std::make_tuple(
					std::make_pair("playerReceivedDmgMult", &Damage::playerReceivedDmgMult),
					std::make_pair("enemyReceivedDmgMult", &Damage::enemyReceivedDmgMult)
				);
			}
		} Damage;

		struct Speed {
			float enemy = 1.0f;
			float human = 1.0f;
			float dTDante[6] = {
				1.1f,
				1.2f,
				1.05f,
				1.1f,
				1.05f,
				1.1f,
			};
			float dTVergil[5] = {
				1.2f,
				1.2f,
				1.2f,
				1.0f,
				1.0f,
			};
			float quicksilverPlayer = 1.05f;
			float quicksilverEnemy = 0.33f;

			static constexpr auto Metadata() {
				return std::make_tuple(
					std::make_pair("enemy", &Speed::enemy),
					std::make_pair("human", &Speed::human),
					std::make_pair("dTDante", &Speed::dTDante),
					std::make_pair("dTVergil", &Speed::dTVergil),
					std::make_pair("quicksilverPlayer", &Speed::quicksilverPlayer),
					std::make_pair("quicksilverEnemy", &Speed::quicksilverEnemy)
				);
			}
		} Speed;

		struct Mobility {
			uint8 airHikeCount[2] = { 1, 1 };
			uint8 kickJumpCount[2] = { 1, 1 };
			uint8 wallHikeCount[2] = { 1, 1 };
			uint8 dashCount[2] = { 3, 3 };
			uint8 skyStarCount[2] = { 1, 1 };
			uint8 danteAirTrickCount[2] = { 1, 1 };
			uint8 vergilAirTrickCount[2] = { 1, 1 };
			uint8 trickUpCount[2] = { 1, 1 };
			uint8 trickDownCount[2] = { 1, 1 };

			static constexpr auto Metadata() {
				return std::make_tuple(
					std::make_pair("airHikeCount", &Mobility::airHikeCount),
					std::make_pair("kickJumpCount", &Mobility::kickJumpCount),
					std::make_pair("wallHikeCount", &Mobility::wallHikeCount),
					std::make_pair("dashCount", &Mobility::dashCount),
					std::make_pair("skyStarCount", &Mobility::skyStarCount),
					std::make_pair("danteAirTrickCount", &Mobility::danteAirTrickCount),
					std::make_pair("vergilAirTrickCount", &Mobility::vergilAirTrickCount),
					std::make_pair("trickUpCount", &Mobility::trickUpCount),
					std::make_pair("trickDownCount", &Mobility::trickDownCount)
				);
			}
		} Mobility;

		struct Misc {
			float quicksilverDepletion = 13.75f;
			float doppelgangerDepletion = 16.5f;
			float dTDepletion = 11.0f;
			float orbReach = 300.0f;
			bool resetMotionState = false;

			static constexpr auto Metadata() {
				return std::make_tuple(
					std::make_pair("quicksilverDepletion", &Misc::quicksilverDepletion),
					std::make_pair("doppelgangerDepletion", &Misc::doppelgangerDepletion),
					std::make_pair("dTDepletion", &Misc::dTDepletion),
					std::make_pair("orbReach", &Misc::orbReach),
					std::make_pair("resetMotionState", &Misc::resetMotionState)
				);
			}
		} Misc;

		struct Dante {
			bool forceRoyalRelease = false;
			bool infiniteShredder = false;
			bool infiniteSwordPierce = false;

			static constexpr auto Metadata() {
				return std::make_tuple(
					std::make_pair("forceRoyalRelease", &Dante::forceRoyalRelease),
					std::make_pair("infiniteShredder", &Dante::infiniteShredder),
					std::make_pair("infiniteSwordPierce", &Dante::infiniteSwordPierce)
				);
			}
		} Dante;

		struct Vergil {
			bool quicksilverStyle = false;
			bool chronoSwords = false;
			bool infiniteRoundTrip = false;
			uint8 airRisingSunCount[2] = { 1, 1 };
			uint8 judgementCutCount[2] = { 2, 2 };

			static constexpr auto Metadata() {
				return std::make_tuple(
					std::make_pair("quicksilverStyle", &Vergil::quicksilverStyle),
					std::make_pair("chronoSwords", &Vergil::chronoSwords),
					std::make_pair("infiniteRoundTrip", &Vergil::infiniteRoundTrip),
					std::make_pair("airRisingSunCount", &Vergil::airRisingSunCount),
					std::make_pair("judgementCutCount", &Vergil::judgementCutCount)
				);
			}
		} Vergil;

		static constexpr auto Metadata() {
			return std::make_tuple(
				std::make_pair("General", &Cheats::General),
				std::make_pair("Training", &Cheats::Training),
				std::make_pair("Damage", &Cheats::Damage),
				std::make_pair("Speed", &Cheats::Speed),
				std::make_pair("Mobility", &Cheats::Mobility),
				std::make_pair("Misc", &Cheats::Misc),
				std::make_pair("Dante", &Cheats::Dante),
				std::make_pair("Vergil", &Cheats::Vergil)
			);
		}
	} Cheats;

	struct Debug {
		bool debugTools = false;

		static constexpr auto Metadata() {
			return std::make_tuple(
				std::make_pair("debugTools", &Debug::debugTools)
			);
		}
	} Debug;

	struct GameMode {
		uint8 preset = GAMEMODEPRESETS::CRIMSON;

		static constexpr auto Metadata() {
			return std::make_tuple(
				std::make_pair("preset", &GameMode::preset)
			);
		}
	} GameMode;

	static constexpr auto Metadata() {
		return std::make_tuple(
            std::make_pair("Gameplay", &CrimsonConfigGameplay::Gameplay),
			std::make_pair("Cheats", &CrimsonConfigGameplay::Cheats),
			std::make_pair("Debug", &CrimsonConfigGameplay::Debug),
			std::make_pair("GameMode", &CrimsonConfigGameplay::GameMode)
		);
	}
};

// Same as in old ddmk, but with new config variables. We declare three config variables, each
// serving a different purpose:
// 
// -- defaultCrimsonConfig will retain our default values for all configurations.
// -- queuedCrimsonConfig will retain config information as shown in the GUI.
// -- activeCrimsonConfig will be our main and most current config variable to be read in functions.
// GUI functions in CrimsonGUI typically change queued and active configs in conjunction with one another.
// 
// Legacy ddmk Configs are present in Config.hpp
// - CrimsonConfigGameplay is a struct that contains all gameplay-related configurations.
// - Mia

extern CrimsonConfigGameplay defaultCrimsonGameplay;
extern CrimsonConfigGameplay queuedCrimsonGameplay;
extern CrimsonConfigGameplay activeCrimsonGameplay;

#pragma pack(pop)
#pragma optimize("", on) // Re-enable optimizations
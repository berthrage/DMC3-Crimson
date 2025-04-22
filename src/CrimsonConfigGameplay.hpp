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

namespace LDKMODE {
enum {
	OFF,
	LDK,
	SUPER_LDK,
	SUPER_LDK_BOSSES,
};
}

struct CrimsonConfigGameplay {
	struct Gameplay {

        struct General {
			bool inertia = true;
			bool sprint = true;
			bool freeformSoftLock = true;
			bool bufferlessReversals = true;
			bool dmc4LockOnDirection = true;
            bool holdToCrazyCombo = true;
			uint8 crazyComboMashRequirement = 6;
			bool disableHeightRestriction = true;
			bool improvedBufferedReversals = true;
			bool increasedJCSpheres = true;
			bool disableJCRestriction = true;
			bool increasedEnemyJuggleTime = true;
			bool fasterTurnRate = true;
			float vanillaWeaponSwitchDelay = 12;
            

			static constexpr auto Metadata() {
				return std::make_tuple(
					std::make_pair("inertia", &General::inertia),
					std::make_pair("sprint", &General::sprint),
					std::make_pair("freeformSoftLock", &General::freeformSoftLock),
					std::make_pair("bufferlessReversals", &General::bufferlessReversals),
					std::make_pair("improvedBufferedReversals", &General::improvedBufferedReversals),
					std::make_pair("dmc4LockOnDirection", &General::dmc4LockOnDirection),
                    std::make_pair("holdToCrazyCombo", &General::holdToCrazyCombo),
					std::make_pair("crazyComboMashRequirement", &General::crazyComboMashRequirement),
					std::make_pair("disableHeightRestriction", &General::disableHeightRestriction),
					std::make_pair("improvedBufferedReversals", &General::improvedBufferedReversals),
					std::make_pair("increasedJCSpheres", &General::increasedJCSpheres),
					std::make_pair("disableJCRestriction", &General::disableJCRestriction),
					std::make_pair("increasedEnemyJuggleTime", &General::increasedEnemyJuggleTime),
					std::make_pair("fasterTurnRate", &General::fasterTurnRate),
					std::make_pair("vanillaWeaponSwitchDelay", &General::vanillaWeaponSwitchDelay)
				);
			}
        } General;

        struct Dante {
			bool improvedCancels = true;
			bool bulletStop = true;
			bool rainstormLift = true;
			bool infiniteRainstorm = true;	
			bool foursomeTime = true;
			bool aerialRaveTweaks = true;
			bool airFlickerTweaks = true;
			bool skyDanceTweaks = true;
			bool shotgunAirShotTweaks = true;
			bool driveTweaks = true;
			bool disableAirSlashKnockback = true;
			bool airStinger = true;
			bool airRevolver = true;
			bool airTornado = true;
			bool airRisingDragonWhirlwind = true;
			bool airAgniRudraWhirlwind = true;
			bool dmc4Mobility = true;
			bool dTInfusedRoyalguard = true;
			bool airHikeCoreAbility = true;
			bool altNevanVortex = true;
			bool artemisRework = true;

			static constexpr auto Metadata() {
                return std::make_tuple(
				    std::make_pair("improvedCancels", &Dante::improvedCancels),
					std::make_pair("bulletStop", &Dante::bulletStop),
					std::make_pair("rainstormLift", &Dante::rainstormLift),
					std::make_pair("infiniteRainstorm", &Dante::infiniteRainstorm),
					std::make_pair("foursomeTime", &Dante::foursomeTime),
					std::make_pair("aerialRaveTweaks", &Dante::aerialRaveTweaks),
					std::make_pair("airFlickerTweaks", &Dante::airFlickerTweaks),
					std::make_pair("skyDanceTweaks", &Dante::skyDanceTweaks),
					std::make_pair("shotgunAirShotTweaks", &Dante::shotgunAirShotTweaks),
					std::make_pair("driveTweaks", &Dante::driveTweaks),
					std::make_pair("disableAirSlashKnockback", &Dante::disableAirSlashKnockback),
					std::make_pair("airStinger", &Dante::airStinger),
					std::make_pair("airRevolver", &Dante::airRevolver),
					std::make_pair("airTornado", &Dante::airTornado),
					std::make_pair("airRisingDragonWhirlwind", &Dante::airRisingDragonWhirlwind),
					std::make_pair("airAgniRudraWhirlwind", &Dante::airAgniRudraWhirlwind),
					std::make_pair("dmc4Mobility", &Dante::dmc4Mobility),
					std::make_pair("dTInfusedRoyalguard", &Dante::dTInfusedRoyalguard),
					std::make_pair("airHikeCoreAbility", &Dante::airHikeCoreAbility),
					std::make_pair("altNevanVortex", &Dante::altNevanVortex),
					std::make_pair("artemisRework", &Dante::artemisRework)
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
			std::string adjustRisingSunPos = "From Air";
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
					std::make_pair("adjustRisingSunPos", &Vergil::adjustRisingSunPos),
					std::make_pair("adjustLunarPhasePos", &Vergil::adjustLunarPhasePos)
				);
			}
        } Vergil;

		struct ExtraDifficulty {
			uint8 ldkMode = 0;

			static constexpr auto Metadata() {
				return std::make_tuple(
					std::make_pair("ldkMode", &ExtraDifficulty::ldkMode)
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
					std::make_pair("quicksilver", &Vergil::quicksilverStyle),
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
				std::make_pair("Dante", &Cheats::Dante),
				std::make_pair("Vergil", &Cheats::Vergil)
			);
		}
	} Cheats;

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
#pragma once
#include "CrimsonConfigGameplay.hpp"

// Mask struct: true = field is relevant for preset matching (default: true)
struct CrimsonConfigGameplayMask {
	struct Gameplay {
		struct General {
			bool inertia = true;
			bool sprint = true;
			bool charHotswap = true;
			bool freeformSoftLock = true;
			bool bufferlessReversals = true;
			bool dmc4LockOnDirection = true;
			bool holdToCrazyCombo = true;
			bool holdToShoot = true;
			bool crazyComboMashRequirement = true;
			bool disableHeightRestriction = true;
			bool improvedBufferedReversals = true;
			bool increasedJCSpheres = true;
			bool disableJCRestriction = true;
			bool increasedEnemyJuggleTime = true;
			bool fasterTurnRate = true;
			bool vanillaWeaponSwitchDelay = true;
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
			bool aerialRaveTweaks = true;
			bool airFlickerTweaks = true;
			bool skyDanceTweaks = true;
			bool downertiaFromAirFlickerSkyDance = true;
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
			bool swapArtemisMultiLockNormalShot = true;
			bool swapDancePierceInputs = true;

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
					std::make_pair("downertiaFromAirFlickerSkyDance", &Dante::downertiaFromAirFlickerSkyDance),
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
					std::make_pair("artemisRework", &Dante::artemisRework),
					std::make_pair("swapArtemisMultiLockNormalShot", &Dante::swapArtemisMultiLockNormalShot),
					std::make_pair("swapDancePierceInputs", &Dante::swapDancePierceInputs)
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
			bool altJudgementCutInput = true;
			bool yamatoRisingStar = true;
			bool yamatoHighTime = true;
			bool mirageTrigger = true;
			bool adjustLunarPhasePos = true;
			

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
					std::make_pair("mirageTrigger", &Vergil::mirageTrigger),
					std::make_pair("adjustLunarPhasePos", &Vergil::adjustLunarPhasePos)
				);
			}
		} Vergil;

		struct ExtraDifficulty {
			bool ldkMode = true;
			bool mustStyleMode = true;
			bool enemyDTMode = true;
			bool forceDifficultyMode = true;

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
			bool customMobility = true;
			bool customDamage = true;
			bool customSpeed = true;
			bool miscCheats = true;
			bool legacyDDMKCharacters = true;
			bool teleporterTool = true;
			bool enemySpawnerTool = true;

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
			bool infiniteHP = true;
			bool infiniteDT = true;
			bool disableTimers = true;
			bool infiniteBossLadyBullets = true;

			static constexpr auto Metadata() {
				return std::make_tuple(
					std::make_pair("infiniteHP", &Training::infiniteHP),
					std::make_pair("infiniteDT", &Training::infiniteDT),
					std::make_pair("disableTimers", &Training::disableTimers),
					std::make_pair("infiniteBossLadyBullets", &Training::infiniteBossLadyBullets)
				);
			}
		} Training;

		struct Damage {
			bool playerReceivedDmgMult = true;
			bool enemyReceivedDmgMult = true;

			static constexpr auto Metadata() {
				return std::make_tuple(
					std::make_pair("playerReceivedDmgMult", &Damage::playerReceivedDmgMult),
					std::make_pair("enemyReceivedDmgMult", &Damage::enemyReceivedDmgMult)
				);
			}
		} Damage;

		struct Speed {
			bool enemy = true;
			bool human = true;
			bool dTDante = true;
			bool dTVergil = true;
			bool quicksilverPlayer = true;
			bool quicksilverEnemy = true;

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
			bool airHikeCount = true;
			bool kickJumpCount = true;
			bool wallHikeCount = true;
			bool dashCount = true;
			bool skyStarCount = true;
			bool danteAirTrickCount = true;
			bool vergilAirTrickCount = true;
			bool trickUpCount = true;
			bool trickDownCount = true;

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
			bool quicksilverDepletion = true;
			bool doppelgangerDepletion = true;
			bool dTDepletion = true;
			bool orbReach = true;
			bool resetMotionState = true;

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
			bool forceRoyalRelease = true;
			bool infiniteShredder = true;
			bool infiniteSwordPierce = true;

			static constexpr auto Metadata() {
				return std::make_tuple(
					std::make_pair("forceRoyalRelease", &Dante::forceRoyalRelease),
					std::make_pair("infiniteShredder", &Dante::infiniteShredder),
					std::make_pair("infiniteSwordPierce", &Dante::infiniteSwordPierce)
				);
			}
		} Dante;

		struct Vergil {
			bool quicksilverStyle = true;
			bool chronoSwords = true;
			bool infiniteRoundTrip = true;
			bool airRisingSunCount = true;
			bool judgementCutCount = true;

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
		bool debugTools = true;

		static constexpr auto Metadata() {
			return std::make_tuple(
				std::make_pair("debugTools", &Debug::debugTools)
			);
		}
	} Debug;

	struct GameMode {
		bool preset = true;

		static constexpr auto Metadata() {
			return std::make_tuple(
				std::make_pair("preset", &GameMode::preset)
			);
		}
	} GameMode;

	static constexpr auto Metadata() {
		return std::make_tuple(
			std::make_pair("Gameplay", &CrimsonConfigGameplayMask::Gameplay),
			std::make_pair("Cheats", &CrimsonConfigGameplayMask::Cheats),
			std::make_pair("Debug", &CrimsonConfigGameplayMask::Debug),
			std::make_pair("GameMode", &CrimsonConfigGameplayMask::GameMode)
		);
	}
};
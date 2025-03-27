#pragma once

#include <stdint.h>
#include "Core/Core.hpp"
#include "Core/Macros.h"
#include "Exp.hpp"
#include "Vars.hpp"

#include "Core/DebugSwitch.hpp"
#pragma optimize("", off) // Disable all optimizations
#pragma pack(push, 8)
struct CrimsonConfig {
	struct MultiplayerBars2D {
		bool show = true;
		bool show1PBar = false;

		static constexpr auto Metadata() {
			return std::make_tuple(
				std::make_pair("show", &MultiplayerBars2D::show),
				std::make_pair("show1PBar", &MultiplayerBars2D::show1PBar)
			);
		}
	} MultiplayerBars2D;

	struct MultiplayerBarsWorldSpace {
		bool show = true;
		bool show1PBar = false;
        bool showOutOfViewIcons = true;

		static constexpr auto Metadata() {
			return std::make_tuple(
				std::make_pair("show", &MultiplayerBarsWorldSpace::show),
				std::make_pair("show1PBar", &MultiplayerBarsWorldSpace::show1PBar),
                std::make_pair("showOutOfViewIcons", &MultiplayerBarsWorldSpace::showOutOfViewIcons)
			);
		}
	} MultiplayerBarsWorldSpace;

	struct GUI {
		uint8 transparencyMode = 2;
		float opacity = 0.9f;
		bool pauseWhenOpened = true;

		static constexpr auto Metadata() {
			return std::make_tuple(
				std::make_pair("transparencyMode", &GUI::transparencyMode),
				std::make_pair("opacity", &GUI::opacity),
				std::make_pair("pauseWhenOpened", &GUI::pauseWhenOpened)
			);
		}
	} GUI;

	struct WeaponWheel {
		bool analogSwitching = true;
		bool disableCameraRotation = true;
		std::string theme = "Crimson";
		std::string scale = "Small";
		bool meleeAlwaysShow = false;
		bool rangedAlwaysShow = false;
		bool force1PMultiplayerPosScale = false;
		bool hide = false;

		static constexpr auto Metadata() {
			return std::make_tuple(
				std::make_pair("analogSwitching", &WeaponWheel::analogSwitching),
				std::make_pair("disableCameraRotation", &WeaponWheel::disableCameraRotation),
				std::make_pair("theme", &WeaponWheel::theme),
				std::make_pair("scale", &WeaponWheel::scale),
				std::make_pair("meleeAlwaysShow", &WeaponWheel::meleeAlwaysShow),
				std::make_pair("rangedAlwaysShow", &WeaponWheel::rangedAlwaysShow),
				std::make_pair("force1PMultiplayerPosScale", &WeaponWheel::force1PMultiplayerPosScale),
				std::make_pair("hide", &WeaponWheel::hide)
			);
		}

	} WeaponWheel;

	struct CrimsonHudAddons {
		bool positionings = true;
		bool redOrbCounter = true;
		bool royalGauge = true;
		bool styleRanksMeter = true;
		bool lockOn = true;

		static constexpr auto Metadata() {
			return std::make_tuple(
				std::make_pair("fovMultiplier", &CrimsonHudAddons::positionings),
                std::make_pair("redOrbCounter", &CrimsonHudAddons::redOrbCounter),
                std::make_pair("royalGauge", &CrimsonHudAddons::royalGauge),
                std::make_pair("styleRanksMeter", &CrimsonHudAddons::styleRanksMeter),
                std::make_pair("lockOn", &CrimsonHudAddons::lockOn)
			);
		}
	} CrimsonHudAddons;


	struct Camera {
		float fovMultiplier = 1.2f;
		uint8 sensitivity = 2;
		uint8 followUpSpeed = 2;
		uint8 distance = 2;
		uint8 lockOnDistance = 2;
		uint8 tilt = 1;
		bool lockedOff = true;
		bool invertX = true;
		uint8 autoAdjust = 0;
		bool disableRightStickCenterCamera = true;
		bool disableBossCamera = false;
		bool multiplayerCamera = true;
		bool panoramicCamera = false;
		bool forceThirdPerson = true;

		static constexpr auto Metadata() {
			return std::make_tuple(
				std::make_pair("fovMultiplier", &Camera::fovMultiplier),
				std::make_pair("sensitivity", &Camera::sensitivity),
                std::make_pair("followUpSpeed", &Camera::followUpSpeed),
                std::make_pair("distance", &Camera::distance),
                std::make_pair("lockOnDistance", &Camera::lockOnDistance),
                std::make_pair("tilt", &Camera::tilt),
                std::make_pair("lockedOff", &Camera::lockedOff),
                std::make_pair("invertX", &Camera::invertX),
                std::make_pair("autoAdjust", &Camera::autoAdjust),
                std::make_pair("disableRightStickCenterCamera", &Camera::disableRightStickCenterCamera),
                std::make_pair("disableBossCamera", &Camera::disableBossCamera),
				std::make_pair("multiplayerCamera", &Camera::multiplayerCamera),
				std::make_pair("panoramicCamera", &Camera::panoramicCamera),
                std::make_pair("forceThirdPerson", &Camera::forceThirdPerson)
			);
		}
	} Camera;

	struct StyleSwitchFX {

		struct Flux {
			bool enable = true;

			uint8 color[6][4] = {
				// r   g  b  a 
				{ 55, 58, 6, 255 }, //trick  
				{ 58, 5, 5, 255 }, //sword  
				{ 13, 5, 58, 255 }, //gun    
				{ 5, 58, 12, 255 }, //royal  
				{ 58, 5, 49, 255 }, //quick  
				{ 58, 28, 5, 255 }, //doppel 
			};

			static constexpr auto Metadata() {
				return std::make_tuple(
					std::make_pair("enable", &StyleSwitchFX::Flux::enable),
                    std::make_pair("color", &StyleSwitchFX::Flux::color)
				);
			}

		} Flux;

		struct Text {
			bool enable = true;
			float maxAlpha = 0.9f;
			float size = 1.0f;

			uint8 color[9][4] = {
				// r   g  b  a  // though alpha is used in maxAlpha instead
				{ 255, 255, 176, 255 }, //trick  
				{ 248, 153, 153, 255 }, //sword  
				{ 168, 186, 246, 255 }, //gun    
				{ 190, 252, 200, 200 }, //royal  
				{ 237, 195, 252, 255 }, //quick  
				{ 252, 226, 203, 255 }, //doppel 
				{ 255, 255, 255, 255 }, //dt     
				{ 255, 255, 255, 255 }, //dte    
				{ 255, 255, 255, 255 }, //ready  
			};

			static constexpr auto Metadata() {
				return std::make_tuple(
					std::make_pair("enable", &StyleSwitchFX::Text::enable),
					std::make_pair("maxAlpha", &StyleSwitchFX::Text::maxAlpha),
					std::make_pair("size", &StyleSwitchFX::Text::size),
                    std::make_pair("color", &StyleSwitchFX::Text::color)
				);
			}

		} Text;

		static constexpr auto Metadata() {
			return std::make_tuple(
				std::make_pair("Flux", &StyleSwitchFX::Flux),
				std::make_pair("Text", &StyleSwitchFX::Text)
				);
		}

	} StyleSwitchFX;

	struct SFX {
		uint8 changeGunNew = 1;
		uint8 changeDevilArmNew = 1;
		uint32 changeWeaponVolume = 30;
		uint32 styleChangeEffectVolume = 10;
		uint32 styleChangeVOVolume = 50;
		uint32 sprintVolume = 20;
		uint32 devilTriggerInL1Volume = 40;
		uint32 devilTriggerInL2Volume = 20;
		uint32 devilTriggerOutVolume = 50;
		uint32 devilTriggerReadyVolume = 110;
		uint32 doppelgangerInVolume = 50;
		uint32 doppelgangerOutVolume = 50;
		uint32 quicksilverInVolume = 50;
		uint32 styleRankAnnouncerVolume = 255;
		uint32 styleRankAnnouncerCooldownSeconds = 20;

		static constexpr auto Metadata() {
			return std::make_tuple(
				std::make_pair("changeGunNew", &SFX::changeGunNew),
                std::make_pair("changeDevilArmNew", &SFX::changeDevilArmNew),
                std::make_pair("changeWeaponVolume", &SFX::changeWeaponVolume),
                std::make_pair("styleChangeEffectVolume", &SFX::styleChangeEffectVolume),
                std::make_pair("styleChangeVOVolume", &SFX::styleChangeVOVolume),
                std::make_pair("sprintVolume", &SFX::sprintVolume),
                std::make_pair("devilTriggerInL1Volume", &SFX::devilTriggerInL1Volume),
                std::make_pair("devilTriggerInL2Volume", &SFX::devilTriggerInL2Volume),
                std::make_pair("devilTriggerOutVolume", &SFX::devilTriggerOutVolume),
                std::make_pair("devilTriggerReadyVolume", &SFX::devilTriggerReadyVolume),
                std::make_pair("doppelgangerInVolume", &SFX::doppelgangerInVolume),
                std::make_pair("doppelgangerOutVolume", &SFX::doppelgangerOutVolume),
                std::make_pair("quicksilverInVolume", &SFX::quicksilverInVolume),
                std::make_pair("styleRankAnnouncerVolume", &SFX::styleRankAnnouncerVolume),
                std::make_pair("styleRankAnnouncerCooldownSeconds", &SFX::styleRankAnnouncerCooldownSeconds)
			);
		}
	} SFX;

	struct Gameplay {

        struct General {
			bool inertia = true;
			bool sprint = true;
			bool freeformSoftLock = true;
			bool bufferlessReversals = true;
			bool dmc4LockOnDirection = true;
            bool holdToCrazyCombo = true;
			bool disableHeightRestriction = true;
			bool improvedBufferedReversals = true;
			bool increasedJCSpheres = true;
			bool disableJCRestriction = true;
			bool increasedEnemyJuggleTime = true;
			bool fasterTurnRate = true;
            

			static constexpr auto Metadata() {
				return std::make_tuple(
					std::make_pair("inertia", &General::inertia),
					std::make_pair("sprint", &General::sprint),
					std::make_pair("freeformSoftLock", &General::freeformSoftLock),
					std::make_pair("bufferlessReversals", &General::bufferlessReversals),
					std::make_pair("improvedBufferedReversals", &General::improvedBufferedReversals),
					std::make_pair("dmc4LockOnDirection", &General::dmc4LockOnDirection),
                    std::make_pair("holdToCrazyCombo", &General::holdToCrazyCombo),
					std::make_pair("disableHeightRestriction", &General::disableHeightRestriction),
					std::make_pair("improvedBufferedReversals", &General::improvedBufferedReversals),
					std::make_pair("increasedJCSpheres", &General::increasedJCSpheres),
					std::make_pair("disableJCRestriction", &General::disableJCRestriction),
					std::make_pair("increasedEnemyJuggleTime", &General::increasedEnemyJuggleTime),
					std::make_pair("fasterTurnRate", &General::fasterTurnRate)
				);
			}
        } General;

        struct Dante {
			bool improvedCancels = true;
			bool bulletStop = true;
			bool rainstormLift = true;
			bool aerialRaveTweaks = true;
			bool airFlickerTweaks = true;
			bool skyDanceTweaks = true;
			bool shotgunAirShotTweaks = true;
			bool driveTweaks = true;
			bool disableAirSlashKnockback = true;
			bool airStinger = true;
			bool airRisingDragonWhirlwind = true;
			bool dmc4Mobility = true;
			bool dTInfusedRoyalguard = true;

			static constexpr auto Metadata() {
                return std::make_tuple(
				    std::make_pair("improvedCancels", &Dante::improvedCancels),
					std::make_pair("bulletStop", &Dante::bulletStop),
					std::make_pair("rainstormLift", &Dante::rainstormLift),
					std::make_pair("aerialRaveTweaks", &Dante::aerialRaveTweaks),
					std::make_pair("airFlickerTweaks", &Dante::airFlickerTweaks),
					std::make_pair("skyDanceTweaks", &Dante::skyDanceTweaks),
					std::make_pair("shotgunAirShotTweaks", &Dante::shotgunAirShotTweaks),
					std::make_pair("driveTweaks", &Dante::driveTweaks),
					std::make_pair("disableAirSlashKnockback", &Dante::disableAirSlashKnockback),
					std::make_pair("airStinger", &Dante::airStinger),
					std::make_pair("airRisingDragonWhirlwind", &Dante::airRisingDragonWhirlwind),
					std::make_pair("dmc4Mobility", &Dante::dmc4Mobility),
					std::make_pair("dTInfusedRoyalguard", &Dante::dTInfusedRoyalguard)
				);
			}
        } Dante;

		struct Vergil {
			bool enableQuicksilver = false;
			bool darkslayerTrickCancels = true;
			bool airStinger = true;
			std::string adjustRisingSunPos = "From Air";
			std::string adjustLunarPhasePos = "From Air";

			static constexpr auto Metadata() {
				return std::make_tuple(
					std::make_pair("enableQuicksilver", &Vergil::enableQuicksilver),
					std::make_pair("darkslayerTrickCancels", &Vergil::darkslayerTrickCancels),
					std::make_pair("airStinger", &Vergil::airStinger),
					std::make_pair("adjustRisingSunPos", &Vergil::adjustRisingSunPos),
					std::make_pair("adjustLunarPhasePos", &Vergil::adjustLunarPhasePos)
				);
			}
        } Vergil;

		struct Remaps {
			uint16_t danteDTButton = 0x0004;
			uint16_t danteShootButton = 0x0080;
			uint16_t vergilDTButton = 0x0080;
			uint16_t vergilShootButton = 0x0004;

			static constexpr auto Metadata() {
				return std::make_tuple(
					std::make_pair("danteDTButton", &Remaps::danteDTButton),
					std::make_pair("danteShootButton", &Remaps::danteShootButton),
					std::make_pair("vergilDTButton", &Remaps::vergilDTButton),
                    std::make_pair("vergilShootButton", &Remaps::vergilShootButton)
				);
			}
		} Remaps;

		static constexpr auto Metadata() {
			return std::make_tuple(
                std::make_pair("General", &Gameplay::General),
                std::make_pair("Dante", &Gameplay::Dante),
                std::make_pair("Vergil", &Gameplay::Vergil),
                std::make_pair("Remaps", &Gameplay::Remaps)
			);
		}

	} Gameplay;


	struct Cheats {

		struct General {
			bool customMobility = false;
			bool customDamage = false;
			bool customSpeed = false;

			static constexpr auto Metadata() {
				return std::make_tuple(
					std::make_pair("customMobility", &General::customMobility),
					std::make_pair("customDamage", &General::customDamage),
					std::make_pair("customSpeed", &General::customSpeed)
					
				);
			}
		} General;


		static constexpr auto Metadata() {
			return std::make_tuple(
				std::make_pair("General", &Cheats::General)
			);
		}
	} Cheats;

	struct PlayerProperties {
		uint8 playerColor[PLAYER_COUNT][4] = {
			// r   g  b  a  
			{ 158, 27, 63, 255 }, // 1P 
			{ 18, 48, 130, 255 }, // 2P  
			{ 228, 160, 16, 255 }, // 3P    
			{ 49, 127, 67, 255 }, // 4P
		};

		std::string playerName[PLAYER_COUNT] = {
			// r   g  b  a  
			"Player 1", // 1P 
			"Player 2", // 2P  
			"Player 3", // 3P    
			"Player 4", // 4P
		};
		

		static constexpr auto Metadata() {
			return std::make_tuple(
				std::make_pair("playerColor", &PlayerProperties::playerColor),
				std::make_pair("playerName", &PlayerProperties::playerName)
			);
		}
	} PlayerProperties;

	int testNumber = 3000;

	static constexpr auto Metadata() {
		return std::make_tuple(
			std::make_pair("MultiplayerBars2D", &CrimsonConfig::MultiplayerBars2D),
			std::make_pair("MultiplayerBarsWorldSpace", &CrimsonConfig::MultiplayerBarsWorldSpace),
			std::make_pair("GUI", &CrimsonConfig::GUI),
			std::make_pair("Sound", &CrimsonConfig::WeaponWheel),
            std::make_pair("CrimsonHudAddons", &CrimsonConfig::CrimsonHudAddons),
            std::make_pair("Camera", &CrimsonConfig::Camera),
            std::make_pair("StyleSwitchFX", &CrimsonConfig::StyleSwitchFX),
            std::make_pair("SFX", &CrimsonConfig::SFX),
            std::make_pair("Gameplay", &CrimsonConfig::Gameplay),
			std::make_pair("Cheats", &CrimsonConfig::Cheats),
			std::make_pair("PlayerProperties", &CrimsonConfig::PlayerProperties),
			std::make_pair("testNumber", &CrimsonConfig::testNumber)
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
// - Mia

extern CrimsonConfig defaultCrimsonConfig;
extern CrimsonConfig queuedCrimsonConfig;
extern CrimsonConfig activeCrimsonConfig;
#pragma pack(pop)
#pragma optimize("", on) // Re-enable optimizations
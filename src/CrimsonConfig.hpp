#pragma once

#include <stdint.h>
#include "Core/Core.hpp"
#include "Core/Macros.h"
#include "Exp.hpp"
#include "Vars.hpp"
#include "CrimsonConfigGameplay.hpp"

#include "Core/DebugSwitch.hpp"
#pragma optimize("", off) // Disable all optimizations
#pragma pack(push, 8)

namespace HUDELEMENTSCALESTATE {
enum {
	SMALL,
	BIG
};
}

namespace HUDELEMENTSHOWSTATE {
enum {
	OFF,
	ONLY_IN_MP,
	ALWAYS,
};
}

namespace STYLESDISPLAY {
enum {
	OFF,
	WITH_BROKEN_GLASS,
	NO_BROKEN_GLASS,
};
}

namespace RIGHTSTICKCENTERCAM {
enum {
	OFF,
	TO_NEAREST_SIDE,
	ON
};
}

namespace DELAYEDCOMBOSFX {
enum {
	TYPE_A,
	TYPE_B
};
}

struct CrimsonConfig {
	struct MultiplayerBars2D {
		bool show = true;
		uint8 show1PAttributes = HUDELEMENTSHOWSTATE::ONLY_IN_MP;

		static constexpr auto Metadata() {
			return std::make_tuple(
				std::make_pair("show", &MultiplayerBars2D::show),
				std::make_pair("show1PAttributes", &MultiplayerBars2D::show1PAttributes)
			);
		}
	} MultiplayerBars2D;

	struct MultiplayerBarsWorldSpace {
		bool show = true;
		uint8 show1PBar = HUDELEMENTSHOWSTATE::ONLY_IN_MP;
        bool showOutOfViewIcons = true;
		bool showPlayerNames = true;

		static constexpr auto Metadata() {
			return std::make_tuple(
				std::make_pair("show", &MultiplayerBarsWorldSpace::show),
				std::make_pair("show1PBar", &MultiplayerBarsWorldSpace::show1PBar),
                std::make_pair("showOutOfViewIcons", &MultiplayerBarsWorldSpace::showOutOfViewIcons),
				std::make_pair("showPlayerNames", &MultiplayerBarsWorldSpace::showPlayerNames)
			);
		}
	} MultiplayerBarsWorldSpace;

	struct GUI {
		uint8 transparencyMode = 2;
		float opacity = 0.9f;
		bool pauseWhenOpened = true;
		bool sounds = true;
		bool cheatsPopup = true;
		bool disableGamepadShortcut = false;

		static constexpr auto Metadata() {
			return std::make_tuple(
				std::make_pair("transparencyMode", &GUI::transparencyMode),
				std::make_pair("opacity", &GUI::opacity),
				std::make_pair("pauseWhenOpened", &GUI::pauseWhenOpened),
				std::make_pair("sounds", &GUI::sounds),
				std::make_pair("cheatsPopup", &GUI::cheatsPopup),
				std::make_pair("disableGamepadShortcut", &GUI::disableGamepadShortcut)
			);
		}
	} GUI;

	struct WeaponWheel {
		bool analogSwitching = true;
		bool disableCameraRotation = true;
		std::string theme = "Crimson";
		uint8 scale = HUDELEMENTSCALESTATE::SMALL;
		uint8 alwaysShow = HUDELEMENTSHOWSTATE::ONLY_IN_MP;
		bool force1PMultiplayerPosScale = false;
		bool hide = false;
		uint8 worldSpaceWheels = HUDELEMENTSHOWSTATE::ONLY_IN_MP;
		uint8 worldSpaceAlwaysShow = HUDELEMENTSHOWSTATE::OFF;

		static constexpr auto Metadata() {
			return std::make_tuple(
				std::make_pair("analogSwitching", &WeaponWheel::analogSwitching),
				std::make_pair("disableCameraRotation", &WeaponWheel::disableCameraRotation),
				std::make_pair("theme", &WeaponWheel::theme),
				std::make_pair("scale", &WeaponWheel::scale),
				std::make_pair("alwaysShow", &WeaponWheel::alwaysShow),
				std::make_pair("force1PMultiplayerPosScale", &WeaponWheel::force1PMultiplayerPosScale),
				std::make_pair("hide", &WeaponWheel::hide),
				std::make_pair("worldSpaceWheels", &WeaponWheel::worldSpaceWheels),
				std::make_pair("worldSpaceAlwaysShow", &WeaponWheel::worldSpaceAlwaysShow)
			);
		}

	} WeaponWheel;

	struct CrimsonHudAddons {
		bool positionings = true;
		uint8 stylesDisplay = STYLESDISPLAY::WITH_BROKEN_GLASS;
		bool displayStyleNames = true;
		bool redOrbCounter = true;
		bool royalGauge = true;
		bool styleRanksMeter = true;
		bool lockOn = true;
		bool scaleLockOnEnemyDistance = false;
		bool stunDisplacementNumericHud = false;
		bool lockOnColorsCharacter = true;
		std::string selectedStyleRanks = "Crimson";
		std::string selectedStyleRanksAccolades = "1 - DMC3Default";

		static constexpr auto Metadata() {
			return std::make_tuple(
				std::make_pair("positionings", &CrimsonHudAddons::positionings),
				std::make_pair("stylesDisplay", &CrimsonHudAddons::stylesDisplay),
				std::make_pair("displayStyleNames", &CrimsonHudAddons::displayStyleNames),
                std::make_pair("redOrbCounter", &CrimsonHudAddons::redOrbCounter),
                std::make_pair("royalGauge", &CrimsonHudAddons::royalGauge),
                std::make_pair("styleRanksMeter", &CrimsonHudAddons::styleRanksMeter),
                std::make_pair("lockOn", &CrimsonHudAddons::lockOn),
				std::make_pair("scaleLockOnEnemyDistance", &CrimsonHudAddons::scaleLockOnEnemyDistance),
				std::make_pair("stunDisplacementNumericHud", &CrimsonHudAddons::stunDisplacementNumericHud),
				std::make_pair("lockOnColorsCharacter", &CrimsonHudAddons::lockOnColorsCharacter),
				std::make_pair("selectedStyleRanks", &CrimsonHudAddons::selectedStyleRanks),
				std::make_pair("selectedStyleRanksAccolades", &CrimsonHudAddons::selectedStyleRanksAccolades)
			);
		}
	} CrimsonHudAddons;

	struct HudOptions {
		bool hideStyleMeter = false;

		static constexpr auto Metadata() {
			return std::make_tuple(
				std::make_pair("hideStyleMeter", &HudOptions::hideStyleMeter)
			);
		}
	} HudOptions;


	struct Camera {
		float fovMultiplier = 1.2f;
		uint8 sensitivity = 2;
		uint8 followUpSpeed = 2;
		uint8 distance = 2;
		uint8 lockOnDistance = 2;
		uint8 verticalTilt = 0;
		bool lockedOff = true;
		bool invertX = true;
		uint8 autoAdjust = 0;
		uint8 rightStickCameraCentering = RIGHTSTICKCENTERCAM::TO_NEAREST_SIDE;
		bool disableBossCamera = false;
		bool multiplayerCamera = true;
		bool panoramicCam = true;
		bool thirdPersonCamera = true;

		static constexpr auto Metadata() {
			return std::make_tuple(
				std::make_pair("fovMultiplier", &Camera::fovMultiplier),
				std::make_pair("sensitivity", &Camera::sensitivity),
                std::make_pair("followUpSpeed", &Camera::followUpSpeed),
                std::make_pair("distance", &Camera::distance),
                std::make_pair("lockOnDistance", &Camera::lockOnDistance),
                std::make_pair("verticalTilt", &Camera::verticalTilt),
                std::make_pair("lockedOff", &Camera::lockedOff),
                std::make_pair("invertX", &Camera::invertX),
                std::make_pair("autoAdjust", &Camera::autoAdjust),
                std::make_pair("rightStickCameraCentering", &Camera::rightStickCameraCentering),
                std::make_pair("disableBossCamera", &Camera::disableBossCamera),
				std::make_pair("multiplayerCamera", &Camera::multiplayerCamera),
				std::make_pair("panoramicCam", &Camera::panoramicCam),
                std::make_pair("thirdPersonCamera", &Camera::thirdPersonCamera)
			);
		}
	} Camera;

	struct StyleSwitchFX {

		struct Flux {
			bool enable = true;

			uint8 color[7][4] = {
				// r   g  b  a 
				{ 29, 29, 0, 255 }, //trick  
				{ 26, 0, 0, 255 }, //sword  
				{ 0, 8, 34, 255 }, //gun    
				{ 0, 35, 6, 255 }, //royal  
				{ 26, 0, 35, 255 }, //quick  
				{ 30, 14, 0, 255 }, //doppel 
				{ 0, 25, 30, 255 } // vergil
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

	struct VFX {
		bool delayedComboVFX = true;

		static constexpr auto Metadata() {
			return std::make_tuple(
				std::make_pair("delayedComboVFX", &VFX::delayedComboVFX)
			);
		}
	} VFX;

	struct SFX {
		uint8 changeGunNew = 1;
		uint8 changeDevilArmNew = 1;
		uint8 changeWeaponEffectVolume = 100;
		uint8 styleChangeVolume = 100;
		uint8 styleChangeVoiceOverVolume = 100;
		uint8 sprintEffectVolume = 100;
		uint8 dTInL1Volume = 100;
		uint8 dTInL2Volume = 100;
		uint8 dTOutVolume = 100;
		uint8 dTReadyVolume = 100;
		uint8 doppelInVolume = 100;
		uint8 doppelOutVolume = 100;
		uint8 quickInVolume = 100;
		uint8 quickOutVolume = 100;
		uint8 announcerVolume = 100;
		uint32 styleRankAnnouncerCooldownSec = 30;
		bool onlyResetAnnouncerWhenHit = true;
		uint8 delayedComboEffectType = 0;
		uint8 delayedComboIndicatorVolume = 100;
		uint8 royalBlockVolume = 100;
		uint8 normalBlockVolume = 100;
		uint8 dTEStartLoopVolume = 100;
		uint8 dTEFinishVolume = 100;
		uint8 dTEReleaseVolume = 100;

		static constexpr auto Metadata() {
			return std::make_tuple(
				std::make_pair("changeGunNew", &SFX::changeGunNew),
                std::make_pair("changeDevilArmNew", &SFX::changeDevilArmNew),
                std::make_pair("changeWeaponEffectVolume", &SFX::changeWeaponEffectVolume),
                std::make_pair("styleChangeVolume", &SFX::styleChangeVolume),
                std::make_pair("styleChangeVoiceOverVolume", &SFX::styleChangeVoiceOverVolume),
                std::make_pair("sprintEffectVolume", &SFX::sprintEffectVolume),
                std::make_pair("dTInL1Volume", &SFX::dTInL1Volume),
                std::make_pair("dTInL2Volume", &SFX::dTInL2Volume),
                std::make_pair("dTOutVolume", &SFX::dTOutVolume),
                std::make_pair("dTReadyVolume", &SFX::dTReadyVolume),
                std::make_pair("doppelInVolume", &SFX::doppelInVolume),
                std::make_pair("doppelOutVolume", &SFX::doppelOutVolume),
                std::make_pair("quickInVolume", &SFX::quickInVolume),
				std::make_pair("quickOutVolume", &SFX::quickOutVolume),
                std::make_pair("announcerVolume", &SFX::announcerVolume),
                std::make_pair("styleRankAnnouncerCooldownSec", &SFX::styleRankAnnouncerCooldownSec),
				std::make_pair("onlyResetAnnouncerWhenHit", &SFX::onlyResetAnnouncerWhenHit),
				std::make_pair("delayedComboIndicatorVolume", &SFX::delayedComboIndicatorVolume),
				std::make_pair("delayedComboEffectType", &SFX::delayedComboEffectType),
				std::make_pair("royalBlockVolume", &SFX::royalBlockVolume),
				std::make_pair("normalBlockVolume", &SFX::normalBlockVolume),
				std::make_pair("dTEStartLoopVolume", &SFX::dTEStartLoopVolume),
				std::make_pair("dTEFinishVolume", &SFX::dTEFinishVolume),
				std::make_pair("dTEReleaseVolume", &SFX::dTEReleaseVolume)
			);
		}
	} SFX;

	struct Sound {
		uint8 channelVolumes[CHANNEL::MAX] = {
			100,
			100,
			100,
			100,
			100,
			100,
			100,
			100,
			100,
			100,
			100,
		};
		bool overrideMissionStartSong = false;

		static constexpr auto Metadata() {
			return std::make_tuple(
				std::make_pair("channelVolumes", &Sound::channelVolumes),
				std::make_pair("overrideMissionStartSong", &Sound::overrideMissionStartSong)
			);
		}
	} Sound;

	struct PlayerProperties {
		uint8 playerColor[PLAYER_COUNT][4] = {
			// r   g  b  a  
			{ 222, 28, 76, 255 }, // 1P 
			{ 12, 133, 197, 255 }, // 2P  
			{ 255, 230, 0, 255 }, // 3P    
			{ 0, 192, 70, 255 }, // 4P
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

	struct System {
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
				std::make_pair("Remaps", &System::Remaps)
			);
		}
	} System;

	struct CachedSettings {
		uint8 lastMaxMeleeWeaponCount[PLAYER_COUNT][CHARACTER_COUNT] = {
			{ 5, 5, 5}, // 1P
			{ 5, 5, 5}, // 2P
			{ 5, 5, 5}, // 3P
			{ 5, 5, 5}, // 4P
		};

		uint8 lastMaxRangedWeaponCount[PLAYER_COUNT][CHARACTER_COUNT] = {
			{ 5, 5, 5}, // 1P
			{ 5, 5, 5}, // 2P
			{ 5, 5, 5}, // 3P
			{ 5, 5, 5}, // 4P
		};

		uint8 lastMaxMeleeWeaponCountVergil[PLAYER_COUNT][CHARACTER_COUNT] = {
			{ 3, 3, 3}, // 1P
			{ 3, 3, 3}, // 2P
			{ 3, 3, 3}, // 3P
			{ 3, 3, 3}, // 4P
		};

		uint8 lastEquippedMeleeWeapons[PLAYER_COUNT][CHARACTER_COUNT][MELEE_WEAPON_COUNT_DANTE] = {
			{ // 1P
				{ WEAPON::REBELLION, WEAPON::CERBERUS, WEAPON::AGNI_RUDRA, WEAPON::NEVAN, WEAPON::BEOWULF_DANTE }, // Character 0
				{ WEAPON::REBELLION, WEAPON::CERBERUS, WEAPON::AGNI_RUDRA, WEAPON::NEVAN, WEAPON::BEOWULF_DANTE }, // Character 1
				{ WEAPON::REBELLION, WEAPON::CERBERUS, WEAPON::AGNI_RUDRA, WEAPON::NEVAN, WEAPON::BEOWULF_DANTE }  // Character 2
			},
			{ // 2P
				{ WEAPON::REBELLION, WEAPON::CERBERUS, WEAPON::AGNI_RUDRA, WEAPON::NEVAN, WEAPON::BEOWULF_DANTE },
				{ WEAPON::REBELLION, WEAPON::CERBERUS, WEAPON::AGNI_RUDRA, WEAPON::NEVAN, WEAPON::BEOWULF_DANTE },
				{ WEAPON::REBELLION, WEAPON::CERBERUS, WEAPON::AGNI_RUDRA, WEAPON::NEVAN, WEAPON::BEOWULF_DANTE }
			},
			{ // 3P
				{ WEAPON::REBELLION, WEAPON::CERBERUS, WEAPON::AGNI_RUDRA, WEAPON::NEVAN, WEAPON::BEOWULF_DANTE },
				{ WEAPON::REBELLION, WEAPON::CERBERUS, WEAPON::AGNI_RUDRA, WEAPON::NEVAN, WEAPON::BEOWULF_DANTE },
				{ WEAPON::REBELLION, WEAPON::CERBERUS, WEAPON::AGNI_RUDRA, WEAPON::NEVAN, WEAPON::BEOWULF_DANTE }
			},
			{ // 4P
				{ WEAPON::REBELLION, WEAPON::CERBERUS, WEAPON::AGNI_RUDRA, WEAPON::NEVAN, WEAPON::BEOWULF_DANTE },
				{ WEAPON::REBELLION, WEAPON::CERBERUS, WEAPON::AGNI_RUDRA, WEAPON::NEVAN, WEAPON::BEOWULF_DANTE },
				{ WEAPON::REBELLION, WEAPON::CERBERUS, WEAPON::AGNI_RUDRA, WEAPON::NEVAN, WEAPON::BEOWULF_DANTE }
			}
		};

		uint8 lastEquippedRangedWeapons[PLAYER_COUNT][CHARACTER_COUNT][RANGED_WEAPON_COUNT_DANTE] = {
			{ // 1P
				{ WEAPON::EBONY_IVORY, WEAPON::SHOTGUN, WEAPON::ARTEMIS, WEAPON::SPIRAL, WEAPON::KALINA_ANN }, // Character 0
				{ WEAPON::EBONY_IVORY, WEAPON::SHOTGUN, WEAPON::ARTEMIS, WEAPON::SPIRAL, WEAPON::KALINA_ANN }, // Character 1
				{ WEAPON::EBONY_IVORY, WEAPON::SHOTGUN, WEAPON::ARTEMIS, WEAPON::SPIRAL, WEAPON::KALINA_ANN }  // Character 2
			},
			{ // 2P
				{ WEAPON::EBONY_IVORY, WEAPON::SHOTGUN, WEAPON::ARTEMIS, WEAPON::SPIRAL, WEAPON::KALINA_ANN },
				{ WEAPON::EBONY_IVORY, WEAPON::SHOTGUN, WEAPON::ARTEMIS, WEAPON::SPIRAL, WEAPON::KALINA_ANN },
				{ WEAPON::EBONY_IVORY, WEAPON::SHOTGUN, WEAPON::ARTEMIS, WEAPON::SPIRAL, WEAPON::KALINA_ANN }
			},
			{ // 3P
				{ WEAPON::EBONY_IVORY, WEAPON::SHOTGUN, WEAPON::ARTEMIS, WEAPON::SPIRAL, WEAPON::KALINA_ANN },
				{ WEAPON::EBONY_IVORY, WEAPON::SHOTGUN, WEAPON::ARTEMIS, WEAPON::SPIRAL, WEAPON::KALINA_ANN },
				{ WEAPON::EBONY_IVORY, WEAPON::SHOTGUN, WEAPON::ARTEMIS, WEAPON::SPIRAL, WEAPON::KALINA_ANN }
			},
			{ // 4P
				{ WEAPON::EBONY_IVORY, WEAPON::SHOTGUN, WEAPON::ARTEMIS, WEAPON::SPIRAL, WEAPON::KALINA_ANN },
				{ WEAPON::EBONY_IVORY, WEAPON::SHOTGUN, WEAPON::ARTEMIS, WEAPON::SPIRAL, WEAPON::KALINA_ANN },
				{ WEAPON::EBONY_IVORY, WEAPON::SHOTGUN, WEAPON::ARTEMIS, WEAPON::SPIRAL, WEAPON::KALINA_ANN }
			}
		};

		uint8 lastEquippedMeleeWeaponsVergil[PLAYER_COUNT][CHARACTER_COUNT][MELEE_WEAPON_COUNT_VERGIL] = {
			{ // 1P
				{ WEAPON::YAMATO_VERGIL, WEAPON::BEOWULF_VERGIL, WEAPON::YAMATO_FORCE_EDGE}, // Character 0
				{ WEAPON::YAMATO_VERGIL, WEAPON::BEOWULF_VERGIL, WEAPON::YAMATO_FORCE_EDGE}, // Character 1
				{ WEAPON::YAMATO_VERGIL, WEAPON::BEOWULF_VERGIL, WEAPON::YAMATO_FORCE_EDGE}  // Character 2
			},
			{ // 2P
				{ WEAPON::YAMATO_VERGIL, WEAPON::BEOWULF_VERGIL, WEAPON::YAMATO_FORCE_EDGE},
				{ WEAPON::YAMATO_VERGIL, WEAPON::BEOWULF_VERGIL, WEAPON::YAMATO_FORCE_EDGE},
				{ WEAPON::YAMATO_VERGIL, WEAPON::BEOWULF_VERGIL, WEAPON::YAMATO_FORCE_EDGE}
			},
			{ // 3P
				{ WEAPON::YAMATO_VERGIL, WEAPON::BEOWULF_VERGIL, WEAPON::YAMATO_FORCE_EDGE},
				{ WEAPON::YAMATO_VERGIL, WEAPON::BEOWULF_VERGIL, WEAPON::YAMATO_FORCE_EDGE},
				{ WEAPON::YAMATO_VERGIL, WEAPON::BEOWULF_VERGIL, WEAPON::YAMATO_FORCE_EDGE}
			},
			{ // 4P
				{ WEAPON::YAMATO_VERGIL, WEAPON::BEOWULF_VERGIL, WEAPON::YAMATO_FORCE_EDGE},
				{ WEAPON::YAMATO_VERGIL, WEAPON::BEOWULF_VERGIL, WEAPON::YAMATO_FORCE_EDGE},
				{ WEAPON::YAMATO_VERGIL, WEAPON::BEOWULF_VERGIL, WEAPON::YAMATO_FORCE_EDGE}
			}
		};

		static constexpr auto Metadata() {
			return std::make_tuple(
				std::make_pair("lastMaxMeleeWeaponCount", &CachedSettings::lastMaxMeleeWeaponCount),
				std::make_pair("lastMaxRangedWeaponCount", &CachedSettings::lastMaxRangedWeaponCount),
				std::make_pair("lastMaxMeleeWeaponCountVergil", &CachedSettings::lastMaxMeleeWeaponCountVergil),
				std::make_pair("lastEquippedMeleeWeapons", &CachedSettings::lastEquippedMeleeWeapons),
				std::make_pair("lastEquippedRangedWeapons", &CachedSettings::lastEquippedRangedWeapons),
				std::make_pair("lastEquippedMeleeWeaponsVergil", &CachedSettings::lastEquippedMeleeWeaponsVergil)
			);
		}
	} CachedSettings;

	static constexpr auto Metadata() {
		return std::make_tuple(
			std::make_pair("MultiplayerBars2D", &CrimsonConfig::MultiplayerBars2D),
			std::make_pair("MultiplayerBarsWorldSpace", &CrimsonConfig::MultiplayerBarsWorldSpace),
			std::make_pair("GUI", &CrimsonConfig::GUI),
			std::make_pair("WeaponWheel", &CrimsonConfig::WeaponWheel),
            std::make_pair("CrimsonHudAddons", &CrimsonConfig::CrimsonHudAddons),
			std::make_pair("HudOptions", &CrimsonConfig::HudOptions),
            std::make_pair("Camera", &CrimsonConfig::Camera),
            std::make_pair("StyleSwitchFX", &CrimsonConfig::StyleSwitchFX),
			std::make_pair("VFX", &CrimsonConfig::VFX),
            std::make_pair("SFX", &CrimsonConfig::SFX),
			std::make_pair("Sound", &CrimsonConfig::Sound),
			std::make_pair("PlayerProperties", &CrimsonConfig::PlayerProperties),
			std::make_pair("System", &CrimsonConfig::System),
			std::make_pair("CachedSettings", &CrimsonConfig::CachedSettings)
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
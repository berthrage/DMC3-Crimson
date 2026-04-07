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

#define NUM_GAMEPADBINDS 16
#define NUM_KEYBINDS 24

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

namespace MISSIONTIMERDISPLAY {
enum {
	OFF,
	ONLY_IN_BP,
	ALWAYS,
};
}

namespace RIGHTSTICKCENTERCAM {
enum {
	OFF,
	TO_NEAREST_SIDE,
	ON
};
}

namespace CAMERASHAKE {
	enum {
		OFF,
		ONLY_IN_SINGLE_PLAYER_CAM,
		ALWAYS_ON
	};
}

namespace DELAYEDCOMBOSFX {
enum {
	TYPE_A,
	TYPE_B
};
}

namespace STYLESWITCHVFXTYPE {
	enum {
	CRIMSON,
	NSWITCH
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
		bool stylishPtsCounter = true;
		uint8 missionTimerDisplay = MISSIONTIMERDISPLAY::ONLY_IN_BP;
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
				std::make_pair("missionTimerDisplay", &CrimsonHudAddons::missionTimerDisplay),
				std::make_pair("stylishPtsCounter", &CrimsonHudAddons::stylishPtsCounter),
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
		uint8 followUpSpd = 0;
		uint8 distance = 2;
		uint8 lockOnDistance = 2;
		uint8 verticalTilt = 0;
		bool lockedOff = true;
		bool invertX = true;
		uint8 autoAdjust = 0;
		uint8 rightStickCameraCentering = RIGHTSTICKCENTERCAM::ON;
		uint8 cameraShake = CAMERASHAKE::ONLY_IN_SINGLE_PLAYER_CAM;
		bool disableBossCamera = false;
		bool multiplayerCamera = true;
		bool panoramicCam = true;
		bool thirdPersonCamera = true;

		static constexpr auto Metadata() {
			return std::make_tuple(
				std::make_pair("fovMultiplier", &Camera::fovMultiplier),
				std::make_pair("sensitivity", &Camera::sensitivity),
                std::make_pair("followUpSpd", &Camera::followUpSpd),
                std::make_pair("distance", &Camera::distance),
                std::make_pair("lockOnDistance", &Camera::lockOnDistance),
                std::make_pair("verticalTilt", &Camera::verticalTilt),
                std::make_pair("lockedOff", &Camera::lockedOff),
                std::make_pair("invertX", &Camera::invertX),
                std::make_pair("autoAdjust", &Camera::autoAdjust),
                std::make_pair("rightStickCameraCentering", &Camera::rightStickCameraCentering),
				std::make_pair("cameraShake", &Camera::cameraShake),
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
			uint8 type = STYLESWITCHVFXTYPE::CRIMSON;

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
					std::make_pair("type", &StyleSwitchFX::Flux::type),
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
		uint8 delayedComboColor[4] = { 48, 0, 10, 255 };
		bool royalBlockVFX = true;
		uint8 royalBlockColor[4] = { 226, 4, 50, 255 };
		bool dtExplosionVFX = true;
		uint8 dtExplosionColorDante[4] = { 48, 0, 10, 255 };
		uint8 dtExplosionColorVergil[4] = { 2, 16, 43, 255 };
		bool dtActivationVibration = true;

		static constexpr auto Metadata() {
			return std::make_tuple(
				std::make_pair("delayedComboVFX", &VFX::delayedComboVFX),
				std::make_pair("delayedComboColor", &VFX::delayedComboColor),
				std::make_pair("royalBlockVFX", &VFX::royalBlockVFX),
				std::make_pair("royalBlockColor", &VFX::royalBlockColor),
				std::make_pair("dtExplosionVFX", &VFX::dtExplosionVFX),
				std::make_pair("dtExplosionColorDante", &VFX::dtExplosionColorDante),
				std::make_pair("dtExplosionColorVergil", &VFX::dtExplosionColorVergil),
				std::make_pair("dtActivationVibration", &VFX::dtActivationVibration)
			);
		}
	} VFX;

	struct SFX {
		uint8 changeGunNew = 1;
		uint8 changeDevilArmNew = 1;
		uint8 changeWeaponEffectVolume = 100;
		uint8 styleChangeNew = 1;
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
				std::make_pair("styleChangeNew", &SFX::styleChangeNew),
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
		struct ButtonConfig {
			uint16_t dante1P[NUM_GAMEPADBINDS] = {
				0x1000, 0x4000, 0x2000, 0x8000,
				0x10,   0x40,   0x20,   0x80,
				0x4,    0x1,    0x200,  0x8,
				0x2,    0x400,  0x100,  0x800
			};

			uint16_t vergil1P[NUM_GAMEPADBINDS] = {
				0x1000, 0x4000, 0x2000, 0x8000,
				0x10,   0x40,   0x20,   0x80,
				0x4,    0x1,    0x200,  0x8,
				0x2,    0x400,  0x100,  0x800
			};

			uint16_t dante2P[NUM_GAMEPADBINDS] = {
				0x1000, 0x4000, 0x2000, 0x8000,
				0x10,   0x40,   0x20,   0x80,
				0x4,    0x1,    0x200,  0x8,
				0x2,    0x400,  0x100,  0x800
			};

			uint16_t vergil2P[NUM_GAMEPADBINDS] = {
				0x1000, 0x4000, 0x2000, 0x8000,
				0x10,   0x40,   0x20,   0x80,
				0x4,    0x1,    0x200,  0x8,
				0x2,    0x400,  0x100,  0x800
			};

			uint16_t dante3P[NUM_GAMEPADBINDS] = {
				0x1000, 0x4000, 0x2000, 0x8000,
				0x10,   0x40,   0x20,   0x80,
				0x4,    0x1,    0x200,  0x8,
				0x2,    0x400,  0x100,  0x800
			};

			uint16_t vergil3P[NUM_GAMEPADBINDS] = {
				0x1000, 0x4000, 0x2000, 0x8000,
				0x10,   0x40,   0x20,   0x80,
				0x4,    0x1,    0x200,  0x8,
				0x2,    0x400,  0x100,  0x800
			};

			uint16_t dante4P[NUM_GAMEPADBINDS] = {
				0x1000, 0x4000, 0x2000, 0x8000,
				0x10,   0x40,   0x20,   0x80,
				0x4,    0x1,    0x200,  0x8,
				0x2,    0x400,  0x100,  0x800
			};

			uint16_t vergil4P[NUM_GAMEPADBINDS] = {
				0x1000, 0x4000, 0x2000, 0x8000,
				0x10,   0x40,   0x20,   0x80,
				0x4,    0x1,    0x200,  0x8,
				0x2,    0x400,  0x100,  0x800
			};

			static constexpr auto Metadata() {
				return std::make_tuple(
					std::make_pair("dante1P", &ButtonConfig::dante1P),
					std::make_pair("vergil1P", &ButtonConfig::vergil1P),
					std::make_pair("dante2P", &ButtonConfig::dante2P),
					std::make_pair("vergil2P", &ButtonConfig::vergil2P),
					std::make_pair("dante3P", &ButtonConfig::dante3P),
					std::make_pair("vergil3P", &ButtonConfig::vergil3P),
					std::make_pair("dante4P", &ButtonConfig::dante4P),
					std::make_pair("vergil4P", &ButtonConfig::vergil4P)
				);
			}
		} ButtonConfig;

		struct KeyboardConfig {
			uint32 keybinds[NUM_KEYBINDS] = {
				DI8::KEY::LEFT_SHIFT, // keyboard_0(HDCdefault: 54) - SELECT / TAUNT - dmc3.exe+5611A0
				DI8::KEY::U, // keyboard_1(HDCdefault: 49) - LB / DEVIL TRIGGER - dmc3.exe+5611A4
				DI8::KEY::C, // keyboard_2(HDCdefault: 46) - LS / CHANGE TARGET - dmc3.exe + 5611A8
				DI8::KEY::ONE, // keyboard_3(HDCdefault: 2) - DPAD UP - dmc3.exe + 5611AC
				DI8::KEY::TWO, // keyboard_4(HDCdefault: 3) - DPAD RIGHT - dmc3.exe + 5611B0
				DI8::KEY::FOUR, // keyboard_5(HDCdefault: 5) - DPAD DOWN - dmc3.exe + 5611B4
				DI8::KEY::THREE, // keyboard_6(HDCdefault: 4) - DPAD LEFT - dmc3.exe + 5611B8
				DI8::KEY::M, // keyboard_7(HDCdefault: 50) - START - dmc3.exe + 5611BC
				DI8::KEY::SPACE, // keyboard_8(HDCdefault: 57) - RB / LOCK ON - dmc3.exe + 5611C0
				DI8::KEY::F, // keyboard_9(HDCdefault: 33) - RS / DEFAULT CAMERA - dmc3.exe + 5611C4
				DI8::KEY::I, // keyboard_10(HDCdefault: 23) - Y / MELEE ATK - dmc3.exe + 5611C8
				DI8::KEY::L, // keyboard_11(HDCdefault: 38) - B / STYLE - dmc3.exe + 5611CC
				DI8::KEY::K, // keyboard_12(HDCdefault: 37) - A / JUMP - dmc3.exe + 5611D0
				DI8::KEY::J, // keyboard_13(HDCdefault: 36) - X / SHOOT - dmc3.exe + 5611D4
				DI8::KEY::W, // keyboard_14(HDCdefault: 17) - LEFT ANALOG UP - dmc3.exe + 5611D8
				DI8::KEY::D, // keyboard_15(HDCdefault: 32) - LEFT ANALOG RIGHT - dmc3.exe + 5611DC
				DI8::KEY::S, // keyboard_16(0HDCdefault: 31) - LEFT ANALOG DOWN - dmc3.exe + 5611E0
				DI8::KEY::A, // keyboard_17(HDCdefault: 30) - LEFT ANALOG LEFT - dmc3.exe + 5611E4
				DI8::KEY::UP, // keyboard_18(HDCdefault: 72) - RIGHT ANALOG UP - dmc3.exe+5611E8
				DI8::KEY::RIGHT, // keyboard_19(HDCdefault: 77) - RIGHT ANALOG RIGHT - dmc3.exe+5611EC
				DI8::KEY::DOWN, // keyboard_20(HDCdefault: 80) - RIGHT ANALOG DOWN - dmc3.exe+5611F0
				DI8::KEY::LEFT, // keyboard_21(HDCdefault: 75) - RIGHT ANALOG LEFT - dmc3.exe+5611F4
				DI8::KEY::Q, // keyboard_22(HDCdefault: 16) - LT / CHANGE GUN - dmc3.exe + 5611F8
				DI8::KEY::E, // keyboard_23(HDCdefault: 18) - RT / CHANGE DEVIL ARM - dmc3.exe + 5611FC
			};

			static constexpr auto Metadata() {
				return std::make_tuple(
					std::make_pair("keybinds", &KeyboardConfig::keybinds)
				);
			}

		} KeyboardConfig;

		uint8 xinputSlots[PLAYER_COUNT] = { 0, 1, 2, 3 };

		bool flipModelPresentation = true;


		static constexpr auto Metadata() {
			return std::make_tuple(
				std::make_pair("ButtonConfig", &System::ButtonConfig),
				std::make_pair("KeyboardConfig", &System::KeyboardConfig),
				std::make_pair("xinputSlots", &System::xinputSlots),
				std::make_pair("flipModelPresentation", &System::flipModelPresentation)
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
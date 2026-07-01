#pragma once

#include <stdint.h>
#include "Core/Core.hpp"
#include "Core/Macros.h"
#include "Exp.hpp"
#include "Vars.hpp"
#include "CrimsonConfig.hpp"

#include "Core/DebugSwitch.hpp"
#pragma optimize("", off) // Disable all optimizations
#pragma pack(push, 8)

// This struct is serialized to CrimsonInput.json, separate from CrimsonConfig.json.
// It contains gamepad and keyboard bindings that were previously part of
// CrimsonConfig::System::ButtonConfig and CrimsonConfig::System::KeyboardConfig.
struct CrimsonInput {

	// A single bindable input with two slots (primary A, secondary B).
	// Values > 0xFFFF represent touchpad zones (SDL-only).
	struct BindPair {
		uint32 slotA = 0;
		uint32 slotB = 0;

		static constexpr auto Metadata() {
			return std::make_tuple(
				std::make_pair("slotA", &BindPair::slotA),
				std::make_pair("slotB", &BindPair::slotB)
			);
		}
	};

	struct ButtonConfig {
		// Each bind has two slots (primary A, secondary B). Touchpad zone values > 0xFFFF.
		// Taunt (index 14) defaults to slot B = TOUCHPAD_LEFT on all characters.
		BindPair dante1P[NUM_GAMEPADBINDS] = {
			{GAMEPAD::UP, 0},    {GAMEPAD::DOWN, 0},  {GAMEPAD::RIGHT, 0}, {GAMEPAD::LEFT, 0},
			{GAMEPAD::Y, 0},     {GAMEPAD::A, 0},     {GAMEPAD::B, 0},     {GAMEPAD::X, 0},
			{GAMEPAD::LEFT_SHOULDER, 0},   {GAMEPAD::LEFT_TRIGGER, 0},     {GAMEPAD::LEFT_STICK_CLICK, 0},  {GAMEPAD::RIGHT_SHOULDER, 0},
			{GAMEPAD::RIGHT_TRIGGER, 0},    {GAMEPAD::RIGHT_STICK_CLICK, 0}, {GAMEPAD::BACK, GAMEPAD::TOUCHPAD_LEFT}, {GAMEPAD::START, 0},
			{GAMEPAD::RIGHT_STICK_CLICK, 0},  // DOPPEL_SWITCH_BUTTON
			{GAMEPAD::BACK, GAMEPAD::TOUCHPAD_RIGHT}  // BACKWARDS_SWITCH
		};

		BindPair vergil1P[NUM_GAMEPADBINDS] = {
			{GAMEPAD::UP, 0},    {GAMEPAD::DOWN, 0},  {GAMEPAD::RIGHT, 0}, {GAMEPAD::LEFT, 0},
			{GAMEPAD::Y, 0},     {GAMEPAD::A, 0},     {GAMEPAD::B, 0},     {GAMEPAD::X, 0},
			{GAMEPAD::LEFT_SHOULDER, 0},   {GAMEPAD::LEFT_TRIGGER, 0},     {GAMEPAD::LEFT_STICK_CLICK, 0},  {GAMEPAD::RIGHT_SHOULDER, 0},
			{GAMEPAD::RIGHT_TRIGGER, 0},    {GAMEPAD::RIGHT_STICK_CLICK, 0}, {GAMEPAD::BACK, GAMEPAD::TOUCHPAD_LEFT}, {GAMEPAD::START, 0},
			{GAMEPAD::RIGHT_STICK_CLICK, 0},  // DOPPEL_SWITCH_BUTTON
			{GAMEPAD::BACK, GAMEPAD::TOUCHPAD_RIGHT}  // BACKWARDS_SWITCH
		};

		BindPair dante2P[NUM_GAMEPADBINDS] = {
			{GAMEPAD::UP, 0},    {GAMEPAD::DOWN, 0},  {GAMEPAD::RIGHT, 0}, {GAMEPAD::LEFT, 0},
			{GAMEPAD::Y, 0},     {GAMEPAD::A, 0},     {GAMEPAD::B, 0},     {GAMEPAD::X, 0},
			{GAMEPAD::LEFT_SHOULDER, 0},   {GAMEPAD::LEFT_TRIGGER, 0},     {GAMEPAD::LEFT_STICK_CLICK, 0},  {GAMEPAD::RIGHT_SHOULDER, 0},
			{GAMEPAD::RIGHT_TRIGGER, 0},    {GAMEPAD::RIGHT_STICK_CLICK, 0}, {GAMEPAD::BACK, GAMEPAD::TOUCHPAD_LEFT}, {GAMEPAD::START, 0},
			{GAMEPAD::RIGHT_STICK_CLICK, 0},  // DOPPEL_SWITCH_BUTTON
			{GAMEPAD::BACK, GAMEPAD::TOUCHPAD_RIGHT}  // BACKWARDS_SWITCH
		};

		BindPair vergil2P[NUM_GAMEPADBINDS] = {
			{GAMEPAD::UP, 0},    {GAMEPAD::DOWN, 0},  {GAMEPAD::RIGHT, 0}, {GAMEPAD::LEFT, 0},
			{GAMEPAD::Y, 0},     {GAMEPAD::A, 0},     {GAMEPAD::B, 0},     {GAMEPAD::X, 0},
			{GAMEPAD::LEFT_SHOULDER, 0},   {GAMEPAD::LEFT_TRIGGER, 0},     {GAMEPAD::LEFT_STICK_CLICK, 0},  {GAMEPAD::RIGHT_SHOULDER, 0},
			{GAMEPAD::RIGHT_TRIGGER, 0},    {GAMEPAD::RIGHT_STICK_CLICK, 0}, {GAMEPAD::BACK, GAMEPAD::TOUCHPAD_LEFT}, {GAMEPAD::START, 0},
			{GAMEPAD::RIGHT_STICK_CLICK, 0},  // DOPPEL_SWITCH_BUTTON
			{GAMEPAD::BACK, GAMEPAD::TOUCHPAD_RIGHT}  // BACKWARDS_SWITCH
		};

		BindPair dante3P[NUM_GAMEPADBINDS] = {
			{GAMEPAD::UP, 0},    {GAMEPAD::DOWN, 0},  {GAMEPAD::RIGHT, 0}, {GAMEPAD::LEFT, 0},
			{GAMEPAD::Y, 0},     {GAMEPAD::A, 0},     {GAMEPAD::B, 0},     {GAMEPAD::X, 0},
			{GAMEPAD::LEFT_SHOULDER, 0},   {GAMEPAD::LEFT_TRIGGER, 0},     {GAMEPAD::LEFT_STICK_CLICK, 0},  {GAMEPAD::RIGHT_SHOULDER, 0},
			{GAMEPAD::RIGHT_TRIGGER, 0},    {GAMEPAD::RIGHT_STICK_CLICK, 0}, {GAMEPAD::BACK, GAMEPAD::TOUCHPAD_LEFT}, {GAMEPAD::START, 0},
			{GAMEPAD::RIGHT_STICK_CLICK, 0},  // DOPPEL_SWITCH_BUTTON
			{GAMEPAD::BACK, GAMEPAD::TOUCHPAD_RIGHT}  // BACKWARDS_SWITCH
		};

		BindPair vergil3P[NUM_GAMEPADBINDS] = {
			{GAMEPAD::UP, 0},    {GAMEPAD::DOWN, 0},  {GAMEPAD::RIGHT, 0}, {GAMEPAD::LEFT, 0},
			{GAMEPAD::Y, 0},     {GAMEPAD::A, 0},     {GAMEPAD::B, 0},     {GAMEPAD::X, 0},
			{GAMEPAD::LEFT_SHOULDER, 0},   {GAMEPAD::LEFT_TRIGGER, 0},     {GAMEPAD::LEFT_STICK_CLICK, 0},  {GAMEPAD::RIGHT_SHOULDER, 0},
			{GAMEPAD::RIGHT_TRIGGER, 0},    {GAMEPAD::RIGHT_STICK_CLICK, 0}, {GAMEPAD::BACK, GAMEPAD::TOUCHPAD_LEFT}, {GAMEPAD::START, 0},
			{GAMEPAD::RIGHT_STICK_CLICK, 0},  // DOPPEL_SWITCH_BUTTON
			{GAMEPAD::BACK, GAMEPAD::TOUCHPAD_RIGHT}  // BACKWARDS_SWITCH
		};

		BindPair dante4P[NUM_GAMEPADBINDS] = {
			{GAMEPAD::UP, 0},    {GAMEPAD::DOWN, 0},  {GAMEPAD::RIGHT, 0}, {GAMEPAD::LEFT, 0},
			{GAMEPAD::Y, 0},     {GAMEPAD::A, 0},     {GAMEPAD::B, 0},     {GAMEPAD::X, 0},
			{GAMEPAD::LEFT_SHOULDER, 0},   {GAMEPAD::LEFT_TRIGGER, 0},     {GAMEPAD::LEFT_STICK_CLICK, 0},  {GAMEPAD::RIGHT_SHOULDER, 0},
			{GAMEPAD::RIGHT_TRIGGER, 0},    {GAMEPAD::RIGHT_STICK_CLICK, 0}, {GAMEPAD::BACK, GAMEPAD::TOUCHPAD_LEFT}, {GAMEPAD::START, 0},
			{GAMEPAD::RIGHT_STICK_CLICK, 0},  // DOPPEL_SWITCH_BUTTON
			{GAMEPAD::BACK, GAMEPAD::TOUCHPAD_RIGHT}  // BACKWARDS_SWITCH
		};

		BindPair vergil4P[NUM_GAMEPADBINDS] = {
			{GAMEPAD::UP, 0},    {GAMEPAD::DOWN, 0},  {GAMEPAD::RIGHT, 0}, {GAMEPAD::LEFT, 0},
			{GAMEPAD::Y, 0},     {GAMEPAD::A, 0},     {GAMEPAD::B, 0},     {GAMEPAD::X, 0},
			{GAMEPAD::LEFT_SHOULDER, 0},   {GAMEPAD::LEFT_TRIGGER, 0},     {GAMEPAD::LEFT_STICK_CLICK, 0},  {GAMEPAD::RIGHT_SHOULDER, 0},
			{GAMEPAD::RIGHT_TRIGGER, 0},    {GAMEPAD::RIGHT_STICK_CLICK, 0}, {GAMEPAD::BACK, GAMEPAD::TOUCHPAD_LEFT}, {GAMEPAD::START, 0},
			{GAMEPAD::RIGHT_STICK_CLICK, 0},  // DOPPEL_SWITCH_BUTTON
			{GAMEPAD::BACK, GAMEPAD::TOUCHPAD_RIGHT}  // BACKWARDS_SWITCH
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
			DI8::KEY::T, // keyboard_2(HDCdefault: 46) - LS / CHANGE TARGET - dmc3.exe + 5611A8
			DI8::KEY::ONE, // keyboard_3(HDCdefault: 2) - DPAD UP - dmc3.exe + 5611AC
			DI8::KEY::TWO, // keyboard_4(HDCdefault: 3) - DPAD RIGHT - dmc3.exe + 5611B0
			DI8::KEY::FOUR, // keyboard_5(HDCdefault: 5) - DPAD DOWN - dmc3.exe + 5611B4
			DI8::KEY::THREE, // keyboard_6(HDCdefault: 4) - DPAD LEFT - dmc3.exe + 5611B8
			DI8::KEY::M, // keyboard_7(HDCdefault: 50) - START - dmc3.exe + 5611BC
			DI8::KEY::SPACE, // keyboard_8(HDCdefault: 57) - RB / LOCK ON - dmc3.exe + 5611C0
			DI8::KEY::R, // keyboard_9(HDCdefault: 33) - RS / DEFAULT CAMERA - dmc3.exe + 5611C4
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

		uint32 directWeaponKeybinds[NUM_DIRECT_WEAPON_BINDS] = {
			DI8::KEY::F, // MELEE SLOT 1
			DI8::KEY::V, // MELEE SLOT 2
			DI8::KEY::C, // MELEE SLOT 3
			DI8::KEY::X, // MELEE SLOT 4
			DI8::KEY::Z, // MELEE SLOT 5
			DI8::KEY::G, // GUN SLOT 1
			DI8::KEY::H, // GUN SLOT 2
			DI8::KEY::Y, // GUN SLOT 3
			DI8::KEY::B, // GUN SLOT 4
			DI8::KEY::N, // GUN SLOT 5
		};

		static constexpr auto Metadata() {
			return std::make_tuple(
				std::make_pair("keybinds", &KeyboardConfig::keybinds),
				std::make_pair("directWeaponKeybinds", &KeyboardConfig::directWeaponKeybinds)
			);
		}

	} KeyboardConfig;

	uint8 xinputSlots[PLAYER_COUNT] = { 0, 1, 2, 3 };

	bool switchButtonCharSwitch[PLAYER_COUNT] = {};

	float vibrationIntensity[PLAYER_COUNT] = { 100.0f, 100.0f, 100.0f, 100.0f };

	static constexpr auto Metadata() {
		return std::make_tuple(
			std::make_pair("ButtonConfig", &CrimsonInput::ButtonConfig),
			std::make_pair("KeyboardConfig", &CrimsonInput::KeyboardConfig),
			std::make_pair("xinputSlots", &CrimsonInput::xinputSlots),
			std::make_pair("switchButtonCharSwitch", &CrimsonInput::switchButtonCharSwitch),
			std::make_pair("vibrationIntensity", &CrimsonInput::vibrationIntensity)
		);
	}
};

// Same pattern as CrimsonConfig: three config variables serving different purposes:
//
// -- defaultCrimsonInput will retain our default values for all input configurations.
// -- queuedCrimsonInput will retain config information as shown in the GUI.
// -- activeCrimsonInput will be our main and most current config variable to be read in functions.
extern CrimsonInput defaultCrimsonInput;
extern CrimsonInput queuedCrimsonInput;
extern CrimsonInput activeCrimsonInput;

#pragma pack(pop)
#pragma optimize("", on) // Re-enable optimizations

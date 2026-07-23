#pragma once

#include "CrimsonEfk.hpp"

namespace CrimsonEfkPreload {

void PreloadAll();

// Rising Star
extern const wchar_t* const risingStar_Sword_Path;
extern const wchar_t* const risingStar_PoseHit_Path;
extern EffekseerRefHandle risingStar_Sword_Handle;
extern EffekseerRefHandle risingStar_PoseHit_Handle;

// JDC Charge
extern const wchar_t* const jdcCharge_Path;
extern const wchar_t* const jdcCharge_DT_Path;
extern EffekseerRefHandle jdcCharge_Handle;
extern EffekseerRefHandle jdcCharge_DT_Handle;

// Just Frame JDC
extern const wchar_t* const justFrameJDC_Path;
extern EffekseerRefHandle justFrameJDC_Handle;

// Drive
extern const wchar_t* const drive_Charge_Path;
extern const wchar_t* const drive_QuickCharge_Path;
extern const wchar_t* const drive_Level1_Path;
extern const wchar_t* const drive_Level2_Path;
extern const wchar_t* const drive_Level3_Path;
extern EffekseerRefHandle drive_Charge_Handle;
extern EffekseerRefHandle drive_QuickCharge_Handle;
extern EffekseerRefHandle drive_Level1_Handle;
extern EffekseerRefHandle drive_Level2_Handle;
extern EffekseerRefHandle drive_Level3_Handle;

// Drive Collision
extern const wchar_t* const driveCol_Part1_Path;
extern const wchar_t* const driveCol_Part2_Path;
extern const wchar_t* const driveCol_Part3_Path;
extern EffekseerRefHandle driveCol_Part1_Handle;
extern EffekseerRefHandle driveCol_Part2_Handle;
extern EffekseerRefHandle driveCol_Part3_Handle;

// Ecstasy
extern const wchar_t* const ecstasy_Rose_Path;
extern EffekseerRefHandle ecstasy_Rose_Handle;
extern const wchar_t* const ecstasy_Hit_Path;
extern EffekseerRefHandle ecstasy_Hit_Handle;

// Delayed Combo
extern const wchar_t* const delayedCombo_Rebellion_Path;
extern const wchar_t* const delayedCombo_Cerberus_Path;
extern const wchar_t* const delayedCombo_AgniRudra1_Path;
extern const wchar_t* const delayedCombo_AgniRudra2_Path;
extern const wchar_t* const delayedCombo_BeowulfArms_Path;
extern const wchar_t* const delayedCombo_BeowulfLegs_Path;
extern EffekseerRefHandle delayedCombo_Rebellion_Handle;
extern EffekseerRefHandle delayedCombo_Cerberus_Handle;
extern EffekseerRefHandle delayedCombo_AgniRudra1_Handle;
extern EffekseerRefHandle delayedCombo_AgniRudra2_Handle;
extern EffekseerRefHandle delayedCombo_BeowulfArms_Handle;
extern EffekseerRefHandle delayedCombo_BeowulfLegs_Handle;

// StyleSwitch — Crimson
extern const wchar_t* const styleSwitchCrimson_Main_Path[7];
extern const wchar_t* const styleSwitchCrimson_Swoosh_Path[7];
extern EffekseerRefHandle styleSwitchCrimson_Main_Handle[7];
extern EffekseerRefHandle styleSwitchCrimson_Swoosh_Handle[7];

// StyleSwitch — Nintendo Switch
extern const wchar_t* const styleSwitchNS_Main_Path[7];
extern const wchar_t* const styleSwitchNS_Swoosh_Path[7];
extern EffekseerRefHandle styleSwitchNS_Main_Handle[7];
extern EffekseerRefHandle styleSwitchNS_Swoosh_Handle[7];

}

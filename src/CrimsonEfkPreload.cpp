// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "CrimsonEfkPreload.hpp"
#include "CrimsonEfk.hpp"
#include "../Core/Core.hpp"

namespace CrimsonEfkPreload {

// Rising Star
const wchar_t* const risingStar_Sword_Path  = L"Crimson\\vfx\\yamato_sword.efkefc";
const wchar_t* const risingStar_PoseHit_Path = L"Crimson\\vfx\\risingstar.efkefc";

// JDC Charge
const wchar_t* const jdcCharge_Path    = L"Crimson\\vfx\\jdc_charge.efkefc";
const wchar_t* const jdcCharge_DT_Path = L"Crimson\\vfx\\jdc_charge_dt.efkefc";

// Just Frame JDC
const wchar_t* const justFrameJDC_Path = L"Crimson\\vfx\\judgementcut\\justframejdc.efkefc";

// Drive
const wchar_t* const drive_Charge_Path      = L"Crimson\\vfx\\drive_charge.efkefc";
const wchar_t* const drive_QuickCharge_Path = L"Crimson\\vfx\\quickdrive_charge.efkefc";
const wchar_t* const drive_Level1_Path      = L"Crimson\\vfx\\drive_level1.efkefc";
const wchar_t* const drive_Level2_Path      = L"Crimson\\vfx\\drive_level2.efkefc";
const wchar_t* const drive_Level3_Path      = L"Crimson\\vfx\\drive_level3.efkefc";

// Drive Collision
const wchar_t* const driveCol_Part1_Path = L"Crimson\\vfx\\drive.efkefc";
const wchar_t* const driveCol_Part2_Path = L"Crimson\\vfx\\drive2.efkefc";
const wchar_t* const driveCol_Part3_Path = L"Crimson\\vfx\\drive3.efkefc";

// Ecstasy
const wchar_t* const ecstasy_Rose_Path = L"Crimson\\vfx\\ecstasy\\ecstasy_rose.efkefc";
const wchar_t* const ecstasy_Hit_Path = L"Crimson\\vfx\\ecstasy\\ecstasy_hit.efkefc";

// Delayed Combo
const wchar_t* const delayedCombo_Rebellion_Path    = L"Crimson\\vfx\\delayedcombo\\rebellion.efkefc";
const wchar_t* const delayedCombo_Cerberus_Path     = L"Crimson\\vfx\\delayedcombo\\cerberus.efkefc";
const wchar_t* const delayedCombo_AgniRudra1_Path   = L"Crimson\\vfx\\delayedcombo\\agni_rudra_1.efkefc";
const wchar_t* const delayedCombo_AgniRudra2_Path   = L"Crimson\\vfx\\delayedcombo\\agni_rudra_2.efkefc";
const wchar_t* const delayedCombo_BeowulfArms_Path  = L"Crimson\\vfx\\delayedcombo\\beowulf_arms.efkefc";
const wchar_t* const delayedCombo_BeowulfLegs_Path  = L"Crimson\\vfx\\delayedcombo\\beowulf_legs.efkefc";

// StyleSwitch — Crimson
const wchar_t* const styleSwitchCrimson_Main_Path[7] = {
    L"Crimson\\vfx\\styleswitch\\crimsonvers\\swordmaster.efkefc",
    L"Crimson\\vfx\\styleswitch\\crimsonvers\\gunslinger.efkefc",
    L"Crimson\\vfx\\styleswitch\\crimsonvers\\trickster.efkefc",
    L"Crimson\\vfx\\styleswitch\\crimsonvers\\royalguard.efkefc",
    L"Crimson\\vfx\\styleswitch\\crimsonvers\\quicksilver.efkefc",
    L"Crimson\\vfx\\styleswitch\\crimsonvers\\doppelganger.efkefc",
    L"Crimson\\vfx\\styleswitch\\crimsonvers\\darkslayer.efkefc",
};
const wchar_t* const styleSwitchCrimson_Swoosh_Path[7] = {
    L"Crimson\\vfx\\styleswitch\\crimsonvers\\swordmaster_swoosh.efkefc",
    L"Crimson\\vfx\\styleswitch\\crimsonvers\\gunslinger_swoosh.efkefc",
    L"Crimson\\vfx\\styleswitch\\crimsonvers\\trickster_swoosh.efkefc",
    L"Crimson\\vfx\\styleswitch\\crimsonvers\\royalguard_swoosh.efkefc",
    L"Crimson\\vfx\\styleswitch\\crimsonvers\\quicksilver_swoosh.efkefc",
    L"Crimson\\vfx\\styleswitch\\crimsonvers\\doppelganger_swoosh.efkefc",
    L"Crimson\\vfx\\styleswitch\\crimsonvers\\darkslayer_swoosh.efkefc",
};

// StyleSwitch — Nintendo Switch
const wchar_t* const styleSwitchNS_Main_Path[7] = {
    L"Crimson\\vfx\\styleswitch\\nswitchvers\\swordmaster.efkefc",
    L"Crimson\\vfx\\styleswitch\\nswitchvers\\gunslinger.efkefc",
    L"Crimson\\vfx\\styleswitch\\nswitchvers\\trickster.efkefc",
    L"Crimson\\vfx\\styleswitch\\nswitchvers\\royalguard.efkefc",
    L"Crimson\\vfx\\styleswitch\\nswitchvers\\quicksilver.efkefc",
    L"Crimson\\vfx\\styleswitch\\nswitchvers\\doppelganger.efkefc",
    L"Crimson\\vfx\\styleswitch\\nswitchvers\\darkslayer.efkefc",
};
const wchar_t* const styleSwitchNS_Swoosh_Path[7] = {
    L"Crimson\\vfx\\styleswitch\\nswitchvers\\swordmaster_swoosh.efkefc",
    L"Crimson\\vfx\\styleswitch\\nswitchvers\\gunslinger_swoosh.efkefc",
    L"Crimson\\vfx\\styleswitch\\nswitchvers\\trickster_swoosh.efkefc",
    L"Crimson\\vfx\\styleswitch\\nswitchvers\\royalguard_swoosh.efkefc",
    L"Crimson\\vfx\\styleswitch\\nswitchvers\\quicksilver_swoosh.efkefc",
    L"Crimson\\vfx\\styleswitch\\nswitchvers\\doppelganger_swoosh.efkefc",
    L"Crimson\\vfx\\styleswitch\\nswitchvers\\darkslayer_swoosh.efkefc",
};

// Rising Star
EffekseerRefHandle risingStar_Sword_Handle;
EffekseerRefHandle risingStar_PoseHit_Handle;

// JDC Charge
EffekseerRefHandle jdcCharge_Handle;
EffekseerRefHandle jdcCharge_DT_Handle;

// Just Frame JDC
EffekseerRefHandle justFrameJDC_Handle;

// Drive
EffekseerRefHandle drive_Charge_Handle;
EffekseerRefHandle drive_QuickCharge_Handle;
EffekseerRefHandle drive_Level1_Handle;
EffekseerRefHandle drive_Level2_Handle;
EffekseerRefHandle drive_Level3_Handle;

// Drive Collision
EffekseerRefHandle driveCol_Part1_Handle;
EffekseerRefHandle driveCol_Part2_Handle;
EffekseerRefHandle driveCol_Part3_Handle;

// Ecstasy
EffekseerRefHandle ecstasy_Rose_Handle;
EffekseerRefHandle ecstasy_Hit_Handle;

// Delayed Combo
EffekseerRefHandle delayedCombo_Rebellion_Handle;
EffekseerRefHandle delayedCombo_Cerberus_Handle;
EffekseerRefHandle delayedCombo_AgniRudra1_Handle;
EffekseerRefHandle delayedCombo_AgniRudra2_Handle;
EffekseerRefHandle delayedCombo_BeowulfArms_Handle;
EffekseerRefHandle delayedCombo_BeowulfLegs_Handle;

// StyleSwitch arrays
EffekseerRefHandle styleSwitchCrimson_Main_Handle[7];
EffekseerRefHandle styleSwitchCrimson_Swoosh_Handle[7];
EffekseerRefHandle styleSwitchNS_Main_Handle[7];
EffekseerRefHandle styleSwitchNS_Swoosh_Handle[7];

#define PRELOAD_LOG(handle, path, scale)                                          \
    do {                                                                          \
        handle = CrimsonEfk::LoadEffect(path, scale);                             \
        if (handle == -1) {                                                       \
            Log("PreloadAll: failed to load '%ls' (scale=%.1f) — skipping",       \
                path, scale);                                                     \
        }                                                                         \
    } while (0)

void PreloadAll() {
    LogFunction();

    // Rising Star
    PRELOAD_LOG(risingStar_Sword_Handle,  risingStar_Sword_Path,  1.0f);
    PRELOAD_LOG(risingStar_PoseHit_Handle, risingStar_PoseHit_Path, 40.0f);

    // JDC Charge
    PRELOAD_LOG(jdcCharge_Handle,    jdcCharge_Path,    1.0f);
    PRELOAD_LOG(jdcCharge_DT_Handle, jdcCharge_DT_Path, 1.0f);

    // Just Frame JDC
    PRELOAD_LOG(justFrameJDC_Handle, justFrameJDC_Path, 40.0f);

    // Drive
    PRELOAD_LOG(drive_Charge_Handle,      drive_Charge_Path,      1.0f);
    PRELOAD_LOG(drive_QuickCharge_Handle, drive_QuickCharge_Path, 1.0f);
    PRELOAD_LOG(drive_Level1_Handle,      drive_Level1_Path,      1.0f);
    PRELOAD_LOG(drive_Level2_Handle,      drive_Level2_Path,      1.0f);
    PRELOAD_LOG(drive_Level3_Handle,      drive_Level3_Path,      1.0f);

    // Drive Collision
    PRELOAD_LOG(driveCol_Part1_Handle, driveCol_Part1_Path, 40.0f);
    PRELOAD_LOG(driveCol_Part2_Handle, driveCol_Part2_Path, 40.0f);
    PRELOAD_LOG(driveCol_Part3_Handle, driveCol_Part3_Path, 40.0f);

    // Ecstasy
	PRELOAD_LOG(ecstasy_Rose_Handle, ecstasy_Rose_Path, 1.0f);
	PRELOAD_LOG(ecstasy_Hit_Handle, ecstasy_Hit_Path, 1.0f);

    // Delayed Combo
    PRELOAD_LOG(delayedCombo_Rebellion_Handle,    delayedCombo_Rebellion_Path,    1.0f);
    PRELOAD_LOG(delayedCombo_Cerberus_Handle,     delayedCombo_Cerberus_Path,     1.0f);
    PRELOAD_LOG(delayedCombo_AgniRudra1_Handle,   delayedCombo_AgniRudra1_Path,  1.0f);
    PRELOAD_LOG(delayedCombo_AgniRudra2_Handle,   delayedCombo_AgniRudra2_Path,  1.0f);
    PRELOAD_LOG(delayedCombo_BeowulfArms_Handle,  delayedCombo_BeowulfArms_Path, 1.0f);
    PRELOAD_LOG(delayedCombo_BeowulfLegs_Handle,  delayedCombo_BeowulfLegs_Path, 1.0f);

    // StyleSwitch — Crimson
    for (int i = 0; i < 7; ++i) {
        PRELOAD_LOG(styleSwitchCrimson_Main_Handle[i],   styleSwitchCrimson_Main_Path[i],   40.0f);
        PRELOAD_LOG(styleSwitchCrimson_Swoosh_Handle[i], styleSwitchCrimson_Swoosh_Path[i], 40.0f);
    }

    // StyleSwitch — Nintendo Switch
    for (int i = 0; i < 7; ++i) {
        PRELOAD_LOG(styleSwitchNS_Main_Handle[i],   styleSwitchNS_Main_Path[i],   40.0f);
        PRELOAD_LOG(styleSwitchNS_Swoosh_Handle[i], styleSwitchNS_Swoosh_Path[i], 40.0f);
    }
}

#undef PRELOAD_LOG

}

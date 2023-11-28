// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "HUD.hpp"
#include "Core/Core.hpp"
#include "File.hpp"
#include "Internal.hpp"
#include "Model.hpp"
#include "Global.hpp"
#include "Vars.hpp"

#include "Core/Macros.h"

#include "Core/DebugSwitch.hpp"

struct HUDIconHelper {
    byte8* modelFile;
    byte8* textureFile;
};

HUDIconHelper styleIcons[STYLE::MAX]      = {};
HUDIconHelper darkSlayerIcon              = {};
HUDIconHelper quicksilverVergilIcon       = {};
HUDIconHelper doppelgangerVergilIcon      = {};
HUDIconHelper weaponIcons[WEAPON::MAX]    = {};
HUDIconHelper devilTriggerGaugeDante      = {};
HUDIconHelper devilTriggerLightningDante  = {};
HUDIconHelper devilTriggerExplosionDante  = {};
HUDIconHelper devilTriggerGaugeVergil     = {};
HUDIconHelper devilTriggerLightningVergil = {};
HUDIconHelper devilTriggerExplosionVergil = {};

void InitIcons() {
    // Style Icons DAnte
    {
        constexpr uint8 fileIndices[STYLE::MAX] = {
            20,
            22,
            18,
            24,
            26,
            28,
        };
        old_for_all(uint8, style, countof(fileIndices)) {
            auto& item       = styleIcons[style];
            auto& fileIndex  = fileIndices[style];
            item.modelFile   = File_staticFiles[id100][(fileIndex + 1)];
            item.textureFile = File_staticFiles[id100][(fileIndex + 0)];
        }

        darkSlayerIcon.modelFile           = File_staticFiles[id100V][(18 + 1)];
        darkSlayerIcon.textureFile         = File_staticFiles[id100V][(18 + 0)];
        quicksilverVergilIcon.modelFile    = File_staticFiles[id100V][(26 + 1)];
        quicksilverVergilIcon.textureFile  = File_staticFiles[id100V][(26 + 0)];
        doppelgangerVergilIcon.modelFile   = File_staticFiles[id100V][(28 + 1)];
        doppelgangerVergilIcon.textureFile = File_staticFiles[id100V][(28 + 0)];

        devilTriggerGaugeDante.modelFile   = File_staticFiles[id100][(6)];
        devilTriggerGaugeDante.textureFile = File_staticFiles[id100][(0)];

        devilTriggerLightningDante.modelFile   = File_staticFiles[id100][(54)];
        devilTriggerLightningDante.textureFile = File_staticFiles[id100][(53)];

        devilTriggerExplosionDante.modelFile   = File_staticFiles[id100][(57)];
        devilTriggerExplosionDante.textureFile = File_staticFiles[id100][(0)];

        devilTriggerGaugeVergil.modelFile   = File_staticFiles[id100V][(6)];
        devilTriggerGaugeVergil.textureFile = File_staticFiles[id100V][(0)];

        devilTriggerLightningVergil.modelFile   = File_staticFiles[id100V][(54)];
        devilTriggerLightningVergil.textureFile = File_staticFiles[id100V][(53)];

        devilTriggerExplosionVergil.modelFile   = File_staticFiles[id100V][(57)];
        devilTriggerExplosionVergil.textureFile = File_staticFiles[id100V][(0)];
    }

    // Weapon Icons Dante
    {
        constexpr uint8 fileIndices[] = {
            42,
            44,
            46,
            48,
            50,
            30,
            32,
            34,
            36,
            38,
        };
        old_for_all(uint8, weapon, countof(fileIndices)) {
            auto& item       = weaponIcons[(WEAPON::REBELLION + weapon)];
            auto& fileIndex  = fileIndices[weapon];
            item.modelFile   = File_staticFiles[id100][(fileIndex + 1)];
            item.textureFile = File_staticFiles[id100][(fileIndex + 0)];
        }
    }

    // Weapon Icons Vergil
    {
        constexpr uint8 fileIndices[] = {
            42,
            44,
            30,
        };
        old_for_all(uint8, weapon, countof(fileIndices)) {
            auto& item       = weaponIcons[(WEAPON::YAMATO_VERGIL + weapon)];
            auto& fileIndex  = fileIndices[weapon];
            item.modelFile   = File_staticFiles[id100V][(fileIndex + 1)];
            item.textureFile = File_staticFiles[id100V][(fileIndex + 0)];
        }
    }
}

void HUD_UpdateStyleIcon(uint8 style, uint8 character) {
    if ((InCutscene()) || (InCredits()) || (style >= STYLE::MAX)) {
        return;
    }

    auto name_143 = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E28);
    if (!name_143) {
        return;
    }
    name_143 -= 0x180;
    auto hudTop    = *reinterpret_cast<byte8**>(name_143 + 0x1B070);
    auto hudBottom = *reinterpret_cast<byte8**>(name_143 + 0x1B078);


    if constexpr (debug) {
        LogFunction();
    }


    auto modelFile   = styleIcons[style].modelFile;
    auto textureFile = styleIcons[style].textureFile;

    if (((character == CHARACTER::BOB) || (character == CHARACTER::VERGIL))) {

        if (style == STYLE::DARK_SLAYER) {
            modelFile   = darkSlayerIcon.modelFile;
            textureFile = darkSlayerIcon.textureFile;
        } else if (style == STYLE::QUICKSILVER) {
            modelFile   = quicksilverVergilIcon.modelFile;
            textureFile = quicksilverVergilIcon.textureFile;
        } else if (style == STYLE::DOPPELGANGER) {
            modelFile   = doppelgangerVergilIcon.modelFile;
            textureFile = doppelgangerVergilIcon.textureFile;
        }

    } else {
        styleIcons[style].modelFile;
        styleIcons[style].textureFile;
    }

    auto& modelData = *reinterpret_cast<ModelData*>(hudTop + hudTopOffs[HUD_TOP::STYLE_ICON]);

    ResetModel(modelData);

    func_89960(modelData, modelFile, textureFile);
    func_89E30(modelData, 1);

    auto map     = reinterpret_cast<uint8*>(appBaseAddr + 0x4E9070);
    auto& effect = *reinterpret_cast<uint8*>(hudTop + 0x690E) = map[style];
}

void HUD_UpdateDevilTriggerGauge(uint8 character) {
    if ((InCutscene()) || (InCredits())) {
        return;
    }

    auto name_202 = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E28);
    if (!name_202) {
        return;
    }
    name_202 -= 0x180;
    auto hudTop    = *reinterpret_cast<byte8**>(name_202 + 0x1B070);
    auto hudBottom = *reinterpret_cast<byte8**>(name_202 + 0x1B078);


    if constexpr (debug) {
        LogFunction();
    }


    auto modelFile   = devilTriggerGaugeDante.modelFile;
    auto textureFile = devilTriggerGaugeDante.textureFile;

    if (((character == CHARACTER::BOB) || (character == CHARACTER::VERGIL))) {


        modelFile   = devilTriggerGaugeVergil.modelFile;
        textureFile = devilTriggerGaugeVergil.textureFile;


    } else {
        devilTriggerGaugeDante.modelFile;
        devilTriggerGaugeDante.textureFile;
    }

    auto& modelData = *reinterpret_cast<ModelData*>(hudTop + hudTopOffs[HUD_TOP::MAGIC_ORBS]);

    ResetModel(modelData);

    func_89960(modelData, modelFile, textureFile);
    func_89E30(modelData, 1);

    auto map = reinterpret_cast<uint8*>(appBaseAddr + 0x4E9070);
    // auto & effect = *reinterpret_cast<uint8 *>(hudTop + 0x690E) = map[style];
}

void HUD_UpdateDevilTriggerLightning(uint8 character) {
    if ((InCutscene()) || (InCredits())) {
        return;
    }

    auto name_254 = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E28);
    if (!name_254) {
        return;
    }
    name_254 -= 0x180;
    auto hudTop    = *reinterpret_cast<byte8**>(name_254 + 0x1B070);
    auto hudBottom = *reinterpret_cast<byte8**>(name_254 + 0x1B078);


    if constexpr (debug) {
        LogFunction();
    }


    auto modelFile   = devilTriggerLightningDante.modelFile;
    auto textureFile = devilTriggerLightningDante.textureFile;

    if (((character == CHARACTER::BOB) || (character == CHARACTER::VERGIL))) {


        modelFile   = devilTriggerLightningVergil.modelFile;
        textureFile = devilTriggerLightningVergil.textureFile;


    } else {
        devilTriggerLightningDante.modelFile;
        devilTriggerLightningDante.textureFile;
    }

    auto& modelData = *reinterpret_cast<ModelData*>(hudTop + hudTopOffs[HUD_TOP::FLUX]);

    ResetModel(modelData);

    func_89960(modelData, modelFile, textureFile);
    func_89E30(modelData, 1);

    auto map = reinterpret_cast<uint8*>(appBaseAddr + 0x4E9070);
    // auto & effect = *reinterpret_cast<uint8 *>(hudTop + 0x690E) = map[style];
}

void HUD_UpdateDevilTriggerExplosion(uint8 character) {
    if ((InCutscene()) || (InCredits())) {
        return;
    }

    auto name_306 = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E28);
    if (!name_306) {
        return;
    }
    name_306 -= 0x180;
    auto hudTop    = *reinterpret_cast<byte8**>(name_306 + 0x1B070);
    auto hudBottom = *reinterpret_cast<byte8**>(name_306 + 0x1B078);


    if constexpr (debug) {
        LogFunction();
    }


    auto modelFile   = devilTriggerExplosionDante.modelFile;
    auto textureFile = devilTriggerExplosionDante.textureFile;

    if (((character == CHARACTER::BOB) || (character == CHARACTER::VERGIL))) {


        modelFile   = devilTriggerExplosionVergil.modelFile;
        textureFile = devilTriggerExplosionVergil.textureFile;


    } else {
        devilTriggerExplosionDante.modelFile;
        devilTriggerExplosionDante.textureFile;
    }

    auto& modelData = *reinterpret_cast<ModelData*>(hudTop + hudTopOffs[HUD_TOP::DTEXPLOSION]);

    ResetModel(modelData);

    func_89960(modelData, modelFile, textureFile);
    func_89E30(modelData, 1);

    auto map = reinterpret_cast<uint8*>(appBaseAddr + 0x4E9070);
    // auto & effect = *reinterpret_cast<uint8 *>(hudTop + 0x690E) = map[style];
}

bool HUD_UpdateWeaponIcon(uint8 index, uint8 weapon) {
    if ((InCutscene()) || (InCredits()) || (weapon >= WEAPON::MAX)) {
        return false;
    }

    auto name_360 = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E28);
    if (!name_360) {
        return false;
    }
    name_360 -= 0x180;
    auto hudTop    = *reinterpret_cast<byte8**>(name_360 + 0x1B070);
    auto hudBottom = *reinterpret_cast<byte8**>(name_360 + 0x1B078);


    if constexpr (debug) {
        LogFunction();
    }

    auto& modelData = *reinterpret_cast<ModelData*>(hudBottom + hudBottomOffs[index]);

    ResetModel(modelData);

    auto modelFile   = weaponIcons[weapon].modelFile;
    auto textureFile = weaponIcons[weapon].textureFile;

    func_89960(modelData, modelFile, textureFile);
    func_89E30(modelData, 1);

    return true;
}

void HUD_Init() {
    LogFunction();

    InitIcons();
}


void ToggleHideMainHUD(bool enable) {
    LogFunction(enable);

    static bool run = false;


    // Top
    {
        auto addr             = (appBaseAddr + 0x27E59C);
        constexpr uint32 size = 2;
        /*
        dmc3.exe+27E59C - 75 0F    - jne dmc3.exe+27E5AD
        dmc3.exe+27E59E - 48 8B D7 - mov rdx,rdi
        */

        if (!run) {
            backupHelper.Save(addr, size);
        }

        if (enable) {
            Write<byte8>(addr, 0xEB);
        } else {
            backupHelper.Restore(addr);
        }
    }

    // Bottom
    {
        auto addr             = (appBaseAddr + 0x2810F8);
        constexpr uint32 size = 2;
        /*
        dmc3.exe+2810F8 - 75 0F    - jne dmc3.exe+281109
        dmc3.exe+2810FA - 49 8B D6 - mov rdx,r14
        */

        if (!run) {
            backupHelper.Save(addr, size);
        }

        if (enable) {
            Write<byte8>(addr, 0xEB);
        } else {
            backupHelper.Restore(addr);
        }
    }

    // Style Rank
    {
        auto addr             = (appBaseAddr + 0x2BB194);
        constexpr uint32 size = 6;
        /*
        dmc3.exe+2BB194 - 0F85 18020000     - jne dmc3.exe+2BB3B2
        dmc3.exe+2BB19A - 83 B9 203D0000 00 - cmp dword ptr [rcx+00003D20],00
        */

        if (!run) {
            backupHelper.Save(addr, size);
        }

        if (enable) {
            Write<byte16>(addr, 0xE990);
        } else {
            backupHelper.Restore(addr);
        }
    }


    run = true;
}

void ToggleHideLockOn(bool enable) {
    LogFunction(enable);

    static bool run = false;


    {
        auto addr             = (appBaseAddr + 0x296E77);
        constexpr uint32 size = 2;
        /*
        dmc3.exe+296E77 - 75 14    - jne dmc3.exe+296E8D
        dmc3.exe+296E79 - 48 8B D3 - mov rdx,rbx
        */

        if (!run) {
            backupHelper.Save(addr, size);
        }

        if (enable) {
            Write<byte8>(addr, 0xEB);
        } else {
            backupHelper.Restore(addr);
        }
    }


    run = true;
}

void ToggleHideBossHUD(bool enable) {
    LogFunction(enable);

    static bool run = false;


    {
        auto addr             = (appBaseAddr + 0x27FF69);
        constexpr uint32 size = 2;
        /*
        dmc3.exe+27FF69 - 75 0F    - jne dmc3.exe+27FF7A
        dmc3.exe+27FF6B - 48 8B D7 - mov rdx,rdi
        */

        if (!run) {
            backupHelper.Save(addr, size);
        }

        if (enable) {
            Write<byte8>(addr, 0xEB);
        } else {
            backupHelper.Restore(addr);
        }
    }


    run = true;
}

void ToggleForceVisibleHUD(bool enable) {
    static bool run = false;


    {
        auto addr             = (appBaseAddr + 0x27E800);
        constexpr uint32 size = 2;
        /*
        dmc3.exe+27E800 - 74 63          - je dmc3.exe+27E865
        dmc3.exe+27E802 - 8B 86 28690000 - mov eax,[rsi+00006928]
        */

        if (!run) {
            backupHelper.Save(addr, size);
        }

        if (enable) {
            Write<byte8>(addr, 0xEB);
        } else {
            backupHelper.Restore(addr);
        }
    }

    {
        auto addr             = (appBaseAddr + 0x27DF3E);
        constexpr uint32 size = 2;
        /*
        dmc3.exe+27DF3E - 75 59               - jne dmc3.exe+27DF99
        dmc3.exe+27DF40 - F3 0F10 8F 18690000 - movss xmm1,[rdi+00006918]
        */

        if (!run) {
            backupHelper.Save(addr, size);
        }

        if (enable) {
            Write<byte8>(addr, 0xEB);
        } else {
            backupHelper.Restore(addr);
        }
    }

    {
        auto addr             = (appBaseAddr + 0x280DB9);
        constexpr uint32 size = 6;
        /*
        dmc3.exe+280DB9 - 0F86 3F020000 - jbe dmc3.exe+280FFE
        dmc3.exe+280DBF - 41 FE 07      - inc byte ptr [r15]
        */

        if (!run) {
            backupHelper.Save(addr, size);
        }

        if (enable) {
            Write<byte16>(addr, 0xE990);
        } else {
            backupHelper.Restore(addr);
        }
    }


    run = true;
}

#ifdef __GARBAGE__
#endif

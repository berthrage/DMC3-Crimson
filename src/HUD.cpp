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
#include "Config.hpp"
#include "Actor.hpp"
#include "CrimsonFileHandling.hpp"

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
HUDIconHelper hpBarDante   = {};
HUDIconHelper hpFrameDante = {};
HUDIconHelper hpBarVergil   = {};
HUDIconHelper hpFrameVergil = {};
HUDIconHelper hpBackgroundDante   = {};
HUDIconHelper hpBackgroundVergil  = {};

HUDIconHelper lockOnIconDante  = {};
HUDIconHelper lockOnIconVergil = {};

byte8* g_dynamicHUD_id100  = nullptr;
byte8* g_dynamicHUD_id100V = nullptr;

void InitIcons(const std::string& selectedHUD) {
    // Style Icons DAnte

	// Select the right archive source based on the HUD selection
	byte8* id100Arch  = nullptr;
	byte8* id100VArch = nullptr;

	if (selectedHUD == "Crimson HUD") {
		// Load dynamically if not already loaded; moved out of File_Init static loading
		if (!newCrimsonHUD1_0_id_100) {
			std::string hudPath = std::string(Paths::gameMods) + "\\newCrimsonHUD1_0_id_100.pac";
			if (!CrimsonFiles::LoadSinglePAC(hudPath, newCrimsonHUD1_0_id_100)) {
				Log("InitIcons: Failed to load newCrimsonHUD1_0_id_100.pac dynamically, falling back to originals");
			}
		}
		if (!newCrimsonHUD1_0_id_100V) {
			std::string hudPath = std::string(Paths::gameMods) + "\\newCrimsonHUD1_0_id_100V.pac";
			if (!CrimsonFiles::LoadSinglePAC(hudPath, newCrimsonHUD1_0_id_100V)) {
				Log("InitIcons: Failed to load newCrimsonHUD1_0_id_100V.pac dynamically, falling back to originals");
			}
		}
		id100Arch  = newCrimsonHUD1_0_id_100 ? newCrimsonHUD1_0_id_100 : (byte8*)File_staticFiles[id100];
		id100VArch = newCrimsonHUD1_0_id_100V ? newCrimsonHUD1_0_id_100V : (byte8*)File_staticFiles[id100V];
	} else if (selectedHUD == "Classic HUD") {
		id100Arch  = (byte8*)File_staticFiles[id100];
		id100VArch = (byte8*)File_staticFiles[id100V];
	} else if (g_dynamicHUD_id100 != nullptr) {
		id100Arch  = g_dynamicHUD_id100;
		id100VArch = g_dynamicHUD_id100V;
	} else {
		// Fallback — should not be reached if ApplySelectedHUD was called
		id100Arch  = (byte8*)File_staticFiles[id100];
		id100VArch = (byte8*)File_staticFiles[id100V];
	}

	// Helper to get subfile pointer from a PAC archive
	auto SubfilePtr = [](byte8* archive, uint32 fileIndex) -> byte8* {
		if (!archive || !IsArchive(archive)) return nullptr;
		auto& archMeta = *reinterpret_cast<ArchiveMetadata*>(archive);
		if (fileIndex >= archMeta.fileCount) return nullptr;
		return archive + archMeta.fileOffs[fileIndex];
	};

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
            item.modelFile   = SubfilePtr(id100Arch, (fileIndex + 1));
            item.textureFile = SubfilePtr(id100Arch, (fileIndex + 0));
        }

        darkSlayerIcon.modelFile           = SubfilePtr(id100VArch, (18 + 1));
        darkSlayerIcon.textureFile         = SubfilePtr(id100VArch, (18 + 0));
        quicksilverVergilIcon.modelFile    = SubfilePtr(id100VArch, (26 + 1));
        quicksilverVergilIcon.textureFile  = SubfilePtr(id100VArch, (26 + 0));
        doppelgangerVergilIcon.modelFile   = SubfilePtr(id100VArch, (28 + 1));
        doppelgangerVergilIcon.textureFile = SubfilePtr(id100VArch, (28 + 0));

        devilTriggerGaugeDante.modelFile   = SubfilePtr(id100Arch, (6));
        devilTriggerGaugeDante.textureFile = SubfilePtr(id100Arch, (0));

        devilTriggerLightningDante.modelFile   = SubfilePtr(id100Arch, (54));
        devilTriggerLightningDante.textureFile = SubfilePtr(id100Arch, (53));

        devilTriggerExplosionDante.modelFile   = SubfilePtr(id100Arch, (57));
        devilTriggerExplosionDante.textureFile = SubfilePtr(id100Arch, (0));

        devilTriggerGaugeVergil.modelFile   = SubfilePtr(id100VArch, (6));
        devilTriggerGaugeVergil.textureFile = SubfilePtr(id100VArch, (0));

        devilTriggerLightningVergil.modelFile   = SubfilePtr(id100VArch, (54));
        devilTriggerLightningVergil.textureFile = SubfilePtr(id100VArch, (53));

        devilTriggerExplosionVergil.modelFile   = SubfilePtr(id100VArch, (57));
        devilTriggerExplosionVergil.textureFile = SubfilePtr(id100VArch, (0));

        // HP Bar — model: 4 = frame, 5 = fill bar; texture: 0 for both
        hpBarDante.modelFile   = SubfilePtr(id100Arch, 5);
        hpBarDante.textureFile = SubfilePtr(id100Arch, 0);
        hpFrameDante.modelFile   = SubfilePtr(id100Arch, 4);
        hpFrameDante.textureFile = SubfilePtr(id100Arch, 0);

        hpBarVergil.modelFile   = SubfilePtr(id100VArch, 5);
        hpBarVergil.textureFile = SubfilePtr(id100VArch, 0);
        hpFrameVergil.modelFile   = SubfilePtr(id100VArch, 4);
        hpFrameVergil.textureFile = SubfilePtr(id100VArch, 0);

        hpBackgroundDante.modelFile   = SubfilePtr(id100Arch, 5);
        hpBackgroundDante.textureFile = SubfilePtr(id100Arch, 0);
        hpBackgroundVergil.modelFile   = SubfilePtr(id100VArch, 5);
        hpBackgroundVergil.textureFile = SubfilePtr(id100VArch, 0);

        // Lock-On icon — texture: 7, model: 8
        lockOnIconDante.modelFile   = SubfilePtr(id100Arch, 8);
        lockOnIconDante.textureFile = SubfilePtr(id100Arch, 7);
        lockOnIconVergil.modelFile   = SubfilePtr(id100VArch, 8);
        lockOnIconVergil.textureFile = SubfilePtr(id100VArch, 7);
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
            item.modelFile   = SubfilePtr(id100Arch, (fileIndex + 1));
            item.textureFile = SubfilePtr(id100Arch, (fileIndex + 0));
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
            item.modelFile   = SubfilePtr(id100VArch, (fileIndex + 1));
            item.textureFile = SubfilePtr(id100VArch, (fileIndex + 0));
        }
    }
}

uint32 g_hudIconSerial = 0;

void HUD_UpdateStyleIcon(uint8 style, uint8 character) {
    g_hudIconSerial++;
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

    // 0x1AEF0 (0x1B070 - 0x180) -- CUIDCockpit00
	// 0x1AEF8 (0x1B078 - 0x180) -- CUIDCockpit02
	// 0x1AF00 (0x1B080 - 0x180) -- CUIDLockOnCuror
	// 0x1AF08 (0x1B088 - 0x180) -- CUIDPause
	// 0x1AF10 (0x1B090 - 0x180) -- CUIDStylishCombo

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
    g_hudIconSerial++;
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
    g_hudIconSerial++;
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
    g_hudIconSerial++;
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
    g_hudIconSerial++;
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

void HUD_UpdateHPBar(uint8 character) {
    g_hudIconSerial++;
    if ((InCutscene()) || (InCredits())) {
        return;
    }

    auto name = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E28);
    if (!name) {
        return;
    }
    name -= 0x180;
    auto hudTop = *reinterpret_cast<byte8**>(name + 0x1B070);

    if constexpr (debug) {
        LogFunction();
    }

    // Choose Dante or Vergil helpers
    bool isVergil = (character == CHARACTER::BOB) || (character == CHARACTER::VERGIL);
    auto& barFile   = isVergil ? hpBarVergil   : hpBarDante;
    auto& frameFile = isVergil ? hpFrameVergil : hpFrameDante;
    auto& bgFile    = isVergil ? hpBackgroundVergil : hpBackgroundDante;

    // Helper to apply a model+texture to a HUD element at a given HUD_TOP offset
    auto UpdateElement = [hudTop](uint32 elementOff, HUDIconHelper& files) {
        auto& modelData = *reinterpret_cast<ModelData*>(hudTop + elementOff);
        ResetModel(modelData);
        func_89960(modelData, files.modelFile, files.textureFile);
        func_89E30(modelData, 1);
    };

    // Frame uses index 4 (same file for both upper/lower)
    UpdateElement(hudTopOffs[HUD_TOP::UPPER_HIT_POINTS_FRAME], frameFile);
    UpdateElement(hudTopOffs[HUD_TOP::LOWER_HIT_POINTS_FRAME], frameFile);

    // Bar (fill) uses index 5 (same file for both upper/lower)
    UpdateElement(hudTopOffs[HUD_TOP::UPPER_HIT_POINTS_BAR], barFile);
    UpdateElement(hudTopOffs[HUD_TOP::LOWER_HIT_POINTS_BAR], barFile);

    // Background uses index 5 (same as bar fill)
    UpdateElement(hudTopOffs[HUD_TOP::UPPER_HIT_POINTS_BACKGROUND], bgFile);
    UpdateElement(hudTopOffs[HUD_TOP::LOWER_HIT_POINTS_BACKGROUND], bgFile);
}

void HUD_UpdateLockOn(uint8 character) {
    g_hudIconSerial++;
    if ((InCutscene()) || (InCredits())) {
        return;
    }

	auto name_143 = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E28);
	if (!name_143) {
		return;
	}
	name_143 -= 0x180;
	auto lockOnCursor = *reinterpret_cast<byte8**>(name_143 + 0x1B080);

	// 0x1AEF0 (0x1B070 - 0x180) -- CUIDCockpit00
	// 0x1AEF8 (0x1B078 - 0x180) -- CUIDCockpit02
	// 0x1AF00 (0x1B080 - 0x180) -- CUIDLockOnCuror
	// 0x1AF08 (0x1B088 - 0x180) -- CUIDPause
	// 0x1AF10 (0x1B090 - 0x180) -- CUIDStylishCombo

    if constexpr (debug) {
        LogFunction();
    }

    auto modelFile   = lockOnIconDante.modelFile;
    auto textureFile = lockOnIconDante.textureFile;

    if (((character == CHARACTER::BOB) || (character == CHARACTER::VERGIL))) {
        modelFile   = lockOnIconVergil.modelFile;
        textureFile = lockOnIconVergil.textureFile;
    }

    auto& modelData = *reinterpret_cast<ModelData*>(lockOnCursor + 0x80);

    ResetModel(modelData);

    func_89960(modelData, modelFile, textureFile);
    func_89E30(modelData, 1);
}

void HUD_Init() {
    LogFunction();

    InitIcons(activeCrimsonConfig.HudOptions.selectedHUD);
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
//     {
//         auto addr             = (appBaseAddr + 0x2BB194);
//         constexpr uint32 size = 6;
//         /*
//         dmc3.exe+2BB194 - 0F85 18020000     - jne dmc3.exe+2BB3B2
//         dmc3.exe+2BB19A - 83 B9 203D0000 00 - cmp dword ptr [rcx+00003D20],00
//         */
// 		//dmc3.exe + 2BB194 - 0F 85 18 02 00 00 - jne dmc3.exe + 2BB3B2
// 		// goes to dmc3.exe+2BB195 - E9 18 02 00 00 - jmp dmc3.exe+2BB3B2
// 
// 
//         if (!run) {
//             backupHelper.Save(addr, size);
//         }
// 
//         if (enable) {
//             Write<byte16>(addr, 0xE990);
//         } else {
//             backupHelper.Restore(addr);
//         }
//     }


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

void HUD_ReloadIcons() {
    LogFunction();

    // Re-read file pointers from the game's static file cache.
    // Uses Crimson HUD 1.0 dedicated archives, dynamic custom HUD, or originals.
    InitIcons(activeCrimsonConfig.HudOptions.selectedHUD);

	// Fetch Player 1's currently active character.
	// CCS route: GetNewActorData — direct lookup via the Crimson config system.
	// Vanilla route: GetVanillaPlayerActor — mirrors GetNewActorData for vanilla actors.
	PlayerActorData* actorDataPtr = nullptr;
	auto& playerData = GetPlayerData(0);

	if (activeConfig.Actor.enable) {
		auto& activeNewActorData = GetNewActorData(0, playerData.activeCharacterIndex, ENTITY::MAIN);
		if (activeNewActorData.baseAddr) {
			actorDataPtr = reinterpret_cast<PlayerActorData*>(activeNewActorData.baseAddr);
		}
	} else {
		actorDataPtr = GetVanillaPlayerActor();
	}

    static bool actorNotFoundLogged = false;
	if (!actorDataPtr) {
		if (!actorNotFoundLogged) {
			Log("Player actor not found, cannot reload HUD");
			actorNotFoundLogged = true;
		}
		return;
	}
	actorNotFoundLogged = false;
	auto& actorData = *actorDataPtr;
    uint8 character = actorData.character;

    // Re-apply style icons for all styles
    HUD_UpdateStyleIcon(actorData.style, character);

    // Re-apply HP bar frame and fill
    HUD_UpdateHPBar(character);

    // Re-apply DT gauge, lightning, and explosion
    HUD_UpdateDevilTriggerGauge(character);
    HUD_UpdateDevilTriggerLightning(character);
    HUD_UpdateDevilTriggerExplosion(character);

    // Re-apply lock-on icon
    HUD_UpdateLockOn(character);

    // Re-apply weapon icons — the game uses a direct index + weapon pair.
    // WEAPON::REBELLION + 0..9 covers both Dante melee & ranged indices.
//     for (uint8 i = 0; i < 10; ++i) {
//         uint8 weapon = WEAPON::REBELLION + i;
//         if (weapon < WEAPON::MAX) {
//             HUD_UpdateWeaponIcon(i, weapon);
//         }
//     }
}

#ifdef __GARBAGE__
#endif

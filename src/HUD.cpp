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
#include "CrimsonSplitscreen.hpp"
#include "Utility/Detour.hpp"
#include "ImGui/imgui.h"

#include <cmath>
#include <cstdio>
#include <cstring>
#include <intrin.h>
#include <iterator>
#include <memory>

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
    HUD_NoteReload("style icon");
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
    HUD_NoteReload("DT gauge");
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
    HUD_NoteReload("DT lightning");
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
    HUD_NoteReload("DT explosion");
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
    HUD_NoteReload("weapon icon");
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
    HUD_NoteReload("HP bar");
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

namespace {

// CDrawModel_t layout (0x780): loaded flag +0x18, heap handle +0x30 (CKernelHeapHandle_t: arena +8, data +0x10,
// first block +0x18, block count +0x1C), internal +0x80 with mesh count +0xE8, flags +0xE0 (builder type in the top
// nibble), block size +0xF0, mesh array +0x100 (0x380 each, per frame command slots at mesh +0x40) and block start
// +0x188. The heap arenas are g_HeapArena0..2 at dmc3.exe+CA8910 (0x28 each, byte map at arena +0).
constexpr size_t DRAW_HEAP_HANDLE = 0x30;
constexpr size_t DRAW_INTERNAL    = 0x80;
constexpr size_t MESH_STRIDE      = 0x380;
constexpr size_t LOCKON_RETICLE   = 0x80;
constexpr size_t LOCKON_MARKERS   = 0x8C0;
constexpr size_t COCKPIT_BOTTOM_ICONS = 0x880;
constexpr size_t COCKPIT_BOTTOM_ICON_COUNT = 4;
constexpr size_t COCKPIT_BOTTOM_FRAME = 0x100;
constexpr size_t COCKPIT_TOP_MODEL_COUNT = 13;

struct HudModel {
	byte8* model;
	const char* name;
};

constexpr const char* TOP_MODEL_NAMES[COCKPIT_TOP_MODEL_COUNT] = {"UpperHPFrame", "UpperHPBar", "UpperHPRed", "LowerHPFrame", "LowerHPBar",
	"LowerHPRed", "DTOrbs", "StyleIcons", "RedOrbCounter", "Unknown1", "Unknown2", "DTFlux", "DTExplosion"};
constexpr const char* BOTTOM_ICON_NAMES[COCKPIT_BOTTOM_ICON_COUNT] = {"Ranged1", "Ranged2", "Melee1", "Melee2"};

byte8* HudObject(size_t slotOffset) {
	auto name = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E28);
	if (!name) {
		return nullptr;
	}
	return *reinterpret_cast<byte8**>(name - 0x180 + slotOffset);
}

// Every CDraw model of the original HUD (cockpit top / bottom, lock-on cursor) that exists right now.
size_t CollectHudModels(HudModel (&out)[32]) {
	size_t n = 0;
	if (auto top = HudObject(0x1B070)) {
		for (size_t i = 0; i < COCKPIT_TOP_MODEL_COUNT; i++) {
			out[n++] = {top + hudTopOffs[i], TOP_MODEL_NAMES[i]};
		}
	}
	if (auto bottom = HudObject(0x1B078)) {
		out[n++] = {bottom + COCKPIT_BOTTOM_FRAME, "BottomFrame"};
		for (size_t i = 0; i < COCKPIT_BOTTOM_ICON_COUNT; i++) {
			out[n++] = {bottom + hudBottomOffs[i], BOTTOM_ICON_NAMES[i]};
		}
	}
	if (auto cursor = HudObject(0x1B080)) {
		out[n++] = {cursor + LOCKON_RETICLE, "LockOnReticle"};
		out[n++] = {cursor + LOCKON_MARKERS, "LockOnMarkers"};
	}
	return n;
}

struct HeapRun {
	byte8* begin;
	byte8* end;
};

// Returns what is wrong with a loaded model's heap block (arena map, builder command slots), or nullptr. run = the
// block range when the handle is sane.
const char* ModelBlockProblem(byte8* model, char* detail, size_t detailSize, HeapRun& run) {
	run = {};
	byte8* handle   = model + DRAW_HEAP_HANDLE;
	byte8* internal = model + DRAW_INTERNAL;

	auto arena = *reinterpret_cast<byte8**>(handle + 8);
	if (!arena || !*(model + 0x18)) {
		return nullptr;
	}

	int arenaIndex = -1;
	for (int i = 0; i < 3; i++) {
		if (arena == appBaseAddr + 0xCA8910 + i * 0x28) {
			arenaIndex = i;
		}
	}
	if (arenaIndex < 0) {
		snprintf(detail, detailSize, "handle arena %p is not a heap arena", arena);
		return "arena";
	}

	auto data       = *reinterpret_cast<byte8**>(handle + 0x10);
	auto first      = *reinterpret_cast<int32*>(handle + 0x18);
	auto count      = *reinterpret_cast<int32*>(handle + 0x1C);
	auto map        = *reinterpret_cast<byte8**>(arena);
	auto arenaData  = *reinterpret_cast<byte8**>(arena + 8);
	auto arenaCount = *reinterpret_cast<int32*>(arena + 0x10);
	auto blockSize  = *reinterpret_cast<int32*>(arena + 0x14);
	if (first < 0 || count <= 0 || first + count > arenaCount) {
		snprintf(detail, detailSize, "arena %d handle first %d count %d outside %d blocks", arenaIndex, first, count, arenaCount);
		return "handle";
	}
	run = {data, data + static_cast<size_t>(count) * blockSize};

	int freeBlocks = 0;
	for (int i = 0; i < count; i++) {
		if (map[first + i] != 1) {
			freeBlocks++;
		}
	}
	if (freeBlocks || data != arenaData + static_cast<size_t>(first) * blockSize) {
		snprintf(detail, detailSize, "arena %d first %d count %d: %d blocks not marked used, data %p expected %p allocCount %d", arenaIndex,
			first, count, freeBlocks, data, arenaData + static_cast<size_t>(first) * blockSize, *reinterpret_cast<int32*>(arena + 0x20));
		return "freed";
	}

	// The layout and command slots below are only known for the builder types 1 and 2.
	const uint32 builder = *reinterpret_cast<uint32*>(internal + 0xE0) & 0xF0000000;
	if (builder != 0x10000000 && builder != 0x20000000) {
		return nullptr;
	}

	auto start      = *reinterpret_cast<byte8**>(internal + 0x188);
	auto blockBytes = *reinterpret_cast<uint32*>(internal + 0xF0);
	auto meshCount  = *reinterpret_cast<uint16*>(internal + 0xE8);
	auto meshes     = *reinterpret_cast<byte8**>(internal + 0x100);
	if (!start || start < run.begin || start + blockBytes > run.end || meshCount > 32 || !meshes || meshes < start ||
		meshes + MESH_STRIDE * meshCount > start + blockBytes) {
		snprintf(detail, detailSize, "block start %p size %X meshes %p x%u outside heap block %p", start, blockBytes, meshes, meshCount, data);
		return "layout";
	}

	// Every command slot of every mesh points into the draw's own block.
	for (uint32 i = 0; i < meshCount; i++) {
		auto slots = reinterpret_cast<uint64*>(meshes + MESH_STRIDE * i + 0x40);
		for (int frame = 0; frame < 2; frame++) {
			auto slot = reinterpret_cast<byte8*>(slots[frame]);
			if (slot < start || slot >= start + blockBytes) {
				snprintf(detail, detailSize, "mesh %u slot[%d] = %llX, block %p+%X, mesh %p", i, frame, slots[frame], start, blockBytes,
					meshes + MESH_STRIDE * i);
				return "slot";
			}
		}
	}
	return nullptr;
}

const char* s_lastReload = "none";
ULONGLONG s_lastReloadTime = 0;
uint32 s_reloadCount = 0;

} // namespace

// Breadcrumb for the audit log: which HUD model loader ran last.
void HUD_NoteReload(const char* what) {
	s_lastReload     = what;
	s_lastReloadTime = GetTickCount64();
	s_reloadCount++;
}

// Walks every HUD model: its heap block must still be marked used in the arena, its command slots must point into
// its own block, and no two models may share heap blocks. Logs the first problem of each kind per model (until the
// HUD objects are rebuilt) and returns true when something is wrong. Cheap enough to run every tick.
bool HUD_AuditModels(const char* where) {
	static byte8* s_objects[3] = {};
	static uint32 s_reported[32] = {};
	static uint32 s_total = 0;

	byte8* objects[3] = {HudObject(0x1B070), HudObject(0x1B078), HudObject(0x1B080)};
	if (std::memcmp(objects, s_objects, sizeof(objects)) != 0) {
		std::memcpy(s_objects, objects, sizeof(objects));
		std::memset(s_reported, 0, sizeof(s_reported));
	}

	HudModel models[32];
	const size_t count = CollectHudModels(models);
	HeapRun runs[32] = {};
	bool bad = false;

	auto report = [&](size_t index, const char* problem, const char* detail) {
		bad = true;
		const uint32 bit = 1u << (problem[0] & 31);
		if ((s_reported[index] & bit) || s_total >= 64) {
			return;
		}
		s_reported[index] |= bit;
		s_total++;
		Log("HudModel %s (%s): %s %p serial %u, last reload %s %llu ms ago (#%u): %s", problem, where, models[index].name, models[index].model,
			g_hudIconSerial, s_lastReload, GetTickCount64() - s_lastReloadTime, s_reloadCount, detail);
	};

	char detail[256];
	for (size_t i = 0; i < count; i++) {
		if (const char* problem = ModelBlockProblem(models[i].model, detail, sizeof(detail), runs[i])) {
			report(i, problem, detail);
		}
	}
	for (size_t i = 0; i < count; i++) {
		for (size_t j = i + 1; j < count; j++) {
			if (runs[i].begin && runs[j].begin && runs[i].begin < runs[j].end && runs[j].begin < runs[i].end) {
				snprintf(detail, sizeof(detail), "shares heap blocks with %s %p: %p..%p and %p..%p", models[j].name, models[j].model, runs[i].begin,
					runs[i].end, runs[j].begin, runs[j].end);
				report(i, "overlap", detail);
			}
		}
	}
	return bad;
}

namespace {

// Watches the game's kernel heap for blocks that a live HUD model still uses: a free through another handle, or an
// allocation that overlaps a model block (the second owner is logged with its caller). CKernelHeap_Alloc_sub_1402C6150
// and CKernelHeapHandle_Free_sub_140337710 are called from every loader, so the checks only read.
using HeapAllocFn = void*(__fastcall*)(byte8*, uint32, int);
using HeapFreeFn  = __int64(__fastcall*)(byte8*);
std::unique_ptr<Utility::Detour_t> s_heapAllocHook;
std::unique_ptr<Utility::Detour_t> s_heapFreeHook;
constexpr uintptr_t HEAP_ALLOC_OFFSET = 0x2C6150;
constexpr uintptr_t HEAP_FREE_OFFSET  = 0x337710;

// Range of heap blocks a handle owns, false when it owns none.
bool HandleRange(byte8* handle, byte8*& begin, byte8*& end) {
	auto arena = *reinterpret_cast<byte8**>(handle + 8);
	if (!arena) {
		return false;
	}
	auto data      = *reinterpret_cast<byte8**>(handle + 0x10);
	auto count     = *reinterpret_cast<int32*>(handle + 0x1C);
	auto blockSize = *reinterpret_cast<int32*>(arena + 0x14);
	if (!data || count <= 0 || blockSize <= 0 || count > 0x100000) {
		return false;
	}
	begin = data;
	end   = data + static_cast<size_t>(count) * blockSize;
	return true;
}

// The live HUD model (other than ignoreHandle's own) whose blocks overlap [begin, end), or nullptr. Reads game memory
// that other threads may release, so a fault counts as no match.
const char* OverlappingHudModel(byte8* begin, byte8* end, byte8* ignoreHandle) {
	__try {
		HudModel models[32];
		const size_t count = CollectHudModels(models);
		for (size_t i = 0; i < count; i++) {
			byte8* handle = models[i].model + DRAW_HEAP_HANDLE;
			byte8* b      = nullptr;
			byte8* e      = nullptr;
			if (handle != ignoreHandle && HandleRange(handle, b, e) && begin < e && b < end) {
				return models[i].name;
			}
		}
	} __except (EXCEPTION_EXECUTE_HANDLER) {
	}
	return nullptr;
}

uint32 s_heapReports = 0;

void* __fastcall HeapAllocHook(byte8* handle, uint32 size, int poolSel) {
	const auto original = s_heapAllocHook->GetTrampoline<HeapAllocFn>();
	void* const caller  = _ReturnAddress();
	void* const result  = original(handle, size, poolSel);
	byte8* begin        = nullptr;
	byte8* end          = nullptr;
	if (result && s_heapReports < 32 && HandleRange(handle, begin, end)) {
		if (const char* name = OverlappingHudModel(begin, end, handle)) {
			s_heapReports++;
			Log("HeapWatch alloc overlaps %s: handle %p %p..%p size %X pool %d, caller dmc3.exe+%llX, serial %u, last reload %s", name, handle, begin,
				end, size, poolSel, reinterpret_cast<uintptr_t>(caller) - reinterpret_cast<uintptr_t>(appBaseAddr), g_hudIconSerial, s_lastReload);
		}
	}
	return result;
}

__int64 __fastcall HeapFreeHook(byte8* handle) {
	const auto original = s_heapFreeHook->GetTrampoline<HeapFreeFn>();
	void* const caller  = _ReturnAddress();
	byte8* begin        = nullptr;
	byte8* end          = nullptr;
	if (s_heapReports < 32 && HandleRange(handle, begin, end)) {
		if (const char* name = OverlappingHudModel(begin, end, handle)) {
			s_heapReports++;
			Log("HeapWatch free through another handle of %s's blocks: handle %p %p..%p, caller dmc3.exe+%llX, serial %u, last reload %s", name, handle,
				begin, end, reinterpret_cast<uintptr_t>(caller) - reinterpret_cast<uintptr_t>(appBaseAddr), g_hudIconSerial, s_lastReload);
		}
	}
	return original(handle);
}

} // namespace

void HUD_InitHeapWatch() {
	s_heapAllocHook = std::make_unique<Utility::Detour_t>(
		(uintptr_t)appBaseAddr + HEAP_ALLOC_OFFSET, &HeapAllocHook, 0, "CKernelHeap_Alloc_sub_1402C6150");
	s_heapAllocHook->Toggle(true);
	s_heapFreeHook = std::make_unique<Utility::Detour_t>(
		(uintptr_t)appBaseAddr + HEAP_FREE_OFFSET, &HeapFreeHook, 0, "CKernelHeapHandle_Free_sub_140337710");
	s_heapFreeHook->Toggle(true);
}

namespace {

// Per player copies of the original HUD models the mod reloads (HP bar, DT gauge / flux / explosion, style icon,
// weapon icons, lock-on reticle). The HUD objects are shared by all views and each tick is built for one player, so
// reloading them for the viewed player every tick (freeing and loading heap blocks, GPU resources and resetting the
// model animation) flickers and breaks the heap. Instead every player keeps its own loaded CDrawModel_t image and the
// images are swapped in place, always at the same address, so the models are only loaded once per player.
constexpr size_t DRAW_MODEL_SIZE = sizeof(ModelData);
constexpr uint32 TOP_SWAPPED[]    = {hudTopOffs[HUD_TOP::UPPER_HIT_POINTS_FRAME], hudTopOffs[HUD_TOP::UPPER_HIT_POINTS_BAR],
	hudTopOffs[HUD_TOP::UPPER_HIT_POINTS_BACKGROUND], hudTopOffs[HUD_TOP::LOWER_HIT_POINTS_FRAME], hudTopOffs[HUD_TOP::LOWER_HIT_POINTS_BAR],
	hudTopOffs[HUD_TOP::LOWER_HIT_POINTS_BACKGROUND], hudTopOffs[HUD_TOP::MAGIC_ORBS], hudTopOffs[HUD_TOP::STYLE_ICON],
	hudTopOffs[HUD_TOP::FLUX], hudTopOffs[HUD_TOP::DTEXPLOSION]};
constexpr uint32 BOTTOM_SWAPPED[] = {hudBottomOffs[HUD_BOTTOM::RANGED_WEAPON_1], hudBottomOffs[HUD_BOTTOM::MELEE_WEAPON_1]};
constexpr uint32 CURSOR_SWAPPED[] = {static_cast<uint32>(LOCKON_RETICLE)};
// CUIDStylishCombo: the 7 rank models (0x780 apart from +0x6C0).
constexpr uint32 COMBO_SWAPPED[]  = {0x6C0, 0xE40, 0x15C0, 0x1D40, 0x24C0, 0x2C40, 0x33C0};
constexpr size_t MAX_SWAPPED = std::size(TOP_SWAPPED);

struct ModelGroup {
	size_t slotOffset;
	const uint32* offsets;
	size_t count;
	byte8* object = nullptr;
	int owner     = -1; // player whose images are in the live object, -1 = nobody's (the game's own)
	bool valid[PLAYER_COUNT] = {};
	alignas(16) uint8 saved[PLAYER_COUNT][MAX_SWAPPED][DRAW_MODEL_SIZE];
};

ModelGroup s_modelGroups[4] = {
	{0x1B070, TOP_SWAPPED, std::size(TOP_SWAPPED)},
	{0x1B078, BOTTOM_SWAPPED, std::size(BOTTOM_SWAPPED)},
	{0x1B080, CURSOR_SWAPPED, std::size(CURSOR_SWAPPED)},
	{0x1B090, COMBO_SWAPPED, std::size(COMBO_SWAPPED)},
};

byte8* GroupModel(ModelGroup& group, size_t i) {
	return group.object + group.offsets[i];
}

// Releases the heap block and texture of a saved image through a scratch copy (the real object may be gone).
void FreeImage(const uint8* image) {
	alignas(16) static uint8 scratch[DRAW_MODEL_SIZE];
	std::memcpy(scratch, image, DRAW_MODEL_SIZE);
	func_897B0(*reinterpret_cast<ModelData*>(scratch));
}

void DiscardSavedImages(ModelGroup& group) {
	for (int p = 0; p < PLAYER_COUNT; p++) {
		if (group.valid[p]) {
			for (size_t i = 0; i < group.count; i++) {
				FreeImage(group.saved[p][i]);
			}
			group.valid[p] = false;
		}
	}
}

// Empties the live model without freeing it: its heap block belongs to a saved image.
void DetachModel(byte8* model) {
	auto& modelData = *reinterpret_cast<ModelData*>(model);
	SetMemory(&modelData, 0, sizeof(ModelData));
	func_89270(modelData);
}

constexpr unsigned MODEL_GROUP_BLANK[4] = {HUD_MODELS_TOP_BLANK, HUD_MODELS_BOTTOM_BLANK, HUD_MODELS_CURSOR_BLANK, 16};
constexpr unsigned MODEL_COMBO_BLANK = 16;

// Makes player's images the live ones. Returns the blank flag when the player has no image yet, and HUD_MODELS_REBUILT
// when the game made a new HUD object (all saved images are dropped).
unsigned ActivateGroup(ModelGroup& group, int groupIndex, int player) {
	byte8* object = HudObject(group.slotOffset);
	unsigned flags = 0;
	if (object != group.object) {
		DiscardSavedImages(group);
		group.object = object;
		group.owner  = -1;
		flags |= HUD_MODELS_REBUILT;
	}
	if (!object || group.owner == player) {
		return flags;
	}
	if (group.owner >= 0) {
		for (size_t i = 0; i < group.count; i++) {
			std::memcpy(group.saved[group.owner][i], GroupModel(group, i), DRAW_MODEL_SIZE);
		}
		group.valid[group.owner] = true;
		if (group.valid[player]) {
			for (size_t i = 0; i < group.count; i++) {
				std::memcpy(GroupModel(group, i), group.saved[player][i], DRAW_MODEL_SIZE);
			}
			group.valid[player] = false;
		} else {
			for (size_t i = 0; i < group.count; i++) {
				DetachModel(GroupModel(group, i));
			}
			flags |= MODEL_GROUP_BLANK[groupIndex];
		}
	}
	group.owner = player;
	return flags;
}

// ---- Per player vanilla style meters (CUIDStylishCombo) ----
// The meter draws its rank models through the display list, whose packets are linked into an intrusive list: drawing the
// same model (the same packet memory) more than once in a frame links it into itself and the display list never ends,
// which froze the game (the upload guard below turned the resulting buffer overflow into an endless wrap). So every extra
// player gets their own set of images like the cockpit: the 7 rank models, the 49 blocks of 0x260 bytes the game hands
// them (7 per model, a table at combo+0x3B40, allocated by the creation function sub_1402BA790) and a copy of the object
// state that follows them (+0x3B40..+0x3D40: block table, draw matrix, rank / alpha / animation state). The extra images
// are loaded with the game's own model loader (sub_1402BAAA0) and updated with the game's own update, once each per tick.
constexpr size_t COMBO_BLOCK_TABLE = 0x3B40; // 7 models x 7 block pointers, 0x38 per model
constexpr size_t COMBO_REGION_END  = 0x3D40;
constexpr size_t COMBO_REGION_SIZE = COMBO_REGION_END - COMBO_BLOCK_TABLE;
constexpr size_t COMBO_STYLE_POINTER = 0x3D10; // pointer to the style data it shows (set by the game, shared)
constexpr size_t COMBO_BLOCKS      = 49;
constexpr size_t COMBO_BLOCK_SIZE  = 0x260;
constexpr size_t COMBO_MODEL_COUNT = 7;
constexpr uintptr_t COMBO_LOAD_OFFSET       = 0x2BAAA0; // sub_1402BAAA0 (combo): loads the 7 rank models, returns 1 on success
constexpr uintptr_t HEAP_HANDLE_INIT_OFFSET = 0x4C7A0;  // sub_14004C7A0 (handle)
constexpr uintptr_t DRAW_BLOCK_INIT_OFFSET  = 0x30E270; // sub_14030E270 (block of 0x260 bytes)

struct ComboSeat {
	alignas(16) uint8 handles[COMBO_BLOCKS][0x20];
	uint8* blocks[COMBO_BLOCKS] = {};
	int allocated    = 0;
	bool failed      = false;
	bool stateValid  = false;
	uint8 state[COMBO_REGION_SIZE] = {};
};
ComboSeat s_comboSeats[PLAYER_COUNT];

void FreeComboSeatBlocks(ComboSeat& seat) {
	for (int i = 0; i < seat.allocated; i++) {
		func_337710(seat.handles[i]);
		seat.blocks[i] = nullptr;
	}
	seat.allocated  = 0;
	seat.failed     = false;
	seat.stateValid = false;
}

bool AllocateComboSeatBlocks(ComboSeat& seat) {
	using HandleInitFn = void(__fastcall*)(void*);
	using BlockInitFn  = void*(__fastcall*)(void*);
	const auto handleInit = reinterpret_cast<HandleInitFn>(appBaseAddr + HEAP_HANDLE_INIT_OFFSET);
	const auto blockInit  = reinterpret_cast<BlockInitFn>(appBaseAddr + DRAW_BLOCK_INIT_OFFSET);
	for (size_t i = 0; i < COMBO_BLOCKS; i++) {
		handleInit(seat.handles[i]);
	}
	for (size_t i = 0; i < COMBO_BLOCKS; i++) {
		if (!func_2C6150(seat.handles[i], COMBO_BLOCK_SIZE, 1)) {
			return false;
		}
		seat.allocated = static_cast<int>(i + 1);
		seat.blocks[i] = *reinterpret_cast<uint8**>(seat.handles[i] + 0x10);
		blockInit(seat.blocks[i]);
	}
	return true;
}

// Makes player's combo (rank model images + object state) the live one. False when that player's set could not be made.
bool ActivateComboSeat(uint8* combo, int player) {
	if (!combo || player < 0 || player >= PLAYER_COUNT) {
		return false;
	}
	ModelGroup& group  = s_modelGroups[3];
	const int previous = group.owner;
	const unsigned flags = ActivateGroup(group, 3, player);
	uint8* live = combo + COMBO_BLOCK_TABLE;

	if (flags & HUD_MODELS_REBUILT) {
		for (auto& seat : s_comboSeats) {
			FreeComboSeatBlocks(seat);
		}
	} else if (previous >= 0 && previous != player) {
		const uint64 stylePointer = *reinterpret_cast<uint64*>(combo + COMBO_STYLE_POINTER);
		ComboSeat& from = s_comboSeats[previous];
		std::memcpy(from.state, live, COMBO_REGION_SIZE);
		from.stateValid = true;
		ComboSeat& to = s_comboSeats[player];
		if (to.stateValid) {
			std::memcpy(live, to.state, COMBO_REGION_SIZE);
		}
		*reinterpret_cast<uint64*>(combo + COMBO_STYLE_POINTER) = stylePointer;
	}

	ComboSeat& seat = s_comboSeats[player];
	if (flags & MODEL_COMBO_BLANK) {
		bool ok = !seat.failed && AllocateComboSeatBlocks(seat);
		if (ok) {
			// A fresh object starts with the rank / animation state zeroed (the update only runs the rank transition that makes
			// a model visible when the rank changes), and these models are fresh, so they must not inherit the live state.
			constexpr size_t animationState = 0x3D18;
			std::memset(combo + animationState, 0, COMBO_REGION_END - animationState);
			for (size_t m = 0; m < COMBO_MODEL_COUNT; m++) {
				for (size_t j = 0; j < 7; j++) {
					*reinterpret_cast<uint64*>(live + 0x38 * m + 8 * j) = reinterpret_cast<uint64>(seat.blocks[7 * m + j]);
				}
			}
			using LoadFn = char(__fastcall*)(uint8*);
			ok = reinterpret_cast<LoadFn>(appBaseAddr + COMBO_LOAD_OFFSET)(combo) != 0;
		}
		if (!ok) {
			Log("Vanilla style meter: could not load the models for player %d", player + 1);
			seat.failed = true;
		}
	}
	return !seat.failed;
}

// Back to the game's own combo, the extra players' images and blocks are freed.
void ReleaseComboSeats() {
	ModelGroup& group = s_modelGroups[3];
	bool any = group.owner > 0;
	for (int p = 0; p < PLAYER_COUNT; p++) {
		any = any || group.valid[p] || s_comboSeats[p].allocated > 0;
	}
	if (!any) {
		return;
	}
	byte8* combo = HudObject(0x1B090);
	if (combo && group.object == combo) {
		ActivateComboSeat(combo, 0);
	}
	DiscardSavedImages(group);
	for (auto& seat : s_comboSeats) {
		FreeComboSeatBlocks(seat);
	}
}

} // namespace

unsigned HUD_ActivatePlayerModels(int player, unsigned groups) {
	if (player < 0 || player >= PLAYER_COUNT) {
		return 0;
	}
	unsigned flags = 0;
	for (int g = 0; g < 3; g++) {
		if (groups & (1u << g)) {
			flags |= ActivateGroup(s_modelGroups[g], g, player);
		}
	}
	return flags;
}

// Back to a single set of models (player 1's), the other players' images are freed.
unsigned HUD_ReleasePlayerModels() {
	ReleaseComboSeats();
	unsigned flags = 0;
	for (int g = 0; g < 3; g++) {
		auto& group = s_modelGroups[g];
		bool any = group.owner > 0;
		for (int p = 0; p < PLAYER_COUNT; p++) {
			any = any || group.valid[p];
		}
		if (!any) {
			continue;
		}
		flags |= ActivateGroup(group, g, 0);
		DiscardSavedImages(group);
	}
	return flags;
}

void HUD_UpdateLockOn(uint8 character) {
    g_hudIconSerial++;
    HUD_NoteReload("lock-on");
    if ((InCutscene()) || (InCredits())) {
        return;
    }

	auto name_143 = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E28);
	if (!name_143) {
		return;
	}
	name_143 -= 0x180;
	auto lockOnCursor = *reinterpret_cast<byte8**>(name_143 + 0x1B080);
	if (!lockOnCursor) {
		return;
	}

	// A reload that finds the block already released would free blocks that now belong to someone else.
	HUD_AuditModels("before lock-on reload");
	static uint32 s_reloads = 0;
	const uint32 reload = ++s_reloads;
	const bool logReload = reload <= 16 || reload % 600 == 0;
	auto handle = lockOnCursor + LOCKON_RETICLE + DRAW_HEAP_HANDLE;
	if (logReload) {
		Log("HUD_UpdateLockOn #%u: character %u cursor %p handle arena %p first %d count %d", reload, character, lockOnCursor,
			*reinterpret_cast<void**>(handle + 8), *reinterpret_cast<int32*>(handle + 0x18), *reinterpret_cast<int32*>(handle + 0x1C));
	}

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

    HUD_AuditModels("after lock-on reload");
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

    // The other players' saved model images were loaded from the files that are being replaced: drop them (the next
    // tick loads them again from the new files) before anything draws them.
    HUD_ReleasePlayerModels();

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

static bool s_repairRequested = false;

void HUD_RequestRepair() {
    s_repairRequested = true;
}

// After a HUD draw fault: reload every HUD model for player 1's character, then the weapon icons, and let the
// splitscreen icon cache start over. At most 3 times in 10 s, so a model that cannot be repaired only skips its draw.
void HUD_RunRepair() {
    if (!s_repairRequested || InCutscene() || InCredits()) {
        return;
    }
    s_repairRequested = false;

    static ULONGLONG s_windowStart = 0;
    static uint32 s_repairs        = 0;
    const ULONGLONG now = GetTickCount64();
    if (now - s_windowStart > 10000) {
        s_windowStart = now;
        s_repairs     = 0;
    }
    if (s_repairs++ >= 3) {
        return;
    }

    Log("HUD repair #%u: reloading the HUD models", s_repairs);
    HUD_ReloadIcons();

    auto& playerData    = GetPlayerData(0);
    auto& characterData = GetCharacterData(0, playerData.activeCharacterIndex, ENTITY::MAIN);
    if (characterData.meleeWeaponIndex < MELEE_WEAPON_COUNT) {
        HUD_UpdateWeaponIcon(HUD_BOTTOM::MELEE_WEAPON_1, static_cast<uint8>(characterData.meleeWeapons[characterData.meleeWeaponIndex]));
    }
    if (characterData.rangedWeaponIndex < RANGED_WEAPON_COUNT) {
        HUD_UpdateWeaponIcon(HUD_BOTTOM::RANGED_WEAPON_1, static_cast<uint8>(characterData.rangedWeapons[characterData.rangedWeaponIndex]));
    }
    g_hudIconSerial++;
}

#pragma region Adjustments of the original game HUD (CUIDCockpit)

namespace {

constexpr uintptr_t COCKPIT_TOP_DRAW_OFFSET    = 0x27F1F0;
constexpr uintptr_t COCKPIT_BOTTOM_DRAW_OFFSET = 0x281120;
// CUIDCockpit00: 13 element matrices, 0x80 apart. The first is the upper HP frame.
constexpr size_t COCKPIT_TOP_MATRIX_BASE   = 0xB0;
constexpr size_t COCKPIT_TOP_MATRIX_STRIDE = 0x80;
constexpr size_t COCKPIT_TOP_MATRIX_COUNT  = 13;
// CUIDCockpit02: two consecutive matrices (identity, the layout lives in the model).
constexpr size_t COCKPIT_BOTTOM_MATRIX_BASE  = 0x26E0;
constexpr size_t COCKPIT_BOTTOM_MATRIX_COUNT = 2;

constexpr uintptr_t RENDER_FRAME_STATE_OFFSET  = 0xD6D300; // pRenderFrameState_140D6D300: +0x20 / +0x22 = HUD frame width / height (int16)
constexpr uintptr_t HUD_WIDESCREEN_FLAG_OFFSET = 0x553034; // HudWidescreenFlag_140553034
constexpr float HUD_WIDESCREEN_WIDTH_FACTOR    = 1.3f;     // visible width = frame width * this when the flag is set

// Every point of the top-left group goes through p' = S * p + (1 - S) * pivot + G on the screen (G = general x/y,
// y down). The game HUD scales around its own origin, so its matrices only get p' = S * p + G / k (k = pixels per
// HUD unit relative to screenHeight / 448; HUD y points up), and the addons use the screen position of that HUD
// origin as pivot. k comes from the game's HUD frame size and widescreen flag; the calibration only trims it.
// Splitscreen applies the general values per layout on top of that layout's top-left group.
struct HudCalibration {
	float unitX; // multiplier on the derived k, x
	float unitY; // multiplier on the derived k, y
	float pivotX; // screen position of the HUD origin, from the screen centre, in units of screenHeight / 448
	float pivotY;
	float scaleRate; // game HUD scale = 1 + (S - 1) * scaleRate; the addons use S as is
};

constexpr HudGeneral HUD_GENERAL_SINGLE = { 1.0f, 0.0f, 0.0f };
constexpr HudGroupTuning HUD_GROUP_IDENTITY = { 1.0f, 1.0f, 0.0f, 0.0f };
constexpr HudCalibration HUD_CALIBRATION = { 0.988f, 0.665f, -19.0f, -3.0f, 1.0f };
constexpr HudPreset HUD_PRESETS_SINGLE[] = {
	{ "Default", { 1.000f, 0.0f, 0.0f } },
	{ "Compact", { 0.810f, -2.0f, 4.0f } },
	{ "Smaller", { 0.800f, -75.5f, -39.0f } },
};

HudGeneral s_singleHudGeneral = HUD_GENERAL_SINGLE;
HudCalibration s_hudCalibration = HUD_CALIBRATION;
bool s_singleHudSyncAddons = true;
bool s_hudShowPivot = false;

using CockpitFn = void(__fastcall*)(uint8_t*);
std::unique_ptr<Utility::Detour_t> s_cockpitTopDrawHook;
std::unique_ptr<Utility::Detour_t> s_cockpitBottomDrawHook;
std::unique_ptr<Utility::Detour_t> s_lockOnDrawHook;
std::unique_ptr<Utility::Detour_t> s_styleComboDrawHook;
constexpr uintptr_t LOCKON_DRAW_OFFSET = 0x297540; // DrawCUIDLockOnCursor_sub_140297540
constexpr uintptr_t STYLE_COMBO_DRAW_OFFSET = 0x2BB160; // DrawCUIDStylishCombo_sub_1402BB160
constexpr uintptr_t SCENE_STATE_CURSOR_OFFSET = 0xCF2330; // pSceneStateCursor_140CF2330

// The original HUD draws go through the game's model code, which faults when a model's heap block is gone. Skip that
// draw instead of taking the game down, log what is wrong and rebuild the HUD models on the next tick.
LONG DrawFault(EXCEPTION_POINTERS* info, const char* what) {
	if (info->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION) {
		return EXCEPTION_CONTINUE_SEARCH;
	}
	static uint32 s_faults = 0;
	if (s_faults++ < 8) {
		Log("HUD draw fault (%s): dmc3.exe+%llX address %llX", what, info->ContextRecord->Rip - reinterpret_cast<DWORD64>(appBaseAddr),
			info->ExceptionRecord->ExceptionInformation[1]);
		HUD_AuditModels("draw fault");
	}
	return EXCEPTION_EXECUTE_HANDLER;
}

bool GuardedDraw(CockpitFn draw, uint8_t* object, const char* what) {
	__try {
		draw(object);
		return true;
	} __except (DrawFault(GetExceptionInformation(), what)) {
		HUD_RequestRepair();
		return false;
	}
}

// DrawCUIDLockOnCursor_sub_140297540
void __fastcall LockOnDrawHook(uint8_t* cursor) {
	GuardedDraw(s_lockOnDrawHook->GetTrampoline<CockpitFn>(), cursor, "lock-on cursor");
}

bool SplitscreenActive() {
	return CrimsonSplitscreen::ViewCount() >= 2;
}

// Interface option "HUD Scale: Smaller": the general adjustment single player runs with (the Single Player HUD window
// values apply with "Default").
constexpr HudGeneral HUD_GENERAL_SINGLE_SMALLER = { 0.800f, -75.5f, -39.0f };

bool SmallerSinglePlayerHud() {
	return activeCrimsonConfig.HudOptions.hudScale == HUDSCALE::SMALLER;
}

const HudGeneral& SinglePlayerGeneral() {
	return SmallerSinglePlayerHud() ? HUD_GENERAL_SINGLE_SMALLER : s_singleHudGeneral;
}

// Red orb counter: one for all players (they share the count). Vanilla = the cockpit's right group, moved like the
// top-left general (scale around the HUD origin, x / y in HUD units, y downward) in single player and on the shared /
// single player camera. In splitscreen the game would draw it inside every view, so it is left out there and the
// ImGui counter (vanilla orb texture when the Crimson addon is off) stands in for it. Crimson = that ImGui counter:
// scale around its top-right corner, x / y in HUD units from its default place.
constexpr HudGeneral HUD_RED_ORB_VANILLA = { 1.0f, 0.0f, 0.0f };
constexpr HudGeneral HUD_RED_ORB_CRIMSON = { 1.0f, 0.0f, 0.0f };
constexpr HudPreset HUD_PRESETS_RED_ORB[] = {
	{ "Default", { 1.000f, 0.0f, 0.0f } },
};
HudGeneral s_redOrbVanilla = HUD_RED_ORB_VANILLA;
HudGeneral s_redOrbCrimson = HUD_RED_ORB_CRIMSON;

bool RedOrbVanillaTouched() {
	const HudGeneral& g = s_redOrbVanilla;
	return g.scale != 1.0f || g.offsetX != 0.0f || g.offsetY != 0.0f;
}

bool SinglePlayerHudTouched() {
	const HudGeneral& g = SinglePlayerGeneral();
	return g.scale != 1.0f || g.offsetX != 0.0f || g.offsetY != 0.0f || RedOrbVanillaTouched();
}

// Pixels per HUD unit of the game HUD, relative to screenHeight / 448, before the calibration trim.
struct HudFrameInfo {
	int frameWidth = 0;
	int frameHeight = 0;
	bool widescreen = false;
	float unitX = 1.0f;
	float unitY = 1.0f;
	bool valid = false;
};

HudFrameInfo ReadHudFrameInfo(float screenWidth, float screenHeight) {
	HudFrameInfo info;
	const auto frameState = *reinterpret_cast<const uintptr_t*>(appBaseAddr + RENDER_FRAME_STATE_OFFSET);
	if (frameState <= 0x10000 || (frameState & 0xFFF0000000000000ull) || screenWidth <= 0.0f || screenHeight <= 0.0f) {
		return info;
	}
	info.frameWidth  = *reinterpret_cast<const int16_t*>(frameState + 0x20);
	info.frameHeight = *reinterpret_cast<const int16_t*>(frameState + 0x22);
	info.widescreen  = *reinterpret_cast<const uint8_t*>(appBaseAddr + HUD_WIDESCREEN_FLAG_OFFSET) != 0;
	if (info.frameWidth <= 0 || info.frameHeight <= 0) {
		return info;
	}
	const float visibleWidth = static_cast<float>(info.frameWidth) * (info.widescreen ? HUD_WIDESCREEN_WIDTH_FACTOR : 1.0f);
	const float unit = screenHeight / HUD_HEIGHT;
	info.unitX = (screenWidth / visibleWidth) / unit;
	info.unitY = (screenHeight / static_cast<float>(info.frameHeight)) / unit;
	info.valid = true;
	return info;
}

// Matrix transform ShrinkHudMatrix applies to the top-left cockpit elements: the layout base (identity in single
// player), then p' = p * S + G / k around the HUD origin (y up).
HudGroupTuning ComposeTopLeftGroup(const HudGroupTuning& base, const HudGeneral& g) {
	const HudCalibration& c = s_hudCalibration;
	const HudFrameInfo frame = ReadHudFrameInfo(g_uiSize.x, g_uiSize.y);
	const float unitX = frame.unitX * c.unitX;
	const float unitY = frame.unitY * c.unitY;
	const float gameScale = 1.0f + (g.scale - 1.0f) * c.scaleRate;
	return {
		base.sizeScale * gameScale,
		base.posScale * gameScale,
		base.offsetX * gameScale + g.offsetX / unitX,
		base.offsetY * gameScale - g.offsetY / unitY,
	};
}

// Row-vector matrix: output x/y are columns 0/1, translation is the last row.
void ShrinkHudMatrix(float* m, const HudGroupTuning& group) {
	for (int row = 0; row < 3; ++row) {
		m[row * 4 + 0] *= group.sizeScale;
		m[row * 4 + 1] *= group.sizeScale;
	}
	m[12] = m[12] * group.posScale + group.offsetX;
	m[13] = m[13] * group.posScale + group.offsetY;
}

// The red orb counter group is anchored to the right of the screen, the rest to the left.
bool IsRightHudElement(size_t i) {
	return i >= 8 && i <= 10;
}

// Multiplayer cockpits (shared / single player camera): every player's top-left cockpit group sits in a screen corner
// (player 1 top-left, 2 top-right, 3 bottom-left, 4 bottom-right), scaled around its own corner and then moved to the
// screen corner, mirroring the margins of the original top-left position. The group's size is a measured guess (HUD
// units, y up, from the HUD origin at the screen centre); the margins mirror the vanilla top-left ones.
constexpr int SEAT_MAX = 4;
struct SeatBounds {
	float width;
	float height;
	float margin;    // gap between the group and the left / right screen edge
	float topMargin; // gap between the group and the top / bottom screen edge
};
constexpr SeatBounds HUD_SEAT_BOUNDS = { 245.0f, 30.0f, 20.0f, 12.0f };
constexpr HudGeneral HUD_SEAT_GENERAL[SEAT_MAX] = {
	{ 0.700f, 1.5f, -17.5f },
	{ 0.700f, -104.0f, -17.5f },
	{ 0.700f, 1.5f, 90.0f },
	{ 0.700f, -104.0f, 90.0f },
};
constexpr HudPreset HUD_PRESETS_SEAT[] = {
	{ "Default", { 0.800f, 0.0f, 0.0f } },
	{ "Large", { 1.000f, 0.0f, 0.0f } },
};
HudGeneral s_seatGeneral[SEAT_MAX] = { HUD_SEAT_GENERAL[0], HUD_SEAT_GENERAL[1], HUD_SEAT_GENERAL[2], HUD_SEAT_GENERAL[3] };
SeatBounds s_seatBounds = HUD_SEAT_BOUNDS;

// Crimson style meter placement, see HUD.hpp. HUD units, y downward, one entry per player / view.
constexpr HudGeneral HUD_STYLE_METER_SINGLE = { 1.0f, 0.0f, 0.0f };
constexpr HudGeneral HUD_STYLE_METER_SEATS[SEAT_MAX] = {
	{ 0.600f, -428.0f, 24.0f },
	{ 0.600f, -2.5f, 24.5f },
	{ 0.600f, -428.0f, 270.5f },
	{ 0.600f, -2.5f, 270.5f },
};
constexpr HudGeneral HUD_STYLE_METER_HALVES[2] = {
	{ 0.630f, -182.0f, 0.0f },
	{ 0.630f, -182.0f, 0.0f },
};
constexpr HudGeneral HUD_STYLE_METER_QUADS[SEAT_MAX] = {
	{ 1.0f, 0.0f, 0.0f },
	{ 1.0f, 0.0f, 0.0f },
	{ 1.0f, 0.0f, 0.0f },
	{ 1.0f, 0.0f, 0.0f },
};
constexpr HudPreset HUD_PRESETS_STYLE_METER[] = {
	{ "Default", { 1.000f, 0.0f, 0.0f } },
	{ "Smaller", { 0.600f, 0.0f, 0.0f } },
};
HudGeneral s_styleMeterSingle = HUD_STYLE_METER_SINGLE;
HudGeneral s_styleMeterSeats[SEAT_MAX] = { HUD_STYLE_METER_SEATS[0], HUD_STYLE_METER_SEATS[1], HUD_STYLE_METER_SEATS[2], HUD_STYLE_METER_SEATS[3] };
HudGeneral s_styleMeterHalves[2] = { HUD_STYLE_METER_HALVES[0], HUD_STYLE_METER_HALVES[1] };
HudGeneral s_styleMeterQuads[SEAT_MAX] = { HUD_STYLE_METER_QUADS[0], HUD_STYLE_METER_QUADS[1], HUD_STYLE_METER_QUADS[2], HUD_STYLE_METER_QUADS[3] };

// Vanilla style meter (CUIDStylishCombo draw): the game draws it at the right edge of the frame, which the halves' centre
// crop cuts off, and on the shared camera every player needs their own. Placement in HUD units: x / y move it (y downward),
// scale scales it. The rank models are placed through the draw matrix the combo passes to them (combo + 0x3CD0), the bar
// is a 2D rect that is moved / scaled in its own coordinates (see DrawHudRectHook).
struct VanillaStyleShift {
	float x, y;
	float scale = 1.0f;
	float barX = 0.0f, barY = 0.0f; // extra offset of the charge bar only
};
constexpr VanillaStyleShift HUD_VANILLA_STYLE_HALVES[2] = { { -187.5f, -14.0f, 0.720f, -13.0f, -4.0f }, { -187.5f, -14.0f, 0.720f, -13.0f, -4.0f } };
constexpr VanillaStyleShift HUD_VANILLA_STYLE_QUADS[SEAT_MAX] = { { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f } };
// Shared / single player camera: one copy per player (the style meter is shared, so they all show the same). 1P is where
// the game draws it.
constexpr VanillaStyleShift HUD_VANILLA_STYLE_SEATS[SEAT_MAX] = { { -359.5f, 14.0f, 0.550f, -33.5f, -20.5f }, { 5.5f, 14.0f, 0.550f, 57.5f, -20.5f }, { -359.5f, 226.5f, 0.550f, -33.5f, -110.0f }, { 5.5f, 226.5f, 0.550f, 57.5f, -110.0f } };
VanillaStyleShift s_vanillaStyleHalves[2] = { HUD_VANILLA_STYLE_HALVES[0], HUD_VANILLA_STYLE_HALVES[1] };
VanillaStyleShift s_vanillaStyleQuads[SEAT_MAX] = { HUD_VANILLA_STYLE_QUADS[0], HUD_VANILLA_STYLE_QUADS[1], HUD_VANILLA_STYLE_QUADS[2], HUD_VANILLA_STYLE_QUADS[3] };
VanillaStyleShift s_vanillaStyleSeats[SEAT_MAX] = { HUD_VANILLA_STYLE_SEATS[0], HUD_VANILLA_STYLE_SEATS[1], HUD_VANILLA_STYLE_SEATS[2], HUD_VANILLA_STYLE_SEATS[3] };
// Pivot of the vanilla meter's scale, HUD units from the screen centre (y up): the models scale about it, so scaling keeps
// the meter in place. A guess from the bar (x 592..621, y 54 in the 640 x 448 space); tune in the debug window.
constexpr float HUD_VANILLA_STYLE_PIVOT_X = 286.0f;
constexpr float HUD_VANILLA_STYLE_PIVOT_Y = 169.0f;
float s_vanillaStylePivotX = HUD_VANILLA_STYLE_PIVOT_X;
float s_vanillaStylePivotY = HUD_VANILLA_STYLE_PIVOT_Y;
bool s_vanillaStyleAllSeats = true; // one vanilla style meter per player; off = a single one at the 1P position
// On: the rank animation plays while shifted (the pose is re-evaluated with the shifted matrix). Off: the older path, which
// clears the model's motion flag for the draw and shows the rest pose (static).
bool s_vanillaStyleAnimate = true;

// Weapon wheel placement. Splitscreen: one size for every view (fraction of the view height). Shared / single player
// camera: one size (fraction of the screen height) and a top-left position per player (fraction of the screen).
struct WheelSeat {
	float meleeX, meleeY;
	float rangedX, rangedY;
};
constexpr float HUD_WHEEL_SPLIT_SCALE = 0.300f;
constexpr float HUD_WHEEL_SEAT_SCALE  = 0.132f;
constexpr WheelSeat HUD_WHEEL_SEATS[SEAT_MAX] = {
	{ 0.1433f, 0.1540f, 0.0853f, 0.1540f },
	{ 0.6793f, 0.1540f, 0.6213f, 0.1540f },
	{ 0.1433f, 0.7102f, 0.0853f, 0.7102f },
	{ 0.6842f, 0.7102f, 0.6262f, 0.7102f },
};
float s_splitWheelScale = HUD_WHEEL_SPLIT_SCALE;
float s_seatWheelScale  = HUD_WHEEL_SEAT_SCALE;
WheelSeat s_wheelSeats[SEAT_MAX] = { HUD_WHEEL_SEATS[0], HUD_WHEEL_SEATS[1], HUD_WHEEL_SEATS[2], HUD_WHEEL_SEATS[3] };

// Anchor corner of a seat's group in HUD units (y up) and how far the scaling keeps it from its screen corner.
struct SeatPlacement {
	float cornerX, cornerY;
	float moveX, moveY;
	bool right, lower;
};

SeatPlacement ComputeSeatPlacement(int seat, const HudFrameInfo& frame) {
	const float halfWidth  = frame.valid ? 0.5f * static_cast<float>(frame.frameWidth) * (frame.widescreen ? HUD_WIDESCREEN_WIDTH_FACTOR : 1.0f) : 416.0f;
	const float halfHeight = frame.valid ? 0.5f * static_cast<float>(frame.frameHeight) : 224.0f;
	const SeatBounds& b = s_seatBounds;
	const float minX    = -halfWidth + b.margin;
	const float maxX    = minX + b.width;
	const float top     = halfHeight - b.topMargin;
	const float bottom  = top - b.height;

	SeatPlacement p;
	p.right   = (seat & 1) != 0;
	p.lower   = (seat & 2) != 0;
	p.cornerX = p.right ? maxX : minX;
	p.cornerY = p.lower ? bottom : top;
	p.moveX   = p.right ? (halfWidth - b.margin - maxX) : 0.0f;
	p.moveY   = p.lower ? (-halfHeight + b.topMargin - bottom) : 0.0f;
	return p;
}

// Matrix transform of a seat's cockpit group: p' = S * (p - corner) + corner + move + nudge (S = the game HUD scale).
HudGroupTuning SeatGroup(int seat, const HudFrameInfo& frame) {
	const HudCalibration& c = s_hudCalibration;
	const HudGeneral& g = s_seatGeneral[seat];
	const SeatPlacement p = ComputeSeatPlacement(seat, frame);
	const float unitX = frame.unitX * c.unitX;
	const float unitY = frame.unitY * c.unitY;
	const float gameScale = 1.0f + (g.scale - 1.0f) * c.scaleRate;
	return {
		gameScale,
		gameScale,
		(1.0f - gameScale) * p.cornerX + p.moveX + g.offsetX / unitX,
		(1.0f - gameScale) * p.cornerY + p.moveY - g.offsetY / unitY,
	};
}

// The three red orb counter models of the cockpit (elements 8-10) are shared, not per player: hide them (clear their
// loaded flag, the draw then skips them) for a draw that must not show the counter.
constexpr size_t DRAW_LOADED_FLAG = 0x18;
void HideRedOrbModels(uint8_t* cockpit, uint8 (&saved)[3]) {
	for (size_t k = 0; k < 3; ++k) {
		uint8* flag = cockpit + hudTopOffs[8 + k] + DRAW_LOADED_FLAG;
		saved[k] = *flag;
		*flag = 0;
	}
}

void RestoreRedOrbModels(uint8_t* cockpit, const uint8 (&saved)[3]) {
	for (size_t k = 0; k < 3; ++k) {
		cockpit[hudTopOffs[8 + k] + DRAW_LOADED_FLAG] = saved[k];
	}
}

// Draws the cockpit once with the seat's transform. Player 1's seat also draws the red orb counter (with its own
// placement); the other seats leave it out, a second draw of the shared models would overwrite the first.
void DrawCockpitSeat(CockpitFn original, uint8_t* cockpit, int seat) {
	auto matrix = [cockpit](size_t i) {
		return reinterpret_cast<float*>(cockpit + COCKPIT_TOP_MATRIX_BASE + COCKPIT_TOP_MATRIX_STRIDE * i);
	};
	const HudFrameInfo frame = ReadHudFrameInfo(g_uiSize.x, g_uiSize.y);
	const HudGroupTuning group = SeatGroup(seat, frame);
	const HudGroupTuning redOrbGroup = ComposeTopLeftGroup(HUD_GROUP_IDENTITY, s_redOrbVanilla);

	float backup[COCKPIT_TOP_MATRIX_COUNT][16];
	for (size_t i = 0; i < COCKPIT_TOP_MATRIX_COUNT; ++i) {
		std::memcpy(backup[i], matrix(i), sizeof(backup[i]));
		if (!IsRightHudElement(i)) {
			ShrinkHudMatrix(matrix(i), group);
		} else if (seat == 0) {
			ShrinkHudMatrix(matrix(i), redOrbGroup);
		}
	}
	uint8 loaded[3] = {};
	if (seat > 0) {
		HideRedOrbModels(cockpit, loaded);
	}
	GuardedDraw(original, cockpit, "cockpit top");
	if (seat > 0) {
		RestoreRedOrbModels(cockpit, loaded);
	}
	for (size_t i = 0; i < COCKPIT_TOP_MATRIX_COUNT; ++i) {
		std::memcpy(matrix(i), backup[i], sizeof(backup[i]));
	}
}

// Player 1's cockpit is the game's own; for every other player the tick update kept their state, matrices and top
// models, which CrimsonSplitscreen::SeatCockpitBegin swaps in for their draw.
void DrawCockpitSeats(CockpitFn original, uint8_t* cockpit, int seats) {
	DrawCockpitSeat(original, cockpit, 0);
	for (int seat = 1; seat < seats; ++seat) {
		if (!CrimsonSplitscreen::SeatCockpitBegin(cockpit, seat)) {
			continue;
		}
		DrawCockpitSeat(original, cockpit, seat);
		CrimsonSplitscreen::SeatCockpitEnd(cockpit);
	}
}

// The draw copies the matrix, so it can be restored right after.
// CUIDCockpit00 draw (vtable +0x20)
void __fastcall CockpitTopDrawHook(uint8_t* cockpit) {
	const auto original = s_cockpitTopDrawHook->GetTrampoline<CockpitFn>();
	HudSplitLayout layout;
	const bool split = CrimsonSplitscreen::HudLayoutInUse(layout);
	const int seats = split ? 0 : HUD_SeatCount();
	if (cockpit && seats >= 2) {
		DrawCockpitSeats(original, cockpit, seats);
		return;
	}
	if (!cockpit || (!split && !SinglePlayerHudTouched())) {
		GuardedDraw(original, cockpit, "cockpit top");
		return;
	}

	auto matrix = [cockpit](size_t i) {
		return reinterpret_cast<float*>(cockpit + COCKPIT_TOP_MATRIX_BASE + COCKPIT_TOP_MATRIX_STRIDE * i);
	};
	const HudGroupTuning topLeftGroup = split
		? ComposeTopLeftGroup(layout.topLeft, layout.general)
		: ComposeTopLeftGroup(HUD_GROUP_IDENTITY, SinglePlayerGeneral());
	const HudGroupTuning redOrbGroup = ComposeTopLeftGroup(HUD_GROUP_IDENTITY, s_redOrbVanilla);
	float backup[COCKPIT_TOP_MATRIX_COUNT][16];
	for (size_t i = 0; i < COCKPIT_TOP_MATRIX_COUNT; ++i) {
		std::memcpy(backup[i], matrix(i), sizeof(backup[i]));
		if (split) {
			if (!IsRightHudElement(i)) {
				ShrinkHudMatrix(matrix(i), topLeftGroup);
			}
		} else if (IsRightHudElement(i)) {
			ShrinkHudMatrix(matrix(i), redOrbGroup);
		} else {
			ShrinkHudMatrix(matrix(i), topLeftGroup);
		}
	}
	// Splitscreen: the red orb counter is global, the ImGui counter stands in for it (see s_redOrbVanilla).
	uint8 loaded[3] = {};
	if (split) {
		HideRedOrbModels(cockpit, loaded);
	}
	GuardedDraw(original, cockpit, "cockpit top");
	if (split) {
		RestoreRedOrbModels(cockpit, loaded);
	}
	for (size_t i = 0; i < COCKPIT_TOP_MATRIX_COUNT; ++i) {
		std::memcpy(matrix(i), backup[i], sizeof(backup[i]));
	}
}

// CUIDCockpit02 draw (vtable +0x20)
void __fastcall CockpitBottomDrawHook(uint8_t* cockpit) {
	const auto original = s_cockpitBottomDrawHook->GetTrampoline<CockpitFn>();
	HUD_AuditModels("draw");
	HudSplitLayout layout;
	if (!cockpit || !CrimsonSplitscreen::HudLayoutInUse(layout)) {
		GuardedDraw(original, cockpit, "cockpit bottom");
		return;
	}

	auto matrix = [cockpit](size_t i) {
		return reinterpret_cast<float*>(cockpit + COCKPIT_BOTTOM_MATRIX_BASE + 0x40 * i);
	};
	float backup[COCKPIT_BOTTOM_MATRIX_COUNT][16];
	for (size_t i = 0; i < COCKPIT_BOTTOM_MATRIX_COUNT; ++i) {
		std::memcpy(backup[i], matrix(i), sizeof(backup[i]));
		ShrinkHudMatrix(matrix(i), layout.bottom);
	}
	GuardedDraw(original, cockpit, "cockpit bottom");
	for (size_t i = 0; i < COCKPIT_BOTTOM_MATRIX_COUNT; ++i) {
		std::memcpy(matrix(i), backup[i], sizeof(backup[i]));
	}
}

// DrawCUIDStylishCombo_sub_1402BB160: the vanilla style meter. It draws the active rank model with the matrix at combo +0x3CD0
// (CDraw vtbl +0x198) and a bar as a 2D rect in absolute HUD coordinates (DrawHudRect_sub_1402E32F0: x / y in the 640 x 448
// HUD space, y downward). That matrix is the identity and the model ignores it: the loader attached a motion to each rank
// model (CDraw vtbl +0x150, sub_14008A000), which sets the byte +0x19, and CDraw_DrawWithMatrix then only updates the
// joints from the animation (the position lives in the joints). Shifting the scene HUD origin does not move the models
// either. So while the combo draws, the active rank model's +0x19 is cleared (the matrix path: CDraw_SetMatrix, then the
// same joint update) and the matrix is set to scale about the pivot and shift (x / y in HUD units, y downward). The same
// shift and scale are applied to the rect (the bar scales about its left centre). One draw per splitscreen view, or one per
// player on the shared / single player camera (the style meter is shared, every copy shows the same).
using DrawHudRectFn = __int64(__fastcall*)(__int64, int16_t, int16_t, int, uint16_t, uint16_t, void*, int);
std::unique_ptr<Utility::Detour_t> s_drawHudRectHook;
constexpr uintptr_t DRAW_HUD_RECT_OFFSET = 0x2E32F0; // DrawHudRect_sub_1402E32F0
constexpr size_t COMBO_DRAW_MATRIX = 0x3CD0;
bool s_inStyleCombo = false;
VanillaStyleShift s_comboShift = {};

__int64 __fastcall DrawHudRectHook(__int64 list, int16_t x, int16_t y, int colour, uint16_t w, uint16_t h, void* colours, int count) {
	const auto original = s_drawHudRectHook->GetTrampoline<DrawHudRectFn>();
	if (s_inStyleCombo) {
		static uint32 s_logged = 0;
		if (s_logged++ < 6) {
			Log("StyleCombo bar rect: x %d y %d w %u h %u, shift %.1f %.1f scale %.3f", x, y, w, h, s_comboShift.x, s_comboShift.y, s_comboShift.scale);
		}
		const float s = s_comboShift.scale;
		const int newW = (std::max)(1, static_cast<int>(std::lround(w * s)));
		const int newH = (std::max)(1, static_cast<int>(std::lround(h * s)));
		x = static_cast<int16_t>(x + static_cast<int>(std::lround(s_comboShift.x + s_comboShift.barX)));
		y = static_cast<int16_t>(y + static_cast<int>(std::lround(s_comboShift.y + s_comboShift.barY)) + (static_cast<int>(h) - newH) / 2);
		w = static_cast<uint16_t>(newW);
		h = static_cast<uint16_t>(newH);
	}
	return original(list, x, y, colour, w, h, colours, count);
}

void DrawStyleComboShifted(CockpitFn original, uint8_t* combo, uint8_t* scene, const VanillaStyleShift& shift) {
	float* matrix = reinterpret_cast<float*>(combo + COMBO_DRAW_MATRIX);
	float saved[16];
	std::memcpy(saved, matrix, sizeof(saved));

	// Only a meter that is moved or scaled leaves the game's own path.
	uint8_t* model = nullptr;
	uint8_t savedFlag = 0;
	if (shift.x != 0.0f || shift.y != 0.0f || shift.scale != 1.0f) {
		const int rank = *reinterpret_cast<int*>(combo + 0x3D18);
		if (rank >= 1 && rank <= 7) {
			model = combo + 0x6C0 + 0x780 * static_cast<size_t>(rank - 1);
			savedFlag = model[0x19];
		}
	}

	static uint32 s_logged = 0;
	if (s_logged++ < 4) {
		Log("StyleCombo matrix: diag %.3f %.3f %.3f translation %.1f %.1f %.1f, scene origin %.1f %.1f, shift %.1f %.1f scale %.3f, model +0x19 %d", matrix[0],
			matrix[5], matrix[10], matrix[12], matrix[13], matrix[14], *reinterpret_cast<float*>(scene + 0x170),
			*reinterpret_cast<float*>(scene + 0x174), shift.x, shift.y, shift.scale, model ? savedFlag : -1);
	}

	if (model) {
		if (!s_vanillaStyleAnimate) {
			model[0x19] = 0;
		}
		const float s = shift.scale;
		for (int row = 0; row < 3; ++row) {
			matrix[row * 4 + 0] *= s;
			matrix[row * 4 + 1] *= s;
		}
		matrix[12] = matrix[12] * s + (1.0f - s) * s_vanillaStylePivotX + shift.x;
		matrix[13] = matrix[13] * s + (1.0f - s) * s_vanillaStylePivotY - shift.y;
		if (s_vanillaStyleAnimate && savedFlag) {
			// Re-poses the animated model with the shifted matrix (CDraw vtbl +0x190, the call the update makes with the
			// identity), the flagged draw then keeps that pose instead of resetting it to the rest pose.
			using PoseFn = void(__fastcall*)(uint8_t*, float*);
			(*reinterpret_cast<PoseFn*>(*reinterpret_cast<uint8_t**>(model) + 0x190))(model, matrix);
		}
	}

	s_comboShift = shift;
	s_inStyleCombo = true;
	original(combo);
	s_inStyleCombo = false;
	if (model) {
		model[0x19] = savedFlag;
	}
	std::memcpy(matrix, saved, sizeof(saved));
}

bool ComboSeatsWanted();

void __fastcall StyleComboDrawHook(uint8_t* combo) {
	const auto original = s_styleComboDrawHook->GetTrampoline<CockpitFn>();
	const int views = CrimsonSplitscreen::ViewCount();
	const int player = CrimsonSplitscreen::CameraPlayer();
	auto* scene = *reinterpret_cast<uint8_t**>(appBaseAddr + SCENE_STATE_CURSOR_OFFSET);
	if (!scene) {
		original(combo);
		return;
	}
	if (views >= 2) {
		if (player < 0 || player >= views) {
			original(combo);
			return;
		}
		DrawStyleComboShifted(original, combo, scene, (views == 2) ? s_vanillaStyleHalves[player] : s_vanillaStyleQuads[player]);
		return;
	}
	const int seats = HUD_SeatCount();
	if (seats >= 2) {
		// One draw per player, each with its own images (a model drawn twice in a frame loops the display list forever,
		// see ActivateComboSeat).
		const bool perPlayer = ComboSeatsWanted();
		const int draws = perPlayer ? seats : 1;
		for (int seat = 0; seat < draws; ++seat) {
			if (seat > 0 && !ActivateComboSeat(combo, seat)) {
				continue;
			}
			DrawStyleComboShifted(original, combo, scene, s_vanillaStyleSeats[seat]);
		}
		if (draws > 1) {
			ActivateComboSeat(combo, 0);
		}
		return;
	}
	original(combo);
}

// CUIDStylishCombo update (vtable +0x08, sub_1402BAC10): animates the live rank model once per tick. With one vanilla meter
// per player each player's combo is updated too (same style data, own images and state, see ActivateComboSeat).
constexpr uintptr_t STYLE_COMBO_UPDATE_OFFSET = 0x2BAC10;
std::unique_ptr<Utility::Detour_t> s_styleComboUpdateHook;

bool ComboSeatsWanted() {
	return s_vanillaStyleAllSeats && HUD_SeatCount() >= 2 && !activeCrimsonConfig.CrimsonHudAddons.styleRanksMeter &&
		!activeCrimsonConfig.HudOptions.hideStyleMeter;
}

void __fastcall StyleComboUpdateHook(uint8_t* combo) {
	const auto original = s_styleComboUpdateHook->GetTrampoline<CockpitFn>();
	if (!combo) {
		original(combo);
		return;
	}
	if (!ComboSeatsWanted()) {
		ReleaseComboSeats();
		original(combo);
		return;
	}
	const int seats = HUD_SeatCount();
	ActivateComboSeat(combo, 0);
	original(combo);
	for (int seat = 1; seat < seats; ++seat) {
		if (ActivateComboSeat(combo, seat)) {
			original(combo);
		}
	}
	ActivateComboSeat(combo, 0);
}

} // namespace

void HUD_InitAdjustmentHooks() {
	HUD_InitHeapWatch();
	s_styleComboUpdateHook = std::make_unique<Utility::Detour_t>(
		(uintptr_t)appBaseAddr + STYLE_COMBO_UPDATE_OFFSET, &StyleComboUpdateHook, 0, "UpdateCUIDStylishCombo_sub_1402BAC10");
	s_styleComboUpdateHook->Toggle(true);
	s_drawHudRectHook = std::make_unique<Utility::Detour_t>(
		(uintptr_t)appBaseAddr + DRAW_HUD_RECT_OFFSET, &DrawHudRectHook, 0, "DrawHudRect_sub_1402E32F0");
	s_drawHudRectHook->Toggle(true);
	s_styleComboDrawHook = std::make_unique<Utility::Detour_t>(
		(uintptr_t)appBaseAddr + STYLE_COMBO_DRAW_OFFSET, &StyleComboDrawHook, 0, "DrawCUIDStylishCombo_sub_1402BB160");
	s_styleComboDrawHook->Toggle(true);
	s_lockOnDrawHook = std::make_unique<Utility::Detour_t>(
		(uintptr_t)appBaseAddr + LOCKON_DRAW_OFFSET, &LockOnDrawHook, 0, "DrawCUIDLockOnCursor_sub_140297540");
	s_lockOnDrawHook->Toggle(true);

	s_cockpitTopDrawHook = std::make_unique<Utility::Detour_t>(
		(uintptr_t)appBaseAddr + COCKPIT_TOP_DRAW_OFFSET, &CockpitTopDrawHook, 0, "DrawCUIDCockpit00_sub_14027F1F0");
	s_cockpitTopDrawHook->Toggle(true);

	s_cockpitBottomDrawHook = std::make_unique<Utility::Detour_t>(
		(uintptr_t)appBaseAddr + COCKPIT_BOTTOM_DRAW_OFFSET, &CockpitBottomDrawHook, 0, "DrawCUIDCockpit02_sub_140281120");
	s_cockpitBottomDrawHook->Toggle(true);
}

int HUD_SeatCount() {
	if (!activeConfig.Actor.enable || activeConfig.Actor.playerCount < 2 || g_multiplayerCameraMode == MULTIPLAYERCAMERA::SPLITSCREEN) {
		return 0;
	}
	if (!InGame() || g_inGameCutscene || InCutscene()) {
		return 0;
	}
	return (activeConfig.Actor.playerCount > PLAYER_COUNT) ? PLAYER_COUNT : activeConfig.Actor.playerCount;
}

// Seat version of the addon transform, in screen pixels: P' = S * (P - corner) + corner + move + nudge, the pixel
// counterpart of SeatGroup (same corner, move and nudge, converted with the calibrated pixels per HUD unit).
static bool SeatAddonTransform(int seat, float screenWidth, float screenHeight, float& posScale, float& sizeScale, float& offsetX, float& offsetY) {
	if (seat >= HUD_SeatCount()) {
		return false;
	}
	const HudCalibration& c = s_hudCalibration;
	const HudFrameInfo frame = ReadHudFrameInfo(screenWidth, screenHeight);
	const SeatPlacement p = ComputeSeatPlacement(seat, frame);
	const HudGeneral& g = s_seatGeneral[seat];
	const float unit = screenHeight / HUD_HEIGHT;
	const float kx = frame.unitX * c.unitX * unit;
	const float ky = frame.unitY * c.unitY * unit;
	const float pivotPxX = screenWidth * 0.5f + c.pivotX * unit;
	const float pivotPxY = screenHeight * 0.5f + c.pivotY * unit;
	const float cornerPxX = pivotPxX + p.cornerX * kx;
	const float cornerPxY = pivotPxY - p.cornerY * ky;

	posScale  = g.scale;
	sizeScale = g.scale;
	offsetX   = (1.0f - g.scale) * cornerPxX + p.moveX * kx + g.offsetX * unit;
	offsetY   = (1.0f - g.scale) * cornerPxY - p.moveY * ky + g.offsetY * unit;
	return true;
}

bool HUD_WheelPlacement(int player, bool melee, bool force1P, float screenWidth, float screenHeight, float& x, float& y, float& size) {
	if (player < 0 || player >= SEAT_MAX || !activeConfig.Actor.enable || screenWidth <= 0.0f || screenHeight <= 0.0f) {
		return false;
	}
	const int count = (activeConfig.Actor.playerCount > PLAYER_COUNT) ? PLAYER_COUNT : activeConfig.Actor.playerCount;

	// Same view layout as CrimsonSplitscreen::ViewRect, without its settle delay so the wheels do not jump meanwhile.
	if (CrimsonSplitscreen::Enabled()) {
		if (player >= count) {
			return false;
		}
		float minX, minY, maxX, maxY;
		if (count == 2) {
			minX = static_cast<float>(player) * screenWidth * 0.5f;
			minY = 0.0f;
			maxY = screenHeight;
		} else {
			minX = static_cast<float>(player & 1) * screenWidth * 0.5f;
			minY = static_cast<float>(player >> 1) * screenHeight * 0.5f;
			maxY = minY + screenHeight * 0.5f;
		}
		maxX = minX + screenWidth * 0.5f;
		size = s_splitWheelScale * (maxY - minY);
		x    = melee ? maxX - size : minX;
		y    = maxY - size;
		return true;
	}

	if (count >= 2 || (force1P && player == 0)) {
		const WheelSeat& seat = s_wheelSeats[player];
		size = s_seatWheelScale * screenHeight;
		x    = (melee ? seat.meleeX : seat.rangedX) * screenWidth;
		y    = (melee ? seat.meleeY : seat.rangedY) * screenHeight;
		return true;
	}
	return false;
}

void HUD_SplitWheelControls() {
	ImGui::DragFloat("weapon wheel size", &s_splitWheelScale, 0.002f, 0.05f, 0.8f, "%.3f of view height");
	ImGui::SameLine();
	if (ImGui::Button("Reset##splitWheel")) {
		s_splitWheelScale = HUD_WHEEL_SPLIT_SCALE;
	}
	ImGui::SameLine();
	if (ImGui::Button("Copy##splitWheel")) {
		char text[96];
		snprintf(text, sizeof(text), "constexpr float HUD_WHEEL_SPLIT_SCALE = %.3ff;\n", s_splitWheelScale);
		ImGui::SetClipboardText(text);
	}
}

bool HUD_RedOrbCrimsonPlacement(float& scale, float& offsetXPx, float& offsetYPx, float screenHeight) {
	const float unit = screenHeight / HUD_HEIGHT;
	scale     = s_redOrbCrimson.scale;
	offsetXPx = s_redOrbCrimson.offsetX * unit;
	offsetYPx = s_redOrbCrimson.offsetY * unit;
	return true;
}

// Both red orb counters of the "Red Orb Counter" debug section: vanilla (cockpit group, single player and shared /
// single player camera) and Crimson (ImGui counter, every mode; in splitscreen it also stands in for the vanilla one).
void HUD_RedOrbControls() {
	if (!ImGui::CollapsingHeader("Red Orb Counter (one for all players)")) {
		return;
	}
	ImGui::TextWrapped("Vanilla: the game's counter, in single player and on the shared / single player camera (hidden in splitscreen, "
		"where the Crimson counter with the vanilla orb stands in for it). Crimson: the overlay counter. Scale, x and y as in the "
		"general controls (HUD units, y downward).");
	ImGui::PushID("redOrbVanilla");
	ImGui::Text("Vanilla");
	HUD_GeneralControls(s_redOrbVanilla, HUD_RED_ORB_VANILLA, HUD_PRESETS_RED_ORB, std::size(HUD_PRESETS_RED_ORB));
	ImGui::PopID();
	ImGui::PushID("redOrbCrimson");
	ImGui::Text("Crimson");
	HUD_GeneralControls(s_redOrbCrimson, HUD_RED_ORB_CRIMSON, HUD_PRESETS_RED_ORB, std::size(HUD_PRESETS_RED_ORB));
	ImGui::PopID();
	if (ImGui::Button("Copy red orb counters as code")) {
		char text[256];
		snprintf(text, sizeof(text),
			"constexpr HudGeneral HUD_RED_ORB_VANILLA = { %.3ff, %.1ff, %.1ff };\nconstexpr HudGeneral HUD_RED_ORB_CRIMSON = { %.3ff, %.1ff, %.1ff };\n",
			s_redOrbVanilla.scale, s_redOrbVanilla.offsetX, s_redOrbVanilla.offsetY, s_redOrbCrimson.scale, s_redOrbCrimson.offsetX,
			s_redOrbCrimson.offsetY);
		ImGui::SetClipboardText(text);
	}
}

bool HUD_StyleMeterTransform(int view, float screenWidth, float screenHeight, float& posScale, float& sizeScale, float& offsetX, float& offsetY) {
	if (screenWidth <= 0.0f || screenHeight <= 0.0f) {
		return false;
	}
	const HudGeneral* g = &s_styleMeterSingle;
	float areaRight = screenWidth;
	float areaTop = 0.0f;
	float originX = 0.0f;
	float originY = 0.0f;
	float viewScale = 1.0f;

	if (SplitscreenActive()) {
		const int count = CrimsonSplitscreen::ViewCount();
		float minX, minY, maxX, maxY;
		if (view < 0 || view >= count || !CrimsonSplitscreen::ViewRect(view, screenWidth, screenHeight, minX, minY, maxX, maxY)) {
			return false;
		}
		g = (count == 2) ? &s_styleMeterHalves[view] : &s_styleMeterQuads[view];
		areaRight = maxX;
		areaTop = minY;
		// Frame pixels -> the view (crop for halves, 0.5 for downscaled quadrants), as the cockpit addons do.
		float unitPointX = 1.0f, unitPointY = 1.0f;
		CrimsonSplitscreen::MapPoint(view, screenWidth, screenHeight, originX, originY);
		CrimsonSplitscreen::MapPoint(view, screenWidth, screenHeight, unitPointX, unitPointY);
		viewScale = unitPointX - originX;
	} else if (view >= 0) {
		if (view >= HUD_SeatCount()) {
			return false;
		}
		g = &s_styleMeterSeats[view];
	}

	// P' = S * (P_view - A) + A + G * unit with P_view = viewScale * P + origin and A the top-right corner of the area.
	const float unit = screenHeight / HUD_HEIGHT * viewScale;
	posScale  = g->scale * viewScale;
	sizeScale = g->scale * viewScale;
	offsetX   = g->scale * (originX - areaRight) + areaRight + g->offsetX * unit;
	offsetY   = g->scale * (originY - areaTop) + areaTop + g->offsetY * unit;
	return true;
}

void HUD_StyleMeterControls(int kind) {
	if (!ImGui::CollapsingHeader("Crimson style meter (one per player)")) {
		return;
	}
	ImGui::TextWrapped("The rank meter and the Stylish PTS counter move as one, scaled about the top-right corner of their screen (or "
		"splitscreen view), x / y in HUD units (y downward). Not anchored to the cockpit.");

	HudGeneral* entries = &s_styleMeterSingle;
	const HudGeneral* defaults = &HUD_STYLE_METER_SINGLE;
	int count = 1;
	const char* codeName = "HUD_STYLE_METER_SINGLE";
	switch (kind) {
	case HUD_STYLE_SEATS:
		entries = s_styleMeterSeats; defaults = HUD_STYLE_METER_SEATS; count = SEAT_MAX; codeName = "HUD_STYLE_METER_SEATS[SEAT_MAX]";
		break;
	case HUD_STYLE_HALVES:
		entries = s_styleMeterHalves; defaults = HUD_STYLE_METER_HALVES; count = 2; codeName = "HUD_STYLE_METER_HALVES[2]";
		break;
	case HUD_STYLE_QUADS:
		entries = s_styleMeterQuads; defaults = HUD_STYLE_METER_QUADS; count = SEAT_MAX; codeName = "HUD_STYLE_METER_QUADS[SEAT_MAX]";
		break;
	}

	for (int i = 0; i < count; ++i) {
		ImGui::PushID(2000 + kind * 10 + i);
		if (count > 1) {
			ImGui::Text("%dP", i + 1);
		}
		HUD_GeneralControls(entries[i], defaults[i], HUD_PRESETS_STYLE_METER, std::size(HUD_PRESETS_STYLE_METER));
		ImGui::PopID();
	}

	if (ImGui::Button("Copy style meters as code")) {
		char text[512];
		int n = snprintf(text, sizeof(text), "constexpr HudGeneral %s = {\n", codeName);
		for (int i = 0; i < count && n > 0 && n < static_cast<int>(sizeof(text)); ++i) {
			n += snprintf(text + n, sizeof(text) - n, "\t{ %.3ff, %.1ff, %.1ff },\n", entries[i].scale, entries[i].offsetX, entries[i].offsetY);
		}
		if (n > 0 && n < static_cast<int>(sizeof(text)) - 4) {
			snprintf(text + n, sizeof(text) - n, "};\n");
		}
		ImGui::SetClipboardText(text);
	}
}

void HUD_VanillaStyleMeterControls(int kind) {
	if (!ImGui::CollapsingHeader("Vanilla style meter (per view / player)")) {
		return;
	}
	ImGui::TextWrapped("The game's own style meter, drawn at the right edge of the frame. x / y shift it (models and bar) in HUD units "
		"(negative x moves it left, y grows downward), scale scales it. One copy per view or player, all showing the shared style meter. "
		"Only visible while the Crimson rank meter is off.");
	VanillaStyleShift* shifts = s_vanillaStyleSeats;
	const VanillaStyleShift* defaults = HUD_VANILLA_STYLE_SEATS;
	int count = SEAT_MAX;
	const char* codeName = "HUD_VANILLA_STYLE_SEATS[SEAT_MAX]";
	if (kind == HUD_STYLE_HALVES) {
		shifts = s_vanillaStyleHalves; defaults = HUD_VANILLA_STYLE_HALVES; count = 2; codeName = "HUD_VANILLA_STYLE_HALVES[2]";
	} else if (kind == HUD_STYLE_QUADS) {
		shifts = s_vanillaStyleQuads; defaults = HUD_VANILLA_STYLE_QUADS; codeName = "HUD_VANILLA_STYLE_QUADS[SEAT_MAX]";
	}
	for (int i = 0; i < count; ++i) {
		ImGui::PushID(3000 + kind * 10 + i);
		ImGui::Text("%dP", i + 1);
		ImGui::DragFloat("x", &shifts[i].x, 0.5f, -800.0f, 800.0f, "%.1f");
		ImGui::DragFloat("y", &shifts[i].y, 0.5f, -800.0f, 800.0f, "%.1f");
		ImGui::DragFloat("scale", &shifts[i].scale, 0.005f, 0.1f, 3.0f, "%.3f");
		ImGui::DragFloat("charge bar x", &shifts[i].barX, 0.5f, -800.0f, 800.0f, "%.1f");
		ImGui::DragFloat("charge bar y", &shifts[i].barY, 0.5f, -800.0f, 800.0f, "%.1f");
		ImGui::PopID();
	}
	if (kind == HUD_STYLE_SEATS) {
		ImGui::Checkbox("Draw one for every player", &s_vanillaStyleAllSeats);
		ImGui::TextWrapped("Off: a single vanilla meter at the 1P position.");
	}
	ImGui::Checkbox("Play rank animation while moved / scaled", &s_vanillaStyleAnimate);
	if (ImGui::Button("Reset vanilla style meters")) {
		for (int i = 0; i < count; ++i) {
			shifts[i] = defaults[i];
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Copy vanilla style meters as code")) {
		char text[768];
		int n = snprintf(text, sizeof(text), "constexpr VanillaStyleShift %s = {", codeName);
		for (int i = 0; i < count && n > 0 && n < static_cast<int>(sizeof(text)); ++i) {
			n += snprintf(text + n, sizeof(text) - n, " { %.1ff, %.1ff, %.3ff, %.1ff, %.1ff },", shifts[i].x, shifts[i].y, shifts[i].scale,
				shifts[i].barX, shifts[i].barY);
		}
		if (n > 0 && n < static_cast<int>(sizeof(text)) - 4) {
			snprintf(text + n, sizeof(text) - n, " };\n");
		}
		ImGui::SetClipboardText(text);
	}
}

bool HUD_AddonTransform(int view, float screenWidth, float screenHeight, float& posScale, float& sizeScale, float& offsetX, float& offsetY) {
	if (screenWidth <= 0.0f || screenHeight <= 0.0f) {
		return false;
	}
	if (view >= 0 && !SplitscreenActive()) {
		return SeatAddonTransform(view, screenWidth, screenHeight, posScale, sizeScale, offsetX, offsetY);
	}
	HudSplitLayout layout;
	if (view < 0 ? (SplitscreenActive() || !s_singleHudSyncAddons || !SinglePlayerHudTouched())
	             : (!CrimsonSplitscreen::HudLayoutInUse(layout) || view >= CrimsonSplitscreen::ViewCount())) {
		return false;
	}

	const HudCalibration& c = s_hudCalibration;
	const float unit = screenHeight / HUD_HEIGHT;
	const float pivotPxX = screenWidth * 0.5f + c.pivotX * unit;
	const float pivotPxY = screenHeight * 0.5f + c.pivotY * unit;

	// The layout base (identity in single player), then the general adjustment, on the frame in pixels:
	// P' = S * (basePos * P + (1 - basePos) * pivot + baseOffset) + (1 - S) * pivot + G * unit.
	// The matching matrix transform is ComposeTopLeftGroup; the base offset is in matrix units (y up).
	const HudFrameInfo frame = ReadHudFrameInfo(screenWidth, screenHeight);
	const HudGroupTuning& base = (view < 0) ? HUD_GROUP_IDENTITY : layout.topLeft;
	const HudGeneral& g = (view < 0) ? SinglePlayerGeneral() : layout.general;
	const float baseOffsetPxX = base.offsetX * frame.unitX * c.unitX * unit;
	const float baseOffsetPxY = -base.offsetY * frame.unitY * c.unitY * unit;
	const float framePosScale = g.scale * base.posScale;
	const float frameSizeScale = g.scale * base.sizeScale;
	const float frameOffsetX = (1.0f - framePosScale) * pivotPxX + g.scale * baseOffsetPxX + g.offsetX * unit;
	const float frameOffsetY = (1.0f - framePosScale) * pivotPxY + g.scale * baseOffsetPxY + g.offsetY * unit;

	if (view < 0) {
		posScale = framePosScale;
		sizeScale = frameSizeScale;
		offsetX = frameOffsetX;
		offsetY = frameOffsetY;
		return true;
	}

	// Frame pixels -> the view (crop for halves, 0.5 for downscaled quadrants).
	float originX = 0.0f, originY = 0.0f, unitPointX = 1.0f, unitPointY = 1.0f;
	CrimsonSplitscreen::MapPoint(view, screenWidth, screenHeight, originX, originY);
	CrimsonSplitscreen::MapPoint(view, screenWidth, screenHeight, unitPointX, unitPointY);
	const float viewScale = unitPointX - originX;

	posScale = viewScale * framePosScale;
	sizeScale = viewScale * frameSizeScale;
	offsetX = viewScale * frameOffsetX + originX;
	offsetY = viewScale * frameOffsetY + originY;
	return true;
}

bool HUD_PlayerTagAnchor(int player, float screenWidth, float screenHeight, float& x, float& y, float& scale, bool& alignRight) {
	alignRight = false;
	int view = -1;
	if (SplitscreenActive()) {
		if (player >= CrimsonSplitscreen::ViewCount()) {
			return false;
		}
		view = player;
	} else if (HUD_SeatCount() >= 2) {
		if (player >= HUD_SeatCount()) {
			return false;
		}
		view = player;
	} else if (player != 0) {
		return false;
	}

	// Right of the cockpit group at its original top-left place, for every seat; the addon transform then moves it together
	// with the group (it scales about the same corner as the group, so the tag keeps its place relative to the cockpit).
	const float baseX = screenWidth * 0.3272f;
	const float baseY = screenHeight * 0.015f;
	float posScale = 1.0f, sizeScale = 1.0f, offsetX = 0.0f, offsetY = 0.0f;
	HUD_AddonTransform(view, screenWidth, screenHeight, posScale, sizeScale, offsetX, offsetY);
	x = baseX * posScale + offsetX;
	y = baseY * posScale + offsetY;
	scale = sizeScale;
	return true;
}

// Scale / x / y, preset picker, reset and copy: the same controls in the single player and splitscreen windows.
void HUD_GeneralControls(HudGeneral& general, const HudGeneral& defaults, const HudPreset* presets, size_t presetCount) {
	ImGui::DragFloat("scale", &general.scale, 0.005f, 0.2f, 2.0f, "%.3f");
	ImGui::DragFloat("x", &general.offsetX, 0.5f, -800.0f, 800.0f, "%.1f");
	ImGui::DragFloat("y", &general.offsetY, 0.5f, -800.0f, 800.0f, "%.1f");

	if (ImGui::BeginCombo("Preset", "Apply...")) {
		for (size_t i = 0; i < presetCount; ++i) {
			if (ImGui::Selectable(presets[i].name)) {
				general = presets[i].general;
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::Button("Reset")) {
		general = defaults;
	}
	ImGui::SameLine();
	if (ImGui::Button("Copy general as code")) {
		char text[128];
		snprintf(text, sizeof(text), "{ \"Name\", { %.3ff, %.1ff, %.1ff } },\n", general.scale, general.offsetX, general.offsetY);
		ImGui::SetClipboardText(text);
	}
}

// Draws the pivot the addons scale around (once per view while splitscreen runs).
void HUD_DrawPivotMarker() {
	if (!s_hudShowPivot) {
		return;
	}
	const ImVec2 size = ImGui::GetIO().DisplaySize;
	const float unit = size.y / HUD_HEIGHT;
	const float pivotX = size.x * 0.5f + s_hudCalibration.pivotX * unit;
	const float pivotY = size.y * 0.5f + s_hudCalibration.pivotY * unit;
	auto* draw = ImGui::GetForegroundDrawList();
	const ImU32 colour = IM_COL32(255, 0, 255, 220);
	auto cross = [draw, colour](float x, float y) {
		draw->AddLine(ImVec2(x - 12.0f, y), ImVec2(x + 12.0f, y), colour, 2.0f);
		draw->AddLine(ImVec2(x, y - 12.0f), ImVec2(x, y + 12.0f), colour, 2.0f);
	};
	const int views = CrimsonSplitscreen::ViewCount();
	if (views < 2) {
		cross(pivotX, pivotY);
		return;
	}
	for (int view = 0; view < views; ++view) {
		float x = pivotX;
		float y = pivotY;
		CrimsonSplitscreen::MapPoint(view, size.x, size.y, x, y);
		cross(x, y);
	}
}

// Shared by the single player and splitscreen windows: one calibration drives both.
void HUD_CalibrationControls() {
	if (!ImGui::CollapsingHeader("Calibration")) {
		return;
	}
	HudCalibration& calibration = s_hudCalibration;
	const HudFrameInfo frame = ReadHudFrameInfo(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
	if (frame.valid) {
		ImGui::Text("Game HUD frame %dx%d%s -> unit %.3f x %.3f", frame.frameWidth, frame.frameHeight,
			frame.widescreen ? " (widescreen x1.3)" : "", frame.unitX, frame.unitY);
	} else {
		ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "Game HUD frame not available, unit 1.0 assumed.");
	}
	ImGui::TextWrapped("The unit (pixels per HUD unit) is derived from the game HUD frame; unitX/unitY only trim it (1.0 = as derived). "
		"With scale 1, move x/y: if the addons run ahead of the game HUD raise the trim, if they lag lower it. "
		"With x/y 0, change scale: if the game HUD shrinks faster or slower than the addons change scaleRate (above 1.0 = game HUD "
		"shrinks faster), then if they still slide apart move pivotX/pivotY until they shrink toward the same point "
		"(the marker shows the pivot the addons use).");
	ImGui::DragFloat("unitX", &calibration.unitX, 0.001f, 0.2f, 3.0f, "%.3f");
	ImGui::DragFloat("unitY", &calibration.unitY, 0.001f, 0.2f, 3.0f, "%.3f");
	ImGui::DragFloat("scaleRate", &calibration.scaleRate, 0.001f, 0.2f, 3.0f, "%.3f");
	ImGui::DragFloat("pivotX", &calibration.pivotX, 0.5f, -1200.0f, 1200.0f, "%.1f");
	ImGui::DragFloat("pivotY", &calibration.pivotY, 0.5f, -1200.0f, 1200.0f, "%.1f");
	ImGui::Checkbox("Show pivot marker", &s_hudShowPivot);
	if (ImGui::Button("Reset calibration")) {
		calibration = HUD_CALIBRATION;
	}
	ImGui::SameLine();
	if (ImGui::Button("Copy calibration as code")) {
		char text[192];
		snprintf(text, sizeof(text), "constexpr HudCalibration HUD_CALIBRATION = { %.3ff, %.3ff, %.1ff, %.1ff, %.3ff };\n",
			calibration.unitX, calibration.unitY, calibration.pivotX, calibration.pivotY, calibration.scaleRate);
		ImGui::SetClipboardText(text);
	}
}

void HUD_SinglePlayerWindow(bool* open) {
	ImGui::SetNextWindowSize(ImVec2(440, 0), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Single Player HUD", open)) {
		ImGui::End();
		return;
	}

	if (SplitscreenActive()) {
		ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "Splitscreen is running: use the Splitscreen HUD window. These values are ignored.");
	}
	if (SmallerSinglePlayerHud()) {
		ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "Interface > HUD Scale is Smaller: its own values are used (Default uses the ones below).");
	}
	ImGui::TextWrapped("General: moves and scales the top-left cockpit group and the style addons as one (x/y in HUD units, %.0f tall, "
		"y grows downward). The calibration is shared with the Splitscreen HUD window.", HUD_HEIGHT);

	ImGui::Checkbox("Sync style icons and names", &s_singleHudSyncAddons);
	HUD_GeneralControls(s_singleHudGeneral, HUD_GENERAL_SINGLE, HUD_PRESETS_SINGLE, std::size(HUD_PRESETS_SINGLE));
	HUD_RedOrbControls();
	HUD_StyleMeterControls(HUD_STYLE_SINGLE);
	HUD_CalibrationControls();
	HUD_DrawPivotMarker();

	ImGui::End();
}

// "Multiplayer HUD (non Splitscreen)" debug window: where every player's cockpit goes on the shared / single player camera.
void HUD_MultiplayerWindow(bool* open) {
	ImGui::SetNextWindowSize(ImVec2(440, 0), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Multiplayer HUD (non Splitscreen)", open)) {
		ImGui::End();
		return;
	}

	const int seats = HUD_SeatCount();
	ImGui::TextWrapped("Every player's cockpit sits in a screen corner (1P top-left, 2P top-right, 3P bottom-left, 4P bottom-right) and "
		"is scaled around that corner; x / y nudge it from there (HUD units, %.0f tall, y grows downward). The player tags and the style "
		"addons follow their cockpit. Splitscreen has its own window; the calibration is shared. %s",
		HUD_HEIGHT, seats >= 2 ? "Running now." : "Not running (needs 2+ players on the shared or single player camera).");

	for (int seat = 0; seat < SEAT_MAX; ++seat) {
		ImGui::PushID(seat);
		ImGui::Separator();
		ImGui::Text("%dP", seat + 1);
		HUD_GeneralControls(s_seatGeneral[seat], HUD_SEAT_GENERAL[seat], HUD_PRESETS_SEAT, std::size(HUD_PRESETS_SEAT));
		ImGui::PopID();
	}

	ImGui::Separator();
	ImGui::TextWrapped("Cockpit group size and screen margins (HUD units). The size only places the far corner of the group: raise the "
		"margins if a cockpit touches the screen edge, change the width if the right cockpits are too far in or out.");
	ImGui::DragFloat("group width", &s_seatBounds.width, 0.5f, 50.0f, 500.0f, "%.1f");
	ImGui::DragFloat("group height", &s_seatBounds.height, 0.5f, 30.0f, 300.0f, "%.1f");
	ImGui::DragFloat("side margin", &s_seatBounds.margin, 0.5f, -300.0f, 300.0f, "%.1f");
	ImGui::DragFloat("top / bottom margin", &s_seatBounds.topMargin, 0.5f, -300.0f, 300.0f, "%.1f");
	if (ImGui::Button("Reset all")) {
		s_seatBounds = HUD_SEAT_BOUNDS;
		for (int seat = 0; seat < SEAT_MAX; ++seat) {
			s_seatGeneral[seat] = HUD_SEAT_GENERAL[seat];
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Copy all as code")) {
		char text[768];
		int n = snprintf(text, sizeof(text), "constexpr SeatBounds HUD_SEAT_BOUNDS = { %.1ff, %.1ff, %.1ff, %.1ff };\n"
			"constexpr HudGeneral HUD_SEAT_GENERAL[SEAT_MAX] = {\n", s_seatBounds.width, s_seatBounds.height, s_seatBounds.margin, s_seatBounds.topMargin);
		for (int seat = 0; seat < SEAT_MAX && n > 0 && n < static_cast<int>(sizeof(text)); ++seat) {
			n += snprintf(text + n, sizeof(text) - n, "\t{ %.3ff, %.1ff, %.1ff },\n", s_seatGeneral[seat].scale, s_seatGeneral[seat].offsetX,
				s_seatGeneral[seat].offsetY);
		}
		if (n > 0 && n < static_cast<int>(sizeof(text)) - 4) {
			snprintf(text + n, sizeof(text) - n, "};\n");
		}
		ImGui::SetClipboardText(text);
	}

	HUD_RedOrbControls();
	HUD_StyleMeterControls(HUD_STYLE_SEATS);
	HUD_VanillaStyleMeterControls(HUD_STYLE_SEATS);

	ImGui::Separator();
	ImGui::TextWrapped("Weapon wheels: one size for every player and a top-left position per wheel (fraction of the screen). "
		"Splitscreen puts them in the bottom corners of each view (size in the Splitscreen HUD window); the Weapon Wheel scale option "
		"only applies to single player.");
	ImGui::DragFloat("wheel size", &s_seatWheelScale, 0.002f, 0.05f, 0.8f, "%.3f of screen height");
	for (int seat = 0; seat < SEAT_MAX; ++seat) {
		ImGui::PushID(1000 + seat);
		WheelSeat& wheel = s_wheelSeats[seat];
		ImGui::Text("%dP wheels", seat + 1);
		ImGui::DragFloat2("melee x, y", &wheel.meleeX, 0.002f, -0.5f, 1.5f, "%.4f");
		ImGui::DragFloat2("ranged x, y", &wheel.rangedX, 0.002f, -0.5f, 1.5f, "%.4f");
		ImGui::PopID();
	}
	if (ImGui::Button("Reset wheels")) {
		s_seatWheelScale = HUD_WHEEL_SEAT_SCALE;
		for (int seat = 0; seat < SEAT_MAX; ++seat) {
			s_wheelSeats[seat] = HUD_WHEEL_SEATS[seat];
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Copy wheels as code")) {
		char text[640];
		int n = snprintf(text, sizeof(text), "constexpr float HUD_WHEEL_SEAT_SCALE = %.3ff;\nconstexpr WheelSeat HUD_WHEEL_SEATS[SEAT_MAX] = {\n",
			s_seatWheelScale);
		for (int seat = 0; seat < SEAT_MAX && n > 0 && n < static_cast<int>(sizeof(text)); ++seat) {
			const WheelSeat& w = s_wheelSeats[seat];
			n += snprintf(text + n, sizeof(text) - n, "\t{ %.4ff, %.4ff, %.4ff, %.4ff },\n", w.meleeX, w.meleeY, w.rangedX, w.rangedY);
		}
		if (n > 0 && n < static_cast<int>(sizeof(text)) - 4) {
			snprintf(text + n, sizeof(text) - n, "};\n");
		}
		ImGui::SetClipboardText(text);
	}

	HUD_CalibrationControls();
	HUD_DrawPivotMarker();

	ImGui::End();
}

#pragma endregion

#ifdef __GARBAGE__
#endif

#pragma once

#include <cstddef>

#include "Core/DataTypes.hpp"

void HUD_UpdateStyleIcon(uint8 style, uint8 character);

void HUD_UpdateDevilTriggerGauge(uint8 character);

void HUD_UpdateDevilTriggerLightning(uint8 character);

void HUD_UpdateDevilTriggerExplosion(uint8 character);

bool HUD_UpdateWeaponIcon(uint8 index, uint8 weapon);

void HUD_UpdateHPBar(uint8 character);

void HUD_UpdateLockOn(uint8 character);

// Breadcrumb for the HUD model audit: which loader ran last.
void HUD_NoteReload(const char* what);

// Checks every HUD model's heap block (still allocated, command slots inside it, no overlap with another model) and logs the
// first problem of each kind. True when something is wrong. See Splitscreen-Plan.md, crash in the reticle draw.
bool HUD_AuditModels(const char* where);

// Reloads all HUD models on the next tick; set after a HUD draw crash was caught.
void HUD_RequestRepair();
void HUD_RunRepair();

// Per player images of the original HUD models the mod reloads (splitscreen). The returned flags say which groups
// are empty for that player and have to be loaded (the loaders are HUD_Update*), or that the game rebuilt the HUD
// objects so everything has to be loaded again.
constexpr unsigned HUD_MODELS_TOP_BLANK    = 1;
constexpr unsigned HUD_MODELS_BOTTOM_BLANK = 2;
constexpr unsigned HUD_MODELS_CURSOR_BLANK = 4;
constexpr unsigned HUD_MODELS_REBUILT      = 8;
unsigned HUD_ActivatePlayerModels(int player);
// Back to player 1's models only (splitscreen off); frees the other players' images.
unsigned HUD_ReleasePlayerModels();

// Bumped by every HUD_Update* loader so callers can tell when the icons changed under them.
extern uint32 g_hudIconSerial;

extern byte8* g_dynamicHUD_id100;
extern byte8* g_dynamicHUD_id100V;

void HUD_Init();
void HUD_ReloadIcons();
void ToggleHideMainHUD(bool enable);
void ToggleHideBossHUD(bool enable);

void ToggleForceVisibleHUD(bool enable);

// ---- Adjustments of the original game HUD (CUIDCockpit) ----
// The top-left group (HP / DT / style icon / flux) and the CrimsonHUD addons that sit on it move and scale as one.
// Scale / x / y ("general") are in HUD units (HUD_HEIGHT tall, y grows downward); the calibration is shared by the
// single player and splitscreen windows.

constexpr float HUD_HEIGHT = 448.0f;

// Matrix transform of a cockpit element group: position * posScale + offset, model scaled by sizeScale.
struct HudGroupTuning {
    float sizeScale;
    float posScale;
    float offsetX;
    float offsetY;
};

struct HudGeneral {
    float scale;
    float offsetX;
    float offsetY;
};

struct HudPreset {
    const char* name;
    HudGeneral general;
};

// The splitscreen layout in use: base groups plus the general adjustment applied on top of the top-left one.
struct HudSplitLayout {
    HudGroupTuning topLeft;
    HudGroupTuning topRight;
    HudGroupTuning bottom;
    HudGeneral general;
};

// Hooks the cockpit draws (CUIDCockpit00 / 02) to apply the adjustments.
void HUD_InitAdjustmentHooks();

// "Single Player HUD" debug window.
void HUD_SinglePlayerWindow(bool* open);

// Controls shared with the splitscreen window.
void HUD_GeneralControls(HudGeneral& general, const HudGeneral& defaults, const HudPreset* presets, size_t presetCount);
void HUD_CalibrationControls();
void HUD_DrawPivotMarker();

// Transform for the ImGui addons that sit on the top-left cockpit group, so they follow the game HUD:
// position' = position * posScale + offset, size' = size * sizeScale, on the final screen.
// view = splitscreen view (player index) or -1 for the whole screen (single player adjustment).
// False when no adjustment applies (untouched, syncing off, or the view does not exist).
bool HUD_AddonTransform(int view, float screenWidth, float screenHeight, float& posScale, float& sizeScale, float& offsetX, float& offsetY);
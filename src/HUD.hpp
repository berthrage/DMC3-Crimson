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
// Model groups HUD_ActivatePlayerModels can swap: the cockpit top (HP / DT / style icon), bottom (weapon icons) and the lock-on cursor.
constexpr unsigned HUD_GROUP_TOP    = 1;
constexpr unsigned HUD_GROUP_BOTTOM = 2;
constexpr unsigned HUD_GROUP_CURSOR = 4;
constexpr unsigned HUD_GROUP_ALL    = 7;
unsigned HUD_ActivatePlayerModels(int player, unsigned groups = HUD_GROUP_ALL);
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

// Watches the kernel heap for blocks shared with a live HUD model (logs HeapWatch lines).
void HUD_InitHeapWatch();

// "Single Player HUD" debug window.
void HUD_SinglePlayerWindow(bool* open);

// "Multiplayer HUD (non Splitscreen)" debug window: placement of every player's cockpit on the shared / single player camera.
void HUD_MultiplayerWindow(bool* open);

// Controls shared with the splitscreen window.
void HUD_GeneralControls(HudGeneral& general, const HudGeneral& defaults, const HudPreset* presets, size_t presetCount);
void HUD_CalibrationControls();
void HUD_DrawPivotMarker();

// Shared / single player camera multiplayer: one cockpit per player in the screen corners (player 1 top-left,
// 2 top-right, 3 bottom-left, 4 bottom-right). Number of cockpits drawn this frame, 0 when they are not in use
// (single player, splitscreen, or outside of normal play).
int HUD_SeatCount();

// Weapon wheels (2D overlay) on the multiplayer layouts: splitscreen puts every player's wheels in the bottom corners of
// their view (ranged left, melee right), the shared / single player camera uses the per seat positions of the
// Multiplayer HUD window. One scale each (fraction of the view / screen height); the Weapon Wheel scale option only
// applies to single player. force1P: use the multiplayer layout for player 1 even when playing alone.
// x / y / size in screen pixels; false when single player's own placement applies.
bool HUD_WheelPlacement(int player, bool melee, bool force1P, float screenWidth, float screenHeight, float& x, float& y, float& size);

// Crimson style meter (rank meter + Stylish PTS, they move as one): one per player, placed per player. The addons are
// drawn in screen pixels, scaled about the top-right corner of the area they belong to (the whole screen, or the
// splitscreen view) and moved from there; scale / x / y in HUD units (y downward), no anchoring to the cockpit.
// kind: HUD_STYLE_SINGLE (one player), HUD_STYLE_SEATS (shared / single player camera, one per player),
// HUD_STYLE_HALVES / HUD_STYLE_QUADS (splitscreen, one per view).
constexpr int HUD_STYLE_SINGLE = 0;
constexpr int HUD_STYLE_SEATS  = 1;
constexpr int HUD_STYLE_HALVES = 2;
constexpr int HUD_STYLE_QUADS  = 3;
void HUD_StyleMeterControls(int kind);
// view = splitscreen view / player index on the shared camera, -1 = single player. False when that view does not exist.
bool HUD_StyleMeterTransform(int view, float screenWidth, float screenHeight, float& posScale, float& sizeScale, float& offsetX, float& offsetY);

// Vanilla style meter shift per splitscreen view / multiplayer player (kind = HUD_STYLE_*), see HUD.cpp (StyleComboDrawHook).
void HUD_VanillaStyleMeterControls(int kind);

// Splitscreen weapon wheel size, as a fraction of the view height ("Splitscreen HUD" window).
void HUD_SplitWheelControls();

// Red orb counter placement (see HUD.cpp): the Crimson (ImGui) counter's scale and pixel offset; the debug section
// with both counters, shared by the Single Player, Multiplayer and Splitscreen HUD windows.
bool HUD_RedOrbCrimsonPlacement(float& scale, float& offsetXPx, float& offsetYPx, float screenHeight);
void HUD_RedOrbControls();

// Transform for the ImGui addons that sit on the top-left cockpit group, so they follow the game HUD:
// position' = position * posScale + offset, size' = size * sizeScale, on the final screen.
// view = splitscreen view or multiplayer cockpit (player index), or -1 for the whole screen (single player adjustment).
// False when no adjustment applies (untouched, syncing off, or the view does not exist).
bool HUD_AddonTransform(int view, float screenWidth, float screenHeight, float& posScale, float& sizeScale, float& offsetX, float& offsetY);

// Where a player's name tag goes beside their cockpit: top-left pixel (top-right when alignRight, the cockpit sits
// on the right edge) and the size scale. False when the player has no cockpit on screen.
bool HUD_PlayerTagAnchor(int player, float screenWidth, float screenHeight, float& x, float& y, float& scale, bool& alignRight);
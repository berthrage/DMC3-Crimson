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
// first problem of each kind. True when something is wrong. crash in the reticle draw.
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

// Creates / frees the per player vanilla style meters (one CUIDStylishCombo object each). Call every tick.
void HUD_SyncVanillaStyleMeters();

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

// Where the original lock-on cursor puts a world point (the target's lock-on position): the game's own projection into
// its HUD frame, then to screen pixels (the splitscreen view of `player` when it runs). False when it cannot be
// projected (frame state unavailable, behind the camera).
bool HUD_LockOnScreenPoint(int player, const float world[3], float screenWidth, float screenHeight, float& x, float& y);

// "Lock-On HUD" debug window (HUD_LockOnWindow): the original lock-on cursor next to the Crimson rings, each moved by
// hand. The ring fields are filled in by the Crimson draw (CrimsonHUD.cpp) for the readout and the ring boxes.
constexpr int HUD_LOCKON_DEBUG_PLAYERS = 4;
struct LockOnHudDebug {
	bool showVanillaReticle = false;    // keep the original reticle while the Crimson lock-on is on
	bool showProjectedPoint = true;     // cross on the point the rings are placed at, before the Crimson shift
	bool showRingBox = false;           // box of every ring window and the centre it puts the texture at
	bool centerRingsVertically = true;  // put the ring texture at 0.5 of the window instead of 0.47
	float vanillaX = 0.0f;              // HUD units, y down: moves the original reticle and the Artemis markers
	float vanillaY = 0.0f;
	float crimsonX = 2.0f;              // pixels at 1080p, y down: moves every Crimson ring
	float crimsonY = -3.3f;
	float ringWindowOverride = 0.0f;    // pixels at 1080p, 0 = the addon's own
	bool anchorOnReticle = true;        // anchor on the position the game projected for its reticle
	float frameScaleX = 1.0f;           // frame units -> pixels, on top of the frame width
	float frameScaleY = 1.0f;           // frame units -> pixels, on top of the frame height
	float frameOffsetX = 0.0f;          // frame units (y down), constant with distance
	float frameOffsetY = 0.0f;
	int player = 0;
	uint8_t* cursor = nullptr;          // last original cursor the update hooks saw
	float projectedX[HUD_LOCKON_DEBUG_PLAYERS] = {};
	float projectedY[HUD_LOCKON_DEBUG_PLAYERS] = {};
	bool ringValid[HUD_LOCKON_DEBUG_PLAYERS] = {};
	bool targetOnScreen[HUD_LOCKON_DEBUG_PLAYERS] = {}; // the game projected its reticle this tick (target on screen)
	float ringWindowPos[HUD_LOCKON_DEBUG_PLAYERS][2] = {};
	float ringWindowActual[HUD_LOCKON_DEBUG_PLAYERS][2] = {};
	float ringWindowRequested[HUD_LOCKON_DEBUG_PLAYERS][2] = {};
	float ringTexture[HUD_LOCKON_DEBUG_PLAYERS][2] = {};
	float ringCentre[HUD_LOCKON_DEBUG_PLAYERS][2] = {};
};
LockOnHudDebug& HUD_LockOnDebug();

// The original cursor projected its reticle for that player on the last tick, i.e. the locked target is on screen.
bool HUD_LockOnTargetOnScreen(int player);

// "Lock-On HUD" debug window: shows the original lock-on cursor next to the Crimson rings and moves each by hand.
void HUD_LockOnWindow(bool* open);

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
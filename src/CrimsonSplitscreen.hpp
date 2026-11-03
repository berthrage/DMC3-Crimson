#pragma once

#include <cstdint>

namespace CrimsonSplitscreen {
	void InitHooks();
	void HudTuningWindow(bool* open);
	// Single player counterpart of HudTuningWindow: tunes the top-left cockpit group.
	void SinglePlayerHudWindow(bool* open);
	// Transform for the ImGui addons that sit on the top-left cockpit group, so they follow the game HUD:
	// position' = position * posScale + offset, size' = size * sizeScale, all on the final screen.
	// view = splitscreen view (player index) or -1 for the whole screen (single player adjustment window).
	// False when no adjustment applies (untouched, syncing off, or the view does not exist).
	bool HudAddonTransform(int view, float screenWidth, float screenHeight, float& posScale, float& sizeScale, float& offsetX, float& offsetY);

	// True while the game is in a state splitscreen can run in (playing, no cutscene); ignores the settle delay.
	bool GateOpen();
	// Splitscreen is switched on (F6 with 2+ players), even while it is paused for a cutscene or load.
	bool Enabled();
	// Number of views the screen is split into (2 = halves, 3-4 = quadrants), 0 when off or settling.
	int ViewCount();
	// Camera object of playerIndex's view as of its last drawn frame, null if unavailable.
	const uint8_t* CameraCopy(int playerIndex);
	// Player whose camera the current game tick updates (0 when splitscreen is off), and that player's main actor
	// (null when off): camera input and reset belong to this player.
	int CameraPlayer();
	uint8_t* CameraActor();
	bool CameraEye(int playerIndex, float out[3]);
	// Maps a full-frame pixel to the view of playerIndex (frame crop -> view cell).
	void MapPoint(int playerIndex, float frameW, float frameH, float& x, float& y);
	// Screen rect of playerIndex's view; false when splitscreen is off or the player has no view.
	bool ViewRect(int playerIndex, float frameW, float frameH, float& minX, float& minY, float& maxX, float& maxY);
	// Size of world-space HUD elements relative to a full-screen view (0.5 in downscaled quadrants).
	float ViewScale(int playerIndex);
}

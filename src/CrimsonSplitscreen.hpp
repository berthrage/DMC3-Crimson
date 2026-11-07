#pragma once

#include <cstdint>

#include "HUD.hpp"

namespace CrimsonSplitscreen {
	void InitHooks();
	void HudTuningWindow(bool* open);
	// The HUD layout of the running splitscreen (base groups + general adjustment); false when splitscreen is off.
	bool HudLayoutInUse(HudSplitLayout& out);

	// Shared / single player camera multiplayer (HUD.cpp draws one cockpit per player): the tick update keeps every
	// player's cockpit (CockpitUpdateHook); SeatCockpitBegin swaps player's in for the draw that follows, SeatCockpitEnd
	// hands the cockpit back to player 1. False when that player has nothing to draw yet.
	bool SeatCockpitBegin(uint8_t* cockpit, int player);
	void SeatCockpitEnd(uint8_t* cockpit);

	// True while the game is in a state splitscreen can run in (playing, no cutscene); ignores the settle delay.
	bool GateOpen();
	// Splitscreen is the active multiplayer camera (2+ players), even while it is paused for a cutscene or load.
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
	// Scene projection matrix (0x40 bytes, the one the game's HUD projects with) playerIndex's last completed frame ended with.
	const uint8_t* ProjectionMatrix(int playerIndex);
	// Maps a full-frame pixel to the view of playerIndex (frame crop -> view cell).
	void MapPoint(int playerIndex, float frameW, float frameH, float& x, float& y);
	// Screen rect of playerIndex's view; false when splitscreen is off or the player has no view.
	bool ViewRect(int playerIndex, float frameW, float frameH, float& minX, float& minY, float& maxX, float& maxY);
	// Size of world-space HUD elements relative to a full-screen view (0.5 in downscaled quadrants).
	float ViewScale(int playerIndex);
}

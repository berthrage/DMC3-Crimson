#pragma once

#include <cstdint>

namespace CrimsonSplitscreen {
	void InitHooks();
	void HudTuningWindow(bool* open);

	// True while the game is in a state splitscreen can run in (playing, no cutscene); ignores the settle delay.
	bool GateOpen();
	// Number of views the screen is split into (2 = halves, 3-4 = quadrants), 0 when off or settling.
	int ViewCount();
	// Camera object of playerIndex's view as of its last drawn frame, null if unavailable.
	const uint8_t* CameraCopy(int playerIndex);
	bool CameraEye(int playerIndex, float out[3]);
	// Maps a full-frame pixel to the view of playerIndex (frame crop -> view cell).
	void MapPoint(int playerIndex, float frameW, float frameH, float& x, float& y);
	// Screen rect of playerIndex's view; false when splitscreen is off or the player has no view.
	bool ViewRect(int playerIndex, float frameW, float frameH, float& minX, float& minY, float& maxX, float& maxY);
	// Size of world-space HUD elements relative to a full-screen view (0.5 in downscaled quadrants).
	float ViewScale(int playerIndex);
}

#pragma once

#include "Core/DataTypes.hpp"
#include "UI/Texture2DD3D11.hpp"
#include <dxgi.h>


namespace CrimsonHUD {

void InitTextures(ID3D11Device* pd3dDevice);
void InitStyleRankTextures(ID3D11Device* pd3dDevice);
void InitStyleNameTextures(ID3D11Device* pd3dDevice);
void RedOrbCounterWindow();
void CheatsHUDIndicatorWindow();
void CheatHotkeysPopUpWindow();
void StyleMeterWindows();
void StylishPointsWindow();
void MissionTimerDisplay();
void LockOnWindows();
void StunDisplacementLockOnWindows();
void ShieldLockOnWindows();
void StyleDisplayWindow();
void StyleTextDisplayWindow();
void StyleEXPDisplayWindow();
void StyleLvlDispWindow();
void RoyalGaugeDispWindow();
void MirageGaugeMainPlayer();
void DanteHUD2Render();
Texture2DD3D11 *getCrimsonGradient();
Texture2DD3D11 *getRedOrbTexture();

struct FadeState {
	float alpha = 0.0f;
	float fadeTimer = 0.0f;
	bool fadingIn = false;
	bool fadingOut = false;
};

// Timer-based linear fade with mutual interruption:
//   - Pressing lock-on always starts a fresh fadeInDuration fade-IN
//   - Releasing lock-on always starts a fresh fadeOutDuration fade-OUT
//   - Interrupting either direction mid-fade picks up from current alpha
inline void UpdateFade(FadeState& state, bool currentlyActive, float fadeInDuration, float fadeOutDuration, float deltaTime) {
	if (currentlyActive) {
		if (state.fadingOut) {
			state.fadingOut = false;
			state.fadingIn = true;
			state.fadeTimer = state.alpha * fadeInDuration;
		} else if (!state.fadingIn && state.alpha < 1.0f) {
			state.fadingIn = true;
			state.fadeTimer = 0.0f;
		}

		if (state.fadingIn) {
			state.fadeTimer += deltaTime;
			if (state.fadeTimer >= fadeInDuration) {
				state.alpha = 1.0f;
				state.fadingIn = false;
			} else {
				state.alpha = state.fadeTimer / fadeInDuration;
			}
		}
	} else {
		if (state.fadingIn) {
			state.fadingIn = false;
			state.fadingOut = true;
			state.fadeTimer = (1.0f - state.alpha) * fadeOutDuration;
		} else if (!state.fadingOut && state.alpha > 0.0f) {
			state.fadingOut = true;
			state.fadeTimer = 0.0f;
		}

		if (state.fadingOut) {
			state.fadeTimer += deltaTime;
			if (state.fadeTimer >= fadeOutDuration) {
				state.alpha = 0.0f;
				state.fadingOut = false;
			} else {
				state.alpha = 1.0f - (state.fadeTimer / fadeOutDuration);
			}
		}
	}
}

inline float SmoothLerp(float from, float to, float speed, float deltaTime) {
	return from + (to - from) * (1.0f - std::exp(-speed * deltaTime));
}

// Snap alpha to 0 instantly — used on early-return / invalid-state paths
// so the next valid frame can start a clean fade-in from 0.
inline void ForceFadeOut(FadeState& state) {
	state.fadingIn = false;
	state.fadingOut = false;
	state.fadeTimer = 0.0f;
	state.alpha = 0.0f;
}

}
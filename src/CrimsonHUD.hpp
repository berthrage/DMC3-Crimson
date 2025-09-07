#pragma once

#include "Core/DataTypes.hpp"
#include <dxgi.h>


namespace CrimsonHUD {

void InitTextures(ID3D11Device* pd3dDevice);
void InitStyleRankTextures(ID3D11Device* pd3dDevice);
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


struct FadeState {
	float alpha = 0.0f;
	bool wasActive = false;
};

inline float SmoothLerp(float from, float to, float speed, float deltaTime) {
	return from + (to - from) * (1.0f - std::exp(-speed * deltaTime));
}

}
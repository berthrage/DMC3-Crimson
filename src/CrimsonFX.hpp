#pragma once

#include "Core/DataTypes.hpp"

#pragma region GeneralFX
void DTReadySFX();
void CalculateCameraPlayerDistance(byte8* actorBaseAddr);
void DTExplosionFXController(byte8* actorBaseAddr);
void StyleRankHudFadeoutController();

#pragma endregion

#pragma region DanteSpecificFX

void RoyalguardSFX(byte8* actorBaseAddr);
void DelayedComboFXController(byte8* actorBaseAddr);
void StyleSwitchFlux(byte8* actorBaseAddr);
void StyleSwitchDrawText(byte8* actorBaseAddr);
void SetStyleSwitchDrawTextTime(int style, byte8* actorBaseAddr);

#pragma endregion
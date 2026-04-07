#pragma once

#include "Core/DataTypes.hpp"

namespace CrimsonFX {

#pragma region GeneralFX
void DTReadySFX();
void CalculateViewProperties(byte8* actorBaseAddr);
void DTExplosionFXController(byte8* actorBaseAddr);
void StyleRankHudFadeoutController();

#pragma endregion

#pragma region DanteSpecificFX

void RoyalBlockFX(byte8* actorBaseAddr);
void DelayedComboFXController(byte8* actorBaseAddr);
void StyleSwitchFluxCrimson(byte8* actorBaseAddr, EffekseerHandle* styleSwitchHandles, EffekseerHandle* swooshHandles, uint8 style, uint32_t color);
void StyleSwitchFluxNS(byte8* actorBaseAddr, EffekseerHandle* styleSwitchHandles, EffekseerHandle* swooshHandles, uint8 style, uint32_t color);
void MoveStyleSwitchFluxes(byte8* actorBaseAddr, EffekseerHandle* styleSwitchHandles);
void StyleSwitchFlux(byte8* actorBaseAddr);
void StyleSwitchDrawText(byte8* actorBaseAddr);
void SetStyleSwitchDrawTextTime(int style, byte8* actorBaseAddr);

#pragma endregion

}
#pragma once

#include "Core/DataTypes.hpp"
#include <dxgi.h>


namespace CrimsonHUD {

void InitTextures(ID3D11Device* pd3dDevice);
void RedOrbCounterWindow();
void CheatsHUDIndicatorWindow();
void CheatHotkeysPopUpWindow();
void StyleMeterWindow();

}
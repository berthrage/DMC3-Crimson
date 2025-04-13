#pragma once

#include "Core/DataTypes.hpp"

void HUD_UpdateStyleIcon(uint8 style, uint8 character);

void HUD_UpdateDevilTriggerGauge(uint8 character);

void HUD_UpdateDevilTriggerLightning(uint8 character);

void HUD_UpdateDevilTriggerExplosion(uint8 character);

bool HUD_UpdateWeaponIcon(uint8 index, uint8 weapon);

void HUD_Init();
void ToggleHideMainHUD(bool enable);
void ToggleHideLockOn(bool enable);
void ToggleHideBossHUD(bool enable);

void ToggleForceVisibleHUD(bool enable);
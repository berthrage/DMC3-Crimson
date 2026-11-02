#pragma once

#include "Core/DataTypes.hpp"

void HUD_UpdateStyleIcon(uint8 style, uint8 character);

void HUD_UpdateDevilTriggerGauge(uint8 character);

void HUD_UpdateDevilTriggerLightning(uint8 character);

void HUD_UpdateDevilTriggerExplosion(uint8 character);

bool HUD_UpdateWeaponIcon(uint8 index, uint8 weapon);

void HUD_UpdateHPBar(uint8 character);

void HUD_UpdateLockOn(uint8 character);

// Bumped by every HUD_Update* loader so callers can tell when the icons changed under them.
extern uint32 g_hudIconSerial;

extern byte8* g_dynamicHUD_id100;
extern byte8* g_dynamicHUD_id100V;

void HUD_Init();
void HUD_ReloadIcons();
void ToggleHideMainHUD(bool enable);
void ToggleHideBossHUD(bool enable);

void ToggleForceVisibleHUD(bool enable);
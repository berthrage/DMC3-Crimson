#pragma once
#include <cstdint>
#include "Vars.hpp"

extern "C" {
// SampleMod
void SampleModDetour1();

// GuardGravity
void GuardGravityDetour();

// EnableAirTaunt
void EnableAirTauntDetour();

// SetAirTaunt
void SetAirTauntDetour();

// CreateEffect
extern int createEffectBank;
extern int createEffectID;
extern int createEffectBone;
extern std::uint64_t createEffectPlayerAddr;
void CreateEffectDetour();

// HoldToCrazyCombo
void HoldToCrazyComboDetour();

// DisableDriveHold
void DisableDriveHoldDetour();

// HudHPSeparation
void HudHPSeparationDetour();

// HudStyleBarPos
void HudStyleBarPosDetour();

// StyleRankHudNoFadeout
void StyleRankHudNoFadeoutDetour();

// ShootRemapDown
void ShootRemapDownDetour();
void ShootRemapHoldDetour();

// VergilNeutralTrick
void VergilNeutralTrickDetour();
}

bool g_HoldToCrazyComboFuncA(PlayerActorData& actorData);
void InitDetours();
void ToggleClassicHUDPositionings(bool enable);
void ToggleStyleRankHudNoFadeout(bool enable);

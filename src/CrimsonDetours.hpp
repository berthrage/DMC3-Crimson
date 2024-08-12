#pragma once
#include <cstdint>
#include "Vars.hpp"

namespace CrimsonDetours {

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

// DisableStaggerRoyalguard: for DT-Infused Royalguard
void DisableStaggerRoyalguardDetour();

// ToggleTakeDamage: for DT-Infused Royalguard
extern std::uint64_t toggleTakeDamageActorBaseAddr;
extern bool toggleTakeDamage;
void ToggleTakeDamageDetour();

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
bool DisableStaggerCheck(PlayerActorData& actorData);
void InitDetours();
void ToggleClassicHUDPositionings(bool enable);
void ToggleStyleRankHudNoFadeout(bool enable);
void ToggleCerberusCrashFix(bool enable);

}
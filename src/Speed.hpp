#pragma once

namespace Speed {
void UpdateEffectiveSpeeds();
void ApplyRuntimeGlobalSpeed();
float GetEffectiveGlobalSpeed();
void Toggle(bool enable);
}
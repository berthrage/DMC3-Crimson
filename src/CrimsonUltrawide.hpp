#pragma once

namespace CrimsonUltrawide {

// Keeps a DMCHDFix.asi loaded into dmc3 (e.g. by an ASI loader) from patching the game. Call from DllMain.
void DisableStandaloneFix();

// Recomputes the aspect from the current render size and toggles the ultrawide and culling fixes. Runs every frame on the game thread.
void Update();

bool Active();

// Render aspect / (16:9), 1.0 while inactive.
float Multiplier();

// True if a DMCHDFix.asi had already patched dmc3 before Crimson could disable it, the integrated fixes stay off then.
bool StandaloneFixLoaded();

}

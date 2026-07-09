#pragma once


// Drop-in replacements for ImGui::GetTime() / ImGui::GetIO()
// Backed by QueryPerformanceCounter — same precision as ImGui.
// Called exactly once per frame from Timestep() (SwapChainWrapper::Present).
// Do NOT call Tick() from any other code path — doing so would
// produce a near-zero delta that corrupts s_Delta.

namespace CrimsonClock {

double Time();        // monotonic seconds since first tick
float  DeltaTime();   // frame delta  (like ImGui::GetIO().DeltaTime)
float  Framerate();   // instantaneous FPS (like ImGui::GetIO().Framerate)
void   Tick();         // update internals — call once per Present / game tick

} // namespace CrimsonClock

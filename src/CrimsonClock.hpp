#pragma once


// Drop-in replacements for ImGui::GetTime() / ImGui::GetIO()
// Backed by QueryPerformanceCounter — same precision as ImGui.
// Call CrimsonClock::Tick() once per frame (in Timestep).

namespace CrimsonClock {

double Time();        // monotonic seconds since first tick
float  DeltaTime();   // frame delta  (like ImGui::GetIO().DeltaTime)
float  Framerate();   // instantaneous FPS (like ImGui::GetIO().Framerate)
void   Tick();         // update internals — call once per Present / game tick

} // namespace CrimsonClock

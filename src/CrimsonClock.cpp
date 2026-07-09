#include "Core/Core.hpp"

namespace CrimsonClock {
namespace {
    LARGE_INTEGER s_QpcFreq   = {};
    LARGE_INTEGER s_LastQpc   = {};
    double        s_Time      = 0.0;
    float         s_Delta     = 0.0f;
    float         s_Framerate = 0.0f;
    bool          s_Init      = false;
}

void Tick() {
    if (!s_Init) {
        QueryPerformanceFrequency(&s_QpcFreq);
        QueryPerformanceCounter(&s_LastQpc);
        s_Init = true;
        return;
    }
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    const double elapsed = double(now.QuadPart - s_LastQpc.QuadPart)
                         / double(s_QpcFreq.QuadPart);

    // Tick() is called exactly once per frame (from Timestep), so
    // elapsed is always the genuine frame delta.  No threshold needed.
    s_LastQpc = now;
    s_Time   += elapsed;
    s_Delta   = static_cast<float>(elapsed);
    s_Framerate = (elapsed > 0.0)
                ? static_cast<float>(1.0 / elapsed)
                : 0.0f;
}

double Time()      { return s_Time; }
float  DeltaTime() { return s_Delta; }
float  Framerate() { return s_Framerate; }

} // namespace CrimsonClock

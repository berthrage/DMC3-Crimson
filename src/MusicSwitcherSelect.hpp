#pragma once

#include "Core/DataTypes.hpp"

// The parts of playback selection that are pure arithmetic. Kept here so they
// can be exercised without the game running.
namespace MusicSwitcherSelect {

// Starting inside the final seconds of a track would be dead air rather than
// music, so the offset is pulled back to leave something audible.
inline int32 StartOffsetMsFromPercent(int32 durationMs, int percent) {
    if (durationMs <= 0) {
        return 0;
    }

    if (percent < 0) {
        percent = 0;
    } else if (percent > 100) {
        percent = 100;
    }

    int64 startMs      = (static_cast<int64>(durationMs) * percent) / 100;
    const int64 latest = durationMs - 2000;

    if (startMs > latest) {
        startMs = (latest > 0) ? latest : 0;
    }

    return static_cast<int32>(startMs);
}

} // namespace MusicSwitcherSelect

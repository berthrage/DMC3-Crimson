#pragma once

#include "Vars.hpp"

namespace DMC3VideoAudio {

// Replaces the mastering voice creation in VideoAudioInit_sub_14003C7E0, which ignores failure and crashes in
// CreateSourceVoice when the default output device can't be opened. Falls back to the other XAudio2 devices;
// returns false (IXAudio2 released, video marked finished) when none of them works.
bool CreateMasteringVoice(PCFullMotionVideo& video);

} // namespace DMC3VideoAudio

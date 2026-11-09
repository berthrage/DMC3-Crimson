#pragma once

#include <string>
#include <vector>

#include "CrimsonMusicSwitcher.hpp"

// Tag, cover-art and duration readers. Kept out of CrimsonMusicSwitcher.cpp so
// the binary format work does not mix with the playback state machine.
namespace MusicSwitcherTags {

// Fills title, artist, album and where the cover art sits. Missing tags leave
// the fields untouched.
void ReadSongTags(MusicSwitcherSong& song, const std::string& path);

// Reads the cover art bytes a song points at. False when it has none.
bool ReadSongArtwork(const MusicSwitcherSong& song, const std::string& path, std::vector<uint8>& outBytes,
    std::string& outMimeType);

} // namespace MusicSwitcherTags

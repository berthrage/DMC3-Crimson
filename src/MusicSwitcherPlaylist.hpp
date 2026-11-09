#pragma once

#include <string>

#include "CrimsonMusicSwitcher.hpp"

// Playlist files: one JSON per playlist, sitting next to the songs they name.
namespace MusicSwitcherPlaylistFile {

// Fills `playlist` from the file. The name defaults to the file name, so a
// playlist can be shared by copying its file.
bool Load(const std::string& path, MusicSwitcherPlaylist& playlist);

bool Save(const std::string& path, const MusicSwitcherPlaylist& playlist);

} // namespace MusicSwitcherPlaylistFile

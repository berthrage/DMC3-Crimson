#pragma once

#include <string>
#include <vector>

#include "Core/DataTypes.hpp"

// Where the cover art lives inside the song file. Bytes are only read when
// something asks for them, so a large folder costs no memory.
struct MusicSwitcherArtwork {
    uint64 offset = 0;
    uint32 size   = 0;
    std::string mimeType;
    // Ogg keeps the picture base64-encoded inside the comment header, so it has
    // to be re-decoded from the head of the file rather than seeked to.
    bool insideComment = false;
};

// A music file found in Crimson\musicswitcher, with whatever its tags gave us.
struct MusicSwitcherSong {
    std::string fileName;
    std::string title;
    std::string artist;
    std::string album;
    int32 durationMs = 0;
    MusicSwitcherArtwork artwork;
    // Measured once per file: integrated loudness, and the gain that puts it on
    // the same level as the game's battle themes.
    float lufs          = 0.0f;
    float peakDbfs      = -120.0f;
    float normGain      = 1.0f;
    bool  loudnessKnown = false;

    bool HasArtwork() const {
        return artwork.insideComment || artwork.size != 0;
    }
};

// How a playlist chooses the song for a battle.
namespace MUSICPLAYLISTMODE {
enum {
    SEQUENTIAL, // in list order, one per battle
    RANDOM,     // a different song each battle
    PER_MISSION,// fixed by the mission number
    COUNT,
};
}

// One entry of a playlist. The song file is shared, the settings are not.
struct MusicSwitcherSongEntry {
    std::string file;
    uint8 startMinPercent = 40;
    uint8 startMaxPercent = 80;
    bool randomStart      = true;
};

struct MusicSwitcherPlaylist {
    std::string name;
    std::string fileName;
    uint8 mode = MUSICPLAYLISTMODE::SEQUENTIAL;
    std::vector<MusicSwitcherSongEntry> songs;
};

namespace CrimsonMusicSwitcher {

void Init();
void OnTick();

// Re-reads the folder, the tags and every playlist. Cheap enough for a button.
void Rescan();
const std::vector<MusicSwitcherSong>& Songs();
const std::vector<MusicSwitcherPlaylist>& Playlists();
const MusicSwitcherSong* FindSong(const std::string& fileName);

// Durations come from the mixer, which is not up yet when the DLL loads.
// False means some song still has no length.
bool AreDurationsProbed();

// Loudness is measured on a worker, one file at a time. False means some song
// still has no level, which only costs it normalization for now.
bool AreLevelsProbed();

// Playlists are edited in place and written back on demand. `name` is the
// playlist's current name; NULL when there is no such playlist.
MusicSwitcherPlaylist* EditablePlaylist(const std::string& name);

// Creates an empty playlist, or returns false when the name is taken or invalid.
bool CreatePlaylist(const std::string& name);

// Writes the playlist to `<name>.playlist.json`, dropping the file it used to
// live in when the name changed.
bool SavePlaylist(MusicSwitcherPlaylist& playlist);

void AddSongToPlaylist(MusicSwitcherPlaylist& playlist, const std::string& fileName);
void RemoveSongFromPlaylist(MusicSwitcherPlaylist& playlist, size_t index);

// Moves an entry to another slot, shifting the rows in between.
void MoveSongInPlaylist(MusicSwitcherPlaylist& playlist, size_t from, size_t to);

bool IsPlaying();
const char* PlayingFileName();

// Cover art bytes for a song. Empty when it has none.
bool ReadArtwork(const MusicSwitcherSong& song, std::vector<uint8>& outBytes, std::string& outMimeType);

// Name of the playlist the current mode, character and config resolve to, or "" when none does.
const char* ResolvedPlaylistName();

bool IsReplaceableBattleTheme(const char* requestedPath);

} // namespace CrimsonMusicSwitcher

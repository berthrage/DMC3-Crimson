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
    // Key playlists store. Songs taken from the game carry a "vanilla/" prefix so
    // they cannot collide with a file of the same name in the Crimson folder.
    std::string fileName;
    // Where the file actually sits, resolved when the folder is scanned.
    std::string path;
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
    // Comes from the game's own sound folder rather than Crimson\musicswitcher.
    bool vanilla = false;

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
    uint8 startMinPercent = 20;
    uint8 startMaxPercent = 80;
    bool randomStart      = true;
};

struct MusicSwitcherPlaylist {
    std::string name;
    std::string fileName;
    uint8 mode = MUSICPLAYLISTMODE::SEQUENTIAL;
    // On, a battle song repeats until the fight ends. Off, this playlist moves to
    // the next song when one finishes. Per Mission always loops.
    bool loopThemes = true;
    std::vector<MusicSwitcherSongEntry> songs;
};

namespace CrimsonMusicSwitcher {
// Per Mission rows: missions 1-19 get a row each and the 20th row is Bloody
// Palace. Mission 20 is the Vergil 3 fight and the credits, so it has no row
// yet; slots past these are unused.
constexpr int kMissionRowCount = 19;
constexpr int kPalaceRow       = kMissionRowCount;

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

// Adds the song to the end of the playlist. Per Mission lists keep one row per
// mission, so the same file may appear more than once there; the other orders
// hold one row per file.
void AddSongToPlaylist(MusicSwitcherPlaylist& playlist, const std::string& fileName);

// Rewrites the start window of every entry in the playlist with the configured
// defaults for new songs.
void ApplyDefaultStartToAll(MusicSwitcherPlaylist& playlist);

// Appends an empty mission slot. Only Per Mission shows gaps; the other orders
// keep them in the file but skip them.
void AddGapToPlaylist(MusicSwitcherPlaylist& playlist);

// Adds the song to a specific mission slot: pads the list with empty slots when
// the mission is past the end, and replaces whatever sits in that slot.
void AddSongToPlaylistAt(MusicSwitcherPlaylist& playlist, const std::string& fileName, size_t slot);
void RemoveSongFromPlaylist(MusicSwitcherPlaylist& playlist, size_t index);

// Moves an entry to another slot, shifting the rows in between.
void MoveSongInPlaylist(MusicSwitcherPlaylist& playlist, size_t from, size_t to);

bool IsPlaying();
const char* PlayingFileName();
// Title of the song that is playing, or "" when nothing is.
const char* PlayingTitle();

// The song behind the banner: cover art, title, artist and where it is now.
const MusicSwitcherSong* PlayingSong();
uint32 PlayingRevision();
double PlayingStartedAt();
int32 PlayingPositionMs();
bool PlayingLoopEnabled();
// Flips the loop setting of the pool that is playing and applies it right away.
bool TogglePlayingLoop();

// True while a song plays from a pool that has an order to step through.
bool CanStepSong();

// Moves playback to the previous/next song of the pool in use, keeping the
// battle it was covering. Per Mission is fixed by mission and cannot step.
bool PreviousSong();
bool SkipSong();

// Auditions the song on a track of its own: a random five second slice, started
// from the beginning when the track is shorter than that. Any preview already
// running is replaced. False when the file cannot be played.
bool PreviewSong(const std::string& fileName);
bool IsPreviewing(const std::string& fileName);
void StopPreview();

// Cover art bytes for a song. Empty when it has none.
bool ReadArtwork(const MusicSwitcherSong& song, std::vector<uint8>& outBytes, std::string& outMimeType);

// Name of the playlist the current mode, character and config resolve to, or "" when none does.
const char* ResolvedPlaylistName();

bool IsReplaceableBattleTheme(const char* requestedPath);

} // namespace CrimsonMusicSwitcher

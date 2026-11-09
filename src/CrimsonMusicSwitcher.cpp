#include "CrimsonMusicSwitcher.hpp"

#include <algorithm>
#include <atomic>
#include <cctype>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <mutex>
#include <random>
#include <thread>
#include <utility>

#include "Core/Core.hpp"
#include "Core/Macros.h"
#include "CrimsonConfig.hpp"
#include "CrimsonFileHandling.hpp"
#include "CrimsonSDL.hpp"
#include "Global.hpp"
#include "MusicSwitcherLoudness.hpp"
#include "MusicSwitcherPlaylist.hpp"
#include "MusicSwitcherSelect.hpp"
#include "MusicSwitcherTags.hpp"
#include "Sound.hpp"
#include "Vars.hpp"

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

namespace fs = std::filesystem;

namespace {

constexpr const char* kAudioExtensions[] = {".mp3", ".ogg", ".wav", ".flac"};
constexpr const char* kPlaylistSuffix    = ".playlist.json";

// The cues a normal fight asks for; Battle_00..Battle_0a.
constexpr const char* kBattleThemes[] = {
    "battle_00",
    "battle_01",
    "battle_01b",
    "battle_02",
    "battle_03",
    "battle_04",
    "battle_05",
    "battle_06",
    "battle_07",
    "battle_08",
    "battle_0a",
};

std::vector<MusicSwitcherSong> g_songs;
std::vector<MusicSwitcherPlaylist> g_playlists;
bool g_durationsProbed = false;

std::string ToLower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    return value;
}

bool HasAudioExtension(const fs::path& path) {
    const std::string extension = ToLower(path.extension().string());

    for (const char* allowed : kAudioExtensions) {
        if (extension == allowed) {
            return true;
        }
    }

    return false;
}

bool EndsWith(const std::string& value, const std::string& suffix) {
    return value.size() >= suffix.size() &&
        (value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0);
}

std::string SongPath(const std::string& fileName) {
    return (fs::path(Paths::musicSwitcher) / fileName).string();
}

fs::path PlaylistPath(const std::string& fileName) {
    return fs::path(Paths::musicSwitcher) / fileName;
}

const MusicSwitcherPlaylist* FindPlaylist(const std::string& name) {
    if (name.empty()) {
        return NULL;
    }

    for (const auto& playlist : g_playlists) {
        if (playlist.name == name) {
            return &playlist;
        }
    }

    return NULL;
}

const std::string& PoolPlaylistName() {
    const auto& config = activeCrimsonConfig.MusicSwitcher;

    if (!config.perCharacterPools) {
        return config.battlePlaylist;
    }

    auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

    return (sessionData.character == CHARACTER::VERGIL) ? config.vergilBattlePlaylist
                                                       : config.danteBattlePlaylist;
}

bool PlaylistHasSong(const MusicSwitcherPlaylist& playlist) {
    std::error_code error;

    for (const auto& entry : playlist.songs) {
        if (fs::exists(SongPath(entry.file), error)) {
            return true;
        }
    }

    return false;
}


void ProbeDurations() {
    if (g_durationsProbed) {
        return;
    }

    bool allKnown = true;

    for (auto& song : g_songs) {
        if (song.durationMs > 0) {
            continue;
        }

        int32 durationMs = 0;
        if (CrimsonSDL::ProbeMusicDurationMs(SongPath(song.fileName).c_str(), durationMs)) {
            song.durationMs = durationMs;
        } else {
            allKnown = false;
        }
    }

    g_durationsProbed = allKnown;
}

// --- loudness ------------------------------------------------------------

// Every file is measured once and remembered. The worker only ever writes
// g_loudness; the render thread copies results into g_songs, so a rescan in the
// middle of a pass cannot race it.
struct SongLoudness {
    float lufs     = 0.0f;
    float peakDbfs = -120.0f;
    bool valid     = false;
    uint64 size    = 0;
    int64 mtime    = 0;
};

std::mutex g_loudnessMutex;
std::unordered_map<std::string, SongLoudness> g_loudness;
std::atomic<uint32> g_loudnessRevision{0};
std::atomic<bool> g_loudnessRunning{false};
std::thread g_loudnessWorker;
uint32 g_loudnessSeenRevision = 0;
// Target the current song was started with, so slider moves can be heard at once.
float g_appliedTargetLufs = 0.0f;

constexpr const char* kLoudnessCacheName = "musicswitcher-loudness.json";

std::string LoudnessCachePath() {
    return (fs::path(Paths::config) / kLoudnessCacheName).string();
}

uint64 FileSize(const std::string& path) {
    std::error_code error;
    const uintmax_t size = fs::file_size(path, error);

    return error ? 0 : static_cast<uint64>(size);
}

int64 ModifiedTime(const std::string& path) {
    std::error_code error;
    const fs::file_time_type stamp = fs::last_write_time(path, error);

    return error ? 0 : static_cast<int64>(stamp.time_since_epoch().count());
}

void LoadLoudnessCache() {
    std::ifstream file(LoudnessCachePath(), std::ios::binary);
    if (!file) {
        return;
    }

    std::string text((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    if (text.empty()) {
        return;
    }

    rapidjson::Document document;
    if (document.Parse(text.c_str()).HasParseError() || !document.IsObject()) {
        return;
    }

    auto songs = document.FindMember("songs");
    if (songs == document.MemberEnd() || !songs->value.IsObject()) {
        return;
    }

    std::lock_guard<std::mutex> lock(g_loudnessMutex);

    for (const auto& item : songs->value.GetObject()) {
        if (!item.value.IsObject()) {
            continue;
        }

        auto size  = item.value.FindMember("size");
        auto mtime = item.value.FindMember("mtime");
        auto lufs  = item.value.FindMember("lufs");
        auto peak  = item.value.FindMember("peak");

        if (size == item.value.MemberEnd() || !size->value.IsUint64() ||
            mtime == item.value.MemberEnd() || !mtime->value.IsInt64() ||
            lufs == item.value.MemberEnd() || !lufs->value.IsNumber() ||
            peak == item.value.MemberEnd() || !peak->value.IsNumber()) {
            continue;
        }

        const std::string name = item.name.GetString();
        const std::string path = SongPath(name);

        // A replaced or edited file has to be measured again.
        if (FileSize(path) != size->value.GetUint64() || ModifiedTime(path) != mtime->value.GetInt64()) {
            continue;
        }

        SongLoudness entry;
        entry.size     = size->value.GetUint64();
        entry.mtime    = mtime->value.GetInt64();
        entry.lufs     = static_cast<float>(lufs->value.GetDouble());
        entry.peakDbfs = static_cast<float>(peak->value.GetDouble());
        entry.valid    = true;

        g_loudness[name] = entry;
    }
}

void SaveLoudnessCache() {
    std::unordered_map<std::string, SongLoudness> snapshot;
    {
        std::lock_guard<std::mutex> lock(g_loudnessMutex);

        for (const auto& pair : g_loudness) {
            if (pair.second.valid) {
                snapshot.insert(pair);
            }
        }
    }

    if (snapshot.empty()) {
        return;
    }

    rapidjson::Document document;
    document.SetObject();

    auto& allocator = document.GetAllocator();

    document.AddMember("version", 1, allocator);

    rapidjson::Value songs(rapidjson::kObjectType);

    for (const auto& pair : snapshot) {
        rapidjson::Value entry(rapidjson::kObjectType);

        entry.AddMember("size", static_cast<uint64_t>(pair.second.size), allocator);
        entry.AddMember("mtime", static_cast<int64_t>(pair.second.mtime), allocator);
        entry.AddMember("lufs", static_cast<double>(pair.second.lufs), allocator);
        entry.AddMember("peak", static_cast<double>(pair.second.peakDbfs), allocator);

        rapidjson::Value name(pair.first.c_str(), allocator);
        songs.AddMember(name, entry, allocator);
    }

    document.AddMember("songs", songs, allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    std::error_code error;
    fs::create_directories(Paths::config, error);

    std::ofstream file(LoudnessCachePath(), std::ios::binary | std::ios::trunc);
    if (!file) {
        return;
    }

    file.write(buffer.GetString(), static_cast<std::streamsize>(buffer.GetSize()));
}

void ApplyLoudnessResults() {
    const float targetLufs = activeCrimsonConfig.MusicSwitcher.normalizeTargetLufs;

    std::lock_guard<std::mutex> lock(g_loudnessMutex);

    for (auto& song : g_songs) {
        auto it = g_loudness.find(song.fileName);

        if (it == g_loudness.end() || !it->second.valid) {
            continue;
        }

        song.lufs          = it->second.lufs;
        song.peakDbfs      = it->second.peakDbfs;
        song.normGain      = MusicSwitcherLoudness::NormalizationGain(it->second.lufs,
            it->second.peakDbfs, targetLufs);
        song.loudnessKnown = true;
    }
}

void LoudnessWorker(std::vector<std::pair<std::string, std::string>> queue) {
    for (const auto& item : queue) {
        if (!CrimsonSDL::SDL3Init) {
            break;
        }

        float lufs = 0.0f;
        float peak = -120.0f;

        SongLoudness entry;
        entry.size  = FileSize(item.second);
        entry.mtime = ModifiedTime(item.second);
        entry.valid = CrimsonSDL::MeasureMusicLoudness(item.second.c_str(), lufs, peak);

        if (entry.valid) {
            entry.lufs     = lufs;
            entry.peakDbfs = peak;
        }

        {
            std::lock_guard<std::mutex> lock(g_loudnessMutex);
            g_loudness[item.first] = entry;
        }

        g_loudnessRevision++;

        // One file at a time, spaced out: a fight must never stutter for this.
        for (int slice = 0; slice < 20; ++slice) {
            std::this_thread::sleep_for(std::chrono::milliseconds(6));
        }
    }

    SaveLoudnessCache();

    Log("MusicSwitcher: measured %u songs for level.", static_cast<uint32>(queue.size()));

    g_loudnessRunning = false;
}

void StartLoudnessWorker() {
    if (g_loudnessRunning) {
        return;
    }

    // The previous pass is over; collect it before the member is reused.
    if (g_loudnessWorker.joinable()) {
        g_loudnessWorker.join();
    }

    std::vector<std::pair<std::string, std::string>> queue;
    {
        std::lock_guard<std::mutex> lock(g_loudnessMutex);

        for (const auto& song : g_songs) {
            if (g_loudness.find(song.fileName) == g_loudness.end()) {
                queue.emplace_back(song.fileName, SongPath(song.fileName));
            }
        }
    }

    if (queue.empty()) {
        return;
    }

    g_loudnessRunning = true;
    g_loudnessWorker  = std::thread(LoudnessWorker, std::move(queue));
}

void UpdateLoudness() {
    const auto& config = activeCrimsonConfig.MusicSwitcher;

    const uint32 revision = g_loudnessRevision.load();

    if (revision != g_loudnessSeenRevision) {
        g_loudnessSeenRevision = revision;

        // Only affects songs started from here on; the playing track keeps the
        // gain it began with.
        ApplyLoudnessResults();
    }

    // Measuring is pointless while the feature is off, and the decoder needs the
    // mixer, which comes up long after the DLL loads.
    if (!config.enabled || !CrimsonSDL::SDL3Init) {
        return;
    }

    StartLoudnessWorker();
}

// --- selection -----------------------------------------------------------

// Playback state. The vanilla track is left alone and muted, so the engine keeps
// its own record of what it is playing: our song covers the battle cue that
// record names, and ends when that cue's channel does.
bool g_playing   = false;
int g_cursor     = 0;
int g_lastPicked = -1;
std::string g_playedTheme;    // theme the current song is covering
std::string g_playedFile;     // entry currently loaded
std::string g_playedPlaylist;
int32 g_appliedVolume = -1;
// The game's music channel is silenced while our song covers its track.
bool g_gameMusicMuted = false;
// Debounces are wall-clock: the game can run above 300 fps, where counting
// ticks turned a five second grace period into less than one.
double g_retryAt      = 0.0;
double g_notPlayingAt = 0.0;
double g_nextProbeAt  = 0.0;
double g_cueEndAt     = 0.0;
// Set once the cue we cover has actually risen above silence.
bool g_cueAudible = false;
// Playback fades: the cue comes in at once and tails off once the fight is over.
constexpr int32 kFadeInMs  = 3000;
constexpr int32 kFadeOutMs = 7500;
// The pause menu drops the music to 30% without silencing it.
constexpr float kPausedDuck = 0.3f;
// Vanilla ends a cue by fading its volume out, so ours ends with it. The grace
// covers the fade tail and a cue that has not been mixed yet.
constexpr double kCueEndGraceSeconds   = 0.4;
constexpr double kCueStartGraceSeconds = 3.0;
constexpr int32  kCueEndFadeMs         = 2400;

double NowSeconds() {
    using Clock = std::chrono::steady_clock;

    return std::chrono::duration<double>(Clock::now().time_since_epoch()).count();
}

// The in-game pause menu, the same event CrimsonSDL::ReduceMusicVolumeInPause
// reads; it stays on the pause screen until the player resumes.
bool PausedNow() {
    auto pool = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
    if (!pool || !pool[8]) {
        return false;
    }

    auto& eventData = *reinterpret_cast<EventData*>(pool[8]);

    return eventData.event == EVENT::PAUSE;
}

std::string ThemeName(const std::string& requestedPath) {
    if (requestedPath.empty()) {
        return std::string();
    }

    return ToLower(fs::path(requestedPath).stem().string());
}

// `dmc3.exe+0xD23A80` is not a sustained "in a fight" flag and the boss counter at
// `0xD5BA88` flickers, so neither is consulted.

// The engine owns two BGM slots at `dmc3.exe+CF3708`, 0x303A8 bytes apart. Each
// one remembers the path it was last asked to play and owns a track object at
// `dmc3.exe+5DE460` that carries the cue's FMOD channel.
//
// Neither the path nor the channel says when a cue is over: the engine never
// clears the path, and it ends music by fading the cue's own volume down and
// leaving the channel in place. The volume the mixer applied to that cue is the
// only truthful "still sounding" signal.
constexpr uintptr_t kEngineBgmSlotArray     = 0xCF3708;
constexpr uintptr_t kEngineBgmSlotSize      = 0x303A8;
constexpr uintptr_t kEngineBgmSlotPath      = 0x301F4;
constexpr uintptr_t kEngineBgmTracks        = 0x5DE460;
constexpr uintptr_t kEngineBgmTrackChannel  = 0x20;
constexpr uintptr_t kEngineBgmTrackVolume   = 0x04;
constexpr int       kEngineBgmSlots         = 2;

// Vanilla plays a cue at its full ratio, ducks it (30% and 20% for the pause
// menu, 45% in cutscenes) and fades it to 5% or 1% when it is done with it.
// Anything below this is the game's own "this cue is over".
constexpr float kCueSilentVolume = 0.08f;

std::string g_bgmSlotPaths[kEngineBgmSlots];

std::string BgmSlotPath(int slot) {
    const char* path = reinterpret_cast<const char*>(
        appBaseAddr + kEngineBgmSlotArray + slot * kEngineBgmSlotSize + kEngineBgmSlotPath);

    return std::string(path, strnlen(path, 128));
}

// The cue's volume as the mixer last applied it, 0..1. It follows the engine's
// own fades and is untouched by our muting, which works on the channel group
// instead.
bool BgmSlotSounding(int slot, float& volume) {
    volume = 0.0f;

    byte8* track = reinterpret_cast<byte8**>(appBaseAddr + kEngineBgmTracks)[slot];

    if (track == NULL) {
        return false;
    }

    if (*reinterpret_cast<byte8**>(track + kEngineBgmTrackChannel) == NULL) {
        return false;
    }

    volume = *reinterpret_cast<float*>(track + kEngineBgmTrackVolume);

    return volume > kCueSilentVolume;
}

// What vanilla is doing with its slots: the battle cue it has just asked for,
// and the one that is still sounding. Both are empty when neither slot wants
// battle music.
struct EngineCue {
    std::string requested;
    std::string playing;
    float playingVolume = 0.0f;
    // Volume of the slot holding the theme we cover, silent or not, for logging.
    float coveredVolume = 0.0f;
};

EngineCue ReadEngineCue() {
    EngineCue cue;

    for (int slot = 0; slot < kEngineBgmSlots; ++slot) {
        const std::string path  = BgmSlotPath(slot);
        const std::string theme = ThemeName(path);

        // A new path is a fresh request; the engine only rewrites it when it
        // wants a different cue. Reading it back also keeps a finished cue from
        // starting ours again.
        if (path != g_bgmSlotPaths[slot]) {
            g_bgmSlotPaths[slot] = path;

            if (CrimsonMusicSwitcher::IsReplaceableBattleTheme(theme.c_str())) {
                cue.requested = theme;
            }
        }

        float volume = 0.0f;
        const bool sounding = BgmSlotSounding(slot, volume);

        if (!g_playedTheme.empty() && theme == g_playedTheme) {
            cue.coveredVolume = volume;
        }

        if (!sounding || !CrimsonMusicSwitcher::IsReplaceableBattleTheme(theme.c_str())) {
            continue;
        }

        // The cue we already cover wins while it still sounds, so a crossfade
        // beside it cannot make ours restart twice.
        if (cue.playing.empty() || theme == g_playedTheme || volume > cue.playingVolume) {
            cue.playing       = theme;
            cue.playingVolume = volume;
        }
    }

    return cue;
}

// With the vanilla cue muted, ours is the only thing audible. Restoring the
// channel brings the game's own music back.
void MuteGameMusic(bool mute) {
    if (g_gameMusicMuted == mute) {
        return;
    }

    g_gameMusicMuted = mute;

    if (mute) {
        SetVolume(CHANNEL::MUSIC, 0.0f);
    } else {
        SetVolume(CHANNEL::MUSIC, activeCrimsonConfig.Sound.channelVolumes[CHANNEL::MUSIC] / 100.0f);
    }
}

// The music volume comes from Options > Sound, like every other channel.
int32 MusicVolume() {
    return activeCrimsonConfig.Sound.channelVolumes[CHANNEL::MUSIC];
}

std::vector<int> Candidates(const MusicSwitcherPlaylist& playlist) {
    std::vector<int> indices;

    for (int index = 0; index < static_cast<int>(playlist.songs.size()); ++index) {
        if (CrimsonMusicSwitcher::FindSong(playlist.songs[index].file) != NULL) {
            indices.push_back(index);
        }
    }

    return indices;
}

int32 RandomRange(int32 low, int32 high) {
    if (high <= low) {
        return low;
    }

    static std::mt19937 engine{std::random_device{}()};
    std::uniform_int_distribution<int32> distribution(low, high);

    return distribution(engine);
}

// Which song of the playlist this battle gets.
int PickCandidate(const MusicSwitcherPlaylist& playlist, const std::vector<int>& candidates) {
    const int count = static_cast<int>(candidates.size());
    if (count <= 0) {
        return -1;
    }

    switch (playlist.mode) {
    case MUSICPLAYLISTMODE::RANDOM: {
        int slot = RandomRange(0, count - 1);

        // Repeating the song that just played would not feel random.
        if (count > 1 && candidates[slot] == g_lastPicked) {
            slot = (slot + 1) % count;
        }

        return candidates[slot];
    }
    case MUSICPLAYLISTMODE::PER_MISSION: {
        auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

        // Missions are 1-based and Bloody Palace is 21, so the list is read as a
        // fixed table of missions.
        const int mission = (sessionData.mission > 0) ? static_cast<int>(sessionData.mission) : 1;
        const int slot     = (mission - 1) % count;

        return candidates[slot];
    }
    case MUSICPLAYLISTMODE::SEQUENTIAL:
    default: {
        if (g_cursor < 0 || g_cursor >= count) {
            g_cursor = 0;
        }

        return candidates[g_cursor];
    }
    }
}

// Where in the song a fight should start. Defaults to the middle-ish window the
// playlist entry asks for, clamped so a short track still plays something.
int32 StartOffsetMs(const MusicSwitcherSong& song, const MusicSwitcherSongEntry& entry) {
    int lowPercent  = entry.startMinPercent;
    int highPercent = entry.startMaxPercent;
    if (lowPercent > highPercent) {
        std::swap(lowPercent, highPercent);
    }

    const int percent = entry.randomStart ? RandomRange(lowPercent, highPercent) : lowPercent;

    return MusicSwitcherSelect::StartOffsetMsFromPercent(song.durationMs, percent);
}

bool StartEntry(const MusicSwitcherPlaylist& playlist, int entryIndex, const std::string& theme) {
    const auto& entry = playlist.songs[entryIndex];
    const MusicSwitcherSong* song = CrimsonMusicSwitcher::FindSong(entry.file);

    if (song == NULL) {
        return false;
    }

    const int32 volume = MusicVolume();

    g_appliedVolume = volume;

    const int32 startMs = StartOffsetMs(*song, entry);

    // Measured level; an unmeasured file plays as mastered for one battle.
    const float gainScale = song->loudnessKnown ? song->normGain : 1.0f;

    g_appliedTargetLufs = activeCrimsonConfig.MusicSwitcher.normalizeTargetLufs;

    if (!CrimsonSDL::PlaySwitcherMusic(SongPath(song->fileName).c_str(), startMs, kFadeInMs, volume,
            gainScale)) {
        Log("MusicSwitcher: could not start %s", song->fileName.c_str());

        return false;
    }

    Log("MusicSwitcher: playing %s (%s) from %ums, level %+.1f dB", song->fileName.c_str(),
        theme.c_str(), static_cast<uint32>(startMs),
        (gainScale > 0.0f) ? 20.0f * std::log10(gainScale) : 0.0f);

    g_playing        = true;
    g_playedFile     = entry.file;
    g_playedPlaylist = playlist.name;
    g_playedTheme    = theme;
    g_lastPicked     = entryIndex;
    g_cueEndAt       = 0.0;
    g_cueAudible = false;

    // Ours is now the only track that should be heard.
    MuteGameMusic(true);

    return true;
}

void StopPlayback(int32 fadeMs, const char* reason) {
    Log("MusicSwitcher: stop (%s)", reason);

    CrimsonSDL::StopSwitcherMusic(fadeMs);

    g_playing        = false;
    g_playedFile.clear();
    g_playedPlaylist.clear();
    g_playedTheme.clear();
    g_cueEndAt       = 0.0;
    g_cueAudible = false;

    // Hand the music channel back to the game.
    MuteGameMusic(false);
}

// --- tick ----------------------------------------------------------------

void Tick() {
    const auto& config = activeCrimsonConfig.MusicSwitcher;
    const double now    = NowSeconds();

    if (!config.enabled) {
        if (g_playing) {
            StopPlayback(0, "disabled");
        }

        MuteGameMusic(false);

        return;
    }

    // In-game cutscenes are their own scene, and vanilla keeps battle music
    // running through them, so they count as still being in the game.
    const bool inPlayableScene = (g_scene == SCENE::GAME) || (g_scene == SCENE::CUTSCENE);

    if (!inPlayableScene) {
        if (g_playing) {
            StopPlayback(0, "left game");
        }

        MuteGameMusic(false);

        g_cursor     = 0;
        g_lastPicked = -1;

        return;
    }

    // Other features can stop the music tag; keep our own idea of state honest
    // so a restart is possible. A track takes a moment to report as playing, so
    // this only reacts after half a second.
    if (g_playing) {
        if (CrimsonSDL::IsSwitcherMusicPlaying()) {
            g_notPlayingAt = 0.0;
        } else {
            if (g_notPlayingAt == 0.0) {
                g_notPlayingAt = now;
            } else if (now - g_notPlayingAt > 0.5) {
                g_notPlayingAt = 0.0;
                g_playing      = false;
                g_playedFile.clear();
                g_playedPlaylist.clear();
                g_playedTheme.clear();
                g_cueEndAt       = 0.0;
                g_cueAudible = false;

                MuteGameMusic(false);
            }
        }
    }

    // The level slider: recompute every song, and follow it right away if one is
    // already playing. This runs whether or not a song is running, so the next
    // battle always starts on the level the slider shows.
    if (config.normalizeTargetLufs != g_appliedTargetLufs) {
        g_appliedTargetLufs = config.normalizeTargetLufs;

        ApplyLoudnessResults();

        const MusicSwitcherSong* playing = g_playing ? CrimsonMusicSwitcher::FindSong(g_playedFile) : NULL;

        if (playing != NULL && playing->loudnessKnown) {
            CrimsonSDL::SetSwitcherMusicLevel(playing->normGain);
        }
    }

    if (g_playing) {
        const int32 volume = MusicVolume();

        if (volume != g_appliedVolume) {
            g_appliedVolume = volume;
            CrimsonSDL::SetSwitcherMusicVolume(volume);

            // A config reload pushes the slider value back onto the game's own
            // music channel, so silence it again while ours is playing.
            if (g_gameMusicMuted) {
                SetVolume(CHANNEL::MUSIC, 0.0f);
            }
        }

        // The pause menu always drops the music; it is not a preference.
        CrimsonSDL::SetSwitcherMusicDuck(PausedNow() ? kPausedDuck : 1.0f);

        // Keep the game's own channel silent for as long as ours is playing.
        MuteGameMusic(true);
    }

    // Vanilla's own BGM slots are the whole decision: a slot asked for a battle
    // theme starts ours, and the cue lasts as long as vanilla keeps that cue
    // sounding. Boss tracks are different files and are not in the replaceable
    // list, so they keep playing from vanilla.
    const EngineCue cue = ReadEngineCue();

    const MusicSwitcherPlaylist* playlist = FindPlaylist(PoolPlaylistName());

    if (playlist == NULL) {
        if (g_playing) {
            StopPlayback(kFadeOutMs, "no playlist resolved");
        }

        g_cueEndAt = 0.0;

        return;
    }

    // A fresh request wins over a cue that is still fading out beside it.
    const std::string theme = cue.requested.empty() ? cue.playing : cue.requested;

    if (theme.empty()) {
        if (g_playing) {
            // Vanilla faded its cue out and left the path and channel in place,
            // so its volume - not the name - is the end of the fight music.
            if (g_cueEndAt == 0.0) {
                g_cueEndAt = now;

                return;
            }

            const double grace = g_cueAudible ? kCueEndGraceSeconds : kCueStartGraceSeconds;

            if (now - g_cueEndAt < grace) {
                return;
            }

            char reason[96];
            snprintf(reason, sizeof(reason), "vanilla cue silent (volume %.3f)", cue.coveredVolume);

            StopPlayback(kCueEndFadeMs, reason);
        }

        return;
    }

    g_cueEndAt = 0.0;

    // Once the covered cue has been heard, silence means it is over.
    if (g_playing && !cue.playing.empty() && cue.playing == g_playedTheme) {
        g_cueAudible = true;
    }

    if (g_retryAt > now) {
        return;
    }

    const std::vector<int> candidates = Candidates(*playlist);
    if (candidates.empty()) {
        if (g_playing) {
            char reason[160];
            snprintf(reason, sizeof(reason), "no playable song in %s (%u entries, files missing)",
                playlist->name.c_str(), static_cast<uint32>(playlist->songs.size()));

            StopPlayback(kFadeOutMs, reason);
        }

        return;
    }

    // The game restarts its battle loop on the same theme; keep ours running.
    if (g_playing && g_playedPlaylist == playlist->name && g_playedTheme == theme) {
        return;
    }

    const bool newBattle = (g_playedTheme != theme);

    const int entryIndex = PickCandidate(*playlist, candidates);
    if (entryIndex < 0) {
        return;
    }

    if (!StartEntry(*playlist, entryIndex, theme)) {
        // A mixer that is still starting up is not a broken file, so only back
        // off once the mixer is actually available.
        g_retryAt = CrimsonSDL::SDL3Init ? (now + 2.0) : 0.0;

        return;
    }

    g_notPlayingAt = 0.0;

    // Sequential moves on for the next battle; the others re-resolve each time.
    if (newBattle && playlist->mode == MUSICPLAYLISTMODE::SEQUENTIAL) {
        g_cursor = (g_cursor + 1) % static_cast<int>(candidates.size());
    }
}

} // namespace

namespace CrimsonMusicSwitcher {

void Init() {
    LogFunction();

    Rescan();
}

void OnTick() {
    // The mixer is created on the first present, long after the DLL loads.
    const double now = NowSeconds();

    if (!g_durationsProbed && now >= g_nextProbeAt) {
        ProbeDurations();

        // A file the decoder cannot measure would otherwise be reopened every
        // frame; retrying twice a second is enough.
        g_nextProbeAt = now + 0.5;
    }

    UpdateLoudness();

    Tick();
}

void Rescan() {
    g_songs.clear();
    g_playlists.clear();
    g_durationsProbed = false;

    const std::string root = Paths::musicSwitcher;

    std::error_code error;
    fs::create_directories(root, error);

    if (!fs::is_directory(root, error)) {
        Log("CrimsonMusicSwitcher: %s unavailable.", root.c_str());

        return;
    }

    std::vector<fs::path> playlistPaths;

    try {
        for (const auto& entry : fs::directory_iterator(root)) {
            if (!entry.is_regular_file()) {
                continue;
            }

            const std::string name = entry.path().filename().string();

            if (HasAudioExtension(entry.path())) {
                MusicSwitcherSong song;
                song.fileName = name;
                song.title    = entry.path().stem().string();

                MusicSwitcherTags::ReadSongTags(song, entry.path().string());

                g_songs.push_back(song);
            } else if (EndsWith(name, kPlaylistSuffix)) {
                playlistPaths.push_back(entry.path());
            }
        }
    } catch (const fs::filesystem_error& e) {
        Log("CrimsonMusicSwitcher: %s", e.what());
    }

    std::sort(g_songs.begin(), g_songs.end(),
        [](const MusicSwitcherSong& lhs, const MusicSwitcherSong& rhs) {
            return ToLower(lhs.fileName) < ToLower(rhs.fileName);
        });

    for (const auto& path : playlistPaths) {
        MusicSwitcherPlaylist playlist;
        playlist.fileName = path.filename().string();
        playlist.name     = playlist.fileName.substr(0, playlist.fileName.size() - strlen(kPlaylistSuffix));

        // A malformed playlist still shows up, empty, so it can be fixed in the UI.
        MusicSwitcherPlaylistFile::Load(path.string(), playlist);

        g_playlists.push_back(playlist);
    }

    std::sort(g_playlists.begin(), g_playlists.end(),
        [](const MusicSwitcherPlaylist& lhs, const MusicSwitcherPlaylist& rhs) {
            return ToLower(lhs.name) < ToLower(rhs.name);
        });

    // The cache is the persistent record; dropping the in-memory copy makes a
    // rescan re-check every file against its current size and timestamp.
    {
        std::lock_guard<std::mutex> lock(g_loudnessMutex);
        g_loudness.clear();
    }

    LoadLoudnessCache();
    ApplyLoudnessResults();

    Log("CrimsonMusicSwitcher: %u songs, %u playlists.", static_cast<uint32>(g_songs.size()),
        static_cast<uint32>(g_playlists.size()));

    ProbeDurations();
}

const std::vector<MusicSwitcherSong>& Songs() {
    return g_songs;
}

const std::vector<MusicSwitcherPlaylist>& Playlists() {
    return g_playlists;
}

const MusicSwitcherSong* FindSong(const std::string& fileName) {
    for (const auto& song : g_songs) {
        if (song.fileName == fileName) {
            return &song;
        }
    }

    return NULL;
}

bool AreDurationsProbed() {
    return g_durationsProbed;
}

bool AreLevelsProbed() {
    std::lock_guard<std::mutex> lock(g_loudnessMutex);

    for (const auto& song : g_songs) {
        if (g_loudness.find(song.fileName) == g_loudness.end()) {
            return false;
        }
    }

    return true;
}

MusicSwitcherPlaylist* EditablePlaylist(const std::string& name) {
    for (auto& playlist : g_playlists) {
        if (playlist.name == name) {
            return &playlist;
        }
    }

    return NULL;
}

bool CreatePlaylist(const std::string& name) {
    if (name.empty() || FindPlaylist(name) != NULL) {
        return false;
    }

    MusicSwitcherPlaylist playlist;
    playlist.name     = name;
    playlist.fileName = name + kPlaylistSuffix;

    if (!MusicSwitcherPlaylistFile::Save(PlaylistPath(playlist.fileName).string(), playlist)) {
        return false;
    }

    Rescan();

    return true;
}

bool SavePlaylist(MusicSwitcherPlaylist& playlist) {
    if (playlist.name.empty()) {
        return false;
    }

    std::error_code error;
    fs::create_directories(Paths::musicSwitcher, error);

    const std::string fileName = playlist.name + kPlaylistSuffix;

    // Renaming writes a new file, so the old one has to go.
    if (!playlist.fileName.empty() && playlist.fileName != fileName) {
        fs::remove(PlaylistPath(playlist.fileName), error);
    }

    playlist.fileName = fileName;

    return MusicSwitcherPlaylistFile::Save(PlaylistPath(fileName).string(), playlist);
}

void AddSongToPlaylist(MusicSwitcherPlaylist& playlist, const std::string& fileName) {
    for (const auto& entry : playlist.songs) {
        if (entry.file == fileName) {
            return;
        }
    }

    MusicSwitcherSongEntry entry;
    entry.file        = fileName;
    entry.randomStart = activeCrimsonConfig.MusicSwitcher.randomStartDefault;

    playlist.songs.push_back(entry);
}

void RemoveSongFromPlaylist(MusicSwitcherPlaylist& playlist, size_t index) {
    if (index < playlist.songs.size()) {
        playlist.songs.erase(playlist.songs.begin() + index);
    }
}

void MoveSongInPlaylist(MusicSwitcherPlaylist& playlist, size_t from, size_t to) {
    if (from >= playlist.songs.size() || to >= playlist.songs.size() || from == to) {
        return;
    }

    const MusicSwitcherSongEntry entry = playlist.songs[from];

    playlist.songs.erase(playlist.songs.begin() + from);
    playlist.songs.insert(playlist.songs.begin() + to, entry);
}

bool IsPlaying() {
    return g_playing && CrimsonSDL::IsSwitcherMusicPlaying();
}

const char* PlayingFileName() {
    return g_playedFile.c_str();
}

bool ReadArtwork(const MusicSwitcherSong& song, std::vector<uint8>& outBytes, std::string& outMimeType) {
    return MusicSwitcherTags::ReadSongArtwork(song, SongPath(song.fileName), outBytes, outMimeType);
}

const char* ResolvedPlaylistName() {
    const MusicSwitcherPlaylist* playlist = FindPlaylist(PoolPlaylistName());

    return (playlist != NULL && PlaylistHasSong(*playlist)) ? playlist->name.c_str() : "";
}

bool IsReplaceableBattleTheme(const char* requestedPath) {
    if (requestedPath == NULL || requestedPath[0] == '\0') {
        return false;
    }

    const std::string name = ToLower(fs::path(requestedPath).stem().string());

    for (const char* theme : kBattleThemes) {
        if (name == theme) {
            return true;
        }
    }

    return false;
}

} // namespace CrimsonMusicSwitcher

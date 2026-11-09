#define SDL_MAIN_HANDLED
#include "../ThirdParty/SDL3/SDL.h"
#include "../ThirdParty/SDL3/SDL_mixer.h"
#include <string>
#include <thread>
#include <chrono>

// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "Core/Core.hpp"
#include "Config.hpp"
#include "SDL.hpp"
#include "Vars.hpp"
#include "CrimsonFileHandling.hpp"
#include "CrimsonUtil.hpp"
#include "Sound.hpp"
#include "DebugDrawDX11.hpp"
#include <iostream>
#include <unordered_set>
#include <deque>
#include <cctype>
#include <Xinput.h>
#include "Core/Input.hpp"
#include "Global.hpp"
#include "DMC3Input.hpp"
#include "MusicSwitcherLoudness.hpp"

namespace CrimsonSDL {

SDL_Gamepad* mainController = NULL;
std::vector<SDL_Gamepad*> controllers(4, NULL);
std::unordered_set<SDL_JoystickID> currentlyConnected;
SDL_Gamepad* sdlGamepadByXiSlot[4] = {NULL, NULL, NULL, NULL};
std::vector<SDL_Gamepad*> sdlGamepadsExtra;
std::string SDL3Initialization   = "";
std::string MixerInitialization  = "";
std::string MixerInitialization2 = "";
bool SDL3Init                    = false;
bool cacheAudioFiles             = false;

static MIX_Mixer* g_mixer        = NULL;
static MIX_Track* g_musicTracks[2] = {NULL, NULL};
static int g_nextMusicTrack      = 0;
static SDL_PropertiesID g_playProperties = 0;
static float g_musicGain         = 1.0f;
static float g_musicDuck         = 1.0f;
static std::unordered_map<std::string, MIX_Audio*> g_audioCache;

MIX_Audio* changeGun;
MIX_Audio* changeDevilArm;
MIX_Audio* changeWeaponDMC3;
MIX_Audio* styleChange;
MIX_Audio* tricksterVO;
MIX_Audio* swordmasterVO;
MIX_Audio* gunslingerVO;
MIX_Audio* royalguardVO;
MIX_Audio* quicksilverVO;
MIX_Audio* doppelganger1VO;
MIX_Audio* doppelganger2VO;
MIX_Audio* sprintL1;
MIX_Audio* sprintL2;
MIX_Audio* devilTriggerInL1;
MIX_Audio* devilTriggerInL2;
MIX_Audio* devilTriggerOut;
MIX_Audio* devilTriggerLoop;
MIX_Audio* devilTriggerReady;
MIX_Audio* dtExplosionStart;
MIX_Audio* dtExplosionLoop;
MIX_Audio* dtExplosionFinish;
MIX_Audio* dtExplosionRelease;
MIX_Audio* doppelgangerIn;
MIX_Audio* doppelgangerOut;
MIX_Audio* quicksilverIn;
MIX_Audio* quicksilverOut;
MIX_Audio* styleRankD1;
MIX_Audio* styleRankD2;
MIX_Audio* styleRankC1;
MIX_Audio* styleRankC2;
MIX_Audio* styleRankB1;
MIX_Audio* styleRankB2;
MIX_Audio* styleRankA1;
MIX_Audio* styleRankA2;
MIX_Audio* styleRankS1;
MIX_Audio* styleRankS2;
MIX_Audio* styleRankSS1;
MIX_Audio* styleRankSS2;
MIX_Audio* styleRankSSS1;
MIX_Audio* styleRankSSS2;
MIX_Audio* delayedCombo1;
MIX_Audio* delayedCombo2;
MIX_Audio* guard;
MIX_Audio* royalBlock;
MIX_Audio* normalBlock;
MIX_Audio* jdc;
MIX_Audio* jdcJustFrame;
MIX_Audio* jdcCharge;
MIX_Audio* driveStart;
MIX_Audio* driveLoop;
MIX_Audio* driveLevelUp;
MIX_Audio* snap;
MIX_Audio* ecstasyHit;
MIX_Audio* missionClearSong;
MIX_Audio* divinityStatueSong;
MIX_Audio* battleOfBrothersSong;

// A named set of mixer tracks. Voice selection, polyphony and per-frame
// spatialization live here, so call sites never compute an index.
//
//   Init          create voicesPerGroup * groups tracks; groups == 1 marks a
//                 global sound, which then ignores the player index
//   Play          voice 0, replacing whatever it held
//   PlayVoice     caller picks the voice; for polyphonic effects where several
//                 different files share a pool and must sound together
//                 (sprint L1/L2, one voice per style rank)
//   PlayFree      first idle voice, dropped when all are busy; a fadeOthersMs >= 0
//                 also fades the group's other voices
//   IsAnyPlaying  any voice of the group is playing
//   SetPosition   apply angle/distance to every voice of the group; no-op when the
//                 pool is not spatialized
//   Stop          stop the group, optionally faded
//   Pause/Resume  pause or resume the group
class SfxVoicePool {
public:
    void Init(const char* poolName, int voicesPerGroup, int groupCount = PLAYER_COUNT, bool spatialized = true);

    void Play(int group, MIX_Audio* audio, float volume, int loops = 0, int fadeInMs = 0);
    void PlayVoice(int group, int voice, MIX_Audio* audio, float volume, int loops = 0, int fadeInMs = 0);
    void PlayFree(int group, MIX_Audio* audio, float volume, int angle = 0, int distance = 0, int fadeOthersMs = -1);

    bool IsAnyPlaying(int group) const;
    void SetPosition(int group, int angle, int distance);
    void Stop(int group, int fadeMs = 0);
    void Pause(int group);
    void Resume(int group);

private:
    MIX_Track* Track(int group, int voice) const;
    int        Group(int group) const;

    const char* name    = "";
    int voicesPerGroup  = 1;
    int groups          = PLAYER_COUNT;
    bool spatialized    = true;
    std::vector<MIX_Track*> tracks;
};

SfxVoicePool poolDevilArm{};
SfxVoicePool poolChangeGun{};
SfxVoicePool poolStyleChange{};
SfxVoicePool poolStyleChangeVO{};
SfxVoicePool poolStyleRank{};
SfxVoicePool poolSprint{};
SfxVoicePool poolDevilTriggerIn{};
SfxVoicePool poolDevilTriggerOut{};
SfxVoicePool poolDevilTriggerLoop{};
SfxVoicePool poolDoppelgangerIn{};
SfxVoicePool poolDoppelgangerOut{};
SfxVoicePool poolQuicksilverIn{};
SfxVoicePool poolQuicksilverOut{};
SfxVoicePool poolDevilTriggerReady{};
SfxVoicePool poolDelayedCombo1{};
SfxVoicePool poolDelayedCombo2{};
SfxVoicePool poolDteStart{};
SfxVoicePool poolDteLoop{};
SfxVoicePool poolDteFinish{};
SfxVoicePool poolDteRelease{};
SfxVoicePool poolGuard{};
SfxVoicePool poolRoyalBlock{};
SfxVoicePool poolNormalBlock{};
SfxVoicePool poolJdc{};
SfxVoicePool poolJdcCharge{};
SfxVoicePool poolSnap{};
SfxVoicePool poolDrive{};
SfxVoicePool poolDriveLevelUp{};
SfxVoicePool poolDriveLoop{};
SfxVoicePool poolDriveClone{};
SfxVoicePool poolDriveLevelUpClone{};
SfxVoicePool poolDriveLoopClone{};
SfxVoicePool poolEcstasyHit{};

#define SDL_FUNCTION_DECLRATION(X) decltype(X)* fn_##X
#define LOAD_SDL_FUNCTION(X) fn_##X = GetSDLFunction<decltype(X)*>(#X)
#define LOAD_MIXER_FUNCTION(X) fn_##X = GetSDLMixerFunction<decltype(X)*>(#X)

SDL_FUNCTION_DECLRATION(SDL_Init)                         = NULL;
SDL_FUNCTION_DECLRATION(SDL_PollEvent)                    = NULL;
SDL_FUNCTION_DECLRATION(SDL_GetError)                     = NULL;
SDL_FUNCTION_DECLRATION(SDL_free)                         = NULL;
SDL_FUNCTION_DECLRATION(SDL_CreateProperties)             = NULL;
SDL_FUNCTION_DECLRATION(SDL_SetNumberProperty)            = NULL;
SDL_FUNCTION_DECLRATION(SDL_DestroyProperties)            = NULL;
SDL_FUNCTION_DECLRATION(SDL_GetGamepads)                  = NULL;
SDL_FUNCTION_DECLRATION(SDL_OpenGamepad)                  = NULL;
SDL_FUNCTION_DECLRATION(SDL_CloseGamepad)                 = NULL;
SDL_FUNCTION_DECLRATION(SDL_GetGamepadPlayerIndex)        = NULL;
SDL_FUNCTION_DECLRATION(SDL_GetGamepadJoystick)           = NULL;
SDL_FUNCTION_DECLRATION(SDL_GetGamepadButton)             = NULL;
SDL_FUNCTION_DECLRATION(SDL_GetGamepadID)                 = NULL;
SDL_FUNCTION_DECLRATION(SDL_UpdateGamepads)               = NULL;
SDL_FUNCTION_DECLRATION(SDL_GetGamepadName)               = NULL;
SDL_FUNCTION_DECLRATION(SDL_IsGamepad)                    = NULL;
SDL_FUNCTION_DECLRATION(SDL_RumbleGamepad)                = NULL;
SDL_FUNCTION_DECLRATION(SDL_GetGamepadPath)               = NULL;
SDL_FUNCTION_DECLRATION(SDL_GetGamepadPathForID)          = NULL;
SDL_FUNCTION_DECLRATION(SDL_GetGamepadAxis)               = NULL;
SDL_FUNCTION_DECLRATION(SDL_GetNumGamepadTouchpads)       = NULL;
SDL_FUNCTION_DECLRATION(SDL_GetGamepadTouchpadFinger)     = NULL;

SDL_FUNCTION_DECLRATION(MIX_Init)                         = NULL;
SDL_FUNCTION_DECLRATION(MIX_Quit)                         = NULL;
SDL_FUNCTION_DECLRATION(MIX_Version)                      = NULL;
SDL_FUNCTION_DECLRATION(MIX_CreateMixerDevice)            = NULL;
SDL_FUNCTION_DECLRATION(MIX_DestroyMixer)                 = NULL;
SDL_FUNCTION_DECLRATION(MIX_LoadAudio)                    = NULL;
SDL_FUNCTION_DECLRATION(MIX_DestroyAudio)                 = NULL;
SDL_FUNCTION_DECLRATION(MIX_CreateTrack)                  = NULL;
SDL_FUNCTION_DECLRATION(MIX_DestroyTrack)                 = NULL;
SDL_FUNCTION_DECLRATION(MIX_SetTrackAudio)                = NULL;
SDL_FUNCTION_DECLRATION(MIX_TagTrack)                     = NULL;
SDL_FUNCTION_DECLRATION(MIX_PlayTrack)                    = NULL;
SDL_FUNCTION_DECLRATION(MIX_StopTrack)                    = NULL;
SDL_FUNCTION_DECLRATION(MIX_StopTag)                      = NULL;
SDL_FUNCTION_DECLRATION(MIX_PauseTrack)                   = NULL;
SDL_FUNCTION_DECLRATION(MIX_ResumeTrack)                  = NULL;
SDL_FUNCTION_DECLRATION(MIX_TrackPlaying)                 = NULL;
SDL_FUNCTION_DECLRATION(MIX_SetTrackGain)                 = NULL;
SDL_FUNCTION_DECLRATION(MIX_SetTagGain)                   = NULL;
SDL_FUNCTION_DECLRATION(MIX_SetTrack3DPosition)           = NULL;
SDL_FUNCTION_DECLRATION(MIX_SetTrackStoppedCallback)      = NULL;
SDL_FUNCTION_DECLRATION(MIX_TrackMSToFrames)              = NULL;
SDL_FUNCTION_DECLRATION(MIX_GetAudioDuration)             = NULL;
SDL_FUNCTION_DECLRATION(MIX_AudioFramesToMS)              = NULL;
SDL_FUNCTION_DECLRATION(MIX_CreateAudioDecoder)           = NULL;
SDL_FUNCTION_DECLRATION(MIX_DecodeAudio)                  = NULL;
SDL_FUNCTION_DECLRATION(MIX_GetAudioDecoderFormat)        = NULL;
SDL_FUNCTION_DECLRATION(MIX_DestroyAudioDecoder)          = NULL;

constexpr const char* kMusicTag = "crimson.music";

// Mix_Volume clamped to 128; MIX_SetTrackGain is linear, so keep the same ceiling.
static float Gain(float volume128) {
    if (volume128 <= 0.0f) {
        return 0.0f;
    }
    if (volume128 > 128.0f) {
        volume128 = 128.0f;
    }

    return volume128 / 128.0f;
}

// Constant power panning holds left² + right² at 1.0, so a centred sound lands on
// 0.707 per channel; the sqrt(2) restores the level the linear pan produced.
static float GainSpatialized(float volume128) {
    return Gain(volume128 * 1.41421356f);
}

static bool TrackPlaying(MIX_Track* track) {
    return track != NULL && fn_MIX_TrackPlaying != NULL && fn_MIX_TrackPlaying(track);
}

static void PlayOnTrack(MIX_Track* track, MIX_Audio* audio, int loops, int fadeInMs) {
    if (track == NULL || audio == NULL || fn_MIX_SetTrackAudio == NULL || fn_MIX_PlayTrack == NULL) {
        return;
    }

    // Reused so per-hit sounds do not allocate a property set each time.
    if (g_playProperties == 0 && fn_SDL_CreateProperties != NULL) {
        g_playProperties = fn_SDL_CreateProperties();
    }
    if (g_playProperties != 0) {
        fn_SDL_SetNumberProperty(g_playProperties, MIX_PROP_PLAY_LOOPS_NUMBER, loops);
        fn_SDL_SetNumberProperty(g_playProperties, MIX_PROP_PLAY_FADE_IN_MILLISECONDS_NUMBER, fadeInMs);
    }

    fn_MIX_SetTrackAudio(track, audio);
    fn_MIX_PlayTrack(track, g_playProperties);
}

static void StopTrack(MIX_Track* track, int fadeOutMs) {
    if (track == NULL || fn_MIX_StopTrack == NULL) {
        return;
    }

    Sint64 fadeFrames = 0;
    if (fadeOutMs > 0 && fn_MIX_TrackMSToFrames != NULL) {
        fadeFrames = fn_MIX_TrackMSToFrames(track, fadeOutMs);
    }

    fn_MIX_StopTrack(track, fadeFrames);
}

// Pseudo-3D through the mixer's spatializer. Its listener is pinned at the origin
// facing -Z and it attenuates as 1/max(|position|, 1), so the distance curve rides
// in the magnitude and the direction drives constant power panning.
// angle: 0 = front, 90 = right, 180 = back, 270 = left.
static void SetTrackPan(MIX_Track* track, int angle, int distance) {
    if (track == NULL || fn_MIX_SetTrack3DPosition == NULL) {
        return;
    }

    float attenuation = 1.0f;
    if (distance > 0) {
        const float clamped = distance > 255 ? 255.0f : (float)distance;
        attenuation         = 1.0f - (clamped / 255.0f);
    }
    if (attenuation < 0.001f) {
        attenuation = 0.001f;
    }

    const float radius  = 1.0f / attenuation;
    const float radians = angle * (3.14159265f / 180.0f);
    MIX_Point3D position{ sinf(radians) * radius, 0.0f, -cosf(radians) * radius };
    fn_MIX_SetTrack3DPosition(track, &position);
}

void SfxVoicePool::Init(const char* poolName, int voicesPerGroupCount, int groupCount, bool spatializedVoices) {
    name           = poolName;
    voicesPerGroup = voicesPerGroupCount;
    groups         = groupCount;
    spatialized    = spatializedVoices;

    const int total = voicesPerGroup * groups;
    tracks.assign(total, NULL);
    if (g_mixer == NULL || fn_MIX_CreateTrack == NULL) {
        return;
    }

    for (int i = 0; i < total; ++i) {
        tracks[i] = fn_MIX_CreateTrack(g_mixer);
        if (tracks[i] != NULL && fn_MIX_TagTrack != NULL) {
            fn_MIX_TagTrack(tracks[i], "sfx");
        }
    }
}

int SfxVoicePool::Group(int group) const {
    // Single-group pools are global sounds and ignore the player index.
    if (groups <= 1) {
        return 0;
    }
    if (group < 0) {
        return 0;
    }

    return group >= groups ? groups - 1 : group;
}

MIX_Track* SfxVoicePool::Track(int group, int voice) const {
    if (voice < 0 || voice >= voicesPerGroup) {
        return NULL;
    }

    const int index = Group(group) * voicesPerGroup + voice;
    if (index < 0 || index >= (int)tracks.size()) {
        return NULL;
    }

    return tracks[index];
}

void SfxVoicePool::Play(int group, MIX_Audio* audio, float volume, int loops, int fadeInMs) {
    PlayVoice(group, 0, audio, volume, loops, fadeInMs);
}

void SfxVoicePool::PlayVoice(int group, int voice, MIX_Audio* audio, float volume, int loops, int fadeInMs) {
    MIX_Track* track = Track(group, voice);
    if (track == NULL || fn_MIX_SetTrackGain == NULL) {
        return;
    }

    fn_MIX_SetTrackGain(track, spatialized ? GainSpatialized(volume) : Gain(volume));
    PlayOnTrack(track, audio, loops, fadeInMs);
}

void SfxVoicePool::PlayFree(int group, MIX_Audio* audio, float volume, int angle, int distance, int fadeOthersMs) {
    if (fn_MIX_SetTrackGain == NULL) {
        return;
    }

    const int g = Group(group);
    for (int v = 0; v < voicesPerGroup; ++v) {
        MIX_Track* track = Track(g, v);
        if (track == NULL || TrackPlaying(track)) {
            continue;
        }

        fn_MIX_SetTrackGain(track, spatialized ? GainSpatialized(volume) : Gain(volume));
        if (spatialized) {
            SetTrackPan(track, angle, distance);
        }
        PlayOnTrack(track, audio, 0, 0);

        if (fadeOthersMs >= 0) {
            for (int other = 0; other < voicesPerGroup; ++other) {
                if (other != v) {
                    StopTrack(Track(g, other), fadeOthersMs);
                }
            }
        }
        return;
    }
}

bool SfxVoicePool::IsAnyPlaying(int group) const {
    const int g = Group(group);
    for (int v = 0; v < voicesPerGroup; ++v) {
        if (TrackPlaying(Track(g, v))) {
            return true;
        }
    }

    return false;
}

void SfxVoicePool::SetPosition(int group, int angle, int distance) {
    if (!spatialized) {
        return;
    }

    const int g = Group(group);
    for (int v = 0; v < voicesPerGroup; ++v) {
        SetTrackPan(Track(g, v), angle, distance);
    }
}

void SfxVoicePool::Stop(int group, int fadeMs) {
    const int g = Group(group);
    for (int v = 0; v < voicesPerGroup; ++v) {
        StopTrack(Track(g, v), fadeMs);
    }
}

void SfxVoicePool::Pause(int group) {
    if (fn_MIX_PauseTrack == NULL) {
        return;
    }

    const int g = Group(group);
    for (int v = 0; v < voicesPerGroup; ++v) {
        MIX_Track* track = Track(g, v);
        if (track != NULL) {
            fn_MIX_PauseTrack(track);
        }
    }
}

void SfxVoicePool::Resume(int group) {
    if (fn_MIX_ResumeTrack == NULL) {
        return;
    }

    const int g = Group(group);
    for (int v = 0; v < voicesPerGroup; ++v) {
        MIX_Track* track = Track(g, v);
        if (track != NULL) {
            fn_MIX_ResumeTrack(track);
        }
    }
}

void InitAudioPools() {
    poolDevilArm.Init("devilArm", 20, 1);
    poolChangeGun.Init("changeGun", 20, 1);
    poolStyleChange.Init("styleChange", 20);
    poolStyleChangeVO.Init("styleChangeVO", 20);
    poolStyleRank.Init("styleRank", 7, 1, false);
    poolSprint.Init("sprint", 2);
    poolDevilTriggerIn.Init("devilTriggerIn", 2);
    poolDevilTriggerOut.Init("devilTriggerOut", 1);
    poolDevilTriggerLoop.Init("devilTriggerLoop", 1);
    poolDoppelgangerIn.Init("doppelgangerIn", 1);
    poolDoppelgangerOut.Init("doppelgangerOut", 1);
    poolQuicksilverIn.Init("quicksilverIn", 1, 1);
    poolQuicksilverOut.Init("quicksilverOut", 1, 1);
    poolDevilTriggerReady.Init("devilTriggerReady", 1);
    poolDelayedCombo1.Init("delayedCombo1", 1);
    poolDelayedCombo2.Init("delayedCombo2", 1);
    poolDteStart.Init("dteStart", 1);
    poolDteLoop.Init("dteLoop", 1);
    poolDteFinish.Init("dteFinish", 1);
    poolDteRelease.Init("dteRelease", 1);
    poolGuard.Init("guard", 2);
    poolRoyalBlock.Init("royalBlock", 5);
    poolNormalBlock.Init("normalBlock", 5);
    poolJdc.Init("jdc", 10);
    poolJdcCharge.Init("jdcCharge", 1);
    poolSnap.Init("snap", 10);
    poolDrive.Init("drive", 1);
    poolDriveLevelUp.Init("driveLevelUp", 2);
    poolDriveLoop.Init("driveLoop", 1);
    poolDriveClone.Init("driveClone", 1);
    poolDriveLevelUpClone.Init("driveLevelUpClone", 2);
    poolDriveLoopClone.Init("driveLoopClone", 1);
    poolEcstasyHit.Init("ecstasyHit", 2);
}

static MIX_Audio* LoadAudioFile(const char* path, bool predecode) {
    if (g_mixer == NULL || fn_MIX_LoadAudio == NULL) {
        return NULL;
    }

    MIX_Audio* audio = fn_MIX_LoadAudio(g_mixer, path, predecode);
    if (audio == NULL) {
        Log("MIX_LoadAudio failed: %s", path);
    }

    return audio;
}

static void ApplyMusicGain() {
    if (g_mixer != NULL && fn_MIX_SetTagGain != NULL) {
        fn_MIX_SetTagGain(g_mixer, kMusicTag, g_musicGain * g_musicDuck);
    }
}

static void PlayMusic(MIX_Audio* song, int fadeInMs) {
    if (song == NULL) {
        return;
    }
    if (g_musicTracks[0] == NULL && g_musicTracks[1] == NULL) {
        return;
    }

    MIX_Track* track = NULL;
    for (int i = 0; i < 2; ++i) {
        const int index = (g_nextMusicTrack + i) % 2;
        if (!TrackPlaying(g_musicTracks[index])) {
            track = g_musicTracks[index];
            g_nextMusicTrack = (index + 1) % 2;
            break;
        }
    }
    if (track == NULL) {
        track             = g_musicTracks[g_nextMusicTrack];
        g_nextMusicTrack  = (g_nextMusicTrack + 1) % 2;
    }

    ApplyMusicGain();
    PlayOnTrack(track, song, -1, fadeInMs);
}

static MIX_Audio* LoadSfx(const char* path) {
    return LoadAudioFile(path, true);
}

static MIX_Audio* LoadMusic(const char* path) {
    return LoadAudioFile(path, false);
}

// The switcher plays on its own pair of tracks so it never steals the slot a
// mission-clear or shop jingle is using. They still carry the music tag, so the
// mixer's tag gain (pause duck) applies to them.
static MIX_Track* g_switcherTracks[2] = {NULL, NULL};
static int g_switcherNext = 0;
static SDL_PropertiesID g_switcherProperties = 0;

static bool PlaySwitcherOnTrack(MIX_Track* track, MIX_Audio* audio, int32 startMs, int32 fadeInMs,
    float gain) {
    if (track == NULL || audio == NULL || fn_MIX_SetTrackAudio == NULL || fn_MIX_PlayTrack == NULL ||
        fn_SDL_CreateProperties == NULL || fn_SDL_SetNumberProperty == NULL) {
        return false;
    }

    // A separate property set: a start offset left behind here would make the
    // next sound effect begin mid-file.
    if (g_switcherProperties == 0) {
        g_switcherProperties = fn_SDL_CreateProperties();
    }
    if (g_switcherProperties == 0) {
        return false;
    }

    if (startMs < 0) {
        startMs = 0;
    }

    fn_SDL_SetNumberProperty(g_switcherProperties, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
    fn_SDL_SetNumberProperty(g_switcherProperties, MIX_PROP_PLAY_FADE_IN_MILLISECONDS_NUMBER, fadeInMs);
    fn_SDL_SetNumberProperty(g_switcherProperties, MIX_PROP_PLAY_START_MILLISECOND_NUMBER, startMs);

    if (fn_MIX_SetTrackGain != NULL) {
        fn_MIX_SetTrackGain(track, gain);
    }

    fn_MIX_SetTrackAudio(track, audio);
    return fn_MIX_PlayTrack(track, g_switcherProperties);
}

// Music is streamed, so a switcher would otherwise reload the same file on
// every crossfade.
static MIX_Audio* LoadMusicCached(const std::string& path) {
    auto it = g_audioCache.find(path);
    if (it != g_audioCache.end()) {
        return it->second;
    }

    MIX_Audio* audio = LoadMusic(path.c_str());
    if (audio != NULL) {
        g_audioCache[path] = audio;
    }

    return audio;
}

void LoadAllSFX() {
	if (!cacheAudioFiles) {

		changeGun = LoadSfx(((std::string)Paths::sounds + "\\changegun.wav").c_str());
		changeDevilArm = LoadSfx(((std::string)Paths::sounds + "\\changedevilarm.wav").c_str());
		changeWeaponDMC3 = LoadSfx(((std::string)Paths::sounds + "\\changeweapondmc3.wav").c_str());
		styleChange = LoadSfx(((std::string)Paths::sounds + "\\stylechange.wav").c_str());
		tricksterVO = LoadSfx(((std::string)Paths::sounds + "\\trickster1.wav").c_str());
		swordmasterVO = LoadSfx(((std::string)Paths::sounds + "\\swordmaster1.wav").c_str());
		gunslingerVO = LoadSfx(((std::string)Paths::sounds + "\\gunslinger1.wav").c_str());
		royalguardVO = LoadSfx(((std::string)Paths::sounds + "\\royalguard1.wav").c_str());
		quicksilverVO = LoadSfx(((std::string)Paths::sounds + "\\quicksilver1.wav").c_str());
		doppelganger1VO = LoadSfx(((std::string)Paths::sounds + "\\doppelganger1.wav").c_str());
        doppelganger2VO = LoadSfx(((std::string)Paths::sounds + "\\doppelganger2.wav").c_str());
		sprintL1 = LoadSfx(((std::string)Paths::sounds + "\\sprint_l1.wav").c_str());
		sprintL2 = LoadSfx(((std::string)Paths::sounds + "\\sprint_l2.wav").c_str());
		devilTriggerInL1 = LoadSfx(((std::string)Paths::sounds + "\\dt_activation_l1.wav").c_str());
		devilTriggerInL2 = LoadSfx(((std::string)Paths::sounds + "\\dt_activation_l2.wav").c_str());
		devilTriggerOut = LoadSfx(((std::string)Paths::sounds + "\\dt_deactivation.wav").c_str());
		devilTriggerLoop = LoadSfx(((std::string)Paths::sounds + "\\dt_loop.wav").c_str());
		dtExplosionStart = LoadSfx(((std::string)Paths::sounds + "\\dte_start.wav").c_str());
		dtExplosionLoop = LoadSfx(((std::string)Paths::sounds + "\\dte_loop.wav").c_str());
		dtExplosionFinish = LoadSfx(((std::string)Paths::sounds + "\\dte_finish.wav").c_str());
		dtExplosionRelease = LoadSfx(((std::string)Paths::sounds + "\\dte_release.wav").c_str());
		doppelgangerIn = LoadSfx(((std::string)Paths::sounds + "\\dopp_activation.wav").c_str());
		doppelgangerOut = LoadSfx(((std::string)Paths::sounds + "\\dopp_deactivation.wav").c_str());
		quicksilverIn = LoadSfx(((std::string)Paths::sounds + "\\qs_activation.wav").c_str());
        quicksilverOut = LoadSfx(((std::string)Paths::sounds + "\\qs_deactivation.wav").c_str());
		devilTriggerReady = LoadSfx(((std::string)Paths::sounds + "\\dt_ready.wav").c_str());
		styleRankD1 = LoadSfx(((std::string)Paths::sounds + "\\styleranks\\d1.wav").c_str());
		styleRankD2 = LoadSfx(((std::string)Paths::sounds + "\\styleranks\\d2.wav").c_str());
		styleRankC1 = LoadSfx(((std::string)Paths::sounds + "\\styleranks\\c1.wav").c_str());
		styleRankC2 = LoadSfx(((std::string)Paths::sounds + "\\styleranks\\c2.wav").c_str());
		styleRankB1 = LoadSfx(((std::string)Paths::sounds + "\\styleranks\\b1.wav").c_str());
		styleRankB2 = LoadSfx(((std::string)Paths::sounds + "\\styleranks\\b2.wav").c_str());
		styleRankA1 = LoadSfx(((std::string)Paths::sounds + "\\styleranks\\a1.wav").c_str());
		styleRankA2 = LoadSfx(((std::string)Paths::sounds + "\\styleranks\\a2.wav").c_str());
		styleRankS1 = LoadSfx(((std::string)Paths::sounds + "\\styleranks\\s1.wav").c_str());
		styleRankS2 = LoadSfx(((std::string)Paths::sounds + "\\styleranks\\s2.wav").c_str());
		styleRankSS1 = LoadSfx(((std::string)Paths::sounds + "\\styleranks\\ss1.wav").c_str());
		styleRankSS2 = LoadSfx(((std::string)Paths::sounds + "\\styleranks\\ss2.wav").c_str());
		styleRankSSS1 = LoadSfx(((std::string)Paths::sounds + "\\styleranks\\sss1.wav").c_str());
		styleRankSSS2 = LoadSfx(((std::string)Paths::sounds + "\\styleranks\\sss2.wav").c_str());
		delayedCombo1 = LoadSfx(((std::string)Paths::sounds + "\\delayedcombo1.wav").c_str());
		delayedCombo2 = LoadSfx(((std::string)Paths::sounds + "\\delayedcombo2.wav").c_str());
        guard = LoadSfx(((std::string)Paths::sounds + "\\guard.wav").c_str());
        royalBlock = LoadSfx(((std::string)Paths::sounds + "\\blockroyal.wav").c_str());
        normalBlock = LoadSfx(((std::string)Paths::sounds + "\\block.wav").c_str());
		jdc = LoadSfx(((std::string)Paths::sounds + "\\jdc.wav").c_str());
		jdcJustFrame = LoadSfx(((std::string)Paths::sounds + "\\jdc_justframe.wav").c_str());
		jdcCharge = LoadSfx(((std::string)Paths::sounds + "\\jdc_charge.wav").c_str());
		driveStart = LoadSfx(((std::string)Paths::sounds + "\\drive_start.wav").c_str());
		driveLoop = LoadSfx(((std::string)Paths::sounds + "\\drive_loop.wav").c_str());
		driveLevelUp = LoadSfx(((std::string)Paths::sounds + "\\drive_levelup.wav").c_str());
		snap = LoadSfx(((std::string)Paths::sounds + "\\snap.wav").c_str());
		ecstasyHit = LoadSfx(((std::string)Paths::sounds + "\\ecstasy_hit.wav").c_str());


		missionClearSong = LoadMusic(((std::string)Paths::sounds + "\\music\\missionclear.mp3").c_str());
        divinityStatueSong = LoadMusic(((std::string)Paths::sounds + "\\music\\divinitystatue.mp3").c_str());
		battleOfBrothersSong = LoadMusic(((std::string)Paths::sounds + "\\music\\battleofbrothers.mp3").c_str());


		cacheAudioFiles = true;
	}
}

// Forward declarations for controller management functions
void RemapSdlControllers();

void AddController(SDL_JoystickID instanceID) {
	if (fn_SDL_IsGamepad == NULL || fn_SDL_OpenGamepad == NULL) return;
	if (fn_SDL_IsGamepad(instanceID)) {
		SDL_Gamepad* gamepad = fn_SDL_OpenGamepad(instanceID);
		if (gamepad) {
			currentlyConnected.insert(instanceID);
			bool assigned = false;
			for (auto& ctrl : controllers) {
				if (ctrl == NULL) {
					ctrl = gamepad;
					assigned = true;
					break;
				}
			}
			if (!assigned) {
				std::cerr << "Too many controllers connected. Ignoring controller " << instanceID << "\n";
				fn_SDL_CloseGamepad(gamepad);
			}
			else {
				const char* name = (fn_SDL_GetGamepadName != NULL) ? fn_SDL_GetGamepadName(gamepad) : "Unknown";
				std::cout << "Opened controller " << instanceID << ": " << name << std::endl;
			}
			RemapSdlControllers();
		}
		else {
			const char* err = (fn_SDL_GetError != NULL) ? fn_SDL_GetError() : "Unknown error";
			std::cerr << "Could not open controller " << instanceID << ": " << err << std::endl;
		}
	}
}

void RemoveController(SDL_JoystickID instanceID) {
	if (fn_SDL_GetGamepadID == NULL || fn_SDL_CloseGamepad == NULL) return;
	for (int i = 0; i < controllers.size(); ++i) {
		if (controllers[i] && fn_SDL_GetGamepadID(controllers[i]) == instanceID) {
			fn_SDL_CloseGamepad(controllers[i]);
			controllers[i] = NULL;
			currentlyConnected.erase(instanceID);
			std::cout << "Closed controller at index " << i << "  instanceID: " << instanceID << "\n";
			break;
		}
	}
	RemapSdlControllers();
}

void InitControllers() {
	if (fn_SDL_GetGamepads == NULL || fn_SDL_free == NULL) return;
	int count;
	SDL_JoystickID* joysticks = fn_SDL_GetGamepads(&count);
	if (joysticks) {
		for (int i = 0; i < count; ++i) {
			AddController(joysticks[i]);
		}
		fn_SDL_free(joysticks);
	}
}

// Normalize a HID device path for cross-API comparison.
// Lowercases and strips ALL HID collection filter segments (e.g. "&IG_00",
// "&MI_03", "&COL01") between the PID and the instance ID separator.
// This makes paths from different SDL backends (XInput vs HIDAPI vs WGI)
// comparable, while preserving the unique device instance ID.
static std::string NormalizeHidPath(const char* path) {
	if (!path || !path[0]) return {};

	std::string result(path);
	// Lowercase
	for (auto& c : result) c = (char)tolower((unsigned char)c);

	// Find "pid_" to locate the PID segment
	size_t pidPos = result.find("pid_");
	if (pidPos == std::string::npos) return result;

	// PID is 4 hex digits: "pid_XXXX" → end at pidPos + 8
	size_t pidEnd = pidPos + 8;
	if (pidEnd > result.size()) return result;

	// Find the next '#' after the PID (start of instance container ID)
	size_t hashPos = result.find('#', pidEnd);
	if (hashPos == std::string::npos) return result;

	// Strip all filter metadata between PID digits and the '#'
	// e.g. "...&pid_09cc&ig_00&mi_03#container..." → "...&pid_09cc#container..."
	result.erase(pidEnd, hashPos - pidEnd);

	return result;
}

// Returns the XInput slot (0-3) that corresponds to the given SDL gamepad path,
// or -1 if the path doesn't match any XInput device.
//
// SDL3 returns different path formats depending on the backend:
//   XInput backend:  "XInput#0", "XInput#1", etc.
//   HIDAPI backend:  "\\?\HID#VID_XXXX&PID_XXXX&...#instance#..."
// This function handles both.
int GetXInputSlotForHIDPath(const char* sdlPath) {
	if (!sdlPath || !sdlPath[0]) return -1;

	// --- Fast path: SDL XInput backend returns "XInput#N" ---
	{
		std::string lower(sdlPath);
		for (auto& c : lower) c = (char)tolower((unsigned char)c);

		// Match "xinput#N" where N is a digit 0-3
		const char* prefix = "xinput#";
		size_t prefixLen = 7;
		if (lower.compare(0, prefixLen, prefix) == 0 && lower.size() > prefixLen) {
			char digit = lower[prefixLen];
			if (digit >= '0' && digit <= '3') {
				return (int)(digit - '0');
			}
		}
		// Match "xinput controller N" (alternative format)
		const char* prefixAlt = "xinput controller ";
		size_t prefixAltLen = 18;
		if (lower.compare(0, prefixAltLen, prefixAlt) == 0 && lower.size() > prefixAltLen) {
			char digit = lower[prefixAltLen];
			if (digit >= '0' && digit <= '3') {
				return (int)(digit - '0');
			}
		}
	}

	// --- Slow path: HIDAPI backend returns HID device paths ---
	// Normalize the SDL path (strips vendor filter segments)
	std::string sdlPathNormalized = NormalizeHidPath(sdlPath);
	// If the path doesn't look like a HID path (no "vid_"), skip RawInput matching
	if (sdlPathNormalized.find("vid_") == std::string::npos)
		return -1;

	UINT count = 0;
	if (GetRawInputDeviceList(nullptr, &count, sizeof(RAWINPUTDEVICELIST)) != 0 || count == 0)
		return -1;

	std::vector<RAWINPUTDEVICELIST> list(count);
	if (GetRawInputDeviceList(list.data(), &count, sizeof(RAWINPUTDEVICELIST)) == (UINT)-1)
		return -1;

	DWORD xiSlot = 0;
	for (const auto& dev : list) {
		if (dev.dwType != RIM_TYPEHID) continue;

		UINT len = 0;
		GetRawInputDeviceInfoA(dev.hDevice, RIDI_DEVICENAME, nullptr, &len);
		if (len == 0) continue;

		std::string path(len, '\0');
		if (GetRawInputDeviceInfoA(dev.hDevice, RIDI_DEVICENAME, path.data(), &len) == (UINT)-1)
			continue;

		if (!path.empty() && path.back() == '\0') path.pop_back();

		// Only XInput HID devices contain "IG_" in their path
		if (path.find("IG_") == std::string::npos) continue;

		std::string xiPathNormalized = NormalizeHidPath(path.c_str());

		if (sdlPathNormalized == xiPathNormalized) {
			return (int)xiSlot;
		}

		if (++xiSlot >= 4) break;
	}

	return -1;
}

// Returns a normalized HID path that uniquely identifies the physical device
// behind a gamepad, enabling cross-backend deduplication.
// - XInput-backend pads (path "XInput#N") are resolved via RawInput to their
//   underlying HID path, then normalized.
// - HIDAPI / WGI pads are normalized directly via NormalizeHidPath.
// Returns empty string if the device cannot be identified.
static std::string GetNormalizedPathForPad(SDL_Gamepad* pad) {
	if (!pad || !fn_SDL_GetGamepadPath) return {};

	const char* path = fn_SDL_GetGamepadPath(pad);
	if (!path || !path[0]) return {};

	std::string lower(path);
	for (auto& c : lower) c = (char)tolower((unsigned char)c);

	// --- XInput backend: resolve virtual slot to underlying HID path ---
	if (lower.compare(0, 7, "xinput#") == 0 && lower.size() > 7) {
		int xiSlot = lower[7] - '0';
		if (xiSlot < 0 || xiSlot >= 4) return {};

		UINT count = 0;
		if (GetRawInputDeviceList(nullptr, &count, sizeof(RAWINPUTDEVICELIST)) != 0 || count == 0)
			return {};

		std::vector<RAWINPUTDEVICELIST> list(count);
		if (GetRawInputDeviceList(list.data(), &count, sizeof(RAWINPUTDEVICELIST)) == (UINT)-1)
			return {};

		DWORD slot = 0;
		for (const auto& dev : list) {
			if (dev.dwType != RIM_TYPEHID) continue;

			UINT len = 0;
			GetRawInputDeviceInfoA(dev.hDevice, RIDI_DEVICENAME, nullptr, &len);
			if (len == 0) continue;

			std::string xiPath(len, '\0');
			if (GetRawInputDeviceInfoA(dev.hDevice, RIDI_DEVICENAME, xiPath.data(), &len) == (UINT)-1)
				continue;

			if (!xiPath.empty() && xiPath.back() == '\0') xiPath.pop_back();
			if (xiPath.find("IG_") == std::string::npos) continue;

			if (slot == (DWORD)xiSlot) {
				return NormalizeHidPath(xiPath.c_str());
			}

			if (++slot >= 4) break;
		}
		return {};
	}

	// "xinput controller N" alternative format
	if (lower.compare(0, 18, "xinput controller ") == 0 && lower.size() > 18) {
		int xiSlot = lower[18] - '0';
		if (xiSlot < 0 || xiSlot >= 4) return {};
		// Same resolution as above; inline for simplicity
		UINT count = 0;
		if (GetRawInputDeviceList(nullptr, &count, sizeof(RAWINPUTDEVICELIST)) != 0 || count == 0)
			return {};

		std::vector<RAWINPUTDEVICELIST> list(count);
		if (GetRawInputDeviceList(list.data(), &count, sizeof(RAWINPUTDEVICELIST)) == (UINT)-1)
			return {};

		DWORD slot = 0;
		for (const auto& dev : list) {
			if (dev.dwType != RIM_TYPEHID) continue;

			UINT len = 0;
			GetRawInputDeviceInfoA(dev.hDevice, RIDI_DEVICENAME, nullptr, &len);
			if (len == 0) continue;

			std::string xiPath(len, '\0');
			if (GetRawInputDeviceInfoA(dev.hDevice, RIDI_DEVICENAME, xiPath.data(), &len) == (UINT)-1)
				continue;

			if (!xiPath.empty() && xiPath.back() == '\0') xiPath.pop_back();
			if (xiPath.find("IG_") == std::string::npos) continue;

			if (slot == (DWORD)xiSlot) {
				return NormalizeHidPath(xiPath.c_str());
			}

			if (++slot >= 4) break;
		}
		return {};
	}

	// --- HIDAPI / WGI / other backends: normalize directly ---
	return NormalizeHidPath(path);
}

void RemapSdlControllers() {
	// Clear existing mapping
	for (int i = 0; i < 4; ++i) sdlGamepadByXiSlot[i] = NULL;
	sdlGamepadsExtra.clear();

	if (fn_SDL_GetGamepadPath == NULL) return;

	// First pass: place XInput-matching controllers into their slots
	for (SDL_Gamepad* pad : controllers) {
		if (pad == NULL) continue;

		const char* path = fn_SDL_GetGamepadPath(pad);
		if (!path) continue;

		int xiSlot = GetXInputSlotForHIDPath(path);

		if (xiSlot >= 0 && xiSlot < 4) {
			if (sdlGamepadByXiSlot[xiSlot] == NULL) {
				sdlGamepadByXiSlot[xiSlot] = pad;
			} else {
				// Slot already occupied; store as extra
				sdlGamepadsExtra.push_back(pad);
			}
		}
	}

	// Second pass: all remaining pads go to extras.
	// Duplicates (same physical device via different SDL backends) are
	// filtered out by the deduplication pass below.
	for (SDL_Gamepad* pad : controllers) {
		if (pad == NULL) continue;

		const char* path = fn_SDL_GetGamepadPath(pad);
		if (!path) continue;

		int xiSlot = GetXInputSlotForHIDPath(path);
		if (xiSlot >= 0 && xiSlot < 4 && sdlGamepadByXiSlot[xiSlot] == pad) {
			continue; // Already placed in XInput slot
		}
		if (xiSlot >= 0 && xiSlot < 4 && sdlGamepadByXiSlot[xiSlot] != NULL) {
			continue; // XInput slot occupied by another pad; already pushed to extras
		}

		// Non-XInput controller (or native duplicate of an XInput controller)
		sdlGamepadsExtra.push_back(pad);
	}

	// === Deduplication pass ===
	// Remove controllers from sdlGamepadsExtra that represent the same
	// physical device as one already mapped in sdlGamepadByXiSlot, or that
	// appear multiple times within extras itself.
	// Uses normalized HID paths (preserving device instance ID) so two
	// genuinely different controllers of the same model are NOT conflated.
	{
		std::unordered_set<std::string> seenPaths;

		// Seed with normalized paths of XInput-slot controllers
		for (int i = 0; i < 4; ++i) {
			if (sdlGamepadByXiSlot[i] != NULL) {
				std::string np = GetNormalizedPathForPad(sdlGamepadByXiSlot[i]);
				if (!np.empty()) seenPaths.insert(np);
			}
		}

		// Filter extras: keep first occurrence, skip duplicates
		std::vector<SDL_Gamepad*> deduped;
		for (SDL_Gamepad* pad : sdlGamepadsExtra) {
			if (pad == NULL) continue;
			std::string np = GetNormalizedPathForPad(pad);
			if (np.empty()) {
				// Cannot identify; keep it (safer than losing a controller)
				deduped.push_back(pad);
			} else if (seenPaths.find(np) == seenPaths.end()) {
				seenPaths.insert(np);
				deduped.push_back(pad);
			}
			// else: duplicate — skip
		}
		sdlGamepadsExtra = std::move(deduped);
	}

	// === Player auto-assignment ===
	// For each player whose configured physical XInput slot has no
	// controller, assign the next available SDL controller (sentinel ≥ 4).
	// Pre-scans already-assigned sentinels so that multiple invocations of
	// RemapSdlControllers (e.g. during init) never give two players the
	// same SDL controller.
	// Does NOT move SDL controllers into XInput slots.
	{
		size_t sdlCount = sdlGamepadsExtra.size();

		// First, detect which XInput slots (0-3) have physical controllers
		// connected — either via SDL or the real XInput API. This prevents
		// SDL controllers from stealing slots that belong to XInput
		// controllers the game hasn't assigned to players yet.
		// Uses the unhooked XI::new_XInputGetState so that subsequent
		// hotplug calls (when the XInputGetState hook is live) still
		// query real hardware instead of being remapped through
		// xinputSlots[] sentinels.
		bool xiSlotOccupied[4] = {};
		for (int slot = 0; slot < 4; ++slot) {
			if (sdlGamepadByXiSlot[slot] != NULL) {
				xiSlotOccupied[slot] = true;
			} else if (XI::new_XInputGetState != NULL) {
				XINPUT_STATE state;
				if (XI::new_XInputGetState((DWORD)slot, &state) == ERROR_SUCCESS) {
					xiSlotOccupied[slot] = true;
				}
			}
		}

		// Track which SDL indices are already claimed by any player
		bool sentinelUsed[8] = {}; // up to 8 SDL extras
		for (int p = 0; p < PLAYER_COUNT; ++p) {
			uint8 slot = activeCrimsonInput.xinputSlots[p];
			if (slot >= 4 && (size_t)(slot - 4) < sdlCount) {
				sentinelUsed[slot - 4] = true; // still valid
			} else if (slot >= 4) {
				// Sentinel points to a removed SDL controller; reset to dead
				activeCrimsonInput.xinputSlots[p] = 0;
				queuedCrimsonInput.xinputSlots[p] = 0;
			}
		}

		// Assign unused SDL controllers to players with dead XInput slots
		int nextSdl = 0;
		for (int p = 0; p < PLAYER_COUNT; ++p) {
			uint8 slot = activeCrimsonInput.xinputSlots[p];
			bool dead = (slot < 4 && !xiSlotOccupied[slot]);
			if (dead) {
				while (nextSdl < (int)sdlCount && sentinelUsed[nextSdl]) ++nextSdl;
				if (nextSdl < (int)sdlCount) {
					activeCrimsonInput.xinputSlots[p] = (uint8)(4 + nextSdl);
					queuedCrimsonInput.xinputSlots[p] = (uint8)(4 + nextSdl);
					sentinelUsed[nextSdl] = true;
					++nextSdl;
				}
			}
		}
	}

	// Log controller mapping summary (after all processing)
	{
		int xiCount = 0;
		for (int i = 0; i < 4; ++i) {
			if (sdlGamepadByXiSlot[i] != NULL) {
				++xiCount;
				const char* name = (fn_SDL_GetGamepadName != NULL)
					? fn_SDL_GetGamepadName(sdlGamepadByXiSlot[i]) : "?";
				Log("RemapSdlControllers: XInput slot %d -> \"%s\"", i, name);
			}
		}
		for (size_t i = 0; i < sdlGamepadsExtra.size(); ++i) {
			const char* name = (fn_SDL_GetGamepadName != NULL)
				? fn_SDL_GetGamepadName(sdlGamepadsExtra[i]) : "?";
			Log("RemapSdlControllers: SDL[%zu]        -> \"%s\"", i, name);
		}
		Log("RemapSdlControllers: %d XInput + %zu SDL = %zu total",
			xiCount, sdlGamepadsExtra.size(), (size_t)xiCount + sdlGamepadsExtra.size());
	}
}

SDL_Gamepad* GetControllerForPlayer(int playerIndex) {
	if (playerIndex < 0 || playerIndex >= 4) return NULL;
	int xiSlot = (int)activeCrimsonInput.xinputSlots[playerIndex];
	// Sentinel values ≥ 4 indicate SDL slot N (where N = xiSlot - 4)
	if (xiSlot >= 4) {
		size_t extrasIdx = (size_t)(xiSlot - 4);
		if (extrasIdx < sdlGamepadsExtra.size())
			return sdlGamepadsExtra[extrasIdx];
		return NULL;
	}
	if (xiSlot >= 0 && xiSlot < 4 && sdlGamepadByXiSlot[xiSlot] != NULL) {
		return sdlGamepadByXiSlot[xiSlot];
	}
	return NULL;
}

SDL_Gamepad* GetControllerByPhysicalSlot(int xiSlot) {
	if (xiSlot < 0 || xiSlot >= 4) return NULL;
	return sdlGamepadByXiSlot[xiSlot];
}

const char* GetControllerNameForXInputSlot(int xiSlot) {
	if (xiSlot >= 0 && xiSlot < 4) {
		SDL_Gamepad* pad = sdlGamepadByXiSlot[xiSlot];
		if (pad != NULL && fn_SDL_GetGamepadName != NULL) {
			return fn_SDL_GetGamepadName(pad);
		}
		return NULL;
	}
	// Sentinel values ≥ 4 indicate SDL slot N (where N = xiSlot - 4)
	if (xiSlot >= 4 && !sdlGamepadsExtra.empty()) {
		size_t extrasIdx = (size_t)(xiSlot - 4);
		if (extrasIdx < sdlGamepadsExtra.size() && fn_SDL_GetGamepadName != NULL) {
			return fn_SDL_GetGamepadName(sdlGamepadsExtra[extrasIdx]);
		}
	}
	return NULL;
}

bool IsNativeControllerConnected() {
	return !sdlGamepadsExtra.empty();
}

bool IsNativeControllerButtonDown(int button) {
	if (fn_SDL_GetGamepadButton == NULL) return false;
	for (SDL_Gamepad* pad : sdlGamepadsExtra) {
		if (pad != NULL && fn_SDL_GetGamepadButton(pad, (SDL_GamepadButton)button)) {
			return true;
		}
	}
	return false;
}

// Populate an XINPUT_STATE from an SDL gamepad.
// Returns true if the state was successfully populated.
bool PopulateXInputStateFromSDL(SDL_Gamepad* pad, XINPUT_STATE* pState) {
	if (pad == NULL || pState == NULL || fn_SDL_GetGamepadButton == NULL) return false;

	ZeroMemory(pState, sizeof(XINPUT_STATE));

	static DWORD s_packetNumber = 0;
	pState->dwPacketNumber = ++s_packetNumber;

	WORD& btns = pState->Gamepad.wButtons;
	auto btn = [&](SDL_GamepadButton b) { return fn_SDL_GetGamepadButton(pad, b); };

	if (btn(SDL_GAMEPAD_BUTTON_DPAD_UP))         btns |= XINPUT_GAMEPAD_DPAD_UP;
	if (btn(SDL_GAMEPAD_BUTTON_DPAD_DOWN))       btns |= XINPUT_GAMEPAD_DPAD_DOWN;
	if (btn(SDL_GAMEPAD_BUTTON_DPAD_LEFT))       btns |= XINPUT_GAMEPAD_DPAD_LEFT;
	if (btn(SDL_GAMEPAD_BUTTON_DPAD_RIGHT))      btns |= XINPUT_GAMEPAD_DPAD_RIGHT;
	if (btn(SDL_GAMEPAD_BUTTON_START))           btns |= XINPUT_GAMEPAD_START;
	if (btn(SDL_GAMEPAD_BUTTON_BACK))            btns |= XINPUT_GAMEPAD_BACK;
	if (btn(SDL_GAMEPAD_BUTTON_LEFT_STICK))      btns |= XINPUT_GAMEPAD_LEFT_THUMB;
	if (btn(SDL_GAMEPAD_BUTTON_RIGHT_STICK))     btns |= XINPUT_GAMEPAD_RIGHT_THUMB;
	if (btn(SDL_GAMEPAD_BUTTON_LEFT_SHOULDER))   btns |= XINPUT_GAMEPAD_LEFT_SHOULDER;
	if (btn(SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER))  btns |= XINPUT_GAMEPAD_RIGHT_SHOULDER;
	if (btn(SDL_GAMEPAD_BUTTON_SOUTH))           btns |= XINPUT_GAMEPAD_A;
	if (btn(SDL_GAMEPAD_BUTTON_EAST))            btns |= XINPUT_GAMEPAD_B;
	if (btn(SDL_GAMEPAD_BUTTON_WEST))            btns |= XINPUT_GAMEPAD_X;
	if (btn(SDL_GAMEPAD_BUTTON_NORTH))           btns |= XINPUT_GAMEPAD_Y;

	// Axes (thumb sticks and triggers)
	if (fn_SDL_GetGamepadAxis != NULL) {
		pState->Gamepad.sThumbLX = fn_SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_LEFTX);
		// SDL Y-axis is inverted vs XInput (SDL: up=-, XInput: up=+).
		// Negate, but handle -32768 which can't be negated in Sint16.
		Sint16 ly = fn_SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_LEFTY);
		Sint16 ry = fn_SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_RIGHTY);
		pState->Gamepad.sThumbLY = (ly == -32768) ? 32767 : -ly;
		pState->Gamepad.sThumbRY = (ry == -32768) ? 32767 : -ry;
		pState->Gamepad.sThumbRX = fn_SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_RIGHTX);

		// SDL triggers: 0-32767, XInput triggers: 0-255
		Sint16 lt = fn_SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER);
		Sint16 rt = fn_SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER);
		pState->Gamepad.bLeftTrigger  = (BYTE)((lt > 0) ? (lt * 255 / 32767) : 0);
		pState->Gamepad.bRightTrigger = (BYTE)((rt > 0) ? (rt * 255 / 32767) : 0);
	}

	return true;
}

// Populate an XINPUT_STATE from the SDL gamepad mapped to a physical XInput slot.
// Returns true if state was populated, false if no SDL controller is available
// for this slot (caller should fall back to real XInput).
bool PopulateXInputStateFromSdlSlot(int xiSlot, XINPUT_STATE* pState) {
	if (xiSlot < 0 || pState == NULL) return false;

	SDL_Gamepad* pad = NULL;

	// Sentinel values ≥ 4 indicate SDL slot N (where N = xiSlot - 4)
	if (xiSlot >= 4) {
		size_t extrasIdx = (size_t)(xiSlot - 4);
		if (extrasIdx < sdlGamepadsExtra.size()) {
			pad = sdlGamepadsExtra[extrasIdx];
		}
	} else if (xiSlot < 4) {
		pad = sdlGamepadByXiSlot[xiSlot];
	}

	return PopulateXInputStateFromSDL(pad, pState);
}

void InitSDL() {
    if (!SDL3Init) {
        // --- Phase 1: Load SDL3 core functions and init SDL3 ---
        LOAD_SDL_FUNCTION(SDL_Init);

		if (fn_SDL_Init == NULL) {
			SDL3Initialization = "SDL3 Error: SDL_Init not found";
			MessageBoxA(NULL,
				"SDL3.dll could not be loaded.\n\nPlease ensure SDL3.dll is present in the game folder.\n\nThe application will now close.",
				"Crimson - Missing Dependency", MB_OK | MB_ICONERROR);
			exit(1);
		}

		// SDL3: SDL_Init returns bool; SDL_INIT_HAPTIC removed (merged into GAMEPAD)
		// Audio is optional: a machine with no output device must still be able to
		// start the game, so only a gamepad failure is fatal.
		if (!fn_SDL_Init(SDL_INIT_GAMEPAD | SDL_INIT_AUDIO) && !fn_SDL_Init(SDL_INIT_GAMEPAD)) {
			SDL3Initialization = std::string("SDL3 Error: ") + (fn_SDL_GetError ? fn_SDL_GetError() : "unknown");
			MessageBoxA(NULL,
				"SDL3.dll could not be loaded.\n\nPlease ensure SDL3.dll is present in the game folder.\n\nThe application will now close.",
				"Crimson - Missing Dependency", MB_OK | MB_ICONERROR);
			exit(1);
		}
		SDL3Initialization = "SDL3 Success";
		Log((SDL3Initialization).c_str());
        LOAD_SDL_FUNCTION(SDL_PollEvent);
        LOAD_SDL_FUNCTION(SDL_GetGamepads);
        LOAD_SDL_FUNCTION(SDL_OpenGamepad);
		LOAD_SDL_FUNCTION(SDL_CloseGamepad);
		LOAD_SDL_FUNCTION(SDL_GetGamepadPlayerIndex);
        LOAD_SDL_FUNCTION(SDL_GetGamepadJoystick);
        LOAD_SDL_FUNCTION(SDL_GetGamepadButton);
        LOAD_SDL_FUNCTION(SDL_IsGamepad);
        LOAD_SDL_FUNCTION(SDL_GetGamepadName);
        LOAD_SDL_FUNCTION(SDL_RumbleGamepad);
        LOAD_SDL_FUNCTION(SDL_GetGamepadID);
        LOAD_SDL_FUNCTION(SDL_UpdateGamepads);
        LOAD_SDL_FUNCTION(SDL_GetError);
        LOAD_SDL_FUNCTION(SDL_free);
        LOAD_SDL_FUNCTION(SDL_GetGamepadPath);
        LOAD_SDL_FUNCTION(SDL_GetGamepadPathForID);
        LOAD_SDL_FUNCTION(SDL_GetGamepadAxis);
        LOAD_SDL_FUNCTION(SDL_GetNumGamepadTouchpads);
        LOAD_SDL_FUNCTION(SDL_GetGamepadTouchpadFinger);
        LOAD_SDL_FUNCTION(SDL_CreateProperties);
        LOAD_SDL_FUNCTION(SDL_SetNumberProperty);
        LOAD_SDL_FUNCTION(SDL_DestroyProperties);

       

        // --- Phase 2: Load SDL3_mixer (only after SDL3 is initialized) ---
        LOAD_MIXER_FUNCTION(MIX_Init);
        LOAD_MIXER_FUNCTION(MIX_Quit);
        LOAD_MIXER_FUNCTION(MIX_Version);
        LOAD_MIXER_FUNCTION(MIX_CreateMixerDevice);
        LOAD_MIXER_FUNCTION(MIX_DestroyMixer);
        LOAD_MIXER_FUNCTION(MIX_LoadAudio);
        LOAD_MIXER_FUNCTION(MIX_DestroyAudio);
        LOAD_MIXER_FUNCTION(MIX_CreateTrack);
        LOAD_MIXER_FUNCTION(MIX_DestroyTrack);
        LOAD_MIXER_FUNCTION(MIX_SetTrackAudio);
        LOAD_MIXER_FUNCTION(MIX_TagTrack);
        LOAD_MIXER_FUNCTION(MIX_PlayTrack);
        LOAD_MIXER_FUNCTION(MIX_StopTrack);
        LOAD_MIXER_FUNCTION(MIX_StopTag);
        LOAD_MIXER_FUNCTION(MIX_PauseTrack);
        LOAD_MIXER_FUNCTION(MIX_ResumeTrack);
        LOAD_MIXER_FUNCTION(MIX_TrackPlaying);
        LOAD_MIXER_FUNCTION(MIX_SetTrackGain);
        LOAD_MIXER_FUNCTION(MIX_SetTagGain);
        LOAD_MIXER_FUNCTION(MIX_SetTrack3DPosition);
        LOAD_MIXER_FUNCTION(MIX_SetTrackStoppedCallback);
        LOAD_MIXER_FUNCTION(MIX_TrackMSToFrames);
        LOAD_MIXER_FUNCTION(MIX_GetAudioDuration);
        LOAD_MIXER_FUNCTION(MIX_AudioFramesToMS);
        LOAD_MIXER_FUNCTION(MIX_CreateAudioDecoder);
        LOAD_MIXER_FUNCTION(MIX_DecodeAudio);
        LOAD_MIXER_FUNCTION(MIX_GetAudioDecoderFormat);
        LOAD_MIXER_FUNCTION(MIX_DestroyAudioDecoder);

        if (fn_MIX_Init == NULL) {
            MixerInitialization = "Mixer3 Error: MIX_Init not found";
        } else if (!fn_MIX_Init()) {
            MixerInitialization = std::string("Mixer3 Error: ") + (fn_SDL_GetError ? fn_SDL_GetError() : "unknown");
        } else {
            MixerInitialization = "Mixer3 Success";
        }

		Log((MixerInitialization).c_str());

        if (fn_MIX_Version != NULL) {
            const int linked   = fn_MIX_Version();
            const int expected = SDL_MIXER_VERSION;
            Log("SDL3_mixer linked %d, headers %d", linked, expected);
        }

        g_mixer = (fn_MIX_CreateMixerDevice != NULL)
            ? fn_MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL)
            : NULL;

        if (g_mixer == NULL || fn_MIX_CreateTrack == NULL) {
            MixerInitialization2 = "Mixer3 Error: mixer creation failed";
        } else {
            InitAudioPools();
            for (int i = 0; i < 2; ++i) {
                g_musicTracks[i] = fn_MIX_CreateTrack(g_mixer);
                if (g_musicTracks[i] != NULL && fn_MIX_TagTrack != NULL) {
                    fn_MIX_TagTrack(g_musicTracks[i], kMusicTag);
                }
            }

            // The switcher tracks are deliberately left untagged. The music tag
            // is what the jingle features and the pause duck pull down, and a
            // switch of that gain mid-fade would change a song's level under it;
            // its own gain is the slider, the measured correction and the pause
            // duck, nothing else.
            for (int i = 0; i < 2; ++i) {
                g_switcherTracks[i] = fn_MIX_CreateTrack(g_mixer);
            }

            MixerInitialization2 = "Mixer3 Tracks OK";
        }

		Log((MixerInitialization2).c_str());

        mainController = NULL;
        if (fn_SDL_GetGamepads != NULL && fn_SDL_IsGamepad != NULL && fn_SDL_OpenGamepad != NULL && fn_SDL_free != NULL) {
            int joyCount;
            SDL_JoystickID* joysticks = fn_SDL_GetGamepads(&joyCount);
            if (joysticks) {
                for (int i = 0; i < joyCount && i < 4; ++i) {
                    if (fn_SDL_IsGamepad(joysticks[i])) {
                        mainController = fn_SDL_OpenGamepad(joysticks[i]);
                        if (mainController) {
                            break;
                        }
                    }
                }
                fn_SDL_free(joysticks);
            }
        }


        int controllerIndex = (fn_SDL_GetGamepadPlayerIndex != NULL && mainController != NULL)
            ? fn_SDL_GetGamepadPlayerIndex(mainController) : -1;

        InitControllers();
        RemapSdlControllers();

        // Install XInput hooks now that SDL controllers are mapped.
        // This must happen here (not earlier in Crimson.cpp) because
        // Hooked_XInputGetState depends on sdlGamepadByXiSlot/sdlGamepadsExtra
        // which are only populated after InitControllers/RemapSdlControllers.
        InitBindings();

        if (!g_SDL3Mixer) {
            MessageBoxA(NULL,
                "SDL3_mixer.dll could not be loaded.\n\nPlease ensure both SDL3.dll and SDL3_mixer.dll are present in the game folder.\n\nThe application will now close.",
                "Crimson - Missing Dependency", MB_OK | MB_ICONERROR);
            exit(1);
        }

        SDL3Init = true;
    }


    LoadAllSFX();
}

struct SnapEvent {
	byte8* actorBaseAddr;
	uint32       initialStyle;
	int         playerIndex;
	int         volume;
	bool        playDouble;
	bool        secondSnapPending;
	std::chrono::steady_clock::time_point fireAt;
};

static std::deque<SnapEvent> snapQueue;

void TickSnapQueue() {
	const auto now = std::chrono::steady_clock::now();

	for (auto it = snapQueue.begin(); it != snapQueue.end(); ) {
		if (now < it->fireAt) {
			++it;
			continue;
		}

		auto& ev = *it;
		auto& actorData = *reinterpret_cast<PlayerActorData*>(ev.actorBaseAddr);
		const auto style = actorData.style;
		const bool canPlaySnap = (actorData.eventData[0].event == 1 && actorData.character == CHARACTER::DANTE);

		if (!ev.secondSnapPending) {
			// First snap
			if (canPlaySnap && ev.initialStyle == style)
				poolSnap.PlayFree(ev.playerIndex, snap, (float)ev.volume);
			else
				ev.playDouble = false;

			if (style != STYLE::GUNSLINGER && style != STYLE::ROYALGUARD)
				ev.playDouble = false;

			if (ev.playDouble) {
				// Re-queue for the second snap
				ev.secondSnapPending = true;
				ev.fireAt = now + std::chrono::milliseconds(40);
				++it;
				continue;
			}
		}
		else {
			// Second snap
			if (canPlaySnap && (style == STYLE::GUNSLINGER || style == STYLE::ROYALGUARD))
				poolSnap.PlayFree(ev.playerIndex, snap, (float)ev.volume);
		}

		it = snapQueue.erase(it);
	}
}


bool IsGamepadButtonDown(SDL_Gamepad* gamepad, int button) {
	if (fn_SDL_GetGamepadButton == NULL || gamepad == NULL) return false;
	return fn_SDL_GetGamepadButton(gamepad, (SDL_GamepadButton)button);
}

bool IsControllerButtonDown(int controllerIndex, int button) {
	SDL_Gamepad* pad = GetControllerForPlayer(controllerIndex);
	return IsGamepadButtonDown(pad, button);
}

uint32_t GetTouchpadZone(SDL_Gamepad* gamepad) {
	if (fn_SDL_GetNumGamepadTouchpads == NULL || fn_SDL_GetGamepadTouchpadFinger == NULL || gamepad == NULL)
		return 0;
	if (fn_SDL_GetGamepadButton == NULL)
		return 0;

	int numTouchpads = fn_SDL_GetNumGamepadTouchpads(gamepad);
	if (numTouchpads <= 0) return 0;

	// ── Per-gamepad state: lock zone at the instant of press ──
	struct TouchState {
		bool     buttonWasPressed = false;
		uint32_t lockedHalf       = 0;
	};
	static std::unordered_map<SDL_Gamepad*, TouchState> s_state;

	auto& state        = s_state[gamepad];
	bool buttonPressed  = fn_SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_TOUCHPAD);
	bool wasPressed     = state.buttonWasPressed;
	state.buttonWasPressed = buttonPressed;

	// Rising edge — scan ALL fingers, pick the one with highest pressure.
	// The finger actively pressing the pad exerts more capacitive pressure
	// than a finger that is merely resting.
	if (buttonPressed && !wasPressed) {
		float bestX = 0.0f, bestY = 0.0f, bestPressure = -1.0f;
		bool  found = false;
		for (int finger = 0; finger < 16; ++finger) {
			bool  down = false;
			float x = 0.0f, y = 0.0f, pressure = 0.0f;
			if (!fn_SDL_GetGamepadTouchpadFinger(gamepad, 0, finger, &down, &x, &y, &pressure))
				break;
			if (down && pressure > bestPressure) {
				bestPressure = pressure;
				bestX = x;
				bestY = y;
				found = true;
			}
		}
		if (found) {
			bool left = (bestX < 0.5f);
			state.lockedHalf = left ? GAMEPAD::TOUCHPAD_LEFT : GAMEPAD::TOUCHPAD_RIGHT;
		}
	}

	// Falling edge — clear locked zone
	if (!buttonPressed) {
		state.lockedHalf = 0;
		return 0;
	}

	// Return the previously locked zone (always halves)
	return state.lockedHalf;
}

bool IsTouchpadBindingActive(int playerIndex, uint32_t slotA, uint32_t slotB) {
	SDL_Gamepad* sdlPad = GetControllerForPlayer(playerIndex);
	if (!sdlPad) return false;

	uint32_t touchZone = GetTouchpadZone(sdlPad);
	if (touchZone == 0) return false;

	return GAMEPAD::TouchpadZoneMatches(slotA, touchZone)
		|| GAMEPAD::TouchpadZoneMatches(slotB, touchZone);
}

void CheckAndOpenControllers() {
	if (fn_SDL_PollEvent == NULL) return;
	SDL_Event event;

	while (fn_SDL_PollEvent(&event)) {
		if (event.type == SDL_EVENT_GAMEPAD_ADDED) {
			AddController(event.gdevice.which);
		} else if (event.type == SDL_EVENT_GAMEPAD_REMOVED) {
			RemoveController(event.gdevice.which); 
		}
	}
}

void UpdateJoysticks() {
    if (fn_SDL_UpdateGamepads != NULL) {
        fn_SDL_UpdateGamepads();
    }
    TickSnapQueue();
}

void VibrateController(int controllerIndex, Uint16 rumbleStrengthLowFreq, Uint16 rumbleStrengthHighFreq, int rumbleDuration) {
	uint8* disableFlag = reinterpret_cast<uint8*>(appBaseAddr + 0xCA8800);
	if (*disableFlag) return;
    if (fn_SDL_RumbleGamepad == NULL) return;
    SDL_Gamepad* pad = GetControllerForPlayer(controllerIndex);
    if (pad != NULL) {
		if (fn_SDL_RumbleGamepad(pad, rumbleStrengthLowFreq, rumbleStrengthHighFreq, rumbleDuration)) {
			// rumble started successfully
		}
		else {
			const char* err = (fn_SDL_GetError != NULL) ? fn_SDL_GetError() : "Unknown";
			std::cerr << "Vibration ERROR " << controllerIndex << ": " << err << std::endl;
		}
    }
}

void VibrateControllerByPhysicalSlot(int xiSlot, Uint16 rumbleLow, Uint16 rumbleHigh) {
    if (fn_SDL_RumbleGamepad == NULL) return;
    Uint32 duration = (rumbleLow == 0 && rumbleHigh == 0) ? 0 : 5000;

    // Reverse-lookup: find which player (if any) is mapped to this physical
    // XInput slot via xinputSlots[]. This handles SDL-only controllers
    // (PS4/PS5/Switch) that live in sdlGamepadsExtra and are mapped to
    // a physical slot through a sentinel value (≥4).
    for (int p = 0; p < PLAYER_COUNT; ++p) {
        if ((int)activeCrimsonInput.xinputSlots[p] == xiSlot) {
            SDL_Gamepad* pad = GetControllerForPlayer(p);
            if (pad != NULL) {
                fn_SDL_RumbleGamepad(pad, rumbleLow, rumbleHigh, duration);
            }
            return;
        }
    }

    // Fallback: no player explicitly maps to this physical slot — try the
    // direct sdlGamepadByXiSlot[] entry (XInput controller at this slot).
    SDL_Gamepad* pad = GetControllerByPhysicalSlot(xiSlot);
    if (pad != NULL) {
        fn_SDL_RumbleGamepad(pad, rumbleLow, rumbleHigh, duration);
        return;
    }

    // Second fallback: all controllers are SDL-only (in sdlGamepadsExtra,
    // mapped via sentinel values). Vibrate player 0's controller since
    // the game always targets the active player for vibration.
    pad = GetControllerForPlayer(0);
    if (pad != NULL) {
        fn_SDL_RumbleGamepad(pad, rumbleLow, rumbleHigh, duration);
    }
}

void PlayChangeDevilArm() {
	float slider = activeCrimsonConfig.SFX.changeWeaponEffectVolume / 100.0f;
	int volume = (int)(30.0f * slider);
    poolDevilArm.PlayFree(0, changeDevilArm, volume, 90, 0, 400);
}

void PlayChangeGun() {
	float slider = activeCrimsonConfig.SFX.changeWeaponEffectVolume / 100.0f;
	int volume = (int)(30.0f * slider);
    poolChangeGun.PlayFree(0, changeGun, volume, 270, 0, 400);
}


void PlayChangeDevilArmMP() {
	float slider = activeCrimsonConfig.SFX.changeWeaponEffectVolume / 100.0f;
	int volume = (int)(20.0f * slider);
	poolDevilArm.PlayFree(0, changeDevilArm, volume, 0, 0, 400);
}

void PlayChangeGunMP() {
	float slider = activeCrimsonConfig.SFX.changeWeaponEffectVolume / 100.0f;
	int volume = (int)(20.0f * slider);
	poolChangeGun.PlayFree(0, changeGun, volume, 0, 0, 400);
}

void PlayChangeWeaponDMC3MP() {
	float slider = activeCrimsonConfig.SFX.changeWeaponEffectVolume / 100.0f;
	int volume = (int)(20.0f * slider);
	poolChangeGun.PlayFree(0, changeWeaponDMC3, volume, 0, 0, 400);
}

void PlayStyleChange(int playerIndex) {
	float slider = activeCrimsonConfig.SFX.styleChangeVolume / 100.0f; 
	int volume = (int)(10.0f * slider); 

    poolStyleChange.PlayFree(playerIndex, styleChange, volume, 0, 0, 150);
}

void PlayStyleChangeVO(int playerIndex, int style, bool doppActive) {
	float slider = activeCrimsonConfig.SFX.styleChangeVoiceOverVolume / 100.0f;
	int volume = (int)(46.08f * slider);

    if (style == 2) {
        poolStyleChangeVO.PlayFree(playerIndex, tricksterVO, volume, 0, 0, 150);
    } else if (style == 0) {
        poolStyleChangeVO.PlayFree(playerIndex, swordmasterVO, volume, 0, 0, 150);
    } else if (style == 1) {
        poolStyleChangeVO.PlayFree(playerIndex, gunslingerVO, volume, 0, 0, 150);
    } else if (style == 3) {
        poolStyleChangeVO.PlayFree(playerIndex, royalguardVO, volume, 0, 0, 150);
    } else if (style == 4) {
        poolStyleChangeVO.PlayFree(playerIndex, quicksilverVO, volume, 0, 0, 150);
    } else if (style == 5) {

        if (doppActive) {
            poolStyleChangeVO.PlayFree(playerIndex, doppelganger1VO, volume, 0, 0, 150);
        }
        else {
            poolStyleChangeVO.PlayFree(playerIndex, doppelganger2VO, volume, 0, 0, 150);
        }
    }
}

void PlaySnap(byte8* actorBaseAddr) {
	if (!actorBaseAddr) return;

	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
	const auto initialStyle = actorData.style;
	const auto playerIndex = actorData.newPlayerIndex;
	const auto actorSpeed = actorData.speed;  // adjust field name as needed

	if (actorData.eventData[0].event != 1 || actorData.character != CHARACTER::DANTE)
		return;
	float slider = (activeCrimsonConfig.SFX.styleChangeSnapVolume / 100.0f);
	//float slider = 5.0f / 100.0f;
	int   volume = (int)(255.0f * 5.0f / 100.0f * slider);

	std::chrono::milliseconds delay{ 0 };
	bool playDouble = false;

	switch (initialStyle) {
	case STYLE::SWORDMASTER:  delay = std::chrono::milliseconds(300); break;
	case STYLE::GUNSLINGER:   delay = std::chrono::milliseconds(300); playDouble = true; break;
	case STYLE::TRICKSTER:    delay = std::chrono::milliseconds(300); break;
	case STYLE::ROYALGUARD:   delay = std::chrono::milliseconds(300); playDouble = true; break;
	case STYLE::QUICKSILVER:  delay = std::chrono::milliseconds(300); break;
	case STYLE::DOPPELGANGER: delay = std::chrono::milliseconds(300); break;
	}

	if (actorSpeed > 0.0f) {
		delay = std::chrono::milliseconds(
			static_cast<long long>(delay.count() / (actorData.speed / g_FrameRateTimeMultiplier))
		);
	}

	snapQueue.push_back({
		.actorBaseAddr = actorBaseAddr,
		.initialStyle = initialStyle,
		.playerIndex = playerIndex,
		.volume = volume,
		.playDouble = playDouble,
		.secondSnapPending = false,
		.fireAt = std::chrono::steady_clock::now() + delay,
		});
}

// Sounds whose spatialization is only applied in multiplayer, matching how the
// old channel blocks were positioned.
static SfxVoicePool* const kMultiplayerPositionedPools[] = {
    &poolDevilArm,
    &poolChangeGun,
};

// Sounds positioned every frame for every player.
static SfxVoicePool* const kPositionedPools[] = {
    &poolStyleChange,
    &poolStyleChangeVO,
    &poolSprint,
    &poolDevilTriggerIn,
    &poolDevilTriggerOut,
    &poolDevilTriggerLoop,
    &poolDoppelgangerIn,
    &poolDoppelgangerOut,
    &poolQuicksilverIn,
    &poolQuicksilverOut,
    &poolDevilTriggerReady,
    &poolDelayedCombo1,
    &poolDelayedCombo2,
    &poolDteStart,
    &poolDteLoop,
    &poolDteFinish,
    &poolDteRelease,
    &poolGuard,
    &poolRoyalBlock,
    &poolNormalBlock,
    &poolJdc,
    &poolJdcCharge,
    &poolSnap,
    &poolDrive,
    &poolDriveLoop,
    &poolDriveLevelUp,
    &poolDriveClone,
    &poolDriveLoopClone,
    &poolDriveLevelUpClone,
};

void SetAllSFXDistance(int playerIndex, int angle, int distance) {
    // This will simulate a pseudo 3D effect for the SFX

    if (activeConfig.Actor.playerCount > 1 && playerIndex == 0) {
        for (SfxVoicePool* pool : kMultiplayerPositionedPools) {
            pool->SetPosition(0, angle, distance);
        }
    }

    for (SfxVoicePool* pool : kPositionedPools) {
        pool->SetPosition(playerIndex, angle, distance);
    }
}

void StyleRankCooldownTracker(int rank) {
    rankAnnouncer[rank].trackerRunning = true;
    rankAnnouncer[rank].offCooldown    = false;
    std::this_thread::sleep_for(std::chrono::seconds(activeCrimsonConfig.SFX.styleRankAnnouncerCooldownSec));
    rankAnnouncer[rank].offCooldown    = true;
    rankAnnouncer[rank].trackerRunning = false;
}

void PlayStyleRank(MIX_Audio* styleRankWAV, MIX_Audio* styleRankWAVAlt, int rank) {
	float slider = activeCrimsonConfig.SFX.announcerVolume / 100.0f;
	int volume = (int)(255.0f * slider);

    auto& announcer = rankAnnouncer[rank - 1];

    if (activeCrimsonConfig.SFX.onlyResetAnnouncerWhenHit && !announcer.wasHit) {
        return;
    }
    if (!announcer.offCooldown) {
        return;
    }

    poolStyleRank.PlayVoice(0, rank - 1, (announcer.turn == 0) ? styleRankWAV : styleRankWAVAlt, (float)volume);
    announcer.turn = (announcer.turn == 0) ? 1 : 0;
    announcer.timer = activeCrimsonConfig.SFX.styleRankAnnouncerCooldownSec;
    announcer.wasHit = false;
}

// Last rank seen per player; a line only plays on a climb, so a falling rank can never replay one.
static int s_announcerLastRank[PLAYER_COUNT] = {};

void StyleRankAnnouncerController(int playerIndex, int rank) {

    if (playerIndex < 0 || playerIndex >= PLAYER_COUNT) {
        return;
    }
    if (rank < STYLE_RANK::NONE || rank > STYLE_RANK::STYLISH) {
        return;
    }

    int& lastRank = s_announcerLastRank[playerIndex];
    bool climbed = rank > lastRank;
    lastRank = rank; // Falling back to no rank re-arms the D line for the next climb.

    if (!climbed) {
        return;
    }

    if (rank == STYLE_RANK::DOPE) {
        PlayStyleRank(styleRankD1, styleRankD2, STYLE_RANK::DOPE);
    } else if (rank == STYLE_RANK::CRAZY) {
        PlayStyleRank(styleRankC1, styleRankC2, STYLE_RANK::CRAZY);
    } else if (rank == STYLE_RANK::BLAST) {
        PlayStyleRank(styleRankB1, styleRankB2, STYLE_RANK::BLAST);
    } else if (rank == STYLE_RANK::ALRIGHT) {
        PlayStyleRank(styleRankA1, styleRankA2, STYLE_RANK::ALRIGHT);
    } else if (rank == STYLE_RANK::SWEET) {
        PlayStyleRank(styleRankS1, styleRankS2, STYLE_RANK::SWEET);
    } else if (rank == STYLE_RANK::SHOWTIME) {
        PlayStyleRank(styleRankSS1, styleRankSS2, STYLE_RANK::SHOWTIME);
    } else if (rank == STYLE_RANK::STYLISH) {
        PlayStyleRank(styleRankSSS1, styleRankSSS2, STYLE_RANK::STYLISH);
    }
}


void PlaySprint(int playerIndex) {
	float slider = activeCrimsonConfig.SFX.sprintEffectVolume / 100.0f;
	int volume = (int)(20.0f * slider);

    poolSprint.PlayVoice(playerIndex, 0, sprintL1, (float)volume);
    poolSprint.PlayVoice(playerIndex, 1, sprintL2, (float)volume);
}

void PlayDevilTriggerIn(int playerIndex) {
	float sliderL1 = activeCrimsonConfig.SFX.dTInL1Volume / 100.0f;
	int volumeL1 = (int)(40.0f * sliderL1);
	float sliderL2 = activeCrimsonConfig.SFX.dTInL2Volume / 100.0f;
	int volumeL2 = (int)(20.0f * sliderL2);

    poolDevilTriggerIn.PlayVoice(playerIndex, 0, devilTriggerInL1, (float)volumeL1);
    poolDevilTriggerIn.PlayVoice(playerIndex, 1, devilTriggerInL2, (float)volumeL2);
}

void PlayDevilTriggerOut(int playerIndex) {
	float slider = activeCrimsonConfig.SFX.dTOutVolume / 100.0f;
	int volume = (int)(50.0f * slider);

    poolDevilTriggerOut.Play(playerIndex, devilTriggerOut, (float)volume);
}

void PlayDevilTriggerLoop(int playerIndex) {
    // Currently unused. - Mia

    poolDevilTriggerLoop.Play(playerIndex, devilTriggerLoop, 30.0f, -1);
}

void StopDevilTriggerLoop(int playerIndex) {

    poolDevilTriggerLoop.Stop(playerIndex);
}

void PlayDoppelgangerIn(int playerIndex) {
	float slider = activeCrimsonConfig.SFX.doppelInVolume / 100.0f;
	int volume = (int)(50.0f * slider);

    poolDoppelgangerIn.Play(playerIndex, doppelgangerIn, (float)volume);
}

void PlayDoppelgangerOut(int playerIndex) {
	float slider = activeCrimsonConfig.SFX.doppelOutVolume / 100.0f;
	int volume = (int)(50.0f * slider);

    poolDoppelgangerOut.Play(playerIndex, doppelgangerOut, (float)volume);
}

void PlayQuicksilverIn() {
	float slider = activeCrimsonConfig.SFX.quickInVolume / 100.0f;
	int volume = (int)(50.0f * slider);

    poolQuicksilverIn.Play(0, quicksilverIn, (float)volume);
}

void PlayQuicksilverOut() {
	float slider = activeCrimsonConfig.SFX.quickOutVolume / 100.0f;
	int volume = (int)(50.0f * slider);

	poolQuicksilverOut.Play(0, quicksilverOut, (float)volume);
}

void PlayJDC(int playerIndex, bool justFrame, float delay) {

	auto playSound = [=]() {
		float slider = 90 / 100.0f;
		int volume = (int)(72.0f * slider);
		if (justFrame) {
			poolJdc.PlayFree(playerIndex, jdcJustFrame, (float)volume);
		} else {
			poolJdc.PlayFree(playerIndex, jdc, (float)volume);
		}
	};

	if (delay > 0) {
		std::thread([=]() {
			std::this_thread::sleep_for(std::chrono::milliseconds((int)delay));
			playSound();
		}).detach();
	} else {
		playSound();
	}
}

void PlayJDCCharge(int playerIndex) {
    float slider = 10 / 100.0f;
    int volume = (int)(65.0f * slider);
    poolJdcCharge.Play(playerIndex, jdcCharge, (float)volume);
}

void PlayDevilTriggerReady(int playerIndex) {
	float slider = activeCrimsonConfig.SFX.dTReadyVolume / 100.0f;
	int volume = (int)(110.0f * slider);

    poolDevilTriggerReady.Play(playerIndex, devilTriggerReady, (float)volume);
}

void PlayDelayedCombo1(int playerIndex) {
    // TYPE B - DmC ping
	float slider = activeCrimsonConfig.SFX.delayedComboIndicatorVolume / 100.0f; // 1.0 to 0.0
	int volumeDelayed1 = (int)(11.0f * slider); // 18 at 100%, 0 at 0%
	poolDelayedCombo1.Play(playerIndex, delayedCombo1, (float)volumeDelayed1);
}

void PlayDelayedCombo2(int playerIndex) {
    // TYPE A - Less Intrusive
	float slider = activeCrimsonConfig.SFX.delayedComboIndicatorVolume / 100.0f; 
	int volumeDelayed2 = (int)(50.0f * slider); 
    poolDelayedCombo2.Play(playerIndex, delayedCombo2, (float)volumeDelayed2);
}

bool DTEStartIsPlaying(int playerIndex) {
    return SDL3Init && poolDteStart.IsAnyPlaying(playerIndex);
}

void PlayDTExplosionStart(int playerIndex) {
	float slider = activeCrimsonConfig.SFX.dTEStartLoopVolume / 100.0f;
	int volume = (int)(50.0f * slider);
    poolDteStart.Play(playerIndex, dtExplosionStart, (float)volume);
}

void PlayDTExplosionLoop(int playerIndex) {
	float slider = activeCrimsonConfig.SFX.dTEStartLoopVolume / 100.0f;
	int volume = (int)(50.0f * slider);
	poolDteLoop.Play(playerIndex, dtExplosionLoop, (float)volume, -1);
}

void PlayDTExplosionFinish(int playerIndex) {
	float slider = activeCrimsonConfig.SFX.dTEFinishVolume / 100.0f;
	int volume = (int)(130.0f * slider);
    poolDteFinish.Play(playerIndex, dtExplosionFinish, (float)volume);
}

void PlayDTEExplosionRelease(int playerIndex, float volumeMultiplier) {
	float slider = activeCrimsonConfig.SFX.dTEReleaseVolume / 100.0f;
	int volume = (int)((85.0f * volumeMultiplier) * slider);
	poolDteRelease.Play(playerIndex, dtExplosionRelease, (float)volume);
}

void InterruptDTExplosionSFX(int playerIndex) {
    poolDteStart.Stop(playerIndex);
    poolDteLoop.Stop(playerIndex);
}

void PauseDTExplosionSFX(int playerIndex) {
    poolDteStart.Pause(playerIndex);
	poolDteLoop.Pause(playerIndex);
}

void ResumeDTExplosionSFX(int playerIndex) {
	poolDteStart.Resume(playerIndex);
	poolDteLoop.Resume(playerIndex);
}


void PlayGuard(int playerIndex) {
	poolGuard.PlayFree(playerIndex, guard, 50.0f);
}

void PlayRoyalBlock(int playerIndex) {
	float slider = activeCrimsonConfig.SFX.royalBlockVolume / 100.0f;
	int volume = (int)(44.0f * slider);

	poolRoyalBlock.PlayFree(playerIndex, royalBlock, (float)volume);
}

void PlayNormalBlock(int playerIndex) {
	float slider = activeCrimsonConfig.SFX.normalBlockVolume / 100.0f;
	int volume = (int)(20.0f * slider);

	poolNormalBlock.PlayFree(playerIndex, normalBlock, (float)volume);
}

void PlayEcstasyHit(uintptr_t shlActorAddr) {
	if (!SDL3Init || fn_MIX_SetTrack3DPosition == NULL) return;
	if (!shlActorAddr) return;

	auto& shlActor = *reinterpret_cast<CPl000Shl10eActor*>(shlActorAddr);
	if (!shlActor.playerActorAddr) return;

	auto& actorData = *reinterpret_cast<PlayerActorData*>(shlActor.playerActorAddr);
	const int playerIndex = actorData.newPlayerIndex;
	if (playerIndex < 0 || playerIndex >= PLAYER_COUNT) return;

	// 3D audio is anchored to the SHL's world position, using the camera as
	// the listener — same convention as playerScreenAngle / cameraPlayerDistanceClamped.
	auto pool_4449 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC8FBD0);
	if (!pool_4449 || !pool_4449[147]) return;
	auto& cameraData = *reinterpret_cast<CameraData*>(pool_4449[147]);

	// Distance: SHL-to-camera distance, scaled the same as cameraPlayerDistanceClamped.
	const float dx = shlActor.position.x - cameraData.data[0].x;
	const float dy = shlActor.position.y - cameraData.data[0].y;
	const float dz = shlActor.position.z - cameraData.data[0].z;
	int distance = (int)(std::sqrt(dx * dx + dy * dy + dz * dz) / 20.0f);
	if (distance > 255) distance = 255;

	// Angle: screen-relative X offset from center (matches playerScreenAngle).
	const float screenCenterX = g_uiSize.x * 0.5f;
	const auto screenPos = debug_draw_world_to_screen((const float*)&shlActor.position, 1.0f);
	const float normalizedOffset = (screenPos.x - screenCenterX) / screenCenterX;
	constexpr float audioDeadzoneAngle = 15.0f;
	float angle = normalizedOffset * 90.0f;
	if (std::abs(angle) < audioDeadzoneAngle) angle = 0.0f;

	const int panAngle = (int)std::round(angle);

	// Overlapping hits are meant to ring out, so this one just takes a free voice
	// and never fades the instance already playing.
	poolEcstasyHit.PlayFree(playerIndex, ecstasyHit, 15.0f, panAngle, distance);
}

void PlayDriveStart(int playerIndex, int entityIndex) {
	float slider = 7.0f / 100.0f;
	int   volume = (int)(255.0f * slider);
	(entityIndex == 0 ? poolDrive : poolDriveClone).Play(playerIndex, driveStart, (float)volume, 0, 400);
}

void PlayDriveLoop(int playerIndex, int entityIndex) {
	float slider = 7.0f / 100.0f;
	int   volume = (int)(255.0f * slider);
	(entityIndex == 0 ? poolDriveLoop : poolDriveLoopClone).Play(playerIndex, driveLoop, (float)volume, -1);
}

void PlayDriveLevelUp(int playerIndex, int entityIndex) {
	float slider = 10.0f / 100.0f;
	int   volume = (int)(255.0f * slider);
	(entityIndex == 0 ? poolDriveLevelUp : poolDriveLevelUpClone).PlayFree(playerIndex, driveLevelUp, (float)volume);
}

void InterruptDriveSFX(int playerIndex, int entityIndex) {
	(entityIndex == 0 ? poolDrive : poolDriveClone).Stop(playerIndex, 100);
	(entityIndex == 0 ? poolDriveLoop : poolDriveLoopClone).Stop(playerIndex, 100);
}

bool DriveStartIsPlaying(int playerIndex, int entityIndex) {
	return SDL3Init && (entityIndex == 0 ? poolDrive : poolDriveClone).IsAnyPlaying(playerIndex);
}

void PlayNewMissionClearSong() {
    if (!SDL3Init) return;
    g_musicGain = Gain((int)(60 * (activeCrimsonConfig.Sound.channelVolumes[9] / 100.0f)));
    PlayMusic(missionClearSong, 500);
}

void PlayDivinityStatueSong() {
    if (!SDL3Init) return;
    g_musicGain = Gain((int)(57 * (activeCrimsonConfig.Sound.channelVolumes[9] / 100.0f)));
    PlayMusic(divinityStatueSong, 500);
}

void FadeOutMusic(int delayMs) {
    if (!SDL3Init || fn_MIX_StopTag == NULL || g_mixer == NULL) return;
    fn_MIX_StopTag(g_mixer, kMusicTag, delayMs);
}

int IsMusicPlaying() {
    if (!SDL3Init) return 0;
    return (TrackPlaying(g_musicTracks[0]) || TrackPlaying(g_musicTracks[1])) ? 1 : 0;
}

// Opens the file just far enough for the decoder to report its length.
bool ProbeMusicDurationMs(const char* absolutePath, int32& outMs) {
    outMs = 0;

    if (!SDL3Init || g_mixer == NULL || absolutePath == NULL || absolutePath[0] == '\0') {
        return false;
    }
    if (fn_MIX_LoadAudio == NULL || fn_MIX_GetAudioDuration == NULL || fn_MIX_AudioFramesToMS == NULL) {
        return false;
    }

    MIX_Audio* audio = fn_MIX_LoadAudio(g_mixer, absolutePath, false);
    if (audio == NULL) {
        return false;
    }

    const Sint64 frames = fn_MIX_GetAudioDuration(audio);
    bool known          = false;

    if (frames > 0) {
        const Sint64 durationMs = fn_MIX_AudioFramesToMS(audio, frames);
        if (durationMs > 0) {
            outMs = static_cast<int32>(durationMs > 0x7FFFFFFF ? 0x7FFFFFFF : durationMs);
            known = true;
        }
    }

    if (fn_MIX_DestroyAudio != NULL) {
        fn_MIX_DestroyAudio(audio);
    }

    return known;
}

// Decodes the whole file through the mixer's own decoder, which needs neither a
// device nor playback, and measures it as it goes.
bool MeasureMusicLoudness(const char* absolutePath, float& outLufs, float& outPeakDbfs) {
    outLufs     = 0.0f;
    outPeakDbfs = -120.0f;

    if (!SDL3Init || absolutePath == NULL || absolutePath[0] == '\0') {
        return false;
    }

    MusicSwitcherLoudness::Result result;

    // WAV is read directly: the mixer's decoder stops after half of a WAV data
    // chunk, which would measure half the track.
    if (!MusicSwitcherLoudness::MeasureWavFile(absolutePath, result)) {
        if (fn_MIX_CreateAudioDecoder == NULL || fn_MIX_DecodeAudio == NULL ||
            fn_MIX_DestroyAudioDecoder == NULL) {
            return false;
        }

        MIX_AudioDecoder* decoder = fn_MIX_CreateAudioDecoder(absolutePath, 0);
        if (decoder == NULL) {
            return false;
        }

        // Fixed output format: the weighting is defined at 48 kHz, so every
        // source rate is converted here rather than biasing the result.
        SDL_AudioSpec spec;
        SDL_zero(spec);
        spec.format   = SDL_AUDIO_F32;
        spec.channels = 2;
        spec.freq     = 48000;

        MusicSwitcherLoudness::Analyzer analyzer;
        std::vector<float> pcm(16384 * 2);

        bool reachedEnd = false;

        for (;;) {
            const int bytes = fn_MIX_DecodeAudio(decoder, pcm.data(),
                static_cast<int>(pcm.size() * sizeof(float)), &spec);
            if (bytes <= 0) {
                reachedEnd = (bytes == 0);
                break;
            }

            analyzer.Feed(pcm.data(), static_cast<size_t>(bytes) / (sizeof(float) * 2));
        }

        fn_MIX_DestroyAudioDecoder(decoder);

        if (!reachedEnd) {
            return false;
        }

        result = analyzer.Finish();
    }

    if (!result.valid) {
        return false;
    }

    outLufs     = result.lufs;
    outPeakDbfs = result.peakDbfs;

    return true;
}

static std::string g_switcherPath;
// Loudness correction per switcher track: the two tracks can hold different
// songs while one crossfades into the other.
static float g_switcherNormGain[2]  = {1.0f, 1.0f};
static float g_switcherSliderGain   = 1.0f;
// Pause duck; 1.0 while the game is running.
static float g_switcherDuck         = 1.0f;
// Track the current song is on, so a level change can find it again.
static int g_switcherCurrent        = -1;

bool PlaySwitcherMusic(const char* absolutePath, int32 startMs, int32 fadeInMs, int32 volume,
    float gainScale) {
    if (!SDL3Init || absolutePath == NULL || absolutePath[0] == '\0') {
        return false;
    }
    if (g_switcherTracks[0] == NULL && g_switcherTracks[1] == NULL) {
        return false;
    }

    MIX_Audio* song = LoadMusicCached(absolutePath);
    if (song == NULL) {
        return false;
    }

    MIX_Track* track = NULL;
    int trackIndex   = 0;
    for (int i = 0; i < 2; ++i) {
        const int index = (g_switcherNext + i) % 2;
        if (!TrackPlaying(g_switcherTracks[index])) {
            track            = g_switcherTracks[index];
            trackIndex       = index;
            g_switcherNext   = (index + 1) % 2;
            break;
        }
    }
    if (track == NULL) {
        trackIndex     = g_switcherNext;
        track          = g_switcherTracks[trackIndex];
        g_switcherNext = (g_switcherNext + 1) % 2;
    }

    g_switcherSliderGain            = Gain((int)(volume * 1.28f));
    g_switcherNormGain[trackIndex]  = std::clamp(gainScale, 0.005f, 8.0f);
    g_switcherCurrent               = trackIndex;

    if (!PlaySwitcherOnTrack(track, song, startMs, fadeInMs,
            g_switcherSliderGain * g_switcherNormGain[trackIndex] * g_switcherDuck)) {
        return false;
    }

    // Only retire the outgoing track once the new one is actually running, so a
    // failed start cannot leave the fight silent.
    for (int i = 0; i < 2; ++i) {
        MIX_Track* other = g_switcherTracks[i];
        if (other != NULL && other != track) {
            StopTrack(other, fadeInMs);
        }
    }

    g_switcherPath = absolutePath;

    return true;
}

void StopSwitcherMusic(int32 fadeOutMs) {
    for (int i = 0; i < 2; ++i) {
        if (g_switcherTracks[i] != NULL) {
            StopTrack(g_switcherTracks[i], fadeOutMs);
        }
    }

    g_switcherPath.clear();
}

bool IsSwitcherMusicPlaying() {
    if (!SDL3Init) {
        return false;
    }

    return TrackPlaying(g_switcherTracks[0]) || TrackPlaying(g_switcherTracks[1]);
}

void SetSwitcherMusicVolume(int32 volume) {
    if (!SDL3Init || fn_MIX_SetTrackGain == NULL) {
        return;
    }

    g_switcherSliderGain = Gain((int)(volume * 1.28f));

    for (int i = 0; i < 2; ++i) {
        if (g_switcherTracks[i] != NULL) {
            fn_MIX_SetTrackGain(g_switcherTracks[i],
                g_switcherSliderGain * g_switcherNormGain[i] * g_switcherDuck);
        }
    }
}

const char* SwitcherMusicPath() {
    return g_switcherPath.c_str();
}

void SetSwitcherMusicDuck(float duck) {
    if (!SDL3Init || fn_MIX_SetTrackGain == NULL) {
        return;
    }

    duck = std::clamp(duck, 0.0f, 1.0f);

    if (duck == g_switcherDuck) {
        return;
    }

    g_switcherDuck = duck;

    for (int i = 0; i < 2; ++i) {
        if (g_switcherTracks[i] != NULL) {
            fn_MIX_SetTrackGain(g_switcherTracks[i],
                g_switcherSliderGain * g_switcherNormGain[i] * g_switcherDuck);
        }
    }
}

void SetSwitcherMusicLevel(float gainScale) {
    if (!SDL3Init || fn_MIX_SetTrackGain == NULL) {
        return;
    }
    if (g_switcherCurrent < 0 || g_switcherCurrent > 1) {
        return;
    }

    g_switcherNormGain[g_switcherCurrent] = std::clamp(gainScale, 0.005f, 8.0f);

    if (g_switcherTracks[g_switcherCurrent] != NULL) {
        fn_MIX_SetTrackGain(g_switcherTracks[g_switcherCurrent],
            g_switcherSliderGain * g_switcherNormGain[g_switcherCurrent] * g_switcherDuck);
    }
}

void PlayBattleOfBrothersSong() {
    if (!SDL3Init) return;
	g_musicGain = Gain((int)(31 * (activeCrimsonConfig.Sound.channelVolumes[9] / 100.0f)));
	PlayMusic(battleOfBrothersSong, 500);
}

void CrossfadeCustomMusic(const char* relativePath, int volume, int fadeInMs, int fadeOutMs) {
    if (!SDL3Init || relativePath == NULL || relativePath[0] == '\0') return;
    if (fn_MIX_StopTag == NULL || g_mixer == NULL) return;

    MIX_Audio* song = LoadMusicCached((std::string)Paths::sounds + "\\music\\" + relativePath);
    if (song == NULL) return;

    g_musicGain = Gain(volume);
    ApplyMusicGain();

    fn_MIX_StopTag(g_mixer, kMusicTag, fadeOutMs);
    PlayMusic(song, fadeInMs);
}

void ReduceMusicVolumeInPause() {
    if (!SDL3Init || fn_MIX_SetTagGain == NULL) return;
	auto pool_19315 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
	if (!pool_19315 || !pool_19315[8]) {
		return;
	}
	auto& eventData = *reinterpret_cast<EventData*>(pool_19315[8]);

	if (g_scene == SCENE::GAME) {
		if (eventData.event == EVENT::PAUSE) {
			g_musicDuck = 0.5f;
			ApplyMusicGain();
		} else if (eventData.event == EVENT::MAIN) {
			g_musicDuck = 1.0f;
			ApplyMusicGain();
		}
	}
}

}

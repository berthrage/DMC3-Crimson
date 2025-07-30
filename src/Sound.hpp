#pragma once

#include "Core/DataTypes.hpp"
#include "Core/Macros.h"
#include "FMOD.hpp"
#include "Config.hpp"
#include "Vars.hpp"

#define _(size)                                     \
    struct {                                        \
        byte8 Prep_Merge(padding_, __LINE__)[size]; \
    }

#pragma pack(push, 1)

// struct ArchiveMetadata
// {
// 	byte8 signature[4];
// 	uint32 fileCount;
// 	uint32 fileOffs[1];
// };

struct HeadMetadata {
    byte8 signature[4];
    _(8);
    uint32 size;
    uint32 waveSize;
    uint32 progMetadataOff;
    uint32 smplMetadataOff;
    uint32 vagiMetadataOff;
};

struct ProgSectItem {
    _(12);
    uint16 smplItemIndex;
};

struct ProgSectMetadata {
    uint8 itemCount;
    _(7);
    ProgSectItem items[1];
};

struct ProgMetadata {
    byte8 signature[4];
    uint32 size;
    uint32 last;
    _(4);
    uint32 sectOffs[1];
};

struct SmplItem {
    _(10);
    uint16 vagiItemIndex;
};

struct SmplMetadata {
    byte8 signature[4];
    uint32 size;
    uint32 last;
    _(4);
    SmplItem items[1];
};

struct VagiItem {
    uint32 off;
    uint32 size;
    _(4);
    uint32 sampleRate;
};

struct VagiMetadata {
    byte8 signature[4];
    uint32 size;
    uint32 last;
    _(4);
    VagiItem items[1];
};

struct WaveMetadata {
    byte8 signature[4];
    _(8);
    uint32 reverseSize;
    _(16);
    char name[32];
};

struct SoundData {
    uint64 off;
    uint64 size;
    FMOD_SOUND* fmodSoundAddr;
};

struct DbstItem {
    _(32);
};

struct DbstMetadata {
    byte8 signature[4];
    _(4);
    uint32 size;
    uint32 itemCount;
    DbstItem items[1];
};

#pragma pack(pop)

#undef _

void PlaySound(int32 group, int32 index);

void PlayTrack(const char* filename);

void SetVolume(uint8 channelIndex, float volume);

inline void UpdateVolumes() {
    old_for_all(uint8, channelIndex, CHANNEL::MAX) {
        SetVolume(channelIndex, activeCrimsonConfig.Sound.channelVolumes[channelIndex] / 100.0f);
    }
}

bool Sound_Init();

namespace Sound {
void Toggle(bool enable);
void EventMain();
void EventDelete();
void UpdateEnemyCount();
void UpdateVolumeTransition();
void SetVolumeGradually(int channel, float targetVolume, float durationMs = 2000.0f);
} // namespace Sound
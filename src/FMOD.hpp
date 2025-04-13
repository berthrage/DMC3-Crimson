#pragma once

#include "Core/DataTypes.hpp"
#include "Core/Macros.h"

extern bool FMOD_init;

enum {
    FMOD_OK                     = 0,
    FMOD_CREATECOMPRESSEDSAMPLE = 0x00000200,
    FMOD_OPENMEMORY             = 0x00000800,
    FMOD_LOWMEM                 = 0x08000000,
    FMOD_CACHE_SIZE             = (128 * 1024 * 1024),
};

typedef int32 FMOD_RESULT;
typedef uint32 FMOD_MODE;
typedef void FMOD_SYSTEM;
typedef void FMOD_SOUND;
typedef void FMOD_CHANNEL;

#define _(size)                                     \
    struct {                                        \
        byte8 Prep_Merge(padding_, __LINE__)[size]; \
    }

#pragma pack(push, 1)

struct FMOD_CREATESOUNDEXINFO {
    int32 cbsize;
    uint32 length;
    _(224);
};

#pragma pack(pop)

#undef _

typedef FMOD_RESULT(__fastcall* FMOD_System_CreateSound_t)(
    FMOD_SYSTEM* system, void* addr, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO* info, FMOD_SOUND** sound);

extern FMOD_System_CreateSound_t FMOD_System_CreateSound;

typedef FMOD_RESULT(__fastcall* FMOD_Channel_SetVolume_t)(FMOD_CHANNEL* channel, float volume);

extern FMOD_Channel_SetVolume_t FMOD_Channel_SetVolume;

bool FMOD_Init();
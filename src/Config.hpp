#pragma once

#include <stdint.h>
#include "Core/Core.hpp"
#include "Core/Macros.h"
#include "Exp.hpp"
#include "Vars.hpp"
#include "CrimsonConfig.hpp"

#include "Core/DebugSwitch.hpp"

#define _(size)                                     \
    struct {                                        \
        byte8 Prep_Merge(padding_, __LINE__)[size]; \
    }

#pragma pack(push, 1)

struct Config {
    struct {
        bool enable                         = (debug) ? true : false;
        uint8 playerCount                   = 1;
        PlayerData playerData[PLAYER_COUNT] = {};
    } Actor;

    struct {
        bool enable         = (debug) ? true : false;
        uint32 mission      = 7;
        uint32 mode         = MODE::DANTE_MUST_DIE;
        uint32 room         = 900;
        bool enableRoomSelection     = (debug) ? false : true;
        uint32 position     = 0;
        bool enablePositionSelection = (debug) ? false : true;
        uint8 floor         = 0;
        uint16 level        = 0;
        float hitPoints     = 20000;
        float magicPoints   = 10000;
        uint8 character     = CHARACTER::DANTE;
        uint8 costume       = 0;
        uint32 style        = STYLE::TRICKSTER;
        uint8 weapons[4]    = {
            WEAPON::REBELLION,
            WEAPON::CERBERUS,
            WEAPON::EBONY_IVORY,
            WEAPON::SHOTGUN,
        };
    } Arcade;

    struct {
        bool swapNormalShotAndMultiLock = false;
        bool instantFullCharge          = false;
    } Artemis;

    struct {
        bool enable = false;
        struct {
            bool skipJester = false;
        } Mission5;
        struct {
            bool skipJester      = false;
            bool skipGeryonPart1 = false;
        } Mission12;
        struct {
            bool skipJester = false;
        } Mission17;
        struct {
            bool skipArkhamPart1 = false;
        } Mission19;
    } BossRush;

    struct {
        uint8 airHike[5][4] = {
            {128, 0, 0, 200},
            {96, 128, 144, 200},
            {160, 64, 16, 200},
            {112, 64, 160, 200},
            {128, 128, 128, 200},
        };

        struct {
            uint8 skyStar[4] = {255, 0, 0, 200};
        } Trickster;
        struct {
            uint8 ultimate[4] = {143, 112, 48, 200};
        } Royalguard;
        struct {
            uint8 clone[4] = {16, 16, 16, 48};
        } Doppelganger;
        struct {
            uint8 dante[5][4] = {
                {128, 0, 0, 200},
                {96, 128, 144, 200},
                {160, 64, 16, 200},
                {112, 64, 160, 200},
                {128, 128, 128, 200},
            };
            uint8 sparda[4]    = {128, 0, 0, 200};
            uint8 vergil[3][4] = {
                {32, 64, 128, 200},
                {32, 64, 128, 200},
                {32, 64, 128, 200},
            };
            uint8 neroAngelo[4] = {64, 0, 255, 200};
        } Aura;
    } Color;


    struct {
        bool foursomeTime      = false;
        bool infiniteRainStorm = false;
    } EbonyIvory;

    struct {
        bool infiniteSwordPierce    = false;
        float stingerDuration[2]    = {16, 16};
        float stingerRange[2]       = {560, 560};
        uint8 airStingerCount[2]    = {1, 1};
        float airStingerDuration[2] = {3, 4};
        float airStingerRange[2]    = {0, 140};
    } Rebellion;

    struct {
        bool forceJustFrameRelease = false;
    } Royalguard;

    struct {
        float mainSpeed = 1.0f;
        float turbo     = 1.2f;
        float enemy     = 1.0f;

        float quicksilverPlayerActor = 1.05f;
        float quicksilverEnemyActor  = 0.33f;

        float human         = 1.0f;
        float devilDante[6] = {
            1.1f,
            1.2f,
            1.05f,
            1.1f,
            1.05f,
            1.1f,
        };
        float devilVergil[5] = {
            1.2f,
            1.2f,
            1.2f,
            1.0f,
            1.0f,
        };
    } Speed;

    struct {
        bool chronoSwords = false;
    } SummonedSwords;

    struct {
        uint8 judgementCutCount[2] = {2, 2};
    } Yamato;

    struct {
        bool infiniteRoundTrip      = false;
        float stingerDuration[2]    = {16, 16};
        float stingerRange[2]       = {560, 560};
        uint8 airStingerCount[2]    = {1, 1};
        float airStingerDuration[2] = {5, 5};
        float airStingerRange[2]    = {280, 280};
    } YamatoForceEdge;

    bool welcome = (debug) ? false : true;

    bool hideBeowulfDante  = false;
    bool hideBeowulfVergil = false;

    bool airHikeCoreAbility = false;

    uint8 crazyComboLevelMultiplier = 6;

    uint8 dotShadow = DOT_SHADOW::DEFAULT;

    float depleteQuicksilver  = 13.75f;
    float depleteDoppelganger = 16.5f;
    float depleteDevil        = 11.0f;

    bool noDevilForm = false;
    float orbReach   = (debug) ? 9000.0f : 300.0f;

    bool resetPermissions = false;

    bool infiniteHitPoints   = false;
    bool infiniteMagicPoints = (debug) ? true : false;
    bool disableTimer        = false;
    bool infiniteBullets     = false;

    float linearWeaponSwitchTimeout = (debug) ? 6 : 12;

    uint8 airHikeCount[2]        = {1, 1};
    uint8 kickJumpCount[2]       = {1, 1};
    uint8 wallHikeCount[2]       = {1, 1};
    uint8 dashCount[2]           = {3, 3};
    uint8 skyStarCount[2]        = {1, 1};
    uint8 airTrickCountDante[2]  = {1, 1};
    uint8 airTrickCountVergil[2] = {1, 1};
    uint8 trickUpCount[2]        = {1, 1};
    uint8 trickDownCount[2]      = {1, 1};

    float channelVolumes[CHANNEL::MAX] = {
        1.0f,
        1.0f,
        1.0f,
        1.0f,
        1.0f,
        1.0f,
        1.0f,
        1.0f,
        1.0f,
        1.0f,
        1.0f,
    };

    struct TextureData {
        vec2 size;
        vec2 pos;
    };

    struct WeaponSwitchControllerTextureData {
        TextureData backgrounds[5];
        TextureData icons[5];
        TextureData highlights[5];
        TextureData arrow;
    };

    WeaponSwitchControllerTextureData meleeWeaponSwitchControllerTextureData = {
        {
            {
                {256, 256},
                {1442, 403},
            },
            {
                {256, 256},
                {1651, 555},
            },
            {
                {256, 256},
                {1571, 801},
            },
            {
                {256, 256},
                {1313, 801},
            },
            {
                {256, 256},
                {1233, 555},
            },
        },
        {
            {
                {256, 256},
                {1442, 403},
            },
            {
                {256, 256},
                {1651, 555},
            },
            {
                {256, 256},
                {1571, 801},
            },
            {
                {256, 256},
                {1313, 801},
            },
            {
                {256, 256},
                {1233, 555},
            },
        },
        {
            {
                {256, 256},
                {1442, 403},
            },
            {
                {256, 256},
                {1651, 555},
            },
            {
                {256, 256},
                {1571, 801},
            },
            {
                {256, 256},
                {1313, 801},
            },
            {
                {256, 256},
                {1233, 555},
            },
        },
        {
            {128, 128},
            {1506, 671},
        },
    };

    WeaponSwitchControllerTextureData rangedWeaponSwitchControllerTextureData = {
        {
            {
                {256, 256},
                {222, 403},
            },
            {
                {256, 256},
                {431, 555},
            },
            {
                {256, 256},
                {351, 801},
            },
            {
                {256, 256},
                {93, 801},
            },
            {
                {256, 256},
                {13, 555},
            },
        },
        {
            {
                {256, 256},
                {222, 403},
            },
            {
                {256, 256},
                {431, 555},
            },
            {
                {256, 256},
                {351, 801},
            },
            {
                {256, 256},
                {93, 801},
            },
            {
                {256, 256},
                {13, 555},
            },
        },
        {
            {
                {256, 256},
                {222, 403},
            },
            {
                {256, 256},
                {431, 555},
            },
            {
                {256, 256},
                {351, 801},
            },
            {
                {256, 256},
                {93, 801},
            },
            {
                {256, 256},
                {13, 555},
            },
        },
        {
            {128, 128},
            {286, 671},
        },
    };

    bool forceIconFocus = false;

    float damagePlayerActorMultiplier = 1.0f;
    float damageEnemyActorMultiplier  = 1.0f;
    uint32 damageStyleRank            = STYLE_RANK::NONE;

    bool skipIntro     = (debug) ? true : false;
    bool skipCutscenes = (debug) ? true : false;

    bool enableFileMods = true;

    float frameRate = 60;
    uint8 vSync     = 1;

    bool hideMouseCursor = false;

    int32 windowPosX      = 0;
    int32 windowPosY      = 0;
    bool forceWindowFocus = true;

    float globalScale = 1;

    uint8 enemyCount                                          = 1;
    ConfigCreateEnemyActorData configCreateEnemyActorData[30] = {};

    bool enemyAutoSpawn = false;

    struct OverlayData {
        bool enable    = false;
        vec2 pos       = {8, 8};
        float color[4] = {1.0f, 0, 0, 1.0f};
    };

    struct MainOverlayData : OverlayData {
        bool showFocus               = true;
        bool showFPS                 = true;
        bool showSizes               = true;
        bool showFrameRateMultiplier = true;
        bool showEventData           = true;
        bool showPosition            = true;
        bool showRegionData          = false;

        MainOverlayData() {
            if constexpr (debug) {
                enable = true;
            }
        }
    } debugOverlayData;

    OverlayData missionOverlayData;
    OverlayData bossLadyActionsOverlayData;
    OverlayData bossVergilActionsOverlayData;

    // To avoid surprises by using __declspec(align) we prefer _(n) and
    // static_assert to get the correct alignment.

    _(10);
    float kalinaAnnHookGrenadeHeight = 1280.0f;
    _(12);
    float kalinaAnnHookGrenadeTime = 90.0f;
    _(12);
    vec4 kalinaAnnHookMultiplier = {
        1.0f,
        1.0f,
        1.0f,
        1.0f,
    };

    bool enableBossLadyFixes   = false;
    bool enableBossVergilFixes = false;
    bool enablePVPFixes        = false;

    bool hideMainHUD = false;
    bool hideLockOn  = false;
    bool hideBossHUD = false;

    bool soundIgnoreEnemyData = false;

    bool enableRebellionAirStinger = true;
    bool enableRebellionNewDrive   = (debug) ? true : false;
    bool enableRebellionQuickDrive = (debug) ? true : false;
    bool enableCerberusAirRevolver = (debug) ? true : false;
    bool enableNevanNewVortex      = (debug) ? true : false;

    bool enableYamatoVergilNewJudgementCut  = (debug) ? true : false;
    bool enableBeowulfVergilAirRisingSun    = (debug) ? true : false;
    bool enableBeowulfVergilAirLunarPhase   = (debug) ? true : false;
    bool enableYamatoForceEdgeNewComboPart4 = (debug) ? true : false;
    bool enableYamatoForceEdgeAirStinger    = true;
    bool enableYamatoForceEdgeNewRoundTrip  = (debug) ? true : false;

    uint8 dergil = 0;

    uint8 beowulfVergilAirRisingSunCount[2] = {1, 1};

    bool forceVisibleHUD = false;

    _(6);

    struct BarsData {
        bool enable         = true;
        bool run            = true;
        vec2 size           = {200, 10};
        vec2 pos            = {8, 8};
        uint32 lastX        = 0;
        uint32 lastY        = 0;
        float hitColor[4] = { 0.29f , 0.99f, 0.44f, 1.0f };
        float magicColor[4] = { 0.78f, 0.05f, 0.41f, 1.0f };
        float magicColorVergil[4] = { 0.06f, 0.74f, 0.81f, 1.0f };
    };

    BarsData barsData[PLAYER_COUNT];

    bool disableStyleRankHudFadeout = true;

    bool forceSyncHitMagicPoints = true;

    bool updateLockOns = true;

    bool showCredits = (debug) ? false : true;

    KeyData keyData[4] = {{{
                               DI8::KEY::DELETE,
                           },
                              1},
        {}, {}, {{DI8::KEY::F1}, 1}};

    bool absoluteUnit = false;

    char gamepadName[128] = "Wireless Controller";
    byte8 gamepadButton   = 13;

    bool disablePlayerActorIdleTimer = false;

    bool rebellionInfiniteShredder = false;
    bool rebellionHoldDrive        = false;

    uint8 costumeRespectsProgression = 2;

    // load these from cfg, here are default values for now
    
    std::string selectedHUD = "Crimson HUD";

    bool disableBlendingEffects = false;
    bool framerateResponsiveGameSpeed = true;

    bool playDTReadySFXAtMissionStart = true;
};

// static_assert((offsetof(Config, kalinaAnnHookGrenadeHeight) % 0x10) == 0);
// static_assert((offsetof(Config, kalinaAnnHookGrenadeTime) % 0x10) == 0);
// static_assert((offsetof(Config, kalinaAnnHookMultiplier) % 0x10) == 0);
// static_assert((offsetof(Config, barsData) % 0x10) == 0);



#pragma pack(pop)

#undef _

extern Config defaultConfig;
extern Config queuedConfig;
extern Config activeConfig;


PlayerData& GetDefaultPlayerData(uint8 playerIndex);

PlayerData& GetActivePlayerData(uint8 playerIndex);

PlayerData& GetQueuedPlayerData(uint8 playerIndex);

PlayerData& GetPlayerData(uint8 playerIndex);

template <typename T> PlayerData& GetDefaultPlayerData(T& actorData) {
    return GetDefaultPlayerData(actorData.newPlayerIndex);
}

template <typename T> PlayerData& GetActivePlayerData(T& actorData) {
    return GetActivePlayerData(actorData.newPlayerIndex);
}

template <typename T> PlayerData& GetQueuedPlayerData(T& actorData) {
    return GetQueuedPlayerData(actorData.newPlayerIndex);
}

template <typename T> PlayerData& GetPlayerData(T& actorData) {
    return GetPlayerData(actorData.newPlayerIndex);
}

CharacterData& GetDefaultCharacterData(uint8 playerIndex, uint8 characterIndex, uint8 entityIndex);

CharacterData& GetActiveCharacterData(uint8 playerIndex, uint8 characterIndex, uint8 entityIndex);

CharacterData& GetQueuedCharacterData(uint8 playerIndex, uint8 characterIndex, uint8 entityIndex);

CharacterData& GetCharacterData(uint8 playerIndex, uint8 characterIndex, uint8 entityIndex);


template <typename T> CharacterData& GetDefaultCharacterData(T& actorData) {
    return GetDefaultCharacterData(actorData.newPlayerIndex, actorData.newCharacterIndex, actorData.newEntityIndex);
}

template <typename T> CharacterData& GetActiveCharacterData(T& actorData) {
    return GetActiveCharacterData(actorData.newPlayerIndex, actorData.newCharacterIndex, actorData.newEntityIndex);
}

template <typename T> CharacterData& GetQueuedCharacterData(T& actorData) {
    return GetQueuedCharacterData(actorData.newPlayerIndex, actorData.newCharacterIndex, actorData.newEntityIndex);
}

template <typename T> CharacterData& GetCharacterData(T& actorData) {
    return GetCharacterData(actorData.newPlayerIndex, actorData.newCharacterIndex, actorData.newEntityIndex);
}

void ApplyDefaultCharacterData(CharacterData& characterData, uint8 character);

void ApplyDefaultPlayerData(PlayerData& playerData);

void InitConfig();

#ifndef NO_SAVE
void SaveConfig();
#endif

#ifndef NO_LOAD
void LoadConfig();
#endif

#ifndef NO_INIT
void InitConfig();
#endif


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
        bool enable                         = true;
        uint8 playerCount                   = 1;
        PlayerData playerData[PLAYER_COUNT] = {};
    } Actor;

    struct {
        bool enable         = false;
        uint32 mission      = 1;
        uint32 mode         = DIFFICULTY_MODE::NORMAL;
        uint32 room         = 900;
        bool enableRoomSelection     = false;
        uint32 position     = 0;
        bool enablePositionSelection = false;
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

	struct  {
		uint8 airHike[5][4] = {
			{128, 0, 16, 200},
			{128, 0, 16, 200},
			{128, 0, 16, 200},
			{128, 0, 16, 200},
			{128, 0, 16, 200},
		};

		struct {
			uint8 skyStar[4] = { 128, 0, 16, 200 };
		} Trickster;
		struct {
			uint8 ultimate[4] = { 128, 0, 16, 200 };
		} Royalguard;
		struct {
			uint8 clone[4] = { 16, 16, 16, 48 };
		} Doppelganger;
		struct {
			uint8 dante[5][4] = {
				{128, 0, 16, 200},
				{128, 0, 16, 200},
				{128, 0, 16, 200},
				{128, 0, 16, 200},
				{128, 0, 16, 200},
			};
			uint8 sparda[4] = { 128, 0, 0, 200 };
			uint8 vergil[3][4] = {
				{32, 64, 128, 200},
				{32, 64, 128, 200},
				{32, 64, 128, 200},
			};
			uint8 neroAngelo[4] = { 64, 0, 255, 200 };
		} Aura;
	} Color;


    struct {
        float stingerDuration[2]    = {16, 16};
        float stingerRange[2]       = {560, 560};
        uint8 airStingerCount[2]    = {1, 1};
        float airStingerDuration[2] = {3, 4};
        float airStingerRange[2]    = {0, 140};
    } Rebellion;

    struct {
        float mainSpeed = 1.0f;
        float turbo     = 1.2f;
    } Speed;

    struct {
        float stingerDuration[2]    = {16, 16};
        float stingerRange[2]       = {560, 560};
        uint8 airStingerCount[2]    = {1, 1};
        float airStingerDuration[2] = {5, 5};
        float airStingerRange[2]    = {280, 280};
    } YamatoForceEdge;

    bool welcome = true;

    bool hideBeowulfDante  = false;
    bool hideBeowulfVergil = false;

    uint8 dotShadow = DOT_SHADOW::DISABLE;

    bool noDevilForm = false;
   
    bool resetPermissions = false;

    bool forceIconFocus = false;

    bool skipIntro     = false;
    bool skipCutscenes = false;

    bool enableFileMods = true;

    float frameRate = 120;
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

	KeyData keyData[5] = {
	    { { DI8::KEY::DELETE }, 1, false },
		{ { DI8::KEY::F1 }, 1, true },
		{ { DI8::KEY::F3 }, 1, true },
	    {}, 
	    {}, 
	};

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

void ApplyDefaultCharacterData(CharacterData& characterData, uint8 character, uint8 playerIndex, uint8 characterIndex);

void ApplyDefaultPlayerData(PlayerData& playerData);

void InitConfig();

#ifndef NO_SAVE
void SaveConfig();
#endif

#ifndef NO_SAVE
void SaveConfigGameplay();
#endif

#ifndef NO_LOAD
void LoadConfig();
#endif

#ifndef NO_LOAD
void LoadConfigGameplay();
#endif

#ifndef NO_INIT
void InitConfig();
#endif


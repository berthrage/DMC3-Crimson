#pragma once

#include <vector>

#include "Core/Core.hpp"
#include "ImGuiExtra.hpp"
#include "Core/Macros.h"
#include "DebugDrawDX11.hpp"
#include <string>

#pragma push_macro("VOID")
#pragma push_macro("IGNORE")
#undef VOID
#undef IGNORE

namespace UNLOCK_DANTE {
enum {
    REBELLION_STINGER_LEVEL_1,
    REBELLION_STINGER_LEVEL_2,
    REBELLION_DRIVE,
    REBELLION_AIR_HIKE,

    CERBERUS_REVOLVER_LEVEL_2,
    CERBERUS_WINDMILL,

    AGNI_RUDRA_JET_STREAM_LEVEL_2,
    AGNI_RUDRA_JET_STREAM_LEVEL_3,
    AGNI_RUDRA_WHIRLWIND,
    AGNI_RUDRA_AIR_HIKE,

    NEVAN_REVERB_SHOCK_LEVEL_1,
    NEVAN_REVERB_SHOCK_LEVEL_2,
    NEVAN_BAT_RIFT_LEVEL_2,
    NEVAN_AIR_RAID,
    NEVAN_VOLUME_UP,

    BEOWULF_STRAIGHT_LEVEL_2,
    BEOWULF_BEAST_UPPERCUT,
    BEOWULF_RISING_DRAGON,
    BEOWULF_AIR_HIKE,

    EBONY_IVORY_LEVEL_2,
    EBONY_IVORY_LEVEL_3,

    SHOTGUN_LEVEL_2,
    SHOTGUN_LEVEL_3,

    ARTEMIS_LEVEL_2,
    ARTEMIS_LEVEL_3,

    SPIRAL_LEVEL_2,
    SPIRAL_LEVEL_3,

    KALINA_ANN_LEVEL_2,
    KALINA_ANN_LEVEL_3,

    COUNT,
};
};

namespace UNLOCK_VERGIL {
enum {
    YAMATO_RAPID_SLASH_LEVEL_1,
    YAMATO_RAPID_SLASH_LEVEL_2,
    YAMATO_JUDGEMENT_CUT_LEVEL_1,
    YAMATO_JUDGEMENT_CUT_LEVEL_2,

    BEOWULF_STARFALL_LEVEL_2,
    BEOWULF_RISING_SUN,
    BEOWULF_LUNAR_PHASE_LEVEL_2,

    YAMATO_FORCE_EDGE_HELM_BREAKER_LEVEL_2,
    YAMATO_FORCE_EDGE_STINGER_LEVEL_1,
    YAMATO_FORCE_EDGE_STINGER_LEVEL_2,
    YAMATO_FORCE_EDGE_ROUND_TRIP,

    SUMMONED_SWORDS_LEVEL_2,
    SUMMONED_SWORDS_LEVEL_3,
    SPIRAL_SWORDS,

    COUNT,
};
};

namespace MISSION {
enum {
    BLOODY_PALACE = 21,
};
};

#pragma region Enums

enum {
    CopyStateFlags_EventData      = 1 << 0,
    CopyStateFlags_Mode           = 1 << 1,
    CopyStateFlags_FixPermissions = 1 << 2,
};

enum {
    CreateEnemyActorFlags_Reset = 1 << 0,
};

// @Todo: Use new enum.
enum {
    Visibility_Default,
    Visibility_Show,
    Visibility_Hide,
};

enum {
    LOWER_BODY,
    UPPER_BODY,
};

enum {
    PLAYER_COUNT              = 4,
    ENTITY_COUNT              = 2,
    CHARACTER_COUNT           = 3,
    STYLE_COUNT               = 4,
    WEAPON_COUNT              = 10,
    WEAPON_COUNT_DANTE        = 10,
    WEAPON_COUNT_BOB          = 1,
    WEAPON_COUNT_VERGIL       = 3,
    MELEE_WEAPON_COUNT        = 5,
    MELEE_WEAPON_COUNT_DANTE  = 5,
    MELEE_WEAPON_COUNT_BOB    = 1,
    MELEE_WEAPON_COUNT_VERGIL = 3,
    RANGED_WEAPON_COUNT       = 5,
    RANGED_WEAPON_COUNT_DANTE = 5,
    CREATE_ENEMY_COUNT        = 30,
    SAVE_COUNT                = 10,
};


enum {
    RIGHT_STICK,
    LEFT_STICK,
    RIGHT_STICK_DEADZONE = 90,
    LEFT_STICK_DEADZONE  = 52,
};

namespace GAMEPAD {
enum {
    LEFT_TRIGGER = 0x1,
    RIGHT_TRIGGER = 0x2,
    LEFT_SHOULDER = 0x4,
    RIGHT_SHOULDER = 0x8,
    Y = 0x10,
    B = 0x20,
    A = 0x40,
    X = 0x80,
    BACK = 0x100,
    LEFT_STICK_CLICK = 0x200,
    RIGHT_STICK_CLICK = 0x400,
    LEFT_PLUS_RIGHT_STICK_CLICK = LEFT_STICK_CLICK | RIGHT_STICK_CLICK,
    START          = 0x800,
    // Direction ids are the same for dpad, left stick and right stick.
    UP    = 0x1000,
    RIGHT = 0x2000,
    DOWN  = 0x4000,
    LEFT  = 0x8000,
};
};

namespace DIRECTION {
enum {
    UP,
    RIGHT,
    DOWN,
    LEFT,
    MAX,
};
};

namespace BINDING {
enum {
    ITEM_SCREEN,  // UP
    EQUIP_SCREEN, // DOWN
    MAP_SCREEN,   // RIGHT
    FILE_SCREEN,  // LEFT
    MELEE_ATTACK,
    JUMP,
    STYLE_ACTION,
    SHOOT,
    DEVIL_TRIGGER,
    CHANGE_GUN,
    CHANGE_TARGET,
    LOCK_ON,
    CHANGE_DEVIL_ARMS,
    DEFAULT_CAMERA,
    TAUNT,
};
};

namespace SCENE {
enum {
    BOOT,
    INTRO,
    MAIN,
    MISSION_SELECT,
    LOAD,
    GAME,
    CUTSCENE,
    MISSION_START,
    MISSION_RESULT,
    GAME_OVER,
    COUNT,
};
};

namespace EVENT {
enum {
    INIT,
    MAIN,
    TELEPORT,
    PAUSE,
    STATUS,
    OPTIONS,
    DEATH,
    ITEM,
    MESSAGE,
    CUSTOMIZE,
    SAVE,
    DELETE,
    END,
    COUNT,
};
};

namespace SCREEN {
enum {
    MISSION_CLEAR = 5,
    GAME_OVER,
    MISSION_START,
    MISSION_SELECT,
};
};

namespace DIFFICULTY_MODE {
enum {
    EASY,
    NORMAL,
    HARD,
    VERY_HARD,
    DANTE_MUST_DIE,
    COUNT,
    HEAVEN_OR_HELL, // Does not exist! Just kept for convenience.
};
};

namespace COLLISION_GROUP {
enum {
    PLAYER = 0,
    ENEMY  = 2,
};
};

enum {
    CollisionFlags_Player = 0x30443, // dmc3.exe+173B5D - C7 86 D0020000 43040300 - mov [rsi+000002D0],00030443
    CollisionFlags_Enemy  = 0x3050C, // dmc3.exe+173B76 - C7 86 D0020000 0C050300 - mov [rsi+000002D0],0003050C
};

namespace PLAYER_ACTOR_DATA_SIZE {
enum {
    DANTE  = 47296,
    BOB    = 46720,
    LADY   = 33408,
    VERGIL = 47296,
};
};

namespace ENTITY {
enum {
    MAIN,
    CLONE,
};
};

namespace CHARACTER {
enum {
    DANTE,
    BOB,
    LADY,
    VERGIL,
    MAX,
    BOSS_LADY,
    BOSS_VERGIL,
};
};

namespace COSTUME {
enum {
    DANTE_DEFAULT,
    DANTE_DEFAULT_NO_COAT,
    DANTE_DEFAULT_TORN,
    DANTE_DMC1,
    DANTE_DMC1_NO_COAT,
    DANTE_SPARDA,
    DANTE_DEFAULT_TORN_INFINITE_MAGIC_POINTS,
    DANTE_SPARDA_INFINITE_MAGIC_POINTS,
    BOB_DEFAULT  = 0,
    LADY_DEFAULT = 0,
    LADY_LEATHER_JUMPSUIT,
    VERGIL_DEFAULT = 0,
    VERGIL_DEFAULT_NO_COAT,
    VERGIL_DEFAULT_INFINITE_MAGIC_POINTS,
    VERGIL_NERO_ANGELO,
    VERGIL_NERO_ANGELO_INFINITE_MAGIC_POINTS,
    MAX        = 8,
    MAX_DANTE  = 8,
    MAX_BOB    = 1,
    MAX_LADY   = 2,
    MAX_VERGIL = 5,
};
};

namespace STYLE {
enum {
    SWORDMASTER,
    GUNSLINGER,
    TRICKSTER,
    ROYALGUARD,
    QUICKSILVER,
    DOPPELGANGER,
    DARK_SLAYER = 2,
    MAX         = 6,
};
};

namespace STYLE_LEVEL {
    enum {
        LEVEL_ONE,
        LEVEL_TWO,
        LEVEL_THREE,
    };
};

namespace STYLE_LEVEL_EXP {
    enum {
        LEVEL_ONE = 0,
        LEVEL_TWO = 30000,
        LEVEL_THREE = 99999,
    };
};

namespace STYLE_UNLOCKS {
    enum {
        SWORDMASTER_LEVEL_TWO,
        SWORDMASTER_LEVEL_THREE,
        GUNSLINGER_LEVEL_TWO,
        GUNSLINGER_LEVEL_THREE,
        TRICKSTER_LEVEL_TWO,
        TRICKSTER_LEVEL_THREE,
        ROYALGUARD_LEVEL_TWO,
        ROYALGUARD_LEVEL_THREE,
        QUICKSILVER_LEVEL_TWO,
        QUICKSILVER_LEVEL_THREE,
        DOPPELGANGER_LEVEL_TWO,
        DOPPELGANGER_LEVEL_THREE,
        DARK_SLAYER_LEVEL_TWO,
        DARK_SLAYER_LEVEL_THREE,
    };
};

namespace STYLE_RANK {
enum {
    NONE,
    DOPE,
    CRAZY,
    BLAST,
    ALRIGHT,
    SWEET,
    SHOWTIME,
    STYLISH,
    COUNT,
};
};

namespace WEAPON {
enum {
    REBELLION,
    CERBERUS,
    AGNI_RUDRA,
    NEVAN,
    BEOWULF_DANTE,
    EBONY_IVORY,
    SHOTGUN,
    ARTEMIS,
    SPIRAL,
    KALINA_ANN,
    YAMATO_VERGIL = 11,
    BEOWULF_VERGIL,
    YAMATO_FORCE_EDGE,
    YAMATO_BOB,
    MAX,
    VOID = 255,
};
};

namespace WEAPON_SWITCH_TYPE {
enum {
    LINEAR,
    ARBITRARY,
    COUNT,
};
};

namespace WEAPON_STATUS {
enum {
    READY,
    ACTIVE,
    RETURN,
    END,
    DISABLED,
};
};

namespace MOTION_GROUP_DANTE {
enum {
    BASE,
    DAMAGE,
    TAUNTS,
    REBELLION,
    CERBERUS,
    AGNI_RUDRA,
    NEVAN,
    BEOWULF,
    EBONY_IVORY,
    SHOTGUN,
    ARTEMIS,
    SPIRAL,
    KALINA_ANN,
    SWORDMASTER_REBELLION,
    SWORDMASTER_CERBERUS,
    SWORDMASTER_AGNI_RUDRA,
    SWORDMASTER_NEVAN,
    SWORDMASTER_BEOWULF,
    GUNSLINGER_EBONY_IVORY,
    GUNSLINGER_SHOTGUN,
    GUNSLINGER_ARTEMIS,
    GUNSLINGER_SPIRAL,
    GUNSLINGER_KALINA_ANN,
    TRICKSTER,
    ROYALGUARD,
    QUICKSILVER,
    DOPPELGANGER,
};
};

namespace MOTION_GROUP_BOB {
enum {
    BASE,
    DAMAGE,
    TAUNTS,
    YAMATO = 31,
};
};

namespace MOTION_GROUP_LADY {
enum {
    BASE,
    DAMAGE,
    TAUNTS,
};
};

namespace MOTION_GROUP_VERGIL {
enum {
    BASE,
    DAMAGE,
    TAUNTS,
    YAMATO,
    BEOWULF,
    YAMATO_FORCE_EDGE,
    DARK_SLAYER,
    NERO_ANGELO_YAMATO,
    NERO_ANGELO_BEOWULF,
    NERO_ANGELO_FORCE_EDGE,
};
};

namespace MOTION_GROUP {
enum {
    CUTSCENE = 33,
    MAX      = 34,
};
};

namespace ACTION_DANTE {
enum {
    REBELLION_COMBO_1_PART_1 = 1,
    REBELLION_COMBO_1_PART_2,
    REBELLION_COMBO_1_PART_3,
    REBELLION_COMBO_2_PART_2,
    REBELLION_COMBO_2_PART_3,
    REBELLION_HELM_BREAKER,
    REBELLION_STINGER_LEVEL_1,
    REBELLION_STINGER_LEVEL_2,
    REBELLION_LEAP,
    REBELLION_HIGH_TIME,
    REBELLION_HIGH_TIME_LAUNCH,
    REBELLION_KICK_1,
    REBELLION_DRIVE_1,
    REBELLION_MILLION_STAB,
    REBELLION_SWORD_PIERCE,
    REBELLION_PROP,
    REBELLION_DIVEKICK,
    REBELLION_KICK_2,
    REBELLION_AERIAL_RAVE_PART_1,
    REBELLION_AERIAL_RAVE_PART_2,
    REBELLION_AERIAL_RAVE_PART_3,
    REBELLION_AERIAL_RAVE_PART_4,
    REBELLION_SWORD_PIERCE_RETURN,
    REBELLION_SHREDDER,
    REBELLION_DRIVE_2,
    CERBERUS_COMBO_1_PART_1 = 30,
    CERBERUS_COMBO_1_PART_2,
    CERBERUS_COMBO_1_PART_3,
    CERBERUS_COMBO_1_PART_4,
    CERBERUS_COMBO_1_PART_5,
    CERBERUS_COMBO_2_PART_3,
    CERBERUS_COMBO_2_PART_4,
    CERBERUS_WINDMILL,
    CERBERUS_REVOLVER_LEVEL_1,
    CERBERUS_REVOLVER_LEVEL_2,
    CERBERUS_SWING,
    CERBERUS_SATELLITE,
    CERBERUS_FLICKER,
    CERBERUS_AIR_FLICKER,
    CERBERUS_CRYSTAL,
    CERBERUS_MILLION_CARATS,
    CERBERUS_ICE_AGE,
    AGNI_RUDRA_COMBO_1_PART_1 = 50,
    AGNI_RUDRA_COMBO_1_PART_2,
    AGNI_RUDRA_COMBO_1_PART_3,
    AGNI_RUDRA_COMBO_1_PART_4,
    AGNI_RUDRA_COMBO_1_PART_5,
    AGNI_RUDRA_COMBO_2_PART_2,
    AGNI_RUDRA_COMBO_2_PART_3,
    AGNI_RUDRA_COMBO_3_PART_3,
    AGNI_RUDRA_JET_STREAM_LEVEL_1,
    AGNI_RUDRA_JET_STREAM_LEVEL_2,
    AGNI_RUDRA_JET_STREAM_LEVEL_3,
    AGNI_RUDRA_AERIAL_CROSS,
    AGNI_RUDRA_WHIRLWIND,
    AGNI_RUDRA_WHIRLWIND_LAUNCH,
    AGNI_RUDRA_MILLION_SLASH,
    AGNI_RUDRA_CROSSED_SWORDS,
    AGNI_RUDRA_CRAWLER,
    AGNI_RUDRA_TWISTER,
    AGNI_RUDRA_SKY_DANCE_PART_1,
    AGNI_RUDRA_SKY_DANCE_PART_2,
    AGNI_RUDRA_SKY_DANCE_PART_3,
    AGNI_RUDRA_TEMPEST,
    NEVAN_TUNE_UP = 80,
    NEVAN_COMBO_1,
    NEVAN_COMBO_2,
    NEVAN_JAM_SESSION,
    NEVAN_BAT_RIFT_LEVEL_1,
    NEVAN_BAT_RIFT_LEVEL_2,
    NEVAN_REVERB_SHOCK_LEVEL_1,
    NEVAN_REVERB_SHOCK_LEVEL_2,
    NEVAN_AIR_PLAY,
    NEVAN_SLASH,
    NEVAN_AIR_SLASH_PART_1,
    NEVAN_AIR_SLASH_PART_2,
    NEVAN_FEEDBACK,
    NEVAN_CRAZY_ROLL,
    NEVAN_DISTORTION,
    NEVAN_VORTEX,
    NEVAN_THUNDER_BOLT,
    BEOWULF_COMBO_1_PART_1 = 110,
    BEOWULF_COMBO_1_PART_2,
    BEOWULF_COMBO_1_PART_3,
    BEOWULF_COMBO_2_PART_3,
    BEOWULF_COMBO_2_PART_4,
    BEOWULF_HYPER_FIST,
    BEOWULF_KILLER_BEE,
    BEOWULF_BEAST_UPPERCUT,
    BEOWULF_RISING_DRAGON,
    BEOWULF_RISING_DRAGON_LAUNCH,
    BEOWULF_RISING_DRAGON_WHIRLWIND,
    BEOWULF_STRAIGHT_LEVEL_1,
    BEOWULF_STRAIGHT_LEVEL_2,
    BEOWULF_ZODIAC,
    BEOWULF_VOLCANO,
    BEOWULF_AIR_VOLCANO,
    BEOWULF_TORNADO,
    BEOWULF_THE_HAMMER,
    BEOWULF_REAL_IMPACT,
    EBONY_IVORY_NORMAL_SHOT = 130,
    EBONY_IVORY_CHARGED_SHOT,
    EBONY_IVORY_AIR_NORMAL_SHOT,
    EBONY_IVORY_AIR_CHARGED_SHOT,
    EBONY_IVORY_TWOSOME_TIME,
    EBONY_IVORY_RAIN_STORM,
    EBONY_IVORY_WILD_STOMP,
    SHOTGUN_NORMAL_SHOT = 140,
    SHOTGUN_CHARGED_SHOT,
    SHOTGUN_AIR_NORMAL_SHOT,
    SHOTGUN_AIR_CHARGED_SHOT,
    SHOTGUN_FIREWORKS,
    SHOTGUN_AIR_FIREWORKS,
    SHOTGUN_GUN_STINGER,
    SHOTGUN_POINT_BLANK,
    ARTEMIS_NORMAL_SHOT = 150,
    ARTEMIS_AIR_NORMAL_SHOT,
    ARTEMIS_MULTI_LOCK_SHOT,
    ARTEMIS_AIR_MULTI_LOCK_SHOT,
    ARTEMIS_SPHERE,
    ARTEMIS_ACID_RAIN,
    SPIRAL_NORMAL_SHOT = 160,
    SPIRAL_SNIPER,
    SPIRAL_TRICK_SHOT,
    KALINA_ANN_NORMAL_SHOT = 170,
    KALINA_ANN_HYSTERIC,
    KALINA_ANN_GRAPPLE,
    TRICKSTER_DASH = 180,
    TRICKSTER_SKY_STAR,
    TRICKSTER_AIR_TRICK,
    TRICKSTER_WALL_HIKE,
    ROYALGUARD_RELEASE_1 = 190,
    ROYALGUARD_RELEASE_2,
    ROYALGUARD_RELEASE_3,
    ROYALGUARD_RELEASE_4,
    ROYALGUARD_AIR_RELEASE_1,
    ROYALGUARD_AIR_RELEASE_2,
    ROYALGUARD_AIR_RELEASE_3,
    ROYALGUARD_AIR_RELEASE_4,
    REBELLION_DANCE_MACABRE_PART_1 = 200,
    REBELLION_DANCE_MACABRE_PART_2,
    REBELLION_DANCE_MACABRE_PART_3,
    REBELLION_DANCE_MACABRE_PART_4,
    REBELLION_DANCE_MACABRE_PART_5,
    REBELLION_DANCE_MACABRE_PART_6,
    REBELLION_DANCE_MACABRE_PART_7,
    REBELLION_DANCE_MACABRE_PART_8,
    REBELLION_CRAZY_DANCE,
    POLE_PLAY,
    ROYAL_BLOCK,
    ROYAL_AIR_BLOCK,
};
};

namespace ACTION_VERGIL {
enum {
    YAMATO_COMBO_PART_1 = 1,
    YAMATO_COMBO_PART_2,
    YAMATO_COMBO_PART_3,
    YAMATO_RAPID_SLASH_LEVEL_1,
    YAMATO_RAPID_SLASH_LEVEL_2,
    YAMATO_LEAP,
    YAMATO_UPPER_SLASH_PART_1,
    YAMATO_UPPER_SLASH_PART_2,
    YAMATO_JUDGEMENT_CUT_LEVEL_1,
    YAMATO_JUDGEMENT_CUT_LEVEL_2,
    YAMATO_AERIAL_RAVE_PART_1,
    YAMATO_AERIAL_RAVE_PART_2,
    BEOWULF_COMBO_PART_1 = 20,
    BEOWULF_COMBO_PART_2,
    BEOWULF_COMBO_PART_3,
    BEOWULF_STARFALL_LEVEL_1,
    BEOWULF_STARFALL_LEVEL_2,
    BEOWULF_RISING_SUN,
    BEOWULF_LUNAR_PHASE_LEVEL_1,
    BEOWULF_LUNAR_PHASE_LEVEL_2,
    YAMATO_FORCE_EDGE_COMBO_PART_1 = 30,
    YAMATO_FORCE_EDGE_COMBO_PART_2,
    YAMATO_FORCE_EDGE_COMBO_PART_3,
    YAMATO_FORCE_EDGE_COMBO_PART_4,
    YAMATO_FORCE_EDGE_HELM_BREAKER_LEVEL_1,
    YAMATO_FORCE_EDGE_HELM_BREAKER_LEVEL_2,
    YAMATO_FORCE_EDGE_HIGH_TIME,
    YAMATO_FORCE_EDGE_HIGH_TIME_LAUNCH,
    YAMATO_FORCE_EDGE_STINGER_LEVEL_1,
    YAMATO_FORCE_EDGE_STINGER_LEVEL_2,
    YAMATO_FORCE_EDGE_ROUND_TRIP,
    YAMATO_FORCE_EDGE_KICK,
    SUMMONED_SWORDS_SHOOT = 52,
    SUMMONED_SWORDS_SPIRAL_SWORDS,
    SUMMONED_SWORDS_BLISTERING_SWORDS,
    SUMMONED_SWORDS_SWORD_STORM,
    DARK_SLAYER_AIR_TRICK = 60,
    DARK_SLAYER_TRICK_UP,
    DARK_SLAYER_TRICK_DOWN,
    NERO_ANGELO_COMBO_1_PART_1 = 70,
    NERO_ANGELO_COMBO_1_PART_2,
    NERO_ANGELO_COMBO_1_PART_3,
    NERO_ANGELO_HELM_BREAKER,
    NERO_ANGELO_HIGH_TIME,
    NERO_ANGELO_HIGH_TIME_LAUNCH,
    NERO_ANGELO_STINGER,
    NERO_ANGELO_FIREBALL_1,
    NERO_ANGELO_COMBO_2_PART_1 = 90,
    NERO_ANGELO_COMBO_2_PART_2,
    NERO_ANGELO_COMBO_2_PART_3,
    NERO_ANGELO_DIVEKICK,
    NERO_ANGELO_ROUNDHOUSE_KICK,
    NERO_ANGELO_UPPERCUT,
    NERO_ANGELO_FIREBALL_2,
};
};

// $ActorEventStart

namespace ACTOR_EVENT {
enum {
    UNKNOWN_0,
    UNKNOWN_1,
    UNKNOWN_2,
    UNKNOWN_3,
    UNKNOWN_4,
    JUMP,
    UNKNOWN_5,
    AIR_HIKE,
    UNKNOWN_6,
    AIR_TRICK_END,
    LOCK_ON,
    UNKNOWN_8,
    UNKNOWN_9,
    UNKNOWN_10,
    UNKNOWN_11,
    UNKNOWN_12,
    UNKNOWN_13,
    ATTACK,
    TAUNT,
    ROYALGUARD_BLOCK,
    ROYALGUARD_BLOCK_CONNECT,
    ROYALGUARD_RELEASE_BLOCKED,
    TRICKSTER_DASH,
    TRICKSTER_SKY_STAR,
    TRICKSTER_AIR_TRICK,
    QUICKSILVER_TIME_LAG,
    UNKNOWN_14,
    DARK_SLAYER_AIR_TRICK,
    DARK_SLAYER_TRICK_UP,
    DARK_SLAYER_TRICK_DOWN,
    MELEE_ATTACK_BLOCKED,
    UNKNOWN_15,
    UNKNOWN_16,
    JUMP_CANCEL,
    UNKNOWN_18,
    UNKNOWN_19,
    UNKNOWN_20,
    UNKNOWN_21,
    UNKNOWN_22,
    UNKNOWN_23,
    NEVAN_AIR_RAID,
    SUPER_JUMP,
    UNKNOWN_24,
    HOLY_WATER,
    STAGGER,
    DEATH,
    NEVAN_KISS,
    COUNT,
};
};

// $ActorEventEnd

static_assert(ACTOR_EVENT::JUMP == 5);
static_assert(ACTOR_EVENT::TRICKSTER_DASH == 22);
static_assert(ACTOR_EVENT::TRICKSTER_SKY_STAR == 23);
static_assert(ACTOR_EVENT::TRICKSTER_AIR_TRICK == 24);
static_assert(ACTOR_EVENT::DARK_SLAYER_AIR_TRICK == 27);
static_assert(ACTOR_EVENT::DARK_SLAYER_TRICK_UP == 28);
static_assert(ACTOR_EVENT::DARK_SLAYER_TRICK_DOWN == 29);
static_assert(ACTOR_EVENT::NEVAN_AIR_RAID == 40);
static_assert(ACTOR_EVENT::COUNT == 47);

namespace EVENT_ENEMY {
enum {
    IDLE = 13,
    SPAWN,
    DEATH = 20,
};
};

namespace EVENT_BOSS_LADY {
enum {
    PISTOL_LOCK_ON_WALK = 23,
    PISTOL_LOCK_ON,
    PISTOL_SHOT,
    SMG_ROUNDHOUSE,
    KALINA_ANN_CHARGED_SHOT,
    KALINA_ANN_HYSTERIC,
    KALINA_ANN_HOOK,
    GRENADE_THROW,
    FALL_BACK,
    FALL_BACK_SHOOT,
    FALL_BACK_RECOVER,
    FALL_BACK_RECOVER_CROSSBOW,
    FALL_BACK_HIT_FLOOR,
    TROOPER_ROLL,
    WHEEL_BACK,
    WHEEL_RIGHT,
    WHEEL_LEFT,
    RUN,
    BLOCK,
    BLOCK_2,
    BLOCK_3,
    PISTOL_RELOAD,
    SMG_RELOAD,
    DEATH,
    RUN_SHORT,
    PISTOL_LOCK_ON_WALK_2,
    SMG_ROUNDHOUSE_2,
    KALINA_ANN_CHARGED_SHOT_2,
    IDLE,
};
};

static_assert(EVENT_BOSS_LADY::IDLE == 51);

namespace EVENT_BOSS_VERGIL {
enum {
    IDLE = 23,
    DEATH,
    WALK,
    IDLE_2,
    IDLE_3,
    WALK_2,
    STRAFE,
    STRAFE_2,
    IDLE_4,
    TAUNT,
    YAMATO_COMBO,
    YAMATO_COMBO_2,
    YAMATO_FORCE_EDGE_COMBO,
    YAMATO_FORCE_EDGE_COMBO_2,
    YAMATO_UPPER_SLASH,
    BEOWULF_COMBO,
    BEOWULF_COMBO_2,
    AIR_TRICK,
    CRASH,
    TRICK_UP,
    TRICK_DOWN,
    YAMATO_3_JUDGEMENT_CUTS,
    YAMATO_RAPID_SLASH,
    FORCE_EDGE_STINGER,
    YAMATO_JUDGEMENT_CUT,
    YAMATO_3_JUDGEMENT_CUTS_2,
    AIR_TRICK_2,
    DAMAGE,
    BLOCK,
    DAMAGE_2,
    DAMAGE_3,
    DAMAGE_4,
    DAMAGE_5,
    TAUNT_2,
    STRONG_SHIELD,
    FORCE_EDGE_ROUND_TRIP,
    FORCE_EDGE_HELM_BREAKER,
    FORCE_EDGE_4_HELM_BREAKERS,
    CRASH_2,
    TRICK_BACK,
    YAMATO_SUPER_JUDGEMENT_CUT,
    YAMATO_SUPER_JUDGEMENT_CUT_FOLLOW,
    SHIELD,
    YAMATO_DEFLECT,
    LEAVE_DEVIL_FORM,
    REST_IN_PEACE,
};
};

static_assert(EVENT_BOSS_VERGIL::LEAVE_DEVIL_FORM == 67);

namespace ACTOR_MODE {
enum {
    DEFAULT,
    MISSION_12,
    MISSION_18,
    MISSION_19,
};
};

namespace CLONE_STATUS {
enum {
    IDLE,
    ACTIVE,
    DECOMMISSION,
};
};

namespace DEVIL {
enum {
    REBELLION,
    CERBERUS,
    AGNI_RUDRA,
    NEVAN,
    BEOWULF,
    SPARDA,
    NERO_ANGELO,
    YAMATO = 0,
    MAX    = 7,
};
};

namespace DEVIL_FLUX {
enum {
    START    = 1,
    END      = 2,
    GLOW_ON  = 3,
    GLOW_OFF = 4,
};
};

namespace NEXT_ACTION_REQUEST_POLICY {
enum {
    IGNORE,
    BUFFER,
    EXECUTE,
    // Indices
    MELEE_ATTACK           = 0,
    SWORDMASTER_GUNSLINGER = 4,
    JUMP_ROLL              = 5,
    ROYALGUARD             = 6,
    TRICKSTER_DARK_SLAYER  = 8,
    RANGED_ATTACK          = 10,
    END                    = 15,
};
};

namespace PERMISSION {
enum {
    UPDATE                   = 1,
    WALK_RUN                 = 2,
    JUMP_ROLL                = 8,
    TARGET                   = 0x10,
    RELEASE                  = 0x20,
    INTERACTION_STYLE_ATTACK = 0x400,
    TRICKSTER_DARK_SLAYER    = 0x1000000,
};
};

namespace STATE {
enum {
    ON_FLOOR = 1,
    IN_AIR   = 2,
    BUSY     = 0x10000,
};
};

namespace QUICKSILVER_STAGE {
enum {
    DEFAULT,
    TO_ON,
    ON,
    TO_OFF,
    OFF,
};
};

namespace WALL_HIKE_DIRECTION {
enum {
    RIGHT,
    LEFT,
    FORWARD,
};
};

namespace SPEED {
enum {
    GLOBAL,
    GLOBAL_2,
    GLOBAL_3,
    PLAYER_ACTOR,
    ENEMY_ACTOR,
    PLAYER_ACTOR_2,
    ENEMY_ACTOR_2,
    GLOBAL_4,
};
};

namespace DEVIL_SPEED {
enum {
    DANTE_REBELLION,
    DANTE_CERBERUS,
    DANTE_AGNI_RUDRA,
    DANTE_NEVAN,
    DANTE_BEOWULF,
    DANTE_SPARDA,
    VERGIL_YAMATO = 0,
    VERGIL_BEOWULF,
    VERGIL_YAMATO_FORCE_EDGE,
    VERGIL_NERO_ANGELO_YAMATO,
    VERGIL_NERO_ANGELO_BEOWULF,
};
};

namespace EFFECT {
enum {
    CLEAR = 0x8000000, // dmc3.exe+1F7CD2 - 81 8B F4CA0100 00000008 - or [rbx+0001CAF4],08000000
};
};

namespace FILE_MODE {
enum {
    MEMORY,
    ARCHIVE,
    LOCAL,
};
};

namespace CHANNEL {
enum {
    SYSTEM,
    COMMON,
    STYLE_WEAPON,
    WEAPON1,
    WEAPON2,
    WEAPON3,
    WEAPON4,
    ENEMY,
    ROOM,
    MUSIC,
    DEMO,
    MAX,
};
};

namespace HUD_TOP {
enum {
    UPPER_HIT_POINTS_FRAME,
    UPPER_HIT_POINTS_BAR,
    UPPER_HIT_POINTS_BACKGROUND,
    LOWER_HIT_POINTS_FRAME,
    LOWER_HIT_POINTS_BAR,
    LOWER_HIT_POINTS_BACKGROUND,
    MAGIC_ORBS,
    STYLE_ICON,
    RED_ORB_COUNTER,
    UNKNOWN_0,
    UNKNOWN_1,
    FLUX,
    DTEXPLOSION,
};
};

namespace HUD_BOTTOM {
enum {
    RANGED_WEAPON_1,
    RANGED_WEAPON_2,
    MELEE_WEAPON_1,
    MELEE_WEAPON_2,
};
};

namespace DOT_SHADOW {
enum {
    DEFAULT,
    DISABLE,
    DISABLE_PLAYER_ACTORS_ONLY,
};
};

namespace DERGIL {
enum {
    DEFAULT,
    FORCE_OFF,
    FORCE_ON,
};
};

namespace ROOM {
enum {
	DANTES_OFFICE = 0,
	DANTES_OFFICE_FRONT = 1,
	SLUM_66_AVENUE = 2,
	BULLSEYE = 3,
	LOVE_PLANET = 4,
	AVENUE_13TH = 5,
	ICE_GUARDIANS_CHAMBER = 6,
	LOVE_PLANET_2 = 7,
	AVENUE_13TH_AFTER = 8,
	DANTES_OFFICE_FRONT_2 = 9,
	DANTES_OFFICE_FRONT_3 = 10,
	SLUM_66_AVENUE_2 = 11,
	CREDITS = 12,
	CHAMBER_OF_ECHOES = 100,
	ENTRANCEWAY = 101,
	LIVING_STATUE_ROOM = 102,
	SILENCE_STATUARY = 103,
	CHAMBER_OF_SINS = 104,
	CURSED_SKULL_CHAMBER = 105,
	SUN_MOON_CHAMBER = 106,
	ICE_GUARDIANS_CHAMBER_2 = 107,
	ENTRANCEWAY_2 = 108,
	CHAMBER_OF_SINS_2 = 109,
	INCANDESCENT_SPACE = 110,
	GIANTWALKER_CHAMBER = 111,
	ENDLESS_INFERNUM = 112,
	SURGE_OF_FORTUNAS = 113,
	HEAVENRISE_CHAMBER = 114,
	THE_DIVINE_LIBRARY = 115,
	INCANDESCENT_SPACE_2 = 116,
	ENDLESS_INFERNUM_2 = 117,
	SURGE_OF_FORTUNAS_2 = 118,
	HIGH_FLY_ZONE = 119,
	AZURE_GARDEN = 120,
	FIRESTORM_CHAMBER = 121,
	MUTE_GODDESS_CHAMBER = 122,
	CHAMBER_OF_3_TRIALS = 123,
	TRIAL_OF_THE_WARRIOR = 124,
	TRIAL_OF_SKILL = 125,
	TRIAL_OF_WISDOM = 126,
	THE_DARK_CORRIDOR = 127,
	GOD_CUBE_CHAMBER = 128,
	TRI_SEALED_ANTECHAMBER = 129,
	TRIAL_OF_THE_WARRIOR_2 = 130,
	TRIAL_OF_SKILL_2 = 131,
	TRIAL_OF_WISDOM_2 = 132,
	THE_DARK_CORRIDOR_2 = 133,
	PITCH_BLACK_VOID = 134,
	SKULL_SPIRE = 135,
	TRANQUIL_SOULS_ROOM = 136,
	LIFT_ROOM = 137,
	MOONLIGHT_MILE = 138,
	APPARITION_INCARNATE = 139,
	PITCH_BLACK_VOID_2 = 140,
	SKULL_SPIRE_2 = 141,
	PEAK_OF_DARKNESS = 142,
	DARK_PACT_CHAMBER = 143,
	PEAK_OF_DARKNESS_2 = 144,
	ASTRAL_CHAMBER = 145,
	WAKING_SUN_CHAMBER = 146,
	FORBIDDEN_LAND_FRONT = 200,
	THE_ROTATING_BRIDGE = 201,
	PROVISIONS_STOREROOM = 202,
	SUBTERRANEAN_GARDEN = 203,
	SUBGROUND_WATER_VEIN = 204,
	ROUNDED_PATHWAY = 205,
	SUBTERRANEAN_LAKE = 206,
	ROUNDED_PATHWAY_2 = 207,
	PROVISIONS_STOREROOM_2 = 208,
	LIMESTONE_CAVERN = 209,
	SUNKEN_OPERA_HOUSE = 210,
	MARBLE_THROUGHWAY = 211,
	GEARS_OF_MADNESS = 212,
	ALTAR_OF_EVIL_PATHWAY = 213,
	ALTAR_OF_EVIL = 214,
	DEBUG_ROOM = 215,
	TEMPERANCE_WAGON = 216,
	TORTURE_CHAMBER = 217,
	SPIRAL_CORRIDOR = 218,
	DEVILSPROUT_LIFT = 219,
	DEBUG_ROOM_2 = 220,
	SUBTERRAN_GARDEN = 221,
	UPPER_SUBTERRAN_GARDEN = 222,
	DEBUG_ROOM_3 = 223,
	ROUNDED_PATHWAY_3 = 224,
	SUBTERRAN_LAKE = 225,
	TOP_SUBTERRIA_LACK = 226,
	ROUNDED_PATHWAY_4 = 227,
	UNDERGROUND_ARENA = 228,
	EFFERVESCENCE_CORRIDOR = 229,
	SPIRAL_STAIRCASE = 230,
	TOP_OBSIDIAN_PATH = 231,
	OBSIDIAN_PATH = 232,
	VESTIBULE = 233,
	LAIR_OF_JUDGEMENT = 234,
	UNDERWATER_ELEVATOR = 235,
	HELLS_HIGHWAY = 236,
	LAIR_OF_JUDGEMENT_RUINS = 237,
	UNDERGROUND_ARENA_2 = 238,
	TEMPERANCE_WAGON_2 = 239,
	TEMPERANCE_WAGON_3 = 240,
	LUX_LUMINOUS_CORRIDOR = 241,
	LEVIATHANS_STOMACH = 300,
	LEVIATHANS_INTESTINES = 301,
	LEVIATHANS_HEARTCORE = 302,
	LEVIATHANS_INTESTINES_2 = 303,
	LEVIATHANS_INTESTINES_3 = 304,
	LEVIATHANS_RETINA = 305,
	LEVIATHANS_INTESTINES_4 = 306,
	LEVIATHANS_INTESTINES_5 = 307,
	LEVIATHANS_STOMACH_2 = 308,
	LEVIATHANS_INTESTINES_6 = 309,
	LEVIATHANS_HEARTCORE_DEBUG = 310,
	LEVIATHANS_INTESTINES_7 = 311,
	LEVIATHANS_INTESTINES_8 = 312,
	LEVIATHANS_RETINA_2 = 313,
	UNSACRED_HELLGATE = 400,
	DAMNED_CHESS_BOARD = 401,
	ROAD_TO_DESPAIR = 402,
	LOST_SOULS_NIRVANA = 403,
	INFINITY_NIRVANA = 404,
	NIRVANA_OF_ILLUSIONS = 405,
	ROOM_OF_FALLEN_ONES = 406,
	DEBUG_ROOM_4 = 407,
	END_OF_THE_LINE = 408,
	FORBIDDEN_NIRVANA = 409,
	NO_USE = 410,
	UNSACRED_HELLGATE_2 = 411,
	ICE_GUARDIAN_REBORN = 412,
	GIANTWALKER_REBORN = 413,
	FIRESTORM_REBORN = 414,
	LIGHTNING_WITCH_REBORN = 415,
	LIGHTBEAST_REBORN = 416,
	TIMESTEED_REBORN = 417,
	DEATHVOID_REBORN = 418,
	EVIL_GOD_BEAST_REBORN = 419,
	DEMON_ARMY_REBORN = 420,
	FORBIDDEN_NIRVANA_2 = 421,
	DEMON_CLOWN_CHAMBER = 422,
	BLOODY_PALACE_1 = 423,
	BLOODY_PALACE_2 = 424,
	BLOODY_PALACE_3 = 425,
	BLOODY_PALACE_4 = 426,
	BLOODY_PALACE_5 = 427,
	BLOODY_PALACE_6 = 428,
	BLOODY_PALACE_7 = 429,
	BLOODY_PALACE_8 = 430,
	BLOODY_PALACE_9 = 431,
	BLOODY_PALACE_10 = 432,
	BLOODY_PALACE_BOSS_1 = 433,
	BLOODY_PALACE_BOSS_2 = 434,
	BLOODY_PALACE_BOSS_3 = 435,
	BLOODY_PALACE_BOSS_4 = 436,
	BLOODY_PALACE_BOSS_5 = 437,
	BLOODY_PALACE_BOSS_6 = 438,
	BLOODY_PALACE_BOSS_7 = 439,
	BLOODY_PALACE_BOSS_8 = 440,
	BLOODY_PALACE_BOSS_9 = 441,
	BLOODY_PALACE_BOSS_10 = 442,
	BLOODY_PALACE_BOSS_11 = 443,
	BLOODY_PALACE_BOSS_12 = 444,
	BLOODY_PALACE_BOSS_13 = 445,
	BLOODY_PALACE_BOSS_14 = 446,
	BLOODY_PALACE_BOSS_15 = 447,
	DEMON_CLOWN_CHAMBER_2 = 448,
	DEMON_CLOWN_CHAMBER_3 = 449,
	SECRET_MISSION_1 = 600,
	SECRET_MISSION_2 = 601,
	SECRET_MISSION_3 = 602,
	SECRET_MISSION_4 = 603,
	SECRET_MISSION_5 = 604,
	SECRET_MISSION_6 = 605,
	SECRET_MISSION_7 = 606,
	SECRET_MISSION_8 = 607,
	SECRET_MISSION_9 = 608,
	SECRET_MISSION_10 = 609,
	SECRET_MISSION_11 = 610,
	SECRET_MISSION_12 = 611,
	DEBUG_ROOM_5 = 900,
	DEBUG_ROOM_6 = 901,
	DEBUG_ROOM_7 = 902,
	DEBUG_ROOM_8 = 903,
	DEBUG_ROOM_9 = 904,
	DEBUG_ROOM_10 = 905,
	DEBUG_ROOM_11 = 906,
	DEBUG_ROOM_12 = 907,
	DEBUG_ROOM_13 = 908,
	DEBUG_ROOM_14 = 909,
	DEBUG_ROOM_15 = 910,
	ROOMS_COUNT   = 171
};
}

enum {
    ARCHIVE_METADATA_SIZE   = 8,
    HEAD_METADATA_SIZE      = 32,
    PROG_SECT_ITEM_SIZE     = 14,
    PROG_SECT_METADATA_SIZE = 8,
    PROG_METADATA_SIZE      = 16,
    SMPL_ITEM_SIZE          = 12,
    SMPL_METADATA_SIZE      = 16,
    VAGI_ITEM_SIZE          = 16,
    VAGI_METADATA_SIZE      = 16,
    WAVE_METADATA_SIZE      = 64,
    SOUND_DATA_SIZE         = 24,
    DBST_METADATA_SIZE      = 16,
    DBST_ITEM_SIZE          = 32,
};

enum {
    PROG_SECT_REBELLION,
    PROG_SECT_CERBERUS,
    PROG_SECT_AGNI_RUDRA,
    PROG_SECT_NEVAN,
    PROG_SECT_BEOWULF_DANTE,
    PROG_SECT_EBONY_IVORY,
    PROG_SECT_SHOTGUN,
    PROG_SECT_ARTEMIS,
    PROG_SECT_SPIRAL,
    PROG_SECT_KALINA_ANN,
    PROG_SECT_YAMATO = 11,
    PROG_SECT_BEOWULF_VERGIL,
    PROG_SECT_YAMATO_FORCE_EDGE,
    PROG_SECT_SWORDMASTER = 100,
    PROG_SECT_GUNSLINGER,
    PROG_SECT_TRICKSTER,
    PROG_SECT_ROYALGUARD,
    PROG_SECT_QUICKSILVER,
    PROG_SECT_DOPPELGANGER,
    PROG_SECT_BATTLE_OF_BROTHERS,
    PROG_SECT_DARK_SLAYER,
    MAX_PROG_SECT,
};

enum {
    HELPER_COMMON_DANTE_DEFAULT,
    HELPER_COMMON_DANTE_NO_COAT,
    HELPER_COMMON_VERGIL_DEFAULT,
    HELPER_COMMON_VERGIL_NERO_ANGELO,
    HELPER_STYLE_WEAPON_DANTE,
    HELPER_STYLE_WEAPON_VERGIL_DEFAULT,
    HELPER_STYLE_WEAPON_VERGIL_NERO_ANGELO,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_GIGAPEDE,
    HELPER_ENEMY_CERBERUS,
    HELPER_ENEMY_AGNI_RUDRA,
    HELPER_ENEMY_NEVAN,
    HELPER_ENEMY_GERYON,
    HELPER_ENEMY_BEOWULF,
    HELPER_ENEMY_DOPPELGANGER,
    HELPER_ENEMY_ARKHAM,
    HELPER_ENEMY_LADY,
    HELPER_ENEMY_VERGIL,
    HELPER_ENEMY_JESTER,
    HELPER_COUNT,
};

#pragma endregion

#pragma region Cache

// $CacheFileStart

enum {
    pl000,
    pl011,
    pl013,
    pl015,
    pl016,
    pl018,
    pl005,
    pl006,
    pl007,
    pl008,
    pl009,
    pl017,
    pl000_00_0,
    pl000_00_1,
    pl000_00_2,
    pl000_00_3,
    pl000_00_4,
    pl000_00_5,
    pl000_00_6,
    pl000_00_7,
    pl000_00_8,
    pl000_00_9,
    pl000_00_10,
    pl000_00_11,
    pl000_00_12,
    pl000_00_13,
    pl000_00_14,
    pl000_00_15,
    pl000_00_16,
    pl000_00_17,
    pl000_00_18,
    pl000_00_19,
    pl000_00_20,
    pl000_00_21,
    pl000_00_22,
    pl000_00_23,
    pl000_00_24,
    pl000_00_25,
    pl000_00_26,
    plwp_sword,
    plwp_sword2,
    plwp_sword3,
    plwp_nunchaku,
    plwp_2sword,
    plwp_guitar,
    plwp_fight,
    plwp_gun,
    plwp_shotgun,
    plwp_laser,
    plwp_rifle,
    plwp_ladygun,
    snd_com0,
    snd_com0a,
    snd_sty02,
    snd_sty03,
    snd_sty04,
    snd_sty05,
    snd_sty06,
    snd_wp00b,
    snd_wp01b,
    snd_wp02b,
    snd_wp03b,
    snd_wp04b,
    snd_wp05b,
    snd_wp06b,
    snd_wp07b,
    snd_wp08b,
    snd_wp09b,
    pl001,
    pl001_00_0,
    pl001_00_1,
    pl001_00_2,
    pl001_00_31,
    plwp_vergilsword,
    pl002,
    pl002_00_0,
    pl002_00_1,
    pl002_00_2,
    pl021,
    pl023,
    pl026,
    pl010,
    pl014,
    pl025,
    pl021_00_0,
    pl021_00_1,
    pl021_00_2,
    pl021_00_3,
    pl021_00_4,
    pl021_00_5,
    pl021_00_6,
    pl021_00_7,
    pl021_00_8,
    pl021_00_9,
    plwp_newvergilsword,
    plwp_newvergilfight,
    plwp_forceedge,
    plwp_nerosword,
    snd_com3,
    snd_com3a,
    snd_sty07,
    snd_sty08,
    snd_wp11a,
    snd_wp11b,
    snd_wp12a,
    snd_wp12b,
    snd_wp13a,
    snd_wp13b,
    em000,
    em006,
    em007,
    em008,
    em010,
    em011,
    em012,
    em013,
    em014,
    em016,
    em017,
    em021,
    em023,
    em025,
    em026,
    em027,
    em028,
    em029,
    em030,
    em031,
    em032,
    em034,
    em035,
    em037,
    snd_em00a,
    snd_em00b,
    snd_em06,
    snd_em07,
    snd_em08,
    snd_em10,
    snd_em11,
    snd_em12,
    snd_em13,
    snd_em14,
    snd_em16,
    snd_em17,
    snd_em21,
    snd_em23,
    snd_em25,
    snd_em26,
    snd_em27,
    snd_em28,
    snd_em29,
    snd_em30,
    snd_em31,
    snd_em32,
    snd_em34,
    snd_em35,
    snd_em37,
    snd_emsr,
    snd_stay,
    id100,
    id100V,
    CACHE_FILE_COUNT,
};

struct CacheFileHelper {
    const char* filename;
    const char* typeName;
};

constexpr CacheFileHelper cacheFileHelpers[CACHE_FILE_COUNT] = {
    {"pl000.pac", "obj\\pl000.pac"},
    {"pl011.pac", "obj\\pl011.pac"},
    {"pl013.pac", "obj\\pl013.pac"},
    {"pl015.pac", "obj\\pl015.pac"},
    {"pl016.pac", "obj\\pl016.pac"},
    {"pl018.pac", "obj\\pl018.pac"},
    {"pl005.pac", "obj\\pl005.pac"},
    {"pl006.pac", "obj\\pl006.pac"},
    {"pl007.pac", "obj\\pl007.pac"},
    {"pl008.pac", "obj\\pl008.pac"},
    {"pl009.pac", "obj\\pl009.pac"},
    {"pl017.pac", "obj\\pl017.pac"},
    {"pl000_00_0.pac", "motion\\pl000\\pl000_00_0.pac"},
    {"pl000_00_1.pac", "motion\\pl000\\pl000_00_1.pac"},
    {"pl000_00_2.pac", "motion\\pl000\\pl000_00_2.pac"},
    {"pl000_00_3.pac", "motion\\pl000\\pl000_00_3.pac"},
    {"pl000_00_4.pac", "motion\\pl000\\pl000_00_4.pac"},
    {"pl000_00_5.pac", "motion\\pl000\\pl000_00_5.pac"},
    {"pl000_00_6.pac", "motion\\pl000\\pl000_00_6.pac"},
    {"pl000_00_7.pac", "motion\\pl000\\pl000_00_7.pac"},
    {"pl000_00_8.pac", "motion\\pl000\\pl000_00_8.pac"},
    {"pl000_00_9.pac", "motion\\pl000\\pl000_00_9.pac"},
    {"pl000_00_10.pac", "motion\\pl000\\pl000_00_10.pac"},
    {"pl000_00_11.pac", "motion\\pl000\\pl000_00_11.pac"},
    {"pl000_00_12.pac", "motion\\pl000\\pl000_00_12.pac"},
    {"pl000_00_13.pac", "motion\\pl000\\pl000_00_13.pac"},
    {"pl000_00_14.pac", "motion\\pl000\\pl000_00_14.pac"},
    {"pl000_00_15.pac", "motion\\pl000\\pl000_00_15.pac"},
    {"pl000_00_16.pac", "motion\\pl000\\pl000_00_16.pac"},
    {"pl000_00_17.pac", "motion\\pl000\\pl000_00_17.pac"},
    {"pl000_00_18.pac", "motion\\pl000\\pl000_00_18.pac"},
    {"pl000_00_19.pac", "motion\\pl000\\pl000_00_19.pac"},
    {"pl000_00_20.pac", "motion\\pl000\\pl000_00_20.pac"},
    {"pl000_00_21.pac", "motion\\pl000\\pl000_00_21.pac"},
    {"pl000_00_22.pac", "motion\\pl000\\pl000_00_22.pac"},
    {"pl000_00_23.pac", "motion\\pl000\\pl000_00_23.pac"},
    {"pl000_00_24.pac", "motion\\pl000\\pl000_00_24.pac"},
    {"pl000_00_25.pac", "motion\\pl000\\pl000_00_25.pac"},
    {"pl000_00_26.pac", "motion\\pl000\\pl000_00_26.pac"},
    {"plwp_sword.pac", "obj\\plwp_sword.pac"},
    {"plwp_sword2.pac", "obj\\plwp_sword2.pac"},
    {"plwp_sword3.pac", "obj\\plwp_sword3.pac"},
    {"plwp_nunchaku.pac", "obj\\plwp_nunchaku.pac"},
    {"plwp_2sword.pac", "obj\\plwp_2sword.pac"},
    {"plwp_guitar.pac", "obj\\plwp_guitar.pac"},
    {"plwp_fight.pac", "obj\\plwp_fight.pac"},
    {"plwp_gun.pac", "obj\\plwp_gun.pac"},
    {"plwp_shotgun.pac", "obj\\plwp_shotgun.pac"},
    {"plwp_laser.pac", "obj\\plwp_laser.pac"},
    {"plwp_rifle.pac", "obj\\plwp_rifle.pac"},
    {"plwp_ladygun.pac", "obj\\plwp_ladygun.pac"},
    {"snd_com0.pac", "se\\snd_com0.pac"},
    {"snd_com0a.pac", "se\\snd_com0a.pac"},
    {"snd_sty02.pac", "se\\snd_sty02.pac"},
    {"snd_sty03.pac", "se\\snd_sty03.pac"},
    {"snd_sty04.pac", "se\\snd_sty04.pac"},
    {"snd_sty05.pac", "se\\snd_sty05.pac"},
    {"snd_sty06.pac", "se\\snd_sty06.pac"},
    {"snd_wp00b.pac", "se\\snd_wp00b.pac"},
    {"snd_wp01b.pac", "se\\snd_wp01b.pac"},
    {"snd_wp02b.pac", "se\\snd_wp02b.pac"},
    {"snd_wp03b.pac", "se\\snd_wp03b.pac"},
    {"snd_wp04b.pac", "se\\snd_wp04b.pac"},
    {"snd_wp05b.pac", "se\\snd_wp05b.pac"},
    {"snd_wp06b.pac", "se\\snd_wp06b.pac"},
    {"snd_wp07b.pac", "se\\snd_wp07b.pac"},
    {"snd_wp08b.pac", "se\\snd_wp08b.pac"},
    {"snd_wp09b.pac", "se\\snd_wp09b.pac"},
    {"pl001.pac", "obj\\pl001.pac"},
    {"pl001_00_0.pac", "motion\\pl001\\pl001_00_0.pac"},
    {"pl001_00_1.pac", "motion\\pl001\\pl001_00_1.pac"},
    {"pl001_00_2.pac", "motion\\pl001\\pl001_00_2.pac"},
    {"pl001_00_31.pac", "motion\\pl001\\pl001_00_31.pac"},
    {"plwp_vergilsword.pac", "obj\\plwp_vergilsword.pac"},
    {"pl002.pac", "obj\\pl002.pac"},
    {"pl002_00_0.pac", "motion\\pl002\\pl002_00_0.pac"},
    {"pl002_00_1.pac", "motion\\pl002\\pl002_00_1.pac"},
    {"pl002_00_2.pac", "motion\\pl002\\pl002_00_2.pac"},
    {"pl021.pac", "obj\\pl021.pac"},
    {"pl023.pac", "obj\\pl023.pac"},
    {"pl026.pac", "obj\\pl026.pac"},
    {"pl010.pac", "obj\\pl010.pac"},
    {"pl014.pac", "obj\\pl014.pac"},
    {"pl025.pac", "obj\\pl025.pac"},
    {"pl021_00_0.pac", "motion\\pl021\\pl021_00_0.pac"},
    {"pl021_00_1.pac", "motion\\pl021\\pl021_00_1.pac"},
    {"pl021_00_2.pac", "motion\\pl021\\pl021_00_2.pac"},
    {"pl021_00_3.pac", "motion\\pl021\\pl021_00_3.pac"},
    {"pl021_00_4.pac", "motion\\pl021\\pl021_00_4.pac"},
    {"pl021_00_5.pac", "motion\\pl021\\pl021_00_5.pac"},
    {"pl021_00_6.pac", "motion\\pl021\\pl021_00_6.pac"},
    {"pl021_00_7.pac", "motion\\pl021\\pl021_00_7.pac"},
    {"pl021_00_8.pac", "motion\\pl021\\pl021_00_8.pac"},
    {"pl021_00_9.pac", "motion\\pl021\\pl021_00_9.pac"},
    {"plwp_newvergilsword.pac", "obj\\plwp_newvergilsword.pac"},
    {"plwp_newvergilfight.pac", "obj\\plwp_newvergilfight.pac"},
    {"plwp_forceedge.pac", "obj\\plwp_forceedge.pac"},
    {"plwp_nerosword.pac", "obj\\plwp_nerosword.pac"},
    {"snd_com3.pac", "se\\snd_com3.pac"},
    {"snd_com3a.pac", "se\\snd_com3a.pac"},
    {"snd_sty07.pac", "se\\snd_sty07.pac"},
    {"snd_sty08.pac", "se\\snd_sty08.pac"},
    {"snd_wp11a.pac", "se\\snd_wp11a.pac"},
    {"snd_wp11b.pac", "se\\snd_wp11b.pac"},
    {"snd_wp12a.pac", "se\\snd_wp12a.pac"},
    {"snd_wp12b.pac", "se\\snd_wp12b.pac"},
    {"snd_wp13a.pac", "se\\snd_wp13a.pac"},
    {"snd_wp13b.pac", "se\\snd_wp13b.pac"},
    {"em000.pac", "obj\\em000.pac"},
    {"em006.pac", "obj\\em006.pac"},
    {"em007.pac", "obj\\em007.pac"},
    {"em008.pac", "obj\\em008.pac"},
    {"em010.pac", "obj\\em010.pac"},
    {"em011.pac", "obj\\em011.pac"},
    {"em012.pac", "obj\\em012.pac"},
    {"em013.pac", "obj\\em013.pac"},
    {"em014.pac", "obj\\em014.pac"},
    {"em016.pac", "obj\\em016.pac"},
    {"em017.pac", "obj\\em017.pac"},
    {"em021.pac", "obj\\em021.pac"},
    {"em023.pac", "obj\\em023.pac"},
    {"em025.pac", "obj\\em025.pac"},
    {"em026.pac", "obj\\em026.pac"},
    {"em027.pac", "obj\\em027.pac"},
    {"em028.pac", "obj\\em028.pac"},
    {"em029.pac", "obj\\em029.pac"},
    {"em030.pac", "obj\\em030.pac"},
    {"em031.pac", "obj\\em031.pac"},
    {"em032.pac", "obj\\em032.pac"},
    {"em034.pac", "obj\\em034.pac"},
    {"em035.pac", "obj\\em035.pac"},
    {"em037.pac", "obj\\em037.pac"},
    {"snd_em00a.pac", "se\\snd_em00a.pac"},
    {"snd_em00b.pac", "se\\snd_em00b.pac"},
    {"snd_em06.pac", "se\\snd_em06.pac"},
    {"snd_em07.pac", "se\\snd_em07.pac"},
    {"snd_em08.pac", "se\\snd_em08.pac"},
    {"snd_em10.pac", "se\\snd_em10.pac"},
    {"snd_em11.pac", "se\\snd_em11.pac"},
    {"snd_em12.pac", "se\\snd_em12.pac"},
    {"snd_em13.pac", "se\\snd_em13.pac"},
    {"snd_em14.pac", "se\\snd_em14.pac"},
    {"snd_em16.pac", "se\\snd_em16.pac"},
    {"snd_em17.pac", "se\\snd_em17.pac"},
    {"snd_em21.pac", "se\\snd_em21.pac"},
    {"snd_em23.pac", "se\\snd_em23.pac"},
    {"snd_em25.pac", "se\\snd_em25.pac"},
    {"snd_em26.pac", "se\\snd_em26.pac"},
    {"snd_em27.pac", "se\\snd_em27.pac"},
    {"snd_em28.pac", "se\\snd_em28.pac"},
    {"snd_em29.pac", "se\\snd_em29.pac"},
    {"snd_em30.pac", "se\\snd_em30.pac"},
    {"snd_em31.pac", "se\\snd_em31.pac"},
    {"snd_em32.pac", "se\\snd_em32.pac"},
    {"snd_em34.pac", "se\\snd_em34.pac"},
    {"snd_em35.pac", "se\\snd_em35.pac"},
    {"snd_em37.pac", "se\\snd_em37.pac"},
    {"snd_emsr.pac", "se\\snd_emsr.pac"},
    {"snd_stay.pac", "se\\snd_stay.pac"},
    {"id100.pac", "id\\id100\\id100.pac"},
    {"id100V.pac", "id\\id100\\id100V.pac"},
};

static_assert(countof(cacheFileHelpers) == CACHE_FILE_COUNT);

// $CacheFileEnd

#pragma endregion

#pragma region constexpr

constexpr uint16 costumeFileIdsDante[COSTUME::MAX_DANTE] = {
    pl000,
    pl011,
    pl013,
    pl015,
    pl016,
    pl018,
    pl013,
    pl018,
};

constexpr uint16 costumeFileIdsBob[COSTUME::MAX_BOB] = {
    pl001,
};

constexpr uint16 costumeFileIdsLady[COSTUME::MAX_LADY] = {
    pl002,
    em034,
};

constexpr uint16 costumeFileIdsVergil[COSTUME::MAX_VERGIL] = {
    pl021,
    pl023,
    pl021,
    pl026,
    pl026,
};

constexpr uint8 costumeCounts[CHARACTER::MAX] = {
    COSTUME::MAX_DANTE,
    COSTUME::MAX_BOB,
    COSTUME::MAX_LADY,
    COSTUME::MAX_VERGIL,
};

constexpr uint16 devilFileIdsDante[6] = {
    pl005,
    pl006,
    pl007,
    pl008,
    pl009,
    pl017,
};

constexpr uint8 weaponDevilIds[WEAPON::MAX] = {
    DEVIL::REBELLION,
    DEVIL::CERBERUS,
    DEVIL::AGNI_RUDRA,
    DEVIL::NEVAN,
    DEVIL::BEOWULF,
    0,
    0,
    0,
    0,
    0,
    0,
    DEVIL::YAMATO,
    DEVIL::BEOWULF,
    DEVIL::YAMATO,
    DEVIL::YAMATO,
};

constexpr uint32 hudTopOffs[] = {
    0x700,
    0xE80,
    0x1600,
    0x1D80,
    0x2500,
    0x2C80,
    0x3400,
    0x3B80,
    0x4300,
    0x4A80,
    0x5200,
    0x5980,
    0x6100,
};

constexpr uint32 hudBottomOffs[] = {
    0x880,
    0x1000,
    0x1780,
    0x1F00,
};

constexpr uint32 itemVitalStarSmallPrices[] = {
    500,
    750,
    1200,
    1800,
    2500,
    3500,
    5000,
};

constexpr uint32 itemVitalStarLargePrices[] = {
    2000,
    3000,
    4500,
    6000,
    7500,
    10000,
};

constexpr uint32 itemDevilStarPrices[] = {
    3000,
    5000,
    7000,
    9000,
    10000,
};

constexpr uint32 itemHolyWaterPrices[] = {
    10000,
    15000,
    20000,
    25000,
    30000,
};

constexpr uint32 itemBlueOrbPrices[] = {
    5000,
    10000,
    15000,
    20000,
    30000,
    50000,
};

constexpr uint32 itemPurpleOrbPrices[] = {
    3000,
    5000,
    7000,
    9000,
    10000,
    20000,
    30000,
};

constexpr uint32 itemGoldOrbPrices[] = {
    10000,
    15000,
    20000,
};

constexpr uint32 itemYellowOrbPrices[] = {
    1000,
    1500,
    2000,
    3000,
};

constexpr uint32 motionArchivesOffs[] = {
    0x698,  // Damned Chessmen Pawn, Knight, Bishop, Rook, Queen, King
    0x6C0,  // Blood-Goyle
    0x6D0,  // Pride, Gluttony, Lust, Sloth, Wrath, Envy
    0x780,  // Soul Eater
    0xA58,  // Enigma
    0xA98,  // Doppelganger
    0x1130, // Geryon
    0x1158, // Agni & Rudra
    0x14F0, // The Fallen
    0x1B48, // Arkham
    0x2160, // Dullahan
    0x23C0, // Jester
    0x2C20, // Arachne
    0x2E18, // Nevan
    0x38A0, // Player Actor
    0x3940, // Greed, Abyss
    0x4020, // Beowulf
    0x49C8, // Hell Vanguard
    0x5410, // Lady
    0xD7B0, // Cerberus
    0xE7D0, // Vergil
};

// $ItemStart
// For use within sessionData.itemcounts and missionData.itemCounts
namespace ITEM {
enum {
    UNKNOWN_0,
    UNKNOWN_1,
    UNKNOWN_2,
    UNKNOWN_3,
    UNKNOWN_4,
    GOLD_ORB,
    YELLOW_ORB,
    BLUE_ORB,
    PURPLE_ORB,
    BLUE_ORB_FRAGMENT,
    UNKNOWN_5,
    UNKNOWN_6,
    UNKNOWN_7,
    UNKNOWN_8,
    UNKNOWN_9,
    UNKNOWN_10,
    VITAL_STAR_LARGE,
    VITAL_STAR_SMALL,
    DEVIL_STAR,
    HOLY_WATER,
    UNKNOWN_11,
    UNKNOWN_12,
    UNKNOWN_13,
    CERBERUS,
    AGNI_RUDRA,
    UNKNOWN_16,
    NEVAN,
    BEOWULF,
    UNKNOWN_19,
    SHOTGUN,
    ARTEMIS,
    SPIRAL,
    UNKNOWN_23,
    KALINA_ANN,
    UNKNOWN_25,
    UNKNOWN_26,
    ASTRONOMICAL_BOARD,
    VAJURA,
    UNKNOWN_27,
    SOUL_OF_STEEL,
    ESSENCE_OF_FIGHTING,
    ESSENCE_OF_TECHNIQUE,
    ESSENCE_OF_INTELLIGENCE,
    ORIHALCON_FRAGMENT,
    SIRENS_SHRIEK,
    CRYSTAL_SKULL,
    IGNIS_FATUUS,
    AMBROSIA,
    STONE_MASK,
    NEO_GENERATOR,
    HAYWIRE_NEO_GENERATOR,
    ORIHALCON,
    ORIHALCON_FRAGMENT_RIGHT,
    ORIHALCON_FRAGMENT_BOTTOM,
    ORIHALCON_FRAGMENT_LEFT,
    GOLDEN_SUN,
    ONYX_MOONSHARD,
    SAMSARA,
    UNKNOWN_28,
    UNKNOWN_29,
    UNKNOWN_30,
    UNKNOWN_31,
    COUNT,
};
};

extern const char* itemNames[ITEM::COUNT];

// For use within sessionData.weaponStyleUnlocks
namespace WEAPONANDSTYLEUNLOCKS {
	enum {
		UNKNOWN_0,
        CERBERUS,
        AGNI_RUDRA,
        UNKNOWN_1, 
        NEVAN,
        BEOWULF,
        UNKNOWN_2,
        SHOTGUN,
        ARTEMIS,
        SPIRAL,
        UNKNOWN_3,
        KALINA_ANN,
        QUICKSILVER,
        DOPPELGANGER,
		COUNT,
	};
};

extern const char* weaponStyleUnlocksNames[WEAPONANDSTYLEUNLOCKS::COUNT];

namespace BUY {
enum {
    VITAL_STAR_SMALL,
    VITAL_STAR_LARGE,
    DEVIL_STAR,
    HOLY_WATER,
    BLUE_ORB,
    PURPLE_ORB,
    GOLD_ORB,
    YELLOW_ORB,
    COUNT,
};
};

extern const char* buyNames[BUY::COUNT];

// $ItemEnd

// $EnemyStart

namespace ENEMY {
enum {
    PRIDE_1,
    PRIDE_2,
    PRIDE_3,
    PRIDE_4,
    GLUTTONY_1,
    GLUTTONY_2,
    GLUTTONY_3,
    GLUTTONY_4,
    LUST_1,
    LUST_2,
    LUST_3,
    LUST_4,
    SLOTH_1,
    SLOTH_2,
    SLOTH_3,
    SLOTH_4,
    WRATH_1,
    WRATH_2,
    WRATH_3,
    WRATH_4,
    GREED_1,
    GREED_2,
    GREED_3,
    GREED_4,
    ABYSS,
    ENVY,
    HELL_VANGUARD,
    UNKNOWN_0,
    ARACHNE,
    THE_FALLEN,
    DULLAHAN,
    ENIGMA,
    BLOOD_GOYLE,
    UNKNOWN_1,
    SOUL_EATER,
    DAMNED_CHESSMEN_PAWN,
    DAMNED_CHESSMEN_KNIGHT,
    DAMNED_CHESSMEN_BISHOP,
    DAMNED_CHESSMEN_ROOK,
    DAMNED_CHESSMEN_QUEEN,
    DAMNED_CHESSMEN_KING,
    GIGAPEDE,
    UNKNOWN_2,
    CERBERUS,
    AGNI_RUDRA_ALL,
    AGNI_RUDRA,
    AGNI_RUDRA_BLACK,
    AGNI_RUDRA_RED,
    AGNI_RUDRA_BLUE,
    NEVAN,
    GERYON,
    BEOWULF,
    DOPPELGANGER,
    ARKHAM,
    ARKHAM_LEECHES,
    LADY,
    UNKNOWN_4,
    UNKNOWN_5,
    VERGIL,
    UNKNOWN_6,
    LEVIATHAN_HEART,
    LEVIATHAN_OTHER,
    JESTER,
    UNKNOWN_7,
    COUNT,
};
};

extern const char* enemyNames[ENEMY::COUNT];

constexpr uint8 enemyHelperIndices[] = {
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    0,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    0,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_GIGAPEDE,
    0,
    HELPER_ENEMY_CERBERUS,
    HELPER_ENEMY_AGNI_RUDRA,
    HELPER_ENEMY_AGNI_RUDRA,
    HELPER_ENEMY_AGNI_RUDRA,
    HELPER_ENEMY_AGNI_RUDRA,
    HELPER_ENEMY_AGNI_RUDRA,
    HELPER_ENEMY_NEVAN,
    HELPER_ENEMY_GERYON,
    HELPER_ENEMY_BEOWULF,
    HELPER_ENEMY_DOPPELGANGER,
    HELPER_ENEMY_ARKHAM,
    0,
    HELPER_ENEMY_LADY,
    0,
    0,
    HELPER_ENEMY_VERGIL,
    0,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_LESSER_ENEMIES,
    HELPER_ENEMY_JESTER,
    0,
};

// $EnemyEnd

static_assert(ENEMY::LADY == 55);
static_assert(ENEMY::VERGIL == 58);
static_assert(ENEMY::COUNT == 64);
static_assert(countof(enemyHelperIndices) == ENEMY::COUNT);

enum {
    ENEMY_FILE_DATA_EM000,
    ENEMY_FILE_DATA_EM006,
    ENEMY_FILE_DATA_EM007,
    ENEMY_FILE_DATA_EM008,
    ENEMY_FILE_DATA_EM010,
    ENEMY_FILE_DATA_EM011,
    ENEMY_FILE_DATA_EM012,
    ENEMY_FILE_DATA_EM013,
    ENEMY_FILE_DATA_EM014,
    ENEMY_FILE_DATA_EM016,
    ENEMY_FILE_DATA_EM017,
    ENEMY_FILE_DATA_EM021,
    ENEMY_FILE_DATA_EM023,
    ENEMY_FILE_DATA_EM025,
    ENEMY_FILE_DATA_EM026,
    ENEMY_FILE_DATA_EM027,
    ENEMY_FILE_DATA_EM028,
    ENEMY_FILE_DATA_EM029,
    ENEMY_FILE_DATA_EM030,
    ENEMY_FILE_DATA_EM031,
    ENEMY_FILE_DATA_EM032,
    ENEMY_FILE_DATA_EM034,
    ENEMY_FILE_DATA_EM035,
    ENEMY_FILE_DATA_EM037,
    ENEMY_FILE_DATA_COUNT,
};

enum {
    ENEMY_FILE_SET_EM000,
    ENEMY_FILE_SET_EM006,
    ENEMY_FILE_SET_EM007,
    ENEMY_FILE_SET_EM008,
    ENEMY_FILE_SET_EM000_2,
    ENEMY_FILE_SET_EM010,
    ENEMY_FILE_SET_EM011,
    ENEMY_FILE_SET_EM012,
    ENEMY_FILE_SET_EM013,
    ENEMY_FILE_SET_EM014,
    ENEMY_FILE_SET_EM000_3,
    ENEMY_FILE_SET_EM016,
    ENEMY_FILE_SET_EM017,
    ENEMY_FILE_SET_EM021,
    ENEMY_FILE_SET_EM023,
    ENEMY_FILE_SET_EM000_4,
    ENEMY_FILE_SET_EM025,
    ENEMY_FILE_SET_EM026,
    ENEMY_FILE_SET_EM026_2,
    ENEMY_FILE_SET_EM028,
    ENEMY_FILE_SET_EM029,
    ENEMY_FILE_SET_EM030,
    ENEMY_FILE_SET_EM031,
    ENEMY_FILE_SET_EM032,
    ENEMY_FILE_SET_EM032_2,
    ENEMY_FILE_SET_EM034,
    ENEMY_FILE_SET_EM035,
    ENEMY_FILE_SET_EM037,
    ENEMY_FILE_SET_EM000_5,
    ENEMY_FILE_SET_EM000_6,
    ENEMY_FILE_SET_COUNT,
};

#pragma endregion

#pragma region Structs

#define _(size)                                     \
    struct {                                        \
        byte8 Prep_Merge(padding_, __LINE__)[size]; \
    }

#pragma pack(push, 1)

IntroduceSizeStruct(32);
IntroduceSizeStruct(112);
IntroduceSizeStruct(192);
IntroduceSizeStruct(240);
IntroduceSizeStruct(288);
IntroduceSizeStruct(768);


// $BloodyPalaceDataStart

struct BloodyPalaceData {
    _(2084);
    uint16 level;     // 0x824
    uint16 lastLevel; // 0x826
};

static_assert(offsetof(BloodyPalaceData, level) == 0x824);
static_assert(offsetof(BloodyPalaceData, lastLevel) == 0x826);

static_assert(sizeof(BloodyPalaceData) == 2088);

// $BloodyPalaceDataEnd

// $EventDataStart

constexpr size_t ROOMS_COUNT = 189;
extern const char* roomNames[ROOMS_COUNT];
extern const uint16 roomsMap[ROOMS_COUNT];

struct EventData {
    _(24);
    uint32 room;       // 0x18
    uint32 position;   // 0x1C
    uint32 event;      // 0x20
    uint32 subevent;   // 0x24
    uint32 screen;     // 0x28
    uint32 nextScreen; // 0x2C
};

static_assert(offsetof(EventData, room) == 0x18);
static_assert(offsetof(EventData, position) == 0x1C);
static_assert(offsetof(EventData, event) == 0x20);
static_assert(offsetof(EventData, subevent) == 0x24);
static_assert(offsetof(EventData, screen) == 0x28);
static_assert(offsetof(EventData, nextScreen) == 0x2C);

static_assert(sizeof(EventData) == 48);

// $EventDataEnd

// $NextEventDataStart

struct NextEventData {
    _(356);
    uint16 room;     // 0x164
    uint16 position; // 0x166
};

static_assert(offsetof(NextEventData, room) == 0x164);
static_assert(offsetof(NextEventData, position) == 0x166);

static_assert(sizeof(NextEventData) == 360);

// $NextEventDataEnd

// @Todo: Update names.
// $SessionDataStart

struct SessionData {
    uint32 mission; // 0
    _(8);
    uint32 mode;     // 0xC
    bool oneHitKill; // 0x10
    _(1);
    bool enableTutorial; // 0x12
    bool useGoldOrb;     // 0x13
    uint8 character;     // 0x14
    _(7);
    bool bloodyPalace; // 0x1C
    _(15);
    uint32 redOrbs;       // 0x2C
    uint8 itemCounts[20]; // 0x30
    _(2);
    bool weaponAndStyleUnlocks[14]; // 0x46 // For weapons and quicksilverStyle/doppelganger
    _(48);
    uint8 weapons[8]; // 0x84
    _(20);
    uint32 rangedWeaponLevels[5]; // 0xA0
    _(20);
    uint32 meleeWeaponIndex;  // 0xC8
    uint32 rangedWeaponIndex; // 0xCC
    uint8 costume;            // 0xD0
    bool unlockDevilTrigger;  // 0xD1
    _(2);
    float hitPoints;         // 0xD4
    float magicPoints;       // 0xD8
    uint32 style;            // 0xDC
    uint32 styleLevels[6];   // 0xE0
    float styleExpPoints[6]; // 0xF8
    byte32 expertise[8];     // 0x110
};

static_assert(offsetof(SessionData, mission) == 0);
static_assert(offsetof(SessionData, mode) == 0xC);
static_assert(offsetof(SessionData, oneHitKill) == 0x10);
static_assert(offsetof(SessionData, enableTutorial) == 0x12);
static_assert(offsetof(SessionData, useGoldOrb) == 0x13);
static_assert(offsetof(SessionData, character) == 0x14);
static_assert(offsetof(SessionData, bloodyPalace) == 0x1C);
static_assert(offsetof(SessionData, redOrbs) == 0x2C);
static_assert(offsetof(SessionData, itemCounts) == 0x30);
static_assert(offsetof(SessionData, weaponAndStyleUnlocks) == 0x46);
static_assert(offsetof(SessionData, weapons) == 0x84);
static_assert(offsetof(SessionData, rangedWeaponLevels) == 0xA0);
static_assert(offsetof(SessionData, meleeWeaponIndex) == 0xC8);
static_assert(offsetof(SessionData, rangedWeaponIndex) == 0xCC);
static_assert(offsetof(SessionData, costume) == 0xD0);
static_assert(offsetof(SessionData, unlockDevilTrigger) == 0xD1);
static_assert(offsetof(SessionData, hitPoints) == 0xD4);
static_assert(offsetof(SessionData, magicPoints) == 0xD8);
static_assert(offsetof(SessionData, style) == 0xDC);
static_assert(offsetof(SessionData, styleLevels) == 0xE0);
static_assert(offsetof(SessionData, styleExpPoints) == 0xF8);
static_assert(offsetof(SessionData, expertise) == 0x110);

static_assert(sizeof(SessionData) == 304);

// $SessionDataEnd

// $MissionDataStart

struct MissionData {
    _(56);
    uint32 redOrbs;       // 0x38
    uint8 itemCounts[62]; // 0x3C
    uint8 buyCounts[8];   // 0x7A
    _(38);
    uint32 frameCount;    // 0xA8
    uint32 damage;        // 0xAC
    uint32 orbsCollected; // 0xB0
    uint32 itemsUsed;     // 0xB4
    uint32 killCount;     // 0xB8
    _(4);
};

static_assert(offsetof(MissionData, redOrbs) == 0x38);
static_assert(offsetof(MissionData, itemCounts) == 0x3C);
static_assert(offsetof(MissionData, buyCounts) == 0x7A);
static_assert(offsetof(MissionData, frameCount) == 0xA8);
static_assert(offsetof(MissionData, damage) == 0xAC);
static_assert(offsetof(MissionData, orbsCollected) == 0xB0);
static_assert(offsetof(MissionData, itemsUsed) == 0xB4);
static_assert(offsetof(MissionData, killCount) == 0xB8);

static_assert(sizeof(MissionData) == 192);

// $MissionDataEnd

// $QueuedMissionActorDataStart

struct QueuedMissionActorData {
    uint8 weapons[5]; // 0
    _(75);
    float hitPoints;         // 0x50
    float magicPoints;       // 0x54
    uint32 style;            // 0x58
    uint32 styleLevels[6];   // 0x5C
    float styleExpPoints[6]; // 0x74
    byte32 expertise[8];     // 0x8C
};

static_assert(offsetof(QueuedMissionActorData, weapons) == 0);
static_assert(offsetof(QueuedMissionActorData, hitPoints) == 0x50);
static_assert(offsetof(QueuedMissionActorData, magicPoints) == 0x54);
static_assert(offsetof(QueuedMissionActorData, style) == 0x58);
static_assert(offsetof(QueuedMissionActorData, styleLevels) == 0x5C);
static_assert(offsetof(QueuedMissionActorData, styleExpPoints) == 0x74);
static_assert(offsetof(QueuedMissionActorData, expertise) == 0x8C);

static_assert(sizeof(QueuedMissionActorData) == 172);

// $QueuedMissionActorDataEnd

// $ActiveMissionActorDataStart

struct ActiveMissionActorData {
    uint8 weapons[5]; // 0
    _(51);
    uint32 style;         // 0x38
    uint32 styleLevel;    // 0x3C
    byte32 expertise[8];  // 0x40
    float styleExpPoints; // 0x60
    float hitPoints;      // 0x64
    float maxHitPoints;   // 0x68
    float magicPoints;    // 0x6C
    float maxMagicPoints; // 0x70
};

static_assert(offsetof(ActiveMissionActorData, weapons) == 0);
static_assert(offsetof(ActiveMissionActorData, style) == 0x38);
static_assert(offsetof(ActiveMissionActorData, styleLevel) == 0x3C);
static_assert(offsetof(ActiveMissionActorData, expertise) == 0x40);
static_assert(offsetof(ActiveMissionActorData, styleExpPoints) == 0x60);
static_assert(offsetof(ActiveMissionActorData, hitPoints) == 0x64);
static_assert(offsetof(ActiveMissionActorData, maxHitPoints) == 0x68);
static_assert(offsetof(ActiveMissionActorData, magicPoints) == 0x6C);
static_assert(offsetof(ActiveMissionActorData, maxMagicPoints) == 0x70);

static_assert(sizeof(ActiveMissionActorData) == 116);

// $ActiveMissionActorDataEnd

struct SavingInGameData {
	_(16396);
	byte32 expertise[8]; // 0x400C
};

static_assert(offsetof(SavingInGameData, expertise) == 0x400C);

// $StyleDataStart

struct StyleData {
    uint32 rank; // 0
    float meter; // 4
    _(328);
    float quotient; // 0x150
    float dividend; // 0x154
    float divisor;  // 0x158
    _(4);
};

static_assert(offsetof(StyleData, rank) == 0);
static_assert(offsetof(StyleData, meter) == 4);
static_assert(offsetof(StyleData, quotient) == 0x150);
static_assert(offsetof(StyleData, dividend) == 0x154);
static_assert(offsetof(StyleData, divisor) == 0x158);

static_assert(sizeof(StyleData) == 352);

// $StyleDataEnd

// $CollisionDataMetadataStart

struct CollisionDataMetadata {
    _(48);
    vec4 data[8]; // 0x30
    _(32);
    void* collisionDataAddr; // 0xD0
    _(8);
    vec4 data2[3];   // 0xE0
    byte8* files[2]; // 0x110
    uint32 mode;     // 0x120
    _(12);
    vec4 data3;             // 0x130
    float heightAdjustment; // 0x140
};

static_assert(offsetof(CollisionDataMetadata, data) == 0x30);
static_assert(offsetof(CollisionDataMetadata, collisionDataAddr) == 0xD0);
static_assert(offsetof(CollisionDataMetadata, data2) == 0xE0);
static_assert(offsetof(CollisionDataMetadata, files) == 0x110);
static_assert(offsetof(CollisionDataMetadata, mode) == 0x120);
static_assert(offsetof(CollisionDataMetadata, data3) == 0x130);
static_assert(offsetof(CollisionDataMetadata, heightAdjustment) == 0x140);

static_assert(sizeof(CollisionDataMetadata) == 324);

// $CollisionDataMetadataEnd

// $CollisionDataStart

struct CollisionData {
    _(4);
    uint32 group; // 4
    _(128);
    CollisionDataMetadata* metadataAddr; // 0x88
    _(160);
    byte8* baseAddr; // 0x130
    _(8);
    byte32 flags; // 0x140
    _(204);
    vec4 data[8]; // 0x210
};

static_assert(offsetof(CollisionData, group) == 4);
static_assert(offsetof(CollisionData, metadataAddr) == 0x88);
static_assert(offsetof(CollisionData, baseAddr) == 0x130);
static_assert(offsetof(CollisionData, flags) == 0x140);
static_assert(offsetof(CollisionData, data) == 0x210);

static_assert(sizeof(CollisionData) == 656);

// $CollisionDataEnd

struct CameraControlMetadata {
	_(1200);
	uint32 fixedCameraAddr; // 0x4B0
};  

static_assert(offsetof(CameraControlMetadata, fixedCameraAddr) == 0x4B0);

// $CameraDataStart

struct CameraData {
    _(32);
    float fov; // 0x20
    float roll; // 0x24
    _(72);
    vec4 data[2]; // 0x70
    uint8 transitionToLockOnCam; // 0x90
    _(31);
    byte8* targetBaseAddr; // 0xB0
    _(24);
    float height;   // 0xD0
    float tilt;     // 0xD4
    float distance; // 0xD8
    _(4);
    float distanceLockOn; // 0xE0
    _(252);
    float cameraLag; // 0x1E0
    _(28);
};

static_assert(offsetof(CameraData, fov) == 0x20);
static_assert(offsetof(CameraData, roll) == 0x24);
static_assert(offsetof(CameraData, data) == 0x70);
static_assert(offsetof(CameraData, transitionToLockOnCam) == 0x90);
static_assert(offsetof(CameraData, targetBaseAddr) == 0xB0);
static_assert(offsetof(CameraData, height) == 0xD0);
static_assert(offsetof(CameraData, tilt) == 0xD4);
static_assert(offsetof(CameraData, distance) == 0xD8);
static_assert(offsetof(CameraData, distanceLockOn) == 0xE0);
static_assert(offsetof(CameraData, cameraLag) == 0x1E0);

static_assert(sizeof(CameraData) == 512);

// $CameraDataEnd

struct NewArchiveMetadata {
    byte8 signature[4];
    uint32 fileCount;
    uint32 fileOffs[1];

    byte8* operator[](uint32 fileIndex) {
        if (fileIndex >= fileCount) {
            return 0;
        }

        auto file = reinterpret_cast<byte8*>(this);

        auto fileOff = fileOffs[fileIndex];

        return (file + fileOff);
    }
};

struct ArchiveMetadata {
    byte8 signature[4];
    uint32 fileCount;
    uint32 fileOffs[1];
};

// $FileDataStatusStart

namespace FILE_DATA_STATUS {
enum {
    FREE,
    UNKNOWN_0,
    UNKNOWN_1,
    IN_USE,
};
};

// $FileDataStatusEnd

static_assert(FILE_DATA_STATUS::IN_USE == 3);

// $FileDataTypeDataStart

struct FileDataTypeData {
    _(8);
    const char* typeName; // 8
};

static_assert(offsetof(FileDataTypeData, typeName) == 8);

static_assert(sizeof(FileDataTypeData) == 16);

// $FileDataTypeDataEnd

// $FileDataStart

struct FileData {
    uint32 group;  // 0
    uint32 status; // 4
    _(16);
    FileDataTypeData* typeDataAddr; // 0x18
    byte8* file;                    // 0x20
    _(32);
};

static_assert(offsetof(FileData, group) == 0);
static_assert(offsetof(FileData, status) == 4);
static_assert(offsetof(FileData, typeDataAddr) == 0x18);
static_assert(offsetof(FileData, file) == 0x20);

static_assert(sizeof(FileData) == 72);

// $FileDataEnd

// $FileDataMetadataStart

struct FileDataMetadata {
    void* funcAddrs;        // 0
    void* lastAddr;         // 8
    void* nextAddr;         // 0x10
    FileData* fileDataAddr; // 0x18
    _(4);
    uint32 category;     // 0x24
    uint32 fileSetIndex; // 0x28
    _(4);
};

static_assert(offsetof(FileDataMetadata, funcAddrs) == 0);
static_assert(offsetof(FileDataMetadata, lastAddr) == 8);
static_assert(offsetof(FileDataMetadata, nextAddr) == 0x10);
static_assert(offsetof(FileDataMetadata, fileDataAddr) == 0x18);
static_assert(offsetof(FileDataMetadata, category) == 0x24);
static_assert(offsetof(FileDataMetadata, fileSetIndex) == 0x28);

static_assert(sizeof(FileDataMetadata) == 48);

// $FileDataMetadataEnd

struct RegionData {
    byte8* metadataAddr;
    byte8* dataAddr;
    uint32 capacity;
    uint32 boundary;
    uint32 size;
    uint32 pipe;
    uint32 count;
    _(4);
};

struct PS2_GAMEPAD {
    _(16);
    uint8 vibration[4];
    byte16 buttons[6];
    uint8 rightStickX;
    uint8 rightStickY;
    uint8 leftStickX;
    uint8 leftStickY;
    _(12);
    uint16 rightStickDirection[4];
    uint16 leftStickDirection[4];
    uint16 rightStickDirectionFast[2];
    uint16 leftStickDirectionFast[2];
    uint16 rightStickPosition;
    uint16 leftStickPosition;
    uint16 rightStickDifference;
    uint16 leftStickDifference;
};

struct ENGINE_GAMEPAD {
    byte16 buttons[4];
    uint16 buttonsTimer[2];
    uint16 rightStickDirection[4];
    uint16 rightStickTimer[2];
    int16 rightStickPosition;
    int16 rightStickRadius;
    uint16 leftStickDirection[4];
    uint16 leftStickTimer[2];
    int16 leftStickPosition;
    int16 leftStickRadius;
};

struct ActorEventData {
    uint32 event;
    uint32 lastEvent;
};

struct MotionData {
    uint8 index;
    uint8 group;
};

struct InputData {
    byte8 flags[8];
    float32 value;
};

struct ShadowData {
    _(192);
};

static_assert(sizeof(ShadowData) == 0xC0);

struct PhysicsData {
    _(240);
};

static_assert(sizeof(PhysicsData) == 0xF0);

struct PhysicsLinkData {
    _(40);
    bool32 enable;
    _(4);
    PhysicsData* physicsData;
    _(72);
    vec4 data[4];
};

static_assert(offsetof(PhysicsLinkData, enable) == 0x28);
static_assert(offsetof(PhysicsLinkData, physicsData) == 0x30);
static_assert(offsetof(PhysicsLinkData, data) == 0x80);
static_assert(sizeof(PhysicsLinkData) == 0xC0);

struct PhysicsMetadata {
    _(256);
    PhysicsLinkData* physicsLinkData;
    vec4* vertices;
    PhysicsData* physicsData;
    _(40);
};

static_assert(offsetof(PhysicsMetadata, physicsLinkData) == 0x100);
static_assert(offsetof(PhysicsMetadata, vertices) == 0x108);
static_assert(offsetof(PhysicsMetadata, physicsData) == 0x110);
static_assert(sizeof(PhysicsMetadata) == 0x140);

struct ModelPartitionData {
    uint8 value;
    _(895);
};

static_assert(sizeof(ModelPartitionData) == 0x380);

struct ModelData {
    _(8);
    byte8** funcAddrs; // 8
    _(8);
    bool visible; // 0x18
    bool physics; // 0x19
    _(358);
    ModelPartitionData* modelPartitionData; // 0x180
    _(888);
    struct {
        _(84);
        float duration[2]; // 0x554
        _(56);
        float duration2[2]; // 0x594
        _(244);
        bool init; // 0x690
        _(35);
        float timer[2]; // 0x6B4
    } Motion;           // 0x500
    _(196);
};

static_assert(offsetof(ModelData, funcAddrs) == 8);
static_assert(offsetof(ModelData, visible) == 0x18);
static_assert(offsetof(ModelData, physics) == 0x19);
static_assert(offsetof(ModelData, modelPartitionData) == 0x180);

static_assert(offsetof(ModelData, Motion.duration) == 0x554);
static_assert(offsetof(ModelData, Motion.duration2) == 0x594);
static_assert(offsetof(ModelData, Motion.init) == 0x690);
static_assert(offsetof(ModelData, Motion.timer) == 0x6B4);

static_assert(sizeof(ModelData) == 1920);

// $BodyPartDataStart

struct BodyPartData {
    _(104);
    byte8** motionArchives;   // 0x68
    ModelData* modelDataAddr; // 0x70
    _(66);
    bool busy; // 0xBA
    _(101);
};

static_assert(offsetof(BodyPartData, motionArchives) == 0x68);
static_assert(offsetof(BodyPartData, modelDataAddr) == 0x70);
static_assert(offsetof(BodyPartData, busy) == 0xBA);

static_assert(sizeof(BodyPartData) == 288);

// $BodyPartDataEnd

struct ModelMetadata {
    uint8 count;
    _(15);
    vec4 vertices[3];
    _(16);
};

static_assert(sizeof(ModelMetadata) == 80);

struct DevilModelMetadata {
    uint8 modelIndex;
    uint8 modelPhysicsMetadataIndex;
};

struct DevilSubmodelMetadata {
    uint8 submodelIndex;
    uint8 devilModelPhysicsMetadataIndex;
    uint8 devilSubmodelIndex;
};

struct DevilModelMetadata1 : DevilModelMetadata {
    DevilSubmodelMetadata devilSubmodelMetadata;
};

struct DevilModelMetadata2 : DevilModelMetadata {
    DevilSubmodelMetadata devilSubmodelMetadata[2];
};

struct DevilModelMetadataDante {
    DevilModelMetadata2 Rebellion;
    DevilModelMetadata1 Cerberus;
    DevilModelMetadata AgniRudra;
    DevilModelMetadata2 Nevan;
    DevilModelMetadata1 Beowulf;
    DevilModelMetadata1 Sparda;

    DevilModelMetadata2& operator[](uint8 index) {
        switch (index) {
        case DEVIL::REBELLION: {
            return Rebellion;
        }
        case DEVIL::CERBERUS: {
            return *reinterpret_cast<DevilModelMetadata2*>(&Cerberus);
        }
        case DEVIL::AGNI_RUDRA: {
            return *reinterpret_cast<DevilModelMetadata2*>(&AgniRudra);
        }
        case DEVIL::NEVAN: {
            return Nevan;
        }
        case DEVIL::BEOWULF: {
            return *reinterpret_cast<DevilModelMetadata2*>(&Beowulf);
        }
        case DEVIL::SPARDA: {
            return *reinterpret_cast<DevilModelMetadata2*>(&Sparda);
        }
        }
        return Rebellion;
    }
};

static_assert(sizeof(DevilModelMetadataDante) == 33);

struct RecoveryData {
    byte8** functions;
    _(24);
    bool init;
    _(15);
    vec4 data[4];
};

static_assert(offsetof(RecoveryData, init) == 0x20);
static_assert(offsetof(RecoveryData, data) == 0x30);
static_assert(sizeof(RecoveryData) == 0x70);

// $WeaponDataStart

struct WeaponData {
    _(20);
    float speed;           // 0x14
    float speedMultiplier; // 0x18
    _(44);
    byte8* baseAddr; // 0x48
    _(194);
    uint8 weapon; // 0x112
    _(5);
    uint8 value; // 0x118
    _(7);
    byte8* actorBaseAddr; // 0x120
};

static_assert(offsetof(WeaponData, speed) == 0x14);
static_assert(offsetof(WeaponData, speedMultiplier) == 0x18);
static_assert(offsetof(WeaponData, baseAddr) == 0x48);
static_assert(offsetof(WeaponData, weapon) == 0x112);
static_assert(offsetof(WeaponData, value) == 0x118);
static_assert(offsetof(WeaponData, actorBaseAddr) == 0x120);

static_assert(sizeof(WeaponData) == 296);

// $WeaponDataEnd

struct SummonedSwordsData {
    _(1376);
    byte8* actorBaseAddr;
};

static_assert(offsetof(SummonedSwordsData, actorBaseAddr) == 0x560);

// struct LockOnData
// {
// 	struct Item
// 	{
// 		byte8 * addr;
// 		byte8 * nextAddr;
// 	};

// 	_(8);
// 	Item items[255];
// 	_(8);
// 	byte8 * var_1000;
// 	byte8 * var_1008;
// 	Item * var_1010;
// 	_(4136);
// 	uint32 var_2040;
// };

// static_assert(offsetof(LockOnData, items) == 8);
// static_assert(offsetof(LockOnData, var_1000) == 0x1000);
// static_assert(offsetof(LockOnData, var_1008) == 0x1008);
// static_assert(offsetof(LockOnData, var_1010) == 0x1010);
// static_assert(offsetof(LockOnData, var_2040) == 0x2040);
// static_assert(sizeof(LockOnData) == 8260);


// $LockOnDataStart

struct LockOnData {
    byte8* var_0; // 0
    _(4096);
    byte8* var_1008; // 01008
    byte8* var_1010; // 0x1010
    byte8* var_1018; // 0x1018
    _(4136);
    uint32 targetCount; // 0x2048
    _(42);
    uint16 rotationTowardsTarget; // 0x2076
    _(200);
    vec4 targetPosition; // 0x2140
    _(24);
    byte8* targetBaseAddr60; // 0x2168
    byte8* var_2170;         // 0x2170
};

static_assert(offsetof(LockOnData, var_0) == 0);
static_assert(offsetof(LockOnData, var_1008) == 0x1008);
static_assert(offsetof(LockOnData, var_1010) == 0x1010);
static_assert(offsetof(LockOnData, var_1018) == 0x1018);
static_assert(offsetof(LockOnData, targetCount) == 0x2048);
static_assert(offsetof(LockOnData, rotationTowardsTarget) == 0x2076);
static_assert(offsetof(LockOnData, targetPosition) == 0x2140);
static_assert(offsetof(LockOnData, targetBaseAddr60) == 0x2168);
static_assert(offsetof(LockOnData, var_2170) == 0x2170);

static_assert(sizeof(LockOnData) == 8568);

// $LockOnDataEnd

// New data used by both player and enemy actors.

struct NewActorData {
    byte8* baseAddr;
    uint8 visibility;
    bool enableCollision;
};

struct CharacterData {
    uint8 character;
    uint8 costume;
    bool ignoreCostume;
    bool forceFiles;
    uint8 forceFilesCharacter;
    uint8 forceFilesCostume;

    uint8 styles[STYLE_COUNT][2];
    uint8 styleIndices[STYLE_COUNT];
    byte16 styleButtons[STYLE_COUNT];
    uint8 styleButtonIndex;

    uint8 meleeWeaponCount;
    uint8 meleeWeapons[MELEE_WEAPON_COUNT];
    uint8 meleeWeaponIndex;
    uint8 lastMeleeWeaponIndex;
    uint8 meleeWeaponSwitchType;
    uint8 meleeWeaponSwitchStick;

    uint8 rangedWeaponCount;
    uint8 rangedWeapons[RANGED_WEAPON_COUNT];
    uint8 rangedWeaponIndex;
    uint8 lastRangedWeaponIndex;
    uint8 rangedWeaponSwitchType;
    uint8 rangedWeaponSwitchStick;

    bool sparda = false;
    bool neloAngelo = false;
};

struct PlayerData {
    uint8 collisionGroup;

    byte16 switchButton;

    uint8 characterCount;
    uint8 characterIndex;
    uint8 lastCharacterIndex;
    uint8 activeCharacterIndex;

    CharacterData characterData[CHARACTER_COUNT][ENTITY_COUNT];

    bool removeBusyFlag;

    byte16 removeBusyFlagButtons[4] = {
        GAMEPAD::UP,
        GAMEPAD::RIGHT,
        GAMEPAD::DOWN,
        GAMEPAD::LEFT,
    };
};

// $ActorDataStart

struct ActorDataBase {
    _(8);
    uint8 status; // 8
    _(11);
    float speed;           // 0x14
    float speedMultiplier; // 0x18
    _(12);
    byte8* lastBaseAddr; // 0x28
    byte8* nextBaseAddr; // 0x30
    _(16);
    byte8* baseAddr; // 0x48
    _(13);
    bool visible; // 0x5D
    _(26);
    union {
        uint32 character; // 0x78
        uint32 enemy;     // 0x78
    };                    // 0x78
    _(4);
    vec4 position; // 0x80
    _(4);
    float verticalPull; // 0x94
    _(12);
    float verticalPullMultiplier; // 0xA4
    _(24);
    uint16 rotation; // 0xC0
    _(6);
};

/*static_assert(offsetof(ActorDataBase, speed) == 0x14);
static_assert(offsetof(ActorDataBase, speedMultiplier) == 0x18);
static_assert(offsetof(ActorDataBase, lastBaseAddr) == 0x28);
static_assert(offsetof(ActorDataBase, nextBaseAddr) == 0x30);
static_assert(offsetof(ActorDataBase, baseAddr) == 0x48);
static_assert(offsetof(ActorDataBase, visible) == 0x5D);
static_assert(offsetof(ActorDataBase, character) == 0x78);
static_assert(offsetof(ActorDataBase, enemy) == 0x78);
static_assert(offsetof(ActorDataBase, position) == 0x80);
static_assert(offsetof(ActorDataBase, verticalPull) == 0x94);
static_assert(offsetof(ActorDataBase, verticalPullMultiplier) == 0xA4);
static_assert(offsetof(ActorDataBase, rotation) == 0xC0);*/


// static_assert(sizeof(ActorDataBase) == 200);

struct PlayerActorDataBase : ActorDataBase {
    _(80);
    uint8 id; // 0x118
    _(3);
    bool32 isClone;    // 0x11C
    uint32 visibility; // 0x120
    _(156);
    float horizontalPull;           // 0x1C0
    float horizontalPullMultiplier; // 0x1C4
    _(56);
    ModelData modelData[3];                           // 0x200
    PhysicsMetadata* modelPhysicsMetadataPool[4][24]; // 0x1880
    uint32 modelAllocationDataCount;                  // 0x1B80
    _(4);
    Size_32 modelAllocationData[209]; // 0x1B88
    _(748);
    float motionSpeed;           // 0x3894
    float motionSpeedMultiplier; // 0x3898
    _(4);
    byte8* motionArchives[34];      // 0x38A0
    MotionData motionData[2];       // 0x39B0
    MotionData motionDataMirror[3]; // 0x39B4
    _(2);
    uint32 var_39BC;                    // 0x39BC
    uint8 var_39C0[16];                 // 0x39C0
    uint32 nextActionRequestPolicy[16]; // 0x39D0
    uint8 var_3A10[8];                  // 0x3A10
    uint32 shadow;                      // 0x3A18
    uint32 lastShadow;                  // 0x3A1C
    _(8);
    byte32 color; // 0x3A28
    _(212);
    RecoveryData recoveryData[3]; // 0x3B00
    byte8 var_3C50[2];            // 0x3C50
    _(382);
    byte8* actionData[6];        // 0x3DD0
    ActorEventData eventData[2]; // 0x3E00
    uint8 recoverState[32];          // 0x3E10
    _(4);
    float motionTimer; // 0x3E34
    float idleTimer;   // 0x3E38
    _(36);
    byte32 permissions;      // 0x3E60
    byte32 state;            // 0x3E64
    byte32 lastState;        // 0x3E68
    uint32 activeModelIndex; // 0x3E6C
    uint32 queuedModelIndex; // 0x3E70
    uint32 devilModels[3];   // 0x3E74
    uint8 modelState;        // 0x3E80
    _(3);
    bool32 lockOn;                 // 0x3E84
    uint32 activeModelIndexMirror; // 0x3E88
    uint32 activeDevil;            // 0x3E8C
    uint32 airRaid;                // 0x3E90
    uint32 mode;                   // 0x3E94
    _(1);
    bool dead; // 0x3E99
    _(1);
    bool devil; // 0x3E9B
    _(2);
    uint8 costume; // 0x3E9E
    union {
        bool sparda;     // 0x3E9F6
        bool neroAngelo; // 0x3E9F
    };                   // 0x3E9F
    _(4);
    bool useHolyWater; // 0x3EA4
    _(11);
    float dtExplosionCharge; // 0x3EB0
    _(4);
    float magicPoints;    // 0x3EB8
    float maxMagicPoints; // 0x3EBC
    _(4);
    float var_3EC4; // 0x3EC4
    float var_3EC8; // 0x3EC8
    _(6);
    uint16 var_3ED2; // 0x3ED2
    _(4);
    uint16 cameraDirection; // 0x3ED8
    _(2);
    float var_3EDC; // 0x3EDC
    _(49);
    uint8 airHikeCount;  // 0x3F11
    uint8 kickJumpCount; // 0x3F12
    uint8 wallHikeCount; // 0x3F13
    bool enableAirRaid;  // 0x3F14
    _(4);
    bool var_3F19; // 0x3F19
    _(138);
    uint8 action;     // 0x3FA4
    uint8 lastAction; // 0x3FA5
    _(2);
    uint8 bufferedAction; // 0x3FA8
    _(2);
    uint8 airGunAttackCount;   // 0x3FAB
    uint8 airSwordAttackCount; // 0x3FAC
    _(1);
    uint8 sumAttackCount; // 0x3FAE
    _(1);
    uint16 rotationOffset; // 0x3FB0
    _(58);
    byte32 activeExpertise[8]; // 0x3FEC
    _(32);
    byte32 queuedExpertise[8]; // 0x402C
    _(160);
    float maxHitPoints; // 0x40EC
    _(44);
    float hitPoints; // 0x411C
    _(160);
    LockOnData lockOnData;    // 0x41C0
    uint32 style;             // 0x6338
    uint32 wallHikeDirection; // 0x633C
    bool guard;          // 0x6340
    _(23);
    uint32 styleLevel;    // 0x6358
    uint8 dashCount;      // 0x635C
    uint8 skyStarCount;   // 0x635D
    uint8 airTrickCount;  // 0x635E
    uint8 trickUpCount;   // 0x635F
    uint8 trickDownCount; // 0x6360
    bool quicksilver;     // 0x6361
    bool doppelganger;    // 0x6362
    _(1);
    float styleExpPoints; // 0x6364
    uint8 royalBlock; //0x6368
    _(7);
    uint32 royalguardBlockType; // 0x6370
    _(4);
    bool airGuard; // 0x6378
    _(7);
    uint16 royalguardReleaseLevel;       // 0x6380
    uint8 royalguardReleaseEffectIndex; // 0x6381
    _(1);
    float royalguardReleaseDamage; // 0x6384
    float var_6388;                // 0x6388
    float var_638C;                // 0x638C
    _(64);
    uint32 var_63D0;         // 0x63D0
    uint32 quicksilverStage; // 0x63D4
    _(56);
    byte8 var_6410[40]; // 0x6410
    byte8* var_6438;    // 0x6438
    uint32 var_6440;    // 0x6440
    _(12);
    uint32 cloneRate;               // 0x6450
    uint32 cloneStatus;             // 0x6454
    byte8 var_6458[32];             // 0x6458
    byte8* cloneActorBaseAddr;      // 0x6478
    bool cloneIsControlledByPlayer; // 0x6480
    _(3);
    uint32 activeMeleeWeaponIndex; // 0x6484
    uint32 queuedMeleeWeaponIndex; // 0x6488
    _(1);
    uint8 activeWeapon; // 0x648D
    _(2);
    uint32 meleeWeaponIndex;  // 0x6490
    uint32 rangedWeaponIndex; // 0x6494
    uint8 weapons[5];         // 0x6498
    _(3);
    WeaponData* weaponDataAddr[5]; // 0x64A0
    uint32 weaponStatus[5];        // 0x64C8
    uint32 weaponLevels[5];        // 0x64DC
    uint8 activeMeleeWeapon;       // 0x64F0
    uint8 activeRangedWeapon;      // 0x64F1
    _(2);
    float weaponTimers[5];           // 0x64F4
    float meleeWeaponSwitchTimeout;  // 0x6508
    float rangedWeaponSwitchTimeout; // 0x650C
    StyleData styleData;             // 0x6510
    float var_6670;                  // 0x6670
    InputData inputData[58];         // 0x6674
    _(36);
    BodyPartData bodyPartData[3][2]; // 0x6950
    _(576);
    CollisionData collisionData; // 0x7250
    byte16 buttons[4];           // 0x74E0
    _(16);
    uint16 rightStickPosition; // 0x74F8
    uint16 rightStickRadius;   // 0x74FA
    _(12);
    uint16 leftStickPosition;    // 0x7508
    uint16 leftStickRadius;      // 0x750A
    uint16 actorCameraDirection; // 0x750C
    _(14);
    uint32 leftStickDirection[2]; // 0x751C
    _(28);

    operator byte8*() { return reinterpret_cast<byte8*>(this); }
};

static_assert(offsetof(PlayerActorDataBase, isClone) == 0x11C);
static_assert(offsetof(PlayerActorDataBase, visibility) == 0x120);
static_assert(offsetof(PlayerActorDataBase, horizontalPull) == 0x1C0);
static_assert(offsetof(PlayerActorDataBase, horizontalPullMultiplier) == 0x1C4);
static_assert(offsetof(PlayerActorDataBase, modelData) == 0x200);
static_assert(offsetof(PlayerActorDataBase, modelPhysicsMetadataPool) == 0x1880);
static_assert(offsetof(PlayerActorDataBase, modelAllocationDataCount) == 0x1B80);
static_assert(offsetof(PlayerActorDataBase, modelAllocationData) == 0x1B88);
static_assert(offsetof(PlayerActorDataBase, motionSpeed) == 0x3894);
static_assert(offsetof(PlayerActorDataBase, motionSpeedMultiplier) == 0x3898);
static_assert(offsetof(PlayerActorDataBase, motionArchives) == 0x38A0);
static_assert(offsetof(PlayerActorDataBase, motionData) == 0x39B0);
static_assert(offsetof(PlayerActorDataBase, motionDataMirror) == 0x39B4);
static_assert(offsetof(PlayerActorDataBase, var_39BC) == 0x39BC);
static_assert(offsetof(PlayerActorDataBase, var_39C0) == 0x39C0);
static_assert(offsetof(PlayerActorDataBase, nextActionRequestPolicy) == 0x39D0);
static_assert(offsetof(PlayerActorDataBase, var_3A10) == 0x3A10);
static_assert(offsetof(PlayerActorDataBase, shadow) == 0x3A18);
static_assert(offsetof(PlayerActorDataBase, lastShadow) == 0x3A1C);
static_assert(offsetof(PlayerActorDataBase, color) == 0x3A28);
static_assert(offsetof(PlayerActorDataBase, recoveryData) == 0x3B00);
static_assert(offsetof(PlayerActorDataBase, var_3C50) == 0x3C50);
static_assert(offsetof(PlayerActorDataBase, actionData) == 0x3DD0);
static_assert(offsetof(PlayerActorDataBase, eventData) == 0x3E00);
static_assert(offsetof(PlayerActorDataBase, recoverState) == 0x3E10);
static_assert(offsetof(PlayerActorDataBase, motionTimer) == 0x3E34);
static_assert(offsetof(PlayerActorDataBase, idleTimer) == 0x3E38);
static_assert(offsetof(PlayerActorDataBase, permissions) == 0x3E60);
static_assert(offsetof(PlayerActorDataBase, state) == 0x3E64);
static_assert(offsetof(PlayerActorDataBase, lastState) == 0x3E68);
static_assert(offsetof(PlayerActorDataBase, activeModelIndex) == 0x3E6C);
static_assert(offsetof(PlayerActorDataBase, queuedModelIndex) == 0x3E70);
static_assert(offsetof(PlayerActorDataBase, devilModels) == 0x3E74);
static_assert(offsetof(PlayerActorDataBase, modelState) == 0x3E80);
static_assert(offsetof(PlayerActorDataBase, lockOn) == 0x3E84);
static_assert(offsetof(PlayerActorDataBase, activeModelIndexMirror) == 0x3E88);
static_assert(offsetof(PlayerActorDataBase, activeDevil) == 0x3E8C);
static_assert(offsetof(PlayerActorDataBase, airRaid) == 0x3E90);
static_assert(offsetof(PlayerActorDataBase, mode) == 0x3E94);
static_assert(offsetof(PlayerActorDataBase, dead) == 0x3E99);
static_assert(offsetof(PlayerActorDataBase, devil) == 0x3E9B);
static_assert(offsetof(PlayerActorDataBase, costume) == 0x3E9E);
static_assert(offsetof(PlayerActorDataBase, sparda) == 0x3E9F);
static_assert(offsetof(PlayerActorDataBase, neroAngelo) == 0x3E9F);
static_assert(offsetof(PlayerActorDataBase, useHolyWater) == 0x3EA4);
static_assert(offsetof(PlayerActorDataBase, dtExplosionCharge) == 0x3EB0);
static_assert(offsetof(PlayerActorDataBase, magicPoints) == 0x3EB8);
static_assert(offsetof(PlayerActorDataBase, maxMagicPoints) == 0x3EBC);
static_assert(offsetof(PlayerActorDataBase, var_3EC4) == 0x3EC4);
static_assert(offsetof(PlayerActorDataBase, var_3EC8) == 0x3EC8);
static_assert(offsetof(PlayerActorDataBase, var_3ED2) == 0x3ED2);
static_assert(offsetof(PlayerActorDataBase, cameraDirection) == 0x3ED8);
static_assert(offsetof(PlayerActorDataBase, var_3EDC) == 0x3EDC);
static_assert(offsetof(PlayerActorDataBase, airHikeCount) == 0x3F11);
static_assert(offsetof(PlayerActorDataBase, kickJumpCount) == 0x3F12);
static_assert(offsetof(PlayerActorDataBase, wallHikeCount) == 0x3F13);
static_assert(offsetof(PlayerActorDataBase, enableAirRaid) == 0x3F14);
static_assert(offsetof(PlayerActorDataBase, var_3F19) == 0x3F19);
static_assert(offsetof(PlayerActorDataBase, action) == 0x3FA4);
static_assert(offsetof(PlayerActorDataBase, lastAction) == 0x3FA5);
static_assert(offsetof(PlayerActorDataBase, bufferedAction) == 0x3FA8);
static_assert(offsetof(PlayerActorDataBase, airGunAttackCount) == 0x3FAB);
static_assert(offsetof(PlayerActorDataBase, airSwordAttackCount) == 0x3FAC);
static_assert(offsetof(PlayerActorDataBase, sumAttackCount) == 0x3FAE);
static_assert(offsetof(PlayerActorDataBase, rotationOffset) == 0x3FB0);
static_assert(offsetof(PlayerActorDataBase, activeExpertise) == 0x3FEC);
static_assert(offsetof(PlayerActorDataBase, queuedExpertise) == 0x402C);
static_assert(offsetof(PlayerActorDataBase, maxHitPoints) == 0x40EC);
static_assert(offsetof(PlayerActorDataBase, hitPoints) == 0x411C);
static_assert(offsetof(PlayerActorDataBase, lockOnData) == 0x41C0);
static_assert(offsetof(PlayerActorDataBase, style) == 0x6338);
static_assert(offsetof(PlayerActorDataBase, wallHikeDirection) == 0x633C);
static_assert(offsetof(PlayerActorDataBase, guard) == 0x6340);
static_assert(offsetof(PlayerActorDataBase, styleLevel) == 0x6358);
static_assert(offsetof(PlayerActorDataBase, dashCount) == 0x635C);
static_assert(offsetof(PlayerActorDataBase, skyStarCount) == 0x635D);
static_assert(offsetof(PlayerActorDataBase, airTrickCount) == 0x635E);
static_assert(offsetof(PlayerActorDataBase, trickUpCount) == 0x635F);
static_assert(offsetof(PlayerActorDataBase, trickDownCount) == 0x6360);
static_assert(offsetof(PlayerActorDataBase, quicksilver) == 0x6361);
static_assert(offsetof(PlayerActorDataBase, doppelganger) == 0x6362);
static_assert(offsetof(PlayerActorDataBase, styleExpPoints) == 0x6364);
static_assert(offsetof(PlayerActorDataBase, royalBlock) == 0x6368);
static_assert(offsetof(PlayerActorDataBase, royalguardBlockType) == 0x6370);
static_assert(offsetof(PlayerActorDataBase, airGuard) == 0x6378);
static_assert(offsetof(PlayerActorDataBase, royalguardReleaseLevel) == 0x6380);
//static_assert(offsetof(PlayerActorDataBase, royalguardReleaseEffectIndex) == 0x6381);
static_assert(offsetof(PlayerActorDataBase, royalguardReleaseDamage) == 0x6384);
static_assert(offsetof(PlayerActorDataBase, var_6388) == 0x6388);
static_assert(offsetof(PlayerActorDataBase, var_638C) == 0x638C);
static_assert(offsetof(PlayerActorDataBase, var_63D0) == 0x63D0);
static_assert(offsetof(PlayerActorDataBase, quicksilverStage) == 0x63D4);
static_assert(offsetof(PlayerActorDataBase, var_6410) == 0x6410);
static_assert(offsetof(PlayerActorDataBase, var_6438) == 0x6438);
static_assert(offsetof(PlayerActorDataBase, var_6440) == 0x6440);
static_assert(offsetof(PlayerActorDataBase, cloneRate) == 0x6450);
static_assert(offsetof(PlayerActorDataBase, cloneStatus) == 0x6454);
static_assert(offsetof(PlayerActorDataBase, var_6458) == 0x6458);
static_assert(offsetof(PlayerActorDataBase, cloneActorBaseAddr) == 0x6478);
static_assert(offsetof(PlayerActorDataBase, cloneIsControlledByPlayer) == 0x6480);
static_assert(offsetof(PlayerActorDataBase, activeMeleeWeaponIndex) == 0x6484);
static_assert(offsetof(PlayerActorDataBase, queuedMeleeWeaponIndex) == 0x6488);
static_assert(offsetof(PlayerActorDataBase, activeWeapon) == 0x648D);
static_assert(offsetof(PlayerActorDataBase, meleeWeaponIndex) == 0x6490);
static_assert(offsetof(PlayerActorDataBase, rangedWeaponIndex) == 0x6494);
static_assert(offsetof(PlayerActorDataBase, weapons) == 0x6498);
static_assert(offsetof(PlayerActorDataBase, weaponDataAddr) == 0x64A0);
static_assert(offsetof(PlayerActorDataBase, weaponStatus) == 0x64C8);
static_assert(offsetof(PlayerActorDataBase, weaponLevels) == 0x64DC);
static_assert(offsetof(PlayerActorDataBase, activeMeleeWeapon) == 0x64F0);
static_assert(offsetof(PlayerActorDataBase, activeRangedWeapon) == 0x64F1);
static_assert(offsetof(PlayerActorDataBase, weaponTimers) == 0x64F4);
static_assert(offsetof(PlayerActorDataBase, meleeWeaponSwitchTimeout) == 0x6508);
static_assert(offsetof(PlayerActorDataBase, rangedWeaponSwitchTimeout) == 0x650C);
static_assert(offsetof(PlayerActorDataBase, styleData) == 0x6510);
static_assert(offsetof(PlayerActorDataBase, var_6670) == 0x6670);
static_assert(offsetof(PlayerActorDataBase, inputData) == 0x6674);
static_assert(offsetof(PlayerActorDataBase, bodyPartData) == 0x6950);
static_assert(offsetof(PlayerActorDataBase, collisionData) == 0x7250);
static_assert(offsetof(PlayerActorDataBase, buttons) == 0x74E0);
static_assert(offsetof(PlayerActorDataBase, rightStickPosition) == 0x74F8);
static_assert(offsetof(PlayerActorDataBase, rightStickRadius) == 0x74FA);
static_assert(offsetof(PlayerActorDataBase, leftStickPosition) == 0x7508);
static_assert(offsetof(PlayerActorDataBase, leftStickRadius) == 0x750A);
static_assert(offsetof(PlayerActorDataBase, actorCameraDirection) == 0x750C);
static_assert(offsetof(PlayerActorDataBase, leftStickDirection) == 0x751C);

static_assert(sizeof(PlayerActorDataBase) == 0x7540);

struct PlayerActorData : PlayerActorDataBase {
	_(17192);
	float artemisCharge; // 0xB868
    _(84);
    uint32 newFirstVar; // 0xB8C0
    _(12);
    ModelData newModelData[6];                           // 0xB8D0
    PhysicsMetadata* newModelPhysicsMetadataPool[7][24]; // 0xE5D0
    Size_32 newModelAllocationData[512];                 // 0xEB10
    RecoveryData newRecoveryData[6];                     // 0x12B10
    uint32 newDevilModels[6];                            // 0x12DB0
    _(8);
    BodyPartData newBodyPartData[7][2]; // 0x12DD0
    ModelData newSubmodelData[11];      // 0x13D90
    bool newSubmodelInit[11];           // 0x19010
    _(5);
    ShadowData newModelShadowData[6];                         // 0x19020
    ShadowData newSubmodelShadowData[11];                     // 0x194A0
    PhysicsMetadata* newDevilModelPhysicsMetadataPool[5][36]; // 0x19CE0
    PhysicsData newDevilSubmodelPhysicsData[10];              // 0x1A280
    PhysicsLinkData newDevilSubmodelPhysicsLinkData[10][4];   // 0x1ABE0
    uint8 newWeapons[10];                                     // 0x1C9E0
    _(6);
    WeaponData* newWeaponDataAddr[10]; // 0x1C9F0
    uint32 newWeaponStatus[10];        // 0x1CA40
    _(8);
    uint32 newWeaponLevels[10]; // 0x1CA70
    _(8);
    float newWeaponTimers[10];    // 0x1CAA0
    bool32 newIsClone;            // 0x1CAC8
    uint8 newPlayerIndex;         // 0x1CACC
    uint8 newCharacterIndex;      // 0x1CACD
    uint8 newEntityIndex;         // 0x1CACE
    bool newForceFiles;           // 0x1CACF
    uint8 newForceFilesCharacter; // 0x1CAD0
    uint8 newForceFilesCostume;   // 0x1CAD1
    bool newEnableVisibility;     // 0x1CAD2
    uint8 newGamepad;             // 0x1CAD3
    byte16 newButtonMask;         // 0x1CAD4
    bool newEnableRightStick;     // 0x1CAD6
    bool newEnableLeftStick;      // 0x1CAD7
    bool newQuickDrive;           // 0x1CAD8
    bool newEnableCollision;      // 0x1CAD9
    bool newActorLoopRun;         // 0x1CADA
    uint8 newAirHikeCount;        // 0x1CADB
    uint8 newKickJumpCount;       // 0x1CADC
    uint8 newWallHikeCount;       // 0x1CADD
    uint8 newDashCount;           // 0x1CADE
    uint8 newSkyStarCount;        // 0x1CADF
    uint8 newAirTrickCount;       // 0x1CAE0
    uint8 newTrickUpCount;        // 0x1CAE1
    uint8 newTrickDownCount;      // 0x1CAE2
    uint8 newAirStingerCount;     // 0x1CAE3
    uint8 newAirRisingSunCount;   // 0x1CAE4
    _(11);
    uint32 newEffectIndices[12]; // 0x1CAF0
    uint32 newLastVar;           // 0x1CB20
};

static_assert(offsetof(PlayerActorData, artemisCharge) == 0xB868);
static_assert(offsetof(PlayerActorData, newFirstVar) == 0xB8C0);
static_assert(offsetof(PlayerActorData, newModelData) == 0xB8D0);
static_assert(offsetof(PlayerActorData, newModelPhysicsMetadataPool) == 0xE5D0);
static_assert(offsetof(PlayerActorData, newModelAllocationData) == 0xEB10);
static_assert(offsetof(PlayerActorData, newRecoveryData) == 0x12B10);
static_assert(offsetof(PlayerActorData, newDevilModels) == 0x12DB0);
static_assert(offsetof(PlayerActorData, newBodyPartData) == 0x12DD0);
static_assert(offsetof(PlayerActorData, newSubmodelData) == 0x13D90);
static_assert(offsetof(PlayerActorData, newSubmodelInit) == 0x19010);
static_assert(offsetof(PlayerActorData, newModelShadowData) == 0x19020);
static_assert(offsetof(PlayerActorData, newSubmodelShadowData) == 0x194A0);
static_assert(offsetof(PlayerActorData, newDevilModelPhysicsMetadataPool) == 0x19CE0);
static_assert(offsetof(PlayerActorData, newDevilSubmodelPhysicsData) == 0x1A280);
static_assert(offsetof(PlayerActorData, newDevilSubmodelPhysicsLinkData) == 0x1ABE0);
static_assert(offsetof(PlayerActorData, newWeapons) == 0x1C9E0);
static_assert(offsetof(PlayerActorData, newWeaponDataAddr) == 0x1C9F0);
static_assert(offsetof(PlayerActorData, newWeaponStatus) == 0x1CA40);
static_assert(offsetof(PlayerActorData, newWeaponLevels) == 0x1CA70);
static_assert(offsetof(PlayerActorData, newWeaponTimers) == 0x1CAA0);
static_assert(offsetof(PlayerActorData, newIsClone) == 0x1CAC8);
static_assert(offsetof(PlayerActorData, newPlayerIndex) == 0x1CACC);
static_assert(offsetof(PlayerActorData, newCharacterIndex) == 0x1CACD);
static_assert(offsetof(PlayerActorData, newEntityIndex) == 0x1CACE);
static_assert(offsetof(PlayerActorData, newForceFiles) == 0x1CACF);
static_assert(offsetof(PlayerActorData, newForceFilesCharacter) == 0x1CAD0);
static_assert(offsetof(PlayerActorData, newForceFilesCostume) == 0x1CAD1);
static_assert(offsetof(PlayerActorData, newEnableVisibility) == 0x1CAD2);
static_assert(offsetof(PlayerActorData, newGamepad) == 0x1CAD3);
static_assert(offsetof(PlayerActorData, newButtonMask) == 0x1CAD4);
static_assert(offsetof(PlayerActorData, newEnableRightStick) == 0x1CAD6);
static_assert(offsetof(PlayerActorData, newEnableLeftStick) == 0x1CAD7);
static_assert(offsetof(PlayerActorData, newQuickDrive) == 0x1CAD8);
static_assert(offsetof(PlayerActorData, newEnableCollision) == 0x1CAD9);
static_assert(offsetof(PlayerActorData, newActorLoopRun) == 0x1CADA);
static_assert(offsetof(PlayerActorData, newAirHikeCount) == 0x1CADB);
static_assert(offsetof(PlayerActorData, newKickJumpCount) == 0x1CADC);
static_assert(offsetof(PlayerActorData, newWallHikeCount) == 0x1CADD);
static_assert(offsetof(PlayerActorData, newDashCount) == 0x1CADE);
static_assert(offsetof(PlayerActorData, newSkyStarCount) == 0x1CADF);
static_assert(offsetof(PlayerActorData, newAirTrickCount) == 0x1CAE0);
static_assert(offsetof(PlayerActorData, newTrickUpCount) == 0x1CAE1);
static_assert(offsetof(PlayerActorData, newTrickDownCount) == 0x1CAE2);
static_assert(offsetof(PlayerActorData, newAirStingerCount) == 0x1CAE3);
static_assert(offsetof(PlayerActorData, newAirRisingSunCount) == 0x1CAE4);
static_assert(offsetof(PlayerActorData, newEffectIndices) == 0x1CAF0);
static_assert(offsetof(PlayerActorData, newLastVar) == 0x1CB20);

struct PlayerActorDataDante : PlayerActorDataBase {
    ModelData submodelData[5]; // 0x7540
    bool submodelInit[5];      // 0x9AC0
    _(11);
    ShadowData modelShadowData[3];                    // 0x9AD0
    ShadowData submodelShadowData[5];                 // 0x9D10
    PhysicsMetadata* submodelPhysicsMetadataPool[39]; // 0xA0D0
    _(8);
    PhysicsData submodelPhysicsData;                       // 0xA210
    PhysicsMetadata* devilModelPhysicsMetadataPool[2][36]; // 0xA300
    PhysicsData devilSubmodelPhysicsData[4];               // 0xA540
    _(256);
    PhysicsLinkData devilSubmodelPhysicsLinkData[4][4]; // 0xAA00
    DevilModelMetadataDante devilModelMetadata;         // 0xB600
    _(15);
    ModelMetadata modelMetadata[6]; // 0xB630
    _(88);
    float artemisChargeValues[2]; // 0xB868
    _(14);
    uint8 artemisStatus; // 0xB87E
    _(1);
    bool artemisMultiLock;        // 0xB880
    bool artemisMultiLockCharged; // 0xB881
    _(62);
    uint32 newFirstVar; // 0xB8C0
    _(12);
    ModelData newModelData[6];                           // 0xB8D0
    PhysicsMetadata* newModelPhysicsMetadataPool[7][24]; // 0xE5D0
    Size_32 newModelAllocationData[512];                 // 0xEB10
    RecoveryData newRecoveryData[6];                     // 0x12B10
    uint32 newDevilModels[6];                            // 0x12DB0
    _(8);
    BodyPartData newBodyPartData[7][2]; // 0x12DD0
    ModelData newSubmodelData[11];      // 0x13D90
    bool newSubmodelInit[11];           // 0x19010
    _(5);
    ShadowData newModelShadowData[6];                         // 0x19020
    ShadowData newSubmodelShadowData[11];                     // 0x194A0
    PhysicsMetadata* newDevilModelPhysicsMetadataPool[5][36]; // 0x19CE0
    PhysicsData newDevilSubmodelPhysicsData[10];              // 0x1A280
    PhysicsLinkData newDevilSubmodelPhysicsLinkData[10][4];   // 0x1ABE0
    uint8 newWeapons[10];                                     // 0x1C9E0
    _(6);
    WeaponData* newWeaponDataAddr[10]; // 0x1C9F0
    uint32 newWeaponStatus[10];        // 0x1CA40
    _(8);
    uint32 newWeaponLevels[10]; // 0x1CA70
    _(8);
    float newWeaponTimers[10];    // 0x1CAA0
    bool32 newIsClone;            // 0x1CAC8
    uint8 newPlayerIndex;         // 0x1CACC
    uint8 newCharacterIndex;      // 0x1CACD
    uint8 newEntityIndex;         // 0x1CACE
    bool newForceFiles;           // 0x1CACF
    uint8 newForceFilesCharacter; // 0x1CAD0
    uint8 newForceFilesCostume;   // 0x1CAD1
    bool newEnableVisibility;     // 0x1CAD2
    uint8 newGamepad;             // 0x1CAD3
    byte16 newButtonMask;         // 0x1CAD4
    bool newEnableRightStick;     // 0x1CAD6
    bool newEnableLeftStick;      // 0x1CAD7
    bool newQuickDrive;           // 0x1CAD8
    bool newEnableCollision;      // 0x1CAD9
    bool newActorLoopRun;         // 0x1CADA
    uint8 newAirHikeCount;        // 0x1CADB
    uint8 newKickJumpCount;       // 0x1CADC
    uint8 newWallHikeCount;       // 0x1CADD
    uint8 newDashCount;           // 0x1CADE
    uint8 newSkyStarCount;        // 0x1CADF
    uint8 newAirTrickCount;       // 0x1CAE0
    uint8 newTrickUpCount;        // 0x1CAE1
    uint8 newTrickDownCount;      // 0x1CAE2
    uint8 newAirStingerCount;     // 0x1CAE3
    uint8 newAirRisingSunCount;   // 0x1CAE4
    _(11);
    uint32 newEffectIndices[12]; // 0x1CAF0
    uint32 newLastVar;           // 0x1CB20
};

static_assert(offsetof(PlayerActorDataDante, submodelData) == 0x7540);
static_assert(offsetof(PlayerActorDataDante, submodelInit) == 0x9AC0);
static_assert(offsetof(PlayerActorDataDante, modelShadowData) == 0x9AD0);
static_assert(offsetof(PlayerActorDataDante, submodelShadowData) == 0x9D10);
static_assert(offsetof(PlayerActorDataDante, submodelPhysicsMetadataPool) == 0xA0D0);
static_assert(offsetof(PlayerActorDataDante, submodelPhysicsData) == 0xA210);
static_assert(offsetof(PlayerActorDataDante, devilModelPhysicsMetadataPool) == 0xA300);
static_assert(offsetof(PlayerActorDataDante, devilSubmodelPhysicsData) == 0xA540);
static_assert(offsetof(PlayerActorDataDante, devilSubmodelPhysicsLinkData) == 0xAA00);
static_assert(offsetof(PlayerActorDataDante, devilModelMetadata) == 0xB600);
static_assert(offsetof(PlayerActorDataDante, modelMetadata) == 0xB630);
static_assert(offsetof(PlayerActorDataDante, artemisChargeValues) == 0xB868);
static_assert(offsetof(PlayerActorDataDante, artemisStatus) == 0xB87E);
static_assert(offsetof(PlayerActorDataDante, artemisMultiLock) == 0xB880);
static_assert(offsetof(PlayerActorDataDante, artemisMultiLockCharged) == 0xB881);
static_assert(offsetof(PlayerActorDataDante, newFirstVar) == 0xB8C0);
static_assert(offsetof(PlayerActorDataDante, newModelData) == 0xB8D0);
static_assert(offsetof(PlayerActorDataDante, newModelPhysicsMetadataPool) == 0xE5D0);
static_assert(offsetof(PlayerActorDataDante, newModelAllocationData) == 0xEB10);
static_assert(offsetof(PlayerActorDataDante, newRecoveryData) == 0x12B10);
static_assert(offsetof(PlayerActorDataDante, newDevilModels) == 0x12DB0);
static_assert(offsetof(PlayerActorDataDante, newBodyPartData) == 0x12DD0);
static_assert(offsetof(PlayerActorDataDante, newSubmodelData) == 0x13D90);
static_assert(offsetof(PlayerActorDataDante, newSubmodelInit) == 0x19010);
static_assert(offsetof(PlayerActorDataDante, newModelShadowData) == 0x19020);
static_assert(offsetof(PlayerActorDataDante, newSubmodelShadowData) == 0x194A0);
static_assert(offsetof(PlayerActorDataDante, newDevilModelPhysicsMetadataPool) == 0x19CE0);
static_assert(offsetof(PlayerActorDataDante, newDevilSubmodelPhysicsData) == 0x1A280);
static_assert(offsetof(PlayerActorDataDante, newDevilSubmodelPhysicsLinkData) == 0x1ABE0);
static_assert(offsetof(PlayerActorDataDante, newWeapons) == 0x1C9E0);
static_assert(offsetof(PlayerActorDataDante, newWeaponDataAddr) == 0x1C9F0);
static_assert(offsetof(PlayerActorDataDante, newWeaponStatus) == 0x1CA40);
static_assert(offsetof(PlayerActorDataDante, newWeaponLevels) == 0x1CA70);
static_assert(offsetof(PlayerActorDataDante, newWeaponTimers) == 0x1CAA0);
static_assert(offsetof(PlayerActorDataDante, newIsClone) == 0x1CAC8);
static_assert(offsetof(PlayerActorDataDante, newPlayerIndex) == 0x1CACC);
static_assert(offsetof(PlayerActorDataDante, newCharacterIndex) == 0x1CACD);
static_assert(offsetof(PlayerActorDataDante, newEntityIndex) == 0x1CACE);
static_assert(offsetof(PlayerActorDataDante, newForceFiles) == 0x1CACF);
static_assert(offsetof(PlayerActorDataDante, newForceFilesCharacter) == 0x1CAD0);
static_assert(offsetof(PlayerActorDataDante, newForceFilesCostume) == 0x1CAD1);
static_assert(offsetof(PlayerActorDataDante, newEnableVisibility) == 0x1CAD2);
static_assert(offsetof(PlayerActorDataDante, newGamepad) == 0x1CAD3);
static_assert(offsetof(PlayerActorDataDante, newButtonMask) == 0x1CAD4);
static_assert(offsetof(PlayerActorDataDante, newEnableRightStick) == 0x1CAD6);
static_assert(offsetof(PlayerActorDataDante, newEnableLeftStick) == 0x1CAD7);
static_assert(offsetof(PlayerActorDataDante, newQuickDrive) == 0x1CAD8);
static_assert(offsetof(PlayerActorDataDante, newEnableCollision) == 0x1CAD9);
static_assert(offsetof(PlayerActorDataDante, newActorLoopRun) == 0x1CADA);
static_assert(offsetof(PlayerActorDataDante, newAirHikeCount) == 0x1CADB);
static_assert(offsetof(PlayerActorDataDante, newKickJumpCount) == 0x1CADC);
static_assert(offsetof(PlayerActorDataDante, newWallHikeCount) == 0x1CADD);
static_assert(offsetof(PlayerActorDataDante, newDashCount) == 0x1CADE);
static_assert(offsetof(PlayerActorDataDante, newSkyStarCount) == 0x1CADF);
static_assert(offsetof(PlayerActorDataDante, newAirTrickCount) == 0x1CAE0);
static_assert(offsetof(PlayerActorDataDante, newTrickUpCount) == 0x1CAE1);
static_assert(offsetof(PlayerActorDataDante, newTrickDownCount) == 0x1CAE2);
static_assert(offsetof(PlayerActorDataDante, newAirStingerCount) == 0x1CAE3);
static_assert(offsetof(PlayerActorDataDante, newAirRisingSunCount) == 0x1CAE4);
static_assert(offsetof(PlayerActorDataDante, newEffectIndices) == 0x1CAF0);
static_assert(offsetof(PlayerActorDataDante, newLastVar) == 0x1CB20);

struct PlayerActorDataBob : PlayerActorDataBase {
    _(17280);
    uint32 newFirstVar; // 0xB8C0
    _(12);
    ModelData newModelData[6];                           // 0xB8D0
    PhysicsMetadata* newModelPhysicsMetadataPool[7][24]; // 0xE5D0
    Size_32 newModelAllocationData[512];                 // 0xEB10
    RecoveryData newRecoveryData[6];                     // 0x12B10
    uint32 newDevilModels[6];                            // 0x12DB0
    _(8);
    BodyPartData newBodyPartData[7][2]; // 0x12DD0
    ModelData newSubmodelData[11];      // 0x13D90
    bool newSubmodelInit[11];           // 0x19010
    _(5);
    ShadowData newModelShadowData[6];                         // 0x19020
    ShadowData newSubmodelShadowData[11];                     // 0x194A0
    PhysicsMetadata* newDevilModelPhysicsMetadataPool[5][36]; // 0x19CE0
    PhysicsData newDevilSubmodelPhysicsData[10];              // 0x1A280
    PhysicsLinkData newDevilSubmodelPhysicsLinkData[10][4];   // 0x1ABE0
    uint8 newWeapons[10];                                     // 0x1C9E0
    _(6);
    WeaponData* newWeaponDataAddr[10]; // 0x1C9F0
    uint32 newWeaponStatus[10];        // 0x1CA40
    _(8);
    uint32 newWeaponLevels[10]; // 0x1CA70
    _(8);
    float newWeaponTimers[10];    // 0x1CAA0
    bool32 newIsClone;            // 0x1CAC8
    uint8 newPlayerIndex;         // 0x1CACC
    uint8 newCharacterIndex;      // 0x1CACD
    uint8 newEntityIndex;         // 0x1CACE
    bool newForceFiles;           // 0x1CACF
    uint8 newForceFilesCharacter; // 0x1CAD0
    uint8 newForceFilesCostume;   // 0x1CAD1
    bool newEnableVisibility;     // 0x1CAD2
    uint8 newGamepad;             // 0x1CAD3
    byte16 newButtonMask;         // 0x1CAD4
    bool newEnableRightStick;     // 0x1CAD6
    bool newEnableLeftStick;      // 0x1CAD7
    bool newQuickDrive;           // 0x1CAD8
    bool newEnableCollision;      // 0x1CAD9
    bool newActorLoopRun;         // 0x1CADA
    uint8 newAirHikeCount;        // 0x1CADB
    uint8 newKickJumpCount;       // 0x1CADC
    uint8 newWallHikeCount;       // 0x1CADD
    uint8 newDashCount;           // 0x1CADE
    uint8 newSkyStarCount;        // 0x1CADF
    uint8 newAirTrickCount;       // 0x1CAE0
    uint8 newTrickUpCount;        // 0x1CAE1
    uint8 newTrickDownCount;      // 0x1CAE2
    uint8 newAirStingerCount;     // 0x1CAE3
    uint8 newAirRisingSunCount;   // 0x1CAE4
    _(11);
    uint32 newEffectIndices[12]; // 0x1CAF0
    uint32 newLastVar;           // 0x1CB20
};

static_assert(offsetof(PlayerActorDataBob, newFirstVar) == 0xB8C0);
static_assert(offsetof(PlayerActorDataBob, newModelData) == 0xB8D0);
static_assert(offsetof(PlayerActorDataBob, newModelPhysicsMetadataPool) == 0xE5D0);
static_assert(offsetof(PlayerActorDataBob, newModelAllocationData) == 0xEB10);
static_assert(offsetof(PlayerActorDataBob, newRecoveryData) == 0x12B10);
static_assert(offsetof(PlayerActorDataBob, newDevilModels) == 0x12DB0);
static_assert(offsetof(PlayerActorDataBob, newBodyPartData) == 0x12DD0);
static_assert(offsetof(PlayerActorDataBob, newSubmodelData) == 0x13D90);
static_assert(offsetof(PlayerActorDataBob, newSubmodelInit) == 0x19010);
static_assert(offsetof(PlayerActorDataBob, newModelShadowData) == 0x19020);
static_assert(offsetof(PlayerActorDataBob, newSubmodelShadowData) == 0x194A0);
static_assert(offsetof(PlayerActorDataBob, newDevilModelPhysicsMetadataPool) == 0x19CE0);
static_assert(offsetof(PlayerActorDataBob, newDevilSubmodelPhysicsData) == 0x1A280);
static_assert(offsetof(PlayerActorDataBob, newDevilSubmodelPhysicsLinkData) == 0x1ABE0);
static_assert(offsetof(PlayerActorDataBob, newWeapons) == 0x1C9E0);
static_assert(offsetof(PlayerActorDataBob, newWeaponDataAddr) == 0x1C9F0);
static_assert(offsetof(PlayerActorDataBob, newWeaponStatus) == 0x1CA40);
static_assert(offsetof(PlayerActorDataBob, newWeaponLevels) == 0x1CA70);
static_assert(offsetof(PlayerActorDataBob, newWeaponTimers) == 0x1CAA0);
static_assert(offsetof(PlayerActorDataBob, newIsClone) == 0x1CAC8);
static_assert(offsetof(PlayerActorDataBob, newPlayerIndex) == 0x1CACC);
static_assert(offsetof(PlayerActorDataBob, newCharacterIndex) == 0x1CACD);
static_assert(offsetof(PlayerActorDataBob, newEntityIndex) == 0x1CACE);
static_assert(offsetof(PlayerActorDataBob, newForceFiles) == 0x1CACF);
static_assert(offsetof(PlayerActorDataBob, newForceFilesCharacter) == 0x1CAD0);
static_assert(offsetof(PlayerActorDataBob, newForceFilesCostume) == 0x1CAD1);
static_assert(offsetof(PlayerActorDataBob, newEnableVisibility) == 0x1CAD2);
static_assert(offsetof(PlayerActorDataBob, newGamepad) == 0x1CAD3);
static_assert(offsetof(PlayerActorDataBob, newButtonMask) == 0x1CAD4);
static_assert(offsetof(PlayerActorDataBob, newEnableRightStick) == 0x1CAD6);
static_assert(offsetof(PlayerActorDataBob, newEnableLeftStick) == 0x1CAD7);
static_assert(offsetof(PlayerActorDataBob, newQuickDrive) == 0x1CAD8);
static_assert(offsetof(PlayerActorDataBob, newEnableCollision) == 0x1CAD9);
static_assert(offsetof(PlayerActorDataBob, newActorLoopRun) == 0x1CADA);
static_assert(offsetof(PlayerActorDataBob, newAirHikeCount) == 0x1CADB);
static_assert(offsetof(PlayerActorDataBob, newKickJumpCount) == 0x1CADC);
static_assert(offsetof(PlayerActorDataBob, newWallHikeCount) == 0x1CADD);
static_assert(offsetof(PlayerActorDataBob, newDashCount) == 0x1CADE);
static_assert(offsetof(PlayerActorDataBob, newSkyStarCount) == 0x1CADF);
static_assert(offsetof(PlayerActorDataBob, newAirTrickCount) == 0x1CAE0);
static_assert(offsetof(PlayerActorDataBob, newTrickUpCount) == 0x1CAE1);
static_assert(offsetof(PlayerActorDataBob, newTrickDownCount) == 0x1CAE2);
static_assert(offsetof(PlayerActorDataBob, newAirStingerCount) == 0x1CAE3);
static_assert(offsetof(PlayerActorDataBob, newAirRisingSunCount) == 0x1CAE4);
static_assert(offsetof(PlayerActorDataBob, newEffectIndices) == 0x1CAF0);
static_assert(offsetof(PlayerActorDataBob, newLastVar) == 0x1CB20);

struct PlayerActorDataLady : PlayerActorDataBase {
    _(17280);
    uint32 newFirstVar; // 0xB8C0
    _(12);
    ModelData newModelData[6];                           // 0xB8D0
    PhysicsMetadata* newModelPhysicsMetadataPool[7][24]; // 0xE5D0
    Size_32 newModelAllocationData[512];                 // 0xEB10
    RecoveryData newRecoveryData[6];                     // 0x12B10
    uint32 newDevilModels[6];                            // 0x12DB0
    _(8);
    BodyPartData newBodyPartData[7][2]; // 0x12DD0
    ModelData newSubmodelData[11];      // 0x13D90
    bool newSubmodelInit[11];           // 0x19010
    _(5);
    ShadowData newModelShadowData[6];                         // 0x19020
    ShadowData newSubmodelShadowData[11];                     // 0x194A0
    PhysicsMetadata* newDevilModelPhysicsMetadataPool[5][36]; // 0x19CE0
    PhysicsData newDevilSubmodelPhysicsData[10];              // 0x1A280
    PhysicsLinkData newDevilSubmodelPhysicsLinkData[10][4];   // 0x1ABE0
    uint8 newWeapons[10];                                     // 0x1C9E0
    _(6);
    WeaponData* newWeaponDataAddr[10]; // 0x1C9F0
    uint32 newWeaponStatus[10];        // 0x1CA40
    _(8);
    uint32 newWeaponLevels[10]; // 0x1CA70
    _(8);
    float newWeaponTimers[10];    // 0x1CAA0
    bool32 newIsClone;            // 0x1CAC8
    uint8 newPlayerIndex;         // 0x1CACC
    uint8 newCharacterIndex;      // 0x1CACD
    uint8 newEntityIndex;         // 0x1CACE
    bool newForceFiles;           // 0x1CACF
    uint8 newForceFilesCharacter; // 0x1CAD0
    uint8 newForceFilesCostume;   // 0x1CAD1
    bool newEnableVisibility;     // 0x1CAD2
    uint8 newGamepad;             // 0x1CAD3
    byte16 newButtonMask;         // 0x1CAD4
    bool newEnableRightStick;     // 0x1CAD6
    bool newEnableLeftStick;      // 0x1CAD7
    bool newQuickDrive;           // 0x1CAD8
    bool newEnableCollision;      // 0x1CAD9
    bool newActorLoopRun;         // 0x1CADA
    uint8 newAirHikeCount;        // 0x1CADB
    uint8 newKickJumpCount;       // 0x1CADC
    uint8 newWallHikeCount;       // 0x1CADD
    uint8 newDashCount;           // 0x1CADE
    uint8 newSkyStarCount;        // 0x1CADF
    uint8 newAirTrickCount;       // 0x1CAE0
    uint8 newTrickUpCount;        // 0x1CAE1
    uint8 newTrickDownCount;      // 0x1CAE2
    uint8 newAirStingerCount;     // 0x1CAE3
    uint8 newAirRisingSunCount;   // 0x1CAE4
    _(11);
    uint32 newEffectIndices[12]; // 0x1CAF0
    uint32 newLastVar;           // 0x1CB20
};

static_assert(offsetof(PlayerActorDataLady, newFirstVar) == 0xB8C0);
static_assert(offsetof(PlayerActorDataLady, newModelData) == 0xB8D0);
static_assert(offsetof(PlayerActorDataLady, newModelPhysicsMetadataPool) == 0xE5D0);
static_assert(offsetof(PlayerActorDataLady, newModelAllocationData) == 0xEB10);
static_assert(offsetof(PlayerActorDataLady, newRecoveryData) == 0x12B10);
static_assert(offsetof(PlayerActorDataLady, newDevilModels) == 0x12DB0);
static_assert(offsetof(PlayerActorDataLady, newBodyPartData) == 0x12DD0);
static_assert(offsetof(PlayerActorDataLady, newSubmodelData) == 0x13D90);
static_assert(offsetof(PlayerActorDataLady, newSubmodelInit) == 0x19010);
static_assert(offsetof(PlayerActorDataLady, newModelShadowData) == 0x19020);
static_assert(offsetof(PlayerActorDataLady, newSubmodelShadowData) == 0x194A0);
static_assert(offsetof(PlayerActorDataLady, newDevilModelPhysicsMetadataPool) == 0x19CE0);
static_assert(offsetof(PlayerActorDataLady, newDevilSubmodelPhysicsData) == 0x1A280);
static_assert(offsetof(PlayerActorDataLady, newDevilSubmodelPhysicsLinkData) == 0x1ABE0);
static_assert(offsetof(PlayerActorDataLady, newWeapons) == 0x1C9E0);
static_assert(offsetof(PlayerActorDataLady, newWeaponDataAddr) == 0x1C9F0);
static_assert(offsetof(PlayerActorDataLady, newWeaponStatus) == 0x1CA40);
static_assert(offsetof(PlayerActorDataLady, newWeaponLevels) == 0x1CA70);
static_assert(offsetof(PlayerActorDataLady, newWeaponTimers) == 0x1CAA0);
static_assert(offsetof(PlayerActorDataLady, newIsClone) == 0x1CAC8);
static_assert(offsetof(PlayerActorDataLady, newPlayerIndex) == 0x1CACC);
static_assert(offsetof(PlayerActorDataLady, newCharacterIndex) == 0x1CACD);
static_assert(offsetof(PlayerActorDataLady, newEntityIndex) == 0x1CACE);
static_assert(offsetof(PlayerActorDataLady, newForceFiles) == 0x1CACF);
static_assert(offsetof(PlayerActorDataLady, newForceFilesCharacter) == 0x1CAD0);
static_assert(offsetof(PlayerActorDataLady, newForceFilesCostume) == 0x1CAD1);
static_assert(offsetof(PlayerActorDataLady, newEnableVisibility) == 0x1CAD2);
static_assert(offsetof(PlayerActorDataLady, newGamepad) == 0x1CAD3);
static_assert(offsetof(PlayerActorDataLady, newButtonMask) == 0x1CAD4);
static_assert(offsetof(PlayerActorDataLady, newEnableRightStick) == 0x1CAD6);
static_assert(offsetof(PlayerActorDataLady, newEnableLeftStick) == 0x1CAD7);
static_assert(offsetof(PlayerActorDataLady, newQuickDrive) == 0x1CAD8);
static_assert(offsetof(PlayerActorDataLady, newEnableCollision) == 0x1CAD9);
static_assert(offsetof(PlayerActorDataLady, newActorLoopRun) == 0x1CADA);
static_assert(offsetof(PlayerActorDataLady, newAirHikeCount) == 0x1CADB);
static_assert(offsetof(PlayerActorDataLady, newKickJumpCount) == 0x1CADC);
static_assert(offsetof(PlayerActorDataLady, newWallHikeCount) == 0x1CADD);
static_assert(offsetof(PlayerActorDataLady, newDashCount) == 0x1CADE);
static_assert(offsetof(PlayerActorDataLady, newSkyStarCount) == 0x1CADF);
static_assert(offsetof(PlayerActorDataLady, newAirTrickCount) == 0x1CAE0);
static_assert(offsetof(PlayerActorDataLady, newTrickUpCount) == 0x1CAE1);
static_assert(offsetof(PlayerActorDataLady, newTrickDownCount) == 0x1CAE2);
static_assert(offsetof(PlayerActorDataLady, newAirStingerCount) == 0x1CAE3);
static_assert(offsetof(PlayerActorDataLady, newAirRisingSunCount) == 0x1CAE4);
static_assert(offsetof(PlayerActorDataLady, newEffectIndices) == 0x1CAF0);
static_assert(offsetof(PlayerActorDataLady, newLastVar) == 0x1CB20);

struct PlayerActorDataVergil : PlayerActorDataBase {
    ModelData submodelData[5]; // 0x7540
    bool submodelInit[5];      // 0x9AC0
    _(43);
    ShadowData modelShadowData[3];                    // 0x9AF0
    ShadowData submodelShadowData[5];                 // 0x9D30
    PhysicsMetadata* submodelPhysicsMetadataPool[39]; // 0xA0F0
    _(8);
    PhysicsData submodelPhysicsData; // 0xA230
    _(4896);
    ModelMetadata modelMetadata[6]; // 0xB640
    _(160);
    uint32 newFirstVar; // 0xB8C0
    _(12);
    ModelData newModelData[6];                           // 0xB8D0
    PhysicsMetadata* newModelPhysicsMetadataPool[7][24]; // 0xE5D0
    Size_32 newModelAllocationData[512];                 // 0xEB10
    RecoveryData newRecoveryData[6];                     // 0x12B10
    uint32 newDevilModels[6];                            // 0x12DB0
    _(8);
    BodyPartData newBodyPartData[7][2]; // 0x12DD0
    ModelData newSubmodelData[11];      // 0x13D90
    bool newSubmodelInit[11];           // 0x19010
    _(5);
    ShadowData newModelShadowData[6];                         // 0x19020
    ShadowData newSubmodelShadowData[11];                     // 0x194A0
    PhysicsMetadata* newDevilModelPhysicsMetadataPool[5][36]; // 0x19CE0
    PhysicsData newDevilSubmodelPhysicsData[10];              // 0x1A280
    PhysicsLinkData newDevilSubmodelPhysicsLinkData[10][4];   // 0x1ABE0
    uint8 newWeapons[10];                                     // 0x1C9E0
    _(6);
    WeaponData* newWeaponDataAddr[10]; // 0x1C9F0
    uint32 newWeaponStatus[10];        // 0x1CA40
    _(8);
    uint32 newWeaponLevels[10]; // 0x1CA70
    _(8);
    float newWeaponTimers[10];    // 0x1CAA0
    bool32 newIsClone;            // 0x1CAC8
    uint8 newPlayerIndex;         // 0x1CACC
    uint8 newCharacterIndex;      // 0x1CACD
    uint8 newEntityIndex;         // 0x1CACE
    bool newForceFiles;           // 0x1CACF
    uint8 newForceFilesCharacter; // 0x1CAD0
    uint8 newForceFilesCostume;   // 0x1CAD1
    bool newEnableVisibility;     // 0x1CAD2
    uint8 newGamepad;             // 0x1CAD3
    byte16 newButtonMask;         // 0x1CAD4
    bool newEnableRightStick;     // 0x1CAD6
    bool newEnableLeftStick;      // 0x1CAD7
    bool newQuickDrive;           // 0x1CAD8
    bool newEnableCollision;      // 0x1CAD9
    bool newActorLoopRun;         // 0x1CADA
    uint8 newAirHikeCount;        // 0x1CADB
    uint8 newKickJumpCount;       // 0x1CADC
    uint8 newWallHikeCount;       // 0x1CADD
    uint8 newDashCount;           // 0x1CADE
    uint8 newSkyStarCount;        // 0x1CADF
    uint8 newAirTrickCount;       // 0x1CAE0
    uint8 newTrickUpCount;        // 0x1CAE1
    uint8 newTrickDownCount;      // 0x1CAE2
    uint8 newAirStingerCount;     // 0x1CAE3
    uint8 newAirRisingSunCount;   // 0x1CAE4
    _(11);
    uint32 newEffectIndices[12]; // 0x1CAF0
    uint32 newLastVar;           // 0x1CB20
};

static_assert(offsetof(PlayerActorDataVergil, submodelData) == 0x7540);
static_assert(offsetof(PlayerActorDataVergil, submodelInit) == 0x9AC0);
static_assert(offsetof(PlayerActorDataVergil, modelShadowData) == 0x9AF0);
static_assert(offsetof(PlayerActorDataVergil, submodelShadowData) == 0x9D30);
static_assert(offsetof(PlayerActorDataVergil, submodelPhysicsMetadataPool) == 0xA0F0);
static_assert(offsetof(PlayerActorDataVergil, submodelPhysicsData) == 0xA230);
static_assert(offsetof(PlayerActorDataVergil, modelMetadata) == 0xB640);
static_assert(offsetof(PlayerActorDataVergil, newFirstVar) == 0xB8C0);
static_assert(offsetof(PlayerActorDataVergil, newModelData) == 0xB8D0);
static_assert(offsetof(PlayerActorDataVergil, newModelPhysicsMetadataPool) == 0xE5D0);
static_assert(offsetof(PlayerActorDataVergil, newModelAllocationData) == 0xEB10);
static_assert(offsetof(PlayerActorDataVergil, newRecoveryData) == 0x12B10);
static_assert(offsetof(PlayerActorDataVergil, newDevilModels) == 0x12DB0);
static_assert(offsetof(PlayerActorDataVergil, newBodyPartData) == 0x12DD0);
static_assert(offsetof(PlayerActorDataVergil, newSubmodelData) == 0x13D90);
static_assert(offsetof(PlayerActorDataVergil, newSubmodelInit) == 0x19010);
static_assert(offsetof(PlayerActorDataVergil, newModelShadowData) == 0x19020);
static_assert(offsetof(PlayerActorDataVergil, newSubmodelShadowData) == 0x194A0);
static_assert(offsetof(PlayerActorDataVergil, newDevilModelPhysicsMetadataPool) == 0x19CE0);
static_assert(offsetof(PlayerActorDataVergil, newDevilSubmodelPhysicsData) == 0x1A280);
static_assert(offsetof(PlayerActorDataVergil, newDevilSubmodelPhysicsLinkData) == 0x1ABE0);
static_assert(offsetof(PlayerActorDataVergil, newWeapons) == 0x1C9E0);
static_assert(offsetof(PlayerActorDataVergil, newWeaponDataAddr) == 0x1C9F0);
static_assert(offsetof(PlayerActorDataVergil, newWeaponStatus) == 0x1CA40);
static_assert(offsetof(PlayerActorDataVergil, newWeaponLevels) == 0x1CA70);
static_assert(offsetof(PlayerActorDataVergil, newWeaponTimers) == 0x1CAA0);
static_assert(offsetof(PlayerActorDataVergil, newIsClone) == 0x1CAC8);
static_assert(offsetof(PlayerActorDataVergil, newPlayerIndex) == 0x1CACC);
static_assert(offsetof(PlayerActorDataVergil, newCharacterIndex) == 0x1CACD);
static_assert(offsetof(PlayerActorDataVergil, newEntityIndex) == 0x1CACE);
static_assert(offsetof(PlayerActorDataVergil, newForceFiles) == 0x1CACF);
static_assert(offsetof(PlayerActorDataVergil, newForceFilesCharacter) == 0x1CAD0);
static_assert(offsetof(PlayerActorDataVergil, newForceFilesCostume) == 0x1CAD1);
static_assert(offsetof(PlayerActorDataVergil, newEnableVisibility) == 0x1CAD2);
static_assert(offsetof(PlayerActorDataVergil, newGamepad) == 0x1CAD3);
static_assert(offsetof(PlayerActorDataVergil, newButtonMask) == 0x1CAD4);
static_assert(offsetof(PlayerActorDataVergil, newEnableRightStick) == 0x1CAD6);
static_assert(offsetof(PlayerActorDataVergil, newEnableLeftStick) == 0x1CAD7);
static_assert(offsetof(PlayerActorDataVergil, newQuickDrive) == 0x1CAD8);
static_assert(offsetof(PlayerActorDataVergil, newEnableCollision) == 0x1CAD9);
static_assert(offsetof(PlayerActorDataVergil, newActorLoopRun) == 0x1CADA);
static_assert(offsetof(PlayerActorDataVergil, newAirHikeCount) == 0x1CADB);
static_assert(offsetof(PlayerActorDataVergil, newKickJumpCount) == 0x1CADC);
static_assert(offsetof(PlayerActorDataVergil, newWallHikeCount) == 0x1CADD);
static_assert(offsetof(PlayerActorDataVergil, newDashCount) == 0x1CADE);
static_assert(offsetof(PlayerActorDataVergil, newSkyStarCount) == 0x1CADF);
static_assert(offsetof(PlayerActorDataVergil, newAirTrickCount) == 0x1CAE0);
static_assert(offsetof(PlayerActorDataVergil, newTrickUpCount) == 0x1CAE1);
static_assert(offsetof(PlayerActorDataVergil, newTrickDownCount) == 0x1CAE2);
static_assert(offsetof(PlayerActorDataVergil, newAirStingerCount) == 0x1CAE3);
static_assert(offsetof(PlayerActorDataVergil, newAirRisingSunCount) == 0x1CAE4);
static_assert(offsetof(PlayerActorDataVergil, newEffectIndices) == 0x1CAF0);
static_assert(offsetof(PlayerActorDataVergil, newLastVar) == 0x1CB20);


// float maxHitPointsDullahan; // 0x238
// float hitPointsDullahan; // 0x2478
// 
// float maxHitPointsChess; //0x530
// float hitPointsChess; // 0x1318
// 
// float maxHitPointsTheFallen; // 0x1840
// float hitPointsTheFallen; // 0x1808
// 
// float maxHitPointsBloodgoyle; // 0x643
// float hitPointsBloodgoyle; // 0x1498
// 
// float maxHitPointsSoulEater; // 0x628
// float hitPointsSoulEater; // 0x1A18
// 
// float maxHitPointsEnigma; // 0x3B14
// float hitPointsEnigma; // 0x3958
// 
// float maxHitPointsArachne; // 0x40EC
// float hitPointsArachne; // 0x4170
// 
// // BOSSES
// 
// float hitPointsCerberusPart1; // 0xE230 -- max cerberus part1 hp is always 2400
// float hitPointsCerberusPart2Head1Red; // 0xE234
// float hitPointsCerberusPart2Head2Green; // 0xE238
// float hitPointsCerberusPart2Head3Blue; // 0xE23C
// float hitPointsCerberusTotal; // 0xE240 -- max cerberus hp is always 7200
// float maxHitPointsCerberusTotal; // 0x352C84
// float maxHitPointsCerberusPart1; // 0x3637D0 // or dmc3.exe + 5728F0
// 
// float maxHitPointsAgniRudra; // 0xFEC
// float hitPointsAgniRudra; // 0x101C
// 
// float maxHitPointsVergil; // 0xED20
// float hitPointsVergil; // 0xEB28
// 
// float maxHitPointsNevan; // 0x69C0
// float hitPointsNevan; // 0x68D8
// 
// float maxHitPointsJester; // 0x61E0
// float hitPointsJester; // 0x5F98
// 
// float maxHitPointsGeryon; // 0xFB0
// float hitPointsGeryon; // 0xFC8
// float maxHitPointsGeryonCarriage; // 0x16484
// float hitPointsGeryonCarriage; // 0x1648C // detect whether aiming at carriage or horse by checking if this is 0
// 
// float maxHitPointsBeowulf; // 0x45F0
// float hitPointsBeowulf; // 0x4338
// 
// float maxHitPointsLady; // 0x5950 
// float hitPointsLady; // 0x5758
// 
// float maxHitPointsDoppelganger; // 0x81C
// float hitPointsDoppelganger; // 0x84C
// 
// float maxHitPointsLeviathan; // 0x388884
// float hitPointsLeviathan; // 0xAF4
// 
// float hitPointsLeviathanOther; // 0xA78 // left one max HP is 2000 and right one max hp is 1500, couldn't find a good addr that works for both
// 
// float maxHitPointsArkham; // 0x2788
// float hitPointsArkham; // 0x1AF4
// 
// float hitPointsArkhamLeech; // 0xE68
// float maxHitPointsArkhamLeech; // 0x12A0
// 
// float maxHitPointsGigapede; // 0x9584
// float hitPointsGigapede; // 0x9BC0
// byte32 gigapedePartAddr; // 0xF68 // check if this is null, if isn't then it's a gigapede part and we must fetch using +95E4 / +9B60 from the gigapedePartAddr

//39A0 is greed TargetPos
// 3A10 is abyss targetPos
// 39F0 is envy targetPos

struct EnemyActorData : ActorDataBase {
	_(248);
    vec4 targetPositionDullahan; // 0x1C0
    _(104);
	float maxHitPointsDullahan; // 0x238
	_(756);
	float maxHitPointsChess; // 0x530
	_(220);
	float maxHitPointsBloodgoyle; // 0x610
	_(92);
    float maxHitPointsSoulEater; // 0x670
	_(424);
	float maxHitPointsDoppelganger; // 0x81C
	_(44);
	float hitPointsDoppelganger; // 0x84C
	_(552);
	float hitPointsLeviathanOther; // 0xA78 // left one max HP is 2000 and right one max hp is 1500, couldn't find a good addr that works for both
	_(120);
	float hitPointsLeviathan; // 0xAF4
	_(880);
	float hitPointsArkhamLeech; // 0xE68
	_(252);
	byte8* gigapedePartAddr; // 0xF68 // check if this is null, if isn't then it's a gigapede part and we must fetch using +95E4 / +9B60 from the gigapedePartAddr
	_(64);
	float maxHitPointsGeryon; // 0xFB0
	_(20);
	float hitPointsGeryon; // 0xFC8
	_(32);
	float maxHitPointsAgniRudra; // 0xFEC
	_(44);
	float hitPointsAgniRudra; // 0x101C
	_(640);
	float maxHitPointsArkhamLeech; // 0x12A0
	_(116);
	float hitPointsChess; // 0x1318
	_(380);
	float hitPointsBloodgoyle; // 0x1498
	_(876);
	float hitPointsTheFallen; // 0x1808
	_(52);
	float maxHitPointsTheFallen; // 0x1840
	_(468);
	float hitPointsSoulEater; // 0x1A18
	_(2652);
	float hitPointsDullahan; // 0x2478
	_(556);
	float hitPointsArkham; // 0x26A8
	_(220);
	float maxHitPointsArkham; // 0x2788
	_(1696);
	float maxHitPointsHells; // 0x2E2C
	_(44);
	float hitPointsHells; // 0x2E5C
	_(1208);
	byte8* stunDisplacementDataAddr; // 0x3318 // +10 from this addr is the stun displacement hells Struct
    _(1592);
	float hitPointsEnigma; // 0x3958
	_(36);
	vec4 targetPosition; // 0x3980 - needs to sum with 60 from ActorDataBase
	_(16);
	vec4 targetPositionGreed; // 0x39A0
    _(64);
	vec4 targetPositionEnvy; // 0x39F0
    _(16);
	vec4 targetPositionAbyss; // 0x3A10
    _(244);
	float maxHitPointsEnigma; // 0x3B14
	_(1492);
    float maxHitPointsArachne; // 0x40EC
    _(128);
    float hitPointsArachne; // 0x4170
	_(452);
	float hitPointsBeowulf; // 0x4338
	_(692);
	float maxHitPointsBeowulf; // 0x45F0
	_(4452);
	float hitPointsLady; // 0x5758
	_(500);
	float maxHitPointsLady; // 0x5950
	_(1604);
	float hitPointsJester; // 0x5F98
	_(580);
	float maxHitPointsJester; // 0x61E0
	_(812);
    byte8* shieldedNevanAddr; // 0x6510 // +224 from this addr is currentShield amount and +A24 is maxShield amount
    _(960);
	float hitPointsNevan; // 0x68D8
	_(228);
	float maxHitPointsNevan; // 0x69C0
    _(7388);
    vec4 targetPositionHellVanguard; // 0x86A0
	_(3892);
	float maxHitPointsGigapede; // 0x95E4
	_(1496);
	float hitPointsGigapede; // 0x9BC0
	_(18028);
	float hitPointsCerberusPart1; // 0xE230 // max cerberus part1 hp is always 2400
	float hitPointsCerberusPart2Head1Red; // 0xE234 
	float hitPointsCerberusPart2Head2Green; // 0xE238
	float hitPointsCerberusPart2Head3Blue; // 0xE23C
	float hitPointsCerberusTotal; // 0xE240 // max cerberus hp is always 7200
	_(2276);
	float hitPointsVergil; // 0xEB28
	_(500);
	float maxHitPointsVergil; // 0xED20
	_(30560);
	float maxHitPointsGeryonCarriage; // 0x16484
	_(4);
	float hitPointsGeryonCarriage; // 0x1648C // detect whether aiming at carriage or horse by checking if this is 0
	_(3393524);
	float maxHitPointsCerberusTotal; // 0x352C84
	_(68424);
	float maxHitPointsCerberusPart1; // 0x3637D0 // or dmc3.exe + 5728F0
	_(151728);
	float maxHitPointsLeviathan; // 0x388884
};

static_assert(offsetof(EnemyActorData, targetPositionDullahan) == 0x1C0);
static_assert(offsetof(EnemyActorData, maxHitPointsDullahan) == 0x238);
static_assert(offsetof(EnemyActorData, maxHitPointsChess) == 0x530);
static_assert(offsetof(EnemyActorData, maxHitPointsBloodgoyle) == 0x610);
static_assert(offsetof(EnemyActorData, maxHitPointsSoulEater) == 0x670);
static_assert(offsetof(EnemyActorData, maxHitPointsDoppelganger) == 0x81C);
static_assert(offsetof(EnemyActorData, hitPointsDoppelganger) == 0x84C);
static_assert(offsetof(EnemyActorData, hitPointsLeviathanOther) == 0xA78);
static_assert(offsetof(EnemyActorData, hitPointsLeviathan) == 0xAF4);
static_assert(offsetof(EnemyActorData, hitPointsArkhamLeech) == 0xE68);
static_assert(offsetof(EnemyActorData, gigapedePartAddr) == 0xF68);
static_assert(offsetof(EnemyActorData, maxHitPointsGeryon) == 0xFB0);
static_assert(offsetof(EnemyActorData, hitPointsGeryon) == 0xFC8);
static_assert(offsetof(EnemyActorData, maxHitPointsAgniRudra) == 0xFEC);
static_assert(offsetof(EnemyActorData, hitPointsAgniRudra) == 0x101C);
static_assert(offsetof(EnemyActorData, maxHitPointsArkhamLeech) == 0x12A0);
static_assert(offsetof(EnemyActorData, hitPointsChess) == 0x1318);
static_assert(offsetof(EnemyActorData, hitPointsBloodgoyle) == 0x1498);
static_assert(offsetof(EnemyActorData, hitPointsTheFallen) == 0x1808);
static_assert(offsetof(EnemyActorData, maxHitPointsTheFallen) == 0x1840);
static_assert(offsetof(EnemyActorData, hitPointsSoulEater) == 0x1A18);
static_assert(offsetof(EnemyActorData, hitPointsDullahan) == 0x2478);
static_assert(offsetof(EnemyActorData, hitPointsArkham) == 0x26A8);
static_assert(offsetof(EnemyActorData, maxHitPointsArkham) == 0x2788);
static_assert(offsetof(EnemyActorData, maxHitPointsHells) == 0x2E2C);
static_assert(offsetof(EnemyActorData, stunDisplacementDataAddr) == 0x3318); 
static_assert(offsetof(EnemyActorData, hitPointsHells) == 0x2E5C);
static_assert(offsetof(EnemyActorData, hitPointsEnigma) == 0x3958);
static_assert(offsetof(EnemyActorData, targetPosition) == 0x3980);
static_assert(offsetof(EnemyActorData, targetPositionGreed) == 0x39A0);
static_assert(offsetof(EnemyActorData, targetPositionEnvy) == 0x39F0);
static_assert(offsetof(EnemyActorData, targetPositionAbyss) == 0x3A10);
static_assert(offsetof(EnemyActorData, maxHitPointsEnigma) == 0x3B14);
static_assert(offsetof(EnemyActorData, maxHitPointsArachne) == 0x40EC);
static_assert(offsetof(EnemyActorData, hitPointsArachne) == 0x4170);
static_assert(offsetof(EnemyActorData, hitPointsBeowulf) == 0x4338);
static_assert(offsetof(EnemyActorData, maxHitPointsBeowulf) == 0x45F0);
static_assert(offsetof(EnemyActorData, hitPointsLady) == 0x5758);
static_assert(offsetof(EnemyActorData, maxHitPointsLady) == 0x5950);
static_assert(offsetof(EnemyActorData, hitPointsJester) == 0x5F98);
static_assert(offsetof(EnemyActorData, maxHitPointsJester) == 0x61E0);
static_assert(offsetof(EnemyActorData, shieldedNevanAddr) == 0x6510);
static_assert(offsetof(EnemyActorData, hitPointsNevan) == 0x68D8);
static_assert(offsetof(EnemyActorData, maxHitPointsNevan) == 0x69C0);
static_assert(offsetof(EnemyActorData, targetPositionHellVanguard) == 0x86A0);
static_assert(offsetof(EnemyActorData, maxHitPointsGigapede) == 0x95E4);
static_assert(offsetof(EnemyActorData, hitPointsGigapede) == 0x9BC0);
static_assert(offsetof(EnemyActorData, hitPointsCerberusPart1) == 0xE230);
static_assert(offsetof(EnemyActorData, hitPointsCerberusPart2Head1Red) == 0xE234);
static_assert(offsetof(EnemyActorData, hitPointsCerberusPart2Head2Green) == 0xE238);
static_assert(offsetof(EnemyActorData, hitPointsCerberusPart2Head3Blue) == 0xE23C);
static_assert(offsetof(EnemyActorData, hitPointsCerberusTotal) == 0xE240);
static_assert(offsetof(EnemyActorData, hitPointsVergil) == 0xEB28);
static_assert(offsetof(EnemyActorData, maxHitPointsVergil) == 0xED20);
static_assert(offsetof(EnemyActorData, maxHitPointsGeryonCarriage) == 0x16484);
static_assert(offsetof(EnemyActorData, hitPointsGeryonCarriage) == 0x1648C);
static_assert(offsetof(EnemyActorData, maxHitPointsCerberusTotal) == 0x352C84);
static_assert(offsetof(EnemyActorData, maxHitPointsCerberusPart1) == 0x3637D0);
static_assert(offsetof(EnemyActorData, maxHitPointsLeviathan) == 0x388884);

struct StunDisplacementHells {
	_(12);
	float displacement; // 0xC
	float stun; // 0x10
};

static_assert(offsetof(StunDisplacementHells, displacement) == 0xC);
static_assert(offsetof(StunDisplacementHells, stun) == 0x10);

struct StunDisplacementData {
    _(16);
    StunDisplacementHells* stunDisplacementHells; // 0x10
};

static_assert(offsetof(StunDisplacementData, stunDisplacementHells) == 0x10);


struct ShieldedNevanData {
    _(548);
	float currentShield; // 0x224
    _(2044);
	float maxShield; // 0xA24
};

static_assert(offsetof(ShieldedNevanData, currentShield) == 0x224);
static_assert(offsetof(ShieldedNevanData, maxShield) == 0xA24);

struct EnemyActorDataPride : ActorDataBase {
    _(11620);
    float maxHitPoints; // 0x2E2C
    _(44);
    float hitPoints; // 0x2E5C
    _(3028);
    uint32 event; // 0x3A34
    uint8 state;  // 0x3A38
};

static_assert(offsetof(EnemyActorDataPride, maxHitPoints) == 0x2E2C);
static_assert(offsetof(EnemyActorDataPride, hitPoints) == 0x2E5C);
static_assert(offsetof(EnemyActorDataPride, event) == 0x3A34);
static_assert(offsetof(EnemyActorDataPride, state) == 0x3A38);

struct EnemyActorDataLady : ActorDataBase {
    _(21400);
    CollisionData collisionData; // 0x5460
    _(104);
    float hitPoints; // 0x5758
    _(556);
    uint32 event; // 0x5988
    _(8);
    uint8 state; // 0x5994
    _(11);
    uint32 pistolBullets; // 0x59A0
    _(12);
    uint32 grenades; // 0x59B0
    _(4);
    uint32 smgBullets; // 0x59B8
    _(4);
    vec4 nextPosition; // 0x59C0
    _(76);
    uint32 phase;     // 0x5A1C
    uint16 rotation2; // 0x5A20
    _(12);
    bool friendly; // 0x5A2E
    _(97);
    uint32 pistolRounds; // 0x5A90
};

static_assert(offsetof(EnemyActorDataLady, collisionData) == 0x5460);
static_assert(offsetof(EnemyActorDataLady, hitPoints) == 0x5758);
static_assert(offsetof(EnemyActorDataLady, event) == 0x5988);
static_assert(offsetof(EnemyActorDataLady, state) == 0x5994);
static_assert(offsetof(EnemyActorDataLady, pistolBullets) == 0x59A0);
static_assert(offsetof(EnemyActorDataLady, grenades) == 0x59B0);
static_assert(offsetof(EnemyActorDataLady, smgBullets) == 0x59B8);
static_assert(offsetof(EnemyActorDataLady, nextPosition) == 0x59C0);
static_assert(offsetof(EnemyActorDataLady, phase) == 0x5A1C);
static_assert(offsetof(EnemyActorDataLady, rotation2) == 0x5A20);
static_assert(offsetof(EnemyActorDataLady, friendly) == 0x5A2E);
static_assert(offsetof(EnemyActorDataLady, pistolRounds) == 0x5A90);

struct EnemyActorDataVergil : ActorDataBase {
    _(272);
    float nextEventTimer; // 0x1D8
    _(58964);
    CollisionData collisionData; // 0xE830
    _(104);
    float hitPoints; // 0xEB28
    _(584);
    uint32 event; // 0xED74
    _(8);
    uint32 var_ED80; // 0xED80
    uint8 state;     // 0xED84
    _(391);
    uint32 variant; // 0xEF0C
    _(9);
    bool toggleDevil; // 0xEF19
    _(1);
    bool devil; // 0xEF1B
};

static_assert(offsetof(EnemyActorDataVergil, nextEventTimer) == 0x1D8);
static_assert(offsetof(EnemyActorDataVergil, collisionData) == 0xE830);
static_assert(offsetof(EnemyActorDataVergil, hitPoints) == 0xEB28);
static_assert(offsetof(EnemyActorDataVergil, event) == 0xED74);
static_assert(offsetof(EnemyActorDataVergil, var_ED80) == 0xED80);
static_assert(offsetof(EnemyActorDataVergil, state) == 0xED84);
static_assert(offsetof(EnemyActorDataVergil, variant) == 0xEF0C);
static_assert(offsetof(EnemyActorDataVergil, toggleDevil) == 0xEF19);
static_assert(offsetof(EnemyActorDataVergil, devil) == 0xEF1B);

// $ActorDataEnd

struct ConfigCreateEnemyActorData {
    uint32 enemy;
    uint32 variant;
    vec4 position;
    uint16 rotation;
    bool useMainActorData = true;
    uint16 spawnMethod;
};

// $CreateEnemyActorDataStart

struct CreateEnemyActorData {
    uint32 enemy; // 0
    _(12);
    vec4 position;      // 0x10
    uint16 rotation;    // 0x20
    uint16 spawnMethod; // 0x22
    _(4);
    uint32 variant; // 0x28
    _(84);
};

static_assert(offsetof(CreateEnemyActorData, enemy) == 0);
static_assert(offsetof(CreateEnemyActorData, position) == 0x10);
static_assert(offsetof(CreateEnemyActorData, rotation) == 0x20);
static_assert(offsetof(CreateEnemyActorData, spawnMethod) == 0x22);
static_assert(offsetof(CreateEnemyActorData, variant) == 0x28);

static_assert(sizeof(CreateEnemyActorData) == 128);

// $CreateEnemyActorDataEnd

struct EnemyVectorDataMetadata {
    byte8* baseAddr;
    union {
        EnemyVectorDataMetadata* lastAddr;
        EnemyVectorDataMetadata* nextAddr;
    };
};

static_assert(sizeof(EnemyVectorDataMetadata) == 0x10);

// $EnemyVectorDataStart

struct EnemyVectorData {
    _(40);
    uint32 count; // 0x28
    _(28);
    EnemyVectorDataMetadata metadata[50]; // 0x48
    _(3296);
    EnemyVectorDataMetadata* lastMetadataAddr; // 0x1048
    EnemyVectorDataMetadata* nextMetadataAddr; // 0x1050
};

static_assert(offsetof(EnemyVectorData, count) == 0x28);
static_assert(offsetof(EnemyVectorData, metadata) == 0x48);
static_assert(offsetof(EnemyVectorData, lastMetadataAddr) == 0x1048);
static_assert(offsetof(EnemyVectorData, nextMetadataAddr) == 0x1050);

static_assert(sizeof(EnemyVectorData) == 4184);

struct HUDData {
	_(26904);
	float topLeftAlphaTimer; // 0x6918
    _(4);
    uint8 topLeftAlpha; // 0x6920
    _(23);
    float orbsCountAlphaTimer; // 0x6938
    uint8 orbsCountAlpha; // 0x693C
};

static_assert(offsetof(HUDData, topLeftAlphaTimer) == 0x6918);
static_assert(offsetof(HUDData, topLeftAlpha) == 0x6920);
static_assert(offsetof(HUDData, orbsCountAlphaTimer) == 0x6938);
static_assert(offsetof(HUDData, orbsCountAlpha) == 0x693C);


// $EnemyVectorDataEnd

// struct ExperienceData
// {


// 	Data Dante;
// 	Data Vergil;
// };

#pragma pack(pop)

#undef _

#pragma endregion

#pragma region Helpers

struct MotionArchiveHelper {
    uint8 group;
    uint16 cacheFileId;
};

constexpr MotionArchiveHelper motionArchiveHelperDante[] = {
    {MOTION_GROUP_DANTE::BASE, pl000_00_0},
    {MOTION_GROUP_DANTE::DAMAGE, pl000_00_1},
    {MOTION_GROUP_DANTE::TAUNTS, pl000_00_2},
    {MOTION_GROUP_DANTE::REBELLION, pl000_00_3},
    {MOTION_GROUP_DANTE::CERBERUS, pl000_00_4},
    {MOTION_GROUP_DANTE::AGNI_RUDRA, pl000_00_5},
    {MOTION_GROUP_DANTE::NEVAN, pl000_00_6},
    {MOTION_GROUP_DANTE::BEOWULF, pl000_00_7},
    {MOTION_GROUP_DANTE::EBONY_IVORY, pl000_00_8},
    {MOTION_GROUP_DANTE::SHOTGUN, pl000_00_9},
    {MOTION_GROUP_DANTE::ARTEMIS, pl000_00_10},
    {MOTION_GROUP_DANTE::SPIRAL, pl000_00_11},
    {MOTION_GROUP_DANTE::KALINA_ANN, pl000_00_12},
    {MOTION_GROUP_DANTE::SWORDMASTER_REBELLION, pl000_00_13},
    {MOTION_GROUP_DANTE::SWORDMASTER_CERBERUS, pl000_00_14},
    {MOTION_GROUP_DANTE::SWORDMASTER_AGNI_RUDRA, pl000_00_15},
    {MOTION_GROUP_DANTE::SWORDMASTER_NEVAN, pl000_00_16},
    {MOTION_GROUP_DANTE::SWORDMASTER_BEOWULF, pl000_00_17},
    {MOTION_GROUP_DANTE::GUNSLINGER_EBONY_IVORY, pl000_00_18},
    {MOTION_GROUP_DANTE::GUNSLINGER_SHOTGUN, pl000_00_19},
    {MOTION_GROUP_DANTE::GUNSLINGER_ARTEMIS, pl000_00_20},
    {MOTION_GROUP_DANTE::GUNSLINGER_SPIRAL, pl000_00_21},
    {MOTION_GROUP_DANTE::GUNSLINGER_KALINA_ANN, pl000_00_22},
    {MOTION_GROUP_DANTE::TRICKSTER, pl000_00_23},
    {MOTION_GROUP_DANTE::ROYALGUARD, pl000_00_24},
    {MOTION_GROUP_DANTE::QUICKSILVER, pl000_00_25},
    {MOTION_GROUP_DANTE::DOPPELGANGER, pl000_00_26},
};

constexpr MotionArchiveHelper motionArchiveHelperBob[] = {
    {MOTION_GROUP_BOB::BASE, pl001_00_0},
    {MOTION_GROUP_BOB::DAMAGE, pl001_00_1},
    {MOTION_GROUP_BOB::TAUNTS, pl001_00_2},
    {MOTION_GROUP_BOB::YAMATO, pl001_00_31},
};

constexpr MotionArchiveHelper motionArchiveHelperLady[] = {
    {MOTION_GROUP_LADY::BASE, pl002_00_0},
    {MOTION_GROUP_LADY::DAMAGE, pl002_00_1},
    {MOTION_GROUP_LADY::TAUNTS, pl002_00_2},
};

constexpr MotionArchiveHelper motionArchiveHelperVergil[] = {
    {MOTION_GROUP_VERGIL::BASE, pl021_00_0},
    {MOTION_GROUP_VERGIL::DAMAGE, pl021_00_1},
    {MOTION_GROUP_VERGIL::TAUNTS, pl021_00_2},
    {MOTION_GROUP_VERGIL::YAMATO, pl021_00_3},
    {MOTION_GROUP_VERGIL::BEOWULF, pl021_00_4},
    {MOTION_GROUP_VERGIL::YAMATO_FORCE_EDGE, pl021_00_5},
    {MOTION_GROUP_VERGIL::DARK_SLAYER, pl021_00_6},
    {MOTION_GROUP_VERGIL::NERO_ANGELO_YAMATO, pl021_00_7},
    {MOTION_GROUP_VERGIL::NERO_ANGELO_BEOWULF, pl021_00_8},
    {MOTION_GROUP_VERGIL::NERO_ANGELO_FORCE_EDGE, pl021_00_9},
};

struct MissionStartHelper {
    uint16 room;
    uint16 position;
};

constexpr MissionStartHelper missionStartHelpers[] = {
    {0, 0},   // Movie
    {0, 0},   // Mission 1
    {1, 0},   // Mission 2
    {2, 0},   // Mission 3
    {100, 0}, // Mission 4
    {111, 2}, // Mission 5
    {122, 0}, // Mission 6
    {127, 2}, // Mission 7
    {300, 0}, // Mission 8
    {201, 0}, // Mission 9
    {209, 2}, // Mission 10
    {212, 1}, // Mission 11
    {217, 1}, // Mission 12
    {229, 0}, // Mission 13
    {237, 1}, // Mission 14
    {222, 1}, // Mission 15
    {106, 0}, // Mission 16
    {133, 1}, // Mission 17
    {400, 0}, // Mission 18
    {406, 0}, // Mission 19
    {411, 0}, // Mission 20
    {423, 0}, // Bloody Palace
};

struct FloorHelper {
    uint16 room;
    uint16 position;
};

constexpr FloorHelper floorHelpers[] = {
    {423, 0}, // Floor 1
    {424, 0}, // Floor 2
    {425, 0}, // Floor 3
    {426, 0}, // Floor 4
    {427, 0}, // Floor 5
    {428, 0}, // Floor 6
    {429, 0}, // Floor 7
    {430, 0}, // Floor 8
    {431, 0}, // Floor 9
    {432, 0}, // Floor 10
    {433, 0}, // Cerberus
    {434, 0}, // Gigapede
    {435, 0}, // Agni & Rudra
    {436, 0}, // Nevan
    {437, 0}, // Beowulf
    {438, 2}, // Geryon
    {439, 0}, // Doppelganger
    {440, 0}, // Heart of Leviathan
    {441, 0}, // Damned Chessmen
    {442, 0}, // Vergil 1
    {443, 0}, // Vergil 2
    {444, 0}, // Vergil 3
    {445, 2}, // Lady
    {446, 0}, // Arkham
    {422, 0}, // Jester 1
    {448, 0}, // Jester 2
    {449, 0}, // Jester 3
};

namespace BOSS {
enum {
    CERBERUS,
    GIGAPEDE,
    JESTER_1,
    AGNI_RUDRA,
    VERGIL_1,
    LEVIATHAN,
    NEVAN,
    BEOWULF,
    JESTER_2,
    GERYON_PART_1,
    GERYON_PART_2,
    VERGIL_2,
    LADY,
    JESTER_3,
    DOPPELGANGER,
    TAIZAI_REBORN,
    CERBERUS_REBORN,
    GIGAPEDE_REBORN,
    AGNI_RUDRA_REBORN,
    LEVIATHAN_REBORN,
    NEVAN_REBORN,
    BEOWULF_REBORN,
    GERYON_REBORN,
    DOPPELGANGER_REBORN,
    ARKHAM_PART_1,
    ARKHAM_PART_2,
    VERGIL_3,
};
};

struct BossHelper {
    uint16 room;
    uint16 position;
    const char* track;
};

struct ColorPresets {
    struct StyleSwitchFlux {
		uint8 colorfulSubtle[7][4] = {
			// r   g  b  a 
			{ 29, 29, 0, 255 }, //trick  
			{ 26, 0, 0, 255 }, //sword  
			{ 0, 8, 34, 255 }, //gun    
			{ 0, 35, 6, 255 }, //royal  
			{ 26, 0, 35, 255 }, //quick  
			{ 30, 14, 0, 255 }, //doppel 
            { 0, 25, 30, 255 } // vergil
		};

		uint8 dMC3Switch[7][4] = {
			// r   g  b  a 
			{ 55, 58, 6, 255 }, //trick  
			{ 58, 5, 5, 255 }, //sword  
			{ 13, 5, 58, 255 }, //gun    
			{ 5, 58, 12, 255 }, //royal  
			{ 58, 5, 49, 255 }, //quick  
			{ 58, 28, 5, 255 }, //doppel 
            { 5, 57, 58, 255 }, //vergil
		};

		uint8 allRed[7][4] = {
			// r   g  b  a 
			{ 29, 0, 0, 255 }, //trick  
			{ 29, 0, 0, 255 }, //sword  
			{ 29, 0, 0, 255 }, //gun    
			{ 29, 0, 0, 255 }, //royal  
			{ 29, 0, 0, 255 }, //quick  
			{ 29, 0, 0, 255 }, //doppel 
            { 0, 25, 30, 255 } // vergil
		};
    } StyleSwitchFlux;

    struct StyleSwitchText {
		uint8 midnight[9][4] = {
			{ 155, 85, 250, 255 }, //trick  
			{ 155, 85, 250, 255 }, //sword  
			{ 155, 85, 250, 255 }, //gun    
			{ 155, 85, 250, 255 }, //royal  
			{ 155, 85, 250, 255 }, //quick  
			{ 155, 85, 250, 255 }, //doppel 
			{ 155, 85, 250, 255 }, //dt     
			{ 155, 85, 250, 255 }, //dte    
			{ 155, 85, 250, 255 }, //ready  
		};

		uint8 allWhite[9][4] = {
			{ 255, 255, 255, 255 }, //trick  
			{ 255, 255, 255, 255 }, //sword  
			{ 255, 255, 255, 255 }, //gun    
			{ 255, 255, 255, 255 }, //royal  
			{ 255, 255, 255, 255 }, //quick  
			{ 255, 255, 255, 255 }, //doppel 
			{ 255, 255, 255, 255 }, //dt     
			{ 255, 255, 255, 255 }, //dte    
			{ 255, 255, 255, 255 }, //ready  
		};

		uint8 colorfulClear[9][4] = {
			// r   g  b  a  // though alpha is used in maxAlpha instead
			{ 255, 255, 176, 255 }, //trick  
			{ 248, 153, 153, 255 }, //sword  
			{ 168, 186, 246, 255 }, //gun    
			{ 190, 252, 200, 200 }, //royal  
			{ 237, 195, 252, 255 }, //quick  
			{ 252, 226, 203, 255 }, //doppel 
			{ 255, 255, 255, 255 }, //dt     
			{ 255, 255, 255, 255 }, //dte    
			{ 255, 255, 255, 255 }, //ready  
		};

		uint8 colorful[9][4] = {
			{ 240, 240, 0, 255 }, //trick  
			{ 255, 1, 1, 255 }, //sword  
			{ 0, 56, 239, 255 }, //gun    
			{ 5, 250, 47, 255 }, //royal  
			{ 189, 0, 255, 255 }, //quick  
			{ 255, 121, 4, 255 }, //doppel 
			{ 255, 255, 255, 255 }, //dt     
			{ 255, 255, 255, 255 }, //dte    
			{ 255, 255, 255, 255 }, //ready  
		};
    } StyleSwitchText;

	struct ColorCrimson {
		uint8 airHike[5][4] = {
			{128, 0, 16, 200},
			{128, 0, 16, 200},
			{128, 0, 16, 200},
			{128, 0, 16, 200},
			{128, 0, 16, 200},
		};

		struct  {
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
	} ColorCrimson;

	struct ColorDMC3Default {
		uint8 airHike[5][4] = {
			{128, 0, 0, 200},
			{96, 128, 144, 200},
			{160, 64, 16, 200},
			{112, 64, 160, 200},
			{128, 128, 128, 200},
		};

		struct {
			uint8 skyStar[4] = { 255, 0, 0, 200 };
		} Trickster;
		struct {
			uint8 ultimate[4] = { 143, 112, 48, 200 };
		} Royalguard;
		struct {
			uint8 clone[4] = { 16, 16, 16, 48 };
		} Doppelganger;
		struct {
			uint8 dante[5][4] = {
				{128, 0, 0, 200},
				{96, 128, 144, 200},
				{160, 64, 16, 200},
				{112, 64, 160, 200},
				{128, 128, 128, 200},
			};
			uint8 sparda[4] = { 128, 0, 0, 200 };
            uint8 vergil[3][4] = {
				{32, 64, 128, 200},
				{32, 64, 128, 200},
				{32, 64, 128, 200},
			};
			uint8 neroAngelo[4] = { 64, 0, 255, 200 };
		} Aura;
	} ColorDMC3Default;

    struct LockOnColors {
		uint32 Dante = 0xDE1C4CFF;
		uint32 Vergil = 0x0C85C5FF;
    } LockOnColors;
};

extern ColorPresets colorPresets;

struct Toggle {
    int disableHeightRestriction  = 2;
    int improvedBufferedReversals = 2;
    int increasedJCSpheres        = 2;
    int disableAirSlashKnockback  = 2;
    int cameraSensitivity         = 5;
    int disableJCRestriction      = 2;
    int bulletStop                = 2;
    int rainstormLift             = 2;
    int inertiaFixes              = 2;
    int disableDriveHold          = 2;
};

extern Toggle toggle;

extern bool g_disableRightStickCenterCamera;

struct CrimsonHudData {
    volatile uint16_t redOrbAlpha = 0; // goes up to 127
    float redOrbAlphaDurationToAlpha = 200.0f;
    std::vector<ImVec2> playerOutViewIconPositions = std::vector<ImVec2>(PLAYER_COUNT, ImVec2(0.0f, 0.0f));
};

extern CrimsonHudData crimsonHud;

struct CheatsPopUpData {
    const char* cheatText = "Toggled Infinite HP On";
    float popupTime = 2.0f;
    bool showPopUp = false;
};

extern CheatsPopUpData cheatsPopUp;

namespace DEVILARMUNLOCKS {
enum {
	CERBERUS,
	AGNI_RUDRA,
	NEVAN,
	BEOWULF,
	COUNT 
};
};

namespace GUNUNLOCKS {
enum {
	SHOTGUN,
	ARTEMIS,
	SPIRAL,
	KALINA_ANN,
	COUNT 
};
};

struct WeaponProgressionData {
    uint8 devilArmsUnlockedQtt = 1;
	uint8 gunsUnlockedQtt = 1;
    bool devilArmUnlocks[DEVILARMUNLOCKS::COUNT] = { false };
    bool gunUnlocks[GUNUNLOCKS::COUNT] = { false };
	std::vector<std::string> meleeWeaponNames = { "Rebellion" };
	std::vector<uint8> meleeWeaponIds = { WEAPON::REBELLION };
    std::vector<std::string> rangedWeaponNames = { "Ebony & Ivory" };
    std::vector<uint8> rangedWeaponIds = { WEAPON::EBONY_IVORY };
};

extern WeaponProgressionData weaponProgression;

namespace CHEATS {
enum {
	TRAINING,
	DAMAGE,
	SPEED,
	MOBILITY,
    DEBUG,
    RMS,
	COUNT
};
}

namespace LDKMODE {
enum {
	OFF,
	LDK,
	SUPER_LDK,
	SUPER_LDK_BOSSES,
};
}

namespace ENEMYDTMODE {
enum {
	DEFAULT,
	INSTANT_DT,
	NO_ENEMY_DT,
};
}

struct GameModeData {
    uint8 missionResultGameMode = 2;
    uint8 ldkNissionResult = LDKMODE::OFF;
    uint32 mustStyleMissionResult = STYLE_RANK::NONE;
    uint8 enemyDTMissionResult = ENEMYDTMODE::DEFAULT;
	std::vector<std::string> names = {
		"VANILLA MODE",
		"STYLE SWITCHER MODE",
		"CRIMSON MODE",
		"CUSTOM MODE",
		"UNRATED",
	};
    std::vector<uint32> colors = {
        0xFFFFFFFF,
        0xE8BA18FF,
        0xDA1B53FF,
        0x4050FFFF,
        0xFFFFFFFF,
    };
	std::vector<std::string> cheatsNames = {
		"TRAINING",
		"DAMAGE",
		"SPEED",
		"MOBILITY",
        "DEBUG",
        "RMS",
	};
    std::array<std::string, DIFFICULTY_MODE::COUNT> difficultyModeNames = {
        "Easy",
        "Normal",
        "Hard",
        "Very Hard",
        "Dante Must Die",
    };
    std::vector<uint8> currentlyUsedCheats = { };
    std::vector<uint8> missionUsedCheats = { };
    bool isMissionInitializedGameModeMResult = false;
    bool isMissionInitializedCheatsUsedMission = false;
    bool arcadeMissionEnabled = false;
    bool bossRushMissionEnabled = false;
};

extern GameModeData gameModeData;

extern bool ebonyIvoryCancel;

extern bool triggerVFX;

extern int styleVFXCount;
extern int styleVFXCountMax;

extern float siytimer;
extern float rainstormMomentum;
extern float highTimeRotation;

struct DoubleTap {
    int bufferDuration  = 200;
    int buffer          = bufferDuration;
    bool canChange      = false;
    bool trackerRunning = false;
	std::chrono::steady_clock::time_point lastTapTime = {};
	int tapCount = 0;
};

extern DoubleTap quickDoubleTap[PLAYER_COUNT];
extern DoubleTap doppDoubleTap[PLAYER_COUNT];

extern bool inGunShoot;
extern bool gunShootInverted;
extern bool gunShootNormalized;
extern float distanceToEnemy;

extern float storedRisingSunTauntPosY;
extern float storedRisingSunTauntPosYClone;
extern float storedLunarPhasePosY;

struct RankAnnouncer {
    int turn            = 0;
    int count           = 0;
    bool trackerRunning = false;
    bool offCooldown    = true;
};

extern RankAnnouncer rankAnnouncer[7];

extern uint16 relativeTiltController;

extern float doppSeconds;
extern float doppSecondsDT;
extern bool doppTimeTrackerRunning;
extern float currentDTDoppOn;
extern float currentDTDoppDTOn;
extern float doppDuration;
extern float doppDurationDT;

struct StoredAirCounts {
	uint8 trickUp = 0;
    uint8 skyStar = 0;
    uint8 airHike = 0;
    uint8 airStinger = 0;
    uint8 airTornado = 0;
    bool cancelTrackerRunning = false;
};

struct AirCounts {
    uint8 airRisingSunWhirlwind = 0;
    uint8 airAgniRudraWhirlwind = 0;
    uint8 airTornado = 0;
};

struct FasterDarkslayer {
    float newSpeed            = 2.0f;
    bool newSpeedSet          = false;
    float storedSpeedHuman    = 1;
    float storedSpeedDevil[4] = {1.2f, 1.2f, 1.2f, 1.2f};
};

extern FasterDarkslayer fasterDarkslayer;
;

struct FasterRapidSlash {
    float newSpeed            = 2.0f;
    bool newSpeedSet          = false;
    float storedSpeedHuman    = 1;
    float storedSpeedDevil[4] = {1.2f, 1.2f, 1.2f, 1.2f};
};

extern FasterRapidSlash fasterRapidSlash;

extern bool inRapidSlash;

extern bool styleChanged[6];

extern float g_FrameRate;
extern float g_FrameRateTimeMultiplier;
extern float g_FrameRateTimeMultiplierRounded;
extern bool g_inCombat;
extern bool g_inBossfight;
extern int g_bossQuantity;
extern bool g_inGameDelayed;
extern bool g_inGameCutscene;
extern bool g_inMainMenu;
extern bool g_allActorsSpawned;
extern bool g_HudVisible;
extern std::string g_gameTrackPlaying;

extern bool exceptionShot;

struct SkyLaunch {
    bool setVolume = false;
    bool forceJustFrameReleaseToggledOff = false;
    bool trackerRunning = false;
    bool executing = false;
    bool appliedProperties = false;
    float storedRank = 0;
    uint16 storedReleaseLevel = 0;
    float storedDevilTrigger = 0;
    vec3 storedPos;
};

struct RoyalRelease {
    bool executing = false;
    bool trackerRunning = false;
};

struct Sprint {
    bool canSprint                  = false;
    float duration                  = 0.65f;
    float durationCombatHuman       = 3.80f;
    float durationCombatDevil       = 1.40f;
    float timer                     = duration;
    bool runTimer                   = false;
    bool isSprinting                = false;
    bool SFXPlayed                  = false;
    bool VFXPlayed                  = false;
    float storedSpeedHuman          = 0;
    float storedSpeedDevilDante[6]  = {};
    float storedSpeedDevilVergil[4] = {};
};

struct SprintVFX {
    int bank = 3;
    int id   = 141;
};

extern SprintVFX sprintVFX;

extern int notHoldingMelee;

struct Drive {
    bool level1EffectPlayed = false;
    bool level2EffectPlayed = false;
    bool level3EffectPlayed = false;
    float timer             = 0;
    bool runTimer           = false;
};
;

extern bool crazyComboHold;

struct TrickUpCancel {
    int cooldownDuration = 1300;
    int cooldown         = cooldownDuration;
    bool canTrickUp      = true;
    bool trackerRunning  = false;
};

extern TrickUpCancel trickUpCancel;

struct GunsCancel {
    int cooldownDuration = 1300;
    int cooldown         = cooldownDuration;
    bool canGun          = true;
    bool trackerRunning  = false;
};

extern GunsCancel gunsCancel;

struct RainstormCancel {
    int cooldownDuration = 1700;
    int cooldown         = cooldownDuration;
    bool canGun          = true;
    bool trackerRunning  = false;
};

extern RainstormCancel rainstormCancel;

struct ImprovedCancels {
    float trickCooldownDuration     = 0.7f;
    float gunsCooldownDuration      = 0.7f;
    float rainstormCooldownDuration = 0.75f;
    float trickCooldown             = trickCooldownDuration;
    float gunsCooldown              = gunsCooldownDuration;
    float rainstormCooldown         = rainstormCooldownDuration;
    bool canTrick                   = true;
    bool canGun                     = true;
    bool canRainstorm               = true;
    float revolverTimer = 0.0f;
    bool revolverTimerRunning = false;
};

struct BackToForward {
    bool backCommand             = false;
    float backDuration           = 0.16f;
    float backBuffer             = backDuration;
    bool backDirectionChanged    = true;
    bool forwardDirectionChanged = true;
    bool forwardCommand          = false;
    float forwardDuration        = 0.2f;
    float forwardBuffer          = forwardDuration;
};

struct StyleSwitchText {
    float duration  = 0.45f;
    float time[9] = { 0 };
    float alpha[9] = { 0 };
    float color[9][3];
    float animSize = 0;
};

struct DTESFX {
    bool started = false;
    bool looped = false;
    bool finished = false;
    float releaseVolumeMult = 0;
};

struct DTEVFX {
    bool started = false;
    bool finished = false;
    float time = 0;
};

struct MoveGravityTweak {
    float gravity           = 0;
    bool gravityPre4Changed = false;
    bool gravity4Changed    = false;
    bool hasAppliedVerticalPullMultiplier = false;
};

extern bool inertiaFixesEnabled;

struct MoveProperties {
    float cachedPull;
    float cachedRotation;
};


struct Inertia {
    MoveProperties aerialRave;
    MoveProperties airFlicker;
    MoveProperties skyDance;
    MoveProperties airSlash;
    MoveProperties theHammer;
    MoveProperties tornado;
    MoveProperties killerBee;

    MoveProperties skyStar;
    MoveProperties jumpCancel;
    MoveProperties airHike;
    MoveProperties airGuard;

    MoveProperties rainstorm;
    MoveProperties fireworks;
    MoveProperties artemisShot;
    MoveProperties artemisMultiLockShot;
    MoveProperties ebonyShot;
    MoveProperties shotgunShot;

    MoveProperties yamatoRave;
    float guardflyTimer = 0;
};

struct VergilMoveAdjustments {
    float storedRisingSunPosY;
    float storedLunarPhasePosY;
    float storedLunarPhaseLv1PosY;
    bool startingRisingSunFromGround = false;
    bool startingLunarPhaseFromGround = false;
};

struct DelayedComboFX {
	float duration = 0.5f;
	int playCount = 0;
	uint8 weaponThatStartedMove = 20;
	int bank = 3;
	int id = 143;
};

constexpr float maxMiragePointsAmount = 10000;

struct VergilDoppelganger {
    float miragePoints = 0;
    float maxMiragePoints = 10000;
    float cooldownTime = 0;
    float cooldownDuration = 1.0f;
    float drainTime = 0;
    float totalDrainDuration = 12.0f;
    bool drainStart = false;
};

extern bool inRoyalBlock;
extern bool inGuardfly;
extern float rainstormPull;

extern float storedHP;
extern float storedDT;


struct CrimsonPlayerData {
    uintptr_t playerPtr;
    uint8 action     = 0;
    uint8 lastAction = 0;
    uint8 motion     = 0;
    uint32 event     = 0;
    uint32 lastEvent = 0;
    byte32 state;
    byte32 lastState;
    float speed      = 0;
	float hitPoints = 20000;
    float maxHitPoints = 20000;
    float magicPoints = 10000;
    float maxMagicPoints = 10000;
    uint32 style = 1;
    float royalguardReleaseDamage = 0;
    float dtExplosionCharge = 0;
    StyleData styleData;
    uint32 actorMode;
    uint32 character = 0;
    ENGINE_GAMEPAD gamepad;
    uint8 tiltDirection;
    bool lockOn;
    int currentAction = 0;
    int currentAnim   = 0;
    float actionTimer = 0;
    float lastActionTime = 0;
    float animTimer   = 0;
    float eventTimer = 0;
    float trickDashTimer = 0;
    uint32 currentEvent = 0;
    bool active;
    bool inNewDrive   = false;
    bool inQuickDrive = false;
    Sprint sprint;
    Drive drive;
    std::vector<uint32> lastEvents{0};
    int lastLastEvent = 0;
    std::vector<byte32> lastStates{0};
    byte32 lastLastState = 0;
    float horizontalPull;
    VergilMoveAdjustments vergilMoves;
    Inertia inertia;
    SkyLaunch skyLaunch;
    RoyalRelease royalRelease;
    ImprovedCancels cancels;
    BackToForward b2F;
    StyleSwitchText styleSwitchText;
    DTESFX dTESFX;
    DTEVFX dTEVFX;
    DelayedComboFX delayedComboFX;
    VergilDoppelganger vergilDoppelganger;
    float fluxtime = 0;
    bool fluxCanStart = true;
    bool fluxCanEnd = false;
    bool fluxCanEndConfirm = false;
    float cameraPlayerDistance = 0;
    int cameraPlayerDistanceClamped = 0;
    float playerTo1PDistance = 0;
    SimpleVec3 playerScreenPosition = { 0,0,0 };
	float cameraLockedEnemyDistance = 0;
    int cameraLockedEnemyDistanceClamped = 0;
    SimpleVec3 lockedEnemyScreenPosition = { 0,0,0 };
    int playerScreenAngle = 0;
    bool playerOutOfView = false;
    uint16 rotationTowardsEnemy = 0;
    StoredAirCounts storedAirCounts;
	AirCounts airCounts;
    float lockedOnEnemyHP = 0;
	float lockedOnEnemyMaxHP = 0;
	float lockedOnEnemyShield = 0;
	float lockedOnEnemyMaxShield = 0;
    float lockedOnEnemyStun = 0;
    float lockedOnEnemyDisplacement = 0;
    float lockedOnEnemyMaxStun = 0;
	float lockedOnEnemyMaxDisplacement = 0;
    float lockedOnEnemyMinusStun = 0;
	float lockedOnEnemyMinusDisplacement = 0;
    MoveGravityTweak airFlickerTweak;
    MoveGravityTweak skyDanceTweak;

    uintptr_t clonePtr;
    uint8 actionClone     = 0;
    uint8 lastActionClone = 0;
    uint8 motionClone     = 0;
    uint32 eventClone     = 0;
    uint32 lastEventClone = 0;
    byte32 stateClone;
    byte32 lastStateClone;
    float speedClone = 0;
    int currentActionClone = 0;
    int currentAnimClone   = 0;
    float actionTimerClone = 0;
    float lastActionTimeClone = 0;
    float animTimerClone   = 0;
    float eventTimerClone = 0;
    float trickDashTimerClone = 0;
    uint32 currentEventClone = 0;
    std::vector<uint32> lastEventsClone{0};
    int lastLastEventClone = 0;
    std::vector<byte32> lastStatesClone{0};
    byte32 lastLastStateClone = 0;
    float horizontalPullClone;
    RoyalRelease royalReleaseClone;
    SkyLaunch skyLaunchClone;
    VergilMoveAdjustments vergilMovesClone;
    ImprovedCancels cancelsClone;
    BackToForward b2FClone;
    Inertia inertiaClone;
    float cameraCloneDistance = 0;
    float cameraCloneDistanceClamped = 0;
    float cloneTo1PDistance = 0;
    SimpleVec3 cloneScreenPosition = { 0,0,0 };
	float cameraCloneLockedEnemyDistance = 0;
	int cameraCloneLockedEnemyDistanceClamped = 0;
	SimpleVec3 cloneLockedEnemyScreenPosition = { 0,0,0 };
	int cloneScreenAngle = 0;
    bool cloneOutOfView = false;
    uint16 rotationCloneTowardsEnemy = 0;
    StoredAirCounts storedAirCountsClone;
    AirCounts airCountsClone;
	float lockedOnEnemyHPClone = 0;
	float lockedOnEnemyMaxHPClone = 0;
	float lockedOnEnemyShieldClone = 0;
	float lockedOnEnemyMaxShieldClone = 0;
	float lockedOnEnemyStunClone = 0;
	float lockedOnEnemyDisplacementClone = 0;
	float lockedOnEnemyMaxStunClone = 0;
	float lockedOnEnemyMaxDisplacementClone = 0;
	float lockedOnEnemyMinusStunClone = 0;
	float lockedOnEnemyMinusDisplacementClone = 0;
	MoveGravityTweak airFlickerTweakClone;
	MoveGravityTweak skyDanceTweakClone;
};

extern CrimsonPlayerData crimsonPlayer[20];


struct HeldStyleExpData {
    float styleExpPoints = 0;
    int styleLevel = 0;
    int accumulatedStyleLevels[6] = { 0 };
    float accumulatedStylePoints[6] = { 0 };
    int missionStyleLevels[6] = { 0 };
};

extern HeldStyleExpData heldStyleExpDataDante;
extern HeldStyleExpData heldStyleExpDataVergil;

extern bool devilTriggerReadyPlayed;

extern bool missionClearSongPlayed;

struct GuiPause {
    bool in       = false;
    float timer   = 0.5f;
    bool canPause = false;
};

extern GuiPause guiPause;

extern std::vector<std::string> HUDdirectories;

extern float frameRateSpeedMultiplier;

// 0 - 1P, 1 - 1P clone, 2 - 2P, 3 - 2P clone, 4 - 3P, 5 - 3P clone, 6 - 4P, 7 - 4P clone
extern std::array<float, PLAYER_COUNT * 2> g_plEntityTo1PDistances;
extern std::array<float, 50> g_enemiesTo1PDistances;
extern std::array<float, PLAYER_COUNT * 2> g_plEntityCameraDistances;
extern std::array<SimpleVec3, PLAYER_COUNT * 2> g_plEntityScreenPositions;
extern std::array<bool, PLAYER_COUNT * 2> g_plEntityOutOfView;
extern std::array<SimpleVec3, PLAYER_COUNT * 2> g_plEntityPositions;
extern int g_activeClonesCount;
extern int g_activePlayableEntitiesCount;
extern int g_activeAllEntitiesCount;
extern bool g_isMPCamActive;
extern bool g_isParanoramicCamActive;

extern __declspec(align(16)) float g_customCameraPos[4];
extern __declspec(align(16)) float g_customCameraSensitivity;
extern bool g_cameraHittingWall;

constexpr BossHelper bossHelpers[] = {
    {6, 2, "afs/sound/Boss_01.adx"},      // Cerberus
    {111, 0, "afs/sound/T_Boss.adx"},     // Gigapede
    {422, 0, "afs/sound/Jester.adx"},     // Jester 1
    {121, 3, "afs/sound/Boss_02.adx"},    // Agni & Rudra
    {144, 0, "afs/sound/Versil_01.adx"},  // Vergil 1
    {302, 0, "afs/sound/Boss_08.adx"},    // Leviathan
    {210, 2, "afs/sound/Boss_03.adx"},    // Nevan
    {217, 2, "afs/sound/Boss_04.adx"},    // Beowulf
    {448, 0, "afs/sound/Jester.adx"},     // Jester 2
    {228, 0, "afs/sound/Boss_05.adx"},    // Geryon Part 1
    {228, 2, "afs/sound/Boss_05.adx"},    // Geryon Part 2
    {234, 0, "afs/sound/Versil_02.adx"},  // Vergil 2
    {115, 2, "afs/sound/Lady.adx"},       // Lady
    {449, 0, "afs/sound/Jester.adx"},     // Jester 3
    {139, 0, "afs/sound/Boss_06.adx"},    // Doppelganger
    {420, 0, "afs/sound/Battle_01B.adx"}, // Taizai Reborn
    {412, 0, "afs/sound/Boss_01B.adx"},   // Cerberus Reborn
    {413, 0, "afs/sound/T_BossB.adx"},    // Gigapede Reborn
    {414, 0, "afs/sound/Boss_02B.adx"},   // Agni & Rudra Reborn
    {419, 0, "afs/sound/Boss_08B.adx"},   // Leviathan Reborn
    {415, 0, "afs/sound/Boss_03B.adx"},   // Nevan Reborn
    {416, 0, "afs/sound/Boss_04B.adx"},   // Beowulf Reborn
    {417, 0, "afs/sound/Boss_05B.adx"},   // Geryon Reborn
    {418, 0, "afs/sound/Boss_06B.adx"},   // Doppelganger Reborn
    {421, 0, "afs/sound/Hine_01.adx"},    // Arkham Part 1
    {421, 0, "afs/sound/Hine_02.adx"},    // Arkham Part 2
    {411, 0, "afs/sound/Versil_03.adx"},  // Vergil 3
};

#pragma endregion

#pragma pop_macro("VOID")
#pragma pop_macro("IGNORE")
#include <stdio.h>
#include <string.h>

#include "Core/RapidJSON.h"
#define RAPIDJSON_HAS_STDSTRING 1

// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "Config.hpp"
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include "Global.hpp"
#include <iostream>

#include "Core/Macros.h"

using namespace DI8;

#include "Core/DebugSwitch.hpp"
#include "CrimsonFileHandling.hpp"

inline const char* directoryName = Paths::config;
inline const char* fileName      = "CrimsonConfig.json";

char locationConfig[64] = {};

Config defaultConfig;
Config queuedConfig;
Config activeConfig;

// $GetDataStart

PlayerData& GetDefaultPlayerData(uint8 playerIndex) {
    return defaultConfig.Actor.playerData[playerIndex];
}

PlayerData& GetActivePlayerData(uint8 playerIndex) {
    return activeConfig.Actor.playerData[playerIndex];
}

PlayerData& GetQueuedPlayerData(uint8 playerIndex) {
    return queuedConfig.Actor.playerData[playerIndex];
}

PlayerData& GetPlayerData(uint8 playerIndex) {
    return GetActivePlayerData(playerIndex);
}

CharacterData& GetDefaultCharacterData(uint8 playerIndex, uint8 characterIndex, uint8 entityIndex) {
    auto& playerData = GetDefaultPlayerData(playerIndex);

    return playerData.characterData[characterIndex][entityIndex];
}

CharacterData& GetActiveCharacterData(uint8 playerIndex, uint8 characterIndex, uint8 entityIndex) {
    auto& playerData = GetActivePlayerData(playerIndex);

    return playerData.characterData[characterIndex][entityIndex];
}

CharacterData& GetQueuedCharacterData(uint8 playerIndex, uint8 characterIndex, uint8 entityIndex) {
    auto& playerData = GetQueuedPlayerData(playerIndex);

    return playerData.characterData[characterIndex][entityIndex];
}

CharacterData& GetCharacterData(uint8 playerIndex, uint8 characterIndex, uint8 entityIndex) {
    auto& playerData = GetPlayerData(playerIndex);

    return playerData.characterData[characterIndex][entityIndex];
}

// $GetDataEnd

void ApplyDefaultCharacterData(CharacterData& characterData, uint8 character) {
    SetMemory(&characterData, 0, sizeof(CharacterData));

    switch (character) {
    case CHARACTER::DANTE: {
        characterData = {CHARACTER::DANTE, 0, false, false, CHARACTER::DANTE, 0,
            {
                {
                    STYLE::TRICKSTER,
                    STYLE::TRICKSTER,
                },
                {
                    STYLE::SWORDMASTER,
                    STYLE::QUICKSILVER,
                },
                {
                    STYLE::ROYALGUARD,
                    STYLE::ROYALGUARD,
                },
                {
                    STYLE::GUNSLINGER,
                    STYLE::DOPPELGANGER,
                },
            },
            {},
            {
                GAMEPAD::UP,
                GAMEPAD::RIGHT,
                GAMEPAD::DOWN,
                GAMEPAD::LEFT,
            },
            0, MELEE_WEAPON_COUNT_DANTE,
            {
                WEAPON::REBELLION,
                WEAPON::CERBERUS,
                WEAPON::AGNI_RUDRA,
                WEAPON::NEVAN,
                WEAPON::BEOWULF_DANTE,
            },
            0, 0, WEAPON_SWITCH_TYPE::LINEAR, RIGHT_STICK, RANGED_WEAPON_COUNT_DANTE,
            {
                WEAPON::EBONY_IVORY,
                WEAPON::SHOTGUN,
                WEAPON::ARTEMIS,
                WEAPON::SPIRAL,
                WEAPON::KALINA_ANN,
            },
            0, 0, WEAPON_SWITCH_TYPE::LINEAR, RIGHT_STICK};

        break;
    };
    case CHARACTER::BOB: {
        characterData = {CHARACTER::BOB};

        break;
    };
    case CHARACTER::LADY: {
        characterData = {CHARACTER::LADY};

        break;
    };
    case CHARACTER::VERGIL: {
        characterData = {CHARACTER::VERGIL, 0, false, false, CHARACTER::DANTE, 0,
            {
                {
                    STYLE::DARK_SLAYER,
                    STYLE::DARK_SLAYER,
                },
                {
                    STYLE::DARK_SLAYER,
                    STYLE::QUICKSILVER,
                },
                {
                    STYLE::DARK_SLAYER,
                    STYLE::DARK_SLAYER,
                },
                {
                    STYLE::DARK_SLAYER,
                    STYLE::DOPPELGANGER,
                },
            },
            {},
            {
                GAMEPAD::UP,
                GAMEPAD::RIGHT,
                GAMEPAD::DOWN,
                GAMEPAD::LEFT,
            },
            0, MELEE_WEAPON_COUNT_VERGIL,
            {
                WEAPON::YAMATO_VERGIL,
                WEAPON::BEOWULF_VERGIL,
                WEAPON::YAMATO_FORCE_EDGE,
            },
            0, 0, WEAPON_SWITCH_TYPE::LINEAR, RIGHT_STICK};

        break;
    };
    case CHARACTER::BOSS_LADY: {
        characterData = {CHARACTER::BOSS_LADY};

        break;
    };
    case CHARACTER::BOSS_VERGIL: {
        characterData = {CHARACTER::BOSS_VERGIL};

        break;
    };
    }
}

void ApplyDefaultPlayerData(PlayerData& playerData) {
    playerData.switchButton = GAMEPAD::RIGHT_THUMB;

    playerData.characterCount = 2;
    playerData.characterIndex = 0;

    old_for_all(uint8, characterIndex, CHARACTER_COUNT) {
        old_for_all(uint8, entityIndex, ENTITY_COUNT) {
            ApplyDefaultCharacterData(
                playerData.characterData[characterIndex][entityIndex], (characterIndex == 1) ? CHARACTER::VERGIL : CHARACTER::DANTE);
        }
    }
}

namespace JSON {

#include "Core/JSON.h"

#pragma region CreateMembers

void CreateMembers_CharacterDataContent(rapidjson::Value& member, CharacterData& config) {
    Create<uint8>(member, "character", config.character);
    Create<uint8>(member, "costume", config.costume);
    Create<bool>(member, "ignoreCostume", config.ignoreCostume);
    Create<bool>(member, "forceFiles", config.forceFiles);
    Create<uint8>(member, "forceFilesCharacter", config.forceFilesCharacter);
    Create<uint8>(member, "forceFilesCostume", config.forceFilesCostume);

    CreateArray2<uint8, STYLE_COUNT, 2>(member, "styles", config.styles);

    CreateArray<uint8, STYLE_COUNT>(member, "styleIndices", config.styleIndices);

    CreateArray<byte16, STYLE_COUNT>(member, "styleButtons", config.styleButtons);

    Create<uint8>(member, "styleButtonIndex", config.styleButtonIndex);


    Create<uint8>(member, "meleeWeaponCount", config.meleeWeaponCount);

    CreateArray<uint8, MELEE_WEAPON_COUNT>(member, "meleeWeapons", config.meleeWeapons);

    Create<uint8>(member, "meleeWeaponIndex", config.meleeWeaponIndex);
    Create<uint8>(member, "lastMeleeWeaponIndex", config.lastMeleeWeaponIndex);
    Create<uint8>(member, "meleeWeaponSwitchType", config.meleeWeaponSwitchType);
    Create<uint8>(member, "meleeWeaponSwitchStick", config.meleeWeaponSwitchStick);


    Create<uint8>(member, "rangedWeaponCount", config.rangedWeaponCount);

    CreateArray<uint8, RANGED_WEAPON_COUNT>(member, "rangedWeapons", config.rangedWeapons);

    Create<uint8>(member, "rangedWeaponIndex", config.rangedWeaponIndex);
    Create<uint8>(member, "lastRangedWeaponIndex", config.lastRangedWeaponIndex);
    Create<uint8>(member, "rangedWeaponSwitchType", config.rangedWeaponSwitchType);
    Create<uint8>(member, "rangedWeaponSwitchStick", config.rangedWeaponSwitchStick);
}


void CreateMembers_PlayerDataContent(rapidjson::Value& member, PlayerData& config) {
    Create<uint8>(member, "collisionGroup", config.collisionGroup);
    Create<uint16>(member, "switchButton", config.switchButton);
    Create<uint8>(member, "characterCount", config.characterCount);
    Create<uint8>(member, "characterIndex", config.characterIndex);
    Create<uint8>(member, "lastCharacterIndex", config.lastCharacterIndex);
    Create<uint8>(member, "activeCharacterIndex", config.activeCharacterIndex);


    auto& characterData = CreateArray2<struct_t, CHARACTER_COUNT, ENTITY_COUNT>(member, "characterData");

    for_all(characterIndex, CHARACTER_COUNT) {
        for_all(entityIndex, ENTITY_COUNT) {
            auto& member2 = characterData[characterIndex][entityIndex];
            auto& config2 = config.characterData[characterIndex][entityIndex];

            CreateMembers_CharacterDataContent(member2, config2);
        }
    }


    Create<bool>(member, "removeBusyFlag", config.removeBusyFlag);

    CreateArray<byte16, 4>(member, "removeBusyFlagButtons", config.removeBusyFlagButtons);
}


template <typename T, new_size_t length> rapidjson::Value& CreateMembers_Vec2(rapidjson::Value& member, T (&name)[length], vec2& config) {
    auto& newMember = Create<struct_t>(member, name);

    Create<float>(newMember, "x", config.x);
    Create<float>(newMember, "y", config.y);

    return newMember;
}

template <typename T, new_size_t length> rapidjson::Value& CreateMembers_Vec4(rapidjson::Value& member, T (&name)[length], vec4& config) {
    auto& newMember = Create<struct_t>(member, name);

    Create<float>(newMember, "x", config.x);
    Create<float>(newMember, "y", config.y);
    Create<float>(newMember, "z", config.z);
    Create<float>(newMember, "a", config.a);

    return newMember;
}

void CreateMembers_TextureDataContent(rapidjson::Value& member, Config::TextureData& config) {
    CreateMembers_Vec2(member, "size", config.size);
    CreateMembers_Vec2(member, "pos", config.pos);
}

template <typename T, new_size_t length>
rapidjson::Value& CreateMembers_TextureData(rapidjson::Value& member, T (&name)[length], Config::TextureData& config) {
    auto& newMember = Create<struct_t>(member, name);

    CreateMembers_TextureDataContent(newMember, config);

    return newMember;
}

template <typename T, new_size_t length>
rapidjson::Value& CreateMembers_TextureDataArray(rapidjson::Value& member, T (&name)[length], Config::TextureData (&config)[5]) {
    auto& newMember = CreateArray<struct_t, 5>(member, name);

    for_all(index, 5) {
        auto& member2 = newMember[index];
        auto& config2 = config[index];

        CreateMembers_TextureDataContent(member2, config2);
    }

    return newMember;
}

template <typename T, new_size_t length>
rapidjson::Value& CreateMembers_WeaponSwitchControllerTextureData(
    rapidjson::Value& member, T (&name)[length], Config::WeaponSwitchControllerTextureData& config) {
    auto& newMember = Create<struct_t>(member, name);

    CreateMembers_TextureDataArray(newMember, "backgrounds", config.backgrounds);
    CreateMembers_TextureDataArray(newMember, "icons", config.icons);
    CreateMembers_TextureDataArray(newMember, "highlights", config.highlights);
    CreateMembers_TextureData(newMember, "arrow", config.arrow);

    return newMember;
}

void CreateMembers_ConfigCreateEnemyActorDataContent(rapidjson::Value& member, ConfigCreateEnemyActorData& config) {
    Create<uint32>(member, "enemy", config.enemy);
    Create<uint32>(member, "variant", config.variant);
    CreateMembers_Vec4(member, "position", config.position);
    Create<uint16>(member, "rotation", config.rotation);
    Create<bool>(member, "useMainActorData", config.useMainActorData);
    Create<uint16>(member, "spawnMethod", config.spawnMethod);
}

void CreateMembers_OverlayDataContent(rapidjson::Value& member, Config::OverlayData& config) {
    Create<bool>(member, "enable", config.enable);
    CreateMembers_Vec2(member, "pos", config.pos);
    CreateArray<float, 4>(member, "color", config.color);
}

void CreateMembers_BarsDataContent(rapidjson::Value& member, Config::BarsData& config) {
    Create<bool>(member, "enable", config.enable);
    Create<bool>(member, "run", config.run);
    CreateMembers_Vec2(member, "size", config.size);
    CreateMembers_Vec2(member, "pos", config.pos);
    Create<uint32>(member, "lastX", config.lastX);
    Create<uint32>(member, "lastY", config.lastY);

    CreateArray<float, 4>(member, "hitColor", config.hitColor);
    CreateArray<float, 4>(member, "magicColor", config.magicColor);
}

void CreateMembers_KeyDataContent(rapidjson::Value& member, KeyData& config) {
    CreateArray<byte8, 4>(member, "keys", config.keys);

    Create<new_size_t>(member, "keyCount", config.keyCount);
}

void CreateMembers(Config& config_) {
    DebugLogFunction();

    {
        auto& member = Create<struct_t>(root, "Actor");
        auto& config = config_.Actor;

        Create<bool>(member, "enable", config.enable);
        Create<uint8>(member, "playerCount", config.playerCount);

        auto& playerData = CreateArray<struct_t, PLAYER_COUNT>(member, "playerData");

        for_all(playerIndex, PLAYER_COUNT) {
            auto& member2 = playerData[playerIndex];
            auto& config2 = config.playerData[playerIndex];

            CreateMembers_PlayerDataContent(member2, config2);
        }
    }


    {
        auto& member = Create<struct_t>(root, "Arcade");
        auto& config = config_.Arcade;

        Create<bool>(member, "enable", config.enable);
        Create<uint32>(member, "mission", config.mission);
        Create<uint32>(member, "mode", config.mode);
        Create<uint32>(member, "room", config.room);
        Create<bool>(member, "enableRoomSelection", config.enableRoomSelection);
        Create<uint32>(member, "position", config.position);
        Create<bool>(member, "enablePositionSelection", config.enablePositionSelection);
        Create<uint8>(member, "floor", config.floor);
        Create<uint16>(member, "level", config.level);
        Create<float>(member, "hitPoints", config.hitPoints);
        Create<float>(member, "magicPoints", config.magicPoints);
        Create<uint8>(member, "character", config.character);
        Create<uint8>(member, "costume", config.costume);
        Create<uint32>(member, "style", config.style);

        CreateArray<uint8, 4>(member, "weapons", config.weapons);
    }


    {
        auto& member = Create<struct_t>(root, "Artemis");
        auto& config = config_.Artemis;

        Create<bool>(member, "swapNormalShotAndMultiLock", config.swapNormalShotAndMultiLock);
        Create<bool>(member, "instantFullCharge", config.instantFullCharge);
    }


    {
        auto& member = Create<struct_t>(root, "BossRush");
        auto& config = config_.BossRush;

        Create<bool>(member, "enable", config.enable);

        {
            auto& member2 = Create<struct_t>(member, "Mission5");
            auto& config2 = config.Mission5;

            Create<bool>(member2, "skipJester", config2.skipJester);
        }

        {
            auto& member2 = Create<struct_t>(member, "Mission12");
            auto& config2 = config.Mission12;

            Create<bool>(member2, "skipJester", config2.skipJester);
            Create<bool>(member2, "skipGeryonPart1", config2.skipGeryonPart1);
        }

        {
            auto& member2 = Create<struct_t>(member, "Mission17");
            auto& config2 = config.Mission17;

            Create<bool>(member2, "skipJester", config2.skipJester);
        }

        {
            auto& member2 = Create<struct_t>(member, "Mission19");
            auto& config2 = config.Mission19;

            Create<bool>(member2, "skipArkhamPart1", config2.skipArkhamPart1);
        }
    }


    {
        auto& member = Create<struct_t>(root, "Color");
        auto& config = config_.Color;

        CreateArray2<uint8, 5, 4>(member, "airHike", config.airHike);


        {
            auto& member2 = Create<struct_t>(member, "Trickster");
            auto& config2 = config.Trickster;

            CreateArray<uint8, 4>(member2, "skyStar", config2.skyStar);
        }

        {
            auto& member2 = Create<struct_t>(member, "Royalguard");
            auto& config2 = config.Royalguard;

            CreateArray<uint8, 4>(member2, "ultimate", config2.ultimate);
        }

        {
            auto& member2 = Create<struct_t>(member, "Doppelganger");
            auto& config2 = config.Doppelganger;

            CreateArray<uint8, 4>(member2, "clone", config2.clone);
        }


        {
            auto& member2 = Create<struct_t>(member, "Aura");
            auto& config2 = config.Aura;

            CreateArray2<uint8, 5, 4>(member2, "dante", config2.dante);
            CreateArray<uint8, 4>(member2, "sparda", config2.sparda);
            CreateArray2<uint8, 3, 4>(member2, "vergil", config2.vergil);
            CreateArray<uint8, 4>(member2, "neroAngelo", config2.neroAngelo);
        }
    }


    {
        auto& member = Create<struct_t>(root, "EbonyIvory");
        auto& config = config_.EbonyIvory;

        Create<bool>(member, "foursomeTime", config.foursomeTime);
        Create<bool>(member, "infiniteRainStorm", config.infiniteRainStorm);
    }


    {
        auto& member = Create<struct_t>(root, "Rebellion");
        auto& config = config_.Rebellion;

        Create<bool>(member, "infiniteSwordPierce", config.infiniteSwordPierce);
    }


    {
        auto& member = Create<struct_t>(root, "Royalguard");
        auto& config = config_.Royalguard;

        Create<bool>(member, "forceJustFrameRelease", config.forceJustFrameRelease);
    }


    {
        auto& member = Create<struct_t>(root, "Speed");
        auto& config = config_.Speed;

        Create<float>(member, "mainSpeed", config.mainSpeed);
        Create<float>(member, "turbo", config.turbo);
        Create<float>(member, "enemy", config.enemy);
        Create<float>(member, "quicksilverPlayerActor", config.quicksilverPlayerActor);
        Create<float>(member, "quicksilverEnemyActor", config.quicksilverEnemyActor);
        Create<float>(member, "human", config.human);

        CreateArray<float, 6>(member, "devilDante", config.devilDante);
        CreateArray<float, 5>(member, "devilVergil", config.devilVergil);
    }


    {
        auto& member = Create<struct_t>(root, "SummonedSwords");
        auto& config = config_.SummonedSwords;

        Create<bool>(member, "chronoSwords", config.chronoSwords);
    }


    {
        auto& member = Create<struct_t>(root, "Yamato");
        auto& config = config_.Yamato;

        CreateArray<uint8, 2>(member, "judgementCutCount", config.judgementCutCount);
    }


    {
        auto& member = Create<struct_t>(root, "YamatoForceEdge");
        auto& config = config_.YamatoForceEdge;

        Create<bool>(member, "infiniteRoundTrip", config.infiniteRoundTrip);
    }


    auto& member = root;
    auto& config = config_;

    Create<bool>(member, "welcome", config.welcome);
    Create<bool>(member, "hideBeowulfDante", config.hideBeowulfDante);
    Create<bool>(member, "hideBeowulfVergil", config.hideBeowulfVergil);
    Create<bool>(member, "airHikeCoreAbility", config.airHikeCoreAbility);
    Create<uint8>(member, "crazyComboLevelMultiplier", config.crazyComboLevelMultiplier);
    Create<uint8>(member, "dotShadow", config.dotShadow);
    Create<float>(member, "depleteQuicksilver", config.depleteQuicksilver);
    Create<float>(member, "depleteDoppelganger", config.depleteDoppelganger);
    Create<float>(member, "depleteDevil", config.depleteDevil);
    Create<bool>(member, "noDevilForm", config.noDevilForm);
    Create<float>(member, "orbReach", config.orbReach);
    Create<bool>(member, "resetPermissions", config.resetPermissions);
    Create<bool>(member, "infiniteHitPoints", config.infiniteHitPoints);
    Create<bool>(member, "infiniteMagicPoints", config.infiniteMagicPoints);
    Create<bool>(member, "disableTimer", config.disableTimer);
    Create<bool>(member, "infiniteBullets", config.infiniteBullets);
    Create<float>(member, "linearWeaponSwitchTimeout", config.linearWeaponSwitchTimeout);

    CreateArray<uint8, 2>(member, "airHikeCount", config.airHikeCount);
    CreateArray<uint8, 2>(member, "kickJumpCount", config.kickJumpCount);
    CreateArray<uint8, 2>(member, "wallHikeCount", config.wallHikeCount);
    CreateArray<uint8, 2>(member, "dashCount", config.dashCount);
    CreateArray<uint8, 2>(member, "skyStarCount", config.skyStarCount);
    CreateArray<uint8, 2>(member, "airTrickCountDante", config.airTrickCountDante);
    CreateArray<uint8, 2>(member, "airTrickCountVergil", config.airTrickCountVergil);
    CreateArray<uint8, 2>(member, "trickUpCount", config.trickUpCount);
    CreateArray<uint8, 2>(member, "trickDownCount", config.trickDownCount);

    CreateArray<float, CHANNEL::MAX>(member, "channelVolumes", config.channelVolumes);

    CreateMembers_WeaponSwitchControllerTextureData(
        member, "meleeWeaponSwitchControllerTextureData", config.meleeWeaponSwitchControllerTextureData);
    CreateMembers_WeaponSwitchControllerTextureData(
        member, "rangedWeaponSwitchControllerTextureData", config.rangedWeaponSwitchControllerTextureData);

    Create<bool>(member, "forceIconFocus", config.forceIconFocus);
    Create<float>(member, "damagePlayerActorMultiplier", config.damagePlayerActorMultiplier);
    Create<float>(member, "damageEnemyActorMultiplier", config.damageEnemyActorMultiplier);
    Create<uint32>(member, "damageStyleRank", config.damageStyleRank);
    Create<bool>(member, "skipIntro", config.skipIntro);
    Create<bool>(member, "skipCutscenes", config.skipCutscenes);
    Create<bool>(member, "enableFileMods", config.enableFileMods);
    Create<float>(member, "frameRate", config.frameRate);
    Create<uint8>(member, "vSync", config.vSync);
    Create<bool>(member, "hideMouseCursor", config.hideMouseCursor);
    Create<int32>(member, "windowPosX", config.windowPosX);
    Create<int32>(member, "windowPosY", config.windowPosY);
    Create<bool>(member, "forceWindowFocus", config.forceWindowFocus);
	Create<float>(member, "globalScale", config.globalScale);

    Create<uint8>(member, "enemyCount", config.enemyCount);

    {
        auto& newMember = CreateArray<struct_t, 30>(member, "configCreateEnemyActorData");

        for_all(index, 30) {
            auto& member2 = newMember[index];
            auto& config2 = config.configCreateEnemyActorData[index];

            CreateMembers_ConfigCreateEnemyActorDataContent(member2, config2);
        }
    }

    Create<bool>(member, "enemyAutoSpawn", config.enemyAutoSpawn);


    {
        auto& member2 = Create<struct_t>(member, "debugOverlayData");
        auto& config2 = config.debugOverlayData;

        CreateMembers_OverlayDataContent(member2, config2);

        Create<bool>(member2, "showFocus", config2.showFocus);
        Create<bool>(member2, "showFPS", config2.showFPS);
        Create<bool>(member2, "showSizes", config2.showSizes);
        Create<bool>(member2, "showFrameRateMultiplier", config2.showFrameRateMultiplier);
        Create<bool>(member2, "showEventData", config2.showEventData);
        Create<bool>(member2, "showPosition", config2.showPosition);
        Create<bool>(member2, "showRegionData", config2.showRegionData);
    }


    {
        auto& member2 = Create<struct_t>(member, "missionOverlayData");
        auto& config2 = config.missionOverlayData;

        CreateMembers_OverlayDataContent(member2, config2);
    }

    {
        auto& member2 = Create<struct_t>(member, "bossLadyActionsOverlayData");
        auto& config2 = config.bossLadyActionsOverlayData;

        CreateMembers_OverlayDataContent(member2, config2);
    }

    {
        auto& member2 = Create<struct_t>(member, "bossVergilActionsOverlayData");
        auto& config2 = config.bossVergilActionsOverlayData;

        CreateMembers_OverlayDataContent(member2, config2);
    }


    Create<float>(member, "kalinaAnnHookGrenadeHeight", config.kalinaAnnHookGrenadeHeight);
    Create<float>(member, "kalinaAnnHookGrenadeTime", config.kalinaAnnHookGrenadeTime);
    CreateMembers_Vec4(member, "kalinaAnnHookMultiplier", config.kalinaAnnHookMultiplier);

    Create<bool>(member, "enableBossLadyFixes", config.enableBossLadyFixes);
    Create<bool>(member, "enableBossVergilFixes", config.enableBossVergilFixes);
    Create<bool>(member, "enablePVPFixes", config.enablePVPFixes);
    Create<bool>(member, "hideMainHUD", config.hideMainHUD);
    Create<bool>(member, "hideLockOn", config.hideLockOn);
    Create<bool>(member, "hideBossHUD", config.hideBossHUD);
    Create<bool>(member, "disableStyleRankHudFadeout", config.disableStyleRankHudFadeout);
    Create<bool>(member, "soundIgnoreEnemyData", config.soundIgnoreEnemyData);
    Create<bool>(member, "enableRebellionNewDrive", config.enableRebellionNewDrive);
    Create<bool>(member, "enableRebellionQuickDrive", config.enableRebellionQuickDrive);
    Create<bool>(member, "enableCerberusAirRevolver", config.enableCerberusAirRevolver);
    Create<bool>(member, "enableNevanNewVortex", config.enableNevanNewVortex);
    Create<bool>(member, "enableYamatoVergilNewJudgementCut", config.enableYamatoVergilNewJudgementCut);
    Create<bool>(member, "enableBeowulfVergilAirRisingSun", config.enableBeowulfVergilAirRisingSun);
    Create<bool>(member, "enableBeowulfVergilAirLunarPhase", config.enableBeowulfVergilAirLunarPhase);
    Create<bool>(member, "enableYamatoForceEdgeNewComboPart4", config.enableYamatoForceEdgeNewComboPart4);
    Create<bool>(member, "enableYamatoForceEdgeNewRoundTrip", config.enableYamatoForceEdgeNewRoundTrip);
    Create<uint8>(member, "dergil", config.dergil);
    Create<uint8>(member, "costumeRespectsProgression", config.costumeRespectsProgression);
    Create<bool>(member, "weaponWheelEnabled", config.weaponWheelEnabled);
    Create<bool>(member, "weaponWheelAnalogSelectionEnabled", config.weaponWheelAnalogSelectionEnabled);
    Create<bool>(member, "weaponWheelAnalogRightStick", config.weaponWheelAnalogRightStick);
    Create<bool>(member, "weaponWheelDisableCameraRotation", config.weaponWheelDisableCameraRotation);
    Create<float>(member, "weaponWheelScaleMultiplier", config.weaponWheelScaleMultiplier);
    Create<float>(member, "weaponWheelHeight", config.weaponWheelHeight);
    Create<float>(member, "weaponWheelHorizontalMelee", config.weaponWheelHorizontalMelee);
    Create<float>(member, "weaponWheelHorizontalRanged", config.weaponWheelHorizontalRanged);

    {
        auto& member = Create<struct_t>(root, "MeleeWeaponWheel");
        auto& config = config_.MeleeWeaponWheel;

        Create<bool>(member, "alwaysShow", config.alwaysShow);
        Create<uint32>(member, "timeout", config.timeout);
    }

    {
        auto& member = Create<struct_t>(root, "RangedWeaponWheel");
        auto& config = config_.MeleeWeaponWheel;

        Create<bool>(member, "alwaysShow", config.alwaysShow);
        Create<uint32>(member, "timeout", config.timeout);
    }

    CreateString(member, "selectedHUD", config.selectedHUD.c_str());

	Create<bool>(member, "disableBlendingEffects", config.disableBlendingEffects);
	Create<bool>(member, "framerateResponsiveGameSpeed", config.framerateResponsiveGameSpeed);

    CreateArray<uint8, 2>(member, "beowulfVergilAirRisingSunCount", config.beowulfVergilAirRisingSunCount);

    Create<bool>(member, "forceVisibleHUD", config.forceVisibleHUD);


    {
        auto& member = CreateArray<struct_t, PLAYER_COUNT>(root, "barsData");
        auto& config = config_.barsData;

        for_all(playerIndex, PLAYER_COUNT) {
            auto& member2 = member[playerIndex];
            auto& config2 = config[playerIndex];

            CreateMembers_BarsDataContent(member2, config2);
        }
    }

    Create<bool>(member, "forceSyncHitMagicPoints", config.forceSyncHitMagicPoints);
    Create<bool>(member, "updateLockOns", config.updateLockOns);
    Create<bool>(member, "showCredits", config.showCredits);

    {
        auto& member = CreateArray<struct_t, 3>(root, "keyData");
        auto& config = config_.keyData;

        for_all(index, 3) {
            auto& member2 = member[index];
            auto& config2 = config[index];

            CreateMembers_KeyDataContent(member2, config2);
        }
    }


    Create<bool>(member, "absoluteUnit", config.absoluteUnit);
    CreateString(member, "gamepadName", config.gamepadName);
    Create<byte8>(member, "gamepadButton", config.gamepadButton);
    Create<bool>(member, "disablePlayerActorIdleTimer", config.disablePlayerActorIdleTimer);
    Create<bool>(member, "rebellionInfiniteShredder", config.rebellionInfiniteShredder);
    Create<bool>(member, "rebellionHoldDrive", config.rebellionHoldDrive);
}

#pragma endregion

#pragma region ToJSON

void ToJSON_CharacterData(rapidjson::Value& member, CharacterData& config) {
    Set<uint8>(member["character"], config.character);
    Set<uint8>(member["costume"], config.costume);
    Set<bool>(member["ignoreCostume"], config.ignoreCostume);
    Set<bool>(member["forceFiles"], config.forceFiles);
    Set<uint8>(member["forceFilesCharacter"], config.forceFilesCharacter);
    Set<uint8>(member["forceFilesCostume"], config.forceFilesCostume);

    SetArray2<uint8, STYLE_COUNT, 2>(member["styles"], config.styles);
    SetArray<uint8, STYLE_COUNT>(member["styleIndices"], config.styleIndices);
    SetArray<byte16, STYLE_COUNT>(member["styleButtons"], config.styleButtons);

    Set<uint8>(member["styleButtonIndex"], config.styleButtonIndex);


    Set<uint8>(member["meleeWeaponCount"], config.meleeWeaponCount);

    SetArray<uint8, MELEE_WEAPON_COUNT>(member["meleeWeapons"], config.meleeWeapons);

    Set<uint8>(member["meleeWeaponIndex"], config.meleeWeaponIndex);
    Set<uint8>(member["lastMeleeWeaponIndex"], config.lastMeleeWeaponIndex);
    Set<uint8>(member["meleeWeaponSwitchType"], config.meleeWeaponSwitchType);
    Set<uint8>(member["meleeWeaponSwitchStick"], config.meleeWeaponSwitchStick);


    Set<uint8>(member["rangedWeaponCount"], config.rangedWeaponCount);

    SetArray<uint8, RANGED_WEAPON_COUNT>(member["rangedWeapons"], config.rangedWeapons);

    Set<uint8>(member["rangedWeaponIndex"], config.rangedWeaponIndex);
    Set<uint8>(member["lastRangedWeaponIndex"], config.lastRangedWeaponIndex);
    Set<uint8>(member["rangedWeaponSwitchType"], config.rangedWeaponSwitchType);
    Set<uint8>(member["rangedWeaponSwitchStick"], config.rangedWeaponSwitchStick);
}

void ToJSON_PlayerData(rapidjson::Value& member, PlayerData& config) {
    Set<uint8>(member["collisionGroup"], config.collisionGroup);
    Set<byte16>(member["switchButton"], config.switchButton);
    Set<uint8>(member["characterCount"], config.characterCount);
    Set<uint8>(member["characterIndex"], config.characterIndex);
    Set<uint8>(member["lastCharacterIndex"], config.lastCharacterIndex);
    Set<uint8>(member["activeCharacterIndex"], config.activeCharacterIndex);


    for_all(characterIndex, CHARACTER_COUNT) {
        for_all(entityIndex, ENTITY_COUNT) {
            ToJSON_CharacterData(member["characterData"][characterIndex][entityIndex], config.characterData[characterIndex][entityIndex]);
        }
    }


    Set<bool>(member["removeBusyFlag"], config.removeBusyFlag);

    SetArray<byte16, 4>(member["removeBusyFlagButtons"], config.removeBusyFlagButtons);
}

void ToJSON_Vec2(rapidjson::Value& member, vec2& config) {
    Set<float>(member["x"], config.x);
    Set<float>(member["y"], config.y);
}

void ToJSON_Vec4(rapidjson::Value& member, vec4& config) {
    Set<float>(member["x"], config.x);
    Set<float>(member["y"], config.y);
    Set<float>(member["z"], config.z);
    Set<float>(member["a"], config.a);
}

void ToJSON_TextureData(rapidjson::Value& member, Config::TextureData& config) {
    ToJSON_Vec2(member["size"], config.size);
    ToJSON_Vec2(member["pos"], config.pos);
}

void ToJSON_TextureDataArray(rapidjson::Value& member, Config::TextureData (&config)[5]) {
    for_all(index, 5) {
        auto& member2 = member[index];
        auto& config2 = config[index];

        ToJSON_TextureData(member2, config2);
    }
}

void ToJSON_WeaponSwitchControllerTextureData(rapidjson::Value& member, Config::WeaponSwitchControllerTextureData& config) {
    ToJSON_TextureDataArray(member["backgrounds"], config.backgrounds);
    ToJSON_TextureDataArray(member["icons"], config.icons);
    ToJSON_TextureDataArray(member["highlights"], config.highlights);
    ToJSON_TextureData(member["arrow"], config.arrow);
}

void ToJSON_ConfigCreateEnemyActorData(rapidjson::Value& member, ConfigCreateEnemyActorData& config) {
    Set<uint32>(member["enemy"], config.enemy);
    Set<uint32>(member["variant"], config.variant);
    ToJSON_Vec4(member["position"], config.position);
    Set<uint16>(member["rotation"], config.rotation);
    Set<bool>(member["useMainActorData"], config.useMainActorData);
    Set<uint16>(member["spawnMethod"], config.spawnMethod);
}

void ToJSON_OverlayData(rapidjson::Value& member, Config::OverlayData& config) {
    Set<bool>(member["enable"], config.enable);

    ToJSON_Vec2(member["pos"], config.pos);

    SetArray<float, 4>(member["color"], config.color);
}

void ToJSON_MainOverlayData(rapidjson::Value& member, Config::MainOverlayData& config) {
    ToJSON_OverlayData(member, config);

    Set<bool>(member["showFocus"], config.showFocus);
    Set<bool>(member["showFPS"], config.showFPS);
    Set<bool>(member["showSizes"], config.showSizes);
    Set<bool>(member["showFrameRateMultiplier"], config.showFrameRateMultiplier);
    Set<bool>(member["showEventData"], config.showEventData);
    Set<bool>(member["showPosition"], config.showPosition);
    Set<bool>(member["showRegionData"], config.showRegionData);
}

void ToJSON_BarsData(rapidjson::Value& member, Config::BarsData& config) {
    Set<bool>(member["enable"], config.enable);
    Set<bool>(member["run"], config.run);

    ToJSON_Vec2(member["size"], config.size);
    ToJSON_Vec2(member["pos"], config.pos);

    Set<uint32>(member["lastX"], config.lastX);
    Set<uint32>(member["lastY"], config.lastY);

    SetArray<float, 4>(member["hitColor"], config.hitColor);
    SetArray<float, 4>(member["magicColor"], config.magicColor);
}

void ToJSON_KeyData(rapidjson::Value& member, KeyData& config) {
    SetArray<byte8, 4>(member["keys"], config.keys);

    Set<new_size_t>(member["keyCount"], config.keyCount);
}

void ToJSON(Config& config_) {
    DebugLogFunction();

    {
        auto& member = root["Actor"];
        auto& config = config_.Actor;

        Set<bool>(member["enable"], config.enable);
        Set<uint8>(member["playerCount"], config.playerCount);


        for_all(playerIndex, PLAYER_COUNT) {
            ToJSON_PlayerData(member["playerData"][playerIndex], config.playerData[playerIndex]);
        }
    }


    {
        auto& member = root["Arcade"];
        auto& config = config_.Arcade;

        Set<bool>(member["enable"], config.enable);
        Set<uint32>(member["mission"], config.mission);
        Set<uint32>(member["mode"], config.mode);
        Set<uint32>(member["room"], config.room);
        Set<bool>(member["enableRoomSelection"], config.enableRoomSelection);
        Set<uint32>(member["position"], config.position);
        Set<bool>(member["enablePositionSelection"], config.enablePositionSelection);
        Set<uint8>(member["floor"], config.floor);
        Set<uint16>(member["level"], config.level);
        Set<float>(member["hitPoints"], config.hitPoints);
        Set<float>(member["magicPoints"], config.magicPoints);
        Set<uint8>(member["character"], config.character);
        Set<uint8>(member["costume"], config.costume);
        Set<uint32>(member["style"], config.style);

        SetArray<uint8, 4>(member["weapons"], config.weapons);
    }


    {
        auto& member = root["Artemis"];
        auto& config = config_.Artemis;

        Set<bool>(member["swapNormalShotAndMultiLock"], config.swapNormalShotAndMultiLock);
        Set<bool>(member["instantFullCharge"], config.instantFullCharge);
    }


    {
        auto& member = root["BossRush"];
        auto& config = config_.BossRush;

        Set<bool>(member["enable"], config.enable);

        {
            auto& member2 = member["Mission5"];
            auto& config2 = config.Mission5;

            Set<bool>(member2["skipJester"], config2.skipJester);
        }

        {
            auto& member2 = member["Mission12"];
            auto& config2 = config.Mission12;

            Set<bool>(member2["skipJester"], config2.skipJester);
            Set<bool>(member2["skipGeryonPart1"], config2.skipGeryonPart1);
        }

        {
            auto& member2 = member["Mission17"];
            auto& config2 = config.Mission17;

            Set<bool>(member2["skipJester"], config2.skipJester);
        }

        {
            auto& member2 = member["Mission19"];
            auto& config2 = config.Mission19;

            Set<bool>(member2["skipArkhamPart1"], config2.skipArkhamPart1);
        }
    }


    {
        auto& member = root["Color"];
        auto& config = config_.Color;

        SetArray2<uint8, 5, 4>(member["airHike"], config.airHike);

        {
            auto& member2 = member["Trickster"];
            auto& config2 = config.Trickster;

            SetArray<uint8, 4>(member2["skyStar"], config2.skyStar);
        }

        {
            auto& member2 = member["Royalguard"];
            auto& config2 = config.Royalguard;

            SetArray<uint8, 4>(member2["ultimate"], config2.ultimate);
        }

        {
            auto& member2 = member["Doppelganger"];
            auto& config2 = config.Doppelganger;

            SetArray<uint8, 4>(member2["clone"], config2.clone);
        }

        {
            auto& member2 = member["Aura"];
            auto& config2 = config.Aura;

            SetArray2<uint8, 5, 4>(member2["dante"], config2.dante);
            SetArray<uint8, 4>(member2["sparda"], config2.sparda);
            SetArray2<uint8, 3, 4>(member2["vergil"], config2.vergil);
            SetArray<uint8, 4>(member2["neroAngelo"], config2.neroAngelo);
        }
    }


    {
        auto& member = root["EbonyIvory"];
        auto& config = config_.EbonyIvory;

        Set<bool>(member["foursomeTime"], config.foursomeTime);
        Set<bool>(member["infiniteRainStorm"], config.infiniteRainStorm);
    }


    {
        auto& member = root["Rebellion"];
        auto& config = config_.Rebellion;

        Set<bool>(member["infiniteSwordPierce"], config.infiniteSwordPierce);
    }


    {
        auto& member = root["Royalguard"];
        auto& config = config_.Royalguard;

        Set<bool>(member["forceJustFrameRelease"], config.forceJustFrameRelease);
    }


    {
        auto& member = root["Speed"];
        auto& config = config_.Speed;

        Set<float>(member["mainSpeed"], config.mainSpeed);
        Set<float>(member["turbo"], config.turbo);
        Set<float>(member["enemy"], config.enemy);
        Set<float>(member["quicksilverPlayerActor"], config.quicksilverPlayerActor);
        Set<float>(member["quicksilverEnemyActor"], config.quicksilverEnemyActor);
        Set<float>(member["human"], config.human);

        SetArray<float, 6>(member["devilDante"], config.devilDante);
        SetArray<float, 5>(member["devilVergil"], config.devilVergil);
    }


    {
        auto& member = root["SummonedSwords"];
        auto& config = config_.SummonedSwords;

        Set<bool>(member["chronoSwords"], config.chronoSwords);
    }


    {
        auto& member = root["Yamato"];
        auto& config = config_.Yamato;

        SetArray<uint8, 2>(member["judgementCutCount"], config.judgementCutCount);
    }


    {
        auto& member = root["YamatoForceEdge"];
        auto& config = config_.YamatoForceEdge;

        Set<bool>(member["infiniteRoundTrip"], config.infiniteRoundTrip);
    }


    auto& member = root;
    auto& config = config_;

    Set<bool>(member["welcome"], config.welcome);
    Set<bool>(member["hideBeowulfDante"], config.hideBeowulfDante);
    Set<bool>(member["hideBeowulfVergil"], config.hideBeowulfVergil);
    Set<bool>(member["airHikeCoreAbility"], config.airHikeCoreAbility);
    Set<uint8>(member["crazyComboLevelMultiplier"], config.crazyComboLevelMultiplier);
    Set<uint8>(member["dotShadow"], config.dotShadow);
    Set<float>(member["depleteQuicksilver"], config.depleteQuicksilver);
    Set<float>(member["depleteDoppelganger"], config.depleteDoppelganger);
    Set<float>(member["depleteDevil"], config.depleteDevil);
    Set<bool>(member["noDevilForm"], config.noDevilForm);
    Set<float>(member["orbReach"], config.orbReach);
    Set<bool>(member["resetPermissions"], config.resetPermissions);
    Set<bool>(member["infiniteHitPoints"], config.infiniteHitPoints);
    Set<bool>(member["infiniteMagicPoints"], config.infiniteMagicPoints);
    Set<bool>(member["disableTimer"], config.disableTimer);
    Set<bool>(member["infiniteBullets"], config.infiniteBullets);
    Set<float>(member["linearWeaponSwitchTimeout"], config.linearWeaponSwitchTimeout);

    SetArray<uint8, 2>(member["airHikeCount"], config.airHikeCount);
    SetArray<uint8, 2>(member["kickJumpCount"], config.kickJumpCount);
    SetArray<uint8, 2>(member["wallHikeCount"], config.wallHikeCount);
    SetArray<uint8, 2>(member["dashCount"], config.dashCount);
    SetArray<uint8, 2>(member["skyStarCount"], config.skyStarCount);
    SetArray<uint8, 2>(member["airTrickCountDante"], config.airTrickCountDante);
    SetArray<uint8, 2>(member["airTrickCountVergil"], config.airTrickCountVergil);
    SetArray<uint8, 2>(member["trickUpCount"], config.trickUpCount);
    SetArray<uint8, 2>(member["trickDownCount"], config.trickDownCount);

    SetArray<float, CHANNEL::MAX>(member["channelVolumes"], config.channelVolumes);


    ToJSON_WeaponSwitchControllerTextureData(
        member["meleeWeaponSwitchControllerTextureData"], config.meleeWeaponSwitchControllerTextureData);
    ToJSON_WeaponSwitchControllerTextureData(
        member["rangedWeaponSwitchControllerTextureData"], config.rangedWeaponSwitchControllerTextureData);


    Set<bool>(member["forceIconFocus"], config.forceIconFocus);
    Set<float>(member["damagePlayerActorMultiplier"], config.damagePlayerActorMultiplier);
    Set<float>(member["damageEnemyActorMultiplier"], config.damageEnemyActorMultiplier);
    Set<uint32>(member["damageStyleRank"], config.damageStyleRank);
    Set<bool>(member["skipIntro"], config.skipIntro);
    Set<bool>(member["skipCutscenes"], config.skipCutscenes);
    Set<bool>(member["enableFileMods"], config.enableFileMods);
    Set<float>(member["frameRate"], config.frameRate);
    Set<uint8>(member["vSync"], config.vSync);
    Set<bool>(member["hideMouseCursor"], config.hideMouseCursor);
    Set<int32>(member["windowPosX"], config.windowPosX);
    Set<int32>(member["windowPosY"], config.windowPosY);
    Set<bool>(member["forceWindowFocus"], config.forceWindowFocus);
    Set<float>(member["globalScale"], config.globalScale);


    Set<uint8>(member["enemyCount"], config.enemyCount);

    for_all(index, 30) {
        ToJSON_ConfigCreateEnemyActorData(member["configCreateEnemyActorData"][index], config.configCreateEnemyActorData[index]);
    }

    Set<bool>(member["enemyAutoSpawn"], config.enemyAutoSpawn);


    ToJSON_MainOverlayData(member["debugOverlayData"], config.debugOverlayData);
    ToJSON_OverlayData(member["missionOverlayData"], config.missionOverlayData);
    ToJSON_OverlayData(member["bossLadyActionsOverlayData"], config.bossLadyActionsOverlayData);
    ToJSON_OverlayData(member["bossVergilActionsOverlayData"], config.bossVergilActionsOverlayData);


    Set<float>(member["kalinaAnnHookGrenadeHeight"], config.kalinaAnnHookGrenadeHeight);
    Set<float>(member["kalinaAnnHookGrenadeTime"], config.kalinaAnnHookGrenadeTime);
    Set<vec4>(member["kalinaAnnHookMultiplier"], config.kalinaAnnHookMultiplier);
    Set<bool>(member["enableBossLadyFixes"], config.enableBossLadyFixes);
    Set<bool>(member["enableBossVergilFixes"], config.enableBossVergilFixes);
    Set<bool>(member["enablePVPFixes"], config.enablePVPFixes);
    Set<bool>(member["hideMainHUD"], config.hideMainHUD);
    Set<bool>(member["hideLockOn"], config.hideLockOn);
    Set<bool>(member["hideBossHUD"], config.hideBossHUD);
    Set<bool>(member["disableStyleRankHudFadeout"], config.disableStyleRankHudFadeout);
    Set<bool>(member["soundIgnoreEnemyData"], config.soundIgnoreEnemyData);
    Set<bool>(member["enableRebellionNewDrive"], config.enableRebellionNewDrive);
    Set<bool>(member["enableRebellionQuickDrive"], config.enableRebellionQuickDrive);
    Set<bool>(member["enableCerberusAirRevolver"], config.enableCerberusAirRevolver);
    Set<bool>(member["enableNevanNewVortex"], config.enableNevanNewVortex);
    Set<bool>(member["enableYamatoVergilNewJudgementCut"], config.enableYamatoVergilNewJudgementCut);
    Set<bool>(member["enableBeowulfVergilAirRisingSun"], config.enableBeowulfVergilAirRisingSun);
    Set<bool>(member["enableBeowulfVergilAirLunarPhase"], config.enableBeowulfVergilAirLunarPhase);
    Set<bool>(member["enableYamatoForceEdgeNewComboPart4"], config.enableYamatoForceEdgeNewComboPart4);
    Set<bool>(member["enableYamatoForceEdgeNewRoundTrip"], config.enableYamatoForceEdgeNewRoundTrip);
    Set<uint8>(member["dergil"], config.dergil);
    Set<uint8>(member["costumeRespectsProgression"], config.costumeRespectsProgression);
    Set<bool>(member["weaponWheelEnabled"], config.weaponWheelEnabled);
    Set<bool>(member["weaponWheelAnalogSelectionEnabled"], config.weaponWheelAnalogSelectionEnabled);
    Set<bool>(member["weaponWheelAnalogRightStick"], config.weaponWheelAnalogRightStick);
    Set<bool>(member["weaponWheelDisableCameraRotation"], config.weaponWheelDisableCameraRotation);
    Set<float>(member["weaponWheelScaleMultiplier"], config.weaponWheelScaleMultiplier);
    Set<float>(member["weaponWheelHeight"], config.weaponWheelHeight);
    Set<float>(member["weaponWheelHorizontalMelee"], config.weaponWheelHorizontalMelee);
    Set<float>(member["weaponWheelHorizontalRanged"], config.weaponWheelHorizontalRanged);

    {
        auto& member = root["MeleeWeaponWheel"];
        auto& config = config_.MeleeWeaponWheel;

        Set<bool>(member["alwaysShow"], config.alwaysShow);
        Set<uint32>(member["timeout"], config.timeout);
    }

    {
        auto& member = root["RangedWeaponWheel"];
        auto& config = config_.RangedWeaponWheel;

        Set<bool>(member["alwaysShow"], config.alwaysShow);
        Set<uint32>(member["timeout"], config.timeout);
    }


    SetString(member["selectedHUD"], config.selectedHUD.c_str());

	Set<bool>(member["disableBlendingEffects"], config.disableBlendingEffects);
	Set<bool>(member["framerateResponsiveGameSpeed"], config.framerateResponsiveGameSpeed);

    SetArray<uint8, 2>(member["beowulfVergilAirRisingSunCount"], config.beowulfVergilAirRisingSunCount);

    Set<bool>(member["forceVisibleHUD"], config.forceVisibleHUD);


    for_all(playerIndex, PLAYER_COUNT) {
        ToJSON_BarsData(member["barsData"][playerIndex], config.barsData[playerIndex]);
    }
    
    Set<bool>(member["forceSyncHitMagicPoints"], config.forceSyncHitMagicPoints);
    Set<bool>(member["updateLockOns"], config.updateLockOns);
    Set<bool>(member["showCredits"], config.showCredits);


    for_all(index, 3) {
        ToJSON_KeyData(member["keyData"][index], config.keyData[index]);
    }


    Set<bool>(member["absoluteUnit"], config.absoluteUnit);
    SetString(member["gamepadName"], config.gamepadName);
    Set<byte8>(member["gamepadButton"], config.gamepadButton);
    Set<bool>(member["disablePlayerActorIdleTimer"], config.disablePlayerActorIdleTimer);
    Set<bool>(member["rebellionInfiniteShredder"], config.rebellionInfiniteShredder);
    Set<bool>(member["rebellionHoldDrive"], config.rebellionHoldDrive);
}

#pragma endregion

#pragma region ToConfig

void ToConfig_CharacterData(CharacterData& config, rapidjson::Value& member) {
    config.character           = Get<uint8>(member["character"]);
    config.costume             = Get<uint8>(member["costume"]);
    config.ignoreCostume       = Get<bool>(member["ignoreCostume"]);
    config.forceFiles          = Get<bool>(member["forceFiles"]);
    config.forceFilesCharacter = Get<uint8>(member["forceFilesCharacter"]);
    config.forceFilesCostume   = Get<uint8>(member["forceFilesCostume"]);


    GetArray2<uint8, STYLE_COUNT, 2>(config.styles, member["styles"]);
    GetArray<uint8, STYLE_COUNT>(config.styleIndices, member["styleIndices"]);
    GetArray<byte16, STYLE_COUNT>(config.styleButtons, member["styleButtons"]);

    config.styleButtonIndex = Get<uint8>(member["styleButtonIndex"]);


    config.meleeWeaponCount = Get<uint8>(member["meleeWeaponCount"]);

    GetArray<uint8, MELEE_WEAPON_COUNT>(config.meleeWeapons, member["meleeWeapons"]);

    config.meleeWeaponIndex       = Get<uint8>(member["meleeWeaponIndex"]);
    config.lastMeleeWeaponIndex   = Get<uint8>(member["lastMeleeWeaponIndex"]);
    config.meleeWeaponSwitchType  = Get<uint8>(member["meleeWeaponSwitchType"]);
    config.meleeWeaponSwitchStick = Get<uint8>(member["meleeWeaponSwitchStick"]);


    config.rangedWeaponCount = Get<uint8>(member["rangedWeaponCount"]);

    GetArray<uint8, RANGED_WEAPON_COUNT>(config.rangedWeapons, member["rangedWeapons"]);

    config.rangedWeaponIndex       = Get<uint8>(member["rangedWeaponIndex"]);
    config.lastRangedWeaponIndex   = Get<uint8>(member["lastRangedWeaponIndex"]);
    config.rangedWeaponSwitchType  = Get<uint8>(member["rangedWeaponSwitchType"]);
    config.rangedWeaponSwitchStick = Get<uint8>(member["rangedWeaponSwitchStick"]);
}

void ToConfig_PlayerData(PlayerData& config, rapidjson::Value& member) {
    config.collisionGroup       = Get<uint8>(member["collisionGroup"]);
    config.switchButton         = Get<byte16>(member["switchButton"]);
    config.characterCount       = Get<uint8>(member["characterCount"]);
    config.characterIndex       = Get<uint8>(member["characterIndex"]);
    config.lastCharacterIndex   = Get<uint8>(member["lastCharacterIndex"]);
    config.activeCharacterIndex = Get<uint8>(member["activeCharacterIndex"]);


    for_all(characterIndex, CHARACTER_COUNT) {
        for_all(entityIndex, ENTITY_COUNT) {
            ToConfig_CharacterData(config.characterData[characterIndex][entityIndex], member["characterData"][characterIndex][entityIndex]);
        }
    }


    config.removeBusyFlag = Get<bool>(member["removeBusyFlag"]);

    GetArray<byte16, 4>(config.removeBusyFlagButtons, member["removeBusyFlagButtons"]);
}

void ToConfig_Vec2(vec2& config, rapidjson::Value& member) {
    config.x = Get<float>(member["x"]);
    config.y = Get<float>(member["y"]);
}

void ToConfig_Vec4(vec4& config, rapidjson::Value& member) {
    config.x = Get<float>(member["x"]);
    config.y = Get<float>(member["y"]);
    config.z = Get<float>(member["z"]);
    config.a = Get<float>(member["a"]);
}


void ToConfig_TextureData(Config::TextureData& config, rapidjson::Value& member) {
    ToConfig_Vec2(config.size, member["size"]);
    ToConfig_Vec2(config.pos, member["pos"]);
}

void ToConfig_TextureDataArray(Config::TextureData (&config)[5], rapidjson::Value& member) {
    for_all(index, 5) {
        auto& config2 = config[index];
        auto& member2 = member[index];

        ToConfig_TextureData(config2, member2);
    }
}

void ToConfig_WeaponSwitchControllerTextureData(Config::WeaponSwitchControllerTextureData& config, rapidjson::Value& member) {
    ToConfig_TextureDataArray(config.backgrounds, member["backgrounds"]);
    ToConfig_TextureDataArray(config.icons, member["icons"]);
    ToConfig_TextureDataArray(config.highlights, member["highlights"]);
    ToConfig_TextureData(config.arrow, member["arrow"]);
}


void ToConfig_ConfigCreateEnemyActorData(ConfigCreateEnemyActorData& config, rapidjson::Value& member) {
    config.enemy   = Get<uint32>(member["enemy"]);
    config.variant = Get<uint32>(member["variant"]);

    ToConfig_Vec4(config.position, member["position"]);

    config.rotation         = Get<uint16>(member["rotation"]);
    config.useMainActorData = Get<bool>(member["useMainActorData"]);
    config.spawnMethod      = Get<uint16>(member["spawnMethod"]);
}


void ToConfig_OverlayData(Config::OverlayData& config, rapidjson::Value& member) {
    config.enable = Get<bool>(member["enable"]);

    ToConfig_Vec2(config.pos, member["pos"]);

    GetArray<float, 4>(config.color, member["color"]);
}

void ToConfig_MainOverlayData(Config::MainOverlayData& config, rapidjson::Value& member) {
    ToConfig_OverlayData(config, member);

    config.showFocus               = Get<bool>(member["showFocus"]);
    config.showFPS                 = Get<bool>(member["showFPS"]);
    config.showSizes               = Get<bool>(member["showSizes"]);
    config.showFrameRateMultiplier = Get<bool>(member["showFrameRateMultiplier"]);
    config.showEventData           = Get<bool>(member["showEventData"]);
    config.showPosition            = Get<bool>(member["showPosition"]);
    config.showRegionData          = Get<bool>(member["showRegionData"]);
}


void ToConfig_BarsData(Config::BarsData& config, rapidjson::Value& member) {
	defaultConfig.barsData[1].pos = { 938, 59 };
	defaultConfig.barsData[2].pos = { 1191, 59 };
	defaultConfig.barsData[3].pos = { 1191, 129 };

    config.enable = Get<bool>(member["enable"]);
    config.run    = Get<bool>(member["run"]);

    ToConfig_Vec2(config.size, member["size"]);
    ToConfig_Vec2(config.pos, member["pos"]);

    config.lastX = Get<uint32>(member["lastX"]);
    config.lastY = Get<uint32>(member["lastY"]);

    GetArray<float, 4>(config.hitColor, member["hitColor"]);
    GetArray<float, 4>(config.magicColor, member["magicColor"]);
}


void ToConfig_KeyData(KeyData& config, rapidjson::Value& member) {
    GetArray<byte8, 4>(config.keys, member["keys"]);

    config.keyCount = Get<new_size_t>(member["keyCount"]);
}


void ToConfig(Config& config_) {
    DebugLogFunction();

    {
        auto& config = config_.Actor;
        auto& member = root["Actor"];

        config.enable      = Get<bool>(member["enable"]);
        config.playerCount = Get<uint8>(member["playerCount"]);

        for_all(playerIndex, PLAYER_COUNT) {
            ToConfig_PlayerData(config.playerData[playerIndex], member["playerData"][playerIndex]);
        }
    }


    {
        auto& config = config_.Arcade;
        auto& member = root["Arcade"];

        config.enable         = Get<bool>(member["enable"]);
        config.mission        = Get<uint32>(member["mission"]);
        config.mode           = Get<uint32>(member["mode"]);
        config.room           = Get<uint32>(member["room"]);
        config.enableRoomSelection = Get<bool>(member["enableRoomSelection"]);
        config.position       = Get<uint32>(member["position"]);
        config.enablePositionSelection = Get<bool>(member["enablePositionSelection"]);
        config.floor          = Get<uint8>(member["floor"]);
        config.level          = Get<uint16>(member["level"]);
        config.hitPoints      = Get<float>(member["hitPoints"]);
        config.magicPoints    = Get<float>(member["magicPoints"]);
        config.character      = Get<uint8>(member["character"]);
        config.costume        = Get<uint8>(member["costume"]);
        config.style          = Get<uint32>(member["style"]);

        GetArray<uint8, 4>(config.weapons, member["weapons"]);
    }


    {
        auto& config = config_.Artemis;
        auto& member = root["Artemis"];

        config.swapNormalShotAndMultiLock = Get<bool>(member["swapNormalShotAndMultiLock"]);
        config.instantFullCharge          = Get<bool>(member["instantFullCharge"]);
    }


    {
        auto& config = config_.BossRush;
        auto& member = root["BossRush"];

        config.enable = Get<bool>(member["enable"]);

        {
            auto& config2 = config.Mission5;
            auto& member2 = member["Mission5"];

            config2.skipJester = Get<bool>(member2["skipJester"]);
        }

        {
            auto& config2 = config.Mission12;
            auto& member2 = member["Mission12"];

            config2.skipJester      = Get<bool>(member2["skipJester"]);
            config2.skipGeryonPart1 = Get<bool>(member2["skipGeryonPart1"]);
        }

        {
            auto& config2 = config.Mission17;
            auto& member2 = member["Mission17"];

            config2.skipJester = Get<bool>(member2["skipJester"]);
        }

        {
            auto& config2 = config.Mission19;
            auto& member2 = member["Mission19"];

            config2.skipArkhamPart1 = Get<bool>(member2["skipArkhamPart1"]);
        }
    }


    {

        auto& config = config_.Color;
        auto& member = root["Color"];

        GetArray2<uint8, 5, 4>(config.airHike, member["airHike"]);

        {
            auto& config2 = config.Trickster;
            auto& member2 = member["Trickster"];

            GetArray<uint8, 4>(config2.skyStar, member2["skyStar"]);
        }

        {
            auto& config2 = config.Royalguard;
            auto& member2 = member["Royalguard"];

            GetArray<uint8, 4>(config2.ultimate, member2["ultimate"]);
        }

        {
            auto& config2 = config.Doppelganger;
            auto& member2 = member["Doppelganger"];

            GetArray<uint8, 4>(config2.clone, member2["clone"]);
        }

        {
            auto& config2 = config.Aura;
            auto& member2 = member["Aura"];

            GetArray2<uint8, 5, 4>(config2.dante, member2["dante"]);
            GetArray<uint8, 4>(config2.sparda, member2["sparda"]);
            GetArray2<uint8, 3, 4>(config2.vergil, member2["vergil"]);
            GetArray<uint8, 4>(config2.neroAngelo, member2["neroAngelo"]);
        }
    }


    {
        auto& config = config_.EbonyIvory;
        auto& member = root["EbonyIvory"];

        config.foursomeTime      = Get<bool>(member["foursomeTime"]);
        config.infiniteRainStorm = Get<bool>(member["infiniteRainStorm"]);
    }


    {
        auto& config = config_.Rebellion;
        auto& member = root["Rebellion"];

        config.infiniteSwordPierce = Get<bool>(member["infiniteSwordPierce"]);
    }


    {
        auto& config = config_.Royalguard;
        auto& member = root["Royalguard"];

        config.forceJustFrameRelease = Get<bool>(member["forceJustFrameRelease"]);
    }


    {
        auto& config = config_.Speed;
        auto& member = root["Speed"];

        config.mainSpeed              = Get<float>(member["mainSpeed"]);
        config.turbo                  = Get<float>(member["turbo"]);
        config.enemy                  = Get<float>(member["enemy"]);
        config.quicksilverPlayerActor = Get<float>(member["quicksilverPlayerActor"]);
        config.quicksilverEnemyActor  = Get<float>(member["quicksilverEnemyActor"]);
        config.human                  = Get<float>(member["human"]);

        GetArray<float, 6>(config.devilDante, member["devilDante"]);
        GetArray<float, 5>(config.devilVergil, member["devilVergil"]);
    }


    {
        auto& config = config_.SummonedSwords;
        auto& member = root["SummonedSwords"];

        config.chronoSwords = Get<bool>(member["chronoSwords"]);
    }


    {
        auto& config = config_.Yamato;
        auto& member = root["Yamato"];

        GetArray<uint8, 2>(config.judgementCutCount, member["judgementCutCount"]);
    }


    {
        auto& config = config_.YamatoForceEdge;
        auto& member = root["YamatoForceEdge"];

        config.infiniteRoundTrip = Get<bool>(member["infiniteRoundTrip"]);
    }


    auto& config = config_;
    auto& member = root;

    config.welcome                   = Get<bool>(member["welcome"]);
    config.hideBeowulfDante          = Get<bool>(member["hideBeowulfDante"]);
    config.hideBeowulfVergil         = Get<bool>(member["hideBeowulfVergil"]);
    config.airHikeCoreAbility        = Get<bool>(member["airHikeCoreAbility"]);
    config.crazyComboLevelMultiplier = Get<uint8>(member["crazyComboLevelMultiplier"]);
    config.dotShadow                 = Get<uint8>(member["dotShadow"]);
    config.depleteQuicksilver        = Get<float>(member["depleteQuicksilver"]);
    config.depleteDoppelganger       = Get<float>(member["depleteDoppelganger"]);
    config.depleteDevil              = Get<float>(member["depleteDevil"]);
    config.noDevilForm               = Get<bool>(member["noDevilForm"]);
    config.orbReach                  = Get<float>(member["orbReach"]);
    config.resetPermissions          = Get<bool>(member["resetPermissions"]);
    config.infiniteHitPoints         = Get<bool>(member["infiniteHitPoints"]);
    config.infiniteMagicPoints       = Get<bool>(member["infiniteMagicPoints"]);
    config.disableTimer              = Get<bool>(member["disableTimer"]);
    config.infiniteBullets           = Get<bool>(member["infiniteBullets"]);
    config.linearWeaponSwitchTimeout = Get<float>(member["linearWeaponSwitchTimeout"]);

    GetArray<uint8, 2>(config.airHikeCount, member["airHikeCount"]);
    GetArray<uint8, 2>(config.kickJumpCount, member["kickJumpCount"]);
    GetArray<uint8, 2>(config.wallHikeCount, member["wallHikeCount"]);
    GetArray<uint8, 2>(config.dashCount, member["dashCount"]);
    GetArray<uint8, 2>(config.skyStarCount, member["skyStarCount"]);
    GetArray<uint8, 2>(config.airTrickCountDante, member["airTrickCountDante"]);
    GetArray<uint8, 2>(config.airTrickCountVergil, member["airTrickCountVergil"]);
    GetArray<uint8, 2>(config.trickUpCount, member["trickUpCount"]);
    GetArray<uint8, 2>(config.trickDownCount, member["trickDownCount"]);

    GetArray<float, CHANNEL::MAX>(config.channelVolumes, member["channelVolumes"]);

    ToConfig_WeaponSwitchControllerTextureData(
        config.meleeWeaponSwitchControllerTextureData, member["meleeWeaponSwitchControllerTextureData"]);
    ToConfig_WeaponSwitchControllerTextureData(
        config.rangedWeaponSwitchControllerTextureData, member["rangedWeaponSwitchControllerTextureData"]);

    config.forceIconFocus              = Get<bool>(member["forceIconFocus"]);
    config.damagePlayerActorMultiplier = Get<float>(member["damagePlayerActorMultiplier"]);
    config.damageEnemyActorMultiplier  = Get<float>(member["damageEnemyActorMultiplier"]);
    config.damageStyleRank             = Get<uint32>(member["damageStyleRank"]);
    config.skipIntro                   = Get<bool>(member["skipIntro"]);
    config.skipCutscenes               = Get<bool>(member["skipCutscenes"]);
    config.enableFileMods            = Get<bool>(member["enableFileMods"]);
    config.frameRate                   = Get<float>(member["frameRate"]);
    config.vSync                       = Get<uint8>(member["vSync"]);
    config.hideMouseCursor             = Get<bool>(member["hideMouseCursor"]);
    config.windowPosX                  = Get<int32>(member["windowPosX"]);
    config.windowPosY                  = Get<int32>(member["windowPosY"]);
    config.forceWindowFocus            = Get<bool>(member["forceWindowFocus"]);
    config.globalScale                 = Get<float>(member["globalScale"]);


    config.enemyCount = Get<uint8>(member["enemyCount"]);

    for_all(index, 30) {
        ToConfig_ConfigCreateEnemyActorData(config.configCreateEnemyActorData[index], member["configCreateEnemyActorData"][index]);
    }

    config.enemyAutoSpawn = Get<bool>(member["enemyAutoSpawn"]);


    ToConfig_MainOverlayData(config.debugOverlayData, member["debugOverlayData"]);
    ToConfig_OverlayData(config.missionOverlayData, member["missionOverlayData"]);
    ToConfig_OverlayData(config.bossLadyActionsOverlayData, member["bossLadyActionsOverlayData"]);
    ToConfig_OverlayData(config.bossVergilActionsOverlayData, member["bossVergilActionsOverlayData"]);


    config.kalinaAnnHookGrenadeHeight = Get<float>(member["kalinaAnnHookGrenadeHeight"]);
    config.kalinaAnnHookGrenadeTime   = Get<float>(member["kalinaAnnHookGrenadeTime"]);

    ToConfig_Vec4(config.kalinaAnnHookMultiplier, member["kalinaAnnHookMultiplier"]);

    config.enableBossLadyFixes                = Get<bool>(member["enableBossLadyFixes"]);
    config.enableBossVergilFixes              = Get<bool>(member["enableBossVergilFixes"]);
    config.enablePVPFixes                     = Get<bool>(member["enablePVPFixes"]);
    config.hideMainHUD                        = Get<bool>(member["hideMainHUD"]);
    config.hideLockOn                         = Get<bool>(member["hideLockOn"]);
    config.disableStyleRankHudFadeout = Get<bool>(member["disableStyleRankHudFadeout"]);
    config.soundIgnoreEnemyData               = Get<bool>(member["soundIgnoreEnemyData"]);
    config.enableRebellionNewDrive            = Get<bool>(member["enableRebellionNewDrive"]);
    config.enableRebellionQuickDrive          = Get<bool>(member["enableRebellionQuickDrive"]);
    config.enableCerberusAirRevolver          = Get<bool>(member["enableCerberusAirRevolver"]);
    config.enableNevanNewVortex               = Get<bool>(member["enableNevanNewVortex"]);
    config.enableYamatoVergilNewJudgementCut  = Get<bool>(member["enableYamatoVergilNewJudgementCut"]);
    config.enableBeowulfVergilAirRisingSun    = Get<bool>(member["enableBeowulfVergilAirRisingSun"]);
    config.enableBeowulfVergilAirLunarPhase   = Get<bool>(member["enableBeowulfVergilAirLunarPhase"]);
    config.enableYamatoForceEdgeNewComboPart4 = Get<bool>(member["enableYamatoForceEdgeNewComboPart4"]);
    config.enableYamatoForceEdgeNewRoundTrip  = Get<bool>(member["enableYamatoForceEdgeNewRoundTrip"]);
    config.dergil                             = Get<uint8>(member["dergil"]);
    config.costumeRespectsProgression         = Get<uint8>(member["costumeRespectsProgression"]);
    config.weaponWheelEnabled                 = Get<bool>(member["weaponWheelEnabled"]);
    config.weaponWheelAnalogSelectionEnabled  = Get<bool>(member["weaponWheelAnalogSelectionEnabled"]);
    config.weaponWheelAnalogRightStick        = Get<bool>(member["weaponWheelAnalogRightStick"]);
    config.weaponWheelDisableCameraRotation   = Get<bool>(member["weaponWheelDisableCameraRotation"]);
    config.weaponWheelScaleMultiplier         = Get<float>(member["weaponWheelScaleMultiplier"]);
    config.weaponWheelHeight                  = Get<float>(member["weaponWheelHeight"]);
    config.weaponWheelHorizontalMelee         = Get<float>(member["weaponWheelHorizontalMelee"]);
    config.weaponWheelHorizontalRanged        = Get<float>(member["weaponWheelHorizontalRanged"]);

    {
        auto& config = config_.MeleeWeaponWheel;
        auto& member = root["MeleeWeaponWheel"];

        config.alwaysShow = Get<bool>(member["alwaysShow"]);
        config.timeout    = Get<uint32>(member["timeout"]);
    }

    {
        auto& config = config_.RangedWeaponWheel;
        auto& member = root["RangedWeaponWheel"];

        config.alwaysShow = Get<bool>(member["alwaysShow"]);
        config.timeout    = Get<uint32>(member["timeout"]);
    }

    config.selectedHUD = Get<std::string>(member["selectedHUD"]);

    //GetString(config.selectedHUD, sizeof(config.selectedHUD), member["selectedHUD"]);


	config.disableBlendingEffects = Get<bool>(member["disableBlendingEffects"]);
	config.framerateResponsiveGameSpeed = Get<bool>(member["framerateResponsiveGameSpeed"]);

    GetArray<uint8, 2>(config.beowulfVergilAirRisingSunCount, member["beowulfVergilAirRisingSunCount"]);

    config.forceVisibleHUD = Get<bool>(member["forceVisibleHUD"]);


    for_all(playerIndex, PLAYER_COUNT) {
        ToConfig_BarsData(config.barsData[playerIndex], member["barsData"][playerIndex]);
    }


    config.forceSyncHitMagicPoints = Get<bool>(member["forceSyncHitMagicPoints"]);
    config.updateLockOns           = Get<bool>(member["updateLockOns"]);
    config.showCredits             = Get<bool>(member["showCredits"]);


    for_all(index, 3) {
        ToConfig_KeyData(config.keyData[index], member["keyData"][index]);
    }


    config.absoluteUnit = Get<bool>(member["absoluteUnit"]);

    GetString(config.gamepadName, sizeof(config.gamepadName), member["gamepadName"]);

    config.gamepadButton               = Get<byte8>(member["gamepadButton"]);
    config.disablePlayerActorIdleTimer = Get<bool>(member["disablePlayerActorIdleTimer"]);
    config.rebellionInfiniteShredder   = Get<bool>(member["rebellionInfiniteShredder"]);
    config.rebellionHoldDrive          = Get<bool>(member["rebellionHoldDrive"]);
}

#pragma endregion

}; // namespace JSON


#define NO_INIT
#include "Global/ConfigFunctions.h"

void InitConfig() {
    LogFunction();

    InitConfigFunction();


    old_for_all(uint8, playerIndex, PLAYER_COUNT) {
        ApplyDefaultPlayerData(defaultConfig.Actor.playerData[playerIndex]);
        ApplyDefaultPlayerData(queuedConfig.Actor.playerData[playerIndex]);
        ApplyDefaultPlayerData(activeConfig.Actor.playerData[playerIndex]);
    }


    DebugLog("kalinaAnnHookGrenadeHeight %llX", offsetof(Config, kalinaAnnHookGrenadeHeight));
    DebugLog("kalinaAnnHookGrenadeTime   %llX", offsetof(Config, kalinaAnnHookGrenadeTime));
    DebugLog("kalinaAnnHookMultiplier    %llX", offsetof(Config, kalinaAnnHookMultiplier));
    DebugLog("barsData                   %llX", offsetof(Config, barsData));

    DebugLog("activeConfig.gamepadName %llX", activeConfig.gamepadName);
}

// Declare template functions
// template <typename Tuple, typename Func, std::size_t... I>
// void for_each_impl(Tuple&& tuple, Func&& func, std::index_sequence<I...>) {
// 	(func(std::get<I>(tuple)), ...);
// }
// 
// template <typename Tuple, typename Func>
// void for_each_tuple(Tuple&& tuple, Func&& func) {
// 	constexpr auto size = std::tuple_size<std::decay_t<Tuple>>::value;
// 	for_each_impl(std::forward<Tuple>(tuple), std::forward<Func>(func), std::make_index_sequence<size>{});
// }
// 
// template <typename T>
// inline void CreateMembersCrimson(rapidjson::Value& root, T& config) {
// 	auto metadata = T::Metadata();
// 	for_each_tuple(metadata, [&](const auto& field) {
// 		using FieldType = typename std::decay_t<decltype(config.*(field.second))>;
// 		auto& memberName = field.first;       // Access the first element of the pair
// 		auto& memberValue = field.second;     // Access the second element of the pair
// 		Create<FieldType>(root, memberName, config.*memberValue);
// 		});
// }
// 
// template <typename T>
// inline void ToJSONCrimson(rapidjson::Value& root, T& config) {
// 	auto metadata = T::Metadata();
// 	for_each_tuple(metadata, [&](const auto& field) {
// 		using FieldType = typename std::decay_t<decltype(config.*(field.second))>;
// 		auto& memberName = field.first;       // Access the first element of the pair
// 		auto& memberValue = field.second;     // Access the second element of the pair
// 		Set<FieldType>(root[memberName], config.*memberValue);
// 		});
// }
// 
// template <typename T>
// inline void ToConfigCrimson(rapidjson::Value& root, T& config) {
// 	auto metadata = T::Metadata();
// 	for_each_tuple(metadata, [&](const auto& field) {
// 		using FieldType = typename std::decay_t<decltype(config.*(field.second))>;
// 		auto& memberName = field.first;       // Access the first element of the pair
// 		auto& memberValue = field.second;     // Access the second element of the pair
// 		config.*memberValue = Get<FieldType>(root[memberName]);
// 		});
// }

namespace ExpConfig {
inline const char* directoryName = Paths::config;
inline const char* fileName      = "CrimsonExp.json";

char location[64] = {};

ExpData missionExpDataDante           = {};
ExpData sessionExpDataDante           = {};
ExpData savedExpDataDante[SAVE_COUNT] = {};

ExpData missionExpDataVergil           = {};
ExpData sessionExpDataVergil           = {};
ExpData savedExpDataVergil[SAVE_COUNT] = {};

inline bool Enable() {
    return activeConfig.Actor.enable;
}

bool Max() {
    return (activeConfig.absoluteUnit || activeConfig.Arcade.enable);
}

#pragma region JSON

namespace JSON {

#include "Core/JSON.h"


#pragma region CreateMembers

void CreateMembers_ExpData(rapidjson::Value& member, ExpData (&expData)[SAVE_COUNT]) {
    for_all(index, SAVE_COUNT) {
        auto& member2  = member[index];
        auto& expData2 = expData[index];

        CreateArray<uint32, STYLE::MAX>(member2, "styleLevels", expData2.styleLevels);
        CreateArray<float, STYLE::MAX>(member2, "styleExpPoints", expData2.styleExpPoints);
        CreateArray<bool, 64>(member2, "unlocks", expData2.unlocks);
        Create<bool>(member2, "hasPairedWithActorSystem", expData2.hasPairedWithActorSystem);
    }
}

void CreateMembers() {
    DebugLogFunction();

    CreateArray<struct_t, SAVE_COUNT>(root, "Dante");
    CreateMembers_ExpData(root["Dante"], savedExpDataDante);

    CreateArray<struct_t, SAVE_COUNT>(root, "Vergil");
    CreateMembers_ExpData(root["Vergil"], savedExpDataVergil);
}

#pragma endregion


#pragma region ToJSON

void ToJSON_ExpData(rapidjson::Value& member, ExpData (&expData)[SAVE_COUNT]) {
    for_all(index, SAVE_COUNT) {
        auto& member2  = member[index];
        auto& expData2 = expData[index];

        SetArray<uint32, STYLE::MAX>(member2["styleLevels"], expData2.styleLevels);
        SetArray<float, STYLE::MAX>(member2["styleExpPoints"], expData2.styleExpPoints);
        SetArray<bool, 64>(member2["unlocks"], expData2.unlocks);
        Set<bool>(member2["hasPairedWithActorSystem"], expData2.hasPairedWithActorSystem);
    }
}

void ToJSON() {
    DebugLogFunction();

    ToJSON_ExpData(root["Dante"], savedExpDataDante);
    ToJSON_ExpData(root["Vergil"], savedExpDataVergil);
}

#pragma endregion


#pragma region ToExpData

void ToExp_ExpData(ExpData (&expData)[SAVE_COUNT], rapidjson::Value& member) {
    for_all(index, SAVE_COUNT) {
        auto& member2  = member[index];
        auto& expData2 = expData[index];

        GetArray<uint32, STYLE::MAX>(expData2.styleLevels, member2["styleLevels"]);
        GetArray<float, STYLE::MAX>(expData2.styleExpPoints, member2["styleExpPoints"]);
        GetArray<bool, 64>(expData2.unlocks, member2["unlocks"]);
        expData2.hasPairedWithActorSystem = Get<bool>(member2["hasPairedWithActorSystem"]);
    }
}

void ToExp() {
    DebugLogFunction();

    ToExp_ExpData(savedExpDataDante, root["Dante"]);
    ToExp_ExpData(savedExpDataVergil, root["Vergil"]);
}

#pragma endregion


}; // namespace JSON

#pragma endregion

void SaveExp() {
    if (!Enable() || Max()) {
        return;
    }

    auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

    LogFunction();


    auto saveIndex = g_saveIndex;
    if (saveIndex >= SAVE_COUNT) {
        return;
    }

    if (g_scene == SCENE::GAME) {
        sessionExpDataDante  = missionExpDataDante;
        sessionExpDataVergil = missionExpDataVergil;
    }

    savedExpDataDante[saveIndex]  = sessionExpDataDante;
    savedExpDataVergil[saveIndex] = sessionExpDataVergil;

// 	if (sessionData.character == CHARACTER::DANTE) {
// 		sessionData.expertise[0] = 0xFFFF5E7F;
//         sessionData.expertise[1] = 0xA7FFAF5F;
//         sessionData.expertise[2] = 0xAF1FFFF3;
//         sessionData.expertise[3] = 0xCB9FFFF9;
//         sessionData.expertise[4] = 0xFBFBFFFE;
//         sessionData.expertise[5] = 0xFFFFEFFD;
//         sessionData.expertise[6] = 0xFFE3FEFF;
//         sessionData.expertise[7] = 0xFFFFFFFF;
// 	}

    using namespace rapidjson;
    using namespace JSON;


    ToJSON();


    StringBuffer stringBuffer;
    PrettyWriter<StringBuffer> prettyWriter(stringBuffer);

    root.Accept(prettyWriter);


    auto name = stringBuffer.GetString();
    auto size = strlen(name);

    if (!SaveFile(location, name, size)) {
        Log("SaveFile failed.");
    }
}

void LoadExp() {
    LogFunction();

    using namespace rapidjson;
    using namespace JSON;


    auto file = LoadFile(location);
    if (!file) {
        Log("LoadFile failed.");

        CreateMembers();

        SaveExp();

        return;
    }


    auto name = const_cast<const char*>(reinterpret_cast<char*>(file));

    if (root.Parse(name).HasParseError()) {
        Log("Parse failed.");

        return;
    }


    CreateMembers();

    // At this point all file members have been applied. Extra or obsolete file members can exist.
    // If members were missing in the file they were created and have their default values.


    // The actual configs are still untouched though.
    // Let's update them!

    ToExp();


    auto saveIndex = g_saveIndex;
    if (saveIndex >= SAVE_COUNT) {
        return;
    }

    sessionExpDataDante  = savedExpDataDante[saveIndex];
    sessionExpDataVergil = savedExpDataVergil[saveIndex];
}

void InitExp() {
    LogFunction();

    using namespace rapidjson;
    using namespace JSON;

    CreateDirectoryA(directoryName, 0);

    snprintf(location, sizeof(location), "%s/%s", directoryName, fileName);

    root.SetObject();

    g_allocator = &root.GetAllocator();
}

ExpertiseHelper expertiseHelpersDanteSwordmasterLevel2[] = {
    {0, 0x8000}, // Sword Pierce (0x4000)
    {1, 0x1000}, // Crystal
    {2, 0x4}, // Crawler
    {2, 0x4000000}, // Feedback
    {3, 0x30000000}, // Ground and Air Volcano (0x20000000)
};

ExpertiseHelper expertiseHelpersDanteSwordmasterLevel3[] = {
    {0, 0x8000}, // Sword Pierce (0x4000)
    {1, 0x5000}, // Crystal and Ice Age (0x4000)
    {2, 0x5000000C}, // Distortion (0x20000000) and Feedback (0x4000000); Crawler (0x4) and Tempest (0x8)
    {3, 0x30000000}, // Ground and Air Volcano
    {4, 0x1}, // Real Impact
    {6, 0x100}, // Dance Macabre
};

ExpertiseHelper expertiseHelpersDanteGunslingerLevel2[] = {
    {4, 0x4000000}, // Sphere (0x4000000)
    {5, 0x1000}, // Grapple (0x1000)
};

ExpertiseHelper expertiseHelpersDanteGunslingerLevel3[] = {
    {4, 0x4040000}, // Sphere and Shotgun Stinger (0x40000)
    {5, 0x1002}, // Grapple and Sniper (0x20)
};

ExpertiseHelper expertiseHelpersDante[] = {
    {0, 0x80},  // Stinger Level 1
    {0, 0x100}, // Stinger Level 2
    {0, 0x2000}, // Drive
    {6, 0x40000}, // Rebellion Air Hike

    {1, 0x40}, // Revolver Level 2
    {1, 0x20}, // Windmill

    {1, 0x4000000}, // Jet-Stream Level 2
    {1, 0x8000000}, // Jet-Stream Level 3
    {1, 0x40000000}, // Whirlwind
    {6, 0x80000}, // Agni and Rudra Air Hike

    {2, 0x400000}, // Reverb Shock Level 1
    {2, 0x800000}, // Reverb Shock Level 2
    {2, 0x200000}, // Bat-Rift Level 2
    {3, 4}, // Air Raid
    {3, 2}, // Volume Up

    {3, 0x2000000}, // Straight Level 2
    {3, 0x200000}, // Beast Uppercut
    {3, 0x400000}, // Rising Dragon
    {6, 0x100000}, // Beowulf Air Hike
};

ExpertiseHelper expertiseHelpersVergil[] = {
    {0, 0x10}, // Rapid Slash Level 1
    {0, 0x20}, // Rapid Slash Level 2
    {0, 0x200}, // Judgement Cut Level 1
    {0, 0x400}, // Judgement Cut Level 2

    {0, 0x800000}, // Starfall Level 2
    {0, 0x2000000}, // Rising Sun
    {0, 0x4000000}, // Lunar Phase Level 2

    {1, 4}, // Helm Breaker Level 2
    {1, 0x40}, // Stinger Level 1
    {1, 0x80}, // Stinger Level 2
    {1, 0x100}, // Round Trip

    {1, 0x40000}, // Summoned Swords Level 2
    {1, 0xC0000}, // Summoned Sword Level 3
    {1, 0x200000}, // Spiral Swords
};

// Documenting this shit has been a real pain the ass. - Mia
// here's how the bits are layed out for Danter:

// activeExpertise[0] - Most Rebellion moves
//                   |||| |||| |||| |||| |||| |||| +--------- Bit 7: Stinger Level 1
//                   |||| |||| |||| |||| |||| |||+----------- Bit 8: Stinger Level 2
//                   |||| |||| |||| ||+--------------------- Bit 13: Unknown (likely Drive (?))
//                   |||| |||| |||| +----------------------- Bit 15: Sword Pierce

// activeExpertise[1] - Mostost Cerberus moves and start of Agni and Rudra
//                   |||| |||| |||| |||| |||| |||| ||+------- Bit 5: Windmill
//                   |||| |||| |||| |||| |||| |||| +--------- Bit 7: Revolver Level 2
//                   |||| |||| |||| |||+-------------------- Bit 12: Crystal / Million Carats
//                   |||| |||| |||| |+---------------------- Bit 14: Ice Age
//                   |||| +--------------------------------- Bit 27: Jet Stream Level 2
//                   |||+----------------------------------- Bit 28: Jet Stream Level 3
//                   |+------------------------------------- Bit 30: Whirlwind

// activeExpertise[2] - Agni and Rudra Swordmaster moves and Nevan
//                   |||| |||| |||| |||| |||| |||| |||| |+---- Bit 2: Crawler
//                   |||| |||| |||| |||| |||| |||| |||| +---- Bit 3: Twister / Tempest
//                   |||| |||| ||+-------------------------- Bit 20: Bat Rift Level 2
//                   |||| |||| |+--------------------------- Bit 21: Reverb Shock Level 1
//                   |||| |||| +---------------------------- Bit 22: Reverb Shock Level 2
//                   |||+----------------------------------- Bit 27: Feedback / Crazy Roll
//                   |+------------------------------------- Bit 29: Distortion

// activeExpertise[3] - Rest of Nevan and Beowulf moves
//                   |||| |||| |||| |||| |||| |||| |||| ||+--- Bit 1: Volume Up
//                   |||| |||| |||| |||| |||| |||| |||| |+---- Bit 2: Air Raid
//                   |||| |||| ||+-------------------------- Bit 21: Beast Uppercut
//                   |||| |||| |+--------------------------- Bit 22: Rising Dragon
//                   |||| |+-------------------------------- Bit 26: Straight Level 2
//                   |||+----------------------------------- Bit 28: Ground Volcano
//                   ||+------------------------------------ Bit 29: Mid-Air Volcano

// activeExpertise[4] - Real Impact and Gunslinger Moves
//                   |||| |||| |||| |||| |||| |||| |||| |||+-- Bit 0: Real Impact
//                   |||| |||| |||| |+---------------------- Bit 18: Shotgun Stinger
//                   |||| |+-------------------------------- Bit 26: Sphere

// activeExpertise[5] - More Gunslinger Moves
//                   |||| |||| |||| |||| |||| |||| ||+------- Bit 5: Sniper / Reflector
//                   |||| |||| |||| |||| |||+--------------- Bit 12: Grapple

// activeExpertise[6] - Dance Macabre and Air Hikes
//                   |||| |||| |||| |||| |||| |||+----------- Bit 8: Dance Macabre 
//                   |||| |||| |||| |+---------------------- Bit 18: Rebellion Air Hike
//                   |||| |||| |||| +----------------------- Bit 19: Agni and Rudra Air Hike
//                   |||| |||| |||+------------------------- Bit 20: Beowulf Air Hike

struct LevelHelper {
    new_size_t index;
    uint32 level;
};

LevelHelper levelHelpers[] = {
    {WEAPON::EBONY_IVORY, 1},
    {WEAPON::EBONY_IVORY, 2},
    {WEAPON::SHOTGUN, 1},
    {WEAPON::SHOTGUN, 2},
    {WEAPON::ARTEMIS, 1},
    {WEAPON::ARTEMIS, 2},
    {WEAPON::SPIRAL, 1},
    {WEAPON::SPIRAL, 2},
    {WEAPON::KALINA_ANN, 1},
    {WEAPON::KALINA_ANN, 2},
};

std::vector<byte32> originalExpertise = {
	0b1111'1111'1111'1111'0101'1110'0111'1111, // 0xffff5e7f  // lacking sword pierce
	0b1010'0111'1111'1111'1010'1111'0101'1111, // 0xa7ffaf5f  // lacking cerberus abilities
	0b1010'1111'0001'1111'1111'1111'1111'0011, // 0xaf1ffff3  // lacking nevan and agni
	0b1100'1011'1001'1111'1111'1111'1111'1001, // 0xcb9ffff9  // lacking beowulf forward
	0b1111'1011'1111'1011'1111'1111'1111'1110, // 0xfbfbfffe  // lacking gunslinger artemis
	0b1111'1111'1111'1111'1110'1111'1101'1111, // 0xffffefdf  // lacking Trick lvl 2 and rest of the gunslinger abilities
	0b1111'1111'1110'0011'1111'1110'1111'1111, // 0xffe3feff  // lacking real impact and dance macabre
	0b1111'1111'1111'1111'1111'1111'1111'1111  // 0xffffffff  
};


void SavePlayerActorExp() {
    if (!Enable() || Max()) {
        return;
    }

	auto name_7058 = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E30);

	auto& missionData = *reinterpret_cast<MissionData*>(name_7058);
	auto& queuedMissionActorData = *reinterpret_cast<QueuedMissionActorData*>(name_7058 + 0xC0);
	auto& activeMissionActorData = *reinterpret_cast<ActiveMissionActorData*>(name_7058 + 0x16C);

    // Only player 1's active actor can accumulate exp.
    // Default actors are not updated.

    auto& playerData = GetPlayerData(0);

    auto& characterData = GetCharacterData(0, playerData.characterIndex, ENTITY::MAIN);
    auto& newActorData  = GetNewActorData(0, playerData.characterIndex, ENTITY::MAIN);

    auto& activeCharacterData = GetCharacterData(0, playerData.activeCharacterIndex, ENTITY::MAIN);
    auto& activeNewActorData  = GetNewActorData(0, playerData.activeCharacterIndex, ENTITY::MAIN);

    auto& leadCharacterData = GetCharacterData(0, 0, ENTITY::MAIN);
    auto& leadNewActorData  = GetNewActorData(0, 0, ENTITY::MAIN);

    auto& mainCharacterData = GetCharacterData(0, playerData.characterIndex, ENTITY::MAIN);
    auto& mainNewActorData  = GetNewActorData(0, playerData.characterIndex, ENTITY::MAIN);


    if (!activeNewActorData.baseAddr) {
        return;
	}
	auto& activeActorData = *reinterpret_cast<PlayerActorData*>(activeNewActorData.baseAddr);

	auto character = activeActorData.character;
	if (character >= CHARACTER::MAX) {
		return;
	}

	auto style = activeActorData.style;
	if (style >= STYLE::MAX) {
		return;
	}

	ExpData* expDataAddr = (character == CHARACTER::DANTE) ? &missionExpDataDante
                           : (character == CHARACTER::VERGIL) ? &missionExpDataVergil
                                                              : 0;

    if (!expDataAddr) {
        return;
    }

    auto& expData = *expDataAddr;

    DebugLogFunction();

	HeldStyleExpData& heldStyleExpData = (character == CHARACTER::DANTE)
		? heldStyleExpDataDante
		: heldStyleExpDataVergil;

    expData.styleLevels[style] = heldStyleExpData.accumulatedStyleLevels[style];
    expData.styleExpPoints[style] = heldStyleExpData.accumulatedStylePoints[style];
}


void UpdatePlayerActorExp(byte8* actorBaseAddr) {
    if (!Enable()) {
        return;
    }

    if (!actorBaseAddr) {
        return;
    }
    auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);

    auto character = actorData.character;
    if (character >= CHARACTER::MAX) {
        return;
    }

    auto style = actorData.style;
    if (style >= STYLE::MAX) {
        return;
    }

    ExpData* expDataAddr = (character == CHARACTER::DANTE)    ? &missionExpDataDante
                           : (character == CHARACTER::VERGIL) ? &missionExpDataVergil
                                                              : 0;

    if (!expDataAddr) {
        return;
    }
    

    auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

    auto& expData = *expDataAddr;

    auto& styleLevel     = actorData.styleLevel;
    auto& styleExpPoints = actorData.styleExpPoints;

    DebugLogFunction(actorBaseAddr);


	//Reset
	{
			actorData.styleLevel = 0;
			actorData.styleExpPoints = 0;

			if (character == CHARACTER::DANTE) {
					actorData.activeExpertise[0] = 0xFFFF5E7F;
					actorData.activeExpertise[1] = 0xA7FFAF5F;
					actorData.activeExpertise[2] = 0xAF1FFFF3;
					actorData.activeExpertise[3] = 0xCB9FFFF9;
					actorData.activeExpertise[4] = 0xFBFBFFFE;
					actorData.activeExpertise[5] = 0xFFFFEFFD;
					actorData.activeExpertise[6] = 0xFFE3FEFF;
					actorData.activeExpertise[7] = 0xFFFFFFFF;
			}
			else if (character == CHARACTER::VERGIL) {
					actorData.activeExpertise[0] = 0xF4FFF9CF;
					actorData.activeExpertise[1] = 0xFFC7FE37;
					actorData.activeExpertise[2] = 0xFFFFFFFF;
					actorData.activeExpertise[3] = 0xFFFFFFFF;
					actorData.activeExpertise[4] = 0xFFFFFFFF;
					actorData.activeExpertise[5] = 0xFFFFFFFF;
					actorData.activeExpertise[6] = 0xFFFFFFFF;
					actorData.activeExpertise[7] = 0xFFFFFFFF;
			}

			SetMemory
			(
					actorData.newWeaponLevels,
					0,
					sizeof(actorData.newWeaponLevels)
			);
	}


    if (Max()) {
        if ((actorData.style >= STYLE::SWORDMASTER) && (actorData.style <= STYLE::ROYALGUARD)) {
            actorData.styleLevel     = 2;
            actorData.styleExpPoints = 100000;
        }

        SetMemory(actorData.activeExpertise, 0xFF, sizeof(actorData.activeExpertise));

        if (actorData.character == CHARACTER::DANTE) {
            for_each(index, WEAPON::EBONY_IVORY, WEAPON_COUNT) {
                actorData.newWeaponLevels[index] = 2;
            }
        }

        return;
    }
    
    if (sessionData.character == actorData.character) { // if Current Main character equates to the campaign being played.
        styleLevel = expData.styleLevels[actorData.style]; // Then we fetch possibly higher Style Level already saved (without Actor System enabled)
    }
    else {
        styleLevel = expData.styleLevels[actorData.style];
    }
	
	if (styleLevel >= 2) {
		styleExpPoints = 100000;
	}
	else {
		styleExpPoints = expData.styleExpPoints[style];
	}

	

    auto UpdateOnce = [&](ExpertiseHelper& helper) {
        actorData.activeExpertise[helper.index] += helper.flags; // Plus instead of or, because a custom bit size is used.
    };

    auto UpdateLoop = [&](ExpertiseHelper* helpers, new_size_t helperCount) {
        for_all(helperIndex, helperCount) {
            auto& helper = helpers[helperIndex];

            UpdateOnce(helper);
        }
    };


     
    if (character == CHARACTER::DANTE) {
        
		switch (style) {
		case STYLE::SWORDMASTER:
		{
			if (styleLevel == 1) {
				UpdateLoop(expertiseHelpersDanteSwordmasterLevel2, countof(expertiseHelpersDanteSwordmasterLevel2));
			}
			else if (styleLevel == 2) {
				UpdateLoop(expertiseHelpersDanteSwordmasterLevel3, countof(expertiseHelpersDanteSwordmasterLevel3));
			}

			break;
		}
		case STYLE::GUNSLINGER:
		{
			if (styleLevel == 1) {
				UpdateLoop(expertiseHelpersDanteGunslingerLevel2, countof(expertiseHelpersDanteGunslingerLevel2));
			}
			else if (styleLevel == 2) {
				UpdateLoop(expertiseHelpersDanteGunslingerLevel3, countof(expertiseHelpersDanteGunslingerLevel3));
			}

			break;
		}
		}


        using namespace UNLOCK_DANTE;

        for_each(index, REBELLION_STINGER_LEVEL_1, EBONY_IVORY_LEVEL_2) {
            auto& unlock = expData.unlocks[index];
            if (!unlock) {
                continue;
            }

            auto& helper = expertiseHelpersDante[index];

            UpdateOnce(helper);
        }


        for_each(index, EBONY_IVORY_LEVEL_2, COUNT) {
            auto& unlock = expData.unlocks[index];
            if (!unlock) {
                continue;
            }

            auto& helper = levelHelpers[(index - EBONY_IVORY_LEVEL_2)];

            actorData.newWeaponLevels[helper.index] = helper.level;
        }
    } else if (character == CHARACTER::VERGIL) {
        using namespace UNLOCK_VERGIL;

        for_all(index, COUNT) {
            auto& unlock = expData.unlocks[index];
            if (!unlock) {
                continue;
            }

            auto& helper = expertiseHelpersVergil[index];

            UpdateOnce(helper);
        }
    }
}


void UpdatePlayerActorExps() {
    if (!Enable()) {
        return;
    }


    DebugLogFunction();

    old_for_all(uint8, playerIndex, PLAYER_COUNT) {
        old_for_all(uint8, characterIndex, CHARACTER_COUNT) {
            old_for_all(uint8, entityIndex, ENTITY_COUNT) {
                auto& playerData = GetPlayerData(playerIndex);

                auto& characterData = GetCharacterData(playerIndex, characterIndex, entityIndex);
                auto& newActorData  = GetNewActorData(playerIndex, characterIndex, entityIndex);

                auto& activeCharacterData = GetCharacterData(playerIndex, playerData.activeCharacterIndex, ENTITY::MAIN);
                auto& activeNewActorData  = GetNewActorData(playerIndex, playerData.activeCharacterIndex, ENTITY::MAIN);

                auto& leadCharacterData = GetCharacterData(playerIndex, 0, ENTITY::MAIN);
                auto& leadNewActorData  = GetNewActorData(playerIndex, 0, ENTITY::MAIN);

                auto& mainCharacterData = GetCharacterData(playerIndex, characterIndex, ENTITY::MAIN);
                auto& mainNewActorData  = GetNewActorData(playerIndex, characterIndex, ENTITY::MAIN);


                if (!newActorData.baseAddr) {
                    continue;
                }
                auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);

                UpdatePlayerActorExp(actorData);
            }
        }
    }
}

void TransferUnlocksToActorSystem() {
    auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);
    // This is to be called when saving the game on Vanilla, so that unlocks you may hvae bought in Vanilla get transfered to the Actor System.

    // If not in Actor System
    if (activeConfig.Actor.enable) {
        return;
    }
	auto saveIndex = g_saveIndex;
	if (saveIndex >= SAVE_COUNT) {
		return;
	}

	if (sessionData.character == CHARACTER::DANTE) {

		// Unlock Moves from Vanilla
		for (int index = UNLOCK_DANTE::REBELLION_STINGER_LEVEL_1; index < UNLOCK_DANTE::EBONY_IVORY_LEVEL_2; ++index) {
			const ExpertiseHelper& helper = expertiseHelpersDante[index];

			// Check if the specific bit in the expertise array is set and if the unlock array does not have it set to true
			if (sessionData.expertise[helper.index] & helper.flags && !savedExpDataDante[saveIndex].unlocks[index]) {

				// Set the corresponding unlock flag to true
                savedExpDataDante[saveIndex].unlocks[index] = true;
			}
		}

		// Unlock Weapon Levels from Vanilla
		for (int index = UNLOCK_DANTE::EBONY_IVORY_LEVEL_2; index < UNLOCK_DANTE::COUNT; ++index) {
			const LevelHelper& helper = levelHelpers[(index - UNLOCK_DANTE::EBONY_IVORY_LEVEL_2)];
			uint8 weaponLevel = 0;

			if (index % 2) {
				weaponLevel = 1;
			}
			else {
				weaponLevel = 2;
			}

			// Check if the current weapon level matches the helper's level
			if (sessionData.rangedWeaponLevels[(helper.index - 5)] >= weaponLevel && !savedExpDataDante[saveIndex].unlocks[index]) {
                savedExpDataDante[saveIndex].unlocks[index] = true;
			}
		}


	}
	else if (sessionData.character == CHARACTER::VERGIL) {

		// Unlock Moves from Vanilla Vergil Edition
		for (int index = 0; index < UNLOCK_VERGIL::COUNT; ++index) {
			const ExpertiseHelper& helper = expertiseHelpersVergil[index];


			if (sessionData.expertise[helper.index] & helper.flags && !savedExpDataVergil[saveIndex].unlocks[index]) {

                savedExpDataVergil[saveIndex].unlocks[index] = true;
			}
		}
	}

	using namespace rapidjson;
	using namespace JSON;


	ToJSON();


	StringBuffer stringBuffer;
	PrettyWriter<StringBuffer> prettyWriter(stringBuffer);

	root.Accept(prettyWriter);


	auto name = stringBuffer.GetString();
	auto size = strlen(name);

	if (!SaveFile(location, name, size)) {
		Log("SaveFile failed.");
	}
    
}

void MaintainUnlockAndExpertiseParity() {
    auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);
    // This is to be called when loading the save to maintain parity between the Vanilla System and Actor System unlocks

    if (activeConfig.Actor.enable) {
        if (sessionData.character == CHARACTER::DANTE) {
            
            // Unlock Moves from Vanilla
			for (int index = UNLOCK_DANTE::REBELLION_STINGER_LEVEL_1; index < UNLOCK_DANTE::EBONY_IVORY_LEVEL_2; ++index) {
                const ExpertiseHelper& helper = expertiseHelpersDante[index];

				// Check if the specific bit in the expertise array is set and if the unlock array does not have it set to true
				if (sessionData.expertise[helper.index] & helper.flags && !sessionExpDataDante.unlocks[index]) {

					// Set the corresponding unlock flag to true
					sessionExpDataDante.unlocks[index] = true;
				}
			}

            // Unlock Weapon Levels from Vanilla
            for (int index = UNLOCK_DANTE::EBONY_IVORY_LEVEL_2; index < UNLOCK_DANTE::COUNT; ++index) {
                const LevelHelper& helper = levelHelpers[(index - UNLOCK_DANTE::EBONY_IVORY_LEVEL_2)];
                uint8 weaponLevel = 0;

                if (index % 2) {
                    weaponLevel = 1;
                }
                else {
                    weaponLevel = 2;
                }

                // Check if the current weapon level matches the helper's level
                if (sessionData.rangedWeaponLevels[(helper.index - 5)] >= weaponLevel  && !sessionExpDataDante.unlocks[index]) {
                    sessionExpDataDante.unlocks[index] = true;
                }
            }

            // Sync Style EXP and Levels
            for (int style = STYLE::SWORDMASTER; style <= STYLE::ROYALGUARD; ++style) {

				sessionExpDataDante.styleExpPoints[style] = (std::max)(sessionExpDataDante.styleExpPoints[style], sessionData.styleExpPoints[style]);
				sessionExpDataDante.styleLevels[style] = (std::max)(sessionExpDataDante.styleLevels[style], sessionData.styleLevels[style]);
            }

            // hasPairedWithActorSystem guarantees the pairing with the actor has occurred at least once 
            // before pairing the moves from the Actor System to vanilla, so that the moves from vanilla
            // won't get suddenly erased if someone has started Crimson from Vanilla.
            sessionExpDataDante.hasPairedWithActorSystem = true;
        }
        else if (sessionData.character == CHARACTER::VERGIL) {
			
			// Unlock Moves from Vanilla Vergil Edition
			for (int index = 0; index < UNLOCK_VERGIL::COUNT; ++index) {
				const ExpertiseHelper& helper = expertiseHelpersVergil[index];

				
				if (sessionData.expertise[helper.index] & helper.flags && !sessionExpDataVergil.unlocks[index]) {

					sessionExpDataVergil.unlocks[index] = true;
				}
			}

            sessionExpDataVergil.styleExpPoints[STYLE::DARK_SLAYER] = (std::max)(sessionExpDataVergil.styleExpPoints[STYLE::DARK_SLAYER], sessionData.styleExpPoints[STYLE::DARK_SLAYER]);
            sessionExpDataVergil.styleLevels[STYLE::DARK_SLAYER] = (std::max)(sessionExpDataVergil.styleLevels[STYLE::DARK_SLAYER], sessionData.styleLevels[STYLE::DARK_SLAYER]);

            sessionExpDataVergil.hasPairedWithActorSystem = true;
        }
		
   
    }
    else { // Not in actor System

        if (sessionData.character == CHARACTER::DANTE && sessionExpDataDante.hasPairedWithActorSystem) {

			// Reverse operation: Set expertise flags based on unlocked moves 
			for (int index = UNLOCK_DANTE::REBELLION_STINGER_LEVEL_1; index < UNLOCK_DANTE::EBONY_IVORY_LEVEL_2; ++index) {
				const ExpertiseHelper& helper = expertiseHelpersDante[index];

				// Check if the unlock flag is set to true and if the expertise does not already have that unlock
				// if it's true, unlock move on Vanilla from the Actor System
				if (sessionExpDataDante.unlocks[index] && !(sessionData.expertise[helper.index] & helper.flags)) {

					sessionData.expertise[helper.index] += helper.flags;
				}
				else if (!sessionExpDataDante.unlocks[index] && (sessionData.expertise[helper.index] & helper.flags)) {

					// if you've sold the move on the Actor System, disable it on vanilla
					sessionData.expertise[helper.index] -= helper.flags;
				}

			}

			// Reverse operation: Set weapon levels based on unlocked weapon levels
			for (int index = UNLOCK_DANTE::EBONY_IVORY_LEVEL_2; index < UNLOCK_DANTE::COUNT; index += 2) {
				const LevelHelper& helper = levelHelpers[(index - UNLOCK_DANTE::EBONY_IVORY_LEVEL_2)];
				uint32& weaponLevel = sessionData.rangedWeaponLevels[(helper.index - 5)];

				// Check if the next index is within bounds
				if (index + 1 < UNLOCK_DANTE::COUNT && sessionExpDataDante.unlocks[index + 1]) {
					weaponLevel = 2; 
				}
				else if (sessionExpDataDante.unlocks[index]) {
					weaponLevel = 1; 
				}
				else {
					weaponLevel = 0; 
				}
			}


			for (int style = STYLE::SWORDMASTER; style <= STYLE::ROYALGUARD; ++style) {

                sessionData.styleExpPoints[style] = sessionExpDataDante.styleExpPoints[style];
                sessionData.styleLevels[style] = sessionExpDataDante.styleLevels[style];
			}
        }
        else if (sessionData.character == CHARACTER::VERGIL && sessionExpDataVergil.hasPairedWithActorSystem) {

//          // Reverse Operation: Vergil Edition
			for (int index = 0; index < UNLOCK_VERGIL::COUNT; ++index) {
				const ExpertiseHelper& helper = expertiseHelpersVergil[index];

				if (sessionExpDataVergil.unlocks[index] && !(sessionData.expertise[helper.index] & helper.flags)) {

					sessionData.expertise[helper.index] += helper.flags;
				}
				else if (!sessionExpDataVergil.unlocks[index] && (sessionData.expertise[helper.index] & helper.flags)) {

					// if you've sold the move on the Actor System, disable it on vanilla
					sessionData.expertise[helper.index] -= helper.flags;
				}

			}

            sessionData.styleExpPoints[STYLE::DARK_SLAYER] = sessionExpDataVergil.styleExpPoints[STYLE::DARK_SLAYER];
            sessionData.styleLevels[STYLE::DARK_SLAYER] = sessionExpDataVergil.styleLevels[STYLE::DARK_SLAYER];

        }

    }

 	std::cout << "Exp Paired! " << std::endl;

}

void MarkAsPairedWithActorSystem() {
	// This function serves to guarantee new saves will be marked as hasPairedWithActorSystem

	auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

	if (!activeConfig.Actor.enable) {
		return;
	}

	auto& sessionExpData =
		(sessionData.character == CHARACTER::DANTE)
		? ExpConfig::sessionExpDataDante
		: ExpConfig::sessionExpDataVergil;

	sessionExpData.hasPairedWithActorSystem = true;

    std::cout << "Marked" << std::endl;

}



namespace Exp {

void InitSession() {
    if (!Enable()) {
        return;
    }


    LogFunction();

    sessionExpDataDante  = {};
    sessionExpDataVergil = {};
}

void SceneMissionStart() {
    if (!Enable()) {
        return;
    }

    LogFunction();

    missionExpDataDante  = sessionExpDataDante;
    missionExpDataVergil = sessionExpDataVergil;
}

void SceneMissionResult() {
    if (!Enable()) {
        return;
    }

    LogFunction();

    sessionExpDataDante  = missionExpDataDante;
    sessionExpDataVergil = missionExpDataVergil;
}

void IncStyleExpPoints(byte8* actorBaseAddr) {
    if (!Enable()) {
        return;
    }

    DebugLogFunction(actorBaseAddr);

    SavePlayerActorExp();
}

}; // namespace Exp
} // namespace ExpConfig

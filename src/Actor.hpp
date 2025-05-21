#pragma once

#include "Core/DataTypes.hpp"
#include "File.hpp"
#include "Config.hpp"
#include "Global.hpp"
#include "Vars.hpp"

#pragma region Enemy

byte8* CreateEnemyActor(const CreateEnemyActorData& createEnemyActorData, byte64 flags = 0);

byte8* CreateEnemyActor(const ConfigCreateEnemyActorData& configCreateEnemyActorData, byte64 flags = 0);

#pragma endregion

#pragma region Main

template <typename T> void SetMainActor(T& actorData) {
    DebugLogFunction(actorData.operator byte8*());

    auto actorBaseAddr = reinterpret_cast<byte8*>(&actorData);

    // FileData
    [&]() {
        auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);


        auto character = static_cast<uint8>(actorData.character);
        auto costume   = actorData.costume;

        uint16 costumeFileId = 0;

        switch (character) {
        case CHARACTER::DANTE: {
            if (costume >= COSTUME::MAX_DANTE) {
                costume = 0;
            }

            costumeFileId = costumeFileIdsDante[costume];

            break;
        }
        case CHARACTER::BOB: {
            if (costume >= COSTUME::MAX_BOB) {
                costume = 0;
            }

            costumeFileId = costumeFileIdsBob[costume];

            break;
        }
        case CHARACTER::LADY: {
            if (costume >= COSTUME::MAX_LADY) {
                costume = 0;
            }

            costumeFileId = costumeFileIdsLady[costume];

            break;
        }
        case CHARACTER::VERGIL: {
            if (costume >= COSTUME::MAX_VERGIL) {
                costume = 0;
            }

            costumeFileId = costumeFileIdsVergil[costume];

            break;
        }
        }

        File_UpdateFileData(static_cast<uint16>(character), costumeFileId);

        if (actorData.character == CHARACTER::DANTE) {
            uint16 swordFileId = plwp_sword;

            if (sessionData.unlockDevilTrigger) {
                swordFileId = plwp_sword2;
            }

            if (actorData.sparda) {
                swordFileId = plwp_sword3;
            }

            File_UpdateFileData(140, swordFileId);
        }
    }();

    // Main
    [&]() {
        auto pool = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
        if (!pool) {
            return;
        }
        pool[3] = actorBaseAddr;
    }();

    // Life, Cutscenes
    [&]() {
        auto pool = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
        if (!pool || !pool[5]) {
            return;
        }
        *reinterpret_cast<byte8**>(pool[5] + 0x2CB8) = actorBaseAddr;
    }();

    // Style Data
    [&]() {
        auto pool = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
        if (!pool || !pool[12]) {
            return;
        }
        auto dest = *reinterpret_cast<byte8**>(pool[12]);
        if (!dest) {
            return;
        }
        *reinterpret_cast<StyleData**>(dest + 0x3D10) = &actorData.styleData;

        DebugLog("Updated Style Data");
    }();

    // SetStyleData(actorData);
}

void SetMainActor(uint32 index);

template <typename T> void ToggleActor(T& actorData, bool enable) {
    auto& newActorData = GetNewActorData(actorData.newPlayerIndex, actorData.newCharacterIndex, actorData.newEntityIndex);

    newActorData.visibility = (enable) ? Visibility_Default : Visibility_Hide;

    newActorData.enableCollision = enable;

    if (enable) {
        actorData.visibility = (g_quicksilver) ? 2 : 1;
    } else {
        actorData.visibility = 0;
    }

    auto playerIndex    = actorData.newPlayerIndex;
    auto characterIndex = actorData.newCharacterIndex;
    auto entityIndex    = actorData.newEntityIndex;

    auto& playerData = GetPlayerData(playerIndex);

    auto actorBaseAddr = reinterpret_cast<byte8*>(&actorData);

    // Quicksilver Fix

    // Time-Lag's effect is bound to the actor that triggered it.
    // If we hide the actor the effect is also hidden.

    if (!enable && g_quicksilver && (actorBaseAddr == g_playerActorBaseAddrs[2]) &&
        (actorData.newCharacterIndex != playerData.activeCharacterIndex)) {
        newActorData.visibility = Visibility_Default;

        actorData.visibility = 0;
    }

    if (enable && !actorData.doppelganger) {
        actorData.shadow = 1;
    } else {
        actorData.shadow = 0;
    }

    actorData.lastShadow = 0;


    ToggleInput(actorData, enable);
}

void ToggleActor(uint32 index, bool enable);

template <typename T> struct GetCharacterId {
    enum {
        value = (TypeMatch<T, PlayerActorDataDante>::value)    ? CHARACTER::DANTE
                : (TypeMatch<T, PlayerActorDataBob>::value)    ? CHARACTER::BOB
                : (TypeMatch<T, PlayerActorDataLady>::value)   ? CHARACTER::LADY
                : (TypeMatch<T, PlayerActorDataVergil>::value) ? CHARACTER::VERGIL
                                                               : 0
    };
};

// Character switch instead of specialization so we can use plain ActorData type.

// @Todo: Add remaining idle ranged weapon indices.
template <typename T> bool IsWeaponActive(T& actorData) {
    auto& motionData = actorData.motionData[UPPER_BODY];

    switch (actorData.character) {
    case CHARACTER::DANTE: {
        if ((motionData.group >= MOTION_GROUP_DANTE::REBELLION) && (motionData.group <= MOTION_GROUP_DANTE::KALINA_ANN) &&
            (motionData.index > 0)) {
            if (motionData.group == MOTION_GROUP_DANTE::EBONY_IVORY) {
                switch (motionData.index) {
                case 2:
                case 4:
                case 5: {
                    return false;
                }
                }
            }

            return true;
        } else if ((motionData.group >= MOTION_GROUP_DANTE::SWORDMASTER_REBELLION) &&
                   (motionData.group <= MOTION_GROUP_DANTE::GUNSLINGER_KALINA_ANN)) {
            return true;
        }

        break;
    }
    case CHARACTER::BOB: {
        if ((motionData.group == MOTION_GROUP_BOB::YAMATO) && (motionData.index > 0)) {
            return true;
        }

        break;
    }
    case CHARACTER::VERGIL: {
        if ((motionData.group >= MOTION_GROUP_VERGIL::YAMATO) && (motionData.group <= MOTION_GROUP_VERGIL::YAMATO_FORCE_EDGE) &&
            (motionData.index > 0)) {
            return true;
        } else if ((motionData.group >= MOTION_GROUP_VERGIL::NERO_ANGELO_YAMATO) &&
                   (motionData.group <= MOTION_GROUP_VERGIL::NERO_ANGELO_BEOWULF) && (motionData.index > 0)) {
            return true;
        }

        break;
    }
    }

    return false;
}

template <typename T> bool IsActive(T& actorData) {
    auto& motionData = actorData.motionData[UPPER_BODY];

    switch (actorData.character) {
    case CHARACTER::DANTE: {
        using namespace MOTION_GROUP_DANTE;

        if (motionData.group == BASE) {
            // switch (motionData.index)
            // {
            // 	case 0:
            // 	case 1:
            // 	case 2:
            // 	case 3:
            // 	case 4:
            // 	case 5:
            // 	case 7:
            // 	case 8:
            // 	case 9:
            // 	case 17:
            // 	case 31:
            // 	case 32:
            // 	case 38:
            // 	case 39:
            // 	{
            // 		return false;
            // 	}
            // }

            return false;
        } else if ((motionData.group >= DAMAGE) && (motionData.group <= TAUNTS)) {
            return true;
        }
        // else if (motionData.group == REBELLION)
        // {
        // 	switch (motionData.index)
        // 	{
        // 		case 13:
        // 		{
        // 			return false;
        // 		}
        // 	}

        // 	return true;
        // }
        else if (IsWeaponActive(actorData)) {
            return true;
        } else if ((motionData.group >= TRICKSTER) && (motionData.group <= DOPPELGANGER)) {
            return true;
        }

        break;
    }
    case CHARACTER::BOB: {
        using namespace MOTION_GROUP_BOB;

        // if (motionData.group == BASE)
        // {
        // 	// switch (motionData.index)
        // 	// {
        // 	// 	case 0:
        // 	// 	case 3:
        // 	// 	{
        // 	// 		return false;
        // 	// 	}
        // 	// }

        // 	return true;
        // }
        if ((motionData.group >= DAMAGE) && (motionData.group <= TAUNTS)) {
            return true;
        } else if (IsWeaponActive(actorData)) {
            return true;
        }

        break;
    }
    case CHARACTER::LADY: {
        using namespace MOTION_GROUP_LADY;

        // if (motionData.group == BASE)
        // {
        // 	switch (motionData.index)
        // 	{
        // 		case 0:
        // 		case 3:
        // 		{
        // 			return false;
        // 		}
        // 	}

        // 	return true;
        // }
        if ((motionData.group >= DAMAGE) && (motionData.group <= TAUNTS)) {
            return true;
        }

        break;
    }
    case CHARACTER::VERGIL: {
        using namespace MOTION_GROUP_VERGIL;

        // if (motionData.group == BASE)
        // {
        // 	switch (motionData.index)
        // 	{
        // 		case 0:
        // 		case 3:
        // 		{
        // 			return false;
        // 		}
        // 	}

        // 	return true;
        // }
        if ((motionData.group >= DAMAGE) && (motionData.group <= TAUNTS)) {
            return true;
        } else if (IsWeaponActive(actorData)) {
            return true;
        } else if (motionData.group == DARK_SLAYER) {
            return true;
        }

        break;
    }
    }

    return false;
}

template <typename T> bool IsNeroAngelo(T& actorData) {
    return ((actorData.character == CHARACTER::VERGIL) && actorData.neroAngelo && actorData.devil);
}

#pragma endregion

#pragma region Actor Management

void ToggleNoDevilForm(bool enable);
void SpawnActors();

#pragma endregion

#pragma region Controllers

void CharacterSwitchController();
void BossLadyController();
void BossVergilController();
void ToggleBossLadyFixes(bool enable);
void ToggleBossVergilFixes(bool enable);

#pragma endregion

#pragma region Speed

void ToggleRebellionHoldDrive(bool enable);

#pragma endregion

void ToggleDeplete(bool enable);
void ToggleOrbReach(bool enable);
void ToggleDamage(bool enable);
void ToggleDergil(uint8 value);

namespace Actor {
void Toggle(bool enable);
}

void ToggleAirHikeCoreAbility(bool enable);
void ToggleRebellionInfiniteSwordPierce(bool enable);
void ToggleYamatoForceEdgeInfiniteRoundTrip(bool enable);
void ToggleEbonyIvoryFoursomeTime(bool enable);
void ToggleEbonyIvoryInfiniteRainStorm(bool enable);
void ToggleArtemisSwapNormalShotAndMultiLock(bool enable);
void ToggleArtemisInstantFullCharge(bool enable);
void UpdateCrazyComboLevelMultiplier();
void ToggleChronoSwords(bool enable);
void ToggleDisablePlayerActorIdleTimer(bool enable);
void ToggleRebellionInfiniteShredder(bool enable);

#pragma region Events

namespace Actor {
void EventCreateMainActor(byte8* actorBaseAddr);
void EventCreateCloneActor(byte8* actorBaseAddr);
void EventDelete();
void EventDeath();
void InGameCutsceneStart();
void EventMain();
void PlayerActorLoop(byte8* actorBaseAddr);
void SetNextScreen(EventData& eventData);
} // namespace Actor

#pragma endregion

void DeactivateDevilHaywire(PlayerActorData& actorData);

#pragma region Scenes

namespace Actor {
void SceneMissionStart();
void SceneGame();
} // namespace Actor

#pragma endregion

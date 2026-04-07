// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <thread>
#include <chrono>
#include <math.h>
#include "../ThirdParty/glm/glm.hpp"
#include "../ThirdParty/ImGui/imgui.h"
#include <ctime>
#include <iostream>
#include <cstdio>
#include "Utility/Detour.hpp"
#include "DebugDrawDX11.hpp"
#define DEBUG_DRAW_EXPLICIT_CONTEXT
#include "debug_draw.hpp"
#include "DMC3Input.hpp"
#include "File.hpp"
#include "Internal.hpp"
#include "ActorBase.hpp"

// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "CrimsonGameplay.hpp"
#include "Core/Core.hpp"
#include "Memory.hpp"
#include "Model.hpp"
#include "ActorRelocations.hpp"
#include "Config.hpp"
#include "Exp.hpp"
#include "Global.hpp"
#include "HUD.hpp"
#include "Sound.hpp"
#include "SoundRelocations.hpp"
#include "Speed.hpp"
#include "Vars.hpp"
#include "StyleSwitchFX.hpp"
#include "Core/Macros.h"
#include <deque>
#include "Training.hpp"
#include "CrimsonDetours.hpp"
#include "CrimsonSDL.hpp"
#include "CrimsonPatches.hpp"
#include "CrimsonTimers.hpp"
#include "CrimsonUtil.hpp"
#include "CrimsonEfk.hpp"

namespace CrimsonGameplay {

#pragma region CrimsonPlayers

void UpdateCrimsonPlayerData() {
    // crimsonPlayer data has a lot of copies from ActorData, we're using this so we can more easily access data from Actors
    // globally. It also serves to create new player data outside of DDMK legacy stuff or DMC3 itself (like Action/Anim Timers, or Mirage Gauge). 
    // Please note that assigning new values to crimsonPlayer directly might have no effect 
    // (you still need to call ActorData to change hit points, for exemple).
    // This function here and PreparePlayersDataBeforeSpawn() from CrimsonOnTick is also important to preserve Players Data in between rooms. - Mia

	old_for_all(uint8, playerIndex, PLAYER_COUNT) {
		auto& playerData = GetPlayerData(playerIndex);

		// auto& activeNewActorData = GetNewActorData(playerIndex, playerData.activeCharacterIndex, ENTITY::MAIN);

		auto& newActorData = GetNewActorData(playerIndex, playerData.activeCharacterIndex, 0);
		auto actorBaseAddr = newActorData.baseAddr;

		if (!actorBaseAddr) {
			continue;
		}
		auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
		auto& cloneActorData = *reinterpret_cast<PlayerActorData*>(actorData.cloneActorBaseAddr);
		auto pool_10371 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
		if (!pool_10371 || !pool_10371[8]) {
			return;
		}
		auto& eventData = *reinterpret_cast<EventData*>(pool_10371[8]);
        auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);
       

		auto& gamepad = GetGamepad(actorData.newPlayerIndex);
		auto tiltDirection = GetRelativeTiltDirection(actorData);
		auto inAir = (actorData.state & STATE::IN_AIR);
		auto lockOn = (actorData.buttons[0] & GetBinding(BINDING::LOCK_ON));

		crimsonPlayer[playerIndex].playerPtr = (uintptr_t)actorData.baseAddr;
		crimsonPlayer[playerIndex].action = actorData.action;
		crimsonPlayer[playerIndex].event = actorData.eventData[0].event;
		crimsonPlayer[playerIndex].lastEvent = actorData.eventData[0].lastEvent;
		crimsonPlayer[playerIndex].state = actorData.state;
		crimsonPlayer[playerIndex].lastState = actorData.lastState;
		crimsonPlayer[playerIndex].motion = actorData.motionData[0].index;
		crimsonPlayer[playerIndex].character = actorData.character;
		crimsonPlayer[playerIndex].gamepad = gamepad;
		crimsonPlayer[playerIndex].tiltDirection = tiltDirection;
		crimsonPlayer[playerIndex].lockOn = lockOn;
		crimsonPlayer[playerIndex].speed = actorData.speed;
        if (eventData.event != EVENT::DEATH) {
            crimsonPlayer[playerIndex].hitPoints = actorData.hitPoints;
			crimsonPlayer[playerIndex].style = actorData.style;
			crimsonPlayer[playerIndex].maxHitPoints = actorData.maxHitPoints;
			crimsonPlayer[playerIndex].magicPoints = actorData.magicPoints;
			crimsonPlayer[playerIndex].maxMagicPoints = actorData.maxMagicPoints;
        }
        else {
            actorData.hitPoints = sessionData.hitPoints;
            crimsonPlayer[playerIndex].hitPoints = sessionData.hitPoints;
        }
        crimsonPlayer[playerIndex].actorMode = actorData.mode;
        crimsonPlayer[playerIndex].royalguardReleaseDamage = actorData.royalguardReleaseDamage;
        if (eventData.event == EVENT::MAIN && actorData.dtExplosionCharge > 300 && g_inGameDelayed) {
            crimsonPlayer[playerIndex].dtExplosionCharge = actorData.dtExplosionCharge;
        }
        
        crimsonPlayer[playerIndex].styleData.rank = actorData.styleData.rank;
        crimsonPlayer[playerIndex].styleData.meter = actorData.styleData.meter;
        crimsonPlayer[playerIndex].styleData.quotient = actorData.styleData.quotient;
        crimsonPlayer[playerIndex].styleData.dividend = actorData.styleData.dividend;
        crimsonPlayer[playerIndex].styleData.divisor = actorData.styleData.divisor;

		crimsonPlayer[playerIndex].horizontalPull = actorData.horizontalPull;

		if (actorData.character == CHARACTER::DANTE || actorData.character == CHARACTER::VERGIL) {
			crimsonPlayer[playerIndex].clonePtr = (uintptr_t)actorData.cloneActorBaseAddr;
			crimsonPlayer[playerIndex].actionClone = cloneActorData.action;
			crimsonPlayer[playerIndex].lastActionClone = cloneActorData.lastAction;
			crimsonPlayer[playerIndex].motionClone = cloneActorData.motionData[0].index;
			crimsonPlayer[playerIndex].eventClone = cloneActorData.eventData[0].event;
			crimsonPlayer[playerIndex].lastEventClone = cloneActorData.eventData[0].lastEvent;
			crimsonPlayer[playerIndex].stateClone = cloneActorData.state;
			crimsonPlayer[playerIndex].lastStateClone = cloneActorData.lastState;
			crimsonPlayer[playerIndex].speedClone = cloneActorData.speed;
			crimsonPlayer[playerIndex].horizontalPullClone = cloneActorData.horizontalPull;
		}
	}
}

#pragma endregion


#pragma region Cancels

void StoreAirCounts(byte8* actorBaseAddr) {
	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
	auto playerIndex = actorData.newPlayerIndex;
	auto& storedAirCounts = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].storedAirCounts : crimsonPlayer[playerIndex].storedAirCountsClone;
	auto& airCounts = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].airCounts : crimsonPlayer[playerIndex].airCountsClone;

	storedAirCounts.trickUp = actorData.newTrickUpCount;
	storedAirCounts.skyStar = actorData.newSkyStarCount;
	storedAirCounts.airHike = actorData.newAirHikeCount;
	storedAirCounts.airStinger = actorData.newAirStingerCount;
	storedAirCounts.airTornado = airCounts.airTornado;
	storedAirCounts.airSwordAttack = actorData.airSwordAttackCount;
	storedAirCounts.airGunAttack = actorData.airGunAttackCount;
}

void AirCancelCountsTracker(byte8* actorBaseAddr) {
    if (!actorBaseAddr) {
        return;
    }
    auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    auto playerIndex = actorData.newPlayerIndex;
    auto& storedAirCounts = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].storedAirCounts : crimsonPlayer[playerIndex].storedAirCountsClone;
	auto& airCounts = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].airCounts : crimsonPlayer[playerIndex].airCountsClone;

    // This restores player counts back to what they were before the Royal Cancel
    storedAirCounts.cancelTrackerRunning = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    actorData.newTrickUpCount = storedAirCounts.trickUp;
    actorData.newSkyStarCount = storedAirCounts.skyStar;
    actorData.newAirHikeCount = storedAirCounts.airHike;
    actorData.newAirStingerCount = storedAirCounts.airStinger;
	airCounts.airTornado = storedAirCounts.airTornado;
	actorData.airSwordAttackCount = storedAirCounts.airSwordAttack;
	actorData.airGunAttackCount = storedAirCounts.airGunAttack;

    storedAirCounts.cancelTrackerRunning = false;
}

void FixAirStingerCancelTime(byte8* actorBaseAddr) {
	using namespace ACTION_DANTE;
	using namespace ACTION_VERGIL;
	if (!actorBaseAddr) return;
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
	auto& gamepad = GetGamepad(actorData.newPlayerIndex);
	auto tiltDirection = GetRelativeTiltDirection(actorData);
	auto inAir = (actorData.state & STATE::IN_AIR);
	auto lockOn = (actorData.buttons[0] & GetBinding(BINDING::LOCK_ON));
	auto playerIndex = actorData.newPlayerIndex;
	auto& actionTimer = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].actionTimer : crimsonPlayer[playerIndex].actionTimerClone;
	auto& storedAirCounts = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].storedAirCounts : crimsonPlayer[playerIndex].storedAirCountsClone;
	auto& airCounts = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].airCounts : crimsonPlayer[playerIndex].airCountsClone;

    if (actorData.character == CHARACTER::DANTE) {
		if ((actorData.action == REBELLION_STINGER_LEVEL_2 || actorData.action == REBELLION_STINGER_LEVEL_1) && actorData.state & STATE::IN_AIR && crimsonPlayer[playerIndex].actionTimer > 0.4f
			&& !storedAirCounts.cancelTrackerRunning) {

			storedAirCounts.trickUp = actorData.newTrickUpCount;
			storedAirCounts.skyStar = actorData.newSkyStarCount;
			storedAirCounts.airHike = actorData.newAirHikeCount;
			storedAirCounts.airStinger = actorData.newAirStingerCount;
            storedAirCounts.airTornado = airCounts.airTornado;

			actorData.action = ROYAL_AIR_BLOCK;

			std::thread aircountstracker(AirCancelCountsTracker, actorBaseAddr);
			aircountstracker.detach();
		}
        else {

        }
    }
    else if (actorData.character == CHARACTER::VERGIL) {
		if ((actorData.action == YAMATO_FORCE_EDGE_STINGER_LEVEL_2 || actorData.action == YAMATO_FORCE_EDGE_STINGER_LEVEL_1) && actorData.state & STATE::IN_AIR && crimsonPlayer[playerIndex].actionTimer > 0.4f
			&& !storedAirCounts.cancelTrackerRunning) {

			storedAirCounts.trickUp = actorData.newTrickUpCount;
			storedAirCounts.skyStar = actorData.newSkyStarCount;
			storedAirCounts.airHike = actorData.newAirHikeCount;
			storedAirCounts.airStinger = actorData.newAirStingerCount;
            storedAirCounts.airTornado = airCounts.airTornado;

			actorData.action = ROYAL_AIR_BLOCK;

			std::thread aircountstracker(AirCancelCountsTracker, actorBaseAddr);
			aircountstracker.detach();
		}
    }
}

void DanteStingerInputCrazyCombo(byte8* actorBaseAddr) {
	using namespace ACTION_DANTE;
	if (!actorBaseAddr) return;
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);

	auto playerIndex = actorData.newPlayerIndex; // simply using actorData.newPlayerIndex also works here.
	auto entityIndex = actorData.newEntityIndex;

	auto tiltDirection = GetRelativeTiltDirection(actorData);

	auto& gamepad = GetGamepad(playerIndex);

	// if the player ptr we fetched is a Clone then we use action/animTimers Clone, if not then use the normal ones instead.
	auto actionTimer =
		(entityIndex == ENTITY::MAIN) ? crimsonPlayer[playerIndex].actionTimer : crimsonPlayer[playerIndex].actionTimerClone;
	auto animTimer =
		(entityIndex == ENTITY::MAIN) ? crimsonPlayer[playerIndex].animTimer : crimsonPlayer[playerIndex].animTimerClone;
	auto motionIndex = actorData.motionData[0].index;
	// --- Melee button hold timer ---
	auto& stingerInput = (entityIndex == ENTITY::MAIN) ? crimsonPlayer[playerIndex].stingerInput : crimsonPlayer[playerIndex].stingerInputClone;
	bool meleeDown = (gamepad.buttons[0] & GetBinding(BINDING::MELEE_ATTACK)) != 0;

	if (meleeDown) {
		stingerInput.meleeButtonHold += ImGui::GetIO().DeltaTime;
	}
	else {
		stingerInput.meleeButtonHold = 0.0f;
	}

	if (actorData.action == REBELLION_STINGER_LEVEL_1 ||
		actorData.action == REBELLION_STINGER_LEVEL_2) {
		if (!meleeDown && actionTimer > 0.05f) { // Short grace period
			stingerInput.meleeReleasedStinger = true;
		}
	}
	else if (actionTimer <= 0.0f) {
		stingerInput.meleeReleasedStinger = false;
	}
}

void ImprovedCancelsRoyalguardController(byte8* actorBaseAddr) {
    using namespace ACTION_DANTE;
    using namespace ACTION_VERGIL;
    using namespace NEXT_ACTION_REQUEST_POLICY;

    // This used to be Reset Permissions Controller, which we'll now use for Improved Cancels (Royalguard) - Mia.
    /*if (
            !activeConfig.resetPermissions ||
            !actorBaseAddr ||
            (actorBaseAddr == g_playerActorBaseAddrs[0]) ||
            (actorBaseAddr == g_playerActorBaseAddrs[1]))

            return;
    }*/

    if (!actorBaseAddr) {
        return;
    }
    auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);

    auto& gamepad = GetGamepad(actorData.newPlayerIndex);

    auto tiltDirection = GetRelativeTiltDirection(actorData);

    auto inAir = (actorData.state & STATE::IN_AIR);

    auto lockOn = (actorData.buttons[0] & GetBinding(BINDING::LOCK_ON));
    auto playerIndex = actorData.newPlayerIndex;
    auto& actionTimer = (actorData.newEntityIndex == 0) ? 
        crimsonPlayer[playerIndex].actionTimer : crimsonPlayer[playerIndex].actionTimerClone;
	auto& actorCancels = (actorData.newEntityIndex == 0) ?
		crimsonPlayer[playerIndex].cancels : crimsonPlayer[playerIndex].cancelsClone;
    auto& storedAirCounts = (actorData.newEntityIndex == 0) ? 
        crimsonPlayer[playerIndex].storedAirCounts : crimsonPlayer[playerIndex].storedAirCountsClone;
	auto& airCounts = (actorData.newEntityIndex == 0) ?
		crimsonPlayer[playerIndex].airCounts : crimsonPlayer[playerIndex].airCountsClone;

	if ((actorData.action == CERBERUS_REVOLVER_LEVEL_1 || actorData.action == CERBERUS_REVOLVER_LEVEL_2) && actorCancels.revolverTimer < 1.1f) {
		if (inAir) {
			actorCancels.revolverTimerRunning = true;
		} else {
			actorCancels.revolverTimerRunning = false;
		}
	} else {
		actorCancels.revolverTimerRunning = false;
	}

    bool inCancellableBasicRebellion = (actorData.action == REBELLION_COMBO_1_PART_1 || actorData.action == REBELLION_COMBO_1_PART_1 ||
        actorData.action == REBELLION_COMBO_1_PART_1 || actorData.action == REBELLION_COMBO_1_PART_2 ||
        actorData.action == REBELLION_COMBO_1_PART_3 || actorData.action == REBELLION_COMBO_2_PART_2 ||
        actorData.action == REBELLION_COMBO_2_PART_3 || actorData.action == REBELLION_PROP || actorData.action == REBELLION_SHREDDER ||
		actorData.action == REBELLION_DRIVE_1 || actorData.action == REBELLION_DRIVE_2 || actorData.action == REBELLION_DANCE_MACABRE_PART_1 || actorData.action == REBELLION_DANCE_MACABRE_PART_2 ||
		actorData.action == REBELLION_DANCE_MACABRE_PART_3 || actorData.action == REBELLION_DANCE_MACABRE_PART_4 ||
		actorData.action == REBELLION_DANCE_MACABRE_PART_5 || actorData.action == REBELLION_DANCE_MACABRE_PART_6 ||
		actorData.action == REBELLION_DANCE_MACABRE_PART_7 || actorData.action == REBELLION_DANCE_MACABRE_PART_8);

    bool inCancellableActionRebellion =
        (actorData.action == REBELLION_MILLION_STAB ||
            actorData.action == REBELLION_CRAZY_DANCE || actorData.action == POLE_PLAY);

    bool inCancellableBasicCerberus = (actorData.action == CERBERUS_COMBO_1_PART_1 || actorData.action == CERBERUS_COMBO_1_PART_2 ||
        actorData.action == CERBERUS_COMBO_1_PART_3 || actorData.action == CERBERUS_COMBO_1_PART_4 ||
        actorData.action == CERBERUS_COMBO_1_PART_5 || actorData.action == CERBERUS_COMBO_2_PART_3 ||
        actorData.action == CERBERUS_COMBO_2_PART_4 || actorData.action == CERBERUS_WINDMILL || actorData.action == CERBERUS_FLICKER
        || actorData.action == CERBERUS_SWING);

    bool inCancellableActionCerberus =
        ((actorData.action == CERBERUS_REVOLVER_LEVEL_1 && actorCancels.revolverTimer > 1.1f) || (actorData.action == CERBERUS_REVOLVER_LEVEL_2 && actorCancels.revolverTimer > 1.1f)
            || actorData.action == CERBERUS_SATELLITE ||
            actorData.action == CERBERUS_CRYSTAL || actorData.action == CERBERUS_MILLION_CARATS || actorData.action == CERBERUS_ICE_AGE);

	bool inCancellableBasicAgni = (actorData.action == AGNI_RUDRA_COMBO_1_PART_1 || actorData.action == AGNI_RUDRA_COMBO_1_PART_2 ||
		actorData.action == AGNI_RUDRA_COMBO_1_PART_3 || actorData.action == AGNI_RUDRA_COMBO_1_PART_4 ||
		actorData.action == AGNI_RUDRA_COMBO_1_PART_5 || actorData.action == AGNI_RUDRA_COMBO_2_PART_2 ||
		actorData.action == AGNI_RUDRA_COMBO_2_PART_3 || actorData.action == AGNI_RUDRA_COMBO_3_PART_3 ||
		actorData.action == AGNI_RUDRA_JET_STREAM_LEVEL_1 || actorData.action == AGNI_RUDRA_JET_STREAM_LEVEL_2 ||
		actorData.action == AGNI_RUDRA_JET_STREAM_LEVEL_3);

    bool inCancellableActionAgni =
        (actorData.action == AGNI_RUDRA_MILLION_SLASH ||
            actorData.action == AGNI_RUDRA_TWISTER || actorData.action == AGNI_RUDRA_TEMPEST);

	bool inCancellableBasicNevan = (actorData.action == NEVAN_TUNE_UP || actorData.action == NEVAN_COMBO_1 || actorData.action == NEVAN_COMBO_2 ||
		actorData.action == NEVAN_BAT_RIFT_LEVEL_1 ||
		actorData.action == NEVAN_BAT_RIFT_LEVEL_2  || actorData.action == NEVAN_SLASH);

    bool inCancellableActionNevan =
        (actorData.action == NEVAN_JAM_SESSION || actorData.action == NEVAN_FEEDBACK ||
            actorData.action == NEVAN_CRAZY_ROLL || actorData.action == NEVAN_DISTORTION);

	bool inCancellableBasicBeowulf = (actorData.action == BEOWULF_COMBO_1_PART_1 || actorData.action == BEOWULF_COMBO_1_PART_2 ||
		actorData.action == BEOWULF_COMBO_1_PART_3 || actorData.action == BEOWULF_COMBO_2_PART_3 ||
		actorData.action == BEOWULF_COMBO_2_PART_4 || actorData.action == BEOWULF_BEAST_UPPERCUT);

    bool inCancellableActionBeowulf = (actorData.action == BEOWULF_HYPER_FIST);

    bool inCancellableActionGuns =
        (actorData.action == EBONY_IVORY_WILD_STOMP || actorData.action == ARTEMIS_ACID_RAIN || actorData.action == KALINA_ANN_GRAPPLE);

    // These are moves used by the Action Set Cancel Method, generally air ones.
    bool inCancellableMovesActionMethod =
        ((((actorData.action == REBELLION_AERIAL_RAVE_PART_1 || actorData.action == REBELLION_AERIAL_RAVE_PART_2 ||
              actorData.action == REBELLION_AERIAL_RAVE_PART_3 || actorData.action == REBELLION_AERIAL_RAVE_PART_4) ||
             (actorData.action == AGNI_RUDRA_SKY_DANCE_PART_1 || actorData.action == AGNI_RUDRA_SKY_DANCE_PART_2 ||
                 actorData.action == AGNI_RUDRA_SKY_DANCE_PART_3) ||
             (actorData.action == NEVAN_AIR_SLASH_PART_1 || actorData.action == NEVAN_AIR_SLASH_PART_2) ||
             (actorData.action == CERBERUS_AIR_FLICKER) || (actorData.action == BEOWULF_TORNADO)) && actionTimer >= 0.25f || (actorData.action == EBONY_IVORY_AIR_NORMAL_SHOT 
                 && actionTimer >= 0.2f)) &&
            actorData.eventData[0].event == 17);

    auto& policy = actorData.nextActionRequestPolicy[MELEE_ATTACK];
    auto& trickDashTimer = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].trickDashTimer : crimsonPlayer[playerIndex].trickDashTimerClone;
	auto& canRoyalMagnetism = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].canRoyalMagnetism : crimsonPlayer[playerIndex].canRoyalMagnetismClone;

	

    // What Royalguard Cancels Completely and at any frame (Not Basic Combos mainly Crazy Combos)
    if ((actorData.style == STYLE::ROYALGUARD) && (actorData.buttons[2] & GetBinding(BINDING::STYLE_ACTION)) &&
        actorData.eventData[0].event != ACTOR_EVENT::STAGGER && actorData.eventData[0].event != ACTOR_EVENT::NEVAN_KISS &&
        (inCancellableActionRebellion || inCancellableActionCerberus || inCancellableActionAgni || inCancellableActionNevan ||
            inCancellableActionBeowulf || inCancellableActionGuns || (actorData.eventData[0].event == ACTOR_EVENT::TRICKSTER_DASH) && trickDashTimer > 0.3f) &&
        !storedAirCounts.cancelTrackerRunning) // The last condition prevents cancelling recovery
    {
        storedAirCounts.trickUp = actorData.newTrickUpCount;
        storedAirCounts.skyStar = actorData.newSkyStarCount;
        storedAirCounts.airHike = actorData.newAirHikeCount;
        storedAirCounts.airStinger = actorData.newAirStingerCount;
        storedAirCounts.airTornado = airCounts.airTornado;

        actorData.permissions = 3080; // This is a softer version of Reset Permissions.

		std::thread royalcountstracker(AirCancelCountsTracker, actorBaseAddr);
		royalcountstracker.detach();
    }

    // Basic Combos and Moves should only cancellable if the player can do an attack
    if ((actorData.style == STYLE::ROYALGUARD) && (actorData.buttons[2] & GetBinding(BINDING::STYLE_ACTION)) &&
        actorData.eventData[0].event != ACTOR_EVENT::STAGGER && actorData.eventData[0].event != ACTOR_EVENT::NEVAN_KISS &&
        (inCancellableBasicRebellion || inCancellableBasicCerberus || inCancellableBasicAgni || inCancellableBasicNevan ||
            inCancellableBasicBeowulf) && (policy == EXECUTE) &&
        !storedAirCounts.cancelTrackerRunning) // The last condition prevents cancelling recovery
    {
        storedAirCounts.trickUp = actorData.newTrickUpCount;
        storedAirCounts.skyStar = actorData.newSkyStarCount;
        storedAirCounts.airHike = actorData.newAirHikeCount;
        storedAirCounts.airStinger = actorData.newAirStingerCount;
        storedAirCounts.airTornado = airCounts.airTornado;

        actorData.permissions = 3080; // This is a softer version of Reset Permissions.

        std::thread royalcountstracker(AirCancelCountsTracker, actorBaseAddr);
        royalcountstracker.detach();
    }

    // Royal Cancelling Sky Star
    if ((actorData.style == STYLE::ROYALGUARD) && (actorData.buttons[2] & GetBinding(BINDING::STYLE_ACTION)) &&
        actorData.eventData[0].event == 23 && !storedAirCounts.cancelTrackerRunning) {


		storedAirCounts.trickUp = actorData.newTrickUpCount;
		storedAirCounts.skyStar = actorData.newSkyStarCount;
		storedAirCounts.airHike = actorData.newAirHikeCount;
		storedAirCounts.airStinger = actorData.newAirStingerCount;
        storedAirCounts.airTornado = airCounts.airTornado;

        actorData.permissions = 0x1C1B; // This is a hard version of Reset Permissions.

		std::thread royalcountstracker(AirCancelCountsTracker, actorBaseAddr);
		royalcountstracker.detach();
    }


    // This is another method for Royal Cancels that involves setting the Actor's Action to a newly created Air Block one (only sets for a
    // split second). It's more reliable for cancelling certain moves (especially air ones). - Mia
	// This enables ROYAL MAGNETISM.
    if (actorData.style == STYLE::ROYALGUARD && (actorData.eventData[0].event == 23 || inCancellableMovesActionMethod)
		&& actionTimer >= 0.2f && canRoyalMagnetism) {
        if (inAir) {

            if ((!(lockOn && tiltDirection == TILT_DIRECTION::UP)) && gamepad.buttons[0] & GetBinding(BINDING::STYLE_ACTION) &&
				actorData.styleLevel >= 1) {

				StoreAirCounts(actorData);

                actorData.action = ROYAL_AIR_BLOCK;
				actorData.state = 65538;

				//canRoyalMagnetism = false; // This prevents the player from doing ROYAL MAGNETISM multiple times in a row without touching the ground or JCing.

				std::thread royalcountstracker(AirCancelCountsTracker, actorBaseAddr);
				royalcountstracker.detach();

				
            }
        }
    }

	// Reset ROYAL MAGNETISM availability when touching the ground or JCing.
	if (actorData.state == 65537 || actorData.eventData[0].event == ACTOR_EVENT::LANDING || 
		actorData.eventData[0].event == ACTOR_EVENT::JUMP_CANCEL || actorData.eventData[0].event == ACTOR_EVENT::JUMP ||
		actorData.eventData[0].event == 3 || actorData.eventData[0].event == ACTOR_EVENT::ROYALGUARD_BLOCK) {
		canRoyalMagnetism = true; 
	}

    /*if (actorData.buttons[2] & GetBinding(BINDING::TAUNT))  // old ddmk Reset Permissions -- Deprecated.
    {
            actorData.permissions = 0x1C1B;
    }*/
}


void ImprovedCancelsDanteController(byte8* actorBaseAddr) {
    using namespace ACTION_DANTE;
    using namespace NEXT_ACTION_REQUEST_POLICY;

    if (!actorBaseAddr) {
        return;
    }
    auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    auto lockOn = (actorData.buttons[0] & GetBinding(BINDING::LOCK_ON));
    auto tiltDirection = GetRelativeTiltDirection(actorData);
    auto playerIndex = actorData.newPlayerIndex;
    auto& actionTimer = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].actionTimer : crimsonPlayer[playerIndex].actionTimerClone;

    bool inCancellableActionRebellion =
        (actorData.action == REBELLION_COMBO_1_PART_1 || actorData.action == REBELLION_COMBO_1_PART_1 ||
            actorData.action == REBELLION_COMBO_1_PART_1 || actorData.action == REBELLION_COMBO_1_PART_2 ||
            actorData.action == REBELLION_COMBO_1_PART_3 || actorData.action == REBELLION_COMBO_2_PART_2 ||
            actorData.action == REBELLION_COMBO_2_PART_3 || actorData.action == REBELLION_PROP || actorData.action == REBELLION_SHREDDER ||
            actorData.action == REBELLION_DRIVE_1 || actorData.action == REBELLION_DRIVE_2 || actorData.action == REBELLION_MILLION_STAB ||
            actorData.action == REBELLION_DANCE_MACABRE_PART_1 || actorData.action == REBELLION_DANCE_MACABRE_PART_2 ||
            actorData.action == REBELLION_DANCE_MACABRE_PART_3 || actorData.action == REBELLION_DANCE_MACABRE_PART_4 ||
            actorData.action == REBELLION_DANCE_MACABRE_PART_5 || actorData.action == REBELLION_DANCE_MACABRE_PART_6 ||
            actorData.action == REBELLION_DANCE_MACABRE_PART_7 || actorData.action == REBELLION_DANCE_MACABRE_PART_8 ||
            actorData.action == REBELLION_CRAZY_DANCE || actorData.action == POLE_PLAY || actorData.action == REBELLION_SWORD_PIERCE);

    bool inCancellableActionCerberus =
        (actorData.action == CERBERUS_COMBO_1_PART_1 || actorData.action == CERBERUS_COMBO_1_PART_2 ||
            actorData.action == CERBERUS_COMBO_1_PART_3 || actorData.action == CERBERUS_COMBO_1_PART_4 ||
            actorData.action == CERBERUS_COMBO_1_PART_5 || actorData.action == CERBERUS_COMBO_2_PART_3 ||
            actorData.action == CERBERUS_COMBO_2_PART_4 || actorData.action == CERBERUS_WINDMILL ||
            (actorData.action == CERBERUS_REVOLVER_LEVEL_1 && actionTimer > 1.25f) || (actorData.action == CERBERUS_REVOLVER_LEVEL_2 && actionTimer > 1.25f) ||
            actorData.action == CERBERUS_SWING || actorData.action == CERBERUS_SATELLITE || actorData.action == CERBERUS_FLICKER ||
            actorData.action == CERBERUS_CRYSTAL || actorData.action == CERBERUS_MILLION_CARATS);

    bool inCancellableActionAgni =
        (actorData.action == AGNI_RUDRA_COMBO_1_PART_1 || actorData.action == AGNI_RUDRA_COMBO_1_PART_2 ||
            actorData.action == AGNI_RUDRA_COMBO_1_PART_3 || actorData.action == AGNI_RUDRA_COMBO_1_PART_4 ||
            actorData.action == AGNI_RUDRA_COMBO_1_PART_5 || actorData.action == AGNI_RUDRA_COMBO_2_PART_2 ||
            actorData.action == AGNI_RUDRA_COMBO_2_PART_3 || actorData.action == AGNI_RUDRA_COMBO_3_PART_3 ||
            actorData.action == AGNI_RUDRA_JET_STREAM_LEVEL_1 || actorData.action == AGNI_RUDRA_JET_STREAM_LEVEL_2 ||
            actorData.action == AGNI_RUDRA_JET_STREAM_LEVEL_3 || actorData.action == AGNI_RUDRA_MILLION_SLASH ||
            actorData.action == AGNI_RUDRA_TWISTER || actorData.action == AGNI_RUDRA_TEMPEST ||
            actorData.action == AGNI_RUDRA_WHIRLWIND_LAUNCH);

    bool inCancellableActionNevan =
        (actorData.action == NEVAN_TUNE_UP || actorData.action == NEVAN_COMBO_1 || actorData.action == NEVAN_COMBO_2 ||
            actorData.action == NEVAN_JAM_SESSION || actorData.action == NEVAN_BAT_RIFT_LEVEL_1 ||
            actorData.action == NEVAN_BAT_RIFT_LEVEL_2 || actorData.action == NEVAN_REVERB_SHOCK_LEVEL_1 ||
            actorData.action == NEVAN_REVERB_SHOCK_LEVEL_2 || actorData.action == NEVAN_SLASH || actorData.action == NEVAN_FEEDBACK ||
            actorData.action == NEVAN_CRAZY_ROLL || actorData.action == NEVAN_DISTORTION);

    bool inCancellableActionBeowulf = (actorData.action == BEOWULF_COMBO_1_PART_1 || actorData.action == BEOWULF_COMBO_1_PART_2 ||
        actorData.action == BEOWULF_COMBO_1_PART_3 || actorData.action == BEOWULF_COMBO_2_PART_3 ||
        actorData.action == BEOWULF_COMBO_2_PART_4 || actorData.action == BEOWULF_BEAST_UPPERCUT ||
        actorData.action == BEOWULF_HYPER_FIST || actorData.action == BEOWULF_TORNADO);

    bool inCancellableActionGuns =
        (actorData.action == EBONY_IVORY_WILD_STOMP || actorData.action == ARTEMIS_ACID_RAIN || actorData.action == KALINA_ANN_GRAPPLE);

    bool inCancellableActionAirSwordmaster =
        ((actorData.action == REBELLION_AERIAL_RAVE_PART_1 || actorData.action == REBELLION_AERIAL_RAVE_PART_2 ||
            actorData.action == REBELLION_AERIAL_RAVE_PART_3 || actorData.action == REBELLION_AERIAL_RAVE_PART_4) ||
            (actorData.action == AGNI_RUDRA_SKY_DANCE_PART_1 || actorData.action == AGNI_RUDRA_SKY_DANCE_PART_2 ||
                actorData.action == AGNI_RUDRA_SKY_DANCE_PART_3) ||
            (actorData.action == NEVAN_AIR_SLASH_PART_1 || actorData.action == NEVAN_AIR_SLASH_PART_2) ||
            (actorData.action == CERBERUS_AIR_FLICKER) || (actorData.action == BEOWULF_TORNADO));

    bool inCancellableActionAirGunslinger = (actorData.action == SHOTGUN_AIR_FIREWORKS || actorData.action == ARTEMIS_AIR_NORMAL_SHOT ||
        actorData.action == ARTEMIS_AIR_MULTI_LOCK_SHOT);
    bool inGunsMove = (actorData.action == EBONY_IVORY_AIR_NORMAL_SHOT || actorData.action == SHOTGUN_AIR_NORMAL_SHOT ||
        actorData.action == ARTEMIS_AIR_NORMAL_SHOT || actorData.action == ARTEMIS_AIR_MULTI_LOCK_SHOT || actorData.action == SPIRAL_NORMAL_SHOT
        || actorData.action == SPIRAL_TRICK_SHOT || actorData.action == KALINA_ANN_NORMAL_SHOT);

    if (playerIndex >= PLAYER_COUNT) playerIndex = 0;
    auto characterIndex = actorData.newCharacterIndex;
    if (characterIndex >= CHARACTER_COUNT) characterIndex = 0;
    auto entityIndex = actorData.newEntityIndex;
    if (entityIndex >= ENTITY_COUNT) entityIndex = 0;

    auto& playerData = GetPlayerData(playerIndex);
    auto& gamepad = GetGamepad(playerIndex);
    auto& skyLaunch = crimsonPlayer[playerIndex].skyLaunch;

    // --- Button press detection arrays ---
    static bool prevStyleButton[PLAYER_COUNT][ENTITY_COUNT] = {};
    static bool prevShootButton[PLAYER_COUNT][ENTITY_COUNT] = {};
    static bool prevStyleButton2[PLAYER_COUNT][ENTITY_COUNT] = {};

    // Buffer for style button press (per player/entity)
    static float styleButtonBuffer[PLAYER_COUNT][ENTITY_COUNT] = {};
    constexpr float STYLE_BUTTON_BUFFER_TIME = 0.2f; // 200ms buffer

    // Buffer for gunslinger style button (per player/entity)
    static float gunStyleButtonBuffer[PLAYER_COUNT][ENTITY_COUNT] = {};
    constexpr float GUN_STYLE_BUTTON_BUFFER_TIME = 0.2f;

    // Buffer for rainstorm style button (per player/entity)
    static float rainstormStyleButtonBuffer[PLAYER_COUNT][ENTITY_COUNT] = {};
    constexpr float RAINSTORM_STYLE_BUTTON_BUFFER_TIME = 0.2f;

    // Buffer for swordmaster cancel (per player/entity)
    static float swordmasterStyleButtonBuffer[PLAYER_COUNT][ENTITY_COUNT] = {};
    constexpr float SWORDMASTER_STYLE_BUTTON_BUFFER_TIME = 0.2f;

    // Buffer for gunshot cancel (per player/entity)
    static float gunshotButtonBuffer[PLAYER_COUNT][ENTITY_COUNT] = {};
    constexpr float GUNSHOT_BUTTON_BUFFER_TIME = 0.2f;

    // Detect button press (not hold)
    bool styleButtonDown = (gamepad.buttons[0] & GetBinding(BINDING::STYLE_ACTION)) != 0;
    bool styleButtonPressed = styleButtonDown && !prevStyleButton[playerIndex][entityIndex];
    prevStyleButton[playerIndex][entityIndex] = styleButtonDown;

    bool shootButtonDown = (gamepad.buttons[0] & GetBinding(BINDING::SHOOT)) != 0;
    bool shootButtonPressed = shootButtonDown && !prevShootButton[playerIndex][entityIndex];
    prevShootButton[playerIndex][entityIndex] = shootButtonDown;

    // For buttons[2] (used in some cancels)
    bool styleButton2Down = (actorData.buttons[2] & GetBinding(BINDING::STYLE_ACTION)) != 0;
    bool styleButton2Pressed = styleButton2Down && !prevStyleButton2[playerIndex][entityIndex];
    prevStyleButton2[playerIndex][entityIndex] = styleButton2Down;
    // -------------------------------------

    // Lambda for updating a buffer
    auto updateBuffer = [](bool pressed, float& buffer, float bufferTime, float deltaTime) {
        if (pressed) {
            buffer = bufferTime;
        } else if (buffer > 0.0f) {
            buffer -= deltaTime;
            if (buffer < 0.0f) buffer = 0.0f;
        }
        };

    // Update all buffers
    updateBuffer(styleButtonPressed, styleButtonBuffer[playerIndex][entityIndex], STYLE_BUTTON_BUFFER_TIME, ImGui::GetIO().DeltaTime);
    updateBuffer(styleButtonPressed, gunStyleButtonBuffer[playerIndex][entityIndex], GUN_STYLE_BUTTON_BUFFER_TIME, ImGui::GetIO().DeltaTime);
    updateBuffer(styleButton2Pressed, rainstormStyleButtonBuffer[playerIndex][entityIndex], RAINSTORM_STYLE_BUTTON_BUFFER_TIME, ImGui::GetIO().DeltaTime);
    updateBuffer(styleButton2Pressed, swordmasterStyleButtonBuffer[playerIndex][entityIndex], SWORDMASTER_STYLE_BUTTON_BUFFER_TIME, ImGui::GetIO().DeltaTime);
    updateBuffer((actorData.buttons[2] & GetBinding(BINDING::SHOOT)) && !prevShootButton[playerIndex][entityIndex],
        gunshotButtonBuffer[playerIndex][entityIndex], GUNSHOT_BUTTON_BUFFER_TIME, ImGui::GetIO().DeltaTime);

    static bool executes[PLAYER_COUNT][CHARACTER_COUNT][ENTITY_COUNT][4] = {};
    auto& cancels = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].cancels : crimsonPlayer[playerIndex].cancelsClone;
    bool doingAirTrick = (actorData.buttons[0] & GetBinding(BINDING::STYLE_ACTION)
        && actorData.style == STYLE::TRICKSTER && actorData.lockOn && tiltDirection == TILT_DIRECTION::UP);
	bool doingSkyStar = (actorData.buttons[0] & GetBinding(BINDING::STYLE_ACTION)
		&& actorData.style == STYLE::TRICKSTER);
    bool doingTricksterDash = (actorData.buttons[0] & GetBinding(BINDING::STYLE_ACTION) && actorData.state & STATE::ON_FLOOR
        && actorData.style == STYLE::TRICKSTER && !doingAirTrick);
    bool doingJump = (actorData.buttons[0] & GetBinding(BINDING::JUMP));
    auto& closeToEnemy = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].isCloseToEnemy : crimsonPlayer[playerIndex].isCloseToEnemyClone;

    auto& policy = actorData.nextActionRequestPolicy[MELEE_ATTACK];
    auto& policyTrick = actorData.nextActionRequestPolicy[TRICKSTER_DARK_SLAYER];
	auto& policyJump = actorData.nextActionRequestPolicy[JUMP_ROLL];

    if (actorData.character == CHARACTER::DANTE) {

        // Improve Prop/Shredder Trick Buffering
        if (actorData.action == REBELLION_PROP || actorData.action == REBELLION_SHREDDER) {
            policyTrick = BUFFER;
            if (doingAirTrick && actionTimer > 0.47f) {
                policyTrick = EXECUTE;
            }
        }

        // Improve Twister/Tempest Trick Buffering
        if (actorData.action == AGNI_RUDRA_TWISTER || actorData.action == AGNI_RUDRA_TEMPEST) {
            policyTrick = BUFFER;
            if (doingAirTrick && actionTimer > 0.50f) {
                policyTrick = EXECUTE;
            }
        }

        // Improved Air Swordmaster Moves (and Air Stinger) Trick Buffering
        if (actorData.action == CERBERUS_AIR_FLICKER || actorData.action == AGNI_RUDRA_SKY_DANCE_PART_1
            || actorData.action == AGNI_RUDRA_SKY_DANCE_PART_2 || actorData.action == AGNI_RUDRA_SKY_DANCE_PART_3
            || actorData.action == BEOWULF_THE_HAMMER || actorData.action == BEOWULF_TORNADO || 
            (actorData.action == REBELLION_STINGER_LEVEL_1 || actorData.action == REBELLION_STINGER_LEVEL_2 && actorData.state & STATE::IN_AIR)) {
            policyTrick = BUFFER;
            if (doingAirTrick || doingSkyStar && actionTimer > 0.13f) {
                policyTrick = EXECUTE;
            }
        }
		

        // Improved Cerberus' Crystal/Million Carats Trick Buffering
        if (actorData.action == CERBERUS_CRYSTAL || actorData.action == CERBERUS_MILLION_CARATS) {
            policyTrick = BUFFER;
            if (doingAirTrick && actionTimer > 0.45f) {
                policyTrick = EXECUTE;
            }
        }

        // Dante's Trickster Actions Cancels Most Things (w/ cooldown)
        if ((actorData.style == STYLE::TRICKSTER) &&
            (actorData.eventData[0].event != ACTOR_EVENT::TRICKSTER_DASH &&
                (inCancellableActionRebellion || inCancellableActionCerberus || inCancellableActionAgni || inCancellableActionNevan ||
                    inCancellableActionBeowulf || inCancellableActionGuns || inCancellableActionAirSwordmaster ||
                    inCancellableActionAirGunslinger || actorData.action == EBONY_IVORY_RAIN_STORM) ||
                skyLaunch.executing || actorData.eventData[0].event == ACTOR_EVENT::TRICKSTER_SKY_STAR) && (policy == BUFFER || policy == EXECUTE)) {
            if (styleButtonBuffer[playerIndex][entityIndex] > 0.0f) { // previously gamepad.buttons[0] & GetBinding(BINDING::STYLE_ACTION)
                if (cancels.canTrick) {
                    actorData.state &= ~STATE::BUSY;
                    cancels.canTrick = false;
                    styleButtonBuffer[playerIndex][entityIndex] = 0.0f; // consume buffer
                }
            }
        }

        // Gunslinger Cancels Most Things (w/ cooldown)
        //  They can also cancel themselves.
        if ((actorData.style == STYLE::GUNSLINGER) && (actorData.state == STATE::IN_AIR || actorData.state == 65538) &&
            (inCancellableActionAirSwordmaster || inCancellableActionAirGunslinger || actorData.eventData[0].event == ACTOR_EVENT::TRICKSTER_SKY_STAR ||
				actorData.eventData[0].event == ACTOR_EVENT::TRICKSTER_AIR_TRICK || actorData.eventData[0].event == ACTOR_EVENT::JUMP_CANCEL || 
                actorData.motionData[0].index == 15) &&
            actorData.action != EBONY_IVORY_RAIN_STORM) {
            if (gamepad.buttons[0] & GetBinding(BINDING::STYLE_ACTION)) { // previously gunStyleButtonBuffer[playerIndex][entityIndex] > 0.0f
                if (cancels.canGun) {
                    actorData.state &= ~STATE::BUSY;
                    cancels.canGun = false;
                    //gunStyleButtonBuffer[playerIndex][entityIndex] = 0.0f; // consume buffer
                }
            }
        }

        // but Rainstorm is an exception here since I wanted it to have a longer CD.
        if ((actorData.style == STYLE::GUNSLINGER) && (actorData.state == STATE::IN_AIR || actorData.state == 65538) &&
            (crimsonPlayer[playerIndex].cancels.canRainstorm) && (actorData.action == EBONY_IVORY_RAIN_STORM)) {
            if (actorData.buttons[2] & GetBinding(BINDING::STYLE_ACTION)) {
                if (cancels.canRainstorm) {
                    actorData.state &= ~STATE::BUSY;
                    cancels.canRainstorm = false;
                    //rainstormStyleButtonBuffer[playerIndex][entityIndex] = 0.0f; // consume buffer
                }
            }
        }

        // This prevents the double Rainstorm from happening (but I still left it on Fireworks and Artemis Shots).
        if (actorData.action == EBONY_IVORY_RAIN_STORM && actorData.motionData[0].index == 15) {
            if (cancels.canRainstorm) {
                cancels.canRainstorm = false;
            }
        }

        // Reset Timers if you jump cancel
        if (actorData.eventData[0].event == ACTOR_EVENT::JUMP_CANCEL) {
            cancels.canTrick = true;
            cancels.canGun = true;
            cancels.canRainstorm = true;
        }
    }

    old_for_all(uint8, buttonIndex, 4) {
        auto& execute = executes[playerIndex][characterIndex][entityIndex][buttonIndex];
        auto& button = playerData.removeBusyFlagButtons[buttonIndex];

        if (actorData.character == CHARACTER::DANTE) {

            // Swordmaster moves cancel out Trickster dashes
            uint32 eventActor = actorData.eventData[0].event;

            if ((actorData.style == STYLE::SWORDMASTER) &&
                (eventActor == ACTOR_EVENT::TRICKSTER_SKY_STAR || eventActor == ACTOR_EVENT::TRICKSTER_DASH)) {
                if (actorData.buttons[2] & GetBinding(BINDING::STYLE_ACTION)) { // previously swordmasterStyleButtonBuffer[playerIndex][entityIndex] > 0.0f
                    if (execute) {
                        execute = false;
                        actorData.state &= ~STATE::BUSY;
                        swordmasterStyleButtonBuffer[playerIndex][entityIndex] = 0.0f; // consume buffer
                    }
                } else if (!styleButton2Down) {
                    execute = true;
                }
            }

            // Cancel Final Aerial Rave Knockback with Gunshot.
            if (actorData.action == REBELLION_AERIAL_RAVE_PART_4 && actorData.eventData[0].event == 17) {
                if (actorData.buttons[2] & GetBinding(BINDING::SHOOT)) { // previously gunshotButtonBuffer[playerIndex][entityIndex] > 0.0f
                    if (execute) {
                        execute = false;
                        actorData.state &= ~STATE::BUSY;
                        gunshotButtonBuffer[playerIndex][entityIndex] = 0.0f; // consume buffer
                    }
                } else if (!shootButtonDown) {
                    execute = true;
                }
            }

            /*if ((actorData.style == STYLE::ROYALGUARD) &&
                            (actorData.buttons[2] & GetBinding(BINDING::STYLE_ACTION)))
            {
                    if(actorData.action != SPIRAL_NORMAL_SHOT && actorData.action != KALINA_ANN_NORMAL_SHOT &&
                    actorData.action != EBONY_IVORY_AIR_NORMAL_SHOT && actorData.action != SHOTGUN_AIR_NORMAL_SHOT) { // Exceptions, these
            cancels are way too OP or buggy in the cases of E&I and Shotgun. if (execute)
                            {
                                    execute = false;

                                    actorData.state &= ~STATE::BUSY;

                            }

                            else
                            {
                                    execute = true;
                            }
                    }
            }*/
        }
    }
}

void DarkslayerCancelsVergilController(byte8* actorBaseAddr) {
	using namespace ACTION_VERGIL;
    using namespace NEXT_ACTION_REQUEST_POLICY;

	if (!actorBaseAddr || (actorBaseAddr == g_playerActorBaseAddrs[0]) || (actorBaseAddr == g_playerActorBaseAddrs[1])) {
		return;
	}

	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
	auto lockOn = (actorData.buttons[0] & GetBinding(BINDING::LOCK_ON));
	auto tiltDirection = GetRelativeTiltDirection(actorData);

	auto playerIndex = actorData.newPlayerIndex;
	if (playerIndex >= PLAYER_COUNT) {
		playerIndex = 0;
	}

	auto characterIndex = actorData.newCharacterIndex;
	auto entityIndex = actorData.newEntityIndex;


	auto& playerData = GetPlayerData(playerIndex);
	auto& gamepad = GetGamepad(playerIndex);
    auto& actionTimer = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].actionTimer : 
        crimsonPlayer[playerIndex].actionTimerClone;

	static bool executes[PLAYER_COUNT][ENTITY_COUNT] = {};
	static bool prevStyleButton[PLAYER_COUNT][ENTITY_COUNT] = {};
	static float cancelTimers[PLAYER_COUNT][ENTITY_COUNT] = {};

	constexpr float COOLDOWN_MS = 700.0f;

	float deltaTime = ImGui::GetIO().DeltaTime * (actorData.speed / g_FrameRateTimeMultiplier) * 1000.0f; // to ms

	// Detect button press (not hold)
	bool styleButtonDown = (actorData.buttons[0] & GetBinding(BINDING::STYLE_ACTION)) != 0;
	bool styleButtonPressed = styleButtonDown && !prevStyleButton[playerIndex][entityIndex];
	prevStyleButton[playerIndex][entityIndex] = styleButtonDown;

	auto& execute = executes[playerIndex][entityIndex];
	auto& timer = cancelTimers[playerIndex][entityIndex];

	if (timer > 0.0f) {
		timer -= deltaTime;
		if (timer < 0.0f) timer = 0.0f;
	}

	// Aggregate all cancel conditions
	bool canCancel = false;

	if (actorData.character == CHARACTER::VERGIL) {
		// Darkslayer Trick Cancel (ground)
		if (actorData.state != STATE::IN_AIR && actorData.state != 65538 && styleButtonPressed) {
			canCancel = true;
		}
		// TRICK UP (air)
		else if ((actorData.state & STATE::IN_AIR) && styleButtonPressed && lockOn && tiltDirection == TILT_DIRECTION::UP && actorData.trickUpCount > 0) {
			canCancel = true;
		}
		// TRICK DOWN (air)
		else if ((actorData.state & STATE::IN_AIR) && styleButtonPressed && lockOn && tiltDirection == TILT_DIRECTION::DOWN && actorData.trickDownCount > 0) {
			canCancel = true;
		}
		// AIR TRICK (air)
		else if ((actorData.state & STATE::IN_AIR) && styleButtonPressed &&
			(((lockOn && tiltDirection == TILT_DIRECTION::NEUTRAL) || !lockOn) && actorData.airTrickCount > 0)) {
			canCancel = true;
		}
	}

	if (canCancel && execute && timer <= 0.0f) {
		execute = false;
		timer = COOLDOWN_MS;
		actorData.state &= ~STATE::BUSY;
	} else if (!styleButtonDown) {
		execute = true;
	}

	bool doingJump = (actorData.buttons[0] & GetBinding(BINDING::JUMP));
	auto& closeToEnemy = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].isCloseToEnemy : crimsonPlayer[playerIndex].isCloseToEnemyClone;

	auto& policy = actorData.nextActionRequestPolicy[MELEE_ATTACK];
	auto& policyTrick = actorData.nextActionRequestPolicy[TRICKSTER_DARK_SLAYER];
	auto& policyJump = actorData.nextActionRequestPolicy[JUMP_ROLL];

	if ((actorData.action == YAMATO_FORCE_EDGE_STINGER_LEVEL_1 || actorData.action == YAMATO_FORCE_EDGE_STINGER_LEVEL_2 && actorData.state & STATE::IN_AIR)) {
		policyTrick = BUFFER;
		if (styleButtonPressed && actionTimer > 0.13f) {
			actorData.permissions = 3080; // This is a soft version of Reset Permissions.
			policyTrick = EXECUTE;
		}
	}
}

void AirStingerJumpCancel(byte8* actorBaseAddr) {
	using namespace ACTION_VERGIL;
	using namespace ACTION_DANTE;
	using namespace NEXT_ACTION_REQUEST_POLICY;

	if (!actorBaseAddr || (actorBaseAddr == g_playerActorBaseAddrs[0]) || (actorBaseAddr == g_playerActorBaseAddrs[1])) {
		return;
	}
    auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
	
	auto playerIndex = actorData.newPlayerIndex;

	bool doingJump = (actorData.buttons[0] & GetBinding(BINDING::JUMP));
    auto& actionTimer = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].actionTimer : crimsonPlayer[playerIndex].actionTimerClone;
	auto& closeToEnemy = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].isCloseToEnemy : crimsonPlayer[playerIndex].isCloseToEnemyClone;

	auto& policy = actorData.nextActionRequestPolicy[MELEE_ATTACK];
	auto& policyTrick = actorData.nextActionRequestPolicy[TRICKSTER_DARK_SLAYER];
	auto& policyJump = actorData.nextActionRequestPolicy[JUMP_ROLL];

    if (actorData.character == CHARACTER::DANTE) {
		if ((actorData.action == REBELLION_STINGER_LEVEL_1 || actorData.action == REBELLION_STINGER_LEVEL_2 && actorData.state & STATE::IN_AIR)) {
			policyJump = BUFFER;
			if (doingJump && actionTimer > 0.19f) {
				actorData.permissions = 3080; // This cancels the ability
				policyJump = EXECUTE;
			}
		}


    } else if (actorData.character == CHARACTER::VERGIL) {
        if ((actorData.action == YAMATO_FORCE_EDGE_STINGER_LEVEL_1 || actorData.action == YAMATO_FORCE_EDGE_STINGER_LEVEL_2 && actorData.state & STATE::IN_AIR)) {
            policyJump = BUFFER;
            if (doingJump && actionTimer > 0.10f) {
                actorData.permissions = 3080; // This cancels the ability
                policyJump = EXECUTE;
            }
        }
    }
}

uint16 GetRotationTowardsEnemy(byte8* actorBaseAddr) {
	if (!actorBaseAddr || (actorBaseAddr == g_playerActorBaseAddrs[0]) || (actorBaseAddr == g_playerActorBaseAddrs[1])) {
		return 0;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
	auto playerIndex = actorData.newPlayerIndex;

	if (actorData.newEntityIndex == ENTITY::CLONE) {
		return crimsonPlayer[playerIndex].rotationCloneTowardsEnemy;
	}
	return crimsonPlayer[playerIndex].rotationTowardsEnemy;
}

#pragma endregion

#pragma region VergilGameplay

void VergilRisingStar(byte8* actorBaseAddr) {
	using namespace ACTION_VERGIL;

	if (!actorBaseAddr || (actorBaseAddr == g_playerActorBaseAddrs[0]) || (actorBaseAddr == g_playerActorBaseAddrs[1])) {
		return;
	}

	if (!ExpConfig::missionExpDataVergil.unlocks[UNLOCK_VERGIL::YAMATO_RISING_STAR] || !activeCrimsonGameplay.Gameplay.General.extramoves) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    if (actorData.character != CHARACTER::VERGIL) return;
	auto playerIndex = actorData.newPlayerIndex;
	auto entityIndex = actorData.newEntityIndex;
	auto& playerData = GetPlayerData(playerIndex);
	auto& gamepad = GetGamepad(playerIndex);
	auto& actionTimer = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].actionTimer :
		crimsonPlayer[playerIndex].actionTimerClone;
	auto& motionTimer = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].animTimer :
		crimsonPlayer[playerIndex].animTimerClone;
    auto& inRisingStar = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].inRisingStar :
        crimsonPlayer[playerIndex].inRisingStarClone;
	auto& inYamatoHighTime = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].inYamatoHighTime :
		crimsonPlayer[playerIndex].inYamatoHighTimeClone;
	static bool applied[PLAYER_COUNT][ENTITY_COUNT] = { false };
	auto& apply = applied[playerIndex][entityIndex];
	auto tiltDirection = GetRelativeTiltDirection(actorData);
	static bool closeToEnemy[PLAYER_COUNT][ENTITY_COUNT] = { false };
	auto& closeEnemy = closeToEnemy[playerIndex][entityIndex];
	static constexpr const wchar_t* weaponParticlePath = L"Crimson\\vfx\\yamato_sword.efkefc";
	static EffekseerRefHandle weaponParticleRef = CrimsonEfk::LoadEffect(weaponParticlePath, 1.0f);
	static constexpr const wchar_t* risingStarParticlePath = L"Crimson\\vfx\\risingstar.efkefc";
	static EffekseerRefHandle risingStarParticleRef = CrimsonEfk::LoadEffect(risingStarParticlePath, 1.0f);
	auto& vergilSword = *reinterpret_cast<Sword*>(actorData.nextBaseAddr);
	cDrawReverse* vergilSwordcDraw = reinterpret_cast<cDrawReverse*>(vergilSword.swordcDraw); // first cDraw is the katana part
	Matrix44* swordMatrix = reinterpret_cast<Matrix44*>(vergilSwordcDraw[0].bones); // index 1 is the hilt
	cDrawReverse playerVergilcDraw = actorData.newModelData[actorData.activeModelIndexMirror]; // activeModelIndex == which DT or Non-DT model
	Matrix44* boneMatrix = reinterpret_cast<Matrix44*>(playerVergilcDraw.bones);

	static EffekseerHandle weaponParticleHandle[PLAYER_COUNT][ENTITY_COUNT] = { 0 };
	static EffekseerHandle risingStarParticleHandle[PLAYER_COUNT][ENTITY_COUNT] = { 0 };

	// --- Melee button hold timer ---
	static float meleeButtonHold[PLAYER_COUNT][ENTITY_COUNT] = {};
	constexpr float MELEE_HOLD_TIME = 0.2f; // 200 ms

	bool meleeDown = (gamepad.buttons[0] & GetBinding(BINDING::MELEE_ATTACK)) != 0;

	if (meleeDown) {
		meleeButtonHold[playerIndex][entityIndex] += ImGui::GetIO().DeltaTime;
	} else {
		meleeButtonHold[playerIndex][entityIndex] = 0.0f;
	}

	static bool meleeReleasedDuringRapidSlash[PLAYER_COUNT][ENTITY_COUNT] = { false };
	if (actorData.action == YAMATO_RAPID_SLASH_LEVEL_1 || actorData.action == YAMATO_RAPID_SLASH_LEVEL_2) {
		if (!meleeDown && actionTimer > 0.05f) { // Short grace period
			meleeReleasedDuringRapidSlash[playerIndex][entityIndex] = true;
		}
	} else {
		meleeReleasedDuringRapidSlash[playerIndex][entityIndex] = false;
	}
	// ------------------------------

	// Reset closeEnemy each frame
	closeEnemy = false;

	if (actorData.lockOnData.targetBaseAddr60 != nullptr) {
		auto& enemyData = *reinterpret_cast<EnemyActorData*>(actorData.lockOnData.targetBaseAddr60 - 0x60);

		glm::vec3 playerPos(actorData.position.x, actorData.position.y, actorData.position.z);
		glm::vec3 enemyPos(enemyData.position.x, enemyData.position.y, enemyData.position.z);

		float distanceToPlayer = glm::distance(playerPos, enemyPos);

		if (distanceToPlayer <= 100.0f) {
			closeEnemy = true;
		}
	}

	// Improved transition logic:
	// Only allow transition if action is YAMATO_RAPID_SLASH_LEVEL_2 and timer is in the correct window
	// and either the button is held long enough OR player is close to an enemy and button is held long enough
	bool canTransition =
		(actorData.action == YAMATO_RAPID_SLASH_LEVEL_2 || actorData.action == YAMATO_RAPID_SLASH_LEVEL_1) &&
		actionTimer > 0.52f && actionTimer < 0.60f &&
		meleeButtonHold[playerIndex][entityIndex] >= MELEE_HOLD_TIME &&
		!meleeReleasedDuringRapidSlash[playerIndex][entityIndex];

	bool canTransitionClose =
		(actorData.action == YAMATO_RAPID_SLASH_LEVEL_2 || actorData.action == YAMATO_RAPID_SLASH_LEVEL_1) &&
		closeEnemy &&
		meleeButtonHold[playerIndex][entityIndex] >= MELEE_HOLD_TIME &&
		!meleeReleasedDuringRapidSlash[playerIndex][entityIndex];

	// Only allow transition once per action
	if ((canTransition || canTransitionClose)) {
        if (!inRisingStar) {
            actorData.motionArchives[MOTION_GROUP_VERGIL::BEOWULF] = newRisingStar_pl021_00_4; // Swap Rising Sun's animation to Rising Star,
        }
        
		actorData.action = BEOWULF_RISING_SUN;
		PlayAnimation_1EFB90(actorData, 4, 11, 20.0f, 0, 0, -1);
		actorData.recoverState[0] = 1;

		risingStarParticleRef = CrimsonEfk::ReloadEffect(risingStarParticleRef, risingStarParticlePath, 40.0f);
		risingStarParticleHandle[playerIndex][entityIndex] = CrimsonEfk::PlayEffectAtMatrix(risingStarParticleRef, boneMatrix->matrix3, &actorData);
		uint32_t vergilColor = CrimsonUtil::HexToAABBGGRR(0x522BFFFF);
		//CrimsonEfk::SetAllColor(risingStarParticleHandle[playerIndex][entityIndex], vergilColor);
        
		 inRisingStar = true;
	}

	// Reset apply flag when action changes away from YAMATO_RAPID_SLASH_LEVEL_2
	if ((actorData.action == BEOWULF_RISING_SUN && actionTimer > 1.0f) || 
		(actorData.action != BEOWULF_RISING_SUN && actorData.action != YAMATO_RAPID_SLASH_LEVEL_2 &&
			actorData.action != YAMATO_RAPID_SLASH_LEVEL_1) ||
        (actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_AIR_TRICK ||
            actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_TRICK_DOWN ||
            actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_TRICK_UP ||
			actorData.eventData[0].event == ACTOR_EVENT::JUMP ||
			actorData.eventData[0].event == ACTOR_EVENT::JUMP_CANCEL
        ) || actorData.action == YAMATO_JUDGEMENT_CUT_LEVEL_1 ||
		actorData.action == YAMATO_JUDGEMENT_CUT_LEVEL_2 && inRisingStar) {
		actorData.motionArchives[MOTION_GROUP_VERGIL::BEOWULF] = File_staticFiles[pl021_00_4]; // Revert to default pac motion archive after Rising Star finishes or is cancelled by a trick.
		inRisingStar = false;
	}

	if (inRisingStar && actorData.newAirRisingSunCount > 0) {
		actorData.newAirRisingSunCount = 0; // Prevents Rising Star from consuming Air Rising Sun's counts.
	}


	// Controlling Yamato's visibility and parenting during Rising Star.
	if (inRisingStar) {
		if (motionTimer >= 0.09f && motionTimer < 0.95f) {
			actorData.weaponMotionState[7] = 11;
			vergilSword.hideYamatoTip = false;
		}
		else {
			actorData.weaponMotionState[7] = 1;
			vergilSword.hideYamatoTip = true;
		}
	}

	// Complementing Guetto Yamato vfx by hiding the sword during Rising Star and Yamato High Time since the 
	// sword isn't parented to the hand's bone properly (and needs animation).
// 	static bool shouldHide[PLAYER_COUNT][ENTITY_COUNT];
// 	shouldHide[playerIndex][entityIndex] = inRisingStar || inYamatoHighTime;
// 	vergilSwordcDraw[0].visible = !shouldHide[playerIndex][entityIndex];
}

void VergilYamatoHighTime(byte8* actorBaseAddr) {
	using namespace ACTION_VERGIL;

	if (!actorBaseAddr || (actorBaseAddr == g_playerActorBaseAddrs[0]) || (actorBaseAddr == g_playerActorBaseAddrs[1])) {
		return;
	}

	if (!ExpConfig::missionExpDataVergil.unlocks[UNLOCK_VERGIL::YAMATO_HIGH_TIME] || !activeCrimsonGameplay.Gameplay.General.extramoves) {
		return;
	}

	//if (!ExpConfig::missionExpDataVergil.funnymodunlock[HIGH_TIME_ENUM_VALUE]) {
	//	return
	//}

	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    if (actorData.character != CHARACTER::VERGIL) return;
	auto playerIndex = actorData.newPlayerIndex;
	auto entityIndex = actorData.newEntityIndex;
	auto& playerData = GetPlayerData(playerIndex);
	auto& gamepad = GetGamepad(playerIndex);
	auto& actionTimer = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].actionTimer :
		crimsonPlayer[playerIndex].actionTimerClone;
	auto& motionTimer = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].animTimer :
		crimsonPlayer[playerIndex].animTimerClone;
    auto tiltDirection = GetRelativeTiltDirection(actorData);
	auto& vergilSword = *reinterpret_cast<Sword*>(actorData.nextBaseAddr);
	cDrawReverse* vergilSwordcDraw = reinterpret_cast<cDrawReverse*>(vergilSword.swordcDraw); // first cDraw is the katana part
	Matrix44* swordMatrix = reinterpret_cast<Matrix44*>(vergilSwordcDraw[0].bones); // index 1 is the hilt
	cDrawReverse playerVergilcDraw = actorData.newModelData[actorData.activeModelIndexMirror]; // activeModelIndex == which DT or Non-DT model
	Matrix44* playerBoneMatrix = reinterpret_cast<Matrix44*>(playerVergilcDraw.bones);
	static EffekseerHandle weaponParticleHandle[PLAYER_COUNT][ENTITY_COUNT] = { 0 };
	auto& yamatoGroundedHighTimeHandle = (actorData.newEntityIndex == ENTITY::MAIN) ? crimsonPlayer[playerIndex].yamatoGroundedHighTimeHandle :
		crimsonPlayer[playerIndex].yamatoGroundedHighTimeHandleClone;
	auto& inRisingStar = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].inRisingStar :
		crimsonPlayer[playerIndex].inRisingStarClone;
	auto& inYamatoHighTime = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].inYamatoHighTime :
		crimsonPlayer[playerIndex].inYamatoHighTimeClone;
	static bool playedGroundedHighTimeGuetto[PLAYER_COUNT][ENTITY_COUNT] = { false };

	// --- Melee button hold timer ---
	static float meleeButtonHold[PLAYER_COUNT][ENTITY_COUNT] = {};
	constexpr float MELEE_HOLD_TIME = 0.2f; // 200 ms

	bool meleeDown = (gamepad.buttons[0] & GetBinding(BINDING::MELEE_ATTACK)) != 0;

	if (meleeDown) {
		meleeButtonHold[playerIndex][entityIndex] += ImGui::GetIO().DeltaTime;
	}
	else {
		meleeButtonHold[playerIndex][entityIndex] = 0.0f;
	}

	static bool meleeReleasedDuringUpperSlash[PLAYER_COUNT][ENTITY_COUNT] = { false };
	if (actorData.action == YAMATO_UPPER_SLASH_PART_1) {
		if (!meleeDown && actionTimer > 0.05f) { // Short grace period
			meleeReleasedDuringUpperSlash[playerIndex][entityIndex] = true;
		}
	}
	else {
		meleeReleasedDuringUpperSlash[playerIndex][entityIndex] = false;
	}

	if (actorData.action == YAMATO_UPPER_SLASH_PART_1
		&& actionTimer > 0.36f && actionTimer < 0.4f && (gamepad.buttons[0] & GetBinding(BINDING::MELEE_ATTACK) &&
			meleeButtonHold[playerIndex][entityIndex] >= 0.3f && !meleeReleasedDuringUpperSlash[playerIndex][entityIndex])) {

		actorData.motionArchives[MOTION_GROUP_VERGIL::YAMATO_FORCE_EDGE] = newYamatoHighTime_pl021_00_5; // Swap Force Edge High Time animation
		actorData.action = YAMATO_FORCE_EDGE_HIGH_TIME;
		PlayAnimation_1EFB90(actorData, 5, 7, 20.0f, 0, 0, -1);
		actorData.recoverState[0] = 1;
		inYamatoHighTime = true;
	}

	//Swapback to regular Force Edge High Time animation if action is different
	if (actorData.action != YAMATO_FORCE_EDGE_HIGH_TIME &&
		actorData.action != YAMATO_UPPER_SLASH_PART_1 &&
		actorData.action != YAMATO_UPPER_SLASH_PART_2
		&& actorData.motionArchives[MOTION_GROUP_VERGIL::YAMATO_FORCE_EDGE] == newYamatoHighTime_pl021_00_5) {
		actorData.motionArchives[MOTION_GROUP_VERGIL::YAMATO_FORCE_EDGE] = File_staticFiles[pl021_00_5];
	}

	if ((actorData.action != YAMATO_FORCE_EDGE_HIGH_TIME_LAUNCH &&
		actorData.action != YAMATO_FORCE_EDGE_HIGH_TIME &&
		actorData.action != YAMATO_UPPER_SLASH_PART_1 && actorData.action != YAMATO_UPPER_SLASH_PART_2)
		|| actorData.eventData[0].event != ACTOR_EVENT::ATTACK) {
		
		inYamatoHighTime = false;
	}


	// Controlling Yamato's visibility and parenting during High Time.
	if (inYamatoHighTime) {
		if (actorData.action == YAMATO_FORCE_EDGE_HIGH_TIME) {
			if (motionTimer >= 0.25f && motionTimer < 0.45f) {
				actorData.weaponMotionState[7] = 5;
				vergilSword.hideYamatoTip = false;
			}
			else if (motionTimer >= 0.45f && motionTimer < 0.95f) {
				actorData.weaponMotionState[7] = 5;
				vergilSword.hideYamatoTip = false;
			}
			else if (motionTimer >= 0.95f && motionTimer < 1.3f) {
				actorData.weaponMotionState[7] = 5;
// 				vergilSwordcDraw[0].modelPartitionData[1].value = 2;
 				
			}
			else if (motionTimer >= 1.3f) {
				if (motionTimer >= 1.7f) {
					vergilSword.hideYamatoTip = true;
					actorData.weaponMotionState[7] = 1;
				}
				
			}
		}
		else if (actorData.action == YAMATO_FORCE_EDGE_HIGH_TIME_LAUNCH) {
			if (motionTimer < 0.4f) {
				actorData.weaponMotionState[7] = 5;
				vergilSword.hideYamatoTip = false;
			}
			else {
				actorData.weaponMotionState[7] = 1;
				vergilSword.hideYamatoTip = false;
			}
		}
	}
	
// 	static bool shouldHide[PLAYER_COUNT][ENTITY_COUNT];
// 	shouldHide[playerIndex][entityIndex] = inRisingStar || inYamatoHighTime;
// 	vergilSwordcDraw[0].visible = !shouldHide[playerIndex][entityIndex];

	// GROUMDED Yamato High Time OUTSOURCED to SetAction(); in Actor.cpp
}

float ComputeDynamicJDCHoldTime(const PlayerActorData& actorData, bool inAir, bool inRisingStar, bool startedInAir) {
	using namespace ACTION_VERGIL;
	constexpr float MELEE_HOLD_TIME_GROUNDED = 0.7f;
	constexpr float MELEE_HOLD_TIME_AIR = 0.6f;
	auto& jCut = (actorData.newEntityIndex == 0) ? crimsonPlayer[actorData.newPlayerIndex].jCut : 
		crimsonPlayer[actorData.newPlayerIndex].jCutClone;

	if (actorData.action == YAMATO_COMBO_PART_3) {
		return 1.88f;
	}
	else if (actorData.action == YAMATO_RAPID_SLASH_LEVEL_1 || actorData.action == YAMATO_RAPID_SLASH_LEVEL_2) {
		return 1.0f; 
	}
	else if (actorData.action == YAMATO_UPPER_SLASH_PART_2) {
		return 2.07f; 
	}
	else if (actorData.action == YAMATO_FORCE_EDGE_HIGH_TIME &&
		actorData.motionArchives[MOTION_GROUP_VERGIL::YAMATO_FORCE_EDGE] == newYamatoHighTime_pl021_00_5) {
		return 1.53f;
	}
	else if ((actorData.action == ACTION_VERGIL::YAMATO_JUDGEMENT_CUT_LEVEL_1 ||
		actorData.action == ACTION_VERGIL::YAMATO_JUDGEMENT_CUT_LEVEL_2) &&
		jCut.state == JDC_STATE::NORMAL_GROUNDED) {
		return 1.4f;
	}
	else if ((actorData.action == ACTION_VERGIL::YAMATO_JUDGEMENT_CUT_LEVEL_1 ||
		actorData.action == ACTION_VERGIL::YAMATO_JUDGEMENT_CUT_LEVEL_2) &&
		jCut.state == JDC_STATE::JUST_FRAME_GROUNDED) {
		return 0.5f;
	}
	else if ((actorData.action == ACTION_VERGIL::YAMATO_JUDGEMENT_CUT_LEVEL_1 ||
		actorData.action == ACTION_VERGIL::YAMATO_JUDGEMENT_CUT_LEVEL_2) &&
		jCut.state == JDC_STATE::NORMAL_AIR) {
		return 0.75f;
	}
	else if ((actorData.action == ACTION_VERGIL::YAMATO_JUDGEMENT_CUT_LEVEL_1 ||
		actorData.action == ACTION_VERGIL::YAMATO_JUDGEMENT_CUT_LEVEL_2) &&
		jCut.state == JDC_STATE::JUST_FRAME_AIR) {
		return 0.42f;
	}
	else if (inRisingStar) {
		return 1.5f; // Rising Star has a unique hold time
	}

	return startedInAir ? MELEE_HOLD_TIME_AIR : MELEE_HOLD_TIME_GROUNDED;
}

void ApplyJDCFlyingArc(byte8* actorBaseAddr) {
	using namespace ACTION_VERGIL;
	if (!actorBaseAddr || (actorBaseAddr == g_playerActorBaseAddrs[0]) || (actorBaseAddr == g_playerActorBaseAddrs[1])) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
	auto playerIndex = actorData.newPlayerIndex;
	auto entityIndex = actorData.newEntityIndex;

	// FLYING ARC AIR JDC
	static bool  airJdcArcActive[PLAYER_COUNT][ENTITY_COUNT] = { false };
	static uint8 airJdcArcLastMotion[PLAYER_COUNT][ENTITY_COUNT] = { 0 };
	static float airJdcArcPhaseTimer[PLAYER_COUNT][ENTITY_COUNT] = { 0.0f };      // per-index timer (for bell/descent)
	static float airJdcArcTotalRiseTimer[PLAYER_COUNT][ENTITY_COUNT] = { 0.0f };  // merged timer for 13 and 14
	static float airJdcArcBaseY[PLAYER_COUNT][ENTITY_COUNT] = { 0.0f };
	static float airJdcArcBaseX[PLAYER_COUNT][ENTITY_COUNT] = { 0.0f };
	static float airJdcArcBaseZ[PLAYER_COUNT][ENTITY_COUNT] = { 0.0f };
	static float airJdcArcDistance[PLAYER_COUNT][ENTITY_COUNT] = { 0.0f };
	static uint16 airJdcArcRotation[PLAYER_COUNT][ENTITY_COUNT] = { 0 };
	static float airJdcArcMotion15StartY[PLAYER_COUNT][ENTITY_COUNT] = { 0.0f };
	static float airJdcArcTotalTimer[PLAYER_COUNT][ENTITY_COUNT] = { 0.0f };
	static float airJdcArcLastPull[PLAYER_COUNT][ENTITY_COUNT] = { 0.0f };
	static int   airJdcConsecutiveCount[PLAYER_COUNT][ENTITY_COUNT] = { 0 };

	bool inNormalAirJdc =
		(actorData.action == YAMATO_JUDGEMENT_CUT_LEVEL_1 || actorData.action == YAMATO_JUDGEMENT_CUT_LEVEL_2) &&
		(actorData.motionArchives[MOTION_GROUP_VERGIL::YAMATO] == newJudgementCutAir_pl021_00_3);

	bool inJFAirJdc =
		(actorData.action == YAMATO_JUDGEMENT_CUT_LEVEL_1 || actorData.action == YAMATO_JUDGEMENT_CUT_LEVEL_2) &&
		(actorData.motionArchives[MOTION_GROUP_VERGIL::YAMATO] == newJudgementCutAirJF_pl021_00_3);

	bool inAnyAirJdc = inNormalAirJdc || inJFAirJdc;

	auto EaseInOut = [](float t) -> float {
		t = glm::clamp(t, 0.0f, 1.0f);
		return t * t * (3.0f - 2.0f * t); // smoothstep
		};

	float dtScaled = ImGui::GetIO().DeltaTime * (actorData.speed / g_FrameRateTimeMultiplier);

	if (actorData.action == DARK_SLAYER_TRICK_DOWN && 
		actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_TRICK_DOWN) {
		actorData.horizontalPull = -25.0f;
	}

	if (inAnyAirJdc) {
		// Detect consecutive JDC by checking if we cycled back to motion 13
		if (airJdcArcLastMotion[playerIndex][entityIndex] != actorData.motionData[0].index) {
			if (actorData.motionData[0].index == 13 && airJdcArcLastMotion[playerIndex][entityIndex] != 0) {
				airJdcArcActive[playerIndex][entityIndex] = false; // Reset to allow chain
				if (inJFAirJdc) {
					airJdcConsecutiveCount[playerIndex][entityIndex]++;
				}
			}

			airJdcArcPhaseTimer[playerIndex][entityIndex] = 0.0f;
			airJdcArcLastMotion[playerIndex][entityIndex] = static_cast<uint8>(actorData.motionData[0].index);

			if (actorData.motionData[0].index == 15) {
				airJdcArcMotion15StartY[playerIndex][entityIndex] = actorData.position.y;
			}
		}

		if (!airJdcArcActive[playerIndex][entityIndex]) {
			airJdcArcActive[playerIndex][entityIndex] = true;
			airJdcArcTotalRiseTimer[playerIndex][entityIndex] = 0.0f;
			airJdcArcTotalTimer[playerIndex][entityIndex] = 0.0f;
			airJdcArcBaseY[playerIndex][entityIndex] = actorData.position.y;
			airJdcArcBaseX[playerIndex][entityIndex] = actorData.position.x;
			airJdcArcBaseZ[playerIndex][entityIndex] = actorData.position.z;

			float currentPull = actorData.horizontalPull;
			airJdcArcLastPull[playerIndex][entityIndex] = currentPull;
// 			if (glm::abs(currentPull) > 0.1f) {
// 				airJdcArcLastPull[playerIndex][entityIndex] = currentPull;
// 			}
// 			else if (inJFAirJdc && airJdcArcLastPull[playerIndex][entityIndex] != 0.0f) {
// 				currentPull = airJdcArcLastPull[playerIndex][entityIndex];
// 			}
// 			else if (inJFAirJdc) {
// 				currentPull = 5.0f; // Fallback if horizontalPull uninitialized on first frame
// 			}
// 			else {
// 				currentPull = airJdcArcLastPull[playerIndex][entityIndex]; // Also fallback for normal if it 0'ed out
// 			}

			float distMult = 95.0f;
			if (inJFAirJdc) {
				distMult = 65.0f; // Constant high momentum extended across entire move
			}

			// DMC3 represents locked-on backwards tricking internally via a negative horizontal pull,
			// rather than flipping inertiaRotation backwards. We MUST extract the sign to know if we are moving backwards, 
			// even if currentPull's magnitude doesn't control the distance anymore.
			float pullSign = (currentPull < 0.0f) ? -1.0f : ((currentPull > 0.0f) ? 1.0f : 0.0f);
			airJdcArcDistance[playerIndex][entityIndex] = distMult * pullSign;

			airJdcArcRotation[playerIndex][entityIndex] = actorData.inertiaRotation;
			//actorData.horizontalPull = 0.0f; // Let script control velocity
		}

		airJdcArcPhaseTimer[playerIndex][entityIndex] += dtScaled;
		airJdcArcTotalTimer[playerIndex][entityIndex] += dtScaled;
		if (actorData.motionData[0].index == 13 || actorData.motionData[0].index == 14) {
			airJdcArcTotalRiseTimer[playerIndex][entityIndex] += dtScaled;
		}

		float TOTAL_RISE_TIME = inJFAirJdc ? 0.15f : 0.56f;
		float TOTAL_FORWARD_TIME = inJFAirJdc ? 0.80f : 1.2f;
		float JUMP_APEX_HEIGHT = inJFAirJdc ? 20.0f : 40.0f;

		auto EaseOut = [](float t) -> float {
			t = glm::clamp(t, 0.0f, 1.0f);
			return 1.0f - ((1.0f - t) * (1.0f - t));
			};

		auto EaseIn = [](float t) -> float {
			t = glm::clamp(t, 0.0f, 1.0f);
			return t * t;
			};

		// Consistent X and Z movement across all indexes
		float angle = airJdcArcRotation[playerIndex][entityIndex] * (3.14159265f / 32768.0f);

		float currentDist;
		if (inJFAirJdc) {
			// Constant velocity formula for Just Frame: extends movement consistently without clamping to time
			currentDist = (airJdcArcDistance[playerIndex][entityIndex] / TOTAL_FORWARD_TIME) * airJdcArcTotalTimer[playerIndex][entityIndex];
		} else {
			float tForward = EaseOut(airJdcArcTotalTimer[playerIndex][entityIndex] / TOTAL_FORWARD_TIME);
			currentDist = airJdcArcDistance[playerIndex][entityIndex] * tForward;
		}

		actorData.position.x = airJdcArcBaseX[playerIndex][entityIndex] + (std::sin(angle) * currentDist);
		actorData.position.z = airJdcArcBaseZ[playerIndex][entityIndex] + (std::cos(angle) * currentDist);



		if (actorData.motionData[0].index == 13 || actorData.motionData[0].index == 14) {
			float tRise = EaseOut(airJdcArcTotalRiseTimer[playerIndex][entityIndex] / TOTAL_RISE_TIME);

			float continuousRise = JUMP_APEX_HEIGHT * tRise;
			actorData.position.y = airJdcArcBaseY[playerIndex][entityIndex] + continuousRise;

			actorData.verticalPullMultiplier = 0.0f;
		}
		else if (actorData.motionData[0].index == 15) {
			float fallTime = inJFAirJdc ? 0.15f : 0.22f;
			float t = EaseIn(airJdcArcPhaseTimer[playerIndex][entityIndex] / fallTime);
			float targetY = airJdcArcBaseY[playerIndex][entityIndex] - 22.0f;
			actorData.position.y = glm::mix(airJdcArcMotion15StartY[playerIndex][entityIndex], targetY, t);
			actorData.horizontalPull = airJdcArcDistance[playerIndex][entityIndex] >= 0 ? 5.0f : -5.0f; // Retain some horizontal momentum during descent for better control
			actorData.verticalPullMultiplier = -0.7f;
		}
	}
	else {
		airJdcArcActive[playerIndex][entityIndex] = false;
		airJdcArcLastMotion[playerIndex][entityIndex] = 0;
		airJdcArcPhaseTimer[playerIndex][entityIndex] = 0.0f;
		airJdcArcTotalRiseTimer[playerIndex][entityIndex] = 0.0f;
		airJdcArcTotalTimer[playerIndex][entityIndex] = 0.0f;
		airJdcArcLastPull[playerIndex][entityIndex] = 0.0f;
		airJdcConsecutiveCount[playerIndex][entityIndex] = 0;
	}
}

void VergilJudgementCutRework(byte8* actorBaseAddr) {
	using namespace ACTION_VERGIL;
	using namespace NEXT_ACTION_REQUEST_POLICY;
	if (!actorBaseAddr || (actorBaseAddr == g_playerActorBaseAddrs[0]) || (actorBaseAddr == g_playerActorBaseAddrs[1])) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
	if (!activeCrimsonGameplay.Gameplay.Vergil.judgementCutRework || actorData.character != CHARACTER::VERGIL) {
		return;
	}
	if (actorData.character != CHARACTER::VERGIL) return;
	auto playerIndex = actorData.newPlayerIndex;
	auto entityIndex = actorData.newEntityIndex;
	auto& playerData = GetPlayerData(playerIndex);
	auto& characterData = playerData.characterData[playerData.activeCharacterIndex];

	if (characterData[ENTITY::MAIN].character != CHARACTER::VERGIL) {
		return; // Ensure the character is currently using Vergil
	}
	auto& gamepad = GetGamepad(playerIndex);
	auto& actionTimerNotTrickChange = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].actionTimerNotTrickChange :
		crimsonPlayer[playerIndex].actionTimerNotTrickChangeClone;
	auto& actionTimerNotEventChange = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].actionTimerNotEventChange :
		crimsonPlayer[playerIndex].actionTimerNotEventChangeClone;
	bool inAir = (actorData.state & STATE::IN_AIR);

	// --- Melee button hold timer for charging Just Frame Judgement Cut ---
	static bool startedInAir[PLAYER_COUNT][ENTITY_COUNT] = { false };
	constexpr float MELEE_HOLD_TIME_GROUNDED = 0.8f;
	constexpr float MELEE_HOLD_TIME_AIR = 0.6f;
	constexpr float JUST_FRAME_WINDOW_GROUND = 0.09f; // ~3 frames at 60 FPS
	constexpr float JUST_FRAME_WINDOW_AIR = 0.08f; // ~4 frames at 60 FPS

	auto& jCut = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].jCut : crimsonPlayer[playerIndex].jCutClone;

	bool meleeDown = (gamepad.buttons[0] & GetBinding(BINDING::MELEE_ATTACK)) != 0;

	static float lockedHoldTime[PLAYER_COUNT][ENTITY_COUNT] = { 0 };
	static bool chargeInitialized[PLAYER_COUNT][ENTITY_COUNT] = { false };
	static bool indicatorFired[PLAYER_COUNT][ENTITY_COUNT] = { false };
	static bool rotatedWhileFiring[PLAYER_COUNT][ENTITY_COUNT] = { false };

	static constexpr const wchar_t* jdcChargeParticlePath = L"Crimson\\vfx\\jdc_charge.efkefc";
	static EffekseerRefHandle chargeParticleRef = CrimsonEfk::LoadEffect(L"Crimson\\vfx\\jdc_charge.efkefc", 1.0f);
	static EffekseerRefHandle chargeDTParticleRef = CrimsonEfk::LoadEffect(L"Crimson\\vfx\\jdc_charge_dt.efkefc", 1.0f);

	static EffekseerHandle chargeParticle[PLAYER_COUNT][ENTITY_COUNT] = { 0 };

	// JUDGEMENT CUT SFX
	if ((actorData.action == YAMATO_JUDGEMENT_CUT_LEVEL_2 ||
		actorData.action == YAMATO_JUDGEMENT_CUT_LEVEL_1)) {

		if (actorData.motionData[0].index == 14) {
			if (actorData.motionArchives[MOTION_GROUP_VERGIL::YAMATO] == newJudgementCutAirJF_pl021_00_3 ||
				actorData.motionArchives[MOTION_GROUP_VERGIL::YAMATO] == newJudgementCut_pl021_00_3) {
				if (jCut.fireSound) {
					CrimsonSDL::PlayJDC(playerIndex, true, 0);
					jCut.fireSound = false;
				}

			}
			else if (actorData.motionArchives[MOTION_GROUP_VERGIL::YAMATO] == newJudgementCutAir_pl021_00_3) {
				if (jCut.fireSound) {
					CrimsonSDL::PlayJDC(playerIndex, false, 0);
					jCut.fireSound = false;
				}
			}
		}
		else {
			jCut.fireSound = true; // Reset sound trigger for next hit in the move
		}
	}

	if (actorData.queuedMeleeWeaponIndex != 0) {
		chargeInitialized[playerIndex][entityIndex] = false;
		jCut.meleeButtonHold = 0.0f;
		jCut.isJustFrameCharged = false;
		jCut.isAfterJustFrameCharged = false;
		indicatorFired[playerIndex][entityIndex] = false;
		if (meleeDown) {
			if (jCut.meleeButtonHold == 0.0f) {
				startedInAir[playerIndex][entityIndex] = inAir;
				jCut.actionWhenChargeStarted = actorData.action;
			}
		}

		jCut.meleeHoldTime = startedInAir[playerIndex][entityIndex] ? MELEE_HOLD_TIME_AIR : MELEE_HOLD_TIME_GROUNDED;

		return; // Ensure the character is currently using Yamato
	}

	if (actorData.eventData[0].event == ACTOR_EVENT::STAGGER) {
		chargeInitialized[playerIndex][entityIndex] = false;
		jCut.meleeButtonHold = 0.0f;
		jCut.isJustFrameCharged = false;
		jCut.isAfterJustFrameCharged = false;
		indicatorFired[playerIndex][entityIndex] = false;
		return;
	}

	auto& inRisingStar = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].inRisingStar :
		crimsonPlayer[playerIndex].inRisingStarClone;

	if (meleeDown) {

		// DYNAMIC HOLD TIME
		// First-time initialization for computing the Dynamic Hold Time
		if (!chargeInitialized[playerIndex][entityIndex]) {
			startedInAir[playerIndex][entityIndex] = inAir;
			jCut.actionWhenChargeStarted = actorData.action;

			lockedHoldTime[playerIndex][entityIndex] = ComputeDynamicJDCHoldTime(
				actorData, inAir, inRisingStar, startedInAir[playerIndex][entityIndex]);

			chargeInitialized[playerIndex][entityIndex] = true;
		}
		// Re-lock when action changes mid-hold
		else if (actorData.action != jCut.actionWhenChargeStarted) {
			jCut.actionWhenChargeStarted = actorData.action;

			lockedHoldTime[playerIndex][entityIndex] = ComputeDynamicJDCHoldTime(
				actorData, inAir, inRisingStar, startedInAir[playerIndex][entityIndex]);

			if (actorData.action != DARK_SLAYER_AIR_TRICK && 
				actorData.action != DARK_SLAYER_TRICK_DOWN && 
				actorData.action != DARK_SLAYER_TRICK_UP) {
				jCut.meleeButtonHold = 0.0f; // Reset hold time when action changes to prevent buffering to interfere with Just Frame timing
			}
		}
		
		// Use locked value instead of recalculating every frame
		jCut.meleeHoldTime = lockedHoldTime[playerIndex][entityIndex];
		float JUST_FRAME_WINDOW = startedInAir[playerIndex][entityIndex] ? JUST_FRAME_WINDOW_AIR : JUST_FRAME_WINDOW_GROUND;
		jCut.meleeMaxHoldTime = jCut.meleeHoldTime + JUST_FRAME_WINDOW;

		jCut.meleeButtonHold += ImGui::GetIO().DeltaTime * (actorData.speed / g_FrameRateTimeMultiplier);

		bool inJFWindow = (jCut.meleeButtonHold >= jCut.meleeHoldTime &&
			jCut.meleeButtonHold <= jCut.meleeMaxHoldTime &&
			actionTimerNotTrickChange >= jCut.meleeHoldTime);

		// JUST FRAME WINDOW ENTER
		if (inJFWindow) {
			jCut.isJustFrameCharged = true;


			// INDICATOR
			if (!indicatorFired[playerIndex][entityIndex]) {
				indicatorFired[playerIndex][entityIndex] = true;
				//CrimsonDetours::CreateEffectDetour(actorData, 3, 143, 1, true, CrimsonUtil::HexToAABBGGRR(0x1fcbed), 1.2f); // Indicator

				auto& vergilSword = *reinterpret_cast<Sword*>(actorData.nextBaseAddr);
				cDrawReverse* vergilSwordcDraw = reinterpret_cast<cDrawReverse*>(vergilSword.swordcDraw); // first cDraw is the katana part
				Matrix44* swordMatrix = reinterpret_cast<Matrix44*>(vergilSwordcDraw[0].bones); // index 1 is the hilt
				chargeParticleRef = CrimsonEfk::ReloadEffect(chargeParticleRef, jdcChargeParticlePath, 1.0f);
				chargeParticle[playerIndex][entityIndex] = CrimsonEfk::PlayEffectAtMatrix(chargeParticleRef, swordMatrix[0].matrix2, actorData); // using katana bone 2
				
				
				CrimsonSDL::PlayJDCCharge(playerIndex); // Charge sound

				
			}
		}

		if (jCut.meleeButtonHold > jCut.meleeMaxHoldTime) {
			jCut.isJustFrameCharged = false; // Discard charge if held for too long
			jCut.isAfterJustFrameCharged = true;
		}
	}
	else {
		chargeInitialized[playerIndex][entityIndex] = false;

		// JUST FRAME RELEASE LOGIC
		if (jCut.isJustFrameCharged) { 

			if (!inAir) {
				actorData.motionArchives[MOTION_GROUP_VERGIL::YAMATO] = newJudgementCut_pl021_00_3; // Swap to Just Frame Judgement Cut animation
				jCut.state = JDC_STATE::JUST_FRAME_GROUNDED;
			}
			else {
				actorData.motionArchives[MOTION_GROUP_VERGIL::YAMATO] = newJudgementCutAirJF_pl021_00_3; // Swap to JF Air Just Frame Judgement Cut animation
				jCut.state = JDC_STATE::JUST_FRAME_AIR;
			}

			actorData.action = YAMATO_JUDGEMENT_CUT_LEVEL_2;
			
			if ((actorData.eventData[0].event == 3 || actorData.eventData[0].event == 6 || actorData.eventData[0].event == 2 
				|| actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_AIR_TRICK ||
				actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_TRICK_DOWN ||
				actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_TRICK_UP ||
				actorData.eventData[0].event == ACTOR_EVENT::JUMP_CANCEL)) {
				
				func_1E0800_TriggerEvent(actorData, ACTOR_EVENT::ATTACK, 0, 0);
				//CrimsonDetours::CreateEffectDetour(actorData, 3, 143, 1, true, CrimsonUtil::HexToAABBGGRR(0xfc0366ff), 1.2f); DEBUG COLOR JF
			}
			else {
				actorData.action = YAMATO_JUDGEMENT_CUT_LEVEL_2;
				actorData.recoverState[0] = 0;
			}
			
			actorData.rotation = GetRotationTowardsEnemy(actorData); // Snap to auto-rotation on JF release for better consistency in direction

			jCut.isJustFrameCharged = false;
			jCut.isAfterJustFrameCharged = false;
			
		}
		// REGULAR JDCS RELEASE LOGIC
		else if (jCut.isAfterJustFrameCharged && actorData.action != YAMATO_JUDGEMENT_CUT_LEVEL_2 &&
			actorData.action != YAMATO_JUDGEMENT_CUT_LEVEL_1 &&
			actorData.motionArchives[MOTION_GROUP_VERGIL::YAMATO] != newJudgementCutAir_pl021_00_3) { 
			
			actorData.action = YAMATO_JUDGEMENT_CUT_LEVEL_2;
			if ((actorData.eventData[0].event == 3 || actorData.eventData[0].event == 6 || actorData.eventData[0].event == 2
				|| actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_AIR_TRICK ||
					actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_TRICK_DOWN ||
				actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_TRICK_UP || 
				actorData.eventData[0].event == ACTOR_EVENT::JUMP_CANCEL)) {
				if (inAir) {
					actorData.motionArchives[MOTION_GROUP_VERGIL::YAMATO] = newJudgementCutAir_pl021_00_3; // Swap to Normal Air Just Frame Judgement Cut animation
					jCut.state = JDC_STATE::NORMAL_AIR;
				}

				func_1E0800_TriggerEvent(actorData, ACTOR_EVENT::ATTACK, 0, 0);
				//CrimsonDetours::CreateEffectDetour(actorData, 3, 143, 1, true, CrimsonUtil::HexToAABBGGRR(0xf71a0a), 1.2f); DEBUG COLOR NORMAL
			}
			else {
				if (inAir) {
					actorData.motionArchives[MOTION_GROUP_VERGIL::YAMATO] = newJudgementCutAir_pl021_00_3; // Swap to Normal Air Just Frame Judgement Cut animation
					jCut.state = JDC_STATE::NORMAL_AIR;
				}
				actorData.recoverState[0] = 0;
			}

			actorData.rotation = GetRotationTowardsEnemy(actorData); // Snap to auto-rotation on J release for better consistency in direction
		}

		jCut.meleeButtonHold = 0.0f;
		jCut.isJustFrameCharged = false; // Reset charge when released
		jCut.isAfterJustFrameCharged = false;
		indicatorFired[playerIndex][entityIndex] = false;
	}

	if (actorData.action != YAMATO_JUDGEMENT_CUT_LEVEL_2 &&
		actorData.action != YAMATO_JUDGEMENT_CUT_LEVEL_1 &&
		(actorData.motionArchives[MOTION_GROUP_VERGIL::YAMATO] == newJudgementCut_pl021_00_3 ||
		 actorData.motionArchives[MOTION_GROUP_VERGIL::YAMATO] == newJudgementCutAir_pl021_00_3 ||
		 actorData.motionArchives[MOTION_GROUP_VERGIL::YAMATO] == newJudgementCutAirJF_pl021_00_3)) {
		actorData.motionArchives[MOTION_GROUP_VERGIL::YAMATO] = File_staticFiles[pl021_00_3]; // Ensure we swap back to regular motion archive after the move is performed
		jCut.state = JDC_STATE::NORMAL_GROUNDED;
		rotatedWhileFiring[playerIndex][entityIndex] = false;
	}

	if (actorData.action == YAMATO_JUDGEMENT_CUT_LEVEL_2 || actorData.action == YAMATO_JUDGEMENT_CUT_LEVEL_1) {
		if (!rotatedWhileFiring[playerIndex][entityIndex] && actorData.motionData[0].index == 14) {
			actorData.rotation = GetRotationTowardsEnemy(actorData); // Continuously snap to auto-rotation while performing Judgement Cut for better directional consistency
			rotatedWhileFiring[playerIndex][entityIndex] = true;
		}
	}


	// HAND BONE CHARGE PLACEMENT
// 	using namespace CREATE_EFFECT_BONE_LOOKUP;
// 	uint32 boneSlot = GetCreateEffectBoneSlot(actorData, 13);
// 	auto boneMetadata = GetBoneMetadataFromNewPool(actorData, boneSlot);
// 	auto bonePhysicsData = GetBonePhysicsData(boneMetadata);
//	EffeseekerMoveEffect(chargeParticle[playerIndex][entityIndex], bonePhysicsData->bonePosition);

	// SWORD BONE CHARGE PLACEMENT
// 	if (!actorData.nextBaseAddr) return;
// 	auto& vergilSword = *reinterpret_cast<Sword*>(actorData.nextBaseAddr);
// 	cDrawReverse* vergilSwordcDraw = reinterpret_cast<cDrawReverse*>(vergilSword.swordcDraw);
// 	Matrix44* swordMatrix = reinterpret_cast<Matrix44*>(vergilSwordcDraw[1].bones);
//     auto& chargeHandle = chargeParticle[playerIndex][entityIndex];
	
	ApplyJDCFlyingArc(actorData);
 }

void VergilAirTauntRisingSunDetection(byte8* actorBaseAddr) {
    using namespace ACTION_VERGIL;

    if (!actorBaseAddr || (actorBaseAddr == g_playerActorBaseAddrs[0]) || (actorBaseAddr == g_playerActorBaseAddrs[1])) {
        return;
    }

    auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    auto playerIndex = actorData.newPlayerIndex;
    auto entityIndex = actorData.newEntityIndex;
    auto& playerData = GetPlayerData(playerIndex);
    auto& gamepad = GetGamepad(playerIndex);
    auto& actionTimer = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].actionTimer :
        crimsonPlayer[playerIndex].actionTimerClone;
    auto& inAirTauntRisingSun = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].inAirTauntRisingSun :
        crimsonPlayer[playerIndex].inAirTauntRisingSunClone;

    if (actorData.action == BEOWULF_RISING_SUN
        && actionTimer > 0.01f && actionTimer < 0.1f && (gamepad.buttons[0] & GetBinding(BINDING::TAUNT))) {

        inAirTauntRisingSun = true;
    }


    if ((actorData.action == BEOWULF_RISING_SUN && actionTimer > 1.0f) ||
        (actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_AIR_TRICK ||
            actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_TRICK_DOWN ||
            actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_TRICK_UP
            ) && inAirTauntRisingSun) {
       
        inAirTauntRisingSun = false;
    }

    if (actorData.action != BEOWULF_RISING_SUN && (gamepad.buttons[0] & GetBinding(BINDING::MELEE_ATTACK))) {
        inAirTauntRisingSun = false;
    }
}

void VergilAirRisingSun(byte8* actorBaseAddr) {
	using namespace ACTION_VERGIL;

	if (!actorBaseAddr || (actorBaseAddr == g_playerActorBaseAddrs[0]) || (actorBaseAddr == g_playerActorBaseAddrs[1])) {
		return;
	}

	if (!ExpConfig::missionExpDataVergil.unlocks[UNLOCK_VERGIL::BEOWULF_RISING_SUN_AIR] || !activeCrimsonGameplay.Gameplay.General.extramoves) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    if (actorData.character != CHARACTER::VERGIL) return;
	auto playerIndex = actorData.newPlayerIndex;
	auto entityIndex = actorData.newEntityIndex;
	auto& playerData = GetPlayerData(playerIndex);
	auto& gamepad = GetGamepad(playerIndex);
	auto& actionTimer = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].actionTimer :
		crimsonPlayer[playerIndex].actionTimerClone;
	auto& inAirTauntRisingSun = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].inAirTauntRisingSun :
		crimsonPlayer[playerIndex].inAirTauntRisingSunClone;
	auto tiltDirection = GetRelativeTiltDirection(actorData);
	auto lockOn = (actorData.buttons[0] & GetBinding(BINDING::LOCK_ON));
	auto& inRisingStar = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].inRisingStar :
		crimsonPlayer[playerIndex].inRisingStarClone;


	if (ExpConfig::missionExpDataVergil.unlocks[UNLOCK_VERGIL::BEOWULF_RISING_SUN_AIR] && actionTimer > 0.05f && actionTimer < 0.2f && (actorData.action == BEOWULF_STARFALL_LEVEL_2 ||
		actorData.action == BEOWULF_STARFALL_LEVEL_1)
		&& lockOn && (tiltDirection == TILT_DIRECTION::DOWN) && actorData.newAirRisingSunCount < 1) {

		if (ExpConfig::missionExpDataVergil.unlocks[UNLOCK_VERGIL::BEOWULF_RISING_SUN]) {
			actorData.action = BEOWULF_RISING_SUN;
			actorData.recoverState[0] = 0;
			actorData.newAirRisingSunCount++;
			
		}
	}
}



bool startingFromGround[PLAYER_COUNT] = { true };
bool risingSunActive[PLAYER_COUNT] = { false };

void VergilAdjustAirMovesPos(byte8* actorBaseAddr) {
    // This is for adjusting DDMK's Vergil's Air Moves positions, so that their startup position is more correct. - Mia

    using namespace ACTION_VERGIL;

    if (!actorBaseAddr) {
        return;
    }
    auto& actorData  = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    if (actorData.character != CHARACTER::VERGIL) return;
    auto playerIndex = actorData.newPlayerIndex;
    auto& gamepad    = GetGamepad(actorData.newPlayerIndex);

    auto* v     = (actorData.newEntityIndex == 0) ? &crimsonPlayer[playerIndex].vergilMoves : &crimsonPlayer[playerIndex].vergilMovesClone;
	auto action = actorData.action;
	auto event = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].event : crimsonPlayer[playerIndex].eventClone;
    auto motion = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].motion : crimsonPlayer[playerIndex].motionClone;
    auto state  = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].state : crimsonPlayer[playerIndex].stateClone;
    auto actionTimer =
        (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].actionTimer : crimsonPlayer[playerIndex].actionTimerClone;
    auto animTimer = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].animTimer : crimsonPlayer[playerIndex].animTimerClone;
    auto& inAirTauntRisingSun = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].inAirTauntRisingSun :
        crimsonPlayer[playerIndex].inAirTauntRisingSunClone;
    auto lastAction = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].lastAction : crimsonPlayer[playerIndex].lastActionClone;
    auto& lastActionTime = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].lastActionTime : crimsonPlayer[playerIndex].lastActionTimeClone;
	auto& tweak = (actorData.newEntityIndex == ENTITY::MAIN) ? crimsonPlayer[playerIndex].lunarPhaseTweak :
		crimsonPlayer[playerIndex].lunarPhaseTweakClone;
    
    if (actorData.character == CHARACTER::VERGIL) {

        // Storing the pos
        if (!(action == BEOWULF_RISING_SUN && event == ACTOR_EVENT::ATTACK)) {
            v->storedRisingSunPosY = actorData.position.y;
        }

        if (!(action == BEOWULF_LUNAR_PHASE_LEVEL_2 && event == ACTOR_EVENT::ATTACK)) {

            v->storedLunarPhasePosY = actorData.position.y;
        }

		if (!(action == BEOWULF_LUNAR_PHASE_LEVEL_1 && event == ACTOR_EVENT::ATTACK)) {

			v->storedLunarPhaseLv1PosY = actorData.position.y;
		}


        // Take configs into account if new positionings will be applied or not
		if (action != BEOWULF_RISING_SUN) {
			if (!(state & STATE::IN_AIR)) {
				v->startingRisingSunFromGround = true;
			} else {
				v->startingRisingSunFromGround = false;
			}
		}

        if (activeCrimsonGameplay.Gameplay.Vergil.adjustLunarPhasePos == "From Air") {
            if (action != BEOWULF_LUNAR_PHASE_LEVEL_1 && action != BEOWULF_LUNAR_PHASE_LEVEL_2) {
                if (!(state & STATE::IN_AIR)) {
                    v->startingLunarPhaseFromGround = true;
                } else {
                    v->startingLunarPhaseFromGround = false;
                }
            }
		} else if (activeCrimsonGameplay.Gameplay.Vergil.adjustLunarPhasePos == "Always") {
			v->startingLunarPhaseFromGround = false;
        } else if (activeCrimsonGameplay.Gameplay.Vergil.adjustLunarPhasePos == "Off") {
            v->startingLunarPhaseFromGround = true;
        }

        // Applying the pos
        if (event == ACTOR_EVENT::ATTACK && state & STATE::IN_AIR) {

            // Adjusts Vergil Pos to be lower when starting Ground/Air Rising Sun
            if (action == BEOWULF_RISING_SUN && inAirTauntRisingSun) {

                if (actionTimer <= 0.6f) {
                    actorData.verticalPullMultiplier = 0.0f;
                }

                actorData.position.y = v->storedRisingSunPosY - 50.0f;
            }

            // Adjusts Vergil Pos to be lower when starting Ground/Air Lunar Phase
            if ((actorData.action == BEOWULF_LUNAR_PHASE_LEVEL_2) && 
                motion != 23 &&
                !v->startingLunarPhaseFromGround) {

                actorData.verticalPullMultiplier = 0.0f;
                actorData.position.y             = v->storedLunarPhasePosY - 20.0f;
            }

			if ((actorData.action == BEOWULF_LUNAR_PHASE_LEVEL_1) &&
				motion != 10 &&
				!v->startingLunarPhaseFromGround) {

				actorData.verticalPullMultiplier = 0.0f;
				actorData.position.y = v->storedLunarPhaseLv1PosY - 20.0f;
			}
        }

		// Fix for the weird carry over to air hike/jump cancel from Lunar Phase
		if ((event == ACTOR_EVENT::JUMP_CANCEL) && 
            (action == ACTION_VERGIL::BEOWULF_LUNAR_PHASE_LEVEL_1 || action == ACTION_VERGIL::BEOWULF_LUNAR_PHASE_LEVEL_2)) {
			actorData.verticalPullMultiplier = -1.5f;
			return;
		}

		// Fix for the weird carry over to air hike/jump cancel from Rising Sun
		if ((event == ACTOR_EVENT::JUMP_CANCEL) &&
			(action == ACTION_VERGIL::BEOWULF_RISING_SUN)) {
			actorData.verticalPullMultiplier = -1.5f;
			return;
		}
    }
}

void VergilDownertia(byte8* actorBaseAddr) {
	using namespace ACTION_VERGIL;

	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
	if (actorData.character != CHARACTER::VERGIL) return;
	auto playerIndex = actorData.newPlayerIndex;
	auto& gamepad = GetGamepad(actorData.newPlayerIndex);

	auto* v = (actorData.newEntityIndex == 0) ? &crimsonPlayer[playerIndex].vergilMoves : &crimsonPlayer[playerIndex].vergilMovesClone;
	auto action = actorData.action;
	auto event = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].event : crimsonPlayer[playerIndex].eventClone;
	auto motion = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].motion : crimsonPlayer[playerIndex].motionClone;
	auto state = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].state : crimsonPlayer[playerIndex].stateClone;
	auto actionTimer =
		(actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].actionTimer : crimsonPlayer[playerIndex].actionTimerClone;
	auto animTimer = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].animTimer : crimsonPlayer[playerIndex].animTimerClone;
	auto& inAirTauntRisingSun = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].inAirTauntRisingSun :
		crimsonPlayer[playerIndex].inAirTauntRisingSunClone;
	auto lastAction = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].lastAction : crimsonPlayer[playerIndex].lastActionClone;
	auto& lastActionTime = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].lastActionTime : crimsonPlayer[playerIndex].lastActionTimeClone;
	auto& lunarPhaseTweak = (actorData.newEntityIndex == ENTITY::MAIN) ? crimsonPlayer[playerIndex].lunarPhaseTweak :
		crimsonPlayer[playerIndex].lunarPhaseTweakClone;
	auto& airTauntRisingSunTweak = (actorData.newEntityIndex == ENTITY::MAIN) ? crimsonPlayer[playerIndex].airTauntRisingSunTweak :
		crimsonPlayer[playerIndex].airTauntRisingSunTweakClone;
	auto& lastInAirTauntRisingSun = (actorData.newEntityIndex == ENTITY::MAIN) ? crimsonPlayer[playerIndex].lastInAirTauntRisingSun :
		crimsonPlayer[playerIndex].lastInAirTauntRisingSunClone;

	if (actorData.character != CHARACTER::VERGIL || !activeCrimsonGameplay.Gameplay.Vergil.downertia) {
		return;
	}

	// LUNAR PHASE DOWNERTIA
	if ((action != BEOWULF_LUNAR_PHASE_LEVEL_1 || action != BEOWULF_LUNAR_PHASE_LEVEL_2) &&
		(lastAction == BEOWULF_LUNAR_PHASE_LEVEL_1 || lastAction == BEOWULF_LUNAR_PHASE_LEVEL_2) &&
		lastActionTime < 0.35f && !lunarPhaseTweak.hasAppliedVerticalPullMultiplier) {
		actorData.verticalPullMultiplier = -2.0f; 
		lunarPhaseTweak.hasAppliedVerticalPullMultiplier = true;
	}

	if ((action != BEOWULF_LUNAR_PHASE_LEVEL_1 || action != BEOWULF_LUNAR_PHASE_LEVEL_2) || event != ACTOR_EVENT::ATTACK ||
		((action != BEOWULF_LUNAR_PHASE_LEVEL_1 || action != BEOWULF_LUNAR_PHASE_LEVEL_2) &&
			(lastAction != BEOWULF_LUNAR_PHASE_LEVEL_1 || lastAction != BEOWULF_LUNAR_PHASE_LEVEL_2))) {
		// Reset flag if not in the action or the attack event
		lunarPhaseTweak.hasAppliedVerticalPullMultiplier = false;
	}

	// AIR TAUNT RISING SUN
	if ((action != BEOWULF_RISING_SUN) &&
		(lastAction == BEOWULF_RISING_SUN) &&
		lastActionTime < 0.4f && !airTauntRisingSunTweak.hasAppliedVerticalPullMultiplier) {
		actorData.verticalPullMultiplier = -2.0f;
		airTauntRisingSunTweak.hasAppliedVerticalPullMultiplier = true;
	}

	if ((action != BEOWULF_RISING_SUN) || event != ACTOR_EVENT::ATTACK ||
		((action != BEOWULF_RISING_SUN) &&
			(lastAction != BEOWULF_RISING_SUN))) {
		// Reset flag if not in the action or the attack event
		airTauntRisingSunTweak.hasAppliedVerticalPullMultiplier = false;
	}
}

void FasterDarkslayerTricks() {
    auto pool_12311 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
    if (!pool_12311 || !pool_12311[3]) {
        return;
    }
    auto& actorData = *reinterpret_cast<PlayerActorData*>(pool_12311[3]);


    if (actorData.character == CHARACTER::VERGIL) {
        float storedspeedVergil = activeCrimsonGameplay.Cheats.Speed.human;

        if ((actorData.motionData[0].index == 51 || actorData.motionData[0].index == 2) &&
            !(actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_AIR_TRICK ||
                actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_TRICK_UP ||
                actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_TRICK_DOWN)) {
            // Storing the original speeds
            fasterDarkslayer.storedSpeedHuman    = activeCrimsonGameplay.Cheats.Speed.human;
            fasterDarkslayer.storedSpeedDevil[0] = activeCrimsonGameplay.Cheats.Speed.dTVergil[0];
            fasterDarkslayer.storedSpeedDevil[1] = activeCrimsonGameplay.Cheats.Speed.dTVergil[1];
            fasterDarkslayer.storedSpeedDevil[2] = activeCrimsonGameplay.Cheats.Speed.dTVergil[2];
            fasterDarkslayer.storedSpeedDevil[3] = activeCrimsonGameplay.Cheats.Speed.dTVergil[3];
        }

        if ((actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_AIR_TRICK ||
                actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_TRICK_UP ||
                actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_TRICK_DOWN) &&
            !fasterDarkslayer.newSpeedSet) {


            // Setting the new speed
            activeCrimsonGameplay.Cheats.Speed.human = fasterDarkslayer.newSpeed;
            activeCrimsonGameplay.Cheats.Speed.dTVergil[0] = fasterDarkslayer.newSpeed;
            activeCrimsonGameplay.Cheats.Speed.dTVergil[1] = fasterDarkslayer.newSpeed;
            activeCrimsonGameplay.Cheats.Speed.dTVergil[2] = fasterDarkslayer.newSpeed;
            activeCrimsonGameplay.Cheats.Speed.dTVergil[3] = fasterDarkslayer.newSpeed;

            fasterDarkslayer.newSpeedSet = true;
        } else if (!(actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_AIR_TRICK ||
                       actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_TRICK_UP ||
                       actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_TRICK_DOWN) &&
                   fasterDarkslayer.newSpeedSet) {

            // Restoring the original speeds
            activeCrimsonGameplay.Cheats.Speed.human = fasterDarkslayer.storedSpeedHuman;
            activeCrimsonGameplay.Cheats.Speed.dTVergil[0] = fasterDarkslayer.storedSpeedDevil[0];
            activeCrimsonGameplay.Cheats.Speed.dTVergil[1] = fasterDarkslayer.storedSpeedDevil[1];
            activeCrimsonGameplay.Cheats.Speed.dTVergil[2] = fasterDarkslayer.storedSpeedDevil[2];
            activeCrimsonGameplay.Cheats.Speed.dTVergil[3] = fasterDarkslayer.storedSpeedDevil[3];


            fasterDarkslayer.newSpeedSet = false;
        }
    }
}

float DrainDTBar(float currentDT, float maxDT, float elapsedTime, bool dtOn, float drainTimeDTOn, float drainTimeDTOff) {
	float drainTime = dtOn ? drainTimeDTOn : drainTimeDTOff;


	// Calculate the proportion of the total drain time that has passed
	float timeRatio = elapsedTime / drainTime;

	// Clamp timeRatio between 0.0 and 1.0 to ensure it doesn't exceed the intended range
	timeRatio = glm::clamp(timeRatio, 0.0f, 1.0f);

	// Calculate the new DT based on the ratio
	float newDT = maxDT * (1.0f - timeRatio);

	return (newDT > 0.0f) ? newDT : 0.0f;
}

float DrainMirageGauge(float currentMP, float elapsedTime, float totalDrainTime) {

	// Calculate the proportion of the total drain time that has passed
	float timeRatio = elapsedTime / totalDrainTime;

	// Clamp timeRatio between 0.0 and 1.0 to ensure it doesn't exceed the intended range
	timeRatio = glm::clamp(timeRatio, 0.0f, 1.0f);

	// Calculate the new DT based on the ratio
	float newMP = maxMiragePointsAmount * (1.0f - timeRatio);

	return (newMP > 0.0f) ? newMP : 0.0f;
}

#pragma endregion

#pragma region InertiaThings


void LastEventStateQueue(byte8* actorBaseAddr) {
    if (!actorBaseAddr) {
        return;
    }
    auto& actorData  = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    auto playerIndex = actorData.newPlayerIndex;


    auto lastEvent = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].lastEvent : crimsonPlayer[playerIndex].lastEventClone;
    auto lastState = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].lastState : crimsonPlayer[playerIndex].lastStateClone;

    auto* lastEvents =
        (actorData.newEntityIndex == 0) ? &crimsonPlayer[playerIndex].lastEvents : &crimsonPlayer[playerIndex].lastEventsClone;
    auto* lastStates =
        (actorData.newEntityIndex == 0) ? &crimsonPlayer[playerIndex].lastStates : &crimsonPlayer[playerIndex].lastStatesClone;


    if (lastEvents->back() != lastEvent) {
        lastEvents->push_back(lastEvent);
    }
    if (lastEvents->size() > 2) {
        crimsonPlayer[playerIndex].lastLastEvent = lastEvents->at(lastEvents->size() - 2);
    }

    if (lastEvents->size() > 4) {
        lastEvents->erase(lastEvents->begin());
    }

    if (lastStates->back() != lastState) {
        lastStates->push_back(lastState);
    }
    if (lastStates->size() > 2) {
        crimsonPlayer[playerIndex].lastLastState = lastStates->at(lastStates->size() - 2);
    }

    if (lastStates->size() > 4) {
        lastStates->erase(lastStates->begin());
    }
}

void FreeformSoftLockController(byte8* actorBaseAddr) {
	using namespace ACTION_DANTE;
	using namespace ACTION_VERGIL;
	using namespace ACTOR_EVENT;
	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
	auto playerIndex = actorData.newPlayerIndex;

	auto lockOn = (actorData.buttons[0] & GetBinding(BINDING::LOCK_ON));
	auto& gamepad = GetGamepad(playerIndex);
	auto tiltDirection = GetRelativeTiltDirection(actorData);
	auto radius = gamepad.leftStickRadius;
	uint16 relativeTilt = 0;
	relativeTilt = (actorData.cameraDirection + actorData.leftStickPosition);
	uint16 rotationStick = (relativeTilt - 0x8000);


	auto* i = &crimsonPlayer[playerIndex].inertia;

	bool inAerialRave = (actorData.action == REBELLION_AERIAL_RAVE_PART_1 || actorData.action == REBELLION_AERIAL_RAVE_PART_2 ||
		actorData.action == REBELLION_AERIAL_RAVE_PART_3 || actorData.action == REBELLION_AERIAL_RAVE_PART_4);

	bool inSkyDance = (actorData.action == AGNI_RUDRA_SKY_DANCE_PART_1 || actorData.action == AGNI_RUDRA_SKY_DANCE_PART_2 ||
		actorData.action == AGNI_RUDRA_SKY_DANCE_PART_3);

	bool inAir = (actorData.state & STATE::IN_AIR);
	bool lastInAir = (actorData.lastState & STATE::IN_AIR);
	static uint8 currentMovePlayer[PLAYER_COUNT] = { 0 };
	static uint8 currentMoveClone[PLAYER_COUNT] = { 0 };
	static uint8& currentMove = (actorData.newEntityIndex == 0) ? currentMovePlayer[playerIndex] : currentMoveClone[playerIndex];

	static uint16 cachedRotationPlayer[PLAYER_COUNT] = { 0 };
	static uint16 cachedRotationClone[PLAYER_COUNT] = { 0 };
	static uint16& cachedRotation = (actorData.newEntityIndex == 0) ? cachedRotationPlayer[playerIndex] : cachedRotationClone[playerIndex];

	auto& actionTimer =
		(actorData.newEntityIndex == 1) ? crimsonPlayer[playerIndex].actionTimerClone : crimsonPlayer[playerIndex].actionTimer;

	CrimsonDetours::ToggleFreeformSoftLockHelper(activeCrimsonGameplay.Gameplay.General.freeformSoftLock);
	float bufferTime = (activeCrimsonGameplay.Gameplay.General.bufferlessReversals) ? 0.3f : 0.02f;

	if (!activeCrimsonGameplay.Gameplay.General.freeformSoftLock) {
		return;
	}

	auto GetAutoRotation = [&]() -> uint16 {
		return (actorData.newEntityIndex == 0)
			? crimsonPlayer[playerIndex].rotationTowardsEnemy
			: crimsonPlayer[playerIndex].rotationCloneTowardsEnemy;
		};

	auto HandleRotationForMultiPartMove = [&](std::initializer_list<int> moveParts, uint16 stickRotation) {
		// First part of the move
		if (actorData.action == *moveParts.begin()) {
			actorData.rotation = (stickRotation != static_cast<uint16>(-1)) ? stickRotation : GetAutoRotation();
			cachedRotation = actorData.rotation;  // Cache rotation for subsequent parts
		}
		// For all subsequent parts of the multi-part move
		else if (std::find(moveParts.begin() + 1, moveParts.end(), actorData.action) != moveParts.end()) {
			actorData.rotation = cachedRotation;
		}
		// For all other moves
		else {
			actorData.rotation = (stickRotation != static_cast<uint16>(-1)) ? stickRotation : GetAutoRotation();;
		}
		};

	if (actorData.eventData[0].event == ACTOR_EVENT::ATTACK) {

		if (!lockOn && actionTimer < bufferTime) {
			uint16 stickRotation = (radius >= RIGHT_STICK_DEADZONE) ? rotationStick : static_cast<uint16>(-1);  // Use -1 as a flag for neutral stick

			if (currentMove != actorData.action) {
				// Character-specific handling for multi-part moves
				if (actorData.character == CHARACTER::DANTE) {

					if (actorData.action == EBONY_IVORY_AIR_NORMAL_SHOT || actorData.action == ROYALGUARD_RELEASE_1 ||
						actorData.action == ROYALGUARD_RELEASE_2 || actorData.action == ROYALGUARD_RELEASE_3 ||
						actorData.action == ROYALGUARD_RELEASE_4 || actorData.action == ROYALGUARD_AIR_RELEASE_1 ||
						actorData.action == ROYALGUARD_AIR_RELEASE_2 || actorData.action == ROYALGUARD_AIR_RELEASE_3 ||
						actorData.action == ROYALGUARD_AIR_RELEASE_4) return;

					HandleRotationForMultiPartMove({ ROYALGUARD_RELEASE_1, ROYALGUARD_RELEASE_2 }, stickRotation);
					HandleRotationForMultiPartMove({ ROYALGUARD_AIR_RELEASE_1, ROYALGUARD_AIR_RELEASE_2 }, stickRotation);
					HandleRotationForMultiPartMove({ REBELLION_HIGH_TIME, REBELLION_HIGH_TIME_LAUNCH }, stickRotation);
				} else if (actorData.character == CHARACTER::VERGIL) {
					HandleRotationForMultiPartMove({ YAMATO_FORCE_EDGE_HIGH_TIME, YAMATO_FORCE_EDGE_HIGH_TIME_LAUNCH }, stickRotation);
				} else {
					// Default handling for other characters
					actorData.rotation = (stickRotation != static_cast<uint16>(-1)) ? stickRotation : GetAutoRotation();
				}

				currentMove = actorData.action;  // Update current move
			}

			cachedRotation = actorData.rotation;  // Keep cached rotation updated
		} else if (lockOn && actionTimer < 0.1f) {

			// Exceptions
            // Royal Release as Exception to AutoRotation towards enemy
			if (actorData.character == CHARACTER::DANTE && (
				actorData.action == ROYALGUARD_RELEASE_2 || actorData.action == ROYALGUARD_RELEASE_3 ||
				actorData.action == ROYALGUARD_RELEASE_4 ||
				actorData.action == ROYALGUARD_AIR_RELEASE_2 || actorData.action == ROYALGUARD_AIR_RELEASE_3 ||
				actorData.action == ROYALGUARD_AIR_RELEASE_4)) return;

			actorData.rotation = GetAutoRotation();

			cachedRotation = actorData.rotation;  // Keep cached rotation updated
		}
	} else {
		if (actorData.eventData[0].lastEvent == ACTOR_EVENT::ATTACK) {
			currentMove = 0;  // Reset move tracking after the attack
		}
	}
}

void StoreInertia(byte8* actorBaseAddr) {
	// Here we store Momentum (Horizontal Pull) for Inertia to work. - Mia
	using namespace ACTION_DANTE;
	using namespace ACTION_VERGIL;

	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
	auto playerIndex = actorData.newPlayerIndex;

	if (actorData.character != CHARACTER::DANTE && actorData.character != CHARACTER::VERGIL) {
		return;
	}

	auto* i = (actorData.newEntityIndex == 0) ? &crimsonPlayer[playerIndex].inertia : &crimsonPlayer[playerIndex].inertiaClone;
	auto action = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].action : crimsonPlayer[playerIndex].actionClone;
	auto motion = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].motion : crimsonPlayer[playerIndex].motionClone;
	auto event = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].event : crimsonPlayer[playerIndex].eventClone;
    auto lastEvent = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].lastEvent : crimsonPlayer[playerIndex].lastEventClone;
    auto& skyLaunch = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].skyLaunch : crimsonPlayer[playerIndex].skyLaunchClone;
	auto state = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].state : crimsonPlayer[playerIndex].stateClone;
	auto horizontalPull =
		(actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].horizontalPull : crimsonPlayer[playerIndex].horizontalPullClone;

	bool inAerialRave = (action == REBELLION_AERIAL_RAVE_PART_1 || action == REBELLION_AERIAL_RAVE_PART_2 ||
		action == REBELLION_AERIAL_RAVE_PART_3 || action == REBELLION_AERIAL_RAVE_PART_4);

	bool inSkyDance =
		(action == AGNI_RUDRA_SKY_DANCE_PART_1 || action == AGNI_RUDRA_SKY_DANCE_PART_2 || action == AGNI_RUDRA_SKY_DANCE_PART_3);

	auto& animTimer = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].animTimer : crimsonPlayer[playerIndex].animTimerClone;
    auto& actionTimer = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].actionTimer : crimsonPlayer[playerIndex].actionTimerClone;
	auto& guardflyTimer = i->guardflyTimer;


	// Part on an idea for a bufferless high time reversal - discarded
	// 		if (!(actorData.action == REBELLION_HIGH_TIME ||
	// 			actorData.action == REBELLION_HIGH_TIME_LAUNCH)) {
	// 			highTimeRotation = actorData.rotation;
	// 		}


	if (!(event == ACTOR_EVENT::JUMP_CANCEL && motion == 33)) {

		if (!inAerialRave || event != ACTOR_EVENT::ATTACK) {
			i->aerialRave.cachedPull = horizontalPull;
			i->aerialRave.cachedRotation = actorData.rotation;
		}

		if (action != CERBERUS_AIR_FLICKER || event != ACTOR_EVENT::ATTACK) {
			i->airFlicker.cachedPull = horizontalPull;
			i->airFlicker.cachedRotation = actorData.rotation;
		}

		if (!inSkyDance || event != ACTOR_EVENT::ATTACK) {
			i->skyDance.cachedPull = horizontalPull;
			i->skyDance.cachedRotation = actorData.rotation;
		}

		if (!(action == NEVAN_AIR_SLASH_PART_1 || action == NEVAN_AIR_SLASH_PART_2) || event != ACTOR_EVENT::ATTACK) {

			i->airSlash.cachedPull = horizontalPull;
			i->airSlash.cachedRotation = actorData.rotation;
		}

// 		if (action != BEOWULF_THE_HAMMER) {
// 			i->theHammer.cachedPull = horizontalPull;
// 			i->theHammer.cachedRotation = actorData.rotation;
// 		}

		if (action != BEOWULF_TORNADO || event != ACTOR_EVENT::ATTACK) {
			i->tornado.cachedPull = horizontalPull;
		}

		if (action != EBONY_IVORY_RAIN_STORM || event != ACTOR_EVENT::ATTACK) {
			i->rainstorm.cachedPull = horizontalPull;
		}


		if (action != SHOTGUN_AIR_FIREWORKS || event != ACTOR_EVENT::ATTACK) {
			i->fireworks.cachedPull = horizontalPull;
		}

		if (action != EBONY_IVORY_AIR_NORMAL_SHOT || event != ACTOR_EVENT::ATTACK) {
			i->ebonyShot.cachedPull = horizontalPull;
		}

		if (action != SHOTGUN_AIR_NORMAL_SHOT || event != ACTOR_EVENT::ATTACK) {
			i->shotgunShot.cachedPull = horizontalPull;
		}

		if (action != ARTEMIS_AIR_NORMAL_SHOT || event != ACTOR_EVENT::ATTACK) {
			i->artemisShot.cachedPull = horizontalPull;
		}

		if (action != ARTEMIS_AIR_MULTI_LOCK_SHOT || event != ACTOR_EVENT::ATTACK) {
			i->artemisMultiLockShot.cachedPull = horizontalPull;
		}

		if (!actorData.airGuard) {
			i->airGuard.cachedRotation = actorData.rotation;
		}

		bool inGuardFlyableMoves = ((action == BEOWULF_KILLER_BEE && actorData.horizontalPull > 20.0f) || 
            (action == NEVAN_VORTEX && actorData.horizontalPull > 20.0f && actionTimer < 0.95f) ||
            ((action == ROYALGUARD_AIR_RELEASE_2 || action == ROYALGUARD_RELEASE_2) && actorData.horizontalPull > 20.0f));

		// Important for Guardflying.
		if (!actorData.airGuard && event != ACTOR_EVENT::JUMP_CANCEL && event != ACTOR_EVENT::AIR_HIKE) {
			if (event == ACTOR_EVENT::TRICKSTER_SKY_STAR) {
				i->airGuard.cachedPull = 28.0f;
			}
			else if (event == ACTOR_EVENT::ATTACK) {
				if (inGuardFlyableMoves) {

					i->airGuard.cachedPull = 28.0f;
				}
			} else {
				i->airGuard.cachedPull = 3.0f;
			}
			
		} 

        if (action == NEVAN_VORTEX && i->airGuard.cachedPull == 28.0f && actionTimer > 0.95f) {
			i->airGuard.cachedPull = 3.0f;  // Reset pull after 0.95s to cut the end of Vortex, preventing Vortex guardfly out of thin air - Berth.
        }

		if ((action == ROYALGUARD_RELEASE_2 ||
            action == ROYALGUARD_AIR_RELEASE_2 ) && i->airGuard.cachedPull == 28.0f && actionTimer > 0.4f) {
			i->airGuard.cachedPull = 3.0f;  // Same reasoning as Vortex.
		}

		if ((action == ROYALGUARD_RELEASE_2 ||
			action == ROYALGUARD_AIR_RELEASE_2) && i->airGuard.cachedPull == 28.0f && skyLaunch.executing) {
			i->airGuard.cachedPull = 3.0f;  // Prevent Sky Launch from being guardflyable.
		}
	}


	// GUARDFLY TIMING
	float timing = 0.14f;
	if (i->airGuard.cachedPull == 28.0f && event == ACTOR_EVENT::JUMP_CANCEL && (actorData.eventData[0].lastEvent == ACTOR_EVENT::ATTACK || actorData.eventData[0].lastEvent == ACTOR_EVENT::TRICKSTER_SKY_STAR ||
		actorData.eventData[0].lastEvent == ACTOR_EVENT::JUMP_CANCEL)) {

		if (guardflyTimer > timing) {
			i->airGuard.cachedPull = 3.0f;  // Reset pull 
		}

		if (actorData.horizontalPull == 28.0f && event != ACTOR_EVENT::AIR_HIKE) {
			CrimsonTimers::ResetGuardflyTimer(guardflyTimer);
		}
	}
	// Handle Air Hike Event (Separate from Jump Cancel)
	else if (i->airGuard.cachedPull == 28.0f && event == ACTOR_EVENT::AIR_HIKE) {

		if (animTimer < 0.01f) {
			CrimsonTimers::ResetGuardflyTimer(guardflyTimer);
		}

		if (guardflyTimer > timing) {
			i->airGuard.cachedPull = 3.0f;  // Reset pull after 0.3s
		}

		if (actorData.horizontalPull == 28.0f) {
			CrimsonTimers::ResetGuardflyTimer(guardflyTimer);
		}
	}
}

void InertiaController(byte8* actorBaseAddr) {
    // Inertia implementation. Momentum (or Pull) is stored before a certain move is executed, then
    // when it's executed the stored pull is applied and may carry over to the next; some moves had their overall momentum
    // increased a bit as well.

    using namespace ACTION_DANTE;
    using namespace ACTION_VERGIL;
    using namespace ACTOR_EVENT;

    if (!actorBaseAddr) {
        return;
    }
    auto& actorData  = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    auto playerIndex = actorData.newPlayerIndex;
    auto* i = (actorData.newEntityIndex == 0) ? &crimsonPlayer[playerIndex].inertia : &crimsonPlayer[playerIndex].inertiaClone;
	auto& action = actorData.action;
	auto& lastAction = actorData.lastAction;
	auto& motion = actorData.motionData[0].index;
	auto& event = actorData.eventData[0].event;
	auto& lastLastEvent =
		(actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].lastLastEvent : crimsonPlayer[playerIndex].lastLastEventClone;
	auto& state = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].state : crimsonPlayer[playerIndex].stateClone;
	auto& lastLastState =
		(actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].lastLastState : crimsonPlayer[playerIndex].lastLastStateClone;

    bool inAerialRave = (action == REBELLION_AERIAL_RAVE_PART_1 || action == REBELLION_AERIAL_RAVE_PART_2 ||
                         action == REBELLION_AERIAL_RAVE_PART_3 || action == REBELLION_AERIAL_RAVE_PART_4);

    bool inSkyDance =
        (action == AGNI_RUDRA_SKY_DANCE_PART_1 || action == AGNI_RUDRA_SKY_DANCE_PART_2 || action == AGNI_RUDRA_SKY_DANCE_PART_3);
    auto& animTimer = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].animTimer : crimsonPlayer[playerIndex].animTimerClone;
    auto& guarflyTimer = i->guardflyTimer;

    //CrimsonPatches::InertiaFixes();


    if (actorData.character == CHARACTER::DANTE) {

        // Guardfly
        if (motion == 5 && actorData.airGuard && (event == JUMP_CANCEL || event == AIR_HIKE)) {

			actorData.horizontalPull = i->airGuard.cachedPull;
			actorData.verticalPullMultiplier = -2;
        }


        // This mimic's DMC4's Air Trick Inertia Boost behaviour, uses LastEventStateQueue
        if (event == ACTOR_EVENT::AIR_TRICK_END && lastLastEvent == ATTACK && lastLastState & STATE::IN_AIR) {
            actorData.horizontalPull = 7.5f;
        }

        // Experimental shit: Reverse Shotgun Stinger
        // 		if (actorData.action == 146 && actorData.eventData[0].event == 17) {
        // 			actorData.horizontalPull = -25.0f;
        // 		}

        if (event == 17 && state & STATE::IN_AIR) { // Attacking in Air

            // Rainstorm
            if (action == EBONY_IVORY_RAIN_STORM) {

                if (i->rainstorm.cachedPull < 0) {
                    i->rainstorm.cachedPull = i->rainstorm.cachedPull * -1.0f;
                }

                float momentum = glm::clamp(i->rainstorm.cachedPull, -9.0f, 9.0f);

                actorData.horizontalPull = momentum / 2.0;
            }

            // E&I Normal Shot
            else if (action == EBONY_IVORY_AIR_NORMAL_SHOT) {
                /*if(inGunShoot) {
                        if(ebonyIvoryShotInertia.cachedDirection == 1 || ebonyIvoryShotInertia.cachedDirection == 0) {
                                if(ebonyIvoryShotInertia.cachedDirection == airRaveInertia.cachedDirection) {

                                        ebonyIvoryShotInertia.cachedPull = ebonyIvoryShotInertia.cachedPull * 1.0f;



                                }
                                else {
                                        if(ebonyIvoryShotInertia.cachedPull > 0) {
                                                ebonyIvoryShotInertia.cachedPull = ebonyIvoryShotInertia.cachedPull * -1.0f;
                                        }
                                }
                        }
                        else {
                                if(ebonyIvoryShotInertia.cachedDirection == airRaveInertia.cachedDirection) {

                                        if(ebonyIvoryShotInertia.cachedPull < 0) {
                                                ebonyIvoryShotInertia.cachedPull = ebonyIvoryShotInertia.cachedPull * -1.0f;
                                        }


                                }
                                else {

                                        if(ebonyIvoryShotInertia.cachedPull > 0) {
                                                ebonyIvoryShotInertia.cachedPull = ebonyIvoryShotInertia.cachedPull * 1.0f;
                                        }
                                }*/

                //}


                /*}
                else {
                        if(ebonyIvoryShotInertia.cachedPull < 0) {
                                ebonyIvoryShotInertia.cachedPull = ebonyIvoryShotInertia.cachedPull * -1.0f;
                        }
                        gunShootInverted = false;
                        gunShootNormalized = false;
                }*/

                if (i->ebonyShot.cachedPull < 0) {
                    i->ebonyShot.cachedPull = i->ebonyShot.cachedPull * -1.0f;
                }

                i->ebonyShot.cachedPull = glm::clamp(i->ebonyShot.cachedPull, -4.0f, 4.0f);

                actorData.horizontalPull           = i->ebonyShot.cachedPull;
                actorData.horizontalPullMultiplier = 0.03f;
            }

            // Shotgun Normal Shot
            else if (action == SHOTGUN_AIR_NORMAL_SHOT) {
                if (i->shotgunShot.cachedPull < 0) {
                    i->shotgunShot.cachedPull = i->shotgunShot.cachedPull * -1.0f;
                }

                i->shotgunShot.cachedPull = glm::clamp(i->shotgunShot.cachedPull, -5.0f, 5.0f);

                actorData.horizontalPull           = i->shotgunShot.cachedPull;
                actorData.horizontalPullMultiplier = 0.05f;
            }

            // Artemis Normal Shot
            else if (action == ARTEMIS_AIR_NORMAL_SHOT) {
                if (i->artemisShot.cachedPull < 0) {
                    i->artemisShot.cachedPull = i->artemisShot.cachedPull * -1.0f;
                }

                i->artemisShot.cachedPull = glm::clamp(i->artemisShot.cachedPull, -5.0f, 5.0f);

                actorData.horizontalPull           = i->artemisShot.cachedPull;
                actorData.horizontalPullMultiplier = 0.05f;
            }

            // Artemis Multi-Lock Shot
            else if (action == ARTEMIS_AIR_MULTI_LOCK_SHOT) {
                if (i->artemisMultiLockShot.cachedPull < 0) {
                    i->artemisMultiLockShot.cachedPull = i->artemisMultiLockShot.cachedPull * -1.0f;
                }

                i->artemisMultiLockShot.cachedPull = glm::clamp(i->artemisMultiLockShot.cachedPull, -5.0f, 5.0f);

                actorData.horizontalPull           = i->artemisMultiLockShot.cachedPull;
                actorData.horizontalPullMultiplier = 0.05f;

            }

            // Fireworks
            else if (action == SHOTGUN_AIR_FIREWORKS) {
                if (i->fireworks.cachedPull < 0) {
                    i->fireworks.cachedPull = i->fireworks.cachedPull * -1.0f;
                }

                i->fireworks.cachedPull  = glm::clamp(i->fireworks.cachedPull, -9.0f, 9.0f);
                actorData.horizontalPull = i->fireworks.cachedPull / 1.5f;
            }

            // Aerial Rave
            else if (inAerialRave) {
                if (i->aerialRave.cachedPull < 0) {
                    i->aerialRave.cachedPull = i->aerialRave.cachedPull * -1.0f;
                }


                i->aerialRave.cachedPull = glm::clamp(i->aerialRave.cachedPull, -9.0f, 9.0f);
                // If this is any weapon air shot then inertia transfers (almost) completely
                if (lastAction != EBONY_IVORY_AIR_NORMAL_SHOT && lastAction != SHOTGUN_AIR_NORMAL_SHOT &&
                    lastAction != ARTEMIS_AIR_NORMAL_SHOT && lastAction != ARTEMIS_AIR_MULTI_LOCK_SHOT &&
                    lastAction != AGNI_RUDRA_SKY_DANCE_PART_1 && lastAction != AGNI_RUDRA_SKY_DANCE_PART_2 &&
                    lastAction != AGNI_RUDRA_SKY_DANCE_PART_3) {

                    actorData.horizontalPull = (i->aerialRave.cachedPull / 3.0) * 1.0f;

                } else {
                    actorData.horizontalPull = (i->aerialRave.cachedPull / 1.2f) * 1.0f;
                }
            }

            // Cerberus Flicker
            else if (action == CERBERUS_AIR_FLICKER) {

                if (i->airFlicker.cachedPull < 0) {
                    i->airFlicker.cachedPull = i->airFlicker.cachedPull * -1.0f;
                }

                i->airFlicker.cachedPull = glm::clamp(i->airFlicker.cachedPull, 2.0f, 9.0f);
                actorData.horizontalPull = (i->airFlicker.cachedPull / 2.0) * 1.0f;
            }

            // Sky Dance
            else if (inSkyDance) {

                if (i->skyDance.cachedPull < 0) {
                    i->skyDance.cachedPull = i->skyDance.cachedPull * -1.0f;
                }

                i->skyDance.cachedPull = glm::clamp(i->skyDance.cachedPull, 3.0f, 9.0f);

                if (lastAction != EBONY_IVORY_AIR_NORMAL_SHOT && lastAction != SHOTGUN_AIR_NORMAL_SHOT &&
                    lastAction != ARTEMIS_AIR_NORMAL_SHOT && lastAction != ARTEMIS_AIR_MULTI_LOCK_SHOT &&
                    lastAction != REBELLION_AERIAL_RAVE_PART_1 && lastAction != REBELLION_AERIAL_RAVE_PART_2 &&
                    lastAction != REBELLION_AERIAL_RAVE_PART_3) {

                    actorData.horizontalPull = (i->skyDance.cachedPull / 4.0f) * 1.0f;
                } else {
                    actorData.horizontalPull = (i->skyDance.cachedPull / 1.2f) * 1.0f;
                }
            }

            // Air Slash
            else if (action == NEVAN_AIR_SLASH_PART_1 || action == NEVAN_AIR_SLASH_PART_2) {

                if (i->airSlash.cachedPull < 0) {
                    i->airSlash.cachedPull = i->airSlash.cachedPull * -1.0f;
                }

                i->airSlash.cachedPull = glm::clamp(i->airSlash.cachedPull, -9.0f, 9.0f);

                actorData.horizontalPull = (i->airSlash.cachedPull / 1.5f) * 1.0f;
            }

            // Air Play
            else if (action == NEVAN_AIR_PLAY) {
                actorData.horizontalPullMultiplier = 0.2f;
            }

            // The Hammer
            else if (action == BEOWULF_THE_HAMMER) {
                   actorData.horizontalPull = 0;  // Having inertia on Hammer makes it hard to use with the new input (lock on + back + style)
            }

            // Tornado
            else if (action == BEOWULF_TORNADO) {
                if (i->tornado.cachedPull < 0) {
                    i->tornado.cachedPull = i->tornado.cachedPull * -1.0f;
                }

                i->tornado.cachedPull    = glm::clamp(i->tornado.cachedPull, -9.0f, 9.0f);
                actorData.horizontalPull = (i->tornado.cachedPull / 1.5f) * 1.0f;

            }

            // Making Killer Bee's inertia be constant may lead to inconsistent speeds
            // therefore we generate guardflying inertia "artificially" so to speak.

        }
    }
}

void GravityCorrections(byte8* actorBaseAddr) {
	using namespace ACTION_DANTE;

	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    auto playerIndex = actorData.newPlayerIndex;
    auto entityIndex = actorData.newEntityIndex;
	auto& action = actorData.action;
	auto& event = actorData.eventData[0].event;
	auto& state = actorData.state;
	auto& animTimer = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].animTimer : crimsonPlayer[playerIndex].animTimerClone;
    static bool setSkyStarGravityPlayer[PLAYER_COUNT][ENTITY_COUNT] = { false };
    auto& setSkyStarGravity = setSkyStarGravityPlayer[playerIndex][entityIndex];

	// Air Hike Resets weights
    if (event == ACTOR_EVENT::AIR_HIKE) {
		actorData.airSwordAttackCount = 0;
	}

    // Sky Star Gravity remains consistent regardless of weights
    if (event == ACTOR_EVENT::TRICKSTER_SKY_STAR && !setSkyStarGravity && animTimer < 0.2f) {
        actorData.verticalPullMultiplier = 0.0f;
        setSkyStarGravity = true;
    }
    else if (event != ACTOR_EVENT::TRICKSTER_SKY_STAR) {
        setSkyStarGravity = false;
    }
}

void ShotgunAirShotTweaks(byte8* actorBaseAddr) {
    // Reduces Shotgun Air Shot Gravity
	using namespace ACTION_DANTE;

	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    auto& action = actorData.action;

    if (action == SHOTGUN_AIR_NORMAL_SHOT) {
        actorData.verticalPullMultiplier = -1.6f; // Experimental: tweaking Shotgun Gravity to be lower.
    }
}

void AerialRaveGravityTweaks(byte8* actorBaseAddr) {
    // Increases gravity while air raving, while also adding weights into the equation. - Mia
    using namespace ACTION_DANTE;

    if (!actorBaseAddr) {
        return;
    }
    auto& actorData  = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    auto playerIndex = actorData.newPlayerIndex;

    auto* tweak    = &crimsonPlayer[playerIndex].airFlickerTweak;
    auto& action    = actorData.action;
    auto& event     = actorData.eventData[0].event;
    auto& state     = actorData.state;
    auto& animTimer = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].animTimer : crimsonPlayer[playerIndex].animTimerClone;

    bool inAerialRave = (action == REBELLION_AERIAL_RAVE_PART_1 || action == REBELLION_AERIAL_RAVE_PART_2 ||
                         action == REBELLION_AERIAL_RAVE_PART_3 || action == REBELLION_AERIAL_RAVE_PART_4);

    if (actorData.character == CHARACTER::DANTE) {
        if (state & STATE::ON_FLOOR || event == ACTOR_EVENT::JUMP_CANCEL) {
            tweak->gravity            = 0;
            tweak->gravityPre4Changed = false;
            tweak->gravity4Changed    = false;
        }
    }

    if (event == ACTOR_EVENT::ATTACK && state & STATE::IN_AIR && actorData.character == CHARACTER::DANTE) {


        if (inAerialRave && animTimer < 0.25f) { // animTimer has been added here to reduce the floatiness feel. - Mia
            if (action != REBELLION_AERIAL_RAVE_PART_4) {
                if (actorData.airSwordAttackCount == 1) {
                    actorData.verticalPull           = 0;
                    actorData.verticalPullMultiplier = 0;
                } else if (actorData.airSwordAttackCount > 1) {
                    if (!tweak->gravityPre4Changed) {

                        tweak->gravity += -1.0f;
                        tweak->gravityPre4Changed = true;
                    }
                    actorData.verticalPull           = tweak->gravity + (-0.3f * actorData.airSwordAttackCount);
                    actorData.verticalPullMultiplier = -0.5 * actorData.airSwordAttackCount;
                    tweak->gravity4Changed           = false;
                }
            } else if (action == REBELLION_AERIAL_RAVE_PART_4) {
                if (!tweak->gravity4Changed) {

                    tweak->gravity += -1.5f;
                    tweak->gravity4Changed = true;
                }
                actorData.verticalPull           = tweak->gravity + (-0.2f * actorData.airSwordAttackCount);
                actorData.verticalPullMultiplier = -0.1 * actorData.airSwordAttackCount;
            }
        }
    }
}

void AirFlickerGravityTweaks(byte8* actorBaseAddr) {
	// Reduces gravity while air flickering, while also adding weights into account.
	// Take into account that Vanilla gravity fall off is VericalPullMultiplier = -1.5f. - Mia
	using namespace ACTION_DANTE;

	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
	if (actorData.character != CHARACTER::DANTE && actorData.character != CHARACTER::VERGIL) return;
	auto playerIndex = actorData.newPlayerIndex;
	auto& gamepad = GetGamepad(actorData.newPlayerIndex);

	auto* tweak = (actorData.newEntityIndex == ENTITY::MAIN) ? &crimsonPlayer[playerIndex].airFlickerTweak : 
        &crimsonPlayer[playerIndex].airFlickerTweakClone;
	auto action = actorData.action;
	auto lastAction = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].lastAction : crimsonPlayer[playerIndex].lastActionClone;
	auto event =  actorData.eventData[0].event;
    auto motion = actorData.motionData[0].index;
	auto& state = actorData.state;
	auto actionTimer =
		(actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].actionTimer : crimsonPlayer[playerIndex].actionTimerClone;
    auto& lastActionTime = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].lastActionTime : crimsonPlayer[playerIndex].lastActionTimeClone;

	if (event == ACTOR_EVENT::ATTACK && event != ACTOR_EVENT::AIR_HIKE && event != ACTOR_EVENT::JUMP_CANCEL && event != ACTOR_EVENT::JUMP && state & STATE::IN_AIR && actorData.character == CHARACTER::DANTE) {

		if (action == CERBERUS_AIR_FLICKER) {
			if (!tweak->hasAppliedVerticalPullMultiplier) {
				if (motion == 7) {
					// Reduces Gravity Fall-off
                    if (actorData.airSwordAttackCount <= 1) {
                        actorData.verticalPullMultiplier = -0.22f; // Vanilla value is -0.27f
                    } else {
                        actorData.verticalPullMultiplier = -0.22f + (-0.05f * actorData.airSwordAttackCount);
                    }
					

// 					if ((actorData.style == STYLE::SWORDMASTER) && (gamepad.buttons[0] & GetBinding(BINDING::STYLE_ACTION))) {
// 						actorData.state &= ~STATE::BUSY; // Allows you to cancel into another Flicker during the falling animation.
// 					}
				} else {
					
					// Reduces Gravity Fall-off
					//actorData.verticalPullMultiplier = -1.5f;
				}
				tweak->hasAppliedVerticalPullMultiplier = true;
			}
		} else {
			tweak->hasAppliedVerticalPullMultiplier = false; // Reset flag when not in flicker
		}

	} 
    if (action != CERBERUS_AIR_FLICKER || event != ACTOR_EVENT::ATTACK || (action != CERBERUS_AIR_FLICKER && lastAction != CERBERUS_AIR_FLICKER)) {
		// Reset flag if not in the action or the attack event
		tweak->hasAppliedVerticalPullMultiplier = false;
	}


}

void SkyDanceGravityTweaks(byte8* actorBaseAddr) {
	// Reduces gravity while sky dancing, while also adding weights into account.
	// This is also combined with the SkyDanceTweak in SetAction,
	// which separates Sky Dance Part 3 into its own ability, triggered by lock on + forward + style. - Mia
	using namespace ACTION_DANTE;

	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
	if (actorData.character != CHARACTER::DANTE && actorData.character != CHARACTER::VERGIL) return;
	auto playerIndex = actorData.newPlayerIndex;
	auto& gamepad = GetGamepad(actorData.newPlayerIndex);

	auto* tweak = (actorData.newEntityIndex == ENTITY::MAIN) ? &crimsonPlayer[playerIndex].skyDanceTweak :
		&crimsonPlayer[playerIndex].skyDanceTweakClone;
    auto action = actorData.action;
	auto lastAction = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].lastAction : crimsonPlayer[playerIndex].lastActionClone;
    auto event = actorData.eventData[0].event;
    auto motion = actorData.motionData[0].index;
	auto& state = actorData.state;
	auto actionTimer =
		(actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].actionTimer : crimsonPlayer[playerIndex].actionTimerClone;

	bool inSkyDance = (action == AGNI_RUDRA_SKY_DANCE_PART_1 || action == AGNI_RUDRA_SKY_DANCE_PART_2);
	bool inSkyDanceLastAction = (lastAction == AGNI_RUDRA_SKY_DANCE_PART_1 || lastAction == AGNI_RUDRA_SKY_DANCE_PART_2);
    auto& lastActionTime = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].lastActionTime : crimsonPlayer[playerIndex].lastActionTimeClone;

	if (event == ACTOR_EVENT::ATTACK && event != ACTOR_EVENT::AIR_HIKE && event != ACTOR_EVENT::JUMP_CANCEL && event != ACTOR_EVENT::JUMP
		&& state & STATE::IN_AIR && actorData.character == CHARACTER::DANTE) {

		if (inSkyDance) {
			if (!tweak->hasAppliedVerticalPullMultiplier) {

				actorData.verticalPullMultiplier = (-0.04f * (actorData.airSwordAttackCount * 1.6f)); // Vanilla value is -0.04f

				tweak->hasAppliedVerticalPullMultiplier = true;
			}
		} else {
			tweak->hasAppliedVerticalPullMultiplier = false; // Reset flag when not in Sky Dance
		}
	}    

    if (!inSkyDance || event != ACTOR_EVENT::ATTACK) {
		// Reset flag if not in the action or the attack event
		tweak->hasAppliedVerticalPullMultiplier = false;
	}
}

void EbonyAndIvoryAerialTweaks(byte8* actorBaseAddr) {
	// Reduces gravity while sky dancing, while also adding weights into account.
	// This is also combined with the SkyDanceTweak in SetAction,
	// which separates Sky Dance Part 3 into its own ability, triggered by lock on + forward + style. - Mia
	using namespace ACTION_DANTE;

	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
	if (actorData.character != CHARACTER::DANTE && actorData.character != CHARACTER::VERGIL) return;
	auto playerIndex = actorData.newPlayerIndex;
	auto& gamepad = GetGamepad(actorData.newPlayerIndex);

	auto* tweak = (actorData.newEntityIndex == ENTITY::MAIN) ? &crimsonPlayer[playerIndex].ebonyIvoryTweak :
		&crimsonPlayer[playerIndex].ebonyIvoryTweakClone;
	auto action = actorData.action;
	auto lastAction = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].lastAction : crimsonPlayer[playerIndex].lastActionClone;
	auto event = actorData.eventData[0].event;
	auto motion = actorData.motionData[0].index;
	auto& state = actorData.state;
	auto actionTimer =
		(actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].actionTimer : crimsonPlayer[playerIndex].actionTimerClone;

	bool inEbonyAndIvoryAirShot = (action == EBONY_IVORY_AIR_NORMAL_SHOT);
	auto& lastActionTime = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].lastActionTime : crimsonPlayer[playerIndex].lastActionTimeClone;

	if (event == ACTOR_EVENT::ATTACK && event != ACTOR_EVENT::AIR_HIKE && event != ACTOR_EVENT::JUMP_CANCEL && event != ACTOR_EVENT::JUMP
		&& state & STATE::IN_AIR && actorData.character == CHARACTER::DANTE) {

        if (inEbonyAndIvoryAirShot) {
			if (!tweak->hasAppliedVerticalPullMultiplier) {

                actorData.verticalPullMultiplier = (-1.2f - (0.3f * actorData.airSwordAttackCount));  // Vanilla value is -1.25f

				tweak->hasAppliedVerticalPullMultiplier = true;
			}
		} else {
			tweak->hasAppliedVerticalPullMultiplier = false; // Reset flag when not in Sky Dance
		}
	}

	if (!inEbonyAndIvoryAirShot || event != ACTOR_EVENT::ATTACK) {
		// Reset flag if not in the action or the attack event
		tweak->hasAppliedVerticalPullMultiplier = false;
	}
}

void DanteDownertia(byte8* actorBaseAddr) {
	using namespace ACTION_DANTE;

	if (!activeCrimsonGameplay.Gameplay.Dante.downertiaD) {
		return;
	}

	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
	if (actorData.character != CHARACTER::DANTE && actorData.character != CHARACTER::VERGIL) return;
	auto playerIndex = actorData.newPlayerIndex;
	auto& gamepad = GetGamepad(actorData.newPlayerIndex);

	auto& airFlickerTweak = (actorData.newEntityIndex == ENTITY::MAIN) ? crimsonPlayer[playerIndex].airFlickerTweak :
		crimsonPlayer[playerIndex].airFlickerTweakClone;
	auto& skyDanceTweak = (actorData.newEntityIndex == ENTITY::MAIN) ? crimsonPlayer[playerIndex].skyDanceTweak :
		crimsonPlayer[playerIndex].skyDanceTweakClone;
	auto action = actorData.action;
	auto lastAction = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].lastAction : crimsonPlayer[playerIndex].lastActionClone;
	auto event = actorData.eventData[0].event;
	auto motion = actorData.motionData[0].index;
	auto& state = actorData.state;
	auto actionTimer =
		(actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].actionTimer : crimsonPlayer[playerIndex].actionTimerClone;

	bool inSkyDance = (action == AGNI_RUDRA_SKY_DANCE_PART_1 || action == AGNI_RUDRA_SKY_DANCE_PART_2);
	bool inSkyDanceLastAction = (lastAction == AGNI_RUDRA_SKY_DANCE_PART_1 || lastAction == AGNI_RUDRA_SKY_DANCE_PART_2);
	auto& lastActionTime = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].lastActionTime : crimsonPlayer[playerIndex].lastActionTimeClone;

	if (event != ACTOR_EVENT::ATTACK) {
		return;
	}

	// AIR FLICKER DOWNERTIA
	if (action != CERBERUS_AIR_FLICKER && lastAction == CERBERUS_AIR_FLICKER && lastActionTime < 0.30f && !airFlickerTweak.hasAppliedVerticalPullMultiplier) {
		actorData.verticalPullMultiplier = -0.05f + (-0.15f * (actorData.airSwordAttackCount + 6)); // Vanilla value is -0.27f
		airFlickerTweak.hasAppliedVerticalPullMultiplier = true;
	}

	if (action != CERBERUS_AIR_FLICKER || event != ACTOR_EVENT::ATTACK || (action != CERBERUS_AIR_FLICKER && lastAction != CERBERUS_AIR_FLICKER)) {
		// Reset flag if not in the action or the attack event
		airFlickerTweak.hasAppliedVerticalPullMultiplier = false;
	}


	// SKY DANCE DOWNERTIA
	if (!inSkyDance && (lastAction == AGNI_RUDRA_SKY_DANCE_PART_1 || lastAction == AGNI_RUDRA_SKY_DANCE_PART_2) && lastActionTime < 0.30f
		&& !skyDanceTweak.hasAppliedVerticalPullMultiplier) {
		actorData.verticalPullMultiplier = (-0.04f * ((actorData.airSwordAttackCount + 6) * 1.6f)); // Vanilla value is -0.04f
		skyDanceTweak.hasAppliedVerticalPullMultiplier = true;
	}

	if (!inSkyDance || event != ACTOR_EVENT::ATTACK) {
		// Reset flag if not in the action or the attack event
		skyDanceTweak.hasAppliedVerticalPullMultiplier = false;
	}
}

#pragma endregion

#pragma region GeneralGameplay

void DMC4Mobility(byte8* actorBaseAddr) {
	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);

	auto playerIndex = actorData.newPlayerIndex;
	auto entityIndex = actorData.newEntityIndex;
	auto& event = actorData.eventData[0].event;
	auto& state = actorData.state;
	auto& airTrickCount = actorData.newAirTrickCount;
	static uint8 savedAirTrickCounts[PLAYER_COUNT][ENTITY_COUNT] = { 0 };
	auto& savedAirTrickCount = savedAirTrickCounts[playerIndex][entityIndex];
	bool enable = activeCrimsonGameplay.Gameplay.Dante.dmc4Mobility;
	static bool run = false;
	// Messing with default Mobility Settings to be equivalent to DMC4 counts.
	if (run != enable && enable) {
		defaultCrimsonGameplay.Cheats.Mobility.airHikeCount[1] = 2;
		defaultCrimsonGameplay.Cheats.Mobility.wallHikeCount[1] = 2;
		defaultCrimsonGameplay.Cheats.Mobility.skyStarCount[1] = 2;
		defaultCrimsonGameplay.Cheats.Mobility.danteAirTrickCount[1] = 2;
		run = enable;
	} else if (run != enable && !enable) {
		defaultCrimsonGameplay.Cheats.Mobility.airHikeCount[1] = 1;
		defaultCrimsonGameplay.Cheats.Mobility.wallHikeCount[1] = 1;
		defaultCrimsonGameplay.Cheats.Mobility.skyStarCount[1] = 1;
		defaultCrimsonGameplay.Cheats.Mobility.danteAirTrickCount[1] = 1;
		if (!activeCrimsonGameplay.Cheats.General.customMobility) {
			queuedCrimsonGameplay.Cheats.Mobility.airHikeCount[1] = 1;
			queuedCrimsonGameplay.Cheats.Mobility.wallHikeCount[1] = 1;
			queuedCrimsonGameplay.Cheats.Mobility.skyStarCount[1] = 1;
			queuedCrimsonGameplay.Cheats.Mobility.danteAirTrickCount[1] = 1;
			activeCrimsonGameplay.Cheats.Mobility.airHikeCount[1] = 1;
			activeCrimsonGameplay.Cheats.Mobility.wallHikeCount[1] = 1;
			activeCrimsonGameplay.Cheats.Mobility.skyStarCount[1] = 1;
			activeCrimsonGameplay.Cheats.Mobility.danteAirTrickCount[1] = 1;
		}
		run = enable;
	}
	if (!activeCrimsonGameplay.Gameplay.Dante.dmc4Mobility) {
		return;
	}
	// The Reset portion
	if (actorData.state & STATE::ON_FLOOR && event == ACTOR_EVENT::TRICKSTER_AIR_TRICK) {
		airTrickCount = savedAirTrickCount;
	} else {
		savedAirTrickCount = airTrickCount;
	}
}

void AdjustDMC4MobilitySettings() {
	if (activeCrimsonGameplay.Gameplay.Dante.dmc4Mobility) {
		queuedCrimsonGameplay.Cheats.Mobility.airHikeCount[1] = 2;
		queuedCrimsonGameplay.Cheats.Mobility.wallHikeCount[1] = 2;
		queuedCrimsonGameplay.Cheats.Mobility.skyStarCount[1] = 2;
		queuedCrimsonGameplay.Cheats.Mobility.danteAirTrickCount[1] = 2;
		activeCrimsonGameplay.Cheats.Mobility.airHikeCount[1] = 2;
		activeCrimsonGameplay.Cheats.Mobility.wallHikeCount[1] = 2;
		activeCrimsonGameplay.Cheats.Mobility.skyStarCount[1] = 2;
		activeCrimsonGameplay.Cheats.Mobility.danteAirTrickCount[1] = 2;
	} else {
		queuedCrimsonGameplay.Cheats.Mobility.airHikeCount[1] = 1;
		queuedCrimsonGameplay.Cheats.Mobility.wallHikeCount[1] = 1;
		queuedCrimsonGameplay.Cheats.Mobility.skyStarCount[1] = 1;
		queuedCrimsonGameplay.Cheats.Mobility.danteAirTrickCount[1] = 1;
		activeCrimsonGameplay.Cheats.Mobility.airHikeCount[1] = 1;
		activeCrimsonGameplay.Cheats.Mobility.wallHikeCount[1] = 1;
		activeCrimsonGameplay.Cheats.Mobility.skyStarCount[1] = 1;
		activeCrimsonGameplay.Cheats.Mobility.danteAirTrickCount[1] = 1;
	}
}


void StyleMeterDoppelganger(byte8* actorBaseAddr) {

    // This allows Doppelgangers to actually fill your style meter with their attacks (but not increase your ranks).
    auto pool_12119 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
    if (!pool_12119 || !pool_12119[3]) {
        return;
    }
    auto& actorData      = *reinterpret_cast<PlayerActorData*>(pool_12119[3]);
    auto& characterData  = GetCharacterData(actorData);
    auto& cloneActorData = *reinterpret_cast<PlayerActorData*>(actorData.cloneActorBaseAddr);

    if (actorData.doppelganger) {
        cloneActorData.styleData.rank = actorData.styleData.rank;

        if (actorData.styleData.meter > 50) {
            actorData.styleData.meter = glm::max(actorData.styleData.meter, cloneActorData.styleData.meter);
        }
    }

//     actorData.styleData.rank = 7;
//     actorData.styleData.meter = 699.0f;

//     if (actorData.royalguardBlockType == 2) {
//         PlayDevilTriggerReady(playerIndex);
//     }
}


void InCombatDetection() {
    auto inCombatDetector = *reinterpret_cast<int*>(appBaseAddr + 0xD23A80);
    auto inBossfightDetector = *reinterpret_cast<int*>(appBaseAddr + 0xD5BA88);

	if (inBossfightDetector > 10) {
		g_inBossfight = true;
	}
	else {
		g_inBossfight = false;
	}
    
    if (inCombatDetector == 1 || g_inBossfight) {
        g_inCombat = true;
    }
    else {
        g_inCombat = false;
    }

    g_bossQuantity = inBossfightDetector - 10;
}


void BackToForwardInputs(byte8* actorBaseAddr) {
    if (!actorBaseAddr) {
        return;
    }
    auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);

    auto lockOn        = actorData.lockOn;
    auto tiltDirection = GetRelativeTiltDirection(actorData);
    auto playerIndex   = actorData.newPlayerIndex;
    auto& gamepad      = GetGamepad(playerIndex);
    auto radius        = gamepad.leftStickRadius;
    auto pos           = gamepad.leftStickPosition;
    uint8 deadzone = 100;
    auto& b2F = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].b2F : crimsonPlayer[playerIndex].b2FClone;

    if (actorData.character != CHARACTER::DANTE && actorData.character != CHARACTER::VERGIL) {
        return;
    }

    if (b2F.backBuffer <= 0) {
        b2F.backCommand = false;
    }

    if (b2F.backBuffer <= 0 && b2F.backDirectionChanged) {
        b2F.backBuffer = b2F.backDuration;
    }

    if (b2F.forwardBuffer <= 0) {
        b2F.forwardCommand = false;
    }

    if (b2F.forwardBuffer <= 0 && b2F.forwardDirectionChanged) {
        b2F.forwardBuffer = b2F.forwardDuration;
    }

    if (lockOn && tiltDirection == TILT_DIRECTION::DOWN && (radius > deadzone)) {
        if (b2F.backBuffer > 0) {
            b2F.backCommand          = true;
            b2F.backDirectionChanged = false;
        }
    } else if (tiltDirection != TILT_DIRECTION::DOWN) {
        b2F.backDirectionChanged = true;
    }


    if (lockOn && tiltDirection == TILT_DIRECTION::UP && (radius > deadzone) && b2F.backCommand) {
        if (b2F.forwardBuffer > 0) {
            b2F.forwardCommand          = true;
            b2F.forwardDirectionChanged = false;
        }
    } 
    else if (!(lockOn && tiltDirection == TILT_DIRECTION::UP && (radius > deadzone))) {
        b2F.forwardDirectionChanged = true;
    }
}


void SprintAbility(byte8* actorBaseAddr) {

    if (!actorBaseAddr) {
        return;
    }
    auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    auto playerIndex = actorData.newPlayerIndex;
    auto& sprintData = crimsonPlayer[playerIndex].sprint;
    auto& playerData = GetPlayerData(actorData);

    
    bool playerRunning = ((actorData.eventData[0].event == 3) && (actorData.motionData[0].index == 2 || actorData.motionData[0].index == 51 || actorData.motionData[0].index == 30));

    if ((actorData.newCharacterIndex == playerData.activeCharacterIndex) && (actorData.newEntityIndex == ENTITY::MAIN)) {

        // Storing the actor's set speed when not sprinting for us to calculate sprintSpeed.
        if (!sprintData.isSprinting) {
           
            sprintData.storedSpeedHuman = activeCrimsonGameplay.Cheats.Speed.human;

            if (actorData.character == CHARACTER::DANTE) {
                for (int i = 0; i < 6; i++) {
                    sprintData.storedSpeedDevilDante[i] = activeCrimsonGameplay.Cheats.Speed.dTDante[i];
                }
  
            } else if (actorData.character == CHARACTER::VERGIL) {
				for (int i = 0; i < 5; i++) {
					sprintData.storedSpeedDevilVergil[i] = activeCrimsonGameplay.Cheats.Speed.dTVergil[i];
				}
            }

            // Resets FX state
            sprintData.SFXPlayed = false;
            sprintData.VFXPlayed = false;
        }


        // This ties the ability only to the main actor.
        // 	if ((actorData.newPlayerIndex == 0) &&
        // 		(actorData.newCharacterIndex == playerData.activeCharacterIndex) &&
        // 		(actorData.newEntityIndex == ENTITY::MAIN)) {

        // Set time before sprint sets in
        if (playerRunning) {

            if (!sprintData.runTimer) {
                if (!g_inCombat) {
                    sprintData.timer = sprintData.duration / (actorData.speed / g_FrameRateTimeMultiplier); // Out of combat sprint is nearly instant
                }
                else {
					if (!actorData.devil) {
						sprintData.timer = sprintData.durationCombatHuman / (actorData.speed / g_FrameRateTimeMultiplier); // In combat it has more wind-up
					}
					else {
						sprintData.timer = sprintData.durationCombatDevil / (actorData.speed / g_FrameRateTimeMultiplier);
					}
                }

            }
            
            if (!sprintData.canSprint) {
                sprintData.runTimer = true;
            }


        } 
        else {
            sprintData.canSprint = false;
            sprintData.runTimer  = false;
        }

        auto gameSpeedValue = (IsTurbo()) ? activeConfig.Speed.turbo : activeConfig.Speed.mainSpeed;

        if (sprintData.canSprint) {

            // Setting the sprint speed. Increasing by 30%.
            float sprintMultiplier = 1.3f;

            // Calculating sprint speed
            float sprintSpeed      = crimsonPlayer[playerIndex].sprint.storedSpeedHuman * sprintMultiplier;
            float sprintSpeedDevilDante[6] = {};
            float sprintSpeedDevilVergil[5] = {};
            if (actorData.character == CHARACTER::DANTE) {
                for (int i = 0; i < 6; i++) {
                    sprintSpeedDevilDante[i] = sprintData.storedSpeedDevilDante[i] * sprintMultiplier;
                }

            } else if (actorData.character == CHARACTER::VERGIL) {
				for (int i = 0; i < 5; i++) {
					sprintSpeedDevilVergil[i] = sprintData.storedSpeedDevilVergil[i] * sprintMultiplier;
				}
            }


            // Applying the new Speed
            if (!actorData.devil) {
                actorData.speed = sprintSpeed * gameSpeedValue;

            } else {
                switch (actorData.character) {
                case CHARACTER::DANTE: {
                    auto devilIndex = actorData.meleeWeaponIndex;
                    if (devilIndex > 4) {
                        devilIndex = 0;
                    }

                    if (actorData.sparda) {
                        devilIndex = DEVIL_SPEED::DANTE_SPARDA;
                    }

                    actorData.speed = sprintSpeedDevilDante[devilIndex] * gameSpeedValue;

                    break;
                }
                case CHARACTER::VERGIL: {
                    auto devilIndex = actorData.queuedMeleeWeaponIndex;
                    if (devilIndex > 2) {
                        devilIndex = 0;
                    }

                    if (actorData.neroAngelo) {
                        if (devilIndex > 1) {
                            devilIndex = 0;
                        }

                        devilIndex += 3;
                    }

                    actorData.speed = sprintSpeedDevilVergil[devilIndex] * gameSpeedValue;

                    break;
                }
                }
            }

            // FX
            if (!sprintData.SFXPlayed) {
                CrimsonSDL::PlaySprint(playerIndex);
                sprintData.SFXPlayed = true;
				actorData.motionArchives[MOTION_GROUP_DANTE::BASE] = newBaseDanteAnims_pl000_00_0; // Change Sprint Animation
				PlayAnimation_1EFB90(actorData, 0, 20, 20.0f, 0, 0, -1); // Play the anim
            }


            if (!sprintData.VFXPlayed) {
                uint8 vfxColor[4] = { 35, 35, 35, 255 };
                uint32 actualColor = CrimsonUtil::Uint8toAABBGGRR(vfxColor);
                CrimsonDetours::CreateEffectDetour(actorBaseAddr, sprintVFX.bank, sprintVFX.id, 1, true, actualColor, 0.3f);

                sprintData.VFXPlayed = true;
            }

            sprintData.isSprinting = true;
            sprintData.runTimer    = false;


		} else {
			// Restore the original Actor's speed when you can't sprint (either in or out of it).
			if (sprintData.isSprinting) {
				// restore the anim when not sprinting
				if (actorData.character == CHARACTER::DANTE) {
					actorData.motionArchives[MOTION_GROUP_DANTE::BASE] = File_staticFiles[pl000_00_0]; 
				} else if (actorData.character == CHARACTER::VERGIL) {
					actorData.motionArchives[MOTION_GROUP_VERGIL::BASE] = File_staticFiles[pl021_00_0];
				}
			}
			sprintData.isSprinting = false;
		}
    }
}

#include <chrono>

void DTInfusedRoyalguardController(byte8* actorBaseAddr) {
	// This makes normal block consume DT instead of health, when DT is above 0,
	// guard breaks occur only when DT is exhausted

	if (!actorBaseAddr) {
		return;
	}

    CrimsonDetours::ToggleDTInfusedRoyalguardDetours((activeCrimsonGameplay.Gameplay.General.extramoves && ExpConfig::missionExpDataDante.unlocks[UNLOCK_DANTE::ROYALGUARD_MODDED_MOVES]));
    if (!(activeCrimsonGameplay.Gameplay.General.extramoves && ExpConfig::missionExpDataDante.unlocks[UNLOCK_DANTE::ROYALGUARD_MODDED_MOVES])) {
        return;
    }

	using namespace std::chrono;

	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
	auto playerIndex = actorData.newPlayerIndex;
	auto& event = actorData.eventData[0].event;
	auto& motionDataIndex = actorData.motionData[0].index;
	auto& playerData = GetPlayerData(actorData);
	bool inNormalBlock = (actorData.royalBlock == 0 || actorData.royalBlock == 4);
	bool inUltimate = (actorData.royalBlock == 5);
	bool inGuardBreak = ((event == 20 && motionDataIndex == 3) && !inNormalBlock);
	bool ensureIsMainPlayer = ((actorData.newCharacterIndex == playerData.activeCharacterIndex) && (actorData.newEntityIndex == ENTITY::MAIN));
	auto& currentDT = actorData.magicPoints;
	auto& currentReleaseDamage = actorData.royalguardReleaseDamage;

	static bool guardPlayed[PLAYER_COUNT] = { false };
	static bool guardBroke[PLAYER_COUNT] = { false };
	static bool normalBlocked[PLAYER_COUNT] = { false };
	static bool toggledCheats[PLAYER_COUNT] = { false };
	static float storedDT[PLAYER_COUNT] = {};
	static float storedReleaseDamage[PLAYER_COUNT] = {};
	static time_point<high_resolution_clock> blockResetTimes[PLAYER_COUNT];

	// This makes it so Guard Breaks have a different anim and vfx - unused since GuardBreaks are now converted to 
    // Normal Blocks through the detours.
	if (actorData.royalBlock != 3 && !inNormalBlock && actorData.royalBlock != 6 && event == 20) {
		actorData.royalBlock = 1;
	}

	if (ensureIsMainPlayer && currentDT > 0) {
		if (inNormalBlock) {
			if (!normalBlocked[playerIndex]) {
				storedDT[playerIndex] = std::max(storedDT[playerIndex] - 2000, 0.0f);
				storedReleaseDamage[playerIndex] = std::min(storedReleaseDamage[playerIndex] + 100, 9000.0f);
				currentReleaseDamage = storedReleaseDamage[playerIndex];
				if (!activeCrimsonGameplay.Cheats.Training.infiniteDT) {
					currentDT = storedDT[playerIndex];
				}
                normalBlocked[playerIndex] = true;

                // VFX
				uint8 vfxColor[4] = { 48, 0, 10, 255 };
				uint32 actualColor = CrimsonUtil::Uint8toAABBGGRR(vfxColor);
				CrimsonDetours::CreateEffectDetour(actorBaseAddr, 3, 61, 15, true, actualColor, 1.3f);

                // SFX
                CrimsonSDL::PlayNormalBlock(playerIndex);
				blockResetTimes[playerIndex] = high_resolution_clock::now() + milliseconds(30); // Set reset time for normal block
			}
		}
		else {
			storedReleaseDamage[playerIndex] = currentReleaseDamage;
			storedDT[playerIndex] = currentDT;
			normalBlocked[playerIndex] = false;
		}


		// UNLIKELY TO FIRE
// 		if (actorData.royalBlock == 1) {
// 			if (!guardBroke[playerIndex]) {
// 				storedDT[playerIndex] = std::max(storedDT[playerIndex] - 1000, 0.0f);
// 				storedReleaseDamage[playerIndex] = std::min(storedReleaseDamage[playerIndex] + 700, 9000.0f);
// 				currentReleaseDamage = storedReleaseDamage[playerIndex];
// 				if (!activeCrimsonGameplay.Cheats.Training.infiniteDT) {
// 					currentDT = storedDT[playerIndex];
// 				}
// 				guardBroke[playerIndex] = true;
// 				uint8 vfxColor[4] = { 48, 0, 10, 255 };
//                 uint32 actualColor = CrimsonUtil::Uint8toAABBGGRR(vfxColor);
// 				CrimsonDetours::CreateEffectDetour(actorBaseAddr, 3, 61, 15, true, actualColor, 1.3f);
// 				blockResetTimes[playerIndex] = high_resolution_clock::now() + milliseconds(30); // Set reset time for guard break
// 			}
// 		}
// 		else {
// 			storedReleaseDamage[playerIndex] = currentReleaseDamage;
// 			storedDT[playerIndex] = currentDT;
// 			guardBroke[playerIndex] = false;
// 		}

		// Reset royalBlock to 6 if the timer has passed for both normal blocks and guard breaks
		if ((inNormalBlock || actorData.royalBlock == 1) && high_resolution_clock::now() >= blockResetTimes[playerIndex]) {
			actorData.royalBlock = 6; // Reset to neutral guard
		}
	}
}


void CalculateRotationTowardsEnemy(byte8* actorBaseAddr) {
	if (!actorBaseAddr || (uintptr_t)actorBaseAddr >= 0xFFFFFFFFFF) {
		return;
	}

	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    if(actorData.character != CHARACTER::DANTE && actorData.character != CHARACTER::VERGIL) return;
	auto playerIndex = actorData.newPlayerIndex;
	auto& rotationTowardsEnemy = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].rotationTowardsEnemy : crimsonPlayer[playerIndex].rotationCloneTowardsEnemy;

	uint16 currentRotation = actorData.rotation;
	uint16 rotationUncalculated = actorData.lockOnData.rotationTowardsTarget;
	uint16 rotationOffset = actorData.rotationOffset;
	double rotationOffsetInverted = static_cast<double>(rotationOffset) * -1.0;
	double rotationPreppedForCalc = 0.0;
	uint16 finalRotationTowardsEnemy = 0;

	// Calculating rotation difference
	double rotationDifference = static_cast<double>(rotationUncalculated) - static_cast<double>(currentRotation);
	double rotationDifferenceInverted = rotationDifference * -1.0;

	// Correcting the logic
	if (rotationDifference <= 0) {
		rotationPreppedForCalc = (rotationDifferenceInverted <= rotationOffset) ? rotationDifferenceInverted : rotationOffsetInverted;
	}
	else {
		rotationPreppedForCalc = (rotationDifference <= rotationOffset) ? rotationDifference : static_cast<double>(rotationOffset);
	}

	// Start calculations
	rotationPreppedForCalc *= 360.0;
	rotationPreppedForCalc *= 0.00001525878906; // normalize to range 0.0 - 0.1
	rotationPreppedForCalc *= static_cast<double>(actorData.speed);
	rotationPreppedForCalc *= 65536.0;
	rotationPreppedForCalc /= 360.0;
	rotationPreppedForCalc += 0.5;

	finalRotationTowardsEnemy = static_cast<uint16>(rotationUncalculated);
	rotationTowardsEnemy = finalRotationTowardsEnemy;
}

void GetLockedOnEnemyHitPoints(byte8* actorBaseAddr) {
	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    if (actorData.character != CHARACTER::DANTE && actorData.character != CHARACTER::VERGIL) return;
	auto playerIndex = actorData.newPlayerIndex;
	auto& lockedOnEnemyHP = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].lockedOnEnemyHP : crimsonPlayer[playerIndex].lockedOnEnemyHPClone;
	auto& lockedOnEnemyMaxHP = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].lockedOnEnemyMaxHP : crimsonPlayer[playerIndex].lockedOnEnemyMaxHPClone;

	if (actorData.lockOnData.targetBaseAddr60 != 0) {
		auto& enemyActorData = *reinterpret_cast<EnemyActorData*>(actorData.lockOnData.targetBaseAddr60 - 0x60); // -0x60 very important don't forget
		auto& enemyId = enemyActorData.enemy;
		bool isHell = (enemyId >= ENEMY::PRIDE_1 && enemyId <= ENEMY::HELL_VANGUARD);
		bool isChess = (enemyId >= ENEMY::DAMNED_CHESSMEN_PAWN && enemyId <= ENEMY::DAMNED_CHESSMEN_KING);
		bool isAgniAndRudra = (enemyId >= ENEMY::AGNI_RUDRA_ALL && enemyId <= ENEMY::AGNI_RUDRA_BLUE);

        if (isHell) {
            lockedOnEnemyHP = enemyActorData.hitPointsHells;
            lockedOnEnemyMaxHP = enemyActorData.maxHitPointsHells;
        } else if (enemyId == ENEMY::ARACHNE) {
            lockedOnEnemyHP = enemyActorData.hitPointsArachne;
            lockedOnEnemyMaxHP = enemyActorData.maxHitPointsArachne;
		} else if (enemyId == ENEMY::THE_FALLEN) {
			lockedOnEnemyHP = enemyActorData.hitPointsTheFallen;
			lockedOnEnemyMaxHP = enemyActorData.maxHitPointsTheFallen;
		} else if (enemyId == ENEMY::DULLAHAN) {
			lockedOnEnemyHP = enemyActorData.hitPointsDullahan;
			lockedOnEnemyMaxHP = enemyActorData.maxHitPointsDullahan;
        } else if (enemyId == ENEMY::ENIGMA) {
            lockedOnEnemyHP = enemyActorData.hitPointsEnigma;
            lockedOnEnemyMaxHP = enemyActorData.maxHitPointsEnigma;
		} else if (enemyId == ENEMY::BLOOD_GOYLE) {
			lockedOnEnemyHP = enemyActorData.hitPointsBloodgoyle;
			lockedOnEnemyMaxHP = enemyActorData.maxHitPointsBloodgoyle;
		} else if (enemyId == ENEMY::SOUL_EATER) {
			lockedOnEnemyHP = enemyActorData.hitPointsSoulEater;
			lockedOnEnemyMaxHP = enemyActorData.maxHitPointsSoulEater;
		} else if (isChess) {
			lockedOnEnemyHP = enemyActorData.hitPointsChess;
			lockedOnEnemyMaxHP = enemyActorData.maxHitPointsChess;
        } else if (enemyId == ENEMY::GIGAPEDE) {
            if (enemyActorData.gigapedePartAddr == 0) {
                lockedOnEnemyHP = enemyActorData.hitPointsGigapede;
                lockedOnEnemyMaxHP = enemyActorData.maxHitPointsGigapede;
            } else {
                auto& currentHitPointsGigapede = *reinterpret_cast<float*>(enemyActorData.gigapedePartAddr + 0x9BC0);
                auto& maxHitPointsGigapede = *reinterpret_cast<float*>(enemyActorData.gigapedePartAddr + 0x95E4);
                lockedOnEnemyHP = currentHitPointsGigapede;
                lockedOnEnemyMaxHP = maxHitPointsGigapede;
            }
		} else if (enemyId == ENEMY::CERBERUS) {
			auto& maxHitPointsCerberusRed = *reinterpret_cast<float*>(appBaseAddr + 0x5728F4); // 1650
			auto& maxHitPointsCerberusPart1 = *reinterpret_cast<float*>(appBaseAddr + 0x5728F0); // 2400
			auto& maxHitPointsCerberusGreen = *reinterpret_cast<float*>(appBaseAddr + 0x5728F8); // 1600
			auto& maxHitPointsCerberusBlue = *reinterpret_cast<float*>(appBaseAddr + 0x5728FC); // 1550
			float totalMaxHP = maxHitPointsCerberusRed + maxHitPointsCerberusPart1 + maxHitPointsCerberusRed + maxHitPointsCerberusRed; // 7200
			lockedOnEnemyHP = enemyActorData.hitPointsCerberusTotal;
			lockedOnEnemyMaxHP = totalMaxHP;
            
		} else if (isAgniAndRudra) {
			lockedOnEnemyHP = enemyActorData.hitPointsAgniRudra;
			lockedOnEnemyMaxHP = enemyActorData.maxHitPointsAgniRudra;
		} else if (enemyId == ENEMY::NEVAN) {
			lockedOnEnemyHP = enemyActorData.hitPointsNevan;
			lockedOnEnemyMaxHP = enemyActorData.maxHitPointsNevan;
          
        } else if (enemyId == ENEMY::GERYON) {
            // aiming at carriage
            if (enemyActorData.hitPointsGeryonCarriage != 0) {
				lockedOnEnemyHP = enemyActorData.hitPointsGeryonCarriage;
				lockedOnEnemyMaxHP = enemyActorData.maxHitPointsGeryonCarriage;
            } else {
				lockedOnEnemyHP = enemyActorData.hitPointsGeryon;
				lockedOnEnemyMaxHP = enemyActorData.maxHitPointsGeryon;
            }
        } else if (enemyId == ENEMY::BEOWULF) {
			lockedOnEnemyHP = enemyActorData.hitPointsBeowulf;
			lockedOnEnemyMaxHP = enemyActorData.maxHitPointsBeowulf;
		} else if (enemyId == ENEMY::DOPPELGANGER) {
			lockedOnEnemyHP = enemyActorData.hitPointsDoppelganger;
			lockedOnEnemyMaxHP = enemyActorData.maxHitPointsDoppelganger;
        } else if (enemyId == ENEMY::ARKHAM) {
			lockedOnEnemyHP = enemyActorData.hitPointsArkham;
			lockedOnEnemyMaxHP = enemyActorData.maxHitPointsArkham;
		} else if (enemyId == ENEMY::ARKHAM_LEECHES) {
			lockedOnEnemyHP = enemyActorData.hitPointsArkhamLeech;
			lockedOnEnemyMaxHP = enemyActorData.maxHitPointsArkhamLeech;
		} else if (enemyId == ENEMY::LADY) {
			lockedOnEnemyHP = enemyActorData.hitPointsLady;
			lockedOnEnemyMaxHP = enemyActorData.maxHitPointsLady;
		} else if (enemyId == ENEMY::VERGIL) {
			lockedOnEnemyHP = enemyActorData.hitPointsVergil;
			lockedOnEnemyMaxHP = enemyActorData.maxHitPointsVergil;
		} else if (enemyId == ENEMY::JESTER) {
			lockedOnEnemyHP = enemyActorData.hitPointsJester;
			lockedOnEnemyMaxHP = enemyActorData.maxHitPointsJester;
		} else if (enemyId == ENEMY::LEVIATHAN_HEART) {
			lockedOnEnemyHP = enemyActorData.hitPointsLeviathan;
			lockedOnEnemyMaxHP = enemyActorData.maxHitPointsLeviathan;

        } else {
			lockedOnEnemyHP = 1000.0f;
            lockedOnEnemyMaxHP = 1000.0f;
        }
	} else {
        lockedOnEnemyHP = 0;
        lockedOnEnemyMaxHP = 0;
	}
}

void GetLockedOnEnemyStunDisplacement(byte8* actorBaseAddr) {
	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    if (actorData.character != CHARACTER::DANTE && actorData.character != CHARACTER::VERGIL) return;
	auto playerIndex = actorData.newPlayerIndex;
	auto& lockedOnEnemyStun = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].lockedOnEnemyStun : crimsonPlayer[playerIndex].lockedOnEnemyStunClone;
	auto& lockedOnEnemyDisplacement = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].lockedOnEnemyDisplacement : crimsonPlayer[playerIndex].lockedOnEnemyDisplacementClone;
	auto& lockedOnEnemyMaxStun = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].lockedOnEnemyMaxStun : crimsonPlayer[playerIndex].lockedOnEnemyMaxStunClone;
	auto& lockedOnEnemyMaxDisplacement = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].lockedOnEnemyMaxDisplacement : crimsonPlayer[playerIndex].lockedOnEnemyMaxDisplacementClone;

	// Set base values assuming no valid enemy is locked on
	lockedOnEnemyStun = lockedOnEnemyMaxStun;
	lockedOnEnemyDisplacement = lockedOnEnemyMaxDisplacement;

	if (actorData.lockOnData.targetBaseAddr60 == 0) {
		return;
	}

	// Adjust pointer back to get correct enemy base
	uintptr_t targetAddr = static_cast<uintptr_t>((uintptr_t)actorData.lockOnData.targetBaseAddr60 - 0x60);
	if (targetAddr == 0 || targetAddr == 0xFFFFFFFFFFFFFFA0 || targetAddr < 0x10000) {
		return;
	}

	auto* enemyActorData = reinterpret_cast<EnemyActorData*>(targetAddr);
	if (!enemyActorData) {
		return;
	}

	// Validate stun/displacement data pointer
	uintptr_t stunDisplacementDataAddr = reinterpret_cast<uintptr_t>(enemyActorData->stunDisplacementDataAddr);
	if (!enemyActorData->stunDisplacementDataAddr ||
		stunDisplacementDataAddr == 0xFFFFFFFFFFFFFFFF ||
		stunDisplacementDataAddr == 0xFFFFFFFFFFFFFFEF ||
		stunDisplacementDataAddr == 0xFFFFFFFF ||
		stunDisplacementDataAddr < 0x10000) {
		return;
	}

	StunDisplacementData* stunDisplacementData = reinterpret_cast<StunDisplacementData*>(stunDisplacementDataAddr);
	if (!stunDisplacementData) {
		return;
	}

	auto& enemyId = enemyActorData->enemy;
	bool isHell = (enemyId >= ENEMY::PRIDE_1 && enemyId <= ENEMY::HELL_VANGUARD);

	// Assign max values based on specific enemy type
	if (enemyId >= ENEMY::PRIDE_1 && enemyId <= ENEMY::PRIDE_2) {
		lockedOnEnemyMaxStun = 30.0f;
		lockedOnEnemyMaxDisplacement = 30.0f;
	} else if (enemyId >= ENEMY::GLUTTONY_1 && enemyId <= ENEMY::GLUTTONY_4) {
		lockedOnEnemyMaxStun = 20.0f;
		lockedOnEnemyMaxDisplacement = 60.0f;
	} else if (enemyId >= ENEMY::LUST_1 && enemyId <= ENEMY::LUST_4) {
		lockedOnEnemyMaxStun = 60.0f;
		lockedOnEnemyMaxDisplacement = 60.0f;
	} else if (enemyId >= ENEMY::SLOTH_1 && enemyId <= ENEMY::SLOTH_4) {
		lockedOnEnemyMaxStun = 60.0f;
		lockedOnEnemyMaxDisplacement = 60.0f;
	} else if (enemyId >= ENEMY::WRATH_1 && enemyId <= ENEMY::WRATH_4) {
		lockedOnEnemyMaxStun = 100000.0f;
		lockedOnEnemyMaxDisplacement = 100000.0f;
	} else if (enemyId >= ENEMY::GREED_1 && enemyId <= ENEMY::GREED_4) {
		lockedOnEnemyMaxStun = 100000.0f;
		lockedOnEnemyMaxDisplacement = 100000.0f;
	} else if (enemyId == ENEMY::ABYSS) {
		lockedOnEnemyMaxStun = 60.0f;
		lockedOnEnemyMaxDisplacement = 60.0f;
	} else if (enemyId == ENEMY::ENVY) {
		lockedOnEnemyMaxStun = 60.0f;
		lockedOnEnemyMaxDisplacement = 300.0f;
	} else if (enemyId == ENEMY::HELL_VANGUARD) {
		lockedOnEnemyMaxStun = 300.0f;
		lockedOnEnemyMaxDisplacement = 1000.0f;
	} else {
		lockedOnEnemyMaxStun = 1.0f;
		lockedOnEnemyMaxDisplacement = 1.0f;
	}

	// Apply updated values
	lockedOnEnemyStun = lockedOnEnemyMaxStun;
	lockedOnEnemyDisplacement = lockedOnEnemyMaxDisplacement;

	// If enemy is a "Hell" type, attempt to read additional data
	if (isHell) {
		void* stunDisplacementHellsPtr = nullptr;

		// Try to access the pointer safely
		__try {
			stunDisplacementHellsPtr = stunDisplacementData->stunDisplacementHells;
		} __except (EXCEPTION_EXECUTE_HANDLER) {
			stunDisplacementHellsPtr = nullptr;
		}

		uintptr_t ptrValue = reinterpret_cast<uintptr_t>(stunDisplacementHellsPtr);
		if (stunDisplacementHellsPtr &&
			ptrValue != 0xFFFFFFFFFFFFFFFF &&
			ptrValue != 0xFFFFFFFFFFFFFFEF &&
			ptrValue > 0x10000) {

			// Try reading actual stun/displacement values
			__try {
				auto* safeHellsPtr = static_cast<decltype(stunDisplacementData->stunDisplacementHells)>(stunDisplacementHellsPtr);
				lockedOnEnemyStun = safeHellsPtr->stun;
				lockedOnEnemyDisplacement = safeHellsPtr->displacement;
			} __except (EXCEPTION_EXECUTE_HANDLER) {
				// If this fails, fallback to earlier values
			}
		}
	}
}

void CalculateLockedOnEnemyLastStunDisplacementValue(byte8* actorBaseAddr) {
	if (!actorBaseAddr) return;

	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
	auto playerIndex = actorData.newPlayerIndex;
	uint32_t entityIdx = actorData.newEntityIndex;

    if (actorData.character != CHARACTER::DANTE && actorData.character != CHARACTER::VERGIL) return;

	auto& lockedOnEnemyStun = (entityIdx == 0) ? crimsonPlayer[playerIndex].lockedOnEnemyStun : crimsonPlayer[playerIndex].lockedOnEnemyStunClone;
	auto& lockedOnEnemyDisplacement = (entityIdx == 0) ? crimsonPlayer[playerIndex].lockedOnEnemyDisplacement : crimsonPlayer[playerIndex].lockedOnEnemyDisplacementClone;
	auto& lockedOnEnemyMinusStun = (entityIdx == 0) ? crimsonPlayer[playerIndex].lockedOnEnemyMinusStun : crimsonPlayer[playerIndex].lockedOnEnemyMinusStunClone;
	auto& lockedOnEnemyMinusDisplacement = (entityIdx == 0) ? crimsonPlayer[playerIndex].lockedOnEnemyMinusDisplacement : crimsonPlayer[playerIndex].lockedOnEnemyMinusDisplacementClone;

	static float lastStun[4][2] = {};
	static float lastDisplacement[4][2] = {};
	static bool firstFrame[4][2] = { true };
	static std::chrono::steady_clock::time_point lastCheck[4][2] = {};

	auto now = std::chrono::steady_clock::now();
	if (now - lastCheck[playerIndex][entityIdx] < std::chrono::milliseconds(200)) {
		return;
	}
	lastCheck[playerIndex][entityIdx] = now;

	float currentStun = lockedOnEnemyStun;
	float currentDisplacement = lockedOnEnemyDisplacement;

	float stunDiff = lastStun[playerIndex][entityIdx] - currentStun;
	float displacementDiff = lastDisplacement[playerIndex][entityIdx] - currentDisplacement;

	if (stunDiff > 0.0f) {
		lockedOnEnemyMinusStun = stunDiff;
	}
	if (displacementDiff > 0.0f) {
		lockedOnEnemyMinusDisplacement = displacementDiff;
	}

	lastStun[playerIndex][entityIdx] = currentStun;
	lastDisplacement[playerIndex][entityIdx] = currentDisplacement;
}

void GetLockedOnEnemyShield(byte8* actorBaseAddr) {
	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
	auto playerIndex = actorData.newPlayerIndex;
	auto& lockedOnEnemyShield = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].lockedOnEnemyShield : crimsonPlayer[playerIndex].lockedOnEnemyShieldClone;
	auto& lockedOnEnemyMaxShield = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].lockedOnEnemyMaxShield : crimsonPlayer[playerIndex].lockedOnEnemyMaxShieldClone;

    if (actorData.character != CHARACTER::DANTE && actorData.character != CHARACTER::VERGIL) return;

	if (actorData.lockOnData.targetBaseAddr60 != 0) {
		auto& enemyActorData = *reinterpret_cast<EnemyActorData*>(actorData.lockOnData.targetBaseAddr60 - 0x60); // -0x60 very important don't forget
		auto& enemyId = enemyActorData.enemy;

        if (enemyId == ENEMY::CERBERUS) {
			
			auto& maxHitPointsCerberusPart1 = *reinterpret_cast<float*>(appBaseAddr + 0x5728F0);
			lockedOnEnemyShield = enemyActorData.hitPointsCerberusPart1;
			lockedOnEnemyMaxShield = maxHitPointsCerberusPart1;
			
		} else if (enemyId == ENEMY::NEVAN) {
			auto& shieldedNevanData = *reinterpret_cast<ShieldedNevanData*>(enemyActorData.shieldedNevanAddr);
			if (!enemyActorData.shieldedNevanAddr) {
				lockedOnEnemyShield = 0;
				lockedOnEnemyMaxShield = 0;
			} else {
				lockedOnEnemyShield = shieldedNevanData.currentShield;
				lockedOnEnemyMaxShield = 350.0f;
			}

		} else {
            lockedOnEnemyShield = 0;
			lockedOnEnemyMaxShield = 0;
		}
	} else {
		lockedOnEnemyShield = 0;
		lockedOnEnemyMaxShield = 0;
	}
}

void DetectCloseToEnemy(byte8* actorBaseAddr) {
	if (!actorBaseAddr || (uintptr_t)actorBaseAddr >= 0xFFFFFFFFFF) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
	if (actorData.character != CHARACTER::DANTE && actorData.character != CHARACTER::VERGIL) return;
	auto playerIndex = actorData.newPlayerIndex;
	auto& closeToEnemy = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].isCloseToEnemy : crimsonPlayer[playerIndex].isCloseToEnemyClone;
	auto pool_2128 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_2128 || !pool_2128[8]) {
		closeToEnemy = false;
		return;
	}
	auto& enemyVectorData = *reinterpret_cast<EnemyVectorData*>(pool_2128[8]);

	bool foundCloseEnemy = false;

	// Loop through enemy data
	for (auto enemy : enemyVectorData.metadata) {
		if (!enemy.baseAddr) continue;
		auto& enemyData = *reinterpret_cast<EnemyActorData*>(enemy.baseAddr);
		if (!enemyData.baseAddr) continue;

		glm::vec3 playerPos(actorData.position.x, actorData.position.y, actorData.position.z);
		glm::vec3 enemyPos(enemyData.position.x, enemyData.position.y, enemyData.position.z);

		float distanceToPlayer = glm::distance(playerPos, enemyPos);

		if (distanceToPlayer <= 150.0f) {
			foundCloseEnemy = true;
			break;
		}
	}

	closeToEnemy = foundCloseEnemy;
}

#pragma endregion

#pragma region DanteAirTaunt


void TrackRoyalReleaseAndSkyLaunch(byte8* actorBaseAddr) {
	if (!actorBaseAddr || (uintptr_t)actorBaseAddr >= 0xFFFFFFFFFF) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    if (actorData.character != CHARACTER::DANTE) return;
	auto playerIndex = actorData.newPlayerIndex;
	auto& royalRelease = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].royalRelease : crimsonPlayer[playerIndex].royalReleaseClone;
	auto& skyLaunch = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].skyLaunch : crimsonPlayer[playerIndex].skyLaunchClone;

	bool isAirReleaseAction = (actorData.action == 195 || actorData.action == 194 || actorData.action == 196 || actorData.action == 197);
	bool isSkyLaunchAction = (actorData.action == 195 || actorData.action == 194 || actorData.action == 212);
	bool isInAir = (actorData.state & STATE::IN_AIR);
	bool isCorrectMotionIndex = (actorData.motionData[0].index == 20 || actorData.motionData[0].index == 19);
	bool isSkyLaunchButtonPressed = (actorData.buttons[0] & GetBinding(BINDING::TAUNT));
	bool isRoyalReleaseButtonPressed = (actorData.buttons[0] & GetBinding(BINDING::STYLE_ACTION));

	// Sky Launch conditions
	if (isInAir && isCorrectMotionIndex && isSkyLaunchAction && isSkyLaunchButtonPressed && !skyLaunch.trackerRunning && !royalRelease.executing) {
		skyLaunch.executing = true;
		skyLaunch.trackerRunning = true;
	}

	// Royal Release conditions
	if (isAirReleaseAction && isCorrectMotionIndex && isRoyalReleaseButtonPressed && !royalRelease.trackerRunning && !skyLaunch.executing) {
		royalRelease.executing = true;
		royalRelease.trackerRunning = true;
	}

	// Reset Royal Release if conditions are not met
	if (!(isAirReleaseAction && isCorrectMotionIndex)) {
		royalRelease.executing = false;
		royalRelease.trackerRunning = false;
	}

	// Reset Sky Launch if conditions are not met
	if (!(isSkyLaunchAction && isCorrectMotionIndex)) {
		skyLaunch.executing = false;
		skyLaunch.trackerRunning = false;
	}

	// Ensure Royal Release is false if Sky Launch is active
	if (skyLaunch.executing) {
		royalRelease.executing = false;
	}

	// Ensure Sky Launch is false if Royal Release is active
	if (royalRelease.executing) {
		skyLaunch.executing = false;
	}
}

void CheckRoyalReleaseAndSkyLaunch(byte8* actorBaseAddr) {
	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);

	// Centralized tracking function to handle both states
	TrackRoyalReleaseAndSkyLaunch(actorData);
}


void StoreSkyLaunchProperties(PlayerActorData& actorData) {
    auto playerIndex = actorData.newPlayerIndex;
    auto& skyLaunch = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].skyLaunch : crimsonPlayer[playerIndex].skyLaunchClone;
    auto& event = actorData.eventData[0].event;
    auto& state = actorData.state;

    if (!(actorData.action == 195 && event == ACTOR_EVENT::ATTACK && state & STATE::IN_AIR)) {

        skyLaunch.storedPos.x = actorData.position.x;
        skyLaunch.storedPos.y = actorData.position.y;
        skyLaunch.storedPos.z = actorData.position.z;
        skyLaunch.storedRank = actorData.styleData.rank;
        skyLaunch.storedReleaseLevel = actorData.royalguardReleaseLevel;
        skyLaunch.storedDevilTrigger = actorData.magicPoints;
        skyLaunch.appliedProperties = false;
    }
}


void ApplySkyLaunchProperties(byte8* actorBaseAddr) {
    if (!actorBaseAddr || (uintptr_t)actorBaseAddr >= 0xFFFFFFFFFF) {
        return;
    }
    auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    if (actorData.character != CHARACTER::DANTE && actorData.character != CHARACTER::VERGIL) return;
    auto playerIndex = actorData.newPlayerIndex;
    auto& action      = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].action : crimsonPlayer[playerIndex].actionClone;
    auto& state       = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].state : crimsonPlayer[playerIndex].stateClone;
	auto& royalRelease = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].royalRelease : crimsonPlayer[playerIndex].royalReleaseClone;
	auto& skyLaunch = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].skyLaunch : crimsonPlayer[playerIndex].skyLaunchClone;
    static vec3 storedSkyLaunchPos[PLAYER_COUNT];
 

    if (skyLaunch.executing) {

        actorData.position.x     = skyLaunch.storedPos.x;
        actorData.position.z     = skyLaunch.storedPos.z;
        actorData.styleData.rank = skyLaunch.storedRank;
        actorData.magicPoints = skyLaunch.storedDevilTrigger;

        if (!skyLaunch.setVolume) {
            SetVolume(2, 0);
            skyLaunch.setVolume = true;
        }

        if (!skyLaunch.appliedProperties) {
            skyLaunch.forceJustFrameReleaseToggledOff = false;
            
            actorData.royalguardReleaseLevel = skyLaunch.storedReleaseLevel; // Important for RG Gauge Restoration at KillRGConsumpDetour2
            
            skyLaunch.appliedProperties = true;
        }

        actorData.horizontalPull         = 0;
        actorData.verticalPullMultiplier = -0.2f;

		actorData.position.x = skyLaunch.storedPos.x;
		actorData.position.z = skyLaunch.storedPos.z;

		actorData.position.x = skyLaunch.storedPos.x;
		actorData.position.z = skyLaunch.storedPos.z;

    } else {
        StoreSkyLaunchProperties(actorData);
        if (!skyLaunch.forceJustFrameReleaseToggledOff) {

            SetVolume(2, activeCrimsonConfig.Sound.channelVolumes[2] / 100.0f);
            //CrimsonPatches::ToggleRoyalguardForceJustFrameRelease(activeConfig.Royalguard.forceJustFrameRelease);
            skyLaunch.forceJustFrameReleaseToggledOff = true;
        }
        skyLaunch.setVolume = false;
    }

}

void SkyLaunchAirTauntController(byte8* actorBaseAddr) {
    CheckRoyalReleaseAndSkyLaunch(actorBaseAddr);
	ApplySkyLaunchProperties(actorBaseAddr);
}

#pragma endregion

#pragma region DanteGameplay

void ToggleRebellionHoldDrive(bool enable) {
	LogFunction(enable);

	static bool run = false;

	{
		auto addr = (appBaseAddr + 0x211581);
		auto dest = (appBaseAddr + 0x211583);
		constexpr new_size_t size = 2;
		/*
		dmc3.exe+211581 - 74 15             - je dmc3.exe+211598
		dmc3.exe+211583 - 80 BE 133E0000 00 - cmp byte ptr [rsi+00003E13],00
		*/

		if (!run) {
			backupHelper.Save(addr, size);
		}

		if (enable) {
			WriteAddress(addr, dest, size);
		}
		else {
			backupHelper.Restore(addr);
		}
	}

	run = true;
}

void DanteDriveRework(byte8* actorBaseAddr) {
    // This function alters some of Drive, it alters its damage to accommodate new "Charge Levels", mimicing DMC4/5 Drive behaviour.

    using namespace ACTION_DANTE;

    if (!actorBaseAddr) {
        return;
    }
    auto& actorData  = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    CrimsonDetours::ToggleDisableDriveHold(activeCrimsonGameplay.Gameplay.Dante.driveRework);
    if (!activeCrimsonGameplay.Gameplay.Dante.driveRework || actorData.character != CHARACTER::DANTE) return;
    auto playerIndex = actorData.newPlayerIndex;
	auto entityIndex = actorData.newEntityIndex;
	auto& drive = (entityIndex == 0) ?
		crimsonPlayer[playerIndex].drive :
		crimsonPlayer[playerIndex].driveClone;
    int vfxBank = 3;
    int vfxId = 144;

    // 	drive physical hit damage dmc3.exe + 5C6D2C, 70.0f
    // 	drive projectile damage dmc3.exe + 5CB1EC, 300.0f
    uintptr_t drivePhysicalDamageAddr   = (uintptr_t)appBaseAddr + 0x5C6D2C;
    uintptr_t driveProjectileDamageAddr = (uintptr_t)appBaseAddr + 0x5CB1EC;
	auto& motionTimer = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].animTimer : crimsonPlayer[playerIndex].animTimerClone;
	auto gamepad = GetGamepad(playerIndex);

	static constexpr const wchar_t* driveChargeParticlePath = L"Crimson\\vfx\\drive_charge.efkefc";
	static EffekseerRefHandle driveChargeParticleRef = CrimsonEfk::LoadEffect(driveChargeParticlePath, 1.0f);
	static constexpr const wchar_t* quickDriveChargeParticlePath = L"Crimson\\vfx\\quickdrive_charge.efkefc";
	static EffekseerRefHandle quickDriveChargeParticleRef = CrimsonEfk::LoadEffect(quickDriveChargeParticlePath, 1.0f);
	static constexpr const wchar_t* driveLevel1ParticlePath = L"Crimson\\vfx\\drive_level1.efkefc";
	static EffekseerRefHandle driveLevel1ParticleRef = CrimsonEfk::LoadEffect(driveLevel1ParticlePath, 1.0f);
	static constexpr const wchar_t* driveLevel2ParticlePath = L"Crimson\\vfx\\drive_level2.efkefc";
	static EffekseerRefHandle driveLevel2ParticleRef = CrimsonEfk::LoadEffect(driveLevel2ParticlePath, 1.0f);
	static constexpr const wchar_t* driveLevel3ParticlePath = L"Crimson\\vfx\\drive_level3.efkefc";
	static EffekseerRefHandle driveLevel3ParticleRef = CrimsonEfk::LoadEffect(driveLevel3ParticlePath, 1.0f);
	bool meleeDown = (gamepad.buttons[0] & GetBinding(BINDING::MELEE_ATTACK)) != 0;

	auto& danteSword = *reinterpret_cast<Sword*>(actorData.nextBaseAddr);
	cDrawReverse* danteSwordcDraw = reinterpret_cast<cDrawReverse*>(danteSword.swordcDraw);
	Matrix44* swordMatrix = reinterpret_cast<Matrix44*>(danteSwordcDraw[0].bones); // index 1 is the hilt

	static bool prevMeleeButton[PLAYER_COUNT][ENTITY_COUNT] = {};
	bool meleePressed = meleeDown && !prevMeleeButton[playerIndex][entityIndex];
	prevMeleeButton[playerIndex][entityIndex] = meleeDown;


	// Reset motion19Timer when entering motion 19
	if (actorData.motionData[0].index == 19 && !drive.resetMotion19Timer) {
		drive.motion19Timer = 0.0f;
		drive.resetMotion19Timer = true;
	}

	// Increment timer while in motion 19
	if (actorData.motionData[0].index == 19 && actorData.action == REBELLION_DRIVE_1) {
		drive.motion19Timer += ImGui::GetIO().DeltaTime * (actorData.speed / g_FrameRateTimeMultiplier);
	}


    // Triggering the Drive Level Timer
    if ((actorData.action == REBELLION_DRIVE_1) &&
        (actorData.motionData[0].index == 17 || actorData.motionData[0].index == 18)) {
        drive.runLevelTimer = true;
	} else if (actorData.action != REBELLION_DRIVE_1 || actorData.motionData[0].index == 19) {
		drive.runLevelTimer = false;
	}

	// Reset the charge Effect bool
    if (actorData.action != REBELLION_DRIVE_1) {
		drive.chargeEffectPlayed = false;
		drive.level1EffectPlayed = false;
		drive.level2EffectPlayed = false;
		drive.level3EffectPlayed = false;
		drive.inPart2 = false;
		drive.inPart3 = false;
    }

	// Incrementing level timer
	if (drive.runLevelTimer) {
		drive.levelTimer += ImGui::GetIO().DeltaTime * (actorData.speed / g_FrameRateTimeMultiplier);
	}
	else {
		drive.levelTimer = 0.0f;
	}


	// Interrupting Charge Effect
	if ((actorData.action == REBELLION_DRIVE_1 &&
		actorData.motionData[0].index == 19 && drive.motion19Timer >= drive.effectInterruptTime &&
		!drive.inQuickDrive) || actorData.action != REBELLION_DRIVE_1 || 
		actorData.eventData[0].event != ACTOR_EVENT::ATTACK) {

		CrimsonEfk::StopEffect(drive.chargeEffectHandle);
	}

	// Interrupting SFX. What a pain to get right as well.
	if ((actorData.motionData[0].index != 17
		&& actorData.motionData[0].index != 18 && ((drive.motion19Timer >= (drive.effectInterruptTime - 0.1f)) ||
			(actorData.action != REBELLION_DRIVE_1 && drive.part3Played))) ||
		actorData.motionData[0].index == 1 || actorData.motionData[0].index == 2) {
		CrimsonSDL::InterruptDriveSFX(playerIndex, entityIndex);
	}

	// Buffering input for Overdrive
	if (actorData.action == REBELLION_DRIVE_1 &&
		actorData.motionData[0].index == 19 &&
		!drive.inQuickDrive && 
		motionTimer >= 0.25f &&
		motionTimer <= 0.59f) {

		if (meleePressed) {
			drive.meleePressedForOverdrive = true;
		}
	}

	// Triggering Overdrive (Drive Part 2 & Part 3)
	if (actorData.action == REBELLION_DRIVE_1 &&
		actorData.motionData[0].index == 19 && 
		!drive.inQuickDrive) {
	

		if (drive.meleePressedForOverdrive && !drive.part2Played && motionTimer >= 0.55f && motionTimer <= 0.79f) {
			CrimsonGameplay::ToggleRebellionHoldDrive(false);
			drive.resetMotion19Timer = false;
			actorData.motionArchives[MOTION_GROUP_DANTE::REBELLION] = newDrivePart2_pl000_00_3;
			actorData.action = REBELLION_DRIVE_1;
			func_1E0800_TriggerEvent(actorBaseAddr, 17, 0, 0);
			actorData.rotation = GetRotationTowardsEnemy(actorData);
			drive.inPart2 = true;
			drive.part2Played = true;
			
		}
		else if (drive.part2Played && !drive.part3Played && motionTimer >= 0.41f) {
			drive.effectInterruptTime = 1.4f;
			CrimsonGameplay::ToggleRebellionHoldDrive(false);
			drive.resetMotion19Timer = false;
			actorData.motionArchives[MOTION_GROUP_DANTE::REBELLION] = newDrivePart3_pl000_00_3;
			actorData.action = REBELLION_DRIVE_1;
			func_1E0800_TriggerEvent(actorBaseAddr, 17, 0, 0);
			actorData.rotation = GetRotationTowardsEnemy(actorData);
			drive.inPart3 = true;
			drive.part3Played = true;
		}
	}

	if (actorData.action == 1) {
		drive.quickDriveEffectPlayed = false;
	}

    // Setting Quick Drive Damage
    if ((actorData.action == REBELLION_DRIVE_1) && drive.inQuickDrive && actorData.eventData[0].event == 17) {

        *(float*)(drivePhysicalDamageAddr)   = 60.0f;
        *(float*)(driveProjectileDamageAddr) = 200.0f;

		drive.chargeEffectPlayed = false;

		if (!drive.quickDriveEffectPlayed) {
			
			quickDriveChargeParticleRef = CrimsonEfk::ReloadEffect(quickDriveChargeParticleRef, quickDriveChargeParticlePath, 1.0f);
			drive.quickDriveChargeEffectHandle = CrimsonEfk::PlayEffectAtMatrix(quickDriveChargeParticleRef, swordMatrix[0].matrix2, actorData); // using sword bone 2

			CrimsonSDL::PlayJDCCharge(playerIndex); // Charge sound

			drive.quickDriveEffectPlayed = true;
		}
    }

    // Regular Drive Logic Starts here
    if ((actorData.action == REBELLION_DRIVE_1) && !drive.inQuickDrive && actorData.eventData[0].event == 17) {

		if (!drive.chargeEffectPlayed) {
			drive.resetMotion19Timer = false;
			drive.meleePressedForOverdrive = false;
			drive.part2Played = false;
			drive.part3Played = false;
			drive.effectInterruptTime = 1.1f;
			drive.sfxLooped = false;

			driveChargeParticleRef = CrimsonEfk::ReloadEffect(driveChargeParticleRef, driveChargeParticlePath, 1.0f);
			drive.chargeEffectHandle = CrimsonEfk::PlayEffectAtMatrix(driveChargeParticleRef, swordMatrix[0].matrix2, actorData); // using sword bone 2
			
			CrimsonSDL::PlayDriveStart(playerIndex, entityIndex); // Charge sound

			drive.chargeEffectPlayed = true;
		}

		if (!CrimsonSDL::DriveStartIsPlaying(playerIndex, entityIndex) && drive.chargeEffectPlayed && !drive.sfxLooped) {
			CrimsonSDL::PlayDriveLoop(playerIndex, entityIndex);
			drive.sfxLooped = true;
		}
		

        uint32 vfxColor = CrimsonUtil::Uint8toAABBGGRR(activeCrimsonConfig.StyleSwitchFX.Flux.color[0]);


//         if (drive.levelTimer >= 1.1f) {
//             if (!drive.level1EffectPlayed) {
// 
//                 //CrimsonDetours::CreateEffectDetour(actorBaseAddr, vfxBank, vfxId, 1, true, vfxColor, 0.8f);
// 				driveLevel1ParticleRef = CrimsonEfk::ReloadEffect(driveLevel1ParticleRef, driveLevel1ParticlePath, 1.0f);
// 				drive.level1EffectHandle = CrimsonEfk::PlayEffectAtMatrix(driveLevel1ParticleRef, swordMatrix[0].matrix2, actorData); // using sword bone 2
// 				CrimsonSDL::PlayDriveLevelUp(playerIndex, entityIndex); // Level up sound
// 
//                 drive.level1EffectPlayed = true;
//             }
//         }

        if (drive.levelTimer < 2.0) {
            *(float*)(drivePhysicalDamageAddr)   = 70.0f;
            *(float*)(driveProjectileDamageAddr) = 200.0f;
        }

        if (drive.levelTimer >= 2.0 && drive.levelTimer < 5.0) {
            *(float*)(drivePhysicalDamageAddr)   = 70.0f;
            *(float*)(driveProjectileDamageAddr) = 300.0f;

            if (!drive.level2EffectPlayed) {

                //CrimsonDetours::CreateEffectDetour(actorBaseAddr, vfxBank, vfxId, 1,true, vfxColor, 0.8f);
				driveLevel2ParticleRef = CrimsonEfk::ReloadEffect(driveLevel2ParticleRef, driveLevel2ParticlePath, 1.0f);
				drive.level2EffectHandle = CrimsonEfk::PlayEffectAtMatrix(driveLevel2ParticleRef, swordMatrix[0].matrix2, actorData); // using sword bone 2
				CrimsonSDL::PlayDriveLevelUp(playerIndex, entityIndex);
                drive.level2EffectPlayed = true;
            }
        }

        if (drive.levelTimer >= 5.0) {
            *(float*)(drivePhysicalDamageAddr)   = 70.0f;
            *(float*)(driveProjectileDamageAddr) = 700.0f;

            if (!drive.level3EffectPlayed) {

                //CrimsonDetours::CreateEffectDetour(actorBaseAddr, vfxBank, vfxId, 1,true, vfxColor, 0.8f);
				driveLevel3ParticleRef = CrimsonEfk::ReloadEffect(driveLevel3ParticleRef, driveLevel3ParticlePath, 1.0f);
				drive.level3EffectHandle = CrimsonEfk::PlayEffectAtMatrix(driveLevel3ParticleRef, swordMatrix[0].matrix2, actorData); // using sword bone 2
				CrimsonSDL::PlayDriveLevelUp(playerIndex, entityIndex);

                drive.level3EffectPlayed = true;
            }
        }
    }
}

void GroundTrickFlagSet(byte8* actorBaseAddr) {
    // Works in tandem with DanteTrickAlterations Detour in CrimsonDetours (requirement)
	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
	if (!(activeCrimsonGameplay.Gameplay.General.extramoves && ExpConfig::missionExpDataDante.unlocks[UNLOCK_DANTE::TRICKSTER_MODDED_MOVES])) return;
	auto playerIndex = actorData.newPlayerIndex;
	auto& b2F = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].b2F : crimsonPlayer[playerIndex].b2FClone;
	auto& playerData = GetPlayerData(playerIndex);
	auto entityIndex = actorData.newEntityIndex; 
    auto& newActorData = GetNewActorData(playerIndex, playerData.activeCharacterIndex, entityIndex);
	
	if (actorData.character != CHARACTER::DANTE) return;

    if (actorData.eventData[0].event == ACTOR_EVENT::TRICKSTER_AIR_TRICK && b2F.forwardCommand) {
        actorData.eventData[0].event = ACTOR_EVENT::TRICKSTER_GROUND_TRICK; // set g. trick flag for the detour
        newActorData.visibility = 2; // hide dante's model
    }

    if (actorData.eventData[0].event == ACTOR_EVENT::TRICKSTER_GROUND_TRICK) { // guarantee attack buffer will come through
        auto& policyMelee = actorData.nextActionRequestPolicy[NEXT_ACTION_REQUEST_POLICY::MELEE_ATTACK];
		auto& policySword = actorData.nextActionRequestPolicy[NEXT_ACTION_REQUEST_POLICY::SWORDMASTER_GUNSLINGER];
        policyMelee = NEXT_ACTION_REQUEST_POLICY::BUFFER;
		policySword = NEXT_ACTION_REQUEST_POLICY::BUFFER; 
        //actorData.state &= ~STATE::BUSY;
    }

    if (actorData.eventData[0].event == ACTOR_EVENT::LANDING) {
        actorData.verticalPull = -150.0f;
		auto& policyMelee = actorData.nextActionRequestPolicy[NEXT_ACTION_REQUEST_POLICY::MELEE_ATTACK];
		auto& policySword = actorData.nextActionRequestPolicy[NEXT_ACTION_REQUEST_POLICY::SWORDMASTER_GUNSLINGER];
		policyMelee = NEXT_ACTION_REQUEST_POLICY::BUFFER;
		policySword = NEXT_ACTION_REQUEST_POLICY::BUFFER;

        if (actorData.state & STATE::ON_FLOOR) {
			policyMelee = NEXT_ACTION_REQUEST_POLICY::EXECUTE;
			policySword = NEXT_ACTION_REQUEST_POLICY::EXECUTE;
			actorData.permissions = 3080; // This is a softer version of Reset Permissions.
			actorData.state &= ~STATE::BUSY;
        }
    }



    if (actorData.newEntityIndex == 1 && !actorData.doppelganger) return; // visibility set with doppel fix

    if ((actorData.eventData[0].event == ACTOR_EVENT::LANDING || actorData.eventData[0].event == ACTOR_EVENT::STAGGER ||
        actorData.eventData[0].event == ACTOR_EVENT::ATTACK)
        && newActorData.visibility == 2) {
        newActorData.visibility = 0; // unhide
    }
}

}
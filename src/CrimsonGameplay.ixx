module;
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

export module CrimsonGameplay;

import Core;

#include "Core/Macros.h"


import Windows;

using namespace Windows;

import ActorBase;
import ActorRelocations;
import Config;
import DetourFunctions;
import Exp;
import File;
import Global;
import HUD;
import Internal;
import Input;
import Memory;
import Model;
import PatchFunctions;
import SoundRelocations;
import Sound;
import Speed;
import Vars;
import ExtraSound;


#pragma region Cancels

void RoyalCancelCountsTracker(byte8* actorBaseAddr) {
	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);

	// This restores player counts back to what they were before the Royal Cancel
	royalCancelTrackerRunning = true;
	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	actorData.newTrickUpCount = storedTrickUpCount;
	actorData.newSkyStarCount = storedSkyStarCount;
	actorData.newAirHikeCount = storedAirHikeCount;

	royalCancelTrackerRunning = false;
}

export void ImprovedCancelsRoyalguardController(byte8* actorBaseAddr) {
	using namespace ACTION_DANTE;
	using namespace ACTION_VERGIL;

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


	bool inCancellableActionRebellion = (actorData.action == REBELLION_COMBO_1_PART_1 || actorData.action == REBELLION_COMBO_1_PART_1 ||
		actorData.action == REBELLION_COMBO_1_PART_1 || actorData.action == REBELLION_COMBO_1_PART_2 ||
		actorData.action == REBELLION_COMBO_1_PART_3 || actorData.action == REBELLION_COMBO_2_PART_2 ||
		actorData.action == REBELLION_COMBO_2_PART_3 || actorData.action == REBELLION_PROP ||
		actorData.action == REBELLION_SHREDDER || actorData.action == REBELLION_DRIVE_1 || actorData.action == REBELLION_DRIVE_2 ||
		actorData.action == REBELLION_MILLION_STAB || actorData.action == REBELLION_DANCE_MACABRE_PART_1 || actorData.action == REBELLION_DANCE_MACABRE_PART_2 ||
		actorData.action == REBELLION_DANCE_MACABRE_PART_3 || actorData.action == REBELLION_DANCE_MACABRE_PART_4 ||
		actorData.action == REBELLION_DANCE_MACABRE_PART_5 || actorData.action == REBELLION_DANCE_MACABRE_PART_6 ||
		actorData.action == REBELLION_DANCE_MACABRE_PART_7 || actorData.action == REBELLION_DANCE_MACABRE_PART_8 ||
		actorData.action == REBELLION_CRAZY_DANCE || actorData.action == POLE_PLAY);

	bool inCancellableActionCerberus = (actorData.action == CERBERUS_COMBO_1_PART_1 || actorData.action == CERBERUS_COMBO_1_PART_2 ||
		actorData.action == CERBERUS_COMBO_1_PART_3 || actorData.action == CERBERUS_COMBO_1_PART_4 ||
		actorData.action == CERBERUS_COMBO_1_PART_5 || actorData.action == CERBERUS_COMBO_2_PART_3 ||
		actorData.action == CERBERUS_COMBO_2_PART_4 || actorData.action == CERBERUS_WINDMILL ||
		actorData.action == CERBERUS_REVOLVER_LEVEL_1 || actorData.action == CERBERUS_REVOLVER_LEVEL_2 ||
		actorData.action == CERBERUS_SWING || actorData.action == CERBERUS_SATELLITE ||
		actorData.action == CERBERUS_FLICKER || actorData.action == CERBERUS_CRYSTAL ||
		actorData.action == CERBERUS_MILLION_CARATS || actorData.action == CERBERUS_ICE_AGE);

	bool inCancellableActionAgni = (actorData.action == AGNI_RUDRA_COMBO_1_PART_1 || actorData.action == AGNI_RUDRA_COMBO_1_PART_2 ||
		actorData.action == AGNI_RUDRA_COMBO_1_PART_3 || actorData.action == AGNI_RUDRA_COMBO_1_PART_4 ||
		actorData.action == AGNI_RUDRA_COMBO_1_PART_5 || actorData.action == AGNI_RUDRA_COMBO_2_PART_2 ||
		actorData.action == AGNI_RUDRA_COMBO_2_PART_3 || actorData.action == AGNI_RUDRA_COMBO_3_PART_3 ||
		actorData.action == AGNI_RUDRA_JET_STREAM_LEVEL_1 || actorData.action == AGNI_RUDRA_JET_STREAM_LEVEL_2 ||
		actorData.action == AGNI_RUDRA_JET_STREAM_LEVEL_3 || actorData.action == AGNI_RUDRA_MILLION_SLASH ||
		actorData.action == AGNI_RUDRA_TWISTER || actorData.action == AGNI_RUDRA_TEMPEST);


	bool inCancellableActionNevan = (actorData.action == NEVAN_TUNE_UP || actorData.action == NEVAN_COMBO_1 ||
		actorData.action == NEVAN_COMBO_2 || actorData.action == NEVAN_JAM_SESSION ||
		actorData.action == NEVAN_BAT_RIFT_LEVEL_1 || actorData.action == NEVAN_BAT_RIFT_LEVEL_2 ||
		actorData.action == NEVAN_REVERB_SHOCK_LEVEL_1 || actorData.action == NEVAN_REVERB_SHOCK_LEVEL_2 ||
		actorData.action == NEVAN_SLASH || actorData.action == NEVAN_FEEDBACK ||
		actorData.action == NEVAN_CRAZY_ROLL || actorData.action == NEVAN_DISTORTION);

	bool inCancellableActionBeowulf = (actorData.action == BEOWULF_COMBO_1_PART_1 || actorData.action == BEOWULF_COMBO_1_PART_2 ||
		actorData.action == BEOWULF_COMBO_1_PART_3 || actorData.action == BEOWULF_COMBO_2_PART_3 ||
		actorData.action == BEOWULF_COMBO_2_PART_4 || actorData.action == BEOWULF_BEAST_UPPERCUT ||
		actorData.action == BEOWULF_HYPER_FIST);

	bool inCancellableActionGuns = (actorData.action == EBONY_IVORY_WILD_STOMP || actorData.action == ARTEMIS_ACID_RAIN
		|| actorData.action == KALINA_ANN_GRAPPLE);

	// These are moves used by the Action Set Cancel Method, generally air ones.
	bool inCancellableMovesActionMethod = (((actorData.action == REBELLION_AERIAL_RAVE_PART_1 ||
		actorData.action == REBELLION_AERIAL_RAVE_PART_2 ||
		actorData.action == REBELLION_AERIAL_RAVE_PART_3 ||
		actorData.action == REBELLION_AERIAL_RAVE_PART_4) || (actorData.action == AGNI_RUDRA_SKY_DANCE_PART_1 ||
			actorData.action == AGNI_RUDRA_SKY_DANCE_PART_2 ||
			actorData.action == AGNI_RUDRA_SKY_DANCE_PART_3) || (actorData.action == NEVAN_AIR_SLASH_PART_1 ||
				actorData.action == NEVAN_AIR_SLASH_PART_2) || (actorData.action == CERBERUS_AIR_FLICKER) || (actorData.action == BEOWULF_TORNADO) ||
		(actorData.action == CERBERUS_REVOLVER_LEVEL_1) || (actorData.action == CERBERUS_REVOLVER_LEVEL_2)) &&
		actorData.eventData[0].event == 17);




	//Royalguard Cancels Everything (Most things)
	if (
		(actorData.style == STYLE::ROYALGUARD) &&
		(actorData.buttons[2] & GetBinding(BINDING::STYLE_ACTION)) && actorData.eventData[0].event != 44 && (inCancellableActionRebellion || inCancellableActionCerberus ||
			inCancellableActionAgni || inCancellableActionNevan || inCancellableActionBeowulf || inCancellableActionGuns || actorData.eventData[0].event == 22) && !royalCancelTrackerRunning) // The last condition prevents cancelling recovery
	{

		// Old list of exceptions, easier to list everything that should be cancellable.

		/*/if (actorData.action != SPIRAL_NORMAL_SHOT && actorData.action != KALINA_ANN_NORMAL_SHOT &&
		actorData.action != EBONY_IVORY_AIR_NORMAL_SHOT && actorData.action != SHOTGUN_AIR_NORMAL_SHOT &&
		actorData.action != SPIRAL_TRICK_SHOT && !royalCancelTrackerRunning) // Exceptions, these cancels are way too OP or buggy in the cases of E&I and Shotgun.*/




		storedTrickUpCount = actorData.newTrickUpCount;
		storedSkyStarCount = actorData.newSkyStarCount;
		storedAirHikeCount = actorData.newAirHikeCount;

		actorData.permissions = 3080; // This is a softer version of Reset Permissions.

		std::thread royalcountstracker(RoyalCancelCountsTracker, actorBaseAddr);
		royalcountstracker.detach();





	}

	// Royal Cancelling Sky Star
	if (
		(actorData.style == STYLE::ROYALGUARD) &&
		(actorData.buttons[2] & GetBinding(BINDING::STYLE_ACTION)) && actorData.eventData[0].event == 23 && !royalCancelTrackerRunning) {


		storedTrickUpCount = actorData.newTrickUpCount;
		storedSkyStarCount = actorData.newSkyStarCount;
		storedAirHikeCount = actorData.newAirHikeCount;

		actorData.permissions = 0x1C1B; // This is a hard version of Reset Permissions.

		std::thread royalcountstracker(RoyalCancelCountsTracker, actorBaseAddr);
		royalcountstracker.detach();
	}




	// This is another method for Royal Cancels that involves setting the Actor's Action to a newly created Air Block one (only sets for a split second). 
	// It's more reliable for cancelling certain moves (especially air ones). - Mia

	if (actorData.style == STYLE::ROYALGUARD && (actorData.eventData[0].event == 23 || inCancellableMovesActionMethod)) {
		if (inAir) {

			if ((!(lockOn && tiltDirection == TILT_DIRECTION::UP)) && gamepad.buttons[0] & GetBinding(BINDING::STYLE_ACTION)) {

				storedTrickUpCount = actorData.newTrickUpCount;
				storedSkyStarCount = actorData.newSkyStarCount;
				storedAirHikeCount = actorData.newAirHikeCount;

				actorData.action = ROYAL_AIR_BLOCK;

				std::thread royalcountstracker(RoyalCancelCountsTracker, actorBaseAddr);
				royalcountstracker.detach();
			}

		}
		/*else {
			if((!(lockOn && tiltDirection == TILT_DIRECTION::UP)) && (!(lockOn && tiltDirection == TILT_DIRECTION::DOWN) && gamepad.buttons[0] & GetBinding(BINDING::STYLE_ACTION)))
			{
				actorData.action = ROYAL_BLOCK;
			}

		}*/

	}



	/*if (actorData.buttons[2] & GetBinding(BINDING::TAUNT))  // old ddmk Reset Permissions -- Deprecated.
	{
		actorData.permissions = 0x1C1B;
	}*/
}




export void ImprovedCancelsDanteController(byte8* actorBaseAddr) {
	using namespace ACTION_DANTE;

	/*if (
		!actorBaseAddr ||
		(actorBaseAddr == g_playerActorBaseAddrs[0]) ||
		(actorBaseAddr == g_playerActorBaseAddrs[1]))
	{
		return;
	}*/

	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
	auto lockOn = (actorData.buttons[0] & GetBinding(BINDING::LOCK_ON));
	auto tiltDirection = GetRelativeTiltDirection(actorData);


	bool inCancellableActionRebellion = (actorData.action == REBELLION_COMBO_1_PART_1 || actorData.action == REBELLION_COMBO_1_PART_1 ||
		actorData.action == REBELLION_COMBO_1_PART_1 || actorData.action == REBELLION_COMBO_1_PART_2 ||
		actorData.action == REBELLION_COMBO_1_PART_3 || actorData.action == REBELLION_COMBO_2_PART_2 ||
		actorData.action == REBELLION_COMBO_2_PART_3 || actorData.action == REBELLION_PROP ||
		actorData.action == REBELLION_SHREDDER || actorData.action == REBELLION_DRIVE_1 || actorData.action == REBELLION_DRIVE_2 ||
		actorData.action == REBELLION_MILLION_STAB || actorData.action == REBELLION_DANCE_MACABRE_PART_1 || actorData.action == REBELLION_DANCE_MACABRE_PART_2 ||
		actorData.action == REBELLION_DANCE_MACABRE_PART_3 || actorData.action == REBELLION_DANCE_MACABRE_PART_4 ||
		actorData.action == REBELLION_DANCE_MACABRE_PART_5 || actorData.action == REBELLION_DANCE_MACABRE_PART_6 ||
		actorData.action == REBELLION_DANCE_MACABRE_PART_7 || actorData.action == REBELLION_DANCE_MACABRE_PART_8 ||
		actorData.action == REBELLION_CRAZY_DANCE || actorData.action == POLE_PLAY || actorData.action == REBELLION_SWORD_PIERCE);

	bool inCancellableActionCerberus = (actorData.action == CERBERUS_COMBO_1_PART_1 || actorData.action == CERBERUS_COMBO_1_PART_2 ||
		actorData.action == CERBERUS_COMBO_1_PART_3 || actorData.action == CERBERUS_COMBO_1_PART_4 ||
		actorData.action == CERBERUS_COMBO_1_PART_5 || actorData.action == CERBERUS_COMBO_2_PART_3 ||
		actorData.action == CERBERUS_COMBO_2_PART_4 || actorData.action == CERBERUS_WINDMILL ||
		actorData.action == CERBERUS_REVOLVER_LEVEL_1 || actorData.action == CERBERUS_REVOLVER_LEVEL_2 ||
		actorData.action == CERBERUS_SWING || actorData.action == CERBERUS_SATELLITE ||
		actorData.action == CERBERUS_FLICKER || actorData.action == CERBERUS_CRYSTAL ||
		actorData.action == CERBERUS_MILLION_CARATS || actorData.action == CERBERUS_ICE_AGE);

	bool inCancellableActionAgni = (actorData.action == AGNI_RUDRA_COMBO_1_PART_1 || actorData.action == AGNI_RUDRA_COMBO_1_PART_2 ||
		actorData.action == AGNI_RUDRA_COMBO_1_PART_3 || actorData.action == AGNI_RUDRA_COMBO_1_PART_4 ||
		actorData.action == AGNI_RUDRA_COMBO_1_PART_5 || actorData.action == AGNI_RUDRA_COMBO_2_PART_2 ||
		actorData.action == AGNI_RUDRA_COMBO_2_PART_3 || actorData.action == AGNI_RUDRA_COMBO_3_PART_3 ||
		actorData.action == AGNI_RUDRA_JET_STREAM_LEVEL_1 || actorData.action == AGNI_RUDRA_JET_STREAM_LEVEL_2 ||
		actorData.action == AGNI_RUDRA_JET_STREAM_LEVEL_3 || actorData.action == AGNI_RUDRA_MILLION_SLASH ||
		actorData.action == AGNI_RUDRA_TWISTER || actorData.action == AGNI_RUDRA_TEMPEST || actorData.action == AGNI_RUDRA_WHIRLWIND_LAUNCH);

	// || actorData.action == AGNI_RUDRA_CROSSED_SWORDS || actorData.action == AGNI_RUDRA_CRAWLER

	bool inCancellableActionNevan = (actorData.action == NEVAN_TUNE_UP || actorData.action == NEVAN_COMBO_1 ||
		actorData.action == NEVAN_COMBO_2 || actorData.action == NEVAN_JAM_SESSION ||
		actorData.action == NEVAN_BAT_RIFT_LEVEL_1 || actorData.action == NEVAN_BAT_RIFT_LEVEL_2 ||
		actorData.action == NEVAN_REVERB_SHOCK_LEVEL_1 || actorData.action == NEVAN_REVERB_SHOCK_LEVEL_2 ||
		actorData.action == NEVAN_SLASH || actorData.action == NEVAN_FEEDBACK ||
		actorData.action == NEVAN_CRAZY_ROLL || actorData.action == NEVAN_DISTORTION);

	bool inCancellableActionBeowulf = (actorData.action == BEOWULF_COMBO_1_PART_1 || actorData.action == BEOWULF_COMBO_1_PART_2 ||
		actorData.action == BEOWULF_COMBO_1_PART_3 || actorData.action == BEOWULF_COMBO_2_PART_3 ||
		actorData.action == BEOWULF_COMBO_2_PART_4 || actorData.action == BEOWULF_BEAST_UPPERCUT ||
		actorData.action == BEOWULF_HYPER_FIST || actorData.action == BEOWULF_TORNADO);

	bool inCancellableActionGuns = (actorData.action == EBONY_IVORY_WILD_STOMP || actorData.action == ARTEMIS_ACID_RAIN ||
		actorData.action == KALINA_ANN_GRAPPLE);

	bool inCancellableActionAirSwordmaster = ((actorData.action == REBELLION_AERIAL_RAVE_PART_1 ||
		actorData.action == REBELLION_AERIAL_RAVE_PART_2 ||
		actorData.action == REBELLION_AERIAL_RAVE_PART_3 ||
		actorData.action == REBELLION_AERIAL_RAVE_PART_4) || (actorData.action == AGNI_RUDRA_SKY_DANCE_PART_1 ||
			actorData.action == AGNI_RUDRA_SKY_DANCE_PART_2 ||
			actorData.action == AGNI_RUDRA_SKY_DANCE_PART_3) || (actorData.action == NEVAN_AIR_SLASH_PART_1 ||
				actorData.action == NEVAN_AIR_SLASH_PART_2) || (actorData.action == CERBERUS_AIR_FLICKER) || (actorData.action == BEOWULF_TORNADO));

	bool inCancellableActionAirGunslinger = (actorData.action == SHOTGUN_AIR_FIREWORKS ||
		actorData.action == ARTEMIS_AIR_NORMAL_SHOT || actorData.action == ARTEMIS_AIR_MULTI_LOCK_SHOT);



	auto playerIndex = actorData.newPlayerIndex;
	if (playerIndex >= PLAYER_COUNT) {
		playerIndex = 0;
	}

	auto characterIndex = actorData.newCharacterIndex;
	if (characterIndex >= CHARACTER_COUNT) {
		characterIndex = 0;
	}

	auto entityIndex = actorData.newEntityIndex;
	if (entityIndex >= ENTITY_COUNT) {
		entityIndex = 0;
	}

	auto& playerData = GetPlayerData(playerIndex);

	auto& gamepad = GetGamepad(playerIndex);

	static bool executes[PLAYER_COUNT][CHARACTER_COUNT][ENTITY_COUNT][4] = {};

	if (actorData.character == CHARACTER::DANTE) {

		//Dante's Trickster Actions Cancels Most Things (w/ cooldown)
		if ((actorData.style == STYLE::TRICKSTER) &&
			(actorData.eventData[0].event != 22 && (inCancellableActionRebellion || inCancellableActionCerberus ||
				inCancellableActionAgni || inCancellableActionNevan || inCancellableActionBeowulf || inCancellableActionGuns ||
				inCancellableActionAirSwordmaster || inCancellableActionAirGunslinger || actorData.action == EBONY_IVORY_RAIN_STORM) || executingSkyLaunch)) {
			if (gamepad.buttons[0] & GetBinding(BINDING::STYLE_ACTION)) {


				if (actorData.newEntityIndex == ENTITY::MAIN) {
					if (crimsonPlayer[playerIndex].cancels.canTrick) {

						actorData.state &= ~STATE::BUSY;

						crimsonPlayer[playerIndex].cancels.canTrick = false;
					}

				}
				else {
					if (crimsonPlayer[playerIndex].cancelsClone.canTrick) {

						actorData.state &= ~STATE::BUSY;

						crimsonPlayer[playerIndex].cancelsClone.canTrick = false;
					}
				}



			}

		}

		//Gunslinger Cancels Most Things (w/ cooldown)
		// They can also cancel themselves.
		if ((actorData.style == STYLE::GUNSLINGER) &&
			(actorData.state == STATE::IN_AIR || actorData.state == 65538) && (inCancellableActionAirSwordmaster ||
				inCancellableActionAirGunslinger || actorData.eventData[0].event == 23 || actorData.eventData[0].event == ACTOR_EVENT::TRICKSTER_AIR_TRICK ||
				actorData.motionData[0].index == 15) && actorData.action != EBONY_IVORY_RAIN_STORM) {
			if (gamepad.buttons[0] & GetBinding(BINDING::STYLE_ACTION)) {

				if (actorData.newEntityIndex == ENTITY::MAIN) {

					if (crimsonPlayer[playerIndex].cancels.canGun) {

						actorData.state &= ~STATE::BUSY;

						crimsonPlayer[playerIndex].cancels.canGun = false;
					}

				}
				else {
					if (crimsonPlayer[playerIndex].cancelsClone.canGun) {

						actorData.state &= ~STATE::BUSY;

						crimsonPlayer[playerIndex].cancelsClone.canGun = false;
					}
				}


			}

		}

		// but Rainstorm is an exception here since I wanted it to have a longer CD.
		if ((actorData.style == STYLE::GUNSLINGER) &&
			(actorData.state == STATE::IN_AIR || actorData.state == 65538) && (crimsonPlayer[playerIndex].cancels.canRainstorm) &&
			(actorData.action == EBONY_IVORY_RAIN_STORM)) {
			if (actorData.buttons[2] & GetBinding(BINDING::STYLE_ACTION)) {



				if (actorData.newEntityIndex == ENTITY::MAIN) {

					if (crimsonPlayer[playerIndex].cancels.canRainstorm) {

						actorData.state &= ~STATE::BUSY;

						crimsonPlayer[playerIndex].cancels.canRainstorm = false;
					}

				}
				else {
					if (crimsonPlayer[playerIndex].cancelsClone.canRainstorm) {

						actorData.state &= ~STATE::BUSY;

						crimsonPlayer[playerIndex].cancelsClone.canRainstorm = false;
					}
				}
			}

		}

		// This prevents the double Rainstorm from happening (but I still left it on Fireworks and Artemis Shots).
		if (actorData.action == EBONY_IVORY_RAIN_STORM && actorData.motionData[0].index == 15) {

			if (actorData.newEntityIndex == ENTITY::MAIN) {

				if (crimsonPlayer[playerIndex].cancels.canRainstorm) {

					crimsonPlayer[playerIndex].cancels.canRainstorm = false;
				}

			}
			else {
				if (crimsonPlayer[playerIndex].cancelsClone.canRainstorm) {

					crimsonPlayer[playerIndex].cancelsClone.canRainstorm = false;
				}
			}


		}
	}

	old_for_all(uint8, buttonIndex, 4) {
		auto& execute = executes[playerIndex][characterIndex][entityIndex][buttonIndex];

		auto& button = playerData.removeBusyFlagButtons[buttonIndex];


		if (actorData.character == CHARACTER::DANTE) {


			// Air Revolver Cancelling with Swordmaster moves
			if ((actorData.style == STYLE::SWORDMASTER) && (actorData.action == CERBERUS_REVOLVER_LEVEL_1 || actorData.action == CERBERUS_REVOLVER_LEVEL_2)) {
				if (actorData.buttons[2] & GetBinding(BINDING::STYLE_ACTION)) {

					if (execute) {
						execute = false;

						actorData.state &= ~STATE::BUSY;

					}
				}
				else {
					execute = true;
				}
			}

			if ((actorData.action == CERBERUS_REVOLVER_LEVEL_1 || actorData.action == CERBERUS_REVOLVER_LEVEL_2) &&
				actorData.state & STATE::IN_AIR) {
				if (actorData.buttons[2] & GetBinding(BINDING::MELEE_ATTACK)) {
					if ((lockOn && tiltDirection != TILT_DIRECTION::UP) || !lockOn) {
						if (execute) {
							execute = false;

							actorData.state &= ~STATE::BUSY;

						}
					}


				}
				else {
					execute = true;
				}
			}

			// Swordmaster moves cancel out Trickster dashes
			uint32 eventActor = actorData.eventData[0].event;

			if ((actorData.style == STYLE::SWORDMASTER) &&
				(eventActor == ACTOR_EVENT::TRICKSTER_SKY_STAR || eventActor == ACTOR_EVENT::TRICKSTER_DASH)) {
				if (actorData.buttons[2] & GetBinding(BINDING::STYLE_ACTION)) {

					if (execute) {
						execute = false;

						actorData.state &= ~STATE::BUSY;

					}
				}
				else {
					execute = true;
				}
			}

			// Cancel Final Aerial Rave Knockback with Gunshot.
			if (actorData.action == REBELLION_AERIAL_RAVE_PART_4 && actorData.eventData[0].event == 17) {
				if (actorData.buttons[2] & GetBinding(BINDING::SHOOT)) {

					if (execute) {
						execute = false;

						actorData.state &= ~STATE::BUSY;

					}
				}
				else {
					execute = true;
				}
			}


			/*if ((actorData.style == STYLE::ROYALGUARD) &&
					(actorData.buttons[2] & GetBinding(BINDING::STYLE_ACTION)))
			{
				if(actorData.action != SPIRAL_NORMAL_SHOT && actorData.action != KALINA_ANN_NORMAL_SHOT &&
				actorData.action != EBONY_IVORY_AIR_NORMAL_SHOT && actorData.action != SHOTGUN_AIR_NORMAL_SHOT) { // Exceptions, these cancels are way too OP or buggy in the cases of E&I and Shotgun.
					if (execute)
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




export void ImprovedCancelsVergilController(byte8* actorBaseAddr) {
	using namespace ACTION_VERGIL;

	if (
		!actorBaseAddr ||
		(actorBaseAddr == g_playerActorBaseAddrs[0]) ||
		(actorBaseAddr == g_playerActorBaseAddrs[1])) {
		return;
	}

	if (!actorBaseAddr) {
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
	if (characterIndex >= CHARACTER_COUNT) {
		characterIndex = 0;
	}

	auto entityIndex = actorData.newEntityIndex;
	if (entityIndex >= ENTITY_COUNT) {
		entityIndex = 0;
	}

	auto& playerData = GetPlayerData(playerIndex);

	auto& gamepad = GetGamepad(playerIndex);

	static bool executes[PLAYER_COUNT][CHARACTER_COUNT][ENTITY_COUNT][4] = {};


	old_for_all(uint8, buttonIndex, 4) {
		auto& execute = executes[playerIndex][characterIndex][entityIndex][buttonIndex];

		auto& button = playerData.removeBusyFlagButtons[buttonIndex];

		//Darkslayer Trick Cancels Everything
		if (actorData.character == CHARACTER::VERGIL && actorData.state != STATE::IN_AIR && actorData.state != 65538) {
			if (gamepad.buttons[0] & GetBinding(BINDING::STYLE_ACTION)) {
				if (execute) {
					execute = false;

					actorData.state &= ~STATE::BUSY;
				}
			}
			else {
				execute = true;
			}
		}

		// TRICK UP
		if (actorData.character == CHARACTER::VERGIL && actorData.state & STATE::IN_AIR) {
			if (gamepad.buttons[0] & GetBinding(BINDING::STYLE_ACTION) && lockOn && tiltDirection == TILT_DIRECTION::UP && actorData.trickUpCount > 0) {
				if (execute) {
					execute = false;

					actorData.state &= ~STATE::BUSY;
				}
			}
			else {
				execute = true;
			}
		}

		// TRICK DOWN
		if (actorData.character == CHARACTER::VERGIL && actorData.state & STATE::IN_AIR) {
			if (gamepad.buttons[0] & GetBinding(BINDING::STYLE_ACTION) && lockOn && tiltDirection == TILT_DIRECTION::DOWN && actorData.trickDownCount > 0) {
				if (execute) {
					execute = false;

					actorData.state &= ~STATE::BUSY;
				}
			}
			else {
				execute = true;
			}
		}

		// AIR TRICK
		if (actorData.character == CHARACTER::VERGIL && actorData.state & STATE::IN_AIR) {
			if (gamepad.buttons[0] & GetBinding(BINDING::STYLE_ACTION) && (lockOn && tiltDirection == TILT_DIRECTION::NEUTRAL || !lockOn) && actorData.airTrickCount > 0) {
				if (execute) {
					execute = false;

					actorData.state &= ~STATE::BUSY;
				}
			}
			else {
				execute = true;
			}
		}

		/*if(actorData.character == CHARACTER::VERGIL && actorData.state & STATE::IN_AIR) {
			if(actorData.action == YAMATO_FORCE_EDGE_STINGER_LEVEL_2 && airStingerEnd.timer < 150) {
				if (execute)
				{
					execute = false;

					actorData.state &= ~STATE::BUSY;
				}


			}
			else
			{
			execute = true;
			}

		}*/

	}
}

#pragma endregion

#pragma region VergilImprovements


export void CalculateAirStingerEndTime() {
	using namespace ACTION_DANTE;
	using namespace ACTION_VERGIL;

	auto pool_6046 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if
		(
			!pool_6046 ||
			!pool_6046[3]
			) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(pool_6046[3]);

	float airStingerEndTime = 100 / actorData.speed;
	airStingerEndTimeInt = (int)airStingerEndTime + 1;


}

void AirStingerEndTracker(byte8* actorBaseAddr) {
	auto speedValue = (IsTurbo()) ? activeConfig.Speed.turbo : activeConfig.Speed.mainSpeed;
	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);

	airStingerEnd.trackerRunning = true;
	airStingerEnd.timer = 0;
	while (actorData.motionData[0].index == 11) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		airStingerEnd.timer++;
	}


	if (actorData.motionData[0].index != 11) {
		airStingerEnd.trackerRunning = false;
	}
}

export void SetAirStingerEnd(byte8* actorBaseAddr) {
	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);

	if (actorData.character == CHARACTER::VERGIL) {
		if (actorData.motionData[0].index == 11 && !airStingerEnd.trackerRunning) {

			std::thread airstingerendtracker(AirStingerEndTracker, actorBaseAddr);
			airstingerendtracker.detach();
		}

		if (actorData.motionData[0].index != 11) {
			airStingerEnd.timer = 0;
		}
	}


}

export void FasterRapidSlashDevil(byte8* actorBaseAddr) {
	using namespace ACTION_DANTE;
	using namespace ACTION_VERGIL;
	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);

	if ((actorData.motionData[0].index == 8 || actorData.motionData[0].index == 10) &&
		(actorData.action == YAMATO_RAPID_SLASH_LEVEL_1 || actorData.action == YAMATO_RAPID_SLASH_LEVEL_2)) {

		inRapidSlash = true;
	}
	else {
		inRapidSlash = false;
	}


	if (actorData.character == CHARACTER::VERGIL) {

		if ((actorData.motionData[0].index == 51 || actorData.motionData[0].index == 2) && !inRapidSlash) { // Coudln't figure out a way to not bug this out then to store this out of walking anim
			// Storing the original speeds
			fasterRapidSlash.storedSpeedDevil[0] = activeConfig.Speed.devilVergil[0];
			fasterRapidSlash.storedSpeedDevil[1] = activeConfig.Speed.devilVergil[1];
			fasterRapidSlash.storedSpeedDevil[2] = activeConfig.Speed.devilVergil[2];
			fasterRapidSlash.storedSpeedDevil[3] = activeConfig.Speed.devilVergil[3];
		}

		if (actorData.devil == 1) {
			if (inRapidSlash && !fasterRapidSlash.newSpeedSet) {


				// Setting the new speed 
				activeConfig.Speed.devilVergil[0] = fasterRapidSlash.newSpeed;
				activeConfig.Speed.devilVergil[1] = fasterRapidSlash.newSpeed;
				activeConfig.Speed.devilVergil[2] = fasterRapidSlash.newSpeed;
				activeConfig.Speed.devilVergil[3] = fasterRapidSlash.newSpeed;

				fasterRapidSlash.newSpeedSet = true;
			}
			else if (!inRapidSlash && fasterRapidSlash.newSpeedSet) {


				// Restoring the original speeds
				activeConfig.Speed.devilVergil[0] = fasterRapidSlash.storedSpeedDevil[0];
				activeConfig.Speed.devilVergil[1] = fasterRapidSlash.storedSpeedDevil[1];
				activeConfig.Speed.devilVergil[2] = fasterRapidSlash.storedSpeedDevil[2];
				activeConfig.Speed.devilVergil[3] = fasterRapidSlash.storedSpeedDevil[3];


				fasterRapidSlash.newSpeedSet = false;
			}
		}


	}
}


export void FasterDarkslayerTricks() {
	auto pool_12311 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if
		(
			!pool_12311 ||
			!pool_12311[3]
			) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(pool_12311[3]);


	if (actorData.character == CHARACTER::VERGIL) {
		float storedspeedVergil = activeConfig.Speed.human;

		if ((actorData.motionData[0].index == 51 || actorData.motionData[0].index == 2) && !(actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_AIR_TRICK || actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_TRICK_UP
			|| actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_TRICK_DOWN)) {
			// Storing the original speeds
			fasterDarkslayer.storedSpeedHuman = activeConfig.Speed.human;
			fasterDarkslayer.storedSpeedDevil[0] = activeConfig.Speed.devilVergil[0];
			fasterDarkslayer.storedSpeedDevil[1] = activeConfig.Speed.devilVergil[1];
			fasterDarkslayer.storedSpeedDevil[2] = activeConfig.Speed.devilVergil[2];
			fasterDarkslayer.storedSpeedDevil[3] = activeConfig.Speed.devilVergil[3];
		}

		if ((actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_AIR_TRICK || actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_TRICK_UP
			|| actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_TRICK_DOWN) && !fasterDarkslayer.newSpeedSet) {



			// Setting the new speed 
			activeConfig.Speed.human = fasterDarkslayer.newSpeed;
			activeConfig.Speed.devilVergil[0] = fasterDarkslayer.newSpeed;
			activeConfig.Speed.devilVergil[1] = fasterDarkslayer.newSpeed;
			activeConfig.Speed.devilVergil[2] = fasterDarkslayer.newSpeed;
			activeConfig.Speed.devilVergil[3] = fasterDarkslayer.newSpeed;

			fasterDarkslayer.newSpeedSet = true;
		}
		else if (!(actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_AIR_TRICK || actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_TRICK_UP
			|| actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_TRICK_DOWN) && fasterDarkslayer.newSpeedSet) {

			// Restoring the original speeds
			activeConfig.Speed.human = fasterDarkslayer.storedSpeedHuman;
			activeConfig.Speed.devilVergil[0] = fasterDarkslayer.storedSpeedDevil[0];
			activeConfig.Speed.devilVergil[1] = fasterDarkslayer.storedSpeedDevil[1];
			activeConfig.Speed.devilVergil[2] = fasterDarkslayer.storedSpeedDevil[2];
			activeConfig.Speed.devilVergil[3] = fasterDarkslayer.storedSpeedDevil[3];


			fasterDarkslayer.newSpeedSet = false;
		}
	}
}

#pragma endregion

#pragma region InertiaThings


export void LastEventStateQueue() {
	//IntroduceMainActorData
	auto pool_12857 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if
		(
			!pool_12857 ||
			!pool_12857[3]
			) {
		return;
	}


	auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_12857[3]);

	auto lastEvent = mainActorData.eventData[0].lastEvent;
	auto lastState = mainActorData.lastState;



	if (lastEvents.back() != lastEvent) {
		lastEvents.push_back(mainActorData.eventData[0].lastEvent);
	}
	if (lastEvents.size() > 2) {
		lastLastEvent = lastEvents.at(lastEvents.size() - 2);
	}

	if (lastEvents.size() > 4) {
		lastEvents.erase(lastEvents.begin());
	}

	if (lastStates.back() != lastState) {
		lastStates.push_back(mainActorData.lastState);
	}
	if (lastStates.size() > 2) {
		lastLastState = lastStates.at(lastStates.size() - 2);
	}

	if (lastStates.size() > 4) {
		lastStates.erase(lastStates.begin());
	}
}

export void RemoveSoftLockOnController(byte8* actorBaseAddr) {
	// Allows you to freely rotate in the air while not locked on with aerial Swordmaster moves.
	// This is important for Inertia (Redirection) and as such both can only be enabled together.

	using namespace ACTION_DANTE;
	using namespace ACTION_VERGIL;
	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
	auto lockOn = (actorData.buttons[0] & GetBinding(BINDING::LOCK_ON));
	auto& gamepad = GetGamepad(0);
	auto tiltDirection = GetRelativeTiltDirection(actorData);
	auto radius = gamepad.leftStickRadius;
	uint16 relativeTilt = 0;
	relativeTilt = (actorData.cameraDirection + gamepad.leftStickPosition);
	uint16 value = (relativeTilt - 0x8000);

	inRoyalBlock = (!(lockOn && tiltDirection == TILT_DIRECTION::UP)) &&
		gamepad.buttons[0] & GetBinding(BINDING::STYLE_ACTION) && (actorData.style == STYLE::ROYALGUARD) &&
		(actorData.state & STATE::IN_AIR);

	if (actorData.character == CHARACTER::DANTE) {

		if (actorData.action == REBELLION_AERIAL_RAVE_PART_1 ||
			actorData.action == REBELLION_AERIAL_RAVE_PART_2 ||
			actorData.action == REBELLION_AERIAL_RAVE_PART_3 ||
			actorData.action == REBELLION_AERIAL_RAVE_PART_4) {


			if (!lockOn) {
				if (!(radius < RIGHT_STICK_DEADZONE)) {

					actorData.rotation = value;
					raveRotation = actorData.rotation;
				}
				else {
					actorData.rotation = raveRotation;
				}
			}

			if (lockOn) {
				if (actorData.eventData[0].event == 33 || actorData.eventData[0].event == 7) {
					actorData.rotation = raveRotation;
				}
			}
		}
		else if (actorData.action == CERBERUS_AIR_FLICKER) {

			if (!lockOn) {
				if (!(radius < RIGHT_STICK_DEADZONE)) {

					actorData.rotation = value;
					airFlickerRotation = actorData.rotation;
				}
				else {
					actorData.rotation = airFlickerRotation;
				}
			}

			if (lockOn) {
				if (actorData.eventData[0].event == 33 || actorData.eventData[0].event == 7) {
					actorData.rotation = airFlickerRotation;
				}
			}
		}
		else if (actorData.action == AGNI_RUDRA_SKY_DANCE_PART_1 ||
			actorData.action == AGNI_RUDRA_SKY_DANCE_PART_2 ||
			actorData.action == AGNI_RUDRA_SKY_DANCE_PART_3) {

			if (!lockOn) {
				if (!(radius < RIGHT_STICK_DEADZONE)) {

					actorData.rotation = value;
					skyDanceRotation = actorData.rotation;
				}
				else {
					actorData.rotation = skyDanceRotation;
				}
			}

			if (lockOn) {
				if (actorData.eventData[0].event == 33 || actorData.eventData[0].event == 7) {
					actorData.rotation = skyDanceRotation;
				}
			}
		}
		else if (actorData.action == NEVAN_AIR_SLASH_PART_1 ||
			actorData.action == NEVAN_AIR_SLASH_PART_2) {

			if (!lockOn) {
				if (!(radius < RIGHT_STICK_DEADZONE)) {

					actorData.rotation = value;
					airSlashRotation = actorData.rotation;
				}
				else {
					actorData.rotation = airSlashRotation;
				}
			}

			if (lockOn) {
				if (actorData.eventData[0].event == 33 || actorData.eventData[0].event == 7) {
					actorData.rotation = airSlashRotation;
				}
			}

		}
		else if (actorData.action == BEOWULF_THE_HAMMER) {

			if (!lockOn) {
				if (!(radius < RIGHT_STICK_DEADZONE)) {

					actorData.rotation = value;
					theHammerRotation = actorData.rotation;
				}
				else {
					actorData.rotation = theHammerRotation;
				}
			}

			if (lockOn) {
				if (actorData.eventData[0].event == 33 || actorData.eventData[0].event == 7) {
					actorData.rotation = theHammerRotation;
				}
			}

		}
		else if (actorData.action == NEVAN_AIR_PLAY) {
			//actorData.horizontalPullMultiplier = 0.2f;
		}
		else if (actorData.action == BEOWULF_KILLER_BEE) {


			// Keep Player's Rotation intact on jump cancelling, this is important for Inertia Redirection and is used for several moves.
			if (lockOn) {
				if (actorData.eventData[0].event == 33) {
					actorData.rotation = killerBeeRotation;
				}
			}
		}
		else if (inRoyalBlock) {

			if (!lockOn) {
				if (!(radius < RIGHT_STICK_DEADZONE)) {

					actorData.rotation = value;
					royalBlockRotation = actorData.rotation;
				}
				else {
					actorData.rotation = royalBlockRotation;
				}
			}

			if (lockOn) {
				if (actorData.eventData[0].event == 33) {
					actorData.rotation = royalBlockRotation;
				}
			}
		}
		else if (actorData.eventData[0].event == 23) {

			/*if(!lockOn) {
				if (radius < RIGHT_STICK_DEADZONE) {
					actorData.rotation = skyStarRotation;
				}
			}*/

			/*if(lockOn) {

				actorData.rotation = skyStarRotation;

			}*/
		}
		else if (actorData.motionData[0].index == 33) {


			if (actorData.eventData[0].event == 33 && (actorData.action == REBELLION_AERIAL_RAVE_PART_1 ||
				actorData.action == REBELLION_AERIAL_RAVE_PART_2 ||
				actorData.action == REBELLION_AERIAL_RAVE_PART_3 ||
				actorData.action == REBELLION_AERIAL_RAVE_PART_4)) {

				actorData.rotation = raveRotation;
			}
			else if (actorData.eventData[0].event == 33 && (actorData.action == AGNI_RUDRA_SKY_DANCE_PART_1 ||
				actorData.action == AGNI_RUDRA_SKY_DANCE_PART_2 ||
				actorData.action == AGNI_RUDRA_SKY_DANCE_PART_3)) {
				actorData.rotation = skyDanceRotation;
			}
			else if (actorData.eventData[0].event == 33 && (actorData.action == CERBERUS_AIR_FLICKER)) {
				actorData.rotation = airFlickerRotation;
			}
			else if (actorData.eventData[0].event == 33 && (actorData.action == NEVAN_AIR_SLASH_PART_1 ||
				actorData.action == NEVAN_AIR_SLASH_PART_2)) {
				actorData.rotation = airSlashRotation;
			}
			else if (actorData.eventData[0].event == 33 && (actorData.action == BEOWULF_THE_HAMMER)) {
				actorData.rotation = airSlashRotation;
			}
			else if (actorData.eventData[0].event == 33 && (actorData.action == BEOWULF_KILLER_BEE)) {
				actorData.rotation = killerBeeRotation;
			}
			else if (actorData.eventData[0].lastEvent == 23) {
				actorData.rotation = skyStarRotation;
			}


		}
		else if (actorData.eventData[0].event == 7) {
			if (lockOn) {

				actorData.rotation = airHikeRotation;

			}
		}



		if (actorData.action != BEOWULF_KILLER_BEE) {

			if (radius < RIGHT_STICK_DEADZONE) {
				killerBeeRotation = actorData.rotation;
			}

		}

		if (actorData.eventData[0].event != 23) {
			if (radius < RIGHT_STICK_DEADZONE) {
				skyStarRotation = actorData.rotation;
			}
		}

		if (actorData.eventData[0].event != 7) {
			if (radius < RIGHT_STICK_DEADZONE) {
				airHikeRotation = actorData.rotation;
			}
		}





	}
	else if (actorData.character == CHARACTER::VERGIL) {

		if (actorData.action == YAMATO_AERIAL_RAVE_PART_1 ||
			actorData.action == YAMATO_AERIAL_RAVE_PART_2) {

			if (!lockOn) {
				if (!(radius < RIGHT_STICK_DEADZONE)) {

					actorData.rotation = value;
					yamatoRaveRotation = actorData.rotation;
				}
				else {
					actorData.rotation = yamatoRaveRotation;
				}
			}

		}
	}

	/*if(!lockOn) {
		if (radius < RIGHT_STICK_DEADZONE) {
			actorData.rotation = jumpCancelRotation;
		}
	}

	if(lockOn) {
		if (actorData.eventData[0].event == 33) {
					actorData.rotation = jumpCancelRotation;
		}
	}*/

}



export void StoreInertia(byte8* actorBaseAddr) {

	using namespace ACTION_DANTE;
	using namespace ACTION_VERGIL;

	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
	auto lockOn = (actorData.buttons[0] & GetBinding(BINDING::LOCK_ON));
	auto& gamepad = GetGamepad(actorData.newPlayerIndex);
	auto tiltDirection = GetRelativeTiltDirection(actorData);
	bool inAir = (actorData.state & STATE::IN_AIR);
	bool lastInAir = (actorData.lastState & STATE::IN_AIR);

	/*bool inRoyalBlock = (!(lockOn && tiltDirection == TILT_DIRECTION::UP)) && (!(lockOn && tiltDirection == TILT_DIRECTION::DOWN) &&
						gamepad.buttons[2] & GetBinding(BINDING::STYLE_ACTION));*/


						//Storing Momentum
	if (actorData.motionData[0].index != 17 && actorData.motionData[0].index != 33) {
		if (actorData.action != EBONY_IVORY_RAIN_STORM) {
			rainstormInertia.cachedPull = actorData.horizontalPull;

		}

		if (!(actorData.action == REBELLION_HIGH_TIME ||
			actorData.action == REBELLION_HIGH_TIME_LAUNCH)) {
			highTimeRotation = actorData.rotation;
		}

		if (!(actorData.action == REBELLION_AERIAL_RAVE_PART_1 ||
			actorData.action == REBELLION_AERIAL_RAVE_PART_2 ||
			actorData.action == REBELLION_AERIAL_RAVE_PART_3 ||
			actorData.action == REBELLION_AERIAL_RAVE_PART_4)) {

			airRaveInertia.cachedPull = actorData.horizontalPull;
			raveRotation = actorData.rotation;


		}

		if (actorData.action != CERBERUS_AIR_FLICKER) {
			airFlickerInertia.cachedPull = actorData.horizontalPull;
			airFlickerRotation = actorData.rotation;

		}

		if (!(actorData.action == AGNI_RUDRA_SKY_DANCE_PART_1 ||
			actorData.action == AGNI_RUDRA_SKY_DANCE_PART_2 ||
			actorData.action == AGNI_RUDRA_SKY_DANCE_PART_3)) {

			skyDanceInertia.cachedPull = actorData.horizontalPull;
			skyDanceRotation = actorData.rotation;

		}

		if (!(actorData.action == NEVAN_AIR_SLASH_PART_1 ||
			actorData.action == NEVAN_AIR_SLASH_PART_2)) {

			airSlashInertia.cachedPull = actorData.horizontalPull;
			airSlashRotation = actorData.rotation;

		}

		if (actorData.action != BEOWULF_THE_HAMMER) {

			theHammerInertia.cachedPull = actorData.horizontalPull;
			theHammerRotation = actorData.rotation;
		}

		if (actorData.action != BEOWULF_TORNADO) {

			tornadoInertia.cachedPull = actorData.horizontalPull;
		}

		if (actorData.action != SHOTGUN_AIR_FIREWORKS) {
			fireworksInertia.cachedPull = actorData.horizontalPull;
		}


		if (!(actorData.action == YAMATO_AERIAL_RAVE_PART_1 ||
			actorData.action == YAMATO_AERIAL_RAVE_PART_2)) {

			yamatoRaveRotation = actorData.rotation;

		}

		if (actorData.action != EBONY_IVORY_AIR_NORMAL_SHOT) {
			ebonyIvoryShotInertia.cachedPull = actorData.horizontalPull;
		}

		if (actorData.action != SHOTGUN_AIR_NORMAL_SHOT) {
			shotgunAirInertia.cachedPull = actorData.horizontalPull;
		}

		if (actorData.action != ARTEMIS_AIR_NORMAL_SHOT) {
			artemisShotInertia.cachedPull = actorData.horizontalPull;
		}

		if (actorData.action != ARTEMIS_AIR_MULTI_LOCK_SHOT) {
			artemisMultiLockInertia.cachedPull = actorData.horizontalPull;
		}

		if (!inRoyalBlock) {
			royalBlockRotation = actorData.rotation;
		}


		if (!inRoyalBlock && actorData.eventData[0].event != 33 && actorData.eventData[0].event != 7) {
			if (actorData.eventData[0].event == 23) {
				royalBlockInertia.cachedPull = 24.0f;
			}
			else {
				royalBlockInertia.cachedPull = actorData.horizontalPull;
			}
		}

		if (!(actorData.action == 195 && actorData.state == 65538)) {
			storedSkyLaunchPosX = actorData.position.x;
			storedSkyLaunchPosY = actorData.position.y;
			storedSkyLaunchPosZ = actorData.position.z;
			storedSkyLaunchRank = actorData.styleData.rank;
			appliedSkyLaunchProperties = false;
		}


		// TO-DO: Separate these pos adjustments to another function;
		auto& cloneActorData = *reinterpret_cast<PlayerActorData*>(actorData.cloneActorBaseAddr);

		if (actorData.action != BEOWULF_RISING_SUN) {
			storedRisingSunTauntPosY = actorData.position.y;
			storedRisingSunTauntPosYClone = cloneActorData.position.y;
		}

		if (!(actorData.state & STATE::IN_AIR && (actorData.action == BEOWULF_LUNAR_PHASE_LEVEL_1 || actorData.action == BEOWULF_LUNAR_PHASE_LEVEL_2) &&
			actorData.eventData[0].event == 17)) {
			storedLunarPhasePosY = actorData.position.y;
		}
	}

	/*if(actorData.eventData[0].event == 33) {
		airRaveInertia.cachedPull = actorData.horizontalPull;
		skyDanceInertia.cachedPull = actorData.horizontalPull;
		airFlickerInertia.cachedPull = actorData.horizontalPull;
		airSlashInertia.cachedPull = actorData.horizontalPull;
		theHammerInertia.cachedPull = actorData.horizontalPull;
		ebonyIvoryShotInertia.cachedPull = actorData.horizontalPull;
		shotgunAirInertia.cachedPull = actorData.horizontalPull;
		artemisShotInertia.cachedPull = actorData.horizontalPull;
		artemisMultiLockInertia.cachedPull = actorData.horizontalPull;
		skyDanceInertia.cachedPull = actorData.horizontalPull;
		//royalBlockInertia.cachedPull = actorData.horizontalPull;
	}*/

	auto inAirShot = (actorData.action == EBONY_IVORY_AIR_NORMAL_SHOT || actorData.action == SHOTGUN_AIR_NORMAL_SHOT ||
		actorData.action == ARTEMIS_AIR_NORMAL_SHOT || actorData.action == ARTEMIS_AIR_MULTI_LOCK_SHOT);

	//Storing Direction
	if ((!(actorData.action == REBELLION_AERIAL_RAVE_PART_1 ||
		actorData.action == REBELLION_AERIAL_RAVE_PART_2 ||
		actorData.action == REBELLION_AERIAL_RAVE_PART_3 ||
		actorData.action == REBELLION_AERIAL_RAVE_PART_4 || inAirShot)) &&
		actorData.eventData[0].event != 6 && actorData.eventData[0].event != 33 && actorData.motionData[0].index != 17 &&
		actorData.motionData[0].index != 33 && actorData.eventData[0].event != 7) {


		if ((tiltDirection == TILT_DIRECTION::UP || tiltDirection == TILT_DIRECTION::DOWN || tiltDirection == TILT_DIRECTION::NEUTRAL)) {
			airRaveInertia.cachedDirection = tiltDirection;
		}
	}

	if ((!(actorData.action == AGNI_RUDRA_SKY_DANCE_PART_1 ||
		actorData.action == AGNI_RUDRA_SKY_DANCE_PART_2 ||
		actorData.action == AGNI_RUDRA_SKY_DANCE_PART_3 || inAirShot))
		&& actorData.eventData[0].event != 6 && actorData.eventData[0].event != 33 && actorData.motionData[0].index != 17 &&
		actorData.motionData[0].index != 33 && actorData.eventData[0].event != 7) {


		if (tiltDirection == TILT_DIRECTION::UP || tiltDirection == TILT_DIRECTION::DOWN || tiltDirection == TILT_DIRECTION::NEUTRAL) {
			skyDanceInertia.cachedDirection = tiltDirection;
		}
	}

	/*if((!(actorData.action == EBONY_IVORY_AIR_NORMAL_SHOT))
	&& actorData.eventData[0].event != 6 && actorData.eventData[0].event != 33
	&& actorData.motionData[0].index != 17 && actorData.motionData[0].index != 33 &&
	actorData.eventData[0].event != 7 && !inGunShoot && !(actorData.action == REBELLION_AERIAL_RAVE_PART_1 ||
					actorData.action == REBELLION_AERIAL_RAVE_PART_2 ||
					actorData.action == REBELLION_AERIAL_RAVE_PART_3 ||
					actorData.action == REBELLION_AERIAL_RAVE_PART_4 ||
					actorData.action == AGNI_RUDRA_SKY_DANCE_PART_1 ||
					actorData.action == AGNI_RUDRA_SKY_DANCE_PART_2 ||
					actorData.action == AGNI_RUDRA_SKY_DANCE_PART_3)) {

		if(tiltDirection == TILT_DIRECTION::UP || tiltDirection == TILT_DIRECTION::DOWN) {
			ebonyIvoryShotInertia.cachedDirection = tiltDirection;
		}

	}*/

	if (!inGunShoot && !(actorData.action == EBONY_IVORY_AIR_NORMAL_SHOT || actorData.action == SHOTGUN_AIR_NORMAL_SHOT ||
		actorData.action == ARTEMIS_AIR_NORMAL_SHOT || actorData.action == ARTEMIS_AIR_MULTI_LOCK_SHOT)) {


		ebonyIvoryShotInertia.cachedDirection = airRaveInertia.cachedDirection;



	}

	if (inGunShoot) {
		if (ebonyIvoryShotInertia.cachedDirection == 0 && airRaveInertia.cachedDirection == 1) {
			ebonyIvoryShotInertia.cachedDirection = 1;
		}
	}

	if (inAirShot && actorData.eventData[0].event != 7 && inGunShoot) {
		if (tiltDirection == TILT_DIRECTION::UP) {

			airRaveInertia.cachedDirection = tiltDirection;
			skyDanceInertia.cachedDirection = tiltDirection;
		}

		if (tiltDirection == TILT_DIRECTION::DOWN) {

			airRaveInertia.cachedDirection = tiltDirection;
			skyDanceInertia.cachedDirection = tiltDirection;
		}
	}

	if ((actorData.motionData[0].index == 33
		|| actorData.motionData[0].index == 38 || actorData.motionData[0].index == 39 || actorData.eventData[0].event == 23 || actorData.action == BEOWULF_KILLER_BEE)
		&& actorData.eventData[0].event != 7) {
		if (tiltDirection == TILT_DIRECTION::NEUTRAL || tiltDirection == TILT_DIRECTION::UP || tiltDirection == TILT_DIRECTION::DOWN) {
			airRaveInertia.cachedDirection = tiltDirection;
			skyDanceInertia.cachedDirection = tiltDirection;
		}
	}

	/*if(actorData.eventData[0].event == 7 || actorData.eventData[0].event == 23) {
		ebonyIvoryShotInertia.cachedDirection = tiltDirection;
	}*/

	if (((actorData.action == REBELLION_AERIAL_RAVE_PART_1 ||
		actorData.action == REBELLION_AERIAL_RAVE_PART_2 ||
		actorData.action == REBELLION_AERIAL_RAVE_PART_3 ||
		actorData.action == REBELLION_AERIAL_RAVE_PART_4 ||
		actorData.action == AGNI_RUDRA_SKY_DANCE_PART_1 ||
		actorData.action == AGNI_RUDRA_SKY_DANCE_PART_2 ||
		actorData.action == AGNI_RUDRA_SKY_DANCE_PART_3) &&
		(actorData.lastAction == EBONY_IVORY_AIR_NORMAL_SHOT || actorData.lastAction == SHOTGUN_AIR_NORMAL_SHOT ||
			actorData.lastAction == ARTEMIS_AIR_NORMAL_SHOT || actorData.lastAction == ARTEMIS_AIR_MULTI_LOCK_SHOT)) ||

		(actorData.lastAction == REBELLION_AERIAL_RAVE_PART_1 ||
			actorData.lastAction == REBELLION_AERIAL_RAVE_PART_2 ||
			actorData.lastAction == REBELLION_AERIAL_RAVE_PART_3 ||
			actorData.lastAction == REBELLION_AERIAL_RAVE_PART_4 ||
			actorData.lastAction == AGNI_RUDRA_SKY_DANCE_PART_1 ||
			actorData.lastAction == AGNI_RUDRA_SKY_DANCE_PART_2 ||
			actorData.lastAction == AGNI_RUDRA_SKY_DANCE_PART_3) &&
		(actorData.lastAction == EBONY_IVORY_AIR_NORMAL_SHOT || actorData.action == SHOTGUN_AIR_NORMAL_SHOT ||
			actorData.action == ARTEMIS_AIR_NORMAL_SHOT || actorData.action == ARTEMIS_AIR_MULTI_LOCK_SHOT)) {


		inGunShoot = true;
	}

	if (!(actorData.state & STATE::IN_AIR) || actorData.eventData[0].event == ACTOR_EVENT::TRICKSTER_AIR_TRICK) {
		inGunShoot = false;
	}

	if (actorData.eventData[0].event == 7) {
		if (tiltDirection == TILT_DIRECTION::UP || tiltDirection == TILT_DIRECTION::DOWN) {
			airRaveInertia.cachedDirection = tiltDirection;
			skyDanceInertia.cachedDirection = tiltDirection;
		}
	}

	if (inRoyalBlock) {
		if (tiltDirection == TILT_DIRECTION::UP || tiltDirection == TILT_DIRECTION::DOWN) {
			royalBlockInertia.cachedDirection = tiltDirection;
		}
	}


}


export void InertiaController(byte8* actorBaseAddr) {
	// Inertia implementation. Momentum (or Pull) is stored before a certain move is executed, then
	// when it's executed that Pull is preserved and carries over, some moves had their overall momentum
	// increased a bit as well.

	using namespace ACTION_DANTE;
	using namespace ACTION_VERGIL;

	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
	if (!inertiaFixesEnabled) {
		AerialRaveInertiaFix(true);
		SkyDanceInertiaFix(true);
		AirSlashInertiaFix(true);
		TatsumakiInertiaFix(true);

		inertiaFixesEnabled = true;
	}



	auto lockOn = (actorData.buttons[0] & GetBinding(BINDING::LOCK_ON));
	auto tiltDirection = GetRelativeTiltDirection(actorData);
	auto& gamepad = GetGamepad(0);
	auto cameraDirection = actorData.cameraDirection;
	auto radius = gamepad.leftStickRadius;
	uint16 relativeTilt = 0;
	relativeTilt = (actorData.cameraDirection + gamepad.leftStickPosition);
	uint16 value = (relativeTilt - 0x8000);

	distanceToEnemy = actorData.position.z - actorData.lockOnData.targetPosition.z;

	bool inAirSwordmaster = ((actorData.action == REBELLION_AERIAL_RAVE_PART_1 ||
		actorData.action == REBELLION_AERIAL_RAVE_PART_2 ||
		actorData.action == REBELLION_AERIAL_RAVE_PART_3 ||
		actorData.action == REBELLION_AERIAL_RAVE_PART_4) || (actorData.action == AGNI_RUDRA_SKY_DANCE_PART_1 ||
			actorData.action == AGNI_RUDRA_SKY_DANCE_PART_2 ||
			actorData.action == AGNI_RUDRA_SKY_DANCE_PART_3) || (actorData.action == NEVAN_AIR_SLASH_PART_1 ||
				actorData.action == NEVAN_AIR_SLASH_PART_2) || (actorData.action == CERBERUS_AIR_FLICKER) || (actorData.action == BEOWULF_TORNADO));

	bool inAirGunslinger = (actorData.action == SHOTGUN_AIR_FIREWORKS ||
		actorData.action == ARTEMIS_AIR_NORMAL_SHOT || actorData.action == ARTEMIS_AIR_NORMAL_SHOT);


	/*bool inRoyalBlock = (!(lockOn && tiltDirection == TILT_DIRECTION::UP)) && (!(lockOn && tiltDirection == TILT_DIRECTION::DOWN) &&
						gamepad.buttons[0] & GetBinding(BINDING::STYLE_ACTION));*/

	exceptionShot = ((actorData.lastAction == REBELLION_AERIAL_RAVE_PART_1
		|| actorData.lastAction == REBELLION_AERIAL_RAVE_PART_2 ||
		actorData.lastAction == REBELLION_AERIAL_RAVE_PART_3 ||
		actorData.lastAction == REBELLION_AERIAL_RAVE_PART_4) && airRaveInertia.cachedDirection == TILT_DIRECTION::UP);

	if (actorData.character == CHARACTER::DANTE) {

		// Guardfly
		if (actorData.motionData[0].index == 5 && inRoyalBlock && (actorData.eventData[0].event == 33 || actorData.eventData[0].event == 7)) {
			actorData.horizontalPull = royalBlockInertia.cachedPull;
			actorData.verticalPullMultiplier = -2;
		}

		// This Mimic's DMC4's Trick Up Inertia Boost behaviour, uses LastEventStateQueue
		if (actorData.eventData[0].event == ACTOR_EVENT::TRICK_UP_END && lastLastEvent == 17 && lastLastState & STATE::IN_AIR) {
			actorData.horizontalPull = 7.5f;
		}

		// Experimental shit: Reverse Shotgun Stinger
// 		if (actorData.action == 146 && actorData.eventData[0].event == 17) {
// 			actorData.horizontalPull = -25.0f;
// 		}

		if (actorData.state == 65538) {

			// Rainstorm
			if (actorData.action == EBONY_IVORY_RAIN_STORM) {

				if (rainstormInertia.cachedPull < 0) {
					rainstormInertia.cachedPull = rainstormInertia.cachedPull * -1.0f;
				}

				rainstormInertia.cachedPull = glm::clamp(rainstormInertia.cachedPull, -9.0f, 9.0f);

				actorData.horizontalPull = rainstormInertia.cachedPull / rainstormInertia.haltDivisor;




				//actorData.horizontalPullMultiplier = 0.2f;
			}

			// E&I Normal Shot
			else if (actorData.action == EBONY_IVORY_AIR_NORMAL_SHOT) {
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

				if (ebonyIvoryShotInertia.cachedPull < 0) {
					ebonyIvoryShotInertia.cachedPull = ebonyIvoryShotInertia.cachedPull * -1.0f;
				}



				ebonyIvoryShotInertia.cachedPull = glm::clamp(ebonyIvoryShotInertia.cachedPull, -4.0f, 4.0f);

				actorData.horizontalPull = ebonyIvoryShotInertia.cachedPull;
				actorData.horizontalPullMultiplier = 0.03f;
			}

			// Shotgun Normal Shot
			else if (actorData.action == SHOTGUN_AIR_NORMAL_SHOT) {
				if (shotgunAirInertia.cachedPull < 0) {
					shotgunAirInertia.cachedPull = shotgunAirInertia.cachedPull * -1.0f;
				}

				shotgunAirInertia.cachedPull = glm::clamp(shotgunAirInertia.cachedPull, -5.0f, 5.0f);

				actorData.horizontalPull = shotgunAirInertia.cachedPull;
				actorData.horizontalPullMultiplier = 0.05f;
				actorData.verticalPullMultiplier = -1.7f;
			}

			// Artemis Normal Shot
			else if (actorData.action == ARTEMIS_AIR_NORMAL_SHOT) {
				if (artemisShotInertia.cachedPull < 0) {
					artemisShotInertia.cachedPull = artemisShotInertia.cachedPull * -1.0f;
				}

				artemisShotInertia.cachedPull = glm::clamp(artemisShotInertia.cachedPull, -5.0f, 5.0f);

				actorData.horizontalPull = artemisShotInertia.cachedPull;
				actorData.horizontalPullMultiplier = 0.05f;
			}

			// Artemis Multi-Lock Shot
			else if (actorData.action == ARTEMIS_AIR_MULTI_LOCK_SHOT) {
				if (artemisMultiLockInertia.cachedPull < 0) {
					artemisMultiLockInertia.cachedPull = artemisMultiLockInertia.cachedPull * -1.0f;
				}

				artemisMultiLockInertia.cachedPull = glm::clamp(artemisMultiLockInertia.cachedPull, -5.0f, 5.0f);

				actorData.horizontalPull = artemisMultiLockInertia.cachedPull;
				actorData.horizontalPullMultiplier = 0.05f;

			}

			// Aerial Rave
			else if (actorData.action == REBELLION_AERIAL_RAVE_PART_1 ||
				actorData.action == REBELLION_AERIAL_RAVE_PART_2 ||
				actorData.action == REBELLION_AERIAL_RAVE_PART_3 ||
				actorData.action == REBELLION_AERIAL_RAVE_PART_4) {
				if (airRaveInertia.cachedPull < 0) {
					airRaveInertia.cachedPull = airRaveInertia.cachedPull * -1.0f;
				}


				airRaveInertia.cachedPull = glm::clamp(airRaveInertia.cachedPull, -9.0f, 9.0f);
				// If this is any weapon air shot then inertia transfers (almost) completely
				if (actorData.lastAction != EBONY_IVORY_AIR_NORMAL_SHOT &&
					actorData.lastAction != SHOTGUN_AIR_NORMAL_SHOT &&
					actorData.lastAction != ARTEMIS_AIR_NORMAL_SHOT &&
					actorData.lastAction != ARTEMIS_AIR_MULTI_LOCK_SHOT &&
					actorData.lastAction != AGNI_RUDRA_SKY_DANCE_PART_1 &&
					actorData.lastAction != AGNI_RUDRA_SKY_DANCE_PART_2 &&
					actorData.lastAction != AGNI_RUDRA_SKY_DANCE_PART_3) {

					actorData.horizontalPull = (airRaveInertia.cachedPull / 3.0f) * 1.0f;

				}
				else {
					actorData.horizontalPull = (airRaveInertia.cachedPull / 1.2f) * 1.0f;

				}


				// Reduces gravity while air raving
				if (actorData.action != REBELLION_AERIAL_RAVE_PART_4) {
					actorData.verticalPull = -1.0f;
					actorData.verticalPullMultiplier = 0;
				}
				else if (actorData.action == REBELLION_AERIAL_RAVE_PART_4) {
					actorData.verticalPull = -2.0f;
					actorData.verticalPullMultiplier = 0;
				}

				//actorData.horizontalPullMultiplier = 0;
				//actorData.horizontalPullMultiplier = -0.12f;
			}

			// Cerberus Flicker
			else if (actorData.action == CERBERUS_AIR_FLICKER) {

				if (airFlickerInertia.cachedPull < 0) {
					airFlickerInertia.cachedPull = airFlickerInertia.cachedPull * -1.0f;
				}

				airFlickerInertia.cachedPull = glm::clamp(airFlickerInertia.cachedPull, 2.0f, 9.0f);
				actorData.horizontalPull = (airFlickerInertia.cachedPull / airFlickerInertia.haltDivisor) * 1.0f;


				// Reduces Gravity Fall-off
				actorData.verticalPullMultiplier = -0.20f;


				//actorData.horizontalPullMultiplier = -0.18f;
			}

			// Sky Dance
			else if (actorData.action == AGNI_RUDRA_SKY_DANCE_PART_1 ||
				actorData.action == AGNI_RUDRA_SKY_DANCE_PART_2 ||
				actorData.action == AGNI_RUDRA_SKY_DANCE_PART_3) {

				if (skyDanceInertia.cachedPull < 0) {
					skyDanceInertia.cachedPull = skyDanceInertia.cachedPull * -1.0f;
				}

				skyDanceInertia.cachedPull = glm::clamp(skyDanceInertia.cachedPull, 3.0f, 9.0f);

				if (actorData.lastAction != EBONY_IVORY_AIR_NORMAL_SHOT &&
					actorData.lastAction != SHOTGUN_AIR_NORMAL_SHOT &&
					actorData.lastAction != ARTEMIS_AIR_NORMAL_SHOT &&
					actorData.lastAction != ARTEMIS_AIR_MULTI_LOCK_SHOT &&
					actorData.lastAction != REBELLION_AERIAL_RAVE_PART_1 &&
					actorData.lastAction != REBELLION_AERIAL_RAVE_PART_2 &&
					actorData.lastAction != REBELLION_AERIAL_RAVE_PART_3) {

					actorData.horizontalPull = (skyDanceInertia.cachedPull / 4.0f) * 1.0f;
				}
				else {
					actorData.horizontalPull = (skyDanceInertia.cachedPull / 1.2f) * 1.0f;
				}
				//actorData.horizontalPullMultiplier = -0.16f;
			}

			// Air Slash
			else if (actorData.action == NEVAN_AIR_SLASH_PART_1 ||
				actorData.action == NEVAN_AIR_SLASH_PART_2) {

				if (airSlashInertia.cachedPull < 0) {
					airSlashInertia.cachedPull = airSlashInertia.cachedPull * -1.0f;
				}

				airSlashInertia.cachedPull = glm::clamp(airSlashInertia.cachedPull, -9.0f, 9.0f);

				actorData.horizontalPull = (airSlashInertia.cachedPull / 1.5f) * 1.0f;
				//actorData.horizontalPullMultiplier = 0.4f;

				//actorData.horizontalPullMultiplier = 0.4f;
			}

			// Air Play
			else if (actorData.action == NEVAN_AIR_PLAY) {
				actorData.horizontalPullMultiplier = 0.2f;
			}

			// The Hammer
			else if (actorData.action == BEOWULF_THE_HAMMER) {
				if (theHammerInertia.cachedPull < 0) {
					theHammerInertia.cachedPull = theHammerInertia.cachedPull * -1.0f;
				}

				theHammerInertia.cachedPull = glm::clamp(theHammerInertia.cachedPull, -9.0f, 9.0f);
				actorData.horizontalPull = (theHammerInertia.cachedPull / 1.5f) * 1.0f;

			}

			// Tornado
			else if (actorData.action == BEOWULF_TORNADO) {
				if (tornadoInertia.cachedPull < 0) {
					tornadoInertia.cachedPull = tornadoInertia.cachedPull * -1.0f;
				}

				tornadoInertia.cachedPull = glm::clamp(tornadoInertia.cachedPull, -9.0f, 9.0f);
				actorData.horizontalPull = (tornadoInertia.cachedPull / 1.5f) * 1.0f;

			}

			// Killer Bee
			else if (actorData.action == BEOWULF_KILLER_BEE) {



				// Makes divekick speed be consistent.
				actorData.horizontalPull = 24.0f;
			}


			// Fireworks
			else if (actorData.action == SHOTGUN_AIR_FIREWORKS) {
				if (fireworksInertia.cachedPull < 0) {
					fireworksInertia.cachedPull = fireworksInertia.cachedPull * -1.0f;
				}

				fireworksInertia.cachedPull = glm::clamp(fireworksInertia.cachedPull, -9.0f, 9.0f);
				actorData.horizontalPull = fireworksInertia.cachedPull / 1.5f;
			}



			/*// GUARDFLY on divekick
			else if (actorData.action == ROYAL_AIR_BLOCK &&
			(distanceToEnemy < 150.0f && distanceToEnemy > -150.0f) && (actorData.eventData[0].event != 23)) {
				//actorData.position.x = 0;
				actorData.horizontalPull = royalBlockInertia.cachedPull * 2.0f;
				actorData.horizontalPullMultiplier = 2.0f;
			}*/

			// GUARDFLY on sky star
			/*else if (actorData.action == ROYAL_AIR_BLOCK && (distanceToEnemy < 80.0f && distanceToEnemy > -80.0f) && (actorData.eventData[0].event == 23)) {
				//actorData.position.x = 0;
				actorData.horizontalPull = royalBlockInertia.cachedPull * 2.0f;
				actorData.horizontalPullMultiplier = 2.0f;
			}*/

			/*else if (actorData.eventData[0].event == 33 && actorData.lastAction == BEOWULF_KILLER_BEE) {

				actorData.horizontalPull = royalBlockInertia.cachedPull * -2.0f;
				actorData.verticalPullMultiplier = 0;
			}

			else if (actorData.eventData[0].event == 7 && actorData.eventData[0].lastEvent == 33) {
				actorData.horizontalPull = 24;
			}*/






			if ((actorData.action == CERBERUS_REVOLVER_LEVEL_1 || actorData.action == CERBERUS_REVOLVER_LEVEL_2) && actorData.state == 65538) {
				//actorData.verticalPull = 0;
				//actorData.verticalPullMultiplier = -0.4f;
			}
		}


	}
	else if (actorData.character == CHARACTER::VERGIL) {
		if (actorData.state == 65538) {

			// Adjusts Vergil Pos to be lower when starting Air Rising Sun
			if (actorData.state & STATE::IN_AIR && actorData.action == BEOWULF_RISING_SUN) {
				actorData.verticalPullMultiplier = 0.0f;
				if (!actorData.newIsClone) {
					actorData.position.y = storedRisingSunTauntPosY - 50.0f;
				}
				else {
					actorData.position.y = storedRisingSunTauntPosYClone - 50.0f;
				}

			}

			// Adjusts Vergil Pos to be lower when starting Air Lunar Phase
			if (actorData.state & STATE::IN_AIR && (actorData.action == BEOWULF_LUNAR_PHASE_LEVEL_1 || actorData.action == BEOWULF_LUNAR_PHASE_LEVEL_2) &&
				actorData.motionData[0].index != 23) {
				actorData.verticalPullMultiplier = 0.0f;
				actorData.position.y = storedLunarPhasePosY - 50.0f;
			}
		}
	}



}

#pragma endregion

#pragma region GeneralGameplay

export void StyleMeterDoppelganger(byte8* actorBaseAddr) {

	//This allows Doppelgangers to actually fill your style meter with their attacks (but not increase your ranks).
	auto pool_12119 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if
		(
			!pool_12119 ||
			!pool_12119[3]
			) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(pool_12119[3]);
	auto& characterData = GetCharacterData(actorData);
	auto& cloneActorData = *reinterpret_cast<PlayerActorData*>(actorData.cloneActorBaseAddr);

	if (actorData.doppelganger) {
		cloneActorData.styleData.rank = actorData.styleData.rank;

		if (actorData.styleData.meter > 50) {
			actorData.styleData.meter = glm::max(actorData.styleData.meter, cloneActorData.styleData.meter);
		}

	}
}

void inCombatDetectionTracker() {
	inCombatTrackerRunning = true;
	inCombatTime = inCombatDelay;
	while (inCombatTime > 0) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		inCombatTime--;
	}


	if (inCombatTime == 0) {
		inCombatTrackerRunning = false;
		inCombat = false;
	}

}

export void inCombatDetection() {
	auto pool_12346 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if
		(
			!pool_12346 ||
			!pool_12346[8]
			) {
		return;
	}
	auto& enemyVectorData = *reinterpret_cast<EnemyVectorData*>(pool_12346[8]);


	if (g_scene != SCENE::GAME) {
		inCombat = false;
	}
	else {
		if (enemyVectorData.count >= 1) {
			inCombat = true;
		}
		else if (enemyVectorData.count == 0 && !inCombatTrackerRunning && inCombat) {
			std::thread incombatdetectiontracker(inCombatDetectionTracker);
			incombatdetectiontracker.detach();
		}
	}

}


export void DTReadySFX() {
	auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

	auto pool_12405 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if
		(
			!pool_12405 ||
			!pool_12405[3]
			) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(pool_12405[3]);


	if (actorData.magicPoints >= 3000 && !devilTriggerReadyPlayed) {
		PlayDevilTriggerReady();
		devilTriggerReadyPlayed = true;
	}
	else if (actorData.magicPoints < 3000) {
		devilTriggerReadyPlayed = false;
	}
}


export void BackToForwardInputs(byte8* actorBaseAddr) {
	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);

	auto lockOn = actorData.lockOn;
	auto tiltDirection = GetRelativeTiltDirection(actorData);
	auto playerIndex = actorData.newPlayerIndex;
	auto& gamepad = GetGamepad(playerIndex);
	auto radius = gamepad.leftStickRadius;
	auto pos = gamepad.leftStickPosition;

	if (crimsonPlayer[playerIndex].b2F.backBuffer <= 0) {

		crimsonPlayer[playerIndex].b2F.backCommand = false;

	}

	if (crimsonPlayer[playerIndex].b2F.backBuffer <= 0 && crimsonPlayer[playerIndex].b2F.backDirectionChanged) {
		crimsonPlayer[playerIndex].b2F.backBuffer = crimsonPlayer[playerIndex].b2F.backDuration;
	}

	if (crimsonPlayer[playerIndex].b2F.forwardBuffer <= 0) {
		crimsonPlayer[playerIndex].b2F.forwardCommand = false;
	}

	if (crimsonPlayer[playerIndex].b2F.forwardBuffer <= 0 && crimsonPlayer[playerIndex].b2F.forwardDirectionChanged) {
		crimsonPlayer[playerIndex].b2F.forwardBuffer = crimsonPlayer[playerIndex].b2F.forwardDuration;
	}

	if (lockOn && tiltDirection == TILT_DIRECTION::DOWN && (radius > RIGHT_STICK_DEADZONE)) {
		if (crimsonPlayer[playerIndex].b2F.backBuffer > 0) {
			crimsonPlayer[playerIndex].b2F.backCommand = true;
			crimsonPlayer[playerIndex].b2F.backDirectionChanged = false;
		}
	}
	else if (!(lockOn && tiltDirection == TILT_DIRECTION::DOWN && (radius > RIGHT_STICK_DEADZONE))) {
		crimsonPlayer[playerIndex].b2F.backDirectionChanged = true;
	}


	if (lockOn && tiltDirection == TILT_DIRECTION::UP && (radius > RIGHT_STICK_DEADZONE) && crimsonPlayer[playerIndex].b2F.backCommand) {
		if (crimsonPlayer[playerIndex].b2F.forwardBuffer > 0) {
			crimsonPlayer[playerIndex].b2F.forwardCommand = true;
			crimsonPlayer[playerIndex].b2F.forwardDirectionChanged = false;
		}
	}
	else if (!(lockOn && tiltDirection == TILT_DIRECTION::UP && (radius > RIGHT_STICK_DEADZONE))) {
		crimsonPlayer[playerIndex].b2F.forwardDirectionChanged = true;
	}

}



export void SprintAbility(byte8* actorBaseAddr) {


	// Old iteration of SprintAbility used mainActorData, working only for the mainActor. Now it's supposed to work for all players.
// 	auto pool_12188 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
// 	if
// 		(
// 			!pool_12188 ||
// 			!pool_12188[3]
// 			) {
// 		return;
// 	}
// 	auto& actorData = *reinterpret_cast<PlayerActorData*>(pool_12188[3

	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);


	auto& gamepad = GetGamepad(actorData.newPlayerIndex);
	auto radius = gamepad.leftStickRadius;
	auto& playerData = GetPlayerData(actorData);

	auto playerIndex = actorData.newPlayerIndex;

	if ((actorData.newCharacterIndex == playerData.activeCharacterIndex) &&
		(actorData.newEntityIndex == ENTITY::MAIN)) {

		//auto playerSprint = crimsonPlayer[playerIndex].sprint;

		if (!crimsonPlayer[playerIndex].sprint.isSprinting) {
			// Storing the actor's set speed when not sprinting for us to calculate sprintSpeed.
			crimsonPlayer[playerIndex].sprint.storedSpeedHuman = activeConfig.Speed.human;

			if (actorData.character == CHARACTER::DANTE) {
				crimsonPlayer[playerIndex].sprint.storedSpeedDevilDante[0] = activeConfig.Speed.devilDante[0];
				crimsonPlayer[playerIndex].sprint.storedSpeedDevilDante[1] = activeConfig.Speed.devilDante[1];
				crimsonPlayer[playerIndex].sprint.storedSpeedDevilDante[2] = activeConfig.Speed.devilDante[2];
				crimsonPlayer[playerIndex].sprint.storedSpeedDevilDante[3] = activeConfig.Speed.devilDante[3];
				crimsonPlayer[playerIndex].sprint.storedSpeedDevilDante[4] = activeConfig.Speed.devilDante[4];
				crimsonPlayer[playerIndex].sprint.storedSpeedDevilDante[5] = activeConfig.Speed.devilDante[5];
			}
			else if (actorData.character == CHARACTER::VERGIL) {
				crimsonPlayer[playerIndex].sprint.storedSpeedDevilVergil[0] = activeConfig.Speed.devilVergil[0];
				crimsonPlayer[playerIndex].sprint.storedSpeedDevilVergil[1] = activeConfig.Speed.devilVergil[1];
				crimsonPlayer[playerIndex].sprint.storedSpeedDevilVergil[2] = activeConfig.Speed.devilVergil[2];
				crimsonPlayer[playerIndex].sprint.storedSpeedDevilVergil[3] = activeConfig.Speed.devilVergil[3];
				crimsonPlayer[playerIndex].sprint.storedSpeedDevilVergil[4] = activeConfig.Speed.devilVergil[4];
			}



			crimsonPlayer[playerIndex].sprint.SFXPlayed = false;
			crimsonPlayer[playerIndex].sprint.VFXPlayed = false;
		}


		// This ties the ability only to the main actor.
	// 	if ((actorData.newPlayerIndex == 0) &&
	// 		(actorData.newCharacterIndex == playerData.activeCharacterIndex) &&
	// 		(actorData.newEntityIndex == ENTITY::MAIN)) {


		if (actorData.state == 524289 && !inCombat) {


			if (!crimsonPlayer[playerIndex].sprint.runTimer) {
				crimsonPlayer[playerIndex].sprint.timer = crimsonPlayer[playerIndex].sprint.timeToTrigger / actorData.speed;
			}

			if (!crimsonPlayer[playerIndex].sprint.canSprint) {
				crimsonPlayer[playerIndex].sprint.runTimer = true;
			}




		}
		else {
			crimsonPlayer[playerIndex].sprint.canSprint = false;
			crimsonPlayer[playerIndex].sprint.runTimer = false;

		}


		auto gameSpeedValue = (IsTurbo()) ? activeConfig.Speed.turbo : activeConfig.Speed.mainSpeed;

		if (!crimsonPlayer[playerIndex].sprint.runTimer && crimsonPlayer[playerIndex].sprint.timer <= 0) {


		}




		if (crimsonPlayer[playerIndex].sprint.canSprint) {

			// Setting the sprint speed. Increasing by 30%.
			float sprintMultiplier = 1.3f;
			float sprintSpeed = crimsonPlayer[playerIndex].sprint.storedSpeedHuman * sprintMultiplier;
			float sprintSpeedDevilDante[6];
			float sprintSpeedDevilVergil[5];

			if (actorData.character == CHARACTER::DANTE) {
				sprintSpeedDevilDante[0] = crimsonPlayer[playerIndex].sprint.storedSpeedDevilDante[0] * sprintMultiplier;
				sprintSpeedDevilDante[1] = crimsonPlayer[playerIndex].sprint.storedSpeedDevilDante[1] * sprintMultiplier;
				sprintSpeedDevilDante[2] = crimsonPlayer[playerIndex].sprint.storedSpeedDevilDante[2] * sprintMultiplier;
				sprintSpeedDevilDante[3] = crimsonPlayer[playerIndex].sprint.storedSpeedDevilDante[3] * sprintMultiplier;
				sprintSpeedDevilDante[4] = crimsonPlayer[playerIndex].sprint.storedSpeedDevilDante[4] * sprintMultiplier;
				sprintSpeedDevilDante[5] = crimsonPlayer[playerIndex].sprint.storedSpeedDevilDante[5] * sprintMultiplier;
			}
			else if (actorData.character == CHARACTER::VERGIL) {
				sprintSpeedDevilVergil[0] = crimsonPlayer[playerIndex].sprint.storedSpeedDevilVergil[0] * sprintMultiplier;
				sprintSpeedDevilVergil[1] = crimsonPlayer[playerIndex].sprint.storedSpeedDevilVergil[1] * sprintMultiplier;
				sprintSpeedDevilVergil[2] = crimsonPlayer[playerIndex].sprint.storedSpeedDevilVergil[2] * sprintMultiplier;
				sprintSpeedDevilVergil[3] = crimsonPlayer[playerIndex].sprint.storedSpeedDevilVergil[3] * sprintMultiplier;
				sprintSpeedDevilVergil[4] = crimsonPlayer[playerIndex].sprint.storedSpeedDevilVergil[4] * sprintMultiplier;
			}



			// Applying the new sprint speed into the Actor.


	// 		old_for_all(uint8, weaponIndex, countof(actorData.newWeaponDataAddr)) {
	// 			auto weaponDataAddr = actorData.newWeaponDataAddr[weaponIndex];
	// 			if (!weaponDataAddr) {
	// 				continue;
	// 			}
	// 			auto& weaponData = *weaponDataAddr;
	// 
	// 			weaponData.speed = 2.0f;
	// 		}

			// Applying the new Speed
			if (!actorData.devil) {
				actorData.speed = sprintSpeed * gameSpeedValue;

			}
			else {
				switch (actorData.character) {
				case CHARACTER::DANTE:
				{
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
				case CHARACTER::VERGIL:
				{
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


			if (!crimsonPlayer[playerIndex].sprint.SFXPlayed) {
				playSprint();
				crimsonPlayer[playerIndex].sprint.SFXPlayed = true;
			}

			if (!crimsonPlayer[playerIndex].sprint.VFXPlayed) {
				createEffectBank = sprintVFX.bank;
				createEffectID = sprintVFX.id;
				CreateEffectDetour();

				crimsonPlayer[playerIndex].sprint.VFXPlayed = true;
			}

			crimsonPlayer[playerIndex].sprint.isSprinting = true;
			crimsonPlayer[playerIndex].sprint.runTimer = false;



		}
		else {
			// Restore the original Actor's speed when you can't sprint (either in or out of it).

			crimsonPlayer[playerIndex].sprint.isSprinting = false;
		}
	}
}

#pragma endregion

#pragma region DanteAirTaunt


void RoyalReleaseTracker() {
	auto pool_12501 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if
		(
			!pool_12501 ||
			!pool_12501[3]
			) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(pool_12501[3]);


	if ((actorData.action == 195 || actorData.action == 194 || actorData.action == 196 || actorData.action == 197) &&
		(actorData.motionData[0].index == 20 || actorData.motionData[0].index == 19)) {

		executingRoyalRelease = true;
		royalReleaseTrackerRunning = true;
		royalReleaseExecuted = true;
	}
	skyLaunchSetJustFrameTrue = false;
}

export void CheckRoyalRelease(byte8* actorBaseAddr) {
	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);

	if (((actorData.state & STATE::IN_AIR && actorData.motionData[0].index == 20 || actorData.motionData[0].index == 19) &&
		(actorData.action == 195 || actorData.action == 194 || actorData.action == 196 || actorData.action == 197) &&
		actorData.buttons[0] & GetBinding(BINDING::STYLE_ACTION) && !royalReleaseTrackerRunning)) {


		std::thread royalreleasetracker(RoyalReleaseTracker);
		royalreleasetracker.detach();
	}

	if (!((actorData.action == 195 || actorData.action == 194 || actorData.action == 196 || actorData.action == 197) &&
		(actorData.motionData[0].index == 20 || actorData.motionData[0].index == 19))) {
		executingRoyalRelease = false;
		royalReleaseTrackerRunning = false;
	}

	if (!royalReleaseTrackerRunning) {
		executingRoyalRelease = false;
	}


}

void SkyLaunchTracker() {
	auto pool_12544 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if
		(
			!pool_12544 ||
			!pool_12544[3]
			) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(pool_12544[3]);


	if ((actorData.action == 195 || actorData.action == 194 || actorData.action == 212) && (actorData.motionData[0].index == 20)) {

		executingSkyLaunch = true;
		skyLaunchTrackerRunning = true;
		//ToggleCerberusDamage(true);

		/*if(!executingSkyLaunch || !skyLaunchTrackerRunning) {
			break;
		}*/
	}




}

export void CheckSkyLaunch(byte8* actorBaseAddr) {
	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);

	if (((actorData.state & STATE::IN_AIR && actorData.motionData[0].index == 20) &&
		(actorData.action == 195) &&
		actorData.buttons[0] & GetBinding(BINDING::TAUNT) && !skyLaunchTrackerRunning && !executingRoyalRelease)) {


		std::thread skylaunchtracker(SkyLaunchTracker);
		skylaunchtracker.detach();
	}

	if (!((actorData.action == 195 || actorData.action == 194) && (actorData.motionData[0].index == 20))) {
		executingSkyLaunch = false;
		skyLaunchTrackerRunning = false;
		//ToggleCerberusDamage(activeConfig.infiniteHitPoints);
	}

	if (!skyLaunchTrackerRunning) {
		executingSkyLaunch = false;
	}


}

export void SkyLaunchProperties(byte8* actorBaseAddr) {
	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);

	if (actorData.character == CHARACTER::DANTE) {
		if (actorData.state & STATE::IN_AIR && !skyLaunchSetJustFrameTrue && !forcingJustFrameRoyalRelease) {
			ToggleRoyalguardForceJustFrameRelease(true);
			skyLaunchSetJustFrameTrue = true;
			skyLaunchSetJustFrameGround = false;
			royalReleaseJustFrameCheck = false;
		}

		if (!executingRoyalRelease && skyLaunchSetJustFrameTrue) {
			skyLaunchSetJustFrameTrue = false;
			royalReleaseExecuted = false;
		}

		if (!(actorData.state & STATE::IN_AIR) && !skyLaunchSetJustFrameGround) {
			ToggleRoyalguardForceJustFrameRelease(activeConfig.Royalguard.forceJustFrameRelease);
			skyLaunchSetJustFrameGround = true;
		}

		if (executingRoyalRelease && !royalReleaseJustFrameCheck) {
			ToggleRoyalguardForceJustFrameRelease(activeConfig.Royalguard.forceJustFrameRelease);
			royalReleaseJustFrameCheck = true;
		}
	}

	if (executingSkyLaunch) {

		actorData.position.x = storedSkyLaunchPosX;
		actorData.position.z = storedSkyLaunchPosZ;
		actorData.styleData.rank = storedSkyLaunchRank;

		if (!skyLaunchSetVolume) {
			SetVolume(2, 0);
			skyLaunchSetVolume = true;
		}

		if (!appliedSkyLaunchProperties) {
			skyLaunchForceJustFrameToggledOff = false;


			//actorData.position.y = storedSkyLaunchPosY;
			appliedSkyLaunchProperties = true;
		}

		actorData.horizontalPull = 0;
		actorData.verticalPullMultiplier = -0.2f;

		actorData.position.x = storedSkyLaunchPosX;
		actorData.position.z = storedSkyLaunchPosZ;

		actorData.position.x = storedSkyLaunchPosX;
		actorData.position.z = storedSkyLaunchPosZ;

	}
	else {
		if (!skyLaunchForceJustFrameToggledOff) {
			beginSkyLaunch = false;
			SetVolume(2, activeConfig.channelVolumes[2]);
			ToggleRoyalguardForceJustFrameRelease(activeConfig.Royalguard.forceJustFrameRelease);
			skyLaunchForceJustFrameToggledOff = true;
		}

		skyLaunchSetVolume = false;

	}
}

#pragma endregion

#pragma region DanteGameplay

template <typename T>
auto GetMeleeWeapon(T& actorData) {
	auto& characterData = GetCharacterData(actorData);

	return characterData.meleeWeapons[characterData.meleeWeaponIndex];
}

export void DelayedComboEffectsController() {
	using namespace ACTION_DANTE;

	/*if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);*/

	//Introduce Main Actor Data
	auto pool_12857 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if
		(
			!pool_12857 ||
			!pool_12857[3]
			) {
		return;
	}


	auto& actorData = *reinterpret_cast<PlayerActorData*>(pool_12857[3]);
	auto weapon = GetMeleeWeapon(actorData);

	auto inAttack = (actorData.eventData[0].event == 17);
	auto rebellionCombo1Anim = (actorData.motionData[0].index == 3);
	auto inRebellionCombo1 = (actorData.action == REBELLION_COMBO_1_PART_1 && actorData.motionData[0].index == 3 && inAttack);
	auto inCerberusCombo2 = (actorData.action == CERBERUS_COMBO_1_PART_2 && actorData.motionData[0].index == 4 && inAttack);
	auto inAgniCombo1 = (actorData.action == AGNI_RUDRA_COMBO_1_PART_1 && actorData.motionData[0].index == 3 && inAttack);
	auto inAgniCombo2 = (actorData.action == AGNI_RUDRA_COMBO_2_PART_2 && actorData.motionData[0].index == 8 && inAttack);
	auto inBeoCombo1 = (actorData.action == BEOWULF_COMBO_1_PART_2 && actorData.motionData[0].index == 4 && inAttack);
	auto meleeWeapon = actorData.newWeapons[actorData.meleeWeaponIndex];

	if (inRebellionCombo1) {
		delayedComboFX.duration = 0.495f;
		delayedComboFX.weaponThatStartedMove = 0;
	}
	else if (inCerberusCombo2) {
		delayedComboFX.duration = 0.55f;
		delayedComboFX.weaponThatStartedMove = 1;
	}
	else if (inAgniCombo1) {
		delayedComboFX.duration = 0.53f;
		delayedComboFX.weaponThatStartedMove = 2;
	}
	else if (inAgniCombo2) {
		delayedComboFX.duration = 0.70f;
		delayedComboFX.weaponThatStartedMove = 2;
	}
	else if (inBeoCombo1) {
		delayedComboFX.duration = 0.55f;  // Beowulf's time can be very inconsistent due to charge time (the more you charge the less you need to wait between delays)
		delayedComboFX.weaponThatStartedMove = 4;
	}




	if (actorData.character == CHARACTER::DANTE) {
		if (delayedComboFX.timer >= delayedComboFX.duration &&
			(inRebellionCombo1 || inCerberusCombo2 || inAgniCombo1 || inAgniCombo2 || inBeoCombo1) &&
			delayedComboFX.playCount == 0 && weapon == delayedComboFX.weaponThatStartedMove) {

			playDelayedCombo1();
			createEffectBank = delayedComboFX.bank;
			createEffectID = delayedComboFX.id;
			CreateEffectDetour();


			delayedComboFX.playCount++;
		}
		else if (delayedComboFX.timer < 0.495f) {
			delayedComboFX.playCount = 0;

		}

		if ((!inRebellionCombo1 && !inCerberusCombo2 && !inAgniCombo1 && !inAgniCombo2 && !inBeoCombo1)) {
			delayedComboFX.timer = 0;
			delayedComboFX.resetTimer = false;


		}
		else {
			if (!delayedComboFX.resetTimer) {
				crimsonPlayer[0].actionTimer = 0;
				delayedComboFX.resetTimer = true;
			}


			delayedComboFX.timer = crimsonPlayer[0].actionTimer;



		}




	}

}


export void DriveTweaks(byte8* actorBaseAddr) {
	// This function alters some of Drive, it alters its damage to accommodate new "Charge Levels", mimicing DMC4/5 Drive behaviour.

	using namespace ACTION_DANTE;

	if (!actorBaseAddr) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
	auto playerIndex = actorData.newPlayerIndex;

	// 	drive physical hit damage dmc3.exe + 5C6D2C, 70.0f
	// 	drive projectile damage dmc3.exe + 5CB1EC, 300.0f
	uintptr_t drivePhysicalDamageAddr = (uintptr_t)appBaseAddr + 0x5C6D2C;
	uintptr_t driveProjectileDamageAddr = (uintptr_t)appBaseAddr + 0x5CB1EC;

	// Triggering the Drive Timer
	if ((actorData.action == ACTION_DANTE::REBELLION_DRIVE_1) && (actorData.motionData[0].index == 17 || actorData.motionData[0].index == 18)) {
		crimsonPlayer[playerIndex].drive.runTimer = true;
	}

	// Fuck this shit, resetting has proved to be waaay more difficult than it should, probably due to SetAction things 
	if (actorData.motionData[0].index == 19 || actorData.motionData[0].index == 1 || actorData.motionData[0].index == 2
		|| actorData.motionData[0].index == 4 || actorData.motionData[0].index == 5 || actorData.motionData[0].index == 6
		|| actorData.motionData[0].index == 7 || actorData.motionData[0].index == 8 || actorData.motionData[0].index == 9 || actorData.motionData[0].index == 10) {

		crimsonPlayer[playerIndex].drive.runTimer = false;
	}

	if (actorData.action == ACTION_DANTE::REBELLION_DRIVE_1 && actorData.eventData[0].event != 17) {
		crimsonPlayer[playerIndex].drive.runTimer = false;
	}

	// Setting Quick Drive Damage
	if ((actorData.action == REBELLION_DRIVE_1) && crimsonPlayer[playerIndex].inQuickDrive && actorData.eventData[0].event == 17) {

		*(float*)(drivePhysicalDamageAddr) = 60.0f;
		*(float*)(driveProjectileDamageAddr) = 200.0f;
	}

	// The actual Drive Tweaks
	if ((actorData.action == REBELLION_DRIVE_1) && !crimsonPlayer[playerIndex].inQuickDrive && actorData.eventData[0].event == 17) {

		if (crimsonPlayer[playerIndex].drive.timer < 1.0f && crimsonPlayer[playerIndex].drive.level1EffectPlayed) {
			crimsonPlayer[playerIndex].drive.level1EffectPlayed = false;
			crimsonPlayer[playerIndex].drive.level2EffectPlayed = false;
			crimsonPlayer[playerIndex].drive.level3EffectPlayed = false;
		}

		if (crimsonPlayer[playerIndex].drive.timer >= 1.1f) {
			if (!crimsonPlayer[playerIndex].drive.level1EffectPlayed) {

				createEffectBank = crimsonPlayer[playerIndex].drive.bank;
				createEffectID = crimsonPlayer[playerIndex].drive.id;
				CreateEffectDetour();

				crimsonPlayer[playerIndex].drive.level1EffectPlayed = true;
			}
		}

		if (crimsonPlayer[playerIndex].drive.timer < 2.0) {
			*(float*)(drivePhysicalDamageAddr) = 70.0f;
			*(float*)(driveProjectileDamageAddr) = 200.0f;
		}

		if (crimsonPlayer[playerIndex].drive.timer >= 2.0 && crimsonPlayer[playerIndex].drive.timer < 3.0) {
			*(float*)(drivePhysicalDamageAddr) = 70.0f;
			*(float*)(driveProjectileDamageAddr) = 300.0f;

			if (!crimsonPlayer[playerIndex].drive.level2EffectPlayed) {

				createEffectBank = crimsonPlayer[playerIndex].drive.bank;
				createEffectID = crimsonPlayer[playerIndex].drive.id;
				CreateEffectDetour();

				crimsonPlayer[playerIndex].drive.level2EffectPlayed = true;
			}
		}

		if (crimsonPlayer[playerIndex].drive.timer >= 3.0) {
			*(float*)(drivePhysicalDamageAddr) = 70.0f;
			*(float*)(driveProjectileDamageAddr) = 700.0f;

			if (!crimsonPlayer[playerIndex].drive.level3EffectPlayed) {

				createEffectBank = crimsonPlayer[playerIndex].drive.bank;
				createEffectID = crimsonPlayer[playerIndex].drive.id;
				CreateEffectDetour();

				crimsonPlayer[playerIndex].drive.level3EffectPlayed = true;
			}
		}

	}

	// 	if (actorData.action != 13 && actorData.eventData[0].event != 17 ){
	// 		crimsonPlayer[playerIndex].driveVFX.level1EffectPlayed = false;
	// 		crimsonPlayer[playerIndex].driveVFX.level2EffectPlayed = false;
	// 		crimsonPlayer[playerIndex].driveVFX.level3EffectPlayed = false;
	// 	}
		//notHoldingMelee = (gamepad.buttons[0] & GetBinding(BINDING::MELEE_ATTACK));




		/* ((actorData.action == REBELLION_DRIVE_1) && actorData.eventData[0].event == 17 && notHoldingMelee == 0 && crimsonPlayer[playerIndex].actionTimer < 1.1f &&
			!crimsonPlayer[playerIndex].inQuickDrive) {




			actorData.state &= ~STATE::BUSY;




		}*/
}
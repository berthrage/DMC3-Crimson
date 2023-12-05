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
#include "DetourFunctions.hpp"
#include "DMC3Input.hpp"
#include "File.hpp"
#include "Internal.hpp"
#include "ActorBase.hpp"

// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "CrimsonGameplay.hpp"
#include "Core/Core.hpp"
#include "ExtraSound.hpp"
#include "Memory.hpp"
#include "Model.hpp"
#include "PatchFunctions.hpp"
#include "ActorRelocations.hpp"
#include "Config.hpp"
#include "Exp.hpp"
#include "Global.hpp"
#include "HUD.hpp"
#include "Sound.hpp"
#include "SoundRelocations.hpp"
#include "Speed.hpp"
#include "Vars.hpp"

#include "Core/Macros.h"


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

void ImprovedCancelsRoyalguardController(byte8* actorBaseAddr) {
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
            actorData.action == REBELLION_CRAZY_DANCE || actorData.action == POLE_PLAY);

    bool inCancellableActionCerberus =
        (actorData.action == CERBERUS_COMBO_1_PART_1 || actorData.action == CERBERUS_COMBO_1_PART_2 ||
            actorData.action == CERBERUS_COMBO_1_PART_3 || actorData.action == CERBERUS_COMBO_1_PART_4 ||
            actorData.action == CERBERUS_COMBO_1_PART_5 || actorData.action == CERBERUS_COMBO_2_PART_3 ||
            actorData.action == CERBERUS_COMBO_2_PART_4 || actorData.action == CERBERUS_WINDMILL ||
            actorData.action == CERBERUS_REVOLVER_LEVEL_1 || actorData.action == CERBERUS_REVOLVER_LEVEL_2 ||
            actorData.action == CERBERUS_SWING || actorData.action == CERBERUS_SATELLITE || actorData.action == CERBERUS_FLICKER ||
            actorData.action == CERBERUS_CRYSTAL || actorData.action == CERBERUS_MILLION_CARATS || actorData.action == CERBERUS_ICE_AGE);

    bool inCancellableActionAgni =
        (actorData.action == AGNI_RUDRA_COMBO_1_PART_1 || actorData.action == AGNI_RUDRA_COMBO_1_PART_2 ||
            actorData.action == AGNI_RUDRA_COMBO_1_PART_3 || actorData.action == AGNI_RUDRA_COMBO_1_PART_4 ||
            actorData.action == AGNI_RUDRA_COMBO_1_PART_5 || actorData.action == AGNI_RUDRA_COMBO_2_PART_2 ||
            actorData.action == AGNI_RUDRA_COMBO_2_PART_3 || actorData.action == AGNI_RUDRA_COMBO_3_PART_3 ||
            actorData.action == AGNI_RUDRA_JET_STREAM_LEVEL_1 || actorData.action == AGNI_RUDRA_JET_STREAM_LEVEL_2 ||
            actorData.action == AGNI_RUDRA_JET_STREAM_LEVEL_3 || actorData.action == AGNI_RUDRA_MILLION_SLASH ||
            actorData.action == AGNI_RUDRA_TWISTER || actorData.action == AGNI_RUDRA_TEMPEST);


    bool inCancellableActionNevan =
        (actorData.action == NEVAN_TUNE_UP || actorData.action == NEVAN_COMBO_1 || actorData.action == NEVAN_COMBO_2 ||
            actorData.action == NEVAN_JAM_SESSION || actorData.action == NEVAN_BAT_RIFT_LEVEL_1 ||
            actorData.action == NEVAN_BAT_RIFT_LEVEL_2 || actorData.action == NEVAN_REVERB_SHOCK_LEVEL_1 ||
            actorData.action == NEVAN_REVERB_SHOCK_LEVEL_2 || actorData.action == NEVAN_SLASH || actorData.action == NEVAN_FEEDBACK ||
            actorData.action == NEVAN_CRAZY_ROLL || actorData.action == NEVAN_DISTORTION);

    bool inCancellableActionBeowulf = (actorData.action == BEOWULF_COMBO_1_PART_1 || actorData.action == BEOWULF_COMBO_1_PART_2 ||
                                       actorData.action == BEOWULF_COMBO_1_PART_3 || actorData.action == BEOWULF_COMBO_2_PART_3 ||
                                       actorData.action == BEOWULF_COMBO_2_PART_4 || actorData.action == BEOWULF_BEAST_UPPERCUT ||
                                       actorData.action == BEOWULF_HYPER_FIST);

    bool inCancellableActionGuns =
        (actorData.action == EBONY_IVORY_WILD_STOMP || actorData.action == ARTEMIS_ACID_RAIN || actorData.action == KALINA_ANN_GRAPPLE);

    // These are moves used by the Action Set Cancel Method, generally air ones.
    bool inCancellableMovesActionMethod =
        (((actorData.action == REBELLION_AERIAL_RAVE_PART_1 || actorData.action == REBELLION_AERIAL_RAVE_PART_2 ||
              actorData.action == REBELLION_AERIAL_RAVE_PART_3 || actorData.action == REBELLION_AERIAL_RAVE_PART_4) ||
             (actorData.action == AGNI_RUDRA_SKY_DANCE_PART_1 || actorData.action == AGNI_RUDRA_SKY_DANCE_PART_2 ||
                 actorData.action == AGNI_RUDRA_SKY_DANCE_PART_3) ||
             (actorData.action == NEVAN_AIR_SLASH_PART_1 || actorData.action == NEVAN_AIR_SLASH_PART_2) ||
             (actorData.action == CERBERUS_AIR_FLICKER) || (actorData.action == BEOWULF_TORNADO)) &&
            actorData.eventData[0].event == 17);


    // Royalguard Cancels Everything (Most things)
    if ((actorData.style == STYLE::ROYALGUARD) && (actorData.buttons[2] & GetBinding(BINDING::STYLE_ACTION)) &&
        actorData.eventData[0].event != 44 &&
        (inCancellableActionRebellion || inCancellableActionCerberus || inCancellableActionAgni || inCancellableActionNevan ||
            inCancellableActionBeowulf || inCancellableActionGuns || actorData.eventData[0].event == 22) &&
        !royalCancelTrackerRunning) // The last condition prevents cancelling recovery
    {

        // Old list of exceptions, easier to list everything that should be cancellable.

        /*/if (actorData.action != SPIRAL_NORMAL_SHOT && actorData.action != KALINA_ANN_NORMAL_SHOT &&
        actorData.action != EBONY_IVORY_AIR_NORMAL_SHOT && actorData.action != SHOTGUN_AIR_NORMAL_SHOT &&
        actorData.action != SPIRAL_TRICK_SHOT && !royalCancelTrackerRunning) // Exceptions, these cancels are way too OP or buggy in the
        cases of E&I and Shotgun.*/


        storedTrickUpCount = actorData.newTrickUpCount;
        storedSkyStarCount = actorData.newSkyStarCount;
        storedAirHikeCount = actorData.newAirHikeCount;

        actorData.permissions = 3080; // This is a softer version of Reset Permissions.

        std::thread royalcountstracker(RoyalCancelCountsTracker, actorBaseAddr);
        royalcountstracker.detach();
    }

    // Royal Cancelling Sky Star
    if ((actorData.style == STYLE::ROYALGUARD) && (actorData.buttons[2] & GetBinding(BINDING::STYLE_ACTION)) &&
        actorData.eventData[0].event == 23 && !royalCancelTrackerRunning) {


        storedTrickUpCount = actorData.newTrickUpCount;
        storedSkyStarCount = actorData.newSkyStarCount;
        storedAirHikeCount = actorData.newAirHikeCount;

        actorData.permissions = 0x1C1B; // This is a hard version of Reset Permissions.

        std::thread royalcountstracker(RoyalCancelCountsTracker, actorBaseAddr);
        royalcountstracker.detach();
    }


    // This is another method for Royal Cancels that involves setting the Actor's Action to a newly created Air Block one (only sets for a
    // split second). It's more reliable for cancelling certain moves (especially air ones). - Mia

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
                if((!(lockOn && tiltDirection == TILT_DIRECTION::UP)) && (!(lockOn && tiltDirection == TILT_DIRECTION::DOWN) &&
        gamepad.buttons[0] & GetBinding(BINDING::STYLE_ACTION)))
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


void ImprovedCancelsDanteController(byte8* actorBaseAddr) {
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
    auto& actorData    = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    auto lockOn        = (actorData.buttons[0] & GetBinding(BINDING::LOCK_ON));
    auto tiltDirection = GetRelativeTiltDirection(actorData);


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
            actorData.action == CERBERUS_REVOLVER_LEVEL_1 || actorData.action == CERBERUS_REVOLVER_LEVEL_2 ||
            actorData.action == CERBERUS_SWING || actorData.action == CERBERUS_SATELLITE || actorData.action == CERBERUS_FLICKER ||
            actorData.action == CERBERUS_CRYSTAL || actorData.action == CERBERUS_MILLION_CARATS || actorData.action == CERBERUS_ICE_AGE);

    bool inCancellableActionAgni =
        (actorData.action == AGNI_RUDRA_COMBO_1_PART_1 || actorData.action == AGNI_RUDRA_COMBO_1_PART_2 ||
            actorData.action == AGNI_RUDRA_COMBO_1_PART_3 || actorData.action == AGNI_RUDRA_COMBO_1_PART_4 ||
            actorData.action == AGNI_RUDRA_COMBO_1_PART_5 || actorData.action == AGNI_RUDRA_COMBO_2_PART_2 ||
            actorData.action == AGNI_RUDRA_COMBO_2_PART_3 || actorData.action == AGNI_RUDRA_COMBO_3_PART_3 ||
            actorData.action == AGNI_RUDRA_JET_STREAM_LEVEL_1 || actorData.action == AGNI_RUDRA_JET_STREAM_LEVEL_2 ||
            actorData.action == AGNI_RUDRA_JET_STREAM_LEVEL_3 || actorData.action == AGNI_RUDRA_MILLION_SLASH ||
            actorData.action == AGNI_RUDRA_TWISTER || actorData.action == AGNI_RUDRA_TEMPEST ||
            actorData.action == AGNI_RUDRA_WHIRLWIND_LAUNCH);

    // || actorData.action == AGNI_RUDRA_CROSSED_SWORDS || actorData.action == AGNI_RUDRA_CRAWLER

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

        // Dante's Trickster Actions Cancels Most Things (w/ cooldown)
        if ((actorData.style == STYLE::TRICKSTER) &&
            (actorData.eventData[0].event != 22 &&
                    (inCancellableActionRebellion || inCancellableActionCerberus || inCancellableActionAgni || inCancellableActionNevan ||
                        inCancellableActionBeowulf || inCancellableActionGuns || inCancellableActionAirSwordmaster ||
                        inCancellableActionAirGunslinger || actorData.action == EBONY_IVORY_RAIN_STORM) ||
                executingSkyLaunch)) {
            if (gamepad.buttons[0] & GetBinding(BINDING::STYLE_ACTION)) {


                if (actorData.newEntityIndex == ENTITY::MAIN) {
                    if (crimsonPlayer[playerIndex].cancels.canTrick) {

                        actorData.state &= ~STATE::BUSY;

                        crimsonPlayer[playerIndex].cancels.canTrick = false;
                    }

                } else {
                    if (crimsonPlayer[playerIndex].cancelsClone.canTrick) {

                        actorData.state &= ~STATE::BUSY;

                        crimsonPlayer[playerIndex].cancelsClone.canTrick = false;
                    }
                }
            }
        }

        // Gunslinger Cancels Most Things (w/ cooldown)
        //  They can also cancel themselves.
        if ((actorData.style == STYLE::GUNSLINGER) && (actorData.state == STATE::IN_AIR || actorData.state == 65538) &&
            (inCancellableActionAirSwordmaster || inCancellableActionAirGunslinger || actorData.eventData[0].event == 23 ||
                actorData.eventData[0].event == ACTOR_EVENT::TRICKSTER_AIR_TRICK || actorData.motionData[0].index == 15) &&
            actorData.action != EBONY_IVORY_RAIN_STORM) {
            if (gamepad.buttons[0] & GetBinding(BINDING::STYLE_ACTION)) {

                if (actorData.newEntityIndex == ENTITY::MAIN) {

                    if (crimsonPlayer[playerIndex].cancels.canGun) {

                        actorData.state &= ~STATE::BUSY;

                        crimsonPlayer[playerIndex].cancels.canGun = false;
                    }

                } else {
                    if (crimsonPlayer[playerIndex].cancelsClone.canGun) {

                        actorData.state &= ~STATE::BUSY;

                        crimsonPlayer[playerIndex].cancelsClone.canGun = false;
                    }
                }
            }
        }

        // but Rainstorm is an exception here since I wanted it to have a longer CD.
        if ((actorData.style == STYLE::GUNSLINGER) && (actorData.state == STATE::IN_AIR || actorData.state == 65538) &&
            (crimsonPlayer[playerIndex].cancels.canRainstorm) && (actorData.action == EBONY_IVORY_RAIN_STORM)) {
            if (actorData.buttons[2] & GetBinding(BINDING::STYLE_ACTION)) {


                if (actorData.newEntityIndex == ENTITY::MAIN) {

                    if (crimsonPlayer[playerIndex].cancels.canRainstorm) {

                        actorData.state &= ~STATE::BUSY;

                        crimsonPlayer[playerIndex].cancels.canRainstorm = false;
                    }

                } else {
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

            } else {
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


            // Swordmaster moves cancel out Trickster dashes
            uint32 eventActor = actorData.eventData[0].event;

            if ((actorData.style == STYLE::SWORDMASTER) &&
                (eventActor == ACTOR_EVENT::TRICKSTER_SKY_STAR || eventActor == ACTOR_EVENT::TRICKSTER_DASH)) {
                if (actorData.buttons[2] & GetBinding(BINDING::STYLE_ACTION)) {

                    if (execute) {
                        execute = false;

                        actorData.state &= ~STATE::BUSY;
                    }
                } else {
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
                } else {
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


void ImprovedCancelsVergilController(byte8* actorBaseAddr) {
    using namespace ACTION_VERGIL;

    if (!actorBaseAddr || (actorBaseAddr == g_playerActorBaseAddrs[0]) || (actorBaseAddr == g_playerActorBaseAddrs[1])) {
        return;
    }

    if (!actorBaseAddr) {
        return;
    }
    auto& actorData    = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    auto lockOn        = (actorData.buttons[0] & GetBinding(BINDING::LOCK_ON));
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

        // Darkslayer Trick Cancels Everything
        if (actorData.character == CHARACTER::VERGIL && actorData.state != STATE::IN_AIR && actorData.state != 65538) {
            if (gamepad.buttons[0] & GetBinding(BINDING::STYLE_ACTION)) {
                if (execute) {
                    execute = false;

                    actorData.state &= ~STATE::BUSY;
                }
            } else {
                execute = true;
            }
        }

        // TRICK UP
        if (actorData.character == CHARACTER::VERGIL && actorData.state & STATE::IN_AIR) {
            if (gamepad.buttons[0] & GetBinding(BINDING::STYLE_ACTION) && lockOn && tiltDirection == TILT_DIRECTION::UP &&
                actorData.trickUpCount > 0) {
                if (execute) {
                    execute = false;

                    actorData.state &= ~STATE::BUSY;
                }
            } else {
                execute = true;
            }
        }

        // TRICK DOWN
        if (actorData.character == CHARACTER::VERGIL && actorData.state & STATE::IN_AIR) {
            if (gamepad.buttons[0] & GetBinding(BINDING::STYLE_ACTION) && lockOn && tiltDirection == TILT_DIRECTION::DOWN &&
                actorData.trickDownCount > 0) {
                if (execute) {
                    execute = false;

                    actorData.state &= ~STATE::BUSY;
                }
            } else {
                execute = true;
            }
        }

        // AIR TRICK
        if (actorData.character == CHARACTER::VERGIL && actorData.state & STATE::IN_AIR) {
            if (gamepad.buttons[0] & GetBinding(BINDING::STYLE_ACTION) && (lockOn && tiltDirection == TILT_DIRECTION::NEUTRAL || !lockOn) &&
                actorData.airTrickCount > 0) {
                if (execute) {
                    execute = false;

                    actorData.state &= ~STATE::BUSY;
                }
            } else {
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

#pragma region VergilGameplay

void VergilAdjustAirMovesPos(byte8* actorBaseAddr) {
    // This is for adjusting DDMK's Vergil's Air Moves positions, so that their startup position is more correct. - Mia

    using namespace ACTION_VERGIL;

    if (!actorBaseAddr) {
        return;
    }
    auto& actorData  = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    auto playerIndex = actorData.newPlayerIndex;
    auto& gamepad    = GetGamepad(actorData.newPlayerIndex);

    auto* v     = (actorData.newEntityIndex == 0) ? &crimsonPlayer[playerIndex].vergilMoves : &crimsonPlayer[playerIndex].vergilMovesClone;
    auto action = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].action : crimsonPlayer[playerIndex].actionClone;
    auto event  = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].event : crimsonPlayer[playerIndex].eventClone;
    auto motion = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].motion : crimsonPlayer[playerIndex].motionClone;
    auto state  = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].state : crimsonPlayer[playerIndex].stateClone;
    auto actionTimer =
        (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].actionTimer : crimsonPlayer[playerIndex].actionTimerClone;
    auto animTimer = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].animTimer : crimsonPlayer[playerIndex].animTimerClone;


    if (actorData.character == CHARACTER::VERGIL) {

        // Storing the pos
        if (action != BEOWULF_RISING_SUN) {
            v->storedRisingSunPosY = actorData.position.y;
        }

        if (!(state & STATE::IN_AIR && (action == BEOWULF_LUNAR_PHASE_LEVEL_1 || action == BEOWULF_LUNAR_PHASE_LEVEL_2) &&
                actorData.eventData[0].event == 17)) {

            v->storedLunarPhasePosY = actorData.position.y;
        }

        // Applying the pos
        if (event == ACTOR_EVENT::ATTACK && state & STATE::IN_AIR) {

            // Adjusts Vergil Pos to be lower when starting Air Rising Sun
            if (action == BEOWULF_RISING_SUN) {

                if (actionTimer <= 0.6f) {
                    actorData.verticalPullMultiplier = 0.0f;
                }

                actorData.position.y = v->storedRisingSunPosY - 50.0f;
            }

            // Adjusts Vergil Pos to be lower when starting Air Lunar Phase
            if ((actorData.action == BEOWULF_LUNAR_PHASE_LEVEL_1 || actorData.action == BEOWULF_LUNAR_PHASE_LEVEL_2) && motion != 23) {

                actorData.verticalPullMultiplier = 0.0f;
                actorData.position.y             = v->storedLunarPhasePosY - 20.0f;
            }
        }
    }
}


void CalculateAirStingerEndTime() {
    using namespace ACTION_DANTE;
    using namespace ACTION_VERGIL;

    auto pool_6046 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
    if (!pool_6046 || !pool_6046[3]) {
        return;
    }
    auto& actorData = *reinterpret_cast<PlayerActorData*>(pool_6046[3]);

    float airStingerEndTime = 100 / actorData.speed;
    airStingerEndTimeInt    = (int)airStingerEndTime + 1;
}

void AirStingerEndTracker(byte8* actorBaseAddr) {
    auto speedValue = (IsTurbo()) ? activeConfig.Speed.turbo : activeConfig.Speed.mainSpeed;
    if (!actorBaseAddr) {
        return;
    }
    auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);

    airStingerEnd.trackerRunning = true;
    airStingerEnd.timer          = 0;
    while (actorData.motionData[0].index == 11) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        airStingerEnd.timer++;
    }


    if (actorData.motionData[0].index != 11) {
        airStingerEnd.trackerRunning = false;
    }
}

void SetAirStingerEnd(byte8* actorBaseAddr) {
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

void FasterRapidSlashDevil(byte8* actorBaseAddr) {
    using namespace ACTION_DANTE;
    using namespace ACTION_VERGIL;
    if (!actorBaseAddr) {
        return;
    }
    auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);

    if ((actorData.motionData[0].index == 8 || actorData.motionData[0].index == 10) &&
        (actorData.action == YAMATO_RAPID_SLASH_LEVEL_1 || actorData.action == YAMATO_RAPID_SLASH_LEVEL_2)) {

        inRapidSlash = true;
    } else {
        inRapidSlash = false;
    }


    if (actorData.character == CHARACTER::VERGIL) {

        if ((actorData.motionData[0].index == 51 || actorData.motionData[0].index == 2) &&
            !inRapidSlash) { // Coudln't figure out a way to not bug this out then to store this out of walking anim
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
            } else if (!inRapidSlash && fasterRapidSlash.newSpeedSet) {


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


void FasterDarkslayerTricks() {
    auto pool_12311 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
    if (!pool_12311 || !pool_12311[3]) {
        return;
    }
    auto& actorData = *reinterpret_cast<PlayerActorData*>(pool_12311[3]);


    if (actorData.character == CHARACTER::VERGIL) {
        float storedspeedVergil = activeConfig.Speed.human;

        if ((actorData.motionData[0].index == 51 || actorData.motionData[0].index == 2) &&
            !(actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_AIR_TRICK ||
                actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_TRICK_UP ||
                actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_TRICK_DOWN)) {
            // Storing the original speeds
            fasterDarkslayer.storedSpeedHuman    = activeConfig.Speed.human;
            fasterDarkslayer.storedSpeedDevil[0] = activeConfig.Speed.devilVergil[0];
            fasterDarkslayer.storedSpeedDevil[1] = activeConfig.Speed.devilVergil[1];
            fasterDarkslayer.storedSpeedDevil[2] = activeConfig.Speed.devilVergil[2];
            fasterDarkslayer.storedSpeedDevil[3] = activeConfig.Speed.devilVergil[3];
        }

        if ((actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_AIR_TRICK ||
                actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_TRICK_UP ||
                actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_TRICK_DOWN) &&
            !fasterDarkslayer.newSpeedSet) {


            // Setting the new speed
            activeConfig.Speed.human          = fasterDarkslayer.newSpeed;
            activeConfig.Speed.devilVergil[0] = fasterDarkslayer.newSpeed;
            activeConfig.Speed.devilVergil[1] = fasterDarkslayer.newSpeed;
            activeConfig.Speed.devilVergil[2] = fasterDarkslayer.newSpeed;
            activeConfig.Speed.devilVergil[3] = fasterDarkslayer.newSpeed;

            fasterDarkslayer.newSpeedSet = true;
        } else if (!(actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_AIR_TRICK ||
                       actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_TRICK_UP ||
                       actorData.eventData[0].event == ACTOR_EVENT::DARK_SLAYER_TRICK_DOWN) &&
                   fasterDarkslayer.newSpeedSet) {

            // Restoring the original speeds
            activeConfig.Speed.human          = fasterDarkslayer.storedSpeedHuman;
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

void FreeRotationSwordMoves(byte8* actorBaseAddr) {
    // Allows you to freely rotate in the air while not locked on with aerial Swordmaster moves.
    // This is important for Inertia (Redirection) and as such both can only be enabled together.
    // Used to be called RemoveSoftLockOn. - Mia

    using namespace ACTION_DANTE;
    using namespace ACTION_VERGIL;
    using namespace ACTOR_EVENT;
    if (!actorBaseAddr) {
        return;
    }
    auto& actorData  = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    auto playerIndex = actorData.newPlayerIndex;

    auto lockOn          = (actorData.buttons[0] & GetBinding(BINDING::LOCK_ON));
    auto& gamepad        = GetGamepad(playerIndex);
    auto tiltDirection   = GetRelativeTiltDirection(actorData);
    auto radius          = gamepad.leftStickRadius;
    uint16 relativeTilt  = 0;
    relativeTilt         = (actorData.cameraDirection + gamepad.leftStickPosition);
    uint16 rotationStick = (relativeTilt - 0x8000);

    auto* i = &crimsonPlayer[playerIndex].inertia;
    auto actionTimer =
        (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].actionTimer : crimsonPlayer[playerIndex].actionTimerClone;

    bool inAerialRave = (actorData.action == REBELLION_AERIAL_RAVE_PART_1 || actorData.action == REBELLION_AERIAL_RAVE_PART_2 ||
                         actorData.action == REBELLION_AERIAL_RAVE_PART_3 || actorData.action == REBELLION_AERIAL_RAVE_PART_4);

    bool inSkyDance = (actorData.action == AGNI_RUDRA_SKY_DANCE_PART_1 || actorData.action == AGNI_RUDRA_SKY_DANCE_PART_2 ||
                       actorData.action == AGNI_RUDRA_SKY_DANCE_PART_3);

    bool inAir     = (actorData.state & STATE::IN_AIR);
    bool lastInAir = (actorData.lastState & STATE::IN_AIR);


    if (actorData.character == CHARACTER::DANTE) {

        if (inAerialRave) {

            if (!lockOn) {
                if (!(radius < RIGHT_STICK_DEADZONE)) {

                    actorData.rotation           = rotationStick;
                    i->aerialRave.cachedRotation = actorData.rotation;
                } else {
                    actorData.rotation = i->aerialRave.cachedRotation;
                }
            }

            if (lockOn) {
                if (actorData.eventData[0].event == JUMP_CANCEL || actorData.eventData[0].event == AIR_HIKE) {
                    actorData.rotation = i->aerialRave.cachedRotation;
                }
            }
        } else if (actorData.action == CERBERUS_AIR_FLICKER) {

            if (!lockOn) {
                if (!(radius < RIGHT_STICK_DEADZONE)) {

                    actorData.rotation           = rotationStick;
                    i->airFlicker.cachedRotation = actorData.rotation;
                } else {
                    actorData.rotation = i->airFlicker.cachedRotation;
                }
            }

            if (lockOn) {
                if (actorData.eventData[0].event == JUMP_CANCEL || actorData.eventData[0].event == AIR_HIKE) {
                    actorData.rotation = i->airFlicker.cachedRotation;
                }
            }
        } else if (inSkyDance) {

            if (!lockOn) {
                if (!(radius < RIGHT_STICK_DEADZONE)) {

                    actorData.rotation         = rotationStick;
                    i->skyDance.cachedRotation = actorData.rotation;
                } else {
                    actorData.rotation = i->skyDance.cachedRotation;
                }
            }

            if (lockOn) {
                if (actorData.eventData[0].event == JUMP_CANCEL || actorData.eventData[0].event == AIR_HIKE) {
                    actorData.rotation = i->skyDance.cachedRotation;
                }
            }
        } else if (actorData.action == NEVAN_AIR_SLASH_PART_1 || actorData.action == NEVAN_AIR_SLASH_PART_2) {

            if (!lockOn) {
                if (!(radius < RIGHT_STICK_DEADZONE)) {

                    actorData.rotation         = rotationStick;
                    i->airSlash.cachedRotation = actorData.rotation;
                } else {
                    actorData.rotation = i->airSlash.cachedRotation;
                }
            }

            if (lockOn) {
                if (actorData.eventData[0].event == JUMP_CANCEL || actorData.eventData[0].event == AIR_HIKE) {
                    actorData.rotation = i->airSlash.cachedRotation;
                }
            }

        } else if (actorData.action == BEOWULF_THE_HAMMER) {

            if (!lockOn) {
                if (!(radius < RIGHT_STICK_DEADZONE)) {

                    actorData.rotation          = rotationStick;
                    i->theHammer.cachedRotation = actorData.rotation;
                } else {
                    actorData.rotation = i->theHammer.cachedRotation;
                }
            }

            if (lockOn) {
                if (actorData.eventData[0].event == JUMP_CANCEL || actorData.eventData[0].event == AIR_HIKE) {
                    actorData.rotation = i->theHammer.cachedRotation;
                }
            }

        } else if (actorData.action == BEOWULF_KILLER_BEE) {


            // Keep Player's Rotation intact on jump cancelling, this is important for Inertia Redirection and is used for several moves.
            if (lockOn) {
                if (actorData.eventData[0].event == JUMP_CANCEL) {
                    actorData.rotation = i->killerBee.cachedRotation;
                }
            }
        } else if (actorData.airGuard) {

            if (!lockOn) {
                if (!(radius < RIGHT_STICK_DEADZONE)) {

                    actorData.rotation         = rotationStick;
                    i->airGuard.cachedRotation = actorData.rotation;
                } else {
                    actorData.rotation = i->airGuard.cachedRotation;
                }
            }

            if (lockOn) {
                if (actorData.eventData[0].event == JUMP_CANCEL) {
                    actorData.rotation = i->airGuard.cachedRotation;
                }
            }
        } else if (actorData.motionData[0].index == JUMP_CANCEL) {


            if (actorData.eventData[0].event == JUMP_CANCEL &&
                (actorData.action == REBELLION_AERIAL_RAVE_PART_1 || actorData.action == REBELLION_AERIAL_RAVE_PART_2 ||
                    actorData.action == REBELLION_AERIAL_RAVE_PART_3 || actorData.action == REBELLION_AERIAL_RAVE_PART_4)) {

                actorData.rotation = i->aerialRave.cachedRotation;
            } else if (actorData.eventData[0].event == JUMP_CANCEL &&
                       (actorData.action == AGNI_RUDRA_SKY_DANCE_PART_1 || actorData.action == AGNI_RUDRA_SKY_DANCE_PART_2 ||
                           actorData.action == AGNI_RUDRA_SKY_DANCE_PART_3)) {
                actorData.rotation = i->skyDance.cachedRotation;
            } else if (actorData.eventData[0].event == JUMP_CANCEL && (actorData.action == CERBERUS_AIR_FLICKER)) {
                actorData.rotation = i->airFlicker.cachedRotation;
            } else if (actorData.eventData[0].event == JUMP_CANCEL &&
                       (actorData.action == NEVAN_AIR_SLASH_PART_1 || actorData.action == NEVAN_AIR_SLASH_PART_2)) {
                actorData.rotation = i->airSlash.cachedRotation;
            } else if (actorData.eventData[0].event == JUMP_CANCEL && (actorData.action == BEOWULF_THE_HAMMER)) {
                actorData.rotation = i->theHammer.cachedRotation;
            } else if (actorData.eventData[0].event == JUMP_CANCEL && (actorData.action == BEOWULF_KILLER_BEE)) {
                actorData.rotation = i->killerBee.cachedRotation;
            } else if (actorData.eventData[0].lastEvent == JUMP_CANCEL) {
                actorData.rotation = i->skyStar.cachedRotation;
            }


        } else if (actorData.eventData[0].event == AIR_HIKE) {
            if (lockOn) {

                actorData.rotation = i->airHike.cachedRotation;
            }
        }


        if (actorData.action != BEOWULF_KILLER_BEE) {

            if (radius < RIGHT_STICK_DEADZONE) {
                i->killerBee.cachedRotation = actorData.rotation;
            }
        }

        if (actorData.eventData[0].event != ACTOR_EVENT::TRICKSTER_SKY_STAR) {
            if (radius < RIGHT_STICK_DEADZONE) {
                i->skyStar.cachedRotation = actorData.rotation;
            }
        }

        if (actorData.eventData[0].event != AIR_HIKE) {
            if (radius < RIGHT_STICK_DEADZONE) {
                i->airHike.cachedRotation = actorData.rotation;
            }
        }


    } else if (actorData.character == CHARACTER::VERGIL) {

        if (actorData.action == YAMATO_AERIAL_RAVE_PART_1 || actorData.action == YAMATO_AERIAL_RAVE_PART_2) {

            if (!lockOn) {
                if (!(radius < RIGHT_STICK_DEADZONE)) {

                    actorData.rotation           = rotationStick;
                    i->yamatoRave.cachedRotation = actorData.rotation;
                } else {
                    actorData.rotation = i->yamatoRave.cachedRotation;
                }
            }
        }
    }
}


void StoreInertia(byte8* actorBaseAddr) {
    // Here we store Momentum (Horizontal Pull) for Inertia and Rotation for FreeRotationSwordMoves. - Mia
    using namespace ACTION_DANTE;
    using namespace ACTION_VERGIL;
    // using namespace ACTOR_EVENT;


    if (!actorBaseAddr) {
        return;
    }
    auto& actorData  = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    auto playerIndex = actorData.newPlayerIndex;


    auto* i = (actorData.newEntityIndex == 0) ? &crimsonPlayer[playerIndex].inertia : &crimsonPlayer[playerIndex].inertiaClone;

    auto action = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].action : crimsonPlayer[playerIndex].actionClone;
    auto motion = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].motion : crimsonPlayer[playerIndex].motionClone;
    auto event  = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].event : crimsonPlayer[playerIndex].eventClone;
    auto state  = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].state : crimsonPlayer[playerIndex].stateClone;
    auto horizontalPull =
        (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].horizontalPull : crimsonPlayer[playerIndex].horizontalPullClone;

    bool inAerialRave = (action == REBELLION_AERIAL_RAVE_PART_1 || action == REBELLION_AERIAL_RAVE_PART_2 ||
                         action == REBELLION_AERIAL_RAVE_PART_3 || action == REBELLION_AERIAL_RAVE_PART_4);

    bool inSkyDance =
        (action == AGNI_RUDRA_SKY_DANCE_PART_1 || action == AGNI_RUDRA_SKY_DANCE_PART_2 || action == AGNI_RUDRA_SKY_DANCE_PART_3);


    // Part on an idea for a bufferless high time reversal - discarded
    // 		if (!(actorData.action == REBELLION_HIGH_TIME ||
    // 			actorData.action == REBELLION_HIGH_TIME_LAUNCH)) {
    // 			highTimeRotation = actorData.rotation;
    // 		}


    if (motion != 17 && motion != ACTOR_EVENT::JUMP_CANCEL) {

        if (!inAerialRave) {
            i->aerialRave.cachedPull     = horizontalPull;
            i->aerialRave.cachedRotation = actorData.rotation;
        }

        if (crimsonPlayer[playerIndex].action != CERBERUS_AIR_FLICKER) {
            i->airFlicker.cachedPull     = horizontalPull;
            i->airFlicker.cachedRotation = actorData.rotation;
        }

        if (!inSkyDance) {
            i->skyDance.cachedPull     = horizontalPull;
            i->skyDance.cachedRotation = actorData.rotation;
        }

        if (!(action == NEVAN_AIR_SLASH_PART_1 || action == NEVAN_AIR_SLASH_PART_2)) {

            i->airSlash.cachedPull     = horizontalPull;
            i->airSlash.cachedRotation = actorData.rotation;
        }

        if (action != BEOWULF_THE_HAMMER) {
            i->theHammer.cachedPull     = horizontalPull;
            i->theHammer.cachedRotation = actorData.rotation;
        }

        if (action != BEOWULF_TORNADO) {
            i->tornado.cachedPull = horizontalPull;
        }

        if (action != EBONY_IVORY_RAIN_STORM) {
            i->rainstorm.cachedPull = horizontalPull;
        }


        if (action != SHOTGUN_AIR_FIREWORKS) {
            i->fireworks.cachedPull = horizontalPull;
        }

        if (action != EBONY_IVORY_AIR_NORMAL_SHOT) {
            i->ebonyShot.cachedPull = horizontalPull;
        }

        if (action != SHOTGUN_AIR_NORMAL_SHOT) {
            i->shotgunShot.cachedPull = horizontalPull;
        }

        if (action != ARTEMIS_AIR_NORMAL_SHOT) {
            i->artemisShot.cachedPull = horizontalPull;
        }

        if (action != ARTEMIS_AIR_MULTI_LOCK_SHOT) {
            i->artemisMultiLockShot.cachedPull = horizontalPull;
        }

        if (!actorData.airGuard) {
            i->airGuard.cachedRotation = actorData.rotation;
        }

        // Important for Sky Star Guardflying.
        if (!actorData.airGuard && event != ACTOR_EVENT::JUMP_CANCEL && event != ACTOR_EVENT::AIR_HIKE) {
            if (event == ACTOR_EVENT::TRICKSTER_SKY_STAR) {
                i->airGuard.cachedPull = 28.0f;
            } else {
                i->airGuard.cachedPull = 28.0f;
            }
        }

        if (!(actorData.action == YAMATO_AERIAL_RAVE_PART_1 || actorData.action == YAMATO_AERIAL_RAVE_PART_2)) {

            i->yamatoRave.cachedRotation = actorData.rotation;
        }

        // TODO: Make Dante's Air Taunt available for all players (hard af) and separate this chunk into its own function. - Mia
        if (!(action == 195 && event == ACTOR_EVENT::ATTACK && state & STATE::IN_AIR) && playerIndex == 0 &&
            actorData.newEntityIndex == 0) {
            storedSkyLaunchPosX        = actorData.position.x;
            storedSkyLaunchPosY        = actorData.position.y;
            storedSkyLaunchPosZ        = actorData.position.z;
            storedSkyLaunchRank        = actorData.styleData.rank;
            appliedSkyLaunchProperties = false;
        }
    }

    // Old Gun Shoot Redirection - discarded.

    // 	auto inAirShot = (actorData.action == EBONY_IVORY_AIR_NORMAL_SHOT || actorData.action == SHOTGUN_AIR_NORMAL_SHOT ||
    // 		actorData.action == ARTEMIS_AIR_NORMAL_SHOT || actorData.action == ARTEMIS_AIR_MULTI_LOCK_SHOT);


    // 	if (((actorData.action == REBELLION_AERIAL_RAVE_PART_1 ||
    // 		actorData.action == REBELLION_AERIAL_RAVE_PART_2 ||
    // 		actorData.action == REBELLION_AERIAL_RAVE_PART_3 ||
    // 		actorData.action == REBELLION_AERIAL_RAVE_PART_4 ||
    // 		actorData.action == AGNI_RUDRA_SKY_DANCE_PART_1 ||
    // 		actorData.action == AGNI_RUDRA_SKY_DANCE_PART_2 ||
    // 		actorData.action == AGNI_RUDRA_SKY_DANCE_PART_3) &&
    // 		(actorData.lastAction == EBONY_IVORY_AIR_NORMAL_SHOT || actorData.lastAction == SHOTGUN_AIR_NORMAL_SHOT ||
    // 			actorData.lastAction == ARTEMIS_AIR_NORMAL_SHOT || actorData.lastAction == ARTEMIS_AIR_MULTI_LOCK_SHOT)) ||
    //
    // 		(actorData.lastAction == REBELLION_AERIAL_RAVE_PART_1 ||
    // 			actorData.lastAction == REBELLION_AERIAL_RAVE_PART_2 ||
    // 			actorData.lastAction == REBELLION_AERIAL_RAVE_PART_3 ||
    // 			actorData.lastAction == REBELLION_AERIAL_RAVE_PART_4 ||
    // 			actorData.lastAction == AGNI_RUDRA_SKY_DANCE_PART_1 ||
    // 			actorData.lastAction == AGNI_RUDRA_SKY_DANCE_PART_2 ||
    // 			actorData.lastAction == AGNI_RUDRA_SKY_DANCE_PART_3) &&
    // 		(actorData.lastAction == EBONY_IVORY_AIR_NORMAL_SHOT || actorData.action == SHOTGUN_AIR_NORMAL_SHOT ||
    // 			actorData.action == ARTEMIS_AIR_NORMAL_SHOT || actorData.action == ARTEMIS_AIR_MULTI_LOCK_SHOT)) {
    //
    //
    // 		inGunShoot = true;
    // 	}
    //
    // 	if (!(actorData.state & STATE::IN_AIR) || actorData.eventData[0].event == ACTOR_EVENT::TRICKSTER_AIR_TRICK) {
    // 		inGunShoot = false;
    // 	}
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

    auto action     = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].action : crimsonPlayer[playerIndex].actionClone;
    auto lastAction = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].lastAction : crimsonPlayer[playerIndex].lastActionClone;
    auto motion     = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].motion : crimsonPlayer[playerIndex].motionClone;
    auto event      = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].event : crimsonPlayer[playerIndex].eventClone;
    auto lastLastEvent =
        (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].lastLastEvent : crimsonPlayer[playerIndex].lastLastEventClone;
    auto state = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].state : crimsonPlayer[playerIndex].stateClone;
    auto lastLastState =
        (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].lastLastState : crimsonPlayer[playerIndex].lastLastStateClone;

    bool inAerialRave = (action == REBELLION_AERIAL_RAVE_PART_1 || action == REBELLION_AERIAL_RAVE_PART_2 ||
                         action == REBELLION_AERIAL_RAVE_PART_3 || action == REBELLION_AERIAL_RAVE_PART_4);

    bool inSkyDance =
        (action == AGNI_RUDRA_SKY_DANCE_PART_1 || action == AGNI_RUDRA_SKY_DANCE_PART_2 || action == AGNI_RUDRA_SKY_DANCE_PART_3);


    if (actorData.character == CHARACTER::DANTE) {

        // Guardfly
        if (motion == 5 && actorData.airGuard && (event == JUMP_CANCEL || event == AIR_HIKE)) {
            actorData.horizontalPull         = i->airGuard.cachedPull;
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
                actorData.verticalPullMultiplier   = -1.7f; // Experimental: tweaking Shotgun Gravity to be higher.
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
                if (i->theHammer.cachedPull < 0) {
                    i->theHammer.cachedPull = i->theHammer.cachedPull * -1.0f;
                }

                i->theHammer.cachedPull  = glm::clamp(i->theHammer.cachedPull, -9.0f, 9.0f);
                actorData.horizontalPull = (i->theHammer.cachedPull / 1.5f) * 1.0f;

            }

            // Tornado
            else if (action == BEOWULF_TORNADO) {
                if (i->tornado.cachedPull < 0) {
                    i->tornado.cachedPull = i->tornado.cachedPull * -1.0f;
                }

                i->tornado.cachedPull    = glm::clamp(i->tornado.cachedPull, -9.0f, 9.0f);
                actorData.horizontalPull = (i->tornado.cachedPull / 1.5f) * 1.0f;

            }

            // Killer Bee
//             else if (action == BEOWULF_KILLER_BEE && state == 65538 && event != 33) {
// 
//                 // Makes divekick speed be consistent, important for Guardflying.
//                 actorData.horizontalPull = 24.0f;
//             }
        }
    }
}

void AerialRaveGravityTweaks(byte8* actorBaseAddr) {
    // Reduces gravity while air raving, while also adding weights into the equation. - Mia
    using namespace ACTION_DANTE;

    if (!actorBaseAddr) {
        return;
    }
    auto& actorData  = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    auto playerIndex = actorData.newPlayerIndex;

    auto* tweak    = &crimsonPlayer[playerIndex].airRaveTweak;
    auto action    = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].action : crimsonPlayer[playerIndex].actionClone;
    auto event     = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].event : crimsonPlayer[playerIndex].eventClone;
    auto state     = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].state : crimsonPlayer[playerIndex].stateClone;
    auto animTimer = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].animTimer : crimsonPlayer[playerIndex].animTimerClone;

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
                    actorData.verticalPull           = tweak->gravity + (-0.2f * actorData.airSwordAttackCount);
                    actorData.verticalPullMultiplier = 0;
                    tweak->gravity4Changed           = false;
                }
            } else if (action == REBELLION_AERIAL_RAVE_PART_4) {
                if (!tweak->gravity4Changed) {

                    tweak->gravity += -1.5f;
                    tweak->gravity4Changed = true;
                }
                actorData.verticalPull           = tweak->gravity + (-0.2f * actorData.airSwordAttackCount);
                actorData.verticalPullMultiplier = 0;
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
    auto& actorData  = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    auto playerIndex = actorData.newPlayerIndex;
    auto& gamepad    = GetGamepad(actorData.newPlayerIndex);

    auto* tweak     = &crimsonPlayer[playerIndex].airRaveTweak;
    auto action     = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].action : crimsonPlayer[playerIndex].actionClone;
    auto lastAction = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].lastAction : crimsonPlayer[playerIndex].lastActionClone;
    auto event      = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].event : crimsonPlayer[playerIndex].eventClone;
    auto motion     = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].motion : crimsonPlayer[playerIndex].motionClone;
    auto state      = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].state : crimsonPlayer[playerIndex].stateClone;
    auto actionTimer =
        (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].actionTimer : crimsonPlayer[playerIndex].actionTimerClone;
    auto animTimer = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].animTimer : crimsonPlayer[playerIndex].animTimerClone;


    if (event == ACTOR_EVENT::ATTACK && state & STATE::IN_AIR && actorData.character == CHARACTER::DANTE) {

        if (action == CERBERUS_AIR_FLICKER) {


            if (motion == 7) {

                // Reduces Gravity Fall-off
                actorData.verticalPullMultiplier = -0.05 + (-0.15f * actorData.airSwordAttackCount); // Vanilla value is -0.27f
            } else {

                if ((actorData.style == STYLE::SWORDMASTER) && gamepad.buttons[0] & GetBinding(BINDING::STYLE_ACTION)) {

                    actorData.state &= ~STATE::BUSY; // Allows you to cancel into another Flicker during the falling animation.
                }

                // Reduces Gravity Fall-off
                actorData.verticalPullMultiplier = -1.5f;
            }
        }

        // Fix for the weird carry over to EbonyIvory Normal Shot
        if (action == 132 && lastAction == CERBERUS_AIR_FLICKER) {
            actorData.verticalPullMultiplier = -1.2f;
        }
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
    auto& actorData  = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    auto playerIndex = actorData.newPlayerIndex;
    auto& gamepad    = GetGamepad(actorData.newPlayerIndex);

    auto* tweak     = &crimsonPlayer[playerIndex].airRaveTweak;
    auto action     = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].action : crimsonPlayer[playerIndex].actionClone;
    auto lastAction = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].lastAction : crimsonPlayer[playerIndex].lastActionClone;
    auto event      = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].event : crimsonPlayer[playerIndex].eventClone;
    auto lastEvent  = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].lastEvent : crimsonPlayer[playerIndex].lastEventClone;
    auto motion     = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].motion : crimsonPlayer[playerIndex].motionClone;
    auto state      = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].state : crimsonPlayer[playerIndex].stateClone;
    auto actionTimer =
        (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].actionTimer : crimsonPlayer[playerIndex].actionTimerClone;
    auto animTimer = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].animTimer : crimsonPlayer[playerIndex].animTimerClone;

    bool inSkyDance = (action == AGNI_RUDRA_SKY_DANCE_PART_1 || action == AGNI_RUDRA_SKY_DANCE_PART_2);

    bool inSkyDanceLastAction = (lastAction == AGNI_RUDRA_SKY_DANCE_PART_1 || lastAction == AGNI_RUDRA_SKY_DANCE_PART_2);


    if (event == ACTOR_EVENT::ATTACK && state & STATE::IN_AIR && actorData.character == CHARACTER::DANTE) {

        if (inSkyDance) {

            if (actionTimer < 0.6f) {
                actorData.verticalPullMultiplier = (-0.04f * (actorData.airSwordAttackCount * 1.6f)); // Vanilla value is -0.04f
            } else {
                actorData.verticalPullMultiplier = -1.5f; // Vanilla value is -0.04f
            }
        }


        // Fix for the weird carry over to EbonyIvory Normal Shot
        if (action == 132 && inSkyDanceLastAction) {
            actorData.verticalPullMultiplier = -1.2f;
        }
    }
}

#pragma endregion

#pragma region GeneralGameplay

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

    actorData.styleData.rank = 1;
    actorData.styleData.meter = 699.0f;
}

void inCombatDetectionTracker() {
    inCombatTrackerRunning = true;
    inCombatTime           = inCombatDelay;
    while (inCombatTime > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        inCombatTime--;
    }


    if (inCombatTime == 0) {
        inCombatTrackerRunning = false;
        inCombat               = false;
    }

}

void inCombatDetection() {
    auto pool_12346 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
    if (!pool_12346 || !pool_12346[8]) {
        return;
    }
    auto& enemyVectorData = *reinterpret_cast<EnemyVectorData*>(pool_12346[8]);


    if (g_scene != SCENE::GAME) {
        inCombat = false;
    } else {
        if (enemyVectorData.count >= 1) {
            inCombat = true;
        } else if (enemyVectorData.count == 0 && !inCombatTrackerRunning && inCombat) {
            std::thread incombatdetectiontracker(inCombatDetectionTracker);
            incombatdetectiontracker.detach();
        }
    }
}


void DTReadySFX() {
    auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

    auto pool_12405 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
    if (!pool_12405 || !pool_12405[3]) {
        return;
    }
    auto& actorData = *reinterpret_cast<PlayerActorData*>(pool_12405[3]);


    if (actorData.magicPoints >= 3000 && !devilTriggerReadyPlayed) {
        PlayDevilTriggerReady();
        devilTriggerReadyPlayed = true;
    } else if (actorData.magicPoints < 3000) {
        devilTriggerReadyPlayed = false;
    }
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
            crimsonPlayer[playerIndex].b2F.backCommand          = true;
            crimsonPlayer[playerIndex].b2F.backDirectionChanged = false;
        }
    } else if (!(lockOn && tiltDirection == TILT_DIRECTION::DOWN && (radius > RIGHT_STICK_DEADZONE))) {
        crimsonPlayer[playerIndex].b2F.backDirectionChanged = true;
    }


    if (lockOn && tiltDirection == TILT_DIRECTION::UP && (radius > RIGHT_STICK_DEADZONE) && crimsonPlayer[playerIndex].b2F.backCommand) {
        if (crimsonPlayer[playerIndex].b2F.forwardBuffer > 0) {
            crimsonPlayer[playerIndex].b2F.forwardCommand          = true;
            crimsonPlayer[playerIndex].b2F.forwardDirectionChanged = false;
        }
    } else if (!(lockOn && tiltDirection == TILT_DIRECTION::UP && (radius > RIGHT_STICK_DEADZONE))) {
        crimsonPlayer[playerIndex].b2F.forwardDirectionChanged = true;
    }
}


void SprintAbility(byte8* actorBaseAddr) {


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


    auto& gamepad    = GetGamepad(actorData.newPlayerIndex);
    auto radius      = gamepad.leftStickRadius;
    auto& playerData = GetPlayerData(actorData);

    auto playerIndex = actorData.newPlayerIndex;

    if ((actorData.newCharacterIndex == playerData.activeCharacterIndex) && (actorData.newEntityIndex == ENTITY::MAIN)) {

        // auto playerSprint = crimsonPlayer[playerIndex].sprint;

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
            } else if (actorData.character == CHARACTER::VERGIL) {
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


        } else {
            crimsonPlayer[playerIndex].sprint.canSprint = false;
            crimsonPlayer[playerIndex].sprint.runTimer  = false;
        }


        auto gameSpeedValue = (IsTurbo()) ? activeConfig.Speed.turbo : activeConfig.Speed.mainSpeed;

        if (!crimsonPlayer[playerIndex].sprint.runTimer && crimsonPlayer[playerIndex].sprint.timer <= 0) {
        }


        if (crimsonPlayer[playerIndex].sprint.canSprint) {

            // Setting the sprint speed. Increasing by 30%.
            float sprintMultiplier = 1.3f;
            float sprintSpeed      = crimsonPlayer[playerIndex].sprint.storedSpeedHuman * sprintMultiplier;
            float sprintSpeedDevilDante[6];
            float sprintSpeedDevilVergil[5];

            if (actorData.character == CHARACTER::DANTE) {
                sprintSpeedDevilDante[0] = crimsonPlayer[playerIndex].sprint.storedSpeedDevilDante[0] * sprintMultiplier;
                sprintSpeedDevilDante[1] = crimsonPlayer[playerIndex].sprint.storedSpeedDevilDante[1] * sprintMultiplier;
                sprintSpeedDevilDante[2] = crimsonPlayer[playerIndex].sprint.storedSpeedDevilDante[2] * sprintMultiplier;
                sprintSpeedDevilDante[3] = crimsonPlayer[playerIndex].sprint.storedSpeedDevilDante[3] * sprintMultiplier;
                sprintSpeedDevilDante[4] = crimsonPlayer[playerIndex].sprint.storedSpeedDevilDante[4] * sprintMultiplier;
                sprintSpeedDevilDante[5] = crimsonPlayer[playerIndex].sprint.storedSpeedDevilDante[5] * sprintMultiplier;
            } else if (actorData.character == CHARACTER::VERGIL) {
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


            if (!crimsonPlayer[playerIndex].sprint.SFXPlayed) {
                playSprint();
                crimsonPlayer[playerIndex].sprint.SFXPlayed = true;
            }

            if (!crimsonPlayer[playerIndex].sprint.VFXPlayed) {
                createEffectBank = sprintVFX.bank;
                createEffectID   = sprintVFX.id;
                CreateEffectDetour();

                crimsonPlayer[playerIndex].sprint.VFXPlayed = true;
            }

            crimsonPlayer[playerIndex].sprint.isSprinting = true;
            crimsonPlayer[playerIndex].sprint.runTimer    = false;


        } else {
            // Restore the original Actor's speed when you can't sprint (either in or out of it).

            crimsonPlayer[playerIndex].sprint.isSprinting = false;
        }
    }
}


void GunDTCharacterRemaps() {
    // this is for Dante/Vergil gun and DT remaps
    // remaps are global for all controllers and only take into account player 1's current character.

    if (!crimsonPlayer[0].playerPtr) {
        return;
    }


    static uint16_t* currentDTButton    = (uint16_t*)(appBaseAddr + 0xD6CE9A);
    static uint16_t* currentShootButton = (uint16_t*)(appBaseAddr + 0xD6CE98);
    if (crimsonPlayer[0].character == CHARACTER::DANTE) {
        *currentDTButton    = activeConfig.Remaps.danteDTButton;
        *currentShootButton = activeConfig.Remaps.danteShootButton;
    } else if (crimsonPlayer[0].character == CHARACTER::VERGIL) {
        *currentDTButton    = activeConfig.Remaps.vergilDTButton;
        *currentShootButton = activeConfig.Remaps.vergilShootButton;
    }
}

#pragma endregion

#pragma region DanteAirTaunt


void RoyalReleaseTracker() {
    auto pool_12501 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
    if (!pool_12501 || !pool_12501[3]) {
        return;
    }
    auto& actorData = *reinterpret_cast<PlayerActorData*>(pool_12501[3]);


    if ((actorData.action == 195 || actorData.action == 194 || actorData.action == 196 || actorData.action == 197) &&
        (actorData.motionData[0].index == 20 || actorData.motionData[0].index == 19)) {

        executingRoyalRelease      = true;
        royalReleaseTrackerRunning = true;
    }
    skyLaunchSetJustFrameTrue = false;
}

void CheckRoyalRelease(byte8* actorBaseAddr) {
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
        executingRoyalRelease      = false;
        royalReleaseTrackerRunning = false;
    }

    if (!royalReleaseTrackerRunning) {
        executingRoyalRelease = false;
    }
}

void SkyLaunchTracker() {
    auto pool_12544 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
    if (!pool_12544 || !pool_12544[3]) {
        return;
    }
    auto& actorData = *reinterpret_cast<PlayerActorData*>(pool_12544[3]);


    if ((actorData.action == 195 || actorData.action == 194 || actorData.action == 212) && (actorData.motionData[0].index == 20)) {

        executingSkyLaunch      = true;
        skyLaunchTrackerRunning = true;
        // ToggleCerberusDamage(true);

        /*if(!executingSkyLaunch || !skyLaunchTrackerRunning) {
                break;
        }*/
    }
}

void CheckSkyLaunch(byte8* actorBaseAddr) {
    if (!actorBaseAddr) {
        return;
    }
    auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);

    if (((actorData.state & STATE::IN_AIR && actorData.motionData[0].index == 20) && (actorData.action == 195) &&
            actorData.buttons[0] & GetBinding(BINDING::TAUNT) && !skyLaunchTrackerRunning && !executingRoyalRelease)) {


        std::thread skylaunchtracker(SkyLaunchTracker);
        skylaunchtracker.detach();
    }

    if (!((actorData.action == 195 || actorData.action == 194) && (actorData.motionData[0].index == 20))) {
        executingSkyLaunch      = false;
        skyLaunchTrackerRunning = false;
        // ToggleCerberusDamage(activeConfig.infiniteHitPoints);
    }

    if (!skyLaunchTrackerRunning) {
        executingSkyLaunch = false;
    }
}

void SkyLaunchProperties(byte8* actorBaseAddr) {
    if (!actorBaseAddr) {
        return;
    }
    auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);

    auto playerIndex = actorData.newPlayerIndex;
    auto action      = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].action : crimsonPlayer[playerIndex].actionClone;
    auto state       = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].state : crimsonPlayer[playerIndex].stateClone;

    if (actorData.character == CHARACTER::DANTE) {


        if (actorData.state & STATE::IN_AIR && !skyLaunchSetJustFrameTrue && !forcingJustFrameRoyalRelease) {
            ToggleRoyalguardForceJustFrameRelease(true);
            skyLaunchSetJustFrameTrue   = true;
            skyLaunchSetJustFrameGround = false;
            royalReleaseJustFrameCheck  = false;
        }

        if (!executingRoyalRelease && skyLaunchSetJustFrameTrue) {
            skyLaunchSetJustFrameTrue = false;
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

        actorData.position.x     = storedSkyLaunchPosX;
        actorData.position.z     = storedSkyLaunchPosZ;
        actorData.styleData.rank = storedSkyLaunchRank;

        if (!skyLaunchSetVolume) {
            SetVolume(2, 0);
            skyLaunchSetVolume = true;
        }

        if (!appliedSkyLaunchProperties) {
            skyLaunchForceJustFrameToggledOff = false;


            // actorData.position.y = storedSkyLaunchPosY;
            appliedSkyLaunchProperties = true;
        }

        actorData.horizontalPull         = 0;
        actorData.verticalPullMultiplier = -0.2f;

        actorData.position.x = storedSkyLaunchPosX;
        actorData.position.z = storedSkyLaunchPosZ;

        actorData.position.x = storedSkyLaunchPosX;
        actorData.position.z = storedSkyLaunchPosZ;

    } else {
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

template <typename T> auto GetMeleeWeapon(T& actorData) {
    auto& characterData = GetCharacterData(actorData);

    return characterData.meleeWeapons[characterData.meleeWeaponIndex];
}

void DelayedComboEffectsController() {
    using namespace ACTION_DANTE;

    /*if (!actorBaseAddr) {
            return;
    }
    auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);*/

    // Introduce Main Actor Data
    auto pool_12857 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
    if (!pool_12857 || !pool_12857[3]) {
        return;
    }


    auto& actorData = *reinterpret_cast<PlayerActorData*>(pool_12857[3]);
    auto weapon     = GetMeleeWeapon(actorData);

    auto inAttack            = (actorData.eventData[0].event == 17);
    auto rebellionCombo1Anim = (actorData.motionData[0].index == 3);
    auto inRebellionCombo1   = (actorData.action == REBELLION_COMBO_1_PART_1 && actorData.motionData[0].index == 3 && inAttack);
    auto inCerberusCombo2    = (actorData.action == CERBERUS_COMBO_1_PART_2 && actorData.motionData[0].index == 4 && inAttack);
    auto inAgniCombo1        = (actorData.action == AGNI_RUDRA_COMBO_1_PART_1 && actorData.motionData[0].index == 3 && inAttack);
    auto inAgniCombo2        = (actorData.action == AGNI_RUDRA_COMBO_2_PART_2 && actorData.motionData[0].index == 8 && inAttack);
    auto inBeoCombo1         = (actorData.action == BEOWULF_COMBO_1_PART_2 && actorData.motionData[0].index == 4 && inAttack);
    auto meleeWeapon         = actorData.newWeapons[actorData.meleeWeaponIndex];

    if (inRebellionCombo1) {
        delayedComboFX.duration              = 0.495f;
        delayedComboFX.weaponThatStartedMove = 0;
    } else if (inCerberusCombo2) {
        delayedComboFX.duration              = 0.55f;
        delayedComboFX.weaponThatStartedMove = 1;
    } else if (inAgniCombo1) {
        delayedComboFX.duration              = 0.53f;
        delayedComboFX.weaponThatStartedMove = 2;
    } else if (inAgniCombo2) {
        delayedComboFX.duration              = 0.70f;
        delayedComboFX.weaponThatStartedMove = 2;
    } else if (inBeoCombo1) {
        delayedComboFX.duration = 0.55f; // Beowulf's time can be very inconsistent due to charge time (the more you charge the less you
                                         // need to wait between delays)
        delayedComboFX.weaponThatStartedMove = 4;
    }


    if (actorData.character == CHARACTER::DANTE) {
        if (delayedComboFX.timer >= delayedComboFX.duration &&
            (inRebellionCombo1 || inCerberusCombo2 || inAgniCombo1 || inAgniCombo2 || inBeoCombo1) && delayedComboFX.playCount == 0 &&
            weapon == delayedComboFX.weaponThatStartedMove) {

            playDelayedCombo1();
            createEffectBank = delayedComboFX.bank;
            createEffectID   = delayedComboFX.id;
            CreateEffectDetour();


            delayedComboFX.playCount++;
        } else if (delayedComboFX.timer < 0.495f) {
            delayedComboFX.playCount = 0;
        }

        if ((!inRebellionCombo1 && !inCerberusCombo2 && !inAgniCombo1 && !inAgniCombo2 && !inBeoCombo1)) {
            delayedComboFX.timer      = 0;
            delayedComboFX.resetTimer = false;


        } else {
            if (!delayedComboFX.resetTimer) {
                crimsonPlayer[0].actionTimer = 0;
                delayedComboFX.resetTimer    = true;
            }


            delayedComboFX.timer = crimsonPlayer[0].actionTimer;
        }
    }
}


void DriveTweaks(byte8* actorBaseAddr) {
    // This function alters some of Drive, it alters its damage to accommodate new "Charge Levels", mimicing DMC4/5 Drive behaviour.

    using namespace ACTION_DANTE;

    if (!actorBaseAddr) {
        return;
    }
    auto& actorData  = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    auto playerIndex = actorData.newPlayerIndex;

    // 	drive physical hit damage dmc3.exe + 5C6D2C, 70.0f
    // 	drive projectile damage dmc3.exe + 5CB1EC, 300.0f
    uintptr_t drivePhysicalDamageAddr   = (uintptr_t)appBaseAddr + 0x5C6D2C;
    uintptr_t driveProjectileDamageAddr = (uintptr_t)appBaseAddr + 0x5CB1EC;

    // Triggering the Drive Timer
    if ((actorData.action == ACTION_DANTE::REBELLION_DRIVE_1) &&
        (actorData.motionData[0].index == 17 || actorData.motionData[0].index == 18)) {
        crimsonPlayer[playerIndex].drive.runTimer = true;
    }

    // Fuck this shit, resetting has proved to be waaay more difficult than it should, probably due to SetAction things
    if (actorData.motionData[0].index == 19 || actorData.motionData[0].index == 1 || actorData.motionData[0].index == 2 ||
        actorData.motionData[0].index == 4 || actorData.motionData[0].index == 5 || actorData.motionData[0].index == 6 ||
        actorData.motionData[0].index == 7 || actorData.motionData[0].index == 8 || actorData.motionData[0].index == 9 ||
        actorData.motionData[0].index == 10) {

        crimsonPlayer[playerIndex].drive.runTimer = false;
    }

    if (actorData.action == ACTION_DANTE::REBELLION_DRIVE_1 && actorData.eventData[0].event != 17) {
        crimsonPlayer[playerIndex].drive.runTimer = false;
    }

    // Setting Quick Drive Damage
    if ((actorData.action == REBELLION_DRIVE_1) && crimsonPlayer[playerIndex].inQuickDrive && actorData.eventData[0].event == 17) {

        *(float*)(drivePhysicalDamageAddr)   = 60.0f;
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
                createEffectID   = crimsonPlayer[playerIndex].drive.id;
                CreateEffectDetour();

                crimsonPlayer[playerIndex].drive.level1EffectPlayed = true;
            }
        }

        if (crimsonPlayer[playerIndex].drive.timer < 2.0) {
            *(float*)(drivePhysicalDamageAddr)   = 70.0f;
            *(float*)(driveProjectileDamageAddr) = 200.0f;
        }

        if (crimsonPlayer[playerIndex].drive.timer >= 2.0 && crimsonPlayer[playerIndex].drive.timer < 3.0) {
            *(float*)(drivePhysicalDamageAddr)   = 70.0f;
            *(float*)(driveProjectileDamageAddr) = 300.0f;

            if (!crimsonPlayer[playerIndex].drive.level2EffectPlayed) {

                createEffectBank = crimsonPlayer[playerIndex].drive.bank;
                createEffectID   = crimsonPlayer[playerIndex].drive.id;
                CreateEffectDetour();

                crimsonPlayer[playerIndex].drive.level2EffectPlayed = true;
            }
        }

        if (crimsonPlayer[playerIndex].drive.timer >= 3.0) {
            *(float*)(drivePhysicalDamageAddr)   = 70.0f;
            *(float*)(driveProjectileDamageAddr) = 700.0f;

            if (!crimsonPlayer[playerIndex].drive.level3EffectPlayed) {

                createEffectBank = crimsonPlayer[playerIndex].drive.bank;
                createEffectID   = crimsonPlayer[playerIndex].drive.id;
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
    // notHoldingMelee = (gamepad.buttons[0] & GetBinding(BINDING::MELEE_ATTACK));


    /* ((actorData.action == REBELLION_DRIVE_1) && actorData.eventData[0].event == 17 && notHoldingMelee == 0 &&
    crimsonPlayer[playerIndex].actionTimer < 1.1f && !crimsonPlayer[playerIndex].inQuickDrive) {




            actorData.state &= ~STATE::BUSY;




    }*/
}

void StyleSwitchDrawText(byte8* actorBaseAddr) {
    if (!actorBaseAddr) {
        return;
    }
    auto& actorData  = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    auto playerIndex = actorData.newPlayerIndex;

    // This function draws the Style Switching Text near Dante when switching styles.
    const ddVec3 trickWorldPos = {actorData.position.x, actorData.position.y + 200.f, actorData.position.z};
    const ddVec3 gunWorldPos   = {actorData.position.x, actorData.position.y + 130.f, actorData.position.z};
    const ddVec3 swordWorldPos = {actorData.position.x, actorData.position.y + 130.f, actorData.position.z};
    const ddVec3 royalWorldPos = {actorData.position.x, actorData.position.y + 70.f, actorData.position.z};
    const ddVec3 quickWorldPos = {actorData.position.x, actorData.position.y + 130.f, actorData.position.z};
    const ddVec3 doppWorldPos  = {actorData.position.x, actorData.position.y + 130.f, actorData.position.z};
    const ddVec3 actorWorldPos = {actorData.position.x, actorData.position.y, actorData.position.z};
    // 	char buffer[256]{};
    // 	sprintf(buffer, "danter: %f, %f, %f",
    // 		actorData.position.x,
    // 		actorData.position.y,
    // 		actorData.position.z
    // 	);

    // const float yellow[4] = { 1.0f, 1.0f, 0.0f, 0.1f }; // rgba alpha does not work/exist for debugDraw yet

    if (crimsonPlayer[playerIndex].styleSwitchText.trickTime > 0) {
        debug_draw_projected_text("TRICK", trickWorldPos, dd::colors::Yellow, crimsonPlayer[playerIndex].styleSwitchText.animSize);
    }

    if (crimsonPlayer[playerIndex].styleSwitchText.swordTime > 0) {
        debug_draw_projected_text("        SWORD", swordWorldPos, dd::colors::Red, crimsonPlayer[playerIndex].styleSwitchText.animSize);
    }

    if (crimsonPlayer[playerIndex].styleSwitchText.gunTime > 0) {
        debug_draw_projected_text("GUN        ", gunWorldPos, dd::colors::DodgerBlue, crimsonPlayer[playerIndex].styleSwitchText.animSize);
    }

    if (crimsonPlayer[playerIndex].styleSwitchText.royalTime > 0) {
        debug_draw_projected_text("ROYAL", royalWorldPos, dd::colors::LightGreen, crimsonPlayer[playerIndex].styleSwitchText.animSize);
    }

    if (crimsonPlayer[playerIndex].styleSwitchText.quickTime > 0) {
        debug_draw_projected_text(
            "          QUICK", quickWorldPos, dd::colors::DeepPink, crimsonPlayer[playerIndex].styleSwitchText.animSize);
    }

    if (crimsonPlayer[playerIndex].styleSwitchText.doppTime > 0) {
        debug_draw_projected_text("DOPP          ", doppWorldPos, dd::colors::Orange, crimsonPlayer[playerIndex].styleSwitchText.animSize);
    }
}

void SetStyleSwitchDrawTextTime(int style, byte8* actorBaseAddr) {
    if (!actorBaseAddr) {
        return;
    }
    auto& actorData  = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    auto playerIndex = actorData.newPlayerIndex;

    float* drawTextTimes[6] = {&crimsonPlayer[playerIndex].styleSwitchText.swordTime, &crimsonPlayer[playerIndex].styleSwitchText.gunTime,
        &crimsonPlayer[playerIndex].styleSwitchText.trickTime, &crimsonPlayer[playerIndex].styleSwitchText.royalTime,
        &crimsonPlayer[playerIndex].styleSwitchText.quickTime, &crimsonPlayer[playerIndex].styleSwitchText.doppTime};

    if (actorData.character == CHARACTER::DANTE) {
        for (int i = 0; i < 6; i++) {
            if (i == style) {
                *drawTextTimes[i]                                   = crimsonPlayer[playerIndex].styleSwitchText.duration;
                crimsonPlayer[playerIndex].styleSwitchText.animSize = 1.0f;
            } else {
                *drawTextTimes[i] = 0;
            }
        }
    }
}
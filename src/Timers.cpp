#include <thread>
#include <chrono>
#include "../ThirdParty/glm/glm.hpp"
#include "../ThirdParty/ImGui/imgui.h"

// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "Timers.hpp"
#include "Core/Core.hpp"
#include "ExtraSound.hpp"
#include "Graphics.hpp"
#include "Actor.hpp"
#include "ActorBase.hpp"
#include "ActorRelocations.hpp"
#include "Config.hpp"
#include "Exp.hpp"
#include "Global.hpp"
#include "Vars.hpp"

#include "Core/Macros.h"

void ActionTimers() {
    // IntroduceMainActorData
    auto pool_12857 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
    if (!pool_12857 || !pool_12857[3]) {
        return;
    }


    auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_12857[3]);

    auto pool_10371 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
    if (!pool_10371 || !pool_10371[8]) {
        return;
    }
    auto& eventData = *reinterpret_cast<EventData*>(pool_10371[8]);


    old_for_all(uint8, playerIndex, PLAYER_COUNT) {
        auto& playerData = GetPlayerData(playerIndex);

        auto& newActorData = GetNewActorData(playerIndex, playerData.activeCharacterIndex, 0);

        auto actorBaseAddr = newActorData.baseAddr;


        if (!actorBaseAddr) {
            continue;
        }
        auto& actorData      = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
        auto& cloneActorData = *reinterpret_cast<PlayerActorData*>(actorData.cloneActorBaseAddr);
        auto inAttack        = (actorData.eventData[0].event == 17);
        auto inAttackClone   = (cloneActorData.eventData[0].event == 17);

        if (inAttack) {
            if (eventData.event != EVENT::PAUSE) {
                crimsonPlayer[playerIndex].actionTimer +=
                    (ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_frameRateMultiplier;
            }
        } else {
            crimsonPlayer[playerIndex].actionTimer = 0;
        }

        // ACTIONS
        if (actorData.action != crimsonPlayer[playerIndex].currentAction) {
            crimsonPlayer[playerIndex].actionTimer = 0;

            crimsonPlayer[playerIndex].currentAction = actorData.action;
        }

        ////

        if (inAttackClone) {
            if (eventData.event != EVENT::PAUSE) {
                crimsonPlayer[playerIndex].actionTimerClone +=
                    (ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speedClone) / g_frameRateMultiplier;
            }
        }

        // ACTIONS CLONE
        if (cloneActorData.action != crimsonPlayer[playerIndex].currentActionClone) {
            crimsonPlayer[playerIndex].actionTimerClone = 0;

            crimsonPlayer[playerIndex].currentActionClone = cloneActorData.action;
        }
    }
}


void AnimTimers() {
    // IntroduceMainActorData
    auto pool_12857 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
    if (!pool_12857 || !pool_12857[3]) {
        return;
    }


    auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_12857[3]);

    auto pool_10371 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
    if (!pool_10371 || !pool_10371[8]) {
        return;
    }
    auto& eventData = *reinterpret_cast<EventData*>(pool_10371[8]);

    auto inAttack = (mainActorData.eventData[0].event == 17);


    old_for_all(uint8, playerIndex, PLAYER_COUNT) {
        auto& playerData = GetPlayerData(playerIndex);

        auto& newActorData = GetNewActorData(playerIndex, playerData.activeCharacterIndex, 0);

        auto actorBaseAddr = newActorData.baseAddr;

        if (!actorBaseAddr) {
            continue;
        }
        auto& actorData      = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
        auto& cloneActorData = *reinterpret_cast<PlayerActorData*>(actorData.cloneActorBaseAddr);

        // ANIMATION IDs
        if (actorData.motionData[0].index != crimsonPlayer[playerIndex].currentAnim) {
            crimsonPlayer[playerIndex].animTimer = 0;

            crimsonPlayer[playerIndex].currentAnim = actorData.motionData[0].index;
        }

        if (eventData.event != EVENT::PAUSE) {
            crimsonPlayer[playerIndex].animTimer += (ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_frameRateMultiplier;
        }

        ////

        // ANIMATION IDs CLONE
        if (cloneActorData.motionData[0].index != crimsonPlayer[playerIndex].currentAnimClone) {
            crimsonPlayer[playerIndex].animTimerClone = 0;

            crimsonPlayer[playerIndex].currentAnimClone = cloneActorData.motionData[0].index;
        }

        if (eventData.event != EVENT::PAUSE) {
            crimsonPlayer[playerIndex].animTimerClone +=
                (ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speedClone) / g_frameRateMultiplier;
        }
    }
}


void SprintTimer() {


    old_for_all(uint8, playerIndex, PLAYER_COUNT) {


        if (crimsonPlayer[playerIndex].sprint.timer > 0 && crimsonPlayer[playerIndex].sprint.runTimer) {
            crimsonPlayer[playerIndex].sprint.timer -= ImGui::GetIO().DeltaTime / g_frameRateMultiplier;
        }


        if (crimsonPlayer[playerIndex].sprint.timer <= 0 && !crimsonPlayer[playerIndex].sprint.canSprint) {
            // sprint.timer = sprint.timeToTrigger;
            crimsonPlayer[playerIndex].sprint.runTimer  = false;
            crimsonPlayer[playerIndex].sprint.canSprint = true;
        }
    }
}

void DriveTimer() {

    old_for_all(uint8, playerIndex, PLAYER_COUNT) {


        if (crimsonPlayer[playerIndex].drive.runTimer) {

            crimsonPlayer[playerIndex].drive.timer += (ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_frameRateMultiplier;
        } else {
            crimsonPlayer[playerIndex].drive.timer = 0;
        }
    }
}

void ImprovedCancelsTimers() {

    old_for_all(uint8, playerIndex, PLAYER_COUNT) {


        // TRICK CANCEL
        if (!crimsonPlayer[playerIndex].cancels.canTrick) {

            crimsonPlayer[playerIndex].cancels.trickCooldown -=
                (ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_frameRateMultiplier;
        }

        if (crimsonPlayer[playerIndex].cancels.trickCooldown <= 0 && !crimsonPlayer[playerIndex].cancels.canTrick) {
            crimsonPlayer[playerIndex].cancels.trickCooldown = crimsonPlayer[playerIndex].cancels.trickCooldownDuration;
            crimsonPlayer[playerIndex].cancels.canTrick      = true;
        }

        // GUN CANCEL
        if (!crimsonPlayer[playerIndex].cancels.canGun) {

            crimsonPlayer[playerIndex].cancels.gunsCooldown -=
                (ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_frameRateMultiplier;
        }

        if (crimsonPlayer[playerIndex].cancels.gunsCooldown <= 0 && !crimsonPlayer[playerIndex].cancels.canGun) {
            crimsonPlayer[playerIndex].cancels.gunsCooldown = crimsonPlayer[playerIndex].cancels.gunsCooldownDuration;
            crimsonPlayer[playerIndex].cancels.canGun       = true;
        }


        // RAINSTORM CANCEL
        if (!crimsonPlayer[playerIndex].cancels.canRainstorm) {

            crimsonPlayer[playerIndex].cancels.rainstormCooldown -=
                (ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_frameRateMultiplier;
        }

        if (crimsonPlayer[playerIndex].cancels.rainstormCooldown <= 0 && !crimsonPlayer[playerIndex].cancels.canRainstorm) {
            crimsonPlayer[playerIndex].cancels.rainstormCooldown = crimsonPlayer[playerIndex].cancels.rainstormCooldownDuration;
            crimsonPlayer[playerIndex].cancels.canRainstorm      = true;
        }

        ///

        // TRICK CANCEL CLONE
        if (!crimsonPlayer[playerIndex].cancelsClone.canTrick) {

            crimsonPlayer[playerIndex].cancelsClone.trickCooldown -=
                (ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_frameRateMultiplier;
        }

        if (crimsonPlayer[playerIndex].cancelsClone.trickCooldown <= 0 && !crimsonPlayer[playerIndex].cancelsClone.canTrick) {
            crimsonPlayer[playerIndex].cancelsClone.trickCooldown = crimsonPlayer[playerIndex].cancelsClone.trickCooldownDuration;
            crimsonPlayer[playerIndex].cancelsClone.canTrick      = true;
        }

        // GUN CANCEL CLONE
        if (!crimsonPlayer[playerIndex].cancelsClone.canGun) {

            crimsonPlayer[playerIndex].cancelsClone.gunsCooldown -=
                (ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_frameRateMultiplier;
        }

        if (crimsonPlayer[playerIndex].cancelsClone.gunsCooldown <= 0 && !crimsonPlayer[playerIndex].cancelsClone.canGun) {
            crimsonPlayer[playerIndex].cancelsClone.gunsCooldown = crimsonPlayer[playerIndex].cancelsClone.gunsCooldownDuration;
            crimsonPlayer[playerIndex].cancelsClone.canGun       = true;
        }


        // RAINSTORM CANCEL CLONE
        if (!crimsonPlayer[playerIndex].cancelsClone.canRainstorm) {

            crimsonPlayer[playerIndex].cancelsClone.rainstormCooldown -=
                (ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_frameRateMultiplier;
        }

        if (crimsonPlayer[playerIndex].cancelsClone.rainstormCooldown <= 0 && !crimsonPlayer[playerIndex].cancelsClone.canRainstorm) {
            crimsonPlayer[playerIndex].cancelsClone.rainstormCooldown = crimsonPlayer[playerIndex].cancelsClone.rainstormCooldownDuration;
            crimsonPlayer[playerIndex].cancelsClone.canRainstorm      = true;
        }
    }
}


void BackToForwardTimers() {
    old_for_all(uint8, playerIndex, PLAYER_COUNT) {

        if (crimsonPlayer[playerIndex].b2F.backCommand) {
            crimsonPlayer[playerIndex].b2F.backBuffer -= ImGui::GetIO().DeltaTime / g_frameRateMultiplier;
        }

        if (crimsonPlayer[playerIndex].b2F.forwardCommand) {
            crimsonPlayer[playerIndex].b2F.forwardBuffer -= ImGui::GetIO().DeltaTime / g_frameRateMultiplier;
        }
    }
}

void StyleSwitchTextTimers() {
    old_for_all(uint8, playerIndex, PLAYER_COUNT) {

        if (crimsonPlayer[playerIndex].styleSwitchText.trickTime > 0) {
            crimsonPlayer[playerIndex].styleSwitchText.trickTime -= ImGui::GetIO().DeltaTime / g_frameRateMultiplier;
            crimsonPlayer[playerIndex].styleSwitchText.animSize += ImGui::GetIO().DeltaTime / g_frameRateMultiplier;
        }

        if (crimsonPlayer[playerIndex].styleSwitchText.swordTime > 0) {
            crimsonPlayer[playerIndex].styleSwitchText.swordTime -= ImGui::GetIO().DeltaTime / g_frameRateMultiplier;
            crimsonPlayer[playerIndex].styleSwitchText.animSize += ImGui::GetIO().DeltaTime / g_frameRateMultiplier;
        }

        if (crimsonPlayer[playerIndex].styleSwitchText.gunTime > 0) {
            crimsonPlayer[playerIndex].styleSwitchText.gunTime -= ImGui::GetIO().DeltaTime / g_frameRateMultiplier;
            crimsonPlayer[playerIndex].styleSwitchText.animSize += ImGui::GetIO().DeltaTime / g_frameRateMultiplier;
        }

        if (crimsonPlayer[playerIndex].styleSwitchText.royalTime > 0) {
            crimsonPlayer[playerIndex].styleSwitchText.royalTime -= ImGui::GetIO().DeltaTime / g_frameRateMultiplier;
            crimsonPlayer[playerIndex].styleSwitchText.animSize += ImGui::GetIO().DeltaTime / g_frameRateMultiplier;
        }

        if (crimsonPlayer[playerIndex].styleSwitchText.quickTime > 0) {
            crimsonPlayer[playerIndex].styleSwitchText.quickTime -= ImGui::GetIO().DeltaTime / g_frameRateMultiplier;
            crimsonPlayer[playerIndex].styleSwitchText.animSize += ImGui::GetIO().DeltaTime / g_frameRateMultiplier;
        }

        if (crimsonPlayer[playerIndex].styleSwitchText.doppTime > 0) {
            crimsonPlayer[playerIndex].styleSwitchText.doppTime -= ImGui::GetIO().DeltaTime / g_frameRateMultiplier;
            crimsonPlayer[playerIndex].styleSwitchText.animSize += ImGui::GetIO().DeltaTime / g_frameRateMultiplier;
        }
    }
}

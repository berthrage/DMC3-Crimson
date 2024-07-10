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
        auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
        auto inAttack = (actorData.eventData[0].event == 17);
        

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

        // We add this condition because Dante and Vergil are the only character capable of having Doppelgangers, 
        // fetching data from Clones (such as playing with Boss Vergil) where none exist will crash the game. - Mia

        if (actorData.character == CHARACTER::DANTE || actorData.character == CHARACTER::VERGIL) {
			auto& cloneActorData = *reinterpret_cast<PlayerActorData*>(actorData.cloneActorBaseAddr);
			auto inAttackClone = (cloneActorData.eventData[0].event == 17);

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
}


void AnimTimers() {

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
        auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
        

        // ANIMATION IDs
        if (actorData.motionData[0].index != crimsonPlayer[playerIndex].currentAnim) {
            crimsonPlayer[playerIndex].animTimer = 0;

            crimsonPlayer[playerIndex].currentAnim = actorData.motionData[0].index;
        }

        if (eventData.event != EVENT::PAUSE) {
            crimsonPlayer[playerIndex].animTimer += (ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_frameRateMultiplier;
        }

        ////

		// We add this condition because Dante and Vergil are the only character capable of having Doppelgangers, 
		// fetching data from Clones (such as playing with Boss Vergil) where none exist will crash the game. Same as with ActionTimers - Mia

        if (actorData.character == CHARACTER::DANTE || actorData.character == CHARACTER::VERGIL) {
			auto& cloneActorData = *reinterpret_cast<PlayerActorData*>(actorData.cloneActorBaseAddr);

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
    for (int playerIndex = 0; playerIndex < PLAYER_COUNT; playerIndex++) {

        auto* sstext = &crimsonPlayer[playerIndex].styleSwitchText;
        float i = 0;

        for (int styleid = 0; styleid < 9; styleid++) {
			if (sstext->time[styleid] > 0) {
                sstext->time[styleid] -= (ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_frameRateMultiplier;
				sstext->animSize += (ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_frameRateMultiplier * 0.1f; //animates size

				float rate = 1.0 / 0.1f;
				i += ImGui::GetIO().DeltaTime;
				
                // animates fade in
                if (sstext->alpha[styleid] < 0.9f && sstext->time[styleid] > 0.3f) {
                    sstext->alpha[styleid] += ((ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_frameRateMultiplier) * 4;
                
                }

                //animates fade out
				if (sstext->time[styleid] < 0.15f) {
					sstext->alpha[styleid] -= ((ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_frameRateMultiplier) * 4;
				}
            
			}

            
        }
    }
}

void StyleSwitchFluxTimers() {
	for (int playerIndex = 0; playerIndex < PLAYER_COUNT; playerIndex++) {

		auto* fluxtime = &crimsonPlayer[playerIndex].fluxtime;
		float i = 0;

        if (*fluxtime > 0) {
            *fluxtime -= (ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_frameRateMultiplier;
        }

	}
}



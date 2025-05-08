#include <thread>
#include <chrono>
#include "../ThirdParty/glm/glm.hpp"
#include "../ThirdParty/ImGui/imgui.h"

// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "CrimsonTimers.hpp"
#include "Core/Core.hpp"
#include "CrimsonSDL.hpp"
#include "Graphics.hpp"
#include "Actor.hpp"
#include "ActorBase.hpp"
#include "ActorRelocations.hpp"
#include "Config.hpp"
#include "Exp.hpp"
#include "Global.hpp"
#include "Vars.hpp"

#include "Core/Macros.h"
#include "DMC3Input.hpp"

namespace CrimsonTimers {

void ActionTimers() {
	auto pool_10371 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
	if (!pool_10371 || !pool_10371[8]) return;
	auto& eventData = *reinterpret_cast<EventData*>(pool_10371[8]);

    old_for_all(uint8, playerIndex, PLAYER_COUNT) {
        old_for_all(uint8, entityIndex, ENTITY_COUNT) {
            auto& playerData = GetPlayerData(playerIndex);

            auto& newActorData = GetNewActorData(playerIndex, playerData.activeCharacterIndex, entityIndex);

            auto actorBaseAddr = newActorData.baseAddr;
            if (!actorBaseAddr) {
                continue;
            }
            auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
            auto& cloneActorData = *reinterpret_cast<PlayerActorData*>(actorData.cloneActorBaseAddr);
            if (actorData.character != CHARACTER::DANTE && actorData.character != CHARACTER::VERGIL) continue;
            auto inAttack = (actorData.eventData[0].event == ACTOR_EVENT::ATTACK);
            auto& currentAction = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].currentAction : crimsonPlayer[playerIndex].currentActionClone;
            auto& actionTimer = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].actionTimer : crimsonPlayer[playerIndex].actionTimerClone;


            if (inAttack) {
                if (eventData.event != EVENT::PAUSE) {
                    actionTimer += ImGui::GetIO().DeltaTime * (actorData.speed / g_FrameRateTimeMultiplier);
                }
            }
            else {
                actionTimer = 0;
            }

            // Reset Timer By Action
            if (actorData.action != currentAction) {
                actionTimer = 0;
                currentAction = actorData.action;
            }
        }
    }
}


void AnimTimers() {
	auto pool_10371 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
	if (!pool_10371 || !pool_10371[8]) return;	
	auto& eventData = *reinterpret_cast<EventData*>(pool_10371[8]);

    old_for_all(uint8, playerIndex, PLAYER_COUNT) {
        old_for_all(uint8, entityIndex, ENTITY_COUNT) {
            auto& playerData = GetPlayerData(playerIndex);

            auto& newActorData = GetNewActorData(playerIndex, playerData.activeCharacterIndex, entityIndex);

            auto actorBaseAddr = newActorData.baseAddr;

            if (!actorBaseAddr) {
                continue;
            }
            auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
            if (actorData.character != CHARACTER::DANTE && actorData.character != CHARACTER::VERGIL) continue;
            auto& currentAnim = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].currentAnim : crimsonPlayer[playerIndex].currentAnimClone;
            auto& actorMotion = actorData.motionData[0].index;
            auto& animTimer = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].animTimer : crimsonPlayer[playerIndex].animTimerClone;


            // Reset Timer By Animation IDs
            if (actorMotion != currentAnim) {
                animTimer = 0;
                currentAnim = actorMotion;
            }

            if (eventData.event != EVENT::PAUSE) {
                animTimer += ImGui::GetIO().DeltaTime * (actorData.speed / g_FrameRateTimeMultiplier);
            }
        }
    }
}

void EventTimers() {
	auto pool_10371 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
	if (!pool_10371 || !pool_10371[8]) return;
	auto& eventData = *reinterpret_cast<EventData*>(pool_10371[8]);

	old_for_all(uint8, playerIndex, PLAYER_COUNT) {
		old_for_all(uint8, entityIndex, ENTITY_COUNT) {
			auto& playerData = GetPlayerData(playerIndex);

			auto& newActorData = GetNewActorData(playerIndex, playerData.activeCharacterIndex, entityIndex);

			auto actorBaseAddr = newActorData.baseAddr;

			if (!actorBaseAddr) {
				continue;
			}
			auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
			if (actorData.character != CHARACTER::DANTE && actorData.character != CHARACTER::VERGIL) continue;
            static uint32 currentEvent[PLAYER_COUNT][ENTITY_COUNT];
            auto& actorEvent = actorData.eventData[0].event;
			auto& eventTimer = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].eventTimer : crimsonPlayer[playerIndex].eventTimerClone;


			// Reset Timer By Animation IDs
			if (actorEvent != currentEvent[playerIndex][entityIndex]) {
				eventTimer = 0;
                currentEvent[playerIndex][entityIndex] = actorEvent;
			}

			if (eventData.event != EVENT::PAUSE) {
				eventTimer += ImGui::GetIO().DeltaTime * (actorData.speed / g_FrameRateTimeMultiplier);
			}
		}
	}
}

void TrickDashTimers () {
	auto pool_10371 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
	if (!pool_10371 || !pool_10371[8]) return;
	auto& eventData = *reinterpret_cast<EventData*>(pool_10371[8]);
    static uint32 currentEvent[PLAYER_COUNT][ENTITY_COUNT] = { 0 };

	old_for_all(uint8, playerIndex, PLAYER_COUNT) {
		old_for_all(uint8, entityIndex, ENTITY_COUNT) {
			auto& playerData = GetPlayerData(playerIndex);

			auto& newActorData = GetNewActorData(playerIndex, playerData.activeCharacterIndex, entityIndex);

			auto actorBaseAddr = newActorData.baseAddr;

			if (!actorBaseAddr) {
				continue;
			}
			auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
			if (actorData.character != CHARACTER::DANTE && actorData.character != CHARACTER::VERGIL) continue;
			
			auto& actorEvent = actorData.eventData[0].event;
			auto& trickDashTimer = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].trickDashTimer : crimsonPlayer[playerIndex].trickDashTimerClone;
            auto& policy = actorData.nextActionRequestPolicy[NEXT_ACTION_REQUEST_POLICY::TRICKSTER_DARK_SLAYER];


			// Reset Timer by Event ID or Style button press
			if (actorEvent != currentEvent[playerIndex][entityIndex] || policy == 2) {
				trickDashTimer = 0;
				currentEvent[playerIndex][entityIndex] = actorEvent;
			}

			if (eventData.event != EVENT::PAUSE) {
				trickDashTimer += ImGui::GetIO().DeltaTime * (actorData.speed / g_FrameRateTimeMultiplier);
			}
		}
	}
}

std::chrono::steady_clock::time_point guardflyStartTime;

void ResetGuardflyTimer(float timer) {
	timer = 0;
	guardflyStartTime = std::chrono::steady_clock::now();
}

void GuardflyTimers() {
	auto pool_10371 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
	if (!pool_10371 || !pool_10371[8]) {
		return;
	}
	auto& eventData = *reinterpret_cast<EventData*>(pool_10371[8]);

	old_for_all(uint8, playerIndex, PLAYER_COUNT) {
        old_for_all(uint8, entityIndex, ENTITY_COUNT) {
            auto& playerData = GetPlayerData(playerIndex);

            auto& newActorData = GetNewActorData(playerIndex, playerData.activeCharacterIndex, entityIndex);

            auto actorBaseAddr = newActorData.baseAddr;

            if (!actorBaseAddr) {
                continue;
            }
            auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
            auto& guardflyTimer = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].inertia.guardflyTimer : crimsonPlayer[playerIndex].inertiaClone.guardflyTimer;
            auto& currentEvent = (actorData.newEntityIndex == 0) ? crimsonPlayer[playerIndex].currentEvent : crimsonPlayer[playerIndex].currentEventClone;
            auto& actorEvent = actorData.eventData[0].event;

            // Track Event
            if (actorEvent != currentEvent) {
                currentEvent = actorEvent;

                ResetGuardflyTimer(guardflyTimer);
            }


            if (eventData.event != EVENT::PAUSE) {
                auto now = std::chrono::steady_clock::now();
                float elapsedTime = std::chrono::duration<float>(now - guardflyStartTime).count();

                guardflyTimer = elapsedTime * (actorData.speed / g_FrameRateTimeMultiplier);
            }
        }
	}
}

void SiyTimerFunc() {
	if (siytimer > 0) {
		siytimer -= ImGui::GetIO().DeltaTime;
	}
	else if (siytimer < 0) {
		siytimer = 0;
	}
}


void SprintTimer() {


    old_for_all(uint8, playerIndex, PLAYER_COUNT) {


        if (crimsonPlayer[playerIndex].sprint.timer > 0 && crimsonPlayer[playerIndex].sprint.runTimer) {
            crimsonPlayer[playerIndex].sprint.timer -= ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed / g_FrameRateTimeMultiplier;
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

            crimsonPlayer[playerIndex].drive.timer += (ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_FrameRateTimeMultiplier;
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
                (ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_FrameRateTimeMultiplier;
        }

        if (crimsonPlayer[playerIndex].cancels.trickCooldown <= 0 && !crimsonPlayer[playerIndex].cancels.canTrick) {
            crimsonPlayer[playerIndex].cancels.trickCooldown = crimsonPlayer[playerIndex].cancels.trickCooldownDuration;
            crimsonPlayer[playerIndex].cancels.canTrick      = true;
        }

        // GUN CANCEL
        if (!crimsonPlayer[playerIndex].cancels.canGun) {

            crimsonPlayer[playerIndex].cancels.gunsCooldown -=
                (ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_FrameRateTimeMultiplier;
        }

        if (crimsonPlayer[playerIndex].cancels.gunsCooldown <= 0 && !crimsonPlayer[playerIndex].cancels.canGun) {
            crimsonPlayer[playerIndex].cancels.gunsCooldown = crimsonPlayer[playerIndex].cancels.gunsCooldownDuration;
            crimsonPlayer[playerIndex].cancels.canGun       = true;
        }


        // RAINSTORM CANCEL
        if (!crimsonPlayer[playerIndex].cancels.canRainstorm) {

            crimsonPlayer[playerIndex].cancels.rainstormCooldown -=
                (ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_FrameRateTimeMultiplier;
        }

        if (crimsonPlayer[playerIndex].cancels.rainstormCooldown <= 0 && !crimsonPlayer[playerIndex].cancels.canRainstorm) {
            crimsonPlayer[playerIndex].cancels.rainstormCooldown = crimsonPlayer[playerIndex].cancels.rainstormCooldownDuration;
            crimsonPlayer[playerIndex].cancels.canRainstorm      = true;
        }

        // REVOLVER TIMER
		if (crimsonPlayer[playerIndex].cancels.revolverTimerRunning) {
			crimsonPlayer[playerIndex].cancels.revolverTimer += (ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_FrameRateTimeMultiplier;
		}
        else {
			crimsonPlayer[playerIndex].cancels.revolverTimer = 0;
        }

        ///

        // TRICK CANCEL CLONE
        if (!crimsonPlayer[playerIndex].cancelsClone.canTrick) {

            crimsonPlayer[playerIndex].cancelsClone.trickCooldown -=
                (ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_FrameRateTimeMultiplier;
        }

        if (crimsonPlayer[playerIndex].cancelsClone.trickCooldown <= 0 && !crimsonPlayer[playerIndex].cancelsClone.canTrick) {
            crimsonPlayer[playerIndex].cancelsClone.trickCooldown = crimsonPlayer[playerIndex].cancelsClone.trickCooldownDuration;
            crimsonPlayer[playerIndex].cancelsClone.canTrick      = true;
        }

        // GUN CANCEL CLONE
        if (!crimsonPlayer[playerIndex].cancelsClone.canGun) {

            crimsonPlayer[playerIndex].cancelsClone.gunsCooldown -=
                (ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_FrameRateTimeMultiplier;
        }

        if (crimsonPlayer[playerIndex].cancelsClone.gunsCooldown <= 0 && !crimsonPlayer[playerIndex].cancelsClone.canGun) {
            crimsonPlayer[playerIndex].cancelsClone.gunsCooldown = crimsonPlayer[playerIndex].cancelsClone.gunsCooldownDuration;
            crimsonPlayer[playerIndex].cancelsClone.canGun       = true;
        }


        // RAINSTORM CANCEL CLONE
        if (!crimsonPlayer[playerIndex].cancelsClone.canRainstorm) {

            crimsonPlayer[playerIndex].cancelsClone.rainstormCooldown -=
                (ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_FrameRateTimeMultiplier;
        }

        if (crimsonPlayer[playerIndex].cancelsClone.rainstormCooldown <= 0 && !crimsonPlayer[playerIndex].cancelsClone.canRainstorm) {
            crimsonPlayer[playerIndex].cancelsClone.rainstormCooldown = crimsonPlayer[playerIndex].cancelsClone.rainstormCooldownDuration;
            crimsonPlayer[playerIndex].cancelsClone.canRainstorm      = true;
        }

		// REVOLVER TIMER CLONE
		if (crimsonPlayer[playerIndex].cancelsClone.revolverTimerRunning) {
			crimsonPlayer[playerIndex].cancelsClone.revolverTimer += (ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_FrameRateTimeMultiplier;
		} else {
			crimsonPlayer[playerIndex].cancelsClone.revolverTimer = 0;
		}
    }
}


void BackToForwardTimers() {
    old_for_all(uint8, playerIndex, PLAYER_COUNT) {
        old_for_all(uint8, entityIndex, ENTITY_COUNT) {
            auto& b2F = (entityIndex == 0) ? crimsonPlayer[playerIndex].b2F : crimsonPlayer[playerIndex].b2FClone;

			if (b2F.backCommand) {
				b2F.backBuffer -= ImGui::GetIO().DeltaTime;
			}

			if (b2F.forwardCommand) {
				b2F.forwardBuffer -= ImGui::GetIO().DeltaTime;
			}

        }
    }
}

void StyleSwitchTextTimers() {
    for (int playerIndex = 0; playerIndex < PLAYER_COUNT; playerIndex++) {

        auto* sstext = &crimsonPlayer[playerIndex].styleSwitchText;


        for (int styleid = 0; styleid < 9; styleid++) {
            
			if (sstext->time[styleid] > 0) {
                sstext->time[styleid] -= (ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_FrameRateTimeMultiplier;
                sstext->animSize += ((ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_FrameRateTimeMultiplier) * 0.1f;

				
				
                // animates fade in
                if (sstext->alpha[styleid] < activeCrimsonConfig.StyleSwitchFX.Text.maxAlpha && sstext->time[styleid] > 0.3f) {
                    sstext->alpha[styleid] += ((ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_FrameRateTimeMultiplier) * 8;
                
                }

                //animates fade out
				if (sstext->time[styleid] < 0.25f) {
					sstext->alpha[styleid] -= ((ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_FrameRateTimeMultiplier) * 2.5f;
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
            *fluxtime -= (ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_FrameRateTimeMultiplier;
        }

	}
}

void DTEVFXTimers() {
	for (int playerIndex = 0; playerIndex < PLAYER_COUNT; playerIndex++) {

		auto& time = crimsonPlayer[playerIndex].dTEVFX.time;
        auto& started = crimsonPlayer[playerIndex].dTEVFX.started;
		float i = 0;

		if (time >= 0 && started) {
			time += (ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_FrameRateTimeMultiplier;
		}

        if (time >= 1.0f) {
            started = false;
            time = 0;
        }

	}
}

void VergilDoppelgangerCooldownTimer() {
	for (int playerIndex = 0; playerIndex < PLAYER_COUNT; playerIndex++) {

        auto& time = crimsonPlayer[playerIndex].vergilDoppelganger.cooldownTime;
		float i = 0;

		if (time > 0) {
			time -= (ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_FrameRateTimeMultiplier;
		}

	}
}

void VergilDoppelgangerDrainTimer() {
	for (int playerIndex = 0; playerIndex < PLAYER_COUNT; playerIndex++) {

		auto& time = crimsonPlayer[playerIndex].vergilDoppelganger.drainTime;
		auto& start = crimsonPlayer[playerIndex].vergilDoppelganger.drainStart;
        auto& miragePoints = crimsonPlayer[playerIndex].vergilDoppelganger.miragePoints;
		float i = 0;

		if (time >= 0 && start) {
			time += (ImGui::GetIO().DeltaTime * crimsonPlayer[playerIndex].speed) / g_FrameRateTimeMultiplier;
		}

        if (!start || miragePoints == 0 || time == 0 || g_inGameCutscene) {
            time = 0;
            start = false;
        }

	}
}


void CallAllTimers() {
	BackToForwardTimers();
	ActionTimers();
	AnimTimers();
    EventTimers();
    TrickDashTimers();
    GuardflyTimers();
	SiyTimerFunc();
	SprintTimer();
	DriveTimer();
	ImprovedCancelsTimers();
	StyleSwitchTextTimers();
	StyleSwitchFluxTimers();
	DTEVFXTimers();
	VergilDoppelgangerCooldownTimer();
    VergilDoppelgangerDrainTimer();
}

}
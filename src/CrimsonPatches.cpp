// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include <intrin.h>
#include <string>
#include <algorithm>
#include "CrimsonPatches.hpp"
#include "Core/Core.hpp"
#include "DMC3Input.hpp"
#include <stdio.h>
#include "Utility/Detour.hpp"
#include "ActorBase.hpp"
#include "Config.hpp"
#include "Global.hpp"
#include "Vars.hpp"

#include "Core/Macros.h"
#include "CrimsonUtil.hpp"
#include <iostream>
#include <iomanip>
#include "DebugDrawDX11.hpp"

#pragma region GameplayImprovements

namespace CrimsonPatches {

	void HoldToAutoFire(bool enable) {
		static bool run = false;

		if (run == enable) {
			return;
		}

		if (activeCrimsonGameplay.Gameplay.General.holdToShoot) {
			//one of these is air, one is ground, I forget which is which.
			_patch((char*)(appBaseAddr + 0x2050A0), (char*)"\xE0", 1);
			_patch((char*)(appBaseAddr + 0x2053E8), (char*)"\xE0", 1); 
		}
		else {
			_patch((char*)(appBaseAddr + 0x2050A0), (char*)"\xE4", 1);
			_patch((char*)(appBaseAddr + 0x2053E8), (char*)"\xE4", 1);
		}

		run = enable;
	}

void DisableHeightRestriction(bool enable) {
	static bool run = false;

	if (run == enable) {
		return;
	}

    uintptr_t raveAddr      = 0x20149524;
    uintptr_t rainstormAddr = 0x20149708;
    uintptr_t airMeleeAddr  = 0x2014970C;

	if (activeCrimsonGameplay.Gameplay.General.disableHeightRestriction) {
		*(float*)(raveAddr) = 0.0f;
		*(float*)(rainstormAddr) = 0.0f;
		*(float*)(airMeleeAddr) = 0.0f;

		_patch((char*)(appBaseAddr + 0x1E62AF), (char*)"\xE9\x2B\xFD\xFF\xFF\x90", 6); // Vergil Yamato and Beowulf
		_nop((char*)(appBaseAddr + 0x1E61EC), 6);                                      // Vergil Force Edge
	}
	else {
		*(float*)(raveAddr) = 80.0f;
		*(float*)(rainstormAddr) = 200.0f;
		*(float*)(airMeleeAddr) = 120.0f;

		_patch((char*)(appBaseAddr + 0x1E62AF), (char*)"\x0F\x87\x2A\xFD\xFF\xFF", 6);
		_patch((char*)(appBaseAddr + 0x1E61EC), (char*)"\x0F\x86\xC3\x00\x00\x00", 6);
	}
    
    run = enable;
}

void IncreasedJCSpheres(bool enable) {
	static bool run = false;
	uintptr_t jesterJCSizeAddr = (uintptr_t)appBaseAddr + 0x580F08;

	if (run == enable) {
		return;
	}
   
	if (enable) {
		// Hells -- dmc3.exe+1C1DCB - F3 0F 5E 0D B1 4F 31 00 - divss xmm1,[dmc3.exe+4D6D84] { (2.14) }
		_patch((char*)(appBaseAddr + 0x1C1DCB), (char*)"\xF3\x0F\x5E\x0D\xB1\x4F\x31\x00", 8);

		// Beowulf -- dmc3.exe+14CFFD - 41 C7 01 00 00 7A 43 - mov [r9],437A0000 { 250.00 }
		_patch((char*)(appBaseAddr + 0x14CFFD), (char*)"\x41\xC7\x01\x00\x00\x7A\x43", 7);

		// Nevan -- dmc3.exe+12F5EB - 41 C7 01 00 00 48 43 - mov [r9],43480000 { 200.00 }
		_patch((char*)(appBaseAddr + 0x12F5EB), (char*)"\x41\xC7\x01\x00\x00\x48\x43", 7);

		*(float*)(jesterJCSizeAddr) = 150.0f;

		// Agni&Rudra, Vergil, The Fallen, Enigmas -- dmc3.exe+1BF26A - 41 C7 01 00 00 34 43 - mov [r9],43340000 { 180,00 }
		_patch((char*)(appBaseAddr + 0x1BF26A), (char*)"\x41\xC7\x01\x00\x00\x34\x43", 7);

		// Bloodgoyles, Chess Pieces -- dmc3.exe+E3371 - 41 C7 01 00 00 48 43 - mov [r9],43480000 { 200,00 }
		_patch((char*)(appBaseAddr + 0xE3371), (char*)"\x41\xC7\x01\x00\x00\x48\x43", 7);

		// Arachne -- dmc3.exe + BF725 - 41 C7 01 00 00 48 43 - mov[r9], 43480000 { 200,00 }
		_patch((char*)(appBaseAddr + 0xBF725), (char*)"\x41\xC7\x01\x00\x00\x48\x43", 7);

		// Soul Eaters -- dmc3.exe+E99FE - 41 C7 01 00 00 48 43 - mov [r9],43480000 { 200,00 }
		_patch((char*)(appBaseAddr + 0xE99FE), (char*)"\x41\xC7\x01\x00\x00\x48\x43", 7);

	}
	else {
		// Hells -- dmc3.exe+1C1DCB - F3 0F 5E 0D 81 42 30 00  - divss xmm1,[dmc3.exe+4C6054] { (3.00) }
		_patch((char*)(appBaseAddr + 0x1C1DCB), (char*)"\xF3\x0F\x5E\x0D\x81\x42\x30\x00", 8);

		// Beowulf -- dmc3.exe+14CFFD - 41 C7 01 00 00 16 43 - mov [r9],43160000 { 150.00 }
		_patch((char*)(appBaseAddr + 0x14CFFD), (char*)"\x41\xC7\x01\x00\x00\x16\x43", 7);

		// Nevan -- dmc3.exe+12F5EB - 41 C7 01 00 00 C8 42 - mov [r9],42C80000 { 100.00 }
		_patch((char*)(appBaseAddr + 0x12F5EB), (char*)"\x41\xC7\x01\x00\x00\xC8\x42", 7);

		*(float*)(jesterJCSizeAddr) = 95.0f;

		// Agni&Rudra, Vergil, The Fallen, Enigmas, Cerb -- dmc3.exe+1BF26A - 41 C7 01 00 00 C8 42 - mov [r9],42C80000 { 100.00 }
		_patch((char*)(appBaseAddr + 0x1BF26A), (char*)"\x41\xC7\x01\x00\x00\xC8\x42", 7);

		// Bloodgoyles -- dmc3.exe+E3371 - 41 C7 01 00 00 16 43 - mov [r9],43160000 { 150,00 }
		_patch((char*)(appBaseAddr + 0xE3371), (char*)"\x41\xC7\x01\x00\x00\x16\x43", 7);

		// Arachne -- dmc3.exe+BF725 - 41 C7 01 00 00 16 43 - mov [r9],43160000 { 150,00 }
		_patch((char*)(appBaseAddr + 0xBF725), (char*)"\x41\xC7\x01\x00\x00\x16\x43", 7);

		// Soul Eaters -- dmc3.exe+E99FE - 41 C7 01 00 00 16 43 - mov [r9],43160000 { 150,00 }
		_patch((char*)(appBaseAddr + 0xE99FE), (char*)"\x41\xC7\x01\x00\x00\x16\x43", 7);
	}
    
    run = enable;
}

void ImprovedBufferedReversals(bool enable) {
	static bool run = false;

	if (run == enable) {
		return;
	}

    uintptr_t danteAddr  = 0x201499BC;
    uintptr_t vergilAddr = 0x21758C1C;

	if (enable) {
		*(float*)(danteAddr) = 24.0f;
		*(float*)(vergilAddr) = 24.0f;
	}
	else {
		*(float*)(danteAddr) = 4.0f;
		*(float*)(vergilAddr) = 4.0f;
	}

    run = enable;
}

void DisableJCRestriction(bool enable) {
	static bool run = false;

	if (run == enable) {
		return;
	}

	if (enable) {
		_nop((char*)(appBaseAddr + 0x1E7A9F), 6);
	}
	else {
		_patch((char*)(appBaseAddr + 0x1E7A9F), (char*)"\x66\x0F\x1F\x44\x00\x00", 6);
	}
    
    run = enable;
}

void BulletStop(bool enable) {
	static bool run = false;

	if (run == enable) {
		return;
	}

	if (enable) {

		_nop((char*)(appBaseAddr + 0x77070), 10); // knockback
		_nop((char*)(appBaseAddr + 0x68C80), 10); // knockback when higher up
		_nop((char*)(appBaseAddr + 0x82380), 6);  // beowulf's hammer
	}
	else {
		_patch((char*)(appBaseAddr + 0x77070), (char*)"\xC7\x81\x20\x01\x00\x00\x01\x00\x00\x00", 10);
		_patch((char*)(appBaseAddr + 0x68C80), (char*)"\xC7\x81\x20\x01\x00\x00\x01\x00\x00\x00", 10);
		_patch((char*)(appBaseAddr + 0x82380), (char*)"\x89\xA9\x18\x01\x00\x00", 6);
	}

    run = enable;
}

void RainstormLift(bool enable) {
	static bool run = false;

	if (run == enable) {
		return;
	}

    uintptr_t rainstormLiftAddr = 0x20149B00;

	if (enable) {
		*(float*)(rainstormLiftAddr) = -0.2f;
	}
	else {
		*(float*)(rainstormLiftAddr) = -0.349999994f;
	}

	run = enable;
}

void ToggleIncreasedEnemyJuggleTime(bool enable) {
	static bool run = false;

	if (run == enable) {
		return;
	}

	// dmc3.exe+68C3A: // melee // C7 83 50 01 00 00 00 00 20 41 - mov [rbx+00000150],41200000 { 10.00 }
	// dmc3.exe+81F70: // e&i shots // C7 83 50 01 00 00 00 00 20 41 - mov [rbx+00000150],41200000 { 10.00 }


	if (enable) {
		_patch((char*)(appBaseAddr + 0x68C3A), (char*)"\xC7\x83\x50\x01\x00\x00\x00\x00\xA0\x41", 10);
		_patch((char*)(appBaseAddr + 0x81F70), (char*)"\xC7\x83\x50\x01\x00\x00\x00\x00\xA0\x41", 10);
	}
	else {
		_patch((char*)(appBaseAddr + 0x68C3A), (char*)"\xC7\x83\x50\x01\x00\x00\x00\x00\x20\x41", 10);
		_patch((char*)(appBaseAddr + 0x81F70), (char*)"\xC7\x83\x50\x01\x00\x00\x00\x00\x20\x41", 10);
	}

	run = enable;
}

void ToggleIncreasedArtemisInstantChargeResponsiveness(bool enable) {
	static bool run = false;

	if (run == enable) {
		return;
	}

	// dmc3.exe + 215E01 - 75 0A - jne dmc3.exe+215E0D
	// dmc3.exe + 215E0B - EB 08 - jmp dmc3.exe+215E15
	// dmc3.exe + 215E4B - 0F 86 27 01 00 00 - jbe dmc3.exe + 215F78
	// dmc3.exe + 215E65 - 75 16 - jne dmc3.exe + 215E7D
	// dmc3.exe + 215E6F - 75 05 - jne dmc3.exe + 215E76
	// dmc3.exe + 215E74 - EB 07 - jmp dmc3.exe + 215E7D
	// dmc3.exe + 215E8A - 74 29 - je dmc3.exe+215EB5
	// dmc3.exe + 215E91 - 72 ED - jb dmc3.exe+215E80
	// dmc3.exe + 215EF3 - 76 2E - jna dmc3.exe + 215F23 // Kill Blinding VisualEffect


	if (enable) {
		_nop((char*)(appBaseAddr + 0x215E01), 2);
		_nop((char*)(appBaseAddr + 0x215E0B), 2);
		_nop((char*)(appBaseAddr + 0x215E4B), 6);
		_nop((char*)(appBaseAddr + 0x215E65), 2);
		_nop((char*)(appBaseAddr + 0x215E6F), 2);
		_nop((char*)(appBaseAddr + 0x215E74), 2);
		_nop((char*)(appBaseAddr + 0x215E8A), 2);
		_nop((char*)(appBaseAddr + 0x215E91), 2);
		//_nop((char*)(appBaseAddr + 0x215EF3), 2);
	} else {
		_patch((char*)(appBaseAddr + 0x215E01), (char*)"\x75\x0A", 2);
		_patch((char*)(appBaseAddr + 0x215E0B), (char*)"\xEB\x08", 2);
		_patch((char*)(appBaseAddr + 0x215E4B), (char*)"\x0F\x86\x27\x01\x00\x00", 6);
		_patch((char*)(appBaseAddr + 0x215E65), (char*)"\x75\x16", 2);
		_patch((char*)(appBaseAddr + 0x215E6F), (char*)"\x75\x05", 2);
		_patch((char*)(appBaseAddr + 0x215E74), (char*)"\xEB\x07", 2);
		_patch((char*)(appBaseAddr + 0x215E8A), (char*)"\x74\x29", 2);
		_patch((char*)(appBaseAddr + 0x215E91), (char*)"\x72\xED", 2);
		//_patch((char*)(appBaseAddr + 0x215EF3), (char*)"\x76\x2E", 2);
	}

	run = enable;
}

#pragma endregion

#pragma region CameraStuff

void CameraSensController() {
	// dmc3.exe + 5772F - C7 87 D4010000 5677563D - mov[rdi + 000001D4], 3D567756{ (0) }
	// dmc3.exe + 5775B - C7 87 D4010000 5677563D - mov[rdi + 000001D4], 3D567756{ (0) }
	// dmc3.exe+4C6429 - 00 80 3E000080        - add [rax-7FFFFFC2],al
	// dmc3.exe+4C642F - 3E 56                 - push rsi

	// Function to patch memory with correct opcodes + float value
	static float lastFrameRate = activeConfig.frameRate;
	auto patchWithFloat = [](byte* address, const char* prefix, int prefixSize, float value) {
		char patchBytes[10];
		memcpy(patchBytes, prefix, prefixSize);
		memcpy(patchBytes + prefixSize, &value, sizeof(float));
		_patch((char*)address, patchBytes, prefixSize + sizeof(float));
		};

	float sensitivityValue;
	int sensitivityEnum;


	switch (activeCrimsonConfig.Camera.sensitivity) {
	case 0: sensitivityValue = 0.0174533f * g_frameRateMultiplier; sensitivityEnum = 0; break;
	case 1: sensitivityValue = 0.0349066f * g_frameRateMultiplier; sensitivityEnum = 1; break;
	case 2: sensitivityValue = 0.0523599f * g_frameRateMultiplier; sensitivityEnum = 2; break;
	case 3: sensitivityValue = 0.1f * g_frameRateMultiplier; sensitivityEnum = 3; break;
	default: sensitivityValue = 0.0523599f * g_frameRateMultiplier; sensitivityEnum = 2;
	}

	if (toggle.cameraSensitivity == sensitivityEnum && lastFrameRate == activeConfig.frameRate) {
		return;
	}
	
	const char prefix1[] = "\xC7\x87\xD4\x01\x00\x00";
	patchWithFloat(appBaseAddr + 0x5772F, prefix1, 6, sensitivityValue);
	patchWithFloat(appBaseAddr + 0x5775B, prefix1, 6, sensitivityValue);
	patchWithFloat(appBaseAddr + 0x4C6430, "", 0, sensitivityValue);

	toggle.cameraSensitivity = sensitivityEnum;
	lastFrameRate = activeConfig.frameRate;
}


void CameraFollowUpSpeedController(CameraData& cameraData, CameraControlMetadata& cameraMetadata) {
	auto pool_166 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_166 || !pool_166[3]) return;
	auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_166[3]);

	static std::chrono::time_point<std::chrono::steady_clock> lockOnStartTime;
	static bool isLockOnTimerActive = false;
	static byte8* targetAddr = nullptr;


	auto dynamicCameraLag = [&](float normalLag, float lockOnLag, float lockOnDuration) {
		if (mainActorData.lockOn && activeConfig.Actor.playerCount == 1) {
			// If lockOn is true and timer is not active, start the timer
			if (!isLockOnTimerActive) {
				lockOnStartTime = std::chrono::steady_clock::now();
				isLockOnTimerActive = true;
			}

			// Reset Timer if Lock On Changes Targets
			if (targetAddr != mainActorData.lockOnData.targetBaseAddr60 || mainActorData.buttons[0] & GetBinding(BINDING::CHANGE_TARGET)) {
				isLockOnTimerActive = false;
				targetAddr = mainActorData.lockOnData.targetBaseAddr60;
			}

			// Calculate elapsed time
			auto now = std::chrono::steady_clock::now();
			auto elapsedTime = std::chrono::duration<float>(now - lockOnStartTime).count();

			if (elapsedTime < lockOnDuration) {
				cameraData.cameraLag = lockOnLag;
			}
			else {
				// After lockOnDuration, return to normal follow-up speed
				cameraData.cameraLag = normalLag;
			}
		}
		else {
			// Reset timer if lockOn becomes false
			isLockOnTimerActive = false;

			// Normal camera lag
			cameraData.cameraLag = normalLag;
		}
		};

	if (activeCrimsonConfig.Camera.multiplayerCamera && activeConfig.Actor.playerCount > 1) {
		return; // Disable follow-up speed adjustment when multiplayer camera is active
	}
	else {
		if (cameraMetadata.fixedCameraAddr == 0) {
			switch (activeCrimsonConfig.Camera.followUpSpeed) {
			case 0: // Low (Vanilla Default)
				return;
				break;
			case 1: // Medium
				dynamicCameraLag(500.0f, 2000.0f, 0.5f); // Apply lockOn behavior for medium follow-up speed
				break;
			case 2: // High
				dynamicCameraLag(330.0f, 3000.0f, 0.5f); // Apply lockOn behavior for high follow-up speed
				break;
			default:
				break;
			}
		} else {
			return; // Disable follow-up speed adjustment when a fixed camera is active
		}
	}
}


void HandleDynamicSPCameraDistance(float& cameraDistance, float groundDistance, float airDistance) {
	auto pool_166 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_166 || !pool_166[3]) return;
	auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_166[3]);

	if (!(mainActorData.state & STATE::IN_AIR)) {
		if (cameraDistance > groundDistance) {
			cameraDistance = groundDistance;
		}
	}
	else {
		if (cameraDistance > (groundDistance - 10)) {
			cameraDistance = airDistance;
		}
	}
}

void HandlePanoramicSPCameraDistance(float& cameraDistance, float groundDistance, float airDistance) {
	static auto lastAdjustmentTime = std::chrono::steady_clock::now();
	const auto timeThreshold = std::chrono::milliseconds(1000); // 1 second delay

	auto pool_2128 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_2128 || !pool_2128[8]) return;
	auto& enemyVectorData = *reinterpret_cast<EnemyVectorData*>(pool_2128[8]);

	auto pool_10222 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_10222 || !pool_10222[3]) return;
	auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_10222[3]);
	auto& cloneMainActorData = *reinterpret_cast<PlayerActorData*>(mainActorData.cloneActorBaseAddr);

	const float baseResX = 1920.0f;
	const float baseResY = 1080.0f;
	float screenWidth = g_renderSize.x;
	float screenHeight = g_renderSize.y;
	float scaleFactorX = screenWidth / baseResX;
	float scaleFactorY = screenWidth / baseResY;

	// Define the screen margin buffer
	const float screenMarginForZoomIn = 115.0f * scaleFactorX;  // Safe margin for zooming in
	const float screenMarginForZoomOut = 50.0f * scaleFactorX;

	float maxDistance = (mainActorData.state & STATE::IN_AIR) ? 1000.0f : 780.0f; // Maximum allowed camera distance
	bool needZoomOut = false;

	for (int i = 0; i < activeConfig.Actor.playerCount * 2; i++) {
		float distanceTo1P = g_plEntityTo1PDistances[i];

		if (distanceTo1P >= 2800.0f) {
			HandleDynamicSPCameraDistance(cameraDistance, groundDistance, airDistance);
			return; // Early exit, no need to adjust further
		}

		auto& screenPos = g_plEntityScreenPositions[i];

		// Check if the entity is close to the edges of the screen (buffer for zooming out)
		if (screenPos.x <= screenMarginForZoomOut || screenPos.x >= (screenWidth - screenMarginForZoomOut) ||
			screenPos.y <= screenMarginForZoomOut || screenPos.y >= (screenHeight - screenMarginForZoomOut)) {
			needZoomOut = true;
		}
	}

	float closestEnemyDistance = FLT_MAX;
	for (std::size_t i = 0; i < 50; ++i) {
		auto& enemy = enemyVectorData.metadata[i];
		if (!enemy.baseAddr) continue;
		auto& enemyData = *reinterpret_cast<EnemyActorData*>(enemy.baseAddr);
		if (!enemyData.baseAddr) continue;

		SimpleVec3 enemyScreenPos = debug_draw_world_to_screen((const float*)&enemyData.position, 1.0f);
		float enemyDistance = g_enemiesTo1PDistances[i];
		if (enemyDistance < closestEnemyDistance) {
			closestEnemyDistance = enemyDistance;
		}

		if ((enemyScreenPos.x <= screenMarginForZoomOut || enemyScreenPos.x >= (screenWidth - screenMarginForZoomOut) ||
			enemyScreenPos.y <= screenMarginForZoomOut || enemyScreenPos.y >= (screenHeight - screenMarginForZoomOut)) &&
			enemyDistance < maxDistance) {
			needZoomOut = true;
		}
	}

	// Additional check: Are all entities within the screen margin for zooming in?
	bool allEntitiesInCenter = true;
	for (int i = 0; i < activeConfig.Actor.playerCount * 2; i++) {
		auto& screenPos = g_plEntityScreenPositions[i];

		if (screenPos.x <= screenMarginForZoomIn || screenPos.x >= (screenWidth - screenMarginForZoomIn) ||
			screenPos.y <= screenMarginForZoomIn || screenPos.y >= (screenHeight - screenMarginForZoomIn)) {
			allEntitiesInCenter = false;
			break; // Early exit if any entity is near the edge
		}
	}

	for (auto enemy : enemyVectorData.metadata) {
		if (!enemy.baseAddr) continue;
		auto& enemyData = *reinterpret_cast<EnemyActorData*>(enemy.baseAddr);
		if (!enemyData.baseAddr) continue;

		SimpleVec3 enemyScreenPos = debug_draw_world_to_screen((const float*)&enemyData.position, 1.0f);

		if (enemyScreenPos.x <= screenMarginForZoomIn || enemyScreenPos.x >= (screenWidth - screenMarginForZoomIn) ||
			enemyScreenPos.y <= screenMarginForZoomIn || enemyScreenPos.y >= (screenHeight - screenMarginForZoomIn)) {
			allEntitiesInCenter = false;
			break; // Early exit if any entity is near the edge
		}
	}

	if (g_inCombat) {
		// Handle camera distance adjustment based on screen position
		if (needZoomOut) {
			lastAdjustmentTime = std::chrono::steady_clock::now(); // Reset timer
			cameraDistance += 20.0f * g_frameRateMultiplier; // Increase camera distance per frame
			if (cameraDistance > maxDistance) {
				cameraDistance = maxDistance; // Cap the distance
			}
		}
		else {
			auto currentTime = std::chrono::steady_clock::now();
			auto timeSinceLastAdjustment = currentTime - lastAdjustmentTime;

			// Adjust the camera back if all entities are in the center and cooldown time has passed
			if (timeSinceLastAdjustment > timeThreshold && allEntitiesInCenter) {
				if (cameraDistance > groundDistance) {
					cameraDistance -= 10.0f * g_frameRateMultiplier; // Gradually reduce distance
					if (cameraDistance < groundDistance) {
						cameraDistance = groundDistance; // Prevent going below default distance
					}
				}
			}
		}

		// Zoom in if closest enemy is far away
		if (closestEnemyDistance > 800.0f) {
			cameraDistance -= 15.0f * g_frameRateMultiplier; // Decrease camera distance to zoom in
			if (cameraDistance < groundDistance) {
				cameraDistance = groundDistance; // Prevent going below default distance
			}
		}
	}
	else {
		// Smoothly reset to ground distance when out of combat or returning to the ground
		if (cameraDistance > groundDistance) {
			cameraDistance -= 10.0f * g_frameRateMultiplier; // Gradually reset to ground distance
			if (cameraDistance < groundDistance) {
				cameraDistance = groundDistance; // Clamp to ground distance
			}
		}
		else if (cameraDistance < groundDistance) {
			cameraDistance += 10.0f * g_frameRateMultiplier; // Gradually increase to ground distance if too low
			if (cameraDistance > groundDistance) {
				cameraDistance = groundDistance; // Clamp to ground distance
			}
		}
	}
}



void HandleMultiplayerCameraDistance(float& cameraDistance, float groundDistanceSP, float airDistanceSP) {
	static auto lastAdjustmentTime = std::chrono::steady_clock::now();
	const auto timeThreshold = std::chrono::milliseconds(1000); // 1 second delay

    const float baseResX = 1920.0f;
    const float baseResY = 1080.0f;
	float screenWidth = g_renderSize.x;
	float screenHeight = g_renderSize.y;
    float scaleFactorX = screenWidth / baseResX;
    float scaleFactorY = screenWidth / baseResY;

	// Define the screen margin buffer
	const float screenMarginForZoomIn = 380.0f * scaleFactorX;  // Safe margin for zooming in
	const float screenMarginForZoomOut = 300.0f * scaleFactorX;
	
	float maxDistance = 2800.0f; // Maximum allowed camera distance
	bool outOfView = false;

	// Check if any entity needs the camera to zoom out
	bool needZoomOut = false;
	for (int i = 0; i < activeConfig.Actor.playerCount * 2; i++) {
		float distanceTo1P = g_plEntityTo1PDistances[i];

		if (distanceTo1P >= 2800.0f) {
            HandleDynamicSPCameraDistance(cameraDistance, groundDistanceSP, airDistanceSP);
			return; // Early exit, no need to adjust further
		}

		auto& screenPos = g_plEntityScreenPositions[i];

		// Check if the entity is close to the edges of the screen (buffer for zooming out)
		if (screenPos.x <= screenMarginForZoomOut || screenPos.x >= (screenWidth - screenMarginForZoomOut) ||
			screenPos.y <= screenMarginForZoomOut || screenPos.y >= (screenHeight - screenMarginForZoomOut)) {
			needZoomOut = true;
		}
	}

	// Additional check: Are all entities within the screen margin for zooming in?
	bool allEntitiesInCenter = true;
	for (int i = 0; i < activeConfig.Actor.playerCount * 2; i++) {
		auto& screenPos = g_plEntityScreenPositions[i];

		
		if (screenPos.x <= screenMarginForZoomIn || screenPos.x >= (screenWidth - screenMarginForZoomIn) ||
			screenPos.y <= screenMarginForZoomIn || screenPos.y >= (screenHeight - screenMarginForZoomIn)) {
			allEntitiesInCenter = false;
			break; // Early exit if any entity is near the edge
		}
	}

	// Handle camera distance adjustment based on screen position
	if (needZoomOut) {
		lastAdjustmentTime = std::chrono::steady_clock::now(); // Reset timer
        cameraDistance += 20.0f * g_frameRateMultiplier; // Increase camera distance per frame
		if (cameraDistance > maxDistance) {
            cameraDistance = maxDistance; // Capping the distance
		}
	}
	else {
		auto currentTime = std::chrono::steady_clock::now();
		auto timeSinceLastAdjustment = currentTime - lastAdjustmentTime;

		// Only adjust the camera if the cooldown time has passed and all entities are in the center
		if (timeSinceLastAdjustment > timeThreshold && allEntitiesInCenter) {
			if (cameraDistance > groundDistanceSP) {
                cameraDistance -= 10.0f * g_frameRateMultiplier;
				if (cameraDistance < groundDistanceSP) {
                    cameraDistance = groundDistanceSP; // Prevent going below default distance
				}
			}
		}
	}
}


void CameraDistanceController(CameraData* cameraData, CameraControlMetadata& cameraMetadata) {
	if (activeCrimsonConfig.Camera.distance == 0 || cameraMetadata.fixedCameraAddr != 0) { // Far (Vanilla Default) // check if the camera is in a fixed pos mode
		return; 
	}

	if (activeCrimsonConfig.Camera.distance == 1) { // Closer
		if (cameraData->distance > 350) {
			cameraData->distance = 350.0f;
		}
	}

	if (activeCrimsonConfig.Camera.distance == 2) { // Dynamic

        if (g_isMPCamActive) {
            HandleMultiplayerCameraDistance(cameraData->distance, 430, 580);
        }
        else if (g_isParanoramicCamActive && g_inCombat) {
            HandlePanoramicSPCameraDistance(cameraData->distance, 430, 580);
        }
        else if (!(g_isMPCamActive || (g_isParanoramicCamActive && g_inCombat))){
            HandleDynamicSPCameraDistance(cameraData->distance, 430, 580);
        }
	}
}


void CameraLockOnDistanceController() {
    auto pool_4449 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC8FBD0);
    if (!pool_4449 || !pool_4449[147]) {
        return;
    }
    auto& cameraData = *reinterpret_cast<CameraData*>(pool_4449[147]);

    auto pool_166 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
    if (!pool_166 || !pool_166[3]) {
        return;
    }
    auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_166[3]);


    if (activeCrimsonConfig.Camera.lockOnDistance == 0) {
        return;
    }

    if (activeCrimsonConfig.Camera.lockOnDistance == 1) {
        cameraData.distanceLockOn = 360.0f;
    }

    if (activeCrimsonConfig.Camera.lockOnDistance == 2) {
		if (g_isMPCamActive) {
			HandleMultiplayerCameraDistance(cameraData.distanceLockOn, 430, 580);
		}
		else if (g_isParanoramicCamActive && g_inCombat) {
			HandlePanoramicSPCameraDistance(cameraData.distanceLockOn, 430, 580);
		}
		else if (!(g_isMPCamActive || (g_isParanoramicCamActive && g_inCombat))) {
			HandleDynamicSPCameraDistance(cameraData.distanceLockOn, 430, 580);
		}
        
    }
}

void CameraTiltController(CameraData* cameraData, CameraControlMetadata& cameraMetadata) {
    if (activeCrimsonConfig.Camera.tilt == 0 || cameraMetadata.fixedCameraAddr != 0) { // Original (Vanilla Default)
        return;
    }

    if (activeCrimsonConfig.Camera.tilt == 1) { // Closer to Ground
        cameraData->tilt = 0.103073f;
    }
}

void ForceThirdPersonCamera(bool enable) {
	static bool run = false;

	// If the function has already run in the current state, return early
	if (run == enable) {
		return;
	}

	// dmc3.exe + 558AC - 75 12 - jne dmc3.exe+558C0 { Force Fixed Cameras to go to Third Person Camera instead }
	// dmc3.exe + 5EBC6 - 7F 14 - jg dmc3.exe + 5EBDC { TPSCamera Collision to keep it going to tight spaces } -- from call dmc3.exe+5EBC0 at dmc3.exe+56832


	if (enable) {
		_nop((char*)(appBaseAddr + 0x558AC), 2);
        _patch((char*)(appBaseAddr + 0x5EBC6), (char*)"\xEB\x14", 2); // jmp dmc3.exe + 5EBDC
	}
	else {
		// Restore the original instruction at dmc3.exe + 27E86A
		_patch((char*)(appBaseAddr + 0x558AC), (char*)"\x75\x12", 2);
        _patch((char*)(appBaseAddr + 0x5EBC6), (char*)"\x7F\x14", 2);
	}

	run = enable;
}

void ToggleLockedOffCamera(bool enable) {

	static bool run = false;

	auto addr = (appBaseAddr + 0x5724D);
	auto jumpAddr = (appBaseAddr + 0x57255);
	constexpr uint32 size = 8;

	static Function func = {};

	constexpr byte8 sect0[] = {
		0x53,                                     // push rbx
		0x48, 0x8B, 0x1D, 0xE8, 0x8A, 0xC2, 0x00, // mov rbx,[dmc3.exe + C18AF8]
		0x48, 0x8B, 0x9B, 0x28, 0x0E, 0xC9, 0x00, // mov rbx,[rbx + 00C90E28]
		0x48, 0x8B, 0x5B, 0x18,                   // mov rbx,[rbx + 18]
		0x80, 0xBB, 0xFA, 0x74, 0x00, 0x00, 0x50, // cmp byte ptr[rbx + 000074FA],#80
		0x0F, 0x82, 0x58, 0x00, 0x00, 0x00,       // jb originalcodepop
		0x80, 0xBB, 0xF9, 0x74, 0x00, 0x00, 0x20, // cmp byte ptr[rbx + 000074F9],#32 // 38
		0x0F, 0x86, 0x3E, 0x00, 0x00, 0x00,       // jbe minus
		0x80, 0xBB, 0xF9, 0x74, 0x00, 0x00, 0xDF, // cmp byte ptr[rbx + 000074F9],#223
		0x0F, 0x83, 0x31, 0x00, 0x00, 0x00,       // jae minus // 57
		0x80, 0xBB, 0xF9, 0x74, 0x00, 0x00, 0x9F, // cmp byte ptr [rbx+74f9],#159
		0x0F, 0x86, 0x05, 0x00, 0x00, 0x00,       // jbe untouched // 71
		0xE9, 0x2C, 0x00, 0x00, 0x00,             // jmp originalcodepop
		// untouched:
		0x80, 0xBB, 0xF9, 0x74, 0x00, 0x00, 0x60, // cmp byte ptr [rbx+74f9],#96 // 83
		0x0F, 0x83, 0x05, 0x00, 0x00, 0x00,       // jae plus
		0xE9, 0x1A, 0x00, 0x00, 0x00,             // jmp originalcodepop // 94
		// plus:
		0xF3, 0x0F, 0x10, 0x35, 0x96, 0xFF, 0xFF, 0xFF, // movss xmm6,[camTiltDown] // 102
		0xE9, 0x0D, 0x00, 0x00, 0x00,                   // jmp originalcodepop
		// minus:
		0xF3, 0x0F, 0x10, 0x35, 0x85, 0xFF, 0xFF, 0xFF, // movss xmm6,[camTiltUp]
		0xE9, 0x00, 0x00, 0x00, 0x00,                   // jmp originalcodepop
		// originalcodepop:
		0x5B,                                           // pop rbx
		0xF3, 0x0F, 0x11, 0xB7, 0x84, 0x01, 0x00, 0x00, // movss [rdi+00000184],xmm6
	};

	constexpr byte8 sect1[] = {
		0x00, 0x00, 0xc8, 0xc2, // -100.0f // I tried and failed
		0x00, 0x00, 0x96, 0x43, // 300.0f
	};

	if (!run) {
		backupHelper.Save(addr, size);
		func = old_CreateFunction(0, jumpAddr, false, true, sizeof(sect0)); // jump at the end of asm
		CopyMemory(func.sect0, sect0, sizeof(sect0));
		WriteAddress(func.sect0 + 4, (appBaseAddr + 0xC18AF8), 4); // mov rbx,[dmc3.exe + C18AF8]
		WriteAddress(func.sect0 + 98, (appBaseAddr + 0x4D1CD4), 4);
		WriteAddress(func.sect0 + 111, (appBaseAddr + 0x4C6094), 4);
	}

	if (enable) {
		WriteJump(addr, func.addr, (size - 5));
	}
	else {
		backupHelper.Restore(addr);
	}

	run = true;
}

void DisableLockOnCamera(bool enable) {
	static bool run = false;

	// If the function has already run in the current state, return early
	if (run == enable) {
		return;
	}

	// dmc3.exe+569AD - C6 83 90 00 00 00 01 - mov byte ptr [rbx+00000090],01 { Setting this byte to 1 will trigger locked on cam }
	if (enable) {
		_nop((char*)(appBaseAddr + 0x569AD), 7);
	}
	else {
		_patch((char*)(appBaseAddr + 0x569AD), (char*)"\xC6\x83\x90\x00\x00\x00\x01", 7);
	}

	run = enable;
}

#pragma endregion

#pragma region GraphicsStuff

void DisableBlendingEffects(bool enable) {
	static bool run = false;

	// If the function has already run in the current state, return early
	if (run == enable) {
		return;
	}

	if (enable) {
		_nop((char*)(appBaseAddr + 0x315BA4), 2);
	}
	else {
		_patch((char*)(appBaseAddr + 0x315BA4), (char*)"\x75\x07", 2); // jne dmc3.exe+315BAD
	}

    run = enable;
}

#pragma endregion

#pragma region InertiaFixes

void AerialRaveInertiaFix(bool enable) {
    // This makes Aerial Rave (and subquently the Sky Dance and Air Slash "inertia fix" functions)
    // behave to world space inertia instead of going forward only.

    if (enable) {
        // Rave 1
        // movzx edx,word ptr [rbx+00003ED0]
        _patch((char*)(appBaseAddr + 0x20F58C), (char*)"\x0F\xB7\x93\xD0\x3E\x00\x00", 7); // swaps forward with inertia rotation
        _nop((char*)(appBaseAddr + 0x20F61B), 5);                                          // kills "stop"

        // Rave 2
        _patch((char*)(appBaseAddr + 0x20F70C), (char*)"\x0F\xB7\x93\xD0\x3E\x00\x00", 7);
        _nop((char*)(appBaseAddr + 0x20F79B), 5);

        // Rave 3
        _patch((char*)(appBaseAddr + 0x20F88C), (char*)"\x0F\xB7\x93\xD0\x3E\x00\x00", 7);
        _nop((char*)(appBaseAddr + 0x20F91B), 5);

        // Rave 4
        _patch((char*)(appBaseAddr + 0x20FA18), (char*)"\x0F\xB7\x93\xD0\x3E\x00\x00", 7);
        _nop((char*)(appBaseAddr + 0x20FAB2), 5);

    } else {
        // Rave 1
        // movzx edx,word ptr [rbx+000000C0]
        _patch((char*)(appBaseAddr + 0x20F58C), (char*)"\x0F\xB7\x93\xC0\x00\x00\x00", 7); // restores the forward only momentum
        // dmc3.exe+20F61B - E8 30DAFEFF           - call dmc3.exe+1FD050
        _patch((char*)(appBaseAddr + 0x20F61B), (char*)"\xE8\x30\xDA\xFE\xFF", 5);

        // Rave 2
        _patch((char*)(appBaseAddr + 0x20F70C), (char*)"\x0F\xB7\x93\xC0\x00\x00\x00", 7);
        // dmc3.exe+20F79B - E8 B0D8FEFF           - call dmc3.exe+1FD050
        _patch((char*)(appBaseAddr + 0x20F79B), (char*)"\xE8\xB0\xD8\xFE\xFF", 5);

        // Rave 3
        _patch((char*)(appBaseAddr + 0x20F88C), (char*)"\x0F\xB7\x93\xC0\x00\x00\x00", 7);
        // dmc3.exe+20F91B - E8 30D7FEFF           - call dmc3.exe+1FD050
        _patch((char*)(appBaseAddr + 0x20F91B), (char*)"\xE8\x30\xD7\xFE\xFF", 5);

        // Rave 4
        _patch((char*)(appBaseAddr + 0x20FA18), (char*)"\x0F\xB7\x93\xC0\x00\x00\x00", 7);
        // dmc3.exe+20FAB2 - E8 99D5FEFF           - call dmc3.exe+1FD050
        _patch((char*)(appBaseAddr + 0x20FAB2), (char*)"\xE8\x99\xD5\xFE\xFF", 5);
    }
}

void SkyDanceInertiaFix(bool enable) {

    if (enable) {
        // Sky Dance 1
        // movzx edx,word ptr [rbx+00003ED0]
        _patch((char*)(appBaseAddr + 0x20978C), (char*)"\x0F\xB7\x93\xD0\x3E\x00\x00", 7); // swaps forward with inertia rotation
        _nop((char*)(appBaseAddr + 0x209704), 5);                                          // kills "stop"

        // Sky Dance 2
        _patch((char*)(appBaseAddr + 0x20990C), (char*)"\x0F\xB7\x93\xD0\x3E\x00\x00", 7);

        // Sky Dance 3
        _patch((char*)(appBaseAddr + 0x209AFB), (char*)"\x0F\xB7\x93\xD0\x3E\x00\x00", 7);


    } else {
        // Sky Dance 1
        // movzx edx,word ptr [rbx+00003ED0]
        _patch((char*)(appBaseAddr + 0x20978C), (char*)"\x0F\xB7\x93\xC0\x00\x00\x00", 7); // restores the forward only momentum
        // dmc3.exe+209704 - E8 470DFFFF           - call dmc3.exe+1FA450
        _patch((char*)(appBaseAddr + 0x209704), (char*)"\xE8\x47\x0D\xFF\xFF", 5);

        // Sky Dance 2
        _patch((char*)(appBaseAddr + 0x20990C), (char*)"\x0F\xB7\x93\xC0\x00\x00\x00", 7);

        // Sky Dance 3
        _patch((char*)(appBaseAddr + 0x209AFB), (char*)"\x0F\xB7\x93\xC0\x00\x00\x00", 7);
    }
}

void AirSlashInertiaFix(bool enable) {

    if (enable) {
        // Air Slash 1
        // movzx edx,word ptr [rbx+00003ED0]
        _patch((char*)(appBaseAddr + 0x20BE9C), (char*)"\x0F\xB7\x93\xD0\x3E\x00\x00", 7); // swaps forward with inertia rotation
        // call dmc3.exe+1FB300
        _patch((char*)(appBaseAddr + 0x20BEA3), (char*)"\xE8\x58\xF4\xFE\xFF", 5); // kills "stop"

        // Air Slash 2
        // dmc3.exe+20C00C - 0FB7 93 D03E0000      - movzx edx,word ptr [rbx+00003ED0]
        _patch((char*)(appBaseAddr + 0x20C00C), (char*)"\x0F\xB7\x93\xD0\x3E\x00\x00", 7);
        // dmc3.exe+20C013 - E8 E8F2FEFF           - call dmc3.exe+1FB300
        _patch((char*)(appBaseAddr + 0x20C013), (char*)"\xE8\xE8\xF2\xFE\xFF", 5);

    } else {
        // Air Slash 1
        // movzx edx,word ptr [rbx+00003ED0]
        _patch((char*)(appBaseAddr + 0x20BE9C), (char*)"\x0F\xB7\x93\xC0\x00\x00\x00", 7); // restores the forward only momentum
        // dmc3.exe+20BEA3 - E8 F804FFFF           - call dmc3.exe+1FC3A0
        _patch((char*)(appBaseAddr + 0x20BEA3), (char*)"\xE8\xF8\x04\xFF\xFF", 5);

        // Air Slash 2
        // dmc3.exe+20C00C - 0FB7 93 C0000000      - movzx edx,word ptr [rbx+000000C0]
        _patch((char*)(appBaseAddr + 0x20C00C), (char*)"\x0F\xB7\x93\xC0\x00\x00\x00", 7);
        // dmc3.exe+20C013 - E8 8803FFFF           - call dmc3.exe+1FC3A0
        _patch((char*)(appBaseAddr + 0x20C013), (char*)"\xE8\x88\x03\xFF\xFF", 5);
    }
}

void TatsumakiInertiaFix(bool enable) {

    if (enable) {

        // movzx edx,word ptr [rbx+00003ED0]
        _patch((char*)(appBaseAddr + 0x20B1D9), (char*)"\x0F\xB7\x83\xD0\x3E\x00\x00", 7); // swaps forward with inertia rotation

    } else {

        // movzx edx,word ptr [rbx+000000C0]
        _patch((char*)(appBaseAddr + 0x20B1D9), (char*)"\x0F\xB7\x83\xC0\x00\x00\x00", 7); // restores the forward only momentum
    }
}

void InertiaFixes() {

    if (toggle.inertiaFixes != (int)activeCrimsonGameplay.Gameplay.General.inertia) {

        if (activeCrimsonGameplay.Gameplay.General.inertia) {
            AerialRaveInertiaFix(true);
            SkyDanceInertiaFix(true);
            AirSlashInertiaFix(true);
            TatsumakiInertiaFix(true);

            toggle.inertiaFixes = 1;
        } else {
            AerialRaveInertiaFix(false);
            SkyDanceInertiaFix(false);
            AirSlashInertiaFix(false);
            TatsumakiInertiaFix(false);

            toggle.inertiaFixes = 0;
        }
    }
}

void DisableAirSlashKnockback(bool enable) {
	//dmc3.exe + 5CA0C6 - 70 42 - jo dmc3.exe + 5CA10A{ Sends Knockback Value to Air Slash pt. 1
	static bool run = false;

	// If the function has already run in the current state, return early
	if (run == enable) {
		return;
	}

	if (enable) {
		// dmc3.exe+5CA0C6 - 00 00 - add [rax],al
		_patch((char*)(appBaseAddr + 0x5CA0C6), (char*)"\x00\x00", 2);
	} else {
		_patch((char*)(appBaseAddr + 0x5CA0C6), (char*)"\x70\x42", 2);
	}
	run = enable;
}

#pragma endregion

#pragma region AirTauntsStuff

void ToggleRoyalguardForceJustFrameRelease(bool enable) {
    LogFunction(enable);

    static bool run = false;
    static bool run2 = false;

    // If the current state is the same as the desired state, return early.
    if (run2 == enable) {
        return;
    }

    // Release
    {
        auto addr = (appBaseAddr + 0x20B714);
        constexpr uint32 size = 7;
        /*
        dmc3.exe+20B714 - C6 83 103E0000 01 - mov byte ptr [rbx+00003E10],01
        dmc3.exe+20B71B - 0F2F BB 30400000  - comiss xmm7,[rbx+00004030]
        */

        if (!run) {
            backupHelper.Save(addr, size);
        }

        if (enable) {
            protectionHelper.Push(addr, size);
            Write<uint32>((addr + 2), offsetof(PlayerActorData, action));
            Write<uint8>((addr + 6), ACTION_DANTE::ROYALGUARD_RELEASE_2);
            protectionHelper.Pop();
        }
        else {
            backupHelper.Restore(addr);
        }
    }

    // Air Release
//     {
//         auto addr = (appBaseAddr + 0x20BCF8);
//         constexpr uint32 size = 7;
//         /*
//         dmc3.exe+20BCF8 - C6 83 10 3E 00 00 01 - mov byte ptr [rbx+00003E10],01
//         dmc3.exe+20BCFF - 0F2F BB 30400000  - comiss xmm7,[rbx+00004030]
//         */
// 
//         if (!run) {
//             backupHelper.Save(addr, size);
//         }
// 
//         if (enable) {
//             protectionHelper.Push(addr, size);
//             Write<uint32>((addr + 2), offsetof(PlayerActorData, action));
//             Write<uint8>((addr + 6), ACTION_DANTE::ROYALGUARD_AIR_RELEASE_2);
//             protectionHelper.Pop();
//         }
//         else {
//             backupHelper.Restore(addr);
//         }
//     }

    // Air Release cheat has been moved to CrimsonDetours::SkyLaunchForceReleaseDetour
    // for Sky Launch to work. Cheat behavior stays the same. - Mia
  
    // Update the state to reflect that the function has handled the enable/disable change
    run2 = enable;

    // Mark that the backup was saved
    run = true;
}

#pragma endregion

#pragma region AirTauntStuff--UNUSED
// Replaced by SetAirTaunt in CrimsonDetours
// keeping it here for documentation only. - Mia

void OverrideTauntInAir(bool enable) {
    // This allows you to use the taunt button on the air.

    if (enable) {
        _nop((char*)(appBaseAddr + 0x1E99F2), 2);
        _nop((char*)(appBaseAddr + 0x1E9A0D), 2);

    } else {
        _patch((char*)(appBaseAddr + 0x1E99F2), (char*)"\x74\x5F", 2); // je dmc3.exe+1E9A53
        _patch((char*)(appBaseAddr + 0x1E9A0D), (char*)"\x75\x44", 2); // jne dmc3.exe+1E9A53
    }
}

void ToggleAirTaunt(bool enable) {
    // This calls Royal Release on Taunt button for Dante.

    static bool run = false;

    auto addr             = (appBaseAddr + 0x1E9A46);
    auto jumpAddr         = (appBaseAddr + 0x1E9A4B);
    constexpr uint32 size = 5;


    static Function func = {};

    constexpr byte8 sect0[] = {
        0x48, 0xBA, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rdx,Mary.stbi__de_iphone_flag_set+1
        0x66, 0xC7, 0x81, 0xA4, 0x3F, 0x00, 0x00, 0xC2, 0x00,       // mov word ptr [rcx+00003FA4],00C2
        0xE8, 0xB8, 0x09, 0x1F, 0x00,                               // call dmc3.exe+1E09D0

    };

    if (!run) {
        backupHelper.Save(addr, size);
        func = old_CreateFunction(0, jumpAddr, false, true, sizeof(sect0));
        CopyMemory(func.sect0, sect0, sizeof(sect0));
        WriteAddress(func.sect0 + 19, (appBaseAddr + 0x1E09D0), 5);
    }

    if (enable) {
        WriteJump(addr, func.addr, (size - 5));
    } else {
        backupHelper.Restore(addr);
    }

    run = true;
}

void ToggleAirTauntVergil(bool enable) {
    // This calls Beowulf Rising Sun on Taunt button for Vergil.

    static bool run = false;

    auto addr             = (appBaseAddr + 0x1E9A46);
    auto jumpAddr         = (appBaseAddr + 0x1E9A4B);
    constexpr uint32 size = 5;


    static Function func = {};

    constexpr byte8 sect0[] = {
        0x48, 0xBA, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rdx,Mary.stbi__de_iphone_flag_set+1
        0x66, 0xC7, 0x81, 0xA4, 0x3F, 0x00, 0x00, 0x19, 0x00,       // mov word ptr [rcx+00003FA4],0019
        0xE8, 0xB8, 0x09, 0x1F, 0x00,                               // call dmc3.exe+1E09D0

    };

    if (!run) {
        backupHelper.Save(addr, size);
        func = old_CreateFunction(0, jumpAddr, false, true, sizeof(sect0));
        CopyMemory(func.sect0, sect0, sizeof(sect0));
        WriteAddress(func.sect0 + 19, (appBaseAddr + 0x1E09D0), 5);
    }

    if (enable) {
        WriteJump(addr, func.addr, (size - 5));
    } else {
        backupHelper.Restore(addr);
    }

    run = true;
}

void AirTauntToggleController(byte8* actorBaseAddr) {
    if (!actorBaseAddr) {
        return;
    }
    auto& actorData      = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);
    auto& cloneActorData = *reinterpret_cast<PlayerActorData*>(actorData.cloneActorBaseAddr);

    if (actorData.character == CHARACTER::DANTE) {

        if ((actorData.state & STATE::IN_AIR && actorData.action != 195 && actorData.action != 196 && actorData.action != 197 &&
                actorData.action != 194)) {

            OverrideTauntInAir(true);
        } else {
            OverrideTauntInAir(false);
        }
    }


    if (actorData.character == CHARACTER::VERGIL) {
        if ((actorData.state & STATE::IN_AIR && actorData.action != 25) ||
            (cloneActorData.state & STATE::IN_AIR && cloneActorData.action != 25)) {
            OverrideTauntInAir(true);
        } else {
            OverrideTauntInAir(false);
        }
    }

    /*if(actorData.state & STATE::IN_AIR) {
            OverrideTauntInAir(true);
    }
    else{
            OverrideTauntInAir(false);
    }*/

    if (actorData.character == CHARACTER::DANTE && actorData.state & STATE::IN_AIR) {
        ToggleAirTaunt(true);
    } else if (actorData.character == CHARACTER::DANTE && !(actorData.state & STATE::IN_AIR)) {
        ToggleAirTaunt(false);
    }

    if ((actorData.character == CHARACTER::VERGIL && actorData.state & STATE::IN_AIR) ||
        (actorData.character == CHARACTER::VERGIL && cloneActorData.character == CHARACTER::VERGIL &&
            cloneActorData.state & STATE::IN_AIR)) {
        ToggleAirTauntVergil(true);
    } else if ((actorData.character == CHARACTER::VERGIL && !(actorData.state & STATE::IN_AIR)) ||
               (actorData.character == CHARACTER::VERGIL && cloneActorData.character == CHARACTER::VERGIL &&
                   !(cloneActorData.state & STATE::IN_AIR))) {
        ToggleAirTaunt(false);
    }
}

#pragma endregion

# pragma region Fixing

void CerberusCrashFixPart2(bool enable) {
	//dmc3.exe + 117451 - 8B 50 10 - mov edx, [rax + 10]

	static bool run = false;

	// If the function has already run in the current state, return early
	if (run == enable) {
		return;
	}

	if (enable) {
		_nop((char*)(appBaseAddr + 0x117451), 3);
	}
	else {
		_patch((char*)(appBaseAddr + 0x117451), (char*)"\x8B\x50\x10", 3);
	}
	run = enable;
}

#pragma endregion

# pragma region Enemy

void DisableEnemyTargetting1PPosition(bool enable) {
	static bool run = false;

	// If the function has already run in the current state, return early
	if (run == enable) {
		return;
	}
	// dmc3.exe + 616F2 - 0F 29 41 40 - movaps[rcx + 40], xmm0
	if (enable) {
		_nop((char*)(appBaseAddr + 0x616F2), 4);
	} else {
		_patch((char*)(appBaseAddr + 0x616F2), (char*)"\x0F\x29\x41\x40", 4);
	}
	run = enable;
}

void ToggleDisableSoulEaterInvis(bool enable) {
	static bool run = false;

	if (run == enable) {
		return;
	}

	//dmc3.exe + E94E2 - 75 24 - jne dmc3.exe + E9508{ Soul Eater goes visible comparison

	if (enable) {
		_patch((char*)(appBaseAddr + 0xE94E2), (char*)"\xEB\x24", 2); // Soul Eater goes visible regardless
	} else {
		_patch((char*)(appBaseAddr + 0xE94E2), (char*)"\x75\x24", 2); // Soul Eater goes visible only when it should
	}

	run = enable;
}

void SetEnemyDTMode(uint8 mode) {
	static uint8 run = -1;
	if (run == mode) {
		return;
	}
	// 	INSTANT ENEMY DT
	// 	dmc3.exe + 6105F - 80 B9 BC 00 00 00 00 - cmp byte ptr[rcx + 000000BC], 00 { 0 }
	// 	Change this comparison to 01 or nop the addr and you get instant enemy DT
	// 
	// 	NO ENEMY DT :
	// 	dmc3.exe + 6105D - 77 09 - ja dmc3.exe + 61068 // nop this addr
	// 	dmc3.exe + 61066 - 74 18 - je dmc3.exe + 61080 // change this addr to a jmp // EB 18

	if (mode == ENEMYDTMODE::INSTANT_DT) {
		_nop((char*)(appBaseAddr + 0x6105F), 7); // Change the cmp to 1

	} else if (mode == ENEMYDTMODE::NO_ENEMY_DT) {
		_nop((char*)(appBaseAddr + 0x6105D), 2); // NOP the ja
		_patch((char*)(appBaseAddr + 0x61066), (char*)"\xEB\x18", 2); // Change the je to a jmp
	} else {
		_patch((char*)(appBaseAddr + 0x6105F), (char*)"\x80\xB9\xBC\x00\x00\x00\x00", 7); // Change the cmp back to 0
		_patch((char*)(appBaseAddr + 0x6105D), (char*)"\x77\x09", 2); // Change the ja back
		_patch((char*)(appBaseAddr + 0x61066), (char*)"\x74\x18", 2); // Change the jmp back to a je
	}
	run = mode;
}

#pragma endregion

# pragma region Damage

void StopDamageToCerberus(bool enable) {
    // Observation: this has been moved to CrimsonDetours::SkyLaunchKillDamageCerberus.
	// this will prevent Cerberus from taking any damage, complementing Infinite HP cheat.
    // we now do this by stopping the noping the instruction that calculates damage to cerb from player, before it writes to his hp values.
    // which is one nop instead of various ones.

	//dmc3.exe + 10B7B6 - F3 0F 10 7A 0C - movss xmm7, [rdx + 0C]


    static bool run = false;

	// If the function has already run in the current state, return early
	if (run == enable) {
		return;
	}

	if (enable) {
		_nop((char*)(appBaseAddr + 0x10B7B6), 5);

	}
	else {
		_patch((char*)(appBaseAddr + 0x10B7B6), (char*)"\xF3\x0F\x10\x7A\x0C", 5);

	}

    run = enable;


    // These are the old instructions to stop damage to cerberus.
// 	    stop Cerberus Part 1 damage
// 		dmc3.exe + 10C22E - F3 0F 11 87 D0 E1 00 00 - movss[rdi + 0000E1D0], xmm0
// 
// 		stop Cerberus Part 2 Damage All Heads
// 		dmc3.exe + 10C0BE - F3 42 0F 11 84 A7 D4 E1 00 00 - movss[rdi + r12 * 4 + 0000E1D4], xmm0
// 		r12 is Cerberus head id(0, 1 or 2)
//    
//     stop Cerberus Part 2 Damage All Heads when -~30% hp threshold (it switches to this instruction for damage instead)
// 	   dmc3.exe + 10BD0B - F3 42 0F 11 84 A7 D4 E1 00 00 - movss[rdi + r12 * 4 + 0000E1D4], xmm0
// 
//     stop Cerberus Part 1 damage to the legs
//     dmc3.exe+10C1C6 - F3 0F 11 87 D0 E1 00 00   - movss [rdi+0000E1D0],xmm0
//     
//     stop Cerberus Part 2 Damage Legs 1 
//     dmc3.exe+10BC57 - F3 0F 11 87 D8 E1 00 00   - movss [rdi+0000E1D8],xmm0
// 
//     stop Cerberus Part 2 Damage Legs 2 
// 	   dmc3.exe + 10BB50 - F3 0F 11 87 D8 E1 00 00 - movss[rdi + 0000E1D8], xmm0
}

void ReduceAirTornadoDamage(bool enable) {
	static bool run = false;
	uintptr_t sometimesStart = (uintptr_t)appBaseAddr + 0x5C6A5C;
	uintptr_t sometimesStart2 = (uintptr_t)appBaseAddr + 0x5C6BEC;
	uintptr_t spin = (uintptr_t)appBaseAddr + 0x5CA4CC;
	uintptr_t knockAwayEnder = (uintptr_t)appBaseAddr + 0x5CA51C;

	// If the function has already run in the current state, return early
	if (run == enable) {
		return;
	}

// 	tatsu damage
// 	dmc3.exe + 5C6A5C // 80.0f, sometimes used at the start of the attack
// 	dmc3.exe + 5CA4CC // 100.0f, always used every spin hit
// 	dmc3.exe + 5CA51C // 100.0f, knock away ender
// 	dmc3.exe + 5C6BEC // 70.0f, sometimes used at the start of the attack
// 	dmc3.exe + 5CA88C // 1.0f, used for the swoop??????

	if (enable) {
		*(float*)(sometimesStart) = 20.0f;
		*(float*)(sometimesStart2) = 10.0f;
		*(float*)(spin) = 13.0f;
		*(float*)(knockAwayEnder) = 20.0f;
	} else {
		*(float*)(sometimesStart) = 80.0f;
		*(float*)(sometimesStart2) = 70.0f;
		*(float*)(spin) = 100.0f;
		*(float*)(knockAwayEnder) = 100.0f;
	}

	run = enable;
}

void ReduceArtemisProjectileDamage(bool enable) {
	static bool run = false;
	uintptr_t artemisProjectileDamage = (uintptr_t)appBaseAddr + 0x5CB3CC;

	// If the function has already run in the current state, return early
	if (run == enable) {
		return;
	}

	// 	dmc3.exe + 5CB3CC // 50.0f, Artemis' Normal Shot and Multi Lock Shot projectile damage

	if (enable) {
		*(float*)(artemisProjectileDamage) = 5.0f;
	} else {
		*(float*)(artemisProjectileDamage) = 50.0f;
	}

	run = enable;
}

# pragma endregion

#pragma region HudStuff

void SetRebOrbCounterDurationTillFadeOut(bool enable, float duration) {
	static bool run = false;

	// If the function has already run in the current state, return early
	if (run == enable) {
		return;
	}

	// dmc3.exe + 27E86A - C7 86 38 69 00 00 00 00 48 43 - mov [rsi + 00006938], 42B40000 { 90.00 }

	if (enable) {
		// Convert the float duration to a little-endian byte array
		char durationBytes[4];
		std::memcpy(durationBytes, &duration, sizeof(durationBytes));

		// Create the patch bytes: mov [rsi + 00006938], immediate float value
		char patchBytes[10] = { (char)0xC7, (char)0x86, (char)0x38,(char)0x69, (char)0x00,(char)0x00,
								durationBytes[0], durationBytes[1], durationBytes[2], durationBytes[3] };

		// Apply the patch to the instruction at dmc3.exe + 27E86A
		_patch((char*)(appBaseAddr + 0x27E86A), patchBytes, sizeof(patchBytes));
	}
	else {
		// Restore the original instruction at dmc3.exe + 27E86A
		_patch((char*)(appBaseAddr + 0x27E86A), (char*)"\xC7\x86\x38\x69\x00\x00\x00\x00\x42\xB4", 10);
	}

	run = enable;
}

void ToggleHideLockOn(bool enable) {
	static bool run = false;
	// If the function has already run in the current state, return early
	if (run == enable) {
		return;
	}
	//dmc3.exe + 296E77 - 75 14 - jne dmc3.exe + 296E8D{ ToggleLockOnDisplay, change this to jmp to hide permanently
	
	if (enable) {
		_patch((char*)(appBaseAddr + 0x296E77), (char*)"\xEB\x14", 2); // jmp dmc3.exe+296E8D
	} else {
		_patch((char*)(appBaseAddr + 0x296E77), (char*)"\x75\x14", 2); // jne dmc3.exe+296E8D
	}

	run = enable;
}
}
# pragma endregion


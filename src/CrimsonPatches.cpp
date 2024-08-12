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

#pragma region GameplayImprovements

namespace CrimsonPatches {

void DisableHeightRestriction() {
    uintptr_t raveAddr      = 0x20149524;
    uintptr_t rainstormAddr = 0x20149708;
    uintptr_t airMeleeAddr  = 0x2014970C;

    if (toggle.disableHeightRestriction != (int)activeConfig.Gameplay.disableHeightRestriction) {

        if (activeConfig.Gameplay.disableHeightRestriction) {
            *(float*)(raveAddr)      = 0.0f;
            *(float*)(rainstormAddr) = 0.0f;
            *(float*)(airMeleeAddr)  = 0.0f;

            _patch((char*)(appBaseAddr + 0x1E62AF), (char*)"\xE9\x2B\xFD\xFF\xFF\x90", 6); // Vergil Yamato and Beowulf
            _nop((char*)(appBaseAddr + 0x1E61EC), 6);                                      // Vergil Force Edge

            toggle.disableHeightRestriction = 1;
        } else {
            *(float*)(raveAddr)      = 80.0f;
            *(float*)(rainstormAddr) = 200.0f;
            *(float*)(airMeleeAddr)  = 120.0f;


            _patch((char*)(appBaseAddr + 0x1E62AF), (char*)"\x0F\x87\x2A\xFD\xFF\xFF", 6);
            _patch((char*)(appBaseAddr + 0x1E61EC), (char*)"\x0F\x86\xC3\x00\x00\x00", 6);

            toggle.disableHeightRestriction = 0;
        }
    }
}

void IncreasedJCSpheres() {

    if (toggle.increasedJCSpheres != (int)activeConfig.Gameplay.increasedJCSpheres) {

        if (activeConfig.Gameplay.increasedJCSpheres) {
            _patch((char*)(appBaseAddr + 0x1C1DCB), (char*)"\xF3\x0F\x5E\x0D\xB1\x4F\x31\x00", 8);

            toggle.increasedJCSpheres = 1;
        } else {
            _patch((char*)(appBaseAddr + 0x1C1DCB), (char*)"\xF3\x0F\x5E\x0D\x81\x42\x30\x00", 8);

            toggle.increasedJCSpheres = 0;
        }
    }
}

void ImprovedBufferedReversals() {
    uintptr_t danteAddr  = 0x201499BC;
    uintptr_t vergilAddr = 0x21758C1C;

    if (toggle.improvedBufferedReversals != (int)activeConfig.Gameplay.improvedBufferedReversals) {
        if (activeConfig.Gameplay.improvedBufferedReversals) {
            *(float*)(danteAddr)  = 24.0f;
            *(float*)(vergilAddr) = 24.0f;

            toggle.improvedBufferedReversals = 1;
        } else {
            *(float*)(danteAddr)  = 4.0f;
            *(float*)(vergilAddr) = 4.0f;

            toggle.improvedBufferedReversals = 0;
        }
    }
}

void DisableJCRestriction() {

    if (toggle.disableJCRestriction != (int)activeConfig.Gameplay.disableJCRestriction) {

        if (activeConfig.Gameplay.disableJCRestriction) {
            _nop((char*)(appBaseAddr + 0x1E7A9F), 6);

            toggle.disableJCRestriction = 1;
        } else {
            _patch((char*)(appBaseAddr + 0x1E7A9F), (char*)"\x66\x0F\x1F\x44\x00\x00", 6);

            toggle.disableJCRestriction = 0;
        }
    }
}

void BulletStop() {

    if (toggle.bulletStop != (int)activeConfig.Gameplay.bulletStop) {
        if (activeConfig.Gameplay.bulletStop) {

            _nop((char*)(appBaseAddr + 0x77070), 10); // knockback
            _nop((char*)(appBaseAddr + 0x68C80), 10); // knockback when higher up
            _nop((char*)(appBaseAddr + 0x82380), 6);  // beowulf's hammer

            toggle.bulletStop = 1;
        } else {
            _patch((char*)(appBaseAddr + 0x77070), (char*)"\xC7\x81\x20\x01\x00\x00\x01\x00\x00\x00", 10);
            _patch((char*)(appBaseAddr + 0x68C80), (char*)"\xC7\x81\x20\x01\x00\x00\x01\x00\x00\x00", 10);
            _patch((char*)(appBaseAddr + 0x82380), (char*)"\x89\xA9\x18\x01\x00\x00", 6);

            toggle.bulletStop = 0;
        }
    }
}

void RainstormLift() {
    uintptr_t rainstormLiftAddr = 0x20149B00;

    if (toggle.rainstormLift != (int)activeConfig.Gameplay.rainstormLift) {
        if (activeConfig.Gameplay.rainstormLift) {

            *(float*)(rainstormLiftAddr) = -0.2f;

            toggle.rainstormLift = 1;
        } else {
            *(float*)(rainstormLiftAddr) = -0.349999994f;

            toggle.rainstormLift = 0;
        }
    }
}

void DisableStagger(bool enable) {
    // used for Royalguard Rebalanced
    uintptr_t patchAddr = (uintptr_t)appBaseAddr + 0x1EC470;


    if (enable) {
		_patch((char*)(patchAddr), (char*)"\x0F\x85\x15\x01\x00\x00", 6); // dmc3.exe+1EC470 - jne dmc3.exe+1EC58B
    }
    else {
        _patch((char*)(patchAddr), (char*)"\x0F\x84\xB5\x00\x00\x00", 6); // dmc3.exe+1EC470 - je dmc3.exe+1EC52B

    }

}

#pragma endregion

#pragma region CameraStuff

void CameraSensController() {

    // original speed
    if (activeConfig.cameraSensitivity != toggle.cameraSensitivity) {
        if (activeConfig.cameraSensitivity == 0) {                                                         // Low (Vanilla Default)
            _patch((char*)(appBaseAddr + 0x5772F), (char*)"\xC7\x87\xD4\x01\x00\x00\x35\xFA\x8E\x3C", 10); // 0.0174533f
            _patch((char*)(appBaseAddr + 0x5775B), (char*)"\xC7\x87\xD4\x01\x00\x00\x35\xFA\x8E\x3C", 10);
            _patch((char*)(appBaseAddr + 0x4C6430), (char*)"\x35\xFA\x8E\x3C", 4);

            toggle.cameraSensitivity = 0;
        } else if (activeConfig.cameraSensitivity == 1) {                                                  // Medium
            _patch((char*)(appBaseAddr + 0x5772F), (char*)"\xC7\x87\xD4\x01\x00\x00\x39\xFA\x0E\x3D", 10); // 0.0349066f
            _patch((char*)(appBaseAddr + 0x5775B), (char*)"\xC7\x87\xD4\x01\x00\x00\x39\xFA\x0E\x3D", 10);
            _patch((char*)(appBaseAddr + 0x4C6430), (char*)"\x39\xFA\x0E\x3D", 4);

            toggle.cameraSensitivity = 1;
        } else if (activeConfig.cameraSensitivity == 2) {                                                  // High
            _patch((char*)(appBaseAddr + 0x5772F), (char*)"\xC7\x87\xD4\x01\x00\x00\x56\x77\x56\x3D", 10); // 0.0523599f
            _patch((char*)(appBaseAddr + 0x5775B), (char*)"\xC7\x87\xD4\x01\x00\x00\x56\x77\x56\x3D", 10);
            _patch((char*)(appBaseAddr + 0x4C6430), (char*)"\x56\x77\x56\x3D", 4);

            toggle.cameraSensitivity = 2;
        } else if (activeConfig.cameraSensitivity == 3) {                                                  // Highest
            _patch((char*)(appBaseAddr + 0x5772F), (char*)"\xC7\x87\xD4\x01\x00\x00\xCD\xCC\xCC\x3D", 10); // 0.1f
            _patch((char*)(appBaseAddr + 0x5775B), (char*)"\xC7\x87\xD4\x01\x00\x00\xCD\xCC\xCC\x3D", 10);
            _patch((char*)(appBaseAddr + 0x4C6430), (char*)"\xCD\xCC\xCC\x3D", 4);

            toggle.cameraSensitivity = 3;
        }
    }
}

void CameraFollowUpSpeedController() {
	auto pool_4449 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC8FBD0);
	if (!pool_4449 || !pool_4449[147]) {
		return;
	}
	auto& cameraData = *reinterpret_cast<CameraData*>(pool_4449[147]);

    
	if (activeConfig.cameraFollowUpSpeed == 0) { // Low (Vanilla Default)
		cameraData.cameraLag = 1000.0f;
	}
	else if (activeConfig.cameraFollowUpSpeed == 1) { // Medium
		cameraData.cameraLag = 500.0f;
	}
	else if (activeConfig.cameraFollowUpSpeed == 2) { // High
		cameraData.cameraLag = 250.0f;
	}
    
}

void CameraDistanceController() {
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

    if (activeConfig.cameraDistance == 0) { // Far (Vanilla Default)
        return;
    }

    if (activeConfig.cameraDistance == 1) { // Closer
        if (cameraData.distance > 350) {
            cameraData.distance = 350.0f;
        }
    }

    if (activeConfig.cameraDistance == 2) { // Dynamic
        if (!(mainActorData.state & STATE::IN_AIR)) {

            if (cameraData.distance > 350) {
                cameraData.distance = 350.0f;
            }
        } else {
            if (cameraData.distance > 340) {
                cameraData.distance = 500.0f;
            }
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


    if (activeConfig.cameraLockOnDistance == 0) {
        return;
    }

    if (activeConfig.cameraLockOnDistance == 1) {
        cameraData.distanceLockOn = 500.0f;
    }

    // mainActorData.position.y > 300.0f

    if (activeConfig.cameraLockOnDistance == 2) {
        if (!(mainActorData.state & STATE::IN_AIR)) {
            if (cameraData.distanceLockOn > 360.0f) {
                cameraData.distanceLockOn = 360.0f;
            }
        } else {
            if (cameraData.distanceLockOn > 350.0f) {
                cameraData.distanceLockOn = 500.0f;
            }
        }
    }
}

void CameraTiltController() {
    auto pool_4449 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC8FBD0);
    if (!pool_4449 || !pool_4449[147]) {
        return;
    }
    auto& cameraData = *reinterpret_cast<CameraData*>(pool_4449[147]);

    if (activeConfig.cameraTilt == 0) { // Original (Vanilla Default)
        return;
    }

    if (activeConfig.cameraTilt == 1) { // Closer to Ground
        cameraData.tilt = 0.103073f;
    }
}

#pragma endregion

#pragma region GraphicsStuff

void DisableBlendingEffects(bool enable) {

	if (enable) {
		_nop((char*)(appBaseAddr + 0x315BA4), 2);
	}
	else {
		_patch((char*)(appBaseAddr + 0x315BA4), (char*)"\x75\x07", 2); // jne dmc3.exe+315BAD
	}

}

#pragma endregion

#pragma region InertiaFixes

void LockedOffCameraToggle(bool enable) {

    static bool run = false;

    auto addr             = (appBaseAddr + 0x5724D);
    auto jumpAddr         = (appBaseAddr + 0x57255);
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
    } else {
        backupHelper.Restore(addr);
    }

    run = true;
}

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

    if (toggle.inertiaFixes != (int)activeConfig.Gameplay.inertia) {

        if (activeConfig.Gameplay.inertia) {
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

void DisableAirSlashKnockback() {
    // dmc3.exe+5CA0C4 0x00 0x00 0x00 0x00

    if (toggle.disableAirSlashKnockback != (int)activeConfig.Gameplay.disableAirSlashKnockback) {

        if (activeConfig.Gameplay.disableAirSlashKnockback) {
            _patch((char*)(appBaseAddr + 0x5CA0C4), (char*)"\x00\x00\x00\x00", 4);

            toggle.disableAirSlashKnockback = 1;
        } else {
            // dmc3.exe+5CA0C4 - 00 00                 - add [rax],al
            _patch((char*)(appBaseAddr + 0x5CA0C4), (char*)"\x00\x00", 2);

            toggle.disableAirSlashKnockback = 0;
        }
    }
}

#pragma endregion

#pragma region AirTauntsStuff

void ToggleRoyalguardForceJustFrameRelease(bool enable) {
    LogFunction(enable);

    static bool run = false;

    // Release
    {
        auto addr             = (appBaseAddr + 0x20B714);
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
            forcingJustFrameRoyalRelease = true;
        } else {
            backupHelper.Restore(addr);
            forcingJustFrameRoyalRelease = false;
        }
    }

    // Air Release
    {
        auto addr             = (appBaseAddr + 0x20BCF8);
        constexpr uint32 size = 7;
        /*
        dmc3.exe+20BCF8 - C6 83 103E0000 01 - mov byte ptr [rbx+00003E10],01
        dmc3.exe+20BCFF - 0F2F BB 30400000  - comiss xmm7,[rbx+00004030]
        */

        if (!run) {
            backupHelper.Save(addr, size);
        }

        if (enable) {
            protectionHelper.Push(addr, size);
            Write<uint32>((addr + 2), offsetof(PlayerActorData, action));
            Write<uint8>((addr + 6), ACTION_DANTE::ROYALGUARD_AIR_RELEASE_2);
            protectionHelper.Pop();
        } else {
            backupHelper.Restore(addr);
        }
    }

    run = true;
}

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
	if (enable) {
		_nop((char*)(appBaseAddr + 0x117451), 3);
	}
	else {
		_patch((char*)(appBaseAddr + 0x117451), (char*)"\x8B\x50\x10", 3);
	}

}

}

# pragma endregion
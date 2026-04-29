// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "Speed.hpp"
#include "Core/Core.hpp"
#include "Config.hpp"
#include "Global.hpp"
#include "Vars.hpp"

#include "Core/Macros.h"

#include "Core/DebugSwitch.hpp"

namespace Speed {

static float GetFrameResponsiveMultiplier() {
    if (g_FrameRateTimeMultiplier > 0.0f) {
        return g_FrameRateTimeMultiplier;
    }
    return 1.0f;
}

static float g_effectiveMainSpeed = 1.0f;
static float g_effectiveTurboSpeed = 1.2f;
static float g_effectiveCutsceneSpeed = 1.0f;

void UpdateEffectiveSpeeds() {
    float multiplier = GetFrameResponsiveMultiplier();
    g_effectiveMainSpeed = activeCrimsonGameplay.Cheats.Speed.defaultGame * multiplier;
    g_effectiveTurboSpeed = activeCrimsonGameplay.Cheats.Speed.turboGame * multiplier;
    g_effectiveCutsceneSpeed = 1.0f * multiplier;
}

static float GetEffectiveGlobalSpeed() {
    return IsTurbo() ? g_effectiveTurboSpeed : g_effectiveMainSpeed;
}

void ApplyRuntimeGlobalSpeed() {
	auto pool_C90E10 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
	if (!pool_C90E10 || !pool_C90E10[5]) {
		return;
	}
	auto& eventData = *reinterpret_cast<CSceneGameMain*>(pool_C90E10[5]);

    auto speeds = reinterpret_cast<float*>(appBaseAddr + 0xCF2D90);

    // Handling menu speeds
	if (activeCrimsonConfig.System.disableMenuTransitions) {
        if (eventData.event != EVENT::MAIN) {
            speeds[SPEED::GLOBAL] = 1.0f * 10000.0f;
            speeds[SPEED::GLOBAL_4] = 1.0f * 1000.0f;
        }
		else {
			speeds[SPEED::GLOBAL_4] = 1.0f;
		}
    }
    else {
        if (eventData.event != EVENT::MAIN) {
			speeds[SPEED::GLOBAL] = g_effectiveCutsceneSpeed;
        }
    }

	// Handling Cutscene speeds and Main game speed
	if (g_scene == SCENE::CUTSCENE) {
		speeds[SPEED::GLOBAL] = g_effectiveCutsceneSpeed;
	}
	else {
		if (eventData.event == EVENT::MAIN) {
            speeds[SPEED::GLOBAL] = GetEffectiveGlobalSpeed();
        }
	}
}

void Toggle(bool enable) {
    //LogFunction();

    static bool run = false;


    // Main
    {
        auto addr             = (appBaseAddr + 0x32694E);
        constexpr uint32 size = 6;
        /*
        dmc3.exe+32694E - C7 01 0000803F - mov [rcx],3F800000
        dmc3.exe+326954 - C3             - ret
        */

        if (!run) {
            backupHelper.Save(addr, size);
        }

        if (enable) {
            Write<float>((addr + 2), g_effectiveMainSpeed);
        } else {
            backupHelper.Restore(addr);
        }
    }

    // Cutscene
    {
        auto addr             = (appBaseAddr + 0x2D4D12);
        constexpr uint32 size = 10;
        /*
        dmc3.exe+2D4D12 - C7 05 74E0A100 0000803F - mov [dmc3.exe+CF2D90],3F800000
        dmc3.exe+2D4D1C - 48 8B F2                - mov rsi,rdx
        */

        if (!run) {
            backupHelper.Save(addr, size);
        }

        if (enable) {
            Write<float>((addr + 6), g_effectiveCutsceneSpeed);
        } else {
            backupHelper.Restore(addr);
        }
    }

    // Turbo
    {
        auto addr             = (appBaseAddr + 0x23E639);
        auto jumpAddr         = (appBaseAddr + 0x23E641);
        constexpr uint32 size = 8;
        /*
        dmc3.exe+23E639 - F3 0F10 05 FFDF2800 - movss xmm0,[dmc3.exe+4CC640]
        dmc3.exe+23E641 - F3 0F11 05 4747AB00 - movss [dmc3.exe+CF2D90],xmm0
        */

        static Function func = {};

        constexpr byte8 sect0[] = {
            0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rax
            0xF3, 0x0F, 0x10, 0x00,                                     // movss xmm0,[rax]
        };

        if (!run) {
            backupHelper.Save(addr, size);
            func = old_CreateFunction(0, jumpAddr, false, true, sizeof(sect0));
            CopyMemory(func.sect0, sect0, sizeof(sect0));
            *reinterpret_cast<float**>(func.sect0 + 2) = &g_effectiveTurboSpeed;
        }

        if (enable) {
            WriteJump(addr, func.addr, (size - 5));
        } else {
            backupHelper.Restore(addr);
        }
    }

    // Enemy
    {
        auto addr             = (appBaseAddr + 0x326ADA);
        constexpr uint32 size = 7;
        /*
        dmc3.exe+326ADA - C7 41 10 0000803F - mov [rcx+10],3F800000
        dmc3.exe+326AE1 - 80 B9 AC000000 01 - cmp byte ptr [rcx+000000AC],01
        */

        if (!run) {
            backupHelper.Save(addr, size);
        }

        if (enable) {
            Write((addr + 3), activeCrimsonGameplay.Cheats.Speed.enemy);
        } else {
            backupHelper.Restore(addr);
        }
    }

    // Quicksilver
    {
        auto addr             = (appBaseAddr + 0x27A982);
        auto jumpAddr         = (appBaseAddr + 0x27A992);
        constexpr uint32 size = 16;
        /*
        dmc3.exe+27A982 - F3 0F10 05 6E432500 - movss xmm0,[dmc3.exe+4CECF8]
        dmc3.exe+27A98A - F3 0F10 0D 5ADB2500 - movss xmm1,[dmc3.exe+4D84EC]
        dmc3.exe+27A992 - F3 0F11 05 8E84A700 - movss [dmc3.exe+CF2E28],xmm0
        */

        static Function func = {};

        constexpr byte8 sect0[] = {
            0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rax
            0xF3, 0x0F, 0x10, 0x00,                                     // movss xmm0,[rax]
            0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mov rax
            0xF3, 0x0F, 0x10, 0x08,                                     // movss xmm1,[rax]
        };

        if (!run) {
            backupHelper.Save(addr, size);
            func = old_CreateFunction(0, jumpAddr, false, true, sizeof(sect0));
            CopyMemory(func.sect0, sect0, sizeof(sect0));
            *reinterpret_cast<float**>(func.sect0 + 2)       = &activeCrimsonGameplay.Cheats.Speed.quicksilverPlayer;
            *reinterpret_cast<float**>(func.sect0 + 0xE + 2) = &activeCrimsonGameplay.Cheats.Speed.quicksilverEnemy;
        }

        if (enable) {
            WriteJump(addr, func.addr, (size - 5));
        } else {
            backupHelper.Restore(addr);
        }
    }

    run = true;
}

}; // namespace Speed

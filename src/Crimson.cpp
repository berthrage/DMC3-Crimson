// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "CrimsonEnemyAITarget.hpp"
#include "Core/Core.hpp"
#include "Core/Input.hpp"
#include "CrimsonDetours.hpp"
#include "DMC3Input.hpp"
#include "CrimsonSDL.hpp"
#include "File.hpp"
#include "FMOD.hpp"
#include "Graphics.hpp"
#include "Hooks.hpp"
#include "Internal.hpp"
#include "Memory.hpp"
#include "Model.hpp"
#include "CrimsonPatches.hpp"
#include "CrimsonGameplay.hpp"
#include "Actor.hpp"
#include "ActorBase.hpp"
#include "ActorRelocations.hpp"
#include "Arcade.hpp"
#include "BossRush.hpp"
#include "Config.hpp"
#include "Event.hpp"
#include "Exp.hpp"
#include "Global.hpp"
#include "Camera.hpp"
#include "CrimsonGUI.hpp"
#include "HUD.hpp"
#include "Scene.hpp"
#include "Sound.hpp"
#include "SoundRelocations.hpp"
#include "Speed.hpp"
#include "Training.hpp"
#include "Window.hpp"
#include "Vars.hpp"

#include "Core/Macros.h"

#include "Core/DebugSwitch.hpp"
#include "CrimsonFileHandling.hpp"
#include "CrimsonGameModes.hpp"



uint32 DllMain(HINSTANCE instance, uint32 reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
#ifndef NDEBUG
		AllocConsole();
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
#endif

        InitLog("logs", "Crimson.txt");

        Log("Session started.");

        if (!Core_Memory_Init()) {
            Log("Core_Memory_Init failed.");

            return 0;
        }

        if (!memoryData.InitData(64 * 1024 * 1024)) {
            Log("memoryData.InitData failed.");

            return 0;
        }

        SetMemory(memoryData.dataAddr, 0xCC, memoryData.dataSize);

        if (!protectionHelper.Init(4096)) {
            Log("protectionHelper.Init failed.");

            return 0;
        }


        if (!backupHelper.Init((8 * 1024 * 1024), (1 * 1024 * 1024))) {
            Log("backupHelper.Init failed.");

            return 0;
        }


        InitConfig();
        LoadConfig();
        LoadConfigGameplay();


        ExpConfig::InitExp();
        ExpConfig::LoadExp();

        copyHUDtoGame();

        if (!Memory_Init()) {
            Log("Memory_Init failed.");

            return 0;
        }

        Memory_ToggleExtendVectors(true);

        Internal_Init();

        if (!File_Init()) {
            Log("File_Init failed.");

            return 0;
        }

        if (!FMOD_Init()) {
            Log("FMOD_Init failed.");

            return 0;
        }

        if (!Sound_Init()) {
            Log("Sound_Init failed.");

            return 0;
        }


        /*
        Tldr: We often run toggle functions twice with false first to ensure that
        backupHelper gets the correct data.

        Toggle and ToggleRelocations share some addresses.

        If ToggleRelocations runs first Toggle will now push the modified data
        instead of the default one to backupHelper.

        This becomes problematic when the data is later restored.

        ToggleRelocations correctly writes the default data, but Toggle will write
        the modified data and this will likely cause a crash later.

        To avoid this we run the toggle functions twice. The first time with false.

        This way, ToggleRelocations writes the default data and Toggle will also
        push the default data to backupHelper.
        */

        

        Actor::Toggle(false);
        Actor::Toggle(activeConfig.Actor.enable);


        ToggleBossLadyFixes(false);
        ToggleBossLadyFixes(activeConfig.enableBossLadyFixes);

        ToggleBossVergilFixes(false);
        ToggleBossVergilFixes(activeConfig.enableBossVergilFixes);


        ToggleDergil(false);
        ToggleDergil(activeConfig.dergil);


        Camera::Toggle(false);
        Camera::Toggle(true);

        Camera::ToggleInvertX(false);
        Camera::ToggleInvertX(activeCrimsonConfig.Camera.invertX);

        Camera::ToggleDisableBossCamera(false);
        Camera::ToggleDisableBossCamera(activeCrimsonConfig.Camera.disableBossCamera);


        ToggleNoDevilForm(false);
        ToggleNoDevilForm(activeConfig.noDevilForm);


        ToggleDeplete(false);
        ToggleDeplete(true);

        ToggleOrbReach(false);
        ToggleOrbReach(true);

        ToggleDamage(false);
        ToggleDamage(true);

        UpdateCrazyComboLevelMultiplier();

        ToggleAirHikeCoreAbility(activeCrimsonGameplay.Gameplay.Dante.airHikeCoreAbility);
        CrimsonPatches::ToggleRoyalguardForceJustFrameRelease(activeCrimsonGameplay.Cheats.Dante.forceRoyalRelease);
        CrimsonPatches::DisableAirSlashKnockback(activeCrimsonGameplay.Gameplay.Dante.disableAirSlashKnockback);
        CrimsonPatches::ToggleDisableSoulEaterInvis(activeCrimsonGameplay.Gameplay.General.disableSoulEaterInvis);
        ToggleRebellionInfiniteSwordPierce(activeCrimsonGameplay.Cheats.Dante.infiniteSwordPierce);
        ToggleYamatoForceEdgeInfiniteRoundTrip(activeCrimsonGameplay.Cheats.Vergil.infiniteRoundTrip);
        ToggleEbonyIvoryFoursomeTime(activeCrimsonGameplay.Gameplay.Dante.foursomeTime);
        ToggleEbonyIvoryInfiniteRainStorm(activeCrimsonGameplay.Gameplay.Dante.infiniteRainstorm);
        ToggleArtemisSwapNormalShotAndMultiLock(activeCrimsonGameplay.Gameplay.Dante.swapArtemisMultiLockNormalShot);
        CrimsonDetours::ToggleArtemisInstantFullCharge(activeCrimsonGameplay.Gameplay.Dante.artemisRework);
        CrimsonPatches::ReduceArtemisProjectileDamage(activeCrimsonGameplay.Gameplay.Dante.artemisRework);
        ToggleChronoSwords(activeCrimsonGameplay.Cheats.Vergil.chronoSwords);
        UI::g_UIContext.SelectedGameMode = (UI::UIContext::GameModes)activeCrimsonGameplay.GameMode.preset;
        CrimsonGameModes::SetGameModeMasked(activeCrimsonGameplay.GameMode.preset);
        CrimsonGameplay::AdjustDMC4MobilitySettings();

        Arcade::Toggle(false);
        Arcade::Toggle(activeConfig.Arcade.enable);

		if (!activeCrimsonGameplay.Cheats.General.enemySpawnerTool) {
			activeConfig.enemyAutoSpawn = false;
			queuedConfig.enemyAutoSpawn = false;
		}

        // @Merge
        Event_Toggle(false);
        Event_Toggle(true);

        Event_Init();


        ToggleSkipIntro(activeConfig.skipIntro);
        ToggleSkipCutscenes(activeConfig.skipCutscenes);


        HUD_Init();

        ToggleHideMainHUD(false);
        ToggleHideMainHUD(activeConfig.hideMainHUD);

        CrimsonPatches::ToggleHideLockOn(false);
        CrimsonPatches::ToggleHideLockOn(activeConfig.hideLockOn || activeCrimsonConfig.CrimsonHudAddons.lockOn);
        CrimsonDetours::ToggleHideStyleRankHUD(activeCrimsonConfig.HudOptions.hideStyleMeter);

        ToggleHideBossHUD(false);
        ToggleHideBossHUD(activeConfig.hideBossHUD);

        ToggleForceVisibleHUD(false);
        ToggleForceVisibleHUD(activeConfig.forceVisibleHUD);

        // Overriding default additional player bars positions so as not to spawn them together in a mush initially.
        defaultConfig.barsData[1].pos = { 900, 60 };
        defaultConfig.barsData[2].pos = { 1180, 60 };
        defaultConfig.barsData[3].pos = { 1180, 140 };

        Scene::Toggle(false);
        Scene::Toggle(true);

        Speed::Toggle(false);
        Speed::Toggle(true);


        ToggleInfiniteHitPoints(activeCrimsonGameplay.Cheats.Training.infiniteHP);
        ToggleInfiniteMagicPoints(activeCrimsonGameplay.Cheats.Training.infiniteDT);
        ToggleDisableTimer(activeCrimsonGameplay.Cheats.Training.disableTimers);
        ToggleInfiniteBullets(activeCrimsonGameplay.Cheats.Training.infiniteBossLadyBullets);
        

        // Why are we calling these with false first???? - Answer: See Line 119

        ToggleForceWindowFocus(false);
        ToggleForceWindowFocus(activeConfig.forceWindowFocus);

        ToggleDisablePlayerActorIdleTimer(false);
        ToggleDisablePlayerActorIdleTimer(activeConfig.disablePlayerActorIdleTimer);

        ToggleRebellionInfiniteShredder(false);
        ToggleRebellionInfiniteShredder(activeCrimsonGameplay.Cheats.Dante.infiniteShredder);

        ToggleRebellionHoldDrive(false);
        ToggleRebellionHoldDrive(activeConfig.rebellionHoldDrive);

        XI::new_Init("xinput9_1_0.dll");

        Hooks::Init();

        CrimsonDetours::InitDetours();
        if (activeConfig.Actor.enable) {
            CrimsonDetours::ToggleHoldToCrazyCombo(activeCrimsonGameplay.Gameplay.General.holdToCrazyCombo);
        }
        else {
            CrimsonDetours::ToggleHoldToCrazyCombo(false);
        }

		if (queuedConfig.Actor.playerCount > 1) {
			activeCrimsonConfig.Camera.multiplayerCamera = true;
			queuedCrimsonConfig.Camera.multiplayerCamera = true;
			activeCrimsonConfig.Camera.forceThirdPerson = true;
			queuedCrimsonConfig.Camera.forceThirdPerson = true;
		}
        
        CrimsonPatches::HoldToAutoFire(activeCrimsonGameplay.Gameplay.General.holdToShoot);
        CrimsonDetours::ToggleClassicHUDPositionings(!activeCrimsonConfig.CrimsonHudAddons.positionings);
        CrimsonDetours::ToggleStyleRankHudNoFadeout(activeConfig.disableStyleRankHudFadeout);
        CrimsonDetours::ToggleDMC4LockOnDirection(activeCrimsonGameplay.Gameplay.General.dmc4LockOnDirection);
        CrimsonDetours::ToggleFasterTurnRate(activeCrimsonGameplay.Gameplay.General.fasterTurnRate);
        CrimsonPatches::ToggleIncreasedEnemyJuggleTime(activeCrimsonGameplay.Gameplay.General.increasedEnemyJuggleTime);
        //CrimsonPatches::SetEnemyDTMode(activeCrimsonGameplay.Gameplay.ExtraDifficulty.enemyDTMode);
        CrimsonDetours::ToggleFixBallsHangHitSpeed(true);
        CrimsonDetours::ToggleCerberusCrashFix(true);
        CrimsonDetours::ToggleVergilM3CrashFix(true);
        CrimsonDetours::ToggleMission5CrashFix(true);
        CrimsonDetours::ToggleArkhamPt2GrabCrashFix(true);
        CrimsonDetours::ToggleArkhamPt2DoppelCrashFix(true);
        CrimsonDetours::ToggleCerbDamageFix(true);
        CrimsonDetours::ToggleStyleLevellingCCSFix(true);
		CrimsonEnemyAITarget::EnemyAIMultiplayerTargettingDetours(true);
        

        CrimsonPatches::DisableBlendingEffects(false);
        CrimsonPatches::DisableBlendingEffects(activeConfig.disableBlendingEffects);
        CrimsonDetours::ToggleGreenOrbsMPRegen(true);

        // Remove FMODGetCodecDescription Label
        SetMemory((appBaseAddr + 0x5505B5), 0, 23, MemoryFlags_VirtualProtectDestination);

    }

    return 1;
}

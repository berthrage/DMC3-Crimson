#include <random>

#include "CrimsonFastcallDetours.hpp"
#include "Internal.hpp"
#include "File.hpp"
#include "Utility/Detour.hpp"
#include "Global.hpp"
#include "../Core/Core.hpp"
#include <memory>
#include <array>
#include "imgui.h"
#include <cassert>
#include "Vars.hpp"
#include "CrimsonConfig.hpp"
#include "CrimsonInput.hpp"
#include "CrimsonReversedCalls.hpp"
#include <Xinput.h>
#include "CrimsonSDL.hpp"
#include "CrimsonClock.hpp"
using namespace Utility;
namespace CrimsonFastcallDetours{

 static constexpr auto DANTE_TAUNT_OFFSET() { return 0x1FE860; }
 static constexpr auto VERGIL_TAUNT_OFFSET() { return 0x21A220; }
 static constexpr auto XINPUT_APPLY_VIBRATION_OFFSET() { return 0x042050; }
 static constexpr auto NEVAN_SHL_TRAVEL_UPDATE_OFFSET() { return 0x1D6450; }
 static std::unique_ptr<Utility::Detour_t> s_DanteTauntHook;
 static std::unique_ptr<Utility::Detour_t> s_VergilTauntHook;
 static std::unique_ptr<Utility::Detour_t> s_XinputApplyVibrationHook;
 static std::unique_ptr<Utility::Detour_t> s_NevanShlTravelUpdateHook;
 std::random_device randomDevice;
 std::mt19937 rng(randomDevice());

 /// <summary>
 /// Returns a uniformly distributed random integer in [0, range) using Mersenne Twister.
 /// Please let`s not use rand() for this.
 /// </summary>
 unsigned bounded_rand(unsigned range)
 {
     std::uniform_int_distribution<unsigned> dist(0, range - 1);
     return dist(rng);
 }

 static uintptr_t __fastcall CPlDanteTauntController_sub_1401FE860(byte8* actorBaseAddr) {
 	typedef uintptr_t (__fastcall *DanteTauntTrampoline)(byte8*);

 	uintptr_t trampoline_raw = s_DanteTauntHook->GetTrampoline();

    uintptr_t res = NULL;
        
    DanteTauntTrampoline trampoline = (DanteTauntTrampoline)trampoline_raw;

    if (!actorBaseAddr) {
        return res;
    }

    auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);

 	if (!activeCrimsonConfig.Visual.moreTauntsAnimations) {
        res = trampoline(actorBaseAddr);
 		return res;
 	}
    int stylerank = actorData.styleData.rank;
    int motionbank = bounded_rand(2);
    if (motionbank < 0 || motionbank > 2) {
        Log("Error in taunt rand function");
        res = trampoline(actorBaseAddr);
        return res;
    }
    if (actorData.recoverState[0] == 0) {
        //if (motionbank == 0) {
        //    actorData.motionArchives[MOTION_GROUP_DANTE::TAUNTS] = File_staticFiles[pl000_00_2]; 
        //    res = trampoline(actorBaseAddr);
        //    return res;
        //}
        if (motionbank == 0) {
            actorData.motionArchives[MOTION_GROUP_DANTE::TAUNTS] = newTauntDanteAnims1_pl000_00_2; // Taunt Bank 1
            actorData.recoverState[0] = 1;
        }
        if (motionbank == 1) {
            actorData.motionArchives[MOTION_GROUP_DANTE::TAUNTS] = newTauntDanteAnims2_pl000_00_2; // Taunt Bank 2
        }
        actorData.recoverState[0] = 1;
        switch (stylerank) {
        case STYLE_RANK::BLAST:
        case STYLE_RANK::ALRIGHT:
            CrimsonReversedCalls::PlayAnimation_sub_1401EFB90((uintptr_t)&actorData, MOTION_GROUP_DANTE::TAUNTS, 1, -1.0f, -1, 2, 5);
            break;
        case STYLE_RANK::SWEET:
        case STYLE_RANK::SHOWTIME:
            CrimsonReversedCalls::PlayAnimation_sub_1401EFB90((uintptr_t)&actorData, MOTION_GROUP_DANTE::TAUNTS, 2, -1.0f, -1, 2, 5);
            break;
        case STYLE_RANK::STYLISH:
            CrimsonReversedCalls::PlayAnimation_sub_1401EFB90((uintptr_t)&actorData, MOTION_GROUP_DANTE::TAUNTS, 3, -1.0f, -1, 2, 0);
            break;
        default:
            CrimsonReversedCalls::PlayAnimation_sub_1401EFB90((uintptr_t)&actorData, MOTION_GROUP_DANTE::TAUNTS, 0, -1.0f, -1, 2, 5);
            break;
        }
        return res;
    }


    res = trampoline(actorBaseAddr);
 	return res;
 }


 static uintptr_t __fastcall CPlNewVergilTauntController_sub_14021A220(byte8* actorBaseAddr) {
     typedef uintptr_t(__fastcall* VergilTauntTrampoline)(byte8*);
     auto& extramovemode = activeCrimsonGameplay.Gameplay.General.extramoves;

     uintptr_t trampoline_raw = s_VergilTauntHook->GetTrampoline();



     uintptr_t res = NULL;

     VergilTauntTrampoline trampoline = (VergilTauntTrampoline)trampoline_raw;


     if (!actorBaseAddr) {
         return res;
     }

     auto& actorData = *reinterpret_cast<PlayerActorDataVergil*>(actorBaseAddr);
     auto meleeWeaponEquipped = actorData.newWeapons[actorData.activeMeleeWeaponIndex];


     
     if (!extramovemode) {
         actorData.motionArchives[MOTION_GROUP_VERGIL::TAUNTS] = File_staticFiles[pl021_00_2];
         res = trampoline(actorBaseAddr);
         return res;
     }
     if (actorData.recoverState[0] == 0){
            if (meleeWeaponEquipped != WEAPON::YAMATO_VERGIL) {
            int fileIndex = bounded_rand(2);
            if (meleeWeaponEquipped == WEAPON::BEOWULF_VERGIL) {
                actorData.motionArchives[MOTION_GROUP_VERGIL::TAUNTS] = newTauntVergilAnims2_pl021_00_2;
            }
            //force edge
            else {
                actorData.motionArchives[MOTION_GROUP_VERGIL::TAUNTS] = newTauntVergilAnims1_pl021_00_2;
            }
            CrimsonReversedCalls::PlayAnimation_sub_1401EFB90((uintptr_t)&actorData, MOTION_GROUP_VERGIL::TAUNTS, fileIndex, -1.0f, -1, 2, 5);
            actorData.recoverState[0] = 1;
            return res;
            }
            //yamato vanilla functionality
            else {
                actorData.motionArchives[MOTION_GROUP_VERGIL::TAUNTS] = File_staticFiles[pl021_00_2];
                res = trampoline(actorBaseAddr);
                return res;
            }
     }
     res = trampoline(actorBaseAddr);
     return res;
 }

 void ModdedTauntDetour() {
 	s_DanteTauntHook =
 		std::make_unique<Utility::Detour_t>(
 			(uintptr_t)appBaseAddr + DANTE_TAUNT_OFFSET(),
 			(uintptr_t)&CPlDanteTauntController_sub_1401FE860,
 			NULL, "dante_taunt_detour");
 	bool res = s_DanteTauntHook->Toggle();
 	assert(res);
 }


 void ModdedTauntVergilDetour() {
     s_VergilTauntHook =
         std::make_unique<Utility::Detour_t>(
             (uintptr_t)appBaseAddr + VERGIL_TAUNT_OFFSET(),
             (uintptr_t)&CPlNewVergilTauntController_sub_14021A220,
             NULL, "vergil_taunt_detour");
     bool res = s_VergilTauntHook->Toggle();
     assert(res);
 }


 // Hook for the game's XInput vibration wrapper at appBaseAddr+0x042050.
 // The original reads *(a1+4) for the XInput slot, but every player vibration commands always leads to player 1.
 // We use g_vibrationPlayerIndex set by the FixMPXinputVibration Detour to route vibration to
 // the correct controller: XInput controllers (slot 0-3) use XInputSetState on the
 // physical slot; SDL-only controllers (sentinel ≥4) use CrimsonSDL::VibrateController.
 // a2[0] = left motor on/off, a2[1] = right motor on/off.
 static __int64 __fastcall XinputApplyVibrationInGame_sub_140042050(__int64 a1, uint8* a2) {
     // Build vibration values (same logic as original)
     uint8* disableFlag = reinterpret_cast<uint8*>(appBaseAddr + 0xCA8800);
     Uint16 leftMotor  = (a2 && !*disableFlag && a2[0]) ? 0xFFFFF : 0;
     Uint16 rightMotor = (a2 && !*disableFlag && a2[1]) ? 0xFFFFF : 0;

     // Helper: route vibration to a single player based on their assigned slot type.
     auto vibratePlayer = [&](int playerIndex) {
         uint8 slot = activeCrimsonInput.xinputSlots[playerIndex];
         if (slot < 4) {
             // Physical XInput slot — use XInputSetState directly
             XINPUT_VIBRATION vib = { leftMotor, rightMotor };
             XInputSetState((DWORD)slot, &vib);
         } else {
             // SDL-only controller (sentinel ≥4)
             int duration = (leftMotor == 0 && rightMotor == 0) ? 0 : 5000;
             CrimsonSDL::VibrateController(playerIndex, leftMotor, rightMotor, duration);
         }
     };

     // Check if we're in the main game. Outside of MAIN (menus, cutscenes, etc.),
     // broadcast the vibration to all players.
     // For in-game stop commands: g_vibrationPlayerIndex is only set on vibration
     // start, so a stop might target the wrong player if two controllers
     // vibrated simultaneously, we broadcast to all as a hacky solution but it seems to work quite well. - Berthrage
     bool inMainGame = false;
     auto pool_C90E10 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
     if (pool_C90E10 && pool_C90E10[5]) {
         auto& eventData = *reinterpret_cast<CSceneGameMain*>(pool_C90E10[5]);
         inMainGame = (eventData.event == EVENT::MAIN);
     }

     bool isStop = (leftMotor == 0 && rightMotor == 0);

     if (!inMainGame || isStop) {
         for (int i = 0; i < PLAYER_COUNT; ++i) {
             vibratePlayer(i);
         }
     } else {
         // In-game start: route only to the player who triggered it.
         vibratePlayer(g_vibrationPlayerIndex);
     }

     return 1;
 }

 void ModdedXinputVibrationDetour() {
     s_XinputApplyVibrationHook =
         std::make_unique<Utility::Detour_t>(
             (uintptr_t)appBaseAddr + XINPUT_APPLY_VIBRATION_OFFSET(),
             (uintptr_t)&XinputApplyVibrationInGame_sub_140042050,
             NULL, "xinput_apply_vibration_detour");
     bool res = s_XinputApplyVibrationHook->Toggle();
     assert(res);
 }


 static constexpr auto DAMAGE_CALC_OFFSET() { return 0x088190; }
 static constexpr auto COLLISION_DAMAGE_HITSTOP_TO_PLAYER_OFFSET() { return 0x1ED460; }
 static constexpr auto PLAY_SFX_WITH_POS_BY_TYPE_OFFSET() { return 0x339930; }
 static std::unique_ptr<Utility::Detour_t> s_DamageCalcHook;
 static std::unique_ptr<Utility::Detour_t> s_CollisionDmgHitstopToPlayerHook;
 static std::unique_ptr<Utility::Detour_t> s_PlaySFXWithPosByTypeHook;


 static void __fastcall ApplyDamageCalc_sub_140088190(
     uintptr_t CDamageCalcAddr,
     DamageData* dmgData,
     uintptr_t actorAddr60,
     uintptr_t floatArray) {
	 // This function is called when the game applies the damage calculation for an attack hit.
     // This is a powerful detour, which we can use to modify (almost) every damage application in the game.
     // Known exceptions are: Cerberus 
	 typedef void(__fastcall* DamageCalcTrampoline)(uintptr_t, DamageData*, uintptr_t, uintptr_t);
	 uintptr_t trampoline_raw = s_DamageCalcHook->GetTrampoline();
     DamageCalcTrampoline trampoline = (DamageCalcTrampoline)trampoline_raw;

	 auto& actorData = *reinterpret_cast<PlayerActorData*>(actorAddr60 - 0x60);
	 auto playerIndex = actorData.newPlayerIndex;
	 auto entityIndex = actorData.newEntityIndex;
	 auto& inRisingStar = (entityIndex == 0) ? crimsonPlayer[playerIndex].inRisingStar :
		 crimsonPlayer[playerIndex].inRisingStarClone;
	 auto& inYamatoHighTime = (entityIndex == 0) ? crimsonPlayer[playerIndex].inYamatoHighTime :
		 crimsonPlayer[playerIndex].inYamatoHighTimeClone;
	 auto& backslide = (entityIndex == 0) ? crimsonPlayer[playerIndex].backslide :
		 crimsonPlayer[playerIndex].backslideClone;
	 auto& skyLaunch = (entityIndex == 0) ? crimsonPlayer[playerIndex].skyLaunch :
		 crimsonPlayer[playerIndex].skyLaunchClone;
	 bool modified = false;
	 DamageData newDmgData = *dmgData; // copy of the original DmgData pointer so we can modify it without affecting the original struct's parameters

	 if (activeCrimsonGameplay.Gameplay.Dante.chargedShotgunLaunches) {
		 // CHARGED SHOTGUN SHL
		 if (((uintptr_t)dmgData == (uintptr_t)(appBaseAddr + damageDataOffsets.shotgunChargedShl))) {
			 newDmgData.knockbackAnimation = 3;
			 newDmgData.displacement = 30.0f;
			 //newDmgData.dmgValue = 7.0f; // default is 7.0f, for reference, normal shotgun's dmg is 5.0f
			 modified = true;
		 }
		 if (((uintptr_t)dmgData == (uintptr_t)(appBaseAddr + damageDataOffsets.shotgunChargedShl2))) {
			 newDmgData.knockbackAnimation = 3;
			 modified = true;
		 }

		 // GUN STINGER SHL
		 if (((uintptr_t)dmgData == (uintptr_t)(appBaseAddr + damageDataOffsets.shotgunGunStingerShl))) {
			 newDmgData.knockbackAnimation = 3;
			 newDmgData.displacement = 20.0f;
			 newDmgData.stun = 30.0f;
			 modified = true;
		 }
		 if (((uintptr_t)dmgData == (uintptr_t)(appBaseAddr + damageDataOffsets.shotgunGunStingerShl2))) {
			 newDmgData.knockbackAnimation = 3;
			 modified = true;
		 }
	 }

	 // BACKSLIDE 
	 if (backslide.performing) {
		 if ((uintptr_t)dmgData == (uintptr_t)(appBaseAddr + damageDataOffsets.shotgunShl)) {

			 newDmgData.displacement = 27.0f; // default is 7.0f
			 newDmgData.dmgValue = 20.0f; // default is 5.0f
			 modified = true;
		 }
	 }

	 // ECSTASY/ROSE THROW (NEVAN SHL)
	 if (dmgData == (DamageData*)(appBaseAddr + damageDataOffsets.nevanShl)) {
		 auto& shlActorData = *reinterpret_cast<CPl000Shl10eActor*>(actorAddr60 - 0x60);
		 if (shlActorData.roseMode) {
			 newDmgData.knockbackAnimation = 3; // default is 0
			 newDmgData.dmgValue = 1.0f; // default is 50.0f
			 newDmgData.stun = 80.0f; // default is 60.0f
			 newDmgData.displacement = 60.0f; // default is 0.0f
			 modified = true;
		 }
	 }

	 // SKY LAUNCH
	 if (skyLaunch.executing) {
		 if ((uintptr_t)dmgData == (uintptr_t)(appBaseAddr + damageDataOffsets.goldRoyalRelease)) {
			 newDmgData.hitStopDuration = 7.0f;
			 newDmgData.stun = 80.0f; // default is 600.0f
			 newDmgData.displacement = 60.0f; // default is 60.0f
			 modified = true;
		 }
	 }

	 // DRIVE & OVERDRIVE SHL (REWORK)
	 if ((uintptr_t)dmgData == (uintptr_t)(appBaseAddr + damageDataOffsets.driveShl) && activeCrimsonGameplay.Gameplay.Dante.driveRework) {
		 auto& shlActorData = *reinterpret_cast<CPl000Shl02Actor*>(actorAddr60 - 0x60);
		 auto& playerActorDataShl = *reinterpret_cast<PlayerActorData*>(shlActorData.playerActorAddr);
		 auto playerIndexShl = playerActorDataShl.newPlayerIndex;
		 auto entityIndexShl = playerActorDataShl.newEntityIndex;
		 auto& drive = (entityIndexShl == 0) ? crimsonPlayer[playerIndexShl].drive :
			 crimsonPlayer[playerIndexShl].driveClone;

		 // Cache the damage level on first hit so subsequent hits on the same projectile
		 // reuse the cached value. damageLevel == 0 means uninitialized.
		 // damageLevel mapping:
		 //   1 = Level 1          (timer < 2.0s)  -> projectile 200, physical 70
		 //   2 = Level 2          (timer >= 2.0s)  -> projectile 300, physical 70
		 //   3 = Level 3          (timer >= 5.0s)  -> projectile 700, physical 70
		 //   4 = QuickDrive       (no OverDrive)  -> projectile 200, physical 60
		 //   5 = QuickDrive+OD    (Part 2/3)      -> projectile  80, physical 60
		 if (shlActorData.damageLevel == 0) {
			 if (drive.quickInPart2 || drive.quickInPart3)
				 shlActorData.damageLevel = 5; // QuickDrive + OverDrive
			 else if (drive.inQuickDrive)
				 shlActorData.damageLevel = 4; // QuickDrive no OverDrive
			 else if (drive.levelTimer >= 5.0f)
				 shlActorData.damageLevel = 3; // Level 3
			 else if (drive.levelTimer >= 2.0f)
				 shlActorData.damageLevel = 2; // Level 2
			 else
				 shlActorData.damageLevel = 1; // Level 1
		 }

		 // Apply damage based on cached level
		 switch (shlActorData.damageLevel) {
		 case 5: newDmgData.dmgValue = 112.0f;  break;
		 case 4: newDmgData.dmgValue = 200.0f; break;
		 case 3: newDmgData.dmgValue = 700.0f; break;
		 case 2: newDmgData.dmgValue = 300.0f; break;
		 default: newDmgData.dmgValue = 200.0f; break; // Level 1
		 }
		 modified = true;
	 }

	 // DRIVE PHYSICAL HIT (DRIVE REWORK)
	 if ((uintptr_t)dmgData == (uintptr_t)(appBaseAddr + damageDataOffsets.drivePhysicalHit) && activeCrimsonGameplay.Gameplay.Dante.driveRework) {
		 auto& playerDataPhys = *reinterpret_cast<PlayerActorData*>(actorAddr60 - 0x60);
		 auto playerIndexPhys = playerDataPhys.newPlayerIndex;
		 auto entityIndexPhys = playerDataPhys.newEntityIndex;
		 auto& drivePhys = (entityIndexPhys == 0) ? crimsonPlayer[playerIndexPhys].drive :
			 crimsonPlayer[playerIndexPhys].driveClone;

		 newDmgData.dmgValue = drivePhys.inQuickDrive ? 60.0f : 70.0f;
		 modified = true;
	 }

	 // JUDGEMENT CUT (REWORK)
	 if ((uintptr_t)dmgData == (uintptr_t)(appBaseAddr + damageDataOffsets.jdcShl) &&
		 activeCrimsonGameplay.Gameplay.Vergil.judgementCutRework && ExpConfig::missionExpDataVergil.unlocks[UNLOCK_VERGIL::YAMATO_JUDGEMENT_CUT_LEVEL_1]) {
		 auto& shlActorData = *reinterpret_cast<CPl021Shl02Actor*>(actorAddr60 - 0x60);
		 auto& playerActorDataShl = *reinterpret_cast<PlayerActorData*>(shlActorData.playerActorAddr);
		 auto playerIndexShl = playerActorDataShl.newPlayerIndex;
		 auto entityIndexShl = playerActorDataShl.newEntityIndex;
		 auto& jCut = (entityIndexShl == 0) ? crimsonPlayer[playerIndexShl].jCut :
			 crimsonPlayer[playerIndexShl].jCutClone;
		 if (shlActorData.justFrame) {
			 // We 1/4 the damage because Just Frame JDCs hit four times.
			 newDmgData.knockbackAnimation = 2;
			 newDmgData.attackReactionIdx = 4;
			 newDmgData.displacement = 5.0f;
			 newDmgData.dmgValue = 45.0f;
			 newDmgData.angle = 0.0f;
			 newDmgData.styleMeterIncrease = 80.0f;
			 newDmgData.knockbackImpact = 10.0f;
		 }
		 else {
			 newDmgData.knockbackAnimation = 0;
			 newDmgData.attackReactionIdx = 6;
			 newDmgData.displacement = 300.0f;
			 newDmgData.dmgValue = 120.0f;
			 newDmgData.angle = 10.0f;
			 newDmgData.styleMeterIncrease = 260.0f;
			 newDmgData.knockbackImpact = 20.0f;
		 }
		 modified = true;

		 // defaults:
		 /*newDmgData.knockbackAnimation = 0;
		 newDmgData.attackReactionIdx = 6;
		 newDmgData.displacement = 300.0f;
		 newDmgData.dmgValue = 300.0f;
		 newDmgData.angle = 10.0f;
		 newDmgData.styleMeterIncrease = 300.0f;
		 newDmgData.knockbackImpact = 20.0f;*/
	 }


	 // RISING STAR
	 if ((uintptr_t)dmgData == (uintptr_t)(appBaseAddr + damageDataOffsets.risingSunHit1)) {
		 if (inRisingStar) {
			 newDmgData.hitStopDuration = 2.0f;
			 modified = true;
		 }
	 }
	 if ((uintptr_t)dmgData == (uintptr_t)(appBaseAddr + damageDataOffsets.risingSunHit2)) {
		 if (inRisingStar) {
			 //newDmgData.stun = 700.0f; // default value is 100.0f, we increase this to guarantee the second hit will lift up DT'ed enemies.
			 newDmgData.hitStopDuration = 2.0f;
			 modified = true;
		 }
	 }

	 // YAMATO HIGH TIME
	 if ((uintptr_t)dmgData == (uintptr_t)(appBaseAddr + damageDataOffsets.forceEdgeHighTimeHit)) {
		 if (inYamatoHighTime) {
			 newDmgData.hitStopDuration = 1.0f;
			 modified = true;
		 }
	 }

	 // STORM SWORDS
	 if (activeCrimsonGameplay.Gameplay.General.extramoves && ExpConfig::missionExpDataVergil.unlocks[UNLOCK_VERGIL::STORM_SWORDS_MODDED]) {
		 if ((uintptr_t)dmgData == (uintptr_t)(appBaseAddr + damageDataOffsets.summonedSwordStormSwordsShl)) {
			 newDmgData.knockbackAnimation = 3;
			 newDmgData.displacement = 60.0f;
			 // 		newDmgData.angle = 90.0f; -- sends them to the moon (DMC5-like) if you uncomment and set kb animation to 7 :D - Berthrage
			 // 		newDmgData.knockbackImpact = 23.0f;
			 modified = true;
		 }
	 }

	 // FASTER SUMMONED SWORDS
	 if (activeCrimsonGameplay.Gameplay.General.extramoves && ExpConfig::missionExpDataVergil.unlocks[UNLOCK_VERGIL::SUMMON_SWORDS_LEVEL_4]) {
		 if ((uintptr_t)dmgData == (uintptr_t)(appBaseAddr + damageDataOffsets.summonedSwordShl)) {
			 newDmgData.dmgValue = 25.0f; // Default is 50.0f, we reduce it to compensate for the increased speed and hit frequency.
			 newDmgData.hitStopDuration = 8.0f; // Default is 3.0f, we increase this to compensate for the frequency of the hits.
			 modified = true;
		 }
	 }

	 trampoline(CDamageCalcAddr, modified ? &newDmgData : dmgData, actorAddr60, floatArray);  // call the original, then fall through
	 return;
 }

 static uintptr_t __fastcall ApplyColDamageHitstopToCPl_sub_1401ED460(
	 uintptr_t actorAddr60,
	 DamageData* dmgData,
	 uintptr_t enemyActorAddr,
	 vec4* coords) {
     // This function applies the Hitstop feedback to the player itself.   
	 typedef uintptr_t(__fastcall* CollisionDmgHitstopToPlayerTrampoline)(uintptr_t, DamageData*, uintptr_t, vec4*);
	 uintptr_t trampoline_raw = s_CollisionDmgHitstopToPlayerHook->GetTrampoline();
	 CollisionDmgHitstopToPlayerTrampoline trampoline = (CollisionDmgHitstopToPlayerTrampoline)trampoline_raw;
     uintptr_t res = NULL;

	 if (!actorAddr60) {
		 return res;
	 }
	 auto& playerActorData = *reinterpret_cast<PlayerActorData*>(actorAddr60 - 0x60);
	 auto playerIndex = playerActorData.newPlayerIndex;
	 auto entityIndex = playerActorData.newEntityIndex;
	 auto& inRisingStar = (entityIndex == 0) ? crimsonPlayer[playerIndex].inRisingStar :
		 crimsonPlayer[playerIndex].inRisingStarClone;
	 auto& inYamatoHighTime = (entityIndex == 0) ? crimsonPlayer[playerIndex].inYamatoHighTime :
		 crimsonPlayer[playerIndex].inYamatoHighTimeClone;
	 
	 DamageData newDmgData = *dmgData; // copy of the original DmgData pointer so we can modify it without affecting the original struct's parameters

	 // RISING STAR
	 if ((uintptr_t)dmgData == (uintptr_t)(appBaseAddr + damageDataOffsets.risingSunHit1)) {
		 if (inRisingStar) {
			 newDmgData.hitStopDuration = 2.0f;
		 }
	 }
	 if ((uintptr_t)dmgData == (uintptr_t)(appBaseAddr + damageDataOffsets.risingSunHit2)) {
		 if (inRisingStar) {
			 newDmgData.hitStopDuration = 2.0f;
		 }
	 }

	 // YAMATO HIGH TIME
	 if ((uintptr_t)dmgData == (uintptr_t)(appBaseAddr + damageDataOffsets.forceEdgeHighTimeHit)) {
		 if (inYamatoHighTime) {
			 newDmgData.hitStopDuration = 1.0f;
		 }
	 }
	 

	 res = trampoline(actorAddr60, &newDmgData, enemyActorAddr, coords);  // call the original, then fall through
	 return res;
 }

 void ModdedDamageCalcDetour() {
	 s_DamageCalcHook =
		 std::make_unique<Utility::Detour_t>(
			 (uintptr_t)appBaseAddr + DAMAGE_CALC_OFFSET(),
			 (uintptr_t)&ApplyDamageCalc_sub_140088190,
			 NULL, "damage_calc_detour");
	 bool res = s_DamageCalcHook->Toggle();
	 assert(res);
 }

 void ModdedCollisionDmgHitstopToPlayerDetour() {
	 s_CollisionDmgHitstopToPlayerHook =
		 std::make_unique<Utility::Detour_t>(
			 (uintptr_t)appBaseAddr + COLLISION_DAMAGE_HITSTOP_TO_PLAYER_OFFSET(),
			 (uintptr_t)&ApplyColDamageHitstopToCPl_sub_1401ED460,
			 NULL, "collision_dmg_hitstop_to_player_detour");
	 bool res = s_CollisionDmgHitstopToPlayerHook->Toggle();
	 assert(res);
 }


 static uintptr_t __fastcall PlaySFXWithPosByType_sub_140339930(uintptr_t fileAddr, uint32 index, uintptr_t posPtr, uint32 type) {
     typedef uintptr_t(__fastcall* PlaySFXWithPosByTypeTrampoline)(uintptr_t, uint32, uintptr_t, uint32);
     uintptr_t trampoline_raw = s_PlaySFXWithPosByTypeHook->GetTrampoline();
     PlaySFXWithPosByTypeTrampoline trampoline = (PlaySFXWithPosByTypeTrampoline)trampoline_raw;

	 // NEW RISING STAR SFX
	 __try {
		 if (posPtr < 0x190000) {
			 volatile uintptr_t actorBase = (uintptr_t)posPtr + 0x40;
			 auto actorPtrPtr = reinterpret_cast<volatile PlayerActorData**>(actorBase);
			 volatile PlayerActorData* actorPtr = *actorPtrPtr; // may fault -> caught below
			 if (actorPtr) {
				 auto& actorData = *actorPtr;
				 auto& inRisingStar = (actorData.newEntityIndex == 0) ? crimsonPlayer[actorData.newPlayerIndex].inRisingStar :
					 crimsonPlayer[actorData.newPlayerIndex].inRisingStarClone;
				 if (type == 12 && inRisingStar && actorData.action == ACTION_VERGIL::BEOWULF_RISING_SUN) {
					 type = 11;
				 }
			 }
		 }
	 }
	 __except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION
			   ? EXCEPTION_EXECUTE_HANDLER
			   : EXCEPTION_CONTINUE_SEARCH) {
		 // Malformed posPtr / actor chain - swallow the fault and continue.
	 }
     return trampoline(fileAddr, index, posPtr, type);
 }

 void ModdedPlaySFXWithPosByTypeDetour() {
     s_PlaySFXWithPosByTypeHook =
         std::make_unique<Utility::Detour_t>(
             (uintptr_t)appBaseAddr + PLAY_SFX_WITH_POS_BY_TYPE_OFFSET(),
             (uintptr_t)&PlaySFXWithPosByType_sub_140339930,
             NULL, "play_sfx_with_pos_by_type_detour");
     bool res = s_PlaySFXWithPosByTypeHook->Toggle();
     assert(res);
 }


 static uintptr_t __fastcall CPl000Shl10eNevanShlTravelUpdate_sub_1401D6450(uintptr_t shlActorAddr) {
     typedef uintptr_t(__fastcall* NevanShlTravelUpdateTrampoline)(uintptr_t);
     uintptr_t trampoline_raw = s_NevanShlTravelUpdateHook->GetTrampoline();
     NevanShlTravelUpdateTrampoline trampoline = (NevanShlTravelUpdateTrampoline)trampoline_raw;

	 // ROSE SHL TRAVEL ARC (GRAVITY)
     auto& shlActorData = *reinterpret_cast<CPl000Shl10eActor*>(shlActorAddr);
	 if (shlActorData.roseMode) {
		// Start with some speed
		 shlActorData.travelTime = 3.7f;
		 // Add gravity
		 shlActorData.travelVelocity.y -= 1.5f * (CrimsonClock::DeltaTime() * (shlActorData.speed / g_FrameRateTimeMultiplier));
	 }

     return trampoline(shlActorAddr);
 }

 void ModdedNevanShlTravelUpdateDetour() {
     s_NevanShlTravelUpdateHook =
         std::make_unique<Utility::Detour_t>(
             (uintptr_t)appBaseAddr + NEVAN_SHL_TRAVEL_UPDATE_OFFSET(),
             (uintptr_t)&CPl000Shl10eNevanShlTravelUpdate_sub_1401D6450,
             NULL, "nevan_shl_travel_update_detour");
     bool res = s_NevanShlTravelUpdateHook->Toggle();
     assert(res);
 }

 void InitDetours() {
     ModdedTauntDetour();
     ModdedTauntVergilDetour();
     ModdedDamageCalcDetour();
	 ModdedCollisionDmgHitstopToPlayerDetour();
     ModdedXinputVibrationDetour();
     ModdedPlaySFXWithPosByTypeDetour();
     ModdedNevanShlTravelUpdateDetour();
 }
}


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
#include "CrimsonReversedCalls.hpp"
using namespace Utility;
namespace CrimsonFastcallDetours{

 static constexpr auto DANTE_TAUNT_OFFSET() { return 0x1FE860; }
 static constexpr auto VERGIL_TAUNT_OFFSET() { return 0x21A220; }
 static std::unique_ptr<Utility::Detour_t> s_DanteTauntHook;
 static std::unique_ptr<Utility::Detour_t> s_VergilTauntHook;
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


 static constexpr auto DAMAGE_CALC_OFFSET() { return 0x088190; }
 static constexpr auto COLLISION_DAMAGE_HITSTOP_TO_PLAYER_OFFSET() { return 0x1ED460; }
 static std::unique_ptr<Utility::Detour_t> s_DamageCalcHook;
 static std::unique_ptr<Utility::Detour_t> s_CollisionDmgHitstopToPlayerHook;


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
			 newDmgData.knockbackAnimation = 3.0f;
			 newDmgData.displacement = 30.0f;
			 modified = true;
		 }
		 if (((uintptr_t)dmgData == (uintptr_t)(appBaseAddr + damageDataOffsets.shotgunChargedShl2))) {
			 newDmgData.knockbackAnimation = 3.0f;
			 modified = true;
		 }

		 // GUN STINGER SHL
		 if (((uintptr_t)dmgData == (uintptr_t)(appBaseAddr + damageDataOffsets.shotgunGunStingerShl))) {
			 newDmgData.knockbackAnimation = 3.0f;
			 newDmgData.displacement = 20.0f;
			 newDmgData.stun = 30.0f;
			 modified = true;
		 }
		 if (((uintptr_t)dmgData == (uintptr_t)(appBaseAddr + damageDataOffsets.shotgunGunStingerShl2))) {
			 newDmgData.knockbackAnimation = 3.0f;
			 modified = true;
		 }
	 }

	 // BACKSLIDE 
	 if ((uintptr_t)dmgData == (uintptr_t)(appBaseAddr + damageDataOffsets.shotgunShl)) {
		 if (backslide.performing) {
			 newDmgData.displacement = 27.0f; // default is 7.0f
			 newDmgData.dmgValue = 20.0f; // default is 5.0f
			 modified = true;
		 }
	 }

	 // SKY LAUNCH
	 if ((uintptr_t)dmgData == (uintptr_t)(appBaseAddr + damageDataOffsets.goldRoyalRelease)) {
		 if (skyLaunch.executing) {
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
		 case 5: newDmgData.dmgValue = 80.0f;  break;
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
	 if (activeCrimsonGameplay.Gameplay.Vergil.stormSwordsLaunches) {
		 if ((uintptr_t)dmgData == (uintptr_t)(appBaseAddr + damageDataOffsets.summonedSwordStormSwordsShl)) {
			 newDmgData.knockbackAnimation = 3;
			 newDmgData.displacement = 60.0f;
			 // 		newDmgData.angle = 90.0f; -- sends them to the moon (DMC5-like) if you uncomment and set kb animation to 7 :D - Berthrage
			 // 		newDmgData.knockbackImpact = 23.0f;
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


 void InitDetours() {
     ModdedTauntDetour();
     ModdedTauntVergilDetour();
     ModdedDamageCalcDetour();
	 ModdedCollisionDmgHitstopToPlayerDetour();
 }
}


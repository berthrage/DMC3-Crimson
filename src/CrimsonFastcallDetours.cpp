#include <random>
#include <cmath>

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
#include "CrimsonDetours.hpp"
#include "CrimsonClock.hpp"
#include <vector>
#include <algorithm>
#include <iterator>
#include <utility>
#include <string>
#include <cstdlib>
#include <cstring>
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


 // State of the Super Judgement Cut (SJC): started by SjcStart, played by the taunt hook below, noticed to be over by SjcPoll.
 static bool s_sjcActive           = false;
 static int s_sjcPhase             = 0;
 static uint32 s_sjcMotionStatus   = 0;       // last value returned by the motion update (4 = the motion is over)
 static uint64 s_sjcStartTick      = 0;
 static uint64 s_sjcPhaseStartTick = 0;
 static bool s_sjcSwordTouched     = false;   // Yamato was controlled by hand and has to be put back
 static byte8* s_sjcHitDropBase    = nullptr; // the actor whose hits are dropped in the damage calculation (ApplyDamageCalc)

 // Hides / shows the model the way ToggleActor does it, but without touching the input or the collision (the SJC isn't over).
 static void SjcSetHidden(uintptr_t actorBaseAddr, bool hide) {
     if (!actorBaseAddr) {
         return;
     }
     auto& actorData    = *reinterpret_cast<PlayerActorDataVergil*>(actorBaseAddr);
     auto& newActorData = GetNewActorData(actorData.newPlayerIndex, actorData.newCharacterIndex, actorData.newEntityIndex);

     newActorData.visibility = hide ? Visibility_Hide : Visibility_Default;
     actorData.visibility    = hide ? 0 : (g_quicksilver ? 2 : 1);
     actorData.shadow        = (!hide && !actorData.doppelganger) ? 1 : 0;
     actorData.lastShadow    = 0;
 }

 // Vergil can't be hurt during the SJC: the hits on him are dropped in the damage calculation (ApplyDamageCalc below), so there is no
 // damage and no reaction. His collision is not touched (switching it off made him float or sink).
 static void SjcSetInvulnerable(uintptr_t actorBaseAddr, bool invulnerable) {
     s_sjcHitDropBase = invulnerable ? reinterpret_cast<byte8*>(actorBaseAddr) : nullptr;
 }

 // The look of the Devil Trigger without a press of the button. The game does the same when the button is pressed (dmc3.exe+1E76xx):
 // the flag, the queue of the devil model, the flux effect and the update of the model. The cost of the gauge and the sound of the game are
 // left out (the move has its own: the gauge is held, the sounds are the boss's).
 static bool s_sjcForcedDevil = false; // the SJC put Vergil in the devil form, it has to take him out of it
 static void SjcSetDevilForm(PlayerActorDataVergil& actorData, bool on) {
     auto actorBase = reinterpret_cast<byte8*>(&actorData);
     actorData.devil = on;
     func_1F92C0(actorBase, on ? 1 : 0);
     DevilFluxVFX_1F94D0(actorBase, on ? DEVIL_FLUX::START : DEVIL_FLUX::END);
     *(reinterpret_cast<uint8*>(&actorData) + 0x3EAE) = 1;
     HeadflipAnimation_1F97F0(actorBase, true);
 }

 static void SjcRestoreDevilForm(uintptr_t actorBaseAddr) {
     if (s_sjcForcedDevil && actorBaseAddr) {
         s_sjcForcedDevil = false;
         SjcSetDevilForm(*reinterpret_cast<PlayerActorDataVergil*>(actorBaseAddr), false);
     }
 }

 // Judgement Cut spheres of the SJC. The boss sends one sphere = one hit; here every volley puts one sphere on every enemy around.
 static constexpr float SJC_TARGET_RANGE           = 2500.0f; // enemies farther away than this are not targeted

 // The numbers of the move, tuned by playing it.
 // A sound of the move: played `ms` after the anchor (start of the move, Vergil vanishing / showing up again, start of the last phase).
 enum SJCAnchor { SJC_ANCHOR_START, SJC_ANCHOR_HIDE, SJC_ANCHOR_SHOW, SJC_ANCHOR_LAST, SJC_ANCHOR_COUNT };
 struct SJCSfxEvent {
     const char* name;
     int anchor;
     int ms;
 };

 struct SJCTuning {
     int crouchHoldMs = 250; // into the second phase (animation 86) the crouching pose is held while the voice line goes on
     int volleys      = 10;  // spheres per enemy
     int firstDelayMs = 200; // between Vergil vanishing and the first volley
     int intervalMs   = 183; // between the volleys (the boss's spheres are 183 ms apart)
     int tailMs       = 600; // the last spheres need time before the move goes on
     int devilDamagePercent    = 120; // damage of the spheres when the move is started in the devil form (the human form: 100)
     int swordHandState        = 2;    // weaponMotionState[7] while Yamato is in the hand (Rising Star uses 2, High Time 5)
     int sheathMs              = 2900; // Yamato goes back to the scabbard this long after the start of the last phase
     int devilGaugeCostPercent = 30;  // of the whole gauge, taken after the move started in the devil form (the human form needs it full and takes it all)
     // Vibration: low frequency motor, high frequency motor (0 - 65535), duration in ms (0: none), lead in ms (it is started this much before
     // the sound of the same moment: the pad answers later than the sound mixer).
     int rumbleTrick[4]  = { 20000, 20000, 300, 0 };   // the trick: Vergil vanishes (the spheres start)
     int rumbleSwing[4]  = { 20000, 20000, 200, 100 }; // the swing of Yamato
     int rumbleSheath[4] = { 30000, 30000, 300, 100 }; // Yamato into the scabbard
     // The order of the sounds of the boss's move: devil form in, the trick in the background, devil form out, the
     // swing of Yamato (two parts), Yamato into the scabbard.
     SJCSfxEvent sfx[6] = {
         { "dt_in",   SJC_ANCHOR_START, 0    },
         { "trick",   SJC_ANCHOR_HIDE,  0    },
         { "dt_out",  SJC_ANCHOR_SHOW,  0    },
         { "swing_a", SJC_ANCHOR_LAST,  900  },
         { "swing_b", SJC_ANCHOR_LAST,  1070 },
         { "sheath",  SJC_ANCHOR_LAST,  2600 },
     };
 };
 static constexpr SJCTuning g_sjcTune{};

 // The damage of the spheres of the move (the multiplier for the devil form) and whose they are; set while the move plays.
 static float s_sjcDamageScale   = 1.0f;
 static uintptr_t s_sjcActorBase = 0;

 // The pad sometimes refuses a vibration ("Controller isn't correlated yet", a quirk of SDL's Windows.Gaming.Input driver under Wine): the call
 // is repeated every 100 ms, up to 5 times.
 struct SJCRumblePending {
     bool active = false;
     uint16_t low = 0, high = 0;
     int ms = 0, player = 0, triesLeft = 0;
     uint64 nextTick = 0;
 };
 static SJCRumblePending s_sjcRumblePending;

 static void SjcRumble(const int* rumble, int playerIndex) {
     if (rumble[2] <= 0) return;
     auto clamp16 = [](int v) { return static_cast<uint16_t>(v < 0 ? 0 : (v > 65535 ? 65535 : v)); };
     if (CrimsonSDL::VibrateController(playerIndex, clamp16(rumble[0]), clamp16(rumble[1]), rumble[2])) {
         s_sjcRumblePending.active = false;
     } else {
         s_sjcRumblePending = { true, clamp16(rumble[0]), clamp16(rumble[1]), rumble[2], playerIndex, 5, GetTickCount64() + 100 };
     }
 }

 static void SjcRumbleRetry(uint64 now) {
     auto& pending = s_sjcRumblePending;
     if (!pending.active || now < pending.nextTick) return;
     if (CrimsonSDL::VibrateController(pending.player, pending.low, pending.high, pending.ms)) {
         pending.active = false;
     } else if (--pending.triesLeft <= 0) {
         pending.active = false;
     } else {
         pending.nextTick = now + 100;
     }
 }

 // May the move be started? On the ground, without Doppelganger, with the gauge: full in the human form (it takes it all),
 // in the devil form enough for the cost.
 bool SjcCanStart(uintptr_t actorBaseAddr) {
     if (!actorBaseAddr) return false;
     auto& actorData = *reinterpret_cast<PlayerActorDataVergil*>(actorBaseAddr);
     if (!newSJCAnims_pl021_00_2) return false; // the animations could not be made out of the data of the game
     if (actorData.state & STATE::IN_AIR) return false;
     if (actorData.doppelganger) return false;
     const float need = actorData.devil ? (actorData.maxMagicPoints * g_sjcTune.devilGaugeCostPercent / 100.0f) : actorData.maxMagicPoints;
     return actorData.magicPoints >= (need - 0.5f);
 }

 // The move is over (or was cut short): the spheres hit like normal ones again.
 static void SjcFinish() {
     s_sjcDamageScale = 1.0f;
     s_sjcActorBase   = 0;
 }

 bool SjcIsActive() {
     return s_sjcActive;
 }

 // Starts the move. The taunt event is only the carrier of the animations: the taunt hook plays the phases.
 void SjcStart(uintptr_t actorBaseAddr) {
     auto& actorData  = *reinterpret_cast<PlayerActorDataVergil*>(actorBaseAddr);
     const uint64 now = GetTickCount64();

     s_sjcActive             = true;
     s_sjcPhase              = 0;
     s_sjcStartTick          = now;
     s_sjcPhaseStartTick     = now;
     actorData.newButtonMask = 0; // the SJC can't be cancelled: no buttons until it is over (same mask ToggleInput uses)
     actorData.recoverState[0] = 0;
     CrimsonReversedCalls::TriggerCPlayerEvent_sub_1401E0800(actorBaseAddr, ACTOR_EVENT::TAUNT, 0, -1);
 }

 // Called every tick while the move is on: notices that the taunt event is over (so the normal taunts work again) and puts everything back.
 void SjcPoll(uintptr_t actorBaseAddr) {
     if (!s_sjcActive || !actorBaseAddr) {
         return;
     }
     auto& actorData  = *reinterpret_cast<PlayerActorDataVergil*>(actorBaseAddr);
     const uint64 now = GetTickCount64();
     if ((now - s_sjcStartTick) <= 300 || actorData.eventData[0].event == ACTOR_EVENT::TAUNT) {
         return;
     }

     s_sjcActive             = false;
     actorData.newButtonMask = 0xFFFF;
     SjcSetHidden(actorBaseAddr, false); // in case the move was cut short while he was hidden
     SjcSetInvulnerable(actorBaseAddr, false);
     SjcRestoreDevilForm(actorBaseAddr); // in case the move was cut short in the devil form
     SjcFinish();
     // Put Yamato back in the scabbard: the SJC controls the sword by hand while it plays.
     if (s_sjcSwordTouched && actorData.nextBaseAddr) {
         actorData.weaponMotionState[7]                                  = 1;
         reinterpret_cast<Sword*>(actorData.nextBaseAddr)->hideYamatoTip = true;
         s_sjcSwordTouched                                               = false;
     }
 }

 struct SJCTarget { float x, y, z; };
 static constexpr int SJC_MAX_TARGETS = 255;

 // The lock-on keeps every point it can lock on to (a Cerberus has one per head and paw, an actor of the enemy list has only the one
 // in the middle of its body) in a linked list: lockOnData + 0x1010 is the first node, a node is { item, next }, item + 0x18 points to
 // a matrix whose translation (+0x30) is the position of the point (see dmc3.exe+1BBCD9, where the game walks the same list).
 static int GatherLockOnPoints(PlayerActorDataVergil& actorData, SJCTarget* out, int maxCount) {
     volatile int count = 0;
     __try {
         uint8* node = *reinterpret_cast<uint8**>(reinterpret_cast<uint8*>(&actorData.lockOnData) + 0x1010);
         for (int guard = 0; node && guard < 255 && count < maxCount; ++guard) {
             uint8* item = *reinterpret_cast<uint8**>(node);
             node        = *reinterpret_cast<uint8**>(node + 8);
             if (!item) continue;
             uint8* holder = *reinterpret_cast<uint8**>(item + 0x18);
             if (!holder) continue;
             const float* p = reinterpret_cast<const float*>(holder + 0x30);
             const float dx = p[0] - actorData.position.x;
             const float dy = p[1] - actorData.position.y;
             const float dz = p[2] - actorData.position.z;
             if (!(std::sqrt(dx * dx + dy * dy + dz * dz) <= SJC_TARGET_RANGE)) continue; // also drops NaN
             out[count] = { p[0], p[1], p[2] };
             count = count + 1;
         }
     } __except (EXCEPTION_EXECUTE_HANDLER) {
     }
     return count;
 }

 static void SpawnSJCVolley(PlayerActorDataVergil& actorData, int volley) {
     std::array<SJCTarget, SJC_MAX_TARGETS> targets;
     int targetCount = GatherLockOnPoints(actorData, targets.data(), static_cast<int>(targets.size()));
     const int lockOnPoints = targetCount;

     const float heightOffset = (actorData.state & STATE::IN_AIR) ? 0.0f : 120.0f; // like the lock-on tracking of the normal judgement cut
     auto pool = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
     if (targetCount == 0 && pool && pool[8]) { // no points to lock on to: the actors of the enemy list
         auto& enemyVectorData = *reinterpret_cast<EnemyVectorData*>(pool[8]);
         for (std::size_t i = 0; i < 50; ++i) { // the list holds 50 entries
             auto& metadata = enemyVectorData.metadata[i];
             if (!metadata.baseAddr) continue;
             auto& enemyData = *reinterpret_cast<EnemyActorData*>(metadata.baseAddr);
             if (!enemyData.baseAddr) continue;
             const float dx = enemyData.position.x - actorData.position.x;
             const float dy = enemyData.position.y - actorData.position.y;
             const float dz = enemyData.position.z - actorData.position.z;
             if (!(std::sqrt(dx * dx + dy * dy + dz * dz) <= SJC_TARGET_RANGE)) continue; // also drops NaN
             targets[targetCount++] = { enemyData.position.x, enemyData.position.y + heightOffset, enemyData.position.z };
         }
     }
     if (targetCount == 0) {
         // Nobody around: in front of Vergil like the normal judgement cut without a lock-on.
         const float yaw = static_cast<float>(actorData.rotation) * (6.28318530717958647692f / 65536.0f);
         targets[targetCount++] = { actorData.position.x + std::sin(yaw) * 500.0f, actorData.position.y + 120.0f,
                                    actorData.position.z + std::cos(yaw) * 500.0f };
     }

     int spawned = 0;
     for (int i = 0; i < targetCount; ++i) {
         // The game loads both with movaps.
         alignas(16) float pos[4] = { targets[i].x, targets[i].y, targets[i].z, 1.0f };
         alignas(16) float rel[4] = { targets[i].x - actorData.position.x, targets[i].y - actorData.position.y,
                                      targets[i].z - actorData.position.z, 1.0f };
         const uintptr_t shl = CrimsonReversedCalls::SetJDCPosition_sub_1401DC1A0(reinterpret_cast<uintptr_t>(pos),
             reinterpret_cast<uintptr_t>(rel), reinterpret_cast<uintptr_t>(&actorData), 10);
         if (!shl) {
             break; // out of spheres
         }
         CrimsonDetours::RegisterSJCShl(shl);
         ++spawned;
     }
     if (spawned > 0) {
         CrimsonSDL::PlayJDC(actorData.newPlayerIndex, false, 0); // the cut of the normal judgement cut, once per volley
     }
     if (volley == 0) {
         for (int i = 0; i < targetCount && i < 32; ++i) {
         }
     }
 }

 static std::unique_ptr<Utility::Detour_t> s_SoundPlayHook;

 // The taunt of Vergil with Yamato: for a short while after it starts, the game's own taunt line (group 1, index 40) may give way to a
 // line of the boss (see CrimsonSDL::PlayVergilTauntVoice and SoundPlay_sub_140339320 below).
 static uint64 s_vergilTauntVoiceUntil           = 0;
 static bool s_vergilTauntDevil                  = false;
 static int s_vergilTauntPlayer                  = 0;
 static constexpr uint64 TAUNT_VOICE_WINDOW_MS   = 300;
 static constexpr int SJC_STANDARD_LINE_MS       = 1670; // length of the game's own taunt line

 // Plays the game's own taunt line of Vergil the way the taunt event does it, around our own hook of the sound.
 static void PlayGameVergilTauntLine(byte8* actorBaseAddr) {
     typedef uintptr_t(__fastcall* SoundPlayTrampoline)(uintptr_t, int32, uint32, uintptr_t, uint32, uint32, uint32);
     if (!s_SoundPlayHook || !actorBaseAddr) {
         return;
     }
     uintptr_t trampoline_raw = s_SoundPlayHook->GetTrampoline();
     auto play                = (SoundPlayTrampoline)trampoline_raw;
     play((uintptr_t)appBaseAddr + 0xD6DC90, 1, 40, (uintptr_t)actorBaseAddr + 0x80, 0, 0, 0);
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


     
     // The Super Judgement Cut plays animations of the boss Vergil on the player Vergil.
     // The boss animations come without the animation of the sword, so Yamato is controlled by hand like Crimson does for Rising Star.
     auto SetYamato = [&](bool inHand) {
         if (!actorData.nextBaseAddr) {
             return;
         }
         auto& yamato                    = *reinterpret_cast<Sword*>(actorData.nextBaseAddr);
         actorData.weaponMotionState[7] = inHand ? g_sjcTune.swordHandState : 1;
         yamato.hideYamatoTip           = !inHand;
         s_sjcSwordTouched            = true;
     };
     if (s_sjcActive && newSJCAnims_pl021_00_2) {
         // SJC: the whole move lives in one taunt event. The phases are boss animations 85, 86, 87, 89 (slots 0, 1, 2, 4); the next one
         // is started when the game reports that the motion of the previous one is over (the motion update returns 4).
         static constexpr int chainSlots[4]         = {0, 1, 2, 4};
         static constexpr uint64 phaseMaxMs[3]      = {900, 1500, 1800}; // fallback: natural length of the phase + margin
         static constexpr uint64 lastPhaseHoldMs    = 3000; // the boss ends the move ~3 s into the last animation (350 frames, 5833 ms)
         const uint64 now = GetTickCount64();
         static int volleysDone         = 0;
         static uint64 lastVolleyTick   = 0;
         static bool modelHidden        = false;
         static uint64 voiceEndTick     = 0;
         static uint64 hideTick         = 0;
         static bool sfxPlayed[32]      = {};
         static bool rumblePlayed[3]    = {};
         static uint64 anchorTicks[SJC_ANCHOR_COUNT] = {}; // when the anchors of the sounds were reached (0 = not yet)
         static float frozenMagicPoints = 0.0f;           // the Devil Trigger gauge doesn't go down while the move is played
         static bool startedInDevilRun  = false;          // the form Vergil was in when the move started

         if (actorData.recoverState[0] == 0) {
             // First tick of the event.
             s_sjcPhase     = 0;
             volleysDone    = 0;
             lastVolleyTick = now;
             modelHidden    = false;
             std::fill(std::begin(sfxPlayed), std::end(sfxPlayed), false);
             std::fill(std::begin(rumblePlayed), std::end(rumblePlayed), false);
             s_sjcRumblePending.active = false;
             std::fill(std::begin(anchorTicks), std::end(anchorTicks), static_cast<uint64>(0));
             anchorTicks[SJC_ANCHOR_START] = now;
             frozenMagicPoints = actorData.magicPoints;
             // The form he was in when the move started: it decides which voice lines he says (the devil form of the move itself doesn't).
             const bool startedInDevil = actorData.devil;
             startedInDevilRun = startedInDevil;
             s_sjcActorBase    = reinterpret_cast<uintptr_t>(&actorData);
             s_sjcDamageScale  = startedInDevil ? (g_sjcTune.devilDamagePercent / 100.0f) : 1.0f;
             SjcSetInvulnerable((uintptr_t)&actorData, true);
             // The voice line: Vergil says it while he charges; the crouching pose of the second phase is held until it is over.
             int voiceMs = CrimsonSDL::PlaySJCVoice(actorData.newPlayerIndex, startedInDevil);
             if (voiceMs < 0) {
                 PlayGameVergilTauntLine(actorBaseAddr); // the game's own taunt line
                 voiceMs = SJC_STANDARD_LINE_MS;
             }
             voiceEndTick      = now + static_cast<uint64>(voiceMs);
             // From the human form Vergil goes into the devil form for the move and out of it before the last phase (like the boss); in the
             // devil form he stays as he is. This comes after the voice line: in the devil form the game has no taunt line for him.
             if (!actorData.devil) {
                 s_sjcForcedDevil = true;
                 SjcSetDevilForm(actorData, true);
             }
             actorData.motionArchives[MOTION_GROUP_VERGIL::TAUNTS] = newSJCAnims_pl021_00_2;
             CrimsonReversedCalls::PlayAnimation_sub_1401EFB90((uintptr_t)&actorData, MOTION_GROUP_VERGIL::TAUNTS, chainSlots[0], -1.0f, -1, 2, 5);
             actorData.recoverState[0] = 1;
             s_sjcMotionStatus         = 0;
             SetYamato(false);
             return res;
         }

         // The gauge is held at the value it had when the move started (the cost of the move is taken after it).
         actorData.magicPoints = frozenMagicPoints;

         // Advance the animations once per tick and ask the game whether the motion is over (it returns 4). Vergil crouches in the second
         // phase (animation 86) before he dashes forward: while the voice line goes on, the motion isn't advanced from that moment on,
         // so the crouching pose is held. When the line is over the animation goes on (the dash) up to its end.
         static uint64 lastUpdateTick = 0;
         const bool holdPose = (s_sjcPhase == 1) && !modelHidden && (now < voiceEndTick) &&
                               ((now - s_sjcPhaseStartTick) >= static_cast<uint64>(g_sjcTune.crouchHoldMs));
         if (!holdPose && (now - lastUpdateTick) >= 8) {
             lastUpdateTick = now;
             CrimsonReversedCalls::CPlTauntTickA_sub_1401FC680((uintptr_t)&actorData, *reinterpret_cast<uint16*>(reinterpret_cast<uint8*>(&actorData) + 0x3ED0));
             const uint32 status = CrimsonReversedCalls::CPlUpdateMotions_sub_1401EF8E0((uintptr_t)&actorData);
             if (status != s_sjcMotionStatus) {
                 s_sjcMotionStatus = status;
             }
         }

         // The second phase (animation 86) lasts as long as the voice line and the judgement cut spheres. Like the boss, Vergil vanishes when
         // the animation (and the line) is over, the spheres come from then on, and he shows up again with the next animation.
         bool spheresBusy = false;
         if (s_sjcPhase == 1) {
             const bool voiceDone = (now >= voiceEndTick);
             const bool stuck     = (now - s_sjcPhaseStartTick) >= 4000; // the animation never reported the end
             if (!modelHidden && voiceDone && (s_sjcMotionStatus == 4 || stuck)) {
                 modelHidden = true;
                 hideTick    = now;
                 anchorTicks[SJC_ANCHOR_HIDE] = now;
                 SjcSetHidden((uintptr_t)&actorData, true);
             }
             if (modelHidden && volleysDone < g_sjcTune.volleys && (now - hideTick) >= static_cast<uint64>(g_sjcTune.firstDelayMs) &&
                 (now - lastVolleyTick) >= static_cast<uint64>(g_sjcTune.intervalMs)) {
                 SpawnSJCVolley(actorData, volleysDone);
                 ++volleysDone;
                 lastVolleyTick = now;
             }
             spheresBusy = !modelHidden || (volleysDone < g_sjcTune.volleys) || ((now - lastVolleyTick) < static_cast<uint64>(g_sjcTune.tailMs));
         }

         if (s_sjcPhase < 3 && !spheresBusy && (((s_sjcMotionStatus == 4) && ((now - s_sjcPhaseStartTick) >= 100)) ||
                                   ((now - s_sjcPhaseStartTick) >= phaseMaxMs[s_sjcPhase]))) {
             ++s_sjcPhase;
             s_sjcPhaseStartTick = now;
             s_sjcMotionStatus   = 0;
             if (modelHidden) {
                 modelHidden = false;
                 SjcSetHidden((uintptr_t)&actorData, false);
                 anchorTicks[SJC_ANCHOR_SHOW] = now;
             }
             if (s_sjcPhase == 3) {
                 anchorTicks[SJC_ANCHOR_LAST] = now;
             }
             CrimsonReversedCalls::PlayAnimation_sub_1401EFB90((uintptr_t)&actorData, MOTION_GROUP_VERGIL::TAUNTS, chainSlots[s_sjcPhase], -1.0f, -1, 2, 5);
         }

         // Yamato like the boss does it: scabbard in phases 1 and 2, hand later, scabbard again `sheathMs` after the start of
         // the last phase.
         const int slot = chainSlots[s_sjcPhase];
         bool inHand    = (slot >= 2);
         if (slot == 4 && (now - s_sjcPhaseStartTick) >= static_cast<uint64>(g_sjcTune.sheathMs)) {
             inHand = false;
         }
         // The sounds of the boss's move, each at its time after its anchor.
         for (std::size_t i = 0; i < std::size(g_sjcTune.sfx) && i < std::size(sfxPlayed); ++i) {
             const auto& sfx = g_sjcTune.sfx[i];
             const uint64 anchorTick = anchorTicks[sfx.anchor];
             if (!sfxPlayed[i] && anchorTick != 0 && (now - anchorTick) >= static_cast<uint64>(sfx.ms)) {
                 sfxPlayed[i] = true;
                 if (std::strcmp(sfx.name, "dt_out") == 0) {
                     SjcRestoreDevilForm((uintptr_t)&actorData); // the look of the devil form goes with its sound
                 }
                 const bool ok = CrimsonSDL::PlaySJCSfx(actorData.newPlayerIndex, sfx.name);
             }
         }
         // The vibration of the same moments (the trick, the swing, Yamato into the scabbard), started `lead` ms before the sound.
         SjcRumbleRetry(now);
         static const char* rumbleSfxNames[3] = { "trick", "swing_a", "sheath" };
         const int* rumbleSets[3]             = { g_sjcTune.rumbleTrick, g_sjcTune.rumbleSwing, g_sjcTune.rumbleSheath };
         for (int r = 0; r < 3; ++r) {
             if (rumblePlayed[r]) continue;
             for (const auto& sfx : g_sjcTune.sfx) {
                 if (std::strcmp(sfx.name, rumbleSfxNames[r]) != 0) continue;
                 const uint64 anchorTick = anchorTicks[sfx.anchor];
                 const int fireMs        = (std::max)(0, sfx.ms - rumbleSets[r][3]);
                 if (anchorTick != 0 && (now - anchorTick) >= static_cast<uint64>(fireMs)) {
                     rumblePlayed[r] = true;
                     SjcRumble(rumbleSets[r], actorData.newPlayerIndex);
                 }
                 break;
             }
         }
         SetYamato(inHand);

         if (s_sjcPhase < 3 || (now - s_sjcPhaseStartTick) < lastPhaseHoldMs) {
             // The motion update above is what the taunt event does every tick (dmc3.exe+21A220, state 1); we ignore its "the motion is
             // over" result (except to start the next phase), so the event doesn't end while the phases play.
             return res;
         }
         // Like the boss, end the move a little after Yamato is back in the scabbard (the last animation would go on for 3 more
         // seconds): the same call the original handler makes when the motion is over.
         if (modelHidden) {
             modelHidden = false;
             SjcSetHidden((uintptr_t)&actorData, false);
         }
         SjcSetInvulnerable((uintptr_t)&actorData, false);
         SjcRestoreDevilForm((uintptr_t)&actorData);
         // The cost of the move, taken now that it is over: the human form spends the whole gauge, the devil form a part of it.
         actorData.magicPoints = startedInDevilRun
             ? (std::max)(0.0f, frozenMagicPoints - actorData.maxMagicPoints * g_sjcTune.devilGaugeCostPercent / 100.0f)
             : 0.0f;
         SjcFinish();
         CrimsonReversedCalls::CPlChangeEvent_sub_1401E0030((uintptr_t)&actorData, ACTOR_EVENT::LOCK_ON);
         return res;
     }

     if (actorData.recoverState[0] == 0) {
         s_vergilTauntVoiceUntil = GetTickCount64() + TAUNT_VOICE_WINDOW_MS;
         s_vergilTauntDevil      = actorData.devil;
         s_vergilTauntPlayer     = actorData.newPlayerIndex;
     }
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

	 // the victim of a hit. dmc3.exe+88190 takes the health from the object at [CDamageCalc + 0x10] and keeps its owner (the actor + 0x60)
	 // at [CDamageCalc + 0x18]; actorAddr60 is the other party, the attacker. During the SJC the hits on Vergil are dropped here (no damage,
	 // no reaction, the collision with the world stays).
	 {
		 const uintptr_t victimActor60 = CDamageCalcAddr ? *reinterpret_cast<const uintptr_t*>(CDamageCalcAddr + 0x18) : 0;
		 const uintptr_t victimBase    = victimActor60 ? (victimActor60 - 0x60) : 0;

		 if (s_sjcHitDropBase && victimBase && victimBase == reinterpret_cast<uintptr_t>(s_sjcHitDropBase)) {
			 return;
		 }
	 }

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

	 // the spheres of the Super Judgement Cut that was started in the devil form hit harder (after all the other changes of the hit).
	 if (s_sjcActorBase && (uintptr_t)dmgData == (uintptr_t)(appBaseAddr + damageDataOffsets.jdcShl)) {
		 auto& sjcShlActorData = *reinterpret_cast<CPl021Shl02Actor*>(actorAddr60 - 0x60);
		 if (sjcShlActorData.playerActorAddr == s_sjcActorBase) {
			 newDmgData.dmgValue *= s_sjcDamageScale;
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

 // The sound the game plays (dmc3.exe+339320: sound object, group, index, position, type). It is hooked to let the taunt of Vergil with Yamato
 // (group 1, index 40) give way to a line of the boss, see CrimsonSDL::PlayVergilTauntVoice.
 static constexpr auto SOUND_PLAY_OFFSET() { return 0x339320; }
 
 static uintptr_t __fastcall SoundPlay_sub_140339320(uintptr_t soundObj, int32 group, uint32 index, uintptr_t pos, uint32 type, uint32 a6, uint32 a7) {
	 typedef uintptr_t(__fastcall* SoundPlayTrampoline)(uintptr_t, int32, uint32, uintptr_t, uint32, uint32, uint32);
	 uintptr_t trampoline_raw = s_SoundPlayHook->GetTrampoline();
	 SoundPlayTrampoline trampoline = (SoundPlayTrampoline)trampoline_raw;

	 if (group == 1 && index == 40 && type == 0 && s_vergilTauntVoiceUntil != 0) {
		 const bool inWindow     = GetTickCount64() <= s_vergilTauntVoiceUntil;
		 s_vergilTauntVoiceUntil = 0;
		 if (inWindow && CrimsonSDL::PlayVergilTauntVoice(s_vergilTauntPlayer, s_vergilTauntDevil)) {
			 return 0; // a line of the boss instead of the game's own
		 }
	 }
	 return trampoline(soundObj, group, index, pos, type, a6, a7);
 }

 void ModdedSoundPlayDetour() {
	 s_SoundPlayHook =
		 std::make_unique<Utility::Detour_t>(
			 (uintptr_t)appBaseAddr + SOUND_PLAY_OFFSET(),
			 (uintptr_t)&SoundPlay_sub_140339320,
			 NULL, "sound_play_detour");
	 bool res = s_SoundPlayHook->Toggle();
	 assert(res);
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
	 ModdedSoundPlayDetour();
 }
}


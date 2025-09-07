#pragma once

#include <intrin.h>
#include "Core/DataTypes.hpp"
#include "Vars.hpp"

typedef void(__fastcall* func_897B0_t)(ModelData& modelData);
typedef void(__fastcall* func_89450_t)(ModelData& modelData);
typedef void(__fastcall* func_89270_t)(ModelData& modelData);
typedef void(__fastcall* func_8B470_t)(ModelData& modelData, bool32 enable);
typedef void(__fastcall* func_89960_t)(ModelData& modelData, byte8* modelFile, byte8* textureFile);
typedef void(__fastcall* func_1EF040_t)(byte8* actorData, uint32 index);
typedef byte8*(__fastcall* func_89DE0_t)(ModelData& modelData);
typedef void(__fastcall* func_8BC60_t)(ShadowData& shadowData, byte8* dest, byte8* file);
typedef void(__fastcall* func_305D80_t)(byte8* dest);
typedef void(__fastcall* func_8A000_t)(ModelData& modelData, byte8* motionFile, PhysicsMetadata** modelPhysicsMetadataPool);
typedef uint32(__fastcall* func_2C9F40_t)(byte8* physicsFile);
typedef void(__fastcall* func_2CA1D0_t)(PhysicsData& physicsData, PhysicsMetadata** physicsMetadata, byte8* physicsFile, uint32 index);
typedef void(__fastcall* func_2CA2F0_t)(PhysicsData& physicsData, void* dest, byte8* addr, ModelMetadata* modelMetadata, uint32 count);
typedef void*(__fastcall* func_2C6150_t)(void* dest, uint32 size, int32 mode); // Alloc
typedef bool(__fastcall* func_1FD3E0_t)(WeaponData& weaponData, uint8);        // IsWeaponReady
typedef void(__fastcall* func_1FDE10_t)(WeaponData& weaponData);               // Show Weapon
typedef void(__fastcall* func_8BF30_t)(uint32 index, void* dest, byte8* baseAddr);
typedef void(__fastcall* func_2EDFC0_t)(byte8*);
typedef void(__fastcall* func_337710_t)(byte8*);
typedef bool(__fastcall* func_27AA90_t)(void*, uint32, void*, float);
typedef byte8*(__fastcall* func_1A4680_t)(byte8*, const CreateEnemyActorData&); // Create Enemy Actor
typedef void(__fastcall* func_3391C0_t)(void*, int32, int32, int32, int32);     // Play Sound
typedef void(__fastcall* func_2EE3D0_t)(byte8* dest);
typedef void(__fastcall* func_1FAF40_t)(byte8* baseAddr);
typedef void(__fastcall* func_1EEF80_t)(byte8* baseAddr);
typedef bool(__fastcall* func_2EE060_t)(byte8* dest, uint32 size);
typedef byte8*(__fastcall* func_1DE820_t)(uint32 character, uint32 id, bool isClone);                      // Create Actor
typedef void(__fastcall* func_217B90_t)(byte8* actorData, ActiveMissionActorData& activeMissionActorData); // Init Actor Dante
typedef void(__fastcall* func_226F10_t)(byte8* actorData, ActiveMissionActorData& activeMissionActorData); // Init Actor Bob
typedef void(__fastcall* func_219660_t)(byte8* actorData, ActiveMissionActorData& activeMissionActorData); // Init Actor Lady
typedef void(__fastcall* func_223CB0_t)(byte8* actorData, ActiveMissionActorData& activeMissionActorData); // Init Actor Vergil
typedef void(__fastcall* func_212BE0_t)(byte8* actorData);                                                 // Update Actor Dante
typedef void(__fastcall* func_225D70_t)(byte8* actorData);                                                 // Update Actor Bob
typedef void(__fastcall* func_219260_t)(byte8* actorData);                                                 // Update Actor Lady
typedef void(__fastcall* func_220970_t)(byte8* actorData);                                                 // Update Actor Vergil
typedef void(__fastcall* func_1DFC20_t)(byte8* actorData);
typedef WeaponData*(__fastcall* func_2310B0_t)(byte8* actorData, uint32 id);                     // Dante Rebellion
typedef WeaponData*(__fastcall* func_22EC90_t)(byte8* actorData, uint32 id);                     // Dante Cerberus
typedef WeaponData*(__fastcall* func_227870_t)(byte8* actorData, uint32 id);                     // Dante Agni & Rudra
typedef WeaponData*(__fastcall* func_22A1E0_t)(byte8* actorData, uint32 id);                     // Dante Nevan
typedef WeaponData*(__fastcall* func_228CF0_t)(byte8* actorData, uint32 id);                     // Dante Vergil Beowulf
typedef WeaponData*(__fastcall* func_22B0C0_t)(byte8* actorData, uint32 id);                     // Dante Ebony & Ivory
typedef WeaponData*(__fastcall* func_2306B0_t)(byte8* actorData, uint32 id);                     // Dante Shotgun
typedef WeaponData*(__fastcall* func_22C4A0_t)(byte8* actorData, uint32 id);                     // Dante Artemis
typedef WeaponData*(__fastcall* func_2300A0_t)(byte8* actorData, uint32 id);                     // Dante Spiral
typedef WeaponData*(__fastcall* func_22BA30_t)(byte8* actorData, uint32 id);                     // Dante Lady Kalina Ann
typedef WeaponData*(__fastcall* func_231A30_t)(byte8* actorData, uint32 id);                     // Bob Yamato
typedef WeaponData*(__fastcall* func_22D960_t)(byte8* actorData, uint32 id);                     // Vergil Yamato
typedef WeaponData*(__fastcall* func_2298E0_t)(byte8* actorData, uint32 id);                     // Vergil Force Edge
typedef WeaponData*(__fastcall* func_22CF00_t)(byte8* actorData, uint32 id);                     // Vergil Nero Angelo Sword
typedef void(__fastcall* func_280120_t)(byte8* dest, uint32 side, uint32 index);                 // Weapon Switch Animation Dante
typedef void(__fastcall* func_280160_t)(byte8* dest, uint32 group, uint32 index, uint32 motion); // Weapon Switch Animation Vergil
typedef void(__fastcall* func_1EB0E0_t)(byte8* actorData, uint32 index);                         // Call after Weapon Switch Animation.
typedef void(__fastcall* func_1F92C0_t)(byte8* actorData, uint32 phase);                         // Queue devil model update.
typedef void(__fastcall* func_1F97F0_t)(byte8* actorData, bool noMotion);                        // Update devil model and play motion.
typedef void(__fastcall* func_1F94D0_t)(byte8* actorData, uint8 index);                          // Devil Flux
typedef void(__fastcall* func_1EAE60_t)(byte8* actorData, uint8 index);                          // Play Doppelganger Effect
typedef void(__fastcall* func_1FCA20_t)(byte8* actorBaseAddr, uint32 visibility);                // Update Color Matrices
typedef float32(__fastcall* func_32E5F0_t)(vec4& position);
typedef void(__fastcall* func_89E30_t)(ModelData& modelData, uint32 index); // Call after icon update.
typedef void(__fastcall* func_1B9FA0_t)(byte8* addr);                       // Adjust file pointers.
typedef void(__fastcall* func_223AC0_t)(byte8* actorData);                  // Create Spiral Swords.
typedef void(__fastcall* func_223F50_t)(byte8* actorData, uint32 index);    // Specific Summoned Swords.
typedef void(__fastcall* func_223BE0_t)(byte8* actorData, uint32 index);    // Shoot Summoned Sword. Use index 52.
typedef void(__fastcall* func_594B0_t)(BodyPartData& bodyPartData, byte8* file, uint32 bodyPart, uint32, byte8** motionArchives,
    byte8*** modelDataFunctions, PhysicsMetadata** modelPhysicsMetadataPool, float32* motionSpeed, void*);
typedef void(__fastcall* func_5A290_t)(byte8* body, uint32 group, uint32 index, uint16, uint8, uint8);          // Play Motion
typedef void(__fastcall* func_8AC80_t)(ModelData& modelData, uint32 bodyPart, byte8* motionFile, uint32, bool); // Play Motion
typedef void(__fastcall* func_1EFB90_t)(
    byte8* actorData, uint32 group, uint32 index, float32, int8, uint8, uint8);             // Play motion and update state.
typedef void(__fastcall* func_1DFDA0_t)(byte8*, uint32, uint8, float32, float32, uint8);    // Drop1
typedef void(__fastcall* func_1FB300_t)(byte8* actorData, uint32 direction, float32 value); // Adjust Position
typedef void(__fastcall* func_1E09D0_t)(byte8* actorBaseAddr, uint32 event);
typedef byte8*(__fastcall* func_175210_t)(vec4*, vec4*, __m128);                            // Boss Lady Grenade
typedef void(__fastcall* func_1E0800_t)(byte8* actorData, uint32 index, uint32, uint32);    // Trigger Attack
typedef void(__fastcall* func_211100_t)(byte8* actorData);                                  // Rebellion Combo 1 Part 3
typedef void(__fastcall* func_1F01F0_t)(byte8* actorData, uint32 index);                    // Reset State
typedef void(__fastcall* func_2F74E0_t)(byte8* dest, uint32 index);                         // Show Model Partition
typedef void(__fastcall* func_2F7350_t)(byte8* dest, uint32 index);                         // Hide Model Partition
typedef void(__fastcall* func_32BE20_t)(byte8* dest);                                       // Init Track
typedef void(__fastcall* func_32BA90_t)(byte8* dest, const char* location, uint32, uint32); // Set Track
typedef byte8*(__fastcall* func_8A520_t)(ModelData& modelData);
typedef void(__fastcall* func_30E630_t)(byte8* dest, uint32 index);

extern func_897B0_t func_897B0;   // (ModelData & modelData)
extern func_89450_t func_89450;   // (ModelData & modelData)
extern func_89270_t func_89270;   // (ModelData & modelData)
extern func_8B470_t func_8B470;   // (ModelData & modelData, bool32 enable)
extern func_89960_t func_89960;   // (ModelData & modelData, byte8 * modelFile, byte8 * textureFile)
extern func_1EF040_t func_1EF040; // (byte8 * actorData, uint32 index)
extern func_89DE0_t func_89DE0;   // (ModelData & modelData)
extern func_8BC60_t func_8BC60;   // (ShadowData & shadowData, byte8 * dest, byte8 * file)
extern func_305D80_t func_305D80; // (byte8 * dest)
extern func_8A000_t func_8A000;   // (ModelData & modelData, byte8 * motionFile, PhysicsMetadata ** modelPhysicsMetadataPool)
extern func_2C9F40_t func_2C9F40; // (byte8 * physicsFile)
extern func_2CA1D0_t func_2CA1D0; // (PhysicsData & physicsData, PhysicsMetadata ** physicsMetadata, byte8 * physicsFile, uint32 index)
extern func_2CA2F0_t func_2CA2F0; // (PhysicsData & physicsData, void * dest, byte8 * addr, ModelMetadata * modelMetadata, uint32 count)
extern func_2C6150_t func_2C6150; // (void * dest, uint32 size, int32 mode)
extern func_1FD3E0_t func_1FD3E0; // (WeaponData & weaponData, uint8)
extern func_1FDE10_t func_1FDE10; // (WeaponData & weaponData)
extern func_8BF30_t func_8BF30;   // (uint32 index, void * dest, byte8 * baseAddr)
extern func_2EDFC0_t func_2EDFC0; // (byte8 *)
extern func_337710_t func_337710; // (byte8 *)
extern func_27AA90_t func_27AA90; // (void *, uint32, void *, float)
extern func_1A4680_t func_1A4680; // (byte8 *, const CreateEnemyActorData &)
extern func_3391C0_t func_3391C0; // (void *, int32, int32, int32, int32)
extern func_2EE3D0_t func_2EE3D0; // (byte8 * dest)
extern func_1FAF40_t func_1FAF40; // (byte8 * baseAddr)
extern func_1EEF80_t func_1EEF80; // (byte8 * baseAddr)
extern func_2EE060_t func_2EE060; // (byte8 * dest, uint32 size)
extern func_1DE820_t CreatePlayerCharFunc_1DE820; // (uint32 character, uint32 id, bool isClone)
extern func_217B90_t func_217B90; // (byte8 * actorData, ActiveMissionActorData & activeMissionActorData)
extern func_226F10_t func_226F10; // (byte8 * actorData, ActiveMissionActorData & activeMissionActorData)
extern func_219660_t func_219660; // (byte8 * actorData, ActiveMissionActorData & activeMissionActorData)
extern func_223CB0_t func_223CB0; // (byte8 * actorData, ActiveMissionActorData & activeMissionActorData)
extern func_212BE0_t func_212BE0; // (byte8 * actorData)
extern func_225D70_t func_225D70; // (byte8 * actorData)
extern func_219260_t func_219260; // (byte8 * actorData)
extern func_220970_t func_220970; // (byte8 * actorData)
extern func_1DFC20_t func_1DFC20; // (byte8 * actorData)
extern func_2310B0_t func_2310B0; // (byte8 * actorData, uint32 id)
extern func_22EC90_t func_22EC90; // (byte8 * actorData, uint32 id)
extern func_227870_t func_227870; // (byte8 * actorData, uint32 id)
extern func_22A1E0_t func_22A1E0; // (byte8 * actorData, uint32 id)
extern func_228CF0_t func_228CF0; // (byte8 * actorData, uint32 id)
extern func_22B0C0_t func_22B0C0; // (byte8 * actorData, uint32 id)
extern func_2306B0_t func_2306B0; // (byte8 * actorData, uint32 id)
extern func_22C4A0_t func_22C4A0; // (byte8 * actorData, uint32 id)
extern func_2300A0_t func_2300A0; // (byte8 * actorData, uint32 id)
extern func_22BA30_t func_22BA30; // (byte8 * actorData, uint32 id)
extern func_231A30_t func_231A30; // (byte8 * actorData, uint32 id)
extern func_22D960_t func_22D960; // (byte8 * actorData, uint32 id)
extern func_2298E0_t func_2298E0; // (byte8 * actorData, uint32 id)
extern func_22CF00_t func_22CF00; // (byte8 * actorData, uint32 id)
extern func_280120_t ChangeGunHudAnim_280120; // (byte8 * dest, uint32 side, uint32 index)
extern func_280160_t func_280160; // (byte8 * dest, uint32 group, uint32 index, uint32 motion)
extern func_1EB0E0_t func_1EB0E0; // (byte8 * actorData, uint32 index)
extern func_1F92C0_t func_1F92C0; // (byte8 * actorData, uint32 phase)
extern func_1F97F0_t HeadflipAnimation_1F97F0; // (byte8 * actorData, bool noMotion)
extern func_1F94D0_t DevilFluxVFX_1F94D0; // (byte8 * actorData, uint8 index)
extern func_1EAE60_t ActivateDoppelgangerFX_1EAE60; // (byte8 * actorData, uint8 index)
extern func_1FCA20_t func_1FCA20; // (byte8 * actorBaseAddr, uint32 visibility)
extern func_32E5F0_t func_32E5F0; // (vec4 & position)
extern func_89E30_t func_89E30;   // (ModelData & modelData, uint32 index)
extern func_1B9FA0_t func_1B9FA0; // (byte8 * addr)
extern func_223AC0_t func_223AC0; // (byte8 * actorData)
extern func_223F50_t func_223F50; // (byte8 * actorData, uint32 index)
extern func_223BE0_t func_223BE0; // (byte8 * actorData, uint32 index)
extern func_594B0_t InitiateModelFunc_594B0;   // (BodyPartData & bodyPartData, byte8 * file, uint32 bodyPart, uint32, byte8 ** motionArchives, byte8 ***
                                  // modelDataFunctions, PhysicsMetadata ** modelPhysicsMetadataPool, float32 * motionSpeed, void *)
extern func_5A290_t func_5A290;   // (byte8 * body, uint32 group, uint32 index, uint16, uint8, uint8)
extern func_8AC80_t func_8AC80;   // (ModelData & modelData, uint32 bodyPart, byte8 * motionFile, uint32, bool)
extern func_1EFB90_t PlayAnimation_1EFB90; // (byte8 * actorData, uint32 group, uint32 index, float32, int8, uint8, uint8)
extern func_1DFDA0_t func_1DFDA0; // (byte8 *, uint32, uint8, float32, float32, uint8)
extern func_1FB300_t func_1FB300; // (byte8 * actorData, uint32 direction, float32 value)
extern func_1E09D0_t func_1E09D0; // (byte8 * actorBaseAddr, uint32 event)
extern func_175210_t func_175210; // (vec4 *, vec4 *, __m128)
extern func_1E0800_t func_1E0800; // (byte8 * actorData, uint32 index, uint32, uint32)
extern func_211100_t func_211100; // (byte8 * actorData)
extern func_1F01F0_t func_1F01F0; // (byte8 * actorData, uint32 index)
extern func_2F74E0_t func_2F74E0; // (byte8 * dest, uint32 index)
extern func_2F7350_t func_2F7350; // (byte8 * dest, uint32 index)
extern func_32BE20_t func_32BE20; // (byte8 * dest)
extern func_32BA90_t func_32BA90; // (byte8 * dest, const char * location, uint32, uint32)
extern func_8A520_t func_8A520;   // (ModelData & modelData)
extern func_30E630_t func_30E630; // (byte8 * dest, uint32 index)

void Internal_Init();
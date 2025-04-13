// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "Internal.hpp"
#include "Core/Core.hpp"

#include "Core/Macros.h"

// $DataStart

func_897B0_t func_897B0   = 0; // (ModelData & modelData)
func_89450_t func_89450   = 0; // (ModelData & modelData)
func_89270_t func_89270   = 0; // (ModelData & modelData)
func_8B470_t func_8B470   = 0; // (ModelData & modelData, bool32 enable)
func_89960_t func_89960   = 0; // (ModelData & modelData, byte8 * modelFile, byte8 * textureFile)
func_1EF040_t func_1EF040 = 0; // (byte8 * actorData, uint32 index)
func_89DE0_t func_89DE0   = 0; // (ModelData & modelData)
func_8BC60_t func_8BC60   = 0; // (ShadowData & shadowData, byte8 * dest, byte8 * file)
func_305D80_t func_305D80 = 0; // (byte8 * dest)
func_8A000_t func_8A000   = 0; // (ModelData & modelData, byte8 * motionFile, PhysicsMetadata ** modelPhysicsMetadataPool)
func_2C9F40_t func_2C9F40 = 0; // (byte8 * physicsFile)
func_2CA1D0_t func_2CA1D0 = 0; // (PhysicsData & physicsData, PhysicsMetadata ** physicsMetadata, byte8 * physicsFile, uint32 index)
func_2CA2F0_t func_2CA2F0 = 0; // (PhysicsData & physicsData, void * dest, byte8 * addr, ModelMetadata * modelMetadata, uint32 count)
func_2C6150_t func_2C6150 = 0; // (void * dest, uint32 size, int32 mode)
func_1FD3E0_t func_1FD3E0 = 0; // (WeaponData & weaponData, uint8)
func_1FDE10_t func_1FDE10 = 0; // (WeaponData & weaponData)
func_8BF30_t func_8BF30   = 0; // (uint32 index, void * dest, byte8 * baseAddr)
func_2EDFC0_t func_2EDFC0 = 0; // (byte8 *)
func_337710_t func_337710 = 0; // (byte8 *)
func_27AA90_t func_27AA90 = 0; // (void *, uint32, void *, float)
func_1A4680_t func_1A4680 = 0; // (byte8 *, const CreateEnemyActorData &)
func_3391C0_t func_3391C0 = 0; // (void *, int32, int32, int32, int32)
func_2EE3D0_t func_2EE3D0 = 0; // (byte8 * dest)
func_1FAF40_t func_1FAF40 = 0; // (byte8 * baseAddr)
func_1EEF80_t func_1EEF80 = 0; // (byte8 * baseAddr)
func_2EE060_t func_2EE060 = 0; // (byte8 * dest, uint32 size)
func_1DE820_t CreatePlayerCharFunc = 0; // (uint32 character, uint32 id, bool isClone)
func_217B90_t func_217B90 = 0; // (byte8 * actorData, ActiveMissionActorData & activeMissionActorData)
func_226F10_t func_226F10 = 0; // (byte8 * actorData, ActiveMissionActorData & activeMissionActorData)
func_219660_t func_219660 = 0; // (byte8 * actorData, ActiveMissionActorData & activeMissionActorData)
func_223CB0_t func_223CB0 = 0; // (byte8 * actorData, ActiveMissionActorData & activeMissionActorData)
func_212BE0_t func_212BE0 = 0; // (byte8 * actorData)
func_225D70_t func_225D70 = 0; // (byte8 * actorData)
func_219260_t func_219260 = 0; // (byte8 * actorData)
func_220970_t func_220970 = 0; // (byte8 * actorData)
func_1DFC20_t func_1DFC20 = 0; // (byte8 * actorData)
func_2310B0_t func_2310B0 = 0; // (byte8 * actorData, uint32 id)
func_22EC90_t func_22EC90 = 0; // (byte8 * actorData, uint32 id)
func_227870_t func_227870 = 0; // (byte8 * actorData, uint32 id)
func_22A1E0_t func_22A1E0 = 0; // (byte8 * actorData, uint32 id)
func_228CF0_t func_228CF0 = 0; // (byte8 * actorData, uint32 id)
func_22B0C0_t func_22B0C0 = 0; // (byte8 * actorData, uint32 id)
func_2306B0_t func_2306B0 = 0; // (byte8 * actorData, uint32 id)
func_22C4A0_t func_22C4A0 = 0; // (byte8 * actorData, uint32 id)
func_2300A0_t func_2300A0 = 0; // (byte8 * actorData, uint32 id)
func_22BA30_t func_22BA30 = 0; // (byte8 * actorData, uint32 id)
func_231A30_t func_231A30 = 0; // (byte8 * actorData, uint32 id)
func_22D960_t func_22D960 = 0; // (byte8 * actorData, uint32 id)
func_2298E0_t func_2298E0 = 0; // (byte8 * actorData, uint32 id)
func_22CF00_t func_22CF00 = 0; // (byte8 * actorData, uint32 id)
func_280120_t ChangeGunHudAnim = 0; // (byte8 * dest, uint32 side, uint32 index)
func_280160_t func_280160 = 0; // (byte8 * dest, uint32 group, uint32 index, uint32 motion)
func_1EB0E0_t func_1EB0E0 = 0; // (byte8 * actorData, uint32 index)
func_1F92C0_t func_1F92C0 = 0; // (byte8 * actorData, uint32 phase)
func_1F97F0_t HeadflipAnimation = 0; // (byte8 * actorData, bool noMotion)
func_1F94D0_t DevilFluxVFX = 0; // (byte8 * actorData, uint8 index)
func_1EAE60_t ActivateDoppelgangerFX = 0; // (byte8 * actorData, uint8 index)
func_1FCA20_t func_1FCA20 = 0; // (byte8 * actorBaseAddr, uint32 visibility)
func_32E5F0_t func_32E5F0 = 0; // (vec4 & position)
func_89E30_t func_89E30   = 0; // (ModelData & modelData, uint32 index)
func_1B9FA0_t func_1B9FA0 = 0; // (byte8 * addr)
func_223AC0_t func_223AC0 = 0; // (byte8 * actorData)
func_223F50_t func_223F50 = 0; // (byte8 * actorData, uint32 index)
func_223BE0_t func_223BE0 = 0; // (byte8 * actorData, uint32 index)
func_594B0_t InitiateModelFunc   = 0; // (BodyPartData & bodyPartData, byte8 * file, uint32 bodyPart, uint32, byte8 ** motionArchives, byte8 ***
                               // modelDataFunctions, PhysicsMetadata ** modelPhysicsMetadataPool, float32 * motionSpeed, void *)
func_5A290_t func_5A290   = 0; // (byte8 * body, uint32 group, uint32 index, uint16, uint8, uint8)
func_8AC80_t func_8AC80   = 0; // (ModelData & modelData, uint32 bodyPart, byte8 * motionFile, uint32, bool)
func_1EFB90_t PlayAnimation = 0; // (byte8 * actorData, uint32 group, uint32 index, float32, int8, uint8, uint8 nFrames)
func_1DFDA0_t func_1DFDA0 = 0; // (byte8 *, uint32, uint8, float32, float32, uint8)
func_1FB300_t func_1FB300 = 0; // (byte8 * actorData, uint32 direction, float32 value)
func_1E09D0_t func_1E09D0 = 0; // (byte8 * actorBaseAddr, uint32 event)
func_175210_t func_175210 = 0; // (vec4 *, vec4 *, __m128)
func_1E0800_t func_1E0800 = 0; // (byte8 * actorData, uint32 index, uint32, uint32)
func_211100_t func_211100 = 0; // (byte8 * actorData)
func_1F01F0_t func_1F01F0 = 0; // (byte8 * actorData, uint32 index)
func_2F74E0_t func_2F74E0 = 0; // (byte8 * dest, uint32 index)
func_2F7350_t func_2F7350 = 0; // (byte8 * dest, uint32 index)
func_32BE20_t func_32BE20 = 0; // (byte8 * dest)
func_32BA90_t func_32BA90 = 0; // (byte8 * dest, const char * location, uint32, uint32)
func_8A520_t func_8A520   = 0; // (ModelData & modelData)
func_30E630_t func_30E630 = 0; // (byte8 * dest, uint32 index)

void Internal_Init() {
    LogFunction();

    {
        auto func  = old_CreateFunction((appBaseAddr + 0x897B0));
        func_897B0 = (func_897B0_t)func.addr;
    }
    {
        auto func  = old_CreateFunction((appBaseAddr + 0x89450));
        func_89450 = (func_89450_t)func.addr;
    }
    {
        auto func  = old_CreateFunction((appBaseAddr + 0x89270));
        func_89270 = (func_89270_t)func.addr;
    }
    {
        auto func  = old_CreateFunction((appBaseAddr + 0x8B470));
        func_8B470 = (func_8B470_t)func.addr;
    }
    {
        auto func  = old_CreateFunction((appBaseAddr + 0x89960));
        func_89960 = (func_89960_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x1EF040));
        func_1EF040 = (func_1EF040_t)func.addr;
    }
    {
        auto func  = old_CreateFunction((appBaseAddr + 0x89DE0), 0, true, false);
        func_89DE0 = (func_89DE0_t)func.addr;
    }
    {
        auto func  = old_CreateFunction((appBaseAddr + 0x8BC60));
        func_8BC60 = (func_8BC60_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x305D80));
        func_305D80 = (func_305D80_t)func.addr;
    }
    {
        auto func  = old_CreateFunction((appBaseAddr + 0x8A000));
        func_8A000 = (func_8A000_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x2C9F40), 0, true, false);
        func_2C9F40 = (func_2C9F40_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x2CA1D0));
        func_2CA1D0 = (func_2CA1D0_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x2CA2F0), 0, true, true, 0, 0, 0, 0, 1);
        func_2CA2F0 = (func_2CA2F0_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x2C6150));
        func_2C6150 = (func_2C6150_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x1FD3E0));
        func_1FD3E0 = (func_1FD3E0_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x1FDE10));
        func_1FDE10 = (func_1FDE10_t)func.addr;
    }
    {
        auto func  = old_CreateFunction((appBaseAddr + 0x8BF30));
        func_8BF30 = (func_8BF30_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x2EDFC0));
        func_2EDFC0 = (func_2EDFC0_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x337710));
        func_337710 = (func_337710_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x27AA90));
        func_27AA90 = (func_27AA90_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x1A4680), 0, true, false);
        func_1A4680 = (func_1A4680_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x3391C0), 0, true, true, 0, 0, 0, 0, 1);
        func_3391C0 = (func_3391C0_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x2EE3D0));
        func_2EE3D0 = (func_2EE3D0_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x1FAF40));
        func_1FAF40 = (func_1FAF40_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x1EEF80));
        func_1EEF80 = (func_1EEF80_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x2EE060));
        func_2EE060 = (func_2EE060_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x1DE820), 0, true, false);
        CreatePlayerCharFunc = (func_1DE820_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x217B90));
        func_217B90 = (func_217B90_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x226F10));
        func_226F10 = (func_226F10_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x219660));
        func_219660 = (func_219660_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x223CB0));
        func_223CB0 = (func_223CB0_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x212BE0));
        func_212BE0 = (func_212BE0_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x225D70));
        func_225D70 = (func_225D70_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x219260));
        func_219260 = (func_219260_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x220970));
        func_220970 = (func_220970_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x1DFC20));
        func_1DFC20 = (func_1DFC20_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x2310B0), 0, true, false);
        func_2310B0 = (func_2310B0_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x22EC90), 0, true, false);
        func_22EC90 = (func_22EC90_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x227870), 0, true, false);
        func_227870 = (func_227870_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x22A1E0), 0, true, false);
        func_22A1E0 = (func_22A1E0_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x228CF0), 0, true, false);
        func_228CF0 = (func_228CF0_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x22B0C0), 0, true, false);
        func_22B0C0 = (func_22B0C0_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x2306B0), 0, true, false);
        func_2306B0 = (func_2306B0_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x22C4A0), 0, true, false);
        func_22C4A0 = (func_22C4A0_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x2300A0), 0, true, false);
        func_2300A0 = (func_2300A0_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x22BA30), 0, true, false);
        func_22BA30 = (func_22BA30_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x231A30), 0, true, false);
        func_231A30 = (func_231A30_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x22D960), 0, true, false);
        func_22D960 = (func_22D960_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x2298E0), 0, true, false);
        func_2298E0 = (func_2298E0_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x22CF00), 0, true, false);
        func_22CF00 = (func_22CF00_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x280120));
        ChangeGunHudAnim = (func_280120_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x280160));
        func_280160 = (func_280160_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x1EB0E0));
        func_1EB0E0 = (func_1EB0E0_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x1F92C0));
        func_1F92C0 = (func_1F92C0_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x1F97F0));
        HeadflipAnimation = (func_1F97F0_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x1F94D0));
        DevilFluxVFX = (func_1F94D0_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x1EAE60));
        ActivateDoppelgangerFX = (func_1EAE60_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x1FCA20));
        func_1FCA20 = (func_1FCA20_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x32E5F0), 0, true, false);
        func_32E5F0 = (func_32E5F0_t)func.addr;
    }
    {
        auto func  = old_CreateFunction((appBaseAddr + 0x89E30));
        func_89E30 = (func_89E30_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x1B9FA0));
        func_1B9FA0 = (func_1B9FA0_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x223AC0));
        func_223AC0 = (func_223AC0_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x223F50));
        func_223F50 = (func_223F50_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x223BE0));
        func_223BE0 = (func_223BE0_t)func.addr;
    }
    {
        auto func  = old_CreateFunction((appBaseAddr + 0x594B0), 0, true, true, 0, 0, 0, 0, 5);
        InitiateModelFunc = (func_594B0_t)func.addr;
    }
    {
        auto func  = old_CreateFunction((appBaseAddr + 0x5A290), 0, true, true, 0, 0, 0, 0, 2);
        func_5A290 = (func_5A290_t)func.addr;
    }
    {
        auto func  = old_CreateFunction((appBaseAddr + 0x8AC80), 0, true, true, 0, 0, 0, 0, 1);
        func_8AC80 = (func_8AC80_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x1EFB90), 0, true, true, 0, 0, 0, 0, 3);
        PlayAnimation = (func_1EFB90_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x1DFDA0), 0, true, true, 0, 0, 0, 0, 2);
        func_1DFDA0 = (func_1DFDA0_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x1FB300));
        func_1FB300 = (func_1FB300_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x1E09D0));
        func_1E09D0 = (func_1E09D0_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x175210), 0, true, false, 0, 0, 0, 0, 0, false, true);
        func_175210 = (func_175210_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x1E0800));
        func_1E0800 = (func_1E0800_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x211100));
        func_211100 = (func_211100_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x1F01F0));
        func_1F01F0 = (func_1F01F0_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x2F74E0));
        func_2F74E0 = (func_2F74E0_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x2F7350));
        func_2F7350 = (func_2F7350_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x32BE20));
        func_32BE20 = (func_32BE20_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x32BA90));
        func_32BA90 = (func_32BA90_t)func.addr;
    }
    {
        auto func  = old_CreateFunction((appBaseAddr + 0x8A520), 0, true, false);
        func_8A520 = (func_8A520_t)func.addr;
    }
    {
        auto func   = old_CreateFunction((appBaseAddr + 0x30E630));
        func_30E630 = (func_30E630_t)func.addr;
    }
}

// $DataEnd

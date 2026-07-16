#pragma once

#include "Core/Core.hpp"
#include "Vars.hpp"

bool IsArchive(byte8* addr);
void AdjustPointers(byte8* archive);

struct FileManager : Container<> {
    struct Metadata {
        byte8* addr;
        uint64 size;
        uint64 alignedSize;

        byte8* operator[](uint32 fileIndex);
        operator byte8*();
    };

    byte8* Push(const char* name, bool skipArchive = false);
    byte8* Push(byte8* addr, uint64 size);
    Metadata& operator[](uint64 index);
};

extern FileManager File_staticFiles;
extern FileManager File_dynamicFiles;
extern byte8* demo_pl000_00_3;
extern byte8* vergil_pl021_00_3;
extern byte8* newRisingStar_pl021_00_4; 
extern byte8* newYamatoHighTime_pl021_00_5;
extern byte8* newBaseDanteAnims_pl000_00_0;
extern byte8* newTauntDanteAnims1_pl000_00_2;
extern byte8* newTauntDanteAnims2_pl000_00_2;
extern byte8* newTauntVergilAnims1_pl021_00_2;
extern byte8* newTauntVergilAnims2_pl021_00_2;
extern byte8* newBaseVergilAnims_pl021_00_0;
extern byte8* newStyleSwitching01_pl000_00_4;
extern byte8* newStyleSwitching02_pl000_00_4;
extern byte8* newJudgementCut_pl021_00_3;
extern byte8* newJudgementCutAir_pl021_00_3;
extern byte8* newJudgementCutAirJF_pl021_00_3;
extern byte8* newDrivePart1_pl000_00_3;
extern byte8* newDrivePart2_pl000_00_3;
extern byte8* newDrivePart3_pl000_00_3;
extern byte8* newBackslide_pl000_00_19;
extern byte8* newCrimsonHUD1_0_id_100;
extern byte8* newCrimsonHUD1_0_id_100V;
extern byte8* newAirTauntRose_pl000_00_4;

void File_UpdateFileData(FileData& fileData, uint16 cacheFileIndex);

void File_UpdateFileData(uint16 fileDataIndex, uint16 cacheFileIndex);

extern FileData enemyFileData[ENEMY_FILE_DATA_COUNT];
extern FileDataMetadata enemyFileDataMetadata[ENEMY::COUNT];

bool File_Init();
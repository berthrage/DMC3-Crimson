#pragma once

#include "Core/Core.hpp"
#include "Vars.hpp"

bool IsArchive(byte8* addr);

struct FileManager : Container<>
{
	struct Metadata
	{
		byte8* addr;
		uint64 size;
		uint64 alignedSize;

		byte8* operator[](uint32 fileIndex);
		operator byte8* ();
	};

	byte8* Push
	(
		const char* name,
		bool skipArchive = false
	);
	byte8* Push
	(
		byte8* addr,
		uint64 size
	);
	Metadata& operator[](uint64 index);
};

extern FileManager File_staticFiles;
extern FileManager File_dynamicFiles;
extern byte8* demo_pl000_00_3;

void File_UpdateFileData
(
	FileData& fileData,
	uint16 cacheFileIndex
);

void File_UpdateFileData
(
	uint16 fileDataIndex,
	uint16 cacheFileIndex
);

extern FileData enemyFileData[ENEMY_FILE_DATA_COUNT];
extern FileDataMetadata enemyFileDataMetadata[ENEMY::COUNT];

bool File_Init();
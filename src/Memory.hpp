#pragma once

#include "Core/DataTypes.hpp"

enum MEMORY_SIZE {
    MEMORY_SIZE_ALL                    = (1 * 1024 * 1024 * 1024),
    MEMORY_SIZE_MAIN                   = (520 * 1024 * 1024), // root allocation is 516 MB (ROOT_ALLOC_SIZE)
    MEMORY_SIZE_STATIC_FILES           = (320 * 1024 * 1024),
	MEMORY_SIZE_STATIC_FILES_METADATA = (1 * 1024 * 1024),
	MEMORY_SIZE_DYNAMIC_FILES = (160 * 1024 * 1024),
	MEMORY_SIZE_DYNAMIC_FILES_METADATA = (1 * 1024 * 1024),
};

extern byte8* Memory_main;

bool Memory_Init();
void Memory_LogFileSlots(bool always = false);
void Memory_ToggleExtendVectors(bool enable);
byte8* GetMasterPointer();
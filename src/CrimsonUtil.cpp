// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "Core/Core.hpp"
#include "DMC3Input.hpp"
#include "Config.hpp"
#include "Global.hpp"
#include "Vars.hpp"
#include <cstdio>
#include "Core/Macros.h"

namespace CrimsonUtil {
int GetPlayerIndexFromAddr(uintptr_t playerPtr) {
    int playerIndexFound = 10; // if none is found then it'll return 10.

    old_for_all(uint8, playerIndex, PLAYER_COUNT) {
        if (playerPtr == crimsonPlayer[playerIndex].playerPtr || playerPtr == crimsonPlayer[playerIndex].clonePtr) {
            playerIndexFound = playerIndex;
        }
    }

    return playerIndexFound;
}

std::uint32_t Uint8toAABBGGRR(uint8 colorUint8[4]) {
    // Combine into AABBGGRR format
    std::uint32_t color = (colorUint8[3] << 24) | (colorUint8[2] << 16) | (colorUint8[1] << 8) | colorUint8[0];
    return color;
}

std::uint32_t HexToAABBGGRR(uint32_t hex) {
	// Map hex to RGBA
	uint8_t r = (hex >> 24) & 0xFF;
	uint8_t g = (hex >> 16) & 0xFF;
	uint8_t b = (hex >> 8) & 0xFF;
	uint8_t a = hex & 0xFF;

	// Convert RGBA to AABBGGRR
	return (a << 24) | (b << 16) | (g << 8) | r;
}

ImVec4 HexToImVec4(uint32_t hexColor) {
	float r = ((hexColor >> 24) & 0xFF) / 255.0f;
	float g = ((hexColor >> 16) & 0xFF) / 255.0f;
	float b = ((hexColor >> 8) & 0xFF) / 255.0f;
	float a = (hexColor & 0xFF) / 255.0f;
	return ImVec4(r, g, b, a);
}

float sexy_clamp(const float val, const float minVal, const float maxVal) {
	return max(minVal, min(val, maxVal));
}

float smoothstep(float edge0, float edge1, float x) {
	x = sexy_clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
	return x * x * (3 - 2 * x);
}

bool IsAddressValidAndWritable(uintptr_t address) {
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(reinterpret_cast<LPCVOID>(address), &mbi, sizeof(mbi))) {
        // Check if the address is committed memory and is writable
        if (mbi.State == MEM_COMMIT &&
            (mbi.Protect == PAGE_READWRITE ||
                mbi.Protect == PAGE_WRITECOPY ||
                mbi.Protect == PAGE_EXECUTE_READWRITE ||
                mbi.Protect == PAGE_EXECUTE_WRITECOPY)) {
            return true;
        }
    }
    return false;
}

}
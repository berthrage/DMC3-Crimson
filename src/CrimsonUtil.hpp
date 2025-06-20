#pragma once
#include <inttypes.h>
#include "Vars.hpp"
#include "../ThirdParty/ImGui/imgui.h"
namespace CrimsonUtil {

void CopyCrimsonPlayerData(CrimsonPlayerData* sourceP, CrimsonPlayerData* destP);

int GetPlayerIndexFromAddr(uintptr_t playerPtr);
float sexy_clamp(const float val, const float minVal, const float maxVal);
float smoothstep(float edge0, float edge1, float x);
std::uint32_t Uint8toAABBGGRR(uint8 colorUint8[4]);
ImVec4 HexToImVec4(uint32_t hexColor);
std::uint32_t HexToAABBGGRR(uint32_t hex);
bool IsAddressValidAndWritable(uintptr_t address);


template <typename T>
void ReverseNonNull(std::vector<T*>& vec) {
	// Extract non-null elements
	std::vector<T*> nonNullElements;
	for (T* elem : vec) {
		if (elem != nullptr) {
			nonNullElements.push_back(elem);
		}
	}

	// Reverse the non-null elements
	std::reverse(nonNullElements.begin(), nonNullElements.end());

	// Reinsert the reversed non-null elements back into the original vector
	auto it = nonNullElements.begin();
	for (T*& elem : vec) {
		if (elem != nullptr) {
			elem = *it++;
		}
	}
}

template <typename T>
T lerp(const T& a, const T& b, float t) {
	return a + t * (b - a);
}
}
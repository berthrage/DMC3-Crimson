#pragma once
#include <inttypes.h>

int GetPlayerIndexFromAddr(uintptr_t playerPtr);
bool IsAddressValidAndWritable(uintptr_t address);

template <typename T>
void reverseNonNull(std::vector<T*>& vec) {
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
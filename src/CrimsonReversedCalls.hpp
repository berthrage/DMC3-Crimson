#pragma once
#include <cstdint>
#include <type_traits>
#include "Core/Core.hpp"
#include "Core/DataTypes.hpp"
#include "Vars.hpp"

// Null-safe call to a __fastcall game function at an offset from appBaseAddr.
// Usage: GAME_CALL_SAFE(0x32CFE0, int16, uintptr_t, int, unsigned int)(a, b, c)
#define GAME_CALL_SAFE(offset, retType, ...) \
	[](auto&&... args) { \
		auto fn = reinterpret_cast<retType(__fastcall*)(__VA_ARGS__)>(appBaseAddr + (offset)); \
		if constexpr (std::is_void_v<retType>) { \
			if (fn) { fn(args...); } \
		} else { \
			return fn ? fn(args...) : retType{}; \
		} \
	}

namespace CrimsonReversedCalls {

	extern float CalcDistanceVec4_sub_14032E5F0(vec4* a, vec4* b);
	extern int16 InputsUpdate_sub_14032CFE0(uintptr_t inputAddr, int playerIndex, unsigned int a3);
	extern uintptr_t SpawnCollision_sub_14005C320(uintptr_t collisionDataStruct, uint8 a2);
	extern uintptr_t SetJDCPosition_sub_1401DC1A0(uintptr_t posPtr, uintptr_t matrixPtr, uintptr_t playerActorAddr, uint8 a4);
	extern uintptr_t SpawnJDCShl_sub_1401DC320(uintptr_t shlAddr);
	extern uintptr_t PlayVFX_sub_1402E7CA0(int group, uint16 index, uintptr_t matrixPtr, int a4);
	extern uintptr_t PlaySFXWithPos_ByType_sub_140339930(uintptr_t fileAddr, uint32 index, uintptr_t posPtr, uint32 type);
	extern uint32 TriggerCPlayerEvent_sub_1401E0800(uintptr_t playerAddr, int eventIndex, int a3, int a4);
	extern uint8 CPlayerSetSwordFormationTargetIndex_sub_140224180(uintptr_t playerActorAddr);
	extern void PlayAnimation_sub_1401EFB90(uintptr_t actorAddr, uint32 group, int index, float initialFrame, uint8 a5, uint8 a6, uint8 nFrames);
	extern void TriggerCPlayerGravity_sub_1401FB300(uintptr_t playerActorAddr, uint16 angle, float a3 = 0.0f);
	extern void ApplyNewYInertiaCPl_sub_1401FD110(uintptr_t playerActorAddr, int index, float newYinertia, float newYInertiaMult);
	extern void ApplyNewXInertiaCPl_sub_1401FD050(uintptr_t playerActorAddr, float newXInertia, float newXInertiaMult);
	extern bool IsCPlGrounded_sub_1401E8470(uintptr_t playerActorAddr, float yInertia);
	extern uintptr_t LandCPlayer_sub_1401E04A0(uintptr_t playerActorAddr);
	extern uintptr_t CPlFetchBoneMatrix_sub_1401FAA90(uintptr_t playerActorAddr, int boneIndex);
	extern void ExecuteYamatoRaveCPl_sub_14021BDE0(uintptr_t playerActorAddr);
	extern void ExecuteJDCMove_sub_14021C8A0(uintptr_t playerActorAddr);
	extern void ShotgunFire_sub_140217FF0(PlayerActorData& actorData, uint8 mode = 8, uint32 unk3 = 0);
	extern void ShotgunShlFire_sub_1401CA390(PlayerActorData& actorData, vec4& pos, vec4& pos2, uint8 mode = 0, uint16 shotType = 8);
}

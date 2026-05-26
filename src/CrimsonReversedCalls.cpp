#include "CrimsonReversedCalls.hpp"


namespace CrimsonReversedCalls {

// old declaration example
// 	static constexpr uintptr_t INPUTSUPDATE_OFFSET() { return 0x32CFE0; }
// 	using InputsUpdate_t = int16(__fastcall*)(uintptr_t inputAddr, int playerIndex, unsigned int a3);
// 	int16 InputsUpdate_sub_14032CFE0(uintptr_t inputAddr, int playerIndex, unsigned int a3) {
// 		InputsUpdate_t InputsUpdateFunc = reinterpret_cast<InputsUpdate_t>(appBaseAddr + INPUTSUPDATE_OFFSET());
// 		if (!InputsUpdateFunc) {
// 			return NULL;
// 		}
// 
// 		return InputsUpdateFunc(inputAddr, playerIndex, a3);
// 	}

	float CalcDistanceVec4_sub_14032E5F0(vec4* a, vec4* b) {
		return GAME_CALL_SAFE(0x32E5F0, float, vec4*, vec4*)(a, b);
	}

	int16 InputsUpdate_sub_14032CFE0(uintptr_t inputAddr, int playerIndex, unsigned int a3) {
		return GAME_CALL_SAFE(0x32CFE0, int16, uintptr_t, int, unsigned int)(inputAddr, playerIndex, a3);
	}

	uintptr_t SpawnCollision_sub_14005C320(uintptr_t collisionDataStruct, uint8 a2) {
		return GAME_CALL_SAFE(0x5C320, uintptr_t, uintptr_t, uint8)(collisionDataStruct, a2);
	}

	void ApplyDamageCalc_sub_140088190(
		uintptr_t CDamageCalcAddr,
		DamageData* dmgData,
		uintptr_t playerActorAddr60,
		uintptr_t floatArray) {
		GAME_CALL_SAFE(0x88190, void, uintptr_t, DamageData*, uintptr_t, uintptr_t)(CDamageCalcAddr, dmgData, playerActorAddr60, floatArray);
	}

	void ApplyCollisionDamageLight_sub_1400612C0(
		uintptr_t CComEmAddr,
		DamageData* dmgData,
		uintptr_t playerActorAddr60) {
		GAME_CALL_SAFE(0x612C0, void, uintptr_t, DamageData*, uintptr_t)(CComEmAddr, dmgData, playerActorAddr60);
	}
	// There's ApplyCollisionDamageNoStun as well, but with several different versions of it for each enemy type. 
	// All of these are called by ApplyDamageCalc through CComEm Vftable +0x40 (Heavy), +0x48 (Light), +0x50 (NoStun), 
	// so we can detour that function instead of all the individual ApplyCollisionDamageNoStun versions if we want to modify the 
	// NoStun behavior for all enemy types at once. -- Berthrage
	
	uintptr_t SetJDCPosition_sub_1401DC1A0(uintptr_t posPtr, uintptr_t matrixPtr, uintptr_t playerActorAddr, uint8 a4) {
		return GAME_CALL_SAFE(0x1DC1A0, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uint8)(posPtr, matrixPtr, playerActorAddr, a4);
	}

	uintptr_t SpawnJDCShl_sub_1401DC320(uintptr_t shlAddr) {
		return GAME_CALL_SAFE(0x1DC320, uintptr_t, uintptr_t)(shlAddr);
	}

	uintptr_t PlayVFX_sub_1402E7CA0(int group, uint16 index, uintptr_t matrixPtr, int a4) {
		return GAME_CALL_SAFE(0x2E7CA0, uintptr_t, int, uint16, uintptr_t, int)(group, index, matrixPtr, a4);
	}

	uintptr_t PlaySFXWithPos_ByType_sub_140339930(uintptr_t fileAddr, uint32 index, uintptr_t posPtr, uint32 type) {
		return GAME_CALL_SAFE(0x339930, uintptr_t, uintptr_t, uint32, uintptr_t, uint32)(fileAddr, index, posPtr, type);
	}

	uint32 TriggerCPlayerEvent_sub_1401E0800(uintptr_t playerAddr, int eventIndex, int a3, int a4) {
		return GAME_CALL_SAFE(0x1E0800, uint32, uintptr_t, int, int, int)(playerAddr, eventIndex, a3, a4);
	}

	uint8 CPlayerSetSwordFormationTargetIndex_sub_140224180(uintptr_t playerActorAddr) {
		return GAME_CALL_SAFE(0x224180, uint8, uintptr_t)(playerActorAddr);
	}

	void PlayAnimation_sub_1401EFB90(uintptr_t actorAddr, uint32 group, int index, float initialFrame, uint8 a5, uint8 a6, uint8 nFrames) {
		GAME_CALL_SAFE(0x1EFB90, void, uintptr_t, uint32, int, float, uint8, uint8, uint8)(actorAddr, group, index, initialFrame, a5, a6, nFrames);
	}

	void TriggerCPlayerGravity_sub_1401FB300(uintptr_t playerActorAddr, uint16 angle, float a3) {
		GAME_CALL_SAFE(0x1FB300, void, uintptr_t, uint16, float)(playerActorAddr, angle, a3);
	}

	void ApplyNewYInertiaCPl_sub_1401FD110(uintptr_t playerActorAddr, int index, float newYinertia, float newYinertiaMult) {
		GAME_CALL_SAFE(0x1FD110, void, uintptr_t, int, float, float)(playerActorAddr, index, newYinertia, newYinertiaMult);
	}

	void ApplyNewXInertiaCPl_sub_1401FD050(uintptr_t playerActorAddr, float newXInertia,float newXInertiaMult) {
		GAME_CALL_SAFE(0x1FD050, void, uintptr_t, float, float)(playerActorAddr, newXInertia, newXInertiaMult);
	}

	bool IsCPlGrounded_sub_1401E8470(uintptr_t playerActorAddr, float yInertia) {
		return GAME_CALL_SAFE(0x1E8470, bool, uintptr_t, float)(playerActorAddr, yInertia);
	}

	uintptr_t LandCPlayer_sub_1401E04A0(uintptr_t playerActorAddr) {
		return GAME_CALL_SAFE(0x1E04A0, uintptr_t, uintptr_t)(playerActorAddr);
	}

	uintptr_t CPlFetchBoneMatrix_sub_1401FAA90(uintptr_t playerActorAddr, int boneIndex) {
		return GAME_CALL_SAFE(0x1FAA90, uintptr_t, uintptr_t, int)(playerActorAddr, boneIndex);
	}

	void ExecuteYamatoRaveCPl_sub_14021BDE0(uintptr_t playerActorAddr) {
		GAME_CALL_SAFE(0x21BDE0, void, uintptr_t)(playerActorAddr);
	}	

	void ExecuteJDCMove_sub_14021C8A0(uintptr_t playerActorAddr) {
		GAME_CALL_SAFE(0x21C8A0, void, uintptr_t)(playerActorAddr);
	}

	void ShotgunShlFire_sub_1401CA390(PlayerActorData& actorData, vec4& pos, vec4& pos2, uint8 mode, uint16 shotType) {
		GAME_CALL_SAFE(0x1CA390, void, PlayerActorData&, vec4&, vec4&, uint8, uint16)(actorData, pos, pos2, mode, shotType);
	}

	void ShotgunFire_sub_140217FF0(PlayerActorData& actorData, uint8 mode, uint32 unk3) {
		GAME_CALL_SAFE(0x217FF0, void, PlayerActorData&, uint8, uint32)(actorData, mode, unk3);
	}

	

}

#pragma once
#include <string>
#include <vector>
#include "../ThirdParty/SDL3/SDL.h"

// Forward-declare XINPUT_STATE so we can reference it without pulling in Xinput.h
typedef struct _XINPUT_STATE XINPUT_STATE;

namespace CrimsonSDL {

extern std::string SDL3Initialization;
extern std::string MixerInitialization;
extern std::string MixerInitialization2;
extern bool SDL3Init;
extern bool cacheAudioFiles;
extern SDL_Gamepad* mainController;
extern std::vector<SDL_Gamepad*> controllers;
extern SDL_Gamepad* sdlGamepadByXiSlot[4];
extern std::vector<SDL_Gamepad*> sdlGamepadsExtra;

void InitSDL();
bool IsGamepadButtonDown(SDL_Gamepad* gamepad, int button);
bool IsControllerButtonDown(int controllerIndex, int button);
void CheckAndOpenControllers();
void UpdateJoysticks();
void VibrateController(int controllerIndex, Uint16 rumbleStrengthLowFreq, Uint16 rumbleStrengthHighFreq, int rumbleDuration);
void RemapSdlControllers();
SDL_Gamepad* GetControllerForPlayer(int playerIndex);
SDL_Gamepad* GetControllerByPhysicalSlot(int xiSlot);
const char* GetControllerNameForXInputSlot(int xiSlot);
bool IsNativeControllerConnected();
bool IsNativeControllerButtonDown(int button);
bool PopulateXInputStateFromSdlSlot(int xiSlot, XINPUT_STATE* pState);
bool PopulateXInputStateFromSDL(SDL_Gamepad* pad, XINPUT_STATE* pState);
void PlayChangeDevilArm();
void PlayChangeGun();
void PlayChangeDevilArmMP();
void PlayChangeGunMP();
void PlayChangeWeaponDMC3MP();
void PlayStyleChange(int playerIndex);
void PlayStyleChangeVO(int playerIndex, int style, bool doppActive);
void PlaySnap(byte8* actorBaseAddr);
void SetAllSFXDistance(int playerIndex, int angle, int distance);
void StyleRankAnnouncerController(int rank);
void PlaySprint(int playerIndex);
void PlayDevilTriggerIn(int playerIndex);
void PlayDevilTriggerOut(int playerIndex);
void PlayDevilTriggerLoop(int playerIndex);
void StopDevilTriggerLoop(int playerIndex);
void PlayDoppelgangerIn(int playerIndex);
void PlayDoppelgangerOut(int playerIndex);
void PlayQuicksilverIn();
void PlayQuicksilverOut();
void PlayJDC(int playerIndex, bool justFrame, float delay);
void PlayJDCCharge(int playerIndex);
void PlayDevilTriggerReady(int playerIndex);
void PlayDelayedCombo1(int playerIndex);
void PlayDelayedCombo2(int playerIndex);
bool ChannelIsPlaying(int channel);
bool DTEStartIsPlaying(int playerIndex);
void PlayDTExplosionStart(int playerIndex);
void PlayDTExplosionLoop(int playerIndex);
void PlayDTExplosionFinish(int playerIndex);
void PlayDTEExplosionRelease(int playerIndex, float multiplier);
void InterruptDTExplosionSFX(int playerIndex);
void PauseDTExplosionSFX(int playerIndex);
void ResumeDTExplosionSFX(int playerIndex);
void PlayGuard(int playerIndex);
void PlayRoyalBlock(int playerIndex);
void PlayNormalBlock(int playerIndex);
void PlayDriveLoop(int playerIndex, int entityIndex);
void PlayDriveStart(int playerIndex, int entityIndex);
void PlayDriveLevelUp(int playerIndex, int entityIndex);
void InterruptDriveSFX(int playerIndex, int entityIndex);
bool DriveStartIsPlaying(int playerIndex, int entityIndex);
void PlayNewMissionClearSong();
void PlayDivinityStatueSong();
void FadeOutMusic(int delayMs = 500);
void PlayBattleOfBrothersSong();
void FadeOutMusic(float fadeoutTime = 500);
void ReduceMusicVolumeInPause();
int IsMusicPlaying();

}

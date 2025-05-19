#pragma once
#include <string>
#include "SDL.h"

namespace CrimsonSDL {

extern std::string SDL2Initialization;
extern std::string MixerInitialization;
extern std::string MixerInitialization2;
extern bool SDL2Init;
extern bool cacheAudioFiles;
extern SDL_GameController* mainController;
extern std::vector<SDL_GameController*> controllers;

void InitSDL();
bool IsControllerButtonDown(int controllerIndex, int button);
void CheckAndOpenControllers();
void UpdateJoysticks();
void VibrateController(int controllerIndex, Uint16 rumbleStrengthLowFreq, Uint16 rumbleStrengthHighFreq, int rumbleDuration);
void PlayChangeDevilArm();
void PlayChangeGun();
void PlayChangeDevilArmMP();
void PlayChangeGunMP();
void PlayChangeWeaponDMC3MP();
void PlayStyleChange(int playerIndex);
void PlayStyleChangeVO(int playerIndex, int style, bool doppActive);
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
void PlayNewMissionClearSong();
void FadeOutNewMissionClearSong();
int IsMusicPlaying();

}
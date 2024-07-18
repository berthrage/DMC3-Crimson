#pragma once
#include <string>
#include "SDL.h"


extern std::string SDL2Initialization;
extern std::string MixerInitialization;
extern std::string MixerInitialization2;
extern bool SDL2Init;
extern bool cacheAudioFiles;
extern SDL_GameController* controller;
extern SDL_Joystick* joystick;
extern SDL_Haptic* styleHaptic;

void initSDL();
void playChangeDevilArm();
void playChangeGun();
void playStyleChange();
void playStyleChangeVO(int style);
void StyleRankAnnouncerController(int rank);
void playSprint();
void PlayDevilTriggerIn();
void PlayDevilTriggerOut();
void playDevilTriggerLoop();
void stopDevilTriggerLoop();
void playDoppelgangerIn();
void playDoppelgangerOut();
void playQuicksilverIn();
void PlayDevilTriggerReady();
void playDelayedCombo1();
void playDelayedCombo2();
void PlayNewMissionClearSong();
bool IsJoystickButtonDown(SDL_Joystick* joystick, int button);
void FadeOutNewMissionClearSong();
int isMusicPlaying();
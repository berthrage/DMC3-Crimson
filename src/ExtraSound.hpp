#pragma once
#include <string>

extern std::string SDL2Initialization;
extern std::string MixerInitialization;
extern std::string MixerInitialization2;
extern bool SDL2Init;
extern bool cacheAudioFiles;

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
void FadeOutNewMissionClearSong();
int isMusicPlaying();
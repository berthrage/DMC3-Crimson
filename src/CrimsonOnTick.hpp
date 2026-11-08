#pragma once

namespace CrimsonOnTick {
	extern bool inputtingFPS;
	void LevelFullyLoadedDelay();
	void FrameResponsiveGameSpeed();
	void GameTrackDetection();
	void InCreditsDetection();
	void PreparePlayersDataBeforeSpawn();
	void FixWeaponUnlocksDante();
	void PairVanillaWeaponSlots();
	void CrimsonMissionClearSong();
	void DivinityStatueSong();
	void DisableBlendingEffectsController();
	void UpdateMainPlayerMotionArchives();
	void TrackMultiplayerStylishPts();
	void ApplyMultiplayerStylishPtsAverage();
	float GetPlayerStylishPts(uint8 playerIndex);
	void DetermineActiveEntitiesCount();
	void MultiplayerCameraPositioningController();
	void SplitscreenCameraPositioningController();
	void CameraShakeController();
	bool SharedCameraNeedsThirdPerson();
	void ForceThirdPersonCameraController();
	void GeneralCameraOptionsController();
	void AirTauntDetoursController();
	void PauseSFXWhenPaused();
	void TrackMissionStyleLevels();
	void OverrideEnemyTargetPosition();
	void WeaponProgressionTracking();
	void FixM7DevilTriggerUnlocking();
	void ToggleOnTickFuncs(bool enable);
}
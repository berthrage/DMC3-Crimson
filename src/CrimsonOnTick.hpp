#pragma once

namespace CrimsonOnTick {
	extern bool inputtingFPS;
	void FrameResponsiveGameSpeed();
	void GameTrackDetection();
	void CorrectFrameRateCutscenes();
	void PreparePlayersDataBeforeSpawn();
	void CrimsonMissionClearSong();
	void DisableBlendingEffectsController();
	void StyleMeterMultiplayer();
	void DetermineActiveEntitiesCount();
	void MultiplayerCameraPositioningController();
	void ForceThirdPersonCameraController();
	void GeneralCameraOptionsController();
	void AirTauntDetoursController();
	void PauseSFXWhenPaused();
	void TrackMissionStyleLevels();
	void OverrideEnemyTargetPosition();
	void WeaponProgressionTracking();
	void FixM7DevilTriggerUnlocking();
}
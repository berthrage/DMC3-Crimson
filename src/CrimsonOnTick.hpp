#pragma once

namespace CrimsonOnTick {
	extern bool inputtingFPS;
	void FrameResponsiveGameSpeed();
	void GameTrackDetection();
	void InCreditsDetection();
	void PreparePlayersDataBeforeSpawn();
	void CrimsonMissionClearSong();
	void DivinityStatueSong();
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
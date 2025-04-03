#pragma once

namespace CrimsonOnTick {
	extern bool inputtingFPS;
	void FrameResponsiveGameSpeed();
	void GameTrackDetection();
	void CorrectFrameRateCutscenes();
	void PreparePlayersDataBeforeSpawn();
	void NewMissionClearSong();
	void DisableBlendingEffectsController();
	void StyleMeterMultiplayer();
	void DetermineActiveEntitiesCount();
	void MultiplayerCameraPositioningController();
	void ForceThirdPersonCameraController();
	void GeneralCameraOptionsController();
	void AirTauntDetoursController();
	void PauseSFXWhenPaused();
	void OverrideEnemyTargetPosition();
}
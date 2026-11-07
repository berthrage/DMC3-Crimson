#include "CrimsonVisualStyle.hpp"
#include "CrimsonDetours.hpp"
#include "CrimsonPatches.hpp"
#include "CrimsonFileHandling.hpp"
#include "CrimsonHUD.hpp"
#include "HUD.hpp"
#include "Core/Macros.h"
#include "Global.hpp"

namespace CrimsonVisualStyle {


// Preset definitions 

static const VisualStyleTracking CLASSIC_TRACKING = []() {
	VisualStyleTracking t;
	t.positionings       = false;
	t.redOrbCounter      = true;
	t.stylesDisplay      = STYLESDISPLAY::OFF;
	t.styleNamesSet		 = STYLENAMESSET::DMC3_SWITCH;
	t.displayStyleNames  = false;
	t.styleRanksMeter    = false;
	t.stylishPtsCounter  = false;
	t.lockOn             = false;
	t.selectedHUD        = "Classic HUD";
	return t;
}();

static const VisualStyleTracking HYBRID_TRACKING = []() {
	VisualStyleTracking t;
	t.positionings       = false;
	t.redOrbCounter      = true;
	t.stylesDisplay      = STYLESDISPLAY::OFF;
	t.styleNamesSet	     = STYLENAMESSET::DMC3_SWITCH;
	t.displayStyleNames  = true;
	t.styleRanksMeter    = true;
	t.stylishPtsCounter  = true;
	t.lockOn             = true;
	t.selectedHUD        = "Classic HUD";
	return t;
}();

static const VisualStyleTracking MODERN_TRACKING = []() {
	VisualStyleTracking t;
	// All defaults already match Modern
	return t;
}();


// PopulateTracking – copy from real CrimsonConfig into tracking struct

void PopulateTracking(VisualStyleTracking& t, const CrimsonConfig& c) {
	t.positionings      = c.CrimsonHudAddons.positionings;
	t.redOrbCounter     = c.CrimsonHudAddons.redOrbCounter;
	t.stylesDisplay     = c.CrimsonHudAddons.stylesDisplay;
	t.styleNamesSet     = c.CrimsonHudAddons.styleNamesSet;
	t.displayStyleNames = c.CrimsonHudAddons.displayStyleNames;
	t.styleRanksMeter   = c.CrimsonHudAddons.styleRanksMeter;
	t.stylishPtsCounter = c.CrimsonHudAddons.stylishPtsCounter;
	t.lockOn            = c.CrimsonHudAddons.lockOn;
	t.selectedHUD       = c.HudOptions.selectedHUD;
}


// ConfigsMatchVisualStyle – exact match via CompareStructs

bool ConfigsMatchVisualStyle(const VisualStyleTracking& a, const VisualStyleTracking& b) {
	return CrimsonGameModes::CompareMembers(a, b);
}


// TrackVisualStyle – detect which preset matches the current active config

void TrackVisualStyle() {
	// Populate tracking from active config
	VisualStyleTracking current;
	PopulateTracking(current, activeCrimsonConfig);

	// Compare against all three presets
	bool matchesClassic = ConfigsMatchVisualStyle(current, CLASSIC_TRACKING);
	bool matchesHybrid  = ConfigsMatchVisualStyle(current, HYBRID_TRACKING);
	bool matchesModern  = ConfigsMatchVisualStyle(current, MODERN_TRACKING);

	uint8 matchedPreset = VISUALSTYLEPRESETS::CUSTOM;
	if (matchesClassic) {
		matchedPreset = VISUALSTYLEPRESETS::CLASSIC;
	} else if (matchesHybrid) {
		matchedPreset = VISUALSTYLEPRESETS::HYBRID;
	} else if (matchesModern) {
		matchedPreset = VISUALSTYLEPRESETS::MODERN;
	}

	// Update if changed
	if (matchedPreset != activeCrimsonConfig.VisualStyle.preset) {
		activeCrimsonConfig.VisualStyle.preset = matchedPreset;
		queuedCrimsonConfig.VisualStyle.preset = matchedPreset;
	}
}

// SetVisualStylePreset – apply a preset to active + queued configs

void SetVisualStylePreset(uint8 mode) {
	const VisualStyleTracking* chosen = nullptr;

	switch (mode) {
	case VISUALSTYLEPRESETS::CLASSIC:
		chosen = &CLASSIC_TRACKING;
		break;
	case VISUALSTYLEPRESETS::HYBRID:
		chosen = &HYBRID_TRACKING;
		break;
	case VISUALSTYLEPRESETS::MODERN:
	default:
		chosen = &MODERN_TRACKING;
		break;
	}

	if (!chosen) return;

	// Copy tracked fields into active + queued — CrimsonHudAddons
	auto applyTo = [&](CrimsonConfig& cfg) {
		cfg.CrimsonHudAddons.positionings       = chosen->positionings;
		cfg.CrimsonHudAddons.redOrbCounter      = chosen->redOrbCounter;
		cfg.CrimsonHudAddons.stylesDisplay      = chosen->stylesDisplay;
		cfg.CrimsonHudAddons.displayStyleNames  = chosen->displayStyleNames;
		cfg.CrimsonHudAddons.styleNamesSet      = chosen->styleNamesSet;
		cfg.CrimsonHudAddons.styleRanksMeter    = chosen->styleRanksMeter;
		cfg.CrimsonHudAddons.stylishPtsCounter  = chosen->stylishPtsCounter;
		cfg.CrimsonHudAddons.lockOn             = chosen->lockOn;
		// HudOptions
		cfg.HudOptions.selectedHUD              = chosen->selectedHUD;
		// VisualStyle meta
		cfg.VisualStyle.preset                  = mode;
	};

	applyTo(activeCrimsonConfig);
	applyTo(queuedCrimsonConfig);

	UpdateHUDPostPreset();
}

// UpdateHUDPostPreset – apply runtime side effects after preset change

void UpdateHUDPostPreset() {
	auto& cfg = activeCrimsonConfig;

	CrimsonDetours::ToggleClassicHUDPositionings(!cfg.CrimsonHudAddons.positionings);
	CrimsonDetours::ToggleHideStyleRankHUD(
		cfg.HudOptions.hideStyleMeter || !cfg.CrimsonHudAddons.styleRanksMeter);
	CrimsonPatches::ToggleHideLockOn(cfg.HudOptions.hideLockOn);
	ToggleHideMainHUD(cfg.HudOptions.hideMainHUD);
	ToggleHideBossHUD(cfg.HudOptions.hideBossHUD);
	ToggleForceVisibleHUD(cfg.HudOptions.forceVisibleHUD);
	CrimsonDetours::ToggleStyleRankHudNoFadeout(cfg.HudOptions.disableStyleRankHudFadeout);
	if (D3D11::device) {
		CrimsonHUD::InitStyleNameTextures(D3D11::device);
	}

	// Refresh HUD files if selectedHUD changed
	CrimsonFiles::CopyHUDFilePactoGameFolder();
	CrimsonFiles::ApplySelectedHUD(activeCrimsonConfig.HudOptions.selectedHUD);
	HUD_ReloadIcons();
}

} 

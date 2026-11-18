#pragma once

#include <stdint.h>
#include <string>
#include <tuple>
#include <utility>
#include "CrimsonConfig.hpp"
#include "CrimsonGameModes.hpp"  // for CompareStructs, CompareValue

namespace CrimsonVisualStyle {

// VisualStyleTracking holds only the fields we compare for preset matching.
// It's not persisted: it's a comparison helper populated from CrimsonConfig.
struct VisualStyleTracking {
	bool positionings = true;
	bool redOrbCounter = true;
	uint8 stylesDisplay = STYLESDISPLAY::WITH_BROKEN_GLASS;
	uint8 styleNamesSet = STYLENAMESSET::CRIMSON;
	bool displayStyleNames = true;
	bool styleRanksMeter = true;
	bool stylishPtsCounter = true;
	bool lockOn = true;
	std::string selectedHUD = "Crimson HUD 2.0";

	static constexpr auto Metadata() {
		return std::make_tuple(
			std::make_pair("positionings", &VisualStyleTracking::positionings),
			std::make_pair("redOrbCounter", &VisualStyleTracking::redOrbCounter),
			std::make_pair("stylesDisplay", &VisualStyleTracking::stylesDisplay),
			std::make_pair("styleNamesSet", &VisualStyleTracking::styleNamesSet),
			std::make_pair("displayStyleNames", &VisualStyleTracking::displayStyleNames),
			std::make_pair("styleRanksMeter", &VisualStyleTracking::styleRanksMeter),
			std::make_pair("stylishPtsCounter", &VisualStyleTracking::stylishPtsCounter),
			std::make_pair("lockOn", &VisualStyleTracking::lockOn),
			std::make_pair("selectedHUD", &VisualStyleTracking::selectedHUD)
		);
	}
};

// Fills a VisualStyleTracking from the real CrimsonConfig
void PopulateTracking(VisualStyleTracking& t, const CrimsonConfig& c);

// Exact comparison of two tracking structs via CompareStructs
bool ConfigsMatchVisualStyle(const VisualStyleTracking& a, const VisualStyleTracking& b);

// Detects which preset (if any) the active config matches and updates VisualStyle.preset
void TrackVisualStyle();

// Applies a preset by copying tracked fields into active/queued CrimsonConfig
void SetVisualStylePreset(uint8 mode);

// Called after applying a preset to update runtime toggle state
void UpdateHUDPostPreset();

} 

#pragma once

#include "Core/Core.hpp"
#include "Core/RapidJSON.h"
#include "ImGuiExtra.hpp"
#include "PatreonData.hpp"

namespace UI {

/// Populates the patron lists from the JSON data embedded in the DLL at compile time.
inline void LoadDefaultPatrons()
{
	g_UIContext.PatronsDT.clear();
	g_UIContext.PatronsSDT.clear();
	g_UIContext.PatronsLDK.clear();

	rapidjson::Document d;
	d.Parse(EMBEDDED_PATRONS_JSON);

	if (d.HasParseError() || !d.IsObject()) {
		Log("LoadDefaultPatrons: Failed to parse embedded patrons JSON.");
		return;
	}

	// Parse tier names
	if (d.HasMember("tierNames") && d["tierNames"].IsObject()) {
		auto& tiers = d["tierNames"];
		if (tiers.HasMember("DT") && tiers["DT"].IsString())
			g_UIContext.TierNames[(size_t)PatreonTiers_t::DT] = tiers["DT"].GetString();
		if (tiers.HasMember("SDT") && tiers["SDT"].IsString())
			g_UIContext.TierNames[(size_t)PatreonTiers_t::SDT] = tiers["SDT"].GetString();
		if (tiers.HasMember("LDK") && tiers["LDK"].IsString())
			g_UIContext.TierNames[(size_t)PatreonTiers_t::LDK] = tiers["LDK"].GetString();
	}

	// Parse patron lists
	if (d.HasMember("patrons") && d["patrons"].IsObject()) {
		auto& patrons = d["patrons"];

		if (patrons.HasMember("DT") && patrons["DT"].IsArray()) {
			g_UIContext.PatronsDT.reserve(patrons["DT"].Size());
			for (auto& name : patrons["DT"].GetArray())
				if (name.IsString())
					g_UIContext.PatronsDT.push_back(name.GetString());
		}
		if (patrons.HasMember("SDT") && patrons["SDT"].IsArray()) {
			g_UIContext.PatronsSDT.reserve(patrons["SDT"].Size());
			for (auto& name : patrons["SDT"].GetArray())
				if (name.IsString())
					g_UIContext.PatronsSDT.push_back(name.GetString());
		}
		if (patrons.HasMember("LDK") && patrons["LDK"].IsArray()) {
			g_UIContext.PatronsLDK.reserve(patrons["LDK"].Size());
			for (auto& name : patrons["LDK"].GetArray())
				if (name.IsString())
					g_UIContext.PatronsLDK.push_back(name.GetString());
		}
	}

// 	Log("LoadDefaultPatrons: Loaded %zu DT, %zu SDT, %zu LDK patrons.",
// 		g_UIContext.PatronsDT.size(),
// 		g_UIContext.PatronsSDT.size(),
// 		g_UIContext.PatronsLDK.size());
}

} // namespace UI

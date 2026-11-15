#include <vector>
#include "Core/DataTypes.hpp"

#ifndef PATHS_H  
#define PATHS_H

namespace Paths {
	extern const char* assets;
	extern const char* config;
	extern const char* sounds;
	extern const char* musicSwitcher;
	extern const char* huds;
	extern const char* styleRanks;
	extern const char* visualStylesPreviews;
	extern const char* cameraPreviews;
	extern const char* weaponwheel;
	extern const char* gameMods;
}

namespace CrimsonFiles {
	// Crimson HUD 2.0 has no pac folder: it is drawn through ImGui and keeps Crimson HUD 1.0's pacs for everything else.
	constexpr const char* CRIMSON_HUD2_NAME = "Crimson HUD 2.0";

	extern std::vector<std::string> HUDdirectories;
	extern std::vector<std::string> StyleRanksdirectories;
	extern std::vector<std::string> StyleRanksAccoladesdirectories;

	std::vector<std::string> GetDirectories(std::string path);
	std::vector<std::string> GetFiles(const std::string& path);
	void GetHUDsDirectories();
	void GetStyleRanksDirectories();
	void GetStyleRanksAccoladesDirectories();
	void CopyHUDFilePactoGameFolder();
	void FreeDynamicHUD();
	bool ApplySelectedHUD(const std::string& hudName);
	bool LoadSinglePAC(const std::string& filePath, byte8*& outPtr);
	bool IsCrimsonHUD2(const std::string& hudName);
	std::string PacHUDName(const std::string& hudName);
}

#endif
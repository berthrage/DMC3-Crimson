#include <iostream>
#include <vector>
#include <algorithm>
#include <filesystem>
#include "Vars.hpp"
#include "Config.hpp"
#include "CrimsonFileHandling.hpp"
#include "File.hpp"
#include "HUD.hpp"

namespace fs = std::filesystem;

namespace Paths {
	const char* assets = "Crimson\\assets";
	const char* config = "Crimson\\configs";
	const char* sounds = "Crimson\\sound";
	const char* musicSwitcher = "Crimson\\musicswitcher";
	const char* huds = "Crimson\\huds";
	const char* styleRanks = "Crimson\\assets\\styleranks";
	const char* visualStylesPreviews = "Crimson\\assets\\visualstylepreviews";
	const char* cameraPreviews = "Crimson\\assets\\camerapreviews";
	const char* weaponwheel = "Crimson\\weaponwheel";
	const char* gameMods = "data\\dmc3\\GData.afs";
}

namespace CrimsonFiles {
std::vector<std::string> HUDdirectories;
std::vector<std::string> StyleRanksdirectories;
std::vector<std::string> StyleRanksAccoladesdirectories;

std::vector<std::string> GetDirectories(std::string path) {
	std::vector<std::string> directories;

	if (fs::exists(path) && fs::is_directory(path)) {
		for (const auto& entry : fs::directory_iterator(path)) {
			if (entry.is_directory()) {
				directories.push_back(entry.path().filename().string());
			}
		}
	}
	return directories;
}

std::vector<std::string> GetFiles(const std::string& path) {
	std::vector<std::string> files;
	try {
		if (fs::exists(path) && fs::is_directory(path)) {
			for (const auto& entry : fs::directory_iterator(path)) {
				if (entry.is_regular_file()) {
					files.push_back(entry.path().filename().string());
				}
			}
		}
	} catch (const fs::filesystem_error& e) {
		const char* errorMsg = e.what();
		Log(errorMsg);
	}
	return files;
}

void GetHUDsDirectories() {
	HUDdirectories = GetDirectories(Paths::huds);
}

void GetStyleRanksDirectories() {
	StyleRanksdirectories = GetDirectories(Paths::styleRanks);
}

void GetStyleRanksAccoladesDirectories() {
	std::string styleRanksAccoladesPath = Paths::styleRanks + (std::string)"\\" + activeCrimsonConfig.CrimsonHudAddons.selectedStyleRanks + "\\";
	StyleRanksAccoladesdirectories = GetDirectories(styleRanksAccoladesPath.c_str());
}

bool LoadSinglePAC(const std::string& filePath, byte8*& outPtr) {
	outPtr = nullptr;

	HANDLE file = OpenFile(filePath.c_str(), FileFlags_Read);
	if (file == INVALID_HANDLE_VALUE) {
		Log("LoadSinglePAC: OpenFile failed. %s", filePath.c_str());
		return false;
	}

	uint64 size = GetFileSize(file);
	if (size == 0) {
		CloseFile(file);
		return false;
	}

	byte8* dest = HighAlloc(size);
	if (!dest) {
		Log("LoadSinglePAC: HighAlloc failed.");
		CloseFile(file);
		return false;
	}

	if (!LoadFile(file, size, dest, 0)) {
		Log("LoadSinglePAC: LoadFile failed.");
		Free(dest);
		CloseFile(file);
		return false;
	}

	CloseFile(file);

	AdjustPointers(dest);

	outPtr = dest;
	return true;
}

void FreeDynamicHUD() {
	if (g_dynamicHUD_id100) {
		Free(g_dynamicHUD_id100);
		g_dynamicHUD_id100 = nullptr;
	}
	if (g_dynamicHUD_id100V) {
		Free(g_dynamicHUD_id100V);
		g_dynamicHUD_id100V = nullptr;
	}
}

bool ApplySelectedHUD(const std::string& hudName) {
	// Free any previously loaded dynamic HUD
	FreeDynamicHUD();

	// Crimson HUD and Classic HUD don't use dynamic loading
	if (hudName == "Crimson HUD" || hudName == "Classic HUD") {
		return true;
	}

	std::string basePath = std::string(Paths::huds) + "\\" + hudName + "\\";

	// Load id100.pac (required)
	std::string id100Path = basePath + "id100.pac";
	LoadSinglePAC(id100Path, g_dynamicHUD_id100);

	// Load id100V.pac (optional — may not exist for some HUDs)
	std::string id100VPath = basePath + "id100V.pac";
	LoadSinglePAC(id100VPath, g_dynamicHUD_id100V);

	return (g_dynamicHUD_id100 != nullptr);
}

void CopyHUDFilePactoGameFolder() {
	std::string hudName = activeCrimsonConfig.HudOptions.selectedHUD;

	struct CopyTask {
		std::string srcDir;
		std::string srcName;
		std::string dstName;
	};

	std::vector<CopyTask> tasks;

	// Always copy both Crimson HUD and Classic HUD files into the game folder
	tasks.push_back({std::string(Paths::huds) + "\\Crimson HUD", "id100.pac",  "newCrimsonHUD1_0_id_100.pac"});
	tasks.push_back({std::string(Paths::huds) + "\\Crimson HUD", "id100V.pac", "newCrimsonHUD1_0_id_100V.pac"});
	tasks.push_back({std::string(Paths::huds) + "\\Classic HUD", "id100.pac",  "id100.pac"});
	tasks.push_back({std::string(Paths::huds) + "\\Classic HUD", "id100V.pac", "id100V.pac"});

	// If a custom HUD is selected, also copy its files alongside the official ones
	if (hudName != "Crimson HUD" && hudName != "Classic HUD") {
		std::string safeName = hudName;
		for (auto& c : safeName) {
			if (c == ' ' || c == '\\' || c == '/' || c == ':' ||
				c == '*' || c == '?' || c == '"' || c == '<' ||
				c == '>' || c == '|') {
				c = '_';
			}
		}
		std::string prefix = "newCustomHUD_" + safeName;
		tasks.push_back({std::string(Paths::huds) + "\\" + hudName, "id100.pac",  prefix + "_id_100.pac"});
		tasks.push_back({std::string(Paths::huds) + "\\" + hudName, "id100V.pac", prefix + "_id_100V.pac"});
	}

	for (const auto& task : tasks) {
		std::vector<std::string> srcFiles = GetFiles(task.srcDir);

		// Case-insensitive filename matching
		auto it = std::find_if(srcFiles.begin(), srcFiles.end(),
			[&](const std::string& file) {
				if (file.size() != task.srcName.size()) return false;
				return std::equal(file.begin(), file.end(), task.srcName.begin(),
					[](char a, char b) {
						return std::tolower(static_cast<unsigned char>(a)) ==
						       std::tolower(static_cast<unsigned char>(b));
					});
			});

		if (it != srcFiles.end()) {
			// Use the actual filename from disk for the source path
			fs::path sourceFile = task.srcDir + "\\" + *it;
			fs::path destinationFile = std::string(Paths::gameMods) + "\\" + task.dstName;

			try {
				fs::copy_file(sourceFile, destinationFile, fs::copy_options::overwrite_existing);

				std::cout << "HUD File copied successfully! " << "(" << task.dstName << ")" << std::endl;
			} catch (const fs::filesystem_error& e) {
				std::cerr << "Filesystem error: " << e.what() << std::endl;
				std::cerr << "Error code: " << e.code() << std::endl;
				std::cerr << "Source: " << e.path1() << std::endl;
				std::cerr << "Destination: " << e.path2() << std::endl;
			} catch (const std::exception& e) {
				std::cerr << "Standard exception: " << e.what() << std::endl;
			} catch (...) {
				std::cerr << "Unknown exception caught!" << std::endl;
			}
		}
	}
}
}



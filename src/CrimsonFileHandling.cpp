#include <iostream>
#include <vector>
#include <filesystem>
#include "Vars.hpp"
#include "Config.hpp"
#include "CrimsonFileHandling.hpp"
#include "File.hpp"

namespace fs = std::filesystem;

namespace Paths {
	const char* assets = "Crimson\\assets";
	const char* config = "Crimson\\configs";
	const char* sounds = "Crimson\\sound";
	const char* huds = "Crimson\\huds";
	const char* weaponwheel = "Crimson\\weaponwheel";
	const char* gameMods = "data\\dmc3\\GData.afs";
}


std::vector<std::string> getDirectories(std::string path) {
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

std::vector<std::string> getFiles(const std::string& path) {
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
		// Optionally, handle or rethrow
	}
	return files;
}

void getHUDsDirectories() {
	HUDdirectories = getDirectories(Paths::huds);
}

void copyHUDtoGame() {
	std::string hudPath = Paths::huds + (std::string)"\\" + activeConfig.selectedHUD;
	std::vector<std::string> files = getFiles(hudPath);
	std::vector<std::string> HUDFilenames = { "id100.pac", "id100V.pac" };

	for (const auto& target : HUDFilenames) {
		if (std::find(files.begin(), files.end(), target) != files.end()) {
			fs::path sourceFile = hudPath + "\\" + target;
			fs::path destinationFile = Paths::gameMods + (std::string)"\\" + target;

			try {
				fs::copy_file(sourceFile, destinationFile, fs::copy_options::overwrite_existing);

				std::cout << "HUD File copied successfully! " << "(" << target << ")" << std::endl;
			}
			catch (const fs::filesystem_error& e) {
				std::cerr << "Filesystem error: " << e.what() << std::endl;
				std::cerr << "Error code: " << e.code() << std::endl;
				std::cerr << "Source: " << e.path1() << std::endl;
				std::cerr << "Destination: " << e.path2() << std::endl;
			}
			catch (const std::exception& e) {
				std::cerr << "Standard exception: " << e.what() << std::endl;
			}
			catch (...) {
				std::cerr << "Unknown exception caught!" << std::endl;
			}
			
		}
	}

}



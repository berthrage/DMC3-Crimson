#include <vector>

#ifndef PATHS_H  
#define PATHS_H

namespace Paths {
	extern const char* assets;
	extern const char* config;
	extern const char* sounds;
	extern const char* huds;
	extern const char* weaponwheel;
	extern const char* gameMods;
}

#endif
std::vector<std::string> getDirectories(std::string path);
std::vector<std::string> getFiles(std::string path);
void getHUDsDirectories();
void copyHUDtoGame();
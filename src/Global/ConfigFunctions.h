#include "Core/RapidJSON.h"
#include <tuple>
#include <type_traits>
#include "CrimsonConfigHandling.h"
#include <filewritestream.h>
#include <../CrimsonConfigGameplay.hpp>
#pragma optimize("", off) // Disable all optimizations
#ifdef NO_SAVE
void SaveConfigFunction()
#else
void SaveConfig()
#endif
{
#ifndef NO_SAVE
	LogFunction();
#endif

	using namespace rapidjson;
	using namespace JSON;

	ToJSON(queuedConfig);
	SerializeConfig(crimsonConfigRoot, queuedCrimsonConfig, crimsonConfigRoot.GetAllocator());
	SerializeConfig(crimsonConfigGameplayRoot, queuedCrimsonGameplay, crimsonConfigGameplayRoot.GetAllocator());

	// Use FILE pointer and FileWriteStream for better large file handling
	FILE* fp = fopen(locationConfig, "w");
	if (!fp) {
		Log("Failed to open file for writing.");
		return;
	}

	FILE* fp_g = fopen(locationConfigGameplay, "w");
	if (!fp_g) {
		Log("Failed to open Gameplay file for writing.");
		return;
	}

	char writeBuffer[65536]; // Use a larger buffer for efficiency
	FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));

	char writeBuffer_g[65536]; 
	FileWriteStream os_g(fp_g, writeBuffer_g, sizeof(writeBuffer_g));

	// Use PrettyWriter for formatted output
	PrettyWriter<FileWriteStream> prettyWriter(os);
	PrettyWriter<FileWriteStream> prettyWriterGameplay(os_g);
	crimsonConfigRoot.Accept(prettyWriter);
	crimsonConfigGameplayRoot.Accept(prettyWriterGameplay);

	fclose(fp);  // Close file to ensure data is fully written
	fclose(fp_g); // Close file to ensure data is fully written
}


#ifdef NO_LOAD
void LoadConfigFunction()
#else
void LoadConfig()
#endif
{
#ifndef NO_LOAD
	LogFunction();
#endif

	using namespace rapidjson;
	using namespace JSON;


	auto file = LoadFile(locationConfig);
	auto fileGameplay = LoadFile(locationConfigGameplay);
	if (!file) {
		Log("LoadFile failed.");

		CreateMembers(defaultConfig);
		SerializeConfig(crimsonConfigRoot, defaultCrimsonConfig, crimsonConfigRoot.GetAllocator());

// 		int numberTest = 42;
// 		int numberArrayTest[2][3] = { {1, 2, 3}, {4, 5, 6} };
// 		uint8 testwithUint8Outside = 33;
// 		int testAgain = 80;
// 
// 		SerializeField(root, root.GetAllocator(), "numberTest", numberTest);
// 		SerializeField(root, root.GetAllocator(), "numberArrayTest", numberArrayTest);
// 		SerializeField(root, root.GetAllocator(), "testwithUint8Outside", testwithUint8Outside);
// 		SerializeField(root, root.GetAllocator(), "testAgain", testAgain);

		SaveConfig();

		return;
	}

	if (!fileGameplay) {
		Log("LoadFile Gameplay failed.");

		SerializeConfig(crimsonConfigGameplayRoot, defaultCrimsonGameplay, crimsonConfigGameplayRoot.GetAllocator());

		SaveConfig();

		return;
	}


	auto name = const_cast<const char*>(reinterpret_cast<char*>(file));
	auto nameGameplay = const_cast<const char*>(reinterpret_cast<char*>(fileGameplay));

	auto& result = crimsonConfigRoot.Parse(name);
	auto& resultGameplay = crimsonConfigGameplayRoot.Parse(nameGameplay);

	if (result.HasParseError()) {
		auto code = result.GetParseError();
		auto off = result.GetErrorOffset();

		Log("Parse failed. "
#ifdef _WIN64
			"%u %llu",
#else
			"%u %u",
#endif
			code, off);

		return;
	}

	if (resultGameplay.HasParseError()) {
		auto code = resultGameplay.GetParseError();
		auto off = resultGameplay.GetErrorOffset();

		Log("Parse Gameplay failed. "
#ifdef _WIN64
			"%u %llu",
#else
			"%u %u",
#endif
			code, off);

		return;
	}

	CreateMembers(defaultConfig);
	//SerializeConfig(root, defaultCrimsonConfig, root.GetAllocator());
	//CreateMembersCrimson(root, defaultCrimsonConfig);

	// At this point all file members have been applied. Extra or obsolete file members can exist.
	// If members were missing in the file they were created and have their default values.


	// The actual configs are still untouched though.
	// Let's update them!

	ToConfig(queuedConfig);
	ParseConfig(crimsonConfigRoot, queuedCrimsonConfig);
	ParseConfig(crimsonConfigGameplayRoot, queuedCrimsonGameplay);

	CopyMemory(&activeConfig, &queuedConfig, sizeof(activeConfig));
	CopyMemory(&activeCrimsonConfig, &queuedCrimsonConfig, sizeof(activeCrimsonConfig));
	CopyMemory(&activeCrimsonGameplay, &queuedCrimsonGameplay, sizeof(activeCrimsonGameplay));


	SaveConfig();

	// SaveConfig here in case new members were created.
	// This way we don't have to rely on a later SaveConfig to update the file.
}

#ifdef NO_INIT
void InitConfigFunction()
#else
void InitConfig()
#endif
{
#ifndef NO_INIT
	LogFunction();
#endif

	using namespace rapidjson;
	using namespace JSON;

	CreateDirectoryA(directoryName, 0);

	snprintf(locationConfig, sizeof(locationConfig), "%s/%s", directoryName, fileName);
	snprintf(locationConfigGameplay, sizeof(locationConfigGameplay), "%s/%s", directoryName, fileNameGameplay);

	crimsonConfigRoot.SetObject();
	crimsonConfigGameplayRoot.SetObject();

	g_allocator = &crimsonConfigRoot.GetAllocator();
	g_gameplay_allocator = &crimsonConfigGameplayRoot.GetAllocator();
}
#pragma optimize("", on) // Re-enable optimizations
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

	// Use FILE pointer and FileWriteStream for better large file handling
	FILE* fp = fopen(locationConfig, "w");
	if (!fp) {
		Log("Failed to open file for writing.");
		return;
	}

	char writeBuffer[65536]; // Use a larger buffer for efficiency
	FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));

	// Use PrettyWriter for formatted output
	PrettyWriter<FileWriteStream> prettyWriter(os);
	crimsonConfigRoot.Accept(prettyWriter);

	fclose(fp);  // Close file to ensure data is fully written
}

void SaveConfigGameplay() {
#ifndef NO_SAVE
	LogFunction();
#endif
	using namespace rapidjson;
	using namespace JSON;

	SerializeConfig(crimsonConfigGameplayRoot, queuedCrimsonGameplay, crimsonConfigGameplayRoot.GetAllocator());

	FILE* fp_g = fopen(locationConfigGameplay, "w");
	if (!fp_g) {
		Log("Failed to open Gameplay file for writing.");
		return;
	}

	char writeBuffer_g[65536];
	FileWriteStream os_g(fp_g, writeBuffer_g, sizeof(writeBuffer_g));
	PrettyWriter<FileWriteStream> prettyWriterGameplay(os_g);

	crimsonConfigGameplayRoot.Accept(prettyWriterGameplay);
	fclose(fp_g);
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
	if (!file) {
		Log("LoadFile failed.");

		CreateMembers(defaultConfig);
		SerializeConfig(crimsonConfigRoot, defaultCrimsonConfig, crimsonConfigRoot.GetAllocator());

		SaveConfig();
		return;
	}

	auto name = const_cast<const char*>(reinterpret_cast<char*>(file));
	auto& result = crimsonConfigRoot.Parse(name);

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

	CreateMembers(defaultConfig);
	//SerializeConfig(root, defaultCrimsonConfig, root.GetAllocator());
	//CreateMembersCrimson(root, defaultCrimsonConfig);

	// At this point all file members have been applied. Extra or obsolete file members can exist.
	// If members were missing in the file they were created and have their default values.

	// The actual configs are still untouched though.
	// Let's update them!

	ToConfig(queuedConfig);
	ParseConfig(crimsonConfigRoot, queuedCrimsonConfig);
	CopyMemory(&activeConfig, &queuedConfig, sizeof(activeConfig));
	CopyMemory(&activeCrimsonConfig, &queuedCrimsonConfig, sizeof(activeCrimsonConfig));

	// SaveConfig here in case new members were created.
	// This way we don't have to rely on a later SaveConfig to update the file.
	SaveConfig();
}

void LoadConfigGameplay() {
#ifndef NO_LOAD
	LogFunction();
#endif
	using namespace rapidjson;
	using namespace JSON;

	auto fileGameplay = LoadFile(locationConfigGameplay);
	if (!fileGameplay) {
		Log("LoadFile Gameplay failed.");

		crimsonConfigGameplayRoot.SetObject();
		g_gameplay_allocator = &crimsonConfigGameplayRoot.GetAllocator();

		SerializeConfig(crimsonConfigGameplayRoot, defaultCrimsonGameplay, crimsonConfigGameplayRoot.GetAllocator());
		CopyMemory(&queuedCrimsonGameplay, &defaultCrimsonGameplay, sizeof(queuedCrimsonGameplay));
		CopyMemory(&activeCrimsonGameplay, &queuedCrimsonGameplay, sizeof(activeCrimsonGameplay));

		SaveConfigGameplay();
		return;
	}

	auto nameGameplay = const_cast<const char*>(reinterpret_cast<char*>(fileGameplay));
	auto& resultGameplay = crimsonConfigGameplayRoot.Parse(nameGameplay);

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

	ParseConfig(crimsonConfigGameplayRoot, queuedCrimsonGameplay);
	CopyMemory(&activeCrimsonGameplay, &queuedCrimsonGameplay, sizeof(activeCrimsonGameplay));

	SaveConfigGameplay();
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
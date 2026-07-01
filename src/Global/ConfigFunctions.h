#include "Core/RapidJSON.h"
#include <tuple>
#include <type_traits>
#include "CrimsonConfigHandling.h"
#include <filewritestream.h>
#include <../CrimsonConfigGameplay.hpp>
#include <../CrimsonInput.hpp>
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

void SaveConfigInput() {
#ifndef NO_SAVE
	LogFunction();
#endif
	using namespace rapidjson;
	using namespace JSON;

	SerializeConfig(crimsonInputRoot, queuedCrimsonInput, crimsonInputRoot.GetAllocator());

	FILE* fp_i = fopen(locationConfigInput, "w");
	if (!fp_i) {
		Log("Failed to open Input file for writing.");
		return;
	}

	char writeBuffer_i[65536];
	FileWriteStream os_i(fp_i, writeBuffer_i, sizeof(writeBuffer_i));
	PrettyWriter<FileWriteStream> prettyWriterInput(os_i);

	crimsonInputRoot.Accept(prettyWriterInput);
	fclose(fp_i);
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

	// === BEGIN INPUT BIND MIGRATION ===
	// If CrimsonConfig.json still has ButtonConfig/KeyboardConfig/xinputSlots in System
	// and CrimsonInput.json does not exist yet, migrate the binds before
	// the main config save would wipe them out.
	if (crimsonConfigRoot.HasMember("System") && crimsonConfigRoot["System"].IsObject()) {
		auto& sys = crimsonConfigRoot["System"];
		if (sys.HasMember("ButtonConfig") || sys.HasMember("KeyboardConfig") || sys.HasMember("xinputSlots")) {
			// Check if CrimsonInput.json already exists on disk
			FILE* testInput = fopen(locationConfigInput, "r");
			if (!testInput) {
				Log("Migrating button/keyboard binds from CrimsonConfig.json to CrimsonInput.json");

				// Build a temp JSON doc from the old bind data
				crimsonInputRoot.SetObject();
				auto& inputAlloc = crimsonInputRoot.GetAllocator();

				if (sys.HasMember("ButtonConfig")) {
					rapidjson::Value btnCfg;
					btnCfg.CopyFrom(sys["ButtonConfig"], inputAlloc);
					crimsonInputRoot.AddMember("ButtonConfig", btnCfg, inputAlloc);
				}
				if (sys.HasMember("KeyboardConfig")) {
					rapidjson::Value kbCfg;
					kbCfg.CopyFrom(sys["KeyboardConfig"], inputAlloc);
					crimsonInputRoot.AddMember("KeyboardConfig", kbCfg, inputAlloc);
				}
				if (sys.HasMember("xinputSlots")) {
					rapidjson::Value xiSlots;
					xiSlots.CopyFrom(sys["xinputSlots"], inputAlloc);
					crimsonInputRoot.AddMember("xinputSlots", xiSlots, inputAlloc);
				}

				// Parse into queued/active CrimsonInput
				ParseConfig(crimsonInputRoot, queuedCrimsonInput);
				CopyMemory(&activeCrimsonInput, &queuedCrimsonInput, sizeof(activeCrimsonInput));

				// Save CrimsonInput.json
				SerializeConfig(crimsonInputRoot, queuedCrimsonInput, inputAlloc);
				SaveConfigInput();

				// Remove old members from main config root so SaveConfig won't rewrite them
				sys.RemoveMember("ButtonConfig");
				sys.RemoveMember("KeyboardConfig");
				sys.RemoveMember("xinputSlots");
			} else {
				fclose(testInput);
			}
		}
	}
	// === END INPUT BIND MIGRATION ===

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

void LoadConfigInput() {
#ifndef NO_LOAD
	LogFunction();
#endif
	using namespace rapidjson;
	using namespace JSON;

	auto fileInput = LoadFile(locationConfigInput);
	if (!fileInput) {
		Log("LoadFile Input failed.");

		// Double-check: if the file actually exists on disk (fopen sees it
		// but LoadFile didn't — e.g. path mismatch), do NOT overwrite it.
		FILE* diskCheck = fopen(locationConfigInput, "r");
		if (diskCheck) {
			fclose(diskCheck);
			Log("Input file exists on disk but LoadFile failed — skipping default creation to avoid data loss.");
		} else {
			crimsonInputRoot.SetObject();
			g_input_allocator = &crimsonInputRoot.GetAllocator();

			SerializeConfig(crimsonInputRoot, defaultCrimsonInput, crimsonInputRoot.GetAllocator());
			CopyMemory(&queuedCrimsonInput, &defaultCrimsonInput, sizeof(queuedCrimsonInput));
			CopyMemory(&activeCrimsonInput, &queuedCrimsonInput, sizeof(activeCrimsonInput));

			SaveConfigInput();
		}
		return;
	}

	auto nameInput = const_cast<const char*>(reinterpret_cast<char*>(fileInput));
	auto& resultInput = crimsonInputRoot.Parse(nameInput);

	if (resultInput.HasParseError()) {
		auto code = resultInput.GetParseError();
		auto off = resultInput.GetErrorOffset();

		Log("Parse Input failed. "
#ifdef _WIN64
			"%u %llu",
#else
			"%u %u",
#endif
			code, off);

		return;
	}

	ParseConfig(crimsonInputRoot, queuedCrimsonInput);
	CopyMemory(&activeCrimsonInput, &queuedCrimsonInput, sizeof(activeCrimsonInput));
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
	snprintf(locationConfigInput, sizeof(locationConfigInput), "%s/%s", directoryName, fileNameInput);

	crimsonConfigRoot.SetObject();
	crimsonConfigGameplayRoot.SetObject();
	crimsonInputRoot.SetObject();

	g_allocator = &crimsonConfigRoot.GetAllocator();
	g_gameplay_allocator = &crimsonConfigGameplayRoot.GetAllocator();
	g_input_allocator = &crimsonInputRoot.GetAllocator();
}
#pragma optimize("", on) // Re-enable optimizations
#include "Core/RapidJSON.h"
#include <tuple>
#include <type_traits>
#include "CrimsonConfigHandling.h"
#include <filewritestream.h>

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
	SerializeConfig(root, queuedCrimsonConfig, root.GetAllocator());

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
	root.Accept(prettyWriter);

	fclose(fp);  // Close file to ensure data is fully written
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
		//CreateMembersCrimson(root, defaultCrimsonConfig);
		SerializeConfig(root, defaultCrimsonConfig, root.GetAllocator());

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


	auto name = const_cast<const char*>(reinterpret_cast<char*>(file));

	auto& result = root.Parse(name);

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
	ParseConfig(root, queuedCrimsonConfig);

	CopyMemory(&activeConfig, &queuedConfig, sizeof(activeConfig));
	CopyMemory(&activeCrimsonConfig, &queuedCrimsonConfig, sizeof(activeCrimsonConfig));


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

	root.SetObject();

	g_allocator = &root.GetAllocator();
}
// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "FMOD.hpp"
#include "Core/Core.hpp"
#include "Vars.hpp"

#include "Core/Macros.h"

#include "Core/DebugSwitch.hpp"

bool FMOD_init = false;

FMOD_System_CreateSound_t FMOD_System_CreateSound = 0;

FMOD_Channel_SetVolume_t FMOD_Channel_SetVolume = 0;

bool FMOD_Init()
{
	LogFunction();

	byte32 error = 0;

	const char * libName = "fmod64.dll";


	SetLastError(0);

	auto lib = LoadLibraryA(libName);

	error = GetLastError();

	if (!lib)
	{
		Log("LoadLibraryA failed. %X", error);

		return false;
	}




	// FMOD_System_CreateSound
	{
		const char * funcName = "FMOD_System_CreateSound";

		SetLastError(0);

		auto funcAddr = GetProcAddress(lib, funcName);

		error = GetLastError();

		if (!funcAddr)
		{
			Log("GetProcAddress failed. %s %X", funcName, error);

			return false;
		}

		FMOD_System_CreateSound = reinterpret_cast<FMOD_System_CreateSound_t>(funcAddr);

		if constexpr (debug)
		{
			Log("%s %llX", funcName, FMOD_System_CreateSound);
		}
	}



	// FMOD_Channel_SetVolume
	{
		const char * funcName = "FMOD_Channel_SetVolume";

		SetLastError(0);

		auto funcAddr = GetProcAddress(lib, funcName);

		error = GetLastError();

		if (!funcAddr)
		{
			Log("GetProcAddress failed. %s %X", funcName, error);

			return false;
		}

		FMOD_Channel_SetVolume = reinterpret_cast<FMOD_Channel_SetVolume_t>(funcAddr);

		if constexpr (debug)
		{
			Log("%s %llX", funcName, FMOD_Channel_SetVolume);
		}
	}



	Write<uint32>((appBaseAddr + 0x32635 + 1), FMOD_CACHE_SIZE);
	/*
	dmc3.exe+32635 - B9 00003002    - mov ecx,02300000
	dmc3.exe+3263A - FF 15 20CE3100 - call qword ptr [dmc3.exe+34F460]
	*/

	Write<uint32>((appBaseAddr + 0x32658 + 1), FMOD_CACHE_SIZE);
	/*
	dmc3.exe+32658 - BA 00003002 - mov edx,02300000
	dmc3.exe+3265D - E8 A62D3100 - call dmc3.exe+345408
	*/

	return true;
}

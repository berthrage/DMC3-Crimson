// @Research: To avoid logging create error level or func enum to indicate point of failure.
// @Research: Add device name for Acquire log.

// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "Input.hpp"

using namespace DI8;

#include "Macros.h"

#define LogFail(name, value) Log(name " failed. %X", value)

bool InputDevice::CreateInterface()
{
	HRESULT result = 0;



	result = DirectInput8Create
	(
		(HINSTANCE)appBaseAddr,
		0x800,
		IID_IDirectInput8A,
		reinterpret_cast<void**>(&deviceInterface),
		0
	);

	if (result != DI_OK)
	{
		LogFail("DirectInput8Create", result);

		return false;
	}



	return true;
}

bool InputDevice::Create(const GUID& guid, const DIDATAFORMAT* dataFormat, DWORD setCooperativeLevelFlags)
{
	HRESULT result = 0;



	result = deviceInterface->CreateDevice
	(
		guid,
		&device,
		0
	);

	if (result != DI_OK)
	{
		LogFail("deviceInterface->CreateDevice", result);

		return false;
	}



	result = device->SetCooperativeLevel
	(
		appWindow,
		setCooperativeLevelFlags
	);

	if (result != DI_OK)
	{
		LogFail("device->SetCooperativeLevel", result);

		return false;
	}



	result = device->SetDataFormat(dataFormat);

	if (result != DI_OK)
	{
		LogFail("device->SetDataFormat", result);

		return false;
	}



	result = device->Acquire();

	if (result != DI_OK)
	{
		LogFail("device->Acquire", result);
	}



	init = true;



	return true;
}

void InputDevice::Update(DWORD size, void* addr)
{
	if (!init)
	{
		return;
	}



	HRESULT result = 0;

	result = device->GetDeviceState
	(
		size,
		addr
	);



	if
		(
			(GetForegroundWindow() == appWindow) &&
			(
				(result == DIERR_INPUTLOST) ||
				(result == DIERR_NOTACQUIRED)
				)
			)
	{
		result = device->Acquire();

		Log("device->Acquire");
	}
}


bool Keyboard::Create()
{
	if (!CreateInterface())
	{
		Log("CreateInterface failed.");

		return false;
	}

	if
		(
			!InputDevice::Create
			(
				GUID_SysKeyboard,
				&c_dfDIKeyboard,
				DISCL_NONEXCLUSIVE |
				DISCL_FOREGROUND
			)
			)
	{
		Log("InputDevice::Create failed.");

		return false;
	}

	return true;
}

void Keyboard::Update()
{
	InputDevice::Update
	(
		sizeof(state),
		&state
	);
}


bool Mouse::Create()
{
	if (!CreateInterface())
	{
		Log("CreateInterface failed.");

		return false;
	}

	if
		(
			!InputDevice::Create
			(
				GUID_SysMouse,
				&c_dfDIMouse2,
				DISCL_NONEXCLUSIVE |
				DISCL_FOREGROUND
			)
			)
	{
		Log("InputDevice::Create failed.");

		return false;
	}

	return true;
}

void Mouse::Update()
{
	InputDevice::Update
	(
		sizeof(state),
		&state
	);
}

bool Gamepad::Create()
{
	if (!CreateInterface())
	{
		Log("CreateInterface failed.");

		return false;
	}



	HRESULT result = 0;

	result = deviceInterface->EnumDevices
	(
		0,
		enumFunc,
		0,
		DIEDFL_ALLDEVICES
	);

	if (result != DI_OK)
	{
		Log("EnumDevices failed.");

		return false;
	}

	if (!match)
	{
		Log("Gamepad: No Match");

		return false;
	}

	// LogGUID("gamepad guid ", deviceInstance.guidInstance);



	if
		(
			!InputDevice::Create
			(
				deviceInstance.guidInstance,
				&c_dfDIJoystick,
				DISCL_NONEXCLUSIVE |
				DISCL_BACKGROUND
			)
			)
	{
		Log("InputDevice::Create failed.");

		return false;
	}

	return true;
}

void Gamepad::Update()
{
	InputDevice::Update
	(
		sizeof(state),
		&state
	);
}

namespace XI {
	new_XInputGetState_t new_XInputGetState = 0;

	void new_Init(const char* libName)
	{
		LogFunction();



		byte32 error = 0;



		SetLastError(0);

		auto lib = LoadLibraryA(libName);
		if (!lib)
		{
			error = GetLastError();

			Log("LoadLibraryA failed. %s %X", libName, error);

			return;
		}



		// XInputGetState
		{
			const char* funcName = "XInputGetState";

			SetLastError(0);

			auto funcAddr = GetProcAddress(lib, funcName);
			if (!funcAddr)
			{
				error = GetLastError();

				Log("GetProcAddress failed. %s %X", funcName, error);

				return;
			}

			new_XInputGetState = reinterpret_cast<new_XInputGetState_t>(funcAddr);
		}
	}
};

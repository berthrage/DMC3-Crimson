#include "../../ThirdParty/ImGui/imgui.h"
#include "../../ThirdParty/ImGui/imgui_internal.h"

// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "Core_ImGui.hpp"
#include <dxgi.h>
#include <d3d11.h>
#include <d3d10_1.h>
#include <d3dcompiler.h>
#include "Input.hpp"

#include "Macros.h"

using namespace XI;
using namespace DI8;

namespace CoreImGui {
	
#pragma region Window

	LPSTR cursorMap[ImGuiMouseCursor_COUNT] =
	{
		IDC_ARROW,
		IDC_IBEAM,
		IDC_SIZEALL,
		IDC_SIZENS,
		IDC_SIZEWE,
		IDC_SIZENESW,
		IDC_SIZENWSE,
		IDC_HAND,
		IDC_NO,
	};

	constexpr uint32 cursorMapItemCount = static_cast<uint32>(countof(cursorMap));

	void UpdateMouseCursor(HWND windowHandle)
	{
		if (!windowHandle)
		{
			return;
		}

		auto cursor = static_cast<uint32>(ImGui::GetMouseCursor());
		if (cursor >= cursorMapItemCount)
		{
			cursor = 0;
		}

		static auto lastCursor = cursor;

		if (lastCursor != cursor)
		{
			lastCursor = cursor;

			auto texture =
				LoadCursorA
				(
					0,
					cursorMap[cursor]
				);


#ifdef _WIN64
			SetClassLongPtrA
#else
			SetClassLongA
#endif
				(
					windowHandle,
					GCLP_HCURSOR,
					reinterpret_cast<LONG_PTR>(texture)
					);
		}
	}

#pragma endregion
	
#pragma region DI8

	namespace DI8 {
		void Init()
		{
			LogFunction();

			auto& io = ImGui::GetIO();

			io.KeyMap[ImGuiKey_Tab] = KEY::TAB;
			io.KeyMap[ImGuiKey_LeftArrow] = KEY::LEFT;
			io.KeyMap[ImGuiKey_RightArrow] = KEY::RIGHT;
			io.KeyMap[ImGuiKey_UpArrow] = KEY::UP;
			io.KeyMap[ImGuiKey_DownArrow] = KEY::DOWN;
			io.KeyMap[ImGuiKey_PageUp] = KEY::PAGE_UP;
			io.KeyMap[ImGuiKey_PageDown] = KEY::PAGE_DOWN;
			io.KeyMap[ImGuiKey_Home] = KEY::HOME;
			io.KeyMap[ImGuiKey_End] = KEY::END;
			io.KeyMap[ImGuiKey_Insert] = KEY::INSERT;
			io.KeyMap[ImGuiKey_Delete] = KEY::DELETE;
			io.KeyMap[ImGuiKey_Backspace] = KEY::BACKSPACE;
			io.KeyMap[ImGuiKey_Space] = KEY::SPACE;
			io.KeyMap[ImGuiKey_Enter] = KEY::ENTER;
			io.KeyMap[ImGuiKey_Escape] = KEY::ESCAPE;
			io.KeyMap[ImGuiKey_A] = KEY::A;
			io.KeyMap[ImGuiKey_C] = KEY::C;
			io.KeyMap[ImGuiKey_V] = KEY::V;
			io.KeyMap[ImGuiKey_X] = KEY::X;
			io.KeyMap[ImGuiKey_Y] = KEY::Y;
			io.KeyMap[ImGuiKey_Z] = KEY::Z;
		}

		void UpdateKeyboard(DIKEYBOARDSTATE* stateAddr)
		{


			if (!stateAddr)
			{
				return;
			}

			auto& state = *stateAddr;



			auto& io = ImGui::GetIO();

			io.KeyCtrl = ((state.keys[KEY::LEFT_CONTROL] & 0x80) || (state.keys[KEY::RIGHT_CONTROL] & 0x80)) ? true : false;
			io.KeyShift = ((state.keys[KEY::LEFT_SHIFT] & 0x80) || (state.keys[KEY::RIGHT_SHIFT] & 0x80)) ? true : false;
			io.KeyAlt = ((state.keys[KEY::LEFT_ALT] & 0x80) || (state.keys[KEY::RIGHT_ALT] & 0x80)) ? true : false;

			SetMemory
			(
				io.KeysDown,
				0,
				256
			);

			for_all(index, 256)
			{
				if (state.keys[index] & 0x80)
				{
					io.KeysDown[index] = true;
				}
			}
		}

		vec2 mousePositionMultiplier =
		{
			1,
			1
		};

		void UpdateMouse
		(
			HWND windowHandle,
			DIMOUSESTATE2* stateAddr
		)
		{
			if
				(
					!windowHandle ||
					(GetForegroundWindow() != appWindow)
					)
			{
				return;
			}



			if (!stateAddr)
			{
				return;
			}

			auto& state = *stateAddr;

			auto& io = ImGui::GetIO();

			POINT pos = {};

			GetCursorPos(&pos);

			ScreenToClient(windowHandle, &pos);

			io.MousePos = ImVec2
			(
				static_cast<float>(pos.x),
				static_cast<float>(pos.y)
			);

			io.MousePos.x *= mousePositionMultiplier.x;
			io.MousePos.y *= mousePositionMultiplier.y;

			for_all(index, countof(io.MouseDown))
			{
				io.MouseDown[index] = (state.rgbButtons[index]) ? true : false;
			}

			io.MouseWheel += (static_cast<float>(state.lZ) / static_cast<float>(WHEEL_DELTA));
		}
	};

#pragma endregion
	
#pragma region XI
	
	namespace XI {
		struct ButtonHelper
		{
			int id;
			byte32 flag;
		};

		ButtonHelper buttonHelpers[] =
		{
			{ ImGuiNavInput_Activate , GAMEPAD::A              },
			{ ImGuiNavInput_Cancel   , GAMEPAD::B              },
			{ ImGuiNavInput_Menu     , GAMEPAD::X              },
			{ ImGuiNavInput_Input    , GAMEPAD::Y              },
			{ ImGuiNavInput_DpadLeft , GAMEPAD::LEFT           },
			{ ImGuiNavInput_DpadRight, GAMEPAD::RIGHT          },
			{ ImGuiNavInput_DpadUp   , GAMEPAD::UP             },
			{ ImGuiNavInput_DpadDown , GAMEPAD::DOWN           },
			{ ImGuiNavInput_FocusPrev, GAMEPAD::LEFT_SHOULDER  },
			{ ImGuiNavInput_FocusNext, GAMEPAD::RIGHT_SHOULDER },
			{ ImGuiNavInput_TweakSlow, GAMEPAD::LEFT_SHOULDER  },
			{ ImGuiNavInput_TweakFast, GAMEPAD::RIGHT_SHOULDER },
		};

		void UpdateGamepad(XINPUT_STATE* stateAddr)
		{


			if (!stateAddr)
			{
				return;
			}


			auto& state = *stateAddr;





			auto& io = ImGui::GetIO();



			SetMemory
			(
				io.NavInputs,
				0,
				sizeof(io.NavInputs)
			);





			new_XInputGetState
			(
				0,
				&state
			);


			for_all(helperIndex, countof(buttonHelpers))
			{
				auto& helper = buttonHelpers[helperIndex];

				io.NavInputs[helper.id] = (state.Gamepad.wButtons & helper.flag) ? 1.0f : 0;
			}




			auto Update = [&]
			(
				int NAV_NO,
				float VALUE,
				float V0,
				float V1
				)
				{
					float vn = (float)(VALUE - V0) / (float)(V1 - V0);

					if (vn > 1.0f)
					{
						vn = 1.0f;
					}

					if
						(
							(vn > 0.0f) &&
							(io.NavInputs[NAV_NO] < vn)
							)
					{
						io.NavInputs[NAV_NO] = vn;
					}
				};

			Update(ImGuiNavInput_LStickLeft, state.Gamepad.sThumbLX, -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, -32768);
			Update(ImGuiNavInput_LStickRight, state.Gamepad.sThumbLX, +XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, +32767);
			Update(ImGuiNavInput_LStickUp, state.Gamepad.sThumbLY, +XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, +32767);
			Update(ImGuiNavInput_LStickDown, state.Gamepad.sThumbLY, -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, -32767);
		}
	};
	
#pragma endregion
	
	void UpdateDisplaySize
	(
		uint32 width,
		uint32 height
	)
	{
		Log
		(
			"%s %u %u",
			FUNC_NAME,
			width,
			height
		);

		auto& io = ImGui::GetIO();

		io.DisplaySize.x = static_cast<float>(width + 1920);
		io.DisplaySize.y = static_cast<float>(height + 1080);
	}
};

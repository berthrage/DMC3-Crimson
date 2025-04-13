#pragma once

#include "Core.hpp"
#include <Xinput.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

namespace CoreImGui {

#pragma region Windows
void UpdateMouseCursor(HWND windowHandle);
#pragma endregion

#pragma region DI8
namespace DI8 {
void Init();
void UpdateKeyboard(::DI8::DIKEYBOARDSTATE* stateAddr);

extern vec2 mousePositionMultiplier;

void UpdateMouse(HWND windowHandle, DIMOUSESTATE2* stateAddr);
} // namespace DI8

namespace XI {
void UpdateGamepad(XINPUT_STATE* stateAddr);
}
#pragma endregion

void UpdateDisplaySize(uint32 width, uint32 height);
} // namespace CoreImGui
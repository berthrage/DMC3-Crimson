#pragma once

#include "Core/GUI.hpp"
#include <dxgi.h>
#include <vector>

namespace TAB {
enum {
    ITEMS,
    DANTE,
    VERGIL,
    COUNT,
};
};

extern std::vector<KeyBinding> keyBindings;
void GUI_Render(IDXGISwapChain* pSwapChain);
void GUI_Init();
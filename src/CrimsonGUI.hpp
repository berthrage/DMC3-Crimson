#pragma once

#include "Core/GUI.hpp"
#include <dxgi.h>
#include <vector>

namespace TAB {
enum {
    DANTE,
    VERGIL,
    ITEMS,
    COUNT,
};
};

namespace CrimsonGUI {
extern float scaleFactorX;
extern float scaleFactorY;
extern float scaledFontSize;
extern float itemWidth;
}

extern std::vector<KeyBinding> keyBindings;
void GUI_Render(IDXGISwapChain* pSwapChain);
void GUI_Init();

#pragma once

#include "Core/GUI.hpp"
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
void GUI_Render();
void GUI_Init();
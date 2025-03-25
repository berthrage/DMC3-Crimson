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

void InitRedOrbTexture(ID3D11Device* pd3dDevice);
void InitDStyleRankTextures(ID3D11Device* pd3dDevice);
extern std::vector<KeyBinding> keyBindings;
void GUI_Render(IDXGISwapChain* pSwapChain);
void GUI_Init();

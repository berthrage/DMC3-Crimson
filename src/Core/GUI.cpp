// @Todo: namespace + uint64 indices
// @Todo: uint64 UpdateMapIndex + add UpdateMapIndices
// @Todo: Scaling support.
//
// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "Core.hpp"
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <dxgi.h>
#include "GUI.hpp"
#undef DELETE

#include "../../ThirdParty/stb/stb_image.h"

#include "Macros.h"

using namespace DI8;

namespace GUI {
bool save       = false;
float saveTimer = 0;


// @Todo: constexpr
float saveTimeout = 1000; // in ms
};                        // namespace GUI

// bool    GUI_hide        = false;
// float   GUI_hideTimeout = 0;

bool GUI_Button(const char* label, const ImVec2& size) {
    UI::PushID();
    auto update = ImGui::Button(label, size);
    UI::PopID();

    if (update) {
        ::GUI::save = true;
    }

    return update;
}

bool GUI_ResetButton() {
    auto update = GUI_Button("Reset");

    if (update) {
        ::GUI::save = true;
    }

    return update;
}

bool GUI_Checkbox(const char* label, bool& var) {
    UI::PushID();
    auto update = ImGui::Checkbox(label, &var);
    UI::PopID();

    if (update) {
        ::GUI::save = true;
    }

    return update;
}

bool GUI_Checkbox2(const char* label, bool& var, bool& var2) {
    auto update = GUI_Checkbox(label, var2);

    if (update) {
        var = var2;
    }

    return update;
}

bool GUI_ColorEdit4(const char* label, uint8 (&var)[4], float (&var2)[4], ImGuiColorEditFlags flags) {
    bool update = false;

    UI::PushID();
    if (ImGui::ColorEdit4(label, var2, flags)) {
        update = true;

        old_for_all(uint8, index, 4) {
            var[index] = static_cast<uint8>(var2[index] * 255);
        }
    }
    UI::PopID();

    if (update) {
        ::GUI::save = true;
    }


    return update;
}

bool GUI_Color(const char* label, uint8 (&var)[4], float (&var2)[4]) {
    return GUI_ColorEdit4(label, var, var2, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview);
}

bool GUI_Color2(const char* label, uint8 (&var)[4], uint8 (&var2)[4], float (&var3)[4]) {
    auto update = GUI_Color(label, var2, var3);

    if (update) {
        memcpy(var, var2, sizeof(var2));

		if constexpr (debug) {
			Log("GUI_Color memcpy size %u", sizeof(var2));
		}
    }

    return update;
}

bool GUI_Color(const char* label, float (&var)[4], ImGuiColorEditFlags flags) {
    UI::PushID();

    auto update = ImGui::ColorEdit4(label, var, flags);

    UI::PopID();

    if (update) {
        ::GUI::save = true;
    }

    return update;
}

bool GUI_Color2(const char* label, float (&var)[4], float (&var2)[4], ImGuiColorEditFlags flags) {
    auto update = GUI_Color(label, var2, flags);

    if (update) {
        memcpy(var, var2, sizeof(var));
    }

    return update;
}

ID3D11ShaderResourceView* CreateTexture(const char* filename, ID3D11Device* device) {
    void* addr = 0;
    int width  = 0;
    int height = 0;

    D3D11_TEXTURE2D_DESC textureDesc                       = {};
    D3D11_SUBRESOURCE_DATA subresourceData                 = {};
    ID3D11Texture2D* texture                               = 0;
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
    ID3D11ShaderResourceView* shaderResourceView           = 0;

    addr = stbi_load(filename, &width, &height, 0, 4);

    if (!addr) {
        return 0;
    }

    textureDesc.Width            = width;
    textureDesc.Height           = height;
    textureDesc.MipLevels        = 1;
    textureDesc.ArraySize        = 1;
    textureDesc.Format           = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage            = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags        = D3D11_BIND_SHADER_RESOURCE;

    subresourceData.pSysMem     = addr;
    subresourceData.SysMemPitch = (textureDesc.Width * 4);

    device->CreateTexture2D(&textureDesc, &subresourceData, &texture);

    shaderResourceViewDesc.Format              = DXGI_FORMAT_R8G8B8A8_UNORM;
    shaderResourceViewDesc.ViewDimension       = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MipLevels = textureDesc.MipLevels;

    device->CreateShaderResourceView(texture, &shaderResourceViewDesc, &shaderResourceView);

    texture->Release();

    stbi_image_free(addr);

    return shaderResourceView;
}

// @Research: Consider inline.
void TooltipHelper(const char* name, const char* description, float x) {
    ImGui::TextDisabled(name);

    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(x);
        ImGui::Text(description);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void DescriptionHelper(const char* description, float width) {
    ImGui::PushTextWrapPos(width);
    ImGui::Text(description);
    ImGui::PopTextWrapPos();
}

void CenterCursorX(float width) {
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - width) / 2);
}

void CenterText(const char* name) {
    float width = ImGui::CalcTextSize(name).x;

    CenterCursorX(width);

    ImGui::Text(name);
}

bool CenterButton(const char* name) {
    auto& style = ImGui::GetStyle();

    float width = (ImGui::CalcTextSize(name).x + (style.FramePadding.x * 2));

    CenterCursorX(width);

    return GUI_Button(name);
}

bool ScrollbarY() {
    auto window = ImGui::GetCurrentWindow();
    if (!window) {
        return false;
    }

    return window->ScrollbarY;
}

#pragma region Key Bindings

KeyBinding::KeyBinding(const char* _name, KeyData& _activeKeyData, KeyData& _queuedKeyData, KeyData& _defaultKeyData, func_t _func /*= 0*/,
    byte32 _flags /*= 0 */)
    : activeKeyData(_activeKeyData)
    , queuedKeyData(_queuedKeyData)
    , defaultKeyData(_defaultKeyData) {
    name  = _name;
    func  = _func;
    flags = _flags;
}

void KeyBinding::UpdateBuffer(Data& data, KeyData& keyData)

{
    auto buffer    = data.buffer;
    auto& pos      = data.pos;
    auto& keys     = keyData.keys;
    auto& keyCount = keyData.keyCount;


    pos = 0;

    if (keyCount < 1) {
        SetMemory(buffer, 0, sizeof(buffer));
    } else {
        for_all(keyIndex, keyCount) {
            auto& key = keys[keyIndex];


            if (keyIndex > 0) {
                auto dest = (buffer + pos);

                const char* name = " + ";

                auto size = strlen(name);

                CopyMemory(dest, name, size);

                pos += size;
            }


            auto dest = (buffer + pos);

            auto name = DI8::keyNames[key];

            auto size = strlen(name);

            CopyMemory(dest, name, size);

            pos += size;
        }

        buffer[pos] = 0;
    }
};

void KeyBinding::Main()

{
    auto keys32      = *reinterpret_cast<uint32*>(activeKeyData.keys);
    auto& lastKeys32 = main.lastKeys32;


    if (!main.run) {
        main.run = true;

        UpdateBuffer(main, activeKeyData);
    }

    if (lastKeys32 != keys32) {
        lastKeys32 = keys32;

        UpdateBuffer(main, activeKeyData);
    }


    const auto buttonSize = ImVec2{150, ImGui::GetFrameHeight()};


    if (GUI_Button(name, buttonSize)) {
        popupKeyData.Clear();

        showPopup = true;
    }
    ImGui::SameLine();


    ImGui::Text(main.buffer);
    ImGui::SameLine(500);


    if (GUI_ResetButton()) {
        CopyMemory(&queuedKeyData, &defaultKeyData, sizeof(queuedKeyData));
        CopyMemory(&activeKeyData, &queuedKeyData, sizeof(activeKeyData));
    }
};

void KeyBinding::Popup()

{
    if (!showPopup) {
        return;
    }


    auto keys32      = *reinterpret_cast<uint32*>(popupKeyData.keys);
    auto& lastKeys32 = popup.lastKeys32;

    constexpr float width  = 420;
    constexpr float height = 128;


    if (!popup.run) {
        popup.run = true;

        ImGui::SetNextWindowSize(ImVec2(width, height));
        ImGui::SetNextWindowPos(ImVec2(((g_renderSize.x - width) / 2), ((g_renderSize.y - height) / 2)));
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));

    if (ImGui::Begin("KeyPopup", &showPopup, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
        ImGui::Text("");


        if (lastKeys32 != keys32) {
            lastKeys32 = keys32;

            UpdateBuffer(popup, popupKeyData);
        }


        CenterText(popup.buffer);
        ImGui::Text("");


        const auto buttonSize = ImVec2{64, ImGui::GetFrameHeight()};

        auto& style = ImGui::GetStyle();

        CenterCursorX((buttonSize.x * 3) + (style.ItemInnerSpacing.x * 2));


        if (GUI_Button("Escape", buttonSize)) {
            popupKeyData.AddKey(KEY::ESCAPE);
        }
        ImGui::SameLine();

        if (GUI_Button("Delete", buttonSize)) {
            popupKeyData.AddKey(KEY::DELETE);
        }
        ImGui::SameLine();

        if (GUI_Button("Enter", buttonSize)) {
            popupKeyData.AddKey(KEY::ENTER);
        }
        ImGui::Text("");


        if (flags & KeyFlags_AtLeastOneKey) {
            CenterText("This binding requires at least one key.");
        }


        ImGui::Text("");
    }

    ImGui::End();

    ImGui::PopStyleVar(4);
}

void KeyBinding::UpdateKeyData(byte8* state) {
    if (!showPopup) {
        return;
    }


    auto& keys     = popupKeyData.keys;
    auto& keyCount = popupKeyData.keyCount;


    // Discard
    {
        auto& execute = executes[0];

        if (state[KEY::ESCAPE] & 0x80) {
            if (execute) {
                execute = false;

                showPopup = false;
            }
        } else {
            execute = true;
        }
    }


    // Clear
    {
        auto& execute = executes[1];

        if (state[KEY::DELETE] & 0x80) {
            if (execute) {
                execute = false;

                popupKeyData.Clear();
            }
        } else {
            execute = true;
        }
    }


    // Apply
    {
        auto& execute = executes[2];

        if (state[KEY::ENTER] & 0x80) {
            if (execute) {
                execute = false;

                [&]() {
                    if ((keyCount < 1) && (flags & KeyFlags_AtLeastOneKey)) {
                        return;
                    }

                    CopyMemory(activeKeyData.keys, keys, sizeof(activeKeyData.keys));

                    activeKeyData.keyCount = keyCount;


                    CopyMemory(queuedKeyData.keys, keys, sizeof(queuedKeyData.keys));

                    queuedKeyData.keyCount = keyCount;


                    showPopup = false;

                    GUI::save = true;
                }();
            }
        } else {
            execute = true;
        }
    }


    constexpr new_size_t count = 256;

    for_all(index, count) {
        auto& execute = executes[index];

        if (keyCount >= countof(keys)) {
            break;
        }

        if ((index == KEY::ESCAPE) || (index == KEY::DELETE) || (index == KEY::ENTER)) {
            continue;
        }

        if (state[index] & 0x80) {
            if (execute) {
                execute = false;

                popupKeyData.AddKey(static_cast<byte8>(index));
            }
        } else {
            execute = true;
        }
    }
}

void KeyBinding::Check(byte8* state)

{
    if (showPopup) {
        return;
    }

    auto& execute  = executes3[0];
    auto& keys     = activeKeyData.keys;
    auto& keyCount = activeKeyData.keyCount;

    new_size_t keysDown = 0;

    if (keyCount < 1) {
        return;
    }

    for_all(keyIndex, keyCount) {
        auto& key = keys[keyIndex];

        if (state[key] & 0x80) {
            keysDown++;
        }
    }

    if (keysDown == keyCount) {
        if (execute) {
            execute = false;

            [&]() {
                if (!func) {
                    return;
                }

                func();
            }();
        }
    } else {
        execute = true;
    }
}

#pragma endregion
// @Todo: namespace + uint64 indices
// @Todo: uint64 UpdateMapIndex + add UpdateMapIndices
// @Todo: Scaling support.
//
// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "Core.hpp"
#include "../Sound.hpp"
#include "../CrimsonConfig.hpp"
#include "../CrimsonUtil.hpp"
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
    auto defaultFontSize = UI::g_UIContext.DefaultFontSize;
    ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
    auto update = ImGui::Button(label, size);
    ImGui::PopFont();
    UI::PopID();

    if (update) {
        ::GUI::save = true;
    }

    return update;
}

bool GUI_ResetButton() {
    auto update = GUI_Button("Default");

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
        if (activeCrimsonConfig.GUI.sounds) PlaySound(0, 25);
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

ImVec4 ConvertColorFromUint8ToVec4(uint8(&var)[4]) {
    ImVec4 color = { var[0] / 255.0f, var[1] / 255.0f, var[2] / 255.0f, var[3] / 255.0f };

    return color;
}

bool GUI_ColorEdit4(const char* label, uint8 (&var)[4], ImGuiColorEditFlags flags) {
    bool update = false;
    ImVec4 color = { var[0] / 255.0f, var[1] / 255.0f, var[2] / 255.0f, var[3] / 255.0f };
    UI::PushID();

    if (ImGui::ColorEdit4(label, (float*)&color, flags)) {
        update = true;

		
		for (int index = 0; index < 4; ++index) {
			
			var[0] = static_cast<uint8>(color.x * 255.0f);  // Red
			var[1] = static_cast<uint8>(color.y * 255.0f);  // Green
			var[2] = static_cast<uint8>(color.z * 255.0f);  // Blue
			var[3] = static_cast<uint8>(color.w * 255.0f);  // Alpha
		}
    }
    UI::PopID();

    if (update) {
        ::GUI::save = true;
    }


    return update;
}

bool GUI_Color(const char* label, uint8 (&var)[4]) {
    return GUI_ColorEdit4(label, var, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview);
}

bool GUI_Color2(const char* label, uint8 (&var)[4], uint8 (&var2)[4]) {
    auto update = GUI_Color(label, var2);

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

#pragma region SiyCodeAdapted
std::pair<uint16_t, const char*> getButtonInfo(uint16_t buttonNum) {
	for (const auto& pair : buttonPairs) {
		if (pair.first == buttonNum) {
			return pair;
		}
	}
	return buttonPairs[0];
}

bool GUI_ButtonCombo(const char* label, uint16_t& currentButton) {
	bool update = false;

	if (ImGui::BeginCombo(label, getButtonInfo(currentButton).second)) {
		for (const auto& buttonPair : buttonPairs) {
			bool is_selected = (currentButton == buttonPair.first);
			if (ImGui::Selectable(buttonPair.second, is_selected)) {
				currentButton = buttonPair.first;
                update = true;
			}
			if (is_selected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	if (update) {
		::GUI::save = true;
	}

	return update;
}

bool GUI_TitleCheckbox2(const char* title, bool& var1, bool& var2, bool ccsRequired, float separatorSize) {
	auto defaultFontSize = UI::g_UIContext.DefaultFontSize;
	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);

	auto update = GUI_Checkbox2(title, var1, var2); 
	ImGui::PopFont();

	if (ccsRequired) {
		ImGui::SameLine();
		GUI_CCSRequirementButton();
	}

	UI::SeparatorEx(separatorSize);
	ImGui::Text("");

	if (update) {
		::GUI::save = true;
	}

	return update;
}

void GUI_Title(const char* title, bool ccsRequired, float separatorSize) {
	auto defaultFontSize = UI::g_UIContext.DefaultFontSize;

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
	ImGui::Text(title);
	ImGui::PopFont();
	if (ccsRequired) {
		ImGui::SameLine();
		GUI_CCSRequirementButton();
	}
	UI::SeparatorEx(separatorSize);
	ImGui::Text("");
}

bool GUI_CCSRequirementButton() {
	// Check if parent window is disabled by checking its alpha
	float currentAlpha = ImGui::GetStyle().Alpha;
	bool wasDisabled = currentAlpha < 1.0f; // Approximate check for disabled state

	if (wasDisabled) {
		// Only restore the item flag but keep the alpha (for visual appearance)
		ImGui::PopItemFlag();
	}

	auto defaultFontSize = UI::g_UIContext.DefaultFontSize;
	ImGui::PushStyleColor(ImGuiCol_Button, CrimsonUtil::HexToImVec4(0xD81B57FF));
	ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.69f]);

	ImGui::SmallButton("CCS");

	ImGui::PopFont();
	ImGui::PopItemFlag();
	ImGui::PopStyleColor();

	if (ImGui::IsItemHovered()) {
		// Push full opacity for tooltip
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
		ImGui::SetTooltip("Requires Crimson Character System");
		ImGui::PopStyleVar();
	}

	if (wasDisabled) {
		// Restore disabled state
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
	}

	return false;
}

bool GUI_ButtonCombo2(const char* label, uint16_t& currentButton, uint16_t& currentButton2) {
	auto update = GUI_ButtonCombo(label, currentButton2);

	if (update) {
        currentButton = currentButton2;
	}

	return update;
}
#pragma endregion

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
	auto defaultFontSize = UI::g_UIContext.DefaultFontSize;
	auto scaleFactorY = ImGui::GetIO().DisplaySize.y / 1080;

    ImGui::TextDisabled(name);

    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
		ImGui::SetWindowFontScale(scaleFactorY);
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

	// Prevent empty key if not allowed
	if (keyCount < 1 && !keyData.allowEmpty) {
		return;
	}

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

void KeyBinding::Main() {
	auto keys32 = *reinterpret_cast<uint32*>(activeKeyData.keys);
	auto& lastKeys32 = main.lastKeys32;
    auto defaultFontSize = UI::g_UIContext.DefaultFontSize;
	auto scaleFactorY = ImGui::GetIO().DisplaySize.y / 1080;
	auto scaledFontSize = UI::g_UIContext.DefaultFontSize * scaleFactorY;

	if (!main.run) {
		main.run = true;
		UpdateBuffer(main, activeKeyData);
	}

	if (lastKeys32 != keys32) {
		lastKeys32 = keys32;
		UpdateBuffer(main, activeKeyData);
	}

	const auto buttonSize = ImVec2{ 300 * scaleFactorY, 100 * scaleFactorY};

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 8 * scaleFactorY, 0 });

	ImGui::BeginGroup();

	float buttonStartY = ImGui::GetCursorPosY();

	if (GUI_Button(name, buttonSize)) {
		popupKeyData.Clear();
		showPopup = true;
	}

	ImGui::SameLine();

	float textHeight = ImGui::GetFontSize();
	float textY = buttonStartY + (buttonSize.y - textHeight) * 0.5f;
	ImGui::SetCursorPosY(textY);
	
	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.2f]);
	// Measure text width so we can add 200 after it
	ImVec2 textSize = ImGui::CalcTextSize(main.buffer);
	ImGui::Text(main.buffer);

	ImGui::SameLine(textSize.x + 320 * scaleFactorY);

	if (GUI_ResetButton()) {
		CopyMemory(&queuedKeyData, &defaultKeyData, sizeof(queuedKeyData));
		CopyMemory(&activeKeyData, &queuedKeyData, sizeof(activeKeyData));
	}

	ImGui::EndGroup();
	ImGui::PopStyleVar();

	ImVec2 spacing = ImGui::GetStyle().ItemSpacing;
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - spacing.y);
	ImGui::PopFont();
}


void KeyBinding::Popup() {
    if (!showPopup) {
        return;
    }
    auto defaultFontSize = UI::g_UIContext.DefaultFontSize;
    auto scaleFactorY = ImGui::GetIO().DisplaySize.y / 1080;

    auto keys32      = *reinterpret_cast<uint32*>(popupKeyData.keys);
    auto& lastKeys32 = popup.lastKeys32;

    float width  = 600 * scaleFactorY;
    float height = 330 * scaleFactorY;


    if (!popup.run) {
        popup.run = true;

        ImGui::SetNextWindowSize(ImVec2(width, height));
        ImGui::SetNextWindowPos(ImVec2(((g_renderSize.x - width) / 2), ((g_renderSize.y - height) / 2)));
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 20.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 20.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.85f)); 

    if (ImGui::Begin("KeyPopup", &showPopup, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize)) {
        ImGui::SetWindowFontScale(scaleFactorY);
		
		// Add some spacing so the text below doesn't overlap the button
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);

        ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.2f]);
        ImGui::Text("Capturing Inputs...");
        ImGui::SameLine();
		// Create a close button 'X' at the top right
		float closeButtonSize = 24.0f;
		ImVec2 windowSize = ImGui::GetWindowSize();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));   
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 0.1f)); 
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 1, 1, 0.2f));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));       // remove inner padding
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);              // no rounding
		ImGui::SetCursorPos(ImVec2(windowSize.x - closeButtonSize - 20.0f, 10.0f));
        ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 1.2f]);
		if (ImGui::Button("X", ImVec2(closeButtonSize, closeButtonSize))) {
			showPopup = false;
		}
        ImGui::PopFont();
        ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3); 


        ImGui::PopFont();
        ImGui::Text("");
        ImGui::Text("");
        ImGui::Text("");


        if (lastKeys32 != keys32) {
            lastKeys32 = keys32;

            UpdateBuffer(popup, popupKeyData);
        }

        ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.3f]);
        CenterText(popup.buffer);
        ImGui::PopFont();
        ImGui::Text("");
        ImGui::Text("");
        ImGui::Text("");


        const auto buttonSize = ImVec2{120 * scaleFactorY, 40 * scaleFactorY};

        auto& style = ImGui::GetStyle();

        CenterCursorX((buttonSize.x * 3) + (style.ItemInnerSpacing.x * 2));


        if (GUI_Button("Capture Escape", buttonSize)) {
            popupKeyData.AddKey(KEY::ESCAPE);
        }
        ImGui::SameLine();

        if (GUI_Button("Capture Delete", buttonSize)) {
            popupKeyData.AddKey(KEY::DELETE);
        }
        ImGui::SameLine();

        if (GUI_Button("Capture Enter", buttonSize)) {
            popupKeyData.AddKey(KEY::ENTER);
        }
        ImGui::Text("");

        CenterText("Press ENTER to confirm the Hotkey.");

        if (flags & KeyFlags_AtLeastOneKey) {
            CenterText("This binding requires at least one key.");
        }


        ImGui::Text("");
    }


    ImGui::End();

    ImGui::PopStyleColor();

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
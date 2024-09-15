#pragma once

#include "Core.hpp"
#include "../../ThirdParty/ImGui/imgui.h"
#include "../../ThirdParty/ImGui/imgui_internal.h"
#include <d3d11.h>
#include <d3d10.h>
#include "../ImGuiExtra.hpp"

#include "DebugSwitch.hpp"

template <typename T> struct GetImGuiDataType {
    static constexpr ImGuiDataType value = (TypeMatch<T, int8>::value)     ? ImGuiDataType_S8
                                           : (TypeMatch<T, int16>::value)  ? ImGuiDataType_S16
                                           : (TypeMatch<T, int32>::value)  ? ImGuiDataType_S32
                                           : (TypeMatch<T, int64>::value)  ? ImGuiDataType_S64
                                           : (TypeMatch<T, uint8>::value)  ? ImGuiDataType_U8
                                           : (TypeMatch<T, uint16>::value) ? ImGuiDataType_U16
                                           : (TypeMatch<T, uint32>::value) ? ImGuiDataType_U32
                                           : (TypeMatch<T, uint64>::value) ? ImGuiDataType_U64
                                           : (TypeMatch<T, float>::value)  ? ImGuiDataType_Float
                                           : (TypeMatch<T, double>::value) ? ImGuiDataType_Double
                                           : (TypeMatch<T, bool>::value)   ? ImGuiDataType_U8
                                                                           : 0;
};

namespace GUI {
extern int id;
extern bool save;
extern float saveTimer;
extern float saveTimeout; // in ms
};                        // namespace GUI

// extern bool    GUI_hide;
// extern float   GUI_hideTimeout;

enum {
    SectionFlags_NoNewLine = 1 << 0,
};

bool GUI_Button(const char* label, const ImVec2& size = ImVec2(0, 0));

bool GUI_ResetButton(); 

bool GUI_Checkbox(const char* label, bool& var);

bool GUI_Checkbox2(const char* label, bool& var, bool& var2);

// @Extend
template <typename T> bool GUI_Input(const char* label, T& var, T step = 1, const char* format = 0, ImGuiInputTextFlags flags = 0) {
    UI::PushID();
    auto update =
        ImGui::InputScalar(label, GetImGuiDataType<T>::value, &var, (step == 0) ? 0 : &step, (step == 0) ? 0 : &step, format, flags);
    UI::PopID();

    if (update) {
        ::GUI::save = true;
    }

    return update;
}

// @Extend
template <typename T>
bool GUI_Input2(const char* label, T& var, T& var2, T step = 1, const char* format = 0, ImGuiInputTextFlags flags = 0) {
    auto update = GUI_Input(label, var2, step, format, flags);

    if (update) {
        var = var2;
    }

    return update;
}

template <typename T>
bool GUI_InputDefault(const char* label, T& var, T& defaultVar, const T step = 1, const char* format = 0, ImGuiInputTextFlags flags = 0) {
    auto& window = *ImGui::GetCurrentWindow();
    auto& style  = ImGui::GetStyle();

    auto buttonSize = ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight());

    auto itemWidth = window.DC.ItemWidth;

    if (window.DC.ItemWidthStack.Size > 0) {
        window.DC.ItemWidth -= (buttonSize.x + style.ItemInnerSpacing.x);
        window.DC.ItemWidthStack.back() = window.DC.ItemWidth;
    }

    UI::PushID();
    auto update = ImGui::InputScalar("", GetImGuiDataType<T>::value, &var, (step == 0) ? 0 : &step, (step == 0) ? 0 : &step, format, flags);
    UI::PopID();

    ImGui::SameLine(0, style.ItemInnerSpacing.x);
    if (GUI_Button("D", buttonSize)) {
        update = true;
        var    = defaultVar;
    }

    ImGui::SameLine(0, style.ItemInnerSpacing.x);
    ImGui::Text(label);

    if (window.DC.ItemWidthStack.Size > 0) {
        window.DC.ItemWidth             = itemWidth;
        window.DC.ItemWidthStack.back() = window.DC.ItemWidth;
    }

    if (update) {
        ::GUI::save = true;
    }

    return update;
}

template <typename T>
bool GUI_InputDefault2(
    const char* label, T& var, T& var2, T& defaultVar, const T step = 1, const char* format = 0, ImGuiInputTextFlags flags = 0) {
    auto update = GUI_InputDefault(label, var2, defaultVar, step, format, flags);

    if (update) {
        var = var2;
    }

    return update;
}



template <typename T> bool GUI_Slider(const char* label, T& var, const T min, const T max) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImGuiIO& io         = ImGui::GetIO();

    UI::PushID();
    auto update = ImGui::SliderScalar(label, GetImGuiDataType<T>::value, &var, &min, &max, "%u");
    if (ImGui::IsItemHovered()) {
        window->Flags |= ImGuiWindowFlags_NoScrollbar;
        window->Flags |= ImGuiWindowFlags_NoScrollWithMouse;
        if (io.MouseWheel < 0) {
            if (var > min) {
                var--;
                update = true;
            }
        } else if (io.MouseWheel > 0) {
            if (var < max) {
                var++;
                update = true;
            }
        }
    }
    UI::PopID();

    if (update) {
        ::GUI::save = true;
    }

    return update;
}

template <typename T> bool GUI_Slider2(const char* label, T& var, T& var2, const T min, const T max) {
    auto update = GUI_Slider(label, var2, min, max);

    if (update) {
        var = var2;
    }

    return update;
}

inline void GUI_SectionStart(const char* label) {
    ImGui::Text(label);
    ImGui::Text("");
}

inline void GUI_PushDisable(bool condition) {
    if (condition) {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    }
}

inline void GUI_SectionEnd(byte64 flags = 0) {
    if (!(flags & SectionFlags_NoNewLine)) {
        ImGui::Text("");
    }

    ImGui::Separator();
}

inline void GUI_PopDisable(bool condition) {
    if (condition) {
        ImGui::PopStyleVar();
        ImGui::PopItemFlag();
    }
}

ImVec4 ConvertColorFromUint8ToVec4(uint8(&var)[4]);

bool GUI_ColorEdit4(const char* label, uint8 (&var)[4], ImGuiColorEditFlags flags = 0);

bool GUI_Color(const char* label, uint8 (&var)[4]);

bool GUI_Color2(const char* label, uint8 (&var)[4], uint8 (&var2)[4]);

#pragma region SiyCodeAdapted
static const std::vector<std::pair<uint16_t, const char*>> buttonPairs = { {0x0000, "Nothing"}, {0x0001, "Left Trigger"},
	{0x0002, "Right Trigger"}, {0x0004, "Left Shoulder"}, {0x0008, "Right Shoulder"}, {0x0010, "Y"}, {0x0020, "B"}, {0x0040, "A"},
	{0x0080, "X"}, {0x0100, "Back"}, {0x0200, "Left Thumb"}, {0x0400, "Right Thumb"}, {0x0800, "Start"}, {0x1000, "Up"}, {0x2000, "Right"},
	{0x4000, "Down"}, {0x8000, "Left"} };

std::pair<uint16_t, const char*> getButtonInfo(uint16_t buttonNum);

bool GUI_ButtonCombo(const char* label, uint16_t& currentButton);

bool GUI_ButtonCombo2(const char* label, uint16_t& currentButton, uint16_t& currentButton2);
#pragma endregion

template <uint8 count> bool GUI_ColorPalette(const char* label, uint8 (&vars)[count][4], float (&vars2)[count][4]) {
    bool update = false;
    auto& style = ImGui::GetStyle();

    old_for_all(uint8, index, count) {
        if (GUI_ColorEdit4("", vars[index], vars2[index],
                ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreview)) {
            update = true;
        }

        ImGui::SameLine(0, style.ItemInnerSpacing.x);
    }

    ImGui::Text(label);

    return update;
}

template <uint8 count>
bool GUI_ColorPalette2(const char* label, uint8 (&vars)[count][4], uint8 (&vars2)[count][4], float (&vars3)[count][4]) {
    auto update = GUI_ColorPalette(label, vars2, vars3);

    if (update) {
        memcpy(vars, vars2, sizeof(vars2));

        if constexpr (debug) {
            Log("GUI_ColorPalette memcpy size %u", sizeof(vars2));
        }
    }

    return update;
}

// @Todo: Create RadioButton2.
template <typename varType> bool GUI_RadioButton(const char* label, varType& var, varType value) {
    bool update = false;

    UI::PushID();

    bool selected = (var == value);

    if (ImGui::RadioButton(label, selected)) {
        update = true;
        var    = value;
    }

    UI::PopID();

    return update;
}

bool GUI_Color(const char* label, float (&var)[4], ImGuiColorEditFlags flags = 0);

bool GUI_Color2(const char* label, float (&var)[4], float (&var2)[4], ImGuiColorEditFlags flags = 0);

ID3D11ShaderResourceView* CreateTexture(const char* filename, ID3D11Device* device);

void TooltipHelper(const char* name, const char* description, float x = 2048.0f);

void DescriptionHelper(const char* description, float width = 500.0f);

void CenterCursorX(float width);

void CenterText(const char* name);

bool CenterButton(const char* name);

bool ScrollbarY();

#pragma region Key Bindings

struct KeyBinding {
    struct Data {
        bool run = false;

        char buffer[512] = {};
        offset_t pos     = 0;

        uint32 lastKeys32 = 0;
    };


    const char* name = "";

    Data main  = {};
    Data popup = {};

    bool showPopup = false;

    KeyData popupKeyData = {};

    KeyData& activeKeyData;
    KeyData& queuedKeyData;
    KeyData& defaultKeyData;

    bool executes[3]    = {};
    bool executes2[256] = {};
    bool executes3[1]   = {};

    byte32 flags = 0;

    typedef void (*func_t)();

    func_t func = 0;

    KeyBinding(
        const char* _name, KeyData& _activeKeyData, KeyData& _queuedKeyData, KeyData& _defaultKeyData, func_t _func = 0, byte32 _flags = 0);

    void UpdateBuffer(Data& data, KeyData& keyData);

    void Main();

    void Popup();

    void UpdateKeyData(byte8* state);

    void Check(byte8* state);
};

#pragma endregion
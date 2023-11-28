#pragma once

#include "../../ThirdParty/ImGui/imgui.h"
#include "../../ThirdParty/ImGui/imgui_internal.h"
#include "../Core/GUI.hpp"
#include "../Config.hpp"

namespace FONT {
enum {
    DEFAULT,
    OVERLAY_8,
    OVERLAY_16,
    OVERLAY_32,
    OVERLAY_64,
    OVERLAY_128,
};
};

void BuildFonts();
void UpdateGlobalScale();
void GUI_UpdateStyle();
void CreditsWindow();
void Welcome();
void ResetNavId();
typedef void (*GamepadClose_func_t)();
void GamepadClose(bool& visible, bool& lastVisible, GamepadClose_func_t func);
void OpenMain();
void CloseMain();
void ToggleShowMain();

template <typename T> void OverlayFunction(const char* label, Config::OverlayData& activeData, Config::OverlayData& queuedData, T& func) {
    if (!activeData.enable) {
        return;
    }

    auto& activePos = *reinterpret_cast<ImVec2*>(&activeData.pos);
    auto& queuedPos = *reinterpret_cast<ImVec2*>(&queuedData.pos);

    static uint32 lastX = 0;
    static uint32 lastY = 0;

    static bool run = false;
    if (!run) {
        run = true;

        ImGui::SetNextWindowPos(activePos);

        lastX = static_cast<uint32>(activeData.pos.x);
        lastY = static_cast<uint32>(activeData.pos.y);
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));

    if (ImGui::Begin(label, &activeData.enable,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing)) {
        activePos = queuedPos = ImGui::GetWindowPos();

        uint32 x = static_cast<uint32>(activeData.pos.x);
        uint32 y = static_cast<uint32>(activeData.pos.y);

        if ((lastX != x) || (lastY != y)) {
            lastX = x;
            lastY = y;

            GUI::save = true;
        }

        auto& io = ImGui::GetIO();
        ImGui::PushFont(io.Fonts->Fonts[FONT::OVERLAY_16]);

        ImGui::PushStyleColor(ImGuiCol_Text, *reinterpret_cast<ImVec4*>(&activeData.color));

        func();

        ImGui::PopStyleColor();
        ImGui::PopFont();
    }

    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(4);
}

template <typename T, typename T2> void OverlaySettings(const char* label, T& activeData, T& queuedData, T& defaultData, T2& func) {
    auto& activePos  = *reinterpret_cast<ImVec2*>(&activeData.pos);
    auto& queuedPos  = *reinterpret_cast<ImVec2*>(&queuedData.pos);
    auto& defaultPos = *reinterpret_cast<ImVec2*>(&defaultData.pos);

    GUI_Checkbox2("Enable", activeData.enable, queuedData.enable);
    ImGui::Text("");

    if (GUI_ResetButton()) {
        CopyMemory(&queuedData, &defaultData, sizeof(queuedData));
        CopyMemory(&activeData, &queuedData, sizeof(activeData));

        ImGui::SetWindowPos(label, activePos);
    }
    ImGui::Text("");

    bool condition = !activeData.enable;

    GUI_PushDisable(condition);

    GUI_Color2("Color", activeData.color, queuedData.color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview);
    ImGui::Text("");

    ImGui::PushItemWidth(150);

    if (GUI_InputDefault2<float>("X", activePos.x, queuedPos.x, defaultPos.x, 1, "%g", ImGuiInputTextFlags_EnterReturnsTrue)) {
        ImGui::SetWindowPos(label, activePos);
    }
    if (GUI_InputDefault2<float>("Y", activePos.y, queuedPos.y, defaultPos.y, 1, "%g", ImGuiInputTextFlags_EnterReturnsTrue)) {
        ImGui::SetWindowPos(label, activePos);
    }

    ImGui::PopItemWidth();

    func();

    GUI_PopDisable(condition);
}

template <typename T> void OverlaySettings(const char* label, T& activeData, T& queuedData, T& defaultData) {
    auto Function = []() {};

    return OverlaySettings(label, activeData, queuedData, defaultData, Function);
}

void HandleSaveTimer(float frameRate);

void HandleKeyBindings(KeyBinding* keyBindings, new_size_t count);
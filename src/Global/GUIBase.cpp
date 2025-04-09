// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "GUIBase.hpp"
#include "../Core/Core.hpp"
#include <Xinput.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include "../Core/Input.hpp"
#include "../Core/Core_ImGui.hpp"
#include "GlobalBase.hpp"
#include "../Fonts/font_robotomedium.hpp"
#include "../Global.hpp"

#include <stdio.h>

#include "../Core/Macros.h"

#include "../Core/DebugSwitch.hpp"
#include "../Sound.hpp"

void BuildFonts() {
    auto& io = ImGui::GetIO();

    io.Fonts->AddFontDefault();

    char overlayFont[512];

    {
        char buffer[64];

        GetWindowsDirectoryA(buffer, sizeof(buffer));

        snprintf(overlayFont, sizeof(overlayFont), "%s\\Fonts\\consola.ttf", buffer);
    }

    io.Fonts->AddFontFromFileTTF(overlayFont, 8);
    io.Fonts->AddFontFromFileTTF(overlayFont, 16);
    io.Fonts->AddFontFromFileTTF(overlayFont, 32);
    io.Fonts->AddFontFromFileTTF(overlayFont, 64);
    io.Fonts->AddFontFromFileTTF(overlayFont, 128);

    // Font
    ImFontConfig font_cfg;
    font_cfg.FontDataOwnedByAtlas = false;

    io.FontDefault = io.Fonts->AddFontFromMemoryCompressedTTF(RobotoMedium_compressed_data, RobotoMedium_compressed_size, 16, &font_cfg);

    io.Fonts->Build();
}


void UpdateGlobalScale() {
    auto& io = ImGui::GetIO();

    io.FontGlobalScale = activeConfig.globalScale;
}

void GUI_UpdateStyle() {
    ImGuiStyle& style = ImGui::GetStyle();

    style.Alpha = 1.0f;
    // style.DisabledAlpha = 0.6000000238418579f;
    style.WindowPadding             = ImVec2(6.0f, 3.0f);
    style.WindowRounding            = 0.0f;
    style.WindowBorderSize          = 1.0f;
    style.WindowMinSize             = ImVec2(32.0f, 32.0f);
    style.WindowTitleAlign          = ImVec2(0.5f, 0.5f);
    style.WindowMenuButtonPosition  = ImGuiDir_Left;
    style.ChildRounding             = 0.0f;
    style.ChildBorderSize           = 1.0f;
    style.PopupRounding             = 0.0f;
    style.PopupBorderSize           = 1.0f;
    style.FramePadding              = ImVec2(5.0f, 1.0f);
    style.FrameRounding             = 3.0f;
    style.FrameBorderSize           = 1.0f;
    style.ItemSpacing               = ImVec2(8.0f, 4.0f);
    style.ItemInnerSpacing          = ImVec2(4.0f, 4.0f);
    style.CellPadding               = ImVec2(4.0f, 2.0f);
    style.IndentSpacing             = 21.0f;
    style.ColumnsMinSpacing         = 6.0f;
    style.ScrollbarSize             = 13.0f;
    style.ScrollbarRounding         = 16.0f;
    style.GrabMinSize               = 20.0f;
    style.GrabRounding              = 2.0f;
    style.TabRounding               = 4.0f;
    style.TabBorderSize             = 1.0f;
    style.TabMinWidthForCloseButton = 0.0f;
    style.ColorButtonPosition       = ImGuiDir_Right;
    style.ButtonTextAlign           = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign       = ImVec2(0.0f, 0.0f);

    style.Colors[ImGuiCol_Text]             = ImVec4(0.8588235378265381f, 0.929411768913269f, 0.886274516582489f, 0.8799999952316284f);
    style.Colors[ImGuiCol_TextDisabled]     = ImVec4(0.8588235378265381f, 0.929411768913269f, 0.886274516582489f, 0.2800000011920929f);
    style.Colors[ImGuiCol_WindowBg]         = ImVec4(0.1294117718935013f, 0.1372549086809158f, 0.168627455830574f, 1.0f);
    style.Colors[ImGuiCol_ChildBg]          = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_PopupBg]          = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2666666805744171f, 0.8999999761581421f);
    style.Colors[ImGuiCol_Border]           = ImVec4(0.5372549295425415f, 0.47843137383461f, 0.2549019753932953f, 0.1620000004768372f);
    style.Colors[ImGuiCol_BorderShadow]     = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_FrameBg]          = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2666666805744171f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered]   = ImVec4(0.529411792755127f, 0.1826989650726318f, 0.2561205327510834f, 0.7490196228027344f);
    style.Colors[ImGuiCol_FrameBgActive]    = ImVec4(0.7882353067398071f, 0.04945790022611618f, 0.2059049755334854f, 0.7333333492279053f);
    style.Colors[ImGuiCol_TitleBg]          = ImVec4(0.5921568870544434f, 0.0470588244497776f, 0.1568627506494522f, 0.6509804129600525f);
    style.Colors[ImGuiCol_TitleBgActive]    = ImVec4(0.5921568870544434f, 0.0470588244497776f, 0.1568627506494522f, 1.0f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2666666805744171f, 0.75f);
    style.Colors[ImGuiCol_MenuBarBg]        = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2666666805744171f, 0.4699999988079071f);
    style.Colors[ImGuiCol_ScrollbarBg]      = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2666666805744171f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrab]    = ImVec4(0.7058823704719543f, 0.3146916329860687f, 0.2740484476089478f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.772549033164978f, 0.2151018679141998f, 0.2151018679141998f, 0.7803921699523926f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.7882353067398071f, 0.062745101749897f, 0.2117647081613541f, 1.0f);
    style.Colors[ImGuiCol_CheckMark]            = ImVec4(0.886274516582489f, 0.07058823853731155f, 0.2352941185235977f, 1.0f);
    style.Colors[ImGuiCol_SliderGrab]           = ImVec4(0.7215686440467834f, 0.202105775475502f, 0.1895886063575745f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.9803921580314636f, 0.2783544659614563f, 0.2614378929138184f, 1.0f);
    style.Colors[ImGuiCol_Button]            = ImVec4(0.1997385770082474f, 0.2154863625764847f, 0.2666666805744171f, 0.4901960790157318f);
    style.Colors[ImGuiCol_ButtonHovered]     = ImVec4(0.529411792755127f, 0.1843137294054031f, 0.2549019753932953f, 0.8588235378265381f);
    style.Colors[ImGuiCol_ButtonActive]      = ImVec4(0.7882353067398071f, 0.062745101749897f, 0.2117647081613541f, 1.0f);
    style.Colors[ImGuiCol_Header]            = ImVec4(0.6901960968971252f, 0.05490196123719215f, 0.1843137294054031f, 0.4941176474094391f);
    style.Colors[ImGuiCol_HeaderHovered]     = ImVec4(0.6901960968971252f, 0.05490196123719215f, 0.1843137294054031f, 0.6549019813537598f);
    style.Colors[ImGuiCol_HeaderActive]      = ImVec4(0.6901960968971252f, 0.05490196123719215f, 0.1843137294054031f, 1.0f);
    style.Colors[ImGuiCol_Separator]         = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.5f);
    style.Colors[ImGuiCol_SeparatorHovered]  = ImVec4(0.7490196228027344f, 0.09803923219442368f, 0.09803923219442368f, 0.7799999713897705f);
    style.Colors[ImGuiCol_SeparatorActive]   = ImVec4(0.8117647171020508f, 0.08276816457509995f, 0.08276816457509995f, 1.0f);
    style.Colors[ImGuiCol_ResizeGrip]        = ImVec4(0.8274509906768799f, 0.2141637802124023f, 0.2141637802124023f, 0.03921568766236305f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.8274509906768799f, 0.2156862765550613f, 0.2156862765550613f, 0.7803921699523926f);
    style.Colors[ImGuiCol_ResizeGripActive]  = ImVec4(0.8274509906768799f, 0.2156862765550613f, 0.2156862765550613f, 1.0f);
    style.Colors[ImGuiCol_Tab]               = ImVec4(0.5921568870544434f, 0.0470588244497776f, 0.1568627506494522f, 0.8627451062202454f);
    style.Colors[ImGuiCol_TabHovered]        = ImVec4(0.8039215803146362f, 0.04313725605607033f, 0.2039215713739395f, 0.8235294222831726f);
    style.Colors[ImGuiCol_TabActive]         = ImVec4(0.7882353067398071f, 0.062745101749897f, 0.2117647081613541f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocused]      = ImVec4(0.06666667014360428f, 0.1019607856869698f, 0.1450980454683304f, 0.9724000096321106f);
    style.Colors[ImGuiCol_TabUnfocusedActive]   = ImVec4(0.4235294163227081f, 0.1333333551883698f, 0.1333333551883698f, 1.0f);
    style.Colors[ImGuiCol_PlotLines]            = ImVec4(0.8588235378265381f, 0.929411768913269f, 0.886274516582489f, 0.6299999952316284f);
    style.Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogram]        = ImVec4(0.8588235378265381f, 0.929411768913269f, 0.886274516582489f, 0.6299999952316284f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.7768239974975586f, 0.1466820389032364f, 0.1300263553857803f, 1.0f);
    style.Colors[ImGuiCol_TableHeaderBg]        = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
    style.Colors[ImGuiCol_TableBorderStrong]    = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
    style.Colors[ImGuiCol_TableBorderLight]     = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
    style.Colors[ImGuiCol_TableRowBg]           = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_TableRowBgAlt]        = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.7854077219963074f, 0.1050894483923912f, 0.02696680091321468f, 0.4300000071525574f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
    style.Colors[ImGuiCol_NavHighlight]   = ImVec4(0.9764705896377563f, 0.2957839071750641f, 0.2588235437870026f, 1.0f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
    style.Colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
    style.Colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);
}


void CreditsWindow() {
    if (!g_showMain || !activeConfig.showCredits) {
        return;
    }


    static bool run         = false;
    static float scrollY    = 0;
    static float maxScrollY = 0;


    if (!run) {
        run = true;


        ImGui::SetNextWindowSize(ImVec2(g_renderSize.x, g_renderSize.y));

        ImGui::SetNextWindowPos(ImVec2(0, 0));
    }


    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));

    if (ImGui::Begin("Credits", &activeConfig.showCredits,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::Text("");


        constexpr float scrollSpeedY = 1.0f;
        constexpr new_size_t padding = 30;

        auto& io = ImGui::GetIO();


        {
            auto window = ImGui::GetCurrentWindow();

            ImGui::BringWindowToDisplayBack(window);
        }


        maxScrollY = ImGui::GetScrollMaxY();

        if (scrollY < maxScrollY) {
            scrollY += (scrollSpeedY * g_frameRateMultiplier);
        } else {
            scrollY = 0;
        }


        ImGui::PushFont(io.Fonts->Fonts[FONT::OVERLAY_32]);


        for_all(index, padding) {
            ImGui::Text("");
        }


        CenterText("Special Thanks");
        ImGui::Text("");

        for (const auto& name : specialNames) {
            CenterText(name);
        }

        ImGui::Text("");
        ImGui::Text("");
        ImGui::Text("");

        CenterText("Gold & Platinum Patrons");
        CenterText("");

        for (const auto& name : goldPlatinumNames) {
            CenterText(name);
        }

        ImGui::Text("");


        for_all(index, padding) {
            ImGui::Text("");
        }


        ImGui::PopFont();


        ImGui::Text("");
    }

    ImGui::SetScrollY(scrollY);

    ImGui::End();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar(4);
}


void Welcome() {
    if (!activeConfig.welcome) {
        return;
    }


    static bool run = false;

    if (!run) {
        run = true;

        constexpr float width  = 350;
        constexpr float height = 130;

        ImGui::SetNextWindowSize(ImVec2(width, height));

        ImGui::SetNextWindowPos(ImVec2(((g_renderSize.x - width) / 2), ((g_renderSize.y - height) / 2)));
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);

    if (ImGui::Begin("Welcome", &activeConfig.welcome,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar)) {
        ImGui::Text("");

        CenterText(WELCOME_TEXT);
        ImGui::Text("");

        if (CenterButton("Close")) {
            activeConfig.welcome = queuedConfig.welcome = false;

            SaveConfig();
        }

        ImGui::Text("");
    }

    ImGui::End();

    ImGui::PopStyleVar();
}

// @Order
void ResetNavId() {
    auto contextAddr = ImGui::GetCurrentContext();
    if (!contextAddr) {
        return;
    }
    auto& context = *contextAddr;

    context.NavId = 0;
}

void GamepadClose(bool& visible, bool& lastVisible, GamepadClose_func_t func) {
    auto& io = ImGui::GetIO();


    if (!ImGui::IsWindowFocused()) {
        visible = false;

        return;
    }


    visible = io.NavVisible;

    if (lastVisible != visible) {
        if (io.NavInputs[ImGuiNavInput_Cancel] > 0) {
            return;
        }

        lastVisible = visible;
    }

    if (visible || lastVisible) {
        return;
    }


    static bool execute = false; // Should be fine here, since only 1 window can be active at all times.

    if (io.NavInputs[ImGuiNavInput_Cancel] > 0) {
        if (execute) {
            execute = false;

            func();
        }
    } else {
        execute = true;
    }
}

void OpenMain() {
    DebugLogFunction();

    g_showMain = true;

    ImGui::SetWindowFocus(DMC3C_TITLE);
    //  Required here since g_show could be false, but we still need the data.
    //  Otherwise the menu could auto-close.

    using namespace XI;

    new_XInputGetState(0, &state);

    ::CoreImGui::XI::UpdateGamepad(&state);

    PlaySound(0, 4);
}

void CloseMain() {
    DebugLogFunction();

    g_showMain = false;
    PlaySound(0, 3);
}

void ToggleCrimsonGUI() {
    DebugLogFunction();

    if (!g_showMain) {
        OpenMain();
    } else {
        CloseMain();
    }
}

void HandleSaveTimer(float frameRate) {
   

    using namespace GUI;


    if (saveTimer > 0) {
        saveTimer -= 1.0f;

        return;
    }

    saveTimer = (frameRate * (saveTimeout / 1000));

    // Log("__GUI__");


    if (save) {
        save = false;

        SaveConfig();
    }
}


void HandleKeyBindings(KeyBinding* keyBindings, new_size_t count) {
    for_all(index, count) {
        auto& keyBinding = keyBindings[index];

        keyBinding.Popup();
    }
}

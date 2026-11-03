#include "CrimsonOnboarding.hpp"

#include "Config.hpp"
#include "CrimsonConfig.hpp"
#include "CrimsonFileHandling.hpp"
#include "CrimsonGameModes.hpp"
#include "CrimsonVisualStyle.hpp"
#include "CrimsonGUI.hpp"
#include "ImGuiExtra.hpp"
#include "FMOD.hpp"
#include "Sound.hpp"
#include "Core/Core.hpp"
#include "Core/GUI.hpp"
#include "UI/Texture2DD3D11.hpp"
#include "UI/EmbeddedImages.hpp"

#include <d3d11.h>
#include <string>
#include <format>


namespace {

    // Tile state: which card the mouse is over
    constexpr float TILE_HEIGHT_FACTOR          = 0.82f;
    constexpr float TILE_WIDTH_RATIO            = 0.28f;
    constexpr float TILE_GAP                    = 0.03f;
    constexpr float LOGO_HEIGHT_RATIO           = 0.22f;
    constexpr float PREVIEW_HEIGHT_RATIO        = 0.30f;
    constexpr float TITLE_FONT_SCALE            = 1.4f;

    ImU32 Color(ImU32 c) { return UI::SwapColorEndianness(c); }

    void PlaySelectSound() {
        if (activeCrimsonConfig.GUI.sounds) FMOD_PlaySound(0, 25);
    }

    bool ClickableCard(const char* id,
                       const Texture2DD3D11& texture,
                       ImVec2 textureSize,
                       const char* title,
                       ImU32 titleColor,
                       const char* description,
                       float titlePosY,
                       float imageOffsetY,
                       const ImVec2& cardMin,
                       const ImVec2& cardMax)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const ImU32 cardBg     = Color(0x2A1E20FF);
        const ImU32 cardBgHov  = Color(0x3A2A2DFF);
        const ImU32 border     = Color(0x585152FF);

        // Hover test
        ImVec2 mousePos = ImGui::GetMousePos();
        bool hovered = (mousePos.x >= cardMin.x && mousePos.x <= cardMax.x &&
                        mousePos.y >= cardMin.y && mousePos.y <= cardMax.y);

        dl->AddRectFilled(cardMin, cardMax, hovered ? cardBgHov : cardBg, 6.0f);
        dl->AddRect(cardMin, cardMax, border, 6.0f);

        // Clip everything inside the card so no text overflows into adjacent columns
        dl->PushClipRect(cardMin + ImVec2(1, 1), cardMax - ImVec2(1, 1), true);

        // Image (centered at top)
        float padX = 10.0f;
        float padY = 10.0f;
        float availW = cardMax.x - cardMin.x - padX * 2.0f;
        float imgW = (std::min)(availW, textureSize.x);
        float imgH = textureSize.y * (imgW / textureSize.x);
        ImVec2 imgMin{ cardMin.x + (cardMax.x - cardMin.x - imgW) * 0.5f, cardMin.y + padY + imageOffsetY };
        ImVec2 imgMax{ imgMin.x + imgW, imgMin.y + imgH };
        dl->AddImage(texture, imgMin, imgMax);

        // Title (Benguiat) — uses fixed titlePosY from the caller so all columns match
        float titleFontSize = ImGui::GetFontSize() * 1.4f;
        ImFont* titleFont = UI::g_ImGuiFont_Benguiat[titleFontSize];
        float actualSize = titleFontSize; // fallback if font not found
        if (titleFont) {
            // Use the outer actualSize variable (no 'float' to avoid shadowing)
            actualSize = titleFont->FontSize;
            ImVec2 titleSize = titleFont->CalcTextSizeA(actualSize, FLT_MAX, 0.0f, title);
            ImVec2 titlePos{ cardMin.x + (cardMax.x - cardMin.x - titleSize.x) * 0.5f, titlePosY };
            dl->AddText(titleFont, actualSize, titlePos, titleColor, title);
        }

        float descY = titlePosY + actualSize + 10.0f;
        const float descFontSize = 16.0f;
        ImFont* descFont = UI::g_ImGuiFont_Roboto[descFontSize];
        if (descFont) {
            float wrapWidth = cardMax.x - cardMin.x - padX * 2.0f;
            ImVec2 textSize = descFont->CalcTextSizeA(descFontSize, FLT_MAX, wrapWidth, description);
            float textOffsetX = (std::max)(0.0f, (wrapWidth - textSize.x) * 0.5f);
            ImVec2 descPos{ cardMin.x + padX + textOffsetX, descY };
            ImGui::SetCursorScreenPos(descPos);
            ImGui::PushFont(descFont);
            ImGui::PushTextWrapPos(cardMax.x - padX);
            ImGui::Text("%s", description);
            ImGui::PopTextWrapPos();
            ImGui::PopFont();
        }

        // Invisible button spanning the entire card to capture clicks
        ImGui::SetCursorScreenPos(cardMin);
        ImGui::PushID(id);
        bool clicked = ImGui::InvisibleButton("", cardMax - cardMin);
        ImGui::PopID();

        dl->PopClipRect();
        return clicked;
    }

    // Advance onboarding to the next stage and save config.
    // Stage 4 means "complete" — the whole system shuts off.
    void AdvanceOnboarding() {
        auto& stage = activeCrimsonConfig.Onboarding.welcomeStage;
        stage = queuedCrimsonConfig.Onboarding.welcomeStage = stage + 1;
        // When we reach stage 4 the Run() function will skip everything on next launch.
        SaveConfig();
    }

    // --------------------- Screen: Game Mode ---------------------
    void ScreenGameMode(ID3D11Device* pDevice) {
        const float winW = g_renderSize.x * 0.78f;
        const float winH = g_renderSize.y * 0.80f;
        const float sf   = CrimsonGUI::scaleFactorY;

        ImGui::SetNextWindowSize(ImVec2(winW, winH));
        ImGui::SetNextWindowPos(ImVec2((g_renderSize.x - winW) * 0.5f, (g_renderSize.y - winH) * 0.5f));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f * sf);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.169f, 0.125f, 0.133f, 1.0f));

        static bool open = true;
        if (ImGui::Begin("Onboarding_GameMode", &open,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar))
        {
            // Title — Benguiat (big)
            ImGui::SetCursorPosY(20.0f * sf);
            ImGui::PushFont(UI::g_ImGuiFont_Benguiat[36]);
            const char* question = "Which Game Mode you wanna play?";
            float qw = ImGui::CalcTextSize(question).x;
            ImGui::SetCursorPosX((winW - qw) * 0.5f);
            ImGui::Text("%s", question);
            ImGui::PopFont();

            // Subtitle
            const char* subtitle = "GAME MODE settings can be changed at anytime at the initial Crimson Overlay page.";
            ImGui::SetCursorPosY(68.0f * sf);
            ImGui::PushFont(UI::g_ImGuiFont_Roboto[16]);
            float sw = ImGui::CalcTextSize(subtitle).x;
            ImGui::SetCursorPosX((winW - sw) * 0.5f);
            ImGui::Text("%s", subtitle);
            ImGui::PopFont();

            ImGui::SetCursorPosY(115.0f * sf);

            // Load embedded logos as textures
            static const Texture2DD3D11 vanillaLogo(
                g_Image_VanillaLogo.GetRGBAData(),
                g_Image_VanillaLogo.GetWidth(),
                g_Image_VanillaLogo.GetHeight(), pDevice);
            static const Texture2DD3D11 ssLogo(
                g_Image_StyleSwitcherLogo.GetRGBAData(),
                g_Image_StyleSwitcherLogo.GetWidth(),
                g_Image_StyleSwitcherLogo.GetHeight(), pDevice);
            static const Texture2DD3D11 crimsonLogo(
                g_Image_CrimsonMainLogo.GetRGBAData(),
                g_Image_CrimsonMainLogo.GetWidth(),
                g_Image_CrimsonMainLogo.GetHeight(), pDevice);

            struct ModeCard {
                const Texture2DD3D11* texture;
                ImVec2 texSize;        // displayed size
                const char* title;
                const char* description;
                uint8 preset;
            };

            float cardW = winW * TILE_WIDTH_RATIO;
            float gap   = winW * TILE_GAP;
            float totalW = cardW * 3.0f + gap * 2.0f;
            float startX = (winW - totalW) * 0.5f;

            // Scale logos to a consistent fixed height so all columns align vertically
            auto scaleToHeight = [&](const Texture2DD3D11& tex, float targetH) -> ImVec2 {
                float r = (float)tex.GetWidth() / (float)tex.GetHeight();
                float w = targetH * r;
                if (w > cardW * 0.85f) {
                    // Clamp width so it doesn't overflow the card
                    w = cardW * 0.85f;
                    targetH = w / r;
                }
                return ImVec2{ w, targetH };
            };
            float logoH = winH * LOGO_HEIGHT_RATIO;

            // Representative title colours matching the Crimson Overlay combo boxes
            ImU32 modeColors[3] = {
                Color(0xFFFFFFFF),   // Vanilla  – white
                Color(0xE8BA18FF),   // Style Switcher – gold
                Color(0xDA1B53FF),   // Crimson  – red
            };

            ModeCard cards[3] = {
                { &vanillaLogo, scaleToHeight(vanillaLogo, logoH),
                  "VANILLA MODE",
                  "I want strictly the vanilla game's rules, no Style Switching,\n"
                  "no Weapon Switching beyond 2 slots. No inertia\n"
                  "mechanics or any additions.",
                  GAMEMODEPRESETS::VANILLA },
                { &ssLogo, scaleToHeight(ssLogo, logoH),
                  "STYLE SWITCHER MODE",
                  "I want an experience similar to the DMC3 Nintendo Switch\n"
                  "version or the old DDMK. Style Switching and Full Weapon\n"
                  "Switching, but not much more than that.",
                  GAMEMODEPRESETS::STYLE_SWITCHER },
                { &crimsonLogo, scaleToHeight(crimsonLogo, logoH),
                  "CRIMSON MODE",
                  "I want to enjoy everything Devil May Cry 3: Crimson has\n"
                  "to offer, including new mechanics, an expanded movelist,\n"
                  "inertia, and quality of life changes on top of Style\n"
                  "Switching and Full Weapon Switching.",
                  GAMEMODEPRESETS::CRIMSON },
            };

            for (int i = 0; i < 3; ++i) {
                float cx = startX + (cardW + gap) * i;
                float cardH = winH * TILE_HEIGHT_FACTOR;

                ImVec2 cardMin{ ImGui::GetWindowPos().x + cx,
                                ImGui::GetWindowPos().y + 120.0f * sf };
                ImVec2 cardMax{ cardMin.x + cardW, cardMin.y + cardH };

                if (ClickableCard(cards[i].title, *cards[i].texture,
                                  cards[i].texSize, cards[i].title, modeColors[i],
                                  cards[i].description,
                                  cardMin.y + 10.0f + logoH + 20.0f,
                                  (i == 1) ? 0.0f : 40.0f,
                                  cardMin, cardMax))
                {
                    PlaySelectSound();
                    CrimsonGameModes::SetGameModePreset(cards[i].preset);
                    AdvanceOnboarding();
                }
            }
        }
        ImGui::End();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
    }

    // --------------------- Screen: Visual Style ---------------------
    void ScreenVisualStyle(ID3D11Device* pDevice) {
        const float winW = g_renderSize.x * 0.78f;
        const float winH = g_renderSize.y * 0.80f;
        const float sf   = CrimsonGUI::scaleFactorY;

        ImGui::SetNextWindowSize(ImVec2(winW, winH));
        ImGui::SetNextWindowPos(ImVec2((g_renderSize.x - winW) * 0.5f, (g_renderSize.y - winH) * 0.5f));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f * sf);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.169f, 0.125f, 0.133f, 1.0f));

        static bool open = true;
        if (ImGui::Begin("Onboarding_VisualStyle", &open,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar))
        {
            // Title
            // Title — Benguiat
            ImGui::SetCursorPosY(20.0f * sf);
            ImGui::PushFont(UI::g_ImGuiFont_Benguiat[36]);
            const char* question = "Which Visual Style do you prefer?";
            float qw = ImGui::CalcTextSize(question).x;
            ImGui::SetCursorPosX((winW - qw) * 0.5f);
            ImGui::Text("%s", question);
            ImGui::PopFont();

            // Subtitle
            const char* subtitle = "VISUAL STYLE settings can be changed at anytime at the initial Overlay page, and individual Interface Settings can be toggled to your liking.";
            ImGui::SetCursorPosY(68.0f * sf);
            ImGui::PushFont(UI::g_ImGuiFont_Roboto[16]);
            float sw = ImGui::CalcTextSize(subtitle).x;
            ImGui::SetCursorPosX((winW - sw) * 0.5f);
            ImGui::Text("%s", subtitle);
            ImGui::PopFont();

            ImGui::SetCursorPosY(115.0f * sf);

            // Load preview images from disk
            static const Texture2DD3D11 classicPreview(
                (std::string(Paths::visualStylesPreviews) + "\\Classic.png").c_str(), pDevice);
            static const Texture2DD3D11 hybridPreview(
                (std::string(Paths::visualStylesPreviews) + "\\Hybrid.png").c_str(), pDevice);
            static const Texture2DD3D11 modernPreview(
                (std::string(Paths::visualStylesPreviews) + "\\Modern.png").c_str(), pDevice);

            struct VSCard {
                const Texture2DD3D11* texture;
                const char* title;
                const char* description;
                uint8 preset;
            };

            float cardW = winW * TILE_WIDTH_RATIO;
            float gap   = winW * TILE_GAP;
            float totalW = cardW * 3.0f + gap * 2.0f;
            float startX = (winW - totalW) * 0.5f;

            // Preview image size
            float previewH = winH * PREVIEW_HEIGHT_RATIO;
            float previewW = previewH * (16.0f / 9.0f); // assume 16:9

            // Representative title colours
            ImU32 vsColors[3] = {
                Color(0xFFFFFFFF),   // Classic – white
                Color(0xE8BA18FF),   // Hybrid  – gold
                Color(0xDA1B53FF),   // Modern  – red
            };

            VSCard cards[3] = {
                { &classicPreview,
                  "CLASSIC VISUAL STYLE",
                  "Purely the Vanilla's HUD visuals, no extra additions,\nor fluff.",
                  VISUALSTYLEPRESETS::CLASSIC },
                { &hybridPreview,
                  "HYBRID VISUAL STYLE",
                  "I want something closer to the DMC3 Nintendo Switch\nversion, with some of Crimson's HUD additions.",
                  VISUALSTYLEPRESETS::HYBRID },
                { &modernPreview,
                  "MODERN VISUAL STYLE",
                  "I want all the modernized visuals included in Crimson HUD 1.0,\nfeaturing a sleek interface inspired by DMC5.",
                  VISUALSTYLEPRESETS::MODERN },
            };

            for (int i = 0; i < 3; ++i) {
                float cx = startX + (cardW + gap) * i;
                float cardH = winH * TILE_HEIGHT_FACTOR;

                ImVec2 cardMin{ ImGui::GetWindowPos().x + cx,
                                ImGui::GetWindowPos().y + 120.0f * sf };
                ImVec2 cardMax{ cardMin.x + cardW, cardMin.y + cardH };

                if (ClickableCard(cards[i].title, *cards[i].texture,
                                  ImVec2(previewW, previewH),
                                  cards[i].title, vsColors[i],
                                  cards[i].description,
                                  cardMin.y + 10.0f + previewH + 20.0f,
                                  0.0f,
                                  cardMin, cardMax))
                {
                    PlaySelectSound();
                    CrimsonVisualStyle::SetVisualStylePreset(cards[i].preset);
                    AdvanceOnboarding();
                }
            }
        }
        ImGui::End();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
    }

    // --------------------- Screen: Camera ---------------------
    void ScreenCamera(ID3D11Device* pDevice) {
        const float winW = g_renderSize.x * 0.78f;
        const float winH = g_renderSize.y * 0.80f;
        const float sf   = CrimsonGUI::scaleFactorY;

        ImGui::SetNextWindowSize(ImVec2(winW, winH));
        ImGui::SetNextWindowPos(ImVec2((g_renderSize.x - winW) * 0.5f, (g_renderSize.y - winH) * 0.5f));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f * sf);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.169f, 0.125f, 0.133f, 1.0f));

        static bool open = true;
        if (ImGui::Begin("Onboarding_Camera", &open,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar))
        {
            // Title
            // Title — Benguiat
            ImGui::SetCursorPosY(20.0f * sf);
            ImGui::PushFont(UI::g_ImGuiFont_Benguiat[36]);
            const char* question = "Which camera type do you prefer for DMC3?";
            float qw = ImGui::CalcTextSize(question).x;
            ImGui::SetCursorPosX((winW - qw) * 0.5f);
            ImGui::Text("%s", question);
            ImGui::PopFont();

            // Subtitle
            const char* subtitle = "CAMERA settings can be changed at any time on the Camera Settings menu.";
            ImGui::SetCursorPosY(68.0f * sf);
            ImGui::PushFont(UI::g_ImGuiFont_Roboto[16]);
            float sw = ImGui::CalcTextSize(subtitle).x;
            ImGui::SetCursorPosX((winW - sw) * 0.5f);
            ImGui::Text("%s", subtitle);
            ImGui::PopFont();

            ImGui::SetCursorPosY(115.0f * sf);

            // Load camera preview images from disk
            static const Texture2DD3D11 fixedPreview(
                (std::string(Paths::cameraPreviews) + "\\FixedCamera.png").c_str(), pDevice);
            static const Texture2DD3D11 thirdPersonPreview(
                (std::string(Paths::cameraPreviews) + "\\ThirdPersonCamera.png").c_str(), pDevice);

            struct CamCard {
                const Texture2DD3D11* texture;
                const char* title;
                const char* description;
                bool thirdPerson; // true = third person, false = fixed
            };

            float cardW = winW * TILE_WIDTH_RATIO;
            float gap   = winW * TILE_GAP;
            // Two cards centered
            float totalW = cardW * 2.0f + gap;
            float startX = (winW - totalW) * 0.5f;

            float previewH = winH * PREVIEW_HEIGHT_RATIO;
            float previewW = previewH * (16.0f / 9.0f);

            CamCard cards[2] = {
                { &fixedPreview,
                  "FIXED CAMERA",
                  "Give me the original Fixed Camera experience,\nwith cinematic angles and gorgeous overhead pans.",
                  false },
                { &thirdPersonPreview,
                  "THIRD PERSON CAMERA",
                  "Give me a more modern Over the Shoulder experience,\nwith all new angles and ways to experience combat and\nvisuals in DMC3. The third person camera switches to\nfixed angles in specific locations, seeking to maintain\nDMC3's cinematic language.",
                  true },
            };

            for (int i = 0; i < 2; ++i) {
                float cx = startX + (cardW + gap) * i;
                float cardH = winH * TILE_HEIGHT_FACTOR;

                ImVec2 cardMin{ ImGui::GetWindowPos().x + cx,
                                ImGui::GetWindowPos().y + 120.0f * sf };
                ImVec2 cardMax{ cardMin.x + cardW, cardMin.y + cardH };

                if (ClickableCard(cards[i].title, *cards[i].texture,
                                  ImVec2(previewW, previewH),
                                  cards[i].title, Color(0xFFFFFFFF),
                                  cards[i].description,
                                  cardMin.y + 10.0f + previewH + 20.0f,
                                  0.0f,
                                  cardMin, cardMax))
                {
                    PlaySelectSound();
                    activeCrimsonConfig.Camera.thirdPersonCamera =
                        queuedCrimsonConfig.Camera.thirdPersonCamera = cards[i].thirdPerson;
                    // When enabling third person, also enable panoramic camera
                    activeCrimsonConfig.Camera.panoramicCam =
                        queuedCrimsonConfig.Camera.panoramicCam = cards[i].thirdPerson;
                    AdvanceOnboarding();
                }
            }
        }
        ImGui::End();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
    }

} 

void CrimsonOnboarding::Run(IDXGISwapChain* pSwapChain) {
    auto& stage = activeCrimsonConfig.Onboarding.welcomeStage;

    // If onboarding is already complete, skip everything.
    if (stage == 4) {
        return;
    }

    ID3D11Device* pDevice = nullptr;
    if (pSwapChain) {
        pSwapChain->GetDevice(IID_PPV_ARGS(&pDevice));
    }
    if (!pDevice) {
        return;
    }

    switch (stage) {
    case 0:
        // Stage 0 is handled by the existing Welcome() in GUIBase.cpp.
        // Welcome() will advance to stage 1 on close.
        break;
    case 1:
        ScreenGameMode(pDevice);
        break;
    case 2:
        ScreenVisualStyle(pDevice);
        break;
    case 3:
        ScreenCamera(pDevice);
        break;
    default:
        break;
    }
}

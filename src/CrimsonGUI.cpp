// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "../ThirdParty/SDL2/SDL_gamecontroller.h"
#include "CrimsonGUI.hpp"
#include "Core/Core.hpp"
#include "CrimsonGameplay.hpp"
#include "Camera.hpp"
#include <shellapi.h>
#include "Core/GUI.hpp"
#include "Core/Core_ImGui.hpp"
#include "Global/GlobalBase.hpp"
#include "Global/GUIBase.hpp"
#include "CrimsonUtil.hpp"
#include "CrimsonFileHandling.hpp"
#include "DetourFunctions.hpp"
#include "DMC3Input.hpp"
#include "SDLStuff.hpp"
#include "File.hpp"
#include "ImGuiExtra.hpp"
#include "FMOD.hpp"
#include "Graphics.hpp"
#include "Internal.hpp"
#include "PatchFunctions.hpp"
#include "Actor.hpp"
#include "ActorBase.hpp"
#include "Arcade.hpp"
#include "Config.hpp"
#include "Event.hpp"
#include "Exp.hpp"
#include "Global.hpp"
#include "HUD.hpp"
#include "Scene.hpp"
#include "Sound.hpp"
#include "Speed.hpp"
#include "Timers.hpp"
#include "Training.hpp"
#include "Window.hpp"
#include "WebAPICalls.hpp"
#include "UI\Texture2DD3D11.hpp"

#include "UI\EmbeddedImages.hpp"

#include <cmath>
#include <array>
#include <format>

#include <shellapi.h>

#include <windows.h>
#include <dxgi.h>
#include <d3d11.h>
#include <thread>

#undef VOID

#include <stdio.h>
#include <intrin.h>
#include <string>
#include <sstream>
#include <iostream>
#include <format>
#include <thread>
#include <chrono>
#include "Vars.hpp"

#include "Core/Macros.h"

#include "Core/DebugSwitch.hpp"
#include <SDL_joystick.h>

#define SDL_FUNCTION_DECLRATION(X) decltype(X)* fn_##X
#define LOAD_SDL_FUNCTION(X) fn_##X = GetSDLFunction<decltype(X)*>(#X)



void DrawMainContent(ID3D11Device* pDevice, UI::UIContext& context);

namespace UI {
    WebAPIResult versionCheckResult = WebAPIResult::Awaiting;
    WebAPIResult patronsQueueResult = WebAPIResult::Awaiting;

	void DrawCrimson(IDXGISwapChain* pSwapChain, const char* title, bool* pIsOpened)
	{
		if (pIsOpened != nullptr && *pIsOpened == false)
			return;

		ID3D11Device* pDevice = nullptr;

		pSwapChain->GetDevice(IID_PPV_ARGS(&pDevice));

		// Todo: move to context
		static bool uiElementsInitialized = false;
		if (!uiElementsInitialized) {
			g_Image_CrimsonHeaderLogo.ResizeByRatioW(size_t(g_UIContext.DefaultFontSize * 12.23f));

			uiElementsInitialized = true;
		}

		// Default font
		ImGui::PushFont(g_ImGuiFont_Roboto[g_UIContext.DefaultFontSize]);
		// 6 tab buttons + a bit more for the update when there is new version
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { g_UIContext.DefaultFontSize * 9.38f * 6.0f + g_UIContext.DefaultFontSize * 3.0f, g_UIContext.DefaultFontSize * 35.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 15.0f);
		ImGui::Begin(title, nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
		ImGui::PopStyleVar(4);
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			ImRect wndRect = window->Rect();
			ImGuiStyle& style = ImGui::GetStyle();

			window->ContentRegionRect = ImRect{ window->ContentRegionRect.Min, window->ContentRegionRect.Max - ImVec2{ 0.0f, 70.0f } };

			float contentMinHeightOffsetFromTop;
			float contentMaxHeightOffsetFromBottom;

			// Draw the main window header graphic
			{
				static const Texture2DD3D11 logo(g_Image_CrimsonHeaderLogo.GetRGBAData(), g_Image_CrimsonHeaderLogo.GetWidth(), g_Image_CrimsonHeaderLogo.GetHeight(), pDevice);

				ImVec2 logoPos = { (wndRect.Max.x + wndRect.Min.x) * 0.5f - logo.GetWidth() * 0.5f, wndRect.Min.y + logo.GetHeight() * 0.3f };

				window->DrawList->AddLine({ wndRect.Min.x + g_UIContext.DefaultFontSize * 1.7f, logoPos.y + logo.GetHeight() * 0.5f },
					{ logoPos.x - g_UIContext.DefaultFontSize * 0.52f, logoPos.y + logo.GetHeight() * 0.5f }, SwapColorEndianness(0xE01D42FF));

				window->DrawList->AddLine({ logoPos.x + logo.GetWidth() + g_UIContext.DefaultFontSize * 0.52f, logoPos.y + logo.GetHeight() * 0.5f },
					{ wndRect.Max.x - g_UIContext.DefaultFontSize * 1.7f, logoPos.y + logo.GetHeight() * 0.5f }, SwapColorEndianness(0xE01D42FF));

				window->DrawList->AddImage(logo, logoPos, logoPos + logo.GetSize());
			}

			// Draw the close button
			{
				if (pIsOpened != nullptr) {
					const auto cursorBackUp = ImGui::GetCursorScreenPos();
					ImGui::SetCursorScreenPos({ wndRect.Max.x - g_UIContext.DefaultFontSize * 1.17f - 10.0f, wndRect.Min.y + g_UIContext.DefaultFontSize * 0.45f });

					if (CloseButton("#Close", { g_UIContext.DefaultFontSize * 1.17f, g_UIContext.DefaultFontSize * 1.17f })) {
						*pIsOpened = false;
					}

					ImGui::SetCursorScreenPos(cursorBackUp);
				}
			}

			// Selected game mode text
			{
				const ImVec2 pos = wndRect.Min + ImVec2{ g_UIContext.DefaultFontSize * 1.33f, g_UIContext.DefaultFontSize * 2.66f };

				window->DrawList->AddText(g_ImGuiFont_Roboto[g_UIContext.DefaultFontSize], g_UIContext.DefaultFontSize, pos, SwapColorEndianness(0xFFFFFFFF), "Selected Game Mode: ");

				const float modeTextWidth = ImGui::CalcTextSize("Selected Game Mode: ").x;

				const char* gameModeString = nullptr;
				ImU32 gameModeStringColor = 0;

				switch (g_UIContext.SelectedGameMode) {
				case UIContext::GameModes::Vanilla:
					gameModeString = "VANILLA MODE";
					gameModeStringColor = 0xFFFFFFFF;
					break;

				case UIContext::GameModes::StyleSwitcher:
					gameModeString = "STYLE SWITCHER MODE";
					gameModeStringColor = SwapColorEndianness(0xE8BA18FF);
					break;

				case UIContext::GameModes::Crimson:
					gameModeString = "CRIMSON MODE";
					gameModeStringColor = SwapColorEndianness(0xDA1B53FF);
					break;

				default:
					gameModeString = "Unknown";
					break;
				}

				window->DrawList->AddText(g_ImGuiFont_RussoOne[g_UIContext.DefaultFontSize], g_UIContext.DefaultFontSize, pos + ImVec2{ modeTextWidth, 0.0f }, gameModeStringColor, gameModeString);
			}

			// Version text
			{
                std::string versionStr{};
				std::string latestUpdateStr{};
				std::string newVersionText{};

				if (g_UIContext.CurrentVersion.PatchLetter == 0) {
					versionStr = std::format("Ver. {}.{}", g_UIContext.CurrentVersion.Major, g_UIContext.CurrentVersion.Minor);
                }
                else {
					versionStr = std::format("Ver. {}.{}{}", g_UIContext.CurrentVersion.Major, g_UIContext.CurrentVersion.Minor, g_UIContext.CurrentVersion.PatchLetter);
                }

                if (g_UIContext.LatestVersion.PatchLetter == 0) {
                    newVersionText = std::format("NEW VERSION AVAILABLE (Ver. {}.{})", g_UIContext.LatestVersion.Major, g_UIContext.LatestVersion.Minor);
                }
                else {
                    newVersionText = std::format("NEW VERSION AVAILABLE (Ver. {}.{}{})", g_UIContext.LatestVersion.Major, g_UIContext.LatestVersion.Minor, g_UIContext.LatestVersion.PatchLetter);
                }
                
				switch (UI::versionCheckResult) {
				case WebAPIResult::Success:
					latestUpdateStr = std::format("Latest Update: {}/{}/{}", g_UIContext.LatestUpdateDate.Day, g_UIContext.LatestUpdateDate.Month, g_UIContext.LatestUpdateDate.Year);
					break;

                case WebAPIResult::Awaiting:
                    latestUpdateStr = "Latest Update...";
                    break;

                default:
                    latestUpdateStr = "";
                    break;
                }

				const float latestUpdateTextWidth = ImGui::CalcTextSize(latestUpdateStr.c_str()).x;
                const float newVersionTextWidth = g_UIContext.NewVersionAvailable ? ImGui::CalcTextSize(newVersionText.c_str()).x : 0.0f;
                const float maxLenthUpdateSection = latestUpdateTextWidth > newVersionTextWidth ? latestUpdateTextWidth : newVersionTextWidth;

                const float versionButtonFontSize = size_t(g_UIContext.DefaultFontSize * 1.45f);

				ImGui::PushFont(g_ImGuiFont_RussoOne[versionButtonFontSize * 0.8f]);

				const float versionButtonWidth = ImGui::CalcTextSize(versionStr.c_str()).x + style.FramePadding.x * 2.0f;

				ImGui::PopFont();

				const ImVec2 pos = ImVec2{ wndRect.Max.x - maxLenthUpdateSection - versionButtonWidth - g_UIContext.DefaultFontSize * 2.0f,
											wndRect.Min.y + g_UIContext.DefaultFontSize * 2.66f };

				// Two lines of text without space in between each with the default font size
				float dateTextAndButtonHeightSum = g_UIContext.DefaultFontSize * 2.0f;

				window->DrawList->AddText(g_ImGuiFont_Roboto[g_UIContext.DefaultFontSize * 0.9f], g_UIContext.DefaultFontSize,
					g_UIContext.NewVersionAvailable ? pos + ImVec2{
						0.0f,
						g_UIContext.DefaultFontSize * 0.5f - dateTextAndButtonHeightSum * 0.5f } : pos, SwapColorEndianness(0xFFFFFFFF), latestUpdateStr.c_str());

				auto cursorBackUp = ImGui::GetCursorScreenPos();
				ImGui::SetCursorScreenPos(pos + ImVec2{ maxLenthUpdateSection + g_UIContext.DefaultFontSize * 0.3f,
														style.FramePadding.y + g_UIContext.DefaultFontSize * 0.5f - g_UIContext.DefaultFontSize * 0.65f });

				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 7.0f);
				ImGui::PushFont(g_ImGuiFont_RussoOne[versionButtonFontSize * 1.0f]);

				if (InfoButton(versionStr.c_str())) {
                    // Todo: Redirect to patch notes
				}

				ImGui::PopFont();
				ImGui::PopStyleVar();

				if (g_UIContext.NewVersionAvailable) {
					ImGui::SetCursorScreenPos(pos + ImVec2{ 0.0f, g_UIContext.DefaultFontSize * 0.5f });

					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 5.0f, 0.0f });
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);

					ImGui::PushFont(g_ImGuiFont_Roboto[g_UIContext.DefaultFontSize * 0.9f]);

					if (InfoButton(newVersionText.c_str())) {
						ShellExecute(0, 0, "https://github.com/berthrage/Devil-May-Cry-3-Crimson/releases/new", 0, 0, SW_SHOW);
					}

					ImGui::PopFont();

					ImGui::PopStyleVar(2);
				}

				ImGui::SetCursorScreenPos(cursorBackUp);
			}

			// Draw main tabs / sub tabs
			{
				const ImVec2 tabBtnSize = { g_UIContext.DefaultFontSize * 9.38f, g_UIContext.DefaultFontSize * 2.4f };
				const ImVec2 subTabBtnSize = { tabBtnSize.x * 0.8f, tabBtnSize.y * 0.8f };

				// The black strip behind tab buttons
				window->DrawList->AddRectFilled(
					{ wndRect.Min.x, wndRect.Min.y + 2.0f * tabBtnSize.y },
					{ wndRect.Max.x, wndRect.Min.y + 2.0f * tabBtnSize.y + tabBtnSize.y },
					SwapColorEndianness(0x000000FF)
				);

				ImGui::PushFont(g_ImGuiFont_RussoOne[g_UIContext.DefaultFontSize * 0.9f]);

				contentMinHeightOffsetFromTop = 2.0f * tabBtnSize.y + tabBtnSize.y;

				// Sub tabs
				if (g_UIContext.SelectedTab == UIContext::MainTabs::Options || g_UIContext.SelectedTab == UIContext::MainTabs::CheatsAndDebug) {
					contentMinHeightOffsetFromTop += subTabBtnSize.y;

					// Grey strip behind the subtabs
					window->DrawList->AddRectFilled(
						{ wndRect.Min.x, wndRect.Min.y + 2.0f * tabBtnSize.y + tabBtnSize.y },
						{ wndRect.Max.x, wndRect.Min.y + 2.0f * tabBtnSize.y + tabBtnSize.y + subTabBtnSize.y },
						SwapColorEndianness(0x4E4E4EFF)
					);

					switch (g_UIContext.SelectedTab) {
					case UIContext::MainTabs::Options:
					{
						const float tabBarWidth = wndRect.GetSize().x;
						const float tabButtonsGap = (tabBarWidth - float(UIContext::OptionsSubTabs::Size) * subTabBtnSize.x) /
							float((size_t)UIContext::OptionsSubTabs::Size + 1);

						auto cursorPosBackup = ImGui::GetCursorScreenPos();
						ImGui::SetCursorScreenPos({ wndRect.Min.x, wndRect.Min.y + 2.0f * tabBtnSize.y + tabBtnSize.y });

						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + tabButtonsGap);
						if (TabButton("GAMEPLAY", g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::Gameplay, true, false, subTabBtnSize)) {
							g_UIContext.SelectedOptionsSubTab = UIContext::OptionsSubTabs::Gameplay;
						}

						ImGui::SameLine(0.0f, tabButtonsGap);
						if (TabButton("CAMERA", g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::Camera, true, false, subTabBtnSize)) {
							g_UIContext.SelectedOptionsSubTab = UIContext::OptionsSubTabs::Camera;
						}

						ImGui::SameLine(0.0f, tabButtonsGap);
						if (TabButton("HOTKEYS", g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::Hotkeys, true, false, subTabBtnSize)) {
							g_UIContext.SelectedOptionsSubTab = UIContext::OptionsSubTabs::Hotkeys;
						}

						ImGui::SameLine(0.0f, tabButtonsGap);
						if (TabButton("INTERFACE", g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::Interface, true, false, subTabBtnSize)) {
							g_UIContext.SelectedOptionsSubTab = UIContext::OptionsSubTabs::Interface;
						}

						ImGui::SameLine(0.0f, tabButtonsGap);
						if (TabButton("WEAPON WHEEL", g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::WeaponWheel, true, false, subTabBtnSize)) {
							g_UIContext.SelectedOptionsSubTab = UIContext::OptionsSubTabs::WeaponWheel;
						}

						ImGui::SameLine(0.0f, tabButtonsGap);
						if (TabButton("SOUND/VISUAL", g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::SoundOrVisual, true, false, subTabBtnSize)) {
							g_UIContext.SelectedOptionsSubTab = UIContext::OptionsSubTabs::SoundOrVisual;
						}

						ImGui::SameLine(0.0f, tabButtonsGap);
						if (TabButton("SYSTEM", g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::System, true, false, subTabBtnSize)) {
							g_UIContext.SelectedOptionsSubTab = UIContext::OptionsSubTabs::System;
						}

						ImGui::SetCursorScreenPos(cursorPosBackup);
					}
					break;

					case UIContext::MainTabs::CheatsAndDebug:
					{
						const float tabBarWidth = wndRect.GetSize().x;
						const float tabButtonsGap = (tabBarWidth - float(UIContext::CheatsAndDebugSubTabs::Size) * subTabBtnSize.x) /
							float((size_t)UIContext::CheatsAndDebugSubTabs::Size + 1);

						auto cursorPosBackup = ImGui::GetCursorScreenPos();
						ImGui::SetCursorScreenPos({ wndRect.Min.x, wndRect.Min.y + 2.0f * tabBtnSize.y + tabBtnSize.y });

						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + tabButtonsGap);
						if (TabButton("COMMON", g_UIContext.SelectedCheatsAndDebugSubTab == UIContext::CheatsAndDebugSubTabs::Common, true, false, subTabBtnSize)) {
							g_UIContext.SelectedCheatsAndDebugSubTab = UIContext::CheatsAndDebugSubTabs::Common;
						}

						ImGui::SameLine(0.0f, tabButtonsGap);
						if (TabButton("SPEED", g_UIContext.SelectedCheatsAndDebugSubTab == UIContext::CheatsAndDebugSubTabs::Speed, true, false, subTabBtnSize)) {
							g_UIContext.SelectedCheatsAndDebugSubTab = UIContext::CheatsAndDebugSubTabs::Speed;
						}

						ImGui::SameLine(0.0f, tabButtonsGap);
						if (TabButton("TELEPORTER", g_UIContext.SelectedCheatsAndDebugSubTab == UIContext::CheatsAndDebugSubTabs::Teleporter, true, false, subTabBtnSize)) {
							g_UIContext.SelectedCheatsAndDebugSubTab = UIContext::CheatsAndDebugSubTabs::Teleporter;
						}

						ImGui::SameLine(0.0f, tabButtonsGap);
						if (TabButton("ENEMY SPAWNER", g_UIContext.SelectedCheatsAndDebugSubTab == UIContext::CheatsAndDebugSubTabs::EnemySpawner, true, false, subTabBtnSize)) {
							g_UIContext.SelectedCheatsAndDebugSubTab = UIContext::CheatsAndDebugSubTabs::EnemySpawner;
						}

						ImGui::SameLine(0.0f, tabButtonsGap);
						if (TabButton("JUKEBOX", g_UIContext.SelectedCheatsAndDebugSubTab == UIContext::CheatsAndDebugSubTabs::JukeBox, true, false, subTabBtnSize)) {
							g_UIContext.SelectedCheatsAndDebugSubTab = UIContext::CheatsAndDebugSubTabs::JukeBox;
						}

						ImGui::SetCursorScreenPos(cursorPosBackup);
					}
					break;

					default:
						break;
					}

				}

				ImGui::PopFont();

				ImGui::PushFont(g_ImGuiFont_RussoOne[size_t(g_UIContext.DefaultFontSize * 1.15f)]);

				// Draw main tabs
				{
					const float tabBarWidth = wndRect.GetSize().x;
					const float tabButtonsGap = (tabBarWidth - float(UIContext::MainTabs::Size) * tabBtnSize.x) /
						float((size_t)UIContext::MainTabs::Size + 2);

					auto cursorPosBackup = ImGui::GetCursorScreenPos();
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f * tabBtnSize.y);

					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + tabButtonsGap);
					if (TabButton("GAME MODE", g_UIContext.SelectedTab == UIContext::MainTabs::GameMode, true, false, tabBtnSize)) {
						g_UIContext.SelectedTab = UIContext::MainTabs::GameMode;
					}

					ImGui::SameLine(0.0f, tabButtonsGap);
					if (TabButton("CHARACTER", g_UIContext.SelectedTab == UIContext::MainTabs::Character, true, false, tabBtnSize)) {
						g_UIContext.SelectedTab = UIContext::MainTabs::Character;
					}

					ImGui::SameLine(0.0f, tabButtonsGap);
					if (TabButton("QUICKPLAY", g_UIContext.SelectedTab == UIContext::MainTabs::Quickplay, true, false, tabBtnSize)) {
						g_UIContext.SelectedTab = UIContext::MainTabs::Quickplay;
					}

					ImGui::SameLine(0.0f, tabButtonsGap);
					if (TabButton("MUSIC SWITCHER", g_UIContext.SelectedTab == UIContext::MainTabs::MusicSwitcher, false, false, tabBtnSize)) {
						g_UIContext.SelectedTab = UIContext::MainTabs::MusicSwitcher;
					}

					ImGui::SameLine(0.0f, tabButtonsGap);
					if (TabButton("OPTIONS", g_UIContext.SelectedTab == UIContext::MainTabs::Options, true, true, tabBtnSize)) {
						g_UIContext.SelectedTab = UIContext::MainTabs::Options;
					}

					ImGui::SameLine(0.0f, tabButtonsGap);
					if (TabButton("CHEATS & DEBUG", g_UIContext.SelectedTab == UIContext::MainTabs::CheatsAndDebug, true, true, tabBtnSize)) {
						g_UIContext.SelectedTab = UIContext::MainTabs::CheatsAndDebug;
					}

					ImGui::SetCursorScreenPos(cursorPosBackup);
				}

				ImGui::PopFont();

				// Background faded window name text
				{
					ImVec2 pos{ wndRect.Min.x + g_UIContext.DefaultFontSize * 0.1f,
								wndRect.Min.y + (2.0f * tabBtnSize.y)/*Header Space*/ + g_UIContext.DefaultFontSize * 1.3f };

					switch (g_UIContext.SelectedTab) {
					case UIContext::MainTabs::GameMode:
					{
						window->DrawList->AddText(g_ImGuiFont_RussoOne256, g_UIContext.DefaultFontSize * 9.6f, pos,
							SwapColorEndianness(0xFFFFFF10), "Game Mode");
					}
					break;

					case UIContext::MainTabs::Character:
					{
						window->DrawList->AddText(g_ImGuiFont_RussoOne256, g_UIContext.DefaultFontSize * 9.6f, pos,
							SwapColorEndianness(0xFFFFFF10), "Character");
					}
					break;

					case UIContext::MainTabs::Quickplay:
					{
						window->DrawList->AddText(g_ImGuiFont_RussoOne256, g_UIContext.DefaultFontSize * 9.6f, pos,
							SwapColorEndianness(0xFFFFFF10), "Quickplay");
					}
					break;

					case UIContext::MainTabs::MusicSwitcher:
					{
						window->DrawList->AddText(g_ImGuiFont_RussoOne256, g_UIContext.DefaultFontSize * 9.6f, pos,
							SwapColorEndianness(0xFFFFFF10), "Music Switcher");
					}
					break;

					case UIContext::MainTabs::Options:
					{
						// Has subtab buttons
						pos += ImVec2{ 0.0f, subTabBtnSize.y };

                        if (g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::Gameplay) {
							window->DrawList->AddText(g_ImGuiFont_RussoOne256, g_UIContext.DefaultFontSize * 9.6f, pos,
								SwapColorEndianness(0xFFFFFF10), "Gameplay");
                        }
                        else if (g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::Camera) {
							window->DrawList->AddText(g_ImGuiFont_RussoOne256, g_UIContext.DefaultFontSize * 9.6f, pos,
								SwapColorEndianness(0xFFFFFF10), "Camera");
                        }
						else if (g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::Hotkeys) {
							window->DrawList->AddText(g_ImGuiFont_RussoOne256, g_UIContext.DefaultFontSize * 9.6f, pos,
								SwapColorEndianness(0xFFFFFF10), "Hotkeys");
						}
						else if (g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::Interface) {
							window->DrawList->AddText(g_ImGuiFont_RussoOne256, g_UIContext.DefaultFontSize * 9.6f, pos,
								SwapColorEndianness(0xFFFFFF10), "Interface");
						}
						else if (g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::WeaponWheel) {
							window->DrawList->AddText(g_ImGuiFont_RussoOne256, g_UIContext.DefaultFontSize * 9.6f, pos,
								SwapColorEndianness(0xFFFFFF10), "Weapon Wheel");
						}
						else if (g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::SoundOrVisual) {
							window->DrawList->AddText(g_ImGuiFont_RussoOne256, g_UIContext.DefaultFontSize * 9.6f, pos,
								SwapColorEndianness(0xFFFFFF10), "Sound/Visual");
						}
						else if (g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::System) {
							window->DrawList->AddText(g_ImGuiFont_RussoOne256, g_UIContext.DefaultFontSize * 9.6f, pos,
								SwapColorEndianness(0xFFFFFF10), "System");
						}

						
					}
					break;

					case UIContext::MainTabs::CheatsAndDebug:
					{
						// Has subtab buttons
						pos += ImVec2{ 0.0f, subTabBtnSize.y + g_UIContext.DefaultFontSize * 0.3f };


						if (g_UIContext.SelectedCheatsAndDebugSubTab == UIContext::CheatsAndDebugSubTabs::Common) {
							window->DrawList->AddText(g_ImGuiFont_RussoOne256, g_UIContext.DefaultFontSize * 9.6f, pos,
								SwapColorEndianness(0xFFFFFF10), "Common Cheats");
						}
						else if (g_UIContext.SelectedCheatsAndDebugSubTab == UIContext::CheatsAndDebugSubTabs::Speed) {
							window->DrawList->AddText(g_ImGuiFont_RussoOne256, g_UIContext.DefaultFontSize * 9.6f, pos,
								SwapColorEndianness(0xFFFFFF10), "Speed");
						}
						else if (g_UIContext.SelectedCheatsAndDebugSubTab == UIContext::CheatsAndDebugSubTabs::Teleporter) {
							window->DrawList->AddText(g_ImGuiFont_RussoOne256, g_UIContext.DefaultFontSize * 9.6f, pos,
								SwapColorEndianness(0xFFFFFF10), "Teleporter");
						}
						else if (g_UIContext.SelectedCheatsAndDebugSubTab == UIContext::CheatsAndDebugSubTabs::EnemySpawner) {
							window->DrawList->AddText(g_ImGuiFont_RussoOne256, g_UIContext.DefaultFontSize * 9.6f, pos,
								SwapColorEndianness(0xFFFFFF10), "Enemy Spawner");
						}
						else if (g_UIContext.SelectedCheatsAndDebugSubTab == UIContext::CheatsAndDebugSubTabs::JukeBox) {
							window->DrawList->AddText(g_ImGuiFont_RussoOne256, g_UIContext.DefaultFontSize * 9.6f, pos,
								SwapColorEndianness(0xFFFFFF10), "Jukebox");
						}
					}
					break;

					default:
						break;
					}
				}
			}

			// Footer of the window
			{
				// Footer background
				window->DrawList->AddRectFilled({ wndRect.Min.x, wndRect.Max.y - g_UIContext.DefaultFontSize * 2.24f }, { wndRect.Max.x, wndRect.Max.y },
					SwapColorEndianness(0x4d3A3EFF), window->WindowRounding, ImDrawCornerFlags_Bot);

				contentMaxHeightOffsetFromBottom = g_UIContext.DefaultFontSize * 2.24f;

				ImVec2 footerInfoBtnSize{ 0.0f, g_UIContext.DefaultFontSize * 1.7f };

				// Left footer section
				{
					constexpr auto PATREON_BUTTON_TEXT = "PATREON";

					float aboutButtonWidth = ImGui::CalcTextSize(PATREON_BUTTON_TEXT).x + style.FramePadding.x * 2.0f;

					ImVec2 pos{ wndRect.Min.x + g_UIContext.DefaultFontSize * 1.0f, wndRect.Max.y - g_UIContext.DefaultFontSize * 1.68f };

					auto cursorPosBackup = ImGui::GetCursorScreenPos();

					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 7.0f);
					ImGui::PushFont(g_ImGuiFont_RussoOne[g_UIContext.DefaultFontSize]);

					ImGui::SetCursorScreenPos(pos + ImVec2{ 0.0f, g_UIContext.DefaultFontSize * 0.5f - footerInfoBtnSize.y * 0.5f });
					if (InfoButton(PATREON_BUTTON_TEXT, footerInfoBtnSize)) {
						ShellExecute(0, 0, "https://www.patreon.com/miaberth", 0, 0, SW_SHOW);
					}

					ImGui::PopFont();
					ImGui::PopStyleVar();

					ImGui::SetCursorScreenPos(cursorPosBackup);
				}

				// Middle footer section
				{
					constexpr auto BACKGROUND_FADED_TEXT = u8"C•Team";
					constexpr auto CREDIT_TEXT = u8"Mia Berth • SSSiyan • deepdarkkapustka • Darkness   ";
					constexpr auto ABOUT_BUTTON_TEXT = "ABOUT";

					ImGui::PushFont(g_ImGuiFont_Roboto[g_UIContext.DefaultFontSize]);

					float creditTextWidth = ImGui::CalcTextSize((const char*)CREDIT_TEXT).x;
					float aboutButtonWidth = ImGui::CalcTextSize(ABOUT_BUTTON_TEXT).x + style.FramePadding.x * 2.0f;

					ImVec2 pos{ (wndRect.Min.x + wndRect.Max.x) * 0.5f - (creditTextWidth + aboutButtonWidth) * 0.5f, wndRect.Max.y - g_UIContext.DefaultFontSize * 1.68f };

					// Background faded text
					ImVec2 bgFadedTextSize = ImGui::CalcTextSize((const char*)BACKGROUND_FADED_TEXT);
					window->DrawList->AddText(g_ImGuiFont_RussoOne256, g_UIContext.DefaultFontSize * 4.8f,
						{ pos.x - g_UIContext.DefaultFontSize * 5.8f,
						pos.y - g_UIContext.DefaultFontSize * 4.8f * 0.5f },
						SwapColorEndianness(0xFFFFFF10), (const char*)BACKGROUND_FADED_TEXT);

					window->DrawList->AddText(pos, SwapColorEndianness(0xFFFFFFFF), (const char*)CREDIT_TEXT);

					ImGui::PopFont();

					auto cursorPosBackup = ImGui::GetCursorScreenPos();

					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 7.0f);
					ImGui::PushFont(g_ImGuiFont_RussoOne[g_UIContext.DefaultFontSize]);

					ImGui::SetCursorScreenPos(pos + ImVec2{ creditTextWidth, g_UIContext.DefaultFontSize * 0.5f - footerInfoBtnSize.y * 0.5f });
					if (InfoButton(ABOUT_BUTTON_TEXT, footerInfoBtnSize)) {
						g_UIContext.SelectedTab = UIContext::MainTabs::None;
					}

					ImGui::PopFont();
					ImGui::PopStyleVar();

					ImGui::SetCursorScreenPos(cursorPosBackup);
				}

				// Right footer section
				{
					constexpr auto CREDIT_TEXT = "Based on serpentiem's ";
					constexpr auto DDMK_BUTTON_TEXT = "DDMK";

					ImGui::PushFont(g_ImGuiFont_Roboto[g_UIContext.DefaultFontSize]);

					float creditTextWidth = ImGui::CalcTextSize(CREDIT_TEXT).x;
					float aboutBtnWidth = ImGui::CalcTextSize(DDMK_BUTTON_TEXT).x + style.FramePadding.x * 2.0f;

					ImVec2 pos{ wndRect.Max.x - creditTextWidth - aboutBtnWidth - g_UIContext.DefaultFontSize, wndRect.Max.y - g_UIContext.DefaultFontSize * 1.68f };

					window->DrawList->AddText(pos, SwapColorEndianness(0xFFFFFFFF), CREDIT_TEXT);

					ImGui::PopFont();

					auto cursorPosBackup = ImGui::GetCursorScreenPos();

					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 7.0f);
					ImGui::PushFont(g_ImGuiFont_RussoOne[g_UIContext.DefaultFontSize]);

					ImGui::SetCursorScreenPos(pos + ImVec2{ creditTextWidth, g_UIContext.DefaultFontSize * 0.5f - footerInfoBtnSize.y * 0.5f });
					if (InfoButton(DDMK_BUTTON_TEXT, footerInfoBtnSize)) {
						ShellExecute(0, 0, "https://github.com/serpentiem/ddmk/", 0, 0, SW_SHOW);
					}

					ImGui::PopFont();
					ImGui::PopStyleVar();

					ImGui::SetCursorScreenPos(cursorPosBackup);
				}
			}

			// Actual content
			{
				ImVec2 contentMin = { wndRect.Min.x + g_UIContext.DefaultFontSize * 0.3f,
									  wndRect.Min.y + contentMinHeightOffsetFromTop + g_UIContext.DefaultFontSize * 0.3f };

				ImGui::SetNextWindowPos(contentMin, ImGuiCond_Always);
				ImGui::BeginChildEx("Content Window", window->GetID("Content Window"),
					window->Size - ImVec2{ g_UIContext.DefaultFontSize * 0.3f * 2.0f, (contentMinHeightOffsetFromTop + g_UIContext.DefaultFontSize * 0.3f + contentMaxHeightOffsetFromBottom) }, false, 0);
				DrawMainContent(pDevice, g_UIContext);
				ImGui::EndChild();
			}
		}
		ImGui::End();
		ImGui::PopFont();
	}
}

bool visibleMain     = false;
bool lastVisibleMain = false;

bool visibleShop     = false;
bool lastVisibleShop = false;

bool updateWeaponWheel = false;

// siy stuff to be moved //
static const std::vector<std::pair<uint16_t, const char*>> buttonPairs = {{0x0000, "Nothing"}, {0x0001, "Left Trigger"},
    {0x0002, "Right Trigger"}, {0x0004, "Left Shoulder"}, {0x0008, "Right Shoulder"}, {0x0010, "Y"}, {0x0020, "B"}, {0x0040, "A"},
    {0x0080, "X"}, {0x0100, "Back"}, {0x0200, "Left Thumb"}, {0x0400, "Right Thumb"}, {0x0800, "Start"}, {0x1000, "Up"}, {0x2000, "Right"},
    {0x4000, "Down"}, {0x8000, "Left"}};

std::pair<uint16_t, const char*> getButtonInfo(uint16_t buttonNum) {
    for (const auto& pair : buttonPairs) {
        if (pair.first == buttonNum) {
            return pair;
        }
    }
    return buttonPairs[0];
}

void DrawButtonCombo(const char* label, uint16_t& currentButton) {
    if (ImGui::BeginCombo(label, getButtonInfo(currentButton).second)) {
        for (const auto& buttonPair : buttonPairs) {
            bool is_selected = (currentButton == buttonPair.first);
            if (ImGui::Selectable(buttonPair.second, is_selected)) {
                currentButton = buttonPair.first;
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}
// siy stuff over //

#pragma region Common

const char* saveNamesDante[] = {
    "Dante 0000",
    "Dante 0001",
    "Dante 0002",
    "Dante 0003",
    "Dante 0004",
    "Dante 0005",
    "Dante 0006",
    "Dante 0007",
    "Dante 0008",
    "Dante 0009",
};


const char* saveNamesVergil[] = {
    "Vergil 0000",
    "Vergil 0001",
    "Vergil 0002",
    "Vergil 0003",
    "Vergil 0004",
    "Vergil 0005",
    "Vergil 0006",
    "Vergil 0007",
    "Vergil 0008",
    "Vergil 0009",
};


const char* indexNames[] = {
    "0000",
    "0001",
    "0002",
    "0003",
    "0004",
    "0005",
    "0006",
    "0007",
    "0008",
    "0009",
    "0010",
    "0011",
    "0012",
    "0013",
    "0014",
    "0015",
    "0016",
    "0017",
    "0018",
    "0019",
    "0020",
    "0021",
    "0022",
    "0023",
    "0024",
    "0025",
    "0026",
    "0027",
    "0028",
    "0029",
    "0030",
    "0031",
    "0032",
    "0033",
    "0034",
    "0035",
    "0036",
    "0037",
    "0038",
    "0039",
    "0040",
    "0041",
    "0042",
    "0043",
    "0044",
    "0045",
    "0046",
    "0047",
    "0048",
    "0049",
    "0050",
    "0051",
    "0052",
    "0053",
    "0054",
    "0055",
    "0056",
    "0057",
    "0058",
    "0059",
    "0060",
    "0061",
    "0062",
    "0063",
    "0064",
    "0065",
    "0066",
    "0067",
    "0068",
    "0069",
    "0070",
    "0071",
    "0072",
    "0073",
    "0074",
    "0075",
    "0076",
    "0077",
    "0078",
    "0079",
    "0080",
    "0081",
    "0082",
    "0083",
    "0084",
    "0085",
    "0086",
    "0087",
    "0088",
    "0089",
    "0090",
    "0091",
    "0092",
    "0093",
    "0094",
    "0095",
    "0096",
    "0097",
    "0098",
    "0099",
};


const char* buttonIndexNames[] = {
    "Button 1",
    "Button 2",
    "Button 3",
    "Button 4",
};

const char* dataNames[] = {
    "Data 0",
    "Data 1",
    "Data 2",
    "Data 3",
    "Data 4",
    "Data 5",
    "Data 6",
    "Data 7",
};

const char* buttonNames[] = {
    "Nothing",
    "Left Trigger",
    "Right Trigger",
    "Left Shoulder",
    "Right Shoulder",
    "Y",
    "B",
    "A",
    "X",
    "Back",
    "Left Thumb",
    "Right Thumb",
    "Start",
    "Up",
    "Right",
    "Down",
    "Left",
};

constexpr byte16 buttons[] = {
    0,
    GAMEPAD::LEFT_TRIGGER,
    GAMEPAD::RIGHT_TRIGGER,
    GAMEPAD::LEFT_SHOULDER,
    GAMEPAD::RIGHT_SHOULDER,
    GAMEPAD::Y,
    GAMEPAD::B,
    GAMEPAD::A,
    GAMEPAD::X,
    GAMEPAD::BACK,
    GAMEPAD::LEFT_THUMB,
    GAMEPAD::RIGHT_THUMB,
    GAMEPAD::START,
    GAMEPAD::UP,
    GAMEPAD::RIGHT,
    GAMEPAD::DOWN,
    GAMEPAD::LEFT,
};

const char* directionNames[] = {
    "Up",
    "Right",
    "Down",
    "Left",
};

const char* playerIndexNames[] = {
    "1P",
    "2P",
    "3P",
    "4P",
};


const char* characterIndexNames[] = {
    "Character 1",
    "Character 2",
    "Character 3",
};

const char* entityNames[] = {
    "Main",
    "Clone",
};

const char* entityEnumNames[] = {
    "ENTITY::MAIN",
    "ENTITY::CLONE",
};

const char* characterNames[] = {
    "Dante",
    "Bob",
    "Lady",
    "Vergil",
};


const char* costumeRespectsProgressionNames[] = {
    "Off",
    "Original (Vanilla Default)",
    "Crimson",
};

const char* cameraSensitivityNames[] = {
    "Low (Vanilla Default)",
    "Medium",
    "High",
    "Highest",
};

const char* cameraFollowUpSpeedNames[] = {
    "Low (Vanilla Default)",
    "Medium",
    "High",
};

const char* cameraDistanceNames[] = {
    "Far (Vanilla Default)",
    "Closer",
    "Dynamic",
};

const char* cameraLockOnDistanceNames[] = {
    "Closer (Vanilla Default)",
    "Far",
    "Dynamic",
};


const char* cameraTiltNames[] = {
    "Original (Vanilla Default)",
    "Closer to Ground",
};

const char* cameraSmoothingNames[] = {
    "Highest",
    "High (Vanilla Default)",
    "Medium",
    "Low",
};

const char* GUITransparencyNames[] = {
	"Off",
	"Static",
	"Dynamic",
};


const char* newCharacterNames[] = {
    "Dante",
    "Bob",
    "Lady",
    "Vergil",
    "Boss Lady",
    "Boss Vergil",
};

constexpr uint8 newCharacters[] = {
    CHARACTER::DANTE,
    CHARACTER::BOB,
    CHARACTER::LADY,
    CHARACTER::VERGIL,
    CHARACTER::BOSS_LADY,
    CHARACTER::BOSS_VERGIL,
};

const char* collisionGroupNames[] = {"Player", "Enemy"};


constexpr uint8 collisionGroups[] = {
    COLLISION_GROUP::PLAYER,
    COLLISION_GROUP::ENEMY,
};

const char* styleNamesDante[] = {
    "Swordmaster",
    "Gunslinger",
    "Trickster",
    "Royalguard",
    "Quicksilver",
    "Doppelganger",
};

const char* styleNamesFX[] = {
    "Trick",
    "Sword",
    "Gun",
    "Royal",
    "Quick",
    "Dopp",
    "DT",
    "DTE",
    "Ready!"
};

constexpr uint8 stylesDante[] = {
    STYLE::SWORDMASTER,
    STYLE::GUNSLINGER,
    STYLE::TRICKSTER,
    STYLE::ROYALGUARD,
    STYLE::QUICKSILVER,
    STYLE::DOPPELGANGER,
};

const char* styleNamesVergil[] = {
    "Dark Slayer",
    "Quicksilver",
    "Doppelganger",
};

const char* styleNamesDanteGameplay[] = {
	"SWORDMASTER",
	"GUNSLINGER",
	"TRICKSTER",
	"ROYALGUARD",
	"QUICKSILVER",
	"DOPPELGANGER",
};

const char* styleNamesVergilGameplay[] = {
	"PLACEHOLDER",
	"PLACEHOLDER",
	"DARK SLAYER",
    "PLACEHOLDER",
    "DOPPELGANGER",
    "QUICKSILVER",
};

constexpr uint8 stylesVergil[] = {
    STYLE::DARK_SLAYER,
    STYLE::QUICKSILVER,
    STYLE::DOPPELGANGER,
};

const char* meleeWeaponNamesDante[] = {
    "Rebellion",
    "Cerberus",
    "Agni & Rudra",
    "Nevan",
    "Beowulf",
};

constexpr uint8 meleeWeaponsDante[] = {
    WEAPON::REBELLION,
    WEAPON::CERBERUS,
    WEAPON::AGNI_RUDRA,
    WEAPON::NEVAN,
    WEAPON::BEOWULF_DANTE,
};

const char* meleeWeaponNamesVergil[] = {
    "Yamato",
    "Beowulf",
    "Yamato & Force Edge",
};

constexpr uint8 meleeWeaponsVergil[] = {
    WEAPON::YAMATO_VERGIL,
    WEAPON::BEOWULF_VERGIL,
    WEAPON::YAMATO_FORCE_EDGE,
};

const char* rangedWeaponNamesDante[] = {
    "Ebony & Ivory",
    "Shotgun",
    "Artemis",
    "Spiral",
    "Kalina Ann",
};

constexpr uint8 rangedWeaponsDante[] = {
    WEAPON::EBONY_IVORY,
    WEAPON::SHOTGUN,
    WEAPON::ARTEMIS,
    WEAPON::SPIRAL,
    WEAPON::KALINA_ANN,
};

const char* weaponSwitchTypeNames[] = {
    "Linear",
    "Arbitrary",
};

const char* stickNames[] = {
    "Right Stick",
    "Left Stick",
};

const char* styleRankNames[] = {
    "None",
    "Dope",
    "Crazy",
    "Blast",
    "Alright",
    "Sweet",
    "Showtime",
    "Stylish",
};

const char* missionNames[] = {
    "Movie",
    "Mission 1",
    "Mission 2",
    "Mission 3",
    "Mission 4",
    "Mission 5",
    "Mission 6",
    "Mission 7",
    "Mission 8",
    "Mission 9",
    "Mission 10",
    "Mission 11",
    "Mission 12",
    "Mission 13",
    "Mission 14",
    "Mission 15",
    "Mission 16",
    "Mission 17",
    "Mission 18",
    "Mission 19",
    "Mission 20",
    "Bloody Palace",
};

const char* modeNames[] = {
    "Easy",
    "Normal",
    "Hard",
    "Very Hard",
    "Dante Must Die",
    "Heaven or Hell",
};

constexpr uint32 modes[] = {
    MODE::EASY,
    MODE::NORMAL,
    MODE::HARD,
    MODE::VERY_HARD,
    MODE::DANTE_MUST_DIE,
    MODE::HEAVEN_OR_HELL,
};

const char* floorNames[] = {
    "Floor 1",
    "Floor 2",
    "Floor 3",
    "Floor 4",
    "Floor 5",
    "Floor 6",
    "Floor 7",
    "Floor 8",
    "Floor 9",
    "Floor 10",
    "Cerberus",
    "Gigapede",
    "Agni & Rudra",
    "Nevan",
    "Beowulf",
    "Geryon",
    "Doppelganger",
    "Leviathan",
    "Damned Chessmen",
    "Vergil 1",
    "Vergil 2",
    "Vergil 3",
    "Lady",
    "Arkham",
    "Jester 1",
    "Jester 2",
    "Jester 3",
};

const char* sceneNames[] = {
    "SCENE::BOOT",
    "SCENE::INTRO",
    "SCENE::MAIN",
    "SCENE::MISSION_SELECT",
    "SCENE::LOAD",
    "SCENE::GAME",
    "SCENE::CUTSCENE",
    "SCENE::MISSION_START",
    "SCENE::MISSION_RESULT",
    "SCENE::GAME_OVER",
};

const char* eventNames[] = {
    "EVENT::INIT",
    "EVENT::MAIN",
    "EVENT::TELEPORT",
    "EVENT::PAUSE",
    "EVENT::STATUS",
    "EVENT::OPTIONS",
    "EVENT::DEATH",
    "EVENT::ITEM",
    "EVENT::MESSAGE",
    "EVENT::CUSTOMIZE",
    "EVENT::SAVE",
    "EVENT::DELETE",
    "EVENT::END",
};

const char* trackFilenames[] = {
    "Battle_00.ogg",
    "Battle_01.ogg",
    "Battle_01b.ogg",
    "Battle_02.ogg",
    "Battle_03.ogg",
    "Battle_04.ogg",
    "Battle_05.ogg",
    "Battle_06.ogg",
    "Battle_07.ogg",
    "Battle_08.ogg",
    "Battle_0a.ogg",
    "Boss_01.ogg",
    "Boss_01b.ogg",
    "Boss_02.ogg",
    "Boss_02b.ogg",
    "Boss_03.ogg",
    "Boss_03b.ogg",
    "Boss_04.ogg",
    "Boss_04b.ogg",
    "Boss_05.ogg",
    "Boss_05b.ogg",
    "Boss_06.ogg",
    "Boss_06b.ogg",
    "Boss_07.ogg",
    "Boss_08.ogg",
    "Boss_08b.ogg",
    "Boss_09.ogg",
    "Continue.ogg",
    "dummy.ogg",
    "Hine_01.ogg",
    "Hine_02.ogg",
    "Jester.ogg",
    "Jikushinzou.ogg",
    "Kaze_01.ogg",
    "Kaze_02.ogg",
    "Kaze_03.ogg",
    "Kaze_04.ogg",
    "Kaze_05.ogg",
    "Kaze_06.ogg",
    "Lady.ogg",
    "m01_c00.ogg",
    "m01_v00.ogg",
    "m01_v01.ogg",
    "m01_v02.ogg",
    "m01_v03.ogg",
    "m01_v04.ogg",
    "m01_v05.ogg",
    "m01_v06.ogg",
    "m01_v07.ogg",
    "m01_v08.ogg",
    "m01_v09.ogg",
    "m02_c01.ogg",
    "m02_s00.ogg",
    "m03_b00.ogg",
    "m03_b01.ogg",
    "m03_c00.ogg",
    "m03_s00.ogg",
    "m03_s01.ogg",
    "m04_c00.ogg",
    "m04_s00.ogg",
    "m05_b00.ogg",
    "m05_c00.ogg",
    "m05_s00.ogg",
    "m06_c00.ogg",
    "m07_b00.ogg",
    "m07_s00.ogg",
    "m08_c00.ogg",
    "m09_b00.ogg",
    "m09_b01.ogg",
    "m09_c00.ogg",
    "m10_c00.ogg",
    "m10_s00.ogg",
    "m11_b00.ogg",
    "m11_b01.ogg",
    "m11_c00.ogg",
    "m11_s00.ogg",
    "m12_b00.ogg",
    "m12_b01.ogg",
    "m12_b02.ogg",
    "m12_c00.ogg",
    "m12_s00.ogg",
    "m13_b00.ogg",
    "m13_s00.ogg",
    "m14_s01.ogg",
    "m15_c00.ogg",
    "m15_s00.ogg",
    "m16_b00.ogg",
    "m16_c00.ogg",
    "m16_s00.ogg",
    "m17_b00.ogg",
    "m17_c00.ogg",
    "m17_s00.ogg",
    "m18_c00.ogg",
    "m18_s00.ogg",
    "m19_b00.ogg",
    "m19_b01.ogg",
    "m19_s00.ogg",
    "m20_b00.ogg",
    "m20_c00.ogg",
    "m20_s00.ogg",
    "m99_c00.ogg",
    "m99_c01.ogg",
    "m99_c03.ogg",
    "Maguma.ogg",
    "Mizu_01.ogg",
    "Mizu_02.ogg",
    "M_clear.ogg",
    "m_demo_02_001.ogg",
    "m_demo_04_004.ogg",
    "m_demo_04_004v.ogg",
    "m_demo_04_014.ogg",
    "m_demo_06_007.ogg",
    "m_demo_06_011.ogg",
    "m_demo_06_012.ogg",
    "m_demo_09_003.ogg",
    "m_demo_09_003v.ogg",
    "m_demo_10_002.ogg",
    "m_demo_10_002v.ogg",
    "m_demo_10_004.ogg",
    "m_demo_10_005.ogg",
    "m_demo_12_004.ogg",
    "m_demo_12_005.ogg",
    "m_demo_15_001.ogg",
    "m_demo_15_001v.ogg",
    "m_demo_19_001.ogg",
    "m_demo_19_016.ogg",
    "M_start.ogg",
    "M_start2.ogg",
    "Nausica.ogg",
    "Option.ogg",
    "Result.ogg",
    "Room_01.ogg",
    "Sm_clear.ogg",
    "Sm_failure.ogg",
    "Staffroll.ogg",
    "Stage_01.ogg",
    "Stage_02.ogg",
    "Stage_02b.ogg",
    "Stage_03.ogg",
    "Stage_04.ogg",
    "Stage_04b.ogg",
    "Stage_05.ogg",
    "Stage_05b.ogg",
    "Stage_06.ogg",
    "Stage_07.ogg",
    "Stage_08.ogg",
    "Stage_09.ogg",
    "Torocco.ogg",
    "T_boss.ogg",
    "T_bossb.ogg",
    "Versil_01.ogg",
    "Versil_02.ogg",
    "Versil_03.ogg",
    "ZZZ.ogg",
};

const char* trackNames[] = {
    "Battle_00",
    "Battle_01",
    "Battle_01b",
    "Battle_02",
    "Battle_03",
    "Battle_04",
    "Battle_05",
    "Battle_06",
    "Battle_07",
    "Battle_08",
    "Battle_0a",
    "Boss_01",
    "Boss_01b",
    "Boss_02",
    "Boss_02b",
    "Boss_03",
    "Boss_03b",
    "Boss_04",
    "Boss_04b",
    "Boss_05",
    "Boss_05b",
    "Boss_06",
    "Boss_06b",
    "Boss_07",
    "Boss_08",
    "Boss_08b",
    "Boss_09",
    "Continue",
    "dummy",
    "Hine_01",
    "Hine_02",
    "Jester",
    "Jikushinzou",
    "Kaze_01",
    "Kaze_02",
    "Kaze_03",
    "Kaze_04",
    "Kaze_05",
    "Kaze_06",
    "Lady",
    "m01_c00",
    "m01_v00",
    "m01_v01",
    "m01_v02",
    "m01_v03",
    "m01_v04",
    "m01_v05",
    "m01_v06",
    "m01_v07",
    "m01_v08",
    "m01_v09",
    "m02_c01",
    "m02_s00",
    "m03_b00",
    "m03_b01",
    "m03_c00",
    "m03_s00",
    "m03_s01",
    "m04_c00",
    "m04_s00",
    "m05_b00",
    "m05_c00",
    "m05_s00",
    "m06_c00",
    "m07_b00",
    "m07_s00",
    "m08_c00",
    "m09_b00",
    "m09_b01",
    "m09_c00",
    "m10_c00",
    "m10_s00",
    "m11_b00",
    "m11_b01",
    "m11_c00",
    "m11_s00",
    "m12_b00",
    "m12_b01",
    "m12_b02",
    "m12_c00",
    "m12_s00",
    "m13_b00",
    "m13_s00",
    "m14_s01",
    "m15_c00",
    "m15_s00",
    "m16_b00",
    "m16_c00",
    "m16_s00",
    "m17_b00",
    "m17_c00",
    "m17_s00",
    "m18_c00",
    "m18_s00",
    "m19_b00",
    "m19_b01",
    "m19_s00",
    "m20_b00",
    "m20_c00",
    "m20_s00",
    "m99_c00",
    "m99_c01",
    "m99_c03",
    "Maguma",
    "Mizu_01",
    "Mizu_02",
    "M_clear",
    "m_demo_02_001",
    "m_demo_04_004",
    "m_demo_04_004v",
    "m_demo_04_014",
    "m_demo_06_007",
    "m_demo_06_011",
    "m_demo_06_012",
    "m_demo_09_003",
    "m_demo_09_003v",
    "m_demo_10_002",
    "m_demo_10_002v",
    "m_demo_10_004",
    "m_demo_10_005",
    "m_demo_12_004",
    "m_demo_12_005",
    "m_demo_15_001",
    "m_demo_15_001v",
    "m_demo_19_001",
    "m_demo_19_016",
    "M_start",
    "M_start2",
    "Nausica",
    "Option",
    "Result",
    "Room_01",
    "Sm_clear",
    "Sm_failure",
    "Staffroll",
    "Stage_01",
    "Stage_02",
    "Stage_02b",
    "Stage_03",
    "Stage_04",
    "Stage_04b",
    "Stage_05",
    "Stage_05b",
    "Stage_06",
    "Stage_07",
    "Stage_08",
    "Stage_09",
    "Torocco",
    "T_boss",
    "T_bossb",
    "Versil_01",
    "Versil_02",
    "Versil_03",
    "ZZZ",
};

static_assert(countof(trackFilenames) == countof(trackNames));

#pragma endregion

void PauseWhenGUIOpen() {
    auto pool_10298 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
    if (!pool_10298 || !pool_10298[8]) {
        return;
    }
    auto& eventData = *reinterpret_cast<EventData*>(pool_10298[8]);

  

    if (g_scene != SCENE::GAME || eventData.event != EVENT::MAIN) {
        guiPause.timer    = 1.0f;
        guiPause.canPause = false;
        g_inGame = false;
    } else {
        g_inGame = true;
        
        if (guiPause.timer > 0) {
            guiPause.timer -= ImGui::GetIO().DeltaTime;
        }
    }


    if (guiPause.timer <= 0) {
        guiPause.canPause = true;
    }

    

    if (!g_show || !guiPause.canPause) {
        activeConfig.Speed.mainSpeed = queuedConfig.Speed.mainSpeed;
        activeConfig.Speed.turbo     = queuedConfig.Speed.turbo;
        Speed::Toggle(true);
        Speed::Toggle(false);
        guiPause.in = true;

    } else if (g_show && guiPause.in && guiPause.canPause) {
        activeConfig.Speed.mainSpeed = 0;
        activeConfig.Speed.turbo     = 0;
        Speed::Toggle(true);
        Speed::Toggle(false);
        guiPause.in = false;
    }
}

void GameTrackDetection() {
    g_gameTrackPlaying = (std::string)reinterpret_cast<char*>(appBaseAddr + 0xD23906);    
}

void CorrectFrameRateCutscenes() {
    // Changing frame rate to above or below 60 will alter cutscene speed, this function corrects this behavior
    // by forcing cutscenes to play at 60 fps. - Mia

    float temp = queuedConfig.frameRate;

    if (g_scene == SCENE::CUTSCENE && !changedFrameRateCorrection) {
        activeConfig.frameRate = 60.0;

        UpdateFrameRate();
        changedFrameRateCorrection = true;
    }

    if (g_scene != SCENE::CUTSCENE && changedFrameRateCorrection) {
        activeConfig.frameRate = temp;

        UpdateFrameRate();
        changedFrameRateCorrection = false;
    }
}


#pragma region Texture

struct TextureData {
    bool enable;
    bool run;
    const char* label;
    ID3D11ShaderResourceView* textureAddr;

    uint32 lastX;
    uint32 lastY;

    void Render(Config::TextureData& activeData, Config::TextureData& queuedData);
    void Settings(Config::TextureData& activeData, Config::TextureData& queuedData);
    void SetPosition(Config::TextureData& data, bool ranged);
};

void TextureData::Render(Config::TextureData& activeData, Config::TextureData& queuedData) {
    if (!enable || !textureAddr) {
        return;
    }

    auto& activeSize = *reinterpret_cast<ImVec2*>(&activeData.size);
    auto& queuedSize = *reinterpret_cast<ImVec2*>(&queuedData.size);

    auto& activePos = *reinterpret_cast<ImVec2*>(&activeData.pos);
    auto& queuedPos = *reinterpret_cast<ImVec2*>(&queuedData.pos);

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

    if (ImGui::Begin(label, &enable, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
        activePos = queuedPos = ImGui::GetWindowPos();

        uint32 x = static_cast<uint32>(activeData.pos.x);
        uint32 y = static_cast<uint32>(activeData.pos.y);

        if ((lastX != x) || (lastY != y)) {
            lastX = x;
            lastY = y;

            GUI::save = true;
        }

        ImGui::Image(textureAddr, activeSize);
    }

    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(4);
}

void TextureData::Settings(Config::TextureData& activeData, Config::TextureData& queuedData) {
    auto& activeSize = *reinterpret_cast<ImVec2*>(&activeData.size);
    auto& queuedSize = *reinterpret_cast<ImVec2*>(&queuedData.size);

    auto& activePos = *reinterpret_cast<ImVec2*>(&activeData.pos);
    auto& queuedPos = *reinterpret_cast<ImVec2*>(&queuedData.pos);

    ImGui::Text(label);

    if constexpr (debug) {
        GUI_Checkbox("Enable", enable);
    }

    ImGui::PushItemWidth(150);

    GUI_Input2<float>("Width", activeSize.x, queuedSize.x, 1, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
    GUI_Input2<float>("Height", activeSize.y, queuedSize.y, 1, "%g", ImGuiInputTextFlags_EnterReturnsTrue);

    if (GUI_Input2<float>("X", activePos.x, queuedPos.x, 1, "%g", ImGuiInputTextFlags_EnterReturnsTrue)) {
        ImGui::SetWindowPos(label, activePos);
    }
    if (GUI_Input2<float>("Y", activePos.y, queuedPos.y, 1, "%g", ImGuiInputTextFlags_EnterReturnsTrue)) {
        ImGui::SetWindowPos(label, activePos);
    }

    ImGui::PopItemWidth();
}

void TextureData::SetPosition(Config::TextureData& data, bool ranged) {
    auto& pos = *reinterpret_cast<ImVec2*>(&data.pos);

    if (!ranged) {
        ImGui::SetWindowPos(label, ImVec2(pos.x + activeConfig.weaponWheelHorizontalMelee, pos.y + activeConfig.weaponWheelHeight));
    } else {
        ImGui::SetWindowPos(label, ImVec2(pos.x + activeConfig.weaponWheelHorizontalRanged, pos.y + activeConfig.weaponWheelHeight));
    }
}

enum {
    TEXTURE_ARROW_0,
    TEXTURE_ARROW_72,
    TEXTURE_ARROW_144,
    TEXTURE_ARROW_216,
    TEXTURE_ARROW_288,
    TEXTURE_BACKGROUND,
    TEXTURE_HIGHLIGHT,
    // TEXTURE_STENCIL,
    TEXTURE_WEAPON_REBELLION,
    TEXTURE_WEAPON_REBELLION_2,
    TEXTURE_WEAPON_FORCE_EDGE,
    TEXTURE_WEAPON_CERBERUS,
    TEXTURE_WEAPON_AGNI_RUDRA,
    TEXTURE_WEAPON_NEVAN,
    TEXTURE_WEAPON_BEOWULF_DANTE,
    TEXTURE_WEAPON_EBONY_IVORY,
    TEXTURE_WEAPON_SHOTGUN,
    TEXTURE_WEAPON_ARTEMIS,
    TEXTURE_WEAPON_SPIRAL,
    TEXTURE_WEAPON_KALINA_ANN,
    TEXTURE_WEAPON_YAMATO_VERGIL,
    TEXTURE_WEAPON_YAMATO_NERO_ANGELO,
    TEXTURE_WEAPON_BEOWULF_VERGIL,
    TEXTURE_WEAPON_BEOWULF_NERO_ANGELO,
    TEXTURE_WEAPON_YAMATO_FORCE_EDGE,
    TEXTURE_COUNT,
};

const char* textureFilenames[] = {
    "arrow_0.png",
    "arrow_72.png",
    "arrow_144.png",
    "arrow_216.png",
    "arrow_288.png",
    "background.png",
    "highlight.png",
    //"stencil.png",
    "weapon_0000_0000.png",
    "weapon_0000_0001.png",
    "weapon_0000_0002.png",
    "weapon_0001.png",
    "weapon_0002.png",
    "weapon_0003.png",
    "weapon_0004.png",
    "weapon_0005.png",
    "weapon_0006.png",
    "weapon_0007.png",
    "weapon_0008.png",
    "weapon_0009.png",
    "weapon_0011_0000.png",
    "weapon_0011_0001.png",
    "weapon_0012_0000.png",
    "weapon_0012_0001.png",
    "weapon_0013.png",
};

ID3D11ShaderResourceView* textureAddrs[TEXTURE_COUNT] = {};

constexpr uint8 textureWeaponMap[WEAPON::MAX] = {
    TEXTURE_WEAPON_REBELLION,
    TEXTURE_WEAPON_CERBERUS,
    TEXTURE_WEAPON_AGNI_RUDRA,
    TEXTURE_WEAPON_NEVAN,
    TEXTURE_WEAPON_BEOWULF_DANTE,
    TEXTURE_WEAPON_EBONY_IVORY,
    TEXTURE_WEAPON_SHOTGUN,
    TEXTURE_WEAPON_ARTEMIS,
    TEXTURE_WEAPON_SPIRAL,
    TEXTURE_WEAPON_KALINA_ANN,
    0,
    TEXTURE_WEAPON_YAMATO_VERGIL,
    TEXTURE_WEAPON_BEOWULF_VERGIL,
    TEXTURE_WEAPON_YAMATO_FORCE_EDGE,
    0,
};

constexpr uint8 textureArrowMap[5] = {
    TEXTURE_ARROW_0,
    TEXTURE_ARROW_288,
    TEXTURE_ARROW_216,
    TEXTURE_ARROW_144,
    TEXTURE_ARROW_72,
};

bool g_showTextures = false;

void CreateTexturesWeaponWheel() {
    char path[128];

    old_for_all(uint8, textureIndex, TEXTURE_COUNT) {
        snprintf(path, sizeof(path), ((std::string)Paths::weaponwheel + "/%s").c_str(), textureFilenames[textureIndex]);

        textureAddrs[textureIndex] = CreateTexture(path, ::D3D11::device);
    }
}

// TextureData stencil = {};

#pragma endregion

#pragma region Weapon Switch Controllers

struct WeaponSwitchControllerTextureData {
    TextureData backgrounds[5];
    TextureData highlights[5];
    TextureData icons[5];
    TextureData arrow;
};

WeaponSwitchControllerTextureData meleeWeaponSwitchControllerTextureData = {};

const char* meleeWeaponSwitchControllerBackgroundNames[5] = {
    "MeleeWeaponSwitchControllerBackground0",
    "MeleeWeaponSwitchControllerBackground1",
    "MeleeWeaponSwitchControllerBackground2",
    "MeleeWeaponSwitchControllerBackground3",
    "MeleeWeaponSwitchControllerBackground4",
};


const char* meleeWeaponSwitchControllerHighlightNames[5] = {
    "MeleeWeaponSwitchControllerHighlight0",
    "MeleeWeaponSwitchControllerHighlight1",
    "MeleeWeaponSwitchControllerHighlight2",
    "MeleeWeaponSwitchControllerHighlight3",
    "MeleeWeaponSwitchControllerHighlight4",
};

const char* meleeWeaponSwitchControllerIconNames[5] = {
    "MeleeWeaponSwitchControllerIcon0",
    "MeleeWeaponSwitchControllerIcon1",
    "MeleeWeaponSwitchControllerIcon2",
    "MeleeWeaponSwitchControllerIcon3",
    "MeleeWeaponSwitchControllerIcon4",
};

const char* meleeWeaponSwitchControllerArrowName = "MeleeWeaponSwitchControllerArrow";


WeaponSwitchControllerTextureData rangedWeaponSwitchControllerTextureData = {};

const char* rangedWeaponSwitchControllerBackgroundNames[5] = {
    "RangedWeaponSwitchControllerBackground0",
    "RangedWeaponSwitchControllerBackground1",
    "RangedWeaponSwitchControllerBackground2",
    "RangedWeaponSwitchControllerBackground3",
    "RangedWeaponSwitchControllerBackground4",
};

const char* rangedWeaponSwitchControllerIconNames[5] = {
    "RangedWeaponSwitchControllerIcon0",
    "RangedWeaponSwitchControllerIcon1",
    "RangedWeaponSwitchControllerIcon2",
    "RangedWeaponSwitchControllerIcon3",
    "RangedWeaponSwitchControllerIcon4",
};

const char* rangedWeaponSwitchControllerHighlightNames[5] = {
    "RangedWeaponSwitchControllerHighlight0",
    "RangedWeaponSwitchControllerHighlight1",
    "RangedWeaponSwitchControllerHighlight2",
    "RangedWeaponSwitchControllerHighlight3",
    "RangedWeaponSwitchControllerHighlight4",
};

const char* rangedWeaponSwitchControllerArrowName = "RangedWeaponSwitchControllerArrow";

void MeleeWeaponWheelTimeTracker() {

    meleeWeaponWheelTiming.wheelRunning = true;
    while (meleeWeaponWheelTiming.wheelTime > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        meleeWeaponWheelTiming.wheelTime--;
    }


    if (meleeWeaponWheelTiming.wheelTime == 0) {
        meleeWeaponWheelTiming.wheelAppear  = false;
        meleeWeaponWheelTiming.wheelRunning = false;
    }
}

void RangedWeaponWheelTimeTracker() {
    rangedWeaponWheelTiming.wheelRunning = true;
    while (rangedWeaponWheelTiming.wheelTime > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        rangedWeaponWheelTiming.wheelTime--;
    }


    if (rangedWeaponWheelTiming.wheelTime == 0) {
        rangedWeaponWheelTiming.wheelAppear  = false;
        rangedWeaponWheelTiming.wheelRunning = false;
    }
}


// @Todo: Templates.
void MeleeWeaponSwitchController() {
    auto& textureData             = meleeWeaponSwitchControllerTextureData;
    auto& activeConfigTextureData = activeConfig.meleeWeaponSwitchControllerTextureData;
    auto& queuedConfigTextureData = queuedConfig.meleeWeaponSwitchControllerTextureData;

    if (g_showTextures) {
        old_for_all(uint8, index, 5) {
            textureData.backgrounds[index].Render(activeConfigTextureData.backgrounds[index], queuedConfigTextureData.backgrounds[index]);
        }

        {
            constexpr uint8 textureIds[5] = {
                TEXTURE_WEAPON_REBELLION,
                TEXTURE_WEAPON_CERBERUS,
                TEXTURE_WEAPON_AGNI_RUDRA,
                TEXTURE_WEAPON_NEVAN,
                TEXTURE_WEAPON_BEOWULF_DANTE,
            };

            old_for_all(uint8, index, 5) {
                auto textureId = textureIds[index];
                if (textureId >= TEXTURE_COUNT) {
                    textureId = 0;
                }

                textureData.icons[index].textureAddr = textureAddrs[textureId];

                textureData.icons[index].Render(activeConfigTextureData.icons[index], queuedConfigTextureData.icons[index]);
            }
        }

        textureData.highlights[0].Render(activeConfigTextureData.highlights[0], queuedConfigTextureData.highlights[0]);

        {
            textureData.arrow.textureAddr = textureAddrs[TEXTURE_ARROW_0];

            textureData.arrow.Render(activeConfigTextureData.arrow, queuedConfigTextureData.arrow);
        }



        return;
    }


    auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);


    if (!InGame()) {
        return;
    }

    auto pool_1431 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
    if (!pool_1431 || !pool_1431[3]) {
        return;
    }
    auto& actorData = *reinterpret_cast<PlayerActorData*>(pool_1431[3]);

    auto& characterData = GetCharacterData(actorData);

    auto& gamepad = GetGamepad(actorData.newPlayerIndex);

    if (InCutscene() || InCredits() || !activeConfig.Actor.enable || (!activeConfig.weaponWheelEnabled) ||
        !((characterData.character == CHARACTER::DANTE) || (characterData.character == CHARACTER::VERGIL))) {
        return;
    }

    // HIDE WEAPON UI WHEN NOT HOLDING BUTTON, WITH DELAY
    if (!activeConfig.MeleeWeaponWheel.alwaysShow) {
        if ((gamepad.buttons[0] & GetBinding(BINDING::CHANGE_DEVIL_ARMS))) {
            meleeWeaponWheelTiming.wheelTime   = activeConfig.MeleeWeaponWheel.timeout;
            meleeWeaponWheelTiming.wheelAppear = true;
        }

        if ((!(gamepad.buttons[0] & GetBinding(BINDING::CHANGE_DEVIL_ARMS)) && meleeWeaponWheelTiming.wheelAppear)) {
            if (!meleeWeaponWheelTiming.wheelRunning) {
                std::thread meleewheeltracker(MeleeWeaponWheelTimeTracker);
                meleewheeltracker.detach();
            }
        }

        if (characterData.character == CHARACTER::VERGIL) {
            if ((gamepad.buttons[0] & GetBinding(BINDING::CHANGE_GUN))) {
                meleeWeaponWheelTiming.wheelTime   = activeConfig.MeleeWeaponWheel.timeout;
                meleeWeaponWheelTiming.wheelAppear = true;
            }

            if ((!(gamepad.buttons[0] & GetBinding(BINDING::CHANGE_GUN)) && meleeWeaponWheelTiming.wheelAppear)) {
                if (!meleeWeaponWheelTiming.wheelRunning) {
                    std::thread meleewheeltracker(MeleeWeaponWheelTimeTracker);
                    meleewheeltracker.detach();
                }
            }
        }
    } else {
        meleeWeaponWheelTiming.wheelAppear = true;
    }


    if (!meleeWeaponWheelTiming.wheelAppear) {
        return;
    }


    // HIDE WEAPON UI WHEN NOT HOLDING BUTTON
    /*if (!(gamepad.buttons[0] & GetBinding(BINDING::CHANGE_DEVIL_ARMS)))
    {
            if (!wheelRunning) {
                            std::thread wheeltracker(WeaponWheelTimeTracker);
                            wheeltracker.detach();
            }

            if(!wheelInCooldown) {
                    wheelInCooldown = true;
                    return;
            }


    }*/

    auto count = characterData.meleeWeaponCount;

    if (characterData.character == CHARACTER::VERGIL) {
        count = (IsNeroAngelo(actorData)) ? 2 : 3;
    }

    old_for_all(uint8, index, count) {
        textureData.backgrounds[index].Render(activeConfigTextureData.backgrounds[index], queuedConfigTextureData.backgrounds[index]);
    }

    auto meleeWeaponIndex = characterData.meleeWeaponIndex;
    if (meleeWeaponIndex >= 5) {
        meleeWeaponIndex = 0;
    }


    textureData.highlights[meleeWeaponIndex].Render(
        activeConfigTextureData.highlights[meleeWeaponIndex], queuedConfigTextureData.highlights[meleeWeaponIndex]);


    {old_for_all(uint8, index, count){auto weapon = characterData.meleeWeapons[index];
    if (weapon >= WEAPON::MAX) {
        weapon = 0;
    }

    auto textureId = textureWeaponMap[weapon];
    if (textureId >= TEXTURE_COUNT) {
        textureId = 0;
    }

    switch (weapon) {
    case WEAPON::REBELLION: {
        if (sessionData.unlockDevilTrigger) {
            textureId = TEXTURE_WEAPON_REBELLION_2;
        }

        if (actorData.sparda) {
            textureId = TEXTURE_WEAPON_FORCE_EDGE;
        }

        break;
    }
    case WEAPON::YAMATO_VERGIL: {
        if (IsNeroAngelo(actorData)) {
            textureId = TEXTURE_WEAPON_YAMATO_NERO_ANGELO;
        }

        break;
    }
    case WEAPON::BEOWULF_VERGIL: {
        if (IsNeroAngelo(actorData)) {
            textureId = TEXTURE_WEAPON_BEOWULF_NERO_ANGELO;
        }

        break;
    }
    }

    textureData.icons[index].textureAddr = textureAddrs[textureId];

    if (index != meleeWeaponIndex) {
        textureData.icons[index].Render(activeConfigTextureData.icons[index], queuedConfigTextureData.icons[index]);
    }
}
}


{
    auto textureId = textureArrowMap[meleeWeaponIndex];

    textureData.arrow.textureAddr = textureAddrs[textureId];

    textureData.arrow.Render(activeConfigTextureData.arrow, queuedConfigTextureData.arrow);


    textureData.icons[meleeWeaponIndex].Render(
        activeConfigTextureData.icons[meleeWeaponIndex], queuedConfigTextureData.icons[meleeWeaponIndex]);
}

    // Force Icons to be on top of Highlights, when Main Menu is opened it forces focus on the Main Menu.
if (!g_show) {
	ImGui::SetWindowFocus(textureData.icons[meleeWeaponIndex].label);
}
//     } else {
//         ImGui::SetWindowFocus(DMC3C_TITLE); // Calling this on-tick was making it so combo dropdowns couldn't be selected. Refer to this if future problems occur.
//     }
}

void RangedWeaponSwitchController() {
    auto& textureData             = rangedWeaponSwitchControllerTextureData;
    auto& activeConfigTextureData = activeConfig.rangedWeaponSwitchControllerTextureData;
    auto& queuedConfigTextureData = queuedConfig.rangedWeaponSwitchControllerTextureData;

    if (g_showTextures) {
        old_for_all(uint8, index, 5) {
            textureData.backgrounds[index].Render(activeConfigTextureData.backgrounds[index], queuedConfigTextureData.backgrounds[index]);
        }

        {
            constexpr uint8 textureIds[5] = {
                TEXTURE_WEAPON_EBONY_IVORY,
                TEXTURE_WEAPON_SHOTGUN,
                TEXTURE_WEAPON_ARTEMIS,
                TEXTURE_WEAPON_SPIRAL,
                TEXTURE_WEAPON_KALINA_ANN,
            };

            old_for_all(uint8, index, 5) {
                auto textureId = textureIds[index];
                if (textureId >= TEXTURE_COUNT) {
                    textureId = 0;
                }

                textureData.icons[index].textureAddr = textureAddrs[textureId];

                textureData.icons[index].Render(activeConfigTextureData.icons[index], queuedConfigTextureData.icons[index]);
            }
        }

        textureData.highlights[0].Render(activeConfigTextureData.highlights[0], queuedConfigTextureData.highlights[0]);

        {
            textureData.arrow.textureAddr = textureAddrs[TEXTURE_ARROW_0];

            textureData.arrow.Render(activeConfigTextureData.arrow, queuedConfigTextureData.arrow);
        }


        return;
    }


    auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);


    if (!InGame()) {
        return;
    }

    auto pool_1720 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
    if (!pool_1720 || !pool_1720[3]) {
        return;
    }
    auto& actorData = *reinterpret_cast<PlayerActorData*>(pool_1720[3]);

    auto& characterData = GetCharacterData(actorData);

    auto& gamepad = GetGamepad(actorData.newPlayerIndex);

    if (InCutscene() || InCredits() || !activeConfig.Actor.enable || (!activeConfig.weaponWheelEnabled) ||
        (characterData.character != CHARACTER::DANTE)) {
        return;
    }

    // HIDE WEAPON SWITCH UI WHEN NOT HOLDING BUTTON, WITH DELAY
    if (!activeConfig.RangedWeaponWheel.alwaysShow) {
        if ((gamepad.buttons[0] & GetBinding(BINDING::CHANGE_GUN))) {
            rangedWeaponWheelTiming.wheelTime   = activeConfig.RangedWeaponWheel.timeout;
            rangedWeaponWheelTiming.wheelAppear = true;
        }

        if ((!(gamepad.buttons[0] & GetBinding(BINDING::CHANGE_GUN)) && rangedWeaponWheelTiming.wheelAppear)) {
            if (!rangedWeaponWheelTiming.wheelRunning) {
                std::thread rangedwheeltracker(RangedWeaponWheelTimeTracker);
                rangedwheeltracker.detach();
            }
        }
    } else {
        rangedWeaponWheelTiming.wheelAppear = true;
    }


    if (!rangedWeaponWheelTiming.wheelAppear) {
        return;
    }


    // HIDE WEAPON SWITCH UI WHEN NOT HOLDING BUTTON
    /*if (!(gamepad.buttons[0] & GetBinding(BINDING::CHANGE_GUN)))
    {
            return;
    }*/

    auto count = characterData.rangedWeaponCount;

    old_for_all(uint8, index, count) {
        textureData.backgrounds[index].Render(activeConfigTextureData.backgrounds[index], queuedConfigTextureData.backgrounds[index]);
    }

    {
        old_for_all(uint8, index, count) {
            auto weapon = characterData.rangedWeapons[index];
            if (weapon >= WEAPON::MAX) {
                weapon = 0;
            }

            auto textureId = textureWeaponMap[weapon];
            if (textureId >= TEXTURE_COUNT) {
                textureId = 0;
            }

            textureData.icons[index].textureAddr = textureAddrs[textureId];

            textureData.icons[index].Render(activeConfigTextureData.icons[index], queuedConfigTextureData.icons[index]);
        }
    }

    auto rangedWeaponIndex = characterData.rangedWeaponIndex;
    if (rangedWeaponIndex >= 5) {
        rangedWeaponIndex = 0;
    }

    textureData.highlights[rangedWeaponIndex].Render(
        activeConfigTextureData.highlights[rangedWeaponIndex], queuedConfigTextureData.highlights[rangedWeaponIndex]);

    {
        auto textureId = textureArrowMap[rangedWeaponIndex];

        textureData.arrow.textureAddr = textureAddrs[textureId];

        textureData.arrow.Render(activeConfigTextureData.arrow, queuedConfigTextureData.arrow);
    }

    // Force Icons to be on top of Highlights, when Main Menu is opened it forces focus on the Main Menu.
    if (!g_show) {
        ImGui::SetWindowFocus(textureData.icons[rangedWeaponIndex].label);
    }
//     } else {
//         ImGui::SetWindowFocus(DMC3C_TITLE);
//     }
}

void UpdateWeaponSwitchControllerTexturePositions() {
    // Melee
    {
        auto& textureData       = meleeWeaponSwitchControllerTextureData;
        auto& configTextureData = activeConfig.meleeWeaponSwitchControllerTextureData;

        old_for_all(uint8, index, 5) {
            textureData.backgrounds[index].SetPosition(configTextureData.backgrounds[index], false);
        }

        old_for_all(uint8, index, 5) {
            textureData.highlights[index].SetPosition(configTextureData.highlights[index], false);
        }


        old_for_all(uint8, index, 5) {
            textureData.icons[index].SetPosition(configTextureData.icons[index], false);
        }

        textureData.arrow.SetPosition(configTextureData.arrow, false);
    }

    // Ranged
    {
        auto& textureData       = rangedWeaponSwitchControllerTextureData;
        auto& configTextureData = activeConfig.rangedWeaponSwitchControllerTextureData;

        old_for_all(uint8, index, 5) {
            textureData.backgrounds[index].SetPosition(configTextureData.backgrounds[index], true);
        }

        old_for_all(uint8, index, 5) {
            textureData.icons[index].SetPosition(configTextureData.icons[index], true);
        }

        old_for_all(uint8, index, 5) {
            textureData.highlights[index].SetPosition(configTextureData.highlights[index], true);
        }

        textureData.arrow.SetPosition(configTextureData.arrow, true);
    }
}

void UpdateWeaponWheelPos() {
    {
        const float multiplier = activeConfig.weaponWheelScaleMultiplier;

        CopyMemory(&queuedConfig.meleeWeaponSwitchControllerTextureData, &defaultConfig.meleeWeaponSwitchControllerTextureData,
            sizeof(queuedConfig.meleeWeaponSwitchControllerTextureData));

        auto& configTextureData = queuedConfig.meleeWeaponSwitchControllerTextureData;

        auto values  = reinterpret_cast<float*>(&configTextureData);
        uint32 count = (sizeof(configTextureData) / 4);

        old_for_all(uint32, index, count) {
            auto& value = values[index];

            value *= multiplier;
        }

        CopyMemory(&activeConfig.meleeWeaponSwitchControllerTextureData, &queuedConfig.meleeWeaponSwitchControllerTextureData,
            sizeof(activeConfig.meleeWeaponSwitchControllerTextureData));
    }

    // Ranged
    {
        const float multiplier = activeConfig.weaponWheelScaleMultiplier;

        CopyMemory(&queuedConfig.rangedWeaponSwitchControllerTextureData, &defaultConfig.rangedWeaponSwitchControllerTextureData,
            sizeof(queuedConfig.rangedWeaponSwitchControllerTextureData));

        auto& configTextureData = queuedConfig.rangedWeaponSwitchControllerTextureData;

        auto values  = reinterpret_cast<float*>(&configTextureData);
        uint32 count = (sizeof(configTextureData) / 4);

        old_for_all(uint32, index, count) {
            auto& value = values[index];

            value *= multiplier;
        }

        CopyMemory(&activeConfig.rangedWeaponSwitchControllerTextureData, &queuedConfig.rangedWeaponSwitchControllerTextureData,
            sizeof(activeConfig.rangedWeaponSwitchControllerTextureData));
    }

    UpdateWeaponSwitchControllerTexturePositions();
}


void WeaponSwitchController() {
    static bool run = false;
    if (!run) {
        run = true;

        // Melee
        {
            auto& textureData = meleeWeaponSwitchControllerTextureData;

            old_for_all(uint8, index, 5) {
                textureData.backgrounds[index] = {
                    true, false, meleeWeaponSwitchControllerBackgroundNames[index], textureAddrs[TEXTURE_BACKGROUND]};
            }

            old_for_all(uint8, index, 5) {
                textureData.icons[index] = {true, false, meleeWeaponSwitchControllerIconNames[index]};
            }

            old_for_all(uint8, index, 5) {
                textureData.highlights[index] = {
                    true, false, meleeWeaponSwitchControllerHighlightNames[index], textureAddrs[TEXTURE_HIGHLIGHT]};
            }

            textureData.arrow = {true, false, meleeWeaponSwitchControllerArrowName};
        }

        // Ranged
        {
            auto& textureData = rangedWeaponSwitchControllerTextureData;

            old_for_all(uint8, index, 5) {
                textureData.backgrounds[index] = {
                    true, false, rangedWeaponSwitchControllerBackgroundNames[index], textureAddrs[TEXTURE_BACKGROUND]};
            }

            old_for_all(uint8, index, 5) {
                textureData.icons[index] = {true, false, rangedWeaponSwitchControllerIconNames[index]};
            }

            old_for_all(uint8, index, 5) {
                textureData.highlights[index] = {
                    true, false, rangedWeaponSwitchControllerHighlightNames[index], textureAddrs[TEXTURE_HIGHLIGHT]};
            }

            textureData.arrow = {true, false, rangedWeaponSwitchControllerArrowName};
        }
    }

    UpdateWeaponWheelPos();
    MeleeWeaponSwitchController();
    RangedWeaponSwitchController();
}


void WeaponSwitchControllerSettings() {
    if (GUI_ResetButton()) {
        CopyMemory(&queuedConfig.meleeWeaponSwitchControllerTextureData, &defaultConfig.meleeWeaponSwitchControllerTextureData,
            sizeof(queuedConfig.meleeWeaponSwitchControllerTextureData));
        CopyMemory(&activeConfig.meleeWeaponSwitchControllerTextureData, &queuedConfig.meleeWeaponSwitchControllerTextureData,
            sizeof(activeConfig.meleeWeaponSwitchControllerTextureData));

        CopyMemory(&queuedConfig.rangedWeaponSwitchControllerTextureData, &defaultConfig.rangedWeaponSwitchControllerTextureData,
            sizeof(queuedConfig.rangedWeaponSwitchControllerTextureData));
        CopyMemory(&activeConfig.rangedWeaponSwitchControllerTextureData, &queuedConfig.rangedWeaponSwitchControllerTextureData,
            sizeof(activeConfig.rangedWeaponSwitchControllerTextureData));


        UpdateWeaponSwitchControllerTexturePositions();

        CopyMemory(&queuedConfig.forceIconFocus, &defaultConfig.forceIconFocus, sizeof(queuedConfig.forceIconFocus));
        CopyMemory(&activeConfig.forceIconFocus, &queuedConfig.forceIconFocus, sizeof(activeConfig.forceIconFocus));
    }
    ImGui::Text("");

    /*GUI_Checkbox2
    (
            "Force Icon Focus",
            activeConfig.forceIconFocus,
            queuedConfig.forceIconFocus
    );
    ImGui::Text("");

    GUI_Checkbox
    (
            "Show Textures",
            g_showTextures
    );
    ImGui::Text("");*/

    const float defaultWidth  = 1920;
    const float defaultHeight = 1080;

    static float targetWidth  = 1280;
    static float targetHeight = 720;

    ImGui::PushItemWidth(200);

    GUI_Checkbox2("Enable New Weapon Wheel", activeConfig.weaponWheelEnabled, queuedConfig.weaponWheelEnabled);
    ImGui::SameLine();
    TooltipHelper("(?)", "Enables a Switch Version-like Weapon Wheel with Analog Selection, only works for Player 1.");

    GUI_Checkbox2(
        "Enable Analog Weapon Selection", activeConfig.weaponWheelAnalogSelectionEnabled, queuedConfig.weaponWheelAnalogSelectionEnabled);

    GUI_Checkbox2("Disable Camera Control While Pressing Change Devil Arm/Gun", activeConfig.weaponWheelDisableCameraRotation,
        queuedConfig.weaponWheelDisableCameraRotation);


    GUI_Checkbox2("Melee Wheel Always Show", activeConfig.MeleeWeaponWheel.alwaysShow, queuedConfig.MeleeWeaponWheel.alwaysShow);
    GUI_Checkbox2("Ranged Wheel Always Show", activeConfig.RangedWeaponWheel.alwaysShow, queuedConfig.RangedWeaponWheel.alwaysShow);
    ImGui::Text("");


    GUI_InputDefault2<uint32>("Melee Wheel Timeout", activeConfig.MeleeWeaponWheel.timeout, queuedConfig.MeleeWeaponWheel.timeout,
        defaultConfig.MeleeWeaponWheel.timeout, 1, "%u");

    GUI_InputDefault2<uint32>("Ranged Wheel Timeout", activeConfig.RangedWeaponWheel.timeout, queuedConfig.RangedWeaponWheel.timeout,
        defaultConfig.RangedWeaponWheel.timeout, 1, "%u");
    ImGui::Text("");


    GUI_InputDefault2<float>("Wheel Scale Multiplier", activeConfig.weaponWheelScaleMultiplier, queuedConfig.weaponWheelScaleMultiplier,
        defaultConfig.weaponWheelScaleMultiplier, 0.1f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);

    GUI_InputDefault2<float>("Melee Wheel Horizontal", activeConfig.weaponWheelHorizontalMelee, queuedConfig.weaponWheelHorizontalMelee,
        defaultConfig.weaponWheelHorizontalMelee, 1, "%g", ImGuiInputTextFlags_EnterReturnsTrue);


    GUI_InputDefault2<float>("Ranged Wheel Horizontal", activeConfig.weaponWheelHorizontalRanged, queuedConfig.weaponWheelHorizontalRanged,
        defaultConfig.weaponWheelHorizontalRanged, 1, "%g", ImGuiInputTextFlags_EnterReturnsTrue);


    GUI_InputDefault2<float>("Wheel Height", activeConfig.weaponWheelHeight, queuedConfig.weaponWheelHeight,
        defaultConfig.weaponWheelHeight, 1, "%g", ImGuiInputTextFlags_EnterReturnsTrue);


    /*GUI_Input
    (
            "Target Height",
            targetHeight,
            1.0f,
            "%g"
    );*/


    // Melee
    UpdateWeaponWheelPos();

    // GUI::save = true;


    ImGui::Text("");

    ImGui::PopItemWidth();


    // Melee
    /*{
            auto & textureData = meleeWeaponSwitchControllerTextureData;
            auto & activeConfigTextureData = activeConfig.meleeWeaponSwitchControllerTextureData;
            auto & queuedConfigTextureData = queuedConfig.meleeWeaponSwitchControllerTextureData;

            old_for_all(uint8, index, 5)
            {
                    textureData.backgrounds[index].Settings
                    (
                            activeConfigTextureData.backgrounds[index],
                            queuedConfigTextureData.backgrounds[index]
                    );
                    ImGui::Text("");
            }

            old_for_all(uint8, index, 5)
            {
                    textureData.icons[index].Settings
                    (
                            activeConfigTextureData.icons[index],
                            queuedConfigTextureData.icons[index]
                    );
                    ImGui::Text("");
            }

            old_for_all(uint8, index, 5)
            {
                    textureData.highlights[index].Settings
                    (
                            activeConfigTextureData.highlights[index],
                            queuedConfigTextureData.highlights[index]
                    );
                    ImGui::Text("");
            }

            textureData.arrow.Settings
            (
                    activeConfigTextureData.arrow,
                    queuedConfigTextureData.arrow
            );
            ImGui::Text("");
    }

    // Ranged
    {
            auto & textureData = rangedWeaponSwitchControllerTextureData;
            auto & activeConfigTextureData = activeConfig.rangedWeaponSwitchControllerTextureData;
            auto & queuedConfigTextureData = queuedConfig.rangedWeaponSwitchControllerTextureData;

            old_for_all(uint8, index, 5)
            {
                    textureData.backgrounds[index].Settings
                    (
                            activeConfigTextureData.backgrounds[index],
                            queuedConfigTextureData.backgrounds[index]
                    );
                    ImGui::Text("");
            }

            old_for_all(uint8, index, 5)
            {
                    textureData.icons[index].Settings
                    (
                            activeConfigTextureData.icons[index],
                            queuedConfigTextureData.icons[index]
                    );
                    ImGui::Text("");
            }

            old_for_all(uint8, index, 5)
            {
                    textureData.highlights[index].Settings
                    (
                            activeConfigTextureData.highlights[index],
                            queuedConfigTextureData.highlights[index]
                    );
                    ImGui::Text("");
            }

            textureData.arrow.Settings
            (
                    activeConfigTextureData.arrow,
                    queuedConfigTextureData.arrow
            );
    }*/
}

#pragma endregion

#pragma region Actor

uint8 Actor_buttonIndices[PLAYER_COUNT] = {};

uint8 Actor_collisionGroupIndices[PLAYER_COUNT] = {};

uint8 Actor_newCharacterIndices[PLAYER_COUNT][CHARACTER_COUNT][ENTITY_COUNT] = {};

uint8 Actor_styleButtonIndices[PLAYER_COUNT][CHARACTER_COUNT][ENTITY_COUNT][STYLE_COUNT] = {};
uint8 Actor_styleIndices[PLAYER_COUNT][CHARACTER_COUNT][ENTITY_COUNT][STYLE_COUNT][2]    = {};

uint8 Actor_meleeWeaponIndices[PLAYER_COUNT][CHARACTER_COUNT][ENTITY_COUNT][MELEE_WEAPON_COUNT]   = {};
uint8 Actor_rangedWeaponIndices[PLAYER_COUNT][CHARACTER_COUNT][ENTITY_COUNT][RANGED_WEAPON_COUNT] = {};

uint8 Actor_removeBusyFlagButtonIndices[PLAYER_COUNT][4] = {};


void Actor_UpdateIndices() {
    old_for_all(uint8, playerIndex, PLAYER_COUNT) {
        auto& activePlayerData = GetActivePlayerData(playerIndex);
        auto& queuedPlayerData = GetQueuedPlayerData(playerIndex);


        UpdateMapIndex(collisionGroups, Actor_collisionGroupIndices[playerIndex], queuedPlayerData.collisionGroup);

        UpdateMapIndex(buttons, Actor_buttonIndices[playerIndex], activePlayerData.switchButton);

        old_for_all(uint8, characterIndex, CHARACTER_COUNT) {
            old_for_all(uint8, entityIndex, ENTITY_COUNT) {
                auto& activeCharacterData = GetActiveCharacterData(playerIndex, characterIndex, entityIndex);
                auto& queuedCharacterData = GetQueuedCharacterData(playerIndex, characterIndex, entityIndex);


                UpdateMapIndex(
                    newCharacters, Actor_newCharacterIndices[playerIndex][characterIndex][entityIndex], queuedCharacterData.character);


                old_for_all(uint8, styleIndex, STYLE_COUNT) {
                    UpdateMapIndex(buttons, Actor_styleButtonIndices[playerIndex][characterIndex][entityIndex][styleIndex],
                        queuedCharacterData.styleButtons[styleIndex]);

                    switch (queuedCharacterData.character) {
                    case CHARACTER::DANTE: {
                        UpdateMapIndex(stylesDante, Actor_styleIndices[playerIndex][characterIndex][entityIndex][styleIndex][0],
                            queuedCharacterData.styles[styleIndex][0]);
                        UpdateMapIndex(stylesDante, Actor_styleIndices[playerIndex][characterIndex][entityIndex][styleIndex][1],
                            queuedCharacterData.styles[styleIndex][1]);

                        break;
                    }
                    case CHARACTER::VERGIL: {
                        UpdateMapIndex(stylesVergil, Actor_styleIndices[playerIndex][characterIndex][entityIndex][styleIndex][0],
                            queuedCharacterData.styles[styleIndex][0]);
                        UpdateMapIndex(stylesVergil, Actor_styleIndices[playerIndex][characterIndex][entityIndex][styleIndex][1],
                            queuedCharacterData.styles[styleIndex][1]);

                        break;
                    }
                    }
                }

                if (queuedCharacterData.character != CHARACTER::DANTE) {
                    continue;
                }

                old_for_all(uint8, meleeWeaponIndex, MELEE_WEAPON_COUNT_DANTE) {
                    UpdateMapIndex(meleeWeaponsDante, Actor_meleeWeaponIndices[playerIndex][characterIndex][entityIndex][meleeWeaponIndex],
                        queuedCharacterData.meleeWeapons[meleeWeaponIndex]);
                }

                old_for_all(uint8, rangedWeaponIndex, RANGED_WEAPON_COUNT_DANTE) {
                    UpdateMapIndex(rangedWeaponsDante,
                        Actor_rangedWeaponIndices[playerIndex][characterIndex][entityIndex][rangedWeaponIndex],
                        queuedCharacterData.rangedWeapons[rangedWeaponIndex]);
                }
            }
        }


        old_for_all(uint8, buttonIndex, 4) {
            UpdateMapIndex(
                buttons, Actor_removeBusyFlagButtonIndices[playerIndex][buttonIndex], activePlayerData.removeBusyFlagButtons[buttonIndex]);
        }
    }
}

void Actor_CharacterTab(uint8 playerIndex, uint8 characterIndex, uint8 entityIndex, size_t defaultFontSize) {
    auto& activeCharacterData = GetActiveCharacterData(playerIndex, characterIndex, entityIndex);
    auto& queuedCharacterData = GetQueuedCharacterData(playerIndex, characterIndex, entityIndex);

	auto& activeCharacterDataClone = GetActiveCharacterData(playerIndex, characterIndex, 1);
	auto& queuedCharacterDataClone = GetQueuedCharacterData(playerIndex, characterIndex, 1);

    auto& mainActiveCharacterData = GetActiveCharacterData(playerIndex, characterIndex, ENTITY::MAIN);
    auto& mainQueuedCharacterData = GetQueuedCharacterData(playerIndex, characterIndex, ENTITY::MAIN);

	const float itemWidth = defaultFontSize * 8.0f;
    ImU32 checkmarkColor = UI::SwapColorEndianness(0xFFFFFFFF);

    if ((entityIndex == ENTITY::CLONE) && (mainQueuedCharacterData.character >= CHARACTER::MAX)) {
        return;
    }
	
    ImGui::Text("");
    ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColor);

    {
		const float columnWidth = 0.8f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;

        if (ImGui::BeginTable("CharacterSelection", 1)) {

            ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
            ImGui::TableSetupColumn("c1", 0, columnWidth);
            ImGui::TableNextRow(0, rowWidth);
            ImGui::TableNextColumn();

            ImGui::PushItemWidth(itemWidth);

            if ((playerIndex == 0) && (characterIndex > 0)) {
                if (UI::ComboMap("CHARACTER", newCharacterNames, newCharacters, Actor_newCharacterIndices[playerIndex][characterIndex][entityIndex],
                    queuedCharacterData.character)) {
                    ApplyDefaultCharacterData(queuedCharacterData, queuedCharacterData.character);

                    if (queuedCharacterData.character == CHARACTER::DANTE || queuedCharacterData.character == CHARACTER::VERGIL) {
                        queuedCharacterDataClone.character = queuedCharacterData.character;
                        ApplyDefaultCharacterData(queuedCharacterDataClone, queuedCharacterDataClone.character);
                    }

                    Actor_UpdateIndices();
                }
            }
            else {
                if (UI::Combo("CHARACTER", characterNames, queuedCharacterData.character)) {
                    ApplyDefaultCharacterData(queuedCharacterData, queuedCharacterData.character);

                    if (queuedCharacterData.character == CHARACTER::DANTE || queuedCharacterData.character == CHARACTER::VERGIL) {
                        queuedCharacterDataClone.character = queuedCharacterData.character;
                        ApplyDefaultCharacterData(queuedCharacterDataClone, queuedCharacterDataClone.character);
                    }

                    Actor_UpdateIndices();
                }
            }

            ImGui::PopItemWidth();
            ImGui::PopFont();

            

            if ((queuedCharacterData.character == CHARACTER::DANTE) || (queuedCharacterData.character == CHARACTER::VERGIL)) {
				ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);

                ImGui::TableNextRow(0, rowWidth);
                ImGui::TableNextColumn();
				ImGui::PushItemWidth(itemWidth);

				// Costume
				{
					bool condition = queuedCharacterData.ignoreCostume;

					GUI_PushDisable(condition);

                    if (GUI_Input("Costume", queuedCharacterData.costume)) {
                        queuedCharacterDataClone.costume = queuedCharacterData.costume;
                    }

					GUI_PopDisable(condition);


                    if (GUI_Checkbox("Ignore", queuedCharacterData.ignoreCostume)) {
                        queuedCharacterDataClone.ignoreCostume = queuedCharacterData.ignoreCostume;
                    }
					ImGui::SameLine();
					TooltipHelper("(?)", "Ignores your setting and uses the global value.");
				}

				ImGui::SameLine();


                if (GUI_Checkbox("Force Specific Model", queuedCharacterData.forceFiles)) {
                    queuedCharacterDataClone.forceFiles = queuedCharacterData.forceFiles;
                }


				if (queuedCharacterData.forceFiles) {

					ImGui::TableNextRow(0, rowWidth);
					ImGui::TableNextColumn();

                    if (UI::Combo("Force Model Character", characterNames, queuedCharacterData.forceFilesCharacter)) {
                        queuedCharacterDataClone.forceFilesCharacter = queuedCharacterData.forceFilesCharacter;
                    }
                    
                    ImGui::TableNextRow(0, rowWidth);
					ImGui::TableNextColumn();

                    if (GUI_Input("Force Model Costume", queuedCharacterData.forceFilesCostume)) {
                        queuedCharacterDataClone.forceFilesCostume = queuedCharacterData.forceFilesCostume;
                    }

				}


				if (UI::Combo("Doppelganger", characterNames, queuedCharacterDataClone.character)) {
					ApplyDefaultCharacterData(queuedCharacterDataClone, queuedCharacterDataClone.character);

					Actor_UpdateIndices();
				}

                ImGui::PopItemWidth();
                ImGui::PopFont();
            }


            ImGui::EndTable();
        }
    }
    
    ImGui::Text("");

    /*ImGui::Text("Styles");

    old_for_all(uint8, styleIndex, STYLE_COUNT)
    {
            if constexpr (debug)
            {
                    ImGui::Text("");
                    ImGui::Text("%u", styleIndex);
            }

            UI::ComboMap
            (
                    "",
                    buttonNames,
                    buttons,
                    Actor_styleButtonIndices[playerIndex][characterIndex][entityIndex][styleIndex],
                    queuedCharacterData.styleButtons[styleIndex],
                    ImGuiComboFlags_HeightLargest
            );

            if constexpr (!debug)
            {
                    ImGui::SameLine();
            }

            switch (queuedCharacterData.character)
            {
                    case CHARACTER::DANTE:
                    {
                            UI::ComboMap
                            (
                                    "",
                                    styleNamesDante,
                                    stylesDante,
                                    Actor_styleIndices[playerIndex][characterIndex][entityIndex][styleIndex][0],
                                    queuedCharacterData.styles[styleIndex][0]
                            );

                            if constexpr (!debug)
                            {
                                    ImGui::SameLine();
                            }

                            UI::ComboMap
                            (
                                    "",
                                    styleNamesDante,
                                    stylesDante,
                                    Actor_styleIndices[playerIndex][characterIndex][entityIndex][styleIndex][1],
                                    queuedCharacterData.styles[styleIndex][1]
                            );

                            break;
                    }
                    case CHARACTER::VERGIL:
                    {
                            UI::ComboMap
                            (
                                    "",
                                    styleNamesVergil,
                                    stylesVergil,
                                    Actor_styleIndices[playerIndex][characterIndex][entityIndex][styleIndex][0],
                                    queuedCharacterData.styles[styleIndex][0]
                            );

                            if constexpr (debug)
                            {
                                    ImGui::SameLine();
                            }

                            UI::ComboMap
                            (
                                    "",
                                    styleNamesVergil,
                                    stylesVergil,
                                    Actor_styleIndices[playerIndex][characterIndex][entityIndex][styleIndex][1],
                                    queuedCharacterData.styles[styleIndex][1]
                            );

                            break;
                    }
            }
    }

    ImGui::PopItemWidth();*/

    ImGui::PopStyleColor();

    if (queuedCharacterData.character != CHARACTER::DANTE) {
        return;
    }

	
	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
	ImGui::Text("WEAPON LOADOUT");
    //GUI_SectionEnd(SectionFlags_NoNewLine);
	ImGui::PopFont();

    ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
    {
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;

        if (ImGui::BeginTable("WeaponLoadout", 2)) {

			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

            ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.0f]);
            ImGui::Text("MELEE");
            ImGui::PopFont();

            ImGui::PushItemWidth(itemWidth);

            GUI_Slider<uint8>("", queuedCharacterData.meleeWeaponCount, 1, MELEE_WEAPON_COUNT_DANTE);

            queuedCharacterDataClone.meleeWeaponCount = queuedCharacterData.meleeWeaponCount;

            old_for_all(uint8, meleeWeaponIndex, MELEE_WEAPON_COUNT_DANTE) {
                bool condition = (meleeWeaponIndex >= queuedCharacterData.meleeWeaponCount);

                GUI_PushDisable(condition);

                UI::ComboMap("", meleeWeaponNamesDante, meleeWeaponsDante,
                    Actor_meleeWeaponIndices[playerIndex][characterIndex][entityIndex][meleeWeaponIndex],
                    queuedCharacterData.meleeWeapons[meleeWeaponIndex]);

                // Doppelganger will now have same weapons equipped as Dante - Mia.
                queuedCharacterDataClone.meleeWeapons[meleeWeaponIndex] = queuedCharacterData.meleeWeapons[meleeWeaponIndex];

                GUI_PopDisable(condition);
            }

            ImGui::TableNextColumn();

            ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.0f]);
            ImGui::Text("RANGED");
            ImGui::PopFont();

            GUI_Slider<uint8>("", queuedCharacterData.rangedWeaponCount, 1, RANGED_WEAPON_COUNT_DANTE);

            old_for_all(uint8, rangedWeaponIndex, RANGED_WEAPON_COUNT_DANTE) {
                bool condition = (rangedWeaponIndex >= queuedCharacterData.rangedWeaponCount);

                GUI_PushDisable(condition);

                UI::ComboMap("", rangedWeaponNamesDante, rangedWeaponsDante,
                    Actor_rangedWeaponIndices[playerIndex][characterIndex][entityIndex][rangedWeaponIndex],
                    queuedCharacterData.rangedWeapons[rangedWeaponIndex]);

				// Doppelganger will now have same weapons equipped as Dante - Mia.
				queuedCharacterDataClone.rangedWeapons[rangedWeaponIndex] = queuedCharacterData.rangedWeapons[rangedWeaponIndex];

                GUI_PopDisable(condition);
            }

            ImGui::PopItemWidth();

            ImGui::EndTable();
        }
    }
    ImGui::PopFont();
}

void Actor_PlayerTab(uint8 playerIndex, size_t defaultFontSize) {
    auto& activePlayerData = GetActivePlayerData(playerIndex);
    auto& queuedPlayerData = GetQueuedPlayerData(playerIndex);

	const float itemWidth = defaultFontSize * 8.0f;
	const float columnWidth = 0.5f * queuedConfig.globalScale;
	const float rowWidth = 40.0f * queuedConfig.globalScale;

    ImGui::Text("");

    ImGui::PushItemWidth(itemWidth);
    ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);

    GUI_Slider<uint8>("Character Count", queuedPlayerData.characterCount, 1, CHARACTER_COUNT);


    UI::ComboMap2("Switch Button", buttonNames, buttons, Actor_buttonIndices[playerIndex], activePlayerData.switchButton, queuedPlayerData.switchButton,
        ImGuiComboFlags_HeightLargest);
    ImGui::SameLine();
    TooltipHelper("(?)", "Multi-purpose button.\n"
                         "Main purpose is to switch characters.\n"
                         "While Doppelganger is active it's used to control the clone's state.\n"
                         "Hold the button and switch style or weapon to trigger the action for the clone.\n");

	if (playerIndex != 0) {

		UI::ComboMap("Type (Collision Group)", collisionGroupNames, collisionGroups, Actor_collisionGroupIndices[playerIndex],
			queuedPlayerData.collisionGroup);

	}


//     if (GUI_Button("Reset")) {
//         CopyMemory(&queuedConfig.Actor.playerData[playerIndex].removeBusyFlag, &defaultConfig.Actor.playerData[playerIndex].removeBusyFlag,
//             sizeof(queuedConfig.Actor.playerData[playerIndex].removeBusyFlag));
//         CopyMemory(&activeConfig.Actor.playerData[playerIndex].removeBusyFlag, &queuedConfig.Actor.playerData[playerIndex].removeBusyFlag,
//             sizeof(activeConfig.Actor.playerData[playerIndex].removeBusyFlag));
// 
//         CopyMemory(&queuedConfig.Actor.playerData[playerIndex].removeBusyFlagButtons,
//             &defaultConfig.Actor.playerData[playerIndex].removeBusyFlagButtons,
//             sizeof(queuedConfig.Actor.playerData[playerIndex].removeBusyFlagButtons));
//         CopyMemory(&activeConfig.Actor.playerData[playerIndex].removeBusyFlagButtons,
//             &queuedConfig.Actor.playerData[playerIndex].removeBusyFlagButtons,
//             sizeof(activeConfig.Actor.playerData[playerIndex].removeBusyFlagButtons));
// 
// 
//         Actor_UpdateIndices();
//     }
/*    ImGui::Text("");*/


// 	GUI_Checkbox2("Remove Busy Flag", activePlayerData.removeBusyFlag, queuedPlayerData.removeBusyFlag);
// 	ImGui::SameLine();
// 	TooltipHelper("(?)", "Removes the actor's busy flag which allows you to do insane combos or just look stupid.");
// 	ImGui::Text("");
// 
// 
//     {
//         bool condition = !activePlayerData.removeBusyFlag;
// 
//         GUI_PushDisable(condition);
// 
// 
//         old_for_all(uint8, buttonIndex, 4) {
//             UI::ComboMap2(buttonIndexNames[buttonIndex], buttonNames, buttons, Actor_removeBusyFlagButtonIndices[playerIndex][buttonIndex],
//                 activePlayerData.removeBusyFlagButtons[buttonIndex], queuedPlayerData.removeBusyFlagButtons[buttonIndex],
//                 ImGuiComboFlags_HeightLargest);
//         }
// 
// 
//         GUI_PopDisable(condition);
// 
//     }

    ImGui::PopItemWidth();
    ImGui::PopFont();
}

void ActorSection(size_t defaultFontSize) {

	const float itemWidth = defaultFontSize * 8.0f;
	const float columnWidth = 0.8f * queuedConfig.globalScale;
	const float rowWidth = 40.0f * queuedConfig.globalScale;
    ImU32 checkmarkColor = UI::SwapColorEndianness(0xFFFFFFFF);

    bool actorCondition = (!queuedConfig.Actor.enable);

    GUI_PushDisable(actorCondition);


// 	if (GUI_ResetButton()) {
// 		CopyMemory(&queuedConfig.Actor, &defaultConfig.Actor, sizeof(queuedConfig.Actor));
// 
// 		old_for_all(uint8, playerIndex, PLAYER_COUNT) {
// 			CopyMemory(&queuedConfig.Actor.playerData[playerIndex].button, &defaultConfig.Actor.playerData[playerIndex].button,
// 				sizeof(queuedConfig.Actor.playerData[playerIndex].button));
// 			CopyMemory(&activeConfig.Actor.playerData[playerIndex].button, &queuedConfig.Actor.playerData[playerIndex].button,
// 				sizeof(activeConfig.Actor.playerData[playerIndex].button));
// 
// 
// 			CopyMemory(&queuedConfig.Actor.playerData[playerIndex].removeBusyFlag,
// 				&defaultConfig.Actor.playerData[playerIndex].removeBusyFlag,
// 				sizeof(queuedConfig.Actor.playerData[playerIndex].removeBusyFlag));
// 			CopyMemory(&activeConfig.Actor.playerData[playerIndex].removeBusyFlag,
// 				&queuedConfig.Actor.playerData[playerIndex].removeBusyFlag,
// 				sizeof(activeConfig.Actor.playerData[playerIndex].removeBusyFlag));
// 
// 			CopyMemory(&queuedConfig.Actor.playerData[playerIndex].removeBusyFlagButtons,
// 				&defaultConfig.Actor.playerData[playerIndex].removeBusyFlagButtons,
// 				sizeof(queuedConfig.Actor.playerData[playerIndex].removeBusyFlagButtons));
// 			CopyMemory(&activeConfig.Actor.playerData[playerIndex].removeBusyFlagButtons,
// 				&queuedConfig.Actor.playerData[playerIndex].removeBusyFlagButtons,
// 				sizeof(activeConfig.Actor.playerData[playerIndex].removeBusyFlagButtons));
// 		}
// 
// 		Actor_UpdateIndices();
// 
// 		CopyMemory(&queuedConfig.updateLockOns, &defaultConfig.updateLockOns, sizeof(queuedConfig.updateLockOns));
// 		CopyMemory(&activeConfig.updateLockOns, &queuedConfig.updateLockOns, sizeof(activeConfig.updateLockOns));
// 
// 		CopyMemory(&queuedConfig.forceSyncHitMagicPoints, &defaultConfig.forceSyncHitMagicPoints,
// 			sizeof(queuedConfig.forceSyncHitMagicPoints));
// 		CopyMemory(
// 			&activeConfig.forceSyncHitMagicPoints, &queuedConfig.forceSyncHitMagicPoints, sizeof(activeConfig.forceSyncHitMagicPoints));
// 
// 		CopyMemory(&queuedConfig.resetPermissions, &defaultConfig.resetPermissions, sizeof(queuedConfig.resetPermissions));
// 		CopyMemory(&activeConfig.resetPermissions, &queuedConfig.resetPermissions, sizeof(activeConfig.resetPermissions));
// 
// 
// 		CopyMemory(&queuedConfig.enableBossLadyFixes, &defaultConfig.enableBossLadyFixes, sizeof(queuedConfig.enableBossLadyFixes));
// 		CopyMemory(&activeConfig.enableBossLadyFixes, &queuedConfig.enableBossLadyFixes, sizeof(activeConfig.enableBossLadyFixes));
// 
// 		CopyMemory(
// 			&queuedConfig.enableBossVergilFixes, &defaultConfig.enableBossVergilFixes, sizeof(queuedConfig.enableBossVergilFixes));
// 		CopyMemory(
// 			&activeConfig.enableBossVergilFixes, &queuedConfig.enableBossVergilFixes, sizeof(activeConfig.enableBossVergilFixes));
// 
// 		CopyMemory(&queuedConfig.enablePVPFixes, &defaultConfig.enablePVPFixes, sizeof(queuedConfig.enablePVPFixes));
// 		CopyMemory(&activeConfig.enablePVPFixes, &queuedConfig.enablePVPFixes, sizeof(activeConfig.enablePVPFixes));
// 
// 		ToggleBossLadyFixes(activeConfig.enableBossLadyFixes);
// 		ToggleBossVergilFixes(activeConfig.enableBossVergilFixes);
// 
// 		CopyMemory(&queuedConfig.absoluteUnit, &defaultConfig.absoluteUnit, sizeof(queuedConfig.absoluteUnit));
// 		CopyMemory(&activeConfig.absoluteUnit, &queuedConfig.absoluteUnit, sizeof(activeConfig.absoluteUnit));
// 	}


    

    uint8 activePlayerIndex;


    if (ImGui::BeginTable("CharacterTable", 2)) {

        // ImGui::TableSetColumnWidth(0, columnWidth);
        ImGui::TableSetupColumn("c1", 0, columnWidth);
        ImGui::TableNextRow(0, rowWidth);
        ImGui::TableNextColumn();


		if (ImGui::BeginTabBar("PlayerTabs")) {
            old_for_all(uint8, playerIndex, PLAYER_COUNT) {
                auto condition = (playerIndex >= queuedConfig.Actor.playerCount);

                GUI_PushDisable(condition);

                ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.7f]);

                if (ImGui::BeginTabItem(playerIndexNames[playerIndex])) {
                    //ImGui::Text("");


                    Actor_PlayerTab(playerIndex, defaultFontSize);
                    activePlayerIndex = playerIndex;
  

                    ImGui::EndTabItem();
                }

                ImGui::PopFont();

                
                GUI_PopDisable(condition);

            }

			ImGui::EndTabBar();
		}

		ImGui::TableNextColumn();

		if (ImGui::BeginTabBar("CharacterTabs")) {

			auto& activePlayerData = GetActivePlayerData(activePlayerIndex);
			auto& queuedPlayerData = GetQueuedPlayerData(activePlayerIndex);


			old_for_all(uint8, characterIndex, CHARACTER_COUNT) {
				auto condition = (characterIndex >= queuedPlayerData.characterCount);

				GUI_PushDisable(condition);

                ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);

				if (ImGui::BeginTabItem(characterIndexNames[characterIndex])) {
		
					Actor_CharacterTab(activePlayerIndex, characterIndex, 0, defaultFontSize);	

					ImGui::EndTabItem();
				}

                ImGui::PopFont();

				GUI_PopDisable(condition);
			}

			ImGui::EndTabBar();
		}

        ImGui::EndTable();

    }

    GUI_PopDisable(actorCondition);

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
	GUI_Checkbox("ACTOR SYSTEM", queuedConfig.Actor.enable);
	ImGui::PopFont();
	UI::SeparatorEx(defaultFontSize * 23.35f);

    ImGui::Text("");

    GUI_PushDisable(actorCondition);

    ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColor);
    ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
    ImGui::PushItemWidth(itemWidth);
    GUI_Slider<uint8>("Player Count", queuedConfig.Actor.playerCount, 1, PLAYER_COUNT);
	UI::Combo2("Costume Respects Game Progression", costumeRespectsProgressionNames, activeConfig.costumeRespectsProgression,
		queuedConfig.costumeRespectsProgression);

	ImGui::SameLine();
	TooltipHelper("(?)", "First Costume updates as the game progresses as in Vanilla.\n"
		"\n"
		"Original behaves the same as the Vanilla Game.\n"
		"Crimson also updates Vergil's First Costume."

	);

    {
		const float columnWidth = 0.7f * queuedConfig.globalScale;
		const float rowWidth = 30.0f * queuedConfig.globalScale;

        if (ImGui::BeginTable("ActorSystemTable", 3)) {

			ImGui::TableSetupColumn("c2", 0, columnWidth * 1.4f);
			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

           
			if (GUI_Checkbox2("Boss Lady Fixes", activeConfig.enableBossLadyFixes, queuedConfig.enableBossLadyFixes)) {
				ToggleBossLadyFixes(activeConfig.enableBossLadyFixes);
			}

            ImGui::TableNextColumn();


            if (GUI_Checkbox2("Boss Vergil Fixes", activeConfig.enableBossVergilFixes, queuedConfig.enableBossVergilFixes)) {
                ToggleBossVergilFixes(activeConfig.enableBossVergilFixes);
            }
			

            ImGui::TableNextColumn();

            GUI_Checkbox2("PVP Fixes", activeConfig.enablePVPFixes, queuedConfig.enablePVPFixes);

            ImGui::EndTable();
        }
    }

    // Deprecated DDMK Options

// 	GUI_Checkbox2("Update Lock-Ons", activeConfig.updateLockOns, queuedConfig.updateLockOns);
// 
// 	GUI_Checkbox2("Force Sync Hit & Magic Points", activeConfig.forceSyncHitMagicPoints, queuedConfig.forceSyncHitMagicPoints);
// 
// 
// 	GUI_Checkbox2("Reset Permissions", activeConfig.resetPermissions, queuedConfig.resetPermissions);
// 	ImGui::SameLine();
// 	TooltipHelper("(?)", "Press the taunt button to reset the actor's permissions.\n"
// 		"Useful when getting stuck.");

	// 
// 	GUI_Checkbox2("Unlock Everything (Absolute Unit)", activeConfig.absoluteUnit, queuedConfig.absoluteUnit);
// 	ImGui::SameLine();
// 	TooltipHelper("(?)",
// 		"I mastered the art of jump-cancelling before I was born.\n"
// 		"I beat the game 5000+ times.\n"
// 		"Star-raving on 3x turbo amuses me.\n"
// 		"Other \"players\" complaining about wrist pain makes me cringe.\n"
// 		"I'm a GROWN, ASS, MAN.\n"
// 		"I can absolutely, positively, under no circumstances be bothered with leveling up again.",
// 		500);


    GUI_PopDisable(actorCondition);
    ImGui::PopStyleColor();
    ImGui::PopItemWidth();
    ImGui::PopFont();
    
}

#pragma endregion

#pragma region Arcade

uint8 Arcade_modeIndex                 = 0;
uint8 Arcade_meleeWeaponIndexDante[2]  = {};
uint8 Arcade_rangedWeaponIndexDante[2] = {};


void Arcade_UpdateIndices() {
    UpdateMapIndex(modes, Arcade_modeIndex, activeConfig.Arcade.mode);

    UpdateMapIndex(meleeWeaponsDante, Arcade_meleeWeaponIndexDante[0], activeConfig.Arcade.weapons[0]);

    UpdateMapIndex(meleeWeaponsDante, Arcade_meleeWeaponIndexDante[1], activeConfig.Arcade.weapons[1]);

    UpdateMapIndex(rangedWeaponsDante, Arcade_rangedWeaponIndexDante[0], activeConfig.Arcade.weapons[2]);

    UpdateMapIndex(rangedWeaponsDante, Arcade_rangedWeaponIndexDante[1], activeConfig.Arcade.weapons[3]);
}

void ArcadeSection(size_t defaultFontSize) {
	
	const float itemWidth = defaultFontSize * 8.0f; 
    const float columnWidth = 0.5f * queuedConfig.globalScale;
    const float rowWidth = 40.0f * queuedConfig.globalScale;

    ImU32 highlightColorBg = UI::SwapColorEndianness(0xFFFFFFFF);
    ImU32 highlightColorText = UI::SwapColorEndianness(0xFF809FFF);



	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);

	if (GUI_Checkbox2("ARCADE", activeConfig.Arcade.enable, queuedConfig.Arcade.enable)) {
		Arcade::Toggle(activeConfig.Arcade.enable);
	}

	ImGui::PopFont();

	UI::SeparatorEx(defaultFontSize * 23.35f);

    
    ImGui::PushItemWidth(itemWidth);
    ImGui::Text("");

// 	if (GUI_ResetButton()) {
// 		CopyMemory(&queuedConfig.Arcade, &defaultConfig.Arcade, sizeof(queuedConfig.Arcade));
// 		CopyMemory(&activeConfig.Arcade, &queuedConfig.Arcade, sizeof(activeConfig.Arcade));
// 
// 
// 		Arcade_UpdateIndices();
// 
// 		Arcade::Toggle(activeConfig.Arcade.enable);
//	}
	

    ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
    ImGui::PushStyleColor(ImGuiCol_CheckMark, highlightColorBg);

	if (ImGui::BeginTable("ArcadeTable", 2)) {

       // ImGui::TableSetColumnWidth(0, columnWidth);
        ImGui::TableSetupColumn("c1", 0, columnWidth);
        ImGui::TableNextRow(0, rowWidth);
        
		ImGui::TableNextColumn();
        
        ImGui::PushItemWidth(itemWidth);

		UI::Combo2("", missionNames, activeConfig.Arcade.mission, queuedConfig.Arcade.mission, ImGuiComboFlags_HeightLargest);

		ImGui::SameLine();
        ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
		ImGui::Text("MISSION");
        ImGui::PopFont();


		ImGui::TableNextColumn();

		if (activeConfig.Arcade.mission > 0) {
            ImGui::PushItemWidth(itemWidth);
			UI::ComboMap2("", modeNames, modes, Arcade_modeIndex, activeConfig.Arcade.mode, queuedConfig.Arcade.mode);
			
            ImGui::SameLine();
            ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
			ImGui::Text("DIFFICULTY");
            ImGui::PopFont();

		}

        ImGui::TableNextRow(0, rowWidth);
        ImGui::TableNextColumn();


		if (activeConfig.Arcade.mission > 0) {
			GUI_InputDefault2<float>("Max HP", activeConfig.Arcade.hitPoints, queuedConfig.Arcade.hitPoints,
				defaultConfig.Arcade.hitPoints, 1000, "%g", ImGuiInputTextFlags_EnterReturnsTrue);

            ImGui::TableNextColumn();

			GUI_InputDefault2<float>("Max Devil Trigger", activeConfig.Arcade.magicPoints, queuedConfig.Arcade.magicPoints,
				defaultConfig.Arcade.magicPoints, 1000, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
		}

		ImGui::TableNextRow(0, rowWidth);
		ImGui::TableNextColumn();

        UI::Combo2("Campaign", characterNames, activeConfig.Arcade.character, queuedConfig.Arcade.character);

		{
            auto queuedCharacterData = GetQueuedCharacterData(0, 0, 0);
			bool disableCondition = queuedConfig.Actor.enable;

            

            if (activeConfig.Arcade.mission > 0 && (queuedCharacterData.ignoreCostume || !queuedConfig.Actor.enable)) {
                ImGui::TableNextColumn();
				GUI_InputDefault2<uint8>("Costume", activeConfig.Arcade.costume, queuedConfig.Arcade.costume, defaultConfig.Arcade.costume, 1,
					"%u", ImGuiInputTextFlags_EnterReturnsTrue);
			}

			
			if ((activeConfig.Arcade.mission > 0) && (activeConfig.Arcade.character == CHARACTER::DANTE) && !queuedConfig.Actor.enable) {
				ImGui::TableNextRow(0, rowWidth);
				ImGui::TableNextColumn();

				UI::Combo2("Style                  ", styleNamesDante, activeConfig.Arcade.style, queuedConfig.Arcade.style);

                ImGui::TableNextColumn();
				UI::ComboMap2("Ranged 1", rangedWeaponNamesDante, rangedWeaponsDante, Arcade_rangedWeaponIndexDante[0],
					activeConfig.Arcade.weapons[2], queuedConfig.Arcade.weapons[2]);

				ImGui::TableNextRow(0, rowWidth);
				ImGui::TableNextColumn();

				UI::ComboMap2("Melee 1             ", meleeWeaponNamesDante, meleeWeaponsDante, Arcade_meleeWeaponIndexDante[0],
					activeConfig.Arcade.weapons[0], queuedConfig.Arcade.weapons[0]);

                ImGui::TableNextColumn();
				UI::ComboMap2("Ranged 2", rangedWeaponNamesDante, rangedWeaponsDante, Arcade_rangedWeaponIndexDante[1],
					activeConfig.Arcade.weapons[3], queuedConfig.Arcade.weapons[3]);

				ImGui::TableNextRow(0, rowWidth);
				ImGui::TableNextColumn();
				UI::ComboMap2("Melee 2", meleeWeaponNamesDante, meleeWeaponsDante, Arcade_meleeWeaponIndexDante[1],
					activeConfig.Arcade.weapons[1], queuedConfig.Arcade.weapons[1]);

			}

		}

		if ((activeConfig.Arcade.mission >= 1) && (activeConfig.Arcade.mission <= 20)) {

			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			// Room
			{
				bool condition = !activeConfig.Arcade.enableRoomSelection;

				GUI_PushDisable(condition);

				GUI_InputDefault2<uint32>("", activeConfig.Arcade.room, queuedConfig.Arcade.room, defaultConfig.Arcade.room, 1, "%u",
					ImGuiInputTextFlags_EnterReturnsTrue);

				GUI_PopDisable(condition);

                ImGui::SameLine();
                ImGui::Text("Room");

				
				ImGui::SameLine();
				GUI_Checkbox2("", activeConfig.Arcade.enableRoomSelection, queuedConfig.Arcade.enableRoomSelection);
			}

            ImGui::TableNextColumn();

			// Position
			{
				bool condition = !activeConfig.Arcade.enablePositionSelection;

				GUI_PushDisable(condition);

				GUI_InputDefault2<uint32>("", activeConfig.Arcade.position, queuedConfig.Arcade.position,
					defaultConfig.Arcade.position, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);

				GUI_PopDisable(condition);

				ImGui::SameLine();
                ImGui::Text("Position");

				ImGui::SameLine();
				GUI_Checkbox2("", activeConfig.Arcade.enablePositionSelection, queuedConfig.Arcade.enablePositionSelection);
			}
		}

		if (activeConfig.Arcade.mission == MISSION::BLOODY_PALACE) {

			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			UI::Combo2("Floor", floorNames, activeConfig.Arcade.floor, queuedConfig.Arcade.floor, ImGuiComboFlags_HeightLargest);

            ImGui::TableNextColumn();

			GUI_InputDefault2<uint16>("Level", activeConfig.Arcade.level, queuedConfig.Arcade.level, defaultConfig.Arcade.level, 1, "%u",
				ImGuiInputTextFlags_EnterReturnsTrue);
		}


		ImGui::EndTable();
	}
    

    ImGui::SameLine();

	//
    ImGui::PopItemWidth();
    ImGui::PopFont();
    ImGui::PopStyleColor();

	ImGui::Text("");
	
}

#pragma endregion

#pragma region Bars

const char* barsNames[PLAYER_COUNT] = {
    "Bars1",
    "Bars2",
    "Bars3",
    "Bars4",
};

bool showBars = false;


void BarsFunction(
    float hitPoints, float magicPoints, const char* name, PlayerActorData actorData, const char* label, Config::BarsData& activeData, Config::BarsData& queuedData) {
    if (!showBars && !activeConfig.showAdditionalBars) {
        return;
    }

    auto& activePos = *reinterpret_cast<ImVec2*>(&activeData.pos);
    auto& queuedPos = *reinterpret_cast<ImVec2*>(&queuedData.pos);

    auto& lastX = activeData.lastX;
    auto& lastY = activeData.lastY;

    auto& run = activeData.run;
    if (!run) {
        
        queuedPos.x = queuedData.lastX;
        queuedPos.y = queuedData.lastY;

        ImGui::SetNextWindowPos(activePos);

        lastX = static_cast<uint32>(activeData.pos.x);
        lastY = static_cast<uint32>(activeData.pos.y);
        run = true;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));

    if (ImGui::Begin(label, &activeConfig.showAdditionalBars, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize) && run) {
  
        if (guiPause.canPause) { // This prevents the resetting of bars' positions by delaying the queued pos update starting point. - Mia
            queuedPos = ImGui::GetWindowPos(); // Queued pos updates according to its pos on screen.
        }
        activePos = queuedPos;
        
        ImGui::SetWindowPos(queuedPos);
        
        
        auto x = static_cast<uint32>(activeData.pos.x);
        auto y = static_cast<uint32>(activeData.pos.y);


        if ((lastX != x) || (lastY != y)) {
            lastX = x;
            lastY = y;

            queuedData.lastX = x;
            queuedData.lastY = y;

            GUI::save = true;
        }
        ImGui::PushFont(UI::g_ImGuiFont_RussoOne[18.0 * 1.1f]);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1));
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, *reinterpret_cast<ImVec4*>(&activeData.hitColor));
        ImGui::ProgressBar(hitPoints, *reinterpret_cast<ImVec2*>(&activeData.size), "");
        ImGui::PopStyleColor(2);

        if (actorData.character != CHARACTER::VERGIL) {
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, *reinterpret_cast<ImVec4*>(&activeData.magicColor));
        }
        else {
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, *reinterpret_cast<ImVec4*>(&activeData.magicColorVergil));
        }
        ImGui::ProgressBar(magicPoints, *reinterpret_cast<ImVec2*>(&activeData.size), "");
        ImGui::PopStyleColor();


        ImGui::Text(name);
        ImGui::SameLine();
        if (actorData.character == CHARACTER::DANTE || actorData.character == CHARACTER::VERGIL) {
			ImGui::Text("  ");
			ImGui::SameLine(0.0f, 0);

            if (actorData.character == CHARACTER::DANTE) {
                ImGui::Text(styleNamesDanteGameplay[actorData.style]);
            }
            else {
                ImGui::Text(styleNamesVergilGameplay[actorData.style]);
            }
        }

        ImGui::PopFont();

        
     
    }

    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(4);
}

void BarsSettingsFunction(const char* label, Config::BarsData& activeData, Config::BarsData& queuedData, Config::BarsData& defaultData) {
    auto& activePos  = *reinterpret_cast<ImVec2*>(&activeData.pos);
    auto& queuedPos  = *reinterpret_cast<ImVec2*>(&queuedData.pos);
    auto& defaultPos = *reinterpret_cast<ImVec2*>(&defaultData.pos);


    /*GUI_Checkbox2("Enable", activeData.enable, queuedData.enable);*/
    ImGui::Text("");

//      if (GUI_ResetButton()) {
//          CopyMemory(&queuedData, &defaultData, sizeof(queuedData));
//          CopyMemory(&activeData, &queuedData, sizeof(activeData));
//  
//          ImGui::SetWindowPos(label, activePos);
//      }
//     ImGui::Text("");

    bool condition = !activeData.enable;

    GUI_PushDisable(condition);

//     GUI_Color2("Hit Color", activeData.hitColor, queuedData.hitColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview);
//     GUI_Color2(
//         "Magic Color", activeData.magicColor, queuedData.magicColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview);
//     ImGui::Text("");

    ImGui::PushItemWidth(150);

//     GUI_InputDefault2("Width", activeData.size.x, queuedData.size.x, defaultData.size.x, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
//     GUI_InputDefault2("Height", activeData.size.y, queuedData.size.y, defaultData.size.y, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
// 
    if (GUI_Input2<float>("X", activePos.x, queuedPos.x, 1, "%g", ImGuiInputTextFlags_EnterReturnsTrue)) {
        ImGui::SetWindowPos(label, activePos);
    }
    ImGui::SameLine();
    if (GUI_Input2<float>("Y", activePos.y, queuedPos.y, 1, "%g", ImGuiInputTextFlags_EnterReturnsTrue)) {
        ImGui::SetWindowPos(label, activePos);
    }

    ImGui::PopItemWidth();

    GUI_PopDisable(condition);
}

void Bars() {
    if (!showBars && !(activeConfig.Actor.enable && InGame())) {
        return;
    }

    uint8 playerCount = (showBars) ? PLAYER_COUNT : activeConfig.Actor.playerCount;

    int minimum = 1;

	

    old_for_all(uint8, playerIndex, playerCount) {
        
		if (!activeConfig.show1Pbar) {
			minimum = 1;
		}
		else {
			minimum = 0;
		}

        if (playerIndex >= minimum) {


            float hit = 0.75f;
            float magic = 0.5f;

            [&]() {
                auto& playerData = GetPlayerData(playerIndex);

                auto& characterData = GetCharacterData(playerIndex, playerData.characterIndex, ENTITY::MAIN);
                auto& newActorData = GetNewActorData(playerIndex, playerData.characterIndex, ENTITY::MAIN);

                auto& activeCharacterData = GetCharacterData(playerIndex, playerData.activeCharacterIndex, ENTITY::MAIN);
                auto& activeNewActorData = GetNewActorData(playerIndex, playerData.activeCharacterIndex, ENTITY::MAIN);

                auto& leadCharacterData = GetCharacterData(playerIndex, 0, ENTITY::MAIN);
                auto& leadNewActorData = GetNewActorData(playerIndex, 0, ENTITY::MAIN);

                auto& mainCharacterData = GetCharacterData(playerIndex, playerData.characterIndex, ENTITY::MAIN);
                auto& mainNewActorData = GetNewActorData(playerIndex, playerData.characterIndex, ENTITY::MAIN);


                if (activeCharacterData.character >= CHARACTER::MAX) {
                    hit = 1.0f;
                    magic = 1.0f;

                    return;
                }

                if (!activeNewActorData.baseAddr) {
                    return;
                }
                auto& activeActorData = *reinterpret_cast<PlayerActorData*>(activeNewActorData.baseAddr);

                hit = (activeActorData.hitPoints / activeActorData.maxHitPoints);
                magic = (activeActorData.magicPoints / activeActorData.maxMagicPoints);

//                 activeConfig.barsData[playerIndex].pos.x = queuedConfig.barsData[playerIndex].pos.x;
//                 activeConfig.barsData[playerIndex].pos.y = queuedConfig.barsData[playerIndex].pos.y;



                BarsFunction(hit, magic, playerIndexNames[playerIndex], activeActorData, barsNames[playerIndex], activeConfig.barsData[playerIndex],
                    queuedConfig.barsData[playerIndex]);
                }();
        }
        
    }
}

void BarsSettings(size_t defaultFontSize) {

    ImGui::Text("");

    const float itemWidth = defaultFontSize * 6.0f;
	const float columnWidth = 0.5f * queuedConfig.globalScale;
	const float rowWidth = 40.0f * queuedConfig.globalScale;
    const float rowWidth2 = 0.5f * queuedConfig.globalScale;
    

	auto& activePos1P = *reinterpret_cast<ImVec2*>(&activeConfig.barsData[0].pos);
	auto& queuedPos1P = *reinterpret_cast<ImVec2*>(&queuedConfig.barsData[0].pos);

	auto& activePos2P = *reinterpret_cast<ImVec2*>(&activeConfig.barsData[1].pos);
	auto& queuedPos2P = *reinterpret_cast<ImVec2*>(&queuedConfig.barsData[1].pos);

	auto& activePos3P = *reinterpret_cast<ImVec2*>(&activeConfig.barsData[2].pos);
	auto& queuedPos3P = *reinterpret_cast<ImVec2*>(&queuedConfig.barsData[2].pos);

	auto& activePos4P = *reinterpret_cast<ImVec2*>(&activeConfig.barsData[3].pos);
	auto& queuedPos4P = *reinterpret_cast<ImVec2*>(&queuedConfig.barsData[3].pos);

    ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);

	if (ImGui::BeginTable("BarsSettingsTable", 2)) {

        ImGui::PushItemWidth(itemWidth);

		ImGui::TableSetupColumn("c1", 0, columnWidth);
		ImGui::TableNextRow(0, rowWidth);
		//ImGui::TableNextColumn();

        for (int i = 0; i < 4; i++) {
			if (i == 2) {
				ImGui::TableNextRow(0, rowWidth);
				ImGui::TableNextRow(0, rowWidth2);
			}
            ImGui::TableNextColumn();

            
            std::string title = std::to_string(i + 1) + "P Position";
            ImGui::Text(title.c_str());

            auto& activePos = *reinterpret_cast<ImVec2*>(&activeConfig.barsData[i].pos);

			ImGui::PushItemWidth(itemWidth * 0.85f);
			if (GUI_Input2<float>("X", activeConfig.barsData[i].pos.x, queuedConfig.barsData[i].pos.x, 10, "%g", ImGuiInputTextFlags_EnterReturnsTrue)) {
                
				ImGui::SetWindowPos(barsNames[i], activePos);
			}
			ImGui::SameLine();
			if (GUI_Input2<float>("Y", activeConfig.barsData[i].pos.y, queuedConfig.barsData[i].pos.y, 10, "%g", ImGuiInputTextFlags_EnterReturnsTrue)) {
				ImGui::SetWindowPos(barsNames[i], activePos);
			}
			ImGui::PopItemWidth();
            
        }
        
		/*ImGui::Text("1P Position");
        //ImGui::Text("");

        ImGui::PushItemWidth(itemWidth);
        if (GUI_Input2<float>("X", activePos1P.x, queuedPos1P.x, 10, "%g", ImGuiInputTextFlags_EnterReturnsTrue)) {
            ImGui::SetWindowPos(barsNames[0], activePos1P);
        }
        ImGui::SameLine();
        if (GUI_Input2<float>("Y", activePos1P.y, queuedPos1P.y, 10, "%g", ImGuiInputTextFlags_EnterReturnsTrue)) {
            ImGui::SetWindowPos(barsNames[0], activePos1P);
        }
        ImGui::PopItemWidth();
			
		ImGui::TableNextColumn();

		ImGui::Text("2P Position");
        //ImGui::Text("");

        ImGui::PushItemWidth(itemWidth);
		if (GUI_Input2<float>("X", activePos2P.x, queuedPos2P.x, 10, "%g", ImGuiInputTextFlags_EnterReturnsTrue)) {
			ImGui::SetWindowPos(barsNames[1], activePos2P);
		}
        ImGui::SameLine();
		if (GUI_Input2<float>("Y", activePos2P.y, queuedPos2P.y, 10, "%g", ImGuiInputTextFlags_EnterReturnsTrue)) {
			ImGui::SetWindowPos(barsNames[1], activePos2P);
		}
        ImGui::PopItemWidth();

		ImGui::TableNextRow(0, rowWidth);
        ImGui::TableNextRow(0, rowWidth2);
        
		ImGui::TableNextColumn();
 

		ImGui::Text("3P Position");
        //ImGui::Text("");

		if (GUI_Input2<float>("X", activePos3P.x, queuedPos3P.x, 10, "%g", ImGuiInputTextFlags_EnterReturnsTrue)) {
			ImGui::SetWindowPos(barsNames[2], activePos3P);
		}
        ImGui::SameLine();
		if (GUI_Input2<float>("Y", activePos3P.y, queuedPos3P.y, 10, "%g", ImGuiInputTextFlags_EnterReturnsTrue)) {
			ImGui::SetWindowPos(barsNames[2], activePos3P);
		}

		ImGui::TableNextColumn();

		ImGui::Text("4P Position");
        //ImGui::Text("");

		if (GUI_Input2<float>("X", activePos4P.x, queuedPos4P.x, 10, "%g", ImGuiInputTextFlags_EnterReturnsTrue)) {
			ImGui::SetWindowPos(barsNames[3], activePos4P);
		}
        ImGui::SameLine();
		if (GUI_Input2<float>("Y", activePos4P.y, queuedPos4P.y, 10, "%g", ImGuiInputTextFlags_EnterReturnsTrue)) {
			ImGui::SetWindowPos(barsNames[3], activePos4P);
		}
        */
        

		ImGui::EndTable();

	}

    ImGui::PopFont();


//     for_all(playerIndex, PLAYER_COUNT) {
//         ImGui::Text("");
// 
//         GUI_SectionStart(playerIndexNames[playerIndex]);
// 
//        
// 
//         BarsSettingsFunction(barsNames[playerIndex], activeConfig.barsData[playerIndex], queuedConfig.barsData[playerIndex],
//             defaultConfig.barsData[playerIndex]);
//     }
}

void BarsSection(size_t defaultFontSize) {
    
	const float itemWidth = defaultFontSize * 8.0f;
    ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);

    GUI_Checkbox2("ADDITIONAL PLAYER BARS", activeConfig.showAdditionalBars, queuedConfig.showAdditionalBars);

    ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	ImGui::PopFont();
    UI::SeparatorEx(defaultFontSize * 23.35f);
	ImGui::Text("");


    ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);

    GUI_Checkbox2("Show 1P Bar", activeConfig.show1Pbar, queuedConfig.show1Pbar);

    ImGui::PopFont();
    ImGui::PopStyleColor();

	BarsSettings(defaultFontSize);

    
}

#pragma endregion

#pragma region Boss Rush

void BossRushSection(size_t defaultFontSize) {

	const float itemWidth = defaultFontSize * 8.0f;
	const float columnWidth = 0.5f * queuedConfig.globalScale;
	const float rowWidth = 30.0f * queuedConfig.globalScale;
    const float rowWidth2 = 40.0f * queuedConfig.globalScale;
    ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);

    GUI_Checkbox2("BOSS RUSH", activeConfig.BossRush.enable, queuedConfig.BossRush.enable);

	ImGui::PopFont();

	UI::SeparatorEx(defaultFontSize * 23.35f);


	ImGui::PushItemWidth(itemWidth);

    ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
    ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	ImGui::Text("");

// 	if (GUI_ResetButton()) {
// 		CopyMemory(&queuedConfig.BossRush, &defaultConfig.BossRush, sizeof(queuedConfig.BossRush));
// 		CopyMemory(&activeConfig.BossRush, &queuedConfig.BossRush, sizeof(activeConfig.BossRush));
// 	}

    if (ImGui::BeginTable("BossRushTable", 2)) {

        ImGui::TableSetupColumn("c1", 0, columnWidth);
        ImGui::TableNextRow(0, rowWidth);
        ImGui::TableNextColumn();

        ImGui::Text("Mission 5");

        ImGui::TableNextColumn();
        ImGui::Text("Mission 12");

        ImGui::TableNextRow(0, rowWidth);
        ImGui::TableNextColumn();

        GUI_Checkbox2("Skip Jester", activeConfig.BossRush.Mission5.skipJester, queuedConfig.BossRush.Mission5.skipJester);

        ImGui::TableNextColumn();
        GUI_Checkbox2("Skip Jester", activeConfig.BossRush.Mission12.skipJester, queuedConfig.BossRush.Mission12.skipJester);

		ImGui::TableNextRow(0, rowWidth2);
		ImGui::TableNextColumn();

        ImGui::TableNextColumn();
		GUI_Checkbox2(
			"Skip Geryon Part 1", activeConfig.BossRush.Mission12.skipGeryonPart1, queuedConfig.BossRush.Mission12.skipGeryonPart1);

		ImGui::TableNextRow(0, rowWidth);
		ImGui::TableNextColumn();

		ImGui::Text("Mission 17");

        ImGui::TableNextColumn();
		ImGui::Text("Mission 19");

		ImGui::TableNextRow(0, rowWidth);
		ImGui::TableNextColumn();

        GUI_Checkbox2("Skip Jester", activeConfig.BossRush.Mission17.skipJester, queuedConfig.BossRush.Mission17.skipJester);

        ImGui::TableNextColumn();

		GUI_Checkbox2(
			"Skip Arkham Part 1", activeConfig.BossRush.Mission19.skipArkhamPart1, queuedConfig.BossRush.Mission19.skipArkhamPart1);
        

        ImGui::EndTable();

    }
	
	ImGui::PopFont();
	ImGui::PopItemWidth();
    ImGui::PopStyleColor();
    
}

#pragma endregion

#pragma region Camera

const char* cameraAutoAdjustNames[] = {
    "Default",
    "Manual",
    "Disable",
};


void CameraSection(size_t defaultFontSize) {
   
	const float itemWidth = defaultFontSize * 8.0f;

	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);


	ImGui::Text("CAMERA OPTIONS");

	ImGui::PopFont();

	UI::SeparatorEx(defaultFontSize * 23.35f);

	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	ImGui::PushItemWidth(itemWidth);
	ImGui::Text("");

    float smallerComboMult = 0.6f;

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;

		if (ImGui::BeginTable("CameraOptionsTable", 2)) {

			ImGui::TableSetupColumn("b1", 0, columnWidth);
			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth);
			GUI_InputDefault2("FOV Multiplier", activeConfig.fovMultiplier, queuedConfig.fovMultiplier, defaultConfig.fovMultiplier, 0.1f, "%g",
				ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

            ImGui::PushItemWidth(itemWidth * smallerComboMult);
            UI::Combo2("Turning Sensitivity", cameraSensitivityNames, activeConfig.cameraSensitivity, queuedConfig.cameraSensitivity);
            ImGui::PopItemWidth();

			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

            ImGui::PushItemWidth(itemWidth * smallerComboMult);
            UI::Combo2("Follow-Up Speed", cameraFollowUpSpeedNames, activeConfig.cameraFollowUpSpeed, queuedConfig.cameraFollowUpSpeed);
            ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth  * smallerComboMult);
			UI::Combo2("Distance", cameraDistanceNames, activeConfig.cameraDistance, queuedConfig.cameraDistance);
			ImGui::SameLine();
			TooltipHelper("(?)", "Camera Distance relative to the player outside Lock-On.\n"
				"\n"
				"Dynamic Option adjusts based on whether player is airborne.");
			ImGui::PopItemWidth();

            ImGui::TableNextRow(0, rowWidth);
            ImGui::TableNextColumn();

            ImGui::PushItemWidth(itemWidth * smallerComboMult);
			UI::Combo2(
				"Lock-On Distance", cameraLockOnDistanceNames, activeConfig.cameraLockOnDistance, queuedConfig.cameraLockOnDistance);
			ImGui::SameLine();
			TooltipHelper("(?)", "Camera Distance relative to the player in Lock-On.\n"
				"\n"
				"Dynamic Option adjusts based on whether player is airborne.");
            ImGui::PopItemWidth();

            ImGui::TableNextColumn();

            ImGui::PushItemWidth(itemWidth);
            UI::Combo2("Vertical Tilt", cameraTiltNames, activeConfig.cameraTilt, queuedConfig.cameraTilt);
            ImGui::PopItemWidth();

			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

            ImGui::PushItemWidth(itemWidth * smallerComboMult);
            UI::Combo2<uint8>("Auto-Adjust", cameraAutoAdjustNames, activeConfig.cameraAutoAdjust, queuedConfig.cameraAutoAdjust);
            ImGui::PopItemWidth();

			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();
            
			GUI_Checkbox2("Locked-Off Camera", activeConfig.cameraLockOff, queuedConfig.cameraLockOff);
			ImGui::SameLine();
			TooltipHelper("(?)", "Allows you to freely rotate the camera using the right stick in Third-Person View sections.");
         
            ImGui::TableNextColumn();

			if (GUI_Checkbox2("Invert X", activeConfig.cameraInvertX, queuedConfig.cameraInvertX)) {
				Camera::ToggleInvertX(activeConfig.cameraInvertX);
			}

			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

            GUI_Checkbox2("Disable Center Camera", activeConfig.disableCenterCamera, queuedConfig.disableCenterCamera);

            ImGui::TableNextColumn();

			if (GUI_Checkbox2("Disable Boss Camera", activeConfig.disableBossCamera, queuedConfig.disableBossCamera)) {
				Camera::ToggleDisableBossCamera(activeConfig.disableBossCamera);
			}

            


			ImGui::EndTable();
		}

// 	if (GUI_ResetButton()) {
// 		CopyMemory(&queuedConfig.cameraInvertX, &defaultConfig.cameraInvertX, sizeof(queuedConfig.cameraInvertX));
// 		CopyMemory(&activeConfig.cameraInvertX, &queuedConfig.cameraInvertX, sizeof(activeConfig.cameraInvertX));
// 
// 		CopyMemory(&queuedConfig.cameraAutoAdjust, &defaultConfig.cameraAutoAdjust, sizeof(queuedConfig.cameraAutoAdjust));
// 		CopyMemory(&activeConfig.cameraAutoAdjust, &queuedConfig.cameraAutoAdjust, sizeof(activeConfig.cameraAutoAdjust));
// 
// 		CopyMemory(&queuedConfig.disableCenterCamera, &defaultConfig.disableCenterCamera, sizeof(queuedConfig.disableCenterCamera));
// 		CopyMemory(&activeConfig.disableCenterCamera, &queuedConfig.disableCenterCamera, sizeof(activeConfig.disableCenterCamera));
// 
// 		CopyMemory(&queuedConfig.disableBossCamera, &defaultConfig.disableBossCamera, sizeof(queuedConfig.disableBossCamera));
// 		CopyMemory(&activeConfig.disableBossCamera, &queuedConfig.disableBossCamera, sizeof(activeConfig.disableBossCamera));
// 
// 		CopyMemory(&queuedConfig.fovMultiplier, &defaultConfig.fovMultiplier, sizeof(queuedConfig.fovMultiplier));
// 		CopyMemory(&activeConfig.fovMultiplier, &queuedConfig.fovMultiplier, sizeof(activeConfig.fovMultiplier));
// 
// 
// 		Camera::ToggleInvertX(activeConfig.cameraInvertX);
// 		Camera::ToggleDisableBossCamera(activeConfig.disableBossCamera);
	}
	


	ImGui::Text("");
    ImGui::PopFont();


	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);


	ImGui::Text("LIVE READINGS");

	ImGui::PopFont();

	UI::SeparatorEx(defaultFontSize * 23.35f);

	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	

	[&]() {
		if (g_scene != SCENE::GAME) {
			return;
		}

		auto pool_4449 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC8FBD0);
		if (!pool_4449 || !pool_4449[147]) {
			return;
		}
		auto& cameraData = *reinterpret_cast<CameraData*>(pool_4449[147]);


		ImGui::PushItemWidth(itemWidth * 0.8f);

        {
            const float columnWidth = 0.5f * queuedConfig.globalScale;
            const float rowWidth = 40.0f * queuedConfig.globalScale;

            if (ImGui::BeginTable("LiveCameraReadingsTable", 2)) {

                ImGui::TableSetupColumn("b1", 0, columnWidth);
                ImGui::TableNextRow(0, rowWidth);
                ImGui::TableNextColumn();
                ImGui::PushItemWidth(itemWidth * 0.8f);
				GUI_Input("FOV", cameraData.fov, 0.1f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                ImGui::PopItemWidth();
				ImGui::Text("");

				ImGui::TableNextRow(0, rowWidth);
                ImGui::TableNextColumn();

                for (int index = 0; index < countof(cameraData.data); index++) {
                    if (index > 0) {
                        ImGui::TableNextColumn();
                    }
                    
                    ImGui::PushItemWidth(itemWidth * 0.8f);
                    ImGui::Text(dataNames[index]);
                    GUI_Input("X", cameraData.data[index].x, 10.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                    GUI_Input("Y", cameraData.data[index].y, 10.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                    GUI_Input("Z", cameraData.data[index].z, 10.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                    GUI_Input("A", cameraData.data[index].a, 10.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                    ImGui::Text("");
                    ImGui::PopItemWidth();
                }

                ImGui::TableNextRow(0, rowWidth);
                ImGui::TableNextColumn();


				GUI_Input("Height", cameraData.height, 10.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);

                ImGui::TableNextColumn();


				GUI_Input("Tilt", cameraData.tilt, 0.05f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);


				ImGui::TableNextRow(0, rowWidth);
				ImGui::TableNextColumn();

				GUI_Input("Distance", cameraData.distance, 10.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);

                ImGui::TableNextColumn();


				GUI_Input("Distance Lock-On", cameraData.distanceLockOn, 10.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);



                ImGui::EndTable();
            }
        }

		ImGui::PopItemWidth();
	}();
        ImGui::PopStyleColor();
        ImGui::PopFont();

        ImGui::Text("");
    
}

#pragma endregion

#pragma region Cosmetics

struct {
    float32 airHike[5][4] = {
        {128, 0, 0, 200},
        {96, 128, 144, 200},
        {160, 64, 16, 200},
        {112, 64, 160, 200},
        {128, 128, 128, 200},
    };
    struct {
        float32 skyStar[4] = {255, 0, 0, 200};
    } Trickster;
    struct {
        float32 ultimate[4] = {143, 112, 48, 200};
    } Royalguard;
    struct {
        float32 clone[4] = {16, 16, 16, 48};
    } Doppelganger;
    struct {
        float32 dante[5][4] = {
            {128, 0, 0, 200},
            {96, 128, 144, 200},
            {160, 64, 16, 200},
            {112, 64, 160, 200},
            {128, 128, 128, 200},
        };
        float32 sparda[4]    = {128, 0, 0, 200};
        float32 vergil[3][4] = {
            {32, 64, 128, 200},
            {32, 64, 128, 200},
            {32, 64, 128, 200},
        };
        float32 neroAngelo[4] = {64, 0, 255, 200};
    } Aura;


    struct {
		float32 flux[6][4] = {
			{ 29, 29, 0, 255 }, //trick  
			{ 26, 0, 0, 255 }, //sword  
			{ 0, 8, 34, 255 }, //gun    
			{ 0, 35, 6, 255 }, //royal  
			{ 26, 0, 35, 255 }, //quick  
			{ 30, 14, 0, 255 }, //doppel 
		};

        float32 text[9][4] = {
			{ 240, 240, 0, 255 }, //trick  
			{ 255, 1, 1, 255 }, //sword  
			{ 0, 56, 239, 255 }, //gun    
			{ 5, 250, 47, 255 }, //royal  
			{ 189, 0, 255, 255 }, //quick  
			{ 255, 121, 4, 255 }, //doppel 
			{ 255, 255, 255, 255 }, //dt     
			{ 255, 255, 255, 255 }, //dte    
			{ 255, 255, 255, 255 }, //ready  
        };

		uint8 textMidnight[9][4] = {
			{ 155, 85, 250, 255 }, //trick  
			{ 155, 85, 250, 255 }, //sword  
			{ 155, 85, 250, 255 }, //gun    
			{ 155, 85, 250, 255 }, //royal  
			{ 155, 85, 250, 255 }, //quick  
			{ 155, 85, 250, 255 }, //doppel 
			{ 155, 85, 250, 255 }, //dt     
			{ 155, 85, 250, 255 }, //dte    
			{ 155, 85, 250, 255 }, //ready  
		};

    } StyleSwitchColor;
} Color;

void Color_UpdateValues() {
    // LogFunction();

    constexpr uint8 itemCount = (sizeof(Color) / 4);

    auto items  = reinterpret_cast<uint8*>(activeConfig.Color.airHike);
    auto items2 = reinterpret_cast<float32*>(Color.airHike);
  

    old_for_all(uint8, index, itemCount) {
        items2[index] = (static_cast<float32>(items[index]) / 255);
        
    }

	for (int style = 0; style < 6; style++) {
		for (int i = 0; i < 4; i++) {
			Color.StyleSwitchColor.flux[style][i] = (float32)activeConfig.StyleSwitchColor.flux[style][i] / 255;
		}
	}

    for (int style = 0; style < 9; style++) {
        for (int i = 0; i < 4; i++) {
            Color.StyleSwitchColor.text[style][i] = (float32)activeConfig.StyleSwitchColor.text[style][i] / 255;
        }
    }

}


#pragma endregion

#pragma region Damage

void Damage() {
    if (ImGui::CollapsingHeader("Damage")) {
        ImGui::Text("");

        if (GUI_ResetButton()) {
            CopyMemory(&queuedConfig.damagePlayerActorMultiplier, &defaultConfig.damagePlayerActorMultiplier,
                sizeof(queuedConfig.damagePlayerActorMultiplier));
            CopyMemory(&activeConfig.damagePlayerActorMultiplier, &queuedConfig.damagePlayerActorMultiplier,
                sizeof(activeConfig.damagePlayerActorMultiplier));

            CopyMemory(&queuedConfig.damageEnemyActorMultiplier, &defaultConfig.damageEnemyActorMultiplier,
                sizeof(queuedConfig.damageEnemyActorMultiplier));
            CopyMemory(&activeConfig.damageEnemyActorMultiplier, &queuedConfig.damageEnemyActorMultiplier,
                sizeof(activeConfig.damageEnemyActorMultiplier));

            CopyMemory(&queuedConfig.damageStyleRank, &defaultConfig.damageStyleRank, sizeof(queuedConfig.damageStyleRank));
            CopyMemory(&activeConfig.damageStyleRank, &queuedConfig.damageStyleRank, sizeof(activeConfig.damageStyleRank));
        }
        ImGui::Text("");

        TooltipHelper("(?)", "Multiplier values are for RECEIVED damage.");
        ImGui::Text("");


        {
            static bool toggled = false;

            if (GUI_Button("One Hit Kill")) {

                if (!toggled) {
                    toggled = true;

                    activeConfig.damagePlayerActorMultiplier = queuedConfig.damagePlayerActorMultiplier = 1.0f;
                    activeConfig.damageEnemyActorMultiplier = queuedConfig.damageEnemyActorMultiplier = 100.0f;
                } else {
                    toggled = false;

                    activeConfig.damagePlayerActorMultiplier = queuedConfig.damagePlayerActorMultiplier = 0;
                    activeConfig.damageEnemyActorMultiplier = queuedConfig.damageEnemyActorMultiplier = 0;
                }
            }
        }


        ImGui::PushItemWidth(150);

        GUI_InputDefault2("Player Actor Multiplier", activeConfig.damagePlayerActorMultiplier, queuedConfig.damagePlayerActorMultiplier,
            defaultConfig.damagePlayerActorMultiplier, 0.1f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);

        GUI_InputDefault2("Enemy Actor Multiplier", activeConfig.damageEnemyActorMultiplier, queuedConfig.damageEnemyActorMultiplier,
            defaultConfig.damageEnemyActorMultiplier, 0.1f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);

        UI::Combo2("Style Rank", styleRankNames, activeConfig.damageStyleRank, queuedConfig.damageStyleRank);

        ImGui::PopItemWidth();

        ImGui::Text("");
    }
}

#pragma endregion

#pragma region Dante

// @Todo: Move.
template <typename T>
void ActionData(const char* label, T (&vars)[2], T (&vars2)[2], T (&defaultVars)[2], T step = 1, const char* format = 0,
    ImGuiInputTextFlags flags = 0, float32 width = 150) {
    auto& style = ImGui::GetStyle();

    ImGui::PushItemWidth(width);
    GUI_InputDefault2("", vars[0], vars2[0], defaultVars[0], step, format, flags);
    ImGui::SameLine(0, style.ItemInnerSpacing.x);
    GUI_InputDefault2(label, vars[1], vars2[1], defaultVars[1], step, format, flags);
    ImGui::PopItemWidth();
}

void Dante() {
    if (ImGui::CollapsingHeader("Dante")) {
        ImGui::Text("");

        if (GUI_ResetButton()) {
            CopyMemory(&queuedConfig.airHikeCoreAbility, &defaultConfig.airHikeCoreAbility, sizeof(queuedConfig.airHikeCoreAbility));
            CopyMemory(&activeConfig.airHikeCoreAbility, &queuedConfig.airHikeCoreAbility, sizeof(activeConfig.airHikeCoreAbility));

            ToggleAirHikeCoreAbility(activeConfig.airHikeCoreAbility);

            CopyMemory(&queuedConfig.Royalguard, &defaultConfig.Royalguard, sizeof(queuedConfig.Royalguard));
            CopyMemory(&activeConfig.Royalguard, &queuedConfig.Royalguard, sizeof(activeConfig.Royalguard));

            ToggleRoyalguardForceJustFrameRelease(activeConfig.Royalguard.forceJustFrameRelease);

            CopyMemory(&queuedConfig.rebellionInfiniteShredder, &defaultConfig.rebellionInfiniteShredder,
                sizeof(queuedConfig.rebellionInfiniteShredder));
            CopyMemory(&activeConfig.rebellionInfiniteShredder, &queuedConfig.rebellionInfiniteShredder,
                sizeof(activeConfig.rebellionInfiniteShredder));

            CopyMemory(&queuedConfig.Rebellion, &defaultConfig.Rebellion, sizeof(queuedConfig.Rebellion));
            CopyMemory(&activeConfig.Rebellion, &queuedConfig.Rebellion, sizeof(activeConfig.Rebellion));

            ToggleRebellionInfiniteShredder(activeConfig.rebellionInfiniteShredder);
            ToggleRebellionInfiniteSwordPierce(activeConfig.Rebellion.infiniteSwordPierce);

            CopyMemory(&queuedConfig.enableRebellionAirStinger, &defaultConfig.enableRebellionAirStinger,
                sizeof(queuedConfig.enableRebellionAirStinger));
            CopyMemory(&activeConfig.enableRebellionAirStinger, &queuedConfig.enableRebellionAirStinger,
                sizeof(activeConfig.enableRebellionAirStinger));

            CopyMemory(&queuedConfig.enableRebellionNewDrive, &defaultConfig.enableRebellionNewDrive,
                sizeof(queuedConfig.enableRebellionNewDrive));
            CopyMemory(
                &activeConfig.enableRebellionNewDrive, &queuedConfig.enableRebellionNewDrive, sizeof(activeConfig.enableRebellionNewDrive));

            CopyMemory(&queuedConfig.enableRebellionQuickDrive, &defaultConfig.enableRebellionQuickDrive,
                sizeof(queuedConfig.enableRebellionQuickDrive));
            CopyMemory(&activeConfig.enableRebellionQuickDrive, &queuedConfig.enableRebellionQuickDrive,
                sizeof(activeConfig.enableRebellionQuickDrive));


            CopyMemory(&queuedConfig.rebellionHoldDrive, &defaultConfig.rebellionHoldDrive, sizeof(queuedConfig.rebellionHoldDrive));
            CopyMemory(&activeConfig.rebellionHoldDrive, &queuedConfig.rebellionHoldDrive, sizeof(activeConfig.rebellionHoldDrive));

            ToggleRebellionHoldDrive(activeConfig.rebellionHoldDrive);

            CopyMemory(&queuedConfig.enableCerberusAirRevolver, &defaultConfig.enableCerberusAirRevolver,
                sizeof(queuedConfig.enableCerberusAirRevolver));
            CopyMemory(&activeConfig.enableCerberusAirRevolver, &queuedConfig.enableCerberusAirRevolver,
                sizeof(activeConfig.enableCerberusAirRevolver));


            CopyMemory(&queuedConfig.enableNevanNewVortex, &defaultConfig.enableNevanNewVortex, sizeof(queuedConfig.enableNevanNewVortex));
            CopyMemory(&activeConfig.enableNevanNewVortex, &queuedConfig.enableNevanNewVortex, sizeof(activeConfig.enableNevanNewVortex));


            CopyMemory(&queuedConfig.EbonyIvory, &defaultConfig.EbonyIvory, sizeof(queuedConfig.EbonyIvory));
            CopyMemory(&activeConfig.EbonyIvory, &queuedConfig.EbonyIvory, sizeof(activeConfig.EbonyIvory));

            ToggleEbonyIvoryFoursomeTime(activeConfig.EbonyIvory.foursomeTime);
            ToggleEbonyIvoryInfiniteRainStorm(activeConfig.EbonyIvory.infiniteRainStorm);

            CopyMemory(&queuedConfig.Artemis, &defaultConfig.Artemis, sizeof(queuedConfig.Artemis));
            CopyMemory(&activeConfig.Artemis, &queuedConfig.Artemis, sizeof(activeConfig.Artemis));

            ToggleArtemisSwapNormalShotAndMultiLock(activeConfig.Artemis.swapNormalShotAndMultiLock);
            ToggleArtemisInstantFullCharge(activeConfig.Artemis.instantFullCharge);
        }

        GUI_SectionEnd();
        ImGui::Text("");

        GUI_SectionStart("Air Hike");

        if (GUI_Checkbox2("Core Ability", activeConfig.airHikeCoreAbility, queuedConfig.airHikeCoreAbility)) {
            ToggleAirHikeCoreAbility(activeConfig.airHikeCoreAbility);
        }
        ImGui::SameLine();
        TooltipHelper("(?)", "Makes Air Hike available for all melee weapons.");

        GUI_SectionEnd();
        ImGui::Text("");

        GUI_SectionStart("Royalguard");

        if (GUI_Checkbox2(
                "Force Just Frame Release", activeConfig.Royalguard.forceJustFrameRelease, queuedConfig.Royalguard.forceJustFrameRelease)) {
            ToggleRoyalguardForceJustFrameRelease(activeConfig.Royalguard.forceJustFrameRelease);
        }

        GUI_SectionEnd();
        ImGui::Text("");


        ImGui::Text("Rebellion");
        ImGui::Text("");

        if (GUI_Checkbox2("Infinite Shredder", activeConfig.rebellionInfiniteShredder, queuedConfig.rebellionInfiniteShredder)) {
            ToggleRebellionInfiniteShredder(activeConfig.rebellionInfiniteShredder);
        }

        if (GUI_Checkbox2(
                "Infinite Sword Pierce", activeConfig.Rebellion.infiniteSwordPierce, queuedConfig.Rebellion.infiniteSwordPierce)) {
            ToggleRebellionInfiniteSwordPierce(activeConfig.Rebellion.infiniteSwordPierce);
        }

        GUI_Checkbox2("Enable Air Stinger", activeConfig.enableRebellionAirStinger, queuedConfig.enableRebellionAirStinger);
        ImGui::SameLine();
        TooltipHelper("(?)", "Requires enabled Actor module.");

        GUI_Checkbox2("Enable New Drive", activeConfig.enableRebellionNewDrive, queuedConfig.enableRebellionNewDrive);
        ImGui::SameLine();
        TooltipHelper("(?)", "Requires enabled Actor module.\n"
                             "\n"
                             "Press Lock-On + Left + Melee Attack.");

        GUI_Checkbox2("Enable Quick Drive", activeConfig.enableRebellionQuickDrive, queuedConfig.enableRebellionQuickDrive);
        ImGui::SameLine();
        TooltipHelper("(?)", "Requires enabled Actor module.\n"
                             "\n"
                             "In Swordmaster hold Style Action and press Melee Attack twice.");

        if (GUI_Checkbox2("Hold Drive", activeConfig.rebellionHoldDrive, queuedConfig.rebellionHoldDrive)) {
            ToggleRebellionHoldDrive(activeConfig.rebellionHoldDrive);
        }
        ImGui::Text("");


        TooltipHelper("(?)", "Requires enabled Actor module.\n"
                             "\n"
                             "Left : Human\n"
                             "Right: Devil");
        ImGui::Text("");

        ActionData("Stinger Duration", activeConfig.Rebellion.stingerDuration, queuedConfig.Rebellion.stingerDuration,
            defaultConfig.Rebellion.stingerDuration, 1.0f, "%g");
        ActionData("Stinger Range", activeConfig.Rebellion.stingerRange, queuedConfig.Rebellion.stingerRange,
            defaultConfig.Rebellion.stingerRange, 10.0f, "%g");


        {
            bool condition = !activeConfig.enableRebellionAirStinger;

            GUI_PushDisable(condition);

            ActionData("Air Stinger Count", activeConfig.Rebellion.airStingerCount, queuedConfig.Rebellion.airStingerCount,
                defaultConfig.Rebellion.airStingerCount);
            ActionData("Air Stinger Duration", activeConfig.Rebellion.airStingerDuration, queuedConfig.Rebellion.airStingerDuration,
                defaultConfig.Rebellion.airStingerDuration, 1.0f, "%g");
            ActionData("Air Stinger Range", activeConfig.Rebellion.airStingerRange, queuedConfig.Rebellion.airStingerRange,
                defaultConfig.Rebellion.airStingerRange, 10.0f, "%g");

            GUI_PopDisable(condition);
        }


        GUI_SectionEnd();
        ImGui::Text("");


        GUI_SectionStart("Cerberus");

        GUI_Checkbox2("Enable Air Revolver", activeConfig.enableCerberusAirRevolver, queuedConfig.enableCerberusAirRevolver);
        ImGui::SameLine();
        TooltipHelper("(?)", "Requires enabled Actor module.");

        GUI_SectionEnd();
        ImGui::Text("");


        GUI_SectionStart("Nevan");

        GUI_Checkbox2("Enable New Vortex", activeConfig.enableNevanNewVortex, queuedConfig.enableNevanNewVortex);
        ImGui::SameLine();
        TooltipHelper("(?)", "Requires enabled Actor module.\n"
                             "\n"
                             "While in devil form press Any Direction + Melee Attack.");

        GUI_SectionEnd();
        ImGui::Text("");


        GUI_SectionStart("Ebony & Ivory");

        if (GUI_Checkbox2("Foursome Time", activeConfig.EbonyIvory.foursomeTime, queuedConfig.EbonyIvory.foursomeTime)) {
            ToggleEbonyIvoryFoursomeTime(activeConfig.EbonyIvory.foursomeTime);
        }
        ImGui::SameLine();
        TooltipHelper("(?)", "Twosome Time fires 2 additional shots.");

        if (GUI_Checkbox2("Infinite Rain Storm", activeConfig.EbonyIvory.infiniteRainStorm, queuedConfig.EbonyIvory.infiniteRainStorm)) {
            ToggleEbonyIvoryInfiniteRainStorm(activeConfig.EbonyIvory.infiniteRainStorm);
        }

        GUI_SectionEnd();
        ImGui::Text("");

        GUI_SectionStart("Artemis");

        if (GUI_Checkbox2("Swap Normal Shot and Multi Lock", activeConfig.Artemis.swapNormalShotAndMultiLock,
                queuedConfig.Artemis.swapNormalShotAndMultiLock)) {
            ToggleArtemisSwapNormalShotAndMultiLock(activeConfig.Artemis.swapNormalShotAndMultiLock);
        }

        if (GUI_Checkbox2("Instant Full Charge", activeConfig.Artemis.instantFullCharge, queuedConfig.Artemis.instantFullCharge)) {
            ToggleArtemisInstantFullCharge(activeConfig.Artemis.instantFullCharge);
        }

        ImGui::Text("");
    }
}

#pragma endregion

#pragma region Debug

// @Order

bool showFileDataWindow    = false;
bool showRegionDataWindow  = false;
bool showSoundWindow       = false;
bool showMissionDataWindow = false;
bool showActorWindow       = false;
bool showExpWindow         = false;
bool showEventDataWindow   = false;

const char* tabNames[] = {
    "Items",
    "Dante",
    "Vergil",
};

// @Research: Shop namespace


// @Rename: ShopExperienceHelper

struct ShopHelper {
    const char* name;
    uint32 price;
    int64 last;
    int64 next;
};

ShopHelper shopHelpersDante[] = {
    {"Rebellion Stinger Level 1", 2500, -1, UNLOCK_DANTE::REBELLION_STINGER_LEVEL_2},
    {"Rebellion Stinger Level 2", 10000, UNLOCK_DANTE::REBELLION_STINGER_LEVEL_1, -1},
    {"Rebellion Drive", 10000, -1, -1},
    {"Rebellion Air Hike", 20000, -1, -1},
    {"Cerberus Revolver Level 2", 15000, -1, -1},
    {"Cerberus Windmill", 7500, -1, -1},
    {"Agni & Rudra Jet-Stream Level 2", 10000, -1, UNLOCK_DANTE::AGNI_RUDRA_JET_STREAM_LEVEL_3},
    {"Agni & Rudra Jet-Stream Level 3", 15000, UNLOCK_DANTE::AGNI_RUDRA_JET_STREAM_LEVEL_2, -1},
    {"Agni & Rudra Whirlwind", 7500, -1, -1},
    {"Agni & Rudra Air Hike", 20000, -1, -1},
    {"Nevan Reverb Shock Level 1", 7500, -1, UNLOCK_DANTE::NEVAN_REVERB_SHOCK_LEVEL_2},
    {"Nevan Reverb Shock Level 2", 15000, UNLOCK_DANTE::NEVAN_REVERB_SHOCK_LEVEL_1, -1},
    {"Nevan Bat Rift Level 2", 10000, -1, -1},
    {"Nevan Air Raid", 20000, -1, -1},
    {"Nevan Volume Up", 20000, -1, -1},
    {"Beowulf Straight Level 2", 10000, -1, -1},
    {"Beowulf Beast Uppercut", 7500, -1, UNLOCK_DANTE::BEOWULF_RISING_DRAGON},
    {"Beowulf Rising Dragon", 15000, UNLOCK_DANTE::BEOWULF_BEAST_UPPERCUT, -1},
    {"Beowulf Air Hike", 20000, -1, -1},
    {"Ebony & Ivory Level 2", 5000, -1, UNLOCK_DANTE::EBONY_IVORY_LEVEL_3},
    {"Ebony & Ivory Level 3", 10000, UNLOCK_DANTE::EBONY_IVORY_LEVEL_2, -1},
    {"Shotgun Level 2", 10000, -1, UNLOCK_DANTE::SHOTGUN_LEVEL_3},
    {"Shotgun Level 3", 20000, UNLOCK_DANTE::SHOTGUN_LEVEL_2, -1},
    {"Artemis Level 2", 10000, -1, UNLOCK_DANTE::ARTEMIS_LEVEL_3},
    {"Artemis Level 3", 20000, UNLOCK_DANTE::ARTEMIS_LEVEL_2, -1},
    {"Spiral Level 2", 7500, -1, UNLOCK_DANTE::SPIRAL_LEVEL_3},
    {"Spiral Level 3", 15000, UNLOCK_DANTE::SPIRAL_LEVEL_2, -1},
    {"Kalina Ann Level 2", 5000, -1, UNLOCK_DANTE::KALINA_ANN_LEVEL_3},
    {"Kalina Ann Level 3", 10000, UNLOCK_DANTE::KALINA_ANN_LEVEL_2, -1},
};

ShopHelper shopHelpersVergil[] = {
    {"Yamato Rapid Slash Level 1", 5000, -1, UNLOCK_VERGIL::YAMATO_RAPID_SLASH_LEVEL_2},
    {"Yamato Rapid Slash Level 2", 15000, UNLOCK_VERGIL::YAMATO_RAPID_SLASH_LEVEL_1, -1},
    {"Yamato Judgement Cut Level 1", 10000, -1, UNLOCK_VERGIL::YAMATO_JUDGEMENT_CUT_LEVEL_2},
    {"Yamato Judgement Cut Level 2", 20000, UNLOCK_VERGIL::YAMATO_JUDGEMENT_CUT_LEVEL_1, -1},
    {"Beowulf Starfall Level 2", 7500, -1, -1},
    {"Beowulf Rising Sun", 5000, -1, -1},
    {"Beowulf Lunar Phase Level 2", 15000, -1, -1},
    {"Yamato & Force Edge Helm Breaker Level 2", 13000, -1, -1},
    {"Yamato & Force Edge Stinger Level 1", 5000, -1, UNLOCK_VERGIL::YAMATO_FORCE_EDGE_STINGER_LEVEL_2},
    {"Yamato & Force Edge Stinger Level 2", 10000, UNLOCK_VERGIL::YAMATO_FORCE_EDGE_STINGER_LEVEL_1, -1},
    {"Yamato & Force Edge Round Trip", 10000, -1, -1},
    {"Summoned Swords Level 2", 7500, -1, UNLOCK_VERGIL::SUMMONED_SWORDS_LEVEL_3},
    {"Summoned Swords Level 3", 15000, UNLOCK_VERGIL::SUMMONED_SWORDS_LEVEL_2, -1},
    {"Spiral Swords", 20000, -1, -1},
};


// @Rename: ShopItemHelper.
struct ItemHelper {
    const uint8 itemIndex;
    const uint8 buyIndex;
    const uint8 itemCount;
    const uint32* prices;
    const uint8 priceCount;
};

// @Research: Moving to Vars causes internal compiler error in MSVC.
constexpr ItemHelper itemHelpers[] = {
    {ITEM::VITAL_STAR_SMALL, BUY::VITAL_STAR_SMALL, 30, itemVitalStarSmallPrices, static_cast<uint8>(countof(itemVitalStarSmallPrices))},
    {ITEM::VITAL_STAR_LARGE, BUY::VITAL_STAR_LARGE, 30, itemVitalStarLargePrices, static_cast<uint8>(countof(itemVitalStarLargePrices))},
    {ITEM::DEVIL_STAR, BUY::DEVIL_STAR, 10, itemDevilStarPrices, static_cast<uint8>(countof(itemDevilStarPrices))},
    {ITEM::HOLY_WATER, BUY::HOLY_WATER, 30, itemHolyWaterPrices, static_cast<uint8>(countof(itemHolyWaterPrices))},
    {ITEM::BLUE_ORB, BUY::BLUE_ORB, 6, itemBlueOrbPrices, static_cast<uint8>(countof(itemBlueOrbPrices))},
    {ITEM::PURPLE_ORB, BUY::PURPLE_ORB, 7, itemPurpleOrbPrices, static_cast<uint8>(countof(itemPurpleOrbPrices))},
    {ITEM::GOLD_ORB, BUY::GOLD_ORB, 3, itemGoldOrbPrices, static_cast<uint8>(countof(itemGoldOrbPrices))},
    {ITEM::YELLOW_ORB, BUY::YELLOW_ORB, 99, itemYellowOrbPrices, static_cast<uint8>(countof(itemYellowOrbPrices))},
};


namespace ITEM_HELPER {
enum {
    BLUE_ORB = 4,
    PURPLE_ORB,
};
};


void ShopWindow() {


    if (!g_showShop) {
        return;
    }


    auto name_5681 = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E30);
    if (!name_5681) {
        return;
    }
    auto& missionData = *reinterpret_cast<MissionData*>(name_5681);

    auto name_5688 = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E30);
    if (!name_5688) {
        return;
    }
    auto& queuedMissionActorData = *reinterpret_cast<QueuedMissionActorData*>(name_5688 + 0xC0);
    auto& activeMissionActorData = *reinterpret_cast<ActiveMissionActorData*>(name_5688 + 0x16C);


    bool unlockDevilTrigger = (activeMissionActorData.maxMagicPoints >= 3000);


    static bool run = false;
    if (!run) {
        run = true;


        constexpr float width  = 700; // 900
        constexpr float height = 700; // 500


        ImGui::SetNextWindowSize(ImVec2(width, height));
        ImGui::SetNextWindowPos(ImVec2(((g_renderSize.x - width) / 2), ((g_renderSize.y - height) / 2)));
    }


    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);


    if (ImGui::Begin("ShopWindow", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize)) {


        if (GUI_Button("Close")) {
            CloseShop();
        }
        ImGui::Text("");


        ImGui::Text("%u Red Orbs", missionData.redOrbs);
        ImGui::Text("");


        if (ImGui::BeginTabBar("ShopTabs")) {


            for_all(tabIndex, TAB::COUNT) {
                if (ImGui::BeginTabItem(tabNames[tabIndex])) {


                    ImGui::Text("");


                    auto Function = [&](


                                        ExpConfig::ExpData& expData,

                                        ShopHelper* helpers, new_size_t helperCount) {
                        for_all(helperIndex, helperCount) {
                            auto& helper = helpers[helperIndex];


                            auto Buy = [&]() {
                                if (missionData.redOrbs < helper.price) {
                                    PlaySound(0, 19);

                                    return;
                                }

                                missionData.redOrbs -= helper.price;

                                PlaySound(0, 18);

                                expData.unlocks[helperIndex] = true;

                                ExpConfig::UpdatePlayerActorExps();
                            };

                            auto Sell = [&]() {
                                missionData.redOrbs += helper.price;

                                PlaySound(0, 18);

                                expData.unlocks[helperIndex] = false;

                                ExpConfig::UpdatePlayerActorExps();
                            };


                            {
                                // Already unlocked or previous action required and not unlocked yet.
                                bool condition = (expData.unlocks[helperIndex] || ((helper.last > -1) && !expData.unlocks[helper.last]));

                                GUI_PushDisable(condition);

                                ImGui::Text(helper.name);
                                ImGui::SameLine(300);

                                ImGui::Text("%u", helper.price);

                                GUI_PopDisable(condition);
                            }


                            ImGui::SameLine(400);


                            if (!expData.unlocks[helperIndex]) {
                                // Previous action required and not unlocked yet.
                                bool condition = ((helper.last > -1) && !expData.unlocks[helper.last]);

                                GUI_PushDisable(condition);

                                if (GUI_Button("Buy")) {
                                    Buy();
                                }

                                GUI_PopDisable(condition);
                            } else {
                                // Next action available and still unlocked.
                                bool condition = ((helper.next > -1) && expData.unlocks[helper.next]);

                                GUI_PushDisable(condition);

                                if (GUI_Button("Sell")) {
                                    Sell();
                                }

                                GUI_PopDisable(condition);
                            }
                        }
                    };


                    // auto saveIndex = g_saveIndex;
                    // if (saveIndex >= SAVE_COUNT)
                    // {
                    // 	saveIndex = 0;
                    // }


                    if (tabIndex == TAB::ITEMS) {


                        if constexpr (debug) {
                            ImGui::Text("unlockDevilTrigger %u", unlockDevilTrigger);
                            ImGui::Text("");
                        }


                        auto GetItemCount = [&](const ItemHelper& itemHelper) -> uint8& {
                            return missionData.itemCounts[itemHelper.itemIndex];
                        };

                        auto GetBuyCount = [&](const ItemHelper& itemHelper) -> uint8& {
                            return missionData.buyCounts[itemHelper.buyIndex];
                        };

                        auto GetPrice = [&](const ItemHelper& itemHelper) {
                            uint32 price = 0;

                            auto& buyCount = GetBuyCount(itemHelper);

                            if (buyCount >= itemHelper.priceCount) {
                                price = itemHelper.prices[(itemHelper.priceCount - 1)];
                            } else {
                                price = itemHelper.prices[buyCount];
                            }

                            return price;
                        };

                        auto BuyItem = [&](uint8 itemHelperIndex) {
                            auto& itemHelper = itemHelpers[itemHelperIndex];

                            auto& itemCount = GetItemCount(itemHelper);
                            auto& buyCount  = GetBuyCount(itemHelper);
                            auto price      = GetPrice(itemHelper);

                            if ((itemCount >= itemHelper.itemCount) || (missionData.redOrbs < price)) {
                                PlaySound(0, 19);

                                return;
                            }

                            itemCount++;
                            buyCount++;

                            if (buyCount > 254) {
                                buyCount = 254;
                            }

                            missionData.redOrbs -= price;

                            PlaySound(0, 18);


                            if (itemHelper.itemIndex == ITEM::BLUE_ORB) {
                                float value = (activeMissionActorData.maxHitPoints + 1000);

                                [&]() {
                                    if (!g_defaultNewActorData[0].baseAddr) {
                                        return;
                                    }
                                    auto& actorData = *reinterpret_cast<PlayerActorData*>(g_defaultNewActorData[0].baseAddr);

                                    value = (actorData.maxHitPoints + 1000);

                                    actorData.hitPoints = actorData.maxHitPoints = value;
                                }();

                                activeMissionActorData.hitPoints = activeMissionActorData.maxHitPoints = value;


                                old_for_all(uint8, playerIndex, PLAYER_COUNT) {
                                    old_for_all(uint8, characterIndex, CHARACTER_COUNT) {
                                        old_for_all(uint8, entityIndex, ENTITY_COUNT) {
                                            auto& playerData = GetPlayerData(playerIndex);

                                            auto& characterData = GetCharacterData(playerIndex, characterIndex, entityIndex);
                                            auto& newActorData  = GetNewActorData(playerIndex, characterIndex, entityIndex);

                                            auto& activeCharacterData =
                                                GetCharacterData(playerIndex, playerData.activeCharacterIndex, ENTITY::MAIN);
                                            auto& activeNewActorData =
                                                GetNewActorData(playerIndex, playerData.activeCharacterIndex, ENTITY::MAIN);

                                            auto& leadCharacterData = GetCharacterData(playerIndex, 0, ENTITY::MAIN);
                                            auto& leadNewActorData  = GetNewActorData(playerIndex, 0, ENTITY::MAIN);

                                            auto& mainCharacterData = GetCharacterData(playerIndex, characterIndex, ENTITY::MAIN);
                                            auto& mainNewActorData  = GetNewActorData(playerIndex, characterIndex, ENTITY::MAIN);


                                            if (!newActorData.baseAddr) {
                                                continue;
                                            }
                                            auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);

                                            actorData.hitPoints = actorData.maxHitPoints = value;
                                        }
                                    }
                                }
                            } else if (itemHelper.itemIndex == ITEM::PURPLE_ORB) {
                                float value = (activeMissionActorData.maxMagicPoints + 1000);

                                [&]() {
                                    if (!g_defaultNewActorData[0].baseAddr) {
                                        return;
                                    }
                                    auto& actorData = *reinterpret_cast<PlayerActorData*>(g_defaultNewActorData[0].baseAddr);

                                    value = (actorData.maxMagicPoints + 1000);

                                    actorData.magicPoints = actorData.maxMagicPoints = value;
                                }();

                                activeMissionActorData.magicPoints = activeMissionActorData.maxMagicPoints = value;


                                old_for_all(uint8, playerIndex, PLAYER_COUNT) {
                                    old_for_all(uint8, characterIndex, CHARACTER_COUNT) {
                                        old_for_all(uint8, entityIndex, ENTITY_COUNT) {
                                            auto& playerData = GetPlayerData(playerIndex);

                                            auto& characterData = GetCharacterData(playerIndex, characterIndex, entityIndex);
                                            auto& newActorData  = GetNewActorData(playerIndex, characterIndex, entityIndex);

                                            auto& activeCharacterData =
                                                GetCharacterData(playerIndex, playerData.activeCharacterIndex, ENTITY::MAIN);
                                            auto& activeNewActorData =
                                                GetNewActorData(playerIndex, playerData.activeCharacterIndex, ENTITY::MAIN);

                                            auto& leadCharacterData = GetCharacterData(playerIndex, 0, ENTITY::MAIN);
                                            auto& leadNewActorData  = GetNewActorData(playerIndex, 0, ENTITY::MAIN);

                                            auto& mainCharacterData = GetCharacterData(playerIndex, characterIndex, ENTITY::MAIN);
                                            auto& mainNewActorData  = GetNewActorData(playerIndex, characterIndex, ENTITY::MAIN);


                                            if (!newActorData.baseAddr) {
                                                continue;
                                            }
                                            auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);

                                            actorData.magicPoints = actorData.maxMagicPoints = value;
                                        }
                                    }
                                }
                            }
                        };


                        auto SellItem = [&](uint8 itemHelperIndex) {
                            auto& itemHelper = itemHelpers[itemHelperIndex];

                            auto& itemCount = GetItemCount(itemHelper);
                            auto& buyCount  = GetBuyCount(itemHelper);
                            uint32 price    = 0;


                            itemCount--;


                            if (buyCount > 0) {
                                buyCount--;
                            }


                            price = GetPrice(itemHelper);


                            missionData.redOrbs += price;

                            PlaySound(0, 18);
                        };


                        // Content


                        old_for_all(uint8, itemHelperIndex, countof(itemHelpers)) {
                            auto& itemHelper = itemHelpers[itemHelperIndex];

                            auto& itemCount = GetItemCount(itemHelper);
                            auto& buyCount  = GetBuyCount(itemHelper);
                            auto price      = GetPrice(itemHelper);


                            bool mainCondition = ((itemHelperIndex == ITEM_HELPER::PURPLE_ORB) && !unlockDevilTrigger);

                            GUI_PushDisable(mainCondition);


                            ImGui::Text("%s", itemNames[itemHelper.itemIndex]);
                            ImGui::SameLine(200);


                            ImGui::Text("%.2u / %.2u", itemCount, itemHelper.itemCount);
                            ImGui::SameLine(300);


                            ImGui::Text("%u", price);
                            ImGui::SameLine(400);


#define conditionExpr itemCount >= itemHelper.itemCount

                            bool condition = (conditionExpr);

                            GUI_PushDisable(condition);

                            if (GUI_Button("Buy")) {
                                BuyItem(itemHelperIndex);

                                if (conditionExpr) {
                                    ResetNavId();
                                }
                            }

                            GUI_PopDisable(condition);

#undef conditionExpr


                            [&]() {
                                if ((itemHelperIndex == ITEM_HELPER::BLUE_ORB) || (itemHelperIndex == ITEM_HELPER::PURPLE_ORB)) {
                                    return;
                                }


                                ImGui::SameLine();


#define conditionExpr itemCount < 1

                                bool condition = (conditionExpr);

                                GUI_PushDisable(condition);

                                if (GUI_Button("Sell")) {
                                    SellItem(itemHelperIndex);

                                    if (conditionExpr) {
                                        ResetNavId();
                                    }
                                }

                                GUI_PopDisable(condition);

#undef conditionExpr
                            }();


                            if constexpr (debug) {
                                ImGui::SameLine(500);
                                ImGui::Text("buyCount %u", buyCount);
                            }


                            GUI_PopDisable(mainCondition);
                        }
                        ImGui::Text("");


                    } else if (tabIndex == TAB::DANTE) {


                        Function(ExpConfig::missionExpDataDante, shopHelpersDante, countof(shopHelpersDante));


                    } else if (tabIndex == TAB::VERGIL) {

                        Function(ExpConfig::missionExpDataVergil, shopHelpersVergil, countof(shopHelpersVergil));
                    }


                    ImGui::EndTabItem();
                }
            }

            ImGui::EndTabBar();
        }


        GamepadClose(visibleShop, lastVisibleShop, CloseShop);


        ImGui::Text("");
    }

    ImGui::End();


    ImGui::PopStyleVar(1);
}


void ExpWindow() {
    if (!showExpWindow) {
        return;
    }


    static bool run = false;
    if (!run) {
        run = true;


        constexpr float width  = 700;
        constexpr float height = 700;


        ImGui::SetNextWindowSize(ImVec2(width, height));

        // // Center
        // ImGui::SetNextWindowPos
        // (
        // 	ImVec2
        // 	(
        // 		((g_renderSize.x - width ) / 2),
        // 		((g_renderSize.y - height) / 2)
        // 	)
        // );

        // Top Right
        ImGui::SetNextWindowPos(ImVec2((g_renderSize.x - width), 0));
    }


    if (ImGui::Begin("Exp Stuff", &showExpWindow)) {
        ImGui::Text("");


        if (GUI_Button("Save")) {
            ExpConfig::SaveExp();
        }
        ImGui::SameLine();

        if (GUI_Button("Load")) {
            ExpConfig::LoadExp();
        }
        ImGui::Text("");


        ImGui::PushItemWidth(150);

        GUI_Input<new_size_t>("g_saveIndex", g_saveIndex, 1, "%llu", ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::Text("");

        ImGui::PopItemWidth();


        if (GUI_Button("SavePlayerActorExp")) {
            ExpConfig::SavePlayerActorExp();
        }
        ImGui::SameLine();

        if (GUI_Button("UpdatePlayerActorExps")) {
            ExpConfig::UpdatePlayerActorExps();
        }
        ImGui::Text("");


        auto FunctionOnce = [&](ExpConfig::ExpData& expData, const char* name, ShopHelper* shopHelpers, new_size_t count) {
            if (!ImGui::CollapsingHeader(name)) {
                return;
            }
            ImGui::Text("");

            ImGui::PushItemWidth(200);


            ImGui::Text("styleLevels");
            for_all(styleIndex, STYLE::MAX) {
                GUI_Input<uint32>(indexNames[styleIndex], expData.styleLevels[styleIndex], 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
            }
            ImGui::Text("");


            ImGui::Text("styleExpPoints");
            for_all(styleIndex, STYLE::MAX) {
                GUI_Input<float>(indexNames[styleIndex], expData.styleExpPoints[styleIndex], 1, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
            }
            ImGui::Text("");


            ImGui::Text("unlocks");
            for_all(index, count) {
                GUI_Checkbox(shopHelpers[index].name, expData.unlocks[index]);
            }
            ImGui::Text("");


            ImGui::PopItemWidth();
        };


        auto FunctionLoop = [&](ExpConfig::ExpData* expDataAddr, const char** saveNames, ShopHelper* shopHelpers, new_size_t count) {
            for_all(saveIndex, SAVE_COUNT) {
                auto& expData = expDataAddr[saveIndex];

                FunctionOnce(expData, saveNames[saveIndex], shopHelpers, count);
            }
        };


        FunctionOnce(ExpConfig::missionExpDataDante, "Dante Mission", shopHelpersDante, countof(shopHelpersDante));
        FunctionOnce(ExpConfig::sessionExpDataDante, "Dante Session", shopHelpersDante, countof(shopHelpersDante));
        FunctionLoop(ExpConfig::savedExpDataDante, saveNamesDante, shopHelpersDante, countof(shopHelpersDante));
        ImGui::Text("");


        FunctionOnce(ExpConfig::missionExpDataVergil, "Vergil Mission", shopHelpersVergil, countof(shopHelpersVergil));
        FunctionOnce(ExpConfig::sessionExpDataVergil, "Vergil Session", shopHelpersVergil, countof(shopHelpersVergil));
        FunctionLoop(ExpConfig::savedExpDataVergil, saveNamesVergil, shopHelpersVergil, countof(shopHelpersVergil));
        ImGui::Text("");


        auto Content = [&](PlayerActorData& actorData) {
            ImGui::PushItemWidth(200);

            GUI_Input<uint32>("character", actorData.character, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);


            GUI_Input<uint32>("style", actorData.style, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);


            GUI_Input<uint32>("styleLevel", actorData.styleLevel, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);

            GUI_Input<float>("styleExpPoints", actorData.styleExpPoints, 1, "%g", ImGuiInputTextFlags_EnterReturnsTrue);


            ImGui::Text("activeExpertise");
            for_all(index, countof(actorData.activeExpertise)) {
                GUI_Input<byte32>(indexNames[index], actorData.activeExpertise[index], 1, "%X",
                    ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsHexadecimal);
            }


            ImGui::Text("newWeaponLevels");
            for_all(index, countof(actorData.newWeaponLevels)) {
                GUI_Input<uint32>(indexNames[index], actorData.newWeaponLevels[index], 1, "%u",
                    ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsHexadecimal);
            }


            ImGui::PopItemWidth();
        };


        [&]() {
            if (!ImGui::CollapsingHeader("Default")) {
                return;
            }
            ImGui::Text("");

            // IntroduceMainActorData(actorData, return);

            if (!g_defaultNewActorData[0].baseAddr) {
                return;
            }
            auto& actorData = *reinterpret_cast<PlayerActorData*>(g_defaultNewActorData[0].baseAddr);


            Content(actorData);
            ImGui::Text("");
        }();


        old_for_all(uint8, playerIndex, PLAYER_COUNT) {
            if (!ImGui::CollapsingHeader(playerIndexNames[playerIndex])) {
                continue;
            }

            ImGui::Indent(20);


            old_for_all(uint8, characterIndex, CHARACTER_COUNT) {
                if (!ImGui::CollapsingHeader(characterIndexNames[characterIndex])) {
                    continue;
                }
                ImGui::Text("");


                old_for_all(uint8, entityIndex, ENTITY_COUNT) {
                    auto& playerData = GetPlayerData(playerIndex);

                    auto& characterData = GetCharacterData(playerIndex, characterIndex, entityIndex);
                    auto& newActorData  = GetNewActorData(playerIndex, characterIndex, entityIndex);

                    auto& activeCharacterData = GetCharacterData(playerIndex, playerData.activeCharacterIndex, ENTITY::MAIN);
                    auto& activeNewActorData  = GetNewActorData(playerIndex, playerData.activeCharacterIndex, ENTITY::MAIN);

                    auto& leadCharacterData = GetCharacterData(playerIndex, 0, ENTITY::MAIN);
                    auto& leadNewActorData  = GetNewActorData(playerIndex, 0, ENTITY::MAIN);

                    auto& mainCharacterData = GetCharacterData(playerIndex, characterIndex, ENTITY::MAIN);
                    auto& mainNewActorData  = GetNewActorData(playerIndex, characterIndex, ENTITY::MAIN);


                    if (!newActorData.baseAddr) {
                        continue;
                    }
                    auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);

                    ImGui::Text(entityNames[entityIndex]);
                    Content(actorData);
                    ImGui::Text("");
                }
            }


            ImGui::Unindent(20);
        }


        ImGui::Text("");
    }

    ImGui::End();
}


void MissionDataWindow() {
    if (!showMissionDataWindow) {
        return;
    }

    auto name_6975 = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E30);
    if (!name_6975) {
        return;
    }
    auto& missionData = *reinterpret_cast<MissionData*>(name_6975);


    auto pool_6996 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
    if (!pool_6996 || !pool_6996[8]) {
        return;
    }
    auto& eventData = *reinterpret_cast<EventData*>(pool_6996[8]);


    auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);


    static bool run = false;
    if (!run) {
        run = true;

        ImGui::SetNextWindowSize(ImVec2(500, 700));
        ImGui::SetNextWindowPos(ImVec2(0, 200));
    }


    if (ImGui::Begin("MissionData", &showMissionDataWindow)) {
        ImGui::Text("");


        ImGui::PushItemWidth(200);

        GUI_Input<uint32>("Red Orbs", missionData.redOrbs, 1000, "%u", ImGuiInputTextFlags_EnterReturnsTrue);

        ImGui::PopItemWidth();

        ImGui::Text("");


        GUI_Checkbox("unlockDevilTrigger", sessionData.unlockDevilTrigger);
        ImGui::Text("");


        [&]() {
            auto name_7058 = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E30);
            if (!name_7058) {
                return;
            }
            auto& queuedMissionActorData = *reinterpret_cast<QueuedMissionActorData*>(name_7058 + 0xC0);
            auto& activeMissionActorData = *reinterpret_cast<ActiveMissionActorData*>(name_7058 + 0x16C);


            ImGui::PushItemWidth(200);

            ImGui::Text("Active");
            GUI_Input<float>("hitPoints", activeMissionActorData.hitPoints, 1000, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input<float>("maxHitPoints", activeMissionActorData.maxHitPoints, 1000, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input<float>("magicPoints", activeMissionActorData.magicPoints, 1000, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input<float>("maxMagicPoints", activeMissionActorData.maxMagicPoints, 1000, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::Text("");

            ImGui::Text("Queued");
            GUI_Input<float>("hitPoints", queuedMissionActorData.hitPoints, 1000, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input<float>("magicPoints", queuedMissionActorData.magicPoints, 1000, "%g", ImGuiInputTextFlags_EnterReturnsTrue);

            ImGui::PopItemWidth();

            ImGui::Text("");
        }();


        ImGui::Text("Item Count");
        ImGui::Text("");

        ImGui::PushItemWidth(150);

        old_for_all(uint8, itemIndex, ITEM::COUNT) {
            ImGui::Text("%.4u %llX", itemIndex, &missionData.itemCounts[itemIndex]);
            ImGui::SameLine();

            GUI_Input<uint8>(itemNames[itemIndex], missionData.itemCounts[itemIndex], 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
        }

        ImGui::PopItemWidth();

        ImGui::Text("");


        ImGui::Text("Buy Count");
        ImGui::Text("");

        ImGui::PushItemWidth(150);

        old_for_all(uint8, buyIndex, BUY::COUNT) {
            ImGui::Text("%.4u %llX", buyIndex, &missionData.buyCounts[buyIndex]);
            ImGui::SameLine();

            GUI_Input<uint8>(buyNames[buyIndex], missionData.buyCounts[buyIndex], 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
        }

        ImGui::PopItemWidth();


        ImGui::Text("");

        if (GUI_Button("Status")) {
            eventData.event = EVENT::STATUS;
        }

        if (GUI_Button("Customize")) {
            eventData.event = EVENT::CUSTOMIZE;
        }


        ImGui::Text("");
    }

    ImGui::End();
}


struct FileContainer : Container<> {
    const char* operator[](uint32 index);

    void Push(uint32 size);
};

const char* FileContainer::operator[](uint32 index) {
    if (index >= count) {
        return 0;
    }

    auto& metadata = reinterpret_cast<Metadata*>(metadataAddr)[index];

    return reinterpret_cast<const char*>(dataAddr + metadata.off);
}

void FileContainer::Push(uint32 size) {
    auto& metadata = reinterpret_cast<Metadata*>(metadataAddr)[count];

    metadata.off  = pos;
    metadata.size = size;

    pos += size;
    count++;
}

void FileDataWindow() {
    constexpr uint32 fileDataGroupItemCounts[] = {
        4,
        136,
        60,
        28,
        1,
        128,
        6,
    };
    constexpr uint8 fileDataGroupCount                              = static_cast<uint8>(countof(fileDataGroupItemCounts));
    static FileContainer fileDataGroupNames                         = {};
    static FileContainer fileDataGroupItemNames[fileDataGroupCount] = {};
    static FileContainer enemyFileDataItemNames                     = {};
    static FileContainer enemyFileDataMetadataItemNames             = {};

    static bool run = false;
    if (!run) {
        run = true;

        {
            auto& container = fileDataGroupNames;

            container.Init((fileDataGroupCount * 5), (fileDataGroupCount * sizeof(FileContainer::Metadata)));

            old_for_all(uint8, groupIndex, fileDataGroupCount) {
                auto dest = reinterpret_cast<char*>(container.dataAddr + container.pos);
                auto size = (container.dataSize - container.pos);

                snprintf(dest, size, "%.4u", groupIndex);

                container.Push(5);
            }
        }


        uint32 g_itemIndex = 0;

        old_for_all(uint8, groupIndex, fileDataGroupCount) {
            auto& container = fileDataGroupItemNames[groupIndex];
            auto& itemCount = fileDataGroupItemCounts[groupIndex];

            container.Init((itemCount * 10), (itemCount * sizeof(FileContainer::Metadata)));

            old_for_all(uint32, itemIndex, itemCount) {
                auto dest = reinterpret_cast<char*>(container.dataAddr + container.pos);
                auto size = (container.dataSize - container.pos);

                snprintf(dest, size, "%.4u %.4u", itemIndex, g_itemIndex);

                container.Push(10);

                g_itemIndex++;
            }
        }


        {
            const uint32 itemCount = ENEMY_FILE_DATA_COUNT;

            auto& container = enemyFileDataItemNames;

            container.Init((itemCount * 6), (itemCount * sizeof(FileContainer::Metadata)));


            old_for_all(uint32, itemIndex, itemCount) {
                auto dest = reinterpret_cast<char*>(container.dataAddr + container.pos);
                auto size = (container.dataSize - container.pos);

                snprintf(dest, size, "%.4u ", itemIndex);

                container.Push(6);
            }
        }


        {
            const uint32 itemCount = ENEMY::COUNT;

            auto& container = enemyFileDataMetadataItemNames;

            container.Init((itemCount * 7), (itemCount * sizeof(FileContainer::Metadata)));


            old_for_all(uint32, itemIndex, itemCount) {
                auto dest = reinterpret_cast<char*>(container.dataAddr + container.pos);
                auto size = (container.dataSize - container.pos);

                snprintf(dest, size, "%.4u  ", itemIndex);

                container.Push(7);
            }
        }
    }


    if (!showFileDataWindow) {
        return;
    }


    static bool run2 = false;
    if (!run2) {
        run2 = true;

        ImGui::SetNextWindowSize(ImVec2(700, 700));
        ImGui::SetNextWindowPos(ImVec2(0, 0));
    }


    if (ImGui::Begin("FileData", &showFileDataWindow)) {
        ImGui::Text("");

        auto GUI_FileData = [&](FileData& fileData) {
            ImGui::PushItemWidth(200);

            auto addr = &fileData;

            GUI_Input<uint64>("this", *reinterpret_cast<uint64*>(&addr), 0, "%.16llX", ImGuiInputTextFlags_ReadOnly);
            GUI_Input<uint32>("group", fileData.group, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input<uint32>("status", fileData.status, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input<uint64>(
                "typeDataAddr", *reinterpret_cast<uint64*>(&fileData.typeDataAddr), 0, "%.16llX", ImGuiInputTextFlags_EnterReturnsTrue);
            if (fileData.typeDataAddr) {
                ImGui::Text((*fileData.typeDataAddr).typeName);
            }
            GUI_Input<uint64>("file", *reinterpret_cast<uint64*>(&fileData.file), 0, "%.16llX", ImGuiInputTextFlags_EnterReturnsTrue);
            if (GUI_Button("Reset")) {
                SetMemory(&fileData, 0, sizeof(fileData));
            }

            ImGui::PopItemWidth();
        };

        auto GUI_FileDataMetadata = [&](FileDataMetadata& metadata) {
            ImGui::PushItemWidth(200);

            auto addr = &metadata;

            GUI_Input<uint64>("this", *reinterpret_cast<uint64*>(&addr), 0, "%.16llX", ImGuiInputTextFlags_ReadOnly);

            GUI_Input<uint64>(
                "funcAddrs", *reinterpret_cast<uint64*>(&metadata.funcAddrs), 0, "%.16llX", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input<uint64>(
                "lastAddr", *reinterpret_cast<uint64*>(&metadata.lastAddr), 0, "%.16llX", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input<uint64>(
                "nextAddr", *reinterpret_cast<uint64*>(&metadata.nextAddr), 0, "%.16llX", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input<uint64>(
                "fileDataAddr", *reinterpret_cast<uint64*>(&metadata.fileDataAddr), 0, "%.16llX", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input<uint32>("category", metadata.category, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input<uint32>("fileSetIndex", metadata.fileSetIndex, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
            if (GUI_Button("Reset")) {
                SetMemory(&metadata, 0, sizeof(metadata));
            }

            ImGui::PopItemWidth();
        };


        GUI_SectionStart("Default File Data");

        uint32 g_itemIndex = 0;

        old_for_all(uint32, groupIndex, fileDataGroupCount) {
            auto open = ImGui::CollapsingHeader(fileDataGroupNames[groupIndex]);

            auto& itemCount = fileDataGroupItemCounts[groupIndex];

            ImGui::Indent(20);

            old_for_all(uint32, itemIndex, itemCount) {
                if (open && ImGui::CollapsingHeader(fileDataGroupItemNames[groupIndex][itemIndex])) {
                    ImGui::Indent(20);

                    auto& fileData = reinterpret_cast<FileData*>(appBaseAddr + 0xC99D30)[g_itemIndex];

                    GUI_FileData(fileData);

                    ImGui::Unindent(20);
                }

                g_itemIndex++;
            }

            ImGui::Unindent(20);
        }

        GUI_SectionEnd();
        ImGui::Text("");

        GUI_SectionStart("Enemy File Data");

        old_for_all(uint32, fileDataIndex, ENEMY_FILE_DATA_COUNT) {
            if (ImGui::CollapsingHeader(enemyFileDataItemNames[fileDataIndex])) {
                ImGui::Indent(20);

                GUI_FileData(enemyFileData[fileDataIndex]);

                ImGui::Unindent(20);
            }
        }

        GUI_SectionEnd();
        ImGui::Text("");

        GUI_SectionStart("Enemy File Data Metadata");

        old_for_all(uint32, index, ENEMY::COUNT) {
            if (ImGui::CollapsingHeader(enemyFileDataMetadataItemNames[index])) {
                ImGui::Indent(20);

                GUI_FileDataMetadata(enemyFileDataMetadata[index]);

                ImGui::Unindent(20);
            }
        }

        ImGui::Text("");
    }

    ImGui::End();
}


void ActorWindow() {
    if (!showActorWindow) {
        return;
    }


    static bool run = false;
    if (!run) {
        run = true;

        ImGui::SetNextWindowSize(ImVec2(700, 700));
        ImGui::SetNextWindowPos(ImVec2(0, 0));
    }


    if (ImGui::Begin("ActorWindow", &showActorWindow)) {
        ImGui::Text("");


        if (ImGui::CollapsingHeader("Default")) {
            ImGui::Text("");

            old_for_all(uint8, entityIndex, ENTITY_COUNT) {
                auto& newActorData = g_defaultNewActorData[entityIndex];

                // auto & characterName = characterIndexNames[characterIndex];
                auto& entityName = entityEnumNames[entityIndex];

                ImGui::Text(entityName);

                ImGui::PushItemWidth(150.0f);

                GUI_Input<byte64>(
                    "baseAddr", *reinterpret_cast<byte64*>(&newActorData.baseAddr), 0, "%llX", ImGuiInputTextFlags_EnterReturnsTrue);
                GUI_Input<uint8>("visibility", newActorData.visibility, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
                GUI_Checkbox("enableCollision", newActorData.enableCollision);
                [&]() {
                    if (!newActorData.baseAddr) {
                        return;
                    }
                    auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);

                    GUI_Input("Speed", actorData.speed, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                    GUI_Input("Speed Multiplier", actorData.speedMultiplier, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);

                    GUI_Input("x", actorData.position.x, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                    GUI_Input("y", actorData.position.y, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                    GUI_Input("z", actorData.position.z, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                    GUI_Input("a", actorData.position.a, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);

                    GUI_Input("Hit Points", actorData.hitPoints, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                    GUI_Input("Max Hit Points", actorData.maxHitPoints, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                    GUI_Input("Magic Points", actorData.magicPoints, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                    GUI_Input("Max Magic Points", actorData.maxMagicPoints, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                }();

                ImGui::PopItemWidth();

                ImGui::Text("");
            }
        }


        old_for_all(uint8, playerIndex, PLAYER_COUNT) {
            auto& playerData = GetPlayerData(playerIndex);

            // auto playerName = playerNames[playerIndex];

            if (!ImGui::CollapsingHeader(playerIndexNames[playerIndex])) {
                continue;
            }

            ImGui::Text("");

            ImGui::PushItemWidth(150.0f);
            GUI_Input<uint8>("characterCount", playerData.characterCount, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input<uint8>("characterIndex", playerData.characterIndex, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input<uint8>("lastCharacterIndex", playerData.lastCharacterIndex, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input<uint8>("activeCharacterIndex", playerData.activeCharacterIndex, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input<uint8>("collisionGroup", playerData.collisionGroup, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::PopItemWidth();
            ImGui::Text("");

            ImGui::Indent(20);

            old_for_all(uint8, characterIndex, CHARACTER_COUNT) {
                auto& characterName = characterIndexNames[characterIndex];

                if (!ImGui::CollapsingHeader(characterName)) {
                    continue;
                }

                ImGui::Text("");

                old_for_all(uint8, entityIndex, ENTITY_COUNT) {
                    auto& characterData = GetCharacterData(playerIndex, characterIndex, entityIndex);
                    auto& newActorData  = GetNewActorData(playerIndex, characterIndex, entityIndex);

                    // auto & characterName = characterIndexNames[characterIndex];
                    auto& entityName = entityEnumNames[entityIndex];

                    ImGui::Text(entityName);

                    ImGui::PushItemWidth(150.0f);

                    GUI_Input<uint8>("character", characterData.character, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
                    GUI_Input<uint8>("costume", characterData.costume, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
                    GUI_Input<byte64>(
                        "baseAddr", *reinterpret_cast<byte64*>(&newActorData.baseAddr), 0, "%llX", ImGuiInputTextFlags_EnterReturnsTrue);
                    GUI_Input<uint8>("visibility", newActorData.visibility, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
                    GUI_Checkbox("enableCollision", newActorData.enableCollision);
                    [&]() {
                        if (!newActorData.baseAddr) {
                            return;
                        }
                        auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);

                        GUI_Input("Speed", actorData.speed, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                        GUI_Input("Speed Multiplier", actorData.speedMultiplier, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);

                        GUI_Input("x", actorData.position.x, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                        GUI_Input("y", actorData.position.y, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                        GUI_Input("z", actorData.position.z, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                        GUI_Input("a", actorData.position.a, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);

                        GUI_Input("Hit Points", actorData.hitPoints, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                        GUI_Input("Max Hit Points", actorData.maxHitPoints, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                        GUI_Input("Magic Points", actorData.magicPoints, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                        GUI_Input("Max Magic Points", actorData.maxMagicPoints, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                    }();

                    ImGui::PopItemWidth();

                    ImGui::Text("");
                }
            }

            ImGui::Unindent(20);

            ImGui::Text("");
        }


        ImGui::Text("");
    }

    ImGui::End();
}


void EventDataWindow() {
    if (!showEventDataWindow) {
        return;
    }

    auto pool_8096 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
    if (!pool_8096 || !pool_8096[8]) {
        return;
    }
    auto& eventData = *reinterpret_cast<EventData*>(pool_8096[8]);


    static EventData eventData2 = {};

    static bool run = false;
    if (!run) {
        run = true;

        ImGui::SetNextWindowSize(ImVec2(700, 700));
        ImGui::SetNextWindowPos(ImVec2(0, 0));
    }


    if (ImGui::Begin("EventData", &showEventDataWindow)) {
        ImGui::Text("");

        ImGui::PushItemWidth(150);

        GUI_Input<uint32>("room", eventData2.room, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
        GUI_Input<uint32>("position", eventData2.position, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
        GUI_Input<uint32>("event", eventData2.event, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
        GUI_Input<uint32>("subevent", eventData2.subevent, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
        GUI_Input<uint32>("screen", eventData2.screen, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
        GUI_Input<uint32>("nextScreen", eventData2.nextScreen, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);

        ImGui::PopItemWidth();

        ImGui::Text("");

        if (GUI_Button("Apply")) {
            eventData.room       = eventData2.room;
            eventData.position   = eventData2.position;
            eventData.event      = eventData2.event;
            eventData.subevent   = eventData2.subevent;
            eventData.screen     = eventData2.screen;
            eventData.nextScreen = eventData2.nextScreen;
        }

        ImGui::Text("");
    }

    ImGui::End();
}


void RegionDataWindow() {
    static bool run = false;
    if (!run) {
        run = true;
    }

    if (!showRegionDataWindow) {
        return;
    }

    static bool run2 = false;
    if (!run2) {
        run2 = true;

        ImGui::SetNextWindowSize(ImVec2(700, 700));
        ImGui::SetNextWindowPos(ImVec2(0, 0));
    }

    if (ImGui::Begin("RegionData", &showRegionDataWindow)) {
        ImGui::Text("");


        auto GUI_RegionData = [&](RegionData& regionData) {
            ImGui::PushItemWidth(200);

            GUI_Input<uint64>("metadataAddr", *reinterpret_cast<uint64*>(&regionData.metadataAddr), 0, "%llX",
                ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
            GUI_Input<uint64>("dataAddr", *reinterpret_cast<uint64*>(&regionData.dataAddr), 0, "%llX",
                ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
            GUI_Input<uint32>(
                "capacity", regionData.capacity, 0, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
            GUI_Input<uint32>(
                "boundary", regionData.boundary, 0, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
            GUI_Input<uint32>("size", regionData.size, 0, "%X", ImGuiInputTextFlags_ReadOnly);
            GUI_Input<uint32>("pipe", regionData.pipe, 0, "%u", ImGuiInputTextFlags_ReadOnly);
            GUI_Input<uint32>("count", regionData.count, 0, "%u", ImGuiInputTextFlags_ReadOnly);

            ImGui::PopItemWidth();
        };

        auto regionDataAddr = reinterpret_cast<RegionData*>(appBaseAddr + 0xCA8910);
        /*
        dmc3.exe+2C61BF - 4C 8D 25 4A279E00 - lea r12,[dmc3.exe+CA8910]
        dmc3.exe+2C61C6 - 44 8B C5          - mov r8d,ebp
        */

        static uint32 count = 3;

        GUI_Input<uint32>("Count", count, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);

        old_for_all(uint32, index, count) {
            auto& regionData = regionDataAddr[index];

            GUI_RegionData(regionData);
            ImGui::Text("");
        }

        ImGui::Text("");
    }
    ImGui::End();
}


void SoundWindow() {
    if (!showSoundWindow) {
        return;
    }

    static bool run = false;
    if (!run) {
        run = true;

        ImGui::SetNextWindowSize(ImVec2(600, 650));
        ImGui::SetNextWindowPos(ImVec2(0, 0));
    }

    if (ImGui::Begin("Sound", &showSoundWindow)) {
        ImGui::Text("");


        static uint32 fmodChannelIndex       = 0;
        static FMOD_CHANNEL* fmodChannelAddr = 0;
        static float volume                  = 1.0f;
        static FMOD_RESULT fmodResult        = 0;

        ImGui::PushItemWidth(200);
        if (GUI_Input<uint32>("FMOD Channel Index", fmodChannelIndex, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue)) {
            fmodChannelAddr = reinterpret_cast<FMOD_CHANNEL**>(appBaseAddr + 0x5DE3E0)[fmodChannelIndex];
        }
        GUI_Input<uint64>("FMOD Channel Address", *reinterpret_cast<uint64*>(&fmodChannelAddr), 0, "%llX",
            ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue);
        GUI_Input<float>("Volume", volume, 0.1f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::PopItemWidth();
        ImGui::Text("");

        if (GUI_Button("SetVolume")) {
            fmodResult = FMOD_Channel_SetVolume(fmodChannelAddr, volume);
        }

        ImGui::Text("fmodResult %X", fmodResult);


        static byte8* headMetadataAddr = 0;

        ImGui::PushItemWidth(200);
        GUI_Input<uint64>("Head Metadata Address", *reinterpret_cast<uint64*>(&headMetadataAddr), 0, "%llX",
            ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::PopItemWidth();
        ImGui::Text("");

        [&]() {
            if (!headMetadataAddr) {
                return;
            }

            auto& headMetadata = *reinterpret_cast<HeadMetadata*>(headMetadataAddr);

            auto& vagiMetadata = *reinterpret_cast<VagiMetadata*>(headMetadataAddr + headMetadata.vagiMetadataOff);


            if (ImGui::CollapsingHeader("Vagi")) {
                ImGui::Text("");

                ImGui::PushItemWidth(200);

                GUI_Input<uint32>(
                    "size", vagiMetadata.size, 0, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue);
                GUI_Input<uint32>("last", vagiMetadata.last, 0);

                ImGui::Text("");

                auto itemCount = (vagiMetadata.last + 1);

                ImGui::Text("itemCount %u", itemCount);

                old_for_all(uint32, itemIndex, itemCount) {
                    auto& item = vagiMetadata.items[itemIndex];

                    ImGui::Text("");

                    ImGui::Text("%u", itemIndex);
                    GUI_Input<uint32>(
                        "off", item.off, 0, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue);
                    GUI_Input<uint32>(
                        "size", item.size, 0, "%X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue);
                    GUI_Input<uint32>("sampleRate", item.sampleRate, 1000, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
                }

                ImGui::PopItemWidth();

                ImGui::Text("");
            }
        }();

        ImGui::Text("");
    }
    ImGui::End();
}


void Debug() {
    if (ImGui::CollapsingHeader("Debug")) {
        ImGui::Text("");

		if (ImGui::Button("heheh")) {
			SampleModDetour1();
		}

		// if (ImGui::Button("EnableCrazyComboHook")){
		//	HoldToCrazyComboHook->Toggle(true);
		// }

		ImGui::InputInt("Effect Bank", &createEffectBank);
		ImGui::InputInt("Effect ID", &createEffectID);
		if (ImGui::Button("CreateEffect")) {
			CreateEffectDetour();
		}

        ImGui::Text("");

        GUI_Checkbox("g_noTeleport", g_noTeleport);


        GUI_Checkbox("g_showShop", g_showShop);

        GUI_Input<float>("saveTimeout", GUI::saveTimeout, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);


        {
            static int32 group = 0;
            static int32 index = 0;

            ImGui::PushItemWidth(150);
            GUI_Input<int32>("group", group, 1, "%d", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input<int32>("index", index, 1, "%d", ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::PopItemWidth();
            ImGui::Text("");

            if (GUI_Button("Play Sound")) {
                PlaySound(group, index);
            }
        }


        GUI_SectionEnd();
        ImGui::Text("");


        GUI_Checkbox2("Welcome", activeConfig.welcome, queuedConfig.welcome);
        ImGui::Text("");


        if (GUI_Button("Actor")) {
            showActorWindow = true;
        }
        ImGui::SameLine();

        if (GUI_Button("EventData")) {
            showEventDataWindow = true;
        }
        ImGui::SameLine();

        if (GUI_Button("Exp Stuff")) {
            showExpWindow = true;
        }
        ImGui::SameLine();

        if (GUI_Button("File Data")) {
            showFileDataWindow = true;
        }
        ImGui::SameLine();

        if (GUI_Button("Mission Data")) {
            showMissionDataWindow = true;
        }
        ImGui::SameLine();

        if (GUI_Button("Region Data")) {
            showRegionDataWindow = true;
        }
        ImGui::SameLine();

        if (GUI_Button("Sound")) {
            showSoundWindow = true;
        }
        ImGui::Text("");

        if (GUI_Button("Close All")) {
            showActorWindow       = false;
            showEventDataWindow   = false;
            showExpWindow         = false;
            showFileDataWindow    = false;
            showMissionDataWindow = false;
            showRegionDataWindow  = false;
            showSoundWindow       = false;
        }
        ImGui::Text("");


        auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);


        GUI_Checkbox("One Hit Kill", sessionData.oneHitKill);
        ImGui::Text("");

        GUI_Checkbox("unlockDevilTrigger", sessionData.unlockDevilTrigger);
        ImGui::Text("");


        ImGui::Text("");

        if (GUI_Checkbox2(
                "Disable Player Actor Idle Timer", activeConfig.disablePlayerActorIdleTimer, queuedConfig.disablePlayerActorIdleTimer)) {
            ToggleDisablePlayerActorIdleTimer(activeConfig.disablePlayerActorIdleTimer);
        }
        ImGui::Text("");


        {
            static ModelData* modelDataAddr        = 0;
            static BodyPartData* bodyPartDataAddr  = 0;
            static NewArchiveMetadata* archiveAddr = 0;

            static uint16 cacheFileIndex = em034;

            static char buffer[8];
            static byte8* file      = 0;
            static uint32 fileIndex = 0;

            static char subbuffer[8];
            static byte8* subfile      = 0;
            static uint32 subfileIndex = 0;


            GUI_Input<byte64>("modelDataAddr", *reinterpret_cast<byte64*>(&modelDataAddr), 0, "%llX",
                ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsHexadecimal);

            GUI_Input<byte64>("bodyPartDataAddr", *reinterpret_cast<byte64*>(&bodyPartDataAddr), 0, "%llX",
                ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsHexadecimal);

            GUI_Input<byte64>("archiveAddr", *reinterpret_cast<byte64*>(&archiveAddr), 0, "%llX",
                ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsHexadecimal);


            // @Todo: Set Archive.

            GUI_Input<uint16>("cacheFileIndex", cacheFileIndex, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);


            ImGui::PushItemWidth(50);
            ImGui::InputText("", buffer, sizeof(buffer));
            ImGui::PopItemWidth();
            ImGui::SameLine();

            ImGui::PushItemWidth(150);
            GUI_Input<byte64>("", *reinterpret_cast<byte64*>(&file), 0, "%llX",
                ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::SameLine();
            GUI_Input<uint32>("", fileIndex);
            ImGui::PopItemWidth();
            ImGui::SameLine();
            if (GUI_Button("Set File")) {
                [&]() {
                    if (!archiveAddr) {
                        return;
                    }
                    auto& archive = *archiveAddr;


                    file = archive[fileIndex];

                    SetMemory(buffer, 0, sizeof(buffer));


                    if (!file) {
                        return;
                    }

                    CopyMemory(buffer, file, 4);
                }();
            }


            ImGui::PushItemWidth(50);
            ImGui::InputText("", subbuffer, sizeof(subbuffer));
            ImGui::PopItemWidth();
            ImGui::SameLine();
            ImGui::PushItemWidth(150);
            GUI_Input<uint64>("", *reinterpret_cast<uint64*>(&subfile), 0, "%llX",
                ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::SameLine();
            GUI_Input<uint32>("", subfileIndex);
            ImGui::PopItemWidth();
            ImGui::SameLine();
            // @Todo: Update.
            if (GUI_Button("Set Subfile")) {
                [&]() {
                    auto subarchive = file;
                    if (!subarchive) {
                        return;
                    }

                    auto& subarchiveMetadata = *reinterpret_cast<ArchiveMetadata*>(subarchive);

                    if ((subarchiveMetadata.signature[0] != 'P') || (subarchiveMetadata.signature[1] != 'A') ||
                        (subarchiveMetadata.signature[2] != 'C')) {
                        MessageBoxA(0, "Wrong Signature", 0, 0);

                        return;
                    }

                    if (subfileIndex >= subarchiveMetadata.fileCount) {
                        MessageBoxA(0, "Out of range.", 0, 0);

                        return;
                    }

                    auto subfileOff = subarchiveMetadata.fileOffs[subfileIndex];
                    if (!subfileOff) {
                        return;
                    }

                    subfile = (file + subfileOff);

                    SetMemory(subbuffer, 0, sizeof(subbuffer));

                    if (!subfile) {
                        return;
                    }

                    CopyMemory(subbuffer, (subfile + 4), 3);
                }();
            }


            if (GUI_Button("Play File Motion")) {
                [&]() {
                    // IntroduceMainActorData(actorBaseAddr, actorData, return);
                    if (!modelDataAddr) {
                        return;
                    }

                    func_8AC80(
                        // actorData.newModelData[0],
                        *modelDataAddr, UPPER_BODY, file, 0, false);
                    func_8AC80(
                        // actorData.newModelData[0],
                        *modelDataAddr, LOWER_BODY, file, 0, false);
                }();
            }

            if (GUI_Button("Play Subfile Motion")) {
                [&]() {
                    if (!modelDataAddr) {
                        return;
                    }
                    // IntroduceMainActorData(actorBaseAddr, actorData, return);

                    func_8AC80(
                        // actorData.newModelData[0],
                        *modelDataAddr, UPPER_BODY, subfile, 0, false);
                    func_8AC80(
                        // actorData.newModelData[0],
                        *modelDataAddr, LOWER_BODY, subfile, 0, false);

                    // auto bodyPartData2

                    auto& done1 = *reinterpret_cast<bool*>(reinterpret_cast<byte8*>(&bodyPartDataAddr[0]) + 0xBA) = false;
                    auto& done2 = *reinterpret_cast<bool*>(reinterpret_cast<byte8*>(&bodyPartDataAddr[1]) + 0xBA) = false;
                }();
            }
        }


        {
            static vec4 __declspec(align(16)) position   = {};
            static vec4 __declspec(align(16)) multiplier = {};
            static __m128 __declspec(align(16)) height   = {};


            GUI_Input("position x", position.x, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input("position y", position.y, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input("position z", position.z, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input("position a", position.a, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);

            GUI_Input("multiplier x", multiplier.x, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input("multiplier y", multiplier.y, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input("multiplier z", multiplier.z, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input("multiplier a", multiplier.a, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);


            GUI_Input("height x", height.m128_f32[0], 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input("height y", height.m128_f32[1], 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input("height z", height.m128_f32[2], 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
            GUI_Input("height a", height.m128_f32[3], 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);


            if (GUI_Button("Grenade")) {
                // HoboBreak();

                func_175210(&position, &multiplier, height);
            }
        }


        {
            static uint32 event      = 0;
            static uint8 action      = 0;
            static uint64 actorIndex = 0;

            GUI_Input<decltype(actorIndex)>("actorIndex", actorIndex, 1, "%llu", ImGuiInputTextFlags_EnterReturnsTrue);

            GUI_Input<uint32>("event", event, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);

            GUI_Input<decltype(action)>("action", action, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);


            if (GUI_Button("Event")) {

                [&]() {
                    if (!g_playerActorBaseAddrs[actorIndex]) {
                        return;
                    }
                    auto& actorData = *reinterpret_cast<PlayerActorData*>(g_playerActorBaseAddrs[actorIndex]);

                    actorData.action = action;

                    func_1E09D0(actorData, event);
                }();
            }
        }


        ImGui::Text("");
    }
}

#pragma endregion

#pragma region Enemy

void Enemy() {
    if (ImGui::CollapsingHeader("Enemy")) {
        ImGui::Text("");


        if (GUI_ResetButton()) {
            CopyMemory(&queuedConfig.enemyCount, &defaultConfig.enemyCount, sizeof(queuedConfig.enemyCount));
            CopyMemory(&activeConfig.enemyCount, &queuedConfig.enemyCount, sizeof(activeConfig.enemyCount));

            CopyMemory(&queuedConfig.configCreateEnemyActorData, &defaultConfig.configCreateEnemyActorData,
                sizeof(queuedConfig.configCreateEnemyActorData));
            CopyMemory(&activeConfig.configCreateEnemyActorData, &queuedConfig.configCreateEnemyActorData,
                sizeof(activeConfig.configCreateEnemyActorData));

            CopyMemory(&queuedConfig.enemyAutoSpawn, &defaultConfig.enemyAutoSpawn, sizeof(queuedConfig.enemyAutoSpawn));
            CopyMemory(&activeConfig.enemyAutoSpawn, &queuedConfig.enemyAutoSpawn, sizeof(activeConfig.enemyAutoSpawn));
        }
        ImGui::Text("");


        ImGui::PushItemWidth(200);


        GUI_Slider2<uint8>("Enemy Count", activeConfig.enemyCount, queuedConfig.enemyCount, 1,
            static_cast<uint8>(countof(activeConfig.configCreateEnemyActorData)));
        ImGui::Text("");

        GUI_Checkbox2("Auto Spawn", activeConfig.enemyAutoSpawn, queuedConfig.enemyAutoSpawn);
        ImGui::Text("");


        if (GUI_Button("Create All")) {
            old_for_all(uint8, index, activeConfig.enemyCount) {
                auto& activeConfigCreateEnemyActorData = activeConfig.configCreateEnemyActorData[index];

                CreateEnemyActor(activeConfigCreateEnemyActorData);
            }
        }
        ImGui::Text("");

        if (GUI_Button("Kill All Ladies")) {
            [&]() {
                auto pool_9347 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
                if (!pool_9347 || !pool_9347[8]) {
                    return;
                }
                auto& enemyVectorData = *reinterpret_cast<EnemyVectorData*>(pool_9347[8]);


                LogFunction();

                old_for_all(uint32, enemyIndex, countof(enemyVectorData.metadata)) {
                    auto& metadata = enemyVectorData.metadata[enemyIndex];

                    if (!metadata.baseAddr) {
                        continue;
                    }
                    auto& actorData = *reinterpret_cast<EnemyActorDataLady*>(metadata.baseAddr);

                    if (!actorData.baseAddr || (actorData.enemy != ENEMY::LADY)) {
                        continue;
                    }

                    actorData.event    = EVENT_BOSS_LADY::DEATH;
                    actorData.state    = 0;
                    actorData.friendly = false;
                }
            }();
        }
        ImGui::Text("");


        old_for_all(uint8, index, activeConfig.enemyCount) {
            auto& activeConfigCreateEnemyActorData = activeConfig.configCreateEnemyActorData[index];
            auto& queuedConfigCreateEnemyActorData = queuedConfig.configCreateEnemyActorData[index];

            ImGui::Text("%u", index);


            UI::Combo2("Enemy", enemyNames, activeConfigCreateEnemyActorData.enemy, queuedConfigCreateEnemyActorData.enemy,
                ImGuiComboFlags_HeightLarge);

            GUI_Input2<uint32>("Variant", activeConfigCreateEnemyActorData.variant, queuedConfigCreateEnemyActorData.variant, 1, "%u",
                ImGuiInputTextFlags_EnterReturnsTrue);

            GUI_Checkbox2("Use Main Actor Data", activeConfigCreateEnemyActorData.useMainActorData,
                queuedConfigCreateEnemyActorData.useMainActorData);


            {
                bool condition = activeConfigCreateEnemyActorData.useMainActorData;

                GUI_PushDisable(condition);

                GUI_Input2<float>("X", activeConfigCreateEnemyActorData.position.x, queuedConfigCreateEnemyActorData.position.x, 10.0f,
                    "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                GUI_Input2<float>("Y", activeConfigCreateEnemyActorData.position.y, queuedConfigCreateEnemyActorData.position.y, 10.0f,
                    "%g", ImGuiInputTextFlags_EnterReturnsTrue);
                GUI_Input2<float>("Z", activeConfigCreateEnemyActorData.position.z, queuedConfigCreateEnemyActorData.position.z, 10.0f,
                    "%g", ImGuiInputTextFlags_EnterReturnsTrue);


                GUI_Input2<uint16>("Rotation", activeConfigCreateEnemyActorData.rotation, queuedConfigCreateEnemyActorData.rotation, 1,
                    "%u", ImGuiInputTextFlags_EnterReturnsTrue);

                GUI_PopDisable(condition);
            }


            GUI_Input2<uint16>("Spawn Method", activeConfigCreateEnemyActorData.spawnMethod, queuedConfigCreateEnemyActorData.spawnMethod,
                1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);

            if (GUI_Button("Create")) {
                CreateEnemyActor(activeConfigCreateEnemyActorData);
            }

            ImGui::Text("");
        }


        ImGui::PopItemWidth();
    }
}

#pragma endregion

#pragma region Jukebox

void Jukebox() {
    if (ImGui::CollapsingHeader("Jukebox")) {
        ImGui::Text("");

        static char location[512];
        static char fileName[256];
        static new_size_t index = 0;
        static bool run         = false;

        if (!run) {
            run = true;

            snprintf(fileName, sizeof(fileName), "%s", trackFilenames[index]);
        }

        ImGui::PushItemWidth(200);

        ImGui::InputText("Filename", fileName, sizeof(fileName));

        if (UI::Combo("", trackNames, index, ImGuiComboFlags_HeightLarge)) {
            snprintf(fileName, sizeof(fileName), "%s", trackFilenames[index]);
        }

        ImGui::PopItemWidth();

        if (GUI_Button("Play")) {
            snprintf(location, sizeof(location), "afs/sound/%s", fileName);

            PlayTrack(location);
        }

        if (GUI_Button("Stop")) {
            PlayTrack("");
        }

        ImGui::Text("");
    }
}

#pragma endregion

#pragma region Lady

void Lady() {
    if (ImGui::CollapsingHeader("Lady")) {
        ImGui::Text("");

        if (GUI_ResetButton()) {
            CopyMemory(&queuedConfig.kalinaAnnHookMultiplier, &defaultConfig.kalinaAnnHookMultiplier,
                sizeof(queuedConfig.kalinaAnnHookMultiplier));
            CopyMemory(
                &activeConfig.kalinaAnnHookMultiplier, &queuedConfig.kalinaAnnHookMultiplier, sizeof(activeConfig.kalinaAnnHookMultiplier));

            CopyMemory(&queuedConfig.kalinaAnnHookGrenadeHeight, &defaultConfig.kalinaAnnHookGrenadeHeight,
                sizeof(queuedConfig.kalinaAnnHookGrenadeHeight));
            CopyMemory(&activeConfig.kalinaAnnHookGrenadeHeight, &queuedConfig.kalinaAnnHookGrenadeHeight,
                sizeof(activeConfig.kalinaAnnHookGrenadeHeight));

            CopyMemory(&queuedConfig.kalinaAnnHookGrenadeTime, &defaultConfig.kalinaAnnHookGrenadeTime,
                sizeof(queuedConfig.kalinaAnnHookGrenadeTime));
            CopyMemory(&activeConfig.kalinaAnnHookGrenadeTime, &queuedConfig.kalinaAnnHookGrenadeTime,
                sizeof(activeConfig.kalinaAnnHookGrenadeTime));
        }

        GUI_SectionEnd();
        ImGui::Text("");

        GUI_SectionStart("Kalina Ann");


        ImGui::PushItemWidth(200.0f);

        GUI_InputDefault2<float>("Hook Multiplier X", activeConfig.kalinaAnnHookMultiplier.x, queuedConfig.kalinaAnnHookMultiplier.x,
            defaultConfig.kalinaAnnHookMultiplier.x, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
        GUI_InputDefault2<float>("Hook Multiplier Y", activeConfig.kalinaAnnHookMultiplier.y, queuedConfig.kalinaAnnHookMultiplier.y,
            defaultConfig.kalinaAnnHookMultiplier.y, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
        GUI_InputDefault2<float>("Hook Multiplier Z", activeConfig.kalinaAnnHookMultiplier.z, queuedConfig.kalinaAnnHookMultiplier.z,
            defaultConfig.kalinaAnnHookMultiplier.z, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
        GUI_InputDefault2<float>("Hook Multiplier A", activeConfig.kalinaAnnHookMultiplier.a, queuedConfig.kalinaAnnHookMultiplier.a,
            defaultConfig.kalinaAnnHookMultiplier.a, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);

        GUI_InputDefault2<float>("Hook Grenade Height", activeConfig.kalinaAnnHookGrenadeHeight, queuedConfig.kalinaAnnHookGrenadeHeight,
            defaultConfig.kalinaAnnHookGrenadeHeight, 10.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);

        GUI_InputDefault2<float>("Hook Grenade Time", activeConfig.kalinaAnnHookGrenadeTime, queuedConfig.kalinaAnnHookGrenadeTime,
            defaultConfig.kalinaAnnHookGrenadeTime, 10.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);

        ImGui::PopItemWidth();


        ImGui::Text("");
    }
}

#pragma endregion

#pragma region Mobility

void Mobility() {
    if (ImGui::CollapsingHeader("Mobility")) {
        ImGui::Text("");

        if (GUI_ResetButton()) {
            CopyMemory(&queuedConfig.airHikeCount, &defaultConfig.airHikeCount, sizeof(queuedConfig.airHikeCount));
            CopyMemory(&activeConfig.airHikeCount, &queuedConfig.airHikeCount, sizeof(activeConfig.airHikeCount));

            CopyMemory(&queuedConfig.kickJumpCount, &defaultConfig.kickJumpCount, sizeof(queuedConfig.kickJumpCount));
            CopyMemory(&activeConfig.kickJumpCount, &queuedConfig.kickJumpCount, sizeof(activeConfig.kickJumpCount));

            CopyMemory(&queuedConfig.wallHikeCount, &defaultConfig.wallHikeCount, sizeof(queuedConfig.wallHikeCount));
            CopyMemory(&activeConfig.wallHikeCount, &queuedConfig.wallHikeCount, sizeof(activeConfig.wallHikeCount));

            CopyMemory(&queuedConfig.dashCount, &defaultConfig.dashCount, sizeof(queuedConfig.dashCount));
            CopyMemory(&activeConfig.dashCount, &queuedConfig.dashCount, sizeof(activeConfig.dashCount));

            CopyMemory(&queuedConfig.skyStarCount, &defaultConfig.skyStarCount, sizeof(queuedConfig.skyStarCount));
            CopyMemory(&activeConfig.skyStarCount, &queuedConfig.skyStarCount, sizeof(activeConfig.skyStarCount));

            CopyMemory(&queuedConfig.airTrickCountDante, &defaultConfig.airTrickCountDante, sizeof(queuedConfig.airTrickCountDante));
            CopyMemory(&activeConfig.airTrickCountDante, &queuedConfig.airTrickCountDante, sizeof(activeConfig.airTrickCountDante));

            CopyMemory(&queuedConfig.airTrickCountVergil, &defaultConfig.airTrickCountVergil, sizeof(queuedConfig.airTrickCountVergil));
            CopyMemory(&activeConfig.airTrickCountVergil, &queuedConfig.airTrickCountVergil, sizeof(activeConfig.airTrickCountVergil));

            CopyMemory(&queuedConfig.trickUpCount, &defaultConfig.trickUpCount, sizeof(queuedConfig.trickUpCount));
            CopyMemory(&activeConfig.trickUpCount, &queuedConfig.trickUpCount, sizeof(activeConfig.trickUpCount));

            CopyMemory(&queuedConfig.trickDownCount, &defaultConfig.trickDownCount, sizeof(queuedConfig.trickDownCount));
            CopyMemory(&activeConfig.trickDownCount, &queuedConfig.trickDownCount, sizeof(activeConfig.trickDownCount));
        }
        ImGui::Text("");

        TooltipHelper("(?)", "Requires enabled Actor module.\n"
                             "\n"
                             "Left : Human\n"
                             "Right: Devil");
        ImGui::Text("");

        ActionData("Air Hike Count", activeConfig.airHikeCount, queuedConfig.airHikeCount, defaultConfig.airHikeCount);
        ActionData("Kick Jump Count", activeConfig.kickJumpCount, queuedConfig.kickJumpCount, defaultConfig.kickJumpCount);
        ActionData("Wall Hike Count", activeConfig.wallHikeCount, queuedConfig.wallHikeCount, defaultConfig.wallHikeCount);
        ActionData("Dash Count", activeConfig.dashCount, queuedConfig.dashCount, defaultConfig.dashCount);
        ActionData("Sky Star Count", activeConfig.skyStarCount, queuedConfig.skyStarCount, defaultConfig.skyStarCount);
        ActionData(
            "Air Trick Count Dante", activeConfig.airTrickCountDante, queuedConfig.airTrickCountDante, defaultConfig.airTrickCountDante);
        ActionData("Air Trick Count Vergil", activeConfig.airTrickCountVergil, queuedConfig.airTrickCountVergil,
            defaultConfig.airTrickCountVergil);
        ActionData("Trick Up Count", activeConfig.trickUpCount, queuedConfig.trickUpCount, defaultConfig.trickUpCount);
        ActionData("Trick Down Count", activeConfig.trickDownCount, queuedConfig.trickDownCount, defaultConfig.trickDownCount);

        ImGui::Text("");
    }
}

#pragma endregion

#pragma region Other

const char* dotShadowNames[] = {"Default", "Disable", "Disable Player Actors Only"};


void Other() {
    if (ImGui::CollapsingHeader("Other")) {
        ImGui::Text("");

     

        if (GUI_ResetButton()) {
            CopyMemory(&queuedConfig.dotShadow, &defaultConfig.dotShadow, sizeof(queuedConfig.dotShadow));
            CopyMemory(&activeConfig.dotShadow, &queuedConfig.dotShadow, sizeof(activeConfig.dotShadow));

            CopyMemory(&queuedConfig.depleteQuicksilver, &defaultConfig.depleteQuicksilver, sizeof(queuedConfig.depleteQuicksilver));
            CopyMemory(&activeConfig.depleteQuicksilver, &queuedConfig.depleteQuicksilver, sizeof(activeConfig.depleteQuicksilver));

            CopyMemory(&queuedConfig.depleteDoppelganger, &defaultConfig.depleteDoppelganger, sizeof(queuedConfig.depleteDoppelganger));
            CopyMemory(&activeConfig.depleteDoppelganger, &queuedConfig.depleteDoppelganger, sizeof(activeConfig.depleteDoppelganger));

            CopyMemory(&queuedConfig.depleteDevil, &defaultConfig.depleteDevil, sizeof(queuedConfig.depleteDevil));
            CopyMemory(&activeConfig.depleteDevil, &queuedConfig.depleteDevil, sizeof(activeConfig.depleteDevil));

            CopyMemory(&queuedConfig.crazyComboLevelMultiplier, &defaultConfig.crazyComboLevelMultiplier,
                sizeof(queuedConfig.crazyComboLevelMultiplier));
            CopyMemory(&activeConfig.crazyComboLevelMultiplier, &queuedConfig.crazyComboLevelMultiplier,
                sizeof(activeConfig.crazyComboLevelMultiplier));

            CopyMemory(&queuedConfig.linearWeaponSwitchTimeout, &defaultConfig.linearWeaponSwitchTimeout,
                sizeof(queuedConfig.linearWeaponSwitchTimeout));
            CopyMemory(&activeConfig.linearWeaponSwitchTimeout, &queuedConfig.linearWeaponSwitchTimeout,
                sizeof(activeConfig.linearWeaponSwitchTimeout));

            CopyMemory(&queuedConfig.orbReach, &defaultConfig.orbReach, sizeof(queuedConfig.orbReach));
            CopyMemory(&activeConfig.orbReach, &queuedConfig.orbReach, sizeof(activeConfig.orbReach));


            UpdateCrazyComboLevelMultiplier();
        }
        ImGui::Text("");

        ImGui::PushItemWidth(200);


        GUI_InputDefault2("Deplete Quicksilver", activeConfig.depleteQuicksilver, queuedConfig.depleteQuicksilver,
            defaultConfig.depleteQuicksilver, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
        GUI_InputDefault2("Deplete Doppelganger", activeConfig.depleteDoppelganger, queuedConfig.depleteDoppelganger,
            defaultConfig.depleteDoppelganger, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
        GUI_InputDefault2("Deplete Devil", activeConfig.depleteDevil, queuedConfig.depleteDevil, defaultConfig.depleteDevil, 1.0f, "%g",
            ImGuiInputTextFlags_EnterReturnsTrue);

        if (GUI_InputDefault2("Crazy Combo Level Multiplier", activeConfig.crazyComboLevelMultiplier,
                queuedConfig.crazyComboLevelMultiplier, defaultConfig.crazyComboLevelMultiplier)) {
            UpdateCrazyComboLevelMultiplier();
        }

        GUI_InputDefault2("Linear Weapon Switch Timeout", activeConfig.linearWeaponSwitchTimeout, queuedConfig.linearWeaponSwitchTimeout,
            defaultConfig.linearWeaponSwitchTimeout, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);

        GUI_InputDefault2("Orb Reach", activeConfig.orbReach, queuedConfig.orbReach, defaultConfig.orbReach, 100.0f, "%g",
            ImGuiInputTextFlags_EnterReturnsTrue);

        ImGui::PopItemWidth();


        ImGui::Text("");
    }
}

#pragma endregion

#pragma region Interface

void NewMissionClearSong() {
    if (g_scene == SCENE::MISSION_RESULT && !missionClearSongPlayed) {
        // Mute Music Channel Volume
        SetVolume(9, 0);

        // Play song
        PlayNewMissionClearSong();
        missionClearSongPlayed = true;
    } else if (g_scene != SCENE::MISSION_RESULT && missionClearSongPlayed) {
        // Fade it out
        FadeOutNewMissionClearSong();

        // Restore original Channnel Volume
        SetVolume(9, activeConfig.channelVolumes[9]);

        missionClearSongPlayed = false;
    }
}

void SiyTimerFunc() {
    if (siytimer > 0) {
        siytimer -= ImGui::GetIO().DeltaTime;
    } else if (siytimer < 0) {
        siytimer = 0;
    }
}


const char* mainOverlayLabel = "MainOverlay";

void MainOverlayWindow(size_t defaultFontSize) {
    NewMissionClearSong();
    
    auto Function = [&]() {
        if (activeConfig.mainOverlayData.showFocus) {
            auto color = ImVec4(0, 1, 0, 1);
            if (GetForegroundWindow() != appWindow) {
                color = ImVec4(1, 0, 0, 1);
            }
            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::Text("Focus");
            ImGui::PopStyleColor();
        }


        if (activeConfig.mainOverlayData.showFPS) {
            ImGui::Text("%.2f FPS", ImGui::GetIO().Framerate);
        }

        if (activeConfig.mainOverlayData.showSizes) {
            ImGui::Text("g_windowSize %g %g", g_windowSize.x, g_windowSize.y);
            ImGui::Text("g_clientSize %g %g", g_clientSize.x, g_clientSize.y);
            ImGui::Text("g_renderSize %g %g", g_renderSize.x, g_renderSize.y);
        }


        if (activeConfig.mainOverlayData.showFrameRateMultiplier) {
            ImGui::Text("g_frameRateMultiplier %g", g_frameRateMultiplier);
        }

        if (activeConfig.mainOverlayData.showFocus || activeConfig.mainOverlayData.showFPS || activeConfig.mainOverlayData.showSizes ||
            activeConfig.mainOverlayData.showFrameRateMultiplier) {
            ImGui::Text("");
        }

        if (activeConfig.mainOverlayData.showEventData) {
            if (g_scene >= SCENE::COUNT) {
                ImGui::Text("Unknown");
            } else {
                auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);
                

                ImGui::Text(sceneNames[g_scene]);
                ImGui::Text("sessionData mission:  %u", sessionData.mission);
                ImGui::Text("SCENE:  %u", g_scene);
                ImGui::Text("TRACK PLAYING: %s", g_gameTrackPlaying.c_str());
                
//                 for (int i = 0; i < 14; i++) {
//                     ImGui::Text("sessionData unlock[%u] : %u", i, sessionData.weaponStyleUnlocks[i]);
//                 }
               
                ImGui::Text("Unlocked DT: %u", sessionData.unlockDevilTrigger);
                ImGui::Text("Quicksilver Level: %u", sessionData.styleLevels[4]);

                /*ImGui::Text("Sky Launch:  %u", executingSkyLaunch);
                ImGui::Text("Sky Launch Tracker Running:  %u", skyLaunchTrackerRunning);
                ImGui::Text("Royal Release:  %u", executingRoyalRelease);
                ImGui::Text("Royal Release Tracker Running:  %u", royalReleaseTrackerRunning);
                ImGui::Text("Forcing Just Frame Royal Release:  %u", forcingJustFrameRoyalRelease);
                ImGui::Text("skyLaunchSetJustFrameTrue:  %u", skyLaunchSetJustFrameTrue);
                ImGui::Text("Royal Release Executed:  %u", royalReleaseExecuted);*/


                /*ImGui::Text("backtoforward Back:  %u", b2F.backCommand);
                ImGui::Text("backtoforward Back Buffer:  %g", b2F.backBuffer);
                ImGui::Text("backtoforward Forward:  %u", b2F.forwardCommand);
                ImGui::Text("backtoforward Forward Buffer:  %g", b2F.forwardBuffer);
                ImGui::Text("backtoforward Back Direction Changed:  %u", b2F.backDirectionChanged);
                ImGui::Text("backtoforward Forward Direction Changed:  %u", b2F.backDirectionChanged);*/

                // 					ImGui::Text("delayed Combo Effect timer:  %g", delayedComboFX.timer);
                // 					ImGui::Text("delayed Combo Effect Start Timer:  %u", delayedComboFX.startTimer);
                // 					ImGui::Text("delayed Combo Effect Duration:  %g", delayedComboFX.duration);
                // 					ImGui::Text("delayed Combo Effect Weapon:  %u",
                // delayedComboFX.weaponThatStartedMove); 					ImGui::Text("trickCancel Cooldown:  %u", trickUpCancel.cooldown);

                ImGui::Text("sprint Can Sprint:  %u", crimsonPlayer[0].sprint.canSprint);
                ImGui::Text("Sprint Timer:  %g", crimsonPlayer[0].sprint.timer);
                ImGui::Text("Sprint Run Timer:  %u", crimsonPlayer[0].sprint.runTimer);


                if (isMusicPlaying() == 0) {
                    ImGui::Text("no music playing");
                } else {
                    ImGui::Text("MUSICPLAYING");
                }
            }

            [&]() {
                if (g_scene != SCENE::GAME) {
                    return;
                }

                auto pool_10298 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
                if (!pool_10298 || !pool_10298[8]) {
                    return;
                }
                auto& eventData = *reinterpret_cast<EventData*>(pool_10298[8]);

                auto pool_10329 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
                if (!pool_10329 || !pool_10329[12]) {
                    return;
                }
                auto& nextEventData = *reinterpret_cast<NextEventData*>(pool_10329[12]);


                if (eventData.event >= EVENT::COUNT) {
                    ImGui::Text("Unknown");
                } else {
                    ImGui::Text(eventNames[eventData.event]);
                }

                ImGui::Text("");

                ImGui::Text("room         %u", eventData.room);
                ImGui::Text("position     %u", eventData.position);
                ImGui::Text("position     %u", eventData.event);
                ImGui::Text("nextRoom     %u", nextEventData.room);
                ImGui::Text("nextPosition %u", nextEventData.position);
            }();

            ImGui::Text("");
        }

        if (activeConfig.mainOverlayData.showPosition) {
            [&]() {
                auto pool_10213 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
                if (!pool_10213 || !pool_10213[3]) {
                    return;
                }
                auto& actorData = *reinterpret_cast<PlayerActorData*>(pool_10213[3]);

                ImGui::Text("X        %g", actorData.position.x);
                ImGui::Text("Y        %g", actorData.position.y);
                ImGui::Text("Z        %g", actorData.position.z);
                ImGui::Text("Rotation %u", actorData.rotation);
            }();


            auto pool_10222 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
            if (!pool_10222 || !pool_10222[3]) {
                return;
            }
            auto& actorData      = *reinterpret_cast<PlayerActorData*>(pool_10222[3]);
            auto& characterData  = GetCharacterData(actorData);
            auto& cloneActorData = *reinterpret_cast<PlayerActorData*>(actorData.cloneActorBaseAddr);
            auto& gamepad        = GetGamepad(0);
            auto tiltDirection   = GetRelativeTiltDirection(actorData);

            auto pool_10320 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
            if (!pool_10320 || !pool_10320[8]) {
                return;
            }
            auto& enemyVectorData = *reinterpret_cast<EnemyVectorData*>(pool_10320[8]);

            auto pool_10371 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
            if (!pool_10371 || !pool_10371[8]) {
                return;
            }
            auto& eventData = *reinterpret_cast<EventData*>(pool_10371[8]);

            auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);


            auto& metadata = enemyVectorData.metadata[0];


            auto pool_12857 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
            if (!pool_12857 || !pool_12857[3]) {
                return;
            }
            auto& mainActorData = *reinterpret_cast<PlayerActorDataDante*>(pool_12857[3]);

			auto name_7058 = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E30);

			auto& missionData = *reinterpret_cast<MissionData*>(name_7058);
            auto& queuedMissionActorData = *reinterpret_cast<QueuedMissionActorData*>(name_7058 + 0xC0);
			auto& activeMissionActorData = *reinterpret_cast<ActiveMissionActorData*>(name_7058 + 0x16C);


            // crazyComboHold = g_HoldToCrazyComboFuncA();
            ImGui::Text("action Timer Main Actor:  %g", crimsonPlayer[0].actionTimer);
            ImGui::Text("anim Timer Main Actor:  %g", crimsonPlayer[0].animTimer);
            // ImGui::Text("crazy combo hold:  %u", crazyComboHold);
            ImGui::Text("Chain Count (weight):  %u", actorData.airSwordAttackCount);
            ImGui::Text("Air Guard:  %u", actorData.airGuard);
            ImGui::Text("rainstorm cached inertia:  %g", crimsonPlayer[0].inertia.rainstorm.cachedPull);
            // ImGui::Text("Gravity Tweak:  %g", crimsonPlayer[0].airRaveTweak.gravity);
            // ImGui::Text("drive timer:  %g", crimsonPlayer[0].drive.timer);
            // ImGui::Text("Actor Speed %g", actorData.speed);
            ImGui::Text("IN COMBAT: %u", g_inCombat);
            ImGui::Text("DTE CHARGE: %g", actorData.dtExplosionCharge);
            ImGui::Text("FLUX TIME: %g", crimsonPlayer[0].fluxtime);
            ImGui::Text("TRICKSTER TIME: %g", crimsonPlayer[0].styleSwitchText.time[0]);
            ImGui::Text("TRICKSTER ALPHA: %g", crimsonPlayer[0].styleSwitchText.alpha[0]);
            // 					ImGui::Text("Trick Cooldown %g", crimsonPlayer[1].cancels.trickCooldown);
            // 					ImGui::Text("Guns Cooldown %g", crimsonPlayer[1].cancels.gunsCooldown);
            // 					ImGui::Text("Rainstorm Cooldown %g", crimsonPlayer[1].cancels.rainstormCooldown);
            // ImGui::Text("Weapon %u", actorData.newWeapons[actorData.meleeWeaponIndex]);
            ImGui::Text("Weapon Ranged %u", actorData.newWeapons[actorData.rangedWeaponIndex]);
            ImGui::Text("Weapon Melee actual %u", characterData.lastMeleeWeaponIndex);
            ImGui::Text("Artemis Status %u", mainActorData.artemisStatus);
            ImGui::Text("lock On: %u", crimsonPlayer[1].lockOn);
            ImGui::Text("tilt direction %u", crimsonPlayer[1].tiltDirection);
            ImGui::Text("Enemy Count %u", enemyVectorData.count);
            ImGui::Text("enemy distance %g", distanceToEnemy);
            // ImGui::Text("enemy vertical Pull Multiplier %g", enemyData.verticalPullMultiplier);
            ImGui::Text("rainstormCancel Cooldown %u", rainstormCancel.cooldown);
            ImGui::Text("In Royal Block:  %u", inRoyalBlock);
            ImGui::Text("Gun Shoot Inverted %u", gunShootInverted);
            ImGui::Text("Gun Shoot Normalized %u", gunShootNormalized);

            ImGui::Text("new speed Set Rapid Slash %u", fasterRapidSlash.newSpeedSet);
            ImGui::Text("inRapidSlash %u", inRapidSlash);
            ImGui::Text("fRapidSlash storedSpeedDevil %g", fasterRapidSlash.storedSpeedDevil[0]);
            ImGui::Text("fDarklasyer storedSpeedDevil %g", fasterDarkslayer.storedSpeedDevil[0]);
            ImGui::Text("Motion Data 1: %u", crimsonPlayer[0].motion);
            ImGui::Text("Event Data 1 %u", actorData.eventData[0]);
            ImGui::Text("Last Event Data %u", actorData.eventData[0].lastEvent);
            ImGui::Text("Last Last Event %u", crimsonPlayer[0].lastLastEvent);
            ImGui::Text("State %u", actorData.state);
            ImGui::Text("Last State %u", actorData.lastState);
            ImGui::Text("Last Last State %u", crimsonPlayer[0].lastLastState);
            ImGui::Text("Character Action %u", crimsonPlayer[0].action);
            ImGui::Text("Character Last Action %u", actorData.lastAction);
            ImGui::Text("Horizontal Pull  %g", actorData.horizontalPull);
            ImGui::Text("Horizontal Pull Multiplier %g", actorData.horizontalPullMultiplier);
            ImGui::Text("Vertical Pull  %g", actorData.verticalPull);
            ImGui::Text("Vertical Pull Multiplier %g", actorData.verticalPullMultiplier);
            ImGui::Text("Position  %g", actorData.position);
            ImGui::Text("Rotation %g", actorData.rotation);
            ImGui::Text("Camera Direction %g", actorData.cameraDirection);
            ImGui::Text("Actor Camera Direction %g", actorData.actorCameraDirection);
            ImGui::Text("RelativeTilt %g", relativeTiltController);
            ImGui::Text("LeftStick Position %u", gamepad.leftStickPosition);
            ImGui::Text("Air Stinger End Timer %u", airStingerEnd.timer);
            ImGui::Text("Air Stinger End Time Int %u", airStingerEndTimeInt);

            ImGui::Text("Royal Block Type:  %u", actorData.royalguardBlockType);
            ImGui::Text("Guardflying:  %u", inGuardfly);
            // ImGui::Text("Track %s", eventData.track);

            ImGui::Text("Air Guard Inertia  %g", crimsonPlayer[0].inertia.airGuard.cachedPull);
            ImGui::Text("Weapon Character Data %u", characterData.meleeWeaponIndex);
            ImGui::Text("Active Weapon %u", actorData.activeWeapon);
            ImGui::Text("Trick Up Count %u", actorData.newTrickUpCount);
            ImGui::Text("Sky Star Count %u", actorData.newSkyStarCount);
            ImGui::Text("Air Hike Count %u", actorData.newAirHikeCount);
            ImGui::Text("Air Rising Sun Count %u", actorData.newAirRisingSunCount);


            ImGui::Text("Config Inf HP: %u", activeConfig.infiniteHitPoints);

            /*ImGui::Text("styleRankCount Dismal %u", rankAnnouncer[0].count);
            ImGui::Text("styleRankCount Crazy %u", rankAnnouncer[1].count);
            ImGui::Text("styleRankCount Dismal Off Cooldown %u", rankAnnouncer[0].offCooldown);
            ImGui::Text("styleRankCount Crazy Off Cooldown %u", rankAnnouncer[1].offCooldown);*/
            ImGui::Text("Wheel Appear %u", meleeWeaponWheelTiming.wheelAppear);
            /*ImGui::Text("SDL2 %s", SDL2Initialization);
            ImGui::Text("Mixer  %s", MixerInitialization);
            ImGui::Text("Mixer2  %s", MixerInitialization2);*/

            ImGui::Text("Wheel Appear %u", meleeWeaponWheelTiming.wheelAppear);
            ImGui::Text("Wheel Running %u", meleeWeaponWheelTiming.wheelRunning);
            ImGui::Text("Wheel Time %u", meleeWeaponWheelTiming.wheelTime);
            ImGui::Text("Quick Double Tap Buffer %u", quickDoubleTap.buffer);
            ImGui::Text("Dopp Double Tap Buffer %u", doppDoubleTap.buffer);

            ImGui::Text("Magic Points Dopp %g", currentDTDoppOn);
            ImGui::Text("Magic Points Dopp DT %g", currentDTDoppDTOn);
            ImGui::Text("Dopp Milliseconds %g", doppSeconds);
            ImGui::Text("Dopp DT Milliseconds %g", doppSecondsDT);
            ImGui::Text("Dopp Tracker Running %u", doppTimeTrackerRunning);
            ImGui::Text("styleVFXCount %u", styleVFXCount);


            using namespace ACTION_DANTE;
            using namespace ACTION_VERGIL;

            auto name_10438 = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E30);
            if (!name_10438) {
                return;
            }
           


            ImGui::Text("Doppelganger active %u", actorData.doppelganger);
            ImGui::Text("Actor Mode %u", actorData.mode);


            ImGui::Text("IsDevil2 %u", actorData.devil);
            ImGui::Text("Style %u", actorData.style);
            ImGui::Text("StyleRank %u", actorData.styleData.rank);
            ImGui::Text("StyleRank Meter %g", actorData.styleData.meter);
            ImGui::Text("StyleRank Quotient %g", actorData.styleData.quotient);
            ImGui::Text("Permissions %u", actorData.permissions);
            if (actorData.cloneActorBaseAddr) {
                ImGui::Text("StyleRankClone %u", cloneActorData.styleData.rank);
                ImGui::Text("StyleRankClone Meter %g", cloneActorData.styleData.meter);
                ImGui::Text("StyleRankClone Quotient %g", cloneActorData.styleData.quotient);
            }


            /*
            ImGui::Text("Style Indice 0 %u", characterData.styleButtons[0]);
            ImGui::Text("Style Indice 1 %u", characterData.styleButtons[1]);
            ImGui::Text("Style Indice 2 %u", characterData.styleButtons[2]);
            ImGui::Text("Style Indice 3 %u", characterData.styleButtons[3]);
            ImGui::Text("Style Indice 4 %u", characterData.styleButtons[4]);
            ImGui::Text("Style Indice 5 %u", characterData.styleButtons[5]);*/


            ImGui::Text("");
        }


        if (activeConfig.mainOverlayData.showRegionData) {
            ImGui::Text("Region Data");

            auto Function = [&](RegionData& regionData) -> void {
                ImGui::Text("metadataAddr %llX", regionData.metadataAddr);
                ImGui::Text("dataAddr     %llX", regionData.dataAddr);
                ImGui::Text("capacity     %u", regionData.capacity);
                ImGui::Text("boundary     %X", regionData.boundary);
                ImGui::Text("size         %X", regionData.size);
                ImGui::Text("pipe         %u", regionData.pipe);
                ImGui::Text("count        %u", regionData.count);
            };

            auto regionDataAddr = reinterpret_cast<RegionData*>(appBaseAddr + 0xCA8910);
            /*
            dmc3.exe+2C61BF - 4C 8D 25 4A279E00 - lea r12,[dmc3.exe+CA8910]
            dmc3.exe+2C61C6 - 44 8B C5          - mov r8d,ebp
            */

            constexpr new_size_t count = 3;

            for_all(index, count) {
                auto& regionData = regionDataAddr[index];

                ImGui::Text("%.4u", index);
                Function(regionData);
                ImGui::Text("");
            }
        }

        if constexpr (!debug) {
            return;
        }

        ImGui::Text("g_saveIndex     %llu", g_saveIndex);
        ImGui::Text("g_lastSaveIndex %llu", g_lastSaveIndex);
        ImGui::Text("");

        ImGui::Text("g_show     %u", g_show);
        ImGui::Text("g_lastShow %u", g_lastShow);
        ImGui::Text("g_showMain %u", g_showMain);
        ImGui::Text("g_showShop %u", g_showShop);


        {
            auto& io = ImGui::GetIO();

            ImGui::Text("io.NavActive  %u", io.NavActive);
            ImGui::Text("io.NavVisible %u", io.NavVisible);


            [&]() {
                auto contextAddr = ImGui::GetCurrentContext();
                if (!contextAddr) {
                    return;
                }
                auto& context = *contextAddr;

                ImGui::Text("context.NavId %X", context.NavId);
            }();
        }


#ifdef SINGLE

        ImGui::Text("visible     %u", visible);
        ImGui::Text("lastVisible %u", lastVisible);
#else


        ImGui::Text("visibleMain     %u", visibleMain);
        ImGui::Text("lastVisibleMain %u", lastVisibleMain);

        ImGui::Text("visibleShop     %u", visibleShop);
        ImGui::Text("lastVisibleShop %u", lastVisibleShop);

#endif


        [&]() {
            auto pool_10480 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
            if (!pool_10480 || !pool_10480[3]) {
                return;
            }
            auto& actorData = *reinterpret_cast<PlayerActorData*>(pool_10480[3]);

            ImGui::Text("action %u", actorData.action);
        }();

        ImGui::Text("g_shopTimer   %g", g_shopTimer);
        ImGui::Text("g_shopTimeout %g", g_shopTimeout);
        ImGui::Text("");


        ImGui::Text("g_secretMission %u", g_secretMission);


        // [&]()
        // {
        // 	if (g_scene != SCENE::GAME)
        // 	{
        // 		return;
        // 	}

        // 	IntroduceEventData(return);
        // 	IntroduceNextEventData(return);

        // 	ImGui::Text("room          %u", eventData.room        );
        // 	ImGui::Text("position      %u", eventData.position    );
        // 	ImGui::Text("next room     %u", nextEventData.room    );
        // 	ImGui::Text("next position %u", nextEventData.position);
        // }();
    };

    
    OverlayFunction(mainOverlayLabel, activeConfig.mainOverlayData, queuedConfig.mainOverlayData, Function);
}

void MainOverlaySettings() {
    auto Function = [&]() {
        GUI_Checkbox2("Show Focus", activeConfig.mainOverlayData.showFocus, queuedConfig.mainOverlayData.showFocus);
        GUI_Checkbox2("Show FPS", activeConfig.mainOverlayData.showFPS, queuedConfig.mainOverlayData.showFPS);
        GUI_Checkbox2("Show Sizes", activeConfig.mainOverlayData.showSizes, queuedConfig.mainOverlayData.showSizes);
        GUI_Checkbox2("Show Frame Rate Multiplier", activeConfig.mainOverlayData.showFrameRateMultiplier,
            queuedConfig.mainOverlayData.showFrameRateMultiplier);
        GUI_Checkbox2("Show Event Data", activeConfig.mainOverlayData.showEventData, queuedConfig.mainOverlayData.showEventData);
        GUI_Checkbox2("Show Position", activeConfig.mainOverlayData.showPosition, queuedConfig.mainOverlayData.showPosition);
        GUI_Checkbox2("Show Region Data", activeConfig.mainOverlayData.showRegionData, queuedConfig.mainOverlayData.showRegionData);
    };

    OverlaySettings(mainOverlayLabel, activeConfig.mainOverlayData, queuedConfig.mainOverlayData, defaultConfig.mainOverlayData, Function);
}


const char* missionOverlayLabel = "MissionOverlay";

void MissionOverlayWindow(size_t defaultFontSize) {
    auto Function = [&]() {
        ImGui::Text("Mission");

        ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 1.1f]);


        auto name_10723 = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E30);
        if (!name_10723) {
            return;
        }
        auto& missionData = *reinterpret_cast<MissionData*>(name_10723);


        auto timeData = TimeData(static_cast<float>(missionData.frameCount), activeConfig.frameRate
            // 60.0f // @Update
        );

        ImGui::Text("Time           %02u:%02u:%02u.%03u", timeData.hours, timeData.minutes, timeData.seconds, timeData.milliseconds);


        ImGui::Text("Damage         %u", missionData.damage);
        ImGui::Text("Orbs Collected %u", missionData.orbsCollected);
        ImGui::Text("Items Used     %u", missionData.itemsUsed);
        ImGui::Text("Kill Count     %u", missionData.killCount);

        auto pool_10621 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
        if (!pool_10621 || !pool_10621[3]) {
            return;
        }
        auto& actorData = *reinterpret_cast<PlayerActorData*>(pool_10621[3]);

        auto stylePoints = (actorData.styleData.quotient * 100.0f);

        ImGui::Text("Style Points   %.2f", stylePoints);
    };

    OverlayFunction(missionOverlayLabel, activeConfig.missionOverlayData, queuedConfig.missionOverlayData, Function);
}

void MissionOverlaySettings() {
    OverlaySettings(
        missionOverlayLabel, activeConfig.missionOverlayData, queuedConfig.missionOverlayData, defaultConfig.missionOverlayData);
}


const char* bossLadyActionsOverlayLabel = "BossLadyActionsOverlay";

void BossLadyActionsOverlayWindow() {
    auto Function = [&]() {
        ImGui::Text("Boss Lady Actions\n"
                    "\n"
                    "Lock-On + Forward + A: Trooper Roll\n"
                    "Lock-On + Right   + A: Wheel Right\n"
                    "Lock-On + Back    + A: Wheel Back\n"
                    "Lock-On + Left    + A: Wheel Left\n"
                    "\n"
                    "Right Trigger: Reload Pistol\n"
                    "Left  Trigger: Reload SMG\n"
                    "\n"
                    "Lock-On + Back + X: Pistol Fall Back Shoot (hold for crossbow)\n"
                    "\n"
                    "B: SMG Roundhouse\n"
                    "\n"
                    "                    Y: Kalina Ann Hook\n"
                    "Lock-On + Forward + Y: Kalina Ann Charged Shot\n"
                    "Lock-On + Back    + Y: Kalina Ann Hysteric\n"
                    "\n"
                    "Left Shoulder: Grenades\n");
    };

    OverlayFunction(
        bossLadyActionsOverlayLabel, activeConfig.bossLadyActionsOverlayData, queuedConfig.bossLadyActionsOverlayData, Function);
}

void BossLadyActionsOverlaySettings() {
    OverlaySettings(bossLadyActionsOverlayLabel, activeConfig.bossLadyActionsOverlayData, queuedConfig.bossLadyActionsOverlayData,
        defaultConfig.bossLadyActionsOverlayData);
}


const char* bossVergilActionsOverlayLabel = "BossVergilActionsOverlay";

void BossVergilActionsOverlayWindow() {
    auto Function = [&]() {
        ImGui::Text("Boss Vergil Actions\n"
                    "\n"
                    "                    Y: Yamato Deflect\n"
                    "Lock-On + Forward + Y: Yamato Super Judgement Cut Follow\n"
                    "Lock-On + Back    + Y: Yamato Super Judgement Cut\n"
                    "\n"
                    "B: Block\n"
                    "\n"
                    "                    X: Shield\n"
                    "Lock-On + Forward + X: Strong Shield\n"
                    "Lock-On + Back    + X: Strong Shield 2\n"
                    "\n"
                    "Left Shoulder: Toggle Devil Form\n"
                    "\n"
                    "Left  Trigger: Taunt\n"
                    "Right Trigger: Rest in Peace\n");
    };

    OverlayFunction(
        bossVergilActionsOverlayLabel, activeConfig.bossVergilActionsOverlayData, queuedConfig.bossVergilActionsOverlayData, Function);
}

void BossVergilActionsOverlaySettings() {
    OverlaySettings(bossVergilActionsOverlayLabel, activeConfig.bossVergilActionsOverlayData, queuedConfig.bossVergilActionsOverlayData,
        defaultConfig.bossVergilActionsOverlayData);
}

void AdjustBackgroundTransparency() {
   	
    switch (queuedConfig.GUI.transparencyMode) {
        // OFF
    case 0 :
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.207f, 0.156f, 0.168f, 1.0f));
        break;
        
        // STATIC
    case 1 :
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.207f, 0.156f, 0.168f, queuedConfig.GUI.transparencyValue));
        break;

        //DYNAMIC
    case 2 :

        if (g_inGame) {
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.207f, 0.156f, 0.168f, queuedConfig.GUI.transparencyValue));

            
        }
		else {
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.207f, 0.156f, 0.168f, 1.0f));
		}
        
        /*ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.207f, 0.156f, 0.168f, 1.0f));*/
        
        break;
    }

}


void InterfaceSection(size_t defaultFontSize) {
    
	const float itemWidth = defaultFontSize * 8.0f;

    ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);


    ImGui::Text("GUI OPTIONS");

	ImGui::PopFont();

	UI::SeparatorEx(defaultFontSize * 23.35f);

    ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
    ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	ImGui::PushItemWidth(itemWidth);
	ImGui::Text("");

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;

		if (ImGui::BeginTable("GUIOptiomsTable", 2)) {

			ImGui::TableSetupColumn("b1", 0, columnWidth);
			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

            ImGui::PushItemWidth(itemWidth);
            UI::Combo2("Transparency Mode", GUITransparencyNames, activeConfig.GUI.transparencyMode, queuedConfig.GUI.transparencyMode);
            ImGui::PopItemWidth();

			ImGui::TableNextColumn();
            

            ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();


			GUI_Input2<float>("Alpha", activeConfig.GUI.transparencyValue, queuedConfig.GUI.transparencyValue, 0.1f, "%g",
				ImGuiInputTextFlags_EnterReturnsTrue);

            ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth);
			if (GUI_InputDefault2("Global Scale", activeConfig.globalScale, queuedConfig.globalScale, defaultConfig.globalScale, 0.1f, "%g",
				ImGuiInputTextFlags_EnterReturnsTrue)) {
				UpdateGlobalScale();
			}
			ImGui::PopItemWidth();

			ImGui::EndTable();
		}
	}

    ImGui::PopFont();
    ImGui::PopStyleColor();

    ImGui::Text("");

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);


	ImGui::Text("HUD OPTIONS");

	ImGui::PopFont();

	UI::SeparatorEx(defaultFontSize * 23.35f);

    ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
    ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

    ImGui::Text("");

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;

		if (ImGui::BeginTable("HUDOptiomsTable", 2)) {

			ImGui::TableSetupColumn("b1", 0, columnWidth);
			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth);
            if (UI::Combo2Vector("Select HUD", HUDdirectories, activeConfig.selectedHUD, queuedConfig.selectedHUD)) {
                copyHUDtoGame();
            }
			ImGui::PopItemWidth();

			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();
            

			ImGui::PushItemWidth(itemWidth);
			if (GUI_Checkbox2("Hide Main", activeConfig.hideMainHUD, queuedConfig.hideMainHUD)) {
				ToggleHideMainHUD(activeConfig.hideMainHUD);
			}
			ImGui::PopItemWidth();

            ImGui::TableNextColumn();

			if (GUI_Checkbox2("Always Show Main", activeConfig.forceVisibleHUD, queuedConfig.forceVisibleHUD)) {
				ToggleForceVisibleHUD(activeConfig.forceVisibleHUD);
			}



			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();


			if (GUI_Checkbox2("Hide Lock-On", activeConfig.hideLockOn, queuedConfig.hideLockOn)) {
				ToggleHideLockOn(activeConfig.hideLockOn);
			}

			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth);
			if (GUI_Checkbox2("Hide Boss", activeConfig.hideBossHUD, queuedConfig.hideBossHUD)) {
				ToggleHideBossHUD(activeConfig.hideBossHUD);
			}
			ImGui::PopItemWidth();

			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

            if (GUI_Checkbox2("Original HUD Positionings", activeConfig.originalHUDpositionings, queuedConfig.originalHUDpositionings)) {
                ToggleOriginalHUDPositionings(activeConfig.originalHUDpositionings);
            }

            ImGui::TableNextColumn();

            GUI_Checkbox2("Disable Style Rank Fadeout", activeConfig.disableStyleRankHudFadeout, queuedConfig.disableStyleRankHudFadeout);


			ImGui::EndTable();
		}
	}

    ImGui::PopFont();
    ImGui::PopStyleColor();
    
    ImGui::PopItemWidth();

	ImGui::Text("");
	BarsSection(defaultFontSize);
	
// 
// 	GUI_SectionEnd();
// 	ImGui::Text("");
// 
// 	GUI_SectionStart("Main");
// 
// 	MainOverlaySettings();
// 
// 	GUI_SectionEnd();
// 	ImGui::Text("");
// 
// 
// 	GUI_SectionStart("Mission");
// 
// 	MissionOverlaySettings();
// 
// 	GUI_SectionEnd();
// 	ImGui::Text("");
// 
// 
// 	GUI_SectionStart("Boss Lady Actions");
// 
// 	BossLadyActionsOverlaySettings();
// 
// 	GUI_SectionEnd();
// 	ImGui::Text("");
// 
// 
// 	GUI_SectionStart("Boss Vergil Actions");
// 
// 	BossVergilActionsOverlaySettings();
// 
// 
// 	ImGui::Text("");

}

#pragma endregion

#pragma region Repair

void Repair() {
    if (ImGui::CollapsingHeader("Repair")) {
        ImGui::Text("");

        DescriptionHelper("Fix wrong values. "
                          "Can be accessed while in the mission select menu. "
                          "Save your game after applying a fix.");
        ImGui::Text("");


        bool condition = (g_scene != SCENE::MISSION_SELECT);

        GUI_PushDisable(condition);

        if (GUI_Button("Reset Weapons")) {
            [&]() {
                auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);


                SetMemory(sessionData.weapons, 0, sizeof(sessionData.weapons));

                switch (sessionData.character) {
                case CHARACTER::DANTE: {
                    sessionData.weapons[0] = WEAPON::REBELLION;
                    sessionData.weapons[1] = WEAPON::CERBERUS;
                    sessionData.weapons[2] = WEAPON::EBONY_IVORY;
                    sessionData.weapons[3] = WEAPON::SHOTGUN;
                    sessionData.weapons[4] = WEAPON::VOID;

                    break;
                }
                case CHARACTER::VERGIL: {
                    sessionData.weapons[0] = WEAPON::YAMATO_VERGIL;
                    sessionData.weapons[1] = WEAPON::BEOWULF_VERGIL;
                    sessionData.weapons[2] = WEAPON::YAMATO_FORCE_EDGE;
                    sessionData.weapons[3] = WEAPON::VOID;
                    sessionData.weapons[4] = WEAPON::VOID;

                    break;
                }
                }

                sessionData.meleeWeaponIndex  = 0;
                sessionData.rangedWeaponIndex = 2;
            }();
        }


        if (GUI_Button("Reset Weapon Levels")) {
            [&]() {
                auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);


                SetMemory(sessionData.rangedWeaponLevels, 0, sizeof(sessionData.rangedWeaponLevels));
            }();
        }

        GUI_PopDisable(condition);


        ImGui::Text("");
    }
}

#pragma endregion

#pragma region Speed

const char* devilSpeedNamesDante[] = {
    "Rebellion",
    "Cerberus",
    "Agni & Rudra",
    "Nevan",
    "Beowulf",
    "Sparda",
};

const char* devilSpeedNamesVergil[] = {
    "Yamato",
    "Beowulf",
    "Yamato & Force Edge",
    "Nero Angelo Yamato",
    "Nero Angelo Beowulf",
};


// @Todo: EnterReturnsTrue.
// @Todo: Apply rounding.

template <typename T>
bool GUI_InputDefault2Speed(
    const char* label, T& var, T& var2, T& defaultVar, const T step = 1, const char* format = 0, ImGuiInputTextFlags flags = 0) {
    auto update = GUI_InputDefault2(label, var, var2, defaultVar, step, format, flags);

    if (update) {
        Speed::Toggle(true);
    }

    return update;
}

void SpeedSection() {
    if (ImGui::CollapsingHeader("Speed")) {
        ImGui::Text("");


        if (GUI_ResetButton()) {
            CopyMemory(&queuedConfig.Speed, &defaultConfig.Speed, sizeof(queuedConfig.Speed));
            CopyMemory(&activeConfig.Speed, &queuedConfig.Speed, sizeof(activeConfig.Speed));

            Speed::Toggle(false);
        }
        ImGui::Text("");

        ImGui::PushItemWidth(200);

        GUI_InputDefault2Speed("Main", activeConfig.Speed.mainSpeed, queuedConfig.Speed.mainSpeed, defaultConfig.Speed.mainSpeed, 0.1f,
            "%g", ImGuiInputTextFlags_EnterReturnsTrue);
        GUI_InputDefault2Speed("Turbo", activeConfig.Speed.turbo, queuedConfig.Speed.turbo, defaultConfig.Speed.turbo, 0.1f, "%g",
            ImGuiInputTextFlags_EnterReturnsTrue);
        GUI_InputDefault2Speed("Enemy Actor", activeConfig.Speed.enemy, queuedConfig.Speed.enemy, defaultConfig.Speed.enemy, 0.1f, "%g",
            ImGuiInputTextFlags_EnterReturnsTrue);
        GUI_SectionEnd();
        ImGui::Text("");

        ImGui::Text("Player Actor");
        ImGui::SameLine();
        TooltipHelper("(?)", "Requires enabled Actor module.");
        ImGui::Text("");

        GUI_InputDefault2Speed("Human", activeConfig.Speed.human, queuedConfig.Speed.human, defaultConfig.Speed.human, 0.1f, "%g",
            ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::Text("");

        ImGui::Text("Devil Dante");

        old_for_all(uint8, index, countof(activeConfig.Speed.devilDante)) {
            GUI_InputDefault2Speed(devilSpeedNamesDante[index], activeConfig.Speed.devilDante[index], queuedConfig.Speed.devilDante[index],
                defaultConfig.Speed.devilDante[index], 0.1f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
        }
        // ImGui::Text("");

        ImGui::Text("Devil Vergil");

        old_for_all(uint8, index, countof(activeConfig.Speed.devilVergil)) {
            GUI_InputDefault2Speed(devilSpeedNamesVergil[index], activeConfig.Speed.devilVergil[index],
                queuedConfig.Speed.devilVergil[index], defaultConfig.Speed.devilVergil[index], 0.1f, "%g",
                ImGuiInputTextFlags_EnterReturnsTrue);
        }
        GUI_SectionEnd();
        ImGui::Text("");

        GUI_SectionStart("Quicksilver");
        GUI_InputDefault2Speed("Actor", activeConfig.Speed.quicksilverPlayerActor, queuedConfig.Speed.quicksilverPlayerActor,
            defaultConfig.Speed.quicksilverPlayerActor, 0.1f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
        GUI_InputDefault2Speed("Enemy", activeConfig.Speed.quicksilverEnemyActor, queuedConfig.Speed.quicksilverEnemyActor,
            defaultConfig.Speed.quicksilverEnemyActor, 0.1f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);

        ImGui::PopItemWidth();


        ImGui::Text("");
    }
}

#pragma endregion

#pragma region System

const char* Graphics_vSyncNames[] = {
    "Auto",
    "Force Off",
    "Force On",
};

const char* Sound_channelNames[CHANNEL::MAX] = {
    "System",
    "Common",
    "Style Weapon",
    "Weapon 1",
    "Weapon 2",
    "Weapon 3",
    "Weapon 4",
    "Enemy",
    "Room",
    "Music",
    "Demo",
};


void SystemSection(size_t defaultFontSize) {
	const float itemWidth = defaultFontSize * 8.0f;
	float smallerComboMult = 0.7f;

	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);


	ImGui::Text("GRAPHICS / WINDOW");

	ImGui::PopFont();

	UI::SeparatorEx(defaultFontSize * 23.35f);


	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	ImGui::PushItemWidth(itemWidth * smallerComboMult);
	ImGui::Text("");

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;

		if (ImGui::BeginTable("GraphicsWindowOptionsTable", 2)) {

			ImGui::TableSetupColumn("b1", 0, columnWidth);
			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * 0.8f);
			if (GUI_InputDefault2<float>("Frame Rate", activeConfig.frameRate, queuedConfig.frameRate, defaultConfig.frameRate, 1, "%.2f",
				ImGuiInputTextFlags_EnterReturnsTrue)) {
				UpdateFrameRate();

				if (activeConfig.framerateResponsiveGameSpeed) {

					activeConfig.Speed.turbo = 1.2 / (activeConfig.frameRate / 60);
					queuedConfig.Speed.turbo = 1.2 / (activeConfig.frameRate / 60);

					activeConfig.Speed.mainSpeed = 1.0 / (activeConfig.frameRate / 60);
					queuedConfig.Speed.mainSpeed = 1.0 / (activeConfig.frameRate / 60);

				}
			}
			ImGui::PopItemWidth();

            ImGui::TableNextColumn();

			if (GUI_Checkbox2("Disable Blending Effects", activeConfig.disableBlendingEffects, queuedConfig.disableBlendingEffects)) {
				DisableBlendingEffects(activeConfig.disableBlendingEffects);
			}

			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			UI::Combo2("V-Sync", Graphics_vSyncNames, activeConfig.vSync, queuedConfig.vSync);

            ImGui::TableNextColumn();

			if (GUI_Checkbox2("Force Focus", activeConfig.forceWindowFocus, queuedConfig.forceWindowFocus)) {
				ToggleForceWindowFocus(activeConfig.forceWindowFocus);
			}

			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			if (GUI_Checkbox2("Frame Rate-Responsive Game Speed", activeConfig.framerateResponsiveGameSpeed, queuedConfig.framerateResponsiveGameSpeed)) {
				activeConfig.Speed.turbo = 1.2 / (activeConfig.frameRate / 60);
				queuedConfig.Speed.turbo = 1.2 / (activeConfig.frameRate / 60);

				activeConfig.Speed.mainSpeed = 1.0 / (activeConfig.frameRate / 60);
				queuedConfig.Speed.mainSpeed = 1.0 / (activeConfig.frameRate / 60);
			}
			ImGui::SameLine();
			TooltipHelper("(?)", "Adjusts Game Speed based on the Frame Rate setting \n"
				"to ensure gameplay stays consistent across different frame rates. \n\n"
				"WARNING: Playing at higher frame rates will greatly reduce input lag, but \n"
				"various gameplay issues may be introduced, such as enemy projectiles being too fast. \n"
				"Help us fix those issues by reporting them individually on our GitHub's issue tracker \n"
				"using the 'high frame issue' label: https://github.com/berthrage/Devil-May-Cry-3-Crimson/issues");

			ImGui::TableNextColumn();


			GUI_Checkbox2("Hide Mouse Cursor", activeConfig.hideMouseCursor, queuedConfig.hideMouseCursor);

			ImGui::EndTable();
		}
	}


	ImGui::Text("");

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);


	ImGui::Text("MISC");

	ImGui::PopFont();

	UI::SeparatorEx(defaultFontSize * 23.35f);

    ImGui::Text("");

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;

		if (ImGui::BeginTable("GraphicsWindowOptionsTable", 2)) {

			ImGui::TableSetupColumn("b1", 0, columnWidth);
			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * 0.8f);
			if (GUI_Checkbox2("Skip Intro", activeConfig.skipIntro, queuedConfig.skipIntro)) {
				ToggleSkipIntro(activeConfig.skipIntro);
			}
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("Skip Cutscenes", activeConfig.skipCutscenes, queuedConfig.skipCutscenes)) {
				ToggleSkipCutscenes(activeConfig.skipCutscenes);
			}

            // Enable File Mods will be a json file only option for now, don't see much need for it in the GUI. - Mia
// 			ImGui::TableNextRow(0, rowWidth);
// 			ImGui::TableNextColumn();
// 
//             GUI_Checkbox2("Enable File Mods", activeConfig.enableFileMods, queuedConfig.enableFileMods);
//             ImGui::SameLine();
// 			TooltipHelper("(?)", "WARNING: Necessary for any and all file mods you may have, \n"
// 				"disabling this will disable any file replacement mod, including custom HUDs and Models.");

		
			ImGui::EndTable();
		}
	}



// 
// 	GUI_SectionStart("Input");
// 
// 	
// 
// 
// 	ImGui::PushItemWidth(300);
// 
// 	if (ImGui::InputText(
// 		"Gamepad Name", queuedConfig.gamepadName, sizeof(queuedConfig.gamepadName), ImGuiInputTextFlags_EnterReturnsTrue)) {
// 		::GUI::save = true;
// 	}
// 
// 	GUI_Input2<byte8>(
// 		"Gamepad Button", activeConfig.gamepadButton, queuedConfig.gamepadButton, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
// 	ImGui::SameLine();
// 	TooltipHelper("(?)", "Toggle Show Main");
// 
// 	ImGui::PopItemWidth();

	

	ImGui::PopFont();
	ImGui::PopStyleColor();

// 	if (GUI_ResetButton()) {
// 		CopyMemory(&queuedConfig.skipIntro, &defaultConfig.skipIntro, sizeof(queuedConfig.skipIntro));
// 		CopyMemory(&activeConfig.skipIntro, &queuedConfig.skipIntro, sizeof(activeConfig.skipIntro));
// 
// 		CopyMemory(&queuedConfig.skipCutscenes, &defaultConfig.skipCutscenes, sizeof(queuedConfig.skipCutscenes));
// 		CopyMemory(&activeConfig.skipCutscenes, &queuedConfig.skipCutscenes, sizeof(activeConfig.skipCutscenes));
// 
// 		ToggleSkipIntro(activeConfig.skipIntro);
// 		ToggleSkipCutscenes(activeConfig.skipCutscenes);
// 
// 		CopyMemory(&queuedConfig.enableFileMods, &defaultConfig.enableFileMods, sizeof(queuedConfig.enableFileMods));
// 		CopyMemory(&activeConfig.enableFileMods, &queuedConfig.enableFileMods, sizeof(activeConfig.enableFileMods));
// 
// 
// 		CopyMemory(&queuedConfig.frameRate, &defaultConfig.frameRate, sizeof(queuedConfig.frameRate));
// 		CopyMemory(&activeConfig.frameRate, &queuedConfig.frameRate, sizeof(activeConfig.frameRate));
// 
// 		CopyMemory(&queuedConfig.vSync, &defaultConfig.vSync, sizeof(queuedConfig.vSync));
// 		CopyMemory(&activeConfig.vSync, &queuedConfig.vSync, sizeof(activeConfig.vSync));
// 
// 		UpdateFrameRate();
// 
// 		CopyMemory(&queuedConfig.hideMainHUD, &defaultConfig.hideMainHUD, sizeof(queuedConfig.hideMainHUD));
// 		CopyMemory(&activeConfig.hideMainHUD, &queuedConfig.hideMainHUD, sizeof(activeConfig.hideMainHUD));
// 
// 		ToggleHideMainHUD(activeConfig.hideMainHUD);
// 
// 		CopyMemory(&queuedConfig.hideLockOn, &defaultConfig.hideLockOn, sizeof(queuedConfig.hideLockOn));
// 		CopyMemory(&activeConfig.hideLockOn, &queuedConfig.hideLockOn, sizeof(activeConfig.hideLockOn));
// 
// 		ToggleHideLockOn(activeConfig.hideLockOn);
// 
// 		CopyMemory(&queuedConfig.hideBossHUD, &defaultConfig.hideBossHUD, sizeof(queuedConfig.hideBossHUD));
// 		CopyMemory(&activeConfig.hideBossHUD, &queuedConfig.hideBossHUD, sizeof(activeConfig.hideBossHUD));
// 
// 		ToggleHideBossHUD(activeConfig.hideBossHUD);
// 
// 		CopyMemory(&queuedConfig.hideMouseCursor, &defaultConfig.hideMouseCursor, sizeof(queuedConfig.hideMouseCursor));
// 		CopyMemory(&activeConfig.hideMouseCursor, &queuedConfig.hideMouseCursor, sizeof(activeConfig.hideMouseCursor));
// 
// 		CopyMemory(&queuedConfig.gamepadName, &defaultConfig.gamepadName, sizeof(queuedConfig.gamepadName));
// 		CopyMemory(&activeConfig.gamepadName, &queuedConfig.gamepadName, sizeof(activeConfig.gamepadName));
// 
// 		CopyMemory(&queuedConfig.gamepadButton, &defaultConfig.gamepadButton, sizeof(queuedConfig.gamepadButton));
// 		CopyMemory(&activeConfig.gamepadButton, &queuedConfig.gamepadButton, sizeof(activeConfig.gamepadButton));
// 
// 
// 		CopyMemory(&queuedConfig.channelVolumes, &defaultConfig.channelVolumes, sizeof(queuedConfig.channelVolumes));
// 		CopyMemory(&activeConfig.channelVolumes, &queuedConfig.channelVolumes, sizeof(activeConfig.channelVolumes));
// 
// 		UpdateVolumes();
// 		CopyMemory(&queuedConfig.soundIgnoreEnemyData, &defaultConfig.soundIgnoreEnemyData, sizeof(queuedConfig.soundIgnoreEnemyData));
// 		CopyMemory(&activeConfig.soundIgnoreEnemyData, &queuedConfig.soundIgnoreEnemyData, sizeof(activeConfig.soundIgnoreEnemyData));
// 
// 
// 		CopyMemory(&queuedConfig.windowPosX, &defaultConfig.windowPosX, sizeof(queuedConfig.windowPosX));
// 		CopyMemory(&activeConfig.windowPosX, &queuedConfig.windowPosX, sizeof(activeConfig.windowPosX));
// 
// 		CopyMemory(&queuedConfig.windowPosY, &defaultConfig.windowPosY, sizeof(queuedConfig.windowPosY));
// 		CopyMemory(&activeConfig.windowPosY, &queuedConfig.windowPosY, sizeof(activeConfig.windowPosY));
// 
// 		CopyMemory(&queuedConfig.forceWindowFocus, &defaultConfig.forceWindowFocus, sizeof(queuedConfig.forceWindowFocus));
// 		CopyMemory(&activeConfig.forceWindowFocus, &queuedConfig.forceWindowFocus, sizeof(activeConfig.forceWindowFocus));
// 
// 		ToggleForceWindowFocus(activeConfig.forceWindowFocus);
// 	}
    
}

#pragma endregion

#pragma region Teleporter

void Teleporter() {
    if (ImGui::CollapsingHeader("Teleporter")) {
        ImGui::Text("");


        [&]() {
            if (!InGame()) {
                ImGui::Text("Invalid Pointer");

                return;
            }

            auto pool_11962 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
            if (!pool_11962 || !pool_11962[8]) {
                return;
            }
            auto& eventData = *reinterpret_cast<EventData*>(pool_11962[8]);

            auto pool_12013 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
            if (!pool_12013 || !pool_12013[12]) {
                return;
            }
            auto& nextEventData = *reinterpret_cast<NextEventData*>(pool_12013[12]);


            if (GUI_Button("Clear")) {
                nextEventData.position = nextEventData.room = 0;
            }
            ImGui::SameLine();

            if (GUI_Button("Current")) {
                nextEventData.room     = static_cast<uint16>(eventData.room);
                nextEventData.position = static_cast<uint16>(eventData.position);
            }
            ImGui::Text("");


            constexpr float width = 150.0f;

            ImGui::PushItemWidth(width);

            ImGui::Text("Current");

            GUI_Input<uint32>("Room", eventData.room, 0, "%u", ImGuiInputTextFlags_ReadOnly);

            GUI_Input<uint32>("Position", eventData.position, 0, "%u", ImGuiInputTextFlags_ReadOnly);

            ImGui::Text("Next");

            GUI_Input<uint16>("Room", nextEventData.room, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);

            GUI_Input<uint16>("Position", nextEventData.position, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);

            if (GUI_Button("Teleport", ImVec2(width, ImGui::GetFrameHeight()))) {
                eventData.event = EVENT::TELEPORT;
            }

            ImGui::PopItemWidth();
        }();


        ImGui::Text("");
    }
}

#pragma endregion

#pragma region Textures

void WeaponWheel() {
    if (ImGui::CollapsingHeader("Weapon Wheel")) {
        ImGui::Text("");

        WeaponSwitchControllerSettings();

        ImGui::Text("");
    }
}

#pragma endregion

#pragma region Training

void TrainingSection() {
    if (ImGui::CollapsingHeader("Training")) {
        ImGui::Text("");


        if (GUI_ResetButton()) {
            CopyMemory(&queuedConfig.infiniteHitPoints, &defaultConfig.infiniteHitPoints, sizeof(queuedConfig.infiniteHitPoints));
            CopyMemory(&activeConfig.infiniteHitPoints, &queuedConfig.infiniteHitPoints, sizeof(activeConfig.infiniteHitPoints));

            ToggleInfiniteHitPoints(activeConfig.infiniteHitPoints);

            CopyMemory(&queuedConfig.infiniteMagicPoints, &defaultConfig.infiniteMagicPoints, sizeof(queuedConfig.infiniteMagicPoints));
            CopyMemory(&activeConfig.infiniteMagicPoints, &queuedConfig.infiniteMagicPoints, sizeof(activeConfig.infiniteMagicPoints));

            ToggleInfiniteMagicPoints(activeConfig.infiniteMagicPoints);

            CopyMemory(&queuedConfig.disableTimer, &defaultConfig.disableTimer, sizeof(queuedConfig.disableTimer));
            CopyMemory(&activeConfig.disableTimer, &queuedConfig.disableTimer, sizeof(activeConfig.disableTimer));

            ToggleDisableTimer(activeConfig.disableTimer);

            CopyMemory(&queuedConfig.infiniteBullets, &defaultConfig.infiniteBullets, sizeof(queuedConfig.infiniteBullets));
            CopyMemory(&activeConfig.infiniteBullets, &queuedConfig.infiniteBullets, sizeof(activeConfig.infiniteBullets));

            ToggleInfiniteBullets(activeConfig.infiniteBullets);
        }
        ImGui::Text("");

        if (GUI_Checkbox2("Infinite Hit Points", activeConfig.infiniteHitPoints, activeConfig.infiniteHitPoints)) {
            ToggleInfiniteHitPoints(activeConfig.infiniteHitPoints);
        }

        if (GUI_Checkbox2("Infinite Magic Points", activeConfig.infiniteMagicPoints, queuedConfig.infiniteMagicPoints)) {
            ToggleInfiniteMagicPoints(activeConfig.infiniteMagicPoints);
        }

        if (GUI_Checkbox2("Disable Timer", activeConfig.disableTimer, queuedConfig.disableTimer)) {
            ToggleDisableTimer(activeConfig.disableTimer);
        }

        if (GUI_Checkbox2("Infinite Bullets", activeConfig.infiniteBullets, queuedConfig.infiniteBullets)) {
            ToggleInfiniteBullets(activeConfig.infiniteBullets);
        }
        ImGui::SameLine();
        TooltipHelper("(?)", "For Boss Lady.");


        ImGui::Text("");
    }
}

#pragma endregion

#pragma region Training

void SoundVisualSection(size_t defaultFontSize) {
    const char* changeGunNewNames[] = {"DMC3 Default", "New"};
    const char* changeDevilArmNewNames[] = {"DMC3 Default", "New"};

	const float itemWidth = defaultFontSize * 8.0f;
    float smallerComboMult = 0.7f;

	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);


	ImGui::Text("CRIMSON SFX OPTIONS");

	ImGui::PopFont();

	UI::SeparatorEx(defaultFontSize * 23.35f);

    

	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	ImGui::PushItemWidth(itemWidth);
	ImGui::Text("");


	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;


		if (ImGui::BeginTable("WeaponwheelSFXTable", 3)) {

			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableNextRow(0, rowWidth * 0.5f);
			ImGui::TableNextColumn();

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("WEAPON WHEEL SFX");
			ImGui::PopFont();

	
			ImGui::PushItemWidth(itemWidth * smallerComboMult);
            UI::Combo2<uint8>("Change Gun", changeGunNewNames, activeConfig.SFX.changeGunNew, queuedConfig.SFX.changeGunNew);
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			UI::Combo2<uint8>(
				"Change Devil Arm", changeDevilArmNewNames, activeConfig.SFX.changeDevilArmNew, queuedConfig.SFX.changeDevilArmNew);
			ImGui::PopItemWidth();

            ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_InputDefault2<uint32>("Volume", activeConfig.SFX.changeWeaponVolume, queuedConfig.SFX.changeWeaponVolume,
				defaultConfig.SFX.changeWeaponVolume, 10, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::PopItemWidth();


			ImGui::TableNextColumn();

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("STYLE SWITCH SFX");
			ImGui::PopFont();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_InputDefault2<uint32>("Effect Volume", activeConfig.SFX.styleChangeEffectVolume,
				queuedConfig.SFX.styleChangeEffectVolume, defaultConfig.SFX.styleChangeEffectVolume, 10, "%u",
				ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_InputDefault2<uint32>("VO Volume", activeConfig.SFX.styleChangeVOVolume, queuedConfig.SFX.styleChangeVOVolume,
				defaultConfig.SFX.styleChangeVOVolume, 10, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

            
			ImGui::TableNextColumn();

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("STYLE RANK ANNOUNCER");
			ImGui::PopFont();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_InputDefault2<uint32>("Volume", activeConfig.SFX.styleRankAnnouncerVolume,
				queuedConfig.SFX.styleRankAnnouncerVolume, defaultConfig.SFX.styleRankAnnouncerVolume, 10, "%u",
				ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_InputDefault2<uint32>("Cooldown", activeConfig.SFX.styleRankAnnouncerCooldownSeconds,
				queuedConfig.SFX.styleRankAnnouncerCooldownSeconds, defaultConfig.SFX.styleRankAnnouncerCooldownSeconds, 1, "%u",
				ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::SameLine();
			TooltipHelper("(?)", "Duration until the Announcer can repeat the same line.\n"
				"\n"
				"(in Seconds)");
			ImGui::PopItemWidth();


			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

            ImGui::Text("");
			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("DEVIL TRIGGER SFX");
			ImGui::PopFont();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_InputDefault2<uint32>("Activation L1 Volume", activeConfig.SFX.devilTriggerInL1Volume, queuedConfig.SFX.devilTriggerInL1Volume,
				defaultConfig.SFX.devilTriggerInL1Volume, 10, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_InputDefault2<uint32>("Activation L2 Volume", activeConfig.SFX.devilTriggerInL2Volume, queuedConfig.SFX.devilTriggerInL2Volume,
				defaultConfig.SFX.devilTriggerInL2Volume, 10, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_InputDefault2<uint32>("DT Ready Volume", activeConfig.SFX.devilTriggerReadyVolume, queuedConfig.SFX.devilTriggerReadyVolume,
				defaultConfig.SFX.devilTriggerReadyVolume, 10, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			ImGui::Text("");

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("DOPPELGANGER SFX");
			ImGui::PopFont();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_InputDefault2<uint32>("Activation Volume", activeConfig.SFX.doppelgangerInVolume, queuedConfig.SFX.doppelgangerInVolume,
				defaultConfig.SFX.doppelgangerInVolume, 10, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(itemWidth* smallerComboMult);
			GUI_InputDefault2<uint32>("Deactivation Volume", activeConfig.SFX.doppelgangerOutVolume, queuedConfig.SFX.doppelgangerOutVolume,
				defaultConfig.SFX.doppelgangerOutVolume, 10, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			ImGui::Text("");

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("QUICKSILVER SFX");
			ImGui::PopFont();

			ImGui::PushItemWidth(itemWidth* smallerComboMult);
			GUI_InputDefault2<uint32>("Activation Volume", activeConfig.SFX.quicksilverInVolume, queuedConfig.SFX.quicksilverInVolume,
				defaultConfig.SFX.quicksilverInVolume, 10, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();


			ImGui::EndTable();
		}
	}

    ImGui::Text("");

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);


	ImGui::Text("IM-GAME SOUND VOLUMES");

	ImGui::PopFont();

    UI::SeparatorEx(defaultFontSize * 23.35f);

	
	ImGui::Text("");

// 	if (GUI_ResetButton()) {
// 		CopyMemory(&queuedConfig.channelVolumes, &defaultConfig.channelVolumes, sizeof(queuedConfig.channelVolumes));
// 		CopyMemory(&activeConfig.channelVolumes, &queuedConfig.channelVolumes, sizeof(activeConfig.channelVolumes));
// 
// 
// 		UpdateVolumes();
// 	}
// 	ImGui::Text("");

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;


		if (ImGui::BeginTable("InGameVolumesTable", 3)) {

			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
            ImGui::TableNextRow(0, rowWidth * 0.1f);
			
			for (int channelIndex = 0; channelIndex < CHANNEL::MAX; channelIndex++) {

                if (channelIndex % 3 == 0) {
                    ImGui::TableNextRow(0, rowWidth * 0.1f);
                }

                ImGui::TableNextColumn();

                ImGui::PushItemWidth(itemWidth* smallerComboMult);
				if (GUI_InputDefault2(Sound_channelNames[channelIndex], activeConfig.channelVolumes[channelIndex],
					queuedConfig.channelVolumes[channelIndex], defaultConfig.channelVolumes[channelIndex], 0.1f, "%g",
					ImGuiInputTextFlags_EnterReturnsTrue)) {
					SetVolume(channelIndex, activeConfig.channelVolumes[channelIndex]);
				}
                ImGui::PopItemWidth();

			}
			

			ImGui::EndTable();
		}
	}

	GUI_Checkbox2("Ignore Enemy Data", activeConfig.soundIgnoreEnemyData, queuedConfig.soundIgnoreEnemyData);
	ImGui::SameLine();
	TooltipHelper("(?)", "Do not look at enemy data when updating the global indices.\n"
		"Most, if not all enemies will lose their sound effects if enabled.\n"
		"Intended as a workaround for playable bosses when the sound effect\n"
		"interferences from other enemies get too annoying. - serpentiem");

	ImGui::Text("");

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);


	ImGui::Text("VFX COLORS");

	ImGui::PopFont();

	UI::SeparatorEx(defaultFontSize * 23.35f);

// 	if (GUI_Button("DMC3 Default")) {
// 		CopyMemory(&queuedConfig.Color, &defaultConfig.Color, sizeof(queuedConfig.Color));
// 		CopyMemory(&activeConfig.Color, &queuedConfig.Color, sizeof(activeConfig.Color));
// 
// 
// 		Color_UpdateValues();
// 
// 		CopyMemory(&queuedConfig.hideBeowulfDante, &defaultConfig.hideBeowulfDante, sizeof(queuedConfig.hideBeowulfDante));
// 		CopyMemory(&activeConfig.hideBeowulfDante, &queuedConfig.hideBeowulfDante, sizeof(activeConfig.hideBeowulfDante));
// 
// 		CopyMemory(&queuedConfig.hideBeowulfVergil, &defaultConfig.hideBeowulfVergil, sizeof(queuedConfig.hideBeowulfVergil));
// 		CopyMemory(&activeConfig.hideBeowulfVergil, &queuedConfig.hideBeowulfVergil, sizeof(activeConfig.hideBeowulfVergil));
// 
// 
// 		CopyMemory(&queuedConfig.noDevilForm, &defaultConfig.noDevilForm, sizeof(queuedConfig.noDevilForm));
// 		CopyMemory(&activeConfig.noDevilForm, &queuedConfig.noDevilForm, sizeof(activeConfig.noDevilForm));
// 
// 		ToggleNoDevilForm(activeConfig.noDevilForm);
// 	}
	
	ImGui::Text("");

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
    GUI_Checkbox2("STYLE SWITCH FLUX", activeConfig.enableStyleSwitchFlux, queuedConfig.enableStyleSwitchFlux);
	ImGui::PopFont();

	for (int style = 0; style < 6; style++) {

		if (style > 0) {
			ImGui::SameLine();
		}
		GUI_Color2("", activeConfig.StyleSwitchColor.flux[style], queuedConfig.StyleSwitchColor.flux[style], Color.StyleSwitchColor.flux[style]);
		ImGui::SameLine();
		ImGui::Text(styleNamesFX[style]);
	}


	if (GUI_Button("Colorful")) {
		CopyMemory(&queuedConfig.StyleSwitchColor.flux, &defaultConfig.StyleSwitchColor.flux, sizeof(queuedConfig.StyleSwitchColor.flux));
		CopyMemory(&activeConfig.StyleSwitchColor.flux, &queuedConfig.StyleSwitchColor.flux, sizeof(activeConfig.StyleSwitchColor.flux));



		Color_UpdateValues();
	}

    ImGui::SameLine();
	if (GUI_Button("All Red")) {
		CopyMemory(&queuedConfig.StyleSwitchColor.flux, &activeConfig.StyleSwitchColor.fluxAllRed, sizeof(queuedConfig.StyleSwitchColor.flux));
		CopyMemory(&activeConfig.StyleSwitchColor.flux, &activeConfig.StyleSwitchColor.fluxAllRed, sizeof(activeConfig.StyleSwitchColor.flux));



		Color_UpdateValues();
	}

	
	ImGui::Text("");

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
    GUI_Checkbox2("STYLE SWITCH TEXT", activeConfig.enableStyleSwitchText, queuedConfig.enableStyleSwitchText);
	ImGui::PopFont();

	for (int style = 0; style < 9; style++) {
		if (style > 0) {
			ImGui::SameLine();
		}
		GUI_Color2("", activeConfig.StyleSwitchColor.text[style], queuedConfig.StyleSwitchColor.text[style], Color.StyleSwitchColor.text[style]);
		ImGui::SameLine();
		ImGui::Text(styleNamesFX[style]);
	}

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;


		if (ImGui::BeginTable("StyleSwitchTextPropertiesTable", 2)) {

			ImGui::TableSetupColumn("b1", 0, columnWidth * 0.5f);
			ImGui::TableNextRow(0, rowWidth * 0.1f);

            ImGui::TableNextColumn();
			
			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_InputDefault2<float>("Alpha", activeConfig.styleSwitchTextMaxAlpha, queuedConfig.styleSwitchTextMaxAlpha,
				defaultConfig.styleSwitchTextMaxAlpha, 0.1f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

            ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_InputDefault2<float>("Size", activeConfig.styleSwitchTextSize, queuedConfig.styleSwitchTextSize,
				defaultConfig.styleSwitchTextSize, 0.1f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

			ImGui::EndTable();
		}
	}
	

	if (GUI_Button("Midnight")) {
		CopyMemory(&queuedConfig.StyleSwitchColor.text, &activeConfig.StyleSwitchColor.textMidnight, sizeof(queuedConfig.StyleSwitchColor.text));
		CopyMemory(&activeConfig.StyleSwitchColor.text, &activeConfig.StyleSwitchColor.textMidnight, sizeof(activeConfig.StyleSwitchColor.text));

		Color_UpdateValues();
	}

    ImGui::SameLine();
	if (GUI_Button("All White")) {
		CopyMemory(&queuedConfig.StyleSwitchColor.text, &activeConfig.StyleSwitchColor.textAllWhite, sizeof(queuedConfig.StyleSwitchColor.text));
		CopyMemory(&activeConfig.StyleSwitchColor.text, &activeConfig.StyleSwitchColor.textAllWhite, sizeof(activeConfig.StyleSwitchColor.text));

		Color_UpdateValues();
	}

    ImGui::SameLine();
	if (GUI_Button("Colorful Clear")) {
		CopyMemory(&queuedConfig.StyleSwitchColor.text, &defaultConfig.StyleSwitchColor.text, sizeof(queuedConfig.StyleSwitchColor.text));
		CopyMemory(&activeConfig.StyleSwitchColor.text, &defaultConfig.StyleSwitchColor.text, sizeof(activeConfig.StyleSwitchColor.text));

		Color_UpdateValues();
	}

	ImGui::SameLine();
	if (GUI_Button("Colorful")) {
		CopyMemory(&queuedConfig.StyleSwitchColor.text, &activeConfig.StyleSwitchColor.textColorful, sizeof(queuedConfig.StyleSwitchColor.text));
		CopyMemory(&activeConfig.StyleSwitchColor.text, &activeConfig.StyleSwitchColor.textColorful, sizeof(activeConfig.StyleSwitchColor.text));

		Color_UpdateValues();
	}

    ImGui::Text("");

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
	ImGui::Text("DEVIL TRIGGER FLUX");
	ImGui::PopFont();

	for (int dt = 0; dt < 5; dt++) {
		if (dt > 0) {
			ImGui::SameLine();
		}
        GUI_Color2("", activeConfig.Color.Aura.dante[dt], queuedConfig.Color.Aura.dante[dt], Color.Aura.dante[dt]);
		ImGui::SameLine();
		ImGui::Text(meleeWeaponNamesDante[dt]);
	}
    ImGui::SameLine();
	GUI_Color2("", activeConfig.Color.Aura.sparda, queuedConfig.Color.Aura.sparda, Color.Aura.sparda);
    ImGui::SameLine();
    ImGui::Text("Sparda");

	for (int dt = 0; dt < 3; dt++) {
		if (dt > 0) {
			ImGui::SameLine();
		}
		GUI_Color2("", activeConfig.Color.Aura.vergil[dt], queuedConfig.Color.Aura.vergil[dt], Color.Aura.vergil[dt]);
		ImGui::SameLine();
		ImGui::Text(meleeWeaponNamesVergil[dt]);
	}
    ImGui::SameLine();
	GUI_Color2("", activeConfig.Color.Aura.neroAngelo, queuedConfig.Color.Aura.neroAngelo, Color.Aura.neroAngelo);
	ImGui::SameLine();
	ImGui::Text("Nelo Angelo");

	ImGui::Text("");

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
	ImGui::Text("AIR HIKE");
	ImGui::PopFont();

	for (int airhike = 0; airhike < 5; airhike++) {
		if (airhike > 0) {
			ImGui::SameLine();
		}
		GUI_Color2("", activeConfig.Color.airHike[airhike], queuedConfig.Color.airHike[airhike], Color.airHike[airhike]);
		ImGui::SameLine();
		ImGui::Text(meleeWeaponNamesDante[airhike]);
	}

    ImGui::Text("");

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
	ImGui::Text("OTHER MOVES");
	ImGui::PopFont();

	GUI_Color2("", activeConfig.Color.Trickster.skyStar, queuedConfig.Color.Trickster.skyStar, Color.Trickster.skyStar);
	ImGui::SameLine();
	ImGui::Text("Sky Star");

    ImGui::SameLine();
	GUI_Color2("", activeConfig.Color.Royalguard.ultimate, queuedConfig.Color.Royalguard.ultimate, Color.Royalguard.ultimate);
	ImGui::SameLine();
	ImGui::Text("Royalguard Ultimate");

    ImGui::SameLine();
	GUI_Color2("", activeConfig.Color.Doppelganger.clone, queuedConfig.Color.Doppelganger.clone, Color.Doppelganger.clone);
	ImGui::SameLine();
	ImGui::Text("Doppelganger");


	if (GUI_Button("DMC3 Default")) {
		CopyMemory(&queuedConfig.Color, &defaultConfig.Color, sizeof(queuedConfig.Color));
		CopyMemory(&activeConfig.Color, &queuedConfig.Color, sizeof(activeConfig.Color));


		Color_UpdateValues();
	}

    ImGui::SameLine();
	if (GUI_Button("Crimson")) {
		CopyMemory(&queuedConfig.Color, &activeConfig.ColorCrimson, sizeof(queuedConfig.Color));
		CopyMemory(&activeConfig.Color, &activeConfig.ColorCrimson, sizeof(activeConfig.Color));


		Color_UpdateValues();
	}
    

	ImGui::Text("");

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);


	ImGui::Text("MISC");

	ImGui::PopFont();

	UI::SeparatorEx(defaultFontSize * 23.35f);
    

    //GUI_Color2("FluxColorTrick", activeConfig.FluxColor.trick, queuedConfig.FluxColor.trick, defaultConfig.FluxColor.trick);


	ImGui::Text("");

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;


		if (ImGui::BeginTable("MiscVisualOptionsTable", 3)) {

			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableNextRow(0, rowWidth * 0.1f);

            ImGui::TableNextColumn();

            ImGui::PushItemWidth(itemWidth * smallerComboMult);
            GUI_Checkbox2("Hide Beowulf Dante", activeConfig.hideBeowulfDante, queuedConfig.hideBeowulfDante);
            ImGui::PopItemWidth();

            ImGui::TableNextColumn();

            ImGui::PushItemWidth(itemWidth * smallerComboMult);
            GUI_Checkbox2("Hide Beowulf Vergil", activeConfig.hideBeowulfVergil, queuedConfig.hideBeowulfVergil);
            ImGui::PopItemWidth();

            ImGui::TableNextColumn();
		
			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			if (GUI_Checkbox2("No Devil Form", activeConfig.noDevilForm, queuedConfig.noDevilForm)) {
				ToggleNoDevilForm(activeConfig.noDevilForm);
			}
			ImGui::PopItemWidth();

            ImGui::TableNextRow(0, rowWidth * 0.1f);
            ImGui::TableNextColumn();
            
			ImGui::PushItemWidth(itemWidth * smallerComboMult);
            UI::Combo2("Dot Shadow", dotShadowNames, activeConfig.dotShadow, queuedConfig.dotShadow);
			ImGui::PopItemWidth();

			ImGui::EndTable();
		}
	}


    ImGui::PopFont();
    ImGui::PopStyleColor();	
    
}

void GameplayOptions() {

    if (ImGui::CollapsingHeader("Gameplay Options (Crimson)")) {
        ImGui::Text("");

        ImGui::Text("");

        ImGui::Text("General");
        ImGui::PushItemWidth(150.0f);
        GUI_Checkbox2("Inertia", activeConfig.Gameplay.inertia, queuedConfig.Gameplay.inertia);
        ImGui::SameLine();
        TooltipHelper("(?)", "Requires Actor System.\n"
                             "\n"
                             "Changes how physics behave during almost all aerial moves. Also allows you to freely rotate performing "
                             "Swordmaster Air Moves.");
        GUI_Checkbox2("Sprint", activeConfig.Gameplay.sprint, queuedConfig.Gameplay.sprint);
        ImGui::SameLine();
        TooltipHelper("(?)", "Requires Actor System.\n"
                             "\n"
                             "Sprints out of combat, similar to DMC4 and 5's Speed Ability.");

        ImGui::Text("");

        ImGui::Text("Dante");

        ImGui::PushItemWidth(150.0f);
        GUI_Checkbox2("Improved Cancels", activeConfig.Gameplay.improvedCancelsDante, queuedConfig.Gameplay.improvedCancelsDante);
        ImGui::SameLine();
        TooltipHelper("(?)", "Requires Actor System.\n"
                             "\n"
                             "Enables a series of animation cancels for Dante, especially for moves between different styles.\nCheck out "
                             "the 1.0 Patch Notes for more info. Replaces DDMK's Remove Busy Flag.");

        GUI_Checkbox2("Aerial Rave Tweaks", activeConfig.Gameplay.aerialRaveTweaks, queuedConfig.Gameplay.aerialRaveTweaks);
        ImGui::SameLine();
        TooltipHelper("(?)", "Requires Actor System.\n"
                             "\n"
                             "Tweaks Aerial Rave Gravity, taking weights into account.");

        GUI_Checkbox2("Air Flicker Tweaks", activeConfig.Gameplay.airFlickerTweaks, queuedConfig.Gameplay.airFlickerTweaks);
        ImGui::SameLine();
        TooltipHelper("(?)", "Requires Actor System.\n"
                             "\n"
                             "Tweaks Air Flicker Gravity, taking weights into account. Initial windup has less gravity than vanilla.");

        GUI_Checkbox2("Sky Dance Tweaks", activeConfig.Gameplay.skyDanceTweaks, queuedConfig.Gameplay.skyDanceTweaks);
        ImGui::SameLine();
        TooltipHelper("(?)", "Requires Actor System.\n"
                             "\n"
                             "Sky Dance Part 3 is now a separate ability executed by Lock On + Forward + Style. Tweaks Sky Dance Gravity, "
                             "taking weights into account.");
        ImGui::Text("");

        ImGui::Text("");

        ImGui::Text("Vergil");

        ImGui::PushItemWidth(150.0f);
        GUI_Checkbox2("Darkslayer Tricks Cancels Everything", activeConfig.Gameplay.darkslayerTrickCancels,
            queuedConfig.Gameplay.darkslayerTrickCancels);
        ImGui::SameLine();
        TooltipHelper("(?)", "Requires Actor System.\n"
                             "\n"
                             "Enables Vergil to cancel any move with any Darkslayer Trick at any time.");
        ImGui::Text("");

        ImGui::Text("");

        ImGui::Text("Input Remaps");
        ImGui::SameLine();
        TooltipHelper(
            "(?)", "Remaps are global for all controllers, will only take into account Player 1's active Character for the switch.");

        DrawButtonCombo("Dante DT Button", activeConfig.Remaps.danteDTButton);
        DrawButtonCombo("Dante Shoot Button", activeConfig.Remaps.danteShootButton);
        DrawButtonCombo("Vergil DT Button", activeConfig.Remaps.vergilDTButton);
        DrawButtonCombo("Vergil Shoot Button", activeConfig.Remaps.vergilShootButton);
    }
}


#pragma endregion

#pragma region Vergil

const char* dergilNames[] = {
    "Default",
    "Force Off",
    "Force On",
};


void Vergil() {
    if (ImGui::CollapsingHeader("Vergil")) {
        ImGui::Text("");

        if (GUI_ResetButton()) {
            CopyMemory(&queuedConfig.Yamato, &defaultConfig.Yamato, sizeof(queuedConfig.Yamato));
            CopyMemory(&activeConfig.Yamato, &queuedConfig.Yamato, sizeof(activeConfig.Yamato));


            CopyMemory(&queuedConfig.YamatoForceEdge, &defaultConfig.YamatoForceEdge, sizeof(queuedConfig.YamatoForceEdge));
            CopyMemory(&activeConfig.YamatoForceEdge, &queuedConfig.YamatoForceEdge, sizeof(activeConfig.YamatoForceEdge));

            ToggleYamatoForceEdgeInfiniteRoundTrip(activeConfig.YamatoForceEdge.infiniteRoundTrip);

            CopyMemory(&queuedConfig.SummonedSwords, &defaultConfig.SummonedSwords, sizeof(queuedConfig.SummonedSwords));
            CopyMemory(&activeConfig.SummonedSwords, &queuedConfig.SummonedSwords, sizeof(activeConfig.SummonedSwords));

            ToggleChronoSwords(activeConfig.SummonedSwords.chronoSwords);

            CopyMemory(&queuedConfig.enableYamatoVergilNewJudgementCut, &defaultConfig.enableYamatoVergilNewJudgementCut,
                sizeof(queuedConfig.enableYamatoVergilNewJudgementCut));
            CopyMemory(&activeConfig.enableYamatoVergilNewJudgementCut, &queuedConfig.enableYamatoVergilNewJudgementCut,
                sizeof(activeConfig.enableYamatoVergilNewJudgementCut));

            CopyMemory(&queuedConfig.enableBeowulfVergilAirRisingSun, &defaultConfig.enableBeowulfVergilAirRisingSun,
                sizeof(queuedConfig.enableBeowulfVergilAirRisingSun));
            CopyMemory(&activeConfig.enableBeowulfVergilAirRisingSun, &queuedConfig.enableBeowulfVergilAirRisingSun,
                sizeof(activeConfig.enableBeowulfVergilAirRisingSun));

            CopyMemory(&queuedConfig.beowulfVergilAirRisingSunCount, &defaultConfig.beowulfVergilAirRisingSunCount,
                sizeof(queuedConfig.beowulfVergilAirRisingSunCount));
            CopyMemory(&activeConfig.beowulfVergilAirRisingSunCount, &queuedConfig.beowulfVergilAirRisingSunCount,
                sizeof(activeConfig.beowulfVergilAirRisingSunCount));

            CopyMemory(&queuedConfig.enableBeowulfVergilAirLunarPhase, &defaultConfig.enableBeowulfVergilAirLunarPhase,
                sizeof(queuedConfig.enableBeowulfVergilAirLunarPhase));
            CopyMemory(&activeConfig.enableBeowulfVergilAirLunarPhase, &queuedConfig.enableBeowulfVergilAirLunarPhase,
                sizeof(activeConfig.enableBeowulfVergilAirLunarPhase));

            CopyMemory(&queuedConfig.enableYamatoForceEdgeNewComboPart4, &defaultConfig.enableYamatoForceEdgeNewComboPart4,
                sizeof(queuedConfig.enableYamatoForceEdgeNewComboPart4));
            CopyMemory(&activeConfig.enableYamatoForceEdgeNewComboPart4, &queuedConfig.enableYamatoForceEdgeNewComboPart4,
                sizeof(activeConfig.enableYamatoForceEdgeNewComboPart4));

            CopyMemory(&queuedConfig.enableYamatoForceEdgeAirStinger, &defaultConfig.enableYamatoForceEdgeAirStinger,
                sizeof(queuedConfig.enableYamatoForceEdgeAirStinger));
            CopyMemory(&activeConfig.enableYamatoForceEdgeAirStinger, &queuedConfig.enableYamatoForceEdgeAirStinger,
                sizeof(activeConfig.enableYamatoForceEdgeAirStinger));

            CopyMemory(&queuedConfig.enableYamatoForceEdgeNewRoundTrip, &defaultConfig.enableYamatoForceEdgeNewRoundTrip,
                sizeof(queuedConfig.enableYamatoForceEdgeNewRoundTrip));
            CopyMemory(&activeConfig.enableYamatoForceEdgeNewRoundTrip, &queuedConfig.enableYamatoForceEdgeNewRoundTrip,
                sizeof(activeConfig.enableYamatoForceEdgeNewRoundTrip));


            CopyMemory(&queuedConfig.dergil, &defaultConfig.dergil, sizeof(queuedConfig.dergil));
            CopyMemory(&activeConfig.dergil, &queuedConfig.dergil, sizeof(activeConfig.dergil));

            ToggleDergil(activeConfig.dergil);
        }

        GUI_SectionEnd();
        ImGui::Text("");


        ImGui::Text("Yamato");
        ImGui::Text("");

        GUI_Checkbox2(
            "Enable New Judgement Cut", activeConfig.enableYamatoVergilNewJudgementCut, queuedConfig.enableYamatoVergilNewJudgementCut);
        ImGui::SameLine();
        TooltipHelper("(?)", "Requires enabled Actor module.\n"
                             "\n"
                             "Press Lock-On + Left + Melee Attack.");
        ImGui::Text("");


        TooltipHelper("(?)", "Requires enabled Actor module.\n"
                             "\n"
                             "Left : Human\n"
                             "Right: Devil");
        ImGui::Text("");

        ActionData("Judgement Cut Count", activeConfig.Yamato.judgementCutCount, queuedConfig.Yamato.judgementCutCount,
            defaultConfig.Yamato.judgementCutCount);

        GUI_SectionEnd();
        ImGui::Text("");


        GUI_SectionStart("Beowulf");

        GUI_Checkbox2("Enable Air Rising Sun", activeConfig.enableBeowulfVergilAirRisingSun, queuedConfig.enableBeowulfVergilAirRisingSun);
        ImGui::SameLine();
        TooltipHelper("(?)", "Requires enabled Actor module.");
        ImGui::Text("");


        TooltipHelper("(?)", "Requires enabled Actor module.\n"
                             "\n"
                             "Left : Human\n"
                             "Right: Devil");
        ImGui::Text("");

        {
            bool condition = !activeConfig.enableBeowulfVergilAirRisingSun;

            GUI_PushDisable(condition);

            ActionData("Air Rising Sun Count", activeConfig.beowulfVergilAirRisingSunCount, queuedConfig.beowulfVergilAirRisingSunCount,
                defaultConfig.beowulfVergilAirRisingSunCount);

            GUI_PopDisable(condition);
        }
        ImGui::Text("");

        GUI_Checkbox2(
            "Enable Air Lunar Phase", activeConfig.enableBeowulfVergilAirLunarPhase, queuedConfig.enableBeowulfVergilAirLunarPhase);
        ImGui::SameLine();
        TooltipHelper("(?)", "Requires enabled Actor module.");

        GUI_SectionEnd();
        ImGui::Text("");


        ImGui::Text("Yamato & Force Edge");
        ImGui::Text("");

        if (GUI_Checkbox2(
                "Infinite Round Trip", activeConfig.YamatoForceEdge.infiniteRoundTrip, queuedConfig.YamatoForceEdge.infiniteRoundTrip)) {
            ToggleYamatoForceEdgeInfiniteRoundTrip(activeConfig.YamatoForceEdge.infiniteRoundTrip);
        }

        GUI_Checkbox2(
            "Enable New Combo Part 4", activeConfig.enableYamatoForceEdgeNewComboPart4, queuedConfig.enableYamatoForceEdgeNewComboPart4);
        ImGui::SameLine();
        TooltipHelper("(?)", "Requires enabled Actor module.\n"
                             "\n"
                             "Press Lock-On + Right + Melee Attack.");

        GUI_Checkbox2("Enable Air Stinger", activeConfig.enableYamatoForceEdgeAirStinger, queuedConfig.enableYamatoForceEdgeAirStinger);
        ImGui::SameLine();
        TooltipHelper("(?)", "Requires enabled Actor module.");

        GUI_Checkbox2(
            "Enable New Round Trip", activeConfig.enableYamatoForceEdgeNewRoundTrip, queuedConfig.enableYamatoForceEdgeNewRoundTrip);
        ImGui::SameLine();
        TooltipHelper("(?)", "Requires enabled Actor module.\n"
                             "\n"
                             "Press Lock-On + Left + Melee Attack.");
        ImGui::Text("");


        TooltipHelper("(?)", "Requires enabled Actor module.\n"
                             "\n"
                             "Left : Human\n"
                             "Right: Devil");
        ImGui::Text("");

        ActionData("Stinger Duration", activeConfig.YamatoForceEdge.stingerDuration, queuedConfig.YamatoForceEdge.stingerDuration,
            defaultConfig.YamatoForceEdge.stingerDuration, 1.0f, "%g");
        ActionData("Stinger Range", activeConfig.YamatoForceEdge.stingerRange, queuedConfig.YamatoForceEdge.stingerRange,
            defaultConfig.YamatoForceEdge.stingerRange, 10.0f, "%g");

        {
            bool condition = !activeConfig.enableYamatoForceEdgeAirStinger;

            GUI_PushDisable(condition);

            ActionData("Air Stinger Count", activeConfig.YamatoForceEdge.airStingerCount, queuedConfig.YamatoForceEdge.airStingerCount,
                defaultConfig.YamatoForceEdge.airStingerCount);

            ActionData("Air Stinger Duration", activeConfig.YamatoForceEdge.airStingerDuration,
                queuedConfig.YamatoForceEdge.airStingerDuration, defaultConfig.YamatoForceEdge.airStingerDuration, 1.0f, "%g");
            ActionData("Air Stinger Range", activeConfig.YamatoForceEdge.airStingerRange, queuedConfig.YamatoForceEdge.airStingerRange,
                defaultConfig.YamatoForceEdge.airStingerRange, 10.0f, "%g");

            GUI_PopDisable(condition);
        }

        GUI_SectionEnd();
        ImGui::Text("");


        GUI_SectionStart("Summoned Swords");

        if (GUI_Checkbox2("Chrono Swords", activeConfig.SummonedSwords.chronoSwords, queuedConfig.SummonedSwords.chronoSwords)) {
            ToggleChronoSwords(activeConfig.SummonedSwords.chronoSwords);
        }
        ImGui::SameLine();
        TooltipHelper("(?)", "Requires enabled Actor module.\n"
                             "\n"
                             "Summoned Swords will continue to levitate as long as Quicksilver is active.");

        GUI_SectionEnd();
        ImGui::Text("");


        ImGui::PushItemWidth(150.0f);

        if (UI::Combo2("Dergil", dergilNames, activeConfig.dergil, queuedConfig.dergil)) {
            ToggleDergil(activeConfig.dergil);
        }

        ImGui::PopItemWidth();


        ImGui::Text("");
    }
}

#pragma endregion


#pragma region Key Bindings

// @Move

void ReloadRoom() {
    if (!InGame()) {
        return;
    }

    auto pool_12898 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
    if (!pool_12898 || !pool_12898[8]) {
        return;
    }
    auto& eventData = *reinterpret_cast<EventData*>(pool_12898[8]);

    auto pool_12959 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
    if (!pool_12959 || !pool_12959[12]) {
        return;
    }
    auto& nextEventData = *reinterpret_cast<NextEventData*>(pool_12959[12]);


    nextEventData.room     = static_cast<uint16>(eventData.room);
    nextEventData.position = static_cast<uint16>(eventData.position);


    eventData.event = EVENT::TELEPORT;
}


void MoveToMainActor() {
    if (!activeConfig.Actor.enable || !InGame()) {
        return;
    }

    LogFunction();


    byte8* mainActorBaseAddr = 0;

    {
        auto& playerData = GetPlayerData(0);

        auto& characterData = GetCharacterData(0, playerData.characterIndex, ENTITY::MAIN);
        auto& newActorData  = GetNewActorData(0, playerData.characterIndex, ENTITY::MAIN);

        auto& activeCharacterData = GetCharacterData(0, playerData.activeCharacterIndex, ENTITY::MAIN);
        auto& activeNewActorData  = GetNewActorData(0, playerData.activeCharacterIndex, ENTITY::MAIN);

        auto& leadCharacterData = GetCharacterData(0, 0, ENTITY::MAIN);
        auto& leadNewActorData  = GetNewActorData(0, 0, ENTITY::MAIN);

        auto& mainCharacterData = GetCharacterData(0, playerData.characterIndex, ENTITY::MAIN);
        auto& mainNewActorData  = GetNewActorData(0, playerData.characterIndex, ENTITY::MAIN);


        mainActorBaseAddr = activeNewActorData.baseAddr;
    }

    if (!mainActorBaseAddr) {
        return;
    }
    auto& mainActorData = *reinterpret_cast<PlayerActorData*>(mainActorBaseAddr);


    old_for_each(uint8, playerIndex, 1, activeConfig.Actor.playerCount) {
        auto& playerData = GetActivePlayerData(playerIndex);

        old_for_all(uint8, characterIndex, playerData.characterCount) {
            old_for_all(uint8, entityIndex, ENTITY_COUNT) {
                auto& playerData = GetPlayerData(playerIndex);

                auto& characterData = GetCharacterData(playerIndex, characterIndex, entityIndex);
                auto& newActorData  = GetNewActorData(playerIndex, characterIndex, entityIndex);

                auto& activeCharacterData = GetCharacterData(playerIndex, playerData.activeCharacterIndex, ENTITY::MAIN);
                auto& activeNewActorData  = GetNewActorData(playerIndex, playerData.activeCharacterIndex, ENTITY::MAIN);

                auto& leadCharacterData = GetCharacterData(playerIndex, 0, ENTITY::MAIN);
                auto& leadNewActorData  = GetNewActorData(playerIndex, 0, ENTITY::MAIN);

                auto& mainCharacterData = GetCharacterData(playerIndex, characterIndex, ENTITY::MAIN);
                auto& mainNewActorData  = GetNewActorData(playerIndex, characterIndex, ENTITY::MAIN);


                if (!newActorData.baseAddr) {
                    continue;
                }
                auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);

                actorData.position = mainActorData.position;
            }
        }
    }
}


void ToggleInfiniteHealth() {

    if (activeConfig.infiniteHitPoints) {
        activeConfig.infiniteHitPoints = false;
    } else {
        activeConfig.infiniteHitPoints = true;
    }

    ToggleInfiniteHitPoints(activeConfig.infiniteHitPoints);
}

void GamepadToggleShowMain() {

    //auto combination = (ImGui::IsNavInputDown(ImGuiGamepad::DpadDown) && ImGui::IsNavInputDown(ImGuiGamepad::R1) && ImGui::IsNavInputDown(ImGuiGamepad::B) && ImGui::IsNavInputDown(ImGuiGamepad::Y));
    auto combination = (IsJoystickButtonDown(joystick, 7) && IsJoystickButtonDown(joystick, 8));

    if (combination && !g_showMain && gamepadCombinationMainRelease) {
        ToggleShowMain();
        ImGui::SetWindowFocus(DMC3C_TITLE);
        gamepadCombinationMainRelease = false;
    }

    if (!combination && !gamepadCombinationMainRelease) {
        gamepadCombinationMainRelease = true;
    }

    if (combination && g_showMain && gamepadCombinationMainRelease) {
        ToggleShowMain();
        gamepadCombinationMainRelease = false;
    }


}

std::vector<KeyBinding> keyBindings = {
    {
        "Toggle Show Main",
        activeConfig.keyData[0],
        queuedConfig.keyData[0],
        defaultConfig.keyData[0],
        ToggleShowMain,
    },
    {"Reload Room", activeConfig.keyData[1], queuedConfig.keyData[1], defaultConfig.keyData[1], ReloadRoom},
    {"Move To Main Actor", activeConfig.keyData[2], queuedConfig.keyData[2], defaultConfig.keyData[2], MoveToMainActor},
    {"Toggle Infinite Health Points", activeConfig.keyData[3], queuedConfig.keyData[3], defaultConfig.keyData[3], ToggleInfiniteHealth},
};

void KeyBindings() {
    if (ImGui::CollapsingHeader("Key Bindings")) {
        ImGui::Text("");


        bool condition = false;

        for_all(index, keyBindings.size()) {
            auto& keyBinding = keyBindings[index];

            if (keyBinding.showPopup) {
                condition = true;

                break;
            }
        }

        GUI_PushDisable(condition);

        for_all(index, keyBindings.size()) {
            auto& keyBinding = keyBindings[index];

            keyBinding.Main();
        }

        GUI_PopDisable(condition);


        ImGui::Text("");
    }
}

#pragma endregion

#pragma region Main

void Main(IDXGISwapChain* pSwapChain) {
    if (!g_showMain) {
        return;
    }
    // ImGui::InputScalar("Heheheh", ImGuiDataType_U64, &g_SampleMod_ReturnAddr1);


    static bool doOnce = false;
	static std::thread versionCheckerThread;

    if (!doOnce) {
        doOnce = true;

        // Originally 800x725, or screenWidth / 2.4 x screenHeight / 1.4 for 1080p
        float width  = g_renderSize.x / 1.5;
        float height = g_renderSize.y / 1.3;

        ImGui::SetNextWindowSize(ImVec2(width, height));

        //((g_renderSize.x - width) / 3)

//         if constexpr (debug) {
//             ImGui::SetNextWindowPos(ImVec2(((g_renderSize.x) / 2), 100)); // Don't know why you'd spawn the screen that way
//         } else {
// 
        // CENTER MAIN SCREEN
        ImGui::SetNextWindowPos(ImVec2(g_renderSize.x * 0.5f, g_renderSize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));


        // ImGuiIO & io = ImGui::GetIO();
        // io.FontDefault = io.Fonts->Fonts[FONT::MAIN];
        // ImGui::PushFont(io.Fonts->Fonts[FONT::OVERLAY_8 + activeConfig.Tools.Overlay.fontSizeIndex]);

        // ImGui::SetCurrentFont(io.Fonts->Fonts[FONT::OVERLAY_8]);

		WebAPICalls::GetInstance().SetVersionCallback([&](WebAPIResult res, Version_t latestVersion) {
			    if (res == WebAPIResult::Success) {
					UI::g_UIContext.LatestVersion.Major = latestVersion.Major;
					UI::g_UIContext.LatestVersion.Minor = latestVersion.Minor;

			    	UI::g_UIContext.LatestUpdateDate.Year = latestVersion.PublishTime.Local.tm_year + 1900;
			    	UI::g_UIContext.LatestUpdateDate.Month = latestVersion.PublishTime.Local.tm_mon + 1;
			    	UI::g_UIContext.LatestUpdateDate.Day = latestVersion.PublishTime.Local.tm_mday;

                    UI::g_UIContext.LatestVersionURL = latestVersion.DirectURL;

					if (UI::g_UIContext.LatestVersion.Major > UI::g_UIContext.CurrentVersion.Major) 
                    {
						UI::g_UIContext.NewVersionAvailable = true;
					}
					else if (UI::g_UIContext.LatestVersion.Major == UI::g_UIContext.CurrentVersion.Major &&
						        UI::g_UIContext.LatestVersion.Minor > UI::g_UIContext.CurrentVersion.Minor) 
                    {
						UI::g_UIContext.NewVersionAvailable = true;
					}
                    else if (UI::g_UIContext.LatestVersion.Major == UI::g_UIContext.CurrentVersion.Major &&
                                UI::g_UIContext.LatestVersion.Minor == UI::g_UIContext.CurrentVersion.Minor &&
                                UI::g_UIContext.LatestVersion.PatchLetter > UI::g_UIContext.CurrentVersion.PatchLetter)
                    {
                        UI::g_UIContext.NewVersionAvailable = true;
                    }
					else 
                    {
						UI::g_UIContext.NewVersionAvailable = false;
					}
                }

			    UI::versionCheckResult = res;
			}
		);

		WebAPICalls::GetInstance().SetPatronsCallback([&](WebAPIResult res, std::vector<Patron_t> patrons) {
			    if (res == WebAPIResult::Success) {
			    	UI::g_UIContext.PatronsRich.clear();
                    UI::g_UIContext.PatronsRichAF.clear();

                    for (const auto& patron : patrons) {
                        switch (patron.Tier) {
                        case PatreonTiers_t::Rich:
                        {
                            UI::g_UIContext.PatronsRich.push_back(patron.UserName);

                            // Set the name of the tier to be displayed
                            UI::g_UIContext.TierNames[(size_t)PatreonTiers_t::Rich] = patron.TierName;
                        }
                            break;

                        case PatreonTiers_t::RichAF:
                        {
                            UI::g_UIContext.PatronsRichAF.push_back(patron.UserName);

                            // Set the name of the tier to be displayed
                            UI::g_UIContext.TierNames[(size_t)PatreonTiers_t::RichAF] = patron.TierName;
                        }
                            break;

                        default:
                            break;
                        }
                    }
			    }

			    UI::patronsQueueResult = res;
			}
		);

		versionCheckerThread = std::thread{
			[&] {
				WebAPICalls::GetInstance().QueueLatestRelease(60000);
                WebAPICalls::GetInstance().QueuePatrons(60000);
			}
		};
    }

    UI::DrawCrimson(pSwapChain, DMC3C_TITLE, &g_showMain);
}

void DrawMainContent(ID3D11Device* pDevice, UI::UIContext& context) {
	ImGuiWindow* cntWindow = ImGui::GetCurrentWindow();
	const ImRect cntRegion = cntWindow->Rect();
	const ImGuiStyle& style = ImGui::GetStyle();

	static bool uiElementsInitialized = false;
	if (!uiElementsInitialized) {
		size_t mainLogoWidth = size_t(context.DefaultFontSize * 37.0f);

		g_Image_CrimsonMainLogo.ResizeByRatioW(mainLogoWidth);
		g_Image_VanillaLogo.ResizeByRatioW(mainLogoWidth);
		g_Image_StyleSwitcherLogo.ResizeByRatioW(mainLogoWidth);
		g_Image_SocialIcons.ResizeByRatioW(size_t(context.DefaultFontSize * 10.0f));

		uiElementsInitialized = true;
	}

	switch (context.SelectedTab) {
	case UI::UIContext::MainTabs::GameMode:
	{
		constexpr float align = 0.5f; // Center = 0.5f

		constexpr const char* MODE_SELECTION_TEXT = "Choose your desired Devil May Cry 3 version!\n"
			"This will affect the entire Gameplay Options globally and tag you at the Mission End Screen.\n"
			"If Gameplay Options diverge too much from any preset, 'Custom' Game Mode will be selected instead automatically.";

		float width = ImGui::CalcTextSize(MODE_SELECTION_TEXT).x;

        ImGui::Text("");
        ImGui::Text("");
        

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (cntRegion.GetWidth() - width) * align);

		ImGui::TextWrapped(MODE_SELECTION_TEXT);
        ImGui::Text("");

		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[context.DefaultFontSize]);

		float comboBoxWidth = width * 0.5f;

		std::array<const char*, 3> modes{ "VANLLA MODE", "STYLE SWITCHER MODE", "CRIMSON MODE" };

		ImGui::SetNextItemWidth(comboBoxWidth);

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (cntRegion.GetWidth() - comboBoxWidth) * align);


		ImU32 frameBG = 0;
		ImU32 frameBGHovered = 0;
		ImU32 textColor = 0;

		switch (context.SelectedGameMode) {
		case UI::UIContext::GameModes::Vanilla:
		{
			frameBG = UI::SwapColorEndianness(0xFFFFFFFF);
			frameBGHovered = UI::SwapColorEndianness(0xFFFFFFAA);
			textColor = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_WindowBgText]);
		}
		break;

		case UI::UIContext::GameModes::StyleSwitcher:
		{
			frameBG = UI::SwapColorEndianness(0xE8BA18FF);
			frameBGHovered = UI::SwapColorEndianness(0xE8BA18AA);
			textColor = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_WindowBgText]);
		}
		break;

		case UI::UIContext::GameModes::Crimson:
		{
			frameBG = UI::SwapColorEndianness(0xDA1B53FF);
			frameBGHovered = UI::SwapColorEndianness(0xDA1B53AA);
			textColor = UI::SwapColorEndianness(0xFFFFFFFF);
		}
		break;

		default:
		{
			frameBG = UI::SwapColorEndianness(0xFFFFFFFF);
			frameBGHovered = UI::SwapColorEndianness(0xFFFFFFAA);
			textColor = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_WindowBgText]);
		}
		break;
		}


		ImGui::PushStyleColor(ImGuiCol_FrameBg, frameBG);
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, frameBGHovered);
		ImGui::PushStyleColor(ImGuiCol_Text, textColor);;

		if (UI::BeginCombo("##Game Mode", modes[size_t(context.SelectedGameMode)], { 0.5f, 0.5f }, 0.9f)) {
			ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, { 0.5f, 0.5f });

			for (size_t i = 0; i < modes.size(); i++)
			{
				ImU32 headerColor = 0;
				ImU32 headerHoveredColor = 0;
				ImU32 headerActiveColor = 0;
				ImU32 headerTextColor = 0;

				switch (UI::UIContext::GameModes(i)) {
				case UI::UIContext::GameModes::Vanilla:
				{
					headerColor = UI::SwapColorEndianness(0x979797FF);
					headerHoveredColor = UI::SwapColorEndianness(0x979797CC);
					headerActiveColor = UI::SwapColorEndianness(0x979797FF);
					headerTextColor = UI::SwapColorEndianness(0xFFFFFFFF);
				}
				break;

				case UI::UIContext::GameModes::StyleSwitcher:
				{
					headerColor = UI::SwapColorEndianness(0x856E1CFF);
					headerHoveredColor = UI::SwapColorEndianness(0x856E1CCC);
					headerActiveColor = UI::SwapColorEndianness(0x856E1CFF);
					headerTextColor = UI::SwapColorEndianness(0xFFFFFFFF);
				}
				break;

				case UI::UIContext::GameModes::Crimson:
				{
					headerColor = UI::SwapColorEndianness(0x821031FF);
					headerHoveredColor = UI::SwapColorEndianness(0x821031CC);
					headerActiveColor = UI::SwapColorEndianness(0x821031FF);
					headerTextColor = UI::SwapColorEndianness(0xFFFFFFFF);
				}
				break;

				default:
				{
					headerColor = UI::SwapColorEndianness(0xB7B7B7FF);
					headerHoveredColor = UI::SwapColorEndianness(0xB7B7B7CC);
					headerActiveColor = UI::SwapColorEndianness(0xB7B7B7FF);
					headerTextColor = UI::SwapColorEndianness(0xFFFFFFFF);
				}
				break;
				}

				ImGui::PushStyleColor(ImGuiCol_Header, headerColor);
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, headerHoveredColor);
				ImGui::PushStyleColor(ImGuiCol_HeaderActive, headerActiveColor);
				ImGui::PushStyleColor(ImGuiCol_Text, headerTextColor);

				bool isSelected = size_t(context.SelectedGameMode) == i;

				if (ImGui::Selectable(modes[i], isSelected))
					context.SelectedGameMode = (decltype(context.SelectedGameMode))i;

				if (isSelected)
					ImGui::SetItemDefaultFocus();

				ImGui::PopStyleColor(4);
			}

			ImGui::PopStyleVar();

			ImGui::EndCombo();
		}

		ImGui::PopStyleColor(3);

		const Texture2DD3D11* pMainLogo = nullptr;
		{
			static const Texture2DD3D11 vanillaLogo(g_Image_VanillaLogo.GetRGBAData(), g_Image_VanillaLogo.GetWidth(), g_Image_VanillaLogo.GetHeight(), pDevice);
			static const Texture2DD3D11 styleSwitcherLogo(g_Image_StyleSwitcherLogo.GetRGBAData(), g_Image_StyleSwitcherLogo.GetWidth(), g_Image_StyleSwitcherLogo.GetHeight(), pDevice);
			static const Texture2DD3D11 crimsonLogo(g_Image_CrimsonMainLogo.GetRGBAData(), g_Image_CrimsonMainLogo.GetWidth(), g_Image_CrimsonMainLogo.GetHeight(), pDevice);

			float heightOffset = 0.0f;

			switch (context.SelectedGameMode) {
			case UI::UIContext::GameModes::Vanilla:
				pMainLogo = &vanillaLogo;
				heightOffset = context.DefaultFontSize * 2.0f;
				break;

			case UI::UIContext::GameModes::StyleSwitcher:
				pMainLogo = &styleSwitcherLogo;
				heightOffset = -(context.DefaultFontSize * 3.2f);
				break;

			case UI::UIContext::GameModes::Crimson:
				pMainLogo = &crimsonLogo;
				heightOffset = context.DefaultFontSize * 2.0f;
				break;

			default:
				pMainLogo = &vanillaLogo;
				break;
			}

			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2{ (cntRegion.GetWidth() - pMainLogo->GetWidth()) * align, heightOffset });

			ImGui::Image(*pMainLogo, pMainLogo->GetSize());//, { 0.001f, 0.001f }, { 0.999f, 0.999f });
		}

		// Bottom text
		{
			constexpr auto MODE_INFO_TEXT_VANILLA = "Experience the game how it was originally made.";
			constexpr auto MODE_INFO_TEXT_SW_LINE1 = "This is DMC3 as you are probably familiar with. Closer to how the Switch Version and DDMK plays.";
			constexpr auto MODE_INFO_TEXT_SW_LINE2 = "Vanilla + Style / Full Weapon Switching.";
			constexpr auto MODE_INFP_TEXT_CRIMSON = "Enjoy the ultimate DMC3 experience! All new Gameplay Improvements and Expansions enabled.";

			ImGui::PushFont(UI::g_ImGuiFont_Roboto[context.DefaultFontSize]);

			const float vanillaWidth = ImGui::CalcTextSize(MODE_INFO_TEXT_VANILLA).x;
			const float swWidthLine1 = ImGui::CalcTextSize(MODE_INFO_TEXT_SW_LINE1).x;
			const float swWidthLine2 = ImGui::CalcTextSize(MODE_INFO_TEXT_SW_LINE2).x;
			const float crimsonWidth = ImGui::CalcTextSize(MODE_INFP_TEXT_CRIMSON).x;

			switch (context.SelectedGameMode) {
			case UI::UIContext::GameModes::Vanilla:
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (cntRegion.GetWidth() - vanillaWidth) * align);
				ImGui::Text(MODE_INFO_TEXT_VANILLA);
				break;

			case UI::UIContext::GameModes::StyleSwitcher:
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - context.DefaultFontSize * 3.0f);

				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (cntRegion.GetWidth() - swWidthLine1) * align);
				ImGui::Text(MODE_INFO_TEXT_SW_LINE1);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (cntRegion.GetWidth() - swWidthLine2) * align);
				ImGui::Text(MODE_INFO_TEXT_SW_LINE2);
				break;

			case UI::UIContext::GameModes::Crimson:
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 1.0f);
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (cntRegion.GetWidth() - crimsonWidth) * align);
				ImGui::Text(MODE_INFP_TEXT_CRIMSON);
				break;

			default:
				break;
			}

			ImGui::PopFont();
		}

		ImGui::PopFont();
	}
	break;

	case UI::UIContext::MainTabs::Character:
	{
		// Widget area
		{
			const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.7f, 0.98f };
			const ImVec2 areaMin{ cntWindow->Pos.x + 0.1f * context.DefaultFontSize,
									 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
			ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
			ImGui::BeginChildEx("Widget Area", cntWindow->GetID("Widget Area"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
			ImGui::PopStyleVar();
			{
                ActorSection(context.DefaultFontSize);
			}
			ImGui::EndChild();
		}

		// Tooltip area
		{
			const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.3f, 0.98f };
			const ImVec2 areaMin{ cntWindow->Pos.x + cntWindow->Size.x - areaSize.x - 0.1f * context.DefaultFontSize,
									 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

			cntWindow->DrawList->AddRect(areaMin, areaMin + areaSize, UI::SwapColorEndianness(0x585152FF));

			ImVec2 padding{ context.DefaultFontSize * 0.8f, context.DefaultFontSize * 0.8f };

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
			ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
			ImGui::BeginChildEx("Tooltip Area", cntWindow->GetID("Tooltip Area"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
			ImGui::PopStyleVar();
			{
                constexpr auto ACTOR_SYSTEM_INTRO = u8"Custom Character Creation and Spawning System created by DDMK's Serpentiem.";
                constexpr auto FEATURE_STYLE_SWITCH = u8"   • Style Switching;";
                constexpr auto FEATURE_WEAPONS = u8"   • Custom Weapon Loadouts;";
                constexpr auto FEATURE_CHAR_SWITCH = u8"   • Character Switching;";
                constexpr auto FEATURE_MULTIPLAYER = u8"   • Local Multiplayer;";
                constexpr auto FEATURE_DOPPEL_TWEAKS = u8"   • Doppelganger Tweaks;";

				ImGui::PushFont(UI::g_ImGuiFont_RussoOne[size_t(context.DefaultFontSize * 1.0f)]);
				ImGui::Text("ACTOR SYSTEM");
				ImGui::PopFont();


				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.8f);
				ImGui::PushFont(UI::g_ImGuiFont_Roboto[size_t(context.DefaultFontSize * 0.9f)]);

                ImGui::TextWrapped((const char*) ACTOR_SYSTEM_INTRO);
                ImGui::TextWrapped("Required for almost all Gameplay Options to be enabled.");
                ImGui::TextWrapped("");
                ImGui::TextWrapped("Enables:");
                ImGui::TextWrapped((const char*) FEATURE_STYLE_SWITCH);
                ImGui::TextWrapped((const char*) FEATURE_WEAPONS);
                ImGui::TextWrapped((const char*) FEATURE_CHAR_SWITCH);
                ImGui::TextWrapped((const char*) FEATURE_MULTIPLAYER);
                ImGui::TextWrapped((const char*) FEATURE_DOPPEL_TWEAKS);
                ImGui::TextWrapped("");
                ImGui::TextWrapped("");
                ImGui::TextWrapped("*Required for Crimson Mode to stay enabled.");
                ImGui::TextWrapped("**Changes the Divinity Statue (Shop) UI.");
				ImGui::TextWrapped("(Automatically disables itself during Battle of Brothers and End Credits for stability).");


				ImGui::PopFont();
			}
			ImGui::EndChild();
		}
	}
	break;

	case UI::UIContext::MainTabs::Quickplay:
	{
		// Widget area
		{
			const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.7f, 0.98f };
			const ImVec2 areaMin{ cntWindow->Pos.x + 0.1f * context.DefaultFontSize,
									 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
			ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
			ImGui::BeginChildEx("Widget Area", cntWindow->GetID("Widget Area"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
			ImGui::PopStyleVar();
            {
                ArcadeSection(context.DefaultFontSize);
                BossRushSection(context.DefaultFontSize);
            }
			ImGui::EndChild();
		}

		// Tooltip area
		{
			const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.3f, 0.98f };
			const ImVec2 areaMin{ cntWindow->Pos.x + cntWindow->Size.x - areaSize.x - 0.1f * context.DefaultFontSize,
									 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

			cntWindow->DrawList->AddRect(areaMin, areaMin + areaSize, UI::SwapColorEndianness(0x585152FF));

			ImVec2 padding{ context.DefaultFontSize * 0.8f, context.DefaultFontSize * 0.8f };

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
			ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
			ImGui::BeginChildEx("Tooltip Area", cntWindow->GetID("Tooltip Area"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
			ImGui::PopStyleVar();
			{
				ImGui::PushFont(UI::g_ImGuiFont_RussoOne[size_t(context.DefaultFontSize * 1.0f)]);
				ImGui::Text("ARCADE");
                ImGui::PopFont();
				

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.8f);
                ImGui::PushFont(UI::g_ImGuiFont_Roboto[size_t(context.DefaultFontSize * 0.9f)]);

				ImGui::TextWrapped("Jumps you directly to a specific point in the game, triggered in the Main Menu. \n\nThis will tag you at the Mission End Screen.");
                ImGui::PopFont();
			}
			ImGui::EndChild();
		}
	}
	break;

	case UI::UIContext::MainTabs::MusicSwitcher:
	{

	}
	break;

	case UI::UIContext::MainTabs::Options:
	{
        if (context.SelectedOptionsSubTab == UI::UIContext::OptionsSubTabs::Gameplay) {
			// Widget area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.7f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("Widget Area", cntWindow->GetID("Widget Area"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					{
						ImGui::Text("");

						std::stringstream sstm;
						sstm << "WINDOWSIZE X: " << g_renderSize.x;
						std::string windowSizeX = sstm.str();

						std::stringstream sstm2;
						sstm2 << "WINDOWSIZE Y: " << g_renderSize.y;
						std::string windowSizeY = sstm2.str();
						const char* var1 = windowSizeX.c_str();
						const char* var2 = windowSizeY.c_str();


						ImGui::Text(var1);
						ImGui::Text(var2);


						GamepadClose(visibleMain, lastVisibleMain, CloseMain);


						ImGui::PushItemWidth(150);


						ImGui::PopItemWidth();

						Damage();
						Dante();

						if constexpr (debug) {
							Debug();
						}

						Enemy();
						Jukebox();
						KeyBindings();
						Lady();
						Mobility();
						Other();
						Repair();
						SpeedSection();
						Teleporter();
						WeaponWheel();
						GameplayOptions();
						TrainingSection();
						Vergil();

					}
				}
				ImGui::EndChild();
			}

			// Tooltip area
            {
                const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.3f, 0.98f };
                const ImVec2 areaMin{ cntWindow->Pos.x + cntWindow->Size.x - areaSize.x - 0.1f * context.DefaultFontSize,
                                         cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

                cntWindow->DrawList->AddRect(areaMin, areaMin + areaSize, UI::SwapColorEndianness(0x585152FF));

                ImVec2 padding{ context.DefaultFontSize * 0.8f, context.DefaultFontSize * 0.8f };

                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
                ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
                ImGui::BeginChildEx("Tooltip Area", cntWindow->GetID("Tooltip Area"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
                ImGui::PopStyleVar();
                {
                    ImGui::PushFont(UI::g_ImGuiFont_RussoOne[size_t(context.DefaultFontSize * 1.0f)]);
                    ImGui::Text("ARCADE");
                    ImGui::PopFont();


                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.8f);
                    ImGui::PushFont(UI::g_ImGuiFont_Roboto[size_t(context.DefaultFontSize * 0.9f)]);

                    ImGui::TextWrapped("Jumps you directly to a specific point in the game, triggered in the Main Menu. \nThis will tag you at the Mission End Screen.");
                    ImGui::PopFont();
                }
                ImGui::EndChild();
            }
        }
        else if (context.SelectedOptionsSubTab == UI::UIContext::OptionsSubTabs::Camera) {
			// Widget area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.7f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("Widget Area", cntWindow->GetID("Widget Area"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					{
                        CameraSection(context.DefaultFontSize);
					}
				}
				ImGui::EndChild();
			}

			// Tooltip area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.3f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + cntWindow->Size.x - areaSize.x - 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				cntWindow->DrawList->AddRect(areaMin, areaMin + areaSize, UI::SwapColorEndianness(0x585152FF));

				ImVec2 padding{ context.DefaultFontSize * 0.8f, context.DefaultFontSize * 0.8f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("Tooltip Area", cntWindow->GetID("Tooltip Area"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[size_t(context.DefaultFontSize * 1.0f)]);
					ImGui::Text("CAMERA OPTIONS");
					ImGui::PopFont();


					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.8f);
					ImGui::PushFont(UI::g_ImGuiFont_Roboto[size_t(context.DefaultFontSize * 0.9f)]);

					ImGui::TextWrapped("Settings related to camera behaviour.");
					ImGui::PopFont();
				}
				ImGui::EndChild();
			}
        }
        else if (context.SelectedOptionsSubTab == UI::UIContext::OptionsSubTabs::Hotkeys) {

        }
        else if (context.SelectedOptionsSubTab == UI::UIContext::OptionsSubTabs::Interface) {
            getHUDsDirectories();

			// Widget area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.7f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("Widget Area", cntWindow->GetID("Widget Area"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					{
						InterfaceSection(context.DefaultFontSize);
					}
				}
				ImGui::EndChild();
			}

			// Tooltip area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.3f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + cntWindow->Size.x - areaSize.x - 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				cntWindow->DrawList->AddRect(areaMin, areaMin + areaSize, UI::SwapColorEndianness(0x585152FF));

				ImVec2 padding{ context.DefaultFontSize * 0.8f, context.DefaultFontSize * 0.8f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("Tooltip Area", cntWindow->GetID("Tooltip Area"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[size_t(context.DefaultFontSize * 1.0f)]);
					ImGui::Text("GUI OPTIONS");
					ImGui::PopFont();


					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.8f);
					ImGui::PushFont(UI::g_ImGuiFont_Roboto[size_t(context.DefaultFontSize * 0.9f)]);

					ImGui::TextWrapped("Change Crimson GUI's settings.");
					ImGui::PopFont();
				}
				ImGui::EndChild();
			}
        }
        else if (context.SelectedOptionsSubTab == UI::UIContext::OptionsSubTabs::WeaponWheel) {
        }
        else if (context.SelectedOptionsSubTab == UI::UIContext::OptionsSubTabs::SoundOrVisual) {
			// Widget area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.7f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("Widget Area", cntWindow->GetID("Widget Area"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					{
						SoundVisualSection(context.DefaultFontSize);
					}
				}
				ImGui::EndChild();
			}

			// Tooltip area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.3f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + cntWindow->Size.x - areaSize.x - 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				cntWindow->DrawList->AddRect(areaMin, areaMin + areaSize, UI::SwapColorEndianness(0x585152FF));

				ImVec2 padding{ context.DefaultFontSize * 0.8f, context.DefaultFontSize * 0.8f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("Tooltip Area", cntWindow->GetID("Tooltip Area"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[size_t(context.DefaultFontSize * 1.0f)]);
					ImGui::Text("SOUND/VISUAL OPTIONS");
					ImGui::PopFont();


					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.8f);
					ImGui::PushFont(UI::g_ImGuiFont_Roboto[size_t(context.DefaultFontSize * 0.9f)]);

					ImGui::TextWrapped("Settings related to Sound and Visual Effects.");
					ImGui::PopFont();
				}
				ImGui::EndChild();
			}
        }
        else if (context.SelectedOptionsSubTab == UI::UIContext::OptionsSubTabs::System) {
			// Widget area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.7f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("Widget Area", cntWindow->GetID("Widget Area"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					{
						SystemSection(context.DefaultFontSize);
					}
				}
				ImGui::EndChild();
			}

			// Tooltip area
			{
				const ImVec2 areaSize = cntWindow->Size * ImVec2{ 0.3f, 0.98f };
				const ImVec2 areaMin{ cntWindow->Pos.x + cntWindow->Size.x - areaSize.x - 0.1f * context.DefaultFontSize,
										 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

				cntWindow->DrawList->AddRect(areaMin, areaMin + areaSize, UI::SwapColorEndianness(0x585152FF));

				ImVec2 padding{ context.DefaultFontSize * 0.8f, context.DefaultFontSize * 0.8f };

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
				ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
				ImGui::BeginChildEx("Tooltip Area", cntWindow->GetID("Tooltip Area"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::PopStyleVar();
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[size_t(context.DefaultFontSize * 1.0f)]);
					ImGui::Text("SYSTEM OPTIONS");
					ImGui::PopFont();


					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.8f);
					ImGui::PushFont(UI::g_ImGuiFont_Roboto[size_t(context.DefaultFontSize * 0.9f)]);

					ImGui::TextWrapped("Settings related to the program's behavior.");
					ImGui::PopFont();
				}
				ImGui::EndChild();
			}
        }


	}
	break;

	case UI::UIContext::MainTabs::CheatsAndDebug:
	{

	}
	break;

	case UI::UIContext::MainTabs::None:
	{
		// If none of the tabs are selected, draw the about page
		static const Texture2DD3D11 socialIcons(g_Image_SocialIcons.GetRGBAData(), g_Image_SocialIcons.GetWidth(), g_Image_SocialIcons.GetWidth(), pDevice);

		const float areaPaddingXRation = 0.326f;
		const float areaPaddingX = (1.0f - 3.0f * (areaPaddingXRation)) * 0.25f * cntWindow->Size.x;

		// C Team area
		{
			const ImVec2 areaSize = cntWindow->Size * ImVec2{ areaPaddingXRation, 0.8f };
			const ImVec2 areaMin{ cntWindow->Pos.x + areaPaddingX,
									 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

			//cntWindow->DrawList->AddRect(areaMin, areaMin + areaSize, UI::SwapColorEndianness(0x00FF00FF));

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
			ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
			ImGui::BeginChildEx("C Team Area", cntWindow->GetID("C Team Area"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
			ImGui::PopStyleVar();
			{
				auto window = ImGui::GetCurrentWindow();
				ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 1.4f)]);
				{
					ImGui::Text("CREDITS");
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.2f);
					ImGui::Text((const char*)u8"C•Team");
				}
				ImGui::PopFont();

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.4f);

				auto fnDrawSocialButton = [window](const void* id, const size_t socialID, const ImVec2 size, const char* tooltip = nullptr)->bool {
					const auto bbTuple = g_Image_SocialIcons.GetUVRect(socialID);

					const ImRect bbUV{
						ImVec2{ std::get<0>(std::get<0>(bbTuple)), std::get<1>(std::get<0>(bbTuple)) },
						ImVec2{ std::get<0>(std::get<1>(bbTuple)), std::get<1>(std::get<1>(bbTuple)) }
					};

					bool clicked = false;

					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
					//ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
					{
						ImGui::PushID(id);
						const ImGuiID calcedID = window->GetID("#socialButton");
						ImGui::PopID();

						clicked = ImGui::ImageButtonEx(calcedID, socialIcons.GetTexture(), size,
							bbUV.Min, bbUV.Max, ImVec2{ 2.0f, 2.0f },
							ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f }, ImVec4{ 1.0f , 1.0f, 1.0f, 1.0f });
					    
						if (tooltip != nullptr && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
						{
							ImGui::SetTooltip(tooltip);
						}
                    }
					//ImGui::PopStyleColor();
					ImGui::PopStyleVar();

					return clicked;
					};

				// Mia
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 0.9f)]);
					{
						ImGui::Text("Project Director, Artist, Gameplay Programmer");
					}
					ImGui::PopFont();

					ImGui::Separator();

					ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
					{
						ImGui::Text("Mia Berth");

						ImGui::SameLine();

						const ImVec2 socialsBBFrameSize{ 6.0f * 4.0f + 6.0f * ImGui::GetFontSize(), 4.0f + ImGui::GetFontSize() };
						const ImVec2 currentCursorPos = ImGui::GetCursorScreenPos();

						ImGui::SetCursorScreenPos(ImVec2{ window->ContentRegionRect.Max.x - socialsBBFrameSize.x, currentCursorPos.y });

						if (fnDrawSocialButton("miatwitter", SocialsIcons::ID_Twitter, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
							ShellExecute(0, 0, "https://twitter.com/MiaBerth", 0, 0, SW_SHOW);
						}

						ImGui::SameLine(0.0f, 0.0f);

						if (fnDrawSocialButton("miagithub", SocialsIcons::ID_Github, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
							ShellExecute(0, 0, "https://github.com/berthrage", 0, 0, SW_SHOW);
						}

						ImGui::SameLine(0.0f, 0.0f);

						if (fnDrawSocialButton("miayt", SocialsIcons::ID_YouTube, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
							ShellExecute(0, 0, "https://www.youtube.com/@MiaBerth", 0, 0, SW_SHOW);
						}

						ImGui::SameLine(0.0f, 0.0f);

						if (fnDrawSocialButton("miareddit", SocialsIcons::ID_Reddit, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
							ShellExecute(0, 0, "https://www.reddit.com/user/Berthrage", 0, 0, SW_SHOW);
						}

						ImGui::SameLine(0.0f, 0.0f);

						if (fnDrawSocialButton("miadiscord", SocialsIcons::ID_Discord, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() }, "@miaberth")) {
							// TODO:: Copy
						}

						ImGui::SameLine(0.0f, 0.0f);

						if (fnDrawSocialButton("mianexus", SocialsIcons::ID_Nexusmods, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
							ShellExecute(0, 0, "https://www.nexusmods.com/users/95598128", 0, 0, SW_SHOW);
						}
					}
					ImGui::PopFont();
				}

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.4f);

				// Sarah
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 0.9f)]);
					{
						ImGui::Text("Reverse Engineering, Gameplay Programmer");
					}
					ImGui::PopFont();

					ImGui::Separator();

					ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
					{
						ImGui::Text("Siyan");
					}
					ImGui::PopFont();

					ImGui::SameLine();

					const ImVec2 socialsBBFrameSize{ 4.0f + ImGui::GetFontSize(), 4.0f + ImGui::GetFontSize() };
					const ImVec2 currentCursorPos = ImGui::GetCursorScreenPos();

					ImGui::SetCursorScreenPos(ImVec2{ window->ContentRegionRect.Max.x - socialsBBFrameSize.x, currentCursorPos.y });

					if (fnDrawSocialButton("sarahtwitter", SocialsIcons::ID_Twitter, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
						ShellExecute(0, 0, "https://twitter.com/SSSiyan", 0, 0, SW_SHOW);
					}
				}

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.4f);

				// Deep
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 0.9f)]);
					{
						ImGui::Text("Reverse Engineering, Graphics Programmer");
					}
					ImGui::PopFont();

					ImGui::Separator();

					ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
					{
						ImGui::Text("deepdarkkapustka");
					}
					ImGui::PopFont();

					ImGui::SameLine();

					const ImVec2 socialsBBFrameSize{ 4.0f * 2.0f + 2.0f * ImGui::GetFontSize(), 4.0f + ImGui::GetFontSize() };
					const ImVec2 currentCursorPos = ImGui::GetCursorScreenPos();

					ImGui::SetCursorScreenPos(ImVec2{ window->ContentRegionRect.Max.x - socialsBBFrameSize.x, currentCursorPos.y });

					if (fnDrawSocialButton("deepgit", SocialsIcons::ID_Github, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
						ShellExecute(0, 0, "https://github.com/muhopensores", 0, 0, SW_SHOW);
					}

					ImGui::SameLine(0.0f, 0.0f);

					if (fnDrawSocialButton("deepyt", SocialsIcons::ID_YouTube, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
						ShellExecute(0, 0, "https://www.youtube.com/@mstislavcapusta7573", 0, 0, SW_SHOW);
					}
				}

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.4f);

				// My smooth shiny bald ass
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 0.9f)]);
					{
						ImGui::Text("Backend Engineering, GUI Programmer");
					}
					ImGui::PopFont();

					ImGui::Separator();

					ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
					{
						ImGui::Text("Darkness");
					}
					ImGui::PopFont();

					ImGui::SameLine();

					const ImVec2 socialsBBFrameSize{ 4.0f * 2.0f + 2.0f * ImGui::GetFontSize(), 4.0f + ImGui::GetFontSize() };
					const ImVec2 currentCursorPos = ImGui::GetCursorScreenPos();

					ImGui::SetCursorScreenPos(ImVec2{ window->ContentRegionRect.Max.x - socialsBBFrameSize.x, currentCursorPos.y });

					if (fnDrawSocialButton("darknesstwitter", SocialsIcons::ID_Twitter, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
						ShellExecute(0, 0, "https://twitter.com/Darknes30239448", 0, 0, SW_SHOW);
					}

					ImGui::SameLine(0.0f, 0.0f);

					if (fnDrawSocialButton("darknessgithub", SocialsIcons::ID_Github, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
						ShellExecute(0, 0, "https://github.com/amir-120", 0, 0, SW_SHOW);
					}
				}

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.7f);

				// Serp
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 1.1f)]);
					{
						ImGui::Text("Original DDMK Developer");
					}
					ImGui::PopFont();

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.2f);

					ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
					{
						ImGui::Text("serpentiem");

						ImGui::SameLine();

						if (fnDrawSocialButton("serpgithub", SocialsIcons::ID_Github, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
							ShellExecute(0, 0, "https://github.com/serpentiem", 0, 0, SW_SHOW);
						}
					}
					ImGui::PopFont();
				}
			}
			ImGui::EndChild();
		}

		// Patrons area
		{
			const ImVec2 areaSize = cntWindow->Size * ImVec2{ areaPaddingXRation, 0.8f };
			const ImVec2 areaMin{ cntWindow->Pos.x + cntWindow->Size.x * areaPaddingXRation + areaPaddingX * 2.0f,
									 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

			//cntWindow->DrawList->AddRect(areaMin, areaMin + areaSize, UI::SwapColorEndianness(0x00FF00FF));

			ImVec2 padding{ context.DefaultFontSize * 0.8f, context.DefaultFontSize * 0.8f };

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
			ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
			ImGui::BeginChildEx("Patrons Area", cntWindow->GetID("Patrons Area"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
			ImGui::PopStyleVar();
			{
				ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 1.4f)]);
				{
					ImGui::Text("PATREON SUPPORTERS");
					ImGui::Separator();
				}
				ImGui::PopFont();

                if (UI::patronsQueueResult == WebAPIResult::Success || UI::patronsQueueResult == WebAPIResult::Awaiting) {
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.5f);

                    ImGui::PushFont(UI::g_ImGuiFont_RussoOne[context.DefaultFontSize]);
                    {
                        ImGui::Text(UI::g_UIContext.TierNames[(size_t)PatreonTiers_t::RichAF].c_str());
                    }
                    ImGui::PopFont();

                    ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
                    {
                        for (const auto& richAF : context.PatronsRichAF) {
                            ImGui::Text(richAF.c_str());
                        }
                    }
                    ImGui::PopFont();

                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.5f);

                    ImGui::PushFont(UI::g_ImGuiFont_RussoOne[context.DefaultFontSize]);
                    {
                        ImGui::Text(UI::g_UIContext.TierNames[(size_t)PatreonTiers_t::Rich].c_str());
                    }
                    ImGui::PopFont();

                    ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
                    {
                        for (const auto& rich : context.PatronsRich) {
                            ImGui::Text(rich.c_str());
                        }
                    }
                    ImGui::PopFont();
                }
			}
			ImGui::EndChild();
		}

		// Special Thanks area
		{
			const ImVec2 areaSize = cntWindow->Size * ImVec2{ areaPaddingXRation, 0.8f };
			const ImVec2 areaMin{ cntWindow->Pos.x + cntWindow->Size.x * areaPaddingXRation * 2.0f + areaPaddingX * 3.0f,
									 cntWindow->Pos.y + context.DefaultFontSize * 0.1f };

			//cntWindow->DrawList->AddRect(areaMin, areaMin + areaSize, UI::SwapColorEndianness(0x00FF00FF));

			ImVec2 padding{ context.DefaultFontSize * 0.8f, context.DefaultFontSize * 0.8f };

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
			ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
			ImGui::BeginChildEx("Special Thanks Area", cntWindow->GetID("Special Thanks Area"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
			ImGui::PopStyleVar();
			{
				ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 1.4f)]);
				{
					ImGui::Text("SPECIAL THANKS");
					ImGui::Separator();
				}
				ImGui::PopFont();

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.5f);

				ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
				{
					for (const auto& name : context.SpecialThanksNames) {
						ImGui::Text(name);
					}
				}
				ImGui::PopFont();
			}
			ImGui::EndChild();
		}

		// Footer area
		{
			const ImVec2 areaSize{ cntWindow->Size.x - areaPaddingX * 2.0f, context.DefaultFontSize * 4.4f };
			const ImVec2 areaMin{ cntWindow->Pos.x + areaPaddingX,
									 cntWindow->Pos.y + cntWindow->Size.y - cntWindow->Size.y * 0.18f };

			//cntWindow->DrawList->AddRect(areaMin, areaMin + areaSize, UI::SwapColorEndianness(0x00FF00FF));

			ImVec2 padding{ context.DefaultFontSize * 0.8f, context.DefaultFontSize * 0.8f };

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { context.DefaultFontSize * 0.4f, context.DefaultFontSize * 0.4f });
			ImGui::SetNextWindowPos(areaMin, ImGuiCond_Always);
			ImGui::BeginChildEx("Footer Area", cntWindow->GetID("Footer Area"), areaSize, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
			ImGui::PopStyleVar();
			{
				auto window = ImGui::GetCurrentWindow();
				constexpr const char* THX_TEXT = "Thank you all for playing and supporting the project!";
				constexpr const char* LICENCE_BUTTON_TEXT = "LICENCE";
				constexpr const char* COPYRIGHT_TEXT = "Copyright (c) 2023 Mia Berth";

				{
					const float thxTextWidth = ImGui::CalcTextSize(THX_TEXT).x;
					window->DrawList->AddText(ImVec2{ areaMin.x + (areaSize.x - thxTextWidth) * 0.5f, areaMin.y + context.DefaultFontSize * 0.6f }, 0xFFFFFFFF, THX_TEXT);
				}

				ImGui::PushFont(UI::g_ImGuiFont_RussoOne[context.DefaultFontSize]);
				{
					const float licenceButtonWidth = ImGui::CalcTextSize(LICENCE_BUTTON_TEXT).x + style.FramePadding.x * 2.0f;

					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
					ImGui::SetCursorScreenPos(ImVec2{ areaMin.x + (areaSize.x - licenceButtonWidth) * 0.5f, areaMin.y + context.DefaultFontSize * 2.0f });
					UI::InfoButton(LICENCE_BUTTON_TEXT);
					ImGui::PopStyleVar();
				}
				ImGui::PopFont();

				ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 0.8f)]);
				{
					const float crTextWidth = ImGui::CalcTextSize(COPYRIGHT_TEXT).x;
					window->DrawList->AddText(ImVec2{ areaMin.x + (areaSize.x - crTextWidth) * 0.5f, areaMin.y + context.DefaultFontSize * 3.5f }, 0xFFFFFFFF, COPYRIGHT_TEXT);
				}
				ImGui::PopFont();
			}
			ImGui::EndChild();
		}
	}
	break;

	default:
		context.SelectedTab = UI::UIContext::MainTabs::None;
		break;
	}
}

#pragma endregion

void GUI_Render(IDXGISwapChain* pSwapChain) {
    static bool run = false;

    if (!run) {
        run = true;

        CreateTexturesWeaponWheel();
    }

    initSDL();
    if (g_scene != SCENE::GAME) {
        devilTriggerReadyPlayed = !activeConfig.playDTReadySFXAtMissionStart;
    }

	UI::ResetID(0);

    AdjustBackgroundTransparency();

    Welcome();
    Main(pSwapChain);
    //CreditsWindow(); // old ddmk credits
    ShopWindow();

    if constexpr (debug) {
        ActorWindow();
        EventDataWindow();
        ExpWindow();
        FileDataWindow();
        MissionDataWindow();
        RegionDataWindow();
        SoundWindow();
    }

    
    PauseWhenGUIOpen();
    GameTrackDetection();
    MainOverlayWindow(UI::g_UIContext.DefaultFontSize);
    MissionOverlayWindow(UI::g_UIContext.DefaultFontSize);
    BossLadyActionsOverlayWindow();
    BossVergilActionsOverlayWindow();
    GunDTCharacterRemaps();

    CorrectFrameRateCutscenes();
    GamepadToggleShowMain();
    DelayedComboEffectsController();

    // TIMERS
    BackToForwardTimers();
    ActionTimers();
    AnimTimers();
    SiyTimerFunc();
    SprintTimer();
    DriveTimer();
    ImprovedCancelsTimers();
    StyleSwitchTextTimers();
    StyleSwitchFluxTimers();


    Bars();
    WeaponSwitchController();


    HandleKeyBindings(keyBindings.data(), keyBindings.size());


    if (g_shopTimer > 0) {
        g_shopTimer -= 1.0f;
    }

    HandleSaveTimer(activeConfig.frameRate);
    ImGui::PopStyleColor();

    // static bool enable = true;
    // ImGui::ShowDemoWindow(&enable);
}

void GUI_Init() {
    LogFunction();

    //BuildFonts();

    UpdateGlobalScale();

    UI::SetStyleCrimson();

    Actor_UpdateIndices();
    Arcade_UpdateIndices();
    Color_UpdateValues();
}

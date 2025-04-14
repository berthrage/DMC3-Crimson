// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "../ThirdParty/SDL2/SDL_gamecontroller.h"
#include "../ThirdParty/glm/glm.hpp"
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
#include "DMC3Input.hpp"
#include "File.hpp"
#include "ImGuiExtra.hpp"
#include "FMOD.hpp"
#include "Graphics.hpp"
#include "Internal.hpp"
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
#include "Training.hpp"
#include "Window.hpp"
#include "WebAPICalls.hpp"
#include "UI\Texture2DD3D11.hpp"


#include "UI\EmbeddedImages.hpp"
#include "UI\WeaponWheel.hpp"

#include <cmath>
#include <array>
#include <format>
#include <memory>

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
#include "CrimsonTimers.hpp"
#include "CrimsonOnTick.hpp"
#include "CrimsonSDL.hpp"
#include "CrimsonPatches.hpp"
#include "CrimsonDetours.hpp"

#include "DebugDrawDX11.hpp"

#include "ImGui/imgui.h"

#define SDL_FUNCTION_DECLRATION(X) decltype(X)* fn_##X
#define LOAD_SDL_FUNCTION(X) fn_##X = GetSDLFunction<decltype(X)*>(#X)

void DrawMainContent(ID3D11Device* pDevice, UI::UIContext& context);

namespace UI {
WebAPIResult versionCheckResult = WebAPIResult::Awaiting;
WebAPIResult patronsQueueResult = WebAPIResult::Awaiting;

void DrawCrimson(IDXGISwapChain* pSwapChain, const char* title, bool* pIsOpened) {
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
					SwapColorEndianness(0x00000050)
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
					if (TabButton("SOUND", g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::Sound, true, false, subTabBtnSize)) {
						g_UIContext.SelectedOptionsSubTab = UIContext::OptionsSubTabs::Sound;
					}

					ImGui::SameLine(0.0f, tabButtonsGap);
					if (TabButton("VISUAL", g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::Visual, true, false, subTabBtnSize)) {
						g_UIContext.SelectedOptionsSubTab = UIContext::OptionsSubTabs::Visual;
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
					else if (g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::Sound) {
						window->DrawList->AddText(g_ImGuiFont_RussoOne256, g_UIContext.DefaultFontSize * 9.6f, pos,
							SwapColorEndianness(0xFFFFFF10), "Sound");
					}
					else if (g_UIContext.SelectedOptionsSubTab == UIContext::OptionsSubTabs::Visual) {
						window->DrawList->AddText(g_ImGuiFont_RussoOne256, g_UIContext.DefaultFontSize * 9.6f, pos,
							SwapColorEndianness(0xFFFFFF10), "Visual");
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
				SwapColorEndianness(0x1F1718FF), window->WindowRounding, ImDrawCornerFlags_Bot);

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
				constexpr auto CREDIT_TEXT = u8"Berthrage • SSSiyan • deepdarkkapustka • Darkness   ";
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

bool visibleMain = false;
bool lastVisibleMain = false;

bool visibleShop = false;
bool lastVisibleShop = false;

bool updateWeaponWheel = false;

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
	"Vanilla",
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

const char* collisionGroupNames[] = { "Player", "Enemy" };


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
	"Doppelganger",
	"Quicksilver",
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
	"QUICKSILVER",
	"DOPPELGANGER",
};

constexpr uint8 stylesVergil[] = {
	STYLE::DARK_SLAYER,
	STYLE::DOPPELGANGER,
	STYLE::QUICKSILVER,
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

const char* costumeNamesDante[] = {
	"DMC3",
	"DMC3 Coatless",
	"DMC3 Torn",
	"DMC1",
	"DMC1 Coatless",
	"Sparda",
	"Super Dante",
	"Super DMC1 Dante",
};

const char* costumeNamesVergil[] = {
	"DMC3",
	"DMC3 Coatless",
	"Super Vergil",
	"Corrupted Vergil",
	"Super Corrupted Vergil",
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

std::vector<std::string> weaponWheelThemeNames = {
	"Crimson",
	"DMC3 Switch",
};

std::vector<std::string> weaponWheelScaleNames = {
	"Small",
	"Big",
};

std::vector<std::string> show1PNames = {
	"Off",
	"Only in Multiplayer",
	"Always",
};

std::vector<std::string> worldSpaceWheelNames = {
	"Off",
	"Only in Multiplayer",
	"Always",
};

std::vector<std::string> VergilMoveAdjustmentsNames = {
	"Off",
	"From Air",
	"Always",
};

static_assert(countof(trackFilenames) == countof(trackNames));

#pragma endregion

void PauseWhenGUIOpened() {
	auto pool_10298 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
	if (!pool_10298 || !pool_10298[8]) {
		return;
	}
	auto& eventData = *reinterpret_cast<EventData*>(pool_10298[8]);

	auto name_10723 = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E30);
	if (!name_10723) {
		return;
	}
	auto& missionData = *reinterpret_cast<MissionData*>(name_10723);



	auto pool_10222 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_10222 || !pool_10222[3]) {
		return;
	}
	auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_10222[3]);

	static uint32 storedFrameCount = 0;
	if (g_inGameCutscene || !activeCrimsonConfig.GUI.pauseWhenOpened) {
		return;
	}

	// We add this timer so we can safely (aka no crash) say when we can pause the game by setting speed to 0.
	if (g_scene != SCENE::GAME || eventData.event != EVENT::MAIN) {
		guiPause.timer = 0.5f;
		guiPause.canPause = false;
		g_inGameDelayed = false;
	}
	else {
		g_inGameDelayed = true;

		if (guiPause.timer > 0) {
			guiPause.timer -= ImGui::GetIO().DeltaTime;
		}
	}

	if (guiPause.timer <= 0) {
		guiPause.canPause = true;
	}
// 
// 
// 	if (!g_show || !guiPause.canPause) {
// 		storedFrameCount = missionData.frameCount; // This stores the game's timer.
// 		activeConfig.Speed.mainSpeed = queuedConfig.Speed.mainSpeed; // This resumes the game speed
// 		activeConfig.Speed.turbo = queuedConfig.Speed.turbo;
// 		Speed::Toggle(true); 
// 		guiPause.in = false;
// 
// 	}
// 	else if (g_show && !guiPause.in && guiPause.canPause) {
// 		activeConfig.Speed.mainSpeed = 0;  // This pauses the game speed
// 		activeConfig.Speed.turbo = 0;
// 		Speed::Toggle(true); // Toggle Speed on and off to set the new speed
// 		Speed::Toggle(false);
// 		guiPause.in = true;
// 	}
// 
// 
// 	if (g_showMain) {
// 		missionData.frameCount = storedFrameCount;  // This pauses the game's timer.
// 	}
}

std::unique_ptr<WW::WeaponWheel> meleeWeaponWheel[PLAYER_COUNT];
std::unique_ptr<WW::WeaponWheel> rangedWeaponWheel[PLAYER_COUNT];
std::unique_ptr<WW::WeaponWheel>  meleeWorldSpaceWeaponWheel[PLAYER_COUNT];
std::unique_ptr<WW::WeaponWheel>  rangedWorldSpaceWeaponWheel[PLAYER_COUNT];
bool multiplayerWheelsLoaded = false;

#pragma endregion

#pragma region Weapon Wheel Controllers

void UpdateMeleeWeaponIDs(PlayerActorData& actorData, std::vector<WW::WeaponIDs>(&currentWeapons)[CHARACTER_COUNT], size_t charIdx) {
	auto playerIndex = actorData.newPlayerIndex;
	auto playerData = GetPlayerData(playerIndex);
	auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

	for (size_t i = 0; i < currentWeapons[charIdx].size(); i++) {
		auto& characterData = playerData.characterData[charIdx][ENTITY::MAIN];

		if (characterData.character == CHARACTER::DANTE) {
			switch (characterData.meleeWeapons[i]) {

			case WEAPON::REBELLION:
				if (!characterData.sparda) {
					if (sessionData.unlockDevilTrigger)
						currentWeapons[charIdx][i] = WW::WeaponIDs::RebellionAwakened;
					else
						currentWeapons[charIdx][i] = WW::WeaponIDs::RebellionDormant;
				}
				else {
					currentWeapons[charIdx][i] = WW::WeaponIDs::ForceEdge;
				}

				break;

			case WEAPON::CERBERUS:
				currentWeapons[charIdx][i] = WW::WeaponIDs::Cerberus;
				break;

			case WEAPON::BEOWULF_DANTE:
				currentWeapons[charIdx][i] = WW::WeaponIDs::Beowulf;
				break;

			case WEAPON::AGNI_RUDRA:
				currentWeapons[charIdx][i] = WW::WeaponIDs::AgniRudra;
				break;

			case WEAPON::NEVAN:
				currentWeapons[charIdx][i] = WW::WeaponIDs::Nevan;
				break;
			}
		}
		else if (characterData.character == CHARACTER::VERGIL) {
			switch (characterData.meleeWeapons[i]) {

			case WEAPON::YAMATO_VERGIL:
				currentWeapons[charIdx][i] = WW::WeaponIDs::Yamato;
				break;

			case WEAPON::BEOWULF_VERGIL:
				currentWeapons[charIdx][i] = WW::WeaponIDs::Beowulf;
				break;

			case WEAPON::YAMATO_FORCE_EDGE:
				currentWeapons[charIdx][i] = WW::WeaponIDs::ForceEdgeYamato;
				break;

			}
		}
	}
}

void UpdateRangedWeaponIDs(PlayerActorData& actorData, std::vector<WW::WeaponIDs>(&currentWeapons)[CHARACTER_COUNT], size_t charIdx) {
	auto playerIndex = actorData.newPlayerIndex;
	auto playerData = GetPlayerData(playerIndex);

	for (size_t i = 0; i < currentWeapons[charIdx].size(); i++) {
		auto& characterData = playerData.characterData[charIdx][ENTITY::MAIN];

		if (characterData.character == CHARACTER::DANTE) {
			switch (characterData.rangedWeapons[i]) {

			case WEAPON::EBONY_IVORY:
				currentWeapons[charIdx][i] = WW::WeaponIDs::EbonyIvory;
				break;

			case WEAPON::SHOTGUN:
				currentWeapons[charIdx][i] = WW::WeaponIDs::Shotgun;
				break;

			case WEAPON::ARTEMIS:
				currentWeapons[charIdx][i] = WW::WeaponIDs::Artemis;
				break;

			case WEAPON::SPIRAL:
				currentWeapons[charIdx][i] = WW::WeaponIDs::Spiral;
				break;

			case WEAPON::KALINA_ANN:
				currentWeapons[charIdx][i] = WW::WeaponIDs::KalinaAnn;
				break;
			}
		}
	}
}

struct WeaponWheelState {
	int oldWeaponIndex = -1;
	int oldCharIndex = -1;
	WW::WheelThemes charTheme = WW::WheelThemes::Neutral;
	std::string oldTheme = activeCrimsonConfig.WeaponWheel.theme;
	std::vector<WW::WeaponIDs> currentWeapons[CHARACTER_COUNT];
	std::array<std::array<uint8_t, 5>, CHARACTER_COUNT> oldCharWeapons;
	std::array<uint8_t, CHARACTER_COUNT> oldPlayerDataChars;
	std::array<uint8_t, CHARACTER_COUNT> oldCharCostumes;
	bool oldUnlockedDevilTrigger = false;
	bool initializedOld = false;
	double startTime = ImGui::GetTime();
};

bool WeaponWheelController(PlayerActorData& actorData, IDXGISwapChain* pSwapChain, std::unique_ptr<WW::WeaponWheel>& pWeaponWheel, 
	const char* windowName, bool cornerPositioning, ImVec2 windowPos, ImVec2 wheelSize, 
	bool alwaysShow, bool trackHide, bool isMelee, WeaponWheelState& state,
	float deltaTimeGameSpeed, float deltaTime) {

	if (!InGame()) {
		return false;
	}
	auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

	auto playerIndex = actorData.newPlayerIndex;
	auto playerData = GetPlayerData(playerIndex);
	auto& characterData = GetCharacterData(actorData);

	auto& gamepad = GetGamepad(actorData.newPlayerIndex);

	if (InCutscene() || InCredits() || !activeConfig.Actor.enable || g_inGameCutscene ||
		!((characterData.character == CHARACTER::DANTE) || (characterData.character == CHARACTER::VERGIL))) {
		return false;
	}

	auto& charIndex = actorData.newCharacterIndex;
	auto& weaponIndex = isMelee ? characterData.meleeWeaponIndex : characterData.rangedWeaponIndex;
	auto& weaponCount = isMelee ? playerData.characterData[charIndex][ENTITY::MAIN].meleeWeaponCount : playerData.characterData[charIndex][ENTITY::MAIN].rangedWeaponCount;
	auto activeGameSpeed = (IsTurbo()) ? activeConfig.Speed.turbo : activeConfig.Speed.mainSpeed;

	auto leftStick = (characterData.rangedWeaponSwitchStick == LEFT_STICK);
	auto radius = (leftStick) ? gamepad.leftStickRadius : gamepad.rightStickRadius;
	auto stickUsed = radius > RIGHT_STICK_DEADZONE ? true : false;
	auto weapons = isMelee ? playerData.characterData[charIndex][ENTITY::MAIN].meleeWeapons : playerData.characterData[charIndex][ENTITY::MAIN].rangedWeapons;

	// Feed old arrays for comparisons. -- only once during runtime.
	if (!state.initializedOld) {
		for (size_t charIdx = 0; charIdx < CHARACTER_COUNT; ++charIdx) {
			for (size_t j = 0; j < 5; ++j) {
				state.oldCharWeapons[charIdx][j] = weapons[j];
			}
			state.oldPlayerDataChars[charIdx] = playerData.characterData[charIdx][0].character;
			state.oldCharCostumes[charIdx] = playerData.characterData[charIdx][0].costume;
		}
		state.oldUnlockedDevilTrigger = false;
		state.initializedOld = true;
	}

	// Set character theme based on actor's character.
	switch (actorData.character) {
	case CHARACTER::DANTE:
		state.charTheme = WW::WheelThemes::Dante;
		break;
	case CHARACTER::VERGIL:
		state.charTheme = WW::WheelThemes::Vergil;
		break;
	default:
		state.charTheme = WW::WheelThemes::Neutral;
		break;
	}

	// Create and feed current arrays for comparisons.
	std::array<std::array<uint8_t, 5>, CHARACTER_COUNT> currentCharWeapons;
	std::array<uint8, 3> currentPlayerDataChars;
	std::array<uint8, 3> currentCharCostumes;
	for (size_t charIdx = 0; charIdx < CHARACTER_COUNT; ++charIdx) {
		for (size_t j = 0; j < 5; ++j) {
			currentCharWeapons[charIdx][j] = weapons[j];
		}
		currentPlayerDataChars[charIdx] = playerData.characterData[charIdx][0].character;
		currentCharCostumes[charIdx] = playerData.characterData[charIdx][0].costume;
	}

	if (!g_allActorsSpawned) {
		return false;
	}

	// Reload the Wheel if Character Settings/Loadouts have changed to update all weapon sprites.
	if (state.currentWeapons[charIndex].size() != weaponCount
		|| state.oldCharWeapons[charIndex] != currentCharWeapons[charIndex]
		|| state.oldUnlockedDevilTrigger != sessionData.unlockDevilTrigger
			|| state.oldPlayerDataChars[charIndex] != currentPlayerDataChars[charIndex]
			|| state.oldCharCostumes[charIndex] != currentCharCostumes[charIndex]) {

		state.currentWeapons[charIndex].resize(weaponCount);

		if (isMelee) {
			UpdateMeleeWeaponIDs(actorData, state.currentWeapons, charIndex);
		} else {
			UpdateRangedWeaponIDs(actorData, state.currentWeapons, charIndex);
		}

		if (pWeaponWheel) {
			pWeaponWheel->ReloadWheel(state.currentWeapons);
			pWeaponWheel->SetWheelTheme(state.charTheme);
			pWeaponWheel->SetActiveSlot(weaponIndex);
		}

		state.oldCharWeapons[charIndex] = currentCharWeapons[charIndex];
		state.oldPlayerDataChars[charIndex] = currentPlayerDataChars[charIndex];
		state.oldCharCostumes[charIndex] = currentCharCostumes[charIndex];
	}

	// Instancing the Wheel class.
	if (!pWeaponWheel) {
		ID3D11DeviceContext* pDeviceContext = nullptr;

		ID3D11Device* pD3D11Device = nullptr;

		pSwapChain->GetDevice(IID_PPV_ARGS(&pD3D11Device));

		pD3D11Device->GetImmediateContext(&pDeviceContext);

		pWeaponWheel = std::make_unique<WW::WeaponWheel>(pD3D11Device, pDeviceContext, wheelSize.x, wheelSize.y,
			state.currentWeapons, activeCrimsonConfig.WeaponWheel.theme == "Crimson" ? state.charTheme : WW::WheelThemes::Neutral
			, actorData.buttons[1] & GetBinding(isMelee ? BINDING::CHANGE_DEVIL_ARMS : BINDING::CHANGE_GUN));

		pWeaponWheel->SetActiveSlot(weaponIndex);
		pWeaponWheel->TrackAlwaysShowConfig(alwaysShow);

		state.oldWeaponIndex = weaponIndex;
	}

	if (!pWeaponWheel) {
		return false;
	}

	if (state.oldWeaponIndex != (int)weaponIndex || state.oldCharIndex != (int)charIndex) {// If changed set it
		if (isMelee) {
			UpdateMeleeWeaponIDs(actorData, state.currentWeapons, actorData.newCharacterIndex);
		} else {
			UpdateRangedWeaponIDs(actorData, state.currentWeapons, actorData.newCharacterIndex);
		}
		pWeaponWheel->UpdateCharIndex(actorData.newCharacterIndex);
		pWeaponWheel->UpdateWeapons(state.currentWeapons);

		if (activeCrimsonConfig.WeaponWheel.theme == "Crimson") {
			pWeaponWheel->SetWheelTheme(state.charTheme);
		} else {
			pWeaponWheel->SetWheelTheme(WW::WheelThemes::Neutral);
		}

		pWeaponWheel->SetActiveSlot((int)weaponIndex);
	}

	if (state.oldTheme != activeCrimsonConfig.WeaponWheel.theme) {
		if (activeCrimsonConfig.WeaponWheel.theme == "Crimson") {
			pWeaponWheel->SetWheelTheme(state.charTheme);
		} else {
			pWeaponWheel->SetWheelTheme(WW::WheelThemes::Neutral);
		}
		pWeaponWheel->SetActiveSlot((int)weaponIndex);
	}

	state.oldTheme = activeCrimsonConfig.WeaponWheel.theme;
	state.oldWeaponIndex = (int)weaponIndex;
	state.oldCharIndex = (int)charIndex;
	state.oldUnlockedDevilTrigger = sessionData.unlockDevilTrigger;

	// Draw the wheel
	static bool isOpen = true;

	auto analogSwitchingState = playerIndex == 0 ? activeCrimsonConfig.WeaponWheel.analogSwitching ? true : false : true;

	static auto windowSize = ImVec2(g_renderSize.x, g_renderSize.y);
	auto cornerPos = isMelee ? windowSize - wheelSize : ImVec2(0, windowSize.y - wheelSize.y);
	ImGui::SetNextWindowSize(wheelSize);
	ImGui::SetNextWindowPos(cornerPositioning ? cornerPos : windowPos);

	pWeaponWheel->OnUpdate(deltaTimeGameSpeed,
		deltaTime, deltaTime, deltaTime);
	pWeaponWheel->TrackButtonHeldState(actorData.buttons[1] & GetBinding(isMelee ? BINDING::CHANGE_DEVIL_ARMS : BINDING::CHANGE_GUN));
	pWeaponWheel->TrackAnalogMovingState(stickUsed);
	pWeaponWheel->TrackAnalogSwitchingConfig(analogSwitchingState);
	pWeaponWheel->TrackAlwaysShowConfig(alwaysShow);
	state.startTime = ImGui::GetTime();

	pWeaponWheel->SetLoaded(pWeaponWheel->OnDraw());

	if (activeCrimsonConfig.WeaponWheel.hide && trackHide) {
		return true;
	}

	if (ImGui::Begin(windowName, &isOpen, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground)) {
		ImGui::Image(pWeaponWheel->GetSRV(), wheelSize);

		ImGui::End();
		return true;
	} 
	
	return false;
}

void WeaponWheels1PController(IDXGISwapChain* pSwapChain) {
	static WeaponWheelState stateMelee;
	static WeaponWheelState stateRanged;
	static bool initialized = false;

	auto pool_1431 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_1431 || !pool_1431[3]) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(pool_1431[3]);
	auto playerIndex = actorData.newPlayerIndex;
	auto& playerScreenPosition = crimsonPlayer[playerIndex].playerScreenPosition;
	auto distanceClamped = crimsonPlayer[playerIndex].cameraPlayerDistanceClamped;

	float deltaTime = ImGui::GetIO().DeltaTime;
	auto activeGameSpeed = (IsTurbo()) ? activeConfig.Speed.turbo : activeConfig.Speed.mainSpeed;
	float deltaTimeAdjustedSpeed = deltaTime * 1000.0f * (activeGameSpeed / g_FrameRateTimeMultiplier);
	float deltaTimeAdjusted = deltaTime * 1000.0f;

	auto inMultiplayer = activeCrimsonConfig.WeaponWheel.force1PMultiplayerPosScale || activeConfig.Actor.playerCount > 1;
	auto& scale = activeCrimsonConfig.WeaponWheel.scale;
	ImVec2 normalPos = ImVec2(g_renderSize.x - g_renderSize.y * 0.45f, g_renderSize.y - g_renderSize.y * 0.45f);
	ImVec2 normalSize = initialized ? scale == "Big" ? ImVec2(g_renderSize.y * 0.45f, g_renderSize.y * 0.45f) :
		ImVec2(g_renderSize.y * 0.35f, g_renderSize.y * 0.35f) :
		ImVec2(g_renderSize.y * 0.45f, g_renderSize.y * 0.45f);

	ImVec2 multiplayerPosMelee = ImVec2(g_renderSize.x * 0.4853f, g_renderSize.y * 0.0f);
	ImVec2 multiplayerPosRanged = ImVec2(g_renderSize.x * 0.4253f, g_renderSize.y * 0.0f);
	ImVec2 multiplayerSize = initialized ? ImVec2(g_renderSize.y * 0.15f, g_renderSize.y * 0.15f) : normalSize;
	// We need to initialize with the bigger size first to avoid the wheel from displaying on a lower quality if the user changes the setting at runtime. -- Berth


	// Adjusts size dynamically based on the distance between Camera and Player

	if (WeaponWheelController(actorData, pSwapChain, meleeWeaponWheel[0], "MeleeWheel1P",
		!inMultiplayer,
		inMultiplayer ? multiplayerPosMelee : normalPos, inMultiplayer ? multiplayerSize : normalSize,
		activeCrimsonConfig.WeaponWheel.alwaysShow, true, true, stateMelee, deltaTimeAdjustedSpeed, deltaTimeAdjusted)) {

		initialized = true;
	}

	WeaponWheelController(actorData, pSwapChain, rangedWeaponWheel[0], "RangedWheel1P",
		!inMultiplayer,
		inMultiplayer ? multiplayerPosRanged : normalPos, inMultiplayer ? multiplayerSize : normalSize,
		activeCrimsonConfig.WeaponWheel.alwaysShow, true, false, stateRanged, deltaTimeAdjustedSpeed, deltaTimeAdjusted);
}

void WeaponWheelsMultiplayerController(IDXGISwapChain* pSwapChain) {
	static WeaponWheelState stateMelee[PLAYER_COUNT];
	static bool initializedMelee[PLAYER_COUNT] = { false };

	static WeaponWheelState stateRanged[PLAYER_COUNT];
	static bool initializedRanged[PLAYER_COUNT] = { false };

	float deltaTime = ImGui::GetIO().DeltaTime;
	auto activeGameSpeed = (IsTurbo()) ? activeConfig.Speed.turbo : activeConfig.Speed.mainSpeed;
	float deltaTimeAdjustedSpeed = deltaTime * 1000.0f * (activeGameSpeed / g_FrameRateTimeMultiplier);
	float deltaTimeAdjusted = deltaTime * 1000.0f;

	if (!activeCrimsonConfig.MultiplayerBars2D.show) return;

	for (uint8 playerIndex = 1; playerIndex < activeConfig.Actor.playerCount; ++playerIndex) {
		auto& playerData = GetPlayerData(playerIndex);
		auto& characterData = GetCharacterData(playerIndex, playerData.characterIndex, ENTITY::MAIN);
		auto& newActorData = GetNewActorData(playerIndex, playerData.characterIndex, ENTITY::MAIN);

		if (!newActorData.baseAddr) {
			return;
		}
		auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);

		auto& playerScreenPosition = crimsonPlayer[playerIndex].playerScreenPosition;
		auto distanceClamped = crimsonPlayer[playerIndex].cameraPlayerDistanceClamped;
		auto activeGameSpeed = (IsTurbo()) ? activeConfig.Speed.turbo : activeConfig.Speed.mainSpeed;


		ImVec2 normalSize = ImVec2(g_renderSize.y * 0.45f, g_renderSize.y * 0.45f);

		ImVec2 baseMultiplayerSize = ImVec2(g_renderSize.y * 0.15f, g_renderSize.y * 0.15f);
		ImVec2 multiplayerSize = initializedMelee[playerIndex] ? baseMultiplayerSize : normalSize;

		auto& meleeWheel = meleeWeaponWheel[playerIndex];
		auto& rangedWheel = rangedWeaponWheel[playerIndex];
		const float baseSpacing = 0.37f;
		const float barSpacing = g_renderSize.x * baseSpacing;

		const float baseXMelee = g_renderSize.x * (0.1722f - baseSpacing);
		const float baseXRanged = g_renderSize.x * (0.1122f - baseSpacing);

		
		const float baseY = g_renderSize.y * 0.8462f; // Bottom of the screen


		ImVec2 multiplayerPosMelee = ImVec2(baseXMelee + barSpacing * (playerIndex), baseY);
		ImVec2 multiplayerPosRanged = ImVec2(baseXRanged + barSpacing * (playerIndex), baseY);

		std::string meleeWheelName = "MeleeWheel " + std::to_string(playerIndex + 1);
		std::string rangedWheelName = "RangedWheel " + std::to_string(playerIndex + 1);


		if (WeaponWheelController(actorData, pSwapChain, meleeWheel, meleeWheelName.c_str(),
			false,
			multiplayerPosMelee, multiplayerSize,
			activeCrimsonConfig.WeaponWheel.alwaysShow, true, true, stateMelee[playerIndex], deltaTimeAdjustedSpeed, deltaTimeAdjusted)) {

			initializedMelee[playerIndex] = true;
		}

		WeaponWheelController(actorData, pSwapChain, rangedWheel, rangedWheelName.c_str(),
			false,
			multiplayerPosRanged, multiplayerSize,
			activeCrimsonConfig.WeaponWheel.alwaysShow, true, false, stateRanged[playerIndex], deltaTimeAdjustedSpeed, deltaTimeAdjusted);
	}

	multiplayerWheelsLoaded = true; 

	for (uint8 playerIndex = 1; playerIndex < activeConfig.Actor.playerCount; ++playerIndex) {
		auto& meleeWheel = meleeWeaponWheel[playerIndex];
		auto& rangedWheel = rangedWeaponWheel[playerIndex];

		if (!meleeWheel->m_loaded || !rangedWheel->m_loaded) {
			multiplayerWheelsLoaded = false; 
			break;
		}
	}
}

void WorldSpaceWeaponWheels1PController(IDXGISwapChain* pSwapChain) {
	static WeaponWheelState stateMelee;
	static bool initialized = false;

	static WeaponWheelState stateRanged;

	if (!multiplayerWheelsLoaded) return;
	if (activeCrimsonConfig.WeaponWheel.worldSpaceWheels == "Off" ||
		(activeCrimsonConfig.WeaponWheel.worldSpaceWheels == "Only in Multiplayer"
			&& activeConfig.Actor.playerCount <= 1)) return;

	float deltaTime = ImGui::GetIO().DeltaTime;
	auto activeGameSpeed = (IsTurbo()) ? activeConfig.Speed.turbo : activeConfig.Speed.mainSpeed;
	float deltaTimeAdjustedSpeed = deltaTime * 1000.0f * (activeGameSpeed / g_FrameRateTimeMultiplier);
	float deltaTimeAdjusted = deltaTime * 1000.0f;

	auto pool_1431 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_1431 || !pool_1431[3]) {
		return;
	}
	auto& actorData = *reinterpret_cast<PlayerActorData*>(pool_1431[3]);

	auto& playerScreenPosition = crimsonPlayer[0].playerScreenPosition;
	auto distanceClamped = crimsonPlayer[0].cameraPlayerDistanceClamped;

	ImVec2 normalSize = ImVec2(g_renderSize.y * 0.45f, g_renderSize.y * 0.45f);
	ImVec2 baseMultiplayerSize = ImVec2(g_renderSize.y * 0.17f, g_renderSize.y * 0.17f);
	ImVec2 sizeDistance = { (baseMultiplayerSize.x * (1.0f / ((float)distanceClamped / 20))),
							(baseMultiplayerSize.y * (1.0f / ((float)distanceClamped / 20))) };
	ImVec2 multiplayerSize = initialized ? sizeDistance : normalSize;
	ImVec2 worldSpaceMultiplayerPos = ImVec2(playerScreenPosition.x - (sizeDistance.x / 2.0f), playerScreenPosition.y - sizeDistance.y);

	auto& meleeWheel = meleeWorldSpaceWeaponWheel[0];
	auto& rangedWheel = rangedWorldSpaceWeaponWheel[0];

	const float baseSpacing = 170.0f;
	float scaleFactor = multiplayerSize.x / normalSize.x;
	float adjustedSpacing = baseSpacing * scaleFactor;

	ImVec2 meleeWheelPos = ImVec2(worldSpaceMultiplayerPos.x + adjustedSpacing, worldSpaceMultiplayerPos.y);
	ImVec2 rangedWheelPos = ImVec2(worldSpaceMultiplayerPos.x - adjustedSpacing, worldSpaceMultiplayerPos.y);

	std::string meleeWheelName = "MeleeWheelWorldSpace " + std::to_string(0);
	std::string rangedWheelName = "RangedWheelWorldSpace " + std::to_string(0);

	if (WeaponWheelController(actorData, pSwapChain, meleeWheel, meleeWheelName.c_str(),
		false,
		meleeWheelPos, multiplayerSize,
		activeCrimsonConfig.WeaponWheel.worldSpaceAlwaysShow, false, true,
		stateMelee, deltaTimeAdjustedSpeed, deltaTimeAdjusted)) {
		initialized = true;
	}


	WeaponWheelController(actorData, pSwapChain, rangedWheel, rangedWheelName.c_str(),
		false,
		rangedWheelPos, multiplayerSize,
		activeCrimsonConfig.WeaponWheel.worldSpaceAlwaysShow, false, false,
		stateRanged, deltaTimeAdjustedSpeed, deltaTimeAdjusted);

}

void WorldSpaceWeaponWheelsController(IDXGISwapChain* pSwapChain) {
	static WeaponWheelState stateMelee[PLAYER_COUNT];
	static bool initializedMelee[PLAYER_COUNT] = { false };

	static WeaponWheelState stateRanged[PLAYER_COUNT];
	static bool initializedRanged[PLAYER_COUNT] = { false };

	if (!multiplayerWheelsLoaded) return;
	if (activeCrimsonConfig.WeaponWheel.worldSpaceWheels == "Off" ||
		(activeCrimsonConfig.WeaponWheel.worldSpaceWheels == "Only in Multiplayer" 
			&& activeConfig.Actor.playerCount <= 1)) return;

	// Measure delta time using chrono
	float deltaTime = ImGui::GetIO().DeltaTime;
	auto activeGameSpeed = (IsTurbo()) ? activeConfig.Speed.turbo : activeConfig.Speed.mainSpeed;
	float deltaTimeAdjustedSpeed = deltaTime * 1000.0f * (activeGameSpeed / g_FrameRateTimeMultiplier);
	float deltaTimeAdjusted = deltaTime * 1000.0f;

	for (int playerIndex = 1; playerIndex < activeConfig.Actor.playerCount; playerIndex++) {
		auto& playerData = GetPlayerData((uint8)playerIndex);
		auto& characterData = GetCharacterData(playerIndex, playerData.characterIndex, ENTITY::MAIN);
		auto& newActorData = GetNewActorData(playerIndex, playerData.characterIndex, ENTITY::MAIN);

		if (!newActorData.baseAddr) {
			return;
		}
		auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);

		auto& playerScreenPosition = crimsonPlayer[playerIndex].playerScreenPosition;
		auto distanceClamped = crimsonPlayer[playerIndex].cameraPlayerDistanceClamped;

		ImVec2 normalSize = ImVec2(g_renderSize.y * 0.45f, g_renderSize.y * 0.45f);
		ImVec2 baseMultiplayerSize = ImVec2(g_renderSize.y * 0.17f, g_renderSize.y * 0.17f);
		ImVec2 sizeDistance = { (baseMultiplayerSize.x * (1.0f / ((float)distanceClamped / 20))),
								(baseMultiplayerSize.y * (1.0f / ((float)distanceClamped / 20))) };
		ImVec2 multiplayerSize = initializedMelee[playerIndex] ? sizeDistance : normalSize;
		ImVec2 worldSpaceMultiplayerPos = ImVec2(playerScreenPosition.x - (sizeDistance.x / 2.0f), playerScreenPosition.y - sizeDistance.y);

		auto& meleeWheel = meleeWorldSpaceWeaponWheel[playerIndex];
		auto& rangedWheel = rangedWorldSpaceWeaponWheel[playerIndex];

		const float baseSpacing = 170.0f;
		float scaleFactor = multiplayerSize.x / normalSize.x;
		float adjustedSpacing = baseSpacing * scaleFactor;

		ImVec2 meleeWheelPos = ImVec2(worldSpaceMultiplayerPos.x + adjustedSpacing, worldSpaceMultiplayerPos.y);
		ImVec2 rangedWheelPos = ImVec2(worldSpaceMultiplayerPos.x - adjustedSpacing, worldSpaceMultiplayerPos.y);

		std::string meleeWheelName = "MeleeWheelWorldSpace " + std::to_string(playerIndex + 1);
		std::string rangedWheelName = "RangedWheelWorldSpace " + std::to_string(playerIndex + 1);

		if (WeaponWheelController(actorData, pSwapChain, meleeWheel, meleeWheelName.c_str(),
			false,
			meleeWheelPos, multiplayerSize,
			activeCrimsonConfig.WeaponWheel.worldSpaceAlwaysShow, false, true,
			stateMelee[playerIndex], deltaTimeAdjustedSpeed, deltaTimeAdjusted)) {
			initializedMelee[playerIndex] = true;
		}
	

		WeaponWheelController(actorData, pSwapChain, rangedWheel, rangedWheelName.c_str(),
			false,
			rangedWheelPos, multiplayerSize,
			activeCrimsonConfig.WeaponWheel.worldSpaceAlwaysShow, false, false,
			stateRanged[playerIndex], deltaTimeAdjustedSpeed, deltaTimeAdjusted);
	}
}


#pragma endregion

#pragma region Actor

uint8 Actor_buttonIndices[PLAYER_COUNT] = {};

uint8 Actor_collisionGroupIndices[PLAYER_COUNT] = {};

uint8 Actor_newCharacterIndices[PLAYER_COUNT][CHARACTER_COUNT][ENTITY_COUNT] = {};

uint8 Actor_styleButtonIndices[PLAYER_COUNT][CHARACTER_COUNT][ENTITY_COUNT][STYLE_COUNT] = {};
uint8 Actor_styleIndices[PLAYER_COUNT][CHARACTER_COUNT][ENTITY_COUNT][STYLE_COUNT][2] = {};

uint8 Actor_meleeWeaponIndices[PLAYER_COUNT][CHARACTER_COUNT][ENTITY_COUNT][MELEE_WEAPON_COUNT] = {};
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
					case CHARACTER::DANTE:
					{
						UpdateMapIndex(stylesDante, Actor_styleIndices[playerIndex][characterIndex][entityIndex][styleIndex][0],
							queuedCharacterData.styles[styleIndex][0]);
						UpdateMapIndex(stylesDante, Actor_styleIndices[playerIndex][characterIndex][entityIndex][styleIndex][1],
							queuedCharacterData.styles[styleIndex][1]);

						break;
					}
					case CHARACTER::VERGIL:
					{
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

void BackgroundPlayerText(uint8 playerIndex) {
	const char* playerIndexNames[PLAYER_COUNT] = { "1P", "2P", "3P", "4P" };
	std::string playerBackgroundText = std::string(playerIndexNames[playerIndex]) + " - " + activeCrimsonConfig.PlayerProperties.playerName[playerIndex];
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	ImRect wndRect = window->Rect();
	const ImVec2 tabBtnSize = { UI::g_UIContext.DefaultFontSize * 9.38f, UI::g_UIContext.DefaultFontSize * 2.4f };

	ImVec2 pos{ wndRect.Min.x + UI::g_UIContext.DefaultFontSize * 0.1f,
							wndRect.Min.y + (3.0f * tabBtnSize.y)/*Header Space*/ + UI::g_UIContext.DefaultFontSize * 1.3f };


	window->DrawList->AddText(UI::g_ImGuiFont_RussoOne256, UI::g_UIContext.DefaultFontSize * 6.6f, pos,
		UI::SwapColorEndianness(0xFFFFFF10), playerBackgroundText.c_str());
}

void Actor_CharacterTab(uint8 playerIndex, uint8 characterIndex, uint8 entityIndex, size_t defaultFontSize) {
	auto& activeCharacterData = GetActiveCharacterData(playerIndex, characterIndex, entityIndex);
	auto& queuedCharacterData = GetQueuedCharacterData(playerIndex, characterIndex, entityIndex);

	auto& activeCharacterDataClone = GetActiveCharacterData(playerIndex, characterIndex, 1);
	auto& queuedCharacterDataClone = GetQueuedCharacterData(playerIndex, characterIndex, 1);

	auto& mainActiveCharacterData = GetActiveCharacterData(playerIndex, characterIndex, ENTITY::MAIN);
	auto& mainQueuedCharacterData = GetQueuedCharacterData(playerIndex, characterIndex, ENTITY::MAIN);

	auto& playerData = GetPlayerData(playerIndex);
	auto& newActorData = GetNewActorData(playerIndex, playerData.characterIndex, ENTITY::MAIN);
	

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

			BackgroundPlayerText(playerIndex);


			if ((queuedCharacterData.character == CHARACTER::DANTE) || (queuedCharacterData.character == CHARACTER::VERGIL)) {
				ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);

				ImGui::TableNextRow(0, rowWidth);
				ImGui::TableNextColumn();
				ImGui::PushItemWidth(itemWidth);

				// Costume
				{
					bool condition = queuedCharacterData.ignoreCostume || queuedCharacterData.forceFiles;

					GUI_PushDisable(condition);

					if (queuedCharacterData.character == CHARACTER::DANTE) {
						UI::Combo2("Costume", costumeNamesDante, activeCharacterData.costume, queuedCharacterData.costume);
					} else if (queuedCharacterData.character == CHARACTER::VERGIL) {
						UI::Combo2("Costume", costumeNamesVergil, activeCharacterData.costume, queuedCharacterData.costume);
					}
					
					GUI_PopDisable(condition);

					if (GUI_Checkbox("Use In-Game Setting ", queuedCharacterData.ignoreCostume)) {
						queuedCharacterDataClone.ignoreCostume = queuedCharacterData.ignoreCostume;
					}
					ImGui::SameLine();
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

					if (queuedCharacterData.forceFilesCharacter == CHARACTER::DANTE) {
						UI::Combo2("Force Files Costume", costumeNamesDante, activeCharacterData.forceFilesCostume, queuedCharacterData.forceFilesCostume);
					} else {
						UI::Combo2("Force Files Costume", costumeNamesVergil, activeCharacterData.forceFilesCostume, queuedCharacterData.forceFilesCostume);
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

			//GUI_Slider<uint8>("", queuedCharacterData.meleeWeaponCount, 1, weaponProgression.devilArmsUnlockedQtt + 1);
			auto meleeSlider = [&]() {
				if (GUI_Slider2<uint8>("", queuedCharacterData.meleeWeaponCount,
					activeCharacterData.meleeWeaponCount, 1, weaponProgression.devilArmsUnlockedQtt + 1)) {
					if (!newActorData.baseAddr) return;
					auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);
					auto& characterData = GetCharacterData(actorData);
				}
				};

			meleeSlider();

			queuedCharacterDataClone.meleeWeaponCount = queuedCharacterData.meleeWeaponCount;

			old_for_all(uint8, meleeWeaponIndex, weaponProgression.devilArmsUnlockedQtt + 1) {
				bool condition = (meleeWeaponIndex >= queuedCharacterData.meleeWeaponCount);

				GUI_PushDisable(condition);

				if (UI::ComboMapVector2("", weaponProgression.meleeWeaponNames, weaponProgression.meleeWeaponIds,
					queuedCharacterData.meleeWeapons[meleeWeaponIndex], activeCharacterData.meleeWeapons[meleeWeaponIndex])) {

					if (!newActorData.baseAddr) break;
					auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);
					actorData.meleeWeaponIndex = queuedCharacterData.meleeWeapons[meleeWeaponIndex];
				}

// 				UI::ComboMap("", meleeWeaponNamesDante, meleeWeaponsDante,
// 					Actor_meleeWeaponIndices[playerIndex][characterIndex][entityIndex][meleeWeaponIndex],
// 					queuedCharacterData.meleeWeapons[meleeWeaponIndex]);

				// Doppelganger will now have same weapons equipped as Dante - Mia.
				queuedCharacterDataClone.meleeWeapons[meleeWeaponIndex] = queuedCharacterData.meleeWeapons[meleeWeaponIndex];

				GUI_PopDisable(condition);
			}

			ImGui::TableNextColumn();

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.0f]);
			ImGui::Text("RANGED");
			ImGui::PopFont();

			auto rangedSlider = [&]() {
				if (GUI_Slider2<uint8>("", queuedCharacterData.rangedWeaponCount,
					activeCharacterData.rangedWeaponCount, 1, weaponProgression.gunsUnlockedQtt + 1)) {
					if (!newActorData.baseAddr) return;
					auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);
					auto& characterData = GetCharacterData(actorData);
// 					// If the character is holding the weapon on last index
// 					if (actorData.rangedWeaponIndex == characterData.rangedWeapons[queuedCharacterData.rangedWeaponCount]) {
// 						auto rangedWeapon = characterData.rangedWeapons[queuedCharacterData.rangedWeaponCount]; // Update it with the penultimate index
// 						actorData.rangedWeaponIndex = rangedWeapon;
// 						queuedCharacterData.rangedWeaponIndex = rangedWeapon;
// 						activeCharacterData.rangedWeaponIndex = rangedWeapon;
// 					}
 				}
				};

			rangedSlider();

			old_for_all(uint8, rangedWeaponIndex, weaponProgression.gunsUnlockedQtt + 1) {
				bool condition = (rangedWeaponIndex >= queuedCharacterData.rangedWeaponCount);

				GUI_PushDisable(condition);

				if (UI::ComboMapVector2("", weaponProgression.rangedWeaponNames, weaponProgression.rangedWeaponIds,
					queuedCharacterData.rangedWeapons[rangedWeaponIndex], activeCharacterData.rangedWeapons[rangedWeaponIndex])) {

					if (!newActorData.baseAddr) break;
					auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);
					actorData.rangedWeaponIndex = queuedCharacterData.rangedWeapons[rangedWeaponIndex];
				}
// 				UI::ComboMapVector("", weaponProgression.rangedWeaponNames, weaponProgression.rangedWeaponIds,
// 					queuedCharacterData.rangedWeapons[rangedWeaponIndex]);


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
	

	GUI_Slider<uint8>("Number of Characters", queuedPlayerData.characterCount, 1, CHARACTER_COUNT);


	UI::ComboMap2("Switch Button", buttonNames, buttons, Actor_buttonIndices[playerIndex], activePlayerData.switchButton, queuedPlayerData.switchButton,
		ImGuiComboFlags_HeightLargest);
	ImGui::SameLine();
	TooltipHelper("(?)", "Press to Switch Loadouts or Characters.\n"
		"Hold the button while pressing L2/R2 to switch Doppelganger's weapons while it's active.\n");

	if (playerIndex != 0) {

		UI::ComboMap("Type (Collision Group)", collisionGroupNames, collisionGroups, Actor_collisionGroupIndices[playerIndex],
			queuedPlayerData.collisionGroup);

	}

	BackgroundPlayerText(playerIndex);


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

void SelectPlayerLoadoutsWeaponsTab() {
	auto defaultFontSize = UI::g_UIContext.DefaultFontSize;
	const float itemWidth = defaultFontSize * 8.0f;
	const float columnWidth = 0.8f * queuedConfig.globalScale;
	const float rowWidth = 40.0f * queuedConfig.globalScale;

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

				ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);

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

				ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);

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

}

void CharacterSection(size_t defaultFontSize) {

	const float itemWidth = defaultFontSize * 8.0f;
	const float columnWidth = 0.8f * queuedConfig.globalScale;
	const float rowWidth = 40.0f * queuedConfig.globalScale;
	ImU32 checkmarkColor = UI::SwapColorEndianness(0xFFFFFFFF);

	bool actorCondition = (!queuedConfig.Actor.enable);

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
	GUI_Checkbox("CRIMSON CHARACTER SYSTEM       ", queuedConfig.Actor.enable);
	ImGui::PopFont();

	if (!queuedConfig.Actor.enable) {
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.871f, 0.110f, 0.298f, 1.0f));
		ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
		ImGui::Text("Crimson Character System is disabled. Most Gameplay Options and Multiplayer will not work.");
		ImGui::PopStyleColor();
		ImGui::PopFont();
	}
	UI::SeparatorEx(defaultFontSize * 23.35f);
	
	GUI_PushDisable(actorCondition);

	ImGui::Text("");

	SelectPlayerLoadoutsWeaponsTab();

	GUI_PopDisable(actorCondition);

	
	GUI_PushDisable(actorCondition);

	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColor);
	

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
	const char* SPMPText = (queuedConfig.Actor.playerCount == 1) ? SPMPText = "SINGLE PLAYER" : SPMPText = "MULTIPLAYER";
	ImGui::Text(SPMPText);
	ImGui::PopFont();
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		ImRect wndRect = window->Rect();
		const ImVec2 tabBtnSize = { UI::g_UIContext.DefaultFontSize * 9.38f, UI::g_UIContext.DefaultFontSize * 2.4f };

		ImVec2 pos{ wndRect.Min.x + UI::g_UIContext.DefaultFontSize * 0.1f,
								wndRect.Min.y + (8.0f * tabBtnSize.y)/*Header Space*/ + UI::g_UIContext.DefaultFontSize * 1.3f };


		window->DrawList->AddText(UI::g_ImGuiFont_RussoOne256, UI::g_UIContext.DefaultFontSize * 6.6f, pos,
			UI::SwapColorEndianness(0xFFFFFF10), SPMPText);
	}
	

	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushItemWidth(itemWidth);
	GUI_Slider<uint8>("Number of Players", queuedConfig.Actor.playerCount, 1, PLAYER_COUNT);
	UI::Combo2("DMC3 Costume Game Progression", costumeRespectsProgressionNames, activeConfig.costumeRespectsProgression,
		queuedConfig.costumeRespectsProgression);

	ImGui::SameLine();
	TooltipHelper("(?)", "Makes DMC3 Costume update as the game progresses, as in the Vanilla game.\n"
		"\n"
		"'Crimson' updates Vergil's Costume at Mission 1."

	);

	{
		const float columnWidth = 0.6f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;

		if (ImGui::BeginTable("ActorSystemTable", 3)) {

			ImGui::TableSetupColumn("c2", 0, columnWidth * 2.0f);
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

			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("PLAYER NAMES / COLORS");
			
			for (int playerIndex = 0; playerIndex < PLAYER_COUNT; playerIndex++) {

				ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
				std::string& playerNameActive = activeCrimsonConfig.PlayerProperties.playerName[playerIndex];
				std::string& playerNameQueued = queuedCrimsonConfig.PlayerProperties.playerName[playerIndex];
				char buffer[20] = { 0 }; 
				strncpy(buffer, playerNameQueued.c_str(), sizeof(buffer) - 1);
				buffer[20 - 1] = '\0';

				std::string inputLabel = "##playerName" + std::to_string(playerIndex);
				ImGui::PushItemWidth(itemWidth * 1.3f);
				if (ImGui::InputText(inputLabel.c_str(), buffer, sizeof(buffer))) {
					playerNameActive = std::string(buffer);
					playerNameQueued = std::string(buffer);
					GUI::save = true;
				}
				ImGui::PopFont();
				ImGui::PopItemWidth();

				ImGui::SameLine();
				GUI_Color2("", activeCrimsonConfig.PlayerProperties.playerColor[playerIndex], queuedCrimsonConfig.PlayerProperties.playerColor[playerIndex]);
				ImGui::SameLine();
				ImGui::Text("%uP", playerIndex + 1);
				ImGui::SameLine();
				ImGui::SameLine();
				if (GUI_Button("D")) {
					CopyMemory(&activeCrimsonConfig.PlayerProperties.playerColor[playerIndex], &defaultCrimsonConfig.PlayerProperties.playerColor[playerIndex],
						sizeof(activeCrimsonConfig.PlayerProperties.playerColor[playerIndex]));
					CopyMemory(&queuedCrimsonConfig.PlayerProperties.playerColor[playerIndex], &defaultCrimsonConfig.PlayerProperties.playerColor[playerIndex],
						sizeof(queuedCrimsonConfig.PlayerProperties.playerColor[playerIndex]));
				}

				if (playerIndex == 1) {
					ImGui::TableNextRow(0, rowWidth);
				}
				
				ImGui::TableNextColumn();
				if (playerIndex == 0) {
					ImGui::Text("");
				}
				
				
			}
			ImGui::PopFont();

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

uint8 Arcade_modeIndex = 0;
uint8 Arcade_meleeWeaponIndexDante[2] = {};
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


	if (GUI_TitleCheckbox2("ARCADE", activeConfig.Arcade.enable, queuedConfig.Arcade.enable)) {
		Arcade::Toggle(activeConfig.Arcade.enable);
	}

	ImGui::PushItemWidth(itemWidth);

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

#pragma region MultiplayerBars

const char* barsNames[PLAYER_COUNT] = {
	"Bars1",
	"Bars2",
	"Bars3",
	"Bars4",
};

bool showBars = false;

void MirageGaugeMainPlayer() {
	if (!(activeConfig.Actor.enable && InGame() && crimsonPlayer[0].character == CHARACTER::VERGIL && !g_inGameCutscene)) {
		return;
	}
	static bool show = true;
	auto name_80 = *reinterpret_cast<byte8**>(appBaseAddr + 0xCF2680);
	if (!name_80) {
		return;
	}
	auto& hudData = *reinterpret_cast<HUDData*>(name_80);

	auto miragePoints = crimsonPlayer[0].vergilDoppelganger.miragePoints / crimsonPlayer[0].vergilDoppelganger.maxMiragePoints;
	float miragePointsColor[4] = { 1.0f , 1.0f, 1.0, hudData.topLeftAlpha / 127.0f };
	float progressBarBgColor[4] = { 0.2f , 0.2f, 0.2f, hudData.topLeftAlpha / 127.0f };

	// Base resolution (1920x1080)
	const float baseWidth = 1920.0f;
	const float baseHeight = 1080.0f;

	// Calculate scaling factors
	float widthScale = g_windowSize.x / baseWidth;
	float heightScale = g_windowSize.y / baseHeight;

	// Adjust the size of the bar
	float barLength = 130.0f * widthScale * (crimsonPlayer[0].vergilDoppelganger.maxMiragePoints / maxMiragePointsAmount);
	vec2 size = { barLength, 10.0f * heightScale };

	// Calculate position 
	float posX = 90.0f * widthScale;
	float posY = 170.0f * heightScale;

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;

	ImGui::SetNextWindowPos(ImVec2(posX, posY));

	if (ImGui::Begin("MirageMainPlayer", &show, windowFlags)) {
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, *reinterpret_cast<ImVec4*>(&miragePointsColor));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, *reinterpret_cast<ImVec4*>(&progressBarBgColor));
		ImGui::ProgressBar(miragePoints, *reinterpret_cast<ImVec2*>(&size), "");
		ImGui::PopStyleColor(2);
	}

	ImGui::End();
}

void RoyalGaugeMainPlayer() {
	if (!(activeConfig.Actor.enable && InGame() && crimsonPlayer[0].character == CHARACTER::DANTE && !g_inGameCutscene)) {
		return;
	}
	static bool show = true;
	auto name_80 = *reinterpret_cast<byte8**>(appBaseAddr + 0xCF2680);
	if (!name_80) {
		return;
	}
	auto& hudData = *reinterpret_cast<HUDData*>(name_80);

	auto pool_10222 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_10222 || !pool_10222[3]) {
		return;
	}
	auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_10222[3]);

	auto royalGauge = mainActorData.royalguardReleaseDamage;
	float miragePointsColor[4] = { 1.0f , 1.0f, 1.0, hudData.topLeftAlpha / 127.0f };
	float progressBarBgColor[4] = { 0.2f , 0.2f, 0.2f, hudData.topLeftAlpha / 127.0f };

	// Base resolution (1920x1080)
	const float baseWidth = 1920.0f;
	const float baseHeight = 1080.0f;

	// Calculate scaling factors
	float widthScale = g_windowSize.x / baseWidth;
	float heightScale = g_windowSize.y / baseHeight;

	// Adjust the size of the bar
	float barLength = 130.0f * widthScale * (crimsonPlayer[0].vergilDoppelganger.maxMiragePoints / maxMiragePointsAmount);
	vec2 size = { barLength, 10.0f * heightScale };

	// Calculate position 
	float posX = 90.0f * widthScale;
	float posY = 170.0f * heightScale;

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;

	ImGui::SetNextWindowPos(ImVec2(posX, posY));

	if (ImGui::Begin("MirageMainPlayer", &show, windowFlags)) {
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, *reinterpret_cast<ImVec4*>(&miragePointsColor));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, *reinterpret_cast<ImVec4*>(&progressBarBgColor));
		ImGui::ProgressBar(royalGauge, *reinterpret_cast<ImVec2*>(&size), "");
		ImGui::PopStyleColor(2);
	}

	ImGui::End();
}

static Texture2DD3D11* RedOrbTexture{ nullptr };

void InitRedOrbTexture(ID3D11Device* pd3dDevice) {
	RedOrbTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "Redorb.png").c_str(), pd3dDevice);
	assert(RedOrbTexture);
}

void DrawRotatedImage(ImTextureID tex_id, ImVec2 pos, ImVec2 size, float angle, ImU32 color) {
	ImVec2 center = ImVec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f);

	float cos_theta = cosf(angle);
	float sin_theta = sinf(angle);

	ImVec2 uvs[4] = {
		ImVec2(0.0f, 0.0f),
		ImVec2(1.0f, 0.0f),
		ImVec2(1.0f, 1.0f),
		ImVec2(0.0f, 1.0f)
	};

	ImVec2 points[4];
	for (int i = 0; i < 4; i++) {
		ImVec2 p = ImVec2(
			(uvs[i].x - 0.5f) * size.x,
			(uvs[i].y - 0.5f) * size.y
		);

		points[i] = ImVec2(
			cos_theta * p.x - sin_theta * p.y,
			sin_theta * p.x + cos_theta * p.y
		) + center;
	}

	ImGui::GetWindowDrawList()->AddImageQuad(
		tex_id,
		points[0], points[1], points[2], points[3],
		uvs[0], uvs[1], uvs[2], uvs[3],
		color
	);
}

void RedOrbCounterWindow(float baseWidth = 1920.0f, float baseHeight = 1080.0f) {
	assert(RedOrbTexture);

	auto name_7058 = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E30);
	if (!name_7058) {
		return;
	}
	auto& missionData = *reinterpret_cast<MissionData*>(name_7058);
	if (!(InGame() && !g_inGameCutscene)) {
		return;
	}

	auto name_80 = *reinterpret_cast<byte8**>(appBaseAddr + 0xCF2680);
	if (!name_80) {
		return;
	}
	auto& hudData = *reinterpret_cast<HUDData*>(name_80);
	auto pool_10222 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_10222 || !pool_10222[3]) {
		return;
	}
	auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_10222[3]);
	if (activeConfig.hideMainHUD || !activeCrimsonConfig.CrimsonHudAddons.redOrbCounter) {
		CrimsonDetours::RerouteRedOrbsCounterAlpha(false, crimsonHud.redOrbAlpha);
		CrimsonPatches::SetRebOrbCounterDurationTillFadeOut(false, 90);
		return;
	}

	// Set up Rerouting Alpha to our Red Orb Counter
	hudData.orbsCountAlpha = 0;
	CrimsonDetours::RerouteRedOrbsCounterAlpha(true, crimsonHud.redOrbAlpha);
	CrimsonPatches::SetRebOrbCounterDurationTillFadeOut(true, crimsonHud.redOrbAlphaDurationToAlpha);

	// Get the current display size
	ImVec2 displaySize = ImGui::GetIO().DisplaySize;
	float scaleFactorX = displaySize.x / baseWidth;
	float scaleFactorY = displaySize.y / baseHeight;

	// Define the orb count and cap it at 999999
	int orbCount = (std::min)(999999, (int)missionData.redOrbs);
	std::string orbCountStr = std::to_string(orbCount);

	// Adjust the font size and the proportional texture size
	float fontSize = 37.0f * scaleFactorY;
	float textureWidth = RedOrbTexture->GetWidth() * (fontSize / 120.0f) * activeConfig.globalScale;  // Adjust proportionally
	float textureHeight = RedOrbTexture->GetHeight() * (fontSize / 120.0f) * activeConfig.globalScale;
	float centerX = textureWidth / 2.0f;
	float centerY = textureHeight / 2.0f;

	// Define the window size and position
	ImVec2 windowSize = ImVec2(300.0f * scaleFactorX, 100.0f * scaleFactorY);
	ImVec2 windowPos = ImVec2(displaySize.x - windowSize.x - 70.0f * scaleFactorX, 30.0f * scaleFactorY);

	ImGui::SetNextWindowSize(windowSize);
	ImGui::SetNextWindowPos(windowPos);
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;

	ImGui::Begin("RedOrbWindow", nullptr, windowFlags);

	// Set the color with alpha for the Red Orb texture
	float alpha = crimsonHud.redOrbAlpha / 127.0f;
	ImColor colorWithAlpha(1.0f, 1.0f, 1.0f, alpha);

	// Adjust the text position
	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[fontSize]);
	ImVec2 textSize = ImGui::CalcTextSize(orbCountStr.c_str(), nullptr, true);
	ImVec2 textPos = ImVec2(windowSize.x - textSize.x - 74.0f * scaleFactorX, (windowSize.y - textSize.y) / 2);

	// Correct the texture position by considering the window's screen position
	ImVec2 texturePos = ImVec2(windowPos.x + textPos.x - textureWidth - 24.0f * scaleFactorX, windowPos.y + (windowSize.y - textureHeight) / 2);

	// Render the texture or a white square if the texture is not valid
	if (RedOrbTexture->IsValid()) {
// 		DrawRotatedImage(
// 			RedOrbTexture->GetTexture(),
// 			texturePos,
// 			ImVec2(textureWidth, textureHeight),
// 			IM_PI / 2.0f, // 90 degrees in radians
// 			colorWithAlpha
// 		);
		ImGui::GetWindowDrawList()->AddImage(RedOrbTexture->GetTexture(), texturePos, ImVec2(texturePos.x + textureWidth, texturePos.y + textureHeight), ImVec2(0, 0), ImVec2(1, 1), colorWithAlpha);
	}
	else {
		ImGui::GetWindowDrawList()->AddRectFilled(texturePos, ImVec2(texturePos.x + textureWidth, texturePos.y + textureHeight), ImColor(1.0f, 1.0f, 1.0f, alpha));
	}

	// Render the orb count text
	ImGui::SetCursorPos(ImVec2(textPos.x, textPos.y));
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, alpha), "%s", orbCountStr.c_str());

	ImGui::PopFont();
	ImGui::End();
}


static Texture2DD3D11* DStyleRankFillTexture{ nullptr };
static Texture2DD3D11* DStyleRankBackgroundTexture{ nullptr };

void InitDStyleRankTextures(ID3D11Device* pd3dDevice) {
	DStyleRankFillTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "Dfill.png").c_str(), pd3dDevice);
	DStyleRankBackgroundTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "Dbg.png").c_str(), pd3dDevice);
	assert(DStyleRankFillTexture);
	assert(DStyleRankBackgroundTexture);
}

void RenderMeterWithFill(ImTextureID texture, ImVec2 pos, ImVec2 size, float fillRatio, ImColor color) {
	// Ensure fillRatio is between 0.0f and 1.0f
	fillRatio = ImClamp(fillRatio, 0.0f, 1.0f);

	// Calculate the visible height based on the fill ratio
	float visibleHeight = size.y * fillRatio;

	// Adjust the position to start from the bottom
	ImVec2 fillPos = ImVec2(pos.x, pos.y + size.y - visibleHeight);

	// Define the UV coordinates for bottom-to-top filling
	ImVec2 uv0 = ImVec2(0.0f, 1.0f - fillRatio);  // Bottom-left
	ImVec2 uv1 = ImVec2(1.0f, 1.0f);              // Top-right

	// Render the texture with the adjusted UV coordinates and position
	ImGui::GetWindowDrawList()->AddImage(texture, fillPos, ImVec2(pos.x + size.x, pos.y + size.y), uv0, uv1, color);
}

void RenderMeterBackground(ImTextureID backgroundTexture, ImVec2 pos, ImVec2 size, ImColor color) {
	// Render the entire background texture as-is
	ImGui::GetWindowDrawList()->AddImage(backgroundTexture, pos, ImVec2(pos.x + size.x, pos.y + size.y), ImVec2(0,0), ImVec2(1,1), color);
}

void StyleMeterWindow() {
	assert(DStyleRankFillTexture);
	assert(DStyleRankBackgroundTexture);
	if (!DStyleRankFillTexture->IsValid() || !DStyleRankBackgroundTexture->IsValid()) {
		return;
	}
	if (!(InGame() && !g_inGameCutscene)) {
		return;
	}
	auto pool_10222 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_10222 || !pool_10222[3]) {
		return;
	}
	auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_10222[3]);
	if (activeConfig.hideMainHUD || !activeCrimsonConfig.CrimsonHudAddons.styleRanksMeter) {
		return;
	}

	auto& styleData = mainActorData.styleData;
	if (styleData.rank != 1) {
		return;
	}
	float fillRatio = styleData.meter / 700.0f;

	// 1080p base size for the window
	const float baseWidth = 1920.0f;
	const float baseHeight = 1080.0f;

	// Calculate responsive size based on the current resolution
	ImVec2 displaySize = ImGui::GetIO().DisplaySize;
	float scaleFactorX = displaySize.x / baseWidth;
	float scaleFactorY = displaySize.y / baseHeight;

	ImVec2 meterSize = ImVec2(241.0f * scaleFactorX, 243.0f * scaleFactorY);
	ImVec2 windowPos = ImVec2(1500.0f * scaleFactorX, 150.0f * scaleFactorY);
	ImColor white = { 1.0f, 1.0f, 1.0f, 1.0f };
	ImColor color = { 0.46f, 0.62f, 0.81f, 1.0f };
	ImVec2 pos = ImGui::GetCursorScreenPos();

	// Begin an ImGui window with no title bar, no resize, no background, no inputs, etc.
	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(meterSize + ImVec2(50.0f, 50.0f), ImGuiCond_Always);

	ImGui::Begin("StyleMeter", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoBackground);

	RenderMeterWithFill(DStyleRankFillTexture->GetTexture(), ImGui::GetCursorScreenPos(), meterSize, fillRatio, white);
	RenderMeterBackground(DStyleRankBackgroundTexture->GetTexture(), ImGui::GetCursorScreenPos(), meterSize, color);

	ImGui::End();
}


void RenderOutOfViewIcon(PlayerActorData actorData, SimpleVec3& screen_pos, float screenMargin, const char* name, Config::BarsData& activeData) {
	auto pool_4449 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC8FBD0);
	if (!pool_4449 || !pool_4449[147]) return;
	auto& cameraData = *reinterpret_cast<CameraData*>(pool_4449[147]);

	auto pool_12857 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_12857 || !pool_12857[3]) return;
	auto& mainActorData = *reinterpret_cast<PlayerActorDataDante*>(pool_12857[3]);

	auto playerIndex = actorData.newPlayerIndex;

	const float screenWidth = g_renderSize.x;
	const float screenHeight = g_renderSize.y;
	const float screenCenterX = screenWidth / 2.0f;
	const float screenCenterY = screenHeight / 2.0f;

	ImVec2 iconPosition;

	glm::vec3 cameraPos = { cameraData.data[0].x, cameraData.data[0].y, cameraData.data[0].z };
	glm::vec3 playerPos = { actorData.position.x, actorData.position.y, actorData.position.z };

	// Calculate the camera's forward vector based on the main actor's (1P) position
	glm::vec3 mainActorPos = { mainActorData.position.x, mainActorData.position.y, mainActorData.position.z };
	SimpleVec3 cameraForwardVec = {
		mainActorPos.x - cameraPos.x,
		mainActorPos.y - cameraPos.y,
		mainActorPos.z - cameraPos.z
};

	// Normalize the camera forward vector
	float magnitude = sqrt(cameraForwardVec.x * cameraForwardVec.x +
		cameraForwardVec.y * cameraForwardVec.y +
		cameraForwardVec.z * cameraForwardVec.z);
	cameraForwardVec.x /= magnitude;
	cameraForwardVec.y /= magnitude;
	cameraForwardVec.z /= magnitude;

	// Calculate and normalize the vector from the camera to the player
	SimpleVec3 cameraToPlayerVec = {
		playerPos.x - cameraPos.x,
		playerPos.y - cameraPos.y,
		playerPos.z - cameraPos.z
	};
	float magnitudeCameraToPlayer = sqrt(cameraToPlayerVec.x * cameraToPlayerVec.x +
		cameraToPlayerVec.y * cameraToPlayerVec.y +
		cameraToPlayerVec.z * cameraToPlayerVec.z);
	cameraToPlayerVec.x /= magnitudeCameraToPlayer;
	cameraToPlayerVec.y /= magnitudeCameraToPlayer;
	cameraToPlayerVec.z /= magnitudeCameraToPlayer;

	// Determine if the player is behind the camera
	float dotProduct = cameraToPlayerVec.x * cameraForwardVec.x +
		cameraToPlayerVec.y * cameraForwardVec.y +
		cameraToPlayerVec.z * cameraForwardVec.z;

	bool isBehindCamera = dotProduct < 0.0f;

	if (isBehindCamera) {
		float deltaX = -cameraToPlayerVec.x;
		float deltaY = -cameraToPlayerVec.y;

		// Introduce bias towards the bottom of the screen
		deltaY += screenHeight * 0.003f;

		// Determine the screen edge closest to the player's position
		float absDeltaX = fabs(deltaX);
		float absDeltaY = fabs(deltaY);

		if (absDeltaX > absDeltaY) {
			// Player is more to the left or right
			if (deltaX > 0) {
				// Right side of the screen
				iconPosition = ImVec2(screenWidth - screenMargin, screenCenterY + deltaY / absDeltaX * (screenHeight / 2.0f - screenMargin));
			}
			else {
				// Left side of the screen
				iconPosition = ImVec2(screenMargin, screenCenterY + deltaY / absDeltaX * (screenHeight / 2.0f - screenMargin));
			}
		}
		else {
			// Player is more to the top or bottom
			if (deltaY > 0) {
				// Bottom side of the screen
				iconPosition = ImVec2(screenCenterX + deltaX / absDeltaY * (screenWidth / 2.0f - screenMargin), screenHeight - screenMargin);
			}
			else {
				// Top side of the screen
				iconPosition = ImVec2(screenCenterX + deltaX / absDeltaY * (screenWidth / 2.0f - screenMargin), screenMargin);
			}
		}
	}
	else {
		// Apply a magnetic effect to pull the icon toward the edges of the screen
		float deltaX = screen_pos.x - screenCenterX;
		float deltaY = screen_pos.y - screenCenterY;
		float edgePullFactor = 0.7f;

		if (fabs(deltaX) > fabs(deltaY)) {
			// More horizontal movement
			iconPosition.x = screenCenterX + edgePullFactor * deltaX;
			iconPosition.y = screenCenterY + edgePullFactor * deltaY * (screenHeight / screenWidth);
		}
		else {
			// More vertical movement
			iconPosition.x = screenCenterX + edgePullFactor * deltaX * (screenWidth / screenHeight);
			iconPosition.y = screenCenterY + edgePullFactor * deltaY;
		}

		// Clamp icon position to screen edges
		iconPosition.x = CrimsonUtil::sexy_clamp(iconPosition.x, screenMargin, screenWidth - screenMargin);
		iconPosition.y = CrimsonUtil::sexy_clamp(iconPosition.y, screenMargin, screenHeight - screenMargin);
	}

	// Scale factor based on 1080p for responsiveness
	float baseResX = 1920.0f;
	float baseResY = 1080.0f;
	float scaleFactorX = g_renderSize.x / baseResX;
	float scaleFactorY = g_renderSize.y / baseResY;


	ImVec2 buttonSize(80.0f * scaleFactorX, 80.0f * scaleFactorY);

	// Clamp the Icon Position again to ensure Button doesn't go out of screen bounds:
	iconPosition.x = CrimsonUtil::sexy_clamp(iconPosition.x, screenMargin, screenWidth - screenMargin - buttonSize.x + 30.0f);
	iconPosition.y = CrimsonUtil::sexy_clamp(iconPosition.y, screenMargin, screenHeight - screenMargin - buttonSize.y + 30.0f);


	auto IsOverlapping = [&](ImVec2 pos1, ImVec2 pos2, ImVec2 size) {
		return !(pos1.x + size.x <= pos2.x || pos1.x >= pos2.x + size.x ||
			pos1.y + size.y <= pos2.y || pos1.y >= pos2.y + size.y);
		};

	// Check for overlap between icons and reposition if necessary
	for (int i = 0; i < crimsonHud.playerOutViewIconPositions.size(); ++i) {
		if (i == playerIndex) continue;

		const auto& previousPos = crimsonHud.playerOutViewIconPositions[i];
		if (IsOverlapping(iconPosition, previousPos, buttonSize)) {

			// Check if both icons are near the top or bottom
			if (iconPosition.y <= screenMargin || iconPosition.y >= screenHeight - screenMargin - buttonSize.y) {
				// Both icons are on top or bottom, move one to the side
				iconPosition.x += buttonSize.x + 5.0f;
				if (iconPosition.x + buttonSize.x > screenWidth - screenMargin) {
					// Wrap around if out of bounds horizontally
					iconPosition.x = screenMargin;
				}
			}
			// Check if both icons are near the left or right
			else if (iconPosition.x <= screenMargin || iconPosition.x >= screenWidth - screenMargin - buttonSize.x) {
				// Both icons are on left or right, move one up or down
				iconPosition.y += buttonSize.y + 5.0f;
				if (iconPosition.y + buttonSize.y > screenHeight - screenMargin) {
					// Wrap around if out of bounds vertically
					iconPosition.y = screenMargin;
				}
			}
		}
	}

	crimsonHud.playerOutViewIconPositions[playerIndex] = iconPosition;

	// Render the icon at the adjusted position with label and distance
	ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;

	ImGui::SetNextWindowPos(crimsonHud.playerOutViewIconPositions[playerIndex]);
	ImGui::Begin((std::string(playerIndex + "out of view icon")).c_str(), nullptr, windowFlags);

	ImGui::InvisibleButton("##button", buttonSize);

	// Draw the button manually
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	ImVec2 buttonPos = ImGui::GetItemRectMin();
	ImVec2 buttonEndPos = ImGui::GetItemRectMax();
	ImVec4 playerColor = ConvertColorFromUint8ToVec4(activeCrimsonConfig.PlayerProperties.playerColor[playerIndex]);
	drawList->AddRectFilled(buttonPos, buttonEndPos, ImGui::GetColorU32(playerColor), 10.0f * scaleFactorX);  // Background color
	drawList->AddRect(buttonPos, buttonEndPos, ImGui::GetColorU32(ImGuiCol_Border), 10.0f * scaleFactorX);  // Border

	// Calculate luminance (perceived brightness)
	float luminance = 0.299f * playerColor.x + 0.587f * playerColor.y + 0.114f * playerColor.z;
	// Choose text color based on luminance 
	ImVec4 textColor = (luminance > 0.4f) ? ImVec4(0.1f, 0.1f, 0.1f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	float minFontSize = 10.0f; // Font cannot go below 10 or else ImGui asserts will come in.
	float fontSizeDistance = (std::max)(17.0f * scaleFactorY, minFontSize);
	float fontSizeNameLabel = (std::max)(29.0f * scaleFactorY, minFontSize);

	// Draw the distance text centered inside the button
	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[fontSizeDistance]);
	ImGui::PushStyleColor(ImGuiCol_Text, textColor);
	std::string distanceText = std::to_string(static_cast<int>(crimsonPlayer[playerIndex].playerTo1PDistance));
	ImVec2 distanceTextSize = ImGui::CalcTextSize(distanceText.c_str());
	ImVec2 distanceTextPos = ImVec2(
		buttonPos.x + (buttonSize.x - distanceTextSize.x) * 0.5f,
		buttonPos.y + 5.0f * scaleFactorY
	);
	drawList->AddText(distanceTextPos, ImGui::GetColorU32(ImGuiCol_Text), distanceText.c_str());
	ImGui::PopFont();

	// Draw name label (2P, 3P, etc)
	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[fontSizeNameLabel]);
	ImVec2 nameTextSize = ImGui::CalcTextSize(name);
	ImVec2 nameTextPos = ImVec2(
		buttonPos.x + (buttonSize.x - nameTextSize.x) * 0.5f,
		buttonPos.y + (buttonSize.y - nameTextSize.y) * 0.5f
	);
	drawList->AddText(nameTextPos, ImGui::GetColorU32(ImGuiCol_Text), name);
	ImGui::PopStyleColor();
	ImGui::PopFont();

	// Calculate positions and sizes for progress bars
	ImVec2 progressBarSize = ImVec2(buttonSize.x, 7.0f * scaleFactorY);
	float progressBarPadding = 1.0f * scaleFactorY;  // Space between progress bars

	// HP Bar
	ImGui::SetCursorScreenPos(ImVec2(buttonPos.x, buttonPos.y + (buttonSize.y * 0.75f)));
	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, *reinterpret_cast<ImVec4*>(&activeData.hitColor));
	ImGui::ProgressBar(actorData.hitPoints / actorData.maxHitPoints, progressBarSize, "");
	ImGui::PopStyleColor();

	// DT Bar
	ImGui::SetCursorScreenPos(ImVec2(buttonPos.x, buttonPos.y + (buttonSize.y * 0.75f) + progressBarSize.y + progressBarPadding));
	auto& magicColor = actorData.character != CHARACTER::VERGIL ? activeData.magicColor : activeData.magicColorVergil;
	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, *reinterpret_cast<ImVec4*>(magicColor));
	ImGui::ProgressBar(actorData.magicPoints / actorData.maxMagicPoints, progressBarSize, "");
	ImGui::PopStyleColor();

	ImGui::End();
}

void RenderMultiplayerBar(
	float hitPoints, float magicPoints, const char* name, PlayerActorData& actorData) {
	if (!showBars && !activeCrimsonConfig.MultiplayerBars2D.show) {
		return;
	}

	auto playerIndex = actorData.newPlayerIndex;

	if (playerIndex == 0) {
		return;
	}
	const float alpha = ImLerp(0.27f, 1.0f, 1.0);
	float hitColor[4] = { 0.29f , 0.99f, 0.44f, 1.0f };
	float magicColor[4] = { 0.78f, 0.05f, 0.41f, 1.0f };
	float magicColorVergil[4] = { 0.06f, 0.74f, 0.81f, 1.0f };

	const float baseSpacing = 0.37f;
	const float barSpacing = g_renderSize.x * baseSpacing; // 37% of screen width per bar
	const float baseX = g_renderSize.x * (0.02f - baseSpacing); // Corner of the screen
	const float baseY = g_renderSize.y * 0.8962f; // Bottom of the screen
	

	ImVec2 pos = ImVec2(
		baseX + barSpacing * (playerIndex),
		//baseY - barSpacing * (activeConfig.Actor.playerCount - playerIndex) // Inverted
		baseY
	);

	ImGui::SetNextWindowPos(pos);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));


	ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;

	std::string label = "2DMPBar " + std::to_string(playerIndex + 1);


	if (ImGui::Begin(label.c_str(), &activeCrimsonConfig.MultiplayerBarsWorldSpace.show, windowFlags)) {

		ImVec4 playerColor = ConvertColorFromUint8ToVec4(activeCrimsonConfig.PlayerProperties.playerColor[playerIndex]);
		float luminance = 0.299f * playerColor.x + 0.587f * playerColor.y + 0.114f * playerColor.z;
		ImVec4 textColor = (luminance > 0.4f) ? ImVec4(0.1f, 0.1f, 0.1f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		
		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[18.0f]);
		ImGui::Text("  ");
		ImGui::SameLine();
		//ImGui::SameLine();
		if (actorData.character == CHARACTER::DANTE || actorData.character == CHARACTER::VERGIL) {

			if (actorData.character == CHARACTER::DANTE) {
				ImGui::Text(styleNamesDanteGameplay[actorData.style]);
			} else {
				ImGui::Text(styleNamesVergilGameplay[actorData.style]);
			}
		}
		ImGui::PopFont();

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, alpha));
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, *reinterpret_cast<ImVec4*>(&hitColor));
		ImGui::ProgressBar(hitPoints, ImVec2(200, 10), "");
		ImGui::PopStyleColor(2);

		if (actorData.character != CHARACTER::VERGIL) {
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, *reinterpret_cast<ImVec4*>(&magicColor));
		} else {
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, *reinterpret_cast<ImVec4*>(&magicColorVergil));
		}
		ImGui::ProgressBar(magicPoints, ImVec2(200, 10), "");
		ImGui::PopStyleColor();

		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[18.0 * 1.1f]);
		ImGui::PushStyleColor(ImGuiCol_Button, playerColor);
		ImGui::PushStyleColor(ImGuiCol_Text, textColor);
		ImGui::Button(name, { 30.0f, 30.0f });
		ImGui::PopStyleColor(2);
		ImGui::PopFont();
		ImGui::SameLine(0.0f, 0);
		ImGui::Text("  ");

		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[22.0f]);
		ImGui::SameLine();
		ImGui::Text(activeCrimsonConfig.PlayerProperties.playerName[playerIndex].c_str());
		// 		ImGui::SameLine();
		// 		ImGui::Text(" -");
		ImGui::PopFont();
	}

	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(4);
}

void Render1PAttributes(const char* name, PlayerActorData& actorData) {
	if (!showBars && !activeCrimsonConfig.MultiplayerBars2D.show) {
		return;
	}

	auto playerIndex = actorData.newPlayerIndex;

	if (playerIndex != 0) {
		return;
	}

	if ((activeCrimsonConfig.MultiplayerBars2D.show1PAttributes == "Only in Multiplayer" 
		&& activeConfig.Actor.playerCount == 1) ||
		(activeCrimsonConfig.MultiplayerBars2D.show1PAttributes == "Off")) {
		return;
	}

	const float baseX = g_renderSize.x * (0.3272f); 
	const float baseY = g_renderSize.y * 0.015f; 

	ImVec2 pos = ImVec2(baseX,baseY);

	ImGui::SetNextWindowPos(pos);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));


	ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;

	std::string label = "1PAttributes " + std::to_string(playerIndex + 1);


	if (ImGui::Begin(label.c_str(), &activeCrimsonConfig.MultiplayerBarsWorldSpace.show, windowFlags)) {

		ImVec4 playerColor = ConvertColorFromUint8ToVec4(activeCrimsonConfig.PlayerProperties.playerColor[playerIndex]);
		float luminance = 0.299f * playerColor.x + 0.587f * playerColor.y + 0.114f * playerColor.z;
		ImVec4 textColor = (luminance > 0.4f) ? ImVec4(0.1f, 0.1f, 0.1f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[18.0 * 1.1f]);
		ImGui::PushStyleColor(ImGuiCol_Button, playerColor);
		ImGui::PushStyleColor(ImGuiCol_Text, textColor);
		ImGui::Button(name, { 30.0f, 30.0f });
		ImGui::PopStyleColor(2);
		ImGui::PopFont();
		ImGui::SameLine(0.0f, 0);
		ImGui::Text("  ");

		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[22.0f]);
		ImGui::SameLine();
		ImGui::Text(activeCrimsonConfig.PlayerProperties.playerName[playerIndex].c_str());
		// 		ImGui::SameLine();
		// 		ImGui::Text(" -");
		ImGui::PopFont();
	}

	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(4);
}


void RenderWorldSpaceMultiplayerBar(
	float hitPoints, float magicPoints, const char* name, const PlayerActorData& actorData, const char* label, Config::BarsData& activeData/*, Config::BarsData& queuedData*/) {
	if (!showBars && !activeCrimsonConfig.MultiplayerBarsWorldSpace.show) {
		return;
	}

	auto playerIndex = actorData.newPlayerIndex;
	auto distanceClamped = crimsonPlayer[playerIndex].cameraPlayerDistanceClamped;

	// Adjusts size dynamically based on the distance between Camera and Player
	ImVec2 sizeDistance = { (activeData.size.x * (1.0f / ((float)distanceClamped / 20))), (activeData.size.y * (1.0f / ((float)distanceClamped / 20))) };
	auto& playerScreenPosition = crimsonPlayer[playerIndex].playerScreenPosition;
	float screenWidth = g_renderSize.x;
	float screenHeight = g_renderSize.y;

	const float screenMargin = 50.0f;

	const float t = CrimsonUtil::smoothstep(0.0f, 1390.0f, crimsonPlayer[playerIndex].cameraPlayerDistance);
	const float alpha = ImLerp(0.27f, 1.0f, t);
	activeData.hitColor[3] = alpha;
	activeData.magicColor[3] = alpha;
	activeData.magicColorVergil[3] = alpha;

	// If player is outside view: Handle Out of View Icons and stop rendering WorldSpace Bars
	if (playerScreenPosition.x < screenMargin || playerScreenPosition.x > screenWidth - screenMargin ||
		playerScreenPosition.y < screenMargin || playerScreenPosition.y > screenHeight - screenMargin) {


		if (activeCrimsonConfig.MultiplayerBarsWorldSpace.showOutOfViewIcons) {
			RenderOutOfViewIcon(actorData, playerScreenPosition, screenMargin, name, activeData);
		}

		return;
	}

	ImGui::SetNextWindowPos(ImVec2(playerScreenPosition.x - (sizeDistance.x / 2.0f), playerScreenPosition.y - sizeDistance.y));

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));


	ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;


	if (ImGui::Begin(label, &activeCrimsonConfig.MultiplayerBarsWorldSpace.show, windowFlags)) {
		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[18.0 * 1.1f]);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, alpha));
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, *reinterpret_cast<ImVec4*>(&activeData.hitColor));
		ImGui::ProgressBar(hitPoints, sizeDistance, "");
		ImGui::PopStyleColor(2);

		if (actorData.character != CHARACTER::VERGIL) {
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, *reinterpret_cast<ImVec4*>(&activeData.magicColor));
		} else {
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, *reinterpret_cast<ImVec4*>(&activeData.magicColorVergil));
		}
		ImGui::ProgressBar(magicPoints, sizeDistance, "");
		ImGui::PopStyleColor();

		ImVec4 playerColor = ConvertColorFromUint8ToVec4(activeCrimsonConfig.PlayerProperties.playerColor[playerIndex]);

		float luminance = 0.299f * playerColor.x + 0.587f * playerColor.y + 0.114f * playerColor.z;
		ImVec4 textColor = (luminance > 0.4f) ? ImVec4(0.1f, 0.1f, 0.1f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

		ImGui::PushStyleColor(ImGuiCol_Button, playerColor);
		ImGui::PushStyleColor(ImGuiCol_Text, textColor);
		ImGui::Button(name, { 30.0f, 30.0f });
		ImGui::PopStyleColor(2);
		ImGui::PopFont();

		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[18.0f]);
		ImGui::SameLine();
		if (actorData.character == CHARACTER::DANTE || actorData.character == CHARACTER::VERGIL) {
			ImGui::Text("  ");
			ImGui::SameLine(0.0f, 0);

			if (actorData.character == CHARACTER::DANTE) {
				ImGui::Text(styleNamesDanteGameplay[actorData.style]);
			} else {
				ImGui::Text(styleNamesVergilGameplay[actorData.style]);
			}
		}
		ImGui::PopFont();

		if (activeCrimsonConfig.MultiplayerBarsWorldSpace.showPlayerNames) {
			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[11.0f]);
			ImGui::Text(activeCrimsonConfig.PlayerProperties.playerName[playerIndex].c_str());
			ImGui::PopFont();
		}

	}

	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(4);
}

void MultiplayerBars(IDXGISwapChain* pSwapChain) {
	if (!showBars && !(activeConfig.Actor.enable && InGame())) {
		return;
	}

	if (InCutscene() || InCredits() || !activeConfig.Actor.enable || g_inGameCutscene ) {
		return;
	}

	uint8 playerCount = (showBars) ? PLAYER_COUNT : activeConfig.Actor.playerCount;

	int minimum = 1;



	old_for_all(uint8, playerIndex, playerCount) {

		if (activeCrimsonConfig.MultiplayerBarsWorldSpace.show1PBar == "Off") {
			minimum = 1;
		}
		else if ((activeCrimsonConfig.MultiplayerBarsWorldSpace.show1PBar == "Only in Multiplayer" 
			&& activeConfig.Actor.playerCount > 1) ||
			(activeCrimsonConfig.MultiplayerBarsWorldSpace.show1PBar == "Always")) {
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
				auto playerIndex = activeActorData.newPlayerIndex;

				hit = (activeActorData.hitPoints / activeActorData.maxHitPoints);
				magic = (activeActorData.magicPoints / activeActorData.maxMagicPoints);

				//                 activeConfig.barsData[playerIndex].pos.x = queuedConfig.barsData[playerIndex].pos.x;
				//                 activeConfig.barsData[playerIndex].pos.y = queuedConfig.barsData[playerIndex].pos.y;

				RenderWorldSpaceMultiplayerBar(hit, magic, playerIndexNames[playerIndex], 
					activeActorData, barsNames[playerIndex], activeConfig.barsData[playerIndex]);

				//WorldSpaceWeaponWheelsController2P(activeActorData, pSwapChain);
				}();
		}

		float hit = 0.75f;
		float magic = 0.5f;

		auto& playerData = GetPlayerData(playerIndex);

		auto& characterData = GetCharacterData(playerIndex, playerData.characterIndex, ENTITY::MAIN);
		auto& newActorData = GetNewActorData(playerIndex, playerData.characterIndex, ENTITY::MAIN);

		auto& activeCharacterData = GetCharacterData(playerIndex, playerData.activeCharacterIndex, ENTITY::MAIN);
		auto& activeNewActorData = GetNewActorData(playerIndex, playerData.activeCharacterIndex, ENTITY::MAIN);

		if (!activeNewActorData.baseAddr) {
			return;
		}
		auto& activeActorData = *reinterpret_cast<PlayerActorData*>(activeNewActorData.baseAddr);

		hit = (activeActorData.hitPoints / activeActorData.maxHitPoints);
		magic = (activeActorData.magicPoints / activeActorData.maxMagicPoints);
		RenderMultiplayerBar(hit, magic, playerIndexNames[playerIndex],
			activeActorData);

		Render1PAttributes(playerIndexNames[0], activeActorData);
	}
}

void BarsSettings(size_t defaultFontSize) {

	ImGui::Text("");

	const float itemWidth = defaultFontSize * 6.0f;
	const float columnWidth = 0.5f * queuedConfig.globalScale;
	const float rowWidth = 20.0f * queuedConfig.globalScale;
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
			auto& queuedPos = *reinterpret_cast<ImVec2*>(&queuedConfig.barsData[i].pos);
			auto& defaultPos = *reinterpret_cast<ImVec2*>(&defaultConfig.barsData[i].pos);

			ImGui::PushItemWidth(itemWidth);
			if (GUI_Input2("X", activePos.x, queuedPos.x, 10.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue)) {

				ImGui::SetWindowPos(barsNames[i], activePos);
			}
			ImGui::SameLine();
			if (GUI_Input2("Y", activePos.y, queuedPos.y, 10.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue)) {
				ImGui::SetWindowPos(barsNames[i], activePos);
			}
			ImGui::SameLine();
			if (GUI_Button("D")) {
				CopyMemory(&activePos, &defaultPos, sizeof(activePos));
				CopyMemory(&queuedPos, &defaultPos, sizeof(queuedPos));

				ImGui::SetWindowPos(barsNames[i], activePos);

			}
			ImGui::PopItemWidth();

		}



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

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;


		if (ImGui::BeginTable("MultiplayerBarsOptionsTable", 2)) {

			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableNextRow(0, rowWidth * 0.5f);
			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			GUI_TitleCheckbox2("2D MULTIPLAYER BARS", activeCrimsonConfig.MultiplayerBars2D.show, 
				queuedCrimsonConfig.MultiplayerBars2D.show, true, defaultFontSize * 46.70f);
			
			ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);


			ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);

			ImGui::PushItemWidth(itemWidth);
			UI::Combo2Vector("Show 1P Attributes", show1PNames,
				activeCrimsonConfig.MultiplayerBars2D.show1PAttributes,
				queuedCrimsonConfig.MultiplayerBars2D.show1PAttributes);
			ImGui::PopItemWidth();

			ImGui::PopStyleColor();
			ImGui::PopFont();


			ImGui::TableNextColumn();

			GUI_TitleCheckbox2("WORLD SPACE MULTIPLAYER BARS", activeCrimsonConfig.MultiplayerBarsWorldSpace.show,
				queuedCrimsonConfig.MultiplayerBarsWorldSpace.show, true);

			ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

			ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);

			ImGui::PushItemWidth(itemWidth);
			UI::Combo2Vector("Show 1P Bar", show1PNames,
				activeCrimsonConfig.MultiplayerBarsWorldSpace.show1PBar,
				queuedCrimsonConfig.MultiplayerBarsWorldSpace.show1PBar);
			ImGui::PopItemWidth();

			GUI_Checkbox2("Out of View Icons",
				activeCrimsonConfig.MultiplayerBarsWorldSpace.showOutOfViewIcons,
				queuedCrimsonConfig.MultiplayerBarsWorldSpace.showOutOfViewIcons);
			GUI_Checkbox2("Show Player Names",
				activeCrimsonConfig.MultiplayerBarsWorldSpace.showPlayerNames,
				queuedCrimsonConfig.MultiplayerBarsWorldSpace.showPlayerNames);

			ImGui::PopFont();

			ImGui::PopStyleColor();
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::EndTable();
		}
	}
	//BarsSettings(defaultFontSize);


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

	float smallerComboMult = 0.7f;

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;

		if (ImGui::BeginTable("CameraOptionsTable", 3)) {

			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * 0.7f);
			GUI_InputDefault2("FOV Multiplier", activeCrimsonConfig.Camera.fovMultiplier, queuedCrimsonConfig.Camera.fovMultiplier, defaultCrimsonConfig.Camera.fovMultiplier, 0.1f, "%g",
				ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * 1.1f);
			UI::Combo2("Sensitivity", cameraSensitivityNames, activeCrimsonConfig.Camera.sensitivity, queuedCrimsonConfig.Camera.sensitivity);
			ImGui::PopItemWidth();

			
			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * 1.1f);
			UI::Combo2("Follow-Up Speed", cameraFollowUpSpeedNames, activeCrimsonConfig.Camera.followUpSpeed, queuedCrimsonConfig.Camera.followUpSpeed);
			ImGui::PopItemWidth();

			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			GUI_PushDisable(activeCrimsonConfig.Camera.multiplayerCamera || activeCrimsonConfig.Camera.panoramicCamera);
			ImGui::PushItemWidth(itemWidth * 1.1f);
			UI::Combo2("Distance", cameraDistanceNames, activeCrimsonConfig.Camera.distance, queuedCrimsonConfig.Camera.distance);
			ImGui::SameLine();
			TooltipHelper("(?)", "Camera Distance relative to the player outside Lock-On.\n"
				"\n"
				"Dynamic Option adjusts based on whether player is airborne.");
			ImGui::PopItemWidth();


			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * 1.1f);
			UI::Combo2(
				"LockOn Distance", cameraLockOnDistanceNames, activeCrimsonConfig.Camera.lockOnDistance, queuedCrimsonConfig.Camera.lockOnDistance);
			ImGui::SameLine();
			TooltipHelper("(?)", "Camera Distance relative to the player in Lock-On.\n"
				"\n"
				"Dynamic Option adjusts based on whether player is airborne.");
			ImGui::PopItemWidth();
			GUI_PopDisable(activeCrimsonConfig.Camera.multiplayerCamera || activeCrimsonConfig.Camera.panoramicCamera);

			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * 1.1f);
			UI::Combo2("Vertical Tilt", cameraTiltNames, activeCrimsonConfig.Camera.tilt, queuedCrimsonConfig.Camera.tilt);
			ImGui::PopItemWidth();

			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			UI::Combo2<uint8>("Auto-Adjust", cameraAutoAdjustNames, activeCrimsonConfig.Camera.autoAdjust, queuedCrimsonConfig.Camera.autoAdjust);
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			GUI_Checkbox2("Locked-Off Camera", activeCrimsonConfig.Camera.lockedOff, queuedCrimsonConfig.Camera.lockedOff);
			ImGui::SameLine();
			TooltipHelper("(?)", "Allows you to freely rotate the camera using the right stick in Third-Person View sections.");

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("Invert X", activeCrimsonConfig.Camera.invertX, queuedCrimsonConfig.Camera.invertX)) {
				Camera::ToggleInvertX(activeCrimsonConfig.Camera.invertX);
			}

			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			GUI_Checkbox2("Disable Right Stick Center Camera", activeCrimsonConfig.Camera.disableRightStickCenterCamera, queuedCrimsonConfig.Camera.disableRightStickCenterCamera);

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("Disable Boss Camera", activeCrimsonConfig.Camera.disableBossCamera, queuedCrimsonConfig.Camera.disableBossCamera)) {
				Camera::ToggleDisableBossCamera(activeCrimsonConfig.Camera.disableBossCamera);
			}

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("[WIP] Force Third Person Camera", activeCrimsonConfig.Camera.forceThirdPerson, queuedCrimsonConfig.Camera.forceThirdPerson)) {
				
			}

			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			if (GUI_Checkbox2("Multiplayer Camera", activeCrimsonConfig.Camera.multiplayerCamera, queuedCrimsonConfig.Camera.multiplayerCamera)) {
				if (activeCrimsonConfig.Camera.multiplayerCamera) {
					activeCrimsonConfig.Camera.distance = 2;
					queuedCrimsonConfig.Camera.distance = 2;

					activeCrimsonConfig.Camera.lockOnDistance = 2;
					queuedCrimsonConfig.Camera.lockOnDistance = 2;
				}
			}
			ImGui::SameLine();
			TooltipHelper("(?)", "Triggers only in Multiplayer or if you spawn Doppelganger. Works best with Full Force Third Person Camera.");

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("Panoramic Camera", activeCrimsonConfig.Camera.panoramicCamera, queuedCrimsonConfig.Camera.panoramicCamera)) {
				if (activeCrimsonConfig.Camera.multiplayerCamera) {
					activeCrimsonConfig.Camera.distance = 2;
					queuedCrimsonConfig.Camera.distance = 2;

					activeCrimsonConfig.Camera.lockOnDistance = 2;
					queuedCrimsonConfig.Camera.lockOnDistance = 2;
				}
			}
			ImGui::SameLine();
			TooltipHelper("(?)", "Multiplayer-like Combat Camera in Single Player. Works best with Full Force Third Person Camera.");


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

			if (ImGui::BeginTable("LiveCameraReadingsTable", 3)) {

				ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
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

#pragma region Damage

void DamageSection() {
	auto defaultFontSize = UI::g_UIContext.DefaultFontSize;
	const float itemWidth = defaultFontSize * 8.0f;
	float smallerComboMult = 0.7f;
	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);
	//Gui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);
	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
	if (GUI_Checkbox2("CUSTOM DAMAGE", activeCrimsonConfig.Cheats.General.customDamage, queuedCrimsonConfig.Cheats.General.customDamage)) {
		if (!activeCrimsonConfig.Cheats.General.customDamage) {
			// Reset damage config to default when the checkbox is unchecked
			CopyMemory(&queuedConfig.damagePlayerActorMultiplier, &defaultConfig.damagePlayerActorMultiplier, sizeof(queuedConfig.damagePlayerActorMultiplier));
			CopyMemory(&activeConfig.damagePlayerActorMultiplier, &queuedConfig.damagePlayerActorMultiplier, sizeof(activeConfig.damagePlayerActorMultiplier));

			CopyMemory(&queuedConfig.damageEnemyActorMultiplier, &defaultConfig.damageEnemyActorMultiplier, sizeof(queuedConfig.damageEnemyActorMultiplier));
			CopyMemory(&activeConfig.damageEnemyActorMultiplier, &queuedConfig.damageEnemyActorMultiplier, sizeof(activeConfig.damageEnemyActorMultiplier));

			CopyMemory(&queuedConfig.damageStyleRank, &defaultConfig.damageStyleRank, sizeof(queuedConfig.damageStyleRank));
			CopyMemory(&activeConfig.damageStyleRank, &queuedConfig.damageStyleRank, sizeof(activeConfig.damageStyleRank));
		}
	}
	ImGui::PopFont();
	UI::SeparatorEx(defaultFontSize * 23.35f);

	ImGui::Text("");

	auto DamageDataInput = [](float& active, float& queued, float& defaultVar) {
		auto defaultFontSize = UI::g_UIContext.DefaultFontSize;
		const float itemWidth = defaultFontSize * 8.0f;
		const float rowWidth = 40.0f * queuedConfig.globalScale * 0.5f;
		float smallerComboMult = 0.7f;

		ImGui::PushItemWidth(itemWidth * smallerComboMult);
		GUI_InputDefault2("", active, queued, defaultVar, 0.1f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::PopItemWidth();
		};

	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);
	{
		// Get initial cursor position for manual layout
		ImVec2 initialPos = ImGui::GetCursorPos();
		const float columnWidth = queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale * 0.5f;
		GUI_PushDisable(!activeCrimsonConfig.Cheats.General.customDamage);

		ImGui::SetCursorPos(initialPos);
		// Start first table (left side)
		if (ImGui::BeginTable("DamageOptionsTable", 3)) {
			ImGui::TableSetupColumn("b1", 0, columnWidth);
			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("PLAYER RECEIVED DAMAGE MULT.");
			ImGui::PopFont();
			DamageDataInput(activeConfig.damagePlayerActorMultiplier, queuedConfig.damagePlayerActorMultiplier, defaultConfig.damagePlayerActorMultiplier);

			ImGui::TableNextColumn();

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("ENEMY RECEIVED DAMAGE MULT.");
			ImGui::PopFont();
			DamageDataInput(activeConfig.damageEnemyActorMultiplier, queuedConfig.damageEnemyActorMultiplier, defaultConfig.damageEnemyActorMultiplier);
			ImGui::SameLine();
			{
				static bool toggled = false;
				if (GUI_Button("One Hit Kill")) {

					if (!toggled) {
						toggled = true;
						activeConfig.damageEnemyActorMultiplier = queuedConfig.damageEnemyActorMultiplier = 100.0f;
					}
					else {
						toggled = false;
						activeConfig.damageEnemyActorMultiplier = queuedConfig.damageEnemyActorMultiplier = defaultConfig.damageEnemyActorMultiplier;
					}
				}
			}

			ImGui::TableNextColumn();

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("CAUSE DAMAGE ONLY ON STYLE RANK");
			ImGui::PopFont();
			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			UI::Combo2("", styleRankNames, activeConfig.damageStyleRank, queuedConfig.damageStyleRank);
			ImGui::PopItemWidth();

			GUI_PopDisable(!activeCrimsonConfig.Cheats.General.customDamage);

			ImGui::EndTable();
		}
	}

	
	ImGui::PopStyleColor();
	ImGui::PopFont();
	ImGui::Text("");
}

#pragma endregion

#pragma region Dante

// @Todo: Move.
template <typename T>
void ActionData(const char* label, T(&vars)[2], T(&vars2)[2], T(&defaultVars)[2], T step = 1, const char* format = 0,
	ImGuiInputTextFlags flags = 0, float32 width = 150) {
	auto& style = ImGui::GetStyle();

	ImGui::PushItemWidth(width);
	GUI_InputDefault2("", vars[0], vars2[0], defaultVars[0], step, format, flags);
	ImGui::SameLine(0, style.ItemInnerSpacing.x);
	GUI_InputDefault2(label, vars[1], vars2[1], defaultVars[1], step, format, flags);
	ImGui::PopItemWidth();
}

#pragma endregion

#pragma region Shop

namespace Shop {

const char* tabNames[] = {
	"Dante",
	"Vergil",
	"Items",
};

struct ShopExperienceHelper {
	const char* name;
	uint32 price;
	int64 last;
	int64 next;
};

ShopExperienceHelper shopHelpersDante[] = {
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

ShopExperienceHelper shopHelpersVergil[] = {
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


struct ShopItemHelper {
	const uint8 itemIndex;
	const uint8 buyIndex;
	const uint8 maxItemCount;
	const uint32* prices;
	const uint8 priceCount;
};


constexpr ShopItemHelper itemHelpers[] = {
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

// Function Declarations
void ShowExperienceTab(ExpConfig::ExpData& expData, ShopExperienceHelper* helpers, new_size_t helperCount, MissionData& missionData);
void ShowItemTab(MissionData& missionData, QueuedMissionActorData& queuedMissionActorData, ActiveMissionActorData& activeMissionActorData, bool unlockDevilTrigger);
void HandleItemPurchase(uint8 itemHelperIndex, MissionData& missionData, ActiveMissionActorData& activeMissionActorData, uint32 price);
void HandleItemSale(uint8 itemHelperIndex, MissionData& missionData, ActiveMissionActorData& activeMissionActorData);
uint32 GetItemPrice(const ShopItemHelper& itemHelper, uint8 buyCount);

void ShopWindow() {
	if (!g_showShop) {
		return;
	}

	auto missionDataPtr = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E30);
	if (!missionDataPtr) {
		return;
	}
	auto& missionData = *reinterpret_cast<MissionData*>(missionDataPtr);
	auto& queuedMissionActorData = *reinterpret_cast<QueuedMissionActorData*>(missionDataPtr + 0xC0);
	auto& activeMissionActorData = *reinterpret_cast<ActiveMissionActorData*>(missionDataPtr + 0x16C);

	bool unlockDevilTrigger = (activeMissionActorData.maxMagicPoints >= 3000);

	static bool run = false;
	if (!run) {
		// TODO: Play Divine Statue Track - Mia
		//PlayTrack("afs/sound/Jikushinzou.ogg");
		run = true;
		constexpr float width = 800; // 900
		constexpr float height = 900; // 500

		ImGui::SetNextWindowSize(ImVec2(width, height));
		ImGui::SetNextWindowPos(ImVec2(((g_renderSize.x - width) / 2), ((g_renderSize.y - height) / 2)));
	}


	if (ImGui::Begin("ShopWindow", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize)) {
		if (GUI_Button("Close")) {
			CloseShop();
		}
		ImGui::Text("");
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[UI::g_UIContext.DefaultFontSize * 2.0f]);
		constexpr auto BULLET = u8"•";
		ImGui::Text((const char*)BULLET);
		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::Text("%u", missionData.redOrbs);
		ImGui::PopFont();
		ImGui::Text("");

// 		for (uint8 playerIndex = 0; playerIndex < activeConfig.Actor.playerCount; ++playerIndex) {
// 			auto& gamepad = GetGamepad(playerIndex);
// 
// 			if ((gamepad.buttons[0] & GetBinding(BINDING::STYLE_ACTION))) {
// 				CloseShop();
// 			}
// 		}

		SelectPlayerLoadoutsWeaponsTab();

		if (ImGui::BeginTabBar("ShopTabs")) {
			for (uint8 tabIndex = 0; tabIndex < TAB::COUNT; ++tabIndex) {
				if (ImGui::BeginTabItem(tabNames[tabIndex])) {
					ImGui::Text("");

					switch (tabIndex) {
					case TAB::DANTE:
						ShowExperienceTab(ExpConfig::missionExpDataDante, shopHelpersDante, sizeof(shopHelpersDante) / sizeof(ShopExperienceHelper), missionData);
						break;

					case TAB::VERGIL:
						ShowExperienceTab(ExpConfig::missionExpDataVergil, shopHelpersVergil, sizeof(shopHelpersVergil) / sizeof(ShopExperienceHelper), missionData);
						break;

					case TAB::ITEMS:
						ShowItemTab(missionData, queuedMissionActorData, activeMissionActorData, unlockDevilTrigger);
						break;
					}

					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
		}

		ImGui::End();
	}
}

void ShowStyleLevelsTab(ExpConfig::ExpData& expData, MissionData& missionData) {
// 	auto Buy = [&]() {
// 		if (missionData.redOrbs < helper.price) {
// 			PlaySound(0, 19);
// 			return;
// 		}
// 
// 		missionData.redOrbs -= helper.price;
// 		PlaySound(0, 18);
// 		expData.unlocks[helperIndex] = true;
// 		ExpConfig::UpdatePlayerActorExps();
// 		};
// 
// 	if (GUI_Button("Swordmaster Level 2", ImVec2(500.0f, 80.0f))) {
// 		Buy();
// 	}
// 
// 	GUI_PopDisable(condition);
// 
// 	// Display price and bullet point in a new column
// 	ImGui::SameLine(120);
// 
// 	bool priceCondition = (missionData.redOrbs < helper.price);
// 	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
// 	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[UI::g_UIContext.DefaultFontSize * 1.5f]);
// 	constexpr auto BULLET = u8"•";
// 	ImGui::Text((const char*)BULLET);
// 	ImGui::PopFont();
// 	ImGui::PopStyleColor();
// 
// 	GUI_PushDisable(priceCondition);
// 	ImGui::SameLine();
// 	ImGui::Text("%u", helper.price);
// 	GUI_PopDisable(priceCondition);
}


void ShowExperienceTab(ExpConfig::ExpData& expData, ShopExperienceHelper* helpers, new_size_t helperCount, MissionData& missionData) {
	auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

	for (size_t helperIndex = 0; helperIndex < helperCount; ++helperIndex) {
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

			// Sets the flag off on sessionData expertise to also update non Actor System
// 			const auto& expertiseHelper =
// 				(sessionData.character == CHARACTER::DANTE)
// 				? ExpConfig::expertiseHelpersDante[helperIndex]
// 				: ExpConfig::expertiseHelpersVergil[helperIndex];
// 
// 			sessionData.expertise[expertiseHelper.index] -= expertiseHelper.flags;
			ExpConfig::UpdatePlayerActorExps();
			};

		bool condition = (expData.unlocks[helperIndex] || ((helper.last > -1) && !expData.unlocks[helper.last]));
		GUI_PushDisable(condition);

		// Begin a new row
		ImGui::BeginGroup();

		if (GUI_Button(helper.name, ImVec2(500.0f, 80.0f))) {
			Buy();
		}

		GUI_PopDisable(condition);

		// Display price and bullet point in a new column
		ImGui::SameLine(120); 

		bool priceCondition = (missionData.redOrbs < helper.price);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[UI::g_UIContext.DefaultFontSize * 1.5f]);
		constexpr auto BULLET = u8"•";
		ImGui::Text((const char*)BULLET);
		ImGui::PopFont();
		ImGui::PopStyleColor();

		GUI_PushDisable(priceCondition);
		ImGui::SameLine();
		ImGui::Text("%u", helper.price);
		GUI_PopDisable(priceCondition);

		// Display the sell button in a new column

		if (expData.unlocks[helperIndex]) {
			ImGui::SameLine(550);
			condition = ((helper.next > -1) && expData.unlocks[helper.next]);
			GUI_PushDisable(condition);

			if (GUI_Button("Sell")) {
				Sell();
			}
			GUI_PopDisable(condition);
		}

		ImGui::EndGroup(); // End the group for the current row
		ImGui::Spacing(); // Add spacing between rows
	}
}


void ShowItemTab(MissionData& missionData, QueuedMissionActorData& queuedMissionActorData, ActiveMissionActorData& activeMissionActorData, bool unlockDevilTrigger) {
	for (int itemHelperIndex = 0; itemHelperIndex < sizeof(itemHelpers) / sizeof(ShopItemHelper); ++itemHelperIndex) {
		auto& itemHelper = itemHelpers[itemHelperIndex];

		uint8& itemCount = missionData.itemCounts[itemHelper.itemIndex];
		uint8& buyCount = missionData.buyCounts[itemHelper.buyIndex];

		if (itemHelper.itemIndex == ITEM::PURPLE_ORB && !unlockDevilTrigger) {
			continue;
		}

		uint32 price = GetItemPrice(itemHelper, buyCount);

		if (GUI_Button(itemNames[itemHelper.itemIndex], ImVec2(500.0f, 80.0f))) {
			HandleItemPurchase(itemHelperIndex, missionData, activeMissionActorData, price);
		}
		ImGui::SameLine(120);

		bool priceCondition = (missionData.redOrbs < price);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[UI::g_UIContext.DefaultFontSize * 1.5f]);
		constexpr auto BULLET = u8"•";
		ImGui::Text((const char*)BULLET);
		ImGui::PopFont();
		ImGui::PopStyleColor();

		GUI_PushDisable(priceCondition);
		ImGui::SameLine();
		ImGui::Text("%u", price);
		GUI_PopDisable(priceCondition);

		ImGui::SameLine(300);

		bool quantityCondition = (itemCount >= itemHelper.maxItemCount);
		GUI_PushDisable(quantityCondition);
		ImGui::Text("x%02u / %02u", itemCount, itemHelper.maxItemCount);
		GUI_PopDisable(quantityCondition);


		if (!(itemHelper.itemIndex == ITEM::BLUE_ORB || itemHelper.itemIndex == ITEM::PURPLE_ORB) && itemCount != 0) {
			ImGui::SameLine(550);
			if (GUI_Button("Sell")) {
				HandleItemSale(itemHelperIndex, missionData, activeMissionActorData);
			}
		}
	}
}

void UpdateStatsAfterPurchase(const ShopItemHelper& itemHelper, ActiveMissionActorData& activeMissionActorData) {
	auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

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

		activeMissionActorData.hitPoints = activeMissionActorData.maxHitPoints = sessionData.hitPoints = value;

		for (uint8 playerIndex = 0; playerIndex < PLAYER_COUNT; ++playerIndex) {
			for (uint8 characterIndex = 0; characterIndex < CHARACTER_COUNT; ++characterIndex) {
				for (uint8 entityIndex = 0; entityIndex < ENTITY_COUNT; ++entityIndex) {
					auto& newActorData = GetNewActorData(playerIndex, characterIndex, entityIndex);

					if (!newActorData.baseAddr) {
						continue;
					}
					auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);

					actorData.hitPoints = actorData.maxHitPoints = value;
				}
			}
		}
	}
	else if (itemHelper.itemIndex == ITEM::PURPLE_ORB) {
		float value = (activeMissionActorData.maxMagicPoints + 500);

		[&]() {
			if (!g_defaultNewActorData[0].baseAddr) {
				return;
			}
			auto& actorData = *reinterpret_cast<PlayerActorData*>(g_defaultNewActorData[0].baseAddr);
			value = (actorData.maxMagicPoints + 500);
			actorData.magicPoints = actorData.maxMagicPoints = value;
		}();

		activeMissionActorData.magicPoints = activeMissionActorData.maxMagicPoints = sessionData.magicPoints = value;

		for (uint8 playerIndex = 0; playerIndex < PLAYER_COUNT; ++playerIndex) {
			for (uint8 characterIndex = 0; characterIndex < CHARACTER_COUNT; ++characterIndex) {
				for (uint8 entityIndex = 0; entityIndex < ENTITY_COUNT; ++entityIndex) {
					auto& newActorData = GetNewActorData(playerIndex, characterIndex, entityIndex);

					if (!newActorData.baseAddr) {
						continue;
					}
					auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);

					activeMissionActorData.magicPoints = activeMissionActorData.maxMagicPoints = value;
				}
			}
		}
	}
}

void HandleItemPurchase(uint8 itemHelperIndex, MissionData& missionData, ActiveMissionActorData& activeMissionActorData, uint32 price) {
	auto& itemHelper = itemHelpers[itemHelperIndex];

	uint8& itemCount = missionData.itemCounts[itemHelper.itemIndex];
	uint8& buyCount = missionData.buyCounts[itemHelper.buyIndex];

	if ((itemCount >= itemHelper.maxItemCount) || (missionData.redOrbs < price)) {
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

	UpdateStatsAfterPurchase(itemHelper, activeMissionActorData);
}

void HandleItemSale(uint8 itemHelperIndex, MissionData& missionData, ActiveMissionActorData& activeMissionActorData) {
	auto& itemHelper = itemHelpers[itemHelperIndex];

	uint8& itemCount = missionData.itemCounts[itemHelper.itemIndex];
	uint8& buyCount = missionData.buyCounts[itemHelper.buyIndex];
	uint32 price = 0;

	itemCount--;
	if (buyCount > 0) {
		buyCount--;
	}

	price = GetItemPrice(itemHelper, 0);

	missionData.redOrbs += price;
	PlaySound(0, 18);
}

uint32 GetItemPrice(const ShopItemHelper& itemHelper, uint8 buyCount) {
	if (buyCount >= itemHelper.priceCount) {
		return itemHelper.prices[itemHelper.priceCount - 1];
	}
	return itemHelper.prices[buyCount];
}

void CloseShop() {
	g_showShop = false;
	if (g_scene == SCENE::GAME) {
		PlayTrack("");
	}
}
}

#pragma endregion

#pragma region DebugSection

bool showExpWindow = false;
bool showMissionDataWindow = false;
bool showFileDataWindow = false;
bool showActorWindow = false;
bool showEventDataWindow = false;
bool showRegionDataWindow = false;
bool showSoundWindow = false;


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


        auto FunctionOnce = [&](ExpConfig::ExpData& expData, const char* name, Shop::ShopExperienceHelper* shopHelpers, new_size_t count) {
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

			GUI_Checkbox("hasPairedWithActorSystem", expData.hasPairedWithActorSystem);
			ImGui::Text("");

            ImGui::PopItemWidth();
        };


        auto FunctionLoop = [&](ExpConfig::ExpData* expDataAddr, const char** saveNames, Shop::ShopExperienceHelper* shopHelpers, new_size_t count) {
            for_all(saveIndex, SAVE_COUNT) {
                auto& expData = expDataAddr[saveIndex];

                FunctionOnce(expData, saveNames[saveIndex], shopHelpers, count);
            }
        };


        FunctionOnce(ExpConfig::missionExpDataDante, "Dante Mission", Shop::shopHelpersDante, countof(Shop::shopHelpersDante));
        FunctionOnce(ExpConfig::sessionExpDataDante, "Dante Session", Shop::shopHelpersDante, countof(Shop::shopHelpersDante));
        FunctionLoop(ExpConfig::savedExpDataDante, saveNamesDante, Shop::shopHelpersDante, countof(Shop::shopHelpersDante));
        ImGui::Text("");


        FunctionOnce(ExpConfig::missionExpDataVergil, "Vergil Mission", Shop::shopHelpersVergil, countof(Shop::shopHelpersVergil));
        FunctionOnce(ExpConfig::sessionExpDataVergil, "Vergil Session", Shop::shopHelpersVergil, countof(Shop::shopHelpersVergil));
        FunctionLoop(ExpConfig::savedExpDataVergil, saveNamesVergil, Shop::shopHelpersVergil, countof(Shop::shopHelpersVergil));
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


void DebugSection() {
	auto defaultFontSize = UI::g_UIContext.DefaultFontSize;
	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);
	const float itemWidth = defaultFontSize * 8.0f;
	ImGui::PushItemWidth(itemWidth);
	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	GUI_Checkbox2("Debug Overlay", activeConfig.debugOverlayData.enable, queuedConfig.debugOverlayData.enable);

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
	if (ImGui::CollapsingHeader("DEBUG")) {

		ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
		

		ImGui::Text("");

		if (ImGui::Button("heheh")) {
			CrimsonDetours::SampleModDetour1();
		}

		// if (ImGui::Button("EnableCrazyComboHook")){
		//	HoldToCrazyComboHook->Toggle(true);
		// }

		// Move these variables if you wanted
		static int vfxBank = 0;
		static int vfxId = 0;
		static int boneIdx = 0;
		static bool enableCustomColor = false;

		ImGui::InputInt("Effect Bank", &vfxBank);
		ImGui::InputInt("Effect ID", &vfxId);
		ImGui::InputInt("Effect Bone", &boneIdx);
		static int effectPlayerID = 0;
		ImGui::InputInt("Player", &effectPlayerID);
		ImGui::Checkbox("CustomColor", &enableCustomColor);
		auto pPlayer = (void*)crimsonPlayer[effectPlayerID].playerPtr;
		uint32 vfxColor = CrimsonUtil::Uint8toAABBGGRR(activeCrimsonConfig.StyleSwitchFX.Flux.color[0]);
		if (ImGui::Button("CreateEffect")) {
			CrimsonDetours::CreateEffectDetour(pPlayer, vfxBank, vfxId, boneIdx, enableCustomColor, vfxColor, 1);
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
			showActorWindow = false;
			showEventDataWindow = false;
			showExpWindow = false;
			showFileDataWindow = false;
			showMissionDataWindow = false;
			showRegionDataWindow = false;
			showSoundWindow = false;
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
			static ModelData* modelDataAddr = 0;
			static BodyPartData* bodyPartDataAddr = 0;
			static NewArchiveMetadata* archiveAddr = 0;

			static uint16 cacheFileIndex = em034;

			static char buffer[8];
			static byte8* file = 0;
			static uint32 fileIndex = 0;

			static char subbuffer[8];
			static byte8* subfile = 0;
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
			static vec4 __declspec(align(16)) position = {};
			static vec4 __declspec(align(16)) multiplier = {};
			static __m128 __declspec(align(16)) height = {};


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
			static uint32 event = 0;
			static uint8 action = 0;
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

		ImGui::PopFont();

		
	}

	ImGui::PopFont();


	ImGui::PopItemWidth();
	ImGui::PopFont();
	ImGui::PopStyleColor();
	ImGui::Text("");
    
}

#pragma endregion

#pragma region Enemy

void EnemySpawnerSection() {
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

				actorData.event = EVENT_BOSS_LADY::DEATH;
				actorData.state = 0;
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

#pragma endregion

#pragma region Jukebox

void JukeboxSection() {
	auto defaultFontSize = UI::g_UIContext.DefaultFontSize;
	const float itemWidth = defaultFontSize * 8.0f;
	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

// 	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
// 	ImGui::Text("JUKEBOX");
// 	ImGui::PopFont();
// 	UI::SeparatorEx(defaultFontSize * 23.35f);
	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);
	ImGui::PushItemWidth(itemWidth);

	static char location[512];
	static char fileName[256];
	static new_size_t index = 0;
	static bool run = false;

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

	ImGui::PopItemWidth();
	ImGui::PopStyleColor();
	ImGui::PopFont();

	ImGui::Text("");
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

void CustomMobilitySection() {
	auto defaultFontSize = UI::g_UIContext.DefaultFontSize;
	const float itemWidth = defaultFontSize * 8.0f;
	float smallerComboMult = 0.7f;
	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);
	GUI_PushDisable(!activeConfig.Actor.enable);
	//Gui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);
	ImGui::PushItemWidth(itemWidth);

	if (GUI_TitleCheckbox2("CUSTOM MOBILITY", activeCrimsonConfig.Cheats.General.customMobility, 
		queuedCrimsonConfig.Cheats.General.customMobility, true)) {
		if (!activeCrimsonConfig.Cheats.General.customMobility) {
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
	}
	

	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);

	auto MobilityDataInput = [](const char* label, uint8(&active)[2], uint8(&queued)[2], uint8(&defaultVar)[2]) {
		auto defaultFontSize = UI::g_UIContext.DefaultFontSize;
		const float itemWidth = defaultFontSize * 8.0f;
		const float rowWidth = 40.0f * queuedConfig.globalScale * 0.5f;
		float smallerComboMult = 0.7f;

		for (size_t i = 0; i < 2; ++i) {
			if (i == 0) {
				ImGui::TableNextRow(0, rowWidth);
				ImGui::TableNextColumn();
			}

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_InputDefault2("", active[i], queued[i], defaultVar[i], (uint8)1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

			if (i == 0) {
				ImGui::TableNextColumn();
			}
			else {
				ImGui::SameLine();
				ImGui::Text(label);
			}
		}
		};

	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);
	{
		// Get initial cursor position for manual layout
		ImVec2 initialPos = ImGui::GetCursorPos();
		const float columnWidth = 0.15f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale * 0.5f;
		GUI_PushDisable(!activeCrimsonConfig.Cheats.General.customMobility);

		ImGui::SetCursorPos(initialPos);
		// Start first table (left side)
		if (ImGui::BeginTable("MobilityOptionsTable1", 2)) {
			ImGui::TableSetupColumn("b1", 0, columnWidth);
			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("HUMAN");
			ImGui::TableNextColumn();
			ImGui::Text("DEVIL TRIGGER");
			ImGui::PopFont();

			// First half of mobility options
			MobilityDataInput("Air Hike Count", activeConfig.airHikeCount, queuedConfig.airHikeCount, defaultConfig.airHikeCount);
			MobilityDataInput("Kick Jump Count", activeConfig.kickJumpCount, queuedConfig.kickJumpCount, defaultConfig.kickJumpCount);
			MobilityDataInput("Wall Hike Count", activeConfig.wallHikeCount, queuedConfig.wallHikeCount, defaultConfig.wallHikeCount);
			MobilityDataInput("Dash Count", activeConfig.dashCount, queuedConfig.dashCount, defaultConfig.dashCount);
			MobilityDataInput("Sky Star Count", activeConfig.skyStarCount, queuedConfig.skyStarCount, defaultConfig.skyStarCount);

			ImGui::EndTable();
		}

		// Add some space between the tables
		ImGui::SetCursorPos(ImVec2(initialPos.x + (420 * queuedConfig.globalScale), initialPos.y)); 

		// Start second table (right side)
		if (ImGui::BeginTable("MobilityOptionsTable2", 2)) {
			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.3f);
			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();


			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("HUMAN");
			ImGui::TableNextColumn();
			ImGui::Text("DEVIL TRIGGER");
			ImGui::PopFont();

			// Second half of mobility options
			MobilityDataInput("Air Trick Count Dante", activeConfig.airTrickCountDante, queuedConfig.airTrickCountDante, defaultConfig.airTrickCountDante);
			MobilityDataInput("Air Trick Count Vergil", activeConfig.airTrickCountVergil, queuedConfig.airTrickCountVergil, defaultConfig.airTrickCountVergil);
			MobilityDataInput("Trick Up Count", activeConfig.trickUpCount, queuedConfig.trickUpCount, defaultConfig.trickUpCount);
			MobilityDataInput("Trick Down Count", activeConfig.trickDownCount, queuedConfig.trickDownCount, defaultConfig.trickDownCount);

			ImGui::EndTable();
		}

		GUI_PopDisable(!activeCrimsonConfig.Cheats.General.customMobility);
	}
	
	//GUI_InputDefault2("", vars[0], vars2[0], defaultVars[0], step, format, flags);
	GUI_PopDisable(!activeConfig.Actor.enable);
	ImGui::PopItemWidth();
	ImGui::PopStyleColor();
	ImGui::PopFont();
	ImGui::Text("");
}

#pragma endregion

#pragma region Other

const char* dotShadowNames[] = {"Default", "Disable", "Disable Player Actors Only"};


void Other() {
    if (ImGui::CollapsingHeader("Other")) {
        ImGui::Text("");

        ImGui::PushItemWidth(200);


        GUI_InputDefault2("Deplete Quicksilver", activeConfig.depleteQuicksilver, queuedConfig.depleteQuicksilver,
            defaultConfig.depleteQuicksilver, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
        GUI_InputDefault2("Deplete Doppelganger", activeConfig.depleteDoppelganger, queuedConfig.depleteDoppelganger,
            defaultConfig.depleteDoppelganger, 1.0f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
        GUI_InputDefault2("Deplete Devil", activeConfig.depleteDevil, queuedConfig.depleteDevil, defaultConfig.depleteDevil, 1.0f, "%g",
            ImGuiInputTextFlags_EnterReturnsTrue);

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


const char* mainOverlayLabel = "DebugOverlay";

void DebugOverlayWindow(size_t defaultFontSize) {
    
    auto Function = [&]() {
        if (activeConfig.debugOverlayData.showFocus) {
            auto color = ImVec4(0, 1, 0, 1);
            if (GetForegroundWindow() != appWindow) {
                color = ImVec4(1, 0, 0, 1);
            }
            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::Text("Focus");
            ImGui::PopStyleColor();
        }


        if (activeConfig.debugOverlayData.showFPS) {
            ImGui::Text("%.2f FPS", ImGui::GetIO().Framerate);
        }

        if (activeConfig.debugOverlayData.showSizes) {
            ImGui::Text("g_windowSize %g %g", g_windowSize.x, g_windowSize.y);
            ImGui::Text("g_clientSize %g %g", g_clientSize.x, g_clientSize.y);
            ImGui::Text("g_renderSize %g %g", g_renderSize.x, g_renderSize.y);
        }


        if (activeConfig.debugOverlayData.showFrameRateMultiplier) {
            ImGui::Text("g_frameRateMultiplier %g", g_frameRateMultiplier);
			ImGui::Text("g_FrameRateTimeMultiplier %g", g_FrameRateTimeMultiplier);
        }

        if (activeConfig.debugOverlayData.showFocus || activeConfig.debugOverlayData.showFPS || activeConfig.debugOverlayData.showSizes ||
            activeConfig.debugOverlayData.showFrameRateMultiplier) {
            ImGui::Text("");
        }

        if (activeConfig.debugOverlayData.showEventData) {
            if (g_scene >= SCENE::COUNT) {
                ImGui::Text("Unknown");
            } else {
                auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);
				auto& gamepad = GetGamepad(0);
                

                ImGui::Text(sceneNames[g_scene]);
                ImGui::Text("sessionData mission:  %u", sessionData.mission);
                ImGui::Text("SCENE:  %u", g_scene);
                ImGui::Text("TRACK PLAYING: %s", g_gameTrackPlaying.c_str());
// 				for (int i = 0; i < 8; i++) {
// 					ImGui::Text("sessionData expertise[%u]:  %x", i, sessionData.expertise[i]);
// 				}
// 
// 				for (int i = 0; i < 5; i++) {
// 					ImGui::Text("sessionData rangedWeaponLevels[%u]:  %x", i, sessionData.rangedWeaponLevels[i]);
// 				}

//                 for (int i = 0; i < 14; i++) {
//                     ImGui::Text("sessionData unlock[%u] : %u", i, sessionData.weaponStyleUnlocks[i]);
//                 }
                ImGui::Text("SessionData Style Level Royalguard: %u", ExpConfig::missionExpDataDante.styleLevels[3]);
				ImGui::Text("Cerbus Unlocked Session? %u", sessionData.weaponAndStyleUnlocks[WEAPONANDSTYLEUNLOCKS::CERBERUS]);
				ImGui::Text("Cerbus Unlock? %u", weaponProgression.devilArmUnlocks[DEVILARMUNLOCKS::CERBERUS]);
				ImGui::Text("GunUnlockedQtt: %u", weaponProgression.gunsUnlockedQtt);
				ImGui::Text("activeCharacterData.rangedWeaponCount: %u", queuedConfig.Actor.playerData[0].characterData[0][0].rangedWeaponCount);
// 				for (int i = 0; i < weaponProgression.rangedWeaponIds.size(); i++) {
// 					ImGui::Text("RangedWeaponId[%u]: %u", i, weaponProgression.rangedWeaponIds[i]);
// 				}
				for (int i = 0; i < weaponProgression.rangedWeaponIds.size(); i++) {
					ImGui::Text("RangedWeaponName[%u]: %s", i, weaponProgression.rangedWeaponNames[i]);
				}
				for (size_t i = 0; i < queuedConfig.Actor.playerData[0].characterData[0][0].rangedWeaponCount; i++) {
					ImGui::Text("RangedWeaponQeued[%u]: %u", i, queuedConfig.Actor.playerData[0].characterData[0][0].rangedWeapons[i]);
				} 
				
                ImGui::Text("Gamepad Style Button: %u", gamepad.buttons[0] & GetBinding(BINDING::STYLE_ACTION));
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


                if (CrimsonSDL::IsMusicPlaying() == 0) {
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

        if (activeConfig.debugOverlayData.showPosition) {
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

            auto name_80 = *reinterpret_cast<byte8**>(appBaseAddr + 0xCF2680);
            if (!name_80) {
                return;
            }
            auto& hudData = *reinterpret_cast<HUDData*>(name_80);

// 			if (!actorData.lockOnData.targetBaseAddr60) {
// 				return;
// 			}
// 
// 			auto& lockedOnEnemyData = *reinterpret_cast<EnemyActorData*>(actorData.lockOnData.targetBaseAddr60);

			ImGui::Text("Enemy Health %g", actorData.lockOnData.targetPosition.z);

//             for (int i = 0; i < 8; i++) {
//                 ImGui::Text("expertise[%u]:  %x", i, actorData.activeExpertise[i]);
//             }
// 
// 			for (int i = 0; i < 10; i++) {
// 				ImGui::Text("actorData rangedWeaponLevels[%u]:  %x", i, actorData.newWeaponLevels[i]);
// 			}
			auto pool_4449 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC8FBD0);
			if (!pool_4449 || !pool_4449[147]) return;
			auto& cameraData = *reinterpret_cast<CameraData*>(pool_4449[147]); // 0x498 = 1176 / 8 = 147
			auto& cameraControlMetadata = *reinterpret_cast<CameraControlMetadata*>(pool_4449);

			auto& costume = activeConfig.Actor.playerData[0].characterData[0]->costume;

// 			auto& enemy = enemyVectorData.metadata[0];
// 			if (!enemy.baseAddr) return;
// 			auto& enemyData = *reinterpret_cast<EnemyActorData*>(enemy.baseAddr);
// 			if (!enemyData.baseAddr) return;
			
            // crazyComboHold = g_HoldToCrazyComboFuncA();
			if (meleeWeaponWheel[0]) {
				ImGui::Text("Wheel 0 timer: %g", meleeWeaponWheel[0]->m_SinceLatestChangeMs);
			}
			if (meleeWeaponWheel[1]) {
				ImGui::Text("Wheel 1 timer: %g", meleeWeaponWheel[1]->m_SinceLatestChangeMs);
			}
			ImGui::Text("fixedCameraAddr: %x", cameraControlMetadata.fixedCameraAddr);
			ImGui::Text("Starting From Ground: %u", crimsonPlayer[0].vergilMoves.startingRisingSunFromGround);
// 			ImGui::Text("Enemy Base Addr: %x", enemyData.baseAddr);
// 			ImGui::Text("Enemy's Target Pos Y: %g", enemyData.targetPosition.y);
			ImGui::Text("AllActorsSpawned: %u", g_allActorsSpawned);
			ImGui::Text("costume: %u", costume);
			ImGui::Text("NextActionPolicyTrickster: %u", actorData.nextActionRequestPolicy[NEXT_ACTION_REQUEST_POLICY::TRICKSTER_DARK_SLAYER]);
			ImGui::Text("NextActionPolicyMelee: %u", actorData.nextActionRequestPolicy[NEXT_ACTION_REQUEST_POLICY::MELEE_ATTACK]);
			ImGui::Text("BufferedAction: %u", actorData.bufferedAction);
			ImGui::Text("Air Trick Count : %u", actorData.newAirTrickCount);
			ImGui::Text("rainstorm cached inertia:  %g", crimsonPlayer[0].inertia.rainstorm.cachedPull);
			ImGui::Text("Horizontal Pull  %g", actorData.horizontalPull);
			ImGui::Text("Horizontal Pull Multiplier %g", actorData.horizontalPullMultiplier);
			ImGui::Text("Vertical Pull  %g", actorData.verticalPull);
			ImGui::Text("Vertical Pull Multiplier %g", actorData.verticalPullMultiplier);
			ImGui::Text("AIR SWORD COUNT: %u", actorData.airSwordAttackCount);
			ImGui::Text("Back Buffer: %g", crimsonPlayer[0].b2F.backBuffer);
			ImGui::Text("Forward Buffer: %g", crimsonPlayer[0].b2F.forwardBuffer);
			ImGui::Text("Forward Command: %u", crimsonPlayer[0].b2F.forwardCommand);
			ImGui::Text("Back Buffer Clone: %g", crimsonPlayer[0].b2FClone.backBuffer);
			ImGui::Text("Forward Buffer Clone: %g", crimsonPlayer[0].b2FClone.forwardBuffer);
			ImGui::Text("Forward Command Clone: %u", crimsonPlayer[0].b2FClone.forwardCommand);
			ImGui::Text("Royal Block: %u", actorData.royalBlock);
			ImGui::Text("Guard: %u", actorData.guard);
			ImGui::Text("maxDT: %g", actorData.maxMagicPoints);
			ImGui::Text("Motion Data 1: %u", crimsonPlayer[0].motion);
			ImGui::Text("Event Data 1 %u", actorData.eventData[0]);
			ImGui::Text("anim Timer Main Actor:  %g", crimsonPlayer[0].animTimer);
			ImGui::Text("anim Timer Clone:  %g", crimsonPlayer[0].animTimerClone);
			ImGui::Text("action Timer Main Actor:  %g", crimsonPlayer[0].actionTimer);
			ImGui::Text("action Timer Clone:  %g", crimsonPlayer[0].actionTimerClone);
			ImGui::Text("AIR GUARD: %u", actorData.airGuard);
			ImGui::Text("GUARDFLY PULL: %g", crimsonPlayer[0].inertia.airGuard.cachedPull);
			ImGui::Text("GUARDFLY TIMER: %g", crimsonPlayer[0].inertia.guardflyTimer);
			ImGui::Text("actorCameraDirection: %u", actorData.actorCameraDirection);
			ImGui::Text("Rotation Offset: %u", actorData.rotationOffset);
			ImGui::Text("Rotation Towards Enemy2: %u", crimsonPlayer[0].rotationTowardsEnemy);
			ImGui::Text("isMPActive:  %u", g_isMPCamActive);
			ImGui::Text("isPanoramicCamActive:  %u", g_isParanoramicCamActive);
			ImGui::Text("CAMERA LAG:  %g", cameraData.cameraLag);
			ImGui::Text("LOCK ON CAM:  %u", cameraData.transitionToLockOnCam);
			ImGui::Text("ACTOR SPEED:  %g", actorData.speed);
			ImGui::Text("ALL ENTITY COUNT:  %u", g_activeAllEntitiesCount);
			ImGui::Text("PLAYABLE ENTITY COUNT:  %u", g_activePlayableEntitiesCount);
			ImGui::Text("avgPos x: %g", g_customCameraPos[0]);
			ImGui::Text("avgPos y: %g", g_customCameraPos[1]);
			ImGui::Text("avgPos z: %g", g_customCameraPos[2]);
			ImGui::Text("RED ORB ALPHA:  %u", crimsonHud.redOrbAlpha);
			ImGui::Text("SKY LAUNCH EXECUTING:  %u", crimsonPlayer[0].skyLaunch.executing);
			ImGui::Text("ROYAL RELEASE EXECUTING:  %u", crimsonPlayer[0].royalRelease.executing);
			ImGui::Text("STORED RELEASE LEVEL:  %u", crimsonPlayer[0].skyLaunch.storedReleaseLevel);
			ImGui::Text("ROYAL GAUGE:  %g", actorData.royalguardReleaseDamage);
			ImGui::Text("ACTION:  %u", actorData.action);
            ImGui::Text("action Timer Main Actor:  %g", crimsonPlayer[0].actionTimer);
            ImGui::Text("DTE CHARGE: %g", actorData.dtExplosionCharge);
            ImGui::Text("DTE CHARGE: %g", crimsonPlayer[0].dtExplosionCharge);
            // ImGui::Text("crazy combo hold:  %u", crazyComboHold);
            ImGui::Text("Chain Count (weight):  %u", actorData.airSwordAttackCount);
            ImGui::Text("Air Guard:  %u", actorData.airGuard);
            // ImGui::Text("Gravity Tweak:  %g", crimsonPlayer[0].airRaveTweak.gravity);
            // ImGui::Text("drive timer:  %g", crimsonPlayer[0].drive.timer);
            // ImGui::Text("Actor Speed %g", actorData.speed);
            ImGui::Text("TOP LEFT ALPHA: %u", hudData.topLeftAlpha);
            ImGui::Text("distance: %u", crimsonPlayer[0].cameraPlayerDistanceClamped);
            ImGui::Text("Release Damage: %g", actorData.royalguardReleaseDamage);
			ImGui::Text("ACTOR STATUS: %u", actorData.status);
            ImGui::Text("MIRAGE GAUGE: %g", crimsonPlayer[0].vergilDoppelganger.miragePoints);
            ImGui::Text("Vergil Dopp Timer Start: %u", crimsonPlayer[0].vergilDoppelganger.drainStart);
            ImGui::Text("Vergil Dopp Timer: %g", crimsonPlayer[0].vergilDoppelganger.drainTime);
            ImGui::Text("HP: %g", actorData.hitPoints);
            ImGui::Text("DT: %g", actorData.magicPoints);
            ImGui::Text("storedHP: %g", storedHP);
            ImGui::Text("storedDT: %g", storedDT);
            ImGui::Text("missionData DAMAGE: %u", missionData.damage);
            ImGui::Text("missionData FRAME: %u", missionData.frameCount);
            ImGui::Text("IN ROYAL BLOCK: %u", ((actorData.eventData[0].event == 20 && actorData.motionData[0].index == 32) || (actorData.eventData[0].event == 20 && actorData.motionData[0].index == 34)));
            ImGui::Text("IN NORMAL BLOCK: %u", ((actorData.eventData[0].event == 20 && actorData.motionData[0].index == 2) || (actorData.eventData[0].event == 20 && actorData.motionData[0].index == 7) || (actorData.eventData[0].event == 20 && actorData.motionData[0].index == 12)));
            ImGui::Text("IN GUARD: %u", actorData.guard);
            ImGui::Text("IN COMBAT: %u", g_inCombat);
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
            ImGui::Text("Last Event Data %u", actorData.eventData[0].lastEvent);
            ImGui::Text("Last Last Event %u", crimsonPlayer[0].lastLastEvent);
            ImGui::Text("State %u", actorData.state);
            ImGui::Text("Last State %u", actorData.lastState);
            ImGui::Text("Last Last State %u", crimsonPlayer[0].lastLastState);
            ImGui::Text("Character Action %u", crimsonPlayer[0].action);
            ImGui::Text("Character Last Action %u", actorData.lastAction);
            ImGui::Text("Position  %g", actorData.position);
            ImGui::Text("Rotation %g", actorData.rotation);
            ImGui::Text("Camera Direction %u", actorData.cameraDirection);
            ImGui::Text("Actor Camera Direction %u", actorData.actorCameraDirection);
            ImGui::Text("RelativeTilt %u", relativeTiltController);
            ImGui::Text("LeftStick Position %u", gamepad.leftStickPosition);
		
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
            ImGui::Text("StyleRank Divident %g", actorData.styleData.dividend);
            ImGui::Text("StyleRank Divisor %g", actorData.styleData.divisor);
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


        if (activeConfig.debugOverlayData.showRegionData) {
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

//         if constexpr (!debug) {
//             return;
//         }

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

    
    OverlayFunction(mainOverlayLabel, activeConfig.debugOverlayData, queuedConfig.debugOverlayData, Function);
}

void MainOverlaySettings() {
    auto Function = [&]() {
        GUI_Checkbox2("Show Focus", activeConfig.debugOverlayData.showFocus, queuedConfig.debugOverlayData.showFocus);
        GUI_Checkbox2("Show FPS", activeConfig.debugOverlayData.showFPS, queuedConfig.debugOverlayData.showFPS);
        GUI_Checkbox2("Show Sizes", activeConfig.debugOverlayData.showSizes, queuedConfig.debugOverlayData.showSizes);
        GUI_Checkbox2("Show Frame Rate Multiplier", activeConfig.debugOverlayData.showFrameRateMultiplier,
            queuedConfig.debugOverlayData.showFrameRateMultiplier);
        GUI_Checkbox2("Show Event Data", activeConfig.debugOverlayData.showEventData, queuedConfig.debugOverlayData.showEventData);
        GUI_Checkbox2("Show Position", activeConfig.debugOverlayData.showPosition, queuedConfig.debugOverlayData.showPosition);
        GUI_Checkbox2("Show Region Data", activeConfig.debugOverlayData.showRegionData, queuedConfig.debugOverlayData.showRegionData);
    };

    OverlaySettings(mainOverlayLabel, activeConfig.debugOverlayData, queuedConfig.debugOverlayData, defaultConfig.debugOverlayData, Function);
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

ImVec4 GetCrimsonGUIBgColor(float opacity = 1.0f) {
	return ImVec4(0.169f, 0.125f, 0.133f, opacity);
}

void AdjustBackgroundColorAndTransparency() {
	// previously ImVec4(0.207f, 0.156f, 0.168f
   	
    switch (queuedCrimsonConfig.GUI.transparencyMode) {
        // OFF
    case 0 :
        ImGui::PushStyleColor(ImGuiCol_WindowBg, GetCrimsonGUIBgColor());
        break;
        
        // STATIC
    case 1 :
        ImGui::PushStyleColor(ImGuiCol_WindowBg, GetCrimsonGUIBgColor(queuedCrimsonConfig.GUI.opacity));
        break;

        //DYNAMIC
    case 2 :
        if (g_inGameDelayed) {
            ImGui::PushStyleColor(ImGuiCol_WindowBg, GetCrimsonGUIBgColor(queuedCrimsonConfig.GUI.opacity));
        }
		else {
			ImGui::PushStyleColor(ImGuiCol_WindowBg, GetCrimsonGUIBgColor());
		}
        
        break;
    }

}


void InterfaceSection(size_t defaultFontSize) {
    
	const float itemWidth = defaultFontSize * 8.0f;

    ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);


    ImGui::Text("CRIMSON GUI OPTIONS");

	ImGui::PopFont();

	UI::SeparatorEx(defaultFontSize * 23.35f);


	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	ImGui::PushItemWidth(itemWidth);
	ImGui::Text("");


	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;
		
		if (ImGui::BeginTable("GUIOptiomsTable", 3)) {

			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableNextRow(0, rowWidth * 0.5f);
			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * 0.6f);
			UI::Combo2("Transparency Mode", GUITransparencyNames, activeCrimsonConfig.GUI.transparencyMode, queuedCrimsonConfig.GUI.transparencyMode);
			ImGui::PopItemWidth();
			ImGui::SameLine();
			TooltipHelper("(?)", "Dynamic will apply transparency only in-game.");

			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * 0.8f);
			GUI_InputDefault2<float>("Opacity", activeCrimsonConfig.GUI.opacity, queuedCrimsonConfig.GUI.opacity,defaultCrimsonConfig.GUI.opacity, 0.1f, "%g",
				ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();


			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * 0.8f);
			if (GUI_InputDefault2("Global Scale", activeConfig.globalScale, queuedConfig.globalScale, defaultConfig.globalScale, 0.1f, "%g",
				ImGuiInputTextFlags_EnterReturnsTrue)) {
				UpdateGlobalScale();
			}
			ImGui::PopItemWidth();

			ImGui::TableNextRow(0, rowWidth * 0.5f);
			ImGui::TableNextColumn();

			if (GUI_Checkbox2("Pause When Opened", activeCrimsonConfig.GUI.pauseWhenOpened, queuedCrimsonConfig.GUI.pauseWhenOpened)) {
				if (g_inGameDelayed) {
					activeConfig.Speed.mainSpeed = queuedConfig.Speed.mainSpeed; // This resumes the game speed
					activeConfig.Speed.turbo = queuedConfig.Speed.turbo;
					Speed::Toggle(true); // Toggle Speed on and off to set the new speed
					Speed::Toggle(false);
				}
			}

			ImGui::TableNextColumn();

			GUI_Checkbox2("GUI Sounds", 
				activeCrimsonConfig.GUI.sounds, queuedCrimsonConfig.GUI.sounds);
	

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


		if (ImGui::BeginTable("HUDOptiomsTable", 3)) {

			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableNextRow(0, rowWidth * 0.5f);
			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth);
			if (UI::Combo2Vector("Select HUD", HUDdirectories, activeConfig.selectedHUD, queuedConfig.selectedHUD)) {
				copyHUDtoGame();
			}
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth);
			if (GUI_Checkbox2("Hide Main HUD", activeConfig.hideMainHUD, queuedConfig.hideMainHUD)) {
				ToggleHideMainHUD(activeConfig.hideMainHUD);
			}
			ImGui::PopItemWidth();


			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth);
			if (GUI_Checkbox2("Always Show Main HUD", activeConfig.forceVisibleHUD, queuedConfig.forceVisibleHUD)) {
				ToggleForceVisibleHUD(activeConfig.forceVisibleHUD);
			}

			ImGui::PopItemWidth();

			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			if (GUI_Checkbox2("Hide Lock-On", activeConfig.hideLockOn, queuedConfig.hideLockOn)) {
				ToggleHideLockOn(activeConfig.hideLockOn);
			}

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("Hide Boss Damage Bar", activeConfig.hideBossHUD, queuedConfig.hideBossHUD)) {
				ToggleHideBossHUD(activeConfig.hideBossHUD);
			}

			ImGui::TableNextColumn();

			GUI_Checkbox2("Disable Style Rank Fadeout", activeConfig.disableStyleRankHudFadeout, queuedConfig.disableStyleRankHudFadeout);

			ImGui::EndTable();
		}
	}


    ImGui::PopFont();
    ImGui::PopStyleColor();
    
	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
	ImGui::Text("CRIMSON HUD ADDONS");
	ImGui::PopFont();
	

	UI::SeparatorEx(defaultFontSize * 23.35f);

	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	ImGui::Text("");

 	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;


		if (ImGui::BeginTable("CrimsonHUDAddonsTable", 3)) {

			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableNextRow(0, rowWidth * 0.5f);
			ImGui::TableNextColumn();

			if (GUI_Checkbox2("HUD Positionings", activeCrimsonConfig.CrimsonHudAddons.positionings, queuedCrimsonConfig.CrimsonHudAddons.positionings)) {
				CrimsonDetours::ToggleClassicHUDPositionings(!activeCrimsonConfig.CrimsonHudAddons.positionings);
			}
			ImGui::SameLine();
			TooltipHelper("(?)", "Uncheck this if you're using Classic HUD.");

			ImGui::TableNextColumn();

			GUI_Checkbox2("Red Orb Counter", activeCrimsonConfig.CrimsonHudAddons.redOrbCounter, queuedCrimsonConfig.CrimsonHudAddons.redOrbCounter);

			ImGui::TableNextColumn();

			GUI_Checkbox2("Royal Gauge", activeCrimsonConfig.CrimsonHudAddons.royalGauge, queuedCrimsonConfig.CrimsonHudAddons.royalGauge);

			ImGui::TableNextRow(0, rowWidth * 0.5f);
			ImGui::TableNextColumn();

			GUI_Checkbox2("Style Ranks Meter", activeCrimsonConfig.CrimsonHudAddons.styleRanksMeter, queuedCrimsonConfig.CrimsonHudAddons.styleRanksMeter);

			ImGui::TableNextColumn();

			GUI_Checkbox2("Lock-On", activeCrimsonConfig.CrimsonHudAddons.lockOn, queuedCrimsonConfig.CrimsonHudAddons.lockOn);


			ImGui::EndTable();
		}
	}

	ImGui::Text("");

	GUI_PushDisable(!activeConfig.Actor.enable);
	GUI_Title("WEAPON WHEEL", true);

	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;

		if (ImGui::BeginTable("WeaponWheelOptionsTable", 3)) {

			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableNextRow(0, rowWidth * 0.5f);
			ImGui::TableNextColumn();

			// First Row - Analog Switching & Disable Camera Control
			ImGui::PushItemWidth(itemWidth * 0.8f);
			if (GUI_Checkbox2(
				"Analog Switching", activeCrimsonConfig.WeaponWheel.analogSwitching,
				queuedCrimsonConfig.WeaponWheel.analogSwitching)) {

				if (!queuedCrimsonConfig.WeaponWheel.analogSwitching) {
					activeCrimsonConfig.WeaponWheel.disableCameraRotation = false;
					queuedCrimsonConfig.WeaponWheel.disableCameraRotation = false;
				} else {
					activeCrimsonConfig.WeaponWheel.disableCameraRotation = true;
					queuedCrimsonConfig.WeaponWheel.disableCameraRotation = true;
				}
			}
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			GUI_PushDisable(!activeCrimsonConfig.WeaponWheel.analogSwitching);
			ImGui::PushItemWidth(itemWidth * 0.8f);
			GUI_Checkbox2("Disable Camera Control While Open",
				activeCrimsonConfig.WeaponWheel.disableCameraRotation,
				queuedCrimsonConfig.WeaponWheel.disableCameraRotation);
			ImGui::PopItemWidth();
			GUI_PopDisable(!activeCrimsonConfig.WeaponWheel.analogSwitching);

			ImGui::TableNextColumn();
			ImGui::PushItemWidth(itemWidth * 0.8f);
			UI::Combo2Vector("Theme", weaponWheelThemeNames,
				activeCrimsonConfig.WeaponWheel.theme,
				queuedCrimsonConfig.WeaponWheel.theme);
			ImGui::PopItemWidth();

			// Second Row - Melee/Ranged Always Show
			ImGui::TableNextRow(0, rowWidth * 0.5f);
			ImGui::TableNextColumn();

			GUI_Checkbox2("Wheel Always Show",
				activeCrimsonConfig.WeaponWheel.alwaysShow,
				queuedCrimsonConfig.WeaponWheel.alwaysShow);

			ImGui::TableNextColumn();
		
			ImGui::PushItemWidth(itemWidth * 0.8f);
			UI::Combo2Vector("Scale", weaponWheelScaleNames,
				activeCrimsonConfig.WeaponWheel.scale,
				queuedCrimsonConfig.WeaponWheel.scale);
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();
			GUI_Checkbox2("Force 1P Multiplayer Positioning/Scale",
				activeCrimsonConfig.WeaponWheel.force1PMultiplayerPosScale,
				queuedCrimsonConfig.WeaponWheel.force1PMultiplayerPosScale);

			ImGui::TableNextRow(0, rowWidth * 0.5f);

			ImGui::TableNextColumn();
			GUI_Checkbox2("Hide Weapon Wheel HUD",
				activeCrimsonConfig.WeaponWheel.hide,
				queuedCrimsonConfig.WeaponWheel.hide);

			ImGui::TableNextColumn();
			ImGui::PushItemWidth(itemWidth * 1.0f);
			UI::Combo2Vector("World Space Wheels", worldSpaceWheelNames,
				activeCrimsonConfig.WeaponWheel.worldSpaceWheels,
				queuedCrimsonConfig.WeaponWheel.worldSpaceWheels);
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();
			GUI_Checkbox2("World Space Wheel Always Show",
				activeCrimsonConfig.WeaponWheel.worldSpaceAlwaysShow,
				queuedCrimsonConfig.WeaponWheel.worldSpaceAlwaysShow);


			ImGui::EndTable();
		}
	}
	GUI_PopDisable(!activeConfig.Actor.enable);

	ImGui::Text("");

	ImGui::PopStyleColor();

	ImGui::PopFont();
	ImGui::PopStyleColor();

	ImGui::PopItemWidth();
	//ImGui::PopFont();

	BarsSection(defaultFontSize);
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



void SpeedSection() {
	ImGui::Text("");


	if (GUI_ResetButton()) {
		CopyMemory(&queuedConfig.Speed, &defaultConfig.Speed, sizeof(queuedConfig.Speed));
		CopyMemory(&activeConfig.Speed, &queuedConfig.Speed, sizeof(activeConfig.Speed));

		Speed::Toggle(false);
	}
	ImGui::Text("");

	ImGui::PushItemWidth(200);

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

		if (ImGui::BeginTable("GraphicsWindowOptionsTable", 3)) {

			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * 0.8f);
			if (GUI_InputDefault2<float>("Frame Rate", activeConfig.frameRate, queuedConfig.frameRate, defaultConfig.frameRate, 1, "%.2f",
				ImGuiInputTextFlags_EnterReturnsTrue)) {
				CrimsonOnTick::inputtingFPS = true;
			} else {
				CrimsonOnTick::inputtingFPS = false;
			}

			ImGui::PopItemWidth();

            ImGui::TableNextColumn();

			GUI_Checkbox2("Disable Blending Effects", activeConfig.disableBlendingEffects, queuedConfig.disableBlendingEffects);

			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * 0.8f);
			UI::Combo2("V-Sync", Graphics_vSyncNames, activeConfig.vSync, queuedConfig.vSync);
			ImGui::PopItemWidth();

			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			if (GUI_Checkbox2("Force Focus", activeConfig.forceWindowFocus, queuedConfig.forceWindowFocus)) {
				ToggleForceWindowFocus(activeConfig.forceWindowFocus);
			}

			ImGui::TableNextColumn();

			GUI_Checkbox2("Hide Mouse Cursor", activeConfig.hideMouseCursor, queuedConfig.hideMouseCursor);


			ImGui::EndTable();
		}
	}

	ImGui::Text("");

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);


	ImGui::Text("GAME SPEED");

	ImGui::PopFont();

	UI::SeparatorEx(defaultFontSize * 23.35f);

	ImGui::Text("");

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;

		if (ImGui::BeginTable("GameSpeedOptionsTable", 3)) {

			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
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

			ImGui::PushItemWidth(itemWidth * 0.8f);
			GUI_InputDefault2SpeedCalc("Main Speed", activeConfig.Speed.mainSpeed, queuedConfig.Speed.mainSpeed, defaultConfig.Speed.mainSpeed, 0.1f,
				"%g", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();
			ImGui::SameLine();
			TooltipHelper("(?)", "Changes Default Game Speed, changing this to be other than default\n" 
				"(without Frame Rate-Responsive Game Speed on) will tag you at the Mission End screen.");

			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * 0.8f);
			GUI_InputDefault2SpeedCalc("Turbo Speed", activeConfig.Speed.turbo, queuedConfig.Speed.turbo, defaultConfig.Speed.turbo, 0.1f, "%g",
				ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();
			ImGui::SameLine();
			TooltipHelper("(?)", "Changes Turbot Game Speed, changing this to be other than default\n"
				"(without Frame Rate-Responsive Game Speed on) will tag you at the Mission End screen.");


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

		if (ImGui::BeginTable("MiscOptionsTable", 2)) {

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

void TeleporterSection() {
   
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
			nextEventData.room = static_cast<uint16>(eventData.room);
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

#pragma endregion

#pragma region Training

void TrainingSection() {
	auto defaultFontSize = UI::g_UIContext.DefaultFontSize;
	const float itemWidth = defaultFontSize * 8.0f;
	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
	ImGui::Text("TRAINING");
	ImGui::PopFont();
	UI::SeparatorEx(defaultFontSize * 23.35f);
	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	ImGui::PushItemWidth(itemWidth);
	ImGui::Text("");

	float smallerComboMult = 0.7f;

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowWidth = 40.0f * queuedConfig.globalScale;

		if (ImGui::BeginTable("TrainingCheatsOptionsTable", 3)) {

			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			if (GUI_Checkbox2("Infinite Hit Points", activeConfig.infiniteHitPoints, activeConfig.infiniteHitPoints)) {
				ToggleInfiniteHitPoints(activeConfig.infiniteHitPoints);
			}

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("Infinite Magic Points", activeConfig.infiniteMagicPoints, queuedConfig.infiniteMagicPoints)) {
				ToggleInfiniteMagicPoints(activeConfig.infiniteMagicPoints);
			}

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("Disable Timer", activeConfig.disableTimer, queuedConfig.disableTimer)) {
				ToggleDisableTimer(activeConfig.disableTimer);
			}

			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			if (GUI_Checkbox2("Infinite Bullets", activeConfig.infiniteBullets, queuedConfig.infiniteBullets)) {
				ToggleInfiniteBullets(activeConfig.infiniteBullets);
			}
			ImGui::SameLine();
			TooltipHelper("(?)", "For Boss Lady.");

			ImGui::EndTable();
		}
	}

	ImGui::PopItemWidth();
	ImGui::PopStyleColor();
	ImGui::PopFont();

	ImGui::Text("");
    
}

#pragma endregion

#pragma region Training

void SoundSection(size_t defaultFontSize) {
	const char* changeGunNewNames[] = { "DMC3 Default", "New" };
	const char* changeDevilArmNewNames[] = { "DMC3 Default", "New" };

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
			UI::Combo2<uint8>("Change Gun", changeGunNewNames, activeCrimsonConfig.SFX.changeGunNew, queuedCrimsonConfig.SFX.changeGunNew);
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			UI::Combo2<uint8>("Change Devil Arm", changeDevilArmNewNames, activeCrimsonConfig.SFX.changeDevilArmNew, queuedCrimsonConfig.SFX.changeDevilArmNew);
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_InputDefault2<uint32>("Volume", activeCrimsonConfig.SFX.changeWeaponVolume, queuedCrimsonConfig.SFX.changeWeaponVolume, defaultCrimsonConfig.SFX.changeWeaponVolume, 10, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("STYLE SWITCH SFX");
			ImGui::PopFont();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_InputDefault2<uint32>("Effect Volume", activeCrimsonConfig.SFX.styleChangeEffectVolume, queuedCrimsonConfig.SFX.styleChangeEffectVolume, defaultCrimsonConfig.SFX.styleChangeEffectVolume, 10, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_InputDefault2<uint32>("VO Volume", activeCrimsonConfig.SFX.styleChangeVOVolume, queuedCrimsonConfig.SFX.styleChangeVOVolume, defaultCrimsonConfig.SFX.styleChangeVOVolume, 10, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("STYLE RANK ANNOUNCER");
			ImGui::PopFont();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_InputDefault2<uint32>("Volume", activeCrimsonConfig.SFX.styleRankAnnouncerVolume, queuedCrimsonConfig.SFX.styleRankAnnouncerVolume, defaultCrimsonConfig.SFX.styleRankAnnouncerVolume, 10, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_InputDefault2<uint32>("Cooldown", activeCrimsonConfig.SFX.styleRankAnnouncerCooldownSeconds, queuedCrimsonConfig.SFX.styleRankAnnouncerCooldownSeconds, defaultCrimsonConfig.SFX.styleRankAnnouncerCooldownSeconds, 1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::SameLine();
			TooltipHelper("(?)", "Duration until the Announcer can repeat the same line.\n\n(in Seconds)");
			ImGui::PopItemWidth();

			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();

			ImGui::Text("");
			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("DEVIL TRIGGER SFX");
			ImGui::PopFont();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_InputDefault2<uint32>("Activation L1 Volume", activeCrimsonConfig.SFX.devilTriggerInL1Volume, queuedCrimsonConfig.SFX.devilTriggerInL1Volume, defaultCrimsonConfig.SFX.devilTriggerInL1Volume, 10, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_InputDefault2<uint32>("Activation L2 Volume", activeCrimsonConfig.SFX.devilTriggerInL2Volume, queuedCrimsonConfig.SFX.devilTriggerInL2Volume, defaultCrimsonConfig.SFX.devilTriggerInL2Volume, 10, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_InputDefault2<uint32>("DT Ready Volume", activeCrimsonConfig.SFX.devilTriggerReadyVolume, queuedCrimsonConfig.SFX.devilTriggerReadyVolume, defaultCrimsonConfig.SFX.devilTriggerReadyVolume, 10, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			ImGui::Text("");

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("DOPPELGANGER SFX");
			ImGui::PopFont();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_InputDefault2<uint32>("Activation Volume", activeCrimsonConfig.SFX.doppelgangerInVolume, queuedCrimsonConfig.SFX.doppelgangerInVolume, defaultCrimsonConfig.SFX.doppelgangerInVolume, 10, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_InputDefault2<uint32>("Deactivation Volume", activeCrimsonConfig.SFX.doppelgangerOutVolume, queuedCrimsonConfig.SFX.doppelgangerOutVolume, defaultCrimsonConfig.SFX.doppelgangerOutVolume, 10, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			ImGui::Text("");

			ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
			ImGui::Text("QUICKSILVER SFX");
			ImGui::PopFont();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_InputDefault2<uint32>("Activation Volume", activeCrimsonConfig.SFX.quicksilverInVolume, queuedCrimsonConfig.SFX.quicksilverInVolume, defaultCrimsonConfig.SFX.quicksilverInVolume, 10, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
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

				ImGui::PushItemWidth(itemWidth * smallerComboMult);
				if (GUI_InputDefault2(Sound_channelNames[channelIndex], activeConfig.channelVolumes[channelIndex], queuedConfig.channelVolumes[channelIndex], defaultConfig.channelVolumes[channelIndex], 0.1f, "%g", ImGuiInputTextFlags_EnterReturnsTrue)) {
					SetVolume(channelIndex, activeConfig.channelVolumes[channelIndex]);
				}
				ImGui::PopItemWidth();
			}

			ImGui::EndTable();
		}
	}

	GUI_Checkbox2("Ignore Enemy Data", activeConfig.soundIgnoreEnemyData, queuedConfig.soundIgnoreEnemyData);
	ImGui::SameLine();
	TooltipHelper("(?)", "Do not look at enemy data when updating the global indices.\nMost, if not all enemies will lose their sound effects if enabled.\nIntended as a workaround for playable bosses when the sound effect\ninterferences from other enemies get too annoying. - serpentiem");

	ImGui::Text("");

	ImGui::PopFont();
	ImGui::PopStyleColor();
}

const char* dergilNames[] = {
	"Default",
	"Force Off",
	"Force On",
};

void VisualSection(size_t defaultFontSize) {
	const float itemWidth = defaultFontSize * 8.0f;
    float smallerComboMult = 0.7f;

	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);


	ImGui::Text("VFX COLORS");

	ImGui::PopFont();

	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	UI::SeparatorEx(defaultFontSize * 23.35f);

	
	ImGui::Text("");

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
    GUI_Checkbox2("STYLE SWITCH FLUX", activeCrimsonConfig.StyleSwitchFX.Flux.enable, queuedCrimsonConfig.StyleSwitchFX.Flux.enable);
	ImGui::PopFont();

	for (int style = 0; style < 6; style++) {
	
		if (style > 0) {
			ImGui::SameLine();
		}
		GUI_Color2("", activeCrimsonConfig.StyleSwitchFX.Flux.color[style], queuedCrimsonConfig.StyleSwitchFX.Flux.color[style]);
		ImGui::SameLine();
		ImGui::Text(styleNamesFX[style]);
	}


	if (GUI_Button("Colorful Subtle")) {
		CopyMemory(&queuedCrimsonConfig.StyleSwitchFX.Flux.color, &colorPresets.StyleSwitchFlux.colorfulSubtle, sizeof(queuedCrimsonConfig.StyleSwitchFX.Flux.color));
		CopyMemory(&activeCrimsonConfig.StyleSwitchFX.Flux.color, &colorPresets.StyleSwitchFlux.colorfulSubtle, sizeof(activeCrimsonConfig.StyleSwitchFX.Flux.color));
	}

	ImGui::SameLine();
	if (GUI_Button("DMC3 Switch")) {
		CopyMemory(&queuedCrimsonConfig.StyleSwitchFX.Flux.color, &colorPresets.StyleSwitchFlux.dMC3Switch, sizeof(queuedCrimsonConfig.StyleSwitchFX.Flux.color));
		CopyMemory(&activeCrimsonConfig.StyleSwitchFX.Flux.color, &colorPresets.StyleSwitchFlux.dMC3Switch, sizeof(activeCrimsonConfig.StyleSwitchFX.Flux.color));
	}

    ImGui::SameLine();
	if (GUI_Button("All Red")) {
		CopyMemory(&queuedCrimsonConfig.StyleSwitchFX.Flux.color, &colorPresets.StyleSwitchFlux.allRed, sizeof(queuedCrimsonConfig.StyleSwitchFX.Flux.color));
		CopyMemory(&activeCrimsonConfig.StyleSwitchFX.Flux.color, &colorPresets.StyleSwitchFlux.allRed, sizeof(activeCrimsonConfig.StyleSwitchFX.Flux.color));
	}

	
	ImGui::Text("");

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
    GUI_Checkbox2("STYLE SWITCH TEXT", activeCrimsonConfig.StyleSwitchFX.Text.enable, queuedCrimsonConfig.StyleSwitchFX.Text.enable);
	ImGui::PopFont();

	for (int style = 0; style < 9; style++) {
		if (style > 0) {
			ImGui::SameLine();
		}
		GUI_Color2("", activeCrimsonConfig.StyleSwitchFX.Text.color[style], queuedCrimsonConfig.StyleSwitchFX.Text.color[style]);
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
			GUI_InputDefault2<float>("Alpha", activeCrimsonConfig.StyleSwitchFX.Text.maxAlpha, queuedCrimsonConfig.StyleSwitchFX.Text.maxAlpha,
				defaultCrimsonConfig.StyleSwitchFX.Text.maxAlpha, 0.1f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

            ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_InputDefault2<float>("Size", activeCrimsonConfig.StyleSwitchFX.Text.size, queuedCrimsonConfig.StyleSwitchFX.Text.size,
				defaultCrimsonConfig.StyleSwitchFX.Text.size, 0.1f, "%g", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

			ImGui::EndTable();
		}
	}
	

	if (GUI_Button("Midnight")) {
		CopyMemory(&queuedCrimsonConfig.StyleSwitchFX.Text.color, &colorPresets.StyleSwitchText.midnight, sizeof(queuedCrimsonConfig.StyleSwitchFX.Text.color));
		CopyMemory(&activeCrimsonConfig.StyleSwitchFX.Text.color, &colorPresets.StyleSwitchText.midnight, sizeof(activeCrimsonConfig.StyleSwitchFX.Text.color));
	}

    ImGui::SameLine();
	if (GUI_Button("All White")) {
		CopyMemory(&queuedCrimsonConfig.StyleSwitchFX.Text.color, &colorPresets.StyleSwitchText.allWhite, sizeof(queuedCrimsonConfig.StyleSwitchFX.Text.color));
		CopyMemory(&activeCrimsonConfig.StyleSwitchFX.Text.color, &colorPresets.StyleSwitchText.allWhite, sizeof(activeCrimsonConfig.StyleSwitchFX.Text.color));
	}

    ImGui::SameLine();
	if (GUI_Button("Colorful Clear")) {
		CopyMemory(&queuedCrimsonConfig.StyleSwitchFX.Text.color, &colorPresets.StyleSwitchText.colorfulClear, sizeof(queuedCrimsonConfig.StyleSwitchFX.Text.color));
		CopyMemory(&activeCrimsonConfig.StyleSwitchFX.Text.color, &colorPresets.StyleSwitchText.colorfulClear, sizeof(activeCrimsonConfig.StyleSwitchFX.Text.color));
	}

	ImGui::SameLine();
	if (GUI_Button("Colorful")) {
		CopyMemory(&queuedCrimsonConfig.StyleSwitchFX.Text.color, &colorPresets.StyleSwitchText.colorful, sizeof(queuedCrimsonConfig.StyleSwitchFX.Text.color));
		CopyMemory(&activeCrimsonConfig.StyleSwitchFX.Text.color, &colorPresets.StyleSwitchText.colorful, sizeof(activeCrimsonConfig.StyleSwitchFX.Text.color));
	}

    ImGui::Text("");

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
	ImGui::Text("DEVIL TRIGGER FLUX");
	ImGui::PopFont();

	for (int dt = 0; dt < 5; dt++) {
		if (dt > 0) {
			ImGui::SameLine();
		}
        GUI_Color2("", activeConfig.Color.Aura.dante[dt], queuedConfig.Color.Aura.dante[dt]);
		ImGui::SameLine();
		ImGui::Text(meleeWeaponNamesDante[dt]);
	}
    ImGui::SameLine();
	GUI_Color2("", activeConfig.Color.Aura.sparda, queuedConfig.Color.Aura.sparda);
    ImGui::SameLine();
    ImGui::Text("Sparda");

	for (int dt = 0; dt < 3; dt++) {
		if (dt > 0) {
			ImGui::SameLine();
		}
		GUI_Color2("", activeConfig.Color.Aura.vergil[dt], queuedConfig.Color.Aura.vergil[dt]);
		ImGui::SameLine();
		ImGui::Text(meleeWeaponNamesVergil[dt]);
	}
    ImGui::SameLine();
	GUI_Color2("", activeConfig.Color.Aura.neroAngelo, queuedConfig.Color.Aura.neroAngelo);
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
		GUI_Color2("", activeConfig.Color.airHike[airhike], queuedConfig.Color.airHike[airhike]);
		ImGui::SameLine();
		ImGui::Text(meleeWeaponNamesDante[airhike]);
	}

    ImGui::Text("");

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
	ImGui::Text("OTHER MOVES");
	ImGui::PopFont();

	GUI_Color2("", activeConfig.Color.Trickster.skyStar, queuedConfig.Color.Trickster.skyStar);
	ImGui::SameLine();
	ImGui::Text("Sky Star");

    ImGui::SameLine();
	GUI_Color2("", activeConfig.Color.Royalguard.ultimate, queuedConfig.Color.Royalguard.ultimate);
	ImGui::SameLine();
	ImGui::Text("Royalguard Ultimate");

    ImGui::SameLine();
	GUI_Color2("", activeConfig.Color.Doppelganger.clone, queuedConfig.Color.Doppelganger.clone);
	ImGui::SameLine();
	ImGui::Text("Doppelganger");


	if (GUI_Button("DMC3 Default")) {
		CopyMemory(&queuedConfig.Color, &defaultConfig.Color, sizeof(queuedConfig.Color));
		CopyMemory(&activeConfig.Color, &queuedConfig.Color, sizeof(activeConfig.Color));
	}

    ImGui::SameLine();
	if (GUI_Button("Crimson")) {
		CopyMemory(&queuedConfig.Color, &colorPresets.ColorCrimson, sizeof(queuedConfig.Color));
		CopyMemory(&activeConfig.Color, &colorPresets.ColorCrimson, sizeof(activeConfig.Color));
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
            GUI_Checkbox2("Hide Beowulf on Dante", activeConfig.hideBeowulfDante, queuedConfig.hideBeowulfDante);
            ImGui::PopItemWidth();

            ImGui::TableNextColumn();

            ImGui::PushItemWidth(itemWidth * smallerComboMult);
            GUI_Checkbox2("Hide Beowulf on Vergil", activeConfig.hideBeowulfVergil, queuedConfig.hideBeowulfVergil);
            ImGui::PopItemWidth();

            ImGui::TableNextColumn();
		
			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			if (GUI_Checkbox2("No Devil Appearance in DT", activeConfig.noDevilForm, queuedConfig.noDevilForm)) {
				ToggleNoDevilForm(activeConfig.noDevilForm);
			}
			ImGui::PopItemWidth();

            ImGui::TableNextRow(0, rowWidth * 0.1f);
            ImGui::TableNextColumn();
            
			ImGui::PushItemWidth(itemWidth * smallerComboMult);
            UI::Combo2("Dot Shadow", dotShadowNames, activeConfig.dotShadow, queuedConfig.dotShadow);
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();
			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			if (UI::Combo2("Red-Coated Vergil Boss", dergilNames, activeConfig.dergil, queuedConfig.dergil)) {
				ToggleDergil(activeConfig.dergil);
			}
			ImGui::PopItemWidth();

			ImGui::EndTable();
		}
	}


    ImGui::PopFont();
    ImGui::PopStyleColor();	
}

// General gameplay options section
void GeneralGameplayOptions() {
	auto& defaultFontSize = UI::g_UIContext.DefaultFontSize;
	const float itemWidth = defaultFontSize * 8.0f;
	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
	ImGui::Text("GENERAL GAMEPLAY OPTIONS");
	ImGui::PopFont();
	UI::SeparatorEx(defaultFontSize * 23.35f);
	ImGui::Text("");

	// General Options Section
	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowHeight = 40.0f * queuedConfig.globalScale;

		if (ImGui::BeginTable("GameplayOptionsTable", 3)) {
			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableSetupColumn("b2", 0, columnWidth * 2.0f);
			ImGui::TableSetupColumn("b3", 0, columnWidth * 2.0f);

			// First row
			ImGui::TableNextRow(0, rowHeight * 0.5f);
			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("Inertia",
				activeCrimsonConfig.Gameplay.General.inertia,
				queuedCrimsonConfig.Gameplay.General.inertia)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Changes how physics behave during almost all aerial moves. Spices up your combos with more aerial momentum variance. Allows Guardflying.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("Sprint",
				activeCrimsonConfig.Gameplay.General.sprint,
				queuedCrimsonConfig.Gameplay.General.sprint)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Sprints out of combat, similar to DMC4 and 5's Speed Ability.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			if (!activeConfig.Actor.enable) {
				activeCrimsonConfig.Gameplay.General.holdToCrazyCombo = false;
				CrimsonDetours::ToggleHoldToCrazyCombo(false);
			}
			if (activeCrimsonConfig.Gameplay.General.holdToCrazyCombo) {
				activeCrimsonConfig.Gameplay.General.crazyComboMashRequirement = 3;
				queuedCrimsonConfig.Gameplay.General.crazyComboMashRequirement = 3;
				UpdateCrazyComboLevelMultiplier();
			}

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("Hold To Crazy Combo",
				activeCrimsonConfig.Gameplay.General.holdToCrazyCombo,
				queuedCrimsonConfig.Gameplay.General.holdToCrazyCombo)) {
				CrimsonDetours::ToggleHoldToCrazyCombo(activeCrimsonConfig.Gameplay.General.holdToCrazyCombo);
				activeCrimsonConfig.Gameplay.General.crazyComboMashRequirement = 3;
				queuedCrimsonConfig.Gameplay.General.crazyComboMashRequirement = 3;
				UpdateCrazyComboLevelMultiplier();
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			GUI_PopDisable(!activeConfig.Actor.enable);

			// Second row
			ImGui::TableNextRow(0, rowHeight * 0.5f);
			ImGui::TableNextColumn();

			GUI_PushDisable(activeCrimsonConfig.Gameplay.General.holdToCrazyCombo);
			ImGui::PushItemWidth(itemWidth * 0.8f);
			if (GUI_InputDefault2("Crazy Combo Mash Requirement",
				activeCrimsonConfig.Gameplay.General.crazyComboMashRequirement,
				queuedCrimsonConfig.Gameplay.General.crazyComboMashRequirement,
				defaultCrimsonConfig.Gameplay.General.crazyComboMashRequirement)) {
				UpdateCrazyComboLevelMultiplier();
			}
			ImGui::PopItemWidth();
			ImGui::SameLine();
			TooltipHelper("(?)", "Multiplier for how hard you need to mash to execute Crazy Combos. Less is easier.");
			GUI_PopDisable(activeCrimsonConfig.Gameplay.General.holdToCrazyCombo);

			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("Freeform Soft Lock",
				activeCrimsonConfig.Gameplay.General.freeformSoftLock,
				queuedCrimsonConfig.Gameplay.General.freeformSoftLock)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Bypass Soft Lock On Auto Rotation when outside Lock On. Enables Bufferless Reversals.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			if (!activeCrimsonConfig.Gameplay.General.freeformSoftLock) {
				activeCrimsonConfig.Gameplay.General.bufferlessReversals = false;
				queuedCrimsonConfig.Gameplay.General.bufferlessReversals = false;
			}

			GUI_PushDisable(!activeConfig.Actor.enable || !activeCrimsonConfig.Gameplay.General.freeformSoftLock);
			if (GUI_Checkbox2("Bufferless Reversals",
				activeCrimsonConfig.Gameplay.General.bufferlessReversals,
				queuedCrimsonConfig.Gameplay.General.bufferlessReversals)) {
				activeCrimsonConfig.Gameplay.General.freeformSoftLock = true;
				queuedCrimsonConfig.Gameplay.General.freeformSoftLock = true;
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Allows you to do Reversals without buffering a move first.");
			GUI_PopDisable(!activeConfig.Actor.enable || !activeCrimsonConfig.Gameplay.General.freeformSoftLock);

			// Third row
			ImGui::TableNextRow(0, rowHeight * 0.5f);
			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("Improved Buffered Reversals",
				activeCrimsonConfig.Gameplay.General.improvedBufferedReversals,
				queuedCrimsonConfig.Gameplay.General.improvedBufferedReversals)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Increased Turn Speed on Attacks for easier Buffered Reversals.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("DMC4/5 Lock On Direction",
				activeCrimsonConfig.Gameplay.General.dmc4LockOnDirection,
				queuedCrimsonConfig.Gameplay.General.dmc4LockOnDirection)) {
				CrimsonDetours::ToggleDMC4LockOnDirection(activeCrimsonConfig.Gameplay.General.dmc4LockOnDirection);
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Directional moves take enemy camera direction instead of player camera direction.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("Increased Jump Cancel Hitboxes",
				activeCrimsonConfig.Gameplay.General.increasedJCSpheres,
				queuedCrimsonConfig.Gameplay.General.increasedJCSpheres)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Increased hitboxes for easier Jump Cancels.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			// Fourth row
			ImGui::TableNextRow(0, rowHeight * 0.5f);
			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("Disable Height Restriction",
				activeCrimsonConfig.Gameplay.General.disableHeightRestriction,
				queuedCrimsonConfig.Gameplay.General.disableHeightRestriction)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Allows you to do Air Moves even at low heights.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("Disable Jump Cancel Restriction",
				activeCrimsonConfig.Gameplay.General.disableJCRestriction,
				queuedCrimsonConfig.Gameplay.General.disableJCRestriction)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Disables cooldown for Jump Cancels.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("Increased Enemy Juggle Time",
				activeCrimsonConfig.Gameplay.General.increasedEnemyJuggleTime,
				queuedCrimsonConfig.Gameplay.General.increasedEnemyJuggleTime)) {
				CrimsonPatches::ToggleIncreasedEnemyJuggleTime(activeCrimsonConfig.Gameplay.General.increasedEnemyJuggleTime);
			}
			ImGui::SameLine();
			TooltipHelper("(?)", "Increases stagger value of moves in the air, similar to DMC5.");

			// Fifth row
			ImGui::TableNextRow(0, rowHeight * 0.5f);
			ImGui::TableNextColumn();

			if (GUI_Checkbox2("Faster Turn Rate",
				activeCrimsonConfig.Gameplay.General.fasterTurnRate,
				queuedCrimsonConfig.Gameplay.General.fasterTurnRate)) {
				CrimsonDetours::ToggleFasterTurnRate(activeCrimsonConfig.Gameplay.General.fasterTurnRate);
			}
			ImGui::SameLine();
			TooltipHelper("(?)", "Increases character turn speed.");

			ImGui::EndTable();
		}
	}

	ImGui::Text("");

	ImGui::PopStyleColor();
	ImGui::PopFont();
}



// Dante gameplay options section
void DanteGameplayOptions() {
	auto& defaultFontSize = UI::g_UIContext.DefaultFontSize;
	const float itemWidth = defaultFontSize * 8.0f;
	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	// Dante Options Section
	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
	ImGui::Text("DANTE GAMEPLAY OPTIONS");
	ImGui::PopFont();

	UI::SeparatorEx(defaultFontSize * 23.35f);

	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	ImGui::Text("");

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowHeight = 40.0f * queuedConfig.globalScale;

		if (ImGui::BeginTable("DanteOptionsTable", 3)) {
			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableSetupColumn("b2", 0, columnWidth * 2.0f);
			ImGui::TableSetupColumn("b3", 0, columnWidth * 2.0f);

			// First row
			ImGui::TableNextRow(0, rowHeight * 0.5f);
			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("Improved Cancels",
				activeCrimsonConfig.Gameplay.Dante.improvedCancels,
				queuedCrimsonConfig.Gameplay.Dante.improvedCancels)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Enables a series of animation cancels for Dante, especially for moves between different styles.\nCheck out the Patch Notes for more info. Replaces DDMK's Remove Busy Flag.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("Aerial Rave Tweaks",
				activeCrimsonConfig.Gameplay.Dante.aerialRaveTweaks,
				queuedCrimsonConfig.Gameplay.Dante.aerialRaveTweaks)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Tweaks Aerial Rave Gravity to be similar to DMC4/5, taking weights into account.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("Air Flicker Tweaks",
				activeCrimsonConfig.Gameplay.Dante.airFlickerTweaks,
				queuedCrimsonConfig.Gameplay.Dante.airFlickerTweaks)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Tweaks Air Flicker Gravity, taking weights into account. Initial windup has less gravity than vanilla.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			// Second row
			ImGui::TableNextRow(0, rowHeight * 0.5f);
			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("Sky Dance Tweaks",
				activeCrimsonConfig.Gameplay.Dante.skyDanceTweaks,
				queuedCrimsonConfig.Gameplay.Dante.skyDanceTweaks)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Sky Dance Part 3 is now a separate ability executed by Lock On + Forward + Style. Tweaks Sky Dance Gravity, taking weights into account.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("Shotgun Air Shot Tweaks",
				activeCrimsonConfig.Gameplay.Dante.shotgunAirShotTweaks,
				queuedCrimsonConfig.Gameplay.Dante.shotgunAirShotTweaks)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Tweaks Air Shotgun Gravity to be lower, making you rise less.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("Drive Tweaks",
				activeCrimsonConfig.Gameplay.Dante.driveTweaks,
				queuedCrimsonConfig.Gameplay.Dante.driveTweaks)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Drive is now inputted by Lock On + Back to Forward + Melee and can be held for more damage. Do Quick Drive by performing Drive after a Rebellion attack.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			// Third row
			ImGui::TableNextRow(0, rowHeight * 0.5f);
			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("Bullet Stop",
				activeCrimsonConfig.Gameplay.Dante.bulletStop,
				queuedCrimsonConfig.Gameplay.Dante.bulletStop)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Shots from E&I among other moves will stop enemy movement on air.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("Rainstorm Lift",
				activeCrimsonConfig.Gameplay.Dante.rainstormLift,
				queuedCrimsonConfig.Gameplay.Dante.rainstormLift)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Rainstorm will elevate you slightly in the air.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("Infinite Rainstorm",
				activeCrimsonConfig.Gameplay.Dante.infiniteRainstorm,
				queuedCrimsonConfig.Gameplay.Dante.infiniteRainstorm)) {
				ToggleEbonyIvoryInfiniteRainStorm(activeCrimsonConfig.Gameplay.Dante.infiniteRainstorm);
			}
			ImGui::SameLine();
			TooltipHelper("(?)", "Makes rainstorm last indefinitely in the air.");

			// Fourth row
			ImGui::TableNextRow(0, rowHeight * 0.5f);
			ImGui::TableNextColumn();

			if (GUI_Checkbox2("E&I Foursome Time",
				activeCrimsonConfig.Gameplay.Dante.foursomeTime,
				queuedCrimsonConfig.Gameplay.Dante.foursomeTime)) {
				ToggleEbonyIvoryFoursomeTime(activeCrimsonConfig.Gameplay.Dante.foursomeTime);
			}
			ImGui::SameLine();
			TooltipHelper("(?)", "Ebony & Ivory's Twosome Time fires 2 additional shots.");

			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("Air Stinger",
				activeCrimsonConfig.Gameplay.Dante.airStinger,
				queuedCrimsonConfig.Gameplay.Dante.airStinger)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "With Rebellion: Lock On + Forward + Melee while in air.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("Air Revolver",
				activeCrimsonConfig.Gameplay.Dante.airRevolver,
				queuedCrimsonConfig.Gameplay.Dante.airRevolver)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "With Cerberus: Lock On + Forward + Melee while in air.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			// Fifth row
			ImGui::TableNextRow(0, rowHeight * 0.5f);
			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("Air Agni & Rudra Whirlwind",
				activeCrimsonConfig.Gameplay.Dante.airAgniRudraWhirlwind,
				queuedCrimsonConfig.Gameplay.Dante.airAgniRudraWhirlwind)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "With Agni & Rudra: LockOn + Back + Melee while in air.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("Air Tornado",
				activeCrimsonConfig.Gameplay.Dante.airTornado,
				queuedCrimsonConfig.Gameplay.Dante.airTornado)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "With Beowulf: Neutral + Style while in air. Reduces Tornado damage across the board. Carries inertia.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("Air Rising Dragon Whirlwind",
				activeCrimsonConfig.Gameplay.Dante.airRisingDragonWhirlwind,
				queuedCrimsonConfig.Gameplay.Dante.airRisingDragonWhirlwind)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "With Beowulf: LockOn + Back + Melee while in air.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			// Sixth row
			ImGui::TableNextRow(0, rowHeight * 0.5f);
			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("DMC4 Mobility",
				activeCrimsonConfig.Gameplay.Dante.dmc4Mobility,
				queuedCrimsonConfig.Gameplay.Dante.dmc4Mobility)) {
				queuedConfig.airHikeCount[1] = 2;
				queuedConfig.wallHikeCount[1] = 2;
				queuedConfig.skyStarCount[1] = 2;
				queuedConfig.airTrickCountDante[1] = 2;

				activeConfig.airHikeCount[1] = 2;
				activeConfig.wallHikeCount[1] = 2;
				activeConfig.skyStarCount[1] = 2;
				activeConfig.airTrickCountDante[1] = 2;
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Air Tricking from the ground does not consume the Air Trick Count. Also sets Air Trick, Air Hike and Sky Star Count to 2 when in DT.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (!activeConfig.Actor.enable) {
				activeCrimsonConfig.Gameplay.Dante.dTInfusedRoyalguard = false;
			}
			if (GUI_Checkbox2("DT Infused Royalguard",
				activeCrimsonConfig.Gameplay.Dante.dTInfusedRoyalguard,
				queuedCrimsonConfig.Gameplay.Dante.dTInfusedRoyalguard)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Royalguard Normal Blocks will consume DT instead, until you're low on DT. This will also prevent Guard Breaks by converting them into Normal Blocks.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			// Adding Air Hike section
			if (GUI_Checkbox2("Air Hike Core Ability",
				activeCrimsonConfig.Gameplay.Dante.airHikeCoreAbility,
				queuedCrimsonConfig.Gameplay.Dante.airHikeCoreAbility)) {
				ToggleAirHikeCoreAbility(activeCrimsonConfig.Gameplay.Dante.airHikeCoreAbility);
			}
			ImGui::SameLine();
			TooltipHelper("(?)", "Makes Air Hike available for all melee weapons.");

			// Seventh row - Last weapon options
			ImGui::TableNextRow(0, rowHeight * 0.5f);
			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("Alternate Nevan Vortex",
				activeCrimsonConfig.Gameplay.Dante.altNevanVortex,
				queuedCrimsonConfig.Gameplay.Dante.altNevanVortex)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "With Nevan and in DT: Lock On + Any Direction + Melee Attack while in air.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("Artemis Swap Normal Shot / Multi Lock",
				activeCrimsonConfig.Gameplay.Dante.artemisSwapShotMultiLock,
				queuedCrimsonConfig.Gameplay.Dante.artemisSwapShotMultiLock)) {
				ToggleArtemisSwapNormalShotAndMultiLock(activeCrimsonConfig.Gameplay.Dante.artemisSwapShotMultiLock);
			}
			ImGui::SameLine();
			TooltipHelper("(?)", "Swaps Artemis' Normal Shot and Multi-Lock functionality.");

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("Artemis Instant Full Charge",
				activeCrimsonConfig.Gameplay.Dante.artemisInstantFullCharge,
				queuedCrimsonConfig.Gameplay.Dante.artemisInstantFullCharge)) {
				ToggleArtemisInstantFullCharge(activeCrimsonConfig.Gameplay.Dante.artemisInstantFullCharge);
			}
			ImGui::SameLine();
			TooltipHelper("(?)", "Artemis charges to full semi-instantly.");

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("Cut Nevan Air Slash Knockback",
				activeCrimsonConfig.Gameplay.Dante.disableAirSlashKnockback,
				queuedCrimsonConfig.Gameplay.Dante.disableAirSlashKnockback)) {
				CrimsonPatches::DisableAirSlashKnockback(activeCrimsonConfig.Gameplay.Dante.disableAirSlashKnockback);
			}
			ImGui::SameLine();
			TooltipHelper("(?)", "Disables the Knockback from Nevan's Air Slash pt. 1.");

			ImGui::EndTable();
		}
	}

	ImGui::Text("");
	ImGui::PopStyleColor();
	ImGui::PopFont();
}

void DanteCheatOptions() {
	auto& defaultFontSize = UI::g_UIContext.DefaultFontSize;
	const float itemWidth = defaultFontSize * 8.0f;
	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	// Dante Cheats Section
	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
	ImGui::Text("DANTE CHEAT OPTIONS");
	ImGui::PopFont();

	UI::SeparatorEx(defaultFontSize * 23.35f);

	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	ImGui::Text("");

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowHeight = 40.0f * queuedConfig.globalScale;

		if (ImGui::BeginTable("DanteCheatsTable", 3)) {
			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableSetupColumn("b2", 0, columnWidth * 2.0f);
			ImGui::TableSetupColumn("b3", 0, columnWidth * 2.0f);

			// First row
			ImGui::TableNextRow(0, rowHeight * 0.5f);
			ImGui::TableNextColumn();

			if (GUI_Checkbox2("Force Royal Release",
				activeCrimsonConfig.Cheats.Dante.forceRoyalRelease,
				queuedCrimsonConfig.Cheats.Dante.forceRoyalRelease)) {
				CrimsonPatches::ToggleRoyalguardForceJustFrameRelease(activeCrimsonConfig.Cheats.Dante.forceRoyalRelease);
			}
			ImGui::SameLine();
			TooltipHelper("(?)", "Forces Royalguard Release to always be a Royal (Just Frame) Release.");

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("Infinite Shredder",
				activeCrimsonConfig.Cheats.Dante.infiniteShredder,
				queuedCrimsonConfig.Cheats.Dante.infiniteShredder)) {
				ToggleRebellionInfiniteShredder(activeCrimsonConfig.Cheats.Dante.infiniteShredder);
			}
			ImGui::SameLine();
			TooltipHelper("(?)", "Makes Rebellion's Shredder last for an indefinite amount of time.");

			ImGui::TableNextColumn();

			if (GUI_Checkbox2("Infinite Sword Pierce",
				activeCrimsonConfig.Cheats.Dante.infiniteSwordPierce,
				queuedCrimsonConfig.Cheats.Dante.infiniteSwordPierce)) {
				ToggleRebellionInfiniteSwordPierce(activeCrimsonConfig.Cheats.Dante.infiniteSwordPierce);
			}
			ImGui::SameLine();
			TooltipHelper("(?)", "Makes Rebellion's Sword Pierce last for an indefinite amount of time.");

			ImGui::EndTable();
		}
	}

	ImGui::Text("");
	ImGui::PopStyleColor();
	ImGui::PopFont();
}

// Vergil gameplay options section
void VergilGameplayOptions() {
	auto& defaultFontSize = UI::g_UIContext.DefaultFontSize;
	const float itemWidth = defaultFontSize * 8.0f;
	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);
	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
	ImGui::Text("VERGIL GAMEPLAY OPTIONS");
	ImGui::PopFont();
	UI::SeparatorEx(defaultFontSize * 23.35f);
	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);
	ImGui::Text("");

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowHeight = 40.0f * queuedConfig.globalScale;
		if (ImGui::BeginTable("VergilOptionsTable", 3)) {
			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableSetupColumn("b2", 0, columnWidth * 2.0f);
			ImGui::TableSetupColumn("b3", 0, columnWidth * 2.0f);

			// First row
			ImGui::TableNextRow(0, rowHeight * 0.5f);
			ImGui::TableNextColumn();
			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("Darkslayer Tricks Cancels All",
				activeCrimsonConfig.Gameplay.Vergil.darkslayerTrickCancels,
				queuedCrimsonConfig.Gameplay.Vergil.darkslayerTrickCancels)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Enables cancelling any move with any Darkslayer Trick at any time.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();
			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("Faster DT Rapid Slash",
				activeCrimsonConfig.Gameplay.Vergil.fasterDTRapidSlash,
				queuedCrimsonConfig.Gameplay.Vergil.fasterDTRapidSlash)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Rapid Slashes will execute faster while in DT.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();
			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("Round Trip Tweaks",
				activeCrimsonConfig.Gameplay.Vergil.roundTripTweaks,
				queuedCrimsonConfig.Gameplay.Vergil.roundTripTweaks)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "With Force Edge: Lock On + Back to Forward + Melee.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			// Second row
			ImGui::TableNextRow(0, rowHeight * 0.5f);
			ImGui::TableNextColumn();
			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("Air Stinger",
				activeCrimsonConfig.Gameplay.Vergil.airStinger,
				queuedCrimsonConfig.Gameplay.Vergil.airStinger)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "With Force Edge : Lock On + Forward + Melee while in air.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();
			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("Air Rising Sun",
				activeCrimsonConfig.Gameplay.Vergil.airRisingSun,
				queuedCrimsonConfig.Gameplay.Vergil.airRisingSun)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "With Beowulf: Lock On + Back + Melee while in air.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();
			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("Air Lunar Phase",
				activeCrimsonConfig.Gameplay.Vergil.airLunarPhase,
				queuedCrimsonConfig.Gameplay.Vergil.airLunarPhase)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "With Beowulf: Lock On + Forward + Melee while in air.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			// Third row
			ImGui::TableNextRow(0, rowHeight * 0.5f);
			ImGui::TableNextColumn();
			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("Alternate Judgement Cut Input",
				activeCrimsonConfig.Gameplay.Vergil.altJudgementCutInput,
				queuedCrimsonConfig.Gameplay.Vergil.altJudgementCutInput)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "With Yamato: Lock On + Back to Forward + Melee.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();
			GUI_PushDisable(!activeConfig.Actor.enable);
			ImGui::PushItemWidth(itemWidth * 0.5f);
			UI::Combo2Vector("Alt. Rising Sun Positioning",
				VergilMoveAdjustmentsNames,
				activeCrimsonConfig.Gameplay.Vergil.adjustRisingSunPos,
				queuedCrimsonConfig.Gameplay.Vergil.adjustRisingSunPos);
			ImGui::PopItemWidth();
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Stops Rising Sun from going upwards, still launches enemy.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();
			GUI_PushDisable(!activeConfig.Actor.enable);
			ImGui::PushItemWidth(itemWidth * 0.5f);
			UI::Combo2Vector("Alt. Lunar Phase Positioning",
				VergilMoveAdjustmentsNames,
				activeCrimsonConfig.Gameplay.Vergil.adjustLunarPhasePos,
				queuedCrimsonConfig.Gameplay.Vergil.adjustLunarPhasePos);
			ImGui::PopItemWidth();
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Stops Lunar Phase from going upwards, enemy behavior intact.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::EndTable();
		}
	}

	ImGui::Text("");
	ImGui::PopStyleColor();
	ImGui::PopFont();
}

void VergilCheatOptions() {
	auto& defaultFontSize = UI::g_UIContext.DefaultFontSize;
	const float itemWidth = defaultFontSize * 8.0f;
	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
	ImGui::Text("VERGIL CHEAT OPTIONS");
	ImGui::PopFont();

	UI::SeparatorEx(defaultFontSize * 23.35f);

	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);
	ImGui::Text("");

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowHeight = 40.0f * queuedConfig.globalScale;
		if (ImGui::BeginTable("VergilCheatsTable", 3)) {
			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableSetupColumn("b2", 0, columnWidth * 2.0f);
			ImGui::TableSetupColumn("b3", 0, columnWidth * 2.0f);

			// First row
			ImGui::TableNextRow(0, rowHeight * 0.5f);
			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("Quicksilver Style",
				activeCrimsonConfig.Cheats.Vergil.quicksilverStyle,
				queuedCrimsonConfig.Cheats.Vergil.quicksilverStyle)) {
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Enables Quicksilver for Vergil. Swap to it with D-Pad Downwards. Swap back to Darkslayer with D-Pad Up.");
			GUI_PopDisable(!activeConfig.Actor.enable);
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();

			GUI_PushDisable(!activeConfig.Actor.enable);
			if (GUI_Checkbox2("Chrono Swords",
				activeCrimsonConfig.Cheats.Vergil.chronoSwords,
				queuedCrimsonConfig.Cheats.Vergil.chronoSwords)) {
				ToggleChronoSwords(activeCrimsonConfig.Cheats.Vergil.chronoSwords);
			}
			ImGui::SameLine();
			GUI_CCSRequirementButton();
			ImGui::SameLine();
			TooltipHelper("(?)", "Summoned Swords will stand still as long as Quicksilver is active.");
			GUI_PopDisable(!activeConfig.Actor.enable);

			ImGui::TableNextColumn();
			if (GUI_Checkbox2("Infinite Round Trip",
				activeCrimsonConfig.Cheats.Vergil.infiniteRoundTrip,
				queuedCrimsonConfig.Cheats.Vergil.infiniteRoundTrip)) {
				ToggleYamatoForceEdgeInfiniteRoundTrip(activeCrimsonConfig.Cheats.Vergil.infiniteRoundTrip);
			}

			ImGui::EndTable();
		}
	}

	// Add special inputs for Rising Sun and Judgement Cut with HUMAN/DEVIL TRIGGER columns
	ImGui::Text("");

	auto VergilActionDataInput = [](const char* label, uint8(&active)[2], uint8(&queued)[2], uint8(&defaultVar)[2], bool disabled = false) {
		auto defaultFontSize = UI::g_UIContext.DefaultFontSize;
		const float itemWidth = defaultFontSize * 8.0f;
		const float rowWidth = 40.0f * queuedConfig.globalScale * 0.5f;
		float smallerComboMult = 0.7f;

		GUI_PushDisable(disabled);

		for (size_t i = 0; i < 2; ++i) {
			if (i == 0) {
				ImGui::TableNextRow(0, rowWidth);
				ImGui::TableNextColumn();
			}

			ImGui::PushItemWidth(itemWidth * smallerComboMult);
			GUI_InputDefault2("", active[i], queued[i], defaultVar[i], (uint8)1, "%u", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();

			if (i == 0) {
				ImGui::TableNextColumn();
			} else {
				ImGui::SameLine();
				ImGui::Text(label);
				ImGui::SameLine();
				GUI_CCSRequirementButton();
			}
		}

		GUI_PopDisable(disabled);
		};

	// Get initial cursor position for manual layout
	ImVec2 initialPos = ImGui::GetCursorPos();
	const float columnWidth = 0.15f * queuedConfig.globalScale;
	const float rowWidth = 40.0f * queuedConfig.globalScale * 0.5f;

	GUI_PushDisable(!activeConfig.Actor.enable);
	ImGui::SetCursorPos(initialPos);
	// Start table for Rising Sun and Judgement Cut
	if (ImGui::BeginTable("VergilActionTable", 2)) {
		ImGui::TableSetupColumn("b1", 0, columnWidth);
		ImGui::TableNextRow(0, rowWidth);
		ImGui::TableNextColumn();

		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 0.9f]);
		ImGui::Text("HUMAN");
		ImGui::TableNextColumn();
		ImGui::Text("DEVIL TRIGGER");
		ImGui::PopFont();

		bool beowulfDisabled = !activeCrimsonConfig.Gameplay.Vergil.airRisingSun;
		VergilActionDataInput("Air Rising Sun Count",
			activeCrimsonConfig.Cheats.Vergil.airRisingSunCount,
			queuedCrimsonConfig.Cheats.Vergil.airRisingSunCount,
			defaultCrimsonConfig.Cheats.Vergil.airRisingSunCount,
			beowulfDisabled);

		VergilActionDataInput("Judgement Cut Count",
			activeCrimsonConfig.Cheats.Vergil.judgementCutCount,
			queuedCrimsonConfig.Cheats.Vergil.judgementCutCount,
			defaultCrimsonConfig.Cheats.Vergil.judgementCutCount);

		ImGui::EndTable();
	}
	GUI_PopDisable(!activeConfig.Actor.enable);

	ImGui::Text("");
	ImGui::PopStyleColor();
	ImGui::PopFont();
}

void InputRemapOptions() {
	auto& defaultFontSize = UI::g_UIContext.DefaultFontSize;
	const float itemWidth = defaultFontSize * 8.0f;
	ImU32 checkmarkColorBg = UI::SwapColorEndianness(0xFFFFFFFF);

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[defaultFontSize * 1.1f]);
	ImGui::Text("INPUT REMAPS");
	ImGui::PopFont();

	UI::SeparatorEx(defaultFontSize * 23.35f);

	ImGui::PushFont(UI::g_ImGuiFont_Roboto[defaultFontSize * 0.9f]);
	ImGui::PushStyleColor(ImGuiCol_CheckMark, checkmarkColorBg);

	ImGui::Text("");
	ImGui::SameLine();
	TooltipHelper("(?)", "Remaps are global for all controllers, will only take into account Player 1's active Character for the switch.");

	{
		const float columnWidth = 0.5f * queuedConfig.globalScale;
		const float rowHeight = 40.0f * queuedConfig.globalScale;

		if (ImGui::BeginTable("InputRemapsTable", 3)) {
			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableSetupColumn("b2", 0, columnWidth * 2.0f);
			ImGui::TableSetupColumn("b3", 0, columnWidth * 2.0f);

			// First row
			ImGui::TableNextRow(0, rowHeight * 0.5f);
			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * 0.8f);
			GUI_ButtonCombo2("Dante DT Button",
				activeCrimsonConfig.Gameplay.Remaps.danteDTButton,
				queuedCrimsonConfig.Gameplay.Remaps.danteDTButton);
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * 0.8f);
			GUI_ButtonCombo2("Dante Shoot Button",
				activeCrimsonConfig.Gameplay.Remaps.danteShootButton,
				queuedCrimsonConfig.Gameplay.Remaps.danteShootButton);
			ImGui::PopItemWidth();

			// Second row
			ImGui::TableNextRow(0, rowHeight * 0.5f);
			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * 0.8f);
			GUI_ButtonCombo2("Vergil DT Button",
				activeCrimsonConfig.Gameplay.Remaps.vergilDTButton,
				queuedCrimsonConfig.Gameplay.Remaps.vergilDTButton);
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();

			ImGui::PushItemWidth(itemWidth * 0.8f);
			GUI_ButtonCombo2("Vergil Shoot Button",
				activeCrimsonConfig.Gameplay.Remaps.vergilShootButton,
				queuedCrimsonConfig.Gameplay.Remaps.vergilShootButton);
			ImGui::PopItemWidth();

			ImGui::EndTable();
		}
	}

	ImGui::PopStyleColor();
	ImGui::PopFont();
}

void GameplaySection() {
	GeneralGameplayOptions();
	DanteGameplayOptions();
	VergilGameplayOptions();
	InputRemapOptions();
}

#pragma endregion

#pragma region Hotkeys

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

void ToggleOneHitKill() {
	static bool toggled = activeConfig.damageEnemyActorMultiplier == 100.0f ? true : false;

	if (!toggled) {
		toggled = true;
		activeConfig.damageEnemyActorMultiplier = 100.0f;
	} else {
		toggled = false;
		activeConfig.damageEnemyActorMultiplier = defaultConfig.damageEnemyActorMultiplier;
	}
}

void GamepadToggleShowMain() {

    static bool gamepadCombinationMainRelease[PLAYER_COUNT] = { false };

	// Loop through each controller
	for (int i = 0; i < 4; ++i) {
		if (CrimsonSDL::controllers[i] != NULL) {
			// Combination of buttons to check
			bool combination = (CrimsonSDL::IsControllerButtonDown(i, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_LEFTSTICK) && 
                CrimsonSDL::IsControllerButtonDown(i, SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSTICK));

			// Combination pressed and was not pressed before, toggle GUI and set window focus
			if (combination && !g_showMain && gamepadCombinationMainRelease[i]) {
				ToggleCrimsonGUI();
				ImGui::SetWindowFocus(DMC3C_TITLE);
				gamepadCombinationMainRelease[i] = false;
			}

			// Combination released, update release state
			if (!combination && !gamepadCombinationMainRelease[i]) {
				gamepadCombinationMainRelease[i] = true;
			}

			// Combination pressed, GUI shown, and was not pressed before, toggle GUI
			if (combination && g_showMain && gamepadCombinationMainRelease[i]) {
				ToggleCrimsonGUI();
				gamepadCombinationMainRelease[i] = false;
			}
		}
	}


}

std::vector<KeyBinding> keyBindings = {
    {
        "Toggle Crimson GUI",
        activeConfig.keyData[0],
        queuedConfig.keyData[0],
        defaultConfig.keyData[0],
        ToggleCrimsonGUI,
    },
	{"Toggle Infinite Health Points", activeConfig.keyData[1], queuedConfig.keyData[1], defaultConfig.keyData[1], ToggleInfiniteHealth},
	{"Toggle One Hit Kill", activeConfig.keyData[2], queuedConfig.keyData[2], defaultConfig.keyData[2], ToggleOneHitKill},
    {"Reload Room", activeConfig.keyData[3], queuedConfig.keyData[3], defaultConfig.keyData[3], ReloadRoom},
    {"Move To Main Character", activeConfig.keyData[4], queuedConfig.keyData[4], defaultConfig.keyData[4], MoveToMainActor},
};

void HotkeysSection() {
	auto& defaultFontSize = UI::g_UIContext.DefaultFontSize;

	GUI_Title("KEYBOARD HOTKEYS");

	bool condition = false;

	for_all(index, keyBindings.size()) {
		auto& keyBinding = keyBindings[index];

		if (keyBinding.showPopup) {
			condition = true;

			break;
		}
	}

	GUI_PushDisable(condition);

	ImVec2 startPos = ImGui::GetCursorScreenPos();

	for_all(index, keyBindings.size()) {
		auto& keyBinding = keyBindings[index];

		ImGui::SetCursorScreenPos(startPos);
		keyBinding.Main();

		startPos.y += 100; // <- matches ImVec2{300, 100} from the keybinding's button height
	}

	GUI_PopDisable(condition);
    
}

void CommonCheatsSection() {
	//if constexpr (debug) {
	DebugSection();
	//}
	TrainingSection();
	DamageSection();
	DanteCheatOptions();
	VergilCheatOptions();
	CustomMobilitySection();
	Other();
	Lady();
	Repair();
}

void PreventEmptyCrimsonGUIHotkey() {
	if (queuedConfig.keyData[0].IsEmpty()) {
		queuedConfig.keyData[0] = defaultConfig.keyData[0];
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
        float width  = g_renderSize.x / 1.40;
        float height = g_renderSize.y / 1.20;

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
                CharacterSection(context.DefaultFontSize);
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
                constexpr auto ACTOR_SYSTEM_INTRO = u8"Custom Character Creation and Spawning System primarily developed by DDMK's Serpentiem (Formerly 'Actor System').";
                constexpr auto FEATURE_STYLE_SWITCH = u8"   • Style Switching;";
                constexpr auto FEATURE_WEAPONS = u8"   • Custom Weapon Loadouts;";
                constexpr auto FEATURE_CHAR_SWITCH = u8"   • Character Switching;";
                constexpr auto FEATURE_MULTIPLAYER = u8"   • Local Multiplayer;";
                constexpr auto FEATURE_DOPPEL_TWEAKS = u8"   • Doppelganger Tweaks;";

				ImGui::PushFont(UI::g_ImGuiFont_RussoOne[size_t(context.DefaultFontSize * 1.0f)]);
				ImGui::Text("CRIMSON CHARACTER SYSTEM");
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
                ImGui::TextWrapped("*Required for Crimson and Style Switcher Modes.");
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
						//GamepadClose(visibleMain, lastVisibleMain, CloseMain);

						GameplaySection();
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
						HotkeysSection();
					}
				}
				ImGui::EndChild();
			}

			//Tooltip area
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
					ImGui::Text("HOTKEY OPTIONS");
					ImGui::PopFont();


					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.8f);
					ImGui::PushFont(UI::g_ImGuiFont_Roboto[size_t(context.DefaultFontSize * 0.9f)]);

					ImGui::TextWrapped("Set various settings to keyboard shortcuts.");
					ImGui::PopFont();
				}
				ImGui::EndChild();
			}
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

			//Tooltip area
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
        else if (context.SelectedOptionsSubTab == UI::UIContext::OptionsSubTabs::Sound) {
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
						SoundSection(context.DefaultFontSize);
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
					ImGui::Text("SOUND OPTIONS");
					ImGui::PopFont();


					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.8f);
					ImGui::PushFont(UI::g_ImGuiFont_Roboto[size_t(context.DefaultFontSize * 0.9f)]);

					ImGui::TextWrapped("Settings related to Sound.");
					ImGui::PopFont();
				}
				ImGui::EndChild();
			}
        }
        else if (context.SelectedOptionsSubTab == UI::UIContext::OptionsSubTabs::Visual) {
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
						VisualSection(context.DefaultFontSize);
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
					ImGui::Text("VISUAL OPTIONS");
					ImGui::PopFont();


					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.8f);
					ImGui::PushFont(UI::g_ImGuiFont_Roboto[size_t(context.DefaultFontSize * 0.9f)]);

					ImGui::TextWrapped("Settings related to various Visual Effects.");
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
		if (context.SelectedCheatsAndDebugSubTab == UI::UIContext::CheatsAndDebugSubTabs::Common) {
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
						CommonCheatsSection();
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
					ImGui::Text("COMMON CHEATS");
					ImGui::PopFont();


					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.8f);
					ImGui::PushFont(UI::g_ImGuiFont_Roboto[size_t(context.DefaultFontSize * 0.9f)]);

					ImGui::TextWrapped("Various Cheats for you to customize the game to your liking.\nThis will tag you at the End of Mission screen.");
					ImGui::PopFont();
				}
				ImGui::EndChild();
			}
		}
		else if (context.SelectedCheatsAndDebugSubTab == UI::UIContext::CheatsAndDebugSubTabs::Speed) {
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
						SpeedSection();						
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
					ImGui::Text("CUSTOM SPEED SETTINGS");
					ImGui::PopFont();


					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.8f);
					ImGui::PushFont(UI::g_ImGuiFont_Roboto[size_t(context.DefaultFontSize * 0.9f)]);

					ImGui::TextWrapped("Various Speed Settings for in-game entities.\nThis will tag you at the End of Mission screen.");
					ImGui::PopFont();
				}
				ImGui::EndChild();
			}
		}
		else if (context.SelectedCheatsAndDebugSubTab == UI::UIContext::CheatsAndDebugSubTabs::Teleporter) {
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
						TeleporterSection();
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
					ImGui::Text("TELEPORTER");
					ImGui::PopFont();


					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.8f);
					ImGui::PushFont(UI::g_ImGuiFont_Roboto[size_t(context.DefaultFontSize * 0.9f)]);

					ImGui::TextWrapped("Teleport anywhere. Using this will tag you at the Mission End screen.");
					ImGui::PopFont();
				}
				ImGui::EndChild();
			}

		}
		else if (context.SelectedCheatsAndDebugSubTab == UI::UIContext::CheatsAndDebugSubTabs::EnemySpawner) {
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
						EnemySpawnerSection();
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
					ImGui::Text("ENEMY SPAWNER");
					ImGui::PopFont();


					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.8f);
					ImGui::PushFont(UI::g_ImGuiFont_Roboto[size_t(context.DefaultFontSize * 0.9f)]);

					ImGui::TextWrapped("Spawn new enemies at your leisure.");
					ImGui::PopFont();
				}
				ImGui::EndChild();
			}
		}
		else if (context.SelectedCheatsAndDebugSubTab == UI::UIContext::CheatsAndDebugSubTabs::JukeBox) {
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
						JukeboxSection();
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
					ImGui::Text("JUKEBOX");
					ImGui::PopFont();


					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.8f);
					ImGui::PushFont(UI::g_ImGuiFont_Roboto[size_t(context.DefaultFontSize * 0.9f)]);

					ImGui::TextWrapped("Play songs from the original game at your heart's content, using the original game's audio system (FMOD).");
					ImGui::PopFont();
				}
				ImGui::EndChild();
			}
		}
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
			const ImVec2 areaSize = cntWindow->Size * ImVec2{ areaPaddingXRation, 1.0f };
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
						ImGui::Text("Project Director, Lead Programmer, Artist, Reverse Engineering");
					}
					ImGui::PopFont();

					ImGui::Separator();

					ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
					{
						ImGui::Text("Berthrage");

						ImGui::SameLine();

						const ImVec2 socialsBBFrameSize{ 6.0f * 4.0f + 6.0f * ImGui::GetFontSize(), 4.0f + ImGui::GetFontSize() };
						const ImVec2 currentCursorPos = ImGui::GetCursorScreenPos();

						ImGui::SetCursorScreenPos(ImVec2{ window->ContentRegionRect.Max.x - socialsBBFrameSize.x, currentCursorPos.y });

						if (fnDrawSocialButton("miatwitter", SocialsIcons::ID_Twitter, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
							ShellExecute(0, 0, "https://twitter.com/Berthrage", 0, 0, SW_SHOW);
						}

						ImGui::SameLine(0.0f, 0.0f);

						if (fnDrawSocialButton("miagithub", SocialsIcons::ID_Github, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
							ShellExecute(0, 0, "https://github.com/berthrage", 0, 0, SW_SHOW);
						}

						ImGui::SameLine(0.0f, 0.0f);

						if (fnDrawSocialButton("miayt", SocialsIcons::ID_YouTube, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
							ShellExecute(0, 0, "https://www.youtube.com/@Berthrage", 0, 0, SW_SHOW);
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
						ImGui::Text("Reverse Engineering, Gameplay Programmer, Q&A, Testing");
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
						ImGui::Text("Reverse Engineering, Tooling, Graphics Programmer");
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
						ImGui::Text("Backend, DevOps, General Programmer, Reverse Engineering");
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

				// Additional Work
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 1.2f)]);
					{
						ImGui::Text("With Additional Work By");
					}
					ImGui::PopFont();

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + context.DefaultFontSize * 0.2f);

					// Charlie
					{
						ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 0.9f)]);
						{
							ImGui::Text("Community Manager, Testing, Q&A");
						}
						ImGui::PopFont();

						ImGui::Separator();

						ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
						{
							ImGui::Text("Charlie");

							ImGui::SameLine();

							if (fnDrawSocialButton("serpgithub", SocialsIcons::ID_Github, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
								ShellExecute(0, 0, "https://github.com/serpentiem", 0, 0, SW_SHOW);
							}
						}
						ImGui::PopFont();
					}

					{
						ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 0.9f)]);
						{
							ImGui::Text("Artist");
						}
						ImGui::PopFont();

						ImGui::Separator();

						ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
						{
							ImGui::Text("Cynuma");

							ImGui::SameLine();
 							
							if (fnDrawSocialButton("serpgithub", SocialsIcons::ID_Github, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
 									ShellExecute(0, 0, "https://github.com/serpentiem", 0, 0, SW_SHOW);
							}
						}
						ImGui::PopFont();
					}

					{
						ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 0.9f)]);
						{
							ImGui::Text("3D Artist");
						}
						ImGui::PopFont();

						ImGui::Separator();

						ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
						{
							ImGui::Text("Omar Nabelse");

							ImGui::SameLine();

							if (fnDrawSocialButton("serpgithub", SocialsIcons::ID_Github, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
								ShellExecute(0, 0, "https://github.com/serpentiem", 0, 0, SW_SHOW);
							}
						}
						ImGui::PopFont();
					}

					{
						ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 0.9f)]);
						{
							ImGui::Text("Mod Tooling");
						}
						ImGui::PopFont();

						ImGui::Separator();

						ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
						{
							ImGui::Text("Che");

							ImGui::SameLine();

							if (fnDrawSocialButton("serpgithub", SocialsIcons::ID_Github, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
								ShellExecute(0, 0, "https://github.com/serpentiem", 0, 0, SW_SHOW);
							}
						}
						ImGui::PopFont();
					}

					{
						ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 0.9f)]);
						{
							ImGui::Text("Artist");
						}
						ImGui::PopFont();

						ImGui::Separator();

						ImGui::PushFont(UI::g_ImGuiFont_Roboto[uint64_t(context.DefaultFontSize * 1.0f)]);
						{
							ImGui::Text("Vainiuss1");

							ImGui::SameLine();

							if (fnDrawSocialButton("serpgithub", SocialsIcons::ID_Github, ImVec2{ ImGui::GetFontSize(), ImGui::GetFontSize() })) {
								ShellExecute(0, 0, "https://github.com/serpentiem", 0, 0, SW_SHOW);
							}
						}
						ImGui::PopFont();
					}
				}

				ImGui::Text("");

				// Serp
				{
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[uint64_t(context.DefaultFontSize * 1.2f)]);
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
				constexpr const char* COPYRIGHT_TEXT = "Copyright (c) 2025 Berthrage";

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

    if (g_scene != SCENE::GAME) {
        devilTriggerReadyPlayed = !activeConfig.playDTReadySFXAtMissionStart;
    }

	UI::ResetID(0);

	PreventEmptyCrimsonGUIHotkey();
    AdjustBackgroundColorAndTransparency();

    Welcome();
    Main(pSwapChain);
    Shop::ShopWindow();


	ActorWindow();
	EventDataWindow();
	ExpWindow();
	FileDataWindow();
	MissionDataWindow();
	RegionDataWindow();
	SoundWindow();
    

    PauseWhenGUIOpened();
    GamepadToggleShowMain();
	if (activeConfig.debugOverlayData.enable) {
		DebugOverlayWindow(UI::g_UIContext.DefaultFontSize);
	}
    MissionOverlayWindow(UI::g_UIContext.DefaultFontSize);
     BossLadyActionsOverlayWindow();
     BossVergilActionsOverlayWindow();
    
	// Calling this from GUI Render is the safest way to ensure this will run on-tick properly
    // outside of In Game.
    
  	CrimsonOnTick::FrameResponsiveGameSpeed();
	CrimsonOnTick::WeaponProgressionTracking();
	CrimsonOnTick::PreparePlayersDataBeforeSpawn();

    // TIMERS
    CrimsonTimers::CallAllTimers();

    MultiplayerBars(pSwapChain);
	WeaponWheels1PController(pSwapChain);
	WeaponWheelsMultiplayerController(pSwapChain);
	WorldSpaceWeaponWheels1PController(pSwapChain);
	WorldSpaceWeaponWheelsController(pSwapChain);
    MirageGaugeMainPlayer();
	RedOrbCounterWindow();
	StyleMeterWindow();


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
}

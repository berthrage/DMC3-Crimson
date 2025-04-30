// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <thread>
#include <chrono>
#include <math.h>
#include "../ThirdParty/glm/glm.hpp"
#include "../ThirdParty/ImGui/imgui.h"
#include <ctime>
#include <iostream>
#include <cstdio>
#include "Utility/Detour.hpp"
#include "DebugDrawDX11.hpp"
#define DEBUG_DRAW_EXPLICIT_CONTEXT
#include "debug_draw.hpp"
#include "DMC3Input.hpp"
#include "File.hpp"
#include "Internal.hpp"
#include "ActorBase.hpp"
#include "Core/Core.hpp"
#include "Memory.hpp"
#include "Model.hpp"
#include "ActorRelocations.hpp"
#include "Config.hpp"
#include "Exp.hpp"
#include "Global.hpp"
#include "HUD.hpp"
#include "Sound.hpp"
#include "SoundRelocations.hpp"
#include "Speed.hpp"
#include "Vars.hpp"
#include "StyleSwitchFX.hpp"
#include "Core/Macros.h"
#include <deque>
#include "Training.hpp"
#include "CrimsonDetours.hpp"
#include "CrimsonSDL.hpp"
#include "CrimsonPatches.hpp"
#include "CrimsonTimers.hpp"
#include "CrimsonUtil.hpp"
#include "UI\Texture2DD3D11.hpp"
#include "UI\EmbeddedImages.hpp"
#include "CrimsonGUI.hpp"
#include "CrimsonFileHandling.hpp"

namespace CrimsonHUD {

using namespace CrimsonGUI;

static Texture2DD3D11* RedOrbTexture{ nullptr };
static Texture2DD3D11* RedOrbVanillaTexture{ nullptr };
static Texture2DD3D11* RedOrbStyleSwitcherTexture{ nullptr };
static Texture2DD3D11* RedOrbCrimsonTexture{ nullptr };
static Texture2DD3D11* RedOrbCustomTexture{ nullptr };

static Texture2DD3D11* DStyleRankFillTexture{ nullptr };
static Texture2DD3D11* DStyleRankBackgroundTexture{ nullptr };

void InitRedOrbTexture(ID3D11Device* pd3dDevice) {
	//RedOrbTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "RedorbVanilla3.png").c_str(), pd3dDevice);
	RedOrbTexture = new Texture2DD3D11(g_Image_RedOrb.GetRGBAData(), g_Image_RedOrb.GetWidth(), g_Image_RedOrb.GetHeight(), pd3dDevice);
	RedOrbVanillaTexture = new Texture2DD3D11(g_Image_RedOrbVanilla.GetRGBAData(), g_Image_RedOrbVanilla.GetWidth(), g_Image_RedOrbVanilla.GetHeight(), pd3dDevice);
	RedOrbStyleSwitcherTexture = new Texture2DD3D11(g_Image_RedOrbStyleSwitcher.GetRGBAData(), g_Image_RedOrbStyleSwitcher.GetWidth(), g_Image_RedOrbStyleSwitcher.GetHeight(), pd3dDevice);
	RedOrbCrimsonTexture = new Texture2DD3D11(g_Image_RedOrbCrimson.GetRGBAData(), g_Image_RedOrbCrimson.GetWidth(), g_Image_RedOrbCrimson.GetHeight(), pd3dDevice);
	RedOrbCustomTexture = new Texture2DD3D11(g_Image_RedOrbCustom.GetRGBAData(), g_Image_RedOrbCustom.GetWidth(), g_Image_RedOrbCustom.GetHeight(), pd3dDevice);
	assert(RedOrbTexture);
}

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
	ImGui::GetWindowDrawList()->AddImage(backgroundTexture, pos, ImVec2(pos.x + size.x, pos.y + size.y), ImVec2(0, 0), ImVec2(1, 1), color);
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

void RedOrbCounterWindow() {
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

	// Define the orb count and cap it at 999999
	int orbCount = (std::min)(999999, (int)missionData.redOrbs);
	std::string orbCountStr = std::to_string(orbCount);

	// Adjust the font size and the proportional texture size
	float fontSize = 37.0f;

	// previously 142x200 -> 43x61; now 178x250 -> 54x76 to make space for the glow.
	float textureBaseSizeX = 54.0f;
	float textureBaseSizeY = 76.0f;
	float textureWidth = textureBaseSizeX * scaleFactorY;
	float textureHeight = textureBaseSizeY * scaleFactorY;
	float centerX = textureWidth / 2.0f;
	float centerY = textureHeight / 2.0f;

	// Define the window size and position
	ImVec2 windowSize = ImVec2(300.0f * scaleFactorX, 100.0f * scaleFactorY);
	float edgeOffsetX = 70.0f * scaleFactorY;
	float edgeOffsetY = 30.0f * scaleFactorY;
	ImVec2 windowPos = ImVec2(displaySize.x - windowSize.x - edgeOffsetX, edgeOffsetY);
	//ImVec2 windowPos = ImVec2(displaySize.x - windowSize.x - 70.0f * scaleFactorX, 30.0f * scaleFactorY);

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
	ImGui::SetWindowFontScale(scaleFactorY);
	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[fontSize]);
	ImVec2 textSize = ImGui::CalcTextSize(orbCountStr.c_str(), nullptr, true);
	ImVec2 textPos = ImVec2(windowSize.x - textSize.x - 74.0f * scaleFactorY, (windowSize.y - textSize.y) / 2);

	// Correct the texture position by considering the window's screen position
	ImVec2 texturePos = ImVec2(windowPos.x + textPos.x - textureWidth - 17.916f * scaleFactorY, windowPos.y + (windowSize.y - textureHeight) / 2);

	static auto* redOrbGameMode = RedOrbCrimsonTexture;
	switch (activeCrimsonGameplay.GameMode.preset) {
	case(GAMEMODEPRESETS::VANILLA):
		redOrbGameMode = RedOrbVanillaTexture;
		break;

	case(GAMEMODEPRESETS::STYLE_SWITCHER):
		redOrbGameMode = RedOrbStyleSwitcherTexture;
		break;

	case(GAMEMODEPRESETS::CRIMSON):
		redOrbGameMode = RedOrbCrimsonTexture;
		break;

	case(GAMEMODEPRESETS::CUSTOM):
		redOrbGameMode = RedOrbCustomTexture;
		break;

	default:
		redOrbGameMode = RedOrbCrimsonTexture;
		break;

	}

	// Render the texture or a white square if the texture is not valid
	if (redOrbGameMode->IsValid()) {
		// 		DrawRotatedImage(
		// 			RedOrbTexture->GetTexture(),
		// 			texturePos,
		// 			ImVec2(textureWidth, textureHeight),
		// 			IM_PI / 2.0f, // 90 degrees in radians
		// 			colorWithAlpha
		// 		);
		ImGui::GetWindowDrawList()->AddImage(redOrbGameMode->GetTexture(), texturePos, ImVec2(texturePos.x + textureWidth, texturePos.y + textureHeight), ImVec2(0, 0), ImVec2(1, 1), colorWithAlpha);
	} else {
		ImGui::GetWindowDrawList()->AddRectFilled(texturePos, ImVec2(texturePos.x + textureWidth, texturePos.y + textureHeight), ImColor(1.0f, 1.0f, 1.0f, alpha));
	}

	// Render the orb count text
	ImGui::SetCursorPos(ImVec2(textPos.x, textPos.y));
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, alpha), "%s", orbCountStr.c_str());

	ImGui::PopFont();
	ImGui::End();
}

void CheatsHUDIndicatorWindow() {
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

	// Define the window size and position
	ImVec2 windowSize = ImVec2(367.0f * scaleFactorX, 100.0f * scaleFactorY);
	float edgeOffsetX = 0.0f * scaleFactorY;
	float edgeOffsetY = 15.0f * scaleFactorY;
	ImVec2 windowPos = ImVec2(g_renderSize.x - windowSize.x - edgeOffsetX, edgeOffsetY);
	auto& currentGameMode = activeCrimsonGameplay.GameMode.preset;

	ImGui::SetNextWindowSize(windowSize);
	ImGui::SetNextWindowPos(windowPos);
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;

	// Adjust the font size and the proportional texture size
	float fontSize = 18.0f;

	ImGui::Begin("CheatsHUDIndicatorWindow", nullptr, windowFlags);
	// Set the color with alpha for the Red Orb texture
	float alpha = crimsonHud.redOrbAlpha / 127.0f;
	ImColor colorWithAlpha(1.0f, 1.0f, 1.0f, alpha);
	ImGui::SetWindowFontScale(scaleFactorY);
	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[fontSize]);

	// Prepare button and text colors with alpha
	ImVec4 buttonColor = ImColor(UI::SwapColorEndianness(gameModeData.colors[currentGameMode]));
	buttonColor.w *= alpha;
	ImVec4 textColor = (currentGameMode <= 1)
		? ImColor(UI::SwapColorEndianness(0x151515FF))
		: ImColor(UI::SwapColorEndianness(0xFFFFFFFF));
	textColor.w *= alpha;
	ImVec4 borderColor = ImGui::GetStyleColorVec4(ImGuiCol_Border);
	borderColor.w *= alpha;
	ImGui::PushStyleColor(ImGuiCol_Border, borderColor);
	ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
	ImGui::PushStyleColor(ImGuiCol_Text, textColor);


	// Calculate total width of all buttons and spacing
	float totalButtonsWidth = 0.0f;
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 7.0f * scaledFontSize);
	float spacing = ImGui::GetStyle().ItemSpacing.x * scaleFactorY;
	std::vector<ImVec2> btnSizes;
	for (auto cheat : gameModeData.currentlyUsedCheats) {
		const std::string& btnLabel = gameModeData.cheatsNames[cheat];
		ImVec2 btnSize = ImGui::CalcTextSize(btnLabel.c_str());
		btnSize.x += ImGui::GetStyle().FramePadding.x * 2.0f * scaleFactorY;
		btnSize.y += ImGui::GetStyle().FramePadding.y * 2.0f * scaleFactorY;
		btnSizes.push_back(btnSize);
		totalButtonsWidth += btnSize.x;
	}
	if (!btnSizes.empty())
		totalButtonsWidth += spacing * (btnSizes.size() - 1);

	// Center the group
	float groupStartX = (windowSize.x - totalButtonsWidth) * 0.5f;
	ImGui::SetCursorPosX(groupStartX);

	// Draw buttons
	for (size_t i = 0; i < gameModeData.currentlyUsedCheats.size(); ++i) {
		auto cheat = gameModeData.currentlyUsedCheats[i];
		const std::string& btnLabel = gameModeData.cheatsNames[cheat];
		ImGui::Button(btnLabel.c_str(), btnSizes[i]);
		if (i + 1 < gameModeData.currentlyUsedCheats.size()) {
			ImGui::SameLine();
			ImGui::SameLine(0.0f, spacing); // Use calculated spacing
		}
	}

	ImGui::PopStyleVar();
	ImGui::PopStyleColor(3);
	ImGui::PopFont();
	ImGui::End();
}

void CheatHotkeysPopUpWindow() {
	// Define the window size and position
	ImVec2 windowSize = ImVec2(367.0f * scaleFactorX, 100.0f * scaleFactorY);
	float edgeOffsetX = (g_renderSize.x * 0.7f) - (windowSize.x * 0.5f);
	float edgeOffsetY = 15.0f * scaleFactorY;
	ImVec2 windowPos = ImVec2(edgeOffsetX, edgeOffsetY);
	auto& currentGameMode = activeCrimsonGameplay.GameMode.preset;

	ImGui::SetNextWindowSize(windowSize);
	ImGui::SetNextWindowPos(windowPos);
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;


	float fontSize = 23.0f;

	if (cheatsPopUp.popupTime <= 0) {
		cheatsPopUp.showPopUp = false;
		cheatsPopUp.popupTime = 2.0f;
	}

	if (!cheatsPopUp.showPopUp || !activeCrimsonConfig.GUI.cheatsPopup) {
		return;
	} else {
		cheatsPopUp.popupTime -= ImGui::GetIO().DeltaTime;
	}

	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[fontSize]);
	ImVec4 windowColor = ImColor(UI::SwapColorEndianness(gameModeData.colors[currentGameMode]));
	windowColor.w = 0.6f; // window opacity
	ImVec4 textColor = ImColor(UI::SwapColorEndianness(0xFFFFFFFF));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, windowColor);
	ImGui::PushStyleColor(ImGuiCol_Text, textColor);

	ImGui::Begin("CheatHotkeysPopUpWindow", nullptr, windowFlags);
	ImGui::SetWindowFontScale(scaleFactorY);
	// Calculate total width of the text
	float textWidth = ImGui::CalcTextSize(cheatsPopUp.cheatText).x;

	// Draw custom background with alpha
// 	ImDrawList* draw_list = ImGui::GetWindowDrawList();
// 	ImVec2 min = ImGui::GetWindowPos();
// 	ImVec2 max = ImVec2(min.x + windowSize.x, min.y + windowSize.y);
// 	draw_list->AddRectFilled(min, max, ImGui::ColorConvertFloat4ToU32(windowColor), 8.0f);

	// Center the text
	float textStartX = (windowSize.x - textWidth) * 0.5f;
	float textStartY = (windowSize.y - ImGui::GetFontSize()) * 0.2f;
	ImGui::SetCursorPosX(textStartX);
	ImGui::SetCursorPosY(textStartY);


	ImGui::Text(cheatsPopUp.cheatText);

	ImGui::PopStyleColor(2);
	ImGui::PopFont();
	ImGui::End();
}

void InitTextures(ID3D11Device* pd3dDevice) {
	InitRedOrbTexture(pd3dDevice);
	InitDStyleRankTextures(pd3dDevice);
}

}
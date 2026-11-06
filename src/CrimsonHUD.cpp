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
#include "Actor.hpp"
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
#include "UI\EmbeddedImages.hpp"
#include "CrimsonGUI.hpp"
#include "CrimsonFileHandling.hpp"
#include "CrimsonHUD.hpp"
#include "CrimsonGameplay.hpp"
#include "CrimsonFX.hpp"
#include "CrimsonSplitscreen.hpp"
#include "DebugDrawDX11.hpp"
#define DEBUG_DRAW_EXPLICIT_CONTEXT
#include "debug_draw.hpp"
#include <Backend/imgui_impl_dx11.cpp>

namespace CrimsonHUD {

using namespace CrimsonGUI;

// Keeps the current window's drawing inside the splitscreen view of playerIndex.
static bool PushSplitscreenViewClip(int playerIndex) {
	const ImVec2 size = ImGui::GetIO().DisplaySize;
	float minX, minY, maxX, maxY;
	if (!CrimsonSplitscreen::ViewRect(playerIndex, size.x, size.y, minX, minY, maxX, maxY)) {
		return false;
	}
	ImGui::PushClipRect(ImVec2(minX, minY), ImVec2(maxX, maxY), true);
	return true;
}

static void PopSplitscreenViewClip(bool pushed) {
	if (pushed) {
		ImGui::PopClipRect();
	}
}

// Keeps what the current window draws inside splitscreen view `view` (-1 = no clipping). Declare it right after
// ImGui::Begin; it works on the window's draw list, so it can outlive ImGui::End.
struct HudViewClipGuard {
	ImDrawList* list = nullptr;

	explicit HudViewClipGuard(int view) {
		if (view < 0) {
			return;
		}
		const ImVec2 size = ImGui::GetIO().DisplaySize;
		float minX, minY, maxX, maxY;
		if (CrimsonSplitscreen::ViewRect(view, size.x, size.y, minX, minY, maxX, maxY)) {
			list = ImGui::GetWindowDrawList();
			list->PushClipRect(ImVec2(minX, minY), ImVec2(maxX, maxY), true);
		}
	}
	~HudViewClipGuard() {
		if (list) {
			list->PopClipRect();
		}
	}
	HudViewClipGuard(const HudViewClipGuard&) = delete;
	HudViewClipGuard& operator=(const HudViewClipGuard&) = delete;
};


// RED ORB
static Texture2DD3D11* RedOrbTexture{ nullptr };
static Texture2DD3D11* RedOrbVanillaTexture{ nullptr };
static Texture2DD3D11* RedOrbStyleSwitcherTexture{ nullptr };
static Texture2DD3D11* RedOrbCrimsonTexture{ nullptr };
static Texture2DD3D11* RedOrbCustomTexture{ nullptr };


// STYLE RANK METER
static Texture2DD3D11* DStyleRankFillTexture{ nullptr };
static Texture2DD3D11* DStyleRankBackgroundTexture{ nullptr };
static Texture2DD3D11* DStyleRankBackground2Texture{ nullptr };
static Texture2DD3D11* DStyleRankBackgroundHighlightTexture{ nullptr };
static Texture2DD3D11* DStyleRankTextTexture{ nullptr };
static Texture2DD3D11* DStyleRankTextHighlightTexture{ nullptr };

static Texture2DD3D11* CStyleRankFillTexture{ nullptr };
static Texture2DD3D11* CStyleRankBackgroundTexture{ nullptr };
static Texture2DD3D11* CStyleRankBackground2Texture{ nullptr };
static Texture2DD3D11* CStyleRankBackgroundHighlightTexture{ nullptr };
static Texture2DD3D11* CStyleRankTextTexture{ nullptr };
static Texture2DD3D11* CStyleRankTextHighlightTexture{ nullptr };

static Texture2DD3D11* BStyleRankFillTexture{ nullptr };
static Texture2DD3D11* BStyleRankBackgroundTexture{ nullptr };
static Texture2DD3D11* BStyleRankBackground2Texture{ nullptr };
static Texture2DD3D11* BStyleRankBackgroundHighlightTexture{ nullptr };
static Texture2DD3D11* BStyleRankTextTexture{ nullptr };
static Texture2DD3D11* BStyleRankTextHighlightTexture{ nullptr };

static Texture2DD3D11* AStyleRankFillTexture{ nullptr };
static Texture2DD3D11* AStyleRankBackgroundTexture{ nullptr };
static Texture2DD3D11* AStyleRankBackground2Texture{ nullptr };
static Texture2DD3D11* AStyleRankBackgroundHighlightTexture{ nullptr };
static Texture2DD3D11* AStyleRankTextTexture{ nullptr };
static Texture2DD3D11* AStyleRankTextHighlightTexture{ nullptr };

static Texture2DD3D11* SStyleRankFillTexture{ nullptr };
static Texture2DD3D11* SStyleRankBackgroundTexture{ nullptr };
static Texture2DD3D11* SStyleRankBackground2Texture{ nullptr };
static Texture2DD3D11* SStyleRankBackgroundHighlightTexture{ nullptr };
static Texture2DD3D11* SStyleRankTextTexture{ nullptr };
static Texture2DD3D11* SStyleRankTextHighlightTexture{ nullptr };

static Texture2DD3D11* SSStyleRankFillTexture{ nullptr };
static Texture2DD3D11* SSStyleRankBackgroundTexture{ nullptr };
static Texture2DD3D11* SSStyleRankBackground2Texture{ nullptr };
static Texture2DD3D11* SSStyleRankBackgroundHighlightTexture{ nullptr };
static Texture2DD3D11* SSStyleRankTextTexture{ nullptr };
static Texture2DD3D11* SSStyleRankTextHighlightTexture{ nullptr };

static Texture2DD3D11* SSSStyleRankFillTexture{ nullptr };
static Texture2DD3D11* SSSStyleRankBackgroundTexture{ nullptr };
static Texture2DD3D11* SSSStyleRankBackground2Texture{ nullptr };
static Texture2DD3D11* SSSStyleRankBackgroundHighlightTexture{ nullptr };
static Texture2DD3D11* SSSStyleRankTextTexture{ nullptr };
static Texture2DD3D11* SSSStyleRankTextHighlightTexture{ nullptr };

// LOCK ON
static Texture2DD3D11* LockOnTexture{ nullptr };
static Texture2DD3D11* LockOnForegroundTexture{ nullptr };
static Texture2DD3D11* LockOnStunTexture{ nullptr };
static Texture2DD3D11* LockOnShieldTexture{ nullptr };

// STYLE GLASSES HUD
static Texture2DD3D11* tSmall{ nullptr };
static Texture2DD3D11* tBig{ nullptr };
static Texture2DD3D11* sSmall{ nullptr };
static Texture2DD3D11* sBig{ nullptr };
static Texture2DD3D11* gSmall{ nullptr };
static Texture2DD3D11* gBig{ nullptr };
static Texture2DD3D11* rSmall{ nullptr };
static Texture2DD3D11* rBig{ nullptr };
static Texture2DD3D11* dSmall{ nullptr };
static Texture2DD3D11* dBig{ nullptr };
static Texture2DD3D11* qSmall{ nullptr };
static Texture2DD3D11* qBig{ nullptr };
static Texture2DD3D11* diamondTGSR{ nullptr };

static Texture2DD3D11* darkslayerVergilGlass{ nullptr };
static Texture2DD3D11* darkslayerVergilBadge{ nullptr };
static Texture2DD3D11* quicksilverVergilGlass{ nullptr };
static Texture2DD3D11* quicksilverVergilBadge{ nullptr };
static Texture2DD3D11* doppelgangerGlass{ nullptr };
static Texture2DD3D11* doppelgangerBadge{ nullptr };
static Texture2DD3D11* gunslingerGlass{ nullptr };
static Texture2DD3D11* gunslingerBadge{ nullptr };
static Texture2DD3D11* quicksilverGlass{ nullptr };
static Texture2DD3D11* quicksilverBadge{ nullptr };
static Texture2DD3D11* royalguardGlass{ nullptr };
static Texture2DD3D11* royalguardBadge{ nullptr };
static Texture2DD3D11* swordmasterGlass{ nullptr };
static Texture2DD3D11* swordmasterBadge{ nullptr };
static Texture2DD3D11* tricksterGlass{ nullptr };
static Texture2DD3D11* tricksterBadge{ nullptr };

static Texture2DD3D11* doppelgangerText{ nullptr };
static Texture2DD3D11* doppelgangerTextBlur{ nullptr };
static Texture2DD3D11* gunslingerText{ nullptr };
static Texture2DD3D11* gunslingerTextBlur{ nullptr };
static Texture2DD3D11* quicksilverText{ nullptr };
static Texture2DD3D11* quicksilverTextBlur{ nullptr };
static Texture2DD3D11* royalguardText{ nullptr };
static Texture2DD3D11* royalguardTextBlur{ nullptr };
static Texture2DD3D11* swordmasterText{ nullptr };
static Texture2DD3D11* swordmasterTextBlur{ nullptr };
static Texture2DD3D11* tricksterText{ nullptr };
static Texture2DD3D11* tricksterTextBlur{ nullptr };

static Texture2DD3D11* styleExpBarBorder{ nullptr };
static Texture2DD3D11* styleExpBarBorderVergil{ nullptr };
static Texture2DD3D11* styleExpBarFill{ nullptr };
static Texture2DD3D11* styleLevelDiamond{ nullptr };

static Texture2DD3D11* royalGaugeCircle{ nullptr };

static Texture2DD3D11* mirageGaugeDpadTexture{ nullptr };
static Texture2DD3D11* mirageGaugeDpadActiveTexture{ nullptr };

static Texture2DD3D11* crimsonTitleGradient{ nullptr };

void InitRedOrbTexture(ID3D11Device* pd3dDevice) {
	//RedOrbTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "RedorbVanilla3.png").c_str(), pd3dDevice);
	RedOrbTexture = new Texture2DD3D11(g_Image_RedOrb.GetRGBAData(), g_Image_RedOrb.GetWidth(), g_Image_RedOrb.GetHeight(), pd3dDevice);
	RedOrbVanillaTexture = new Texture2DD3D11(g_Image_RedOrbVanilla.GetRGBAData(), g_Image_RedOrbVanilla.GetWidth(), g_Image_RedOrbVanilla.GetHeight(), pd3dDevice);
	RedOrbStyleSwitcherTexture = new Texture2DD3D11(g_Image_RedOrbStyleSwitcher.GetRGBAData(), g_Image_RedOrbStyleSwitcher.GetWidth(), g_Image_RedOrbStyleSwitcher.GetHeight(), pd3dDevice);
	RedOrbCrimsonTexture = new Texture2DD3D11(g_Image_RedOrbCrimson.GetRGBAData(), g_Image_RedOrbCrimson.GetWidth(), g_Image_RedOrbCrimson.GetHeight(), pd3dDevice);
	RedOrbCustomTexture = new Texture2DD3D11(g_Image_RedOrbCustom.GetRGBAData(), g_Image_RedOrbCustom.GetWidth(), g_Image_RedOrbCustom.GetHeight(), pd3dDevice);
	assert(RedOrbTexture);
}

void InitStyleRankTextures(ID3D11Device* pd3dDevice) {
	std::string styleRanksAccoladesPath = Paths::styleRanks + (std::string)"\\" + activeCrimsonConfig.CrimsonHudAddons.selectedStyleRanks + 
		"\\" + activeCrimsonConfig.CrimsonHudAddons.selectedStyleRanksAccolades;
	DStyleRankFillTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "Dfill.png").c_str(), pd3dDevice);
	DStyleRankBackgroundTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "Dbg.png").c_str(), pd3dDevice);
	DStyleRankBackground2Texture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "Dbg2.png").c_str(), pd3dDevice);
	DStyleRankBackgroundHighlightTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "Dbghighlight.png").c_str(), pd3dDevice);
	DStyleRankTextTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "Dtext.png").c_str(), pd3dDevice);
	DStyleRankTextHighlightTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "Dtexthighlight.png").c_str(), pd3dDevice);

	CStyleRankFillTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "Cfill.png").c_str(), pd3dDevice);
	CStyleRankBackgroundTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "Cbg.png").c_str(), pd3dDevice);
	CStyleRankBackground2Texture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "Cbg2.png").c_str(), pd3dDevice);
	CStyleRankBackgroundHighlightTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "Cbghighlight.png").c_str(), pd3dDevice);
	CStyleRankTextTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "Ctext.png").c_str(), pd3dDevice);
	CStyleRankTextHighlightTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "Ctexthighlight.png").c_str(), pd3dDevice);

	BStyleRankFillTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "Bfill.png").c_str(), pd3dDevice);
	BStyleRankBackgroundTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "Bbg.png").c_str(), pd3dDevice);
	BStyleRankBackground2Texture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "Bbg2.png").c_str(), pd3dDevice);
	BStyleRankBackgroundHighlightTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "Bbghighlight.png").c_str(), pd3dDevice);
	BStyleRankTextTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "Btext.png").c_str(), pd3dDevice);
	BStyleRankTextHighlightTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "Btexthighlight.png").c_str(), pd3dDevice);

	AStyleRankFillTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "Afill.png").c_str(), pd3dDevice);
	AStyleRankBackgroundTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "Abg.png").c_str(), pd3dDevice);
	AStyleRankBackground2Texture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "Abg2.png").c_str(), pd3dDevice); 
	AStyleRankBackgroundHighlightTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "Abghighlight.png").c_str(), pd3dDevice);
	AStyleRankTextTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "Atext.png").c_str(), pd3dDevice);
	AStyleRankTextHighlightTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "Atexthighlight.png").c_str(), pd3dDevice);

	SStyleRankFillTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "Sfill.png").c_str(), pd3dDevice);
	SStyleRankBackgroundTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "Sbg.png").c_str(), pd3dDevice);
	SStyleRankBackground2Texture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "Sbg2.png").c_str(), pd3dDevice);
	SStyleRankBackgroundHighlightTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "Sbghighlight.png").c_str(), pd3dDevice);
	SStyleRankTextTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "Stext.png").c_str(), pd3dDevice);
	SStyleRankTextHighlightTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "Stexthighlight.png").c_str(), pd3dDevice);

	SSStyleRankFillTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "SSfill.png").c_str(), pd3dDevice);
	SSStyleRankBackgroundTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "SSbg.png").c_str(), pd3dDevice);
	SSStyleRankBackground2Texture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "SSbg2.png").c_str(), pd3dDevice);
	SSStyleRankBackgroundHighlightTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "SSbghighlight.png").c_str(), pd3dDevice);
	SSStyleRankTextTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "SStext.png").c_str(), pd3dDevice);
	SSStyleRankTextHighlightTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "SStexthighlight.png").c_str(), pd3dDevice);

	SSSStyleRankFillTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "SSSfill.png").c_str(), pd3dDevice);
	SSSStyleRankBackgroundTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "SSSbg.png").c_str(), pd3dDevice);
	SSSStyleRankBackground2Texture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "SSSbg2.png").c_str(), pd3dDevice);
	SSSStyleRankBackgroundHighlightTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "SSSbghighlight.png").c_str(), pd3dDevice);
	SSSStyleRankTextTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "SSStext.png").c_str(), pd3dDevice);
	SSSStyleRankTextHighlightTexture = new Texture2DD3D11(((std::string)styleRanksAccoladesPath + "\\" + "SSStexthighlight.png").c_str(), pd3dDevice);

	assert(DStyleRankFillTexture);
	assert(DStyleRankBackgroundTexture);
	assert(DStyleRankBackground2Texture);
	assert(DStyleRankBackgroundHighlightTexture);
	assert(DStyleRankTextTexture);
	assert(DStyleRankTextHighlightTexture);
	assert(CStyleRankFillTexture);
	assert(CStyleRankBackgroundTexture);
	assert(CStyleRankBackground2Texture);
	assert(CStyleRankBackgroundHighlightTexture);
	assert(CStyleRankTextTexture);
	assert(CStyleRankTextHighlightTexture);
	assert(BStyleRankFillTexture);
	assert(BStyleRankBackgroundTexture);
	assert(BStyleRankBackground2Texture);
	assert(BStyleRankBackgroundHighlightTexture);
	assert(BStyleRankTextTexture);
	assert(BStyleRankTextHighlightTexture);
	assert(AStyleRankFillTexture);
	assert(AStyleRankBackgroundTexture);
	assert(AStyleRankBackground2Texture);
	assert(AStyleRankBackgroundHighlightTexture);
	assert(AStyleRankTextTexture);
	assert(AStyleRankTextHighlightTexture);
	assert(SStyleRankFillTexture);
	assert(SStyleRankBackgroundTexture);
	assert(SStyleRankBackground2Texture);
	assert(SStyleRankBackgroundHighlightTexture);
	assert(SStyleRankTextTexture);
	assert(SStyleRankTextHighlightTexture);
	assert(SSStyleRankFillTexture);
	assert(SSStyleRankBackgroundTexture);
	assert(SSStyleRankBackground2Texture);
	assert(SSStyleRankBackgroundHighlightTexture);
	assert(SSStyleRankTextTexture);
	assert(SSStyleRankTextHighlightTexture);
	assert(SSSStyleRankFillTexture);
	assert(SSSStyleRankBackgroundTexture);
	assert(SSSStyleRankBackground2Texture);
	assert(SSSStyleRankBackgroundHighlightTexture);
	assert(SSSStyleRankTextTexture);
	assert(SSSStyleRankTextHighlightTexture);
}

void InitLockOnTexture(ID3D11Device* pd3dDevice) {
	LockOnTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "lockon.png").c_str(), pd3dDevice);
	LockOnForegroundTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "lockonforeground.png").c_str(), pd3dDevice);
	LockOnStunTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "lockonstun.png").c_str(), pd3dDevice);
	LockOnShieldTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "lockonshield.png").c_str(), pd3dDevice);
	assert(LockOnTexture);
	assert(LockOnForegroundTexture);
	assert(LockOnStunTexture);
	assert(LockOnShieldTexture);
}

void InitStyleGlassTextures(ID3D11Device* pd3dDevice) {
	tSmall = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\tgrs\\tSmall.png").c_str(), pd3dDevice);
	tBig = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\tgrs\\tBig.png").c_str(), pd3dDevice);
	sSmall = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\tgrs\\sSmall.png").c_str(), pd3dDevice);
	sBig = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\tgrs\\sBig.png").c_str(), pd3dDevice);
	gSmall = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\tgrs\\gSmall.png").c_str(), pd3dDevice);
	gBig = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\tgrs\\gBig.png").c_str(), pd3dDevice);
	rSmall = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\tgrs\\rSmall.png").c_str(), pd3dDevice);
	rBig = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\tgrs\\rBig.png").c_str(), pd3dDevice);
	dSmall = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\tgrs\\dSmall.png").c_str(), pd3dDevice);
	dBig = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\tgrs\\dBig.png").c_str(), pd3dDevice);
	qSmall = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\tgrs\\qSmall.png").c_str(), pd3dDevice);
	qBig = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\tgrs\\qBig.png").c_str(), pd3dDevice);
	diamondTGSR = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\tgrs\\diamond.png").c_str(), pd3dDevice);

	darkslayerVergilGlass = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\styles\\darkslayer-vergil-glass.png").c_str(), pd3dDevice);
	darkslayerVergilBadge = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\styles\\darkslayer-vergil-badge.png").c_str(), pd3dDevice);
	quicksilverVergilGlass = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\styles\\quicksilver-vergil-glass.png").c_str(), pd3dDevice);
	quicksilverVergilBadge = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\styles\\quicksilver-vergil-badge.png").c_str(), pd3dDevice);
	doppelgangerGlass = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\styles\\doppelganger-glass.png").c_str(), pd3dDevice);
	doppelgangerBadge = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\styles\\doppelganger-badge.png").c_str(), pd3dDevice);
	gunslingerGlass = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\styles\\gunslinger-glass.png").c_str(), pd3dDevice);
	gunslingerBadge = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\styles\\gunslinger-badge.png").c_str(), pd3dDevice);
	quicksilverGlass = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\styles\\quicksilver-glass.png").c_str(), pd3dDevice);
	quicksilverBadge = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\styles\\quicksilver-badge.png").c_str(), pd3dDevice);
	royalguardGlass = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\styles\\royalguard-glass.png").c_str(), pd3dDevice);
	royalguardBadge = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\styles\\royalguard-badge.png").c_str(), pd3dDevice);
	swordmasterGlass = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\styles\\swordmaster-glass.png").c_str(), pd3dDevice);
	swordmasterBadge = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\styles\\swordmaster-badge.png").c_str(), pd3dDevice);
	tricksterGlass = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\styles\\trickster-glass.png").c_str(), pd3dDevice);
	tricksterBadge = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\styles\\trickster-badge.png").c_str(), pd3dDevice);

	styleExpBarBorder = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\exp\\style-exp-border.png").c_str(), pd3dDevice);
	styleExpBarBorderVergil = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\exp\\style-exp-border-vergil.png").c_str(), pd3dDevice);
	styleExpBarFill = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\exp\\style-exp-inside.png").c_str(), pd3dDevice);
	styleLevelDiamond = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\exp\\diamondLevel.png").c_str(), pd3dDevice);

	royalGaugeCircle = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\royal\\gauge.png").c_str(), pd3dDevice);

	assert(tSmall);
	assert(tBig);
	assert(sSmall);
	assert(sBig);
	assert(gSmall);
	assert(gBig);
	assert(rSmall);
	assert(rBig);
	assert(dSmall);
	assert(dBig);
	assert(qSmall);
	assert(qBig);
	assert(diamondTGSR);
	assert(darkslayerVergilGlass);
	assert(darkslayerVergilBadge);
	assert(quicksilverVergilGlass);
	assert(quicksilverVergilBadge);
	assert(doppelgangerGlass);
	assert(doppelgangerBadge);
	assert(gunslingerGlass);
	assert(gunslingerBadge);
	assert(quicksilverGlass);
	assert(quicksilverBadge);
	assert(royalguardGlass);
	assert(royalguardBadge);
	assert(swordmasterGlass);
	assert(swordmasterBadge);
	assert(tricksterGlass);
	assert(tricksterBadge);

	InitStyleNameTextures(pd3dDevice);
	assert(quicksilverText);
	assert(quicksilverTextBlur);
	assert(royalguardText);
	assert(royalguardTextBlur);
	assert(swordmasterText);
	assert(swordmasterTextBlur);
	assert(tricksterText);
	assert(tricksterTextBlur);
	assert(styleExpBarBorder);
	assert(styleExpBarBorderVergil);
	assert(styleExpBarFill);
	assert(styleLevelDiamond);
	assert(royalGaugeCircle);
}

static std::string GetStyleNamesBasePath() {
	if (activeCrimsonConfig.CrimsonHudAddons.styleNamesSet == STYLENAMESSET::DMC3_SWITCH) {
		return (std::string)Paths::assets + "\\crimsonhud\\stylenames_switch";
	}

	return (std::string)Paths::assets + "\\crimsonhud\\stylenames_crimson";
}

void InitStyleNameTextures(ID3D11Device* pd3dDevice) {
	std::string styleNamesPath = GetStyleNamesBasePath();

	doppelgangerText = new Texture2DD3D11(((std::string)styleNamesPath + "\\doppelganger.png").c_str(), pd3dDevice);
	doppelgangerTextBlur = new Texture2DD3D11(((std::string)styleNamesPath + "\\doppelganger-blur.png").c_str(), pd3dDevice);
	gunslingerText = new Texture2DD3D11(((std::string)styleNamesPath + "\\gunslinger.png").c_str(), pd3dDevice);
	gunslingerTextBlur = new Texture2DD3D11(((std::string)styleNamesPath + "\\gunslinger-blur.png").c_str(), pd3dDevice);
	quicksilverText = new Texture2DD3D11(((std::string)styleNamesPath + "\\quicksilver.png").c_str(), pd3dDevice);
	quicksilverTextBlur = new Texture2DD3D11(((std::string)styleNamesPath + "\\quicksilver-blur.png").c_str(), pd3dDevice);
	royalguardText = new Texture2DD3D11(((std::string)styleNamesPath + "\\royalguard.png").c_str(), pd3dDevice);
	royalguardTextBlur = new Texture2DD3D11(((std::string)styleNamesPath + "\\royalguard-blur.png").c_str(), pd3dDevice);
	swordmasterText = new Texture2DD3D11(((std::string)styleNamesPath + "\\swordmaster.png").c_str(), pd3dDevice);
	swordmasterTextBlur = new Texture2DD3D11(((std::string)styleNamesPath + "\\swordmaster-blur.png").c_str(), pd3dDevice);
	tricksterText = new Texture2DD3D11(((std::string)styleNamesPath + "\\trickster.png").c_str(), pd3dDevice);
	tricksterTextBlur = new Texture2DD3D11(((std::string)styleNamesPath + "\\trickster-blur.png").c_str(), pd3dDevice);

	assert(doppelgangerText);
	assert(doppelgangerTextBlur);
	assert(gunslingerText);
	assert(gunslingerTextBlur);
	assert(quicksilverText);
	assert(quicksilverTextBlur);
	assert(royalguardText);
	assert(royalguardTextBlur);
	assert(swordmasterText);
	assert(swordmasterTextBlur);
	assert(tricksterText);
	assert(tricksterTextBlur);
}

void InitMirageGaugeTextures(ID3D11Device* pd3dDevice) {
	mirageGaugeDpadTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\miragegauge\\dpad.png").c_str(), pd3dDevice);
	mirageGaugeDpadActiveTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\miragegauge\\dpad-active.png").c_str(), pd3dDevice);
	assert(mirageGaugeDpadTexture);
	assert(mirageGaugeDpadActiveTexture);
}

void InitGradientTextures(ID3D11Device* pd3dDevice) {
	crimsonTitleGradient = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "CrimsonGradient.png").c_str(), pd3dDevice);
	assert(crimsonTitleGradient);
}


void RenderMeterWithFill(ImTextureID texture, ImVec2 pos, ImVec2 size, float fillRatio, ImColor color, float angle = 0.0f) {
	fillRatio = ImClamp(fillRatio, 0.0f, 1.0f);
	float visibleHeight = size.y * fillRatio;
	ImVec2 fillPos = ImVec2(pos.x, pos.y + size.y - visibleHeight);
	ImVec2 uv0 = ImVec2(0.0f, 1.0f - fillRatio);
	ImVec2 uv1 = ImVec2(1.0f, 1.0f);

	// Center of the fill area
	ImVec2 center = ImVec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f);

	// Four corners of the filled rect
	ImVec2 corners[4] = {
		ImVec2(fillPos.x, fillPos.y),                         // Bottom-left
		ImVec2(fillPos.x + size.x, fillPos.y),                // Bottom-right
		ImVec2(pos.x + size.x, pos.y + size.y),               // Top-right
		ImVec2(pos.x, pos.y + size.y)                         // Top-left
	};

	// UVs for the filled area
	ImVec2 uvs[4] = {
		ImVec2(uv0.x, uv0.y),
		ImVec2(uv1.x, uv0.y),
		ImVec2(uv1.x, uv1.y),
		ImVec2(uv0.x, uv1.y)
	};

	// Rotate corners around center if angle != 0
	if (angle != 0.0f) {
		float s = sinf(angle), c = cosf(angle);
		for (int i = 0; i < 4; ++i) {
			ImVec2 p = corners[i] - center;
			corners[i] = ImVec2(
				center.x + p.x * c - p.y * s,
				center.y + p.x * s + p.y * c
			);
		}
	}

	ImDrawList* drawList = ImGui::GetWindowDrawList();
	drawList->AddImageQuad(
		texture,
		corners[0], corners[1], corners[2], corners[3],
		uvs[0], uvs[1], uvs[2], uvs[3],
		color
	);
}

void RenderExpWithFill(ImTextureID texture, ImVec2 pos, ImVec2 size, float fillRatio, ImColor color, float angle = 0.0f) {
	fillRatio = ImClamp(fillRatio, 0.0f, 1.0f);
	float visibleHeight = size.y * fillRatio;
	ImVec2 fillPos = ImVec2(pos.x, pos.y); // Start at the top
	ImVec2 uv0 = ImVec2(0.0f, 0.0f);
	ImVec2 uv1 = ImVec2(1.0f, fillRatio);

	// Center of the fill area
	ImVec2 center = ImVec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f);

	// Four corners of the filled rect (top to bottom)
	ImVec2 corners[4] = {
		ImVec2(fillPos.x, fillPos.y),                         // Top-left
		ImVec2(fillPos.x + size.x, fillPos.y),                // Top-right
		ImVec2(pos.x + size.x, pos.y + visibleHeight),        // Bottom-right
		ImVec2(pos.x, pos.y + visibleHeight)                  // Bottom-left
	};

	// UVs for the filled area
	ImVec2 uvs[4] = {
		ImVec2(uv0.x, uv0.y),
		ImVec2(uv1.x, uv0.y),
		ImVec2(uv1.x, uv1.y),
		ImVec2(uv0.x, uv1.y)
	};

	// Rotate corners around center if angle != 0
	if (angle != 0.0f) {
		float s = sinf(angle), c = cosf(angle);
		for (int i = 0; i < 4; ++i) {
			ImVec2 p = corners[i] - center;
			corners[i] = ImVec2(
				center.x + p.x * c - p.y * s,
				center.y + p.x * s + p.y * c
			);
		}
	}

	ImDrawList* drawList = ImGui::GetWindowDrawList();
	drawList->AddImageQuad(
		texture,
		corners[0], corners[1], corners[2], corners[3],
		uvs[0], uvs[1], uvs[2], uvs[3],
		color
	);
}

inline float StyleHudHybridPositioningOffsetX() {
	return (440.0f - 395.0f) * scaleFactorY;
}

// HUD adjustments (CrimsonSplitscreen single player / splitscreen HUD windows) applied to the vanilla top-left cockpit.
// The style icon/name addons sit on top of it, so they get the same position and size transform.
struct TopLeftHudSync {
	float scale = 1.0f;    // sizes
	float posScale = 1.0f; // positions
	ImVec2 offset = ImVec2(0.0f, 0.0f);

	ImVec2 Pos(ImVec2 p) const {
		return ImVec2(p.x * posScale + offset.x, p.y * posScale + offset.y);
	}
};

// view = splitscreen view (player index), -1 = the whole screen.
static TopLeftHudSync GetTopLeftHudSync(int view) {
	TopLeftHudSync sync;
	HUD_AddonTransform(view, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y,
		sync.posScale, sync.scale, sync.offset.x, sync.offset.y);
	return sync;
}

// The style addons draw once per splitscreen view, once per multiplayer cockpit (shared / single player camera), or
// once on the whole screen.
template <typename F>
static void ForEachHudView(F&& draw) {
	int views = CrimsonSplitscreen::ViewCount();
	if (views < 2) {
		views = HUD_SeatCount();
	}
	if (views < 2) {
		draw(-1);
		return;
	}
	for (int view = 0; view < views; ++view) {
		draw(view);
	}
}

static PlayerActorData* HudViewActor(int view) {
	if (view < 0) {
		auto pool_10222 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
		return (pool_10222 && pool_10222[3]) ? reinterpret_cast<PlayerActorData*>(pool_10222[3]) : nullptr;
	}
	auto& playerData = GetPlayerData(static_cast<uint8>(view));
	auto& actorData = GetNewActorData(static_cast<uint8>(view), playerData.activeCharacterIndex, ENTITY::MAIN);
	return actorData.baseAddr ? reinterpret_cast<PlayerActorData*>(actorData.baseAddr) : nullptr;
}

static const char* HudViewWindowName(const char* name, int view) {
	if (view < 0) {
		return name;
	}
	static char buffers[16][64];
	static int next = 0;
	char* buffer = buffers[next++ & 15];
	snprintf(buffer, 64, "%s##view%d", name, view);
	return buffer;
}

void RenderTexture(ImTextureID texture, ImVec2 pos, ImVec2 size, ImColor color, float angle = 0.0f) {
	ImVec2 center = ImVec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f);

	ImVec2 corners[4] = {
		ImVec2(pos.x, pos.y),
		ImVec2(pos.x + size.x, pos.y),
		ImVec2(pos.x + size.x, pos.y + size.y),
		ImVec2(pos.x, pos.y + size.y)
	};

	ImVec2 uvs[4] = {
		ImVec2(0, 0),
		ImVec2(1, 0),
		ImVec2(1, 1),
		ImVec2(0, 1)
	};

	if (angle != 0.0f) {
		float s = sinf(angle), c = cosf(angle);
		for (int i = 0; i < 4; ++i) {
			ImVec2 p = corners[i] - center;
			corners[i] = ImVec2(
				center.x + p.x * c - p.y * s,
				center.y + p.x * s + p.y * c
			);
		}
	}

	ImDrawList* drawList = ImGui::GetWindowDrawList();
	drawList->AddImageQuad(
		texture,
		corners[0], corners[1], corners[2], corners[3],
		uvs[0], uvs[1], uvs[2], uvs[3],
		color
	);
}

void RenderSkewedMeterWithFill(ImTextureID texture, ImVec2 pos, ImVec2 size, float fillRatio, ImColor color, float angle, float skewT) {
	fillRatio = ImClamp(fillRatio, 0.0f, 1.0f);
	float visibleHeight = size.y * fillRatio;

	// EXTREME skew and pointiness
	float skewAmount = (1.0f - skewT) * 4.2f;

	ImVec2 fillPos = ImVec2(pos.x, pos.y + size.y - visibleHeight);
	float fillTop = fillPos.y;
	float fillBottom = pos.y + size.y;

	ImVec2 center = ImVec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f);

	ImVec2 corners[4] = {
		ImVec2(pos.x + size.x * skewAmount * 2.0f, fillTop),                                   // Top-left - far right when animating
		ImVec2(pos.x + size.x + size.x * skewAmount, fillTop),                                 // Top-right - even further right
		ImVec2(pos.x + size.x, fillBottom),                                                     // Bottom-right - normal
		ImVec2(pos.x + size.x * skewAmount, fillBottom)                                        // Bottom-left - also pulled right
	};

	float uvFill = 1.0f - fillRatio;
	ImVec2 uvs[4] = {
		ImVec2(0, uvFill),
		ImVec2(1, uvFill),
		ImVec2(1, 1),
		ImVec2(0, 1)
	};

	if (angle != 0.0f) {
		float s = sinf(angle), c = cosf(angle);
		for (int i = 0; i < 4; ++i) {
			ImVec2 p = corners[i] - center;
			corners[i] = ImVec2(
				center.x + p.x * c - p.y * s,
				center.y + p.x * s + p.y * c
			);
		}
	}

	ImGui::GetWindowDrawList()->AddImageQuad(
		texture,
		corners[0], corners[1], corners[2], corners[3],
		uvs[0], uvs[1], uvs[2], uvs[3],
		color
	);
}

void RenderSkewedTexture(ImTextureID texture, ImVec2 pos, ImVec2 size, ImColor color, float angle, float skewT) {
	ImVec2 center = pos + size * 0.5f;
	float skewAmount = (1.0f - skewT) * 4.2f;

	ImVec2 corners[4] = {
		ImVec2(pos.x + size.x * skewAmount * 2.0f, pos.y),                             // Top-left - far right when animating
		ImVec2(pos.x + size.x + size.x * skewAmount, pos.y),                           // Top-right - even further right
		ImVec2(pos.x + size.x, pos.y + size.y),                                        // Bottom-right - normal
		ImVec2(pos.x + size.x * skewAmount, pos.y + size.y)                            // Bottom-left - also pulled right
	};

	if (angle != 0.0f) {
		float s = sinf(angle), c = cosf(angle);
		for (int i = 0; i < 4; ++i) {
			ImVec2 p = corners[i] - center;
			corners[i] = ImVec2(
				center.x + p.x * c - p.y * s,
				center.y + p.x * s + p.y * c
			);
		}
	}

	ImVec2 uvs[4] = {
		ImVec2(0, 0),
		ImVec2(1, 0),
		ImVec2(1, 1),
		ImVec2(0, 1)
	};

	ImGui::GetWindowDrawList()->AddImageQuad(
		texture,
		corners[0], corners[1], corners[2], corners[3],
		uvs[0], uvs[1], uvs[2], uvs[3],
		color
	);
}

struct StyleMeterAnimState {
	FadeState fade;
	bool prevActive = false;
	float animTimer = 0.0f;
	bool animating = false;
};

// Global previous style rank for entrance animation logic (one per view, index view + 1)
static int prevGlobalStyleRanks[PLAYER_COUNT + 1] = { -1, -1, -1, -1, -1 };
static constexpr float kStyleMeterScale = 0.76f;
static float g_styleMeterOutOfCombatTimer = 0.0f;
static bool g_styleMeterForceHidden = false;
static float g_stylishPtsOutOfCombatTimer = 0.0f;
// The view the style meter windows are being drawn for (-1 = the whole screen), see StyleMeterWindows.
static int s_styleView = -1;

// Position / size transform of the style meter of a view (HUD.cpp); identity when none applies.
static TopLeftHudSync GetStyleMeterSync(int view) {
	TopLeftHudSync sync;
	HUD_StyleMeterTransform(view, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y,
		sync.posScale, sync.scale, sync.offset.x, sync.offset.y);
	return sync;
}

// The style data every view shows. The game's style meter is not per player yet: StyleMeterMultiplayer (CrimsonOnTick.cpp)
// merges the highest rank / meter / points of all players into the main player's actor, so every view reads that one and
// all the meters rise together. Switch to the view's own actor once the style meter logic is per player.
static PlayerActorData* StyleMeterSourceActor(int /*view*/) {
	return HudViewActor(-1);
}

void StyleMeterWindowRank(
	int currentRank,
	ImTextureID fillTexture,
	ImTextureID bgTexture,
	ImTextureID bgHighlightTexture,
	ImTextureID textTexture,
	ImTextureID textHighlightTexture,
	uint32 auraColor,
	uint32 fillColor,
	uint32 textColor,
	float animIntensity = 1.0f,
	ImVec2 windowPos = ImVec2(g_uiSize.x - (480.0 * scaleFactorY), 190.0f * scaleFactorY),
	bool sSStyle = false,
	bool sSSStyle = false,
	ImTextureID bgTexture2 = nullptr
) {
	// Use a static map to store animation state per rank (and per view)
	static std::unordered_map<int, StyleMeterAnimState> animStatesPerView[PLAYER_COUNT + 1];
	StyleMeterAnimState& state = animStatesPerView[s_styleView + 1][currentRank];
	int& prevGlobalStyleRank = prevGlobalStyleRanks[s_styleView + 1];

	const float animDuration = 0.32f;
	const float textDelay = 0.64f;
	const float textAnimDuration = 0.22f;
	const float flashDuration = 0.6f;

	float fillRatio = 0.0f;
	bool active = false;

	if (!fillTexture || !bgTexture || !textTexture || !textHighlightTexture)
		return;

	if (InGame() && !g_inGameCutscene) {
		if (auto* mainActorPtr = StyleMeterSourceActor(s_styleView)) {
			auto& mainActorData = *mainActorPtr;
			if (activeCrimsonConfig.CrimsonHudAddons.styleRanksMeter) {
				auto& styleData = mainActorData.styleData;
				if (styleData.rank == currentRank) {
					fillRatio = styleData.meter / 700.0f;
					active = fillRatio > 0.0f;
				}
			}
		}
	}

// 	if (g_styleMeterForceHidden) {
// 		active = false;
// 		fillRatio = 0.0f;
// 		state.animating = false;
// 	}

	if (activeCrimsonGameplay.Gameplay.ExtraDifficulty.mustStyleMode > 0) {
		if (currentRank < activeCrimsonGameplay.Gameplay.ExtraDifficulty.mustStyleMode) {
			auraColor = 0xccccccFF;
			fillColor = 0xccccccFF;
			textColor = 0xccccccFF;
		} else {
			auraColor = 0xfff727FF;
			fillColor = 0xfffdb9FF;
			textColor = 0xece400FF;
		}
	}

	float deltaTime = ImGui::GetIO().DeltaTime;
	float targetAlpha = active ? 1.0f : 0.0f;
	float fadeSpeed = 8.0f;
	state.fade.alpha = SmoothLerp(state.fade.alpha, targetAlpha, fadeSpeed, deltaTime);

	// Animation trigger: only when going up in rank (including rank 0 to rank 1)
	if (!state.prevActive && active && currentRank > prevGlobalStyleRank) {
		state.animTimer = 0.0f;
		state.animating = true;
	}
	state.prevActive = active;

	// Update the global previous style rank if this rank is active
	// For rank transitions, set to 0 when no rank is active to allow rank 1 animation
	if (active) {
		prevGlobalStyleRank = currentRank;
	} else if (prevGlobalStyleRank == currentRank) {
		// When this rank becomes inactive, reset to 0 to allow proper rank 1 animation
		prevGlobalStyleRank = 0;
	}

	// --- Animation update ---
	if (state.animating) {
		state.animTimer += deltaTime;
		if (state.animTimer > animDuration + textDelay + textAnimDuration)
			state.animating = false;
	}

	if (state.fade.alpha <= 0.01f && !state.animating)
		return;

	windowPos.x += 50.0f * scaleFactorY;

	// Per player placement (HUD.cpp): move / scale about the top-right corner of the player's screen or view. Everything
	// below is sized with the scale that includes it.
	const TopLeftHudSync styleSync = GetStyleMeterSync(s_styleView);
	windowPos = styleSync.Pos(windowPos);
	const float scaleFactorY = CrimsonGUI::scaleFactorY * styleSync.scale;

	ImVec2 baseMeterSize = ImVec2(301.0f * scaleFactorY * 0.95f, 303.0f * scaleFactorY * 0.95f);
	ImVec2 meterSize = ImVec2(baseMeterSize.x * kStyleMeterScale, baseMeterSize.y * kStyleMeterScale);
	ImColor white = { 1.0f, 1.0f, 1.0f, state.fade.alpha };
	ImVec4 colorHex = ImGui::ColorConvertU32ToFloat4(UI::SwapColorEndianness(auraColor));
	ImColor color = (colorHex);
	float h, s, v;
	ImGui::ColorConvertRGBtoHSV(color.Value.x, color.Value.y, color.Value.z, h, s, v);

	s = ImClamp(s * 1.0f, 0.0f, 1.0f);
	v = ImClamp(v * 1.3f, 0.0f, 1.0f);

	ImVec4 poppedColor;
	ImGui::ColorConvertHSVtoRGB(h, s, v, poppedColor.x, poppedColor.y, poppedColor.z);
	poppedColor.w = state.fade.alpha;

	// --- Flash color logic ---
	float flashT = 0.0f;
	if (state.animating && state.animTimer < flashDuration) {
		flashT = 1.0f - (state.animTimer / flashDuration);
		flashT = ImClamp(flashT, 0.0f, 1.0f);
	}
	ImVec4 poppedColorV = poppedColor;
	ImVec4 whiteV = ImVec4(1.0f, 1.0f, 1.0f, state.fade.alpha);
	ImVec4 flashBgV = ImLerp(poppedColorV, whiteV, flashT);
	ImColor flashBg(flashBgV);

	ImColor fillColorC = ImColor(ImGui::ColorConvertU32ToFloat4(UI::SwapColorEndianness(fillColor)));
	if (state.animating && state.animTimer < animDuration) {
		fillColorC = ImColor(ImLerp(poppedColorV, whiteV, flashT));
	}
	fillColorC.Value.w = state.fade.alpha;

	ImColor highlightColor = white;
	if (state.animating && state.animTimer < animDuration) {
		highlightColor = ImColor(ImLerp(white, whiteV, flashT));
	}
	highlightColor.Value.w = state.fade.alpha;

	float extraLeft = (g_uiSize.y - 100.0f * scaleFactorY);
	ImVec2 adjustedWindowPos = ImVec2(windowPos.x - extraLeft, windowPos.y);
	ImVec2 adjustedWindowSize = meterSize + ImVec2(g_uiSize.x, g_uiSize.y);

	ImGui::SetNextWindowPos(adjustedWindowPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(adjustedWindowSize, ImGuiCond_Always);

	ImGui::Begin(HudViewWindowName("StyleMeter", s_styleView), nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoBackground);
	HudViewClipGuard meterClip(s_styleView);

	float animScale = 1.0f;
	float animAngle = 0.0f;
	float skewT = 1.0f;
	ImVec2 animOffset(0, 0);
	if (state.animating && state.animTimer < animDuration) {
		float t = ImClamp(state.animTimer / animDuration, 0.0f, 1.0f);

		auto easeInOut = [](float t) {
			return t < 0.5f ? 4.0f * t * t * t : 1.0f - powf(-2.0f * t + 2.0f, 3.0f) / 2.0f;
			};

		float phase1 = 0.55f;
		float centerDist = meterSize.x * animIntensity;

		if (t < phase1) {
			float t1 = easeInOut(t / phase1);
			animOffset.x = centerDist * (1.0f - t1);
			skewT = t1 * 0.5f;
			animScale = 2.8f - 1.2f * t1;
			animAngle = (1.0f - t1) * 0.18f * sinf(t1 * 8.0f);
		} else {
			float t2 = easeInOut((t - phase1) / (1.0f - phase1));
			animOffset.x = ImLerp(0.0f, 0.0f, t2);
			skewT = 0.5f + t2 * 0.5f;
			animScale = 1.6f - 0.6f * t2;
			animAngle = (1.0f - t2) * 0.08f * sinf(t2 * 8.0f);
		}

		if (t > 0.96f) {
			float snapT = (t - 0.96f) / 0.04f;
			animScale = ImLerp(animScale, 1.0f, snapT);
			animAngle = ImLerp(animAngle, 0.0f, snapT);
			animOffset.x = ImLerp(animOffset.x, 0.0f, snapT);
			skewT = ImLerp(skewT, 1.0f, snapT);
		}
	}

	ImVec2 center = ImGui::GetCursorScreenPos() + meterSize * 0.5f + animOffset;
	ImVec2 renderSize = meterSize * animScale;
	ImVec2 renderPos = center - renderSize * 0.5f;
	ImVec2 renderPosExtraLeft = ImVec2(renderPos.x + extraLeft, renderPos.y);
	ImVec2 actualRenderPos = renderPosExtraLeft;

	ImVec2 renderPosSSStyle = ImVec2(renderPos.x - (50.0f * scaleFactorY * kStyleMeterScale), renderPos.y + (50.0f * scaleFactorY * kStyleMeterScale));
	ImVec2 renderPosSSStyleExtraLeft = ImVec2(renderPosSSStyle.x + extraLeft, renderPosSSStyle.y);
	ImVec2 actualRenderPosSSStyle = renderPosSSStyleExtraLeft;
	ImVec2 renderPosSSSStyle = ImVec2(renderPos.x - (100.0f * scaleFactorY * kStyleMeterScale), renderPos.y + (20.0f * scaleFactorY * kStyleMeterScale));
	ImVec2 renderPosSSSStyleExtraLeft = ImVec2(renderPosSSSStyle.x + extraLeft, renderPosSSSStyle.y);
	ImVec2 actualRenderPosSSSStyle = renderPosSSSStyleExtraLeft;

	if (sSSStyle) {
		if (bgTexture2) {
			RenderSkewedTexture(bgTexture2, actualRenderPosSSSStyle, renderSize, flashBg, animAngle, skewT);
		}
		RenderSkewedMeterWithFill(fillTexture, actualRenderPosSSSStyle, renderSize, fillRatio, fillColorC, animAngle, skewT);
		RenderSkewedTexture(bgTexture, actualRenderPosSSSStyle, renderSize, flashBg, animAngle, skewT);
		RenderSkewedTexture(bgHighlightTexture, actualRenderPosSSSStyle, renderSize, highlightColor, animAngle, skewT);
	}

	if (sSStyle || sSSStyle) {
		if (bgTexture2) {
			RenderSkewedTexture(bgTexture2, actualRenderPosSSStyle, renderSize, flashBg, animAngle, skewT);
		}
		RenderSkewedMeterWithFill(fillTexture, actualRenderPosSSStyle, renderSize, fillRatio, fillColorC, animAngle, skewT);
		RenderSkewedTexture(bgTexture, actualRenderPosSSStyle, renderSize, flashBg, animAngle, skewT);
		RenderSkewedTexture(bgHighlightTexture, actualRenderPosSSStyle, renderSize, highlightColor, animAngle, skewT);
	}

	if (bgTexture2) {
		RenderSkewedTexture(bgTexture2, actualRenderPos, renderSize, flashBg, animAngle, skewT);
	}
	RenderSkewedMeterWithFill(fillTexture, actualRenderPos, renderSize, fillRatio, fillColorC, animAngle, skewT);
	RenderSkewedTexture(bgTexture, actualRenderPos, renderSize, flashBg, animAngle, skewT);
	RenderSkewedTexture(bgHighlightTexture, actualRenderPos, renderSize, highlightColor, animAngle, skewT);

	ImGui::End();

	// --- Text Animation: delayed, ease in-out, straight ---
	ImVec2 textSize = ImVec2(1000.0f * scaleFactorY * 0.55f * kStyleMeterScale, 243.0f * scaleFactorY * 0.55f * kStyleMeterScale);
	ImVec2 textWindowPos = ImVec2(windowPos.x + (194.0f * scaleFactorY * kStyleMeterScale), windowPos.y + (125.0f * scaleFactorY * kStyleMeterScale));
	ImVec4 textColorHex = ImGui::ColorConvertU32ToFloat4(UI::SwapColorEndianness(textColor));
	ImColor textColorC = (textColorHex);
	float hT, sT, vT;
	ImGui::ColorConvertRGBtoHSV(textColorC.Value.x, textColorC.Value.y, textColorC.Value.z, hT, sT, vT);

	sT = ImClamp(sT * 1.9f, 0.0f, 1.0f);
	vT = ImClamp(vT * 1.8f, 0.0f, 1.0f);

	ImVec4 poppedTextColor;
	ImGui::ColorConvertHSVtoRGB(hT, sT, vT, poppedTextColor.x, poppedTextColor.y, poppedTextColor.z);
	poppedTextColor.w = state.fade.alpha;
	ImGui::SetNextWindowPos(textWindowPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(textSize + ImVec2(50.0f, 50.0f), ImGuiCond_Always);

	ImGui::Begin(HudViewWindowName("StyleMeterText", s_styleView), nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoBackground);
	HudViewClipGuard textClip(s_styleView);

	float textAnimT = 1.0f;
	if (state.animating) {
		if (state.animTimer > textDelay) {
			float t = ImClamp((state.animTimer - textDelay) / textAnimDuration, 0.0f, 1.0f);
			textAnimT = t < 0.5f ? 4.0f * t * t * t : 1.0f - powf(-2.0f * t + 2.0f, 3.0f) / 2.0f;
		} else {
			textAnimT = 0.0f;
		}
	}
	ImVec2 textAnimOffset = ImVec2((1.0f - textAnimT) * -meterSize.x * 1.2f * animIntensity, 0.0f);

	ImVec2 textPos = ImGui::GetCursorScreenPos() + textAnimOffset;
	RenderTexture(textTexture, textPos, textSize, poppedTextColor);
	RenderTexture(textHighlightTexture, textPos, textSize, white);

	ImGui::End();
}

static void StyleMeterWindowsView(int view) {
	s_styleView = view;
	// postponing this until we get better inCombat detection
// 	float deltaTime = ImGui::GetIO().DeltaTime;
// 	if (!InGame() || g_inGameCutscene) {
// 		g_styleMeterOutOfCombatTimer = 0.0f;
// 		g_styleMeterForceHidden = false;
// 	} else if (g_inCombat) {
// 		g_styleMeterOutOfCombatTimer = 0.0f;
// 		g_styleMeterForceHidden = false;
// 	} else {
// 		g_styleMeterOutOfCombatTimer += deltaTime;
// 		g_styleMeterForceHidden = g_styleMeterOutOfCombatTimer >= 3.0f;
// 	}

	CrimsonDetours::ToggleHideStyleRankHUD(activeCrimsonConfig.CrimsonHudAddons.styleRanksMeter); // Hide the original style rank HUD

	// D
	StyleMeterWindowRank(
		1,
		DStyleRankFillTexture->GetTexture(),
		DStyleRankBackgroundTexture->GetTexture(),
		DStyleRankBackgroundHighlightTexture->GetTexture(),
		DStyleRankTextTexture->GetTexture(),
		DStyleRankTextHighlightTexture->GetTexture(),
		0x1f5d8aFF,
		0xFFFFFFFF,
		0x93beb9FF,
		1.0f,
		ImVec2(g_uiSize.x - (480.0 * scaleFactorY), 190.0f * scaleFactorY),
		false,
		false,
		DStyleRankBackground2Texture->GetTexture()
	);

	// C
	StyleMeterWindowRank(
		2,
		CStyleRankFillTexture->GetTexture(),
		CStyleRankBackgroundTexture->GetTexture(),
		CStyleRankBackgroundHighlightTexture->GetTexture(),
		CStyleRankTextTexture->GetTexture(),
		CStyleRankTextHighlightTexture->GetTexture(),
		0x2d6a4eFF,
		0xFFFFFFFF,
		0x8de7c9FF,
		1.0f,
		ImVec2(g_uiSize.x - (480.0 * scaleFactorY), 190.0f * scaleFactorY),
		false,
		false,
		CStyleRankBackground2Texture->GetTexture()
	);

	// B
	StyleMeterWindowRank(
		3,
		BStyleRankFillTexture->GetTexture(),
		BStyleRankBackgroundTexture->GetTexture(),
		BStyleRankBackgroundHighlightTexture->GetTexture(),
		BStyleRankTextTexture->GetTexture(),
		BStyleRankTextHighlightTexture->GetTexture(),
		0x887018FF,
		0xFFFFFFFF,
		0xd8cb78FF,
		1.0f,
		ImVec2(g_uiSize.x - (480.0 * scaleFactorY), 190.0f * scaleFactorY),
		false,
		false,
		BStyleRankBackground2Texture->GetTexture()
	);

	// A
	StyleMeterWindowRank(
		4,
		AStyleRankFillTexture->GetTexture(),
		AStyleRankBackgroundTexture->GetTexture(),
		AStyleRankBackgroundHighlightTexture->GetTexture(),
		AStyleRankTextTexture->GetTexture(),
		AStyleRankTextHighlightTexture->GetTexture(),
		0xbb5b10FF,
		0xFFFFFFFF,
		0xdea264FF,
		1.0f,
		ImVec2(g_uiSize.x - (480.0 * scaleFactorY), 190.0f * scaleFactorY),
		false,
		false,
		AStyleRankBackground2Texture->GetTexture()
	);

	// S
	StyleMeterWindowRank(
		5,
		SStyleRankFillTexture->GetTexture(),
		SStyleRankBackgroundTexture->GetTexture(),
		SStyleRankBackgroundHighlightTexture->GetTexture(),
		SStyleRankTextTexture->GetTexture(),
		SStyleRankTextHighlightTexture->GetTexture(),
		0x97418cFF,
		0xFFFFFFFF,
		0xdb86d4FF,
		1.5f,
		ImVec2(g_uiSize.x - (480.0 * scaleFactorY), 190.0f * scaleFactorY),
		false,
		false,
		SStyleRankBackground2Texture->GetTexture()
	);

	// SS
	StyleMeterWindowRank(
		6,
		SSStyleRankFillTexture->GetTexture(),
		SSStyleRankBackgroundTexture->GetTexture(),
		SSStyleRankBackgroundHighlightTexture->GetTexture(),
		SSStyleRankTextTexture->GetTexture(),
		SSStyleRankTextHighlightTexture->GetTexture(),
		0x632881FF,
		0xFFFFFFFF,
		0xb303ceFF,
		1.5f,
		ImVec2(g_uiSize.x - (480.0 * scaleFactorY), 190.0f * scaleFactorY),
		true,
		false,
		SSStyleRankBackground2Texture->GetTexture()
	);

	// SSS
	StyleMeterWindowRank(
		7,
		SSSStyleRankFillTexture->GetTexture(),
		SSSStyleRankBackgroundTexture->GetTexture(),
		SSSStyleRankBackgroundHighlightTexture->GetTexture(),
		SSSStyleRankTextTexture->GetTexture(),
		SSSStyleRankTextHighlightTexture->GetTexture(),
		0x82141aFF,
		0xf6bbadFF,
		0xeb0024FF,
		1.5f,
		ImVec2(g_uiSize.x - (480.0 * scaleFactorY), 190.0f * scaleFactorY),
		true,
		true,
		SSSStyleRankBackground2Texture->GetTexture()
	);
}


void StyleMeterWindows() {
	if (activeCrimsonConfig.HudOptions.hideStyleMeter) {
		return;
	}
	ForEachHudView(StyleMeterWindowsView);
	s_styleView = -1;
}

static void StylishPointsWindowView(int view, float stylishPtsFadeAlpha, bool outOfCombatDone) {
	auto* mainActorPtr = StyleMeterSourceActor(view);
	if (!mainActorPtr) return;
	auto& mainActorData = *mainActorPtr;
	ImVec2 windowPos = ImVec2(g_uiSize.x - (320.0f * scaleFactorY), 395.0f * scaleFactorY);
	ImVec2 windowPosClassic = ImVec2(g_uiSize.x - (415.0f * scaleFactorY), 290.0f * scaleFactorY);
	ImVec2 windowSize = ImVec2(301.0f * 0.95f, 303.0f * scaleFactorY * 0.95f);
	float extraLeft = (g_uiSize.y - 100.0f * scaleFactorY);
	ImVec2 adjustedWindowPos = !activeCrimsonConfig.CrimsonHudAddons.styleRanksMeter ? ImVec2(windowPosClassic.x, windowPosClassic.y) 
		: ImVec2(windowPos.x, windowPos.y);
	ImVec2 adjustedWindowSize = windowSize + ImVec2(g_uiSize.x, g_uiSize.y); 

	// Same per player placement as the rank meter (it is bound to it); sized with the scale that includes it.
	const TopLeftHudSync styleSync = GetStyleMeterSync(view);
	adjustedWindowPos = styleSync.Pos(adjustedWindowPos);
	const float scaleFactorY = CrimsonGUI::scaleFactorY * styleSync.scale;

	auto stylePoints = (mainActorData.styleData.quotient * 100.0f);

	// Animation state for stylish points text, one per view
	struct State {
		float animTimer = 0.0f;
		bool animating = false;
		bool wasVisible = false;
		float prevStylePoints = 0.0f;
	};
	static State states[PLAYER_COUNT + 1];
	State& state = states[view + 1];
	float& animTimer = state.animTimer;
	bool& animating = state.animating;
	bool& wasVisible = state.wasVisible;
	float& prevStylePoints = state.prevStylePoints;

	if (outOfCombatDone) {
		wasVisible = false;
		animTimer = 0.0f;
		animating = true;
		return;
	}
	if (mainActorData.styleData.rank <= 0 || stylePoints <= 0 
		|| !InGame() || g_inGameCutscene || !activeCrimsonConfig.CrimsonHudAddons.stylishPtsCounter) {
		wasVisible = false;
		animTimer = 0.0f;
		animating = true;
		return;
	}

	// Animation constants
	const float delayDuration = 0.3f; // 1 second delay
	const float animDuration = 0.13f; // 130ms
	const float totalDuration = delayDuration + animDuration;
	
	// Check if we should start animation (when stylePoints changes or first appears)
	bool currentlyVisible = (mainActorData.styleData.rank > 0 && stylePoints > 0);
	if (currentlyVisible && (!wasVisible)) {
		animTimer = 0.0f;
		animating = true;
	}
	wasVisible = currentlyVisible;
	prevStylePoints = stylePoints;

	// Update animation timer
	if (animating) {
		animTimer += ImGui::GetIO().DeltaTime;
		if (animTimer >= totalDuration) {
			animTimer = totalDuration;
			animating = false;
		}
	}

	// EaseInOutCirc function
	auto easeInOutCirc = [](float t) -> float {
		if (t < 0.5f) {
			return 0.5f * (1.0f - sqrtf(1.0f - 4.0f * t * t));
		} else {
			return 0.5f * (sqrtf(1.0f - powf(-2.0f * t + 2.0f, 2.0f)) + 1.0f);
		}
	};

	// Calculate animation progress and offset
	float progress = 0.0f;
	if (animating) {
		if (animTimer < delayDuration) {
			// During delay phase, progress stays at 0
			progress = 0.0f;
		} else {
			// During animation phase, calculate progress from 0 to 1
			float animPhaseTimer = animTimer - delayDuration;
			progress = easeInOutCirc(animPhaseTimer / animDuration);
		}
	} else {
		progress = 1.0f;
	}
	float animOffset = (1.0f - progress) * 500.0f * scaleFactorY; // Start from right (positive offset)

	ImGui::SetNextWindowPos(adjustedWindowPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(adjustedWindowSize, ImGuiCond_Always);

	ImGui::Begin(HudViewWindowName("StylishPointsWindow", view), nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoBackground);
	HudViewClipGuard stylishClip(view);

	
	float fontSize = 44.0f;
	ImGui::SetWindowFontScale(scaleFactorY);
	
	// Draw "Stylish PTS: " with red outline
	ImGui::PushFont(UI::g_ImGuiFont_Benguiat[fontSize * 0.75f]);
	ImVec2 cursorPos = ImGui::GetCursorScreenPos();
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	ImGui::SetCursorScreenPos(ImVec2(cursorPos.x, cursorPos.y + (02.0f * scaleFactorY)));

	ImVec2 textPos = ImGui::GetCursorScreenPos();
	textPos.x += animOffset; // Apply animation offset
	
	// Draw red outline 
	const char* stylishText = "Stylish PTS: ";
	ImU32 outlineColor = ImColor(0.49f, 0.0f, 0.0f, stylishPtsFadeAlpha); // #7f0000; 
	ImU32 textColor = ImColor(1.0f, 1.0f, 1.0f, stylishPtsFadeAlpha); 
	float outlineThickness = 1.0f;
	
	// Draw outline by rendering text multiple times with slight offsets
	for (int x = -outlineThickness; x <= outlineThickness; x++) {
		for (int y = -outlineThickness; y <= outlineThickness; y++) {
			if (x == 0 && y == 0) continue; // Skip center position
			drawList->AddText(ImVec2(textPos.x + x, textPos.y + y), outlineColor, stylishText);
		}
	}
	// Draw main text on top
	drawList->AddText(textPos, textColor, stylishText);
	
	// Calculate text width for positioning
	ImVec2 textSize = ImGui::CalcTextSize(stylishText);
	ImGui::SetCursorScreenPos(ImVec2(textPos.x + textSize.x, textPos.y - (02.0f * scaleFactorY)));
	ImGui::PopFont();
	
	// Draw stylePoints with red outline
	ImGui::PushFont(UI::g_ImGuiFont_RedOrbRusso[fontSize]);
	ImVec2 textPos2 = ImGui::GetCursorScreenPos();
	ImVec2 pointsTextPos = ImVec2(textPos2.x + animOffset, textPos2.y); // Apply animation offset to points text
	
	// Format the stylePoints text
	char pointsText[32];
	snprintf(pointsText, sizeof(pointsText), "%.0f", stylePoints);
	
	// Draw red outline for stylePoints
	ImGui::PushFont(UI::g_ImGuiFont_RedOrbRussoBackdrop[fontSize]);
	drawList->AddText(pointsTextPos, outlineColor, pointsText);
	ImGui::PopFont();
	// Draw main text on top
	drawList->AddText(pointsTextPos, textColor, pointsText);
	
	ImGui::PopFont();
	ImGui::End();
}

// The out of combat fade is global (combat is), so it advances once per frame here, not once per view.
void StylishPointsWindow() {
	float deltaTime = ImGui::GetIO().DeltaTime;
	if (!InGame() || g_inGameCutscene) {
		g_stylishPtsOutOfCombatTimer = 0.0f;
	} else if (g_inCombat) {
		g_stylishPtsOutOfCombatTimer = 0.0f;
	} else {
		g_stylishPtsOutOfCombatTimer += deltaTime;
	}
	const float stylishPtsStayDuration = 3.5f;
	const float stylishPtsFadeDuration = 2.0f;
	float stylishPtsFadeAlpha = 1.0f;
	if (g_stylishPtsOutOfCombatTimer > stylishPtsStayDuration) {
		float fadeT = (g_stylishPtsOutOfCombatTimer - stylishPtsStayDuration) / stylishPtsFadeDuration;
		stylishPtsFadeAlpha = ImClamp(1.0f - fadeT, 0.0f, 1.0f);
	}
	const bool outOfCombatDone = g_stylishPtsOutOfCombatTimer >= stylishPtsStayDuration + stylishPtsFadeDuration;

	ForEachHudView([&](int view) { StylishPointsWindowView(view, stylishPtsFadeAlpha, outOfCombatDone); });
}

void MissionTimerDisplay() {
	auto pool_10222 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!(pool_10222 && pool_10222[3])) return;
	auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_10222[3]);
	auto name_10723 = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E30);
	if (!name_10723) return;
	auto& missionData = *reinterpret_cast<MissionData*>(name_10723);
	auto  pool_10298 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
	if (!pool_10298 || !pool_10298[8]) return;
	auto& eventData = *reinterpret_cast<EventData*>(pool_10298[8]);
	auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

	auto& config = activeCrimsonConfig.CrimsonHudAddons.missionTimerDisplay;

	// Display conditions
	if (eventData.event != EVENT::MAIN ||
		g_inGameCutscene ||
		config == MISSIONTIMERDISPLAY::OFF ||
		(config == MISSIONTIMERDISPLAY::ONLY_IN_BP && sessionData.mission != MISSION::BLOODY_PALACE)) {
		return;
	}

	// Centred on the screen (display size, whatever the mode): the text width is measured at its final size.
	const ImVec2 displaySize = ImGui::GetIO().DisplaySize;
	ImVec2 windowSize = ImVec2(301.0f * scaleFactorY * 0.95f, 303.0f * scaleFactorY * 0.95f);

	// Convert shown missionTimer to hours, minutes, seconds
	unsigned int hours = (unsigned int)(g_missionTimer / 3600.0f);
	unsigned int minutes = (unsigned int)(fmod(g_missionTimer, 3600.0f) / 60.0f);
	unsigned int seconds = (unsigned int)fmod(g_missionTimer, 60.0f);

	float fontSize = 44.0f;
	ImGui::PushFont(UI::g_ImGuiFont_RedOrbRusso[fontSize * 0.9f]);
	char timeText[32];
	snprintf(timeText, sizeof(timeText), "%02u:%02u:%02u", hours, minutes, seconds);

	ImVec2 adjustedWindowSize = windowSize + displaySize;

	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
	ImGui::SetNextWindowSize(adjustedWindowSize, ImGuiCond_Always);

	ImGui::Begin("MissionTimerDisplayWindow", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoBackground);

	ImGui::SetWindowFontScale(scaleFactorY);

	ImDrawList* drawList = ImGui::GetWindowDrawList();

	const float textWidth = ImGui::CalcTextSize(timeText).x;
	ImVec2 textPos = ImVec2(displaySize.x * 0.5f - textWidth * 0.5f, 12.0f * scaleFactorY);
	ImU32 outlineColor = IM_COL32(100, 10, 10, 255); // #500812
	ImU32 textColor = IM_COL32(255, 255, 255, 255);
	float outlineThickness = 1.0f;

	// Draw red backdrop on the background
	ImGui::PushFont(UI::g_ImGuiFont_RedOrbRussoBackdrop[fontSize * 0.9]);
	drawList->AddText(textPos, outlineColor, timeText);
	ImGui::PopFont();
	// Draw main text on top
	drawList->AddText(textPos, textColor, timeText);

	ImGui::PopFont();
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


void DrawRotatedImagePie(ImTextureID tex_id, ImVec2 pos, ImVec2 size, float angle, ImU32 color, float fill) {
	if (fill <= 0.0f) return;
	if (fill >= 1.0f) {
		DrawRotatedImage(tex_id, pos, size, angle, color);
		return;
	}

	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImVec2 center = ImVec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f);

	const int NUM_SEGMENTS = 64;
	int arc_segments = std::max(1, (int)(NUM_SEGMENTS * fill));

	float rx = size.x * 0.5f;
	float ry = size.y * 0.5f;

	// Clockwise: end_angle < start_angle
	//float start_angle = -IM_PI / 2.0f + angle; // with angle it rotates
	float start_angle = -IM_PI / 2.0f + angle;
	float end_angle = start_angle - fill * 2.0f * IM_PI; // Subtract for clockwise

	for (int i = 0; i < arc_segments; ++i) {
		float t0 = (float)i / arc_segments;
		float t1 = (float)(i + 1) / arc_segments;
		float theta0 = start_angle - t0 * (start_angle - end_angle);
		float theta1 = start_angle - t1 * (start_angle - end_angle);

		ImVec2 p0 = center;
		ImVec2 p1 = ImVec2(center.x + cosf(theta0) * rx, center.y + sinf(theta0) * ry);
		ImVec2 p2 = ImVec2(center.x + cosf(theta1) * rx, center.y + sinf(theta1) * ry);

		ImVec2 uv0(0.5f, 0.5f);
		ImVec2 uv1(0.5f + 0.5f * cosf(theta0), 0.5f + 0.5f * sinf(theta0));
		ImVec2 uv2(0.5f + 0.5f * cosf(theta1), 0.5f + 0.5f * sinf(theta1));

		draw_list->AddImageQuad(
			tex_id,
			p0, p1, p2, p0,
			uv0, uv1, uv2, uv0,
			color
		);
	}
}

// The Crimson counter shows exactly when the vanilla one is drawn. DrawUILayers_sub_140326870 only draws UI objects whose
// byte +0x5D is set; UpdateCWorkEntities_sub_1403265A0 clears it every frame and CUIDCockpit00_Tick_sub_14027DEA0 marks it
// again (MarkUIObjectVisible_sub_140326C80), except while the HUD is hidden (pause, events: PlayerActorPool+0x12).
// DrawCUIDCockpit00_sub_14027F1F0 then leaves the counter out during the credits (PlayerActorPool+0x11).
static bool VanillaRedOrbCounterDrawn() {
	constexpr size_t UI_OBJECT_VISIBLE = 0x5D;
	constexpr size_t COCKPIT_ACTOR     = 0x6880; // set for a cockpit bound to a specific actor
	constexpr size_t POOL_CREDITS      = 0x11;
	constexpr size_t POOL_COCKPIT00    = 0x1B070 - 0x180; // CSceneGameMain+0x1B070, the pool sits at +0x180

	auto pool = *reinterpret_cast<const byte8* const*>(appBaseAddr + 0xC90E28);
	if (g_scene != SCENE::GAME || !pool) {
		return false;
	}
	auto cockpit = *reinterpret_cast<const byte8* const*>(pool + POOL_COCKPIT00);
	if (!cockpit || cockpit[UI_OBJECT_VISIBLE] != 1) {
		return false;
	}
	if (*reinterpret_cast<const uintptr_t*>(cockpit + COCKPIT_ACTOR) == 0 && pool[POOL_CREDITS]) {
		return false;
	}
	return true;
}

void RedOrbCounterWindow() {
	assert(RedOrbTexture);

	auto name_7058 = *reinterpret_cast<byte8**>(appBaseAddr + 0xC90E30);
	if (!name_7058) {
		return;
	}
	auto& missionData = *reinterpret_cast<MissionData*>(name_7058);
	if (g_inGameCutscene) {
		return;
	}

	auto name_80 = *reinterpret_cast<byte8**>(appBaseAddr + 0xCF2680);
	if (!name_80) {
		return;
	}
	auto& hudData = *reinterpret_cast<HUDData*>(name_80);
	auto pool_10222 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	// This element is mandatory for non-vanilla modes
// 	if (activeCrimsonGameplay.GameMode.preset >= GAMEMODEPRESETS::STYLE_SWITCHER) {
// 		activeCrimsonConfig.CrimsonHudAddons.redOrbCounter = true;
// 		queuedCrimsonConfig.CrimsonHudAddons.redOrbCounter = true;
// 	}

	// In splitscreen the game's own counter is left out of every view; this counter stands in for it (vanilla orb
	// texture when the Crimson addon is off).
	const bool standIn = CrimsonSplitscreen::ViewCount() >= 2;
	const bool crimsonCounter = activeCrimsonConfig.CrimsonHudAddons.redOrbCounter;
	if (activeCrimsonConfig.HudOptions.hideMainHUD || (!crimsonCounter && !standIn)) {
		CrimsonDetours::RerouteRedOrbsCounterAlpha(false, crimsonHud.redOrbAlpha);
		CrimsonPatches::SetRebOrbCounterDurationTillFadeOut(false, 90);
		return;
	}

	// Set up Rerouting Alpha to our Red Orb Counter
	hudData.orbsCountAlpha = 0;
	CrimsonDetours::RerouteRedOrbsCounterAlpha(true, crimsonHud.redOrbAlpha);
	CrimsonPatches::SetRebOrbCounterDurationTillFadeOut(true, crimsonHud.redOrbAlphaDurationToAlpha);

	if (!VanillaRedOrbCounterDrawn()) {
		return;
	}

	// Get the current display size
	ImVec2 displaySize = ImGui::GetIO().DisplaySize;

	// Placement from the debug windows (HUD.cpp): scale around the top-right corner, offset from the default place.
	float placementScale = 1.0f, placementX = 0.0f, placementY = 0.0f;
	HUD_RedOrbCrimsonPlacement(placementScale, placementX, placementY, displaySize.y);
	const float scaleFactorY = CrimsonGUI::scaleFactorY * placementScale;
	const float scaleFactorX = CrimsonGUI::scaleFactorX * placementScale;

	// Define the orb count and cap it at 999999
	int orbCount = (std::min)(999999, (int)missionData.redOrbs);
	std::string orbCountStr = std::to_string(orbCount);

	// Adjust the font size and the proportional texture size
	float fontSize = 44.0f;

	// previously 142x200 -> 43x61; now 178x250 -> 54x76 to make space for the glow.
	float textureBaseSizeX = 54.0f;
	float textureBaseSizeY = 76.0f;
	float textureWidth = textureBaseSizeX * scaleFactorY;
	float textureHeight = textureBaseSizeY * scaleFactorY;
	float centerX = textureWidth / 2.0f;
	float centerY = textureHeight / 2.0f;

	// Define the window size and position
	ImVec2 windowSize = ImVec2(300.0f * scaleFactorX, 100.0f * scaleFactorY);
	float edgeOffsetX = 38.0f * scaleFactorY;
	float edgeOffsetY = 30.0f * scaleFactorY;
	ImVec2 windowPos = ImVec2(displaySize.x - windowSize.x - edgeOffsetX + placementX, edgeOffsetY + placementY);

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
	ImGui::PushFont(UI::g_ImGuiFont_RedOrbRusso[fontSize]);
	ImVec2 textSize = ImGui::CalcTextSize(orbCountStr.c_str(), nullptr, true);
	ImVec2 textPos = ImVec2(windowSize.x - textSize.x - 74.0f * scaleFactorY, (windowSize.y - textSize.y) / 2);

	// Correct the texture position by considering the window's screen position
	ImVec2 texturePos = ImVec2(windowPos.x + textPos.x - textureWidth - 5.916f * scaleFactorY, windowPos.y + (windowSize.y - textureHeight) / 2);

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

	// Stand-in for the vanilla counter: the vanilla orb.
	if (!crimsonCounter && RedOrbVanillaTexture) {
		redOrbGameMode = RedOrbVanillaTexture;
	}

	// Render the texture or a white square if the texture is not valid
	if (redOrbGameMode->IsValid()) {
		ImGui::GetWindowDrawList()->AddImage(redOrbGameMode->GetTexture(), texturePos, ImVec2(texturePos.x + textureWidth, texturePos.y + textureHeight), ImVec2(0, 0), ImVec2(1, 1), colorWithAlpha);
	}
	else {
		ImGui::GetWindowDrawList()->AddRectFilled(texturePos, ImVec2(texturePos.x + textureWidth, texturePos.y + textureHeight), ImColor(1.0f, 1.0f, 1.0f, alpha));
	}

	// Render the orb count text with a red shadow using AddText
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	ImFont* font = UI::g_ImGuiFont_RedOrbRusso[fontSize];
	ImFont* backdropFont = UI::g_ImGuiFont_RedOrbRussoBackdrop[fontSize];
	ImVec2 screenTextPos = ImGui::GetWindowPos() + textPos;

	// Shadow offset and color
	ImVec2 shadowOffset = ImVec2(2.0f * scaleFactorY, 2.0f * scaleFactorY);
	ImU32 shadowColor = IM_COL32(100, 10, 10, 255 * alpha); // #500812
	

	// Main text color
	//ImU32 mainColor = ImColor(0.83f, 0.85f, 0.858f, alpha); // #D5D9DB
	ImU32 mainColor = ImColor(1.0f, 1.0f, 1.0f, alpha); // Pure White

	// Draw shadow
	drawList->AddText(backdropFont, fontSize * scaleFactorY, screenTextPos, shadowColor, orbCountStr.c_str());
	// Draw main text
	drawList->AddText(font, fontSize * scaleFactorY, screenTextPos, mainColor, orbCountStr.c_str());

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
	if (activeCrimsonConfig.HudOptions.hideMainHUD) {
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

	// The labels belong to the Crimson red orb counter: same scale (about the top-right corner) and offset from the debug
	// windows (HUD.cpp), so they move and scale with it as one thing.
	float placementScale = 1.0f, placementX = 0.0f, placementY = 0.0f;
	HUD_RedOrbCrimsonPlacement(placementScale, placementX, placementY, ImGui::GetIO().DisplaySize.y);
	const float scaleFactorY = CrimsonGUI::scaleFactorY * placementScale;
	const float scaleFactorX = CrimsonGUI::scaleFactorX * placementScale;
	const float scaledFontSize = CrimsonGUI::scaledFontSize * placementScale;

	auto CheatIndicatorWindow = [&](ImVec2 windowSize, ImVec2 windowPos, std::vector<uint8> cheatIndices, const char* label) {
		// Define the window size and position
		float edgeOffsetX = 0.0f * scaleFactorY;
		float edgeOffsetY = 15.0f * scaleFactorY;
		auto& currentGameMode = activeCrimsonGameplay.GameMode.preset;

		ImGui::SetNextWindowSize(windowSize);
		ImGui::SetNextWindowPos(windowPos);
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;

		// Adjust the font size and the proportional texture size
		float fontSize = 18.0f;

		ImGui::Begin(label, nullptr, windowFlags);
		// Set the color with alpha for the Red Orb texture
		float alpha = crimsonHud.redOrbAlpha / 127.0f;
		ImColor colorWithAlpha(1.0f, 1.0f, 1.0f, alpha);
		ImGui::SetWindowFontScale(scaleFactorY);
		ImGui::PushFont(UI::g_ImGuiFont_Benguiat[fontSize]);

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

		// Prepare the list of cheats to show in this window
		std::vector<uint8> cheatsToShow;
		for (auto cheatIdx : cheatIndices) {
			// Only show if this cheat is currently active
			if (std::find(gameModeData.currentlyUsedCheats.begin(), gameModeData.currentlyUsedCheats.end(), cheatIdx) != gameModeData.currentlyUsedCheats.end()) {
				cheatsToShow.push_back(cheatIdx);
			}
		}

		// Calculate total width of all buttons and spacing
		float totalButtonsWidth = 0.0f;
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 7.0f * scaledFontSize);
		float spacing = ImGui::GetStyle().ItemSpacing.x * scaleFactorY;
		std::vector<ImVec2> btnSizes;
		for (size_t i = 0; i < cheatsToShow.size(); ++i) {
			auto cheat = cheatsToShow[i];
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
		for (size_t i = 0; i < cheatsToShow.size(); ++i) {
			auto cheat = cheatsToShow[i];
			const std::string& btnLabel = gameModeData.cheatsNames[cheat];
			ImGui::Button(btnLabel.c_str(), btnSizes[i]);
			if (i + 1 < cheatsToShow.size()) {
				ImGui::SameLine();
				ImGui::SameLine(0.0f, spacing); // Use calculated spacing
			}
		}

		ImGui::PopStyleVar();
		ImGui::PopStyleColor(3);
		ImGui::PopFont();
		ImGui::End();
		};

	// Top window: TRAINING, DAMAGE, SPEED, MOBILITY
	CheatIndicatorWindow(
		ImVec2(367.0f * scaleFactorX, 100.0f * scaleFactorY),
		ImVec2(g_uiSize.x - (367.0f * scaleFactorX) + placementX, 15.0f * scaleFactorY + placementY),
		{ CHEATS::TRAINING, CHEATS::DAMAGE, CHEATS::SPEED, CHEATS::MOBILITY },
		"CheatsHUDIndicatorWindow1"
	);

	// Bottom window: DEBUG, RMS
	CheatIndicatorWindow(
		ImVec2(367.0f * scaleFactorX, 100.0f * scaleFactorY),
		ImVec2(g_uiSize.x - (367.0f * scaleFactorX) + placementX, 117.0f * scaleFactorY + placementY),
		{ CHEATS::DEBUG, CHEATS::RMS },
		"CheatsHUDIndicatorWindow2"
	);
}

void CheatHotkeysPopUpWindow() {
	// Define the window size and position
	ImVec2 windowSize = ImVec2(367.0f * scaleFactorX, 100.0f * scaleFactorY);
	float edgeOffsetX = (g_uiSize.x * 0.7f) - (windowSize.x * 0.5f);
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


// Smoothed lock-on distance helper — continuous float with exponential
// smoothing to eliminate stepping from integer quantization in
// ComputeLockOnScreenData.  Capped at capDistance so the lock-on icon
// never grows beyond ~14 % above the default (non-scaling) size.

static float g_smoothLockOnDist[PLAYER_COUNT] = { 0.0f };
static bool  g_smoothLockOnDistInit[PLAYER_COUNT] = { false };

inline float GetSmoothedLockOnDist(uint8 playerIndex, float rawDist, float deltaTime) {
	if (!g_smoothLockOnDistInit[playerIndex]) {
		g_smoothLockOnDist[playerIndex] = rawDist;
		g_smoothLockOnDistInit[playerIndex] = true;
		return rawDist;
	}
	const float smoothSpeed = 6.0f; // lower == smoother; but more lag
	g_smoothLockOnDist[playerIndex] = SmoothLerp(
		g_smoothLockOnDist[playerIndex], rawDist, smoothSpeed, deltaTime);
	return g_smoothLockOnDist[playerIndex];
}

const float g_capDistance = 29.0f; // max size cap distance for the lockOns

void LockOnWindows() {
	static float lockOnAngle[PLAYER_COUNT] = { 0.0f };
	static FadeState lockOnFade[PLAYER_COUNT];

	assert(LockOnTexture);
	if (!LockOnTexture->IsValid()) {
		for (int i = 0; i < PLAYER_COUNT; ++i) ForceFadeOut(lockOnFade[i]);
		return;
	}
	if (!(InGame() && !g_inGameCutscene)) {
		for (int i = 0; i < PLAYER_COUNT; ++i) ForceFadeOut(lockOnFade[i]);
		return;
	}
	auto pool_4449 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC8FBD0);
	if (!pool_4449 || !pool_4449[147]) {
		for (int i = 0; i < PLAYER_COUNT; ++i) ForceFadeOut(lockOnFade[i]);
		return;
	}
	auto& cameraData = *reinterpret_cast<CameraData*>(pool_4449[147]);


	ImVec2 displaySize = ImGui::GetIO().DisplaySize;

	// Spin speed in radians per second (adjust as needed)
	const float spinSpeed = 0.08f; // slow spin
	const float scaleFactorY = CrimsonGUI::scaleFactorY * CrimsonSplitscreen::ViewScale(0);

	if (!activeCrimsonConfig.CrimsonHudAddons.lockOn ) {
		CrimsonPatches::ToggleHideLockOn(activeCrimsonConfig.HudOptions.hideLockOn);
		for (int i = 0; i < PLAYER_COUNT; ++i) ForceFadeOut(lockOnFade[i]);
		return;
	}

	if (activeCrimsonConfig.HudOptions.hideLockOn) {
		for (int i = 0; i < PLAYER_COUNT; ++i) ForceFadeOut(lockOnFade[i]);
		return;
	}

	CrimsonPatches::ToggleHideLockOn(activeCrimsonConfig.CrimsonHudAddons.lockOn);

	// Determine player count based on actor module state
	uint8 playerCount = activeConfig.Actor.enable ? activeConfig.Actor.playerCount : 1;

	// Loop through player data
	for (uint8 playerIndex = 0; playerIndex < playerCount; ++playerIndex) {
		PlayerActorData* actorDataPtr = nullptr;

		if (activeConfig.Actor.enable) {
			// CCS route: GetNewActorData — direct lookup via the Crimson config system.
			auto& playerData = GetPlayerData(playerIndex);
			auto& newActorData = GetNewActorData(playerIndex, playerData.characterIndex, ENTITY::MAIN);
			if (!newActorData.baseAddr) {
				continue; // FIX: Use continue instead of return to avoid mismatched Begin/End
			}
			actorDataPtr = reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);
		} else {
			// Vanilla route: GetVanillaPlayerActor — only playerIndex 0 is valid.
			if (playerIndex > 0) continue;
			actorDataPtr = GetVanillaPlayerActor();
			if (!actorDataPtr) continue;
		}
		auto& actorData = *actorDataPtr;

		// Update angle
		float deltaTime = ImGui::GetIO().DeltaTime;
		lockOnAngle[playerIndex] += spinSpeed * deltaTime;

		// Keep angle in [0, 2*PI]
		if (lockOnAngle[playerIndex] > IM_PI * 2.0f) lockOnAngle[playerIndex] -= IM_PI * 2.0f;

		// Compute lock-on screen position, camera distance, and clamped distance
		CrimsonFX::ComputeLockOnScreenData(actorData, cameraData, playerIndex);
		auto& lockedEnemyScreenPosition = crimsonPlayer[playerIndex].lockedEnemyScreenPosition;

		// Adjusts size dynamically based on the distance between Camera and Player
		float textureBaseSizeX = 600.0f * scaleFactorY;
		float textureBaseSizeY = 581.0f * scaleFactorY;

		float rawDist = crimsonPlayer[playerIndex].cameraLockedEnemyDistance / 20.0f;
		float smoothDist = GetSmoothedLockOnDist(playerIndex, rawDist, ImGui::GetIO().DeltaTime);
		float safeDistance = (std::max)(smoothDist, g_capDistance);  // capped to prevent oversized icons when close

		ImVec2 sizeDistance = {
			(textureBaseSizeX * (1.0f / (safeDistance / 40))),
			(textureBaseSizeY * (1.0f / (safeDistance / 40)))
		};

		float textureWidth = sizeDistance.x * 0.25f;
		float textureHeight = sizeDistance.y * 0.25f;

		ImVec2 windowSize = ImVec2(700.0f * scaleFactorY, 700.0f * scaleFactorY);
		float edgeOffsetX = 350.0f * scaleFactorY;
		float edgeOffsetY = 350.0f * scaleFactorY;

		float offsetX = 0.45f;
		float offsetY = 0.39f;

		ImVec2 texturePos = ImVec2(
			lockedEnemyScreenPosition.x - (sizeDistance.x / 2.0f) + offsetX - (offsetX * 0.03f * playerIndex),
			lockedEnemyScreenPosition.y - (sizeDistance.y / 2.0f) + offsetY - (offsetY *  0.03f * playerIndex)
		);

		ImVec2 windowPos = ImVec2(
			texturePos.x + (sizeDistance.x / 2.0f) - (windowSize.x / 2.0f),
			texturePos.y + (sizeDistance.y / 2.0f) - (windowSize.y / 2.0f)
		);
		ImGui::SetNextWindowSize(windowSize);
		ImGui::SetNextWindowPos(windowPos);

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;

		std::string windowName = "LockOnWindow" + std::to_string(playerIndex);

		// Only call Begin if you will always call End in this loop iteration!
		bool windowOpen = ImGui::Begin(windowName.c_str(), nullptr, windowFlags);
		const bool viewClipped = PushSplitscreenViewClip(playerIndex);

		float alpha = 1.0f;

		// Decide the Lock-On Color
		ImVec4 playerColor = ConvertColorFromUint8ToVec4(activeCrimsonConfig.PlayerProperties.playerColor[playerIndex]);
		static ImVec4 usedColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		if (activeCrimsonConfig.CrimsonHudAddons.lockOnColorsCharacter && activeConfig.Actor.playerCount == 1) {
			if (actorData.character == CHARACTER::DANTE) {
				usedColor = ImGui::ColorConvertU32ToFloat4(UI::SwapColorEndianness(colorPresets.LockOnColors.Dante));
			} else {
				usedColor = ImGui::ColorConvertU32ToFloat4(UI::SwapColorEndianness(colorPresets.LockOnColors.Vergil));
			}
		} else {
			usedColor = playerColor;
		}

		ImColor color(usedColor);
		float h, s, v;
		ImGui::ColorConvertRGBtoHSV(color.Value.x, color.Value.y, color.Value.z, h, s, v);

		// Boost saturation and value for more "pop"
		s = ImClamp(s * 1.4f, 0.0f, 1.0f); // Increase saturation by 40%
		v = ImClamp(v * 1.8f, 0.0f, 1.0f); // Increase brightness by 80%

		ImVec4 poppedColor;
		ImGui::ColorConvertHSVtoRGB(h, s, v, poppedColor.x, poppedColor.y, poppedColor.z);
		poppedColor.w = lockOnFade[playerIndex].alpha; // Set alpha

		ImColor colorWithAlpha(poppedColor);
		ImColor fgColorWithAlpha(1.0f, 1.0f, 1.0f, lockOnFade[playerIndex].alpha);

		CrimsonGameplay::GetLockedOnEnemyHitPoints(actorData);
		float healthFraction = crimsonPlayer[playerIndex].lockedOnEnemyHP / crimsonPlayer[playerIndex].lockedOnEnemyMaxHP; // Clamp to [0,1] as needed

		bool lockOnActive = (actorData.buttons[0] & GetBinding(BINDING::LOCK_ON)) && actorData.lockOnData.targetBaseAddr60 != 0;

		const float fadeInDuration = 0.25f;
		const float fadeOutDuration = 0.25f;
		UpdateFade(lockOnFade[playerIndex], lockOnActive, fadeInDuration, fadeOutDuration, ImGui::GetIO().DeltaTime);

		// Calculate centered texture position within the window
		ImVec2 windowContentPos = ImGui::GetWindowPos();
		ImVec2 centeredTexturePos = ImVec2(
			windowContentPos.x + (windowSize.x - textureWidth) * (0.5f - ((0.010f * (sizeDistance.x / 400.0f)) * playerIndex)),
			windowContentPos.y + (windowSize.y - textureHeight) * (0.47f - ((0.010f * (sizeDistance.y / 400.0f)) * playerIndex))
		);

		if (lockOnFade[playerIndex].alpha > 0.01f) {
			if (LockOnTexture->IsValid()) {
				DrawRotatedImagePie(
					LockOnTexture->GetTexture(),
					centeredTexturePos,
					ImVec2(textureWidth, textureHeight),
					lockOnAngle[playerIndex],
					colorWithAlpha,
					healthFraction
				);
				DrawRotatedImagePie(
					LockOnForegroundTexture->GetTexture(),
					centeredTexturePos,
					ImVec2(textureWidth, textureHeight),
					lockOnAngle[playerIndex],
					fgColorWithAlpha,
					healthFraction
				);
			} else {
				ImGui::GetWindowDrawList()->AddRectFilled(texturePos, ImVec2(texturePos.x + textureWidth, texturePos.y + textureHeight), ImColor(1.0f, 1.0f, 1.0f, alpha));
			}
		}

		PopSplitscreenViewClip(viewClipped);
		ImGui::End();
	}
}

void StunDisplacementLockOnWindows() {
	static float lockOnAngle[PLAYER_COUNT] = { 0.0f };
	static FadeState lockOnFade[PLAYER_COUNT];

	assert(LockOnStunTexture);
	if (!LockOnStunTexture->IsValid()) {
		for (int i = 0; i < PLAYER_COUNT; ++i) ForceFadeOut(lockOnFade[i]);
		return;
	}
	if (!(InGame() && !g_inGameCutscene)) {
		for (int i = 0; i < PLAYER_COUNT; ++i) ForceFadeOut(lockOnFade[i]);
		return;
	}

	if (activeCrimsonConfig.HudOptions.hideLockOn) {
		for (int i = 0; i < PLAYER_COUNT; ++i) ForceFadeOut(lockOnFade[i]);
		return;
	}

	ImVec2 displaySize = ImGui::GetIO().DisplaySize;

	// Spin speed in radians per second (adjust as needed)
	const float spinSpeed = -0.12f; // slow spin
	const float scaleFactorY = CrimsonGUI::scaleFactorY * CrimsonSplitscreen::ViewScale(0);

	auto pool_4449 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC8FBD0);
	if (!pool_4449 || !pool_4449[147]) {
		for (int i = 0; i < PLAYER_COUNT; ++i) ForceFadeOut(lockOnFade[i]);
		return;
	}
	auto& cameraData = *reinterpret_cast<CameraData*>(pool_4449[147]);

	// Loop through player data
	for (uint8 playerIndex = 0; playerIndex < activeConfig.Actor.playerCount; ++playerIndex) {
		auto& playerData = GetPlayerData(playerIndex);
		auto& characterData = GetCharacterData(playerIndex, playerData.characterIndex, ENTITY::MAIN);
		auto& newActorData = GetNewActorData(playerIndex, playerData.characterIndex, ENTITY::MAIN);
		PlayerActorData* actorDataPtr = nullptr;

		if (activeConfig.Actor.enable) {
			// CCS route: GetNewActorData — direct lookup via the Crimson config system.
			auto& playerData = GetPlayerData(playerIndex);
			auto& newActorData = GetNewActorData(playerIndex, playerData.characterIndex, ENTITY::MAIN);
			if (!newActorData.baseAddr) {
				continue; // FIX: Use continue instead of return to avoid mismatched Begin/End
			}
			actorDataPtr = reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);
		}
		else {
			// Vanilla route: GetVanillaPlayerActor — only playerIndex 0 is valid.
			if (playerIndex > 0) continue;
			actorDataPtr = GetVanillaPlayerActor();
			if (!actorDataPtr) continue;
		}
		auto& actorData = *actorDataPtr;

		if (actorData.character != CHARACTER::DANTE && actorData.character != CHARACTER::VERGIL) continue;

		// Update angle
		float deltaTime = ImGui::GetIO().DeltaTime;
		lockOnAngle[playerIndex] += spinSpeed * deltaTime;

		// Keep angle in [0, 2*PI]
		if (lockOnAngle[playerIndex] > IM_PI * 2.0f) lockOnAngle[playerIndex] -= IM_PI * 2.0f;

		CrimsonFX::ComputeLockOnScreenData(actorData, cameraData, playerIndex);

		auto& lockedEnemyScreenPosition = crimsonPlayer[playerIndex].lockedEnemyScreenPosition;

		// Adjusts size dynamically based on the distance between Camera and Player
		float textureBaseSizeX = 600.0f * scaleFactorY;
		float textureBaseSizeY = 581.0f * scaleFactorY;

		float rawDist = crimsonPlayer[playerIndex].cameraLockedEnemyDistance / 20.0f;
		float smoothDist = GetSmoothedLockOnDist(playerIndex, rawDist, ImGui::GetIO().DeltaTime);
		float safeDistance = (std::max)(smoothDist, g_capDistance);  // capped to prevent oversized icons when close

		// DISPLACEMENT LOCK-ON (OUTER DARKER CIRCLE)

		ImVec2 sizeDistance = {
			(textureBaseSizeX * (1.0f / (safeDistance / 40))),
			(textureBaseSizeY * (1.0f / (safeDistance / 40)))
		};

		float textureWidth = sizeDistance.x * 0.18f;
		float textureHeight = sizeDistance.y * 0.18f;

		ImVec2 windowSize = ImVec2(700.0f * scaleFactorY, 700.0f * scaleFactorY);
		float edgeOffsetX = 350.0f * scaleFactorY;
		float edgeOffsetY = 350.0f * scaleFactorY;

		float offsetX = 0.45f;
		float offsetY = 0.39f;

		ImVec2 texturePos = ImVec2(
			lockedEnemyScreenPosition.x - (sizeDistance.x / 2.0f) + offsetX - (offsetX * 0.03f * playerIndex),
			lockedEnemyScreenPosition.y - (sizeDistance.y / 2.0f) + offsetY - (offsetY * 0.03f * playerIndex)
		);

		ImVec2 windowPos = ImVec2(
			texturePos.x + (sizeDistance.x / 2.0f) - (windowSize.x / 2.0f),
			texturePos.y + (sizeDistance.y / 2.0f) - (windowSize.y / 2.0f)
		);
		ImGui::SetNextWindowSize(windowSize);
		ImGui::SetNextWindowPos(windowPos);

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;

		std::string windowName = "LockOnDisplacementWindow" + std::to_string(playerIndex);

		ImGui::Begin(windowName.c_str(), nullptr, windowFlags);
		const bool viewClipped = PushSplitscreenViewClip(playerIndex);

		float alpha = 1.0f;

		ImVec4 displacementColor = ImGui::ColorConvertU32ToFloat4(UI::SwapColorEndianness(0x5948a7FF));

		ImColor color(displacementColor);
		float h, s, v;
		ImGui::ColorConvertRGBtoHSV(color.Value.x, color.Value.y, color.Value.z, h, s, v);

		// Boost saturation and value for more "pop"
		s = ImClamp(s * 1.4f, 0.0f, 1.0f); // Increase saturation by 40%
		v = ImClamp(v * 1.8f, 0.0f, 1.0f); // Increase brightness by 80%

		ImVec4 poppedColor;
		ImGui::ColorConvertHSVtoRGB(h, s, v, poppedColor.x, poppedColor.y, poppedColor.z);
		poppedColor.w = lockOnFade[playerIndex].alpha; // Set alpha

		ImColor colorWithAlpha(poppedColor);
		ImColor fgColorWithAlpha(1.0f, 1.0f, 1.0f, lockOnFade[playerIndex].alpha);

		CrimsonGameplay::GetLockedOnEnemyStunDisplacement(actorData);
		auto& lockedOnEnemyStun = crimsonPlayer[playerIndex].lockedOnEnemyStun;
		auto& lockedOnEnemyMaxStun = crimsonPlayer[playerIndex].lockedOnEnemyMaxStun;
		auto& lockedOnEnemyDisplacement = crimsonPlayer[playerIndex].lockedOnEnemyDisplacement;
		auto& lockedOnEnemyMaxDisplacement = crimsonPlayer[playerIndex].lockedOnEnemyMaxDisplacement;
		auto& lockedOnEnemyMinusStun = crimsonPlayer[playerIndex].lockedOnEnemyMinusStun;
		auto& lockedOnEnemyMinusDisplacement = crimsonPlayer[playerIndex].lockedOnEnemyMinusDisplacement;

		float displacementFraction = 1.0f - (lockedOnEnemyDisplacement / lockedOnEnemyMaxDisplacement);

		bool lockOnActive = (actorData.buttons[0] & GetBinding(BINDING::LOCK_ON)) && actorData.lockOnData.targetBaseAddr60 != 0;

		const float fadeInDuration = 0.25f;
		const float fadeOutDuration = 0.25f;
		UpdateFade(lockOnFade[playerIndex], lockOnActive, fadeInDuration, fadeOutDuration, ImGui::GetIO().DeltaTime);

		// Calculate centered texture position within the window
		ImVec2 windowContentPos = ImGui::GetWindowPos();
		ImVec2 centeredTexturePos = ImVec2(
			windowContentPos.x + (windowSize.x - textureWidth) * 0.5f,
			windowContentPos.y + (windowSize.y - textureHeight) * 0.47f
		);

		if (lockOnFade[playerIndex].alpha > 0.01f) {
			if (LockOnTexture->IsValid()) {
				if (activeCrimsonConfig.CrimsonHudAddons.lockOnStunDisplacement) {
					DrawRotatedImagePie(
						LockOnStunTexture->GetTexture(),
						centeredTexturePos,
						ImVec2(textureWidth, textureHeight),
						lockOnAngle[playerIndex],
						colorWithAlpha,
						displacementFraction
					);
					DrawRotatedImagePie(
						LockOnForegroundTexture->GetTexture(),
						centeredTexturePos,
						ImVec2(textureWidth, textureHeight),
						lockOnAngle[playerIndex],
						fgColorWithAlpha,
						displacementFraction
					);
				}
			} else {
				ImGui::GetWindowDrawList()->AddRectFilled(texturePos, ImVec2(texturePos.x + textureWidth, texturePos.y + textureHeight), ImColor(1.0f, 1.0f, 1.0f, alpha));
			}

			if (actorData.lockOnData.targetBaseAddr60 != 0) {
				auto& enemyActorData = *reinterpret_cast<EnemyActorData*>(actorData.lockOnData.targetBaseAddr60 - 0x60); // -0x60 very important don't forget


				ImVec2 sizeDistance = {
					(textureBaseSizeX * (1.0f / (safeDistance / 40))),
					(textureBaseSizeY * (1.0f / (safeDistance / 40)))
				};

				float textureWidth = sizeDistance.x * 0.18f;
				float textureHeight = sizeDistance.y * 0.18f;

				ImVec2 windowSize = ImVec2(700.0f * scaleFactorY, 700 * scaleFactorY);
				float edgeOffsetX = 350.0f * scaleFactorY;
				float edgeOffsetY = 350.0f * scaleFactorY;

				float offsetX = 0.45f;
				float offsetY = 0.39f;

				ImVec2 texturePos = ImVec2(
					lockedEnemyScreenPosition.x - (sizeDistance.x / 2.0f) + offsetX - (offsetX * 0.03f * playerIndex),
					lockedEnemyScreenPosition.y - (sizeDistance.y / 2.0f) + offsetY - (offsetY * 0.03f * playerIndex)
				);

				ImVec2 windowPos = ImVec2(
					texturePos.x + (sizeDistance.x / 2.0f) - (windowSize.x / 2.0f),
					texturePos.y + (sizeDistance.y / 2.0f) - (windowSize.y / 2.0f)
				);

				float textureBaseSizeXStun = 600.0f * scaleFactorY;
				float textureBaseSizeYStun = 581.0f * scaleFactorY;

				// STUN LOCK-ON (INNER LIGHTER CIRCLE)

				ImVec2 sizeDistanceStun = {
					(textureBaseSizeXStun * (1.0f / (safeDistance / 40))),
					(textureBaseSizeYStun * (1.0f / (safeDistance / 40)))
				};

				float textureWidthStun = textureBaseSizeXStun * 0.10f;
				float textureHeightStun = textureBaseSizeYStun * 0.10f;

				ImVec2 windowSizeStun = ImVec2(sizeDistanceStun.x, sizeDistanceStun.y);

				// --- Center the StunLockOn on the Regular Lock On ---
				ImVec2 texturePosStun = ImVec2(
					lockedEnemyScreenPosition.x - (sizeDistance.x / 2.0f) + offsetX - (offsetX * 0.03f * playerIndex),
					lockedEnemyScreenPosition.y - (sizeDistance.y / 2.0f) + offsetY - (offsetY * 0.03f * playerIndex)
				);

				ImVec2 windowPosStun = ImVec2(
					texturePosStun.x + (sizeDistanceStun.x / 2.0f) - (windowSize.x / 2.0f),
					texturePosStun.y + (sizeDistanceStun.y / 2.0f) - (windowSize.y / 2.0f)
				);
				ImGui::SetNextWindowSize(windowSize);
				ImGui::SetNextWindowPos(windowPos);

				std::string windowNameStun = "LockOnStunWindow" + std::to_string(playerIndex);

				ImGui::Begin(windowNameStun.c_str(), nullptr, windowFlags);
				const bool stunViewClipped = PushSplitscreenViewClip(playerIndex);

				ImVec4 stunColor = ImGui::ColorConvertU32ToFloat4(UI::SwapColorEndianness(0xcfc4ffFF));

				ImColor colorStunC(stunColor);
				float hS, sS, vS;
				ImGui::ColorConvertRGBtoHSV(colorStunC.Value.x, colorStunC.Value.y, colorStunC.Value.z, hS, sS, vS);

				// Boost saturation and value for more "pop"
				sS = ImClamp(sS * 1.4f, 0.0f, 1.0f); // Increase saturation by 40%
				vS = ImClamp(vS * 2.0f, 0.0f, 1.0f); // Increase brightness by 80%

				ImVec4 poppedColorStun;
				ImGui::ColorConvertHSVtoRGB(hS, sS, vS, poppedColorStun.x, poppedColorStun.y, poppedColorStun.z);
				poppedColorStun.w = lockOnFade[playerIndex].alpha; // Set alpha

				ImColor colorStunWithAlpha(poppedColorStun);

				// Calculate centered texture position for the STUN window
				ImVec2 windowContentPosStun = ImGui::GetWindowPos();
				ImVec2 centeredTexturePosStun = ImVec2(
					windowContentPosStun.x + (windowSize.x - textureWidthStun) * 0.5f,
					windowContentPosStun.y + (windowSize.y - textureHeightStun) * 0.47f
				);


				float stunFraction = 1.0f - (lockedOnEnemyStun / lockedOnEnemyMaxStun);

				if (activeCrimsonConfig.CrimsonHudAddons.lockOnStunDisplacement) {
					if (LockOnTexture->IsValid()) {
						DrawRotatedImagePie(
							LockOnStunTexture->GetTexture(),
							centeredTexturePosStun,
							ImVec2(textureWidthStun, textureHeightStun),
							lockOnAngle[playerIndex],
							colorStunWithAlpha,
							stunFraction
						);
						DrawRotatedImagePie(
							LockOnForegroundTexture->GetTexture(),
							centeredTexturePosStun,
							ImVec2(textureWidthStun, textureHeightStun),
							lockOnAngle[playerIndex],
							fgColorWithAlpha,
							stunFraction
						);
					} else {
						ImGui::GetWindowDrawList()->AddRectFilled(texturePos, ImVec2(texturePos.x + textureWidth, texturePos.y + textureHeight), ImColor(1.0f, 1.0f, 1.0f, alpha));
					}
				}

				// STUN / DISPLACEMENT NUMERIC HUD

				if (activeCrimsonConfig.CrimsonHudAddons.stunDisplacementNumericHud) {
					CrimsonGameplay::CalculateLockedOnEnemyLastStunDisplacementValue(actorData);
					ImVec4 textColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
					ImVec4 fadedTextColor = textColor;
					fadedTextColor.w *= lockOnFade[playerIndex].alpha;
					ImGui::PushStyleColor(ImGuiCol_Text, fadedTextColor);
					ImGui::SetWindowFontScale(scaleFactorY);

					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[18.0f]);
					// Stun Text
					ImGui::Text("Stun: ");
					ImGui::SameLine();
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[24.0f]);
					ImGui::Text("%.2f", lockedOnEnemyStun);
					ImGui::PopFont();
					if (lockedOnEnemyMinusStun > 0.0f) {
						float baseX = ImGui::GetCursorPosX();
						float offset = 170.0f * scaleFactorY; // Adjust as needed for your font/UI
						ImGui::SameLine();
						ImGui::SetCursorPosX(baseX + offset);
						ImGui::PushFont(UI::g_ImGuiFont_RussoOne[20.0f]);
						ImGui::Text(" -%.1f", lockedOnEnemyMinusStun);
						ImGui::PopFont();
					}

					// Displacement Text
					ImGui::Text("Displacement: ");
					ImGui::SameLine();
					ImGui::PushFont(UI::g_ImGuiFont_RussoOne[24.0f]);
					ImGui::Text("%.2f", lockedOnEnemyDisplacement);
					ImGui::PopFont();
					if (lockedOnEnemyMinusDisplacement > 0.0f) {
						float baseX = ImGui::GetCursorPosX();
						float offset = 185.0f * scaleFactorY; // Adjust as needed for your font/UI
						ImGui::SameLine();
						ImGui::SetCursorPosX(baseX + offset);
						ImGui::PushFont(UI::g_ImGuiFont_RussoOne[20.0f]);
						ImGui::Text(" -%.1f", lockedOnEnemyMinusDisplacement);
						ImGui::PopFont();
					}
					ImGui::PopFont();

					ImGui::PopStyleColor();
				}

				PopSplitscreenViewClip(stunViewClipped);
				ImGui::End(); // End LockOnStunWindow
			}
		}

		PopSplitscreenViewClip(viewClipped);
		ImGui::End(); // End LockOnDisplacementWindow
	}
}

void ShieldLockOnWindows() {
	static float lockOnAngle[PLAYER_COUNT] = { 0.0f };
	static FadeState lockOnFade[PLAYER_COUNT];

	assert(LockOnShieldTexture);
	if (!LockOnShieldTexture->IsValid()) {
		for (int i = 0; i < PLAYER_COUNT; ++i) ForceFadeOut(lockOnFade[i]);
		return;
	}
	if (!(InGame() && !g_inGameCutscene)) {
		for (int i = 0; i < PLAYER_COUNT; ++i) ForceFadeOut(lockOnFade[i]);
		return;
	}

	ImVec2 displaySize = ImGui::GetIO().DisplaySize;

	// Spin speed in radians per second (adjust as needed)
	const float spinSpeed = -0.12f; // slow spin
	const float scaleFactorY = CrimsonGUI::scaleFactorY * CrimsonSplitscreen::ViewScale(0);

	if (activeCrimsonConfig.HudOptions.hideMainHUD || !activeCrimsonConfig.CrimsonHudAddons.lockOn) {
		for (int i = 0; i < PLAYER_COUNT; ++i) ForceFadeOut(lockOnFade[i]);
		return;
	}

	auto pool_4449 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC8FBD0);
	if (!pool_4449 || !pool_4449[147]) {
		for (int i = 0; i < PLAYER_COUNT; ++i) ForceFadeOut(lockOnFade[i]);
		return;
	}
	auto& cameraData = *reinterpret_cast<CameraData*>(pool_4449[147]);

	// Loop through player data
	for (uint8 playerIndex = 0; playerIndex < activeConfig.Actor.playerCount; ++playerIndex) {
		PlayerActorData* actorDataPtr = nullptr;

		if (activeConfig.Actor.enable) {
			// CCS route: GetNewActorData — direct lookup via the Crimson config system.
			auto& playerData = GetPlayerData(playerIndex);
			auto& newActorData = GetNewActorData(playerIndex, playerData.characterIndex, ENTITY::MAIN);
			if (!newActorData.baseAddr) {
				continue; // FIX: Use continue instead of return to avoid mismatched Begin/End
			}
			actorDataPtr = reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);
		}
		else {
			// Vanilla route: GetVanillaPlayerActor — only playerIndex 0 is valid.
			if (playerIndex > 0) continue;
			actorDataPtr = GetVanillaPlayerActor();
			if (!actorDataPtr) continue;
		}
		auto& actorData = *actorDataPtr;

		if (actorData.character != CHARACTER::DANTE && actorData.character != CHARACTER::VERGIL) continue;

		// Update angle
		float deltaTime = ImGui::GetIO().DeltaTime;
		lockOnAngle[playerIndex] += spinSpeed * deltaTime;

		// Keep angle in [0, 2*PI]
		if (lockOnAngle[playerIndex] > IM_PI * 2.0f) lockOnAngle[playerIndex] -= IM_PI * 2.0f;

		CrimsonFX::ComputeLockOnScreenData(actorData, cameraData, playerIndex);

		auto& lockedEnemyScreenPosition = crimsonPlayer[playerIndex].lockedEnemyScreenPosition;

		// Adjusts size dynamically based on the distance between Camera and Player
		float textureBaseSizeX = 600.0f * scaleFactorY;
		float textureBaseSizeY = 581.0f * scaleFactorY;

		float rawDist = crimsonPlayer[playerIndex].cameraLockedEnemyDistance / 20.0f;
		float smoothDist = GetSmoothedLockOnDist(playerIndex, rawDist, ImGui::GetIO().DeltaTime);
		float safeDistance = (std::max)(smoothDist, g_capDistance);  // capped to prevent oversized icons when close

		ImVec2 sizeDistance = {
			(textureBaseSizeX * (1.0f / (safeDistance / 40))),
			(textureBaseSizeY * (1.0f / (safeDistance / 40)))
		};

		float textureWidth = sizeDistance.x * 0.18f;
		float textureHeight = sizeDistance.y * 0.18f;

		ImVec2 windowSize = ImVec2(700.0f * scaleFactorY, 700.0f * scaleFactorY);
		float edgeOffsetX = 350.0f * scaleFactorY;
		float edgeOffsetY = 350.0f * scaleFactorY;

		float offsetX = 0.45f;
		float offsetY = 0.39f;

		ImVec2 texturePos = ImVec2(
			lockedEnemyScreenPosition.x - (sizeDistance.x / 2.0f) + offsetX - (offsetX * 0.03f * playerIndex),
			lockedEnemyScreenPosition.y - (sizeDistance.y / 2.0f) + offsetY - (offsetY * 0.03f * playerIndex)
		);

		ImVec2 windowPos = ImVec2(
			texturePos.x + (sizeDistance.x / 2.0f) - (windowSize.x / 2.0f),
			texturePos.y + (sizeDistance.y / 2.0f) - (windowSize.y / 2.0f)
		);
		ImGui::SetNextWindowSize(windowSize);
		ImGui::SetNextWindowPos(windowPos);

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;

		std::string windowName = "LockOnShieldWindow" + std::to_string(playerIndex);

		ImGui::Begin(windowName.c_str(), nullptr, windowFlags);
		const bool viewClipped = PushSplitscreenViewClip(playerIndex);

		float alpha = 1.0f;

		ImVec4 shieldColor = ImGui::ColorConvertU32ToFloat4(UI::SwapColorEndianness(0xFFFFFFFF));

		ImColor color(shieldColor);
		float h, s, v;
		ImGui::ColorConvertRGBtoHSV(color.Value.x, color.Value.y, color.Value.z, h, s, v);

		// Boost saturation and value for more "pop"
		s = ImClamp(s * 1.4f, 0.0f, 1.0f); // Increase saturation by 40%
		v = ImClamp(v * 1.8f, 0.0f, 1.0f); // Increase brightness by 80%

		ImVec4 poppedColor;
		ImGui::ColorConvertHSVtoRGB(h, s, v, poppedColor.x, poppedColor.y, poppedColor.z);
		poppedColor.w = lockOnFade[playerIndex].alpha; // Set alpha

		ImColor colorWithAlpha(poppedColor);
		ImColor fgColorWithAlpha(1.0f, 1.0f, 1.0f, lockOnFade[playerIndex].alpha);

		CrimsonGameplay::GetLockedOnEnemyShield(actorData);
		auto& lockedOnEnemyShield = crimsonPlayer[playerIndex].lockedOnEnemyShield;
		auto& lockedOnEnemyMaxShield = crimsonPlayer[playerIndex].lockedOnEnemyMaxShield;

		float shieldFraction = lockedOnEnemyShield / lockedOnEnemyMaxShield;

		bool lockOnActive = (actorData.buttons[0] & GetBinding(BINDING::LOCK_ON)) && actorData.lockOnData.targetBaseAddr60 != 0;

		const float fadeInDuration = 0.25f;
		const float fadeOutDuration = 0.25f;
		UpdateFade(lockOnFade[playerIndex], lockOnActive, fadeInDuration, fadeOutDuration, ImGui::GetIO().DeltaTime);

		// Calculate centered texture position within the window
		ImVec2 windowContentPos = ImGui::GetWindowPos();
		ImVec2 centeredTexturePos = ImVec2(
			windowContentPos.x + (windowSize.x - textureWidth) * 0.5f,
			windowContentPos.y + (windowSize.y - textureHeight) * 0.47f
		);

		if (lockOnFade[playerIndex].alpha > 0.01f) {
			if (LockOnTexture->IsValid()) {
				DrawRotatedImagePie(
					LockOnShieldTexture->GetTexture(),
					centeredTexturePos,
					ImVec2(textureWidth, textureHeight),
					lockOnAngle[playerIndex],
					colorWithAlpha,
					shieldFraction
				);
				DrawRotatedImagePie(
					LockOnShieldTexture->GetTexture(),
					centeredTexturePos,
					ImVec2(textureWidth, textureHeight),
					lockOnAngle[playerIndex],
					fgColorWithAlpha,
					shieldFraction
				);
			} else {
				ImGui::GetWindowDrawList()->AddRectFilled(texturePos, ImVec2(texturePos.x + textureWidth, texturePos.y + textureHeight), ImColor(1.0f, 1.0f, 1.0f, alpha));
			}
		}

		PopSplitscreenViewClip(viewClipped);
		ImGui::End();
	}
}

ImColor GetStyleFlashColor(int style, uint8 character) {
	switch (style) {
	case STYLE::TRICKSTER:     if (character == CHARACTER::DANTE) return ImColor(UI::SwapColorEndianness(0xf4dc07FF)); 
								else return ImColor(UI::SwapColorEndianness(0x44bccdFF)); // Yellow / Light Blue
	case STYLE::SWORDMASTER:   return ImColor(UI::SwapColorEndianness(0xf30202FF)); // Red
	case STYLE::GUNSLINGER:    return ImColor(UI::SwapColorEndianness(0x0096bfff)); // Blue
	case STYLE::ROYALGUARD:    return ImColor(UI::SwapColorEndianness(0x00fe27FF)); // Green
	case STYLE::DOPPELGANGER:  return ImColor(UI::SwapColorEndianness(0xfe5102FF)); // Orange
	case STYLE::QUICKSILVER:   if (character == CHARACTER::DANTE) return ImColor(UI::SwapColorEndianness(0xd40af0FF));
						   else return ImColor(UI::SwapColorEndianness(0xb5b5b5FF));
		 // Purple
	default:                   return ImColor(UI::SwapColorEndianness(0xFFFFFFFF)); // White fallback
	}
}

static void StyleDisplayWindowView(int view) {
	auto* mainActorPtr = HudViewActor(view);
	if (!mainActorPtr) {
		return;
	}
	auto& mainActorData = *mainActorPtr;
	auto name_80 = *reinterpret_cast<byte8**>(appBaseAddr + 0xCF2680);
	if (!name_80) {
		return;
	}
	auto& hudData = *reinterpret_cast<HUDData*>(name_80);
	if (!(InGame() && !g_inGameCutscene)) {
		return;
	}
	if (!activeCrimsonConfig.CrimsonHudAddons.stylesDisplay || g_inCredits) {
		return;
	}
	// dmc3.exe + 27E86A - C7 86 38 69 00 00 00 00 48 43 - mov[rsi + 00006938], 43480000 { Top Left Count Alpha Timer
	// setting max alpha value(127) }

	// Animation state, one per view.
	struct State {
		int prevStyle = -1;
		int prevCharacter = -1;
		double shakeStartTime = 0.0;
		bool shaking = false;
		double flashStartTime = 0.0;
		bool flashing = false;
		ImColor lastFlashColor = ImColor(0xFFFFFFFF);
		double scaleStartTime = 0.0;
		bool scaling = false;
		float currentScale = 1.0f;
		double compassFlashStartTime = 0.0;
		bool compassFlashing = false;
		ImColor compassLastFlashColor = ImColor(0xDE1C4CFF);
	};
	static State states[PLAYER_COUNT + 1];
	State& state = states[view + 1];

	// --- Shake Animation State ---
	int& prevStyle = state.prevStyle;
	int& prevCharacter = state.prevCharacter;
	double& shakeStartTime = state.shakeStartTime;
	bool& shaking = state.shaking;
	const float shakeDuration = 0.1f; // seconds

	// --- Flash Animation State ---
	double& flashStartTime = state.flashStartTime;
	bool& flashing = state.flashing;
	const float flashDuration = 0.6f;
	ImColor& lastFlashColor = state.lastFlashColor;

	// --- Scale Animation State ---
	double& scaleStartTime = state.scaleStartTime;
	bool& scaling = state.scaling;
	const float scaleDuration = 0.1f;
	float& currentScale = state.currentScale;

	// --- Compass Letter Flash Animation State ---
	double& compassFlashStartTime = state.compassFlashStartTime;
	bool& compassFlashing = state.compassFlashing;
	const float compassFlashDuration = 0.25f;
	ImColor& compassLastFlashColor = state.compassLastFlashColor;

	auto& character = mainActorData.character;
	float inGameAlpha = hudData.topLeftAlpha / 127.0f;

	int currentStyle = mainActorData.style;
	int currentCharacter = mainActorData.character;

	// Detect style or character change
	if (currentStyle != prevStyle || currentCharacter != prevCharacter) {
		shakeStartTime = ImGui::GetTime();
		shaking = true;
		// Flash logic
		flashStartTime = ImGui::GetTime();
		flashing = true;
		lastFlashColor = GetStyleFlashColor(currentStyle, character);
		// Scale logic
		scaleStartTime = ImGui::GetTime();
		scaling = true;
		// Compass Letter Flash logic
		compassFlashStartTime = ImGui::GetTime();
		compassFlashing = true;
		compassLastFlashColor = GetStyleFlashColor(currentStyle, character);
		prevStyle = currentStyle;
		prevCharacter = currentCharacter;
	}

	float styleGlassTextureSizeX = 596.0f * 0.95f * scaleFactorY;
	float styleGlassTextureSizeY = 485.50f * 0.95f * scaleFactorY;

	ImVec2 styleGlassWindowSize = ImVec2(
		styleGlassTextureSizeX + (50.0f * scaleFactorY),
		styleGlassTextureSizeY + (50.0f * scaleFactorY));

	// Get the center of the screen 
	float screenCenterX = ImGui::GetIO().DisplaySize.x * 0.5f - styleGlassWindowSize.x;
	float screenCenterY = ImGui::GetIO().DisplaySize.y * 0.5f - styleGlassWindowSize.y;

	// Apply your negative offset from center
	ImVec2 styleGlassWindowPos = ImVec2(
		screenCenterX - (440.0f * scaleFactorY),
		-120.0f * scaleFactorY
	);

	ImVec2 styleCompassHybridWindowPos = ImVec2(
		screenCenterX - (395.0f * scaleFactorY),
		-120.0f * scaleFactorY
	);

	ImVec2 styleCompassWindowPos = activeCrimsonConfig.CrimsonHudAddons.positionings ?
		styleGlassWindowPos :
		styleCompassHybridWindowPos;

	const TopLeftHudSync hudSync = GetTopLeftHudSync(view);
	styleGlassWindowPos = hudSync.Pos(styleGlassWindowPos);
	styleCompassWindowPos = hudSync.Pos(styleCompassWindowPos);
	styleGlassTextureSizeX *= hudSync.scale;
	styleGlassTextureSizeY *= hudSync.scale;
	styleGlassWindowSize = ImVec2(styleGlassWindowSize.x * hudSync.scale, styleGlassWindowSize.y * hudSync.scale);

	ImGui::SetNextWindowSize(styleGlassWindowSize);
	ImGui::SetNextWindowPos(styleCompassWindowPos);

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;

	// STYLE COMPASS
	ImGui::Begin(HudViewWindowName("StyleCompassWindow", view), nullptr, windowFlags);
	HudViewClipGuard compassClip(view);

	{
		float textureWidth = styleGlassTextureSizeX;
		float textureHeight = styleGlassTextureSizeY;

		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 texturePos = ImVec2(
			windowPos.x,
			windowPos.y + (styleGlassWindowSize.y - textureHeight) * 0.5f
		);

		auto DrawSmallLetterCompass = [](ID3D11ShaderResourceView* texture, uint8 currentStyle, uint8 style,
			ImVec2 windowPos, ImVec2 styleGlassTextureSize, ImColor colorWithAlpha) {
				auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

				float textureWidth = styleGlassTextureSize.x;
				float textureHeight = styleGlassTextureSize.y;
				ImVec2 texturePosNoShake = ImVec2(
					windowPos.x,
					windowPos.y + (styleGlassTextureSize.y - textureHeight) * 0.5f
				);

				if (style != currentStyle) {
					if (style == STYLE::QUICKSILVER && !sessionData.weaponAndStyleUnlocks[WEAPONANDSTYLEUNLOCKS::QUICKSILVER]) return;
					if (style == STYLE::DOPPELGANGER && !sessionData.weaponAndStyleUnlocks[WEAPONANDSTYLEUNLOCKS::DOPPELGANGER]) return;
					ImGui::GetWindowDrawList()->AddImage(
						texture,
						texturePosNoShake,
						ImVec2(texturePosNoShake.x + textureWidth, texturePosNoShake.y + textureHeight),
						ImVec2(0, 0), ImVec2(1, 1),
						colorWithAlpha
					);
				}
			};

		// --- Apply Shake Offset ---
		float shakeOffsetX = 0.0f;
		float shakeOffsetY = 0.0f;
		const float shakeAmountX = 1.5f;
		const float shakeAmountY = 2.0f;
		if (shaking) {
			float t = static_cast<float>(ImGui::GetTime() - shakeStartTime);
			if (t < shakeDuration) {
				float norm = t / shakeDuration;
				float ease = -0.5f * (cosf(3.14159265f * norm) - 1.0f);
				float wave = sinf(norm * 8.0f * 3.14159265f);
				shakeOffsetX = wave * shakeAmountX * ease;
				shakeOffsetY = wave * shakeAmountY * ease;
			} else {
				shaking = false;
			}
		}
		texturePos.x += shakeOffsetX;
		texturePos.y += shakeOffsetY;

		ImColor colorWithAlpha(1.0f, 1.0f, 1.0f, inGameAlpha);
		auto usedActiveCompassTexture = tBig->GetTexture();

		if (character == CHARACTER::DANTE) {
			switch (mainActorData.style) {
			case (STYLE::TRICKSTER):
				usedActiveCompassTexture = tBig->GetTexture();
				break;
			case (STYLE::SWORDMASTER):
				usedActiveCompassTexture = sBig->GetTexture();
				break;
			case (STYLE::GUNSLINGER):
				usedActiveCompassTexture = gBig->GetTexture();
				break;
			case (STYLE::ROYALGUARD):
				usedActiveCompassTexture = rBig->GetTexture();
				break;
			case (STYLE::DOPPELGANGER):
				usedActiveCompassTexture = dBig->GetTexture();
				break;
			case (STYLE::QUICKSILVER):
				usedActiveCompassTexture = qBig->GetTexture();
				break;
			default:
				usedActiveCompassTexture = tBig->GetTexture();
				break;
			}
		}
		if (character == CHARACTER::DANTE) {
			DrawSmallLetterCompass(tSmall->GetTexture(), mainActorData.style, STYLE::TRICKSTER,
				texturePos, ImVec2(styleGlassTextureSizeX, styleGlassTextureSizeY), colorWithAlpha);
			DrawSmallLetterCompass(sSmall->GetTexture(), mainActorData.style, STYLE::SWORDMASTER,
				texturePos, ImVec2(styleGlassTextureSizeX, styleGlassTextureSizeY), colorWithAlpha);
			DrawSmallLetterCompass(gSmall->GetTexture(), mainActorData.style, STYLE::GUNSLINGER,
				texturePos, ImVec2(styleGlassTextureSizeX, styleGlassTextureSizeY), colorWithAlpha);
			DrawSmallLetterCompass(rSmall->GetTexture(), mainActorData.style, STYLE::ROYALGUARD,
				texturePos, ImVec2(styleGlassTextureSizeX, styleGlassTextureSizeY), colorWithAlpha);
			DrawSmallLetterCompass(dSmall->GetTexture(), mainActorData.style, STYLE::DOPPELGANGER,
				texturePos, ImVec2(styleGlassTextureSizeX, styleGlassTextureSizeY), colorWithAlpha);
			DrawSmallLetterCompass(qSmall->GetTexture(), mainActorData.style, STYLE::QUICKSILVER,
				texturePos, ImVec2(styleGlassTextureSizeX, styleGlassTextureSizeY), colorWithAlpha);

			// --- Scaling Animation for Active Compass Texture ---
			float scale = 1.0f;
			if (scaling) {
				float t = static_cast<float>(ImGui::GetTime() - scaleStartTime);
				if (t < scaleDuration) {
					float norm = t / scaleDuration;
					const float s = 9.70158f;
					float ease = 1.0f + s * powf(norm - 1.0f, 3) + s * (norm - 1.0f) * powf(norm - 1.0f, 2);
					scale = ImLerp(0.87f, 1.0f, ease);
				} else {
					scaling = false;
					scale = 1.0f;
				}
			}
			ImVec2 scaledSize = ImVec2(textureWidth * scale, textureHeight * scale);
			ImVec2 scaledPos = ImVec2(
				texturePos.x + (textureWidth - scaledSize.x) * 0.5f,
				texturePos.y + (textureHeight - scaledSize.y) * 0.5f
			);

			// --- Flash Color Animation for Active Compass Texture ---
			ImColor compassColorWithAlpha = ImColor(1.0f, 1.0f, 1.0f, inGameAlpha);
			ImVec4 flashV = compassLastFlashColor;
			ImVec4 whiteV = ImVec4(1.0f, 1.0f, 1.0f, inGameAlpha);
			ImVec4 baseV = ImLerp(flashV, whiteV, 1.0f);
			baseV.w = inGameAlpha;

			if (compassFlashing) {
				float t = static_cast<float>(ImGui::GetTime() - compassFlashStartTime);
				if (t < compassFlashDuration) {
					float lerpT = t / compassFlashDuration;
					lerpT = ImClamp(lerpT, 0.0f, 1.0f);
					ImVec4 lerped = ImLerp(flashV, baseV, lerpT); // Lerp from 100% color to 45% color
					lerped.w = inGameAlpha;
					compassColorWithAlpha = ImColor(lerped);
				} else {
					compassFlashing = false;
					compassColorWithAlpha = ImColor(baseV);
				}
			} else {
				compassColorWithAlpha = ImColor(baseV);
			}

			if (usedActiveCompassTexture) {
				ImGui::GetWindowDrawList()->AddImage(
					usedActiveCompassTexture,
					scaledPos,
					ImVec2(scaledPos.x + scaledSize.x, scaledPos.y + scaledSize.y),
					ImVec2(0, 0), ImVec2(1, 1),
					compassColorWithAlpha
				);
			}
		}
	}

	ImGui::End(); // End StyleCompassWindow

	ImGui::SetNextWindowSize(styleGlassWindowSize);
	ImGui::SetNextWindowPos(styleCompassWindowPos);

	// STYLE BADGE
	ImGui::Begin(HudViewWindowName("StyleBadgeWindow", view), nullptr, windowFlags);
	HudViewClipGuard badgeClip(view);

	{
		float textureWidth = styleGlassTextureSizeX;
		float textureHeight = styleGlassTextureSizeY;

		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 texturePos = ImVec2(
			windowPos.x,
			windowPos.y + (styleGlassWindowSize.y - textureHeight) * 0.5f
		);

		// --- Apply Shake Offset ---
		float shakeOffsetX = 0.0f;
		float shakeOffsetY = 0.0f;
		const float shakeAmountX = 1.5f;
		const float shakeAmountY = 2.0f;
		if (shaking) {
			float t = static_cast<float>(ImGui::GetTime() - shakeStartTime);
			if (t < shakeDuration) {
				float norm = t / shakeDuration;
				float ease = -0.5f * (cosf(3.14159265f * norm) - 1.0f);
				float wave = sinf(norm * 8.0f * 3.14159265f);
				shakeOffsetX = wave * shakeAmountX * ease;
				shakeOffsetY = wave * shakeAmountY * ease;
			} else {
				shaking = false;
			}
		}
		texturePos.x += shakeOffsetX;
		texturePos.y += shakeOffsetY;

		// --- Flash Color Logic ---
		ImColor badgeColor = ImColor(1.0f, 1.0f, 1.0f, inGameAlpha);
		ImVec4 flashV = lastFlashColor;
		ImVec4 whiteV = ImVec4(1.0f, 1.0f, 1.0f, inGameAlpha);
		ImVec4 baseV = ImLerp(flashV, whiteV, 0.55f); // 45% color, 55% white
		baseV.w = inGameAlpha;

		if (flashing) {
			float t = static_cast<float>(ImGui::GetTime() - flashStartTime);
			if (t < flashDuration) {
				float lerpT = t / flashDuration;
				lerpT = ImClamp(lerpT, 0.0f, 1.0f);
				ImVec4 lerped = ImLerp(flashV, baseV, lerpT); // Lerp from 100% color to 45% color
				lerped.w = inGameAlpha;
				badgeColor = ImColor(lerped);
			} else {
				flashing = false;
				badgeColor = ImColor(baseV);
			}
		} else {
			badgeColor = ImColor(baseV);
		}

		auto usedBadgeTexture = tricksterBadge->GetTexture();
		if (character == CHARACTER::DANTE) {
			switch (mainActorData.style) {
			case (STYLE::TRICKSTER):
				usedBadgeTexture = tricksterBadge->GetTexture();
				break;
			case (STYLE::SWORDMASTER):
				usedBadgeTexture = swordmasterBadge->GetTexture();
				break;
			case (STYLE::GUNSLINGER):
				usedBadgeTexture = gunslingerBadge->GetTexture();
				break;
			case (STYLE::ROYALGUARD):
				usedBadgeTexture = royalguardBadge->GetTexture();
				break;
			case (STYLE::DOPPELGANGER):
				usedBadgeTexture = doppelgangerBadge->GetTexture();
				break;
			case (STYLE::QUICKSILVER):
				usedBadgeTexture = quicksilverBadge->GetTexture();
				break;
			}
		} else if (character == CHARACTER::VERGIL) {
			switch (mainActorData.style) {
			case (STYLE::DARK_SLAYER):
				usedBadgeTexture = darkslayerVergilBadge->GetTexture();
				break;
			case (STYLE::QUICKSILVER):
				usedBadgeTexture = quicksilverVergilBadge->GetTexture();
				break;
			}
		}

		// Draw the texture
		if (usedBadgeTexture) {
			ImGui::GetWindowDrawList()->AddImage(
				usedBadgeTexture,
				texturePos,
				ImVec2(texturePos.x + textureWidth, texturePos.y + textureHeight),
				ImVec2(0, 0), ImVec2(1, 1),
				badgeColor
			);
		}

		if (diamondTGSR->GetTexture() && mainActorData.character == CHARACTER::DANTE) {
			ImGui::GetWindowDrawList()->AddImage(
				diamondTGSR->GetTexture(),
				texturePos,
				ImVec2(texturePos.x + textureWidth, texturePos.y + textureHeight),
				ImVec2(0, 0), ImVec2(1, 1),
				badgeColor
			);
		}
	}

	ImGui::End(); // End StyleBadgeWindow

	// STYLE GLASS 
	if (activeCrimsonConfig.CrimsonHudAddons.stylesDisplay == STYLESDISPLAY::WITH_BROKEN_GLASS) {
		ImGui::SetNextWindowSize(styleGlassWindowSize);
		ImGui::SetNextWindowPos(styleGlassWindowPos);

		ImGui::Begin(HudViewWindowName("StyleGlassWindow", view), nullptr, windowFlags);
		HudViewClipGuard glassClip(view);

		{
			float textureWidth = styleGlassTextureSizeX;
			float textureHeight = styleGlassTextureSizeY;

			ImVec2 windowPos = ImGui::GetWindowPos();
			ImVec2 texturePos = ImVec2(
				windowPos.x,
				windowPos.y + (styleGlassWindowSize.y - textureHeight) * 0.5f
			);

			// --- Apply Shake Offset ---
			float shakeOffsetX = 0.0f;
			float shakeOffsetY = 0.0f;
			const float shakeAmountX = 1.5f;
			const float shakeAmountY = 2.0f;
			if (shaking) {
				float t = static_cast<float>(ImGui::GetTime() - shakeStartTime);
				if (t < shakeDuration) {
					float norm = t / shakeDuration;
					float ease = -0.5f * (cosf(3.14159265f * norm) - 1.0f);
					float wave = sinf(norm * 8.0f * 3.14159265f);
					shakeOffsetX = wave * shakeAmountX * ease;
					shakeOffsetY = wave * shakeAmountY * ease;
				} else {
					shaking = false;
				}
			}
			texturePos.x += shakeOffsetX;
			texturePos.y += shakeOffsetY;

			// --- Scaling Animation for Glass Texture (same as style letter) ---
			float scale = 1.0f;
			if (scaling) {
				float t = static_cast<float>(ImGui::GetTime() - scaleStartTime);
				if (t < scaleDuration) {
					float norm = t / scaleDuration;
					const float s = -5.70158f;
					float ease = 1.0f + s * powf(norm - 1.0f, 3) + s * (norm - 1.0f) * powf(norm - 1.0f, 2);
					scale = ImLerp(0.87f, 1.0f, ease);
				} else {
					scale = 1.0f;
				}
			}
			ImVec2 scaledSize = ImVec2(textureWidth * scale, textureHeight * scale);
			ImVec2 scaledPos = ImVec2(
				texturePos.x + (textureWidth - scaledSize.x) * 0.5f,
				texturePos.y + (textureHeight - scaledSize.y) * 0.5f
			);

			ImColor colorWithAlpha(1.0f, 1.0f, 1.0f, inGameAlpha);

			auto usedGlassTexture = tricksterGlass->GetTexture();
			if (character == CHARACTER::DANTE) {
				switch (mainActorData.style) {
				case (STYLE::TRICKSTER):
					usedGlassTexture = tricksterGlass->GetTexture();
					break;
				case (STYLE::SWORDMASTER):
					usedGlassTexture = swordmasterGlass->GetTexture();
					break;
				case (STYLE::GUNSLINGER):
					usedGlassTexture = gunslingerGlass->GetTexture();
					break;
				case (STYLE::ROYALGUARD):
					usedGlassTexture = royalguardGlass->GetTexture();
					break;
				case (STYLE::DOPPELGANGER):
					usedGlassTexture = doppelgangerGlass->GetTexture();
					break;
				case (STYLE::QUICKSILVER):
					usedGlassTexture = quicksilverGlass->GetTexture();
					break;
				}
			} else if (character == CHARACTER::VERGIL) {
				switch (mainActorData.style) {
				case (STYLE::DARK_SLAYER):
					usedGlassTexture = darkslayerVergilGlass->GetTexture();
					break;
				case (STYLE::QUICKSILVER):
					usedGlassTexture = quicksilverVergilGlass->GetTexture();
					break;
				}
			}

			// Draw the texture with scaling
			if (usedGlassTexture) {
				ImGui::GetWindowDrawList()->AddImage(
					usedGlassTexture,
					scaledPos,
					ImVec2(scaledPos.x + scaledSize.x, scaledPos.y + scaledSize.y),
					ImVec2(0, 0), ImVec2(1, 1),
					colorWithAlpha
				);
			}
		}

		ImGui::End(); // End StyleGlassWindow
	}
}

void StyleDisplayWindow() {
	ForEachHudView(StyleDisplayWindowView);
}

static void StyleTextDisplayWindowView(int view) {
	auto* mainActorPtr = HudViewActor(view);
	if (!mainActorPtr) {
		return;
	}
	auto& mainActorData = *mainActorPtr;
	auto name_80 = *reinterpret_cast<byte8**>(appBaseAddr + 0xCF2680);
	if (!name_80) {
		return;
	}
	auto& hudData = *reinterpret_cast<HUDData*>(name_80);
	if (!(InGame() && !g_inGameCutscene)) {
		return;
	}
	if (activeCrimsonConfig.HudOptions.hideMainHUD || g_inCredits) {
		return;
	}
	auto& character = mainActorData.character;

	if (!activeCrimsonConfig.CrimsonHudAddons.displayStyleNames) return;
	if (character != CHARACTER::DANTE) return;

	float styleTextTextureSizeX = (1100.0f * 0.5f) * scaleFactorY;
	float styleTextTextureSizeY = (200.00f * 0.5f) * scaleFactorY;

	ImVec2 styleTextWindowSize = ImVec2(
		styleTextTextureSizeX + (50.0f * scaleFactorY),
		styleTextTextureSizeY + (50.0f * scaleFactorY));

	// Get the center of the screen 
	float screenCenterX = ImGui::GetIO().DisplaySize.x * 0.5f - styleTextWindowSize.x;
	float screenCenterY = ImGui::GetIO().DisplaySize.y * 0.5f - styleTextWindowSize.y;

	// Apply your negative offset from center
	ImVec2 styleTextWindowPos = ImVec2(
		screenCenterX - (200.0f * scaleFactorY),
		-47.0f * scaleFactorY);

	ImVec2 styleTextHybridWindowPos = ImVec2(
		screenCenterX - (200.0f * scaleFactorY),
		-20.0f * scaleFactorY);

	const TopLeftHudSync hudSync = GetTopLeftHudSync(view);
	const ImVec2 styleTextBasePos = activeCrimsonConfig.CrimsonHudAddons.positionings ? styleTextWindowPos : styleTextHybridWindowPos;
	styleTextTextureSizeX *= hudSync.scale;
	styleTextTextureSizeY *= hudSync.scale;
	styleTextWindowSize = ImVec2(styleTextWindowSize.x * hudSync.scale, styleTextWindowSize.y * hudSync.scale);

	ImGui::SetNextWindowSize(styleTextWindowSize);
	ImGui::SetNextWindowPos(hudSync.Pos(styleTextBasePos));

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;

	// Animation state, one per view.
	struct State {
		int prevStyle = -1;
		int prevCharacter = -1;
		double blurStartTime = 0.0;
		bool showBlur = false;
		double slideStartTime = 0.0;
		bool sliding = false;
	};
	static State states[PLAYER_COUNT + 1];
	State& state = states[view + 1];

	// --- Blur Animation State ---
	int& prevStyle = state.prevStyle;
	int& prevCharacter = state.prevCharacter;
	double& blurStartTime = state.blurStartTime;
	const float blurDuration = 0.13f;
	bool& showBlur = state.showBlur;

	// --- Slide Animation State ---
	double& slideStartTime = state.slideStartTime;
	const float slideDuration = 0.2f;
	bool& sliding = state.sliding;

	int currentStyle = mainActorData.style;
	int currentCharacter = mainActorData.character;

	// Detect style or character change
	if (currentStyle != prevStyle || currentCharacter != prevCharacter) {
		blurStartTime = ImGui::GetTime();
		showBlur = true;
		sliding = false; // Reset sliding, will start after blur
		prevStyle = currentStyle;
		prevCharacter = currentCharacter;
	}

	ImGui::Begin(HudViewWindowName("StyleTextWindow", view), nullptr, windowFlags);
	HudViewClipGuard textClip(view);
	{
		float textureWidth = styleTextTextureSizeX;
		float textureHeight = styleTextTextureSizeY;
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 texturePos = ImVec2(
			windowPos.x,
			windowPos.y + (styleTextWindowSize.y - textureHeight) * 0.5f
		);

		auto usedActiveTextTexture = tricksterText->GetTexture();
		auto usedActiveTextBlurTexture = tricksterTextBlur->GetTexture();
		if (character == CHARACTER::DANTE) {
			switch (mainActorData.style) {
			case (STYLE::TRICKSTER):
				usedActiveTextTexture = tricksterText->GetTexture();
				usedActiveTextBlurTexture = tricksterTextBlur->GetTexture();
				break;
			case (STYLE::SWORDMASTER):
				usedActiveTextTexture = swordmasterText->GetTexture();
				usedActiveTextBlurTexture = swordmasterTextBlur->GetTexture();
				break;
			case (STYLE::GUNSLINGER):
				usedActiveTextTexture = gunslingerText->GetTexture();
				usedActiveTextBlurTexture = gunslingerTextBlur->GetTexture();
				break;
			case (STYLE::ROYALGUARD):
				usedActiveTextTexture = royalguardText->GetTexture();
				usedActiveTextBlurTexture = royalguardTextBlur->GetTexture();
				break;
			case (STYLE::DOPPELGANGER):
				usedActiveTextTexture = doppelgangerText->GetTexture();
				usedActiveTextBlurTexture = doppelgangerTextBlur->GetTexture();
				break;
			case (STYLE::QUICKSILVER):
				usedActiveTextTexture = quicksilverText->GetTexture();
				usedActiveTextBlurTexture = quicksilverTextBlur->GetTexture();
				break;
			default:
				usedActiveTextTexture = tricksterText->GetTexture();
				usedActiveTextBlurTexture = tricksterTextBlur->GetTexture();
				break;
			}
		}

		float alpha = hudData.topLeftAlpha / 127.0f;

		// Blur logic: show blur for blurDuration after style change
		bool blurActive = false;
		if (showBlur) {
			float t = static_cast<float>(ImGui::GetTime() - blurStartTime);
			if (t < blurDuration) {
				blurActive = true;
			} else {
				showBlur = false;
				sliding = true;
				slideStartTime = ImGui::GetTime();
			}
		}

		if (blurActive && usedActiveTextBlurTexture) {
			ImGui::GetWindowDrawList()->AddImage(
				usedActiveTextBlurTexture,
				texturePos,
				ImVec2(texturePos.x + textureWidth, texturePos.y + textureHeight),
				ImVec2(0, 0), ImVec2(1, 1),
				ImColor(1.0f, 1.0f, 1.0f, alpha)
			);
		} else if (usedActiveTextTexture) {
			// --- Slide Animation for non-blurred text ---
			float offsetX = 0.0f;
			if (sliding) {
				float t = static_cast<float>(ImGui::GetTime() - slideStartTime);
				if (t < slideDuration) {
					float norm = t / slideDuration;
					// Ease out back (bouncy) curve
					const float s = 1.70158f * 1.2f; // slightly more bounce
					float ease = 1.0f + s * powf(norm - 1.0f, 3) + s * (norm - 1.0f) * powf(norm - 1.0f, 2);
					offsetX = ImLerp(-100.0f * scaleFactorY * hudSync.scale, 0.0f, ease);
				} else {
					sliding = false;
					offsetX = 0.0f;
				}
			}
			ImVec2 animatedPos = ImVec2(texturePos.x + offsetX, texturePos.y);


			ImGui::GetWindowDrawList()->AddImage(
				usedActiveTextTexture,
				animatedPos,
				ImVec2(animatedPos.x + textureWidth, animatedPos.y + textureHeight),
				ImVec2(0, 0), ImVec2(1, 1),
				ImColor(1.0f, 1.0f, 1.0f, alpha)
			);
		}
	}
	ImGui::End();
}

void StyleTextDisplayWindow() {
	ForEachHudView(StyleTextDisplayWindowView);
}

static void StyleEXPDisplayWindowView(int view) {
	auto* mainActorPtr = HudViewActor(view);
	if (!mainActorPtr) {
		return;
	}
	auto& mainActorData = *mainActorPtr;
	auto name_80 = *reinterpret_cast<byte8**>(appBaseAddr + 0xCF2680);
	if (!name_80) {
		return;
	}
	auto& hudData = *reinterpret_cast<HUDData*>(name_80);
	if (!(InGame() && !g_inGameCutscene)) {
		return;
	}
	if (!activeCrimsonConfig.CrimsonHudAddons.stylesDisplay || g_inCredits) {
		return;
	}

	if (mainActorData.style == STYLE::DOPPELGANGER || mainActorData.style == STYLE::QUICKSILVER) return;

	auto& character = mainActorData.character;
	if (character != CHARACTER::DANTE && character != CHARACTER::VERGIL) return;
	float alpha = hudData.topLeftAlpha / 127.0f;
	float styleExpTextureSizeX = (80.0f * 0.23f) * scaleFactorY;
	float styleExpTextureSizeY = (513.00f * 0.23f) * scaleFactorY;

	ImVec2 styleExpDispWindowSize = ImVec2(
		styleExpTextureSizeX + (50.0f * scaleFactorY),
		styleExpTextureSizeY + (50.0f * scaleFactorY));

	// Get the center of the screen 
	float screenCenterX = ImGui::GetIO().DisplaySize.x * 0.5f - styleExpDispWindowSize.x;
	float screenCenterY = ImGui::GetIO().DisplaySize.y * 0.5f - styleExpDispWindowSize.y;

	// Apply your negative offset from center
	ImVec2 styleExpDispWindowPosDefault = ImVec2(
		screenCenterX - (830.0f * scaleFactorY),
		180 * scaleFactorY);

	const float styleHudHyrbidPositioningOffsetX = StyleHudHybridPositioningOffsetX();
	ImVec2 styleExpDispWindowPos = styleExpDispWindowPosDefault;
	if (!activeCrimsonConfig.CrimsonHudAddons.positionings) {
		styleExpDispWindowPos.x += styleHudHyrbidPositioningOffsetX;
	}

	const TopLeftHudSync hudSync = GetTopLeftHudSync(view);
	styleExpDispWindowPos = hudSync.Pos(styleExpDispWindowPos);
	styleExpTextureSizeX *= hudSync.scale;
	styleExpTextureSizeY *= hudSync.scale;
	styleExpDispWindowSize = ImVec2(styleExpDispWindowSize.x * hudSync.scale, styleExpDispWindowSize.y * hudSync.scale);

	ImGui::SetNextWindowSize(styleExpDispWindowSize);
	ImGui::SetNextWindowPos(styleExpDispWindowPos);

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;

	// Animation state, one per view.
	struct State {
		double expShakeStartTime = 0.0;
		bool expShaking = false;
		int prevExpStyle = -1;
	};
	static State states[PLAYER_COUNT + 1];
	State& state = states[view + 1];

	// --- EXP Bar Shake Animation State ---
	double& expShakeStartTime = state.expShakeStartTime;
	bool& expShaking = state.expShaking;
	const float expShakeDuration = 0.1f; // seconds

	// Detect style change for shake (you may want to trigger this externally)
	int& prevExpStyle = state.prevExpStyle;
	int currentExpStyle = mainActorData.style;
	if (currentExpStyle != prevExpStyle) {
		expShakeStartTime = ImGui::GetTime();
		expShaking = true;
		prevExpStyle = currentExpStyle;
	}

	if (mainActorData.styleLevel == 2) return;

	ImGui::Begin(HudViewWindowName("StyleExpDisplayWindow", view), nullptr, windowFlags);
	HudViewClipGuard expClip(view);

	{
		float textureWidth = styleExpTextureSizeX;
		float textureHeight = styleExpTextureSizeY;
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 texturePos = ImVec2(
			windowPos.x,
			windowPos.y + (styleExpDispWindowSize.y - textureHeight) * 0.5f
		);

		// --- Apply EXP Shake Offset ---
		float expShakeOffsetX = 0.0f;
		float expShakeOffsetY = 0.0f;
		const float expShakeAmountX = 1.5f;
		const float expShakeAmountY = 2.0f;
		if (expShaking) {
			float t = static_cast<float>(ImGui::GetTime() - expShakeStartTime);
			if (t < expShakeDuration) {
				float norm = t / expShakeDuration;
				float ease = -0.5f * (cosf(3.14159265f * norm) - 1.0f);
				float wave = sinf(norm * 8.0f * 3.14159265f);
				expShakeOffsetX = wave * expShakeAmountX * ease;
				expShakeOffsetY = wave * expShakeAmountY * ease;
			} else {
				expShaking = false;
			}
		}
		texturePos.x += expShakeOffsetX;
		texturePos.y += expShakeOffsetY;

		auto expBorderTexture = character == CHARACTER::DANTE ? styleExpBarBorder->GetTexture() : styleExpBarBorderVergil->GetTexture();
		auto expFillTexture = styleExpBarFill->GetTexture();

		// Determine fill amount

		float maxFillAmount = mainActorData.styleLevel == 0 ? 30000.0f : 99999.0f;

		// Draw fill first
		if (expFillTexture) {
			RenderExpWithFill(expFillTexture,
				texturePos,
				ImVec2(textureWidth, textureHeight),
				mainActorData.styleExpPoints / maxFillAmount,
				ImColor(1.0f, 1.0f, 1.0f, alpha)
			);
		}

		// Draw border on top
		if (expBorderTexture) {
			ImGui::GetWindowDrawList()->AddImage(
				expBorderTexture,
				texturePos,
				ImVec2(texturePos.x + textureWidth, texturePos.y + textureHeight),
				ImVec2(0, 0), ImVec2(1, 1),
				ImColor(1.0f, 1.0f, 1.0f, alpha)
			);
		}
	}

	ImGui::End();
}

void StyleEXPDisplayWindow() {
	ForEachHudView(StyleEXPDisplayWindowView);
}

static void StyleLvlDispWindowView(int view) {
	PlayerActorData* actorDataPtr = nullptr;
	if (view >= 0) {
		actorDataPtr = HudViewActor(view);
	} else if (activeConfig.Actor.enable) {
		auto& playerData = GetPlayerData(0);
		auto& activeNewActorData = GetNewActorData(0, playerData.activeCharacterIndex, ENTITY::MAIN);
		if (activeNewActorData.baseAddr) {
			actorDataPtr = reinterpret_cast<PlayerActorData*>(activeNewActorData.baseAddr);
		}
	}
	else {
		actorDataPtr = GetVanillaPlayerActor();
	}

	if (!actorDataPtr) {
		return;
	}
	auto& actorData = *actorDataPtr;
	auto name_80 = *reinterpret_cast<byte8**>(appBaseAddr + 0xCF2680);
	if (!name_80) {
		return;
	}
	auto& hudData = *reinterpret_cast<HUDData*>(name_80);
	if (!(InGame() && !g_inGameCutscene)) {
		return;
	}
	if (!activeCrimsonConfig.CrimsonHudAddons.stylesDisplay || g_inCredits) {
		return;
	}
	
	if (actorData.style == STYLE::DOPPELGANGER || actorData.style == STYLE::QUICKSILVER) return;

	uint8 character = actorData.character;
	if (character != CHARACTER::DANTE && character != CHARACTER::VERGIL) return;
	float alpha = hudData.topLeftAlpha / 127.0f;
	float styleLvlTextureSizeX = (76.0f * 0.17f) * scaleFactorY;
	float styleLvlTextureSizeY = (97.0f * 0.17f) * scaleFactorY;

	ImVec2 styleLvlDispWindowSize = ImVec2(
		styleLvlTextureSizeX + (50.0f * 3 * scaleFactorY),
		styleLvlTextureSizeY + (50.0f * 3 * scaleFactorY));

	// Center of the screen
	float screenCenterX = ImGui::GetIO().DisplaySize.x * 0.5f;
	float screenCenterY = ImGui::GetIO().DisplaySize.y * 0.5f;

	ImVec2 styleLvlDispWindowPosDefault = character == CHARACTER::DANTE ? 
		ImVec2(
		screenCenterX - (920.0f * scaleFactorY),
		67.0f * scaleFactorY) : 
		ImVec2(
		screenCenterX - (858.0f * scaleFactorY),
		37.0f * scaleFactorY);

	const float styleHudHyrbidPositioningOffsetX = StyleHudHybridPositioningOffsetX();
	ImVec2 styleLvlDispWindowPos = styleLvlDispWindowPosDefault;
	if (!activeCrimsonConfig.CrimsonHudAddons.positionings) {
		styleLvlDispWindowPos.x += styleHudHyrbidPositioningOffsetX;
	}

	const TopLeftHudSync hudSync = GetTopLeftHudSync(view);
	styleLvlDispWindowPos = hudSync.Pos(styleLvlDispWindowPos);
	styleLvlTextureSizeX *= hudSync.scale;
	styleLvlTextureSizeY *= hudSync.scale;
	styleLvlDispWindowSize = ImVec2(styleLvlDispWindowSize.x * hudSync.scale, styleLvlDispWindowSize.y * hudSync.scale);
	const float scaleFactorY = CrimsonGUI::scaleFactorY * hudSync.scale;

	ImGui::SetNextWindowSize(styleLvlDispWindowSize);
	ImGui::SetNextWindowPos(styleLvlDispWindowPos);

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;

	ImGui::Begin(HudViewWindowName("StyleLvlDisplayWindow", view), nullptr, windowFlags);
	HudViewClipGuard lvlClip(view);

	ImColor diamondStyleColor = GetStyleFlashColor(actorData.style, character);
	diamondStyleColor.Value.w = alpha;

	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 diamondMin = ImVec2(windowPos.x + 25.0f * scaleFactorY, windowPos.y + 25.0f * scaleFactorY);
	ImVec2 diamondMax = ImVec2(diamondMin.x + styleLvlTextureSizeX, diamondMin.y + styleLvlTextureSizeY);

	ImGui::GetWindowDrawList()->AddImage(
		styleLevelDiamond->GetTexture(),
		diamondMin,
		diamondMax,
		ImVec2(0, 0), ImVec2(1, 1),
		diamondStyleColor
	);


	if (actorData.styleLevel == 0) {
		ImGui::End(); 
		return;
	}

	ImGui::GetWindowDrawList()->AddImage(
		styleLevelDiamond->GetTexture(),
		diamondMin + ImVec2(20 * scaleFactorY, 0),
		diamondMax + ImVec2(20 * scaleFactorY, 0),
		ImVec2(0, 0), ImVec2(1, 1),
		diamondStyleColor
	);

	if (actorData.styleLevel == 1) {
		ImGui::End();
		return;
	}

	ImGui::GetWindowDrawList()->AddImage(
		styleLevelDiamond->GetTexture(),
		diamondMin + ImVec2(40 * scaleFactorY, 0),
		diamondMax + ImVec2(40 * scaleFactorY, 0),
		ImVec2(0, 0), ImVec2(1, 1),
		diamondStyleColor
	);

	bool isInStyleLevel4 = actorData.character == CHARACTER::DANTE && (
		(actorData.style == STYLE::SWORDMASTER && ExpConfig::missionExpDataDante.unlocks[UNLOCK_DANTE::SWORDMASTER_MODDED_MOVES]) ||
		(actorData.style == STYLE::TRICKSTER && ExpConfig::missionExpDataDante.unlocks[UNLOCK_DANTE::TRICKSTER_MODDED_MOVES]) ||
		(actorData.style == STYLE::ROYALGUARD && ExpConfig::missionExpDataDante.unlocks[UNLOCK_DANTE::ROYALGUARD_MODDED_MOVES]) ||
		(actorData.style == STYLE::GUNSLINGER && ExpConfig::missionExpDataDante.unlocks[UNLOCK_DANTE::GUNSLINGER_MODDED_MOVES]) );

	if (!isInStyleLevel4) {
		ImGui::End();
		return;
	}

	ImGui::GetWindowDrawList()->AddImage(
		styleLevelDiamond->GetTexture(),
		diamondMin + ImVec2(60 * scaleFactorY, 0),
		diamondMax + ImVec2(60 * scaleFactorY, 0),
		ImVec2(0, 0), ImVec2(1, 1),
		diamondStyleColor
	);

	ImGui::End();
}

void StyleLvlDispWindow() {
	ForEachHudView(StyleLvlDispWindowView);
}

static void RoyalGaugeDispWindowView(int view) {
	auto* mainActorPtr = HudViewActor(view);
	if (!mainActorPtr) {
		return;
	}
	auto& mainActorData = *mainActorPtr;
	auto name_80 = *reinterpret_cast<byte8**>(appBaseAddr + 0xCF2680);
	if (!name_80) {
		return;
	}
	auto& hudData = *reinterpret_cast<HUDData*>(name_80);
	if (!(InGame() && !g_inGameCutscene)) {
		return;
	}
	if (!activeCrimsonConfig.CrimsonHudAddons.stylesDisplay || g_inCredits) {
		return;
	}

	auto& character = mainActorData.character;
	if (character != CHARACTER::DANTE) return;

	float alpha = hudData.topLeftAlpha / 127.0f;
	float royalGaugeTextureSizeX = (200.0f * 0.5f) * scaleFactorY;
	float royalGaugeTextureSizeY = (200.0f * 0.5f) * scaleFactorY;

	ImVec2 royalGaugeWindowSize = ImVec2(
		royalGaugeTextureSizeX + (50.0f * 3 * scaleFactorY),
		royalGaugeTextureSizeY + (50.0f * 3 * scaleFactorY));

	// Center of the screen
	float screenCenterX = ImGui::GetIO().DisplaySize.x * 0.5f;
	float screenCenterY = ImGui::GetIO().DisplaySize.y * 0.5f;

	ImVec2 royalGaugeWindowPosDefault =
		ImVec2(
			screenCenterX - (845.0f * scaleFactorY),
			83.0f * scaleFactorY);

	const float styleHudHyrbidPositioningOffsetX = StyleHudHybridPositioningOffsetX();
	ImVec2 royalGaugeWindowPos = royalGaugeWindowPosDefault;
	if (!activeCrimsonConfig.CrimsonHudAddons.positionings) {
		royalGaugeWindowPos.x += styleHudHyrbidPositioningOffsetX;
	}

	const TopLeftHudSync hudSync = GetTopLeftHudSync(view);
	royalGaugeWindowPos = hudSync.Pos(royalGaugeWindowPos);
	royalGaugeTextureSizeX *= hudSync.scale;
	royalGaugeTextureSizeY *= hudSync.scale;
	royalGaugeWindowSize = ImVec2(royalGaugeWindowSize.x * hudSync.scale, royalGaugeWindowSize.y * hudSync.scale);
	const float scaleFactorY = CrimsonGUI::scaleFactorY * hudSync.scale;

	ImGui::SetNextWindowSize(royalGaugeWindowSize);
	ImGui::SetNextWindowPos(royalGaugeWindowPos);

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;

	ImGui::Begin(HudViewWindowName("RoyalGaugeWindow", view), nullptr, windowFlags);
	HudViewClipGuard royalClip(view);

	float textureWidth = royalGaugeTextureSizeX;
	float textureHeight = royalGaugeTextureSizeY;
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 texturePos = ImVec2(
		windowPos.x,
		windowPos.y + (royalGaugeWindowSize.y - textureHeight) * 0.5f
	);

	float divisorRGLevel = 0.0;
	if (heldStyleExpDataDante.missionStyleLevels[STYLE::ROYALGUARD] == 0) {
		divisorRGLevel = 3000.0f;
	}
	else if (heldStyleExpDataDante.missionStyleLevels[STYLE::ROYALGUARD] == 1) {
		divisorRGLevel = 6000.0f;
	}
	else if (heldStyleExpDataDante.missionStyleLevels[STYLE::ROYALGUARD] == 2) {
		divisorRGLevel = 9000.0f;
	}

	float royalGaugeFraction = divisorRGLevel / 9000.0f; // Should be 0.0f to 1.0f
	float royalGaugeFraction2 = mainActorData.royalguardReleaseDamage / 9000.0f; // Should be 0.0f to 1.0f

	ImColor pinkColor = ImColor(UI::SwapColorEndianness(0x221b67FF));
	pinkColor.Value.w = alpha;

	if (mainActorData.style == STYLE::ROYALGUARD) {
		// LARGE CIRCLE (Pie)
			DrawRotatedImagePie(
				royalGaugeCircle->GetTexture(),
				texturePos,
				ImVec2(textureWidth, textureHeight),
				0.0f,
				pinkColor,
				royalGaugeFraction
		);

			DrawRotatedImagePie(
				royalGaugeCircle->GetTexture(),
				texturePos,
				ImVec2(textureWidth, textureHeight),
				0.0f,
				ImColor(1.0f, 1.0f, 1.0f, alpha),
				royalGaugeFraction2
			);
	} else {
		// Small CIRCLE (Pie, scaled and centered over the big one)
		float smallScale = 0.6f;
		float smallWidth = textureWidth * smallScale;
		float smallHeight = textureHeight * smallScale;
		ImVec2 smallPos = ImVec2(
			texturePos.x + (textureWidth - smallWidth) * 0.5f,
			texturePos.y + (textureHeight - smallHeight) * 0.5f - (15.0f * scaleFactorY)
		);
		DrawRotatedImagePie(
			royalGaugeCircle->GetTexture(),
			smallPos,
			ImVec2(smallWidth, smallHeight),
			0.0f,
			pinkColor,
			royalGaugeFraction
		);

		DrawRotatedImagePie(
			royalGaugeCircle->GetTexture(),
			smallPos,
			ImVec2(smallWidth, smallHeight),
			0.0f,
			ImColor(1.0f, 1.0f, 1.0f, alpha),
			royalGaugeFraction2
		);
	}

	ImGui::End();
}

void RoyalGaugeDispWindow() {
	ForEachHudView(RoyalGaugeDispWindowView);
}

static void MirageGaugeMainPlayerView(int view) {
	const int playerIndex = (view < 0) ? 0 : view;
	if (!(activeConfig.Actor.enable &&
		InGame() &&
		crimsonPlayer[playerIndex].character == CHARACTER::VERGIL &&
		!g_inGameCutscene &&
		activeCrimsonGameplay.Gameplay.Vergil.mirageTrigger)) {
		return;
	}
	static bool show = true;
	auto name_80 = *reinterpret_cast<byte8**>(appBaseAddr + 0xCF2680);
	if (!name_80) {
		return;
	}
	auto& hudData = *reinterpret_cast<HUDData*>(name_80);
	auto* mainActorPtr = HudViewActor(view);
	if (!mainActorPtr) {
		return;
	}
	auto& mainActorData = *mainActorPtr;

	auto& vergilDoppelganger = crimsonPlayer[playerIndex].vergilDoppelganger;
	auto miragePoints = vergilDoppelganger.miragePoints / vergilDoppelganger.maxMiragePoints;
	auto& mirageTriggerActive = mainActorData.doppelganger;

	const TopLeftHudSync hudSync = GetTopLeftHudSync(view);
	const float baseScaleFactorY = CrimsonGUI::scaleFactorY;
	const float scaleFactorY = baseScaleFactorY * hudSync.scale;

	float dpadTextureWidth = (200.0f * 0.17f) * scaleFactorY;
	float dpadTextureHeight = (200.0f * 0.17f) * scaleFactorY;

	// Adjust the size of the bar
	float barLength = 130.0f * (vergilDoppelganger.maxMiragePoints / maxMiragePointsAmount) * scaleFactorY;
	vec2 size = { barLength, 10.0f * scaleFactorY };

	// Calculate positions
	float padding = 8.0f * scaleFactorY;
	const ImVec2 gaugePos = hudSync.Pos(ImVec2(70.0f * baseScaleFactorY, 150.0f * baseScaleFactorY));
	float posX = gaugePos.x;
	float posY = gaugePos.y;

	// The bar's X position is shifted to the right by the dpad texture width + padding
	float barPosX = posX + dpadTextureWidth + padding;

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;

	// Window size should fit both dpad and bar
	float windowWidth = dpadTextureWidth + padding + barLength + (50.0f * scaleFactorY);
	float windowHeight = std::max(dpadTextureHeight, 70.0f * scaleFactorY);

	ImGui::SetNextWindowPos(ImVec2(posX, posY));
	ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));

	// --- Fade logic ---
	static float fades[PLAYER_COUNT + 1] = {};
	float& fade = fades[view + 1];
	const float fadeSpeed = 6.0f * ImGui::GetIO().DeltaTime; // Adjust speed as needed

	if (mirageTriggerActive) {
		fade += fadeSpeed;
		if (fade > 1.0f) fade = 1.0f;
	} else {
		fade -= fadeSpeed;
		if (fade < 0.0f) fade = 0.0f;
	}

	// Colors
	ImVec4 inactiveColor = CrimsonUtil::HexToImVec4(0xFFFFFFFF);
	ImVec4 activeColor = CrimsonUtil::HexToImVec4(0x0791d9FF);

	// Lerp between inactive and active color based on fade
	ImVec4 mirageGaugeColor;
	mirageGaugeColor.x = inactiveColor.x + (activeColor.x - inactiveColor.x) * fade;
	mirageGaugeColor.y = inactiveColor.y + (activeColor.y - inactiveColor.y) * fade;
	mirageGaugeColor.z = inactiveColor.z + (activeColor.z - inactiveColor.z) * fade;
	mirageGaugeColor.w = (hudData.topLeftAlpha / 127.0f);

	float miragePointsColor[4] = { 1.0f , 1.0f, 1.0, hudData.topLeftAlpha / 127.0f };
	float progressBarBgColor[4] = { 0.2f , 0.2f, 0.2f, hudData.topLeftAlpha / 127.0f };

	if (ImGui::Begin(HudViewWindowName("MirageMainPlayer", view), &show, windowFlags)) {
		HudViewClipGuard mirageClip(view);
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImVec2 windowPos = ImGui::GetWindowPos();

		// Draw dpad texture at the left
		ImVec2 dpadPos = ImVec2(windowPos.x, (windowPos.y + 19.0f * scaleFactorY));
		drawList->AddImage(
			mirageGaugeDpadTexture->GetTexture(),
			dpadPos,
			ImVec2(dpadPos.x + dpadTextureWidth, dpadPos.y + dpadTextureHeight),
			ImVec2(0, 0), ImVec2(1, 1),
			*reinterpret_cast<ImColor*>(&miragePointsColor)
		);
		// Draw active overlay with fade
		if (fade > 0.0f) {
			ImColor fadeColor = ImColor(mirageGaugeColor);
			drawList->AddImage(
				mirageGaugeDpadActiveTexture->GetTexture(),
				dpadPos,
				ImVec2(dpadPos.x + dpadTextureWidth, dpadPos.y + dpadTextureHeight),
				ImVec2(0, 0), ImVec2(1, 1),
				fadeColor
			);
		}

		// Move cursor to the right for the bar
		ImGui::SetCursorPos(ImVec2(dpadTextureWidth + padding, (windowHeight - size.y) * 0.5f));

		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, mirageGaugeColor);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, *reinterpret_cast<ImVec4*>(&progressBarBgColor));
		ImGui::PushStyleColor(ImGuiCol_Border, *reinterpret_cast<ImVec4*>(&progressBarBgColor));
		ImGui::ProgressBar(miragePoints, *reinterpret_cast<ImVec2*>(&size), "");
		ImGui::PopStyleColor(3);
	}

	ImGui::End();
}

void MirageGaugeMainPlayer() {
	ForEachHudView(MirageGaugeMainPlayerView);
}

Texture2DD3D11 *getCrimsonGradient()
{
	return crimsonTitleGradient;
}

Texture2DD3D11 *getRedOrbTexture()
{
	if (!activeCrimsonConfig.CrimsonHudAddons.redOrbCounter && RedOrbVanillaTexture && RedOrbVanillaTexture->IsValid()) {
		return RedOrbVanillaTexture;
	}
	return RedOrbTexture;
}

#pragma region Player Icons

// Screen area a world-space HUD element belongs to: the whole screen, or one splitscreen view.
struct WorldHudView {
	int viewer = -1; // splitscreen view index, -1 = whole screen
	float minX = 0.0f;
	float minY = 0.0f;
	float maxX = 0.0f;
	float maxY = 0.0f;
	float scale = 1.0f;

	float Width() const { return maxX - minX; }
	float Height() const { return maxY - minY; }
};

static WorldHudView MakeWorldHudView(int viewer) {
	WorldHudView view;
	view.maxX = g_uiSize.x;
	view.maxY = g_uiSize.y;
	if (viewer >= 0 && CrimsonSplitscreen::ViewRect(viewer, g_uiSize.x, g_uiSize.y, view.minX, view.minY, view.maxX, view.maxY)) {
		view.viewer = viewer;
		view.scale = CrimsonSplitscreen::ViewScale(viewer);
	}
	return view;
}

static void RenderOutOfViewIcon(PlayerActorData actorData, SimpleVec3& screen_pos, float screenMargin, const char* name, const WorldHudView& view) {
	auto pool_4449 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC8FBD0);
	if (!pool_4449 || !pool_4449[147]) return;
	auto& cameraData = *reinterpret_cast<CameraData*>(pool_4449[147]);

	auto pool_12857 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_12857 || !pool_12857[3]) return;
	auto& mainActorData = *reinterpret_cast<PlayerActorDataDante*>(pool_12857[3]);

	auto playerIndex = actorData.newPlayerIndex;

	// Everything below works in view-local coordinates and is offset by the view origin at the end.
	const float screenWidth = view.Width();
	const float screenHeight = view.Height();
	const float screenCenterX = screenWidth / 2.0f;
	const float screenCenterY = screenHeight / 2.0f;
	const size_t iconSlotBase = static_cast<size_t>((view.viewer >= 0) ? view.viewer : 0) * PLAYER_COUNT;

	ImVec2 iconPosition;

	glm::vec3 cameraPos = { cameraData.data[0].x, cameraData.data[0].y, cameraData.data[0].z };
	glm::vec3 playerPos = { actorData.position.x, actorData.position.y, actorData.position.z };

	// The distance shown is from the player the view belongs to, or from 1P without splitscreen.
	float distanceToViewer = crimsonPlayer[playerIndex].playerTo1PDistance;
	if (view.viewer >= 0) {
		const auto& viewerPos = g_plEntityPositions[view.viewer * 2];
		distanceToViewer = glm::distance(playerPos, glm::vec3(viewerPos.x, viewerPos.y, viewerPos.z));
	}

	// Calculate the camera's forward vector based on the main actor's (1P) position
	glm::vec3 mainActorPos = { mainActorData.position.x, mainActorData.position.y, mainActorData.position.z };
	SimpleVec3 cameraForwardVec = {
		mainActorPos.x - cameraPos.x,
		mainActorPos.y - cameraPos.y,
		mainActorPos.z - cameraPos.z
	};
	if (const uint8_t* camCopy = CrimsonSplitscreen::CameraCopy(view.viewer)) {
		// Each splitscreen view has its own camera.
		const float* eye = reinterpret_cast<const float*>(camCopy + 0x70);
		const float* lookAt = reinterpret_cast<const float*>(camCopy + 0x80);
		cameraPos = { eye[0], eye[1], eye[2] };
		cameraForwardVec = { lookAt[0] - eye[0], lookAt[1] - eye[1], lookAt[2] - eye[2] };
	}

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
		float deltaX = screen_pos.x - view.minX - screenCenterX;
		float deltaY = screen_pos.y - view.minY - screenCenterY;
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
	float scaleFactorX = g_uiSize.x * view.scale / baseResX;
	float scaleFactorY = g_uiSize.y * view.scale / baseResY;

	ImVec2 buttonSize(80.0f * scaleFactorX, 80.0f * scaleFactorY);

	// Clamp the Icon Position again to ensure Button doesn't go out of screen bounds:
	iconPosition.x = CrimsonUtil::sexy_clamp(iconPosition.x, screenMargin, screenWidth - screenMargin - buttonSize.x + 30.0f);
	iconPosition.y = CrimsonUtil::sexy_clamp(iconPosition.y, screenMargin, screenHeight - screenMargin - buttonSize.y + 30.0f);

	auto IsOverlapping = [&](ImVec2 pos1, ImVec2 pos2, ImVec2 size) {
		return !(pos1.x + size.x <= pos2.x || pos1.x >= pos2.x + size.x ||
			pos1.y + size.y <= pos2.y || pos1.y >= pos2.y + size.y);
		};

	// Check for overlap between icons and reposition if necessary
	for (int i = 0; i < PLAYER_COUNT; ++i) {
		if (i == playerIndex) continue;

		const auto& previousPos = crimsonHud.playerOutViewIconPositions[iconSlotBase + i];
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

	crimsonHud.playerOutViewIconPositions[iconSlotBase + playerIndex] = iconPosition;

	// Render the icon at the adjusted position with label and distance
	ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;

	ImGui::SetNextWindowPos(ImVec2(view.minX + iconPosition.x, view.minY + iconPosition.y));
	const std::string iconWindowName = "out of view icon " + std::to_string(view.viewer) + "_" + std::to_string(playerIndex);
	ImGui::Begin(iconWindowName.c_str(), nullptr, windowFlags);
	if (view.viewer >= 0) {
		ImGui::PushClipRect(ImVec2(view.minX, view.minY), ImVec2(view.maxX, view.maxY), true);
	}

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
	float fontSizeNameLabel = (std::max)(23.0f * scaleFactorY, minFontSize);

	// Draw the distance text centered inside the button
	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[fontSizeDistance]);
	ImGui::PushStyleColor(ImGuiCol_Text, textColor);
	std::string distanceText = std::to_string(static_cast<int>(distanceToViewer));
	ImVec2 distanceTextSize = ImGui::CalcTextSize(distanceText.c_str());
	ImVec2 distanceTextPos = ImVec2(
		buttonPos.x + (buttonSize.x - distanceTextSize.x) * 0.5f,
		buttonPos.y + 5.0f * scaleFactorY
	);
	drawList->AddText(distanceTextPos, ImGui::GetColorU32(ImGuiCol_Text), distanceText.c_str());
	ImGui::PopStyleColor();
	ImGui::PopFont();

	// Draw name label (2P, 3P, etc)
	ImGui::PushFont(UI::g_ImGuiFont_RussoOne[fontSizeNameLabel]);
	ImGui::PushStyleColor(ImGuiCol_Text, textColor);
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

	ImVec4 hitColor = CrimsonUtil::HexToImVec4(0x43fe65FF);
	ImVec4 magicColor = CrimsonUtil::HexToImVec4(0xde1c4cFF);
	ImVec4 magicColorVergil = { 0.06f, 0.74f, 0.81f, 1.0f };

	// HP Bar
	ImGui::SetCursorScreenPos(ImVec2(buttonPos.x, buttonPos.y + (buttonSize.y * 0.75f)));
	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, *reinterpret_cast<ImVec4*>(&hitColor));
	ImGui::ProgressBar(actorData.hitPoints / actorData.maxHitPoints, progressBarSize, "");
	ImGui::PopStyleColor();

	// DT Bar
	ImGui::SetCursorScreenPos(ImVec2(buttonPos.x, buttonPos.y + (buttonSize.y * 0.75f) + progressBarSize.y + progressBarPadding));
	auto& magicColorChosen = actorData.character != CHARACTER::VERGIL ? magicColor : magicColorVergil;
	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, magicColorChosen);
	ImGui::ProgressBar(actorData.magicPoints / actorData.maxMagicPoints, progressBarSize, "");
	ImGui::PopStyleColor();

	if (view.viewer >= 0) {
		ImGui::PopClipRect();
	}
	ImGui::End();
}

static void RenderWorldSpacePlayerIcon(const PlayerActorData& actorData, const char* name, const char* baseLabel, int viewer) {
	auto playerIndex = actorData.newPlayerIndex;
	auto& activeData = activeConfig.barsData[playerIndex];
	const float hitPoints = actorData.hitPoints / actorData.maxHitPoints;
	const float magicPoints = actorData.magicPoints / actorData.maxMagicPoints;

	const WorldHudView view = MakeWorldHudView(viewer);
	const float scaleFactorY = CrimsonGUI::scaleFactorY * view.scale;
	// Each splitscreen view draws its own copy of the bar, projected with that view's camera.
	const std::string labelStorage = (view.viewer >= 0) ? (std::string(baseLabel) + "##view" + std::to_string(view.viewer)) : std::string(baseLabel);
	const char* label = labelStorage.c_str();

	auto distanceClamped = crimsonPlayer[playerIndex].cameraPlayerDistanceClamped;
	float cameraPlayerDistance = crimsonPlayer[playerIndex].cameraPlayerDistance;
	SimpleVec3 playerScreenPosition = crimsonPlayer[playerIndex].playerScreenPosition;
	if (view.viewer >= 0) {
		playerScreenPosition = debug_draw_world_to_screen_for_player(reinterpret_cast<const float*>(&actorData.position), 1.0f, view.viewer);
		float eye[3];
		if (CrimsonSplitscreen::CameraEye(view.viewer, eye)) {
			const glm::vec3 eyePos(eye[0], eye[1], eye[2]);
			cameraPlayerDistance = glm::distance(glm::vec3(actorData.position.x, actorData.position.y, actorData.position.z), eyePos);
			distanceClamped = static_cast<decltype(distanceClamped)>(glm::clamp(static_cast<int>(cameraPlayerDistance) / 20, 0, 255));
		}
	}

	// Adjusts size dynamically based on the distance between Camera and Player
	ImVec2 sizeDistance = { (activeData.size.x * (1.0f / ((float)distanceClamped / 20))) * scaleFactorY, (activeData.size.y * (1.0f / ((float)distanceClamped / 20))) * scaleFactorY };
	// Coordinates below are in screen space; the view bounds are the whole screen without splitscreen.
	const float screenMargin = 50.0f * view.scale;

	const float t = CrimsonUtil::smoothstep(0.0f, 1390.0f, cameraPlayerDistance);
	const float alpha = ImLerp(0.27f, 1.0f, t);
	ImVec4 hitColor = CrimsonUtil::HexToImVec4(0x43fe65FF);
	ImVec4 magicColor = CrimsonUtil::HexToImVec4(0xde1c4cFF);
	ImVec4 magicColorVergil = { 0.06f, 0.74f, 0.81f, 1.0f };
	hitColor.z = alpha;
	magicColor.z = alpha;
	magicColorVergil.z = alpha;

	// If player is outside view: Handle Out of View Icons and stop rendering WorldSpace Bars
	if (playerScreenPosition.x < view.minX + screenMargin || playerScreenPosition.x > view.maxX - screenMargin ||
		playerScreenPosition.y < view.minY + screenMargin || playerScreenPosition.y > view.maxY - screenMargin) {

		if (activeCrimsonConfig.MultiplayerBarsWorldSpace.showOutOfViewIcons) {
			RenderOutOfViewIcon(actorData, playerScreenPosition, screenMargin, name, view);
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

	const bool windowOpen = ImGui::Begin(label, &activeCrimsonConfig.MultiplayerBarsWorldSpace.show, windowFlags);
	if (view.viewer >= 0) {
		ImGui::PushClipRect(ImVec2(view.minX, view.minY), ImVec2(view.maxX, view.maxY), true);
	}
	if (windowOpen) {
		ImGui::SetWindowFontScale(scaleFactorY);
		ImGui::PushFont(UI::g_ImGuiFont_RussoOne[18.0 * 1.1f]);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, alpha));
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, *reinterpret_cast<ImVec4*>(&hitColor));
		ImGui::ProgressBar(hitPoints, sizeDistance, "");
		ImGui::PopStyleColor(2);

		if (actorData.character != CHARACTER::VERGIL) {
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, *reinterpret_cast<ImVec4*>(&magicColor));
		} else {
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, *reinterpret_cast<ImVec4*>(&magicColorVergil));
		}
		ImGui::ProgressBar(magicPoints, sizeDistance, "");
		ImGui::PopStyleColor();

		ImVec4 playerColor = ConvertColorFromUint8ToVec4(activeCrimsonConfig.PlayerProperties.playerColor[playerIndex]);

		float luminance = 0.299f * playerColor.x + 0.587f * playerColor.y + 0.114f * playerColor.z;
		ImVec4 textColor = (luminance > 0.4f) ? ImVec4(0.1f, 0.1f, 0.1f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

		ImGui::PushStyleColor(ImGuiCol_Button, playerColor);
		ImGui::PushStyleColor(ImGuiCol_Text, textColor);
		ImGui::Button(name, { 30.0f * scaleFactorY, 30.0f * scaleFactorY });
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

	if (view.viewer >= 0) {
		ImGui::PopClipRect();
	}
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(4);
}

// Splitscreen: every view gets its own copy of the player's bar and out-of-view icon.
void WorldSpacePlayerIcons(const PlayerActorData& actorData, const char* name, const char* label) {
	const int views = CrimsonSplitscreen::ViewCount();
	if (views < 2) {
		RenderWorldSpacePlayerIcon(actorData, name, label, -1);
		return;
	}
	for (int viewer = 0; viewer < views; ++viewer) {
		RenderWorldSpacePlayerIcon(actorData, name, label, viewer);
	}
}

#pragma endregion



void InitTextures(ID3D11Device* pd3dDevice) {
	InitRedOrbTexture(pd3dDevice);
	InitStyleRankTextures(pd3dDevice);
	InitLockOnTexture(pd3dDevice);
	InitStyleGlassTextures(pd3dDevice);
	InitMirageGaugeTextures(pd3dDevice);
	InitGradientTextures(pd3dDevice);
}

}
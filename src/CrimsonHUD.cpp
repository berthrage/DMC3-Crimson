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
#include "CrimsonHUD.hpp"
#include "CrimsonGameplay.hpp"
#include "DebugDrawDX11.hpp"
#define DEBUG_DRAW_EXPLICIT_CONTEXT
#include "debug_draw.hpp"

namespace CrimsonHUD {

using namespace CrimsonGUI;


// RED ORB
static Texture2DD3D11* RedOrbTexture{ nullptr };
static Texture2DD3D11* RedOrbVanillaTexture{ nullptr };
static Texture2DD3D11* RedOrbStyleSwitcherTexture{ nullptr };
static Texture2DD3D11* RedOrbCrimsonTexture{ nullptr };
static Texture2DD3D11* RedOrbCustomTexture{ nullptr };


// STYLE RANK METER
static Texture2DD3D11* DStyleRankFillTexture{ nullptr };
static Texture2DD3D11* DStyleRankBackgroundTexture{ nullptr };
static Texture2DD3D11* DStyleRankBackgroundHighlightTexture{ nullptr };
static Texture2DD3D11* DStyleRankTextTexture{ nullptr };
static Texture2DD3D11* DStyleRankTextHighlightTexture{ nullptr };

static Texture2DD3D11* CStyleRankFillTexture{ nullptr };
static Texture2DD3D11* CStyleRankBackgroundTexture{ nullptr };
static Texture2DD3D11* CStyleRankBackgroundHighlightTexture{ nullptr };
static Texture2DD3D11* CStyleRankTextTexture{ nullptr };
static Texture2DD3D11* CStyleRankTextHighlightTexture{ nullptr };

static Texture2DD3D11* BStyleRankFillTexture{ nullptr };
static Texture2DD3D11* BStyleRankBackgroundTexture{ nullptr };
static Texture2DD3D11* BStyleRankBackgroundHighlightTexture{ nullptr };
static Texture2DD3D11* BStyleRankTextTexture{ nullptr };
static Texture2DD3D11* BStyleRankTextHighlightTexture{ nullptr };

static Texture2DD3D11* AStyleRankFillTexture{ nullptr };
static Texture2DD3D11* AStyleRankBackgroundTexture{ nullptr };
static Texture2DD3D11* AStyleRankBackgroundHighlightTexture{ nullptr };
static Texture2DD3D11* AStyleRankTextTexture{ nullptr };
static Texture2DD3D11* AStyleRankTextHighlightTexture{ nullptr };

static Texture2DD3D11* SStyleRankFillTexture{ nullptr };
static Texture2DD3D11* SStyleRankBackgroundTexture{ nullptr };
static Texture2DD3D11* SStyleRankBackgroundHighlightTexture{ nullptr };
static Texture2DD3D11* SStyleRankTextTexture{ nullptr };
static Texture2DD3D11* SStyleRankTextHighlightTexture{ nullptr };

static Texture2DD3D11* SSStyleRankFillTexture{ nullptr };
static Texture2DD3D11* SSStyleRankBackgroundTexture{ nullptr };
static Texture2DD3D11* SSStyleRankBackgroundHighlightTexture{ nullptr };
static Texture2DD3D11* SSStyleRankTextTexture{ nullptr };
static Texture2DD3D11* SSStyleRankTextHighlightTexture{ nullptr };

static Texture2DD3D11* SSSStyleRankFillTexture{ nullptr };
static Texture2DD3D11* SSSStyleRankBackgroundTexture{ nullptr };
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
	DStyleRankFillTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "Dfill.png").c_str(), pd3dDevice);
	DStyleRankBackgroundTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "Dbg.png").c_str(), pd3dDevice);
	DStyleRankBackgroundHighlightTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "Dbghighlight.png").c_str(), pd3dDevice);
	DStyleRankTextTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "Dtext.png").c_str(), pd3dDevice);
	DStyleRankTextHighlightTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "Dtexthighlight.png").c_str(), pd3dDevice);

	CStyleRankFillTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "Cfill.png").c_str(), pd3dDevice);
	CStyleRankBackgroundTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "Cbg.png").c_str(), pd3dDevice);
	CStyleRankBackgroundHighlightTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "Cbghighlight.png").c_str(), pd3dDevice);
	CStyleRankTextTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "Ctext.png").c_str(), pd3dDevice);
	CStyleRankTextHighlightTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "Ctexthighlight.png").c_str(), pd3dDevice);

	BStyleRankFillTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "Bfill.png").c_str(), pd3dDevice);
	BStyleRankBackgroundTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "Bbg.png").c_str(), pd3dDevice);
	BStyleRankBackgroundHighlightTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "Bbghighlight.png").c_str(), pd3dDevice);
	BStyleRankTextTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "Btext.png").c_str(), pd3dDevice);
	BStyleRankTextHighlightTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "Btexthighlight.png").c_str(), pd3dDevice);

	AStyleRankFillTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "Afill.png").c_str(), pd3dDevice);
	AStyleRankBackgroundTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "Abg.png").c_str(), pd3dDevice);
	AStyleRankBackgroundHighlightTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "Abghighlight.png").c_str(), pd3dDevice);
	AStyleRankTextTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "Atext.png").c_str(), pd3dDevice);
	AStyleRankTextHighlightTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "Atexthighlight.png").c_str(), pd3dDevice);

	SStyleRankFillTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "Sfill.png").c_str(), pd3dDevice);
	SStyleRankBackgroundTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "Sbg.png").c_str(), pd3dDevice);
	SStyleRankBackgroundHighlightTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "Sbghighlight.png").c_str(), pd3dDevice);
	SStyleRankTextTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "Stext.png").c_str(), pd3dDevice);
	SStyleRankTextHighlightTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "Stexthighlight.png").c_str(), pd3dDevice);

	SSStyleRankFillTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "SSfill.png").c_str(), pd3dDevice);
	SSStyleRankBackgroundTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "SSbg.png").c_str(), pd3dDevice);
	SSStyleRankBackgroundHighlightTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "SSbghighlight.png").c_str(), pd3dDevice);
	SSStyleRankTextTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "SStext.png").c_str(), pd3dDevice);
	SSStyleRankTextHighlightTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "SStexthighlight.png").c_str(), pd3dDevice);

	SSSStyleRankFillTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "SSSfill.png").c_str(), pd3dDevice);
	SSSStyleRankBackgroundTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "SSSbg.png").c_str(), pd3dDevice);
	SSSStyleRankBackgroundHighlightTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "SSSbghighlight.png").c_str(), pd3dDevice);
	SSSStyleRankTextTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "SSStext.png").c_str(), pd3dDevice);
	SSSStyleRankTextHighlightTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "SSStexthighlight.png").c_str(), pd3dDevice);

	assert(DStyleRankFillTexture);
	assert(DStyleRankBackgroundTexture);
	assert(DStyleRankBackgroundHighlightTexture);
	assert(DStyleRankTextTexture);
	assert(DStyleRankTextHighlightTexture);
	assert(CStyleRankFillTexture);
	assert(CStyleRankBackgroundTexture);
	assert(CStyleRankBackgroundHighlightTexture);
	assert(CStyleRankTextTexture);
	assert(CStyleRankTextHighlightTexture);
	assert(BStyleRankFillTexture);
	assert(BStyleRankBackgroundTexture);
	assert(BStyleRankBackgroundHighlightTexture);
	assert(BStyleRankTextTexture);
	assert(BStyleRankTextHighlightTexture);
	assert(AStyleRankFillTexture);
	assert(AStyleRankBackgroundTexture);
	assert(AStyleRankBackgroundHighlightTexture);
	assert(AStyleRankTextTexture);
	assert(AStyleRankTextHighlightTexture);
	assert(SStyleRankFillTexture);
	assert(SStyleRankBackgroundTexture);
	assert(SStyleRankBackgroundHighlightTexture);
	assert(SStyleRankTextTexture);
	assert(SStyleRankTextHighlightTexture);
	assert(SSStyleRankFillTexture);
	assert(SSStyleRankBackgroundTexture);
	assert(SSStyleRankBackgroundHighlightTexture);
	assert(SSStyleRankTextTexture);
	assert(SSStyleRankTextHighlightTexture);
	assert(SSSStyleRankFillTexture);
	assert(SSSStyleRankBackgroundTexture);
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

	doppelgangerText = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\stylenames\\doppelganger.png").c_str(), pd3dDevice);
	doppelgangerTextBlur = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\stylenames\\doppelganger-blur.png").c_str(), pd3dDevice);
	gunslingerText = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\stylenames\\gunslinger.png").c_str(), pd3dDevice);
	gunslingerTextBlur = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\stylenames\\gunslinger-blur.png").c_str(), pd3dDevice);
	quicksilverText = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\stylenames\\quicksilver.png").c_str(), pd3dDevice);
	quicksilverTextBlur = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\stylenames\\quicksilver-blur.png").c_str(), pd3dDevice);
	royalguardText = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\stylenames\\royalguard.png").c_str(), pd3dDevice);
	royalguardTextBlur = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\stylenames\\royalguard-blur.png").c_str(), pd3dDevice);
	swordmasterText = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\stylenames\\swordmaster.png").c_str(), pd3dDevice);
	swordmasterTextBlur = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\stylenames\\swordmaster-blur.png").c_str(), pd3dDevice);
	tricksterText = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\stylenames\\trickster.png").c_str(), pd3dDevice);
	tricksterTextBlur = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\stylenames\\trickster-blur.png").c_str(), pd3dDevice);

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
	assert(styleExpBarBorder);
	assert(styleExpBarBorderVergil);
	assert(styleExpBarFill);
	assert(styleLevelDiamond);
	assert(royalGaugeCircle);
}

void InitMirageGaugeTextures(ID3D11Device* pd3dDevice) {
	mirageGaugeDpadTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\miragegauge\\dpad.png").c_str(), pd3dDevice);
	mirageGaugeDpadActiveTexture = new Texture2DD3D11(((std::string)Paths::assets + "\\" + "crimsonHud\\miragegauge\\dpad-active.png").c_str(), pd3dDevice);
	assert(mirageGaugeDpadTexture);
	assert(mirageGaugeDpadActiveTexture);
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
	float pointy = (1.0f - skewT) * 6.0f;

	ImVec2 fillPos = ImVec2(pos.x, pos.y + size.y - visibleHeight);
	float fillTop = fillPos.y;
	float fillBottom = pos.y + size.y;

	ImVec2 center = ImVec2(pos.x + size.x * 0.5f, pos.y + size.y * 0.5f);

	ImVec2 corners[4] = {
		ImVec2(pos.x - size.x * skewAmount, fillTop + visibleHeight * pointy),                  // Top-left
		ImVec2(pos.x + size.x + size.x * skewAmount, fillTop + visibleHeight * pointy),         // Top-right
		ImVec2(pos.x + size.x, fillBottom),                                                     // Bottom-right
		ImVec2(pos.x, fillBottom)                                                               // Bottom-left
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
	float pointy = (1.0f - skewT) * 6.0f;

	ImVec2 corners[4] = {
		ImVec2(pos.x - size.x * skewAmount, pos.y + size.y * pointy),                  // Top-left
		ImVec2(pos.x + size.x + size.x * skewAmount, pos.y + size.y * pointy),         // Top-right
		ImVec2(pos.x + size.x, pos.y + size.y),                                        // Bottom-right
		ImVec2(pos.x, pos.y + size.y)                                                  // Bottom-left
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

// Global previous style rank for entrance animation logic
static int prevGlobalStyleRank = -1;

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
	ImVec2 windowPos = ImVec2(g_renderSize.x - (480.0 * scaleFactorY), 190.0f * scaleFactorY),
	bool sSStyle = false,
	bool sSSStyle = false
) {
	// Use a static map to store animation state per rank
	static std::unordered_map<int, StyleMeterAnimState> animStates;
	StyleMeterAnimState& state = animStates[currentRank];

	const float animDuration = 0.32f;
	const float textDelay = 0.64f;
	const float textAnimDuration = 0.22f;
	const float flashDuration = 0.6f;

	float fillRatio = 0.0f;
	bool active = false;

	if (!fillTexture || !bgTexture || !textTexture || !textHighlightTexture)
		return;

	if (InGame() && !g_inGameCutscene) {
		auto pool_10222 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
		if (pool_10222 && pool_10222[3]) {
			auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_10222[3]);
			if (activeCrimsonConfig.CrimsonHudAddons.styleRanksMeter) {
				auto& styleData = mainActorData.styleData;
				if (styleData.rank == currentRank) {
					fillRatio = styleData.meter / 700.0f;
					active = fillRatio > 0.0f;
				}
			}
		}
	}

	float deltaTime = ImGui::GetIO().DeltaTime;
	float targetAlpha = active ? 1.0f : 0.0f;
	float fadeSpeed = 8.0f;
	state.fade.alpha = SmoothLerp(state.fade.alpha, targetAlpha, fadeSpeed, deltaTime);

	// Animation trigger: only when going up in rank
	if (!state.prevActive && active && currentRank > prevGlobalStyleRank) {
		state.animTimer = 0.0f;
		state.animating = true;
	}
	state.prevActive = active;

	// Update the global previous style rank if this rank is active
	if (active) {
		prevGlobalStyleRank = currentRank;
	}

	// --- Animation update ---
	if (state.animating) {
		state.animTimer += deltaTime;
		if (state.animTimer > animDuration + textDelay + textAnimDuration)
			state.animating = false;
	}

	if (state.fade.alpha <= 0.01f && !state.animating)
		return;

	ImVec2 meterSize = ImVec2(301.0f * scaleFactorY * 0.95f, 303.0f * scaleFactorY * 0.95f);
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

	float extraLeft = (100.0f * scaleFactorY);
	ImVec2 adjustedWindowPos = ImVec2(windowPos.x - extraLeft, windowPos.y);
	ImVec2 adjustedWindowSize = meterSize + ImVec2(300.0f + extraLeft, 300.0f);

	ImGui::SetNextWindowPos(adjustedWindowPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(adjustedWindowSize, ImGuiCond_Always);

	ImGui::Begin("StyleMeter", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoBackground);

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
		float centerDist = meterSize.x * 1.2f * animIntensity;

		if (t < phase1) {
			float t1 = easeInOut(t / phase1);
			animOffset.x = -centerDist * (1.0f - t1);
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

	ImVec2 renderPosSSStyle = ImVec2(renderPos.x - (50.0f * scaleFactorY), renderPos.y + (50.0f * scaleFactorY));
	ImVec2 renderPosSSStyleExtraLeft = ImVec2(renderPosSSStyle.x + extraLeft, renderPosSSStyle.y);
	ImVec2 actualRenderPosSSStyle = renderPosSSStyleExtraLeft;
	ImVec2 renderPosSSSStyle = ImVec2(renderPos.x - (100.0f * scaleFactorY), renderPos.y + (20.0f * scaleFactorY));
	ImVec2 renderPosSSSStyleExtraLeft = ImVec2(renderPosSSSStyle.x + extraLeft, renderPosSSSStyle.y);
	ImVec2 actualRenderPosSSSStyle = renderPosSSSStyleExtraLeft;

	if (sSSStyle) {
		RenderSkewedMeterWithFill(fillTexture, actualRenderPosSSSStyle, renderSize, fillRatio, fillColorC, animAngle, skewT);
		RenderSkewedTexture(bgTexture, actualRenderPosSSSStyle, renderSize, flashBg, animAngle, skewT);
		RenderSkewedTexture(bgHighlightTexture, actualRenderPosSSSStyle, renderSize, highlightColor, animAngle, skewT);
	}

	if (sSStyle || sSSStyle) {
		RenderSkewedMeterWithFill(fillTexture, actualRenderPosSSStyle, renderSize, fillRatio, fillColorC, animAngle, skewT);
		RenderSkewedTexture(bgTexture, actualRenderPosSSStyle, renderSize, flashBg, animAngle, skewT);
		RenderSkewedTexture(bgHighlightTexture, actualRenderPosSSStyle, renderSize, highlightColor, animAngle, skewT);
	}

	RenderSkewedMeterWithFill(fillTexture, actualRenderPos, renderSize, fillRatio, fillColorC, animAngle, skewT);
	RenderSkewedTexture(bgTexture, actualRenderPos, renderSize, flashBg, animAngle, skewT);
	RenderSkewedTexture(bgHighlightTexture, actualRenderPos, renderSize, highlightColor, animAngle, skewT);

	ImGui::End();

	// --- Text Animation: delayed, ease in-out, straight ---
	ImVec2 textSize = ImVec2(1000.0f * scaleFactorY * 0.55f, 243.0f * scaleFactorY * 0.55f);
	ImVec2 textWindowPos = ImVec2(windowPos.x + (194.0f * scaleFactorY), windowPos.y + (125.0f * scaleFactorY));
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

	ImGui::Begin("StyleMeterText", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoBackground);

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

void StyleMeterWindows() {

	if (activeCrimsonConfig.HudOptions.hideStyleMeter) {
		return;
	}

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
		0x02c4ffFF
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
		0x3ad897FF
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
		0xe0d143FF
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
		0xe29d2fFF
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
		0xff50e2FF,
		1.5f,
		ImVec2(g_renderSize.x - (480.0 * scaleFactorY), 190.0f * scaleFactorY),
		false,
		false
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
		ImVec2(g_renderSize.x - (480.0 * scaleFactorY), 190.0f * scaleFactorY),
		true,
		false
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
		ImVec2(g_renderSize.x - (480.0 * scaleFactorY), 190.0f * scaleFactorY),
		true,
		true
	);
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
	// This element is mandatory for non-vanilla modes
	if (activeCrimsonGameplay.GameMode.preset >= GAMEMODEPRESETS::STYLE_SWITCHER) {
		activeCrimsonConfig.CrimsonHudAddons.redOrbCounter = true;
		queuedCrimsonConfig.CrimsonHudAddons.redOrbCounter = true;
	}

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
	ImVec2 texturePos = ImVec2(windowPos.x + textPos.x - textureWidth - 14.916f * scaleFactorY, windowPos.y + (windowSize.y - textureHeight) / 2);

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
	ImU32 shadowColor = ImColor(0.49f, 0.0f, 0.0f, alpha); // #7f0000
	

	// Main text color
	//ImU32 mainColor = ImColor(0.83f, 0.85f, 0.858f, alpha); // #D5D9DB
	ImU32 mainColor = ImColor(1.0f, 1.0f, 1.0f, alpha); // #D5D9DB

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
	if (activeConfig.hideMainHUD) {
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

	auto CheatIndicatorWindow = [](ImVec2 windowSize, ImVec2 windowPos, std::vector<uint8> cheatIndices, const char* label) {
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
		ImVec2(g_renderSize.x - (367.0f * scaleFactorX) - (0.0f * scaleFactorY), 15.0f * scaleFactorY),
		{ CHEATS::TRAINING, CHEATS::DAMAGE, CHEATS::SPEED, CHEATS::MOBILITY },
		"CheatsHUDIndicatorWindow1"
	);

	// Bottom window: DEBUG, RMS
	CheatIndicatorWindow(
		ImVec2(367.0f * scaleFactorX, 100.0f * scaleFactorY),
		ImVec2(g_renderSize.x - (367.0f * scaleFactorX) - (0.0f * scaleFactorY), 117.0f * scaleFactorY),
		{ CHEATS::DEBUG, CHEATS::RMS },
		"CheatsHUDIndicatorWindow2"
	);
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

void LockOnWindows() {
	static float lockOnAngle[PLAYER_COUNT] = { 0.0f };
	static FadeState lockOnFade[PLAYER_COUNT];

	assert(LockOnTexture);
	if (!LockOnTexture->IsValid()) {
		return;
	}
	if (!(InGame() && !g_inGameCutscene)) {
		return;
	}
	auto pool_4449 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC8FBD0);
	if (!pool_4449 || !pool_4449[147]) return;
	auto& cameraData = *reinterpret_cast<CameraData*>(pool_4449[147]);


	ImVec2 displaySize = ImGui::GetIO().DisplaySize;

	// Spin speed in radians per second (adjust as needed)
	const float spinSpeed = 0.08f; // slow spin

	if (!activeCrimsonConfig.CrimsonHudAddons.lockOn || !activeConfig.Actor.enable) {
		CrimsonPatches::ToggleHideLockOn(activeConfig.hideLockOn);
		return;
	}

	if (activeConfig.hideLockOn) {
		return;
	}

	CrimsonPatches::ToggleHideLockOn(activeCrimsonConfig.CrimsonHudAddons.lockOn);

	// Loop through player data
	for (uint8 playerIndex = 0; playerIndex < activeConfig.Actor.playerCount; ++playerIndex) {
		auto& playerData = GetPlayerData(playerIndex);
		auto& characterData = GetCharacterData(playerIndex, playerData.characterIndex, ENTITY::MAIN);
		auto& newActorData = GetNewActorData(playerIndex, playerData.characterIndex, ENTITY::MAIN);

		if (!newActorData.baseAddr) {
			continue; // FIX: Use continue instead of return to avoid mismatched Begin/End
		}
		auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);

		// Update angle
		float deltaTime = ImGui::GetIO().DeltaTime;
		lockOnAngle[playerIndex] += spinSpeed * deltaTime;

		// Keep angle in [0, 2*PI]
		if (lockOnAngle[playerIndex] > IM_PI * 2.0f) lockOnAngle[playerIndex] -= IM_PI * 2.0f;

		auto lockedEnemyScreenPositionDD = debug_draw_world_to_screen((const float*)&actorData.lockOnData.targetPosition, 1.0f);
		auto& lockedEnemyScreenPosition = crimsonPlayer[playerIndex].lockedEnemyScreenPosition;
		lockedEnemyScreenPosition = lockedEnemyScreenPositionDD;
		glm::vec3 lockedEnemyPosition = { actorData.lockOnData.targetPosition.x, actorData.lockOnData.targetPosition.y, actorData.lockOnData.targetPosition.z };
		glm::vec3 cameraPosition = { cameraData.data[0].x, cameraData.data[0].y, cameraData.data[0].z };
		crimsonPlayer[playerIndex].cameraLockedEnemyDistance = glm::distance(lockedEnemyPosition, cameraPosition);
		int distanceLockedEnemy = (int)crimsonPlayer[playerIndex].cameraLockedEnemyDistance / 20;
		crimsonPlayer[playerIndex].cameraLockedEnemyDistanceClamped = glm::clamp(distanceLockedEnemy, 0, 255);

		// Adjusts size dynamically based on the distance between Camera and Playerfloat minDistance = 5.0f;
		float textureBaseSizeX = 600.0f * scaleFactorY;
		float textureBaseSizeY = 581.0f * scaleFactorY;

		float minDistance = 5.0f;
		float safeDistance = (std::max)((float)crimsonPlayer[playerIndex].cameraLockedEnemyDistanceClamped, minDistance);

		ImVec2 sizeDistance = {
			(textureBaseSizeX * (1.0f / (safeDistance / 40))),
			(textureBaseSizeY * (1.0f / (safeDistance / 40)))
		};

		float textureWidth = sizeDistance.x * 0.25f;
		float textureHeight = sizeDistance.y * 0.25f;

		ImVec2 windowSize = ImVec2(sizeDistance.x, sizeDistance.y);
		float edgeOffsetX = 350.0f * scaleFactorY;
		float edgeOffsetY = 350.0f * scaleFactorY;

		float offsetX = 0.38f * sizeDistance.x;
		float offsetY = 0.39f * sizeDistance.y;

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

		float fadeSpeed = 8.0f; // Higher = faster fade
		float targetAlpha = lockOnActive ? 1.0f : 0.0f;
		lockOnFade[playerIndex].alpha = SmoothLerp(lockOnFade[playerIndex].alpha, targetAlpha, fadeSpeed, ImGui::GetIO().DeltaTime);

		if (lockOnFade[playerIndex].alpha > 0.01f) {
			if (LockOnTexture->IsValid()) {
				DrawRotatedImagePie(
					LockOnTexture->GetTexture(),
					texturePos,
					ImVec2(textureWidth, textureHeight),
					lockOnAngle[playerIndex],
					colorWithAlpha,
					healthFraction
				);
				DrawRotatedImagePie(
					LockOnForegroundTexture->GetTexture(),
					texturePos,
					ImVec2(textureWidth, textureHeight),
					lockOnAngle[playerIndex],
					fgColorWithAlpha,
					healthFraction
				);
			} else {
				ImGui::GetWindowDrawList()->AddRectFilled(texturePos, ImVec2(texturePos.x + textureWidth, texturePos.y + textureHeight), ImColor(1.0f, 1.0f, 1.0f, alpha));
			}
		}

		ImGui::End();
	}
}

void StunDisplacementLockOnWindows() {
	static float lockOnAngle[PLAYER_COUNT] = { 0.0f };
	static FadeState lockOnFade[PLAYER_COUNT];

	assert(LockOnStunTexture);
	if (!LockOnStunTexture->IsValid()) {
		return;
	}
	if (!(InGame() && !g_inGameCutscene)) {
		return;
	}

	if (activeConfig.hideLockOn) {
		return;
	}

	ImVec2 displaySize = ImGui::GetIO().DisplaySize;

	// Spin speed in radians per second (adjust as needed)
	const float spinSpeed = -0.12f; // slow spin

	if (!activeCrimsonConfig.CrimsonHudAddons.lockOn || !activeConfig.Actor.enable) {
		return;
	}

	// Loop through player data
	for (uint8 playerIndex = 0; playerIndex < activeConfig.Actor.playerCount; ++playerIndex) {
		auto& playerData = GetPlayerData(playerIndex);
		auto& characterData = GetCharacterData(playerIndex, playerData.characterIndex, ENTITY::MAIN);
		auto& newActorData = GetNewActorData(playerIndex, playerData.characterIndex, ENTITY::MAIN);

		if (!newActorData.baseAddr) {
			continue; // FIX: Use continue instead of return to avoid mismatched Begin/End
		}
		auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);

		if (actorData.character != CHARACTER::DANTE && actorData.character != CHARACTER::VERGIL) continue;

		// Update angle
		float deltaTime = ImGui::GetIO().DeltaTime;
		lockOnAngle[playerIndex] += spinSpeed * deltaTime;

		// Keep angle in [0, 2*PI]
		if (lockOnAngle[playerIndex] > IM_PI * 2.0f) lockOnAngle[playerIndex] -= IM_PI * 2.0f;

		auto distanceClamped = crimsonPlayer[playerIndex].cameraLockedEnemyDistanceClamped;
		auto& lockedEnemyScreenPosition = crimsonPlayer[playerIndex].lockedEnemyScreenPosition;

		// Adjusts size dynamically based on the distance between Camera and Playerfloat minDistance = 5.0f;
		float textureBaseSizeX = 600.0f * scaleFactorY;
		float textureBaseSizeY = 581.0f * scaleFactorY;

		float minDistance = 5.0f;
		float safeDistance = (std::max)((float)crimsonPlayer[playerIndex].cameraLockedEnemyDistanceClamped, minDistance);

		// DISPLACEMENT LOCK-ON (OUTER DARKER CIRCLE)

		ImVec2 sizeDistance = {
			(textureBaseSizeX * (1.0f / (safeDistance / 30))),
			(textureBaseSizeY * (1.0f / (safeDistance / 30)))
		};

		float textureWidth = sizeDistance.x * 0.25f;
		float textureHeight = sizeDistance.y * 0.25f;

		ImVec2 windowSize = ImVec2(sizeDistance.x, sizeDistance.y);

		// --- Calculate the Regular Lock On's center exactly as in LockOnWindows ---
		float regularBaseSizeX = 600.0f * scaleFactorY;
		float regularBaseSizeY = 581.0f * scaleFactorY;
		float regularSafeDistance = (std::max)((float)crimsonPlayer[playerIndex].cameraLockedEnemyDistanceClamped, 5.0f);
		ImVec2 regularSizeDistance = {
			(regularBaseSizeX * (1.0f / (regularSafeDistance / 40))),
			(regularBaseSizeY * (1.0f / (regularSafeDistance / 40)))
		};
		float regularOffsetX = 0.38f * regularSizeDistance.x;
		float regularOffsetY = 0.39f * regularSizeDistance.y;
		ImVec2 regularTexturePos = ImVec2(
			lockedEnemyScreenPosition.x - (regularSizeDistance.x / 2.0f) + regularOffsetX,
			lockedEnemyScreenPosition.y - (regularSizeDistance.y / 2.0f) + regularOffsetY
		);
		float regularTextureWidth = regularSizeDistance.x * 0.25f;
		float regularTextureHeight = regularSizeDistance.y * 0.25f;
		ImVec2 regularCenter = ImVec2(
			regularTexturePos.x + (regularTextureWidth / 2.0f),
			regularTexturePos.y + (regularTextureHeight / 2.0f)
		);

		// --- Center the DisplacementLockOn on the Regular Lock On ---
		ImVec2 texturePos = ImVec2(
			regularCenter.x - (textureWidth / 2.0f),
			regularCenter.y - (textureHeight / 2.0f)
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

		float fadeSpeed = 8.0f; // Higher = faster fade
		float targetAlpha = lockOnActive ? 1.0f : 0.0f;
		lockOnFade[playerIndex].alpha = SmoothLerp(lockOnFade[playerIndex].alpha, targetAlpha, fadeSpeed, ImGui::GetIO().DeltaTime);

		if (lockOnFade[playerIndex].alpha > 0.01f) {
			if (LockOnTexture->IsValid()) {
				DrawRotatedImagePie(
					LockOnStunTexture->GetTexture(),
					texturePos,
					ImVec2(textureWidth, textureHeight),
					lockOnAngle[playerIndex],
					colorWithAlpha,
					displacementFraction
				);
				DrawRotatedImagePie(
					LockOnForegroundTexture->GetTexture(),
					texturePos,
					ImVec2(textureWidth, textureHeight),
					lockOnAngle[playerIndex],
					fgColorWithAlpha,
					displacementFraction
				);
			} else {
				ImGui::GetWindowDrawList()->AddRectFilled(texturePos, ImVec2(texturePos.x + textureWidth, texturePos.y + textureHeight), ImColor(1.0f, 1.0f, 1.0f, alpha));
			}

			if (actorData.lockOnData.targetBaseAddr60 != 0) {
				auto& enemyActorData = *reinterpret_cast<EnemyActorData*>(actorData.lockOnData.targetBaseAddr60 - 0x60); // -0x60 very important don't forget

				float textureBaseSizeXStun = 300.0f * scaleFactorY;
				float textureBaseSizeYStun = 290.50f * scaleFactorY;

				// STUN LOCK-ON (INNER LIGHTER CIRCLE)

				ImVec2 sizeDistanceStun = {
					(textureBaseSizeXStun * (1.0f / (safeDistance / 35))),
					(textureBaseSizeYStun * (1.0f / (safeDistance / 35)))
				};

				float textureWidthStun = sizeDistanceStun.x * 0.25f;
				float textureHeightStun = sizeDistanceStun.y * 0.25f;

				ImVec2 windowSizeStun = ImVec2(sizeDistanceStun.x, sizeDistanceStun.y);

				// --- Center the StunLockOn on the Regular Lock On ---
				ImVec2 texturePosStun = ImVec2(
					regularCenter.x - (textureWidthStun / 2.0f),
					regularCenter.y - (textureHeightStun / 2.0f)
				);

				ImVec2 windowPosStun = ImVec2(
					texturePosStun.x + (sizeDistanceStun.x / 2.0f) - (windowSize.x / 2.0f),
					texturePosStun.y + (sizeDistanceStun.y / 2.0f) - (windowSize.y / 2.0f)
				);
				ImGui::SetNextWindowSize(windowSizeStun);
				ImGui::SetNextWindowPos(windowPosStun);

				std::string windowNameStun = "LockOnStunWindow" + std::to_string(playerIndex);

				ImGui::Begin(windowNameStun.c_str(), nullptr, windowFlags);

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

				float stunFraction = 1.0f - (lockedOnEnemyStun / lockedOnEnemyMaxStun);

				if (LockOnTexture->IsValid()) {
					DrawRotatedImagePie(
						LockOnStunTexture->GetTexture(),
						texturePosStun,
						ImVec2(textureWidthStun, textureHeightStun),
						lockOnAngle[playerIndex],
						colorStunWithAlpha,
						stunFraction
					);
					DrawRotatedImagePie(
						LockOnForegroundTexture->GetTexture(),
						texturePosStun,
						ImVec2(textureWidthStun, textureHeightStun),
						lockOnAngle[playerIndex],
						fgColorWithAlpha,
						stunFraction
					);
				} else {
					ImGui::GetWindowDrawList()->AddRectFilled(texturePos, ImVec2(texturePos.x + textureWidth, texturePos.y + textureHeight), ImColor(1.0f, 1.0f, 1.0f, alpha));
				}

				// STUN / DISPLACEMENT NUMERIC HUD

				auto& enemyId = enemyActorData.enemy;
				bool isHell = (enemyId >= ENEMY::PRIDE_1 && enemyId <= ENEMY::HELL_VANGUARD);

				if (isHell && activeCrimsonConfig.CrimsonHudAddons.stunDisplacementNumericHud) {
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

				ImGui::End(); // End LockOnStunWindow
			}
		}

		ImGui::End(); // End LockOnDisplacementWindow
	}
}

void ShieldLockOnWindows() {
	static float lockOnAngle[PLAYER_COUNT] = { 0.0f };
	static FadeState lockOnFade[PLAYER_COUNT];

	assert(LockOnShieldTexture);
	if (!LockOnShieldTexture->IsValid()) {
		return;
	}
	if (!(InGame() && !g_inGameCutscene)) {
		return;
	}

	ImVec2 displaySize = ImGui::GetIO().DisplaySize;

	// Spin speed in radians per second (adjust as needed)
	const float spinSpeed = -0.12f; // slow spin

	if (activeConfig.hideMainHUD || !activeCrimsonConfig.CrimsonHudAddons.lockOn || !activeConfig.Actor.enable) {
		return;
	}

	// Loop through player data
	for (uint8 playerIndex = 0; playerIndex < activeConfig.Actor.playerCount; ++playerIndex) {
		auto& playerData = GetPlayerData(playerIndex);
		auto& characterData = GetCharacterData(playerIndex, playerData.characterIndex, ENTITY::MAIN);
		auto& newActorData = GetNewActorData(playerIndex, playerData.characterIndex, ENTITY::MAIN);

		if (!newActorData.baseAddr) {
			continue; // FIX: Use continue instead of return to avoid mismatched Begin/End
		}
		auto& actorData = *reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);

		if (actorData.character != CHARACTER::DANTE && actorData.character != CHARACTER::VERGIL) continue;

		// Update angle
		float deltaTime = ImGui::GetIO().DeltaTime;
		lockOnAngle[playerIndex] += spinSpeed * deltaTime;

		// Keep angle in [0, 2*PI]
		if (lockOnAngle[playerIndex] > IM_PI * 2.0f) lockOnAngle[playerIndex] -= IM_PI * 2.0f;

		auto distanceClamped = crimsonPlayer[playerIndex].cameraLockedEnemyDistanceClamped;
		auto& lockedEnemyScreenPosition = crimsonPlayer[playerIndex].lockedEnemyScreenPosition;

		// Adjusts size dynamically based on the distance between Camera and Playerfloat minDistance = 5.0f;
		float textureBaseSizeX = 600.0f * scaleFactorY;
		float textureBaseSizeY = 581.0f * scaleFactorY;

		float minDistance = 5.0f;
		float safeDistance = (std::max)((float)crimsonPlayer[playerIndex].cameraLockedEnemyDistanceClamped, minDistance);

		ImVec2 sizeDistance = {
			(textureBaseSizeX * (1.0f / (safeDistance / 30))),
			(textureBaseSizeY * (1.0f / (safeDistance / 30)))
		};

		float textureWidth = sizeDistance.x * 0.25f;
		float textureHeight = sizeDistance.y * 0.25f;

		ImVec2 windowSize = ImVec2(sizeDistance.x, sizeDistance.y);
		// --- Calculate the Regular Lock On's center exactly as in LockOnWindows ---
		float regularBaseSizeX = 600.0f * scaleFactorY;
		float regularBaseSizeY = 581.0f * scaleFactorY;
		float regularSafeDistance = (std::max)((float)crimsonPlayer[playerIndex].cameraLockedEnemyDistanceClamped, 5.0f);
		ImVec2 regularSizeDistance = {
			(regularBaseSizeX * (1.0f / (regularSafeDistance / 40))),
			(regularBaseSizeY * (1.0f / (regularSafeDistance / 40)))
		};
		float regularOffsetX = 0.38f * regularSizeDistance.x;
		float regularOffsetY = 0.39f * regularSizeDistance.y;
		ImVec2 regularTexturePos = ImVec2(
			lockedEnemyScreenPosition.x - (regularSizeDistance.x / 2.0f) + regularOffsetX,
			lockedEnemyScreenPosition.y - (regularSizeDistance.y / 2.0f) + regularOffsetY
		);
		float regularTextureWidth = regularSizeDistance.x * 0.25f;
		float regularTextureHeight = regularSizeDistance.y * 0.25f;
		ImVec2 regularCenter = ImVec2(
			regularTexturePos.x + (regularTextureWidth / 2.0f),
			regularTexturePos.y + (regularTextureHeight / 2.0f)
		);

		// --- Center the StunDisplacementLockOn on the Regular Lock On ---
		ImVec2 texturePos = ImVec2(
			regularCenter.x - (textureWidth / 2.0f),
			regularCenter.y - (textureHeight / 2.0f)
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

		float fadeSpeed = 8.0f; // Higher = faster fade
		float targetAlpha = lockOnActive ? 1.0f : 0.0f;
		lockOnFade[playerIndex].alpha = SmoothLerp(lockOnFade[playerIndex].alpha, targetAlpha, fadeSpeed, ImGui::GetIO().DeltaTime);

		if (lockOnFade[playerIndex].alpha > 0.01f) {
			if (LockOnTexture->IsValid()) {
				DrawRotatedImagePie(
					LockOnShieldTexture->GetTexture(),
					texturePos,
					ImVec2(textureWidth, textureHeight),
					lockOnAngle[playerIndex],
					colorWithAlpha,
					shieldFraction
				);
				DrawRotatedImagePie(
					LockOnShieldTexture->GetTexture(),
					texturePos,
					ImVec2(textureWidth, textureHeight),
					lockOnAngle[playerIndex],
					fgColorWithAlpha,
					shieldFraction
				);
			} else {
				ImGui::GetWindowDrawList()->AddRectFilled(texturePos, ImVec2(texturePos.x + textureWidth, texturePos.y + textureHeight), ImColor(1.0f, 1.0f, 1.0f, alpha));
			}
		}

		ImGui::End();
	}
}

ImColor GetStyleFlashColor(int style, uint8 character) {
	switch (style) {
	case STYLE::TRICKSTER:     if (character == CHARACTER::DANTE) return ImColor(UI::SwapColorEndianness(0xf4dc07FF)); 
								else return ImColor(UI::SwapColorEndianness(0x44bccdFF)); // Yellow / Light Blue
	case STYLE::SWORDMASTER:   return ImColor(UI::SwapColorEndianness(0xf30202FF)); // Red
	case STYLE::GUNSLINGER:    return ImColor(UI::SwapColorEndianness(0x0c59f6FF)); // Blue
	case STYLE::ROYALGUARD:    return ImColor(UI::SwapColorEndianness(0x00fe27FF)); // Green
	case STYLE::DOPPELGANGER:  return ImColor(UI::SwapColorEndianness(0xfe5102FF)); // Orange
	case STYLE::QUICKSILVER:   if (character == CHARACTER::DANTE) return ImColor(UI::SwapColorEndianness(0xd40af0FF));
						   else return ImColor(UI::SwapColorEndianness(0xb5b5b5FF));
		 // Purple
	default:                   return ImColor(UI::SwapColorEndianness(0xFFFFFFFF)); // White fallback
	}
}

void StyleDisplayWindow() {
	auto pool_10222 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_10222 || !pool_10222[3]) {
		return;
	}
	auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_10222[3]);
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

	// --- Shake Animation State ---
	static int prevStyle = -1;
	static int prevCharacter = -1;
	static double shakeStartTime = 0.0;
	static bool shaking = false;
	const float shakeDuration = 0.1f; // seconds

	// --- Flash Animation State ---
	static double flashStartTime = 0.0;
	static bool flashing = false;
	const float flashDuration = 0.6f;
	static ImColor lastFlashColor = ImColor(0xFFFFFFFF);

	// --- Scale Animation State ---
	static double scaleStartTime = 0.0;
	static bool scaling = false;
	const float scaleDuration = 0.1f;
	static float currentScale = 1.0f;

	// --- Compass Letter Flash Animation State ---
	static double compassFlashStartTime = 0.0;
	static bool compassFlashing = false;
	const float compassFlashDuration = 0.25f;
	static ImColor compassLastFlashColor = ImColor(0xDE1C4CFF);

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

	ImGui::SetNextWindowSize(styleGlassWindowSize);
	ImGui::SetNextWindowPos(styleGlassWindowPos);

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;

	// STYLE COMPASS
	ImGui::Begin("StyleCompassWindow", nullptr, windowFlags);

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
	ImGui::SetNextWindowPos(styleGlassWindowPos);

	// STYLE BADGE
	ImGui::Begin("StyleBadgeWindow", nullptr, windowFlags);

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

		ImGui::Begin("StyleGlassWindow", nullptr, windowFlags);

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

void StyleTextDisplayWindow() {
	auto pool_10222 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_10222 || !pool_10222[3]) {
		return;
	}
	auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_10222[3]);
	auto name_80 = *reinterpret_cast<byte8**>(appBaseAddr + 0xCF2680);
	if (!name_80) {
		return;
	}
	auto& hudData = *reinterpret_cast<HUDData*>(name_80);
	if (!(InGame() && !g_inGameCutscene)) {
		return;
	}
	if (activeConfig.hideMainHUD || !activeCrimsonConfig.CrimsonHudAddons.stylesDisplay || g_inCredits) {
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

	ImGui::SetNextWindowSize(styleTextWindowSize);
	ImGui::SetNextWindowPos(styleTextWindowPos);

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;

	// --- Blur Animation State ---
	static int prevStyle = -1;
	static int prevCharacter = -1;
	static double blurStartTime = 0.0;
	const float blurDuration = 0.13f;
	static bool showBlur = false;

	// --- Slide Animation State ---
	static double slideStartTime = 0.0;
	const float slideDuration = 0.2f;
	static bool sliding = false;

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

	ImGui::Begin("StyleTextWindow", nullptr, windowFlags);
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
					offsetX = ImLerp(-100.0f * scaleFactorY, 0.0f, ease);
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

void StyleEXPDisplayWindow() {
	auto pool_10222 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_10222 || !pool_10222[3]) {
		return;
	}
	auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_10222[3]);
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
	ImVec2 styleExpDispWindowPos = ImVec2(
		screenCenterX - (830.0f * scaleFactorY),
		180 * scaleFactorY);

	ImGui::SetNextWindowSize(styleExpDispWindowSize);
	ImGui::SetNextWindowPos(styleExpDispWindowPos);

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;

	// --- EXP Bar Shake Animation State ---
	static double expShakeStartTime = 0.0;
	static bool expShaking = false;
	const float expShakeDuration = 0.1f; // seconds

	// Detect style change for shake (you may want to trigger this externally)
	static int prevExpStyle = -1;
	int currentExpStyle = mainActorData.style;
	if (currentExpStyle != prevExpStyle) {
		expShakeStartTime = ImGui::GetTime();
		expShaking = true;
		prevExpStyle = currentExpStyle;
	}

	if (mainActorData.styleLevel == 2) return;

	ImGui::Begin("StyleExpDisplayWindow", nullptr, windowFlags);

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


void StyleLvlDispWindow() {
	auto pool_10222 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_10222 || !pool_10222[3]) {
		return;
	}
	auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_10222[3]);
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
	float styleLvlTextureSizeX = (76.0f * 0.17f) * scaleFactorY;
	float styleLvlTextureSizeY = (97.0f * 0.17f) * scaleFactorY;

	ImVec2 styleLvlDispWindowSize = ImVec2(
		styleLvlTextureSizeX + (50.0f * 3 * scaleFactorY),
		styleLvlTextureSizeY + (50.0f * 3 * scaleFactorY));

	// Center of the screen
	float screenCenterX = ImGui::GetIO().DisplaySize.x * 0.5f;
	float screenCenterY = ImGui::GetIO().DisplaySize.y * 0.5f;

	ImVec2 styleLvlDispWindowPos = character == CHARACTER::DANTE ? 
		ImVec2(
		screenCenterX - (896.0f * scaleFactorY),
		67.0f * scaleFactorY) : 
		ImVec2(
		screenCenterX - (858.0f * scaleFactorY),
		37.0f * scaleFactorY);

	ImGui::SetNextWindowSize(styleLvlDispWindowSize);
	ImGui::SetNextWindowPos(styleLvlDispWindowPos);

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;

	ImGui::Begin("StyleLvlDisplayWindow", nullptr, windowFlags);

	ImColor diamondStyleColor = GetStyleFlashColor(mainActorData.style, character);
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


	if (mainActorData.styleLevel == 0) {
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

	if (mainActorData.styleLevel == 1) {
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

	ImGui::End();
}

void RoyalGaugeDispWindow() {
	auto pool_10222 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_10222 || !pool_10222[3]) {
		return;
	}
	auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_10222[3]);
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

	ImVec2 royalGaugeWindowPos =
		ImVec2(
			screenCenterX - (845.0f * scaleFactorY),
			83.0f * scaleFactorY);

	ImGui::SetNextWindowSize(royalGaugeWindowSize);
	ImGui::SetNextWindowPos(royalGaugeWindowPos);

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs;

	ImGui::Begin("RoyalGaugeWindow", nullptr, windowFlags);

	float textureWidth = royalGaugeTextureSizeX;
	float textureHeight = royalGaugeTextureSizeY;
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 texturePos = ImVec2(
		windowPos.x,
		windowPos.y + (royalGaugeWindowSize.y - textureHeight) * 0.5f
	);

	float royalGaugeFraction = 1.0f; // Should be 0.0f to 1.0f
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


void MirageGaugeMainPlayer() {
	if (!(activeConfig.Actor.enable &&
		InGame() &&
		crimsonPlayer[0].character == CHARACTER::VERGIL &&
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
	auto pool_10222 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
	if (!pool_10222 || !pool_10222[3]) {
		return;
	}
	auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_10222[3]);

	auto miragePoints = crimsonPlayer[0].vergilDoppelganger.miragePoints / crimsonPlayer[0].vergilDoppelganger.maxMiragePoints;
	auto& mirageTriggerActive = mainActorData.doppelganger;

	float dpadTextureWidth = (200.0f * 0.17f) * scaleFactorY;
	float dpadTextureHeight = (200.0f * 0.17f) * scaleFactorY;

	// Adjust the size of the bar
	float barLength = 130.0f * (crimsonPlayer[0].vergilDoppelganger.maxMiragePoints / maxMiragePointsAmount) * scaleFactorY;
	vec2 size = { barLength, 10.0f * scaleFactorY };

	// Calculate positions
	float padding = 8.0f * scaleFactorY;
	float posX = 70.0f * scaleFactorY;
	float posY = 150.0f * scaleFactorY;

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
	static float fade = 0.0f;
	static bool prevActive = false;
	const float fadeSpeed = 6.0f * ImGui::GetIO().DeltaTime; // Adjust speed as needed

	if (mirageTriggerActive) {
		fade += fadeSpeed;
		if (fade > 1.0f) fade = 1.0f;
	} else {
		fade -= fadeSpeed;
		if (fade < 0.0f) fade = 0.0f;
	}
	prevActive = mirageTriggerActive;

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

	if (ImGui::Begin("MirageMainPlayer", &show, windowFlags)) {
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



void InitTextures(ID3D11Device* pd3dDevice) {
	InitRedOrbTexture(pd3dDevice);
	InitStyleRankTextures(pd3dDevice);
	InitLockOnTexture(pd3dDevice);
	InitStyleGlassTextures(pd3dDevice);
	InitMirageGaugeTextures(pd3dDevice);
}

}
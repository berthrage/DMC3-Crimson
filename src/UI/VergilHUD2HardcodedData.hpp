#pragma once

#include "VergilHUD2.hpp"

#include <vector>

// Vergil's sprites live in atlas2.png (texture array slice 1), everything else is reused from atlas1 through HUD::Dante::HC.
namespace HUD::Vergil::HC
{
namespace Sprites
{
struct SpriteInfo_t
{
	Graphics::Rect RegionRect;
};

enum class SpriteID_t {
	Compass = 0,
	Mirage_Frame,
	Mirage_Fill,
	Doppel_Orb_Top_Active,
	Doppel_Orb_Left_Active,
	Doppel_Orb_Right_Active,
	Doppel_Orb_Top_Inactive,
	Doppel_Orb_Left_Inactive,
	Doppel_Orb_Right_Inactive,
	Orb_DT,
	Glow_Orb_DT,
	Glow_Orb_DT_Small,
	Exp_DS,
	Exp_QS,

	SIZE
};

constexpr Graphics::Rect AtlasRect(float l, float t, float r, float b)
{
	return { l / 4096.0f, t / 4096.0f, r / 4096.0f, b / 4096.0f };
}

constexpr SpriteInfo_t SPRITES_INFO[] =
{
	{ AtlasRect(24.0f,   24.0f, 495.0f,  539.0f) }, // Compass
	{ AtlasRect(512.0f,  24.0f, 738.0f,  519.0f) }, // Mirage_Frame
	{ AtlasRect(755.0f,  24.0f, 888.0f,  404.0f) }, // Mirage_Fill
	{ AtlasRect(905.0f,  24.0f, 1002.0f, 110.0f) }, // Doppel_Orb_Top_Active
	{ AtlasRect(1019.0f, 24.0f, 1115.0f, 121.0f) }, // Doppel_Orb_Left_Active
	{ AtlasRect(1132.0f, 24.0f, 1228.0f, 120.0f) }, // Doppel_Orb_Right_Active
	{ AtlasRect(1245.0f, 24.0f, 1319.0f, 99.0f) },  // Doppel_Orb_Top_Inactive
	{ AtlasRect(1336.0f, 24.0f, 1410.0f, 98.0f) },  // Doppel_Orb_Left_Inactive
	{ AtlasRect(1427.0f, 24.0f, 1500.0f, 98.0f) },  // Doppel_Orb_Right_Inactive
	{ AtlasRect(1517.0f, 24.0f, 1613.0f, 120.0f) }, // Orb_DT
	{ AtlasRect(1630.0f, 24.0f, 1766.0f, 160.0f) }, // Glow_Orb_DT
	{ AtlasRect(1783.0f, 24.0f, 1897.0f, 138.0f) }, // Glow_Orb_DT_Small
	// EXP fills sit in atlas1's Exp_DS and Exp_RG slots so they share its EXP transforms and masks.
	{ AtlasRect(2270.0f, 3772.0f, 3376.0f, 3788.0f) }, // Exp_DS
	{ AtlasRect(2270.0f, 3805.0f, 3376.0f, 3821.0f) }, // Exp_QS
};

static_assert(sizeof(SPRITES_INFO) / sizeof(SpriteInfo_t) == (size_t)SpriteID_t::SIZE);

constexpr const auto& GetSpriteInfo(SpriteID_t id)
{
	return SPRITES_INFO[(size_t)id];
}

constexpr const char* GetTextureNameFromSpriteID(SpriteID_t id)
{
	switch (id)
	{
	case SpriteID_t::Compass:					return "Vergil_Compass";
	case SpriteID_t::Mirage_Frame:				return "Vergil_Mirage_Frame";
	case SpriteID_t::Mirage_Fill:				return "Vergil_Mirage_Fill";
	case SpriteID_t::Doppel_Orb_Top_Active:		return "Vergil_Doppel_Orb_Top_Active";
	case SpriteID_t::Doppel_Orb_Left_Active:	return "Vergil_Doppel_Orb_Left_Active";
	case SpriteID_t::Doppel_Orb_Right_Active:	return "Vergil_Doppel_Orb_Right_Active";
	case SpriteID_t::Doppel_Orb_Top_Inactive:	return "Vergil_Doppel_Orb_Top_Inactive";
	case SpriteID_t::Doppel_Orb_Left_Inactive:	return "Vergil_Doppel_Orb_Left_Inactive";
	case SpriteID_t::Doppel_Orb_Right_Inactive:	return "Vergil_Doppel_Orb_Right_Inactive";
	case SpriteID_t::Orb_DT:					return "Vergil_Orb_DT";
	case SpriteID_t::Glow_Orb_DT:				return "Vergil_Glow_Orb_DT";
	case SpriteID_t::Glow_Orb_DT_Small:			return "Vergil_Glow_Orb_DT_Small";
	case SpriteID_t::Exp_DS:					return "Vergil_Exp_DS";
	case SpriteID_t::Exp_QS:					return "Vergil_Exp_QS";
	default:									return "Unknown";
	}
}

constexpr SpriteID_t GetDoppelOrbSpriteID(DoppelDelay_t delay, bool active)
{
	switch (delay)
	{
	case DoppelDelay_t::Left:	return active ? SpriteID_t::Doppel_Orb_Left_Active : SpriteID_t::Doppel_Orb_Left_Inactive;
	case DoppelDelay_t::Right:	return active ? SpriteID_t::Doppel_Orb_Right_Active : SpriteID_t::Doppel_Orb_Right_Inactive;
	default:					return active ? SpriteID_t::Doppel_Orb_Top_Active : SpriteID_t::Doppel_Orb_Top_Inactive;
	}
}

// The first three orbs are the big ones.
constexpr SpriteID_t GetDTBarOrbGlowSpriteID(size_t orbIdx)
{
	return orbIdx < 3 ? SpriteID_t::Glow_Orb_DT : SpriteID_t::Glow_Orb_DT_Small;
}

// Glow height over the orb's height, from the 96px big and 76px small orbs of the layout export. Orb_DT fills its rect.
constexpr float GetDTBarOrbGlowScale(size_t orbIdx)
{
	return orbIdx < 3 ? 136.0f / 96.0f : 114.0f / 76.0f;
}

inline void LoadSpriteDescs(std::vector<Graphics::SpriteDesc>& spriteDescs, const std::string& path)
{
	spriteDescs.reserve(spriteDescs.size() + (size_t)SpriteID_t::SIZE);

	for (size_t i = 0; i < (size_t)SpriteID_t::SIZE; i++)
	{
		spriteDescs.emplace_back(Graphics::SpriteDesc(path, 1, SPRITES_INFO[i].RegionRect));
	}
}
}

namespace Transforms
{
struct SpriteTransform
{
	float			Scale;
	glm::f32vec2	Position;
};

// The compass ring center sits on Dante's, at the same height. The rest keeps the layout export's placement around it.
constexpr SpriteTransform COMPASS		= { 0.4810f, { -0.7869f, 0.0889f } };
constexpr SpriteTransform MIRAGE_FRAME	= { 0.4623f, { -0.6999f, 0.0721f } };
constexpr SpriteTransform MIRAGE_FILL	= { 0.3549f, { -0.6665f, 0.0917f } };

constexpr SpriteTransform DOPPEL_ORBS_ACTIVE[(size_t)DoppelDelay_t::SIZE] = {
	{ 0.0803f, { -0.7864f, 0.4840f } }, // Top
	{ 0.0906f, { -0.8723f, 0.1655f } }, // Left
	{ 0.0897f, { -0.7005f, 0.1664f } }, // Right
};

constexpr SpriteTransform DOPPEL_ORBS_INACTIVE[(size_t)DoppelDelay_t::SIZE] = {
	{ 0.0700f, { -0.7861f, 0.4943f } }, // Top
	{ 0.0691f, { -0.8723f, 0.1646f } }, // Left
	{ 0.0691f, { -0.7007f, 0.1664f } }, // Right
};

constexpr const SpriteTransform& GetDoppelOrb(DoppelDelay_t delay, bool active)
{
	return active ? DOPPEL_ORBS_ACTIVE[(size_t)delay] : DOPPEL_ORBS_INACTIVE[(size_t)delay];
}
}

namespace Masks
{
// Arc around the compass, filled bottom to top: EndDeg goes from StartDeg (empty) to here (full).
constexpr Graphics::BatchedSprites::MaskInfo MIRAGE_FILL =
{
	.Type = Graphics::BatchedSprites::MaskType_t::RadialMask,
	.Radial =
	{
		.Origin		  = { -0.3276f, 0.5913f },
		.Radius		  = 0.79f,
		.StartDeg	  = 300.0f,
		.EndDeg		  = 400.0f,
		.KeepPositive = true,
		.FlipMask	  = false
	}
};
}
}

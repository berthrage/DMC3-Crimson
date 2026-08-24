#include "DanteHUD.hpp"

#include <format>

#include "DanteHUDHardcodedData.hpp"
#include "imgui.h"

//#include "HUDDebug.hpp"

namespace HUD::Dante {

std::shared_ptr<Graphics::Texture2DArrayD3D11> DanteHUD::s_pAtlasTexture;

DanteHUD::DanteHUD(ID3D11Device* pD3D11Device, ID3D11DeviceContext* pD3D11DeviceContext, UINT width, UINT height)
	: m_pD3D11Device(pD3D11Device), m_pD3D11DeviceContext(pD3D11DeviceContext), m_Width(width), m_Height(height)
{
	constexpr auto texturePath = R"(atlas.png)";

	// Initialize the atlas texture for the sprites only once
	if (!s_pAtlasTexture) {
		s_pAtlasTexture = std::make_shared<Graphics::Texture2DArrayD3D11>(texturePath, m_pD3D11Device);
	}

	if (m_SpriteDescs.size() == 0) {
		HC::Sprites::LoadSpriteDescs(m_SpriteDescs, texturePath);
	}

	m_SpriteIndices.UpdateSprites(m_ThemeID, m_ActiveStyle, m_ActiveStyleLevel,
								  m_HPLevel, m_DTLevel, m_RoyalGuardGaugeLevel);

	m_pSpriteBatch = std::make_unique<Graphics::BatchedSprites>(
		m_pD3D11Device,
		m_Width,
		m_Height,
		m_SpriteDescs,
		s_pAtlasTexture,
		m_SpriteIndices.GetSpriteIds()
	);

	SetSpritePositions();
	SetSpriteMasks();
	UpdateDTBarOrbs(m_DTFill);

	using RatioFlags = Graphics::BatchedSprites::RatioFlags;

	m_pSpriteBatch->SetRatioFlags((int)RatioFlags::FitToVP);
}

DanteHUD::~DanteHUD()
{
}

void DanteHUD::OnUpdate(double ts)
{
	
}

bool DanteHUD::OnDraw()
{
	OnDrawDebugLayer();

	return m_pSpriteBatch->Draw(m_pD3D11DeviceContext);
}

ID3D11ShaderResourceView* DanteHUD::GetSRV()
{
	return m_pSpriteBatch->GetRTSRV();
}

const std::string& DanteHUD::GetLastRenderingErrorMsg()
{
	return m_pSpriteBatch->GetLastErrorMsg();
}

HRESULT DanteHUD::GetLastRenderingErrorCode()
{
	return m_pSpriteBatch->GetLastErrorCode();
}

void DanteHUD::SetActiveStyle(Style_t style)
{
	m_ActiveStyle = style;

	UpdateSprites();
}

void DanteHUD::SetHUDTheme(Theme_t theme)
{
	m_ThemeID = theme;

	UpdateSprites();
}

void DanteHUD::SetHPVitalityAmount(float vit)
{
	// Make sure the value is clamped
	m_HPFill = glm::clamp(vit, 0.0f, 1.0f);

	UpdateHPBarVitality(m_HPFill);
}

void DanteHUD::SetHPDamageAmount(float dmg)
{
	// Make sure the value is clamped
	m_DMGFill = glm::clamp(dmg, 0.0f, 1.0f);

	UpdateHPBarDamage(m_DMGFill);
}

void DanteHUD::SetRGGuageAmount(float rg)
{
	// Make sure the value is clamped
	m_RoyalguardGuageFill = glm::clamp(rg, 0.0f, 1.0f);

	UpdateRGGuage(m_RoyalguardGuageFill);
}

void DanteHUD::SetActiveStyleExpFillAmount(float exp)
{
	// Make sure the value is clamped
	m_ActiveStyleExpFill = glm::clamp(exp, 0.0f, 1.0f);

	UpdateStyleExpBar(m_ActiveStyleExpFill);
}

void DanteHUD::SetDTFill(float dt)
{
	// Make sure the value is clamped
	m_DTFill = glm::clamp(dt, 0.0f, 1.0f);

	UpdateDTBarOrbs(m_DTFill);
}

void DanteHUD::SetActiveStyleLevel(size_t level)
{
	if (level >= 1 && level <= 3)
	{
		m_ActiveStyleLevel = level;
	}
	else
	{
		m_ActiveStyleLevel = 1;
	}

	UpdateSprites();
}

void DanteHUD::SetHPLevel(size_t level)
{
	if (level >= 1 && level <= 20)
	{
		m_HPLevel = level;
	}
	else
	{
		m_HPLevel = 20;
	}

	UpdateSprites();
}

void DanteHUD::SetDTLevel(size_t level)
{
	if (level >= 0 && level <= 8)
	{
		m_DTLevel = level;
	}
	else
	{
		m_DTLevel = 0;
	}

	UpdateSprites();
}

void DanteHUD::SetRGGaugeLevel(size_t level)
{
	if (level >= 1 && level <= 3)
	{
		m_RoyalGuardGaugeLevel = level;
	}
	else
	{
		m_RoyalGuardGaugeLevel = 1;
	}

	UpdateSprites();
}

void DanteHUD::SpriteIndices::UpdateSprites(Theme_t theme, Style_t activeStyle, size_t activeStyleLevel,
	size_t hpBarLevel, size_t dtBarLevel, size_t rgGuageLevel)
{
	using namespace HC::Sprites;

	m_SpriteIds.clear();
	m_SpriteIds.reserve(128);

	// Order matters here, back to front

	// 1st: Center piece of the compass
	{
		m_CompassCenterPieceIdx = m_SpriteIds.size();
		m_SpriteIds.push_back((size_t)GetCompassStyleCenterPieceSpriteID(theme, activeStyle));
	}

	// 2nd: Compass frame
	{
		m_CompassFrameIdx = m_SpriteIds.size();
		m_SpriteIds.push_back((size_t)GetCompassFrameSpriteID(theme));
	}

	// 3rd: Inactive style initials
	{
		// Top: Trickster
		{
			if (activeStyle != Style_t::Trickster)
			{
				m_CompassNameSlotIdxs[(size_t)CompassSlot_t::Top] = m_SpriteIds.size();

				m_SpriteIds.push_back(
					(size_t)GetStyleNameInitialsSpriteID(
						theme,
						Style_t::Trickster,
						StyleNameInitialsVarient::Shadowed
					)
				);
			}
		}

		// Right: Swordmaster & Darkslayer
		{
			if (activeStyle != Style_t::Swordmaster && activeStyle != Style_t::Darkslayer)
			{
				m_CompassNameSlotIdxs[(size_t)CompassSlot_t::RightSecondary] = m_SpriteIds.size();

				m_SpriteIds.push_back(
					(size_t)GetStyleNameInitialsSpriteID(
						theme,
						Style_t::Darkslayer,
						StyleNameInitialsVarient::Shadowed
					)
				);

				m_CompassNameSlotIdxs[(size_t)CompassSlot_t::Right] = m_SpriteIds.size();

				m_SpriteIds.push_back(
					(size_t)GetStyleNameInitialsSpriteID(
						theme,
						Style_t::Swordmaster,
						StyleNameInitialsVarient::Shadowed
					)
				);
			}
			else if (activeStyle == Style_t::Swordmaster)
			{
				m_CompassNameSlotIdxs[(size_t)CompassSlot_t::RightSecondary] = m_SpriteIds.size();

				m_SpriteIds.push_back(
					(size_t)GetStyleNameInitialsSpriteID(
						theme,
						Style_t::Darkslayer,
						StyleNameInitialsVarient::Shadowed
					)
				);
			}
			else if (activeStyle == Style_t::Darkslayer)
			{
				m_CompassNameSlotIdxs[(size_t)CompassSlot_t::RightSecondary] = m_SpriteIds.size();

				m_SpriteIds.push_back(
					(size_t)GetStyleNameInitialsSpriteID(
						theme,
						Style_t::Swordmaster,
						StyleNameInitialsVarient::Shadowed
					)
				);
			}
		}

		// Bottom: Royalguard & Quicksilver
		{
			if (activeStyle != Style_t::Royalguard && activeStyle != Style_t::Quicksilver)
			{
				m_CompassNameSlotIdxs[(size_t)CompassSlot_t::BottomSecondary] = m_SpriteIds.size();

				m_SpriteIds.push_back(
					(size_t)GetStyleNameInitialsSpriteID(
						theme,
						Style_t::Quicksilver,
						StyleNameInitialsVarient::Shadowed
					)
				);

				m_CompassNameSlotIdxs[(size_t)CompassSlot_t::Bottom] = m_SpriteIds.size();

				m_SpriteIds.push_back(
					(size_t)GetStyleNameInitialsSpriteID(
						theme,
						Style_t::Royalguard,
						StyleNameInitialsVarient::Shadowed
					)
				);
			}
			else if (activeStyle == Style_t::Royalguard)
			{
				m_CompassNameSlotIdxs[(size_t)CompassSlot_t::BottomSecondary] = m_SpriteIds.size();

				m_SpriteIds.push_back(
					(size_t)GetStyleNameInitialsSpriteID(
						theme,
						Style_t::Quicksilver,
						StyleNameInitialsVarient::Shadowed
					)
				);
			}
			else if (activeStyle == Style_t::Quicksilver)
			{
				m_CompassNameSlotIdxs[(size_t)CompassSlot_t::BottomSecondary] = m_SpriteIds.size();

				m_SpriteIds.push_back(
					(size_t)GetStyleNameInitialsSpriteID(
						theme,
						Style_t::Royalguard,
						StyleNameInitialsVarient::Shadowed
					)
				);
			}
		}

		// Left: Gunslinger & Doppleganger
		{
			if (activeStyle != Style_t::Gunslinger && activeStyle != Style_t::Doppleganger)
			{
				m_CompassNameSlotIdxs[(size_t)CompassSlot_t::LeftSecondary] = m_SpriteIds.size();

				m_SpriteIds.push_back(
					(size_t)GetStyleNameInitialsSpriteID(
						theme,
						Style_t::Doppleganger,
						StyleNameInitialsVarient::Shadowed
					)
				);

				m_CompassNameSlotIdxs[(size_t)CompassSlot_t::Left] = m_SpriteIds.size();

				m_SpriteIds.push_back(
					(size_t)GetStyleNameInitialsSpriteID(
						theme,
						Style_t::Gunslinger,
						StyleNameInitialsVarient::Shadowed
					)
				);
			}
			else if (activeStyle == Style_t::Gunslinger)
			{
				m_CompassNameSlotIdxs[(size_t)CompassSlot_t::LeftSecondary] = m_SpriteIds.size();

				m_SpriteIds.push_back(
					(size_t)GetStyleNameInitialsSpriteID(
						theme,
						Style_t::Doppleganger,
						StyleNameInitialsVarient::Shadowed
					)
				);
			}
			else if (activeStyle == Style_t::Doppleganger)
			{
				m_CompassNameSlotIdxs[(size_t)CompassSlot_t::LeftSecondary] = m_SpriteIds.size();

				m_SpriteIds.push_back(
					(size_t)GetStyleNameInitialsSpriteID(
						theme,
						Style_t::Gunslinger,
						StyleNameInitialsVarient::Shadowed
					)
				);
			}
		}
	}

	// 4th: Active style initials background
	{
		m_CompassStyleShatteredBGIdx = m_SpriteIds.size();

		m_SpriteIds.push_back(
			(size_t)GetCompassStyleShatteredBGSpriteID(
				theme,
				activeStyle
			)
		);
	}

	// 5th: Active style initials
	{
		switch (activeStyle)
		{
		case Style_t::Trickster:
			m_CompassNameSlotIdxs[(size_t)CompassSlot_t::Top] = m_SpriteIds.size();
			break;

		case Style_t::Swordmaster:
		case Style_t::Darkslayer:
			m_CompassNameSlotIdxs[(size_t)CompassSlot_t::Right] = m_SpriteIds.size();
			break;

		case Style_t::Royalguard:
		case Style_t::Quicksilver:
			m_CompassNameSlotIdxs[(size_t)CompassSlot_t::Bottom] = m_SpriteIds.size();
			break;

		case Style_t::Gunslinger:
		case Style_t::Doppleganger:
			m_CompassNameSlotIdxs[(size_t)CompassSlot_t::Left] = m_SpriteIds.size();
			break;
		}

		m_SpriteIds.push_back(
				(size_t)GetStyleNameInitialsSpriteID(
				theme,
				activeStyle,
				StyleNameInitialsVarient::Colored
			)
		);
	}

	// 6th: Royal guard guage
	{
		m_RoyalguardGaugeIdx = m_SpriteIds.size();

		m_SpriteIds.push_back(
			(size_t)GetRoyalguardGuageSpriteID(
				theme,
				rgGuageLevel
			)
		);
	}

	// 7th: Royal guard guage frame
	{
		m_RoyalguardGaugeFrameIdx = m_SpriteIds.size();

		m_SpriteIds.push_back(
			(size_t)GetRoyalguardGuageFrameSpriteID(
				theme,
				rgGuageLevel
			)
		);
	}

	// 8th: Style exp bar level
	{
		m_StyleExpBarLevelIdx = m_SpriteIds.size();

		switch (activeStyleLevel)
		{
		default:
		case 1:
			m_SpriteIds.push_back(
				(size_t)GetStyleExpBarLevelNumberSpriteID(theme, 1)
			);
			break;

		case 2:
			m_SpriteIds.push_back(
				(size_t)GetStyleExpBarLevelNumberSpriteID(theme, 2)
			);
			break;

		case 3:
			m_SpriteIds.push_back(
				(size_t)GetStyleExpBarLevelNumberSpriteID(theme, 3)
			);
			break;
		}
	}

	// 9th: Style exp bar frame
	{
		m_StyleExpBarFrameIdx = m_SpriteIds.size();

		m_SpriteIds.push_back(
			(size_t)GetStyleExpBarFrameSpriteID(theme)
		);
	}

	// 10th: Style exp bar
	{
		m_StyleExpBarIdx = m_SpriteIds.size();

		m_SpriteIds.push_back(
			(size_t)GetStyleExpBarSpriteID(theme, activeStyle)
		);
	}

	// 11th: Style exp bar name
	{
		m_StyleExpBarNameIdx = m_SpriteIds.size();

		m_SpriteIds.push_back(
			(size_t)GetStyleExpBarNameSpriteID(theme, activeStyle, false)
		);
	}

	// 12th: DT bar orbs
	{
		if (dtBarLevel > 0)
		{
			m_DTBarOrbsIdxs.clear();
			m_DTBarOrbsIdxs.reserve(10);

			for (size_t i = 0; i < dtBarLevel + 2; i++)
			{
				m_DTBarOrbsIdxs.push_back(m_SpriteIds.size());

				m_SpriteIds.push_back(
					(size_t)GetDTBarOrbSpriteID(theme, false)
				);
			}
		}
	}

	// 13th: DT bar frames
	{
		if (dtBarLevel > 0)
		{
			m_DTBarFrameIdxs.clear();
			m_DTBarFrameIdxs.reserve(10);

			for (size_t i = 0; i < dtBarLevel; i++)
			{
				m_DTBarFrameIdxs.push_back(m_SpriteIds.size());

				m_SpriteIds.push_back(
					(size_t)GetDTBarFrameSpriteID(theme, i + 1)
				);
			}
		}
	}

	// 14th: HP bar bg
	{
		m_HPBarBGIdxs.clear();
		m_HPBarBGIdxs.reserve(2);

		m_HPBarBGIdxs.push_back(m_SpriteIds.size());

		m_SpriteIds.push_back(
			(size_t)GetHPBarBGSpriteID(theme)
		);
		
		// If higher than level 10, need one for the bottom bar as well
		if (hpBarLevel > 10)
		{
			m_HPBarBGIdxs.push_back(m_SpriteIds.size());

			m_SpriteIds.push_back(
				(size_t)GetHPBarBGSpriteID(theme)
			);
		}
	}

	// 15th: HP bar damage bar
	{
		m_HPBarDamageIdxs.clear();
		m_HPBarDamageIdxs.reserve(2);

		m_HPBarDamageIdxs.push_back(m_SpriteIds.size());

		m_SpriteIds.push_back(
			(size_t)GetHPBarDamageSpriteID(theme)
		);

		// If higher than level 10, need one for the bottom bar as well
		if (hpBarLevel > 10)
		{
			m_HPBarDamageIdxs.push_back(m_SpriteIds.size());

			m_SpriteIds.push_back(
				(size_t)GetHPBarDamageSpriteID(theme)
			);
		}
	}

	// 16th: HP bar vitality bar
	{
		m_HPBarVitalityIdxs.clear();
		m_HPBarVitalityIdxs.reserve(2);

		m_HPBarVitalityIdxs.push_back(m_SpriteIds.size());

		m_SpriteIds.push_back(
			(size_t)GetHPBarVitalitySpriteID(theme)
		);

		// If higher than level 10, need one for the bottom bar as well
		if (hpBarLevel > 10)
		{
			m_HPBarVitalityIdxs.push_back(m_SpriteIds.size());

			m_SpriteIds.push_back(
				(size_t)GetHPBarVitalitySpriteID(theme)
			);
		}
	}

	// 17th: HP bar vitality tail glow
	{
		m_VitalityEndGlowIdx = m_SpriteIds.size();

		m_SpriteIds.push_back(
			(size_t)GetHPBarVitalityTailGlowSpriteID(theme)
		);
	}

	// 18th: HP bar frames
	{
		m_HPBarFrameIdxs.clear();
		m_HPBarFrameIdxs.reserve(20);

		for (size_t i = 0; i < hpBarLevel; i++)
		{
			// The order of the else if chain in this hierarchy is of UTMOST importance, 
			// do not fuck with the said hierarchy please
			if (hpBarLevel == 1) // If level 1, only a signle frame sprite is used
			{
				m_HPBarFrameIdxs.push_back(m_SpriteIds.size());

				m_SpriteIds.push_back(
					(size_t)GetHPBarFrameSpriteID(theme, HPBarFrameVarient_t::Single)
				);
			}
			else if (i == 0) // If higher than level 1, the 1st sprite is a head sprite
			{
				m_HPBarFrameIdxs.push_back(m_SpriteIds.size());

				m_SpriteIds.push_back(
					(size_t)GetHPBarFrameSpriteID(theme, HPBarFrameVarient_t::Head)
				);
			}
			else if (i == 10 && hpBarLevel == 11) // If level 11, 11th sprite would be a single frame sprite
			{
				m_HPBarFrameIdxs.push_back(m_SpriteIds.size());

				m_SpriteIds.push_back(
					(size_t)GetHPBarFrameSpriteID(theme, HPBarFrameVarient_t::Single)
				);
			}
			else if (i == hpBarLevel - 1) // Last one would be the tail sprite
			{
				m_HPBarFrameIdxs.push_back(m_SpriteIds.size());

				m_SpriteIds.push_back(
					(size_t)GetHPBarFrameSpriteID(theme, HPBarFrameVarient_t::Tail)
				);
			}
			else if (i == 9) // If higher than level 10, 10th would be the tail of the top bar
			{
				m_HPBarFrameIdxs.push_back(m_SpriteIds.size());

				m_SpriteIds.push_back(
					(size_t)GetHPBarFrameSpriteID(theme, HPBarFrameVarient_t::Tail)
				);
			}
			else if (i == 10) // If more than level 11, 11th sprite is the head of the bottom bar
			{
				m_HPBarFrameIdxs.push_back(m_SpriteIds.size());

				m_SpriteIds.push_back(
					(size_t)GetHPBarFrameSpriteID(theme, HPBarFrameVarient_t::Head)
				);
			}
			else // Anything else would be the middle sprite
			{
				m_HPBarFrameIdxs.push_back(m_SpriteIds.size());

				m_SpriteIds.push_back(
					(size_t)GetHPBarFrameSpriteID(theme, HPBarFrameVarient_t::Mid)
				);
			}
		}
	}
}

void DanteHUD::SetSpritePositions()
{
	using namespace HC::Transforms;
	// 1st: Center piece of the compass
	{
		const auto idx		 = m_SpriteIndices.GetCompassCenterPieceIdx();
		const auto transform = GetCompassCenterPiece(m_ThemeID, m_ActiveStyle);

		m_pSpriteBatch->SetTransform(
			idx, 
			{ transform.Position.x, transform.Position.y, 0.0f },
			glm::f32vec3(0.0f),
			glm::f32vec3(transform.Scale)
		);
	}

	// 2nd: Compass frame
	{
		const auto idx		 = m_SpriteIndices.GetCompassFrameIdx();
		const auto transform = GetCompassFrame(m_ThemeID);

		m_pSpriteBatch->SetTransform(idx, { transform.Position.x, transform.Position.y, 0.0f }, glm::f32vec3(0.0f), glm::f32vec3(transform.Scale));
	}

	// 3rd: Style initials
	{
		// Top: Trickster
		{
			if (m_ActiveStyle != Style_t::Trickster)
			{
				const auto idx	     = m_SpriteIndices.GetCompassNameSlotIdx(CompassSlot_t::Top);
				const auto transform = GetCompassStyleInitials(m_ThemeID, CompassSlotID_t::Top, Style_t::Trickster, StyleInitialsVarient_t::Shadowed);

				m_pSpriteBatch->SetTransform(
					idx,
					{ transform.Position.x, transform.Position.y, 0.0f },
					glm::f32vec3(0.0f),
					glm::f32vec3(transform.Scale)
				);
			}
			else
			{
				const auto idx	     = m_SpriteIndices.GetCompassNameSlotIdx(CompassSlot_t::Top);
				const auto transform = GetCompassStyleInitials(m_ThemeID, CompassSlotID_t::Top, Style_t::Trickster, StyleInitialsVarient_t::Colored);

				m_pSpriteBatch->SetTransform(
					idx,
					{ transform.Position.x, transform.Position.y, 0.0f },
					glm::f32vec3(0.0f),
					glm::f32vec3(transform.Scale)
				);
			}
		}

		// Right: Swordmaster & Darkslayer
		{
			if (m_ActiveStyle != Style_t::Swordmaster && m_ActiveStyle != Style_t::Darkslayer)
			{
				// Main
				{
					const auto idx		 = m_SpriteIndices.GetCompassNameSlotIdx(CompassSlot_t::Right);
					const auto transform = GetCompassStyleInitials(m_ThemeID, CompassSlotID_t::Right, Style_t::Swordmaster, StyleInitialsVarient_t::Shadowed);

					m_pSpriteBatch->SetTransform(
						idx, 
						{ transform.Position.x, transform.Position.y, 0.0f },
						glm::f32vec3(0.0f),
						glm::f32vec3(transform.Scale)
					);
				}

				// Secondary
				{
					const auto idx		 = m_SpriteIndices.GetCompassNameSlotIdx(CompassSlot_t::RightSecondary);
					const auto transform = GetCompassStyleInitials(m_ThemeID, CompassSlotID_t::RightSecondary, Style_t::Darkslayer, StyleInitialsVarient_t::Shadowed);

					m_pSpriteBatch->SetTransform(
						idx,
						{ transform.Position.x, transform.Position.y, 0.0f },
						glm::f32vec3(0.0f),
						glm::f32vec3(transform.Scale)
					);
				}
			}
			else if (m_ActiveStyle == Style_t::Swordmaster)
			{
				// Main
				{
					const auto idx		 = m_SpriteIndices.GetCompassNameSlotIdx(CompassSlot_t::Right);
					const auto transform = GetCompassStyleInitials(m_ThemeID, CompassSlotID_t::Right, Style_t::Swordmaster, StyleInitialsVarient_t::Colored);

					m_pSpriteBatch->SetTransform(
						idx,
						{ transform.Position.x, transform.Position.y, 0.0f },
						glm::f32vec3(0.0f),
						glm::f32vec3(transform.Scale)
					);
				}

				// Secondary
				{
					const auto idx		 = m_SpriteIndices.GetCompassNameSlotIdx(CompassSlot_t::RightSecondary);
					const auto transform = GetCompassStyleInitials(m_ThemeID, CompassSlotID_t::RightSecondary, Style_t::Darkslayer, StyleInitialsVarient_t::Shadowed);

					m_pSpriteBatch->SetTransform(
						idx,
						{ transform.Position.x, transform.Position.y, 0.0f },
						glm::f32vec3(0.0f),
						glm::f32vec3(transform.Scale)
					);
				}
			}
			else if (m_ActiveStyle == Style_t::Darkslayer)
			{
				// Main
				{
					const auto idx		 = m_SpriteIndices.GetCompassNameSlotIdx(CompassSlot_t::Right);
					const auto transform = GetCompassStyleInitials(m_ThemeID, CompassSlotID_t::Right, Style_t::Darkslayer, StyleInitialsVarient_t::Colored);

					m_pSpriteBatch->SetTransform(
						idx,
						{ transform.Position.x, transform.Position.y, 0.0f },
						glm::f32vec3(0.0f),
						glm::f32vec3(transform.Scale)
					);
				}

				// Secondary
				{
					const auto idx		 = m_SpriteIndices.GetCompassNameSlotIdx(CompassSlot_t::RightSecondary);
					const auto transform = GetCompassStyleInitials(m_ThemeID, CompassSlotID_t::RightSecondary, Style_t::Swordmaster, StyleInitialsVarient_t::Shadowed);

					m_pSpriteBatch->SetTransform(
						idx,
						{ transform.Position.x, transform.Position.y, 0.0f },
						glm::f32vec3(0.0f),
						glm::f32vec3(transform.Scale)
					);
				}
			}
		}

		// Bottom: Royalguard & Quicksilver
		{
			if (m_ActiveStyle != Style_t::Royalguard && m_ActiveStyle != Style_t::Quicksilver)
			{
				// Main
				{
					const auto idx		 = m_SpriteIndices.GetCompassNameSlotIdx(CompassSlot_t::Bottom);
					const auto transform = GetCompassStyleInitials(m_ThemeID, CompassSlotID_t::Bottom, Style_t::Royalguard, StyleInitialsVarient_t::Shadowed);

					m_pSpriteBatch->SetTransform(
						idx,
						{ transform.Position.x, transform.Position.y, 0.0f },
						glm::f32vec3(0.0f),
						glm::f32vec3(transform.Scale)
					);
				}

				// Secondary
				{
					const auto idx		 = m_SpriteIndices.GetCompassNameSlotIdx(CompassSlot_t::BottomSecondary);
					const auto transform = GetCompassStyleInitials(m_ThemeID, CompassSlotID_t::BottomSecondary, Style_t::Quicksilver, StyleInitialsVarient_t::Shadowed);

					m_pSpriteBatch->SetTransform(
						idx,
						{ transform.Position.x, transform.Position.y, 0.0f },
						glm::f32vec3(0.0f),
						glm::f32vec3(transform.Scale)
					);
				}
			}
			else if (m_ActiveStyle == Style_t::Royalguard)
			{
				// Main
				{
					const auto idx		 = m_SpriteIndices.GetCompassNameSlotIdx(CompassSlot_t::Bottom);
					const auto transform = GetCompassStyleInitials(m_ThemeID, CompassSlotID_t::Bottom, Style_t::Royalguard, StyleInitialsVarient_t::Colored);

					m_pSpriteBatch->SetTransform(
						idx,
						{ transform.Position.x, transform.Position.y, 0.0f },
						glm::f32vec3(0.0f),
						glm::f32vec3(transform.Scale)
					);
				}

				// Secondary
				{
					const auto idx		 = m_SpriteIndices.GetCompassNameSlotIdx(CompassSlot_t::BottomSecondary);
					const auto transform = GetCompassStyleInitials(m_ThemeID, CompassSlotID_t::BottomSecondary, Style_t::Quicksilver, StyleInitialsVarient_t::Shadowed);

					m_pSpriteBatch->SetTransform(
						idx,
						{ transform.Position.x, transform.Position.y, 0.0f },
						glm::f32vec3(0.0f),
						glm::f32vec3(transform.Scale)
					);
				}
			}
			else if (m_ActiveStyle == Style_t::Quicksilver)
			{
				// Main
				{
					const auto idx		 = m_SpriteIndices.GetCompassNameSlotIdx(CompassSlot_t::Bottom);
					const auto transform = GetCompassStyleInitials(m_ThemeID, CompassSlotID_t::Bottom, Style_t::Quicksilver, StyleInitialsVarient_t::Colored);

					m_pSpriteBatch->SetTransform(
						idx,
						{ transform.Position.x, transform.Position.y, 0.0f },
						glm::f32vec3(0.0f),
						glm::f32vec3(transform.Scale)
					);
				}

				// Secondary
				{
					const auto idx = m_SpriteIndices.GetCompassNameSlotIdx(CompassSlot_t::BottomSecondary);
					const auto transform = GetCompassStyleInitials(m_ThemeID, CompassSlotID_t::BottomSecondary, Style_t::Royalguard, StyleInitialsVarient_t::Shadowed);

					m_pSpriteBatch->SetTransform(
						idx,
						{ transform.Position.x, transform.Position.y, 0.0f },
						glm::f32vec3(0.0f),
						glm::f32vec3(transform.Scale)
					);
				}
			}
		}

		// Left: Gunslinger & Doppleganger
		{
			if (m_ActiveStyle != Style_t::Gunslinger && m_ActiveStyle != Style_t::Doppleganger)
			{
				// Main
				{
					const auto idx		 = m_SpriteIndices.GetCompassNameSlotIdx(CompassSlot_t::Left);
					const auto transform = GetCompassStyleInitials(m_ThemeID, CompassSlotID_t::Left, Style_t::Gunslinger, StyleInitialsVarient_t::Shadowed);

					m_pSpriteBatch->SetTransform(
						idx,
						{ transform.Position.x, transform.Position.y, 0.0f },
						glm::f32vec3(0.0f),
						glm::f32vec3(transform.Scale)
					);
				}

				// Secondary
				{
					const auto idx		 = m_SpriteIndices.GetCompassNameSlotIdx(CompassSlot_t::LeftSecondary);
					const auto transform = GetCompassStyleInitials(m_ThemeID, CompassSlotID_t::LeftSecondary, Style_t::Doppleganger, StyleInitialsVarient_t::Shadowed);

					m_pSpriteBatch->SetTransform(
						idx,
						{ transform.Position.x, transform.Position.y, 0.0f },
						glm::f32vec3(0.0f),
						glm::f32vec3(transform.Scale)
					);
				}
			}
			else if (m_ActiveStyle == Style_t::Gunslinger)
			{
				// Main
				{
					const auto idx		 = m_SpriteIndices.GetCompassNameSlotIdx(CompassSlot_t::Left);
					const auto transform = GetCompassStyleInitials(m_ThemeID, CompassSlotID_t::Left, Style_t::Gunslinger, StyleInitialsVarient_t::Colored);

					m_pSpriteBatch->SetTransform(
						idx,
						{ transform.Position.x, transform.Position.y, 0.0f },
						glm::f32vec3(0.0f),
						glm::f32vec3(transform.Scale)
					);
				}

				// Secondary
				{
					const auto idx		 = m_SpriteIndices.GetCompassNameSlotIdx(CompassSlot_t::LeftSecondary);
					const auto transform = GetCompassStyleInitials(m_ThemeID, CompassSlotID_t::LeftSecondary, Style_t::Doppleganger, StyleInitialsVarient_t::Shadowed);

					m_pSpriteBatch->SetTransform(
						idx,
						{ transform.Position.x, transform.Position.y, 0.0f },
						glm::f32vec3(0.0f),
						glm::f32vec3(transform.Scale)
					);
				}
			}
			else if (m_ActiveStyle == Style_t::Doppleganger)
			{
				// Main
				{
					const auto idx		 = m_SpriteIndices.GetCompassNameSlotIdx(CompassSlot_t::Left);
					const auto transform = GetCompassStyleInitials(m_ThemeID, CompassSlotID_t::Left, Style_t::Doppleganger, StyleInitialsVarient_t::Colored);

					m_pSpriteBatch->SetTransform(
						idx,
						{ transform.Position.x, transform.Position.y, 0.0f },
						glm::f32vec3(0.0f),
						glm::f32vec3(transform.Scale)
					);
				}

				// Secondary
				{
					const auto idx		 = m_SpriteIndices.GetCompassNameSlotIdx(CompassSlot_t::LeftSecondary);
					const auto transform = GetCompassStyleInitials(m_ThemeID, CompassSlotID_t::LeftSecondary, Style_t::Gunslinger, StyleInitialsVarient_t::Shadowed);

					m_pSpriteBatch->SetTransform(
						idx,
						{ transform.Position.x, transform.Position.y, 0.0f },
						glm::f32vec3(0.0f),
						glm::f32vec3(transform.Scale)
					);
				}
			}
		}
	}

	// 4th: Active style initials background
	{
		const auto idx		 = m_SpriteIndices.GetCompassStyleShatteredBGIdx();
		const auto transform = GetCompassStyleShatteredBackground(m_ThemeID, m_ActiveStyle);

		m_pSpriteBatch->SetTransform(
			idx, 
			{ transform.Position.x, 
			transform.Position.y, 0.0f },
			glm::f32vec3(0.0f),
			glm::f32vec3(transform.Scale)
		);
	}

	// 5th: Royal guard guage
	{
		const auto idx	     = m_SpriteIndices.GetRoyalguardGaugeIdx();
		const auto transform = GetRGGuage(m_ThemeID, m_RoyalGuardGaugeLevel);

		m_pSpriteBatch->SetTransform(
			idx, 
			{ transform.Position.x, transform.Position.y, 0.0f },
			glm::f32vec3(0.0f),
			glm::f32vec3(transform.Scale)
		);
	}

	// 6th: Royal guard guage frame
	{
		const auto idx		 = m_SpriteIndices.GetRoyalguardGaugeFrameIdx();
		const auto transform = GetRGGuageFrame(m_ThemeID, m_RoyalGuardGaugeLevel);

		m_pSpriteBatch->SetTransform(
			idx, 
			{ transform.Position.x, transform.Position.y, 0.0f },
			glm::f32vec3(0.0f),
			glm::f32vec3(transform.Scale)
		);
	}
	
	// 7th: Style exp bar level
	{
		const auto idx		 = m_SpriteIndices.GetStyleExpBarLevelIdx();
		const auto transform = GetStyleExpBarLevelDigit(m_ThemeID, m_ActiveStyleLevel);

		m_pSpriteBatch->SetTransform(
			idx, 
			{ transform.Position.x, transform.Position.y, 0.0f },
			glm::f32vec3(0.0f),
			glm::f32vec3(transform.Scale)
		);
	}

	// 8th: Style exp bar frame
	{
		const auto idx		 = m_SpriteIndices.GetStyleExpBarFrameIdx();
		const auto transform = GetStyleExpBarFrame(m_ThemeID);

		m_pSpriteBatch->SetTransform(
			idx, 
			{ transform.Position.x, transform.Position.y, 0.0f },
			glm::f32vec3(0.0f),
			glm::f32vec3(transform.Scale)
		);
	}

	// 9th: Style exp bar
	{
		const auto idx		 = m_SpriteIndices.GetStyleExpBarIdx();
		const auto transform = GetStyleExpBar(m_ThemeID, m_ActiveStyle);

		m_pSpriteBatch->SetTransform(
			idx, 
			{ transform.Position.x, transform.Position.y, 0.0f },
			glm::f32vec3(0.0f),
			glm::f32vec3(transform.Scale)
		);
	}

	// 10th: Style exp bar name
	{
		const auto idx	     = m_SpriteIndices.GetStyleExpBarNameIdx();
		const auto transform = GetStyleExpBarName(m_ThemeID, m_ActiveStyle);

		m_pSpriteBatch->SetTransform(
			idx, 
			{ transform.Position.x, transform.Position.y, 0.0f },
			glm::f32vec3(0.0f),
			glm::f32vec3(transform.Scale)
		);
	}

	// 11th: DT bar orbs
	{
		const auto idxs = m_SpriteIndices.GetDTBarOrbIdxs();

		for (size_t i = 0; i < m_DTLevel + 2; i++)
		{
			const auto transform = GetDTBarOrb(m_ThemeID, m_HPLevel < 11 ? BarPositionID_t::Top : BarPositionID_t::Bottom, i + 1);

			m_pSpriteBatch->SetTransform(
				idxs[i],
				{transform.Position.x, transform.Position.y, 0.0f},
				glm::f32vec3(0.0f),
				glm::f32vec3(transform.Scale)
			);
		}
	}

	// 12th: DT bar frames
	{
		const auto idxs = m_SpriteIndices.GetDTBarFrameIdxs();

		for (size_t i = 0; i < m_DTLevel; i++)
		{
			const auto transform = GetDTBarFrame(m_ThemeID, m_HPLevel < 11 ? BarPositionID_t::Top : BarPositionID_t::Bottom, (DTBarFrameID_t)i);

			m_pSpriteBatch->SetTransform(
				idxs[i], 
				{ transform.Position.x, transform.Position.y, 0.0f },
				glm::f32vec3(0.0f),
				glm::f32vec3(transform.Scale)
			);
		}
	}

	// 13th: HP bar bg
	{
		const auto& idxs = m_SpriteIndices.GetHPBarBGIdxs();

		const auto topTransform = GetHPBar(m_ThemeID, BarPositionID_t::Top, HPBarVarient_t::Background);

		m_pSpriteBatch->SetTransform(idxs[0], { topTransform.Position.x, topTransform.Position.y, 0.0f }, glm::f32vec3(0.0f), glm::f32vec3(topTransform.Scale));

		if (idxs.size() == 2)
		{
			const auto bottomTransform = GetHPBar(m_ThemeID, BarPositionID_t::Bottom, HPBarVarient_t::Background);

			m_pSpriteBatch->SetTransform(
				idxs[1],
				{ bottomTransform.Position.x, bottomTransform.Position.y, 0.0f },
				glm::f32vec3(0.0f),
				glm::f32vec3(bottomTransform.Scale)
			);
		}
	}

	// 14th: HP bar damage bar
	{
		const auto& idxs = m_SpriteIndices.GetHPBarDamageIdxs();

		const auto topTransform = GetHPBar(m_ThemeID, BarPositionID_t::Top, HPBarVarient_t::Damage);

		m_pSpriteBatch->SetTransform(idxs[0], { topTransform.Position.x, topTransform.Position.y, 0.0f }, glm::f32vec3(0.0f), glm::f32vec3(topTransform.Scale));

		if (idxs.size() == 2)
		{
			const auto bottomTransform = GetHPBar(m_ThemeID, BarPositionID_t::Bottom, HPBarVarient_t::Damage);

			m_pSpriteBatch->SetTransform(
				idxs[1],
				{ bottomTransform.Position.x, bottomTransform.Position.y, 0.0f },
				glm::f32vec3(0.0f),
				glm::f32vec3(bottomTransform.Scale)
			);
		}
	}

	// 15th: HP bar vitality bar
	{
		const auto& idxs = m_SpriteIndices.GetHPBarVitalityIdxs();

		const auto topTransform = GetHPBar(m_ThemeID, BarPositionID_t::Top, HPBarVarient_t::Vitality);

		m_pSpriteBatch->SetTransform(idxs[0], { topTransform.Position.x, topTransform.Position.y, 0.0f }, glm::f32vec3(0.0f), glm::f32vec3(topTransform.Scale));

		if (idxs.size() == 2)
		{
			const auto bottomTransform = GetHPBar(m_ThemeID, BarPositionID_t::Bottom, HPBarVarient_t::Vitality);

			m_pSpriteBatch->SetTransform(
				idxs[1],
				{ bottomTransform.Position.x, bottomTransform.Position.y, 0.0f },
				glm::f32vec3(0.0f),
				glm::f32vec3(bottomTransform.Scale)
			);
		}
	}

	// 16th: HP bar vitality tail glow
	{
		const auto& idx = m_SpriteIndices.GetHPBarVitalityEndGlowIdx();

		if (m_HPLevel < 11)
		{
			const auto x	 = GetVitalityGlowX(BarPositionID_t::Top, m_HPLevel, 1.0f);
			const auto y	 = GetVitalityBarGlow(m_ThemeID, BarPositionID_t::Top, GlowPos_t::Head).Position.y;
			const auto scale = GetVitalityBarGlow(m_ThemeID, BarPositionID_t::Top, GlowPos_t::Head).Scale;

			m_pSpriteBatch->SetTransform(
				idx,
				{ x, y, 0.0f },
				glm::f32vec3(0.0f),
				glm::f32vec3(scale)
			);
		}
		else
		{
			const auto x	 = GetVitalityGlowX(BarPositionID_t::Bottom, m_HPLevel - 10, 1.0f);
			const auto y	 = GetVitalityBarGlow(m_ThemeID, BarPositionID_t::Bottom, GlowPos_t::Head).Position.y;
			const auto scale = GetVitalityBarGlow(m_ThemeID, BarPositionID_t::Bottom, GlowPos_t::Head).Scale;

			m_pSpriteBatch->SetTransform(
				idx,
				{ x, y, 0.0f },
				glm::f32vec3(0.0f),
				glm::f32vec3(scale)
			);
		}
	}

	// 17th: HP bar frames
	{
		const auto& idxs = m_SpriteIndices.GetHPBarFrameIdxs();

		for (size_t i = 0; i < m_HPLevel; i++)
		{
			if (m_HPLevel == 1) // If level 1, only a signle frame sprite is used
			{
				const auto& transform = GetHPBarFrame(m_ThemeID, i + 1, HPBarFrameVarient_t::Single);

				m_pSpriteBatch->SetTransform(
					idxs[i],
					{ transform.Position.x, transform.Position.y, 0.0f },
					glm::f32vec3(0.0f),
					glm::f32vec3(transform.Scale)
				);
			}
			else if (i == 0) // If higher than level 1, the 1st sprite is a head sprite
			{
				const auto& transform = GetHPBarFrame(m_ThemeID, i + 1, HPBarFrameVarient_t::Head);

				m_pSpriteBatch->SetTransform(
					idxs[i],
					{ transform.Position.x, transform.Position.y, 0.0f },
					glm::f32vec3(0.0f),
					glm::f32vec3(transform.Scale)
				);
			}
			else if (i == 10 && m_HPLevel == 11) // If level 11, 11th sprite would be a single frame sprite
			{
				const auto& transform = GetHPBarFrame(m_ThemeID, i + 1, HPBarFrameVarient_t::Single);

				m_pSpriteBatch->SetTransform(
					idxs[i],
					{ transform.Position.x, transform.Position.y, 0.0f },
					glm::f32vec3(0.0f),
					glm::f32vec3(transform.Scale)
				);
			}
			else if (i == m_HPLevel - 1) // Last one would be the tail sprite
			{
				const auto& transform = GetHPBarFrame(m_ThemeID, i + 1, HPBarFrameVarient_t::Tail);

				m_pSpriteBatch->SetTransform(
					idxs[i],
					{ transform.Position.x, transform.Position.y, 0.0f },
					glm::f32vec3(0.0f),
					glm::f32vec3(transform.Scale)
				);
			}
			else if (i == 9) // If higher than level 10, 10th would be the tail of the top bar
			{
				const auto& transform = GetHPBarFrame(m_ThemeID, i + 1, HPBarFrameVarient_t::Tail);

				m_pSpriteBatch->SetTransform(
					idxs[i],
					{ transform.Position.x, transform.Position.y, 0.0f },
					glm::f32vec3(0.0f),
					glm::f32vec3(transform.Scale)
				);
			}
			else if (i == 10) // If more than level 11, 11th sprite is the head of the bottom bar
			{
				const auto& transform = GetHPBarFrame(m_ThemeID, i + 1, HPBarFrameVarient_t::Head);

				m_pSpriteBatch->SetTransform(
					idxs[i],
					{ transform.Position.x, transform.Position.y, 0.0f },
					glm::f32vec3(0.0f),
					glm::f32vec3(transform.Scale)
				);
			}
			else // Anything else would be the middle sprite
			{
				const auto& transform = GetHPBarFrame(m_ThemeID, i + 1, HPBarFrameVarient_t::Middle);

				m_pSpriteBatch->SetTransform(
					idxs[i],
					{ transform.Position.x, transform.Position.y, 0.0f },
					glm::f32vec3(0.0f),
					glm::f32vec3(transform.Scale)
				);
			}
		}
	}
}

void DanteHUD::SetSpriteMasks()
{
	for (size_t i = 0; i < m_SpriteIndices.GetHPBarBGIdxs().size(); i++)
	{
		const auto idx = m_SpriteIndices.GetHPBarBGIdxs()[i];

		switch (i)
		{
		case 0:
			m_pSpriteBatch->SetSpriteMasks(idx, HC::Masks::GetHPBarBackgroundMasks(m_HPLevel >= 10 ? 1.0f : (m_HPLevel * 0.1f)));
			break;

		case 1:
			m_pSpriteBatch->SetSpriteMasks(idx, HC::Masks::GetHPBarBackgroundMasks((m_HPLevel - 10) * 0.1f));
			break;

		default:
			break;
		}
	}

	for (size_t i = 0; i < m_SpriteIndices.GetHPBarDamageIdxs().size(); i++)
	{
		const auto idx = m_SpriteIndices.GetHPBarDamageIdxs()[i];

		switch (i)
		{
		case 0:
			m_pSpriteBatch->SetSpriteMasks(idx, HC::Masks::GetHPBarDamageMasks(m_HPLevel >= 10 ? 1.0f : (m_HPLevel * 0.1f)));
			break;

		case 1:
			m_pSpriteBatch->SetSpriteMasks(idx, HC::Masks::GetHPBarDamageMasks((m_HPLevel - 10) * 0.1f));
			break;

		default:
			break;
		}
	}

	for (size_t i = 0; i < m_SpriteIndices.GetHPBarVitalityIdxs().size(); i++)
	{
		const auto idx = m_SpriteIndices.GetHPBarVitalityIdxs()[i];

		switch (i)
		{
		case 0:
			m_pSpriteBatch->SetSpriteMasks(idx, HC::Masks::GetHPBarVitalityMasks(m_HPLevel >= 10 ? 1.0f : (m_HPLevel * 0.1f)));
			break;

		case 1:
			m_pSpriteBatch->SetSpriteMasks(idx, HC::Masks::GetHPBarVitalityMasks((m_HPLevel - 10) * 0.1f));
			break;

		default:
			break;
		}
	}

	m_pSpriteBatch->SetSpriteMasks(m_SpriteIndices.GetHPBarVitalityEndGlowIdx(), HC::Masks::GetHPBarVitalityEndGlowMasks());
	m_pSpriteBatch->SetSpriteMasks(m_SpriteIndices.GetRoyalguardGaugeIdx(), HC::Masks::GetRGGuageBarMasks(m_RoyalGuardGaugeLevel));
	m_pSpriteBatch->SetSpriteMasks(m_SpriteIndices.GetStyleExpBarIdx(), HC::Masks::GetStyleExpBarMasks());

	UpdateHPBarVitality(m_HPFill);
	UpdateHPBarDamage(m_DMGFill);
	UpdateRGGuage(m_RoyalguardGuageFill);
	UpdateStyleExpBar(m_ActiveStyleExpFill);
}

void DanteHUD::UpdateHPBarVitality(float fraction)
{
	using namespace HC::Sprites;
	using namespace HC::Transforms;
	using namespace HC::Masks;

	// Vitality bar
	const     auto  vitalSpriteIdxs = m_SpriteIndices.GetHPBarVitalityIdxs();
	constexpr auto  headMask	    = GetMaskInfo(MaskInfoID_t::HPBarVitalitySlantedHead);
	constexpr auto  tailMask	    = GetMaskInfo(MaskInfoID_t::HPBarVitalitySlantedTail);
	constexpr float	fullSpan	    = tailMask.Slanted.Origin.x - headMask.Slanted.Origin.x;

	// End glow
	const auto glowMaskRangeX	  = HC::Processed::GetGlowMaskRangeX();
	const auto vitalityEndGlowIdx = m_SpriteIndices.GetHPBarVitalityEndGlowIdx();

	if (m_HPLevel > 10)
	{
		constexpr auto fullSpanTopBar		 = fullSpan;
		const     auto bottomBarLevel		 = m_HPLevel - 10;
		const     auto fullBottomBarFraction = bottomBarLevel * 0.1f;
		const	  auto fullSpanBottomBar	 = fullBottomBarFraction * fullSpan;
		const	  auto fullSpanWhole		 = fullSpanTopBar + fullSpanBottomBar;

		const auto expectedCurrentFill = fraction * fullSpanWhole;

		// Top bar
		{
			auto tailMaskInfoTopBar = m_pSpriteBatch->GetSpriteMaskInfo(vitalSpriteIdxs[0], 1);

			if (expectedCurrentFill < fullSpanTopBar)
			{
				// Vitality bar
				{
					tailMaskInfoTopBar.Slanted.Origin.x = headMask.Slanted.Origin.x + expectedCurrentFill;

					m_pSpriteBatch->UpdateSpriteMask(vitalSpriteIdxs[0], 1, tailMaskInfoTopBar);
				}

				// Glow
				{
					const     auto  newX		= GetVitalityGlowX(BarPositionID_t::Top, 10, expectedCurrentFill / fullSpanTopBar);
					constexpr auto  newY		= GetVitalityBarGlow(Theme_t::Colored, BarPositionID_t::Top, GlowPos_t::Head).Position.y;
					const	  auto  newTrans	= glm::f32vec3{ newX, newY, 0.0f };
					auto			newMaskInfo = m_pSpriteBatch->GetSpriteMaskInfo(m_SpriteIndices.GetHPBarVitalityEndGlowIdx(), 0);

					newMaskInfo.Slanted.Origin.x = glowMaskRangeX - expectedCurrentFill;

					m_pSpriteBatch->TranslateTo(vitalityEndGlowIdx, newTrans);
					m_pSpriteBatch->UpdateSpriteMask(vitalityEndGlowIdx, 0, newMaskInfo);
				}
			}
			else // Full top bar
			{
				// Vitality bar
				{
					tailMaskInfoTopBar.Slanted.Origin.x = headMask.Slanted.Origin.x + fullSpanTopBar;

					m_pSpriteBatch->UpdateSpriteMask(vitalSpriteIdxs[0], 1, tailMaskInfoTopBar);
				}

				// Glow
				{
					const     auto  newX		= GetVitalityGlowX(BarPositionID_t::Top, 10, 1.0f);
					constexpr auto  newY		= GetVitalityBarGlow(Theme_t::Colored, BarPositionID_t::Top, GlowPos_t::Head).Position.y;
					const     auto	newTrans	= glm::f32vec3{ newX, newY, 0.0f };
					auto			newMaskInfo = m_pSpriteBatch->GetSpriteMaskInfo(m_SpriteIndices.GetHPBarVitalityEndGlowIdx(), 0);

					newMaskInfo.Slanted.Origin.x = glowMaskRangeX - expectedCurrentFill;

					m_pSpriteBatch->TranslateTo(m_SpriteIndices.GetHPBarVitalityEndGlowIdx(), newTrans);
					m_pSpriteBatch->UpdateSpriteMask(vitalityEndGlowIdx, 0, newMaskInfo);
				}
			}
		}

		// Bottom bar
		{
			float expectedBottomFill = expectedCurrentFill - fullSpanTopBar;
			
			if (expectedBottomFill > 0)
			{
				// Vitality bar
				{
					auto tailMaskInfoBottomBar = m_pSpriteBatch->GetSpriteMaskInfo(vitalSpriteIdxs[1], 1);

					tailMaskInfoBottomBar.Slanted.Origin.x = headMask.Slanted.Origin.x + expectedBottomFill;

					m_pSpriteBatch->UpdateSpriteMask(vitalSpriteIdxs[1], 1, tailMaskInfoBottomBar);
				}

				// Glow
				{
					const     auto  newX		= GetVitalityGlowX(BarPositionID_t::Bottom, bottomBarLevel, expectedBottomFill / fullSpanBottomBar);
					constexpr auto  newY		= GetVitalityBarGlow(Theme_t::Colored, BarPositionID_t::Bottom, GlowPos_t::Head).Position.y;
					const     auto  newTrans	= glm::f32vec3{ newX, newY, 0.0f };
					auto			newMaskInfo = m_pSpriteBatch->GetSpriteMaskInfo(m_SpriteIndices.GetHPBarVitalityEndGlowIdx(), 0);

					newMaskInfo.Slanted.Origin.x = glowMaskRangeX - expectedBottomFill;

					m_pSpriteBatch->TranslateTo(m_SpriteIndices.GetHPBarVitalityEndGlowIdx(), newTrans);
					m_pSpriteBatch->UpdateSpriteMask(vitalityEndGlowIdx, 0, newMaskInfo);
				}
			}
			else // Empty bottom bar
			{
				auto tailMaskInfoBottomBar = m_pSpriteBatch->GetSpriteMaskInfo(vitalSpriteIdxs[1], 1);

				tailMaskInfoBottomBar.Slanted.Origin.x = headMask.Slanted.Origin.x;

				m_pSpriteBatch->UpdateSpriteMask(vitalSpriteIdxs[1], 1, tailMaskInfoBottomBar);
			}
		}

		// If the hp is full, don't need to show the glow at the end of the bar
		m_pSpriteBatch->SetOpacity(
			m_SpriteIndices.GetHPBarVitalityEndGlowIdx(),
			Graphics::EpsilonEqual(expectedCurrentFill, fullSpanWhole) ? 0.0f : 1.0f
		);
	}
	else
	{
		const float fullBarFraction = m_HPLevel * 0.1f;
		const auto  fullSpanBar		= fullBarFraction * fullSpan;

		const auto expectedCurrentFill = fraction * fullSpanBar;

		// Vitality bar
		{
			auto tailMaskInfo = m_pSpriteBatch->GetSpriteMaskInfo(vitalSpriteIdxs[0], 1);
			tailMaskInfo.Slanted.Origin.x = headMask.Slanted.Origin.x + expectedCurrentFill;

			m_pSpriteBatch->UpdateSpriteMask(vitalSpriteIdxs[0], 1, tailMaskInfo);
		}

		// Glow
		{
			const     auto  newX		= GetVitalityGlowX(BarPositionID_t::Top, m_HPLevel, expectedCurrentFill / fullSpanBar);
			constexpr auto  newY		= GetVitalityBarGlow(Theme_t::Colored, BarPositionID_t::Top, GlowPos_t::Head).Position.y;
			const	  auto  newTrans	= glm::f32vec3{ newX, newY, 0.0f };
			auto			newMaskInfo = m_pSpriteBatch->GetSpriteMaskInfo(m_SpriteIndices.GetHPBarVitalityEndGlowIdx(), 0);

			newMaskInfo.Slanted.Origin.x = glowMaskRangeX - expectedCurrentFill;

			m_pSpriteBatch->TranslateTo(m_SpriteIndices.GetHPBarVitalityEndGlowIdx(), newTrans);
			m_pSpriteBatch->UpdateSpriteMask(vitalityEndGlowIdx, 0, newMaskInfo);
		}

		// If the hp is full, don't need to show the glow at the end of the bar
		m_pSpriteBatch->SetOpacity(
			m_SpriteIndices.GetHPBarVitalityEndGlowIdx(),
			Graphics::EpsilonEqual(expectedCurrentFill, fullSpanBar) ? 0.0f : 1.0f
		);
	}
}

void DanteHUD::UpdateHPBarDamage(float fraction)
{
	using namespace HC::Sprites;
	using namespace HC::Transforms;
	using namespace HC::Masks;

	const     auto  dmgSpriteIdxs	= m_SpriteIndices.GetHPBarDamageIdxs();
	constexpr auto  headMask	    = GetMaskInfo(MaskInfoID_t::HPBarDamageSlantedHead);
	constexpr auto  tailMask	    = GetMaskInfo(MaskInfoID_t::HPBarDamageSlantedTail);
	constexpr float	fullSpan	    = tailMask.Slanted.Origin.x - headMask.Slanted.Origin.x;

	if (m_HPLevel > 10)
	{
		constexpr auto fullSpanTopBar		 = fullSpan;
		const     auto bottomBarLevel		 = m_HPLevel - 10;
		const     auto fullBottomBarFraction = bottomBarLevel * 0.1f;
		const	  auto fullSpanBottomBar	 = fullBottomBarFraction * fullSpan;
		const	  auto fullSpanWhole		 = fullSpanTopBar + fullSpanBottomBar;

		const auto expectedCurrentFill = fraction * fullSpanWhole;

		// Top bar
		{
			auto tailMaskInfoTopBar = m_pSpriteBatch->GetSpriteMaskInfo(dmgSpriteIdxs[0], 1);

			if (expectedCurrentFill < fullSpanTopBar)
			{
				tailMaskInfoTopBar.Slanted.Origin.x = headMask.Slanted.Origin.x + expectedCurrentFill;

				m_pSpriteBatch->UpdateSpriteMask(dmgSpriteIdxs[0], 1, tailMaskInfoTopBar);
			}
			else // Full top bar
			{
				tailMaskInfoTopBar.Slanted.Origin.x = headMask.Slanted.Origin.x + fullSpanTopBar;

				m_pSpriteBatch->UpdateSpriteMask(dmgSpriteIdxs[0], 1, tailMaskInfoTopBar);
			}
		}

		// Bottom bar
		{
			float expectedBottomFill = expectedCurrentFill - fullSpanTopBar;
			
			if (expectedBottomFill > 0)
			{
				auto tailMaskInfoBottomBar = m_pSpriteBatch->GetSpriteMaskInfo(dmgSpriteIdxs[1], 1);

				tailMaskInfoBottomBar.Slanted.Origin.x = headMask.Slanted.Origin.x + expectedBottomFill;

				m_pSpriteBatch->UpdateSpriteMask(dmgSpriteIdxs[1], 1, tailMaskInfoBottomBar);
			}
			else // Empty bottom bar
			{
				auto tailMaskInfoBottomBar = m_pSpriteBatch->GetSpriteMaskInfo(dmgSpriteIdxs[1], 1);

				tailMaskInfoBottomBar.Slanted.Origin.x = headMask.Slanted.Origin.x;

				m_pSpriteBatch->UpdateSpriteMask(dmgSpriteIdxs[1], 1, tailMaskInfoBottomBar);
			}
		}
	}
	else
	{
		const float fullBarFraction = m_HPLevel * 0.1f;
		const auto  fullSpanBar		= fullBarFraction * fullSpan;

		const auto expectedCurrentFill = fraction * fullSpanBar;

		auto tailMaskInfo = m_pSpriteBatch->GetSpriteMaskInfo(dmgSpriteIdxs[0], 1);
		tailMaskInfo.Slanted.Origin.x = headMask.Slanted.Origin.x + expectedCurrentFill;

		m_pSpriteBatch->UpdateSpriteMask(dmgSpriteIdxs[0], 1, tailMaskInfo);
	}
}

void DanteHUD::UpdateRGGuage(float fraction)
{
	using namespace HC::Masks;

	auto maskInfoKey = MaskInfoID_t::RGGuageBarLV1;

	switch (m_RoyalGuardGaugeLevel)
	{
	case 1:
		maskInfoKey = MaskInfoID_t::RGGuageBarLV1;
		break;

	case 2:
		maskInfoKey = MaskInfoID_t::RGGuageBarLV2;
		break;

	case 3:
		maskInfoKey = MaskInfoID_t::RGGuageBarLV3;
		break;

	default:
		return;
	}

	const auto rgGuageBarIdx = m_SpriteIndices.GetRoyalguardGaugeIdx();
	auto	   rgGuageMask	 = GetMaskInfo(maskInfoKey);

	rgGuageMask.Radial.EndDeg = glm::mix(rgGuageMask.Radial.StartDeg, rgGuageMask.Radial.EndDeg, fraction);
	m_pSpriteBatch->UpdateSpriteMask(m_SpriteIndices.GetRoyalguardGaugeIdx(), 0, rgGuageMask);
}

void DanteHUD::UpdateStyleExpBar(float fraction)
{
	using namespace HC::Masks;

	const auto styleExpBarIdx  = m_SpriteIndices.GetRoyalguardGaugeIdx();
	auto	   styleExpBarMask = GetMaskInfo(MaskInfoID_t::StyleExpBarSlantedTail);

	styleExpBarMask.Slanted.Origin.x = glm::mix(0.0f, styleExpBarMask.Slanted.Origin.x, fraction);

	m_pSpriteBatch->UpdateSpriteMask(m_SpriteIndices.GetStyleExpBarIdx(), 0, styleExpBarMask);
}

void DanteHUD::UpdateDTBarOrbs(float fraction)
{
	const auto orbTextureIdxs = m_SpriteIndices.GetDTBarOrbIdxs();
	const auto eachOrbShare   = 1.0f / (m_DTLevel + 2);

	const size_t numFullOrbs = fraction / eachOrbShare;

	// DT orbs should be invisible by default
	for (const auto& idx : orbTextureIdxs)
	{
		m_pSpriteBatch->SetOpacity(idx, 0.0f);
	}

	// Fill the orbs that are full
	for (size_t i = 0; i < numFullOrbs; i++)
	{
		m_pSpriteBatch->SetOpacity(orbTextureIdxs[i], 1.0f);
	}

	// Set the opacity of the partially full orb unless all orbs are full
	if (numFullOrbs != m_DTLevel + 2)
	{
		m_pSpriteBatch->SetOpacity(orbTextureIdxs[numFullOrbs], glm::mod(fraction, eachOrbShare) / eachOrbShare);
	}
}

void DanteHUD::UpdateSprites()
{
	m_SpriteIndices.UpdateSprites(m_ThemeID, m_ActiveStyle, m_ActiveStyleLevel,
		m_HPLevel, m_DTLevel, m_RoyalGuardGaugeLevel);

	m_pSpriteBatch->ChangeSprites(m_pD3D11Device, m_SpriteDescs, m_SpriteIndices.GetSpriteIds(), s_pAtlasTexture);

	SetSpritePositions();
	SetSpriteMasks();
	UpdateDTBarOrbs(m_DTFill);
}

void DanteHUD::OnDrawDebugLayer()
{
	using namespace HC::Sprites;

	ImGui::Begin("Debug Layer");

	{
		//static nlohmann::json config{};
		static int	 currentSelectedTexture = 0;
		static int	 currentSelectedMask	= 0;
		static int	 currentSelectedStyle	= 0;
		static int	 currentSelectedTheme	= 0;
		static float hpFill					= 1.0f;
		static float dmgFill			    = 1.0f;
		static float rgFill					= 1.0f;
		static float expFill				= 1.0f;
		static float dtFill					= 1.0f;
		static int	 styleLevel				= 1;
		static int	 hpLevel				= 1;
		static int	 dtLevel				= 1;
		static int	 rgGuageLevel			= 1;

		std::vector<std::string> textureIDNameHolders;
		std::vector<const char*> textureIDNames;
		const auto& spritesIds = m_SpriteIndices.GetSpriteIds();

		static glm::vec3 transform = m_pSpriteBatch->GetTranslation(currentSelectedTexture);
		static glm::vec3 scale = m_pSpriteBatch->GetScale(currentSelectedTexture);

		static bool once = true;
		if (once)
		{
			//Load(config);

			//SetFromConfig(config, m_pSpriteBatch, m_SpriteIndices);

			transform = m_pSpriteBatch->GetTranslation(currentSelectedTexture);
			scale = m_pSpriteBatch->GetScale(currentSelectedTexture);

			once = false;
		}

		static constexpr const char* styleNameItems[] =
		{
			"Trickster",
			"Swordmaster",
			"Royalguard",
			"Gungslinger",
			"Darkslayer",
			"Quicksilver",
			"Doppleganger",
		};

		static constexpr const char* themeItems[] =
		{
			"Color",
			"Crimson",
		};

		for (size_t i = 0; i < spritesIds.size(); i++)
		{
			textureIDNameHolders.push_back(std::format( "{} ({})",
					GetTextureNameFromSpriteID((SpriteID_t)spritesIds[i]),
					i
				)
			);
		}

		textureIDNames.reserve(textureIDNameHolders.size());
		for (size_t i = 0; i < textureIDNameHolders.size(); i++)
			textureIDNames.push_back(textureIDNameHolders[i].c_str());

		if (ImGui::ListBox("Theme", &currentSelectedTheme, themeItems, 2))
		{
			switch (currentSelectedTheme)
			{
			default:
			case 0:
				SetHUDTheme(Theme_t::Colored);
				break;

			case 1:
				SetHUDTheme(Theme_t::Crimson);
				break;
			}

			//nlohmann::json config;
			//Load(config);
			//SetFromConfig(config, m_pSpriteBatch, m_SpriteIndices);

			if (currentSelectedTexture >= m_pSpriteBatch->GetCustomIndices().size())
			{
				currentSelectedTexture = m_pSpriteBatch->GetCustomIndices().size() - 1;
			}

			transform = m_pSpriteBatch->GetTranslation(currentSelectedTexture);
			scale = m_pSpriteBatch->GetScale(currentSelectedTexture);

		}

		if (ImGui::ListBox("Active Style", &currentSelectedStyle, styleNameItems, 7))
		{
			SetActiveStyle((Style_t)currentSelectedStyle);

			//nlohmann::json config;
			//Load(config);
			//SetFromConfig(config, m_pSpriteBatch, m_SpriteIndices);

			if (currentSelectedTexture >= m_pSpriteBatch->GetCustomIndices().size())
			{
				currentSelectedTexture = m_pSpriteBatch->GetCustomIndices().size() - 1;
			}

			transform = m_pSpriteBatch->GetTranslation(currentSelectedTexture);
			scale = m_pSpriteBatch->GetScale(currentSelectedTexture);
		}

		if (ImGui::SliderInt("Style Level", &styleLevel, 1, 3))
		{
			SetActiveStyleLevel(styleLevel);

			//nlohmann::json config;
			//Load(config);
			//SetFromConfig(config, m_pSpriteBatch, m_SpriteIndices);

			if (currentSelectedTexture >= m_pSpriteBatch->GetCustomIndices().size())
			{
				currentSelectedTexture = m_pSpriteBatch->GetCustomIndices().size() - 1;
			}

			transform = m_pSpriteBatch->GetTranslation(currentSelectedTexture);
			scale = m_pSpriteBatch->GetScale(currentSelectedTexture);
		}

		if (ImGui::SliderInt("RG Guage Level", &rgGuageLevel, 1, 3))
		{
			SetRGGaugeLevel(rgGuageLevel);

			//nlohmann::json config;
			//Load(config);
			//SetFromConfig(config, m_pSpriteBatch, m_SpriteIndices);

			if (currentSelectedTexture >= m_pSpriteBatch->GetCustomIndices().size())
			{
				currentSelectedTexture = m_pSpriteBatch->GetCustomIndices().size() - 1;
			}

			transform = m_pSpriteBatch->GetTranslation(currentSelectedTexture);
			scale = m_pSpriteBatch->GetScale(currentSelectedTexture);
		}

		if (ImGui::SliderInt("HP Level", &hpLevel, 1, 20))
		{
			SetHPLevel(hpLevel);

			//nlohmann::json config;
			//Load(config);
			//SetFromConfig(config, m_pSpriteBatch, m_SpriteIndices);

			if (currentSelectedTexture >= m_pSpriteBatch->GetCustomIndices().size())
			{
				currentSelectedTexture = m_pSpriteBatch->GetCustomIndices().size() - 1;
			}

			transform = m_pSpriteBatch->GetTranslation(currentSelectedTexture);
			scale = m_pSpriteBatch->GetScale(currentSelectedTexture);
		}

		if (ImGui::SliderInt("DT Level", &dtLevel, 1, 8))
		{
			SetDTLevel(dtLevel);

			//nlohmann::json config;
			//Load(config);
			//SetFromConfig(config, m_pSpriteBatch, m_SpriteIndices);

			if (currentSelectedTexture >= m_pSpriteBatch->GetCustomIndices().size())
			{
				currentSelectedTexture = m_pSpriteBatch->GetCustomIndices().size() - 1;
			}

			transform = m_pSpriteBatch->GetTranslation(currentSelectedTexture);
			scale = m_pSpriteBatch->GetScale(currentSelectedTexture);
		}

		if (ImGui::SliderFloat("HP Fill", &hpFill, 0.0f, 1.0f))
		{
			SetHPVitalityAmount(hpFill);
		}

		if (ImGui::SliderFloat("DMG Fill", &dmgFill, 0.0f, 1.0f))
		{
			SetHPDamageAmount(dmgFill);
		}

		if (ImGui::SliderFloat("RG Fill", &rgFill, 0.0f, 1.0f))
		{
			SetRGGuageAmount(rgFill);
		}

		if (ImGui::SliderFloat("Style Exp Fill", &expFill, 0.0f, 1.0f))
		{
			SetActiveStyleExpFillAmount(expFill);
		}

		if (ImGui::SliderFloat("DT Fill", &dtFill, 0.0f, 1.0f))
		{
			SetDTFill(dtFill);
		}

		if (ImGui::ListBox("Texture List", &currentSelectedTexture, textureIDNames.data(), textureIDNames.size()))
		{
			currentSelectedMask = 0;
			transform = m_pSpriteBatch->GetTranslation(currentSelectedTexture);
			scale = m_pSpriteBatch->GetScale(currentSelectedTexture);
		}

		{
			std::vector<std::string> textureMaskNames;

			const auto masks = m_pSpriteBatch->GetSpriteMasksInfo(currentSelectedTexture);

			if (masks.size())
			{
				for (size_t maskIdx = 0; maskIdx < masks.size(); maskIdx++)
				{
					switch (masks[maskIdx].Type)
					{
					case Graphics::BatchedSprites::MaskType_t::SlantedMask:
						textureMaskNames.push_back(std::format("Slanted ({})", maskIdx));
						break;

					case Graphics::BatchedSprites::MaskType_t::RadialMask:
						textureMaskNames.push_back(std::format("Radial ({})", maskIdx));
						break;

					default:
						textureMaskNames.push_back(std::format("Unknown ({})", maskIdx));
						break;
					}
				}

				std::vector<const char*> textureMaskNamesCCP;

				for (const auto& name : textureMaskNames)
				{
					textureMaskNamesCCP.push_back(name.c_str());
				}

				ImGui::ListBox("Texture Masks", &currentSelectedMask, textureMaskNamesCCP.data(), textureMaskNamesCCP.size());
				Graphics::BatchedSprites::MaskInfo maskInfo = masks[currentSelectedMask];

				switch (masks[currentSelectedMask].Type)
				{
				case Graphics::BatchedSprites::MaskType_t::SlantedMask:
				{
					bool update = false;
					ImGui::Text("Slanted mask settings:");
					if (ImGui::DragFloat2("Origin", (float*)&maskInfo.Slanted.Origin, 0.005f)) update = true;
					if (ImGui::DragFloat("Line normal (degrees)", &maskInfo.Slanted.LineNormalDeg, 0.5f, 0.0f, 360.0f)) update = true;
					if (ImGui::DragFloat2("Softness range", (float*)&maskInfo.Slanted.SoftnessRange, 0.05f)) update = true;;

					if (update)
					{
						m_pSpriteBatch->UpdateSpriteMask(currentSelectedTexture, currentSelectedMask, maskInfo);
					}
				} break;

				case Graphics::BatchedSprites::MaskType_t::RadialMask:
				{
					bool update = false;
					ImGui::Text("Radial mask settings:");
					if (ImGui::DragFloat2("Origin", (float*)&maskInfo.Radial.Origin, 0.005f)) update = true;
					if (ImGui::DragFloat("Radius", (float*)&maskInfo.Radial.Radius, 0.005f)) update = true;
					if (ImGui::DragFloat("Start (degrees)", (float*)&maskInfo.Radial.StartDeg, 0.05f)) update = true;
					if (ImGui::DragFloat("End (degrees)", (float*)&maskInfo.Radial.EndDeg, 0.05f)) update = true;
					if (ImGui::Checkbox("Keep positive side of the pie", &maskInfo.Radial.KeepPositive)) update = true;
					if (ImGui::Checkbox("Flip mask", &maskInfo.Radial.FlipMask)) update = true;

					if (update)
					{
						m_pSpriteBatch->UpdateSpriteMask(currentSelectedTexture, currentSelectedMask, maskInfo);
					}
				} break;

				default:
					break;
				}
			}

			if (ImGui::Button("Add mask (slanted)"))
			{
				Graphics::BatchedSprites::MaskInfo maskInfo =
				{
					.Type = Graphics::BatchedSprites::MaskType_t::SlantedMask,
					.Slanted =
					{
						.Origin = { 0.0f, 0.0f },
						.LineNormalDeg = 0,
						.SoftnessRange = { 0.0f, 0.0f }
					}
				};

				m_pSpriteBatch->AddMask(currentSelectedTexture, maskInfo);
			}

			ImGui::SameLine();

			if (ImGui::Button("Add mask (radial)"))
			{
				Graphics::BatchedSprites::MaskInfo maskInfo =
				{
					.Type = Graphics::BatchedSprites::MaskType_t::RadialMask,
					.Radial =
					{
						.Origin = { 0.0f, 0.0f },
						.Radius = 0.5f,
						.StartDeg = 45.0f,
						.EndDeg = 135.0f,
						.KeepPositive = true,
						.FlipMask = false
					}
				};

				m_pSpriteBatch->AddMask(currentSelectedTexture, maskInfo);
			}

			ImGui::SameLine();

			if (ImGui::Button("Remove mask"))
			{
				m_pSpriteBatch->RemoveSpriteMask(currentSelectedTexture, currentSelectedMask);
			}

			ImGui::SameLine();

			if (ImGui::Button("Clear masks"))
			{
				m_pSpriteBatch->RemoveSpriteMask(currentSelectedTexture, {});
			}
		}

		if (ImGui::DragFloat2("Position XY", (float*)&transform, 0.001f, -1.5f, 1.5f))
		{
			m_pSpriteBatch->TranslateTo(currentSelectedTexture, transform);
		}

		if (ImGui::DragFloat("Scale XY", (float*)&scale, 0.001f, 0.0f, 10.0f))
		{
			m_pSpriteBatch->ScaleTo(currentSelectedTexture, glm::vec3(scale.x, scale.x, scale.z));
		}

		//if (ImGui::Button("Save"))
		//{
		//	Save(config, m_pSpriteBatch, m_SpriteIndices);
		//}
		//
		//ImGui::SameLine();
		//
		//if (ImGui::Button("Load"))
		//{
		//	Load(config);
		//
		//	SetFromConfig(config, m_pSpriteBatch, m_SpriteIndices);
		//
		//	transform = m_pSpriteBatch->GetTranslation(currentSelectedTexture);
		//	scale = m_pSpriteBatch->GetScale(currentSelectedTexture);
		//}
	}

	ImGui::End();
}

}
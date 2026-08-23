#pragma once

#include "BatchedSprites.hpp"
#include "GraphicsBase.hpp"
#include "Sprite.hpp"

#include <memory>

namespace HUD::Dante {
	enum class Theme_t {
		Colored = 0,
		Crimson,

		SIZE
	};

	enum class Style_t {
		Trickster = 0,
		Swordmaster,
		Royalguard,
		Gunslinger,
		Darkslayer,
		Quicksilver,
		Doppleganger,

		SIZE
	};

	enum class CompassSlot_t {
		Top = 0,
		Right,
		RightSecondary,
		Bottom,
		BottomSecondary,
		Left,
		LeftSecondary,

		SIZE
	};

class DanteHUD {
public:
	DanteHUD(ID3D11Device* pD3D11Device, ID3D11DeviceContext* pD3D11DeviceContext, UINT width, UINT height);
	virtual ~DanteHUD();

public:
	const auto& GetWidth() { return m_Width; }
	const auto& GetHeight() { return m_Height; }

	void OnUpdate(double ts);
	bool OnDraw();

	ID3D11ShaderResourceView* GetSRV();

	const std::string& GetLastRenderingErrorMsg();
	HRESULT GetLastRenderingErrorCode();

public: // Controls
	void SetActiveStyle(Style_t style);
	void SetActiveStyleLevel(size_t level);
	void SetHPLevel(size_t level);
	void SetDTLevel(size_t level);
	void SetRGGaugeLevel(size_t level);
	void SetHUDTheme(Theme_t theme);

	// Todo: Give these animations
	void SetHPVitalityAmount(float vit); // 0.0f to 1.0f
	void SetHPDamageAmount(float dmg); // 0.0f to 1.0f
	void SetRGGuageAmount(float rg); // 0.0f to 1.0f
	void SetActiveStyleExpFillAmount(float exp); // 0.0f to 1.0f
	void SetDTFill(float dt); // 0.0f to 1.0f
	void SetDTVisible(bool visible); // Shows or hides the DT bar frames (Devil Trigger locked)

public: // Types Todo: Private
	// This object manages the IDs for the sprites used in the hud
	class SpriteIndices {
	public:
		SpriteIndices() = default;
		~SpriteIndices() = default;

		void UpdateSprites(Theme_t theme, Style_t activeStyle, size_t activeStyleLevel,
						   size_t hpBarLevel, size_t dtBarLevel, size_t rgGuageLevel);

		inline const auto& GetSpriteIds() const { return m_SpriteIds; }

		inline const auto& GetCompassCenterPieceIdx() const { return m_CompassCenterPieceIdx; }
		inline const auto& GetCompassFrameIdx() const { return m_CompassFrameIdx; }
		inline const auto& GetCompassNameSlotIdx(CompassSlot_t slot) const { return m_CompassNameSlotIdxs[(size_t)slot]; }
		inline const auto& GetCompassStyleShatteredBGIdx() const { return m_CompassStyleShatteredBGIdx; }
		inline const auto& GetRoyalguardGaugeIdx() const { return m_RoyalguardGaugeIdx; }
		inline const auto& GetRoyalguardGaugeFrameIdx() const { return m_RoyalguardGaugeFrameIdx; }
		inline const auto& GetStyleExpBarFrameIdx() const { return m_StyleExpBarFrameIdx; }
		inline const auto& GetStyleExpBarIdx() const { return m_StyleExpBarIdx; }
		inline const auto& GetStyleExpBarLevelIdx() const { return m_StyleExpBarLevelIdx; }
		inline const auto& GetStyleExpBarNameIdx() const { return m_StyleExpBarNameIdx; }
		inline const auto& GetDTBarOrbIdxs() const { return m_DTBarOrbsIdxs; }
		inline const auto& GetDTBarFrameIdxs() const { return m_DTBarFrameIdxs; }
		inline const auto& GetHPBarBGIdxs() const { return m_HPBarBGIdxs; }
		inline const auto& GetHPBarDamageIdxs() const { return m_HPBarDamageIdxs; }
		inline const auto& GetHPBarVitalityIdxs() const { return m_HPBarVitalityIdxs; }
		inline const auto& GetHPBarVitalityEndGlowIdx() const { return m_VitalityEndGlowIdx; }
		inline const auto& GetHPBarFrameIdxs() const { return m_HPBarFrameIdxs; }

	private:
		std::vector<size_t> m_SpriteIds;

		size_t m_CompassCenterPieceIdx{};
		size_t m_CompassFrameIdx{};
		std::array<size_t, (size_t)CompassSlot_t::SIZE> m_CompassNameSlotIdxs{};
		size_t m_CompassStyleShatteredBGIdx{};
		size_t m_RoyalguardGaugeIdx;
		size_t m_RoyalguardGaugeFrameIdx;
		size_t m_StyleExpBarFrameIdx{};
		size_t m_StyleExpBarIdx{};
		size_t m_StyleExpBarLevelIdx{};
		size_t m_StyleExpBarNameIdx{};
		std::vector<size_t> m_DTBarOrbsIdxs{};
		std::vector<size_t> m_DTBarFrameIdxs{};
		std::vector<size_t> m_HPBarBGIdxs{};
		std::vector<size_t> m_HPBarDamageIdxs{};
		std::vector<size_t> m_HPBarVitalityIdxs{};
		size_t m_VitalityEndGlowIdx{};
		std::vector<size_t> m_HPBarFrameIdxs{};
	};

private:
	void SetSpritePositions();
	void SetSpriteMasks();
	void UpdateHPBarVitality(float fraction);
	void UpdateHPBarDamage(float fraction);
	void UpdateRGGuage(float fraction);
	void UpdateStyleExpBar(float fraction);
	void UpdateDTBarOrbs(float fraction);
	void ApplyDTFrameOpacity();
	void UpdateSprites();

private: // Debug layer

	void OnDrawDebugLayer();

private: // Members
	UINT m_Width{ 0 }, m_Height{ 0 };

	Style_t m_ActiveStyle{ Style_t::Trickster };

	size_t	m_HPLevel{ 1 }, m_DTLevel{ 1 }, 
			m_ActiveStyleLevel{ 1 }, m_RoyalGuardGaugeLevel{ 1 };

	float	m_HPFill{ 1.0f }, m_DMGFill{ 1.0f }, m_DTFill{1.0f},
			m_ActiveStyleExpFill{ 1.0f }, m_RoyalguardGuageFill{ 1.0f };

	bool	m_DTFrameVisible{ true };

	Theme_t m_ThemeID{ Theme_t::Colored };

	SpriteIndices m_SpriteIndices;

	ID3D11Device* m_pD3D11Device;
	ID3D11DeviceContext* m_pD3D11DeviceContext;

	std::vector<Graphics::SpriteDesc> m_SpriteDescs;
	std::unique_ptr<Graphics::BatchedSprites> m_pSpriteBatch;

private:
	static std::shared_ptr<Graphics::Texture2DArrayD3D11> s_pAtlasTexture;
};

}
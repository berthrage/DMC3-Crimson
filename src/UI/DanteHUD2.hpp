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

	enum class Group_t {
		HPBars = 0,
		DTBars,
		ExpBarStyleName,
		CompassRoyalguard,

		SIZE
	};

class DanteHUD2 {
public:
	DanteHUD2(ID3D11Device* pD3D11Device, ID3D11DeviceContext* pD3D11DeviceContext, UINT width, UINT height);
	virtual ~DanteHUD2();

public:
	const auto& GetWidth() { return m_Width; }
	const auto& GetHeight() { return m_Height; }

	void OnUpdate(double ts);
	bool OnDraw();

	ID3D11ShaderResourceView* GetSRV();

	const std::string& GetLastRenderingErrorMsg();
	HRESULT GetLastRenderingErrorCode();

public: // Controls
	// Plays the compass switch animation, except for the first style applied.
	void SetActiveStyle(Style_t style);
	void SetActiveStyleLevel(size_t level);
	void SetHPLevel(size_t level);
	void SetDTLevel(size_t level); // Level 0 means dt is not unlocked and will remove the dt bar entirely from the hud
	void SetRGGaugeLevel(size_t level);
	void SetHUDTheme(Theme_t theme);
	Theme_t GetHUDTheme() const { return m_ThemeID; }
	// Hides the style name next to the exp bar so the caller can draw its own.
	void SetStyleNameVisible(bool visible);
	void SetStyleLevelVisible(bool visible);
	// Hides a style's initials on the compass while it isn't the active style (locked or unavailable styles).
	void SetStyleInitialsVisible(Style_t style, bool visible);

	// Todo: Give these animations
	void SetHPVitalityAmount(float vit); // 0.0f to 1.0f
	void SetHPDamageAmount(float dmg); // 0.0f to 1.0f
	void SetRGGuageAmount(float rg); // 0.0f to 1.0f
	void SetActiveStyleExpFillAmount(float exp); // 0.0f to 1.0f
	void SetDTFill(float dt); // 0.0f to 1.0f

	// Shakes the HP and DT bars like the vanilla cockpit does on a hit, hpLost in actor HP.
	void TriggerHPShake(float hpLost);

	// Scales a group around its anchor sprite and then moves it by offset (sprite space), applied on top of the base layout.
	void SetGroupTransform(Group_t group, float scale, glm::vec2 offset);
	void SetGroupVisible(Group_t group, bool visible);
	// The gauge shares the compass group, this hides just the gauge and its frame.

	// Grows the render target around its center while sprites keep their on-screen size, so nothing gets clipped at the edges.
	void SetBoundsScale(float scale);
	float GetBoundsScale() const { return m_BoundsScale; }

	// Per-sprite editor, drawn into the caller's ImGui window.
	void DrawDebugControls();
	// Current sprite transforms and masks, formatted as code.
	std::string DumpSpriteLayout();

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
	struct SavedSprite {
		size_t idx;
		glm::vec3 translation;
		glm::vec3 scale;
		float opacity;
		float brightness;
		glm::vec4 tint;
	};

	void CommitActiveStyle(Style_t style);
	void ApplyStyleSwitchAnimation(std::vector<SavedSprite>& saved);
	void ApplyHPShake(std::vector<SavedSprite>& saved);
	void SetSpritePositions();
	void SetSpriteMasks();
	void UpdateHPBarVitality(float fraction);
	void UpdateHPBarDamage(float fraction);
	void UpdateRGGuage(float fraction);
	void UpdateStyleExpBar(float fraction);
	void UpdateDTBarOrbs(float fraction);
	void UpdateCompassInitialsVisibility();
	void UpdateSprites();
	bool GetGroupRange(Group_t group, size_t& begin, size_t& end, size_t& anchor) const;

private: // Members
	UINT m_Width{ 0 }, m_Height{ 0 };
	float m_BoundsScale{ 1.0f };

	Style_t m_ActiveStyle{ Style_t::Trickster };

	bool m_StyleApplied{ false };
	bool m_StyleSwitchActive{ false };
	float m_StyleSwitchTime{ 0.0f };
	std::array<bool, (size_t)CompassSlot_t::SIZE> m_StyleSwitchFadeSlots{};
	// Leaving a secondary style for another side fades its initials out before the switch lands.
	bool m_StyleExitPending{ false };
	float m_StyleExitTime{ 0.0f };
	Style_t m_PendingStyle{ Style_t::Trickster };

	// Hit shake, timer in 60 Hz frames like the cockpit's.
	float m_HPShakeFrames{ 0.0f };
	float m_HPShakeStep{ 0.0f };
	int m_HPShakeRange{ 0 };
	float m_HPShakeOffset{ 0.0f };
	uint32_t m_HPShakeRng{ 0x2545F491u };

	size_t	m_HPLevel{ 1 }, m_DTLevel{ 1 }, 
			m_ActiveStyleLevel{ 1 }, m_RoyalGuardGaugeLevel{ 1 };

	float	m_HPFill{ 1.0f }, m_DMGFill{ 1.0f }, m_DTFill{1.0f},
			m_ActiveStyleExpFill{ 1.0f }, m_RoyalguardGuageFill{ 1.0f };

	Theme_t m_ThemeID{ Theme_t::Colored };
	bool m_StyleNameVisible{ true };
	bool m_StyleLevelVisible{ true };
	std::array<bool, (size_t)Style_t::SIZE> m_StyleInitialsVisible{ true, true, true, true, true, true, true };

	SpriteIndices m_SpriteIndices;

	struct GroupTransform {
		float scale{ 1.0f };
		glm::vec2 offset{ 0.0f, 0.0f };
	};
	std::array<GroupTransform, (size_t)Group_t::SIZE> m_GroupTransforms{};
	std::array<bool, (size_t)Group_t::SIZE> m_GroupVisible{ true, true, true, true };

	ID3D11Device* m_pD3D11Device;
	ID3D11DeviceContext* m_pD3D11DeviceContext;

	std::vector<Graphics::SpriteDesc> m_SpriteDescs;
	std::unique_ptr<Graphics::BatchedSprites> m_pSpriteBatch;

private:
	static std::shared_ptr<Graphics::Texture2DArrayD3D11> s_pAtlasTexture;
};

}
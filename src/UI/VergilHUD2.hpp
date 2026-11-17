#pragma once

#include "BatchedSprites.hpp"
#include "GraphicsBase.hpp"
#include "Sprite.hpp"
#include "DanteHUD2.hpp"

#include <memory>

namespace Graphics { class EffekseerLayer; }

namespace HUD::Vergil {
	// Same groups and layout as Dante's HUD, CompassRoyalguard holds the compass and Mirage gauge, ExpBarStyleName only the EXP bar.
	using Group_t = HUD::Dante::Group_t;
	using Style_t = HUD::Dante::Style_t;

	// Mirage Trigger activation direction, one orb each.
	enum class DoppelDelay_t {
		Top = 0,
		Left,
		Right,

		SIZE
	};

class VergilHUD2 {
public:
	VergilHUD2(ID3D11Device* pD3D11Device, ID3D11DeviceContext* pD3D11DeviceContext, UINT width, UINT height);
	virtual ~VergilHUD2();

public:
	const auto& GetWidth() { return m_Width; }
	const auto& GetHeight() { return m_Height; }

	void OnUpdate(double ts);
	bool OnDraw();

	ID3D11ShaderResourceView* GetSRV();

	const std::string& GetLastRenderingErrorMsg();
	HRESULT GetLastRenderingErrorCode();

public: // Controls
	void SetHPLevel(size_t level);
	void SetDTLevel(size_t level); // Level 0 removes the DT bar

	void SetHPVitalityAmount(float vit); // 0.0f to 1.0f
	void SetHPDamageAmount(float dmg); // 0.0f to 1.0f
	void SetDTFill(float dt); // 0.0f to 1.0f
	// Full DT orbs pulse their glow while DT can be triggered.
	void SetDTReady(bool ready);
	// Lightning over the DT bar, plays while Devil Trigger is active.
	void SetDTActive(bool active);
	// DT Explosion charge over the DT orbs, the charged orbs burst on release.
	void SetDTExplosionCharge(float charge);

	void SetMirageFill(float fill); // 0.0f to 1.0f
	// The Mirage fill blinks in the DT pulse rhythm while Mirage Trigger can be used.
	void SetMirageReady(bool ready);
	// Lights the orb of the running Mirage Trigger's doppel delay, SIZE for none.
	void SetActiveDoppelDelay(DoppelDelay_t delay);
	void SetMirageGaugeVisible(bool visible);

	// Darkslayer and Quicksilver use Vergil's own EXP fills.
	void SetActiveStyle(Style_t style);
	void SetActiveStyleLevel(size_t level); // 1 to 4
	void SetStyleLevelVisible(bool visible);
	void SetActiveStyleExpFillAmount(float exp); // 0.0f to 1.0f

	void TriggerHPShake(float hpLost);

	void SetGroupTransform(Group_t group, float scale, glm::vec2 offset);
	void SetGroupVisible(Group_t group, bool visible);

	void SetBoundsScale(float scale);
	float GetBoundsScale() const { return m_BoundsScale; }

	void DrawCompassEffectControls();
	void DrawDTLightningControls();
	void DrawDTExplosionControls();
	void DrawDebugControls();
	std::string DumpSpriteLayout();

private:
	class SpriteIndices {
	public:
		void UpdateSprites(size_t hpBarLevel, size_t dtBarLevel, Style_t style, size_t styleLevel);

		inline const auto& GetSpriteIds() const { return m_SpriteIds; }

		inline const auto& GetCompassIdx() const { return m_CompassIdx; }
		inline const auto& GetDoppelOrbIdx(DoppelDelay_t delay, bool active) const { return active ? m_DoppelOrbActiveIdxs[(size_t)delay] : m_DoppelOrbInactiveIdxs[(size_t)delay]; }
		inline const auto& GetMirageFillIdx() const { return m_MirageFillIdx; }
		inline const auto& GetMirageFrameIdx() const { return m_MirageFrameIdx; }
		inline const auto& GetStyleExpBarLevelIdx() const { return m_StyleExpBarLevelIdx; }
		inline const auto& GetStyleExpBarFrameIdx() const { return m_StyleExpBarFrameIdx; }
		inline const auto& GetStyleExpBarIdx() const { return m_StyleExpBarIdx; }
		inline const auto& GetDTBarBeginIdx() const { return m_DTBarBeginIdx; }
		inline const auto& GetDTBarOrbBGIdxs() const { return m_DTBarOrbBGIdxs; }
		inline const auto& GetDTBarOrbIdxs() const { return m_DTBarOrbsIdxs; }
		inline const auto& GetDTBarOrbGlowIdxs() const { return m_DTBarOrbGlowIdxs; }
		inline const auto& GetDTBarOrbBurstIdxs() const { return m_DTBarOrbBurstIdxs; }
		inline const auto& GetDTBarFrameIdxs() const { return m_DTBarFrameIdxs; }
		inline const auto& GetHPBarBGIdxs() const { return m_HPBarBGIdxs; }
		inline const auto& GetHPBarDamageIdxs() const { return m_HPBarDamageIdxs; }
		inline const auto& GetHPBarVitalityIdxs() const { return m_HPBarVitalityIdxs; }
		inline const auto& GetHPBarVitalityEndGlowIdx() const { return m_VitalityEndGlowIdx; }
		inline const auto& GetHPBarFrameIdxs() const { return m_HPBarFrameIdxs; }

	private:
		std::vector<size_t> m_SpriteIds;

		size_t m_CompassIdx{};
		std::array<size_t, (size_t)DoppelDelay_t::SIZE> m_DoppelOrbActiveIdxs{};
		std::array<size_t, (size_t)DoppelDelay_t::SIZE> m_DoppelOrbInactiveIdxs{};
		size_t m_MirageFillIdx{};
		size_t m_MirageFrameIdx{};
		size_t m_StyleExpBarLevelIdx{};
		size_t m_StyleExpBarFrameIdx{};
		size_t m_StyleExpBarIdx{};
		size_t m_DTBarBeginIdx{};
		std::vector<size_t> m_DTBarOrbBGIdxs{};
		std::vector<size_t> m_DTBarOrbsIdxs{};
		std::vector<size_t> m_DTBarOrbGlowIdxs{};
		std::vector<size_t> m_DTBarOrbBurstIdxs{};
		std::vector<size_t> m_DTBarFrameIdxs{};
		std::vector<size_t> m_HPBarBGIdxs{};
		std::vector<size_t> m_HPBarDamageIdxs{};
		std::vector<size_t> m_HPBarVitalityIdxs{};
		size_t m_VitalityEndGlowIdx{};
		std::vector<size_t> m_HPBarFrameIdxs{};
	};

	struct SavedSprite {
		size_t idx;
		glm::vec3 translation;
		glm::vec3 scale;
		float opacity;
		float brightness;
		glm::vec4 tint;
	};

	void SaveSprite(std::vector<SavedSprite>& saved, size_t idx) const;
	void ApplyHPShake(std::vector<SavedSprite>& saved);
	void ApplyDTOrbPulse(std::vector<SavedSprite>& saved);
	void ApplyDTExplosionBurst(std::vector<SavedSprite>& saved);
	void ApplyMiragePulse(std::vector<SavedSprite>& saved);
	void SetSpritePositions();
	void SetSpriteMasks();
	void UpdateHPBarVitality(float fraction);
	void UpdateHPBarDamage(float fraction);
	void UpdateDTBarOrbs(float fraction);
	void UpdateMirageGauge();
	void UpdateStyleExpBar(float fraction);
	void UpdateSprites();
	bool GetGroupRange(Group_t group, size_t& begin, size_t& end, size_t& anchor) const;
	bool RenderCompassEffects();
	void RenderDTLightning();
	void RenderDTExplosion();

private: // Members
	UINT m_Width{ 0 }, m_Height{ 0 };
	float m_BoundsScale{ 1.0f };

	// Hit shake, timer in 60 Hz frames like the cockpit's.
	float m_HPShakeFrames{ 0.0f };
	float m_HPShakeStep{ 0.0f };
	int m_HPShakeRange{ 0 };
	float m_HPShakeOffset{ 0.0f };
	uint32_t m_HPShakeRng{ 0x2545F491u };

	bool m_DTReady{ false };
	float m_DTPulseTime{ 0.0f };

	bool m_MirageReady{ false };
	float m_MiragePulseTime{ 0.0f };
	float m_MirageFill{ 1.0f };
	bool m_MirageGaugeVisible{ true };
	DoppelDelay_t m_ActiveDoppelDelay{ DoppelDelay_t::SIZE };

	Style_t m_ActiveStyle{ Style_t::Darkslayer };
	size_t m_ActiveStyleLevel{ 1 };
	bool m_StyleLevelVisible{ true };
	float m_StyleExpFill{ 1.0f };

	size_t m_HPLevel{ 1 }, m_DTLevel{ 1 };
	float m_HPFill{ 1.0f }, m_DMGFill{ 1.0f }, m_DTFill{ 1.0f };

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

	// Under the compass, filling its hollow middle: the void at the bottom, the storm over it.
	// The void has one effect per style, only the active style's plays so a switch restarts it.
	// The storm is shared by all styles and only uses the first layer.
	enum class CompassStyle_t { Darkslayer = 0, Quicksilver, SIZE };
	struct CompassEffect {
		std::array<std::unique_ptr<Graphics::EffekseerLayer>, (size_t)CompassStyle_t::SIZE> layers;
		bool perStyle{ false };
		bool enabled{ true };
		float scale{ 11.8f }; // HUD pixels per effect unit
		glm::vec2 offset{ -26.5f, -3.5f }; // HUD pixels from the compass ring center, y up
		float speed{ 1.0f };
		int warmupFrames{ 0 }; // Starts this far in on a style switch
	};
	CompassEffect m_CompassVoid;
	CompassEffect m_CompassStorm;
	CompassStyle_t m_LastCompassStyle{ CompassStyle_t::SIZE };
	CompassStyle_t GetCompassStyle() const { return m_ActiveStyle == Style_t::Quicksilver ? CompassStyle_t::Quicksilver : CompassStyle_t::Darkslayer; }
	size_t GetActiveLayerIdx(const CompassEffect& effect) const { return effect.perStyle ? (size_t)GetCompassStyle() : 0; }

	std::unique_ptr<Graphics::EffekseerLayer> m_pDTLightning;
	bool m_DTActive{ false };
	bool m_DTLightningEnabled{ true };
	bool m_DTLightningForce{ false };
	float m_DTLightningScale{ 10.0f }; // HUD pixels per effect unit
	glm::vec2 m_DTLightningOffset{ 0.0f, 0.0f }; // HUD pixels from the first DT orb, y up
	float m_DTLightningSpeed{ 1.0f };

	std::unique_ptr<HUD::DTExplosionHUD2> m_pDTExplosion;

private:
	static std::shared_ptr<Graphics::Texture2DArrayD3D11> s_pAtlasTexture;
};

}

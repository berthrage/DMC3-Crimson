#pragma once

#include "GraphicsBase.hpp"

#include <array>
#include <memory>
#include <vector>

namespace Graphics { class EffekseerLayer; }

namespace HUD {

// DT Explosion charge over the DT orbs, the same for Dante's and Vergil's HUD 2.0.
// Every 1000 charge is one orb; the first 3 light as soon as the charge starts since the explosion only spends orbs past them.
class DTExplosionHUD2 {
public:
	static constexpr size_t kMaxOrbs = 10;
	static constexpr size_t kFreeOrbs = 3;
	static constexpr float kChargePerOrb = 1000.0f;

	DTExplosionHUD2(ID3D11Device* pD3D11Device, ID3D11DeviceContext* pD3D11DeviceContext);
	~DTExplosionHUD2();

	void SetCharge(float charge);
	void Update(float dt, size_t orbCount);
	// Orb positions in render target pixels from its center, y up. Drawn over what is already in the target.
	void Render(ID3D11DeviceContext* pD3D11DeviceContext, ID3D11RenderTargetView* pRTV, UINT width, UINT height,
		const std::vector<glm::vec2>& orbPositions, float groupScale);

	// Sprite burst of the orbs the charge reached, right after release.
	bool GetOrbBurst(size_t orb, float& scale, float& opacity) const;

	void DrawControls();

private:
	void Release(float charge);
	size_t GetReachedOrbs(float charge) const;

private:
	std::unique_ptr<Graphics::EffekseerLayer> m_pFirstOrbs;
	std::array<std::unique_ptr<Graphics::EffekseerLayer>, kMaxOrbs - kFreeOrbs> m_pOrbs;
	std::unique_ptr<Graphics::EffekseerLayer> m_pRelease;

	size_t m_OrbCount{ 0 };
	float m_Charge{ 0.0f };
	float m_LastCharge{ 0.0f };
	size_t m_ReachedOrbs{ 0 };

	// Charge per second, smoothed since the game charges at its own tick rate.
	float m_Rate{ 0.0f };
	float m_MaxRate{ 0.0f };
	float m_ChargingOrbSpeed{ 1.0f };

	bool m_Enabled{ true };
	float m_Scale{ 10.0f }; // HUD pixels per effect unit
	glm::vec2 m_FirstOrbsOffset{ 0.0f, 0.0f }; // HUD pixels from the first orb, y up
	glm::vec2 m_OrbOffset{ 0.0f, 0.0f }; // HUD pixels from each later orb, y up
	float m_Speed{ 1.0f };
	// The orb being charged plays at the charge rate, so it holds still when the charge stalls.
	bool m_LinkSpeedToCharge{ true };
	float m_ReferenceRate{ 0.0f }; // Charge per second for normal speed, 0 uses the fastest rate of the current charge
	float m_RateSmoothing{ 0.12f }; // Seconds

	float m_MinReleaseCharge{ 2000.0f }; // Releasing below this skips the burst and release effect
	float m_BurstTime{ -1.0f };
	size_t m_BurstOrbs{ 0 };
	float m_BurstDuration{ 0.375f };
	float m_BurstScale{ 3.0f };

	bool m_ReleaseEnabled{ true };
	float m_ReleaseTime{ 0.0f };
	float m_ReleaseDuration{ 1.0f };
	glm::vec2 m_ReleaseOffset{ 0.0f, 0.0f }; // HUD pixels from the first orb, y up

	bool m_Preview{ false };
	float m_PreviewCharge{ 0.0f };
	float m_PreviewRate{ 0.0f };
	float m_PreviewStall{ 10000.0f };
};

}

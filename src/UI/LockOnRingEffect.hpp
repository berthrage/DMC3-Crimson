#pragma once

#include "GraphicsBase.hpp"

#include <array>
#include <memory>
#include <vector>

namespace Graphics { class EffekseerLayer; }

namespace HUD {

// A Crimson lock-on ring as an Effekseer effect. It draws into a target of its own so the HUD keeps the rotating pie
// mask the ring sprite had: lost health or displacement still cuts the ring away.
class LockOnRingEffect {
public:
	LockOnRingEffect(ID3D11Device* pD3D11Device, ID3D11DeviceContext* pD3D11DeviceContext, const wchar_t* effectPath,
		int targetCount, UINT targetSize = 512);
	~LockOnRingEffect();

	bool IsLoaded() const;

	// Once per frame; visible keeps the looping effect running.
	void Update(float dt, bool visible);

	// Renders the ring into that player's target and returns its view, or null when it cannot draw.
	// Color goes into the Ring node's fixed custom data 1 (ToonBase's colour); null keeps the colour authored in the
	// effect, so tint the returned view when drawing it instead.
	// Fill goes to dynamic input 3 (0 = hidden ring, 1 = full); scale is target pixels per effect unit; offset is from
	// the target centre (y up).
	ID3D11ShaderResourceView* Render(ID3D11DeviceContext* pD3D11DeviceContext, int target, const glm::vec3* color,
		float fill, float scale, glm::vec2 offset);

private:
	void ApplyColor(const glm::vec3* color);

private:
	struct Target {
		Graphics::ComPtr<ID3D11Texture2D> Texture;
		Graphics::ComPtr<ID3D11RenderTargetView> RTV;
		Graphics::ComPtr<ID3D11ShaderResourceView> SRV;
	};

	std::unique_ptr<Graphics::EffekseerLayer> m_pLayer;
	std::vector<Target> m_Targets;
	UINT m_TargetSize{ 0 };

	// The node whose custom data 1 is a fixed colour, looked up again after a reload.
	const void* m_ColorEffect{ nullptr };
	std::array<float, 4>* m_pColorData{ nullptr };
	std::array<float, 4> m_AuthoredColor{};
	// Undoes the material's Core_Add_Level boost, so a requested colour reads 1:1.
	float m_ColorScale{ 1.0f };
};

}

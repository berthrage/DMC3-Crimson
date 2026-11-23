#include "LockOnRingEffect.hpp"

#include "EffekseerLayer.hpp"

#include "Effekseer/Effekseer.EffectNode.h"
#include "Effekseer/Material/Effekseer.MaterialFile.h"

#include <filesystem>
#include <fstream>
#include <iterator>

namespace HUD {

namespace {

// ToonBase's emissive is colour + colour * Core_Add_Level inside the ring's core. The runtime material keeps no uniform
// names, so the name comes from the .efkmat and the value from the node (it can be overridden per node).
float CoreAddLevel(const std::wstring& effectPath, Effekseer::Effect* effect, Effekseer::EffectNodeImplemented* node)
{
	const auto& material = node->RendererCommon.MaterialData;
	if (material.MaterialIndex < 0 || material.MaterialIndex >= effect->GetMaterialCount())
		return 0.0f;
	const char16_t* materialPath = effect->GetMaterialPath(material.MaterialIndex);
	if (!materialPath)
		return 0.0f;

	const std::filesystem::path path = std::filesystem::path(effectPath).parent_path()
		/ std::filesystem::path(reinterpret_cast<const wchar_t*>(materialPath));
	std::ifstream file(path, std::ios::binary);
	const std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	Effekseer::MaterialFile materialFile;
	if (data.empty() || !materialFile.Load(data.data(), static_cast<int32_t>(data.size())))
		return 0.0f;

	for (int32_t i = 0; i < materialFile.GetUniformCount(); ++i) {
		if (std::string("Core_Add_Level") == materialFile.GetUniformName(i))
			return i < (int32_t)material.MaterialUniforms.size() ? material.MaterialUniforms[i][0] : 0.0f;
	}
	return 0.0f;
}

}

LockOnRingEffect::LockOnRingEffect(ID3D11Device* pD3D11Device, ID3D11DeviceContext* pD3D11DeviceContext,
	const wchar_t* effectPath, int targetCount, UINT targetSize)
	: m_TargetSize(targetSize)
{
	if (!pD3D11Device || !pD3D11DeviceContext || !effectPath || targetCount <= 0 || targetSize == 0)
		return;

	m_pLayer = std::make_unique<Graphics::EffekseerLayer>(pD3D11Device, pD3D11DeviceContext);
	m_pLayer->SetEffectPath(effectPath);

	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = targetSize;
	desc.Height = targetSize;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	m_Targets.resize((size_t)targetCount);
	for (Target& target : m_Targets) {
		if (FAILED(pD3D11Device->CreateTexture2D(&desc, nullptr, target.Texture.GetAddressOf()))
			|| FAILED(pD3D11Device->CreateRenderTargetView(target.Texture.Get(), nullptr, target.RTV.GetAddressOf()))
			|| FAILED(pD3D11Device->CreateShaderResourceView(target.Texture.Get(), nullptr, target.SRV.GetAddressOf()))) {
			target = Target{};
			m_Targets.clear();
			return;
		}
	}
}

LockOnRingEffect::~LockOnRingEffect()
{
}

bool LockOnRingEffect::IsLoaded() const
{
	return m_pLayer && m_pLayer->IsLoaded() && !m_Targets.empty();
}

void LockOnRingEffect::Update(float dt, bool visible)
{
	if (!m_pLayer)
		return;

	m_pLayer->SetPlaying(visible);
	m_pLayer->Update(dt);
}

ID3D11ShaderResourceView* LockOnRingEffect::Render(ID3D11DeviceContext* pD3D11DeviceContext, int target,
	const glm::vec3* color, float fill, float scale, glm::vec2 offset)
{
	if (!m_pLayer || !pD3D11DeviceContext || target < 0 || (size_t)target >= m_Targets.size())
		return nullptr;

	Target& rt = m_Targets[(size_t)target];
	if (!rt.RTV)
		return nullptr;

	// Custom data is read from the node at draw time, so each player's colour only has to be set before its render.
	ApplyColor(color);
	// In0/In1 drive the root's DynamicPos (x, y), so they stay at 0 or the ring leaves the target.
	m_pLayer->SetDynamicInput(3, fill);
	m_pLayer->SetTransform(offset, scale);

	const float clear[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	pD3D11DeviceContext->ClearRenderTargetView(rt.RTV.Get(), clear);
	m_pLayer->Render(pD3D11DeviceContext, rt.RTV.Get(), m_TargetSize, m_TargetSize);

	return rt.SRV.Get();
}

void LockOnRingEffect::ApplyColor(const glm::vec3* color)
{
	Effekseer::Effect* effect = m_pLayer->GetEffect();
	if (effect != m_ColorEffect) {
		m_ColorEffect = effect;
		m_pColorData = nullptr;
		m_ColorScale = 1.0f;
		if (effect) {
			auto* root = static_cast<Effekseer::EffectNodeImplemented*>(effect->GetRoot());
			root->Traverse([this, effect](Effekseer::EffectNodeImplemented* node) {
				auto& customData = node->RendererCommon.CustomData1;
				if (customData.Type != Effekseer::ParameterCustomDataType::Fixed4D)
					return true;
				m_pColorData = &customData.Fixed4D;
				m_AuthoredColor = customData.Fixed4D;
				const float coreLevel = CoreAddLevel(m_pLayer->GetEffectPath(), effect, node);
				m_ColorScale = coreLevel > 0.0f ? 1.0f / (1.0f + coreLevel) : 1.0f;
				return false;
			});
		}
	}

	if (!m_pColorData)
		return;

	// W is the material's mask strength, not alpha, so it stays as authored. The colour is scaled down so the ring's
	// core comes out at the colour asked for rather than brightened by the material.
	if (color)
		*m_pColorData = { color->r * m_ColorScale, color->g * m_ColorScale, color->b * m_ColorScale, m_AuthoredColor[3] };
	else
		*m_pColorData = m_AuthoredColor;
}

}

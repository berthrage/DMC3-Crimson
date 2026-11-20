#include "DTExplosionHUD2.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <format>
#include <string>

#include "EffekseerLayer.hpp"
#include "imgui.h"

namespace HUD {

namespace {

constexpr const wchar_t* kFirstOrbsPath = LR"(Crimson\vfx\hud2\dt_explosion_charge_1-3.efkefc)";
constexpr const wchar_t* kReleasePath = LR"(Crimson\vfx\hud2\dt_explosion_release.efkefc)";
constexpr int kMaxSprites = 500;

void DrawLayerStatus(const Graphics::EffekseerLayer& layer)
{
	const std::string path = std::filesystem::path(layer.GetEffectPath()).filename().string();
	if (layer.IsLoaded())
		ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "%s: loaded", path.c_str());
	else
		ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.3f, 1.0f), "%s: not loaded", path.c_str());
}

}

DTExplosionHUD2::DTExplosionHUD2(ID3D11Device* pD3D11Device, ID3D11DeviceContext* pD3D11DeviceContext)
{
	const auto makeLayer = [&](const std::wstring& path) {
		auto layer = std::make_unique<Graphics::EffekseerLayer>(pD3D11Device, pD3D11DeviceContext, kMaxSprites);
		layer->SetEffectPath(path);
		return layer;
	};

	m_pFirstOrbs = makeLayer(kFirstOrbsPath);
	for (size_t i = 0; i < m_pOrbs.size(); i++)
		m_pOrbs[i] = makeLayer(std::format(LR"(Crimson\vfx\hud2\dt_explosion_charge_{}.efkefc)", i + kFreeOrbs + 1));
	m_pRelease = makeLayer(kReleasePath);
}

DTExplosionHUD2::~DTExplosionHUD2()
{
}

void DTExplosionHUD2::SetCharge(float charge)
{
	m_Charge = (std::max)(charge, 0.0f);
}

size_t DTExplosionHUD2::GetReachedOrbs(float charge) const
{
	if (charge <= 0.0f)
		return 0;

	const size_t reached = (std::max)((size_t)std::ceil(charge / kChargePerOrb), kFreeOrbs);
	return (std::min)(reached, m_OrbCount);
}

void DTExplosionHUD2::Release(float charge)
{
	m_BurstOrbs = charge >= m_MinReleaseCharge ? GetReachedOrbs(charge) : 0;
	m_BurstTime = m_BurstOrbs > 0 ? 0.0f : -1.0f;

	if (m_ReleaseEnabled && m_BurstOrbs > 0)
	{
		m_pRelease->SetPlaying(true);
		m_pRelease->Restart();
		m_ReleaseTime = m_ReleaseDuration;
	}
}

void DTExplosionHUD2::Update(float dt, size_t orbCount)
{
	m_OrbCount = (std::min)(orbCount, kMaxOrbs);

	if (m_Preview && m_PreviewRate > 0.0f && m_PreviewCharge > 0.0f)
		m_PreviewCharge = (std::min)(m_PreviewCharge + m_PreviewRate * dt, m_PreviewStall);

	const float charge = m_Preview ? m_PreviewCharge : m_Charge;

	// The charge only grows while held, any drop is the release.
	if (charge < m_LastCharge && m_Enabled)
		Release(m_LastCharge);

	if (m_LastCharge <= 0.0f && charge > 0.0f)
	{
		m_Rate = 0.0f;
		m_MaxRate = 0.0f;
	}

	if (dt > 0.0f && charge > 0.0f)
	{
		const float rate = (std::max)(charge - m_LastCharge, 0.0f) / dt;
		m_Rate += (rate - m_Rate) * (1.0f - std::exp(-dt / (std::max)(m_RateSmoothing, 0.001f)));
		m_MaxRate = (std::max)(m_MaxRate, m_Rate);
	}
	m_LastCharge = charge;

	m_ReachedOrbs = m_Enabled ? GetReachedOrbs(charge) : 0;

	const float reference = m_ReferenceRate > 0.0f ? m_ReferenceRate : m_MaxRate;
	m_ChargingOrbSpeed = reference > 0.0f ? (std::min)(m_Rate / reference, 1.0f) : 1.0f;
	const size_t chargingOrb = (size_t)(charge / kChargePerOrb);

	m_pFirstOrbs->SetPlaying(m_ReachedOrbs > 0 && charge >= kChargePerOrb);
	m_pFirstOrbs->SetSpeed(m_Speed);
	m_pFirstOrbs->Update(dt);

	for (size_t i = 0; i < m_pOrbs.size(); i++)
	{
		const size_t orb = i + kFreeOrbs;
		const bool charging = m_LinkSpeedToCharge && orb == chargingOrb;
		m_pOrbs[i]->SetPlaying(orb < m_ReachedOrbs);
		m_pOrbs[i]->SetSpeed(m_Speed * (charging ? m_ChargingOrbSpeed : 1.0f));
		m_pOrbs[i]->Update(dt);
	}

	if (m_ReleaseTime > 0.0f)
	{
		m_ReleaseTime -= dt;
		if (m_ReleaseTime <= 0.0f)
			m_pRelease->SetPlaying(false);
	}
	m_pRelease->SetSpeed(m_Speed);
	m_pRelease->Update(dt);

	if (m_BurstTime >= 0.0f)
	{
		m_BurstTime += dt;
		if (m_BurstTime >= m_BurstDuration)
			m_BurstTime = -1.0f;
	}
}

void DTExplosionHUD2::Render(ID3D11DeviceContext* pD3D11DeviceContext, ID3D11RenderTargetView* pRTV, UINT width, UINT height,
	const std::vector<glm::vec2>& orbPositions, float groupScale)
{
	if (!pRTV || width == 0 || height == 0 || orbPositions.empty())
		return;

	const float scale = m_Scale * groupScale;
	const auto render = [&](Graphics::EffekseerLayer& layer, size_t orb, glm::vec2 offset) {
		if (!layer.IsPlaying() || orb >= orbPositions.size())
			return;

		layer.SetTransform(orbPositions[orb] + offset * groupScale, scale);
		layer.Render(pD3D11DeviceContext, pRTV, width, height);
	};

	render(*m_pFirstOrbs, 0, m_FirstOrbsOffset);
	for (size_t i = 0; i < m_pOrbs.size(); i++)
		render(*m_pOrbs[i], i + kFreeOrbs, m_OrbOffset);
	render(*m_pRelease, 0, m_ReleaseOffset);
}

bool DTExplosionHUD2::GetOrbBurst(size_t orb, float& scale, float& opacity) const
{
	if (m_BurstTime < 0.0f || orb >= m_BurstOrbs || m_BurstDuration <= 0.0f)
		return false;

	const float t = (std::min)(m_BurstTime / m_BurstDuration, 1.0f);
	const float out = 1.0f - t;
	scale = 1.0f + (m_BurstScale - 1.0f) * (1.0f - out * out * out);
	opacity = out * out;
	return true;
}

void DTExplosionHUD2::DrawControls()
{
	ImGui::TextWrapped("Paths are relative to the game folder. Effect units are centered on each orb (the 1-3 effect and the "
		"release on the first orb), y up, front view. Drawn over every HUD sprite.");

	DrawLayerStatus(*m_pFirstOrbs);
	for (const auto& layer : m_pOrbs)
		DrawLayerStatus(*layer);
	DrawLayerStatus(*m_pRelease);

	ImGui::Checkbox("Enabled", &m_Enabled);
	ImGui::SameLine();
	if (ImGui::Button("Restart"))
	{
		m_pFirstOrbs->Restart();
		for (const auto& layer : m_pOrbs)
			layer->Restart();
	}

	ImGui::DragFloat("HUD px per unit", &m_Scale, 0.1f, 0.1f, 200.0f, "%.2f");
	ImGui::DragFloat2("Orbs 1-3 offset (px, y up)", &m_FirstOrbsOffset.x, 0.5f, -1000.0f, 1000.0f, "%.1f");
	ImGui::DragFloat2("Orb 4+ offset (px, y up)", &m_OrbOffset.x, 0.5f, -1000.0f, 1000.0f, "%.1f");
	ImGui::DragFloat("Speed", &m_Speed, 0.01f, 0.0f, 10.0f, "%.2f");
	ImGui::Checkbox("Charging orb follows the charge rate", &m_LinkSpeedToCharge);
	ImGui::DragFloat("Normal rate (charge/s, 0 = auto)", &m_ReferenceRate, 10.0f, 0.0f, 100000.0f, "%.0f");
	ImGui::DragFloat("Rate smoothing (s)", &m_RateSmoothing, 0.005f, 0.0f, 1.0f, "%.3f");
	ImGui::Text("Charge %.0f, rate %.0f/s (max %.0f), orbs reached %zu, charging orb speed %.2f",
		m_LastCharge, m_Rate, m_MaxRate, m_ReachedOrbs, m_ChargingOrbSpeed);

	ImGui::Separator();
	ImGui::Text("Release");
	ImGui::DragFloat("Min charge to explode", &m_MinReleaseCharge, 10.0f, 0.0f, 10000.0f, "%.0f");
	ImGui::DragFloat("Burst scale", &m_BurstScale, 0.01f, 1.0f, 5.0f, "%.2f");
	ImGui::DragFloat("Burst duration (s)", &m_BurstDuration, 0.005f, 0.01f, 2.0f, "%.3f");
	ImGui::Checkbox("Release effect", &m_ReleaseEnabled);
	ImGui::DragFloat2("Release offset (px, y up)", &m_ReleaseOffset.x, 0.5f, -1000.0f, 1000.0f, "%.1f");
	ImGui::DragFloat("Release effect length (s)", &m_ReleaseDuration, 0.01f, 0.05f, 10.0f, "%.2f");

	ImGui::Separator();
	ImGui::Text("Preview");
	ImGui::Checkbox("Preview charge (ignore the game)", &m_Preview);
	ImGui::SliderFloat("Charge", &m_PreviewCharge, 0.0f, 10000.0f, "%.0f");
	ImGui::DragFloat("Auto charge (per s)", &m_PreviewRate, 10.0f, 0.0f, 20000.0f, "%.0f");
	ImGui::DragFloat("Stall at", &m_PreviewStall, 10.0f, 0.0f, 10000.0f, "%.0f");
	if (ImGui::Button("Start"))
		m_PreviewCharge = 1.0f;
	ImGui::SameLine();
	if (ImGui::Button("Release"))
		m_PreviewCharge = 0.0f;
}

}

#include "VergilHUD2.hpp"

#include <format>
#include <algorithm>
#include <cmath>
#include <filesystem>

#include "DanteHUD2HardcodedData.hpp"
#include "VergilHUD2HardcodedData.hpp"
#include "DTExplosionHUD2.hpp"
#include "EffekseerLayer.hpp"
#include "imgui.h"

namespace HUD::Vergil {

std::shared_ptr<Graphics::Texture2DArrayD3D11> VergilHUD2::s_pAtlasTexture;

namespace {

namespace DC = HUD::Dante::HC;
using HUD::Dante::Theme_t;

constexpr const char* kAtlas1Path = R"(Crimson\assets\crimsonhud\hud2_atlases\atlas1.png)";
constexpr const char* kAtlas2Path = R"(Crimson\assets\crimsonhud\hud2_atlases\atlas2.png)";

// Indexed by CompassStyle_t.
constexpr const wchar_t* kCompassVoidPaths[] = {
	LR"(Crimson\vfx\hud2\vergil_compass_void_darkslayer.efkefc)",
	LR"(Crimson\vfx\hud2\vergil_compass_void_quicksilver.efkefc)",
};
constexpr const wchar_t* kCompassStormPath = LR"(Crimson\vfx\hud2\vergil_compass_storm.efkefc)";
constexpr const wchar_t* kDTLightningPath = LR"(Crimson\vfx\hud2\dt_lightning_vergil.efkefc)";

// HP and DT bars are Dante's colored theme, always slanted.
constexpr Theme_t kTheme = Theme_t::Colored;
constexpr bool kStraightHP = false;

// Vergil's sprite ids come after all of atlas1's in the batch descs.
constexpr size_t VergilSprite(HC::Sprites::SpriteID_t id)
{
	return (size_t)DC::Sprites::SpriteID_t::SIZE + (size_t)id;
}

size_t GetStyleExpBarSprite(Style_t style)
{
	switch (style)
	{
	case Style_t::Darkslayer:	return VergilSprite(HC::Sprites::SpriteID_t::Exp_DS);
	case Style_t::Quicksilver:	return VergilSprite(HC::Sprites::SpriteID_t::Exp_QS);
	default:					return (size_t)DC::Sprites::GetStyleExpBarSpriteID(kTheme, style);
	}
}

// Cockpit hit shake tiers from CUIDCockpit00_UpdateHP_sub_14027D3F0, same as Dante's HUD.
constexpr float kHPShakeBigHit = 1000.0f;
constexpr float kHPShakeMediumHit = 50.0f;
constexpr float kHPShakeBigFrames = 15.0f;
constexpr float kHPShakeFrames = 5.0f;
constexpr int kHPShakeBigRange = 10;
constexpr int kHPShakeMediumRange = 5;
constexpr int kHPShakeSmallRange = 3;
constexpr float kHPShakeUnit = 2.0f * 1080.0f / (448.0f * 215.0f);

// DT orb glow pulse, the Mirage fill blinks with the same period.
constexpr float kDTPulsePeriod = 0.9f;
constexpr float kDTPulseMinOpacity = 0.2f;
constexpr float kMiragePulseMinBrightness = 0.25f;

uint32_t NextShakeRandom(uint32_t& state)
{
	state ^= state << 13;
	state ^= state >> 17;
	state ^= state << 5;
	return state;
}

float RollShakeOffset(uint32_t& state, int range)
{
	const bool up = NextShakeRandom(state) & 1;
	const float amount = (float)(NextShakeRandom(state) % (uint32_t)(std::max)(range, 1));
	return up ? amount : -amount;
}

float PulseWave(float time)
{
	return 0.5f - 0.5f * std::cos(6.28318531f * time / kDTPulsePeriod);
}

// The compass ring center is Dante's compass frame center, the Vergil compass sprite is offset from it.
glm::vec2 GetCompassRingOffset()
{
	const auto& ring = DC::Transforms::GetCompassFrame(kTheme).Position;
	const auto& compass = HC::Transforms::COMPASS.Position;
	return { ring.x - compass.x, ring.y - compass.y };
}

DC::Transforms::HPBarFrameVarient_t GetHPFrameTransformVarient(size_t i, size_t level)
{
	using V = DC::Transforms::HPBarFrameVarient_t;
	if (level == 1)					return V::Single;
	if (i == 0)						return V::Head;
	if (i == 10 && level == 11)		return V::Single;
	if (i == level - 1)				return V::Tail;
	if (i == 9)						return V::Tail;
	if (i == 10)					return V::Head;
	return V::Middle;
}

DC::Sprites::HPBarFrameVarient_t GetHPFrameSpriteVarient(size_t i, size_t level)
{
	using V = DC::Sprites::HPBarFrameVarient_t;
	switch (GetHPFrameTransformVarient(i, level))
	{
	case DC::Transforms::HPBarFrameVarient_t::Single:	return V::Single;
	case DC::Transforms::HPBarFrameVarient_t::Head:		return V::Head;
	case DC::Transforms::HPBarFrameVarient_t::Tail:		return V::Tail;
	default:											return V::Mid;
	}
}

}

void VergilHUD2::PreloadTextures(ID3D11Device* pD3D11Device)
{
	if (!s_pAtlasTexture) {
		s_pAtlasTexture = std::make_shared<Graphics::Texture2DArrayD3D11>(
			std::vector<std::string>{ kAtlas1Path, kAtlas2Path }, pD3D11Device);
	}
}

VergilHUD2::VergilHUD2(ID3D11Device* pD3D11Device, ID3D11DeviceContext* pD3D11DeviceContext, UINT width, UINT height)
	: m_pD3D11Device(pD3D11Device), m_pD3D11DeviceContext(pD3D11DeviceContext), m_Width(width), m_Height(height)
{
	PreloadTextures(pD3D11Device);

	if (m_SpriteDescs.size() == 0) {
		DC::Sprites::LoadSpriteDescs(m_SpriteDescs, kAtlas1Path);
		HC::Sprites::LoadSpriteDescs(m_SpriteDescs, kAtlas2Path);
	}

	m_SpriteIndices.UpdateSprites(m_HPLevel, m_DTLevel, m_ActiveStyle, m_ActiveStyleLevel);

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
	UpdateMirageGauge();

	using RatioFlags = Graphics::BatchedSprites::RatioFlags;

	m_pSpriteBatch->SetRatioFlags((int)RatioFlags::FitToVP);

	const auto makeLayer = [&](const wchar_t* path, bool playing) {
		auto layer = std::make_unique<Graphics::EffekseerLayer>(m_pD3D11Device, m_pD3D11DeviceContext);
		layer->SetEffectPath(path);
		layer->SetPlaying(playing);
		return layer;
	};

	m_CompassVoid.perStyle = true;
	m_CompassVoid.warmupFrames = 206;
	m_LastCompassStyle = GetCompassStyle();
	for (size_t i = 0; i < (size_t)CompassStyle_t::SIZE; i++)
		m_CompassVoid.layers[i] = makeLayer(kCompassVoidPaths[i], (CompassStyle_t)i == GetCompassStyle());
	m_CompassStorm.layers[0] = makeLayer(kCompassStormPath, true);
	m_pDTLightning = makeLayer(kDTLightningPath, false);
	m_pDTExplosion = std::make_unique<HUD::DTExplosionHUD2>(m_pD3D11Device, m_pD3D11DeviceContext);
}

VergilHUD2::~VergilHUD2()
{
}

void VergilHUD2::OnUpdate(double ts)
{
	const float dt = (float)ts;

	// The cockpit re-rolls the jitter once per game frame, keep that rate at any fps.
	if (m_HPShakeOffset != 0.0f || m_HPShakeFrames > 0.0f)
	{
		m_HPShakeStep += dt * 60.0f;
		if (m_HPShakeStep >= 1.0f)
		{
			const float steps = std::floor(m_HPShakeStep);
			m_HPShakeStep -= steps;
			if (m_HPShakeFrames > 0.0f)
			{
				m_HPShakeFrames = (std::max)(0.0f, m_HPShakeFrames - steps);
				m_HPShakeOffset = RollShakeOffset(m_HPShakeRng, m_HPShakeRange);
			}
			else
			{
				m_HPShakeOffset = 0.0f;
			}
		}
	}

	if (m_DTReady)
		m_DTPulseTime = std::fmod(m_DTPulseTime + dt, kDTPulsePeriod);

	if (m_MirageReady)
		m_MiragePulseTime = std::fmod(m_MiragePulseTime + dt, kDTPulsePeriod);

	const CompassStyle_t compassStyle = GetCompassStyle();
	const bool styleSwitched = compassStyle != m_LastCompassStyle;
	m_LastCompassStyle = compassStyle;

	for (CompassEffect* effect : { &m_CompassVoid, &m_CompassStorm })
	{
		const size_t activeIdx = GetActiveLayerIdx(*effect);
		for (size_t i = 0; i < effect->layers.size(); i++)
		{
			auto& layer = effect->layers[i];
			if (!layer)
				continue;

			const bool active = i == activeIdx;
			layer->SetPlaying(effect->enabled && active);
			if (active && styleSwitched && effect->perStyle)
				layer->Restart(effect->warmupFrames);
			if (layer->IsPlaying())
				layer->Update(dt);
		}
	}

	if (m_pDTLightning)
	{
		m_pDTLightning->SetPlaying(m_DTLightningEnabled && (m_DTActive || m_DTLightningForce) && m_DTLevel > 0);
		m_pDTLightning->Update(dt);
	}

	if (m_pDTExplosion)
		m_pDTExplosion->Update(dt, m_DTLevel > 0 ? m_DTLevel + 2 : 0);
}

void VergilHUD2::TriggerHPShake(float hpLost)
{
	if (hpLost <= 0.0f)
		return;

	// A new hit replaces the running shake, even a bigger one.
	m_HPShakeFrames = (hpLost >= kHPShakeBigHit) ? kHPShakeBigFrames : kHPShakeFrames;
	m_HPShakeRange = (hpLost >= kHPShakeBigHit) ? kHPShakeBigRange
		: (hpLost >= kHPShakeMediumHit) ? kHPShakeMediumRange : kHPShakeSmallRange;
	m_HPShakeStep = 0.0f;

	// The hit frame already counts and rolls.
	m_HPShakeFrames -= 1.0f;
	m_HPShakeOffset = RollShakeOffset(m_HPShakeRng, m_HPShakeRange);
}

void VergilHUD2::SaveSprite(std::vector<SavedSprite>& saved, size_t idx) const
{
	saved.push_back({ idx, m_pSpriteBatch->GetTranslation(idx), m_pSpriteBatch->GetScale(idx),
		m_pSpriteBatch->GetOpacity(idx), m_pSpriteBatch->GetBrightness(idx), m_pSpriteBatch->GetTint(idx) });
}

void VergilHUD2::ApplyHPShake(std::vector<SavedSprite>& saved)
{
	if (m_HPShakeOffset == 0.0f)
		return;

	const float offsetY = m_HPShakeOffset * kHPShakeUnit;
	for (const Group_t group : { Group_t::HPBars, Group_t::DTBars })
	{
		size_t begin, end, anchor;
		if (!GetGroupRange(group, begin, end, anchor))
			continue;

		for (size_t idx = begin; idx < end; idx++)
		{
			SaveSprite(saved, idx);
			const glm::vec3 translation = m_pSpriteBatch->GetTranslation(idx);
			m_pSpriteBatch->TranslateTo(idx, { translation.x, translation.y + offsetY, translation.z });
		}
	}
}

void VergilHUD2::ApplyDTOrbPulse(std::vector<SavedSprite>& saved)
{
	if (!m_DTReady || m_DTLevel == 0)
		return;

	const auto& orbIdxs = m_SpriteIndices.GetDTBarOrbIdxs();
	const auto& glowIdxs = m_SpriteIndices.GetDTBarOrbGlowIdxs();
	// First fade in starts from zero, then the pulse bottoms out at the min opacity.
	const float minOpacity = m_DTPulseTime < kDTPulsePeriod * 0.5f ? 0.0f : kDTPulseMinOpacity;
	const float pulse = minOpacity + (1.0f - minOpacity) * PulseWave(m_DTPulseTime);

	for (size_t i = 0; i < orbIdxs.size() && i < glowIdxs.size(); i++)
	{
		if (m_pSpriteBatch->GetOpacity(orbIdxs[i]) < 1.0f)
			continue;

		SaveSprite(saved, glowIdxs[i]);
		m_pSpriteBatch->SetOpacity(glowIdxs[i], pulse);
	}
}

// Dims the fill from full brightness and back, in the DT glow's rhythm.
void VergilHUD2::ApplyMiragePulse(std::vector<SavedSprite>& saved)
{
	if (!m_MirageReady || !m_MirageGaugeVisible)
		return;

	const size_t idx = m_SpriteIndices.GetMirageFillIdx();
	const float brightness = glm::mix(1.0f, kMiragePulseMinBrightness, PulseWave(m_MiragePulseTime));

	SaveSprite(saved, idx);
	m_pSpriteBatch->SetBrightness(idx, m_pSpriteBatch->GetBrightness(idx) * brightness);
}

bool VergilHUD2::OnDraw()
{
	// Group transforms and pulses only live for this draw, the HP updates keep re-translating sprites from the base layout.
	std::vector<SavedSprite> saved;

	for (size_t g = 0; g < (size_t)Group_t::SIZE; g++)
	{
		const auto& gt = m_GroupTransforms[g];
		if (gt.scale == 1.0f && gt.offset.x == 0.0f && gt.offset.y == 0.0f)
			continue;

		size_t begin, end, anchor;
		if (!GetGroupRange((Group_t)g, begin, end, anchor))
			continue;

		const glm::vec3 pivot = m_pSpriteBatch->GetTranslation(anchor);
		for (size_t idx = begin; idx < end; idx++)
		{
			const glm::vec3 translation = m_pSpriteBatch->GetTranslation(idx);
			const glm::vec3 scale = m_pSpriteBatch->GetScale(idx);
			SaveSprite(saved, idx);

			m_pSpriteBatch->TranslateTo(idx, {
				pivot.x + (translation.x - pivot.x) * gt.scale + gt.offset.x,
				pivot.y + (translation.y - pivot.y) * gt.scale + gt.offset.y,
				translation.z });
			m_pSpriteBatch->ScaleTo(idx, scale * gt.scale);
		}
	}

	ApplyHPShake(saved);
	ApplyDTOrbPulse(saved);
	ApplyDTExplosionBurst(saved);
	ApplyMiragePulse(saved);

	for (size_t g = 0; g < (size_t)Group_t::SIZE; g++)
	{
		size_t begin, end, anchor;
		if (m_GroupVisible[g] || !GetGroupRange((Group_t)g, begin, end, anchor))
			continue;

		for (size_t idx = begin; idx < end; idx++)
		{
			SaveSprite(saved, idx);
			m_pSpriteBatch->SetOpacity(idx, 0.0f);
		}
	}

	// The compass effects go into the target first so the compass draws over them.
	const bool effectsDrawn = RenderCompassEffects();
	const bool result = m_pSpriteBatch->Draw(m_pD3D11DeviceContext, !effectsDrawn);
	// The lightning goes in after the sprites so it sits over the whole HUD.
	RenderDTLightning();
	RenderDTExplosion();

	// Reverse order so sprites touched twice end up back at their first saved state.
	for (auto it = saved.rbegin(); it != saved.rend(); ++it)
	{
		m_pSpriteBatch->TranslateTo(it->idx, it->translation);
		m_pSpriteBatch->ScaleTo(it->idx, it->scale);
		if (m_pSpriteBatch->GetOpacity(it->idx) != it->opacity)
			m_pSpriteBatch->SetOpacity(it->idx, it->opacity);
		if (m_pSpriteBatch->GetBrightness(it->idx) != it->brightness)
			m_pSpriteBatch->SetBrightness(it->idx, it->brightness);
		if (m_pSpriteBatch->GetTint(it->idx) != it->tint)
			m_pSpriteBatch->TintTo(it->idx, it->tint);
	}

	return result;
}

void VergilHUD2::SetGroupTransform(Group_t group, float scale, glm::vec2 offset)
{
	auto& gt = m_GroupTransforms[(size_t)group];
	gt.scale = scale;
	gt.offset = offset;
}

void VergilHUD2::SetGroupVisible(Group_t group, bool visible)
{
	m_GroupVisible[(size_t)group] = visible;
}

// Runs with the draw-time transforms applied, so the effects follow the compass group scale and offset.
bool VergilHUD2::RenderCompassEffects()
{
	if (!m_GroupVisible[(size_t)Group_t::CompassRoyalguard])
		return false;

	ID3D11RenderTargetView* pRTV = m_pSpriteBatch->GetRTV();
	const auto& dims = m_pSpriteBatch->GetDims();
	if (!pRTV || dims.x == 0 || dims.y == 0)
		return false;

	const float groupScale = m_GroupTransforms[(size_t)Group_t::CompassRoyalguard].scale;
	const glm::vec3 compass = m_pSpriteBatch->GetTranslation(m_SpriteIndices.GetCompassIdx());
	const glm::vec2 ring = glm::vec2(compass.x, compass.y) + GetCompassRingOffset() * groupScale;

	bool cleared = false;
	// Void first, the storm draws over it.
	for (CompassEffect* effect : { &m_CompassVoid, &m_CompassStorm })
	{
		auto& layer = effect->layers[GetActiveLayerIdx(*effect)];
		if (!layer || !effect->enabled)
			continue;

		// Sprite translations are NDC of the unscaled HUD, the bounds scale doesn't move them in HUD pixels.
		const glm::vec2 position = {
			ring.x * (float)m_Width * 0.5f + effect->offset.x,
			ring.y * (float)m_Height * 0.5f + effect->offset.y };

		if (!cleared)
		{
			m_pSpriteBatch->Clear(m_pD3D11DeviceContext);
			cleared = true;
		}

		layer->SetTransform(position, effect->scale * groupScale);
		layer->Render(m_pD3D11DeviceContext, pRTV, dims.x, dims.y);
	}

	return cleared;
}

void VergilHUD2::DrawCompassEffectControls()
{
	const auto drawControls = [&](CompassEffect& effect, const char* id) {
		const size_t activeIdx = GetActiveLayerIdx(effect);
		ImGui::PushID(id);
		ImGui::Separator();
		ImGui::TextUnformatted(id);

		for (size_t i = 0; i < effect.layers.size(); i++)
		{
			const auto& layer = effect.layers[i];
			if (!layer)
				continue;

			const std::string name = std::filesystem::path(layer->GetEffectPath()).filename().string();
			const char* state = effect.perStyle && i == activeIdx ? " (active)" : "";
			if (layer->IsLoaded())
				ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "%s: loaded%s", name.c_str(), state);
			else
				ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.3f, 1.0f), "%s: not loaded%s", name.c_str(), state);
		}

		ImGui::Checkbox("Enabled", &effect.enabled);
		ImGui::SameLine();
		if (ImGui::Button("Restart"))
			effect.layers[activeIdx]->Restart();

		ImGui::DragFloat("HUD px per unit", &effect.scale, 0.1f, 0.1f, 200.0f, "%.2f");
		ImGui::DragFloat2("Offset (px, y up)", &effect.offset.x, 0.5f, -1000.0f, 1000.0f, "%.1f");
		if (ImGui::DragFloat("Speed", &effect.speed, 0.01f, 0.0f, 10.0f, "%.2f"))
		{
			for (const auto& layer : effect.layers)
				if (layer)
					layer->SetSpeed(effect.speed);
		}
		if (effect.perStyle)
			ImGui::DragInt("Style switch warm-up (frames)", &effect.warmupFrames, 1.0f, 0, 600);

		ImGui::PopID();
	};

	drawControls(m_CompassVoid, "Compass void");
	drawControls(m_CompassStorm, "Compass storm");
}

void VergilHUD2::SetDTActive(bool active)
{
	m_DTActive = active;
}

// Anchored on the first DT orb and drawn over the sprites.
void VergilHUD2::RenderDTLightning()
{
	if (!m_pDTLightning || !m_pDTLightning->IsPlaying() || !m_GroupVisible[(size_t)Group_t::DTBars])
		return;

	const auto& orbIdxs = m_SpriteIndices.GetDTBarOrbIdxs();
	ID3D11RenderTargetView* pRTV = m_pSpriteBatch->GetRTV();
	const auto& dims = m_pSpriteBatch->GetDims();
	if (orbIdxs.empty() || !pRTV || dims.x == 0 || dims.y == 0)
		return;

	const glm::vec3 orb = m_pSpriteBatch->GetTranslation(orbIdxs.front());
	const glm::vec2 position = {
		orb.x * (float)m_Width * 0.5f + m_DTLightningOffset.x,
		orb.y * (float)m_Height * 0.5f + m_DTLightningOffset.y };
	const float scale = m_DTLightningScale * m_GroupTransforms[(size_t)Group_t::DTBars].scale;

	m_pDTLightning->SetTransform(position, scale);
	m_pDTLightning->Render(m_pD3D11DeviceContext, pRTV, dims.x, dims.y);
}

void VergilHUD2::DrawDTLightningControls()
{
	if (!m_pDTLightning)
		return;

	ImGui::PushID("VergilDTLightning");

	ImGui::TextWrapped("Effect: %s", std::filesystem::path(m_pDTLightning->GetEffectPath()).string().c_str());
	if (m_pDTLightning->IsLoaded())
		ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "Loaded, reloads when the file is saved.");
	else
		ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.3f, 1.0f), "Not loaded, it loads as soon as the file exists.");

	ImGui::Checkbox("Enabled", &m_DTLightningEnabled);
	ImGui::SameLine();
	ImGui::Checkbox("Force on (ignore Devil Trigger)", &m_DTLightningForce);
	ImGui::SameLine();
	if (ImGui::Button("Restart"))
		m_pDTLightning->Restart();

	ImGui::DragFloat("HUD px per unit", &m_DTLightningScale, 0.1f, 0.1f, 200.0f, "%.2f");
	ImGui::DragFloat2("Offset (px, y up)", &m_DTLightningOffset.x, 0.5f, -1000.0f, 1000.0f, "%.1f");
	if (ImGui::DragFloat("Speed", &m_DTLightningSpeed, 0.01f, 0.0f, 10.0f, "%.2f"))
		m_pDTLightning->SetSpeed(m_DTLightningSpeed);

	ImGui::PopID();
}

void VergilHUD2::SetDTExplosionCharge(float charge)
{
	if (m_pDTExplosion)
		m_pDTExplosion->SetCharge(charge);
}

void VergilHUD2::ApplyDTExplosionBurst(std::vector<SavedSprite>& saved)
{
	if (!m_pDTExplosion)
		return;

	const auto& burstIdxs = m_SpriteIndices.GetDTBarOrbBurstIdxs();
	for (size_t i = 0; i < burstIdxs.size(); i++)
	{
		float scale, opacity;
		if (!m_pDTExplosion->GetOrbBurst(i, scale, opacity))
			continue;

		SaveSprite(saved, burstIdxs[i]);
		m_pSpriteBatch->ScaleTo(burstIdxs[i], m_pSpriteBatch->GetScale(burstIdxs[i]) * scale);
		m_pSpriteBatch->SetOpacity(burstIdxs[i], opacity);
	}
}

void VergilHUD2::RenderDTExplosion()
{
	if (!m_pDTExplosion || !m_GroupVisible[(size_t)Group_t::DTBars])
		return;

	std::vector<glm::vec2> orbPositions;
	for (const size_t idx : m_SpriteIndices.GetDTBarOrbIdxs())
	{
		const glm::vec3 orb = m_pSpriteBatch->GetTranslation(idx);
		orbPositions.push_back({ orb.x * (float)m_Width * 0.5f, orb.y * (float)m_Height * 0.5f });
	}

	const auto& dims = m_pSpriteBatch->GetDims();
	m_pDTExplosion->Render(m_pD3D11DeviceContext, m_pSpriteBatch->GetRTV(), dims.x, dims.y,
		orbPositions, m_GroupTransforms[(size_t)Group_t::DTBars].scale);
}

void VergilHUD2::DrawDTExplosionControls()
{
	if (!m_pDTExplosion)
		return;

	ImGui::PushID("VergilDTExplosion");
	m_pDTExplosion->DrawControls();
	ImGui::PopID();
}

void VergilHUD2::SetBoundsScale(float scale)
{
	scale = (std::max)(scale, 0.1f);
	if (scale == m_BoundsScale)
		return;

	const UINT width = (std::max)(1u, (UINT)((float)m_Width * scale + 0.5f));
	const UINT height = (std::max)(1u, (UINT)((float)m_Height * scale + 0.5f));
	if (!m_pSpriteBatch->Resize(m_pD3D11Device, width, height))
		return;

	m_BoundsScale = scale;
	m_pSpriteBatch->SetContentScale(1.0f / scale);
}

// Sprite order in SpriteIndices::UpdateSprites keeps every group contiguous.
bool VergilHUD2::GetGroupRange(Group_t group, size_t& begin, size_t& end, size_t& anchor) const
{
	const size_t count = m_SpriteIndices.GetSpriteIds().size();
	const auto& hpBG = m_SpriteIndices.GetHPBarBGIdxs();
	const auto& hpFrames = m_SpriteIndices.GetHPBarFrameIdxs();
	if (count == 0 || hpBG.empty())
		return false;

	switch (group)
	{
	case Group_t::CompassRoyalguard:
		begin = 0;
		end = m_SpriteIndices.GetStyleExpBarLevelIdx();
		anchor = m_SpriteIndices.GetCompassIdx();
		break;
	case Group_t::ExpBarStyleName:
		begin = m_SpriteIndices.GetStyleExpBarLevelIdx();
		end = m_SpriteIndices.GetDTBarBeginIdx();
		anchor = m_SpriteIndices.GetStyleExpBarFrameIdx();
		break;
	case Group_t::DTBars:
		begin = m_SpriteIndices.GetDTBarBeginIdx();
		end = hpBG.front();
		anchor = begin;
		break;
	case Group_t::HPBars:
		begin = hpBG.front();
		end = count;
		anchor = hpFrames.empty() ? begin : hpFrames.front();
		break;
	default:
		return false;
	}

	return begin < end && end <= count && anchor < count;
}

ID3D11ShaderResourceView* VergilHUD2::GetSRV()
{
	return m_pSpriteBatch->GetRTSRV();
}

const std::string& VergilHUD2::GetLastRenderingErrorMsg()
{
	return m_pSpriteBatch->GetLastErrorMsg();
}

HRESULT VergilHUD2::GetLastRenderingErrorCode()
{
	return m_pSpriteBatch->GetLastErrorCode();
}

void VergilHUD2::SetHPVitalityAmount(float vit)
{
	m_HPFill = glm::clamp(vit, 0.0f, 1.0f);

	UpdateHPBarVitality(m_HPFill);
}

void VergilHUD2::SetHPDamageAmount(float dmg)
{
	m_DMGFill = glm::clamp(dmg, 0.0f, 1.0f);

	UpdateHPBarDamage(m_DMGFill);
}

void VergilHUD2::SetDTFill(float dt)
{
	m_DTFill = glm::clamp(dt, 0.0f, 1.0f);

	UpdateDTBarOrbs(m_DTFill);
}

void VergilHUD2::SetDTReady(bool ready)
{
	if (ready == m_DTReady)
		return;

	// Restart from zero so the glow fades in.
	m_DTReady = ready;
	m_DTPulseTime = 0.0f;
}

void VergilHUD2::SetMirageFill(float fill)
{
	fill = glm::clamp(fill, 0.0f, 1.0f);
	if (fill == m_MirageFill)
		return;

	m_MirageFill = fill;
	UpdateMirageGauge();
}

void VergilHUD2::SetMirageReady(bool ready)
{
	if (ready == m_MirageReady)
		return;

	// Restart at full brightness.
	m_MirageReady = ready;
	m_MiragePulseTime = 0.0f;
}

void VergilHUD2::SetActiveDoppelDelay(DoppelDelay_t delay)
{
	if (delay > DoppelDelay_t::SIZE)
		delay = DoppelDelay_t::SIZE;

	if (delay == m_ActiveDoppelDelay)
		return;

	m_ActiveDoppelDelay = delay;
	UpdateMirageGauge();
}

void VergilHUD2::SetMirageGaugeVisible(bool visible)
{
	if (visible == m_MirageGaugeVisible)
		return;

	m_MirageGaugeVisible = visible;
	UpdateMirageGauge();
}

void VergilHUD2::SetActiveStyle(Style_t style)
{
	if (style >= Style_t::SIZE || style == m_ActiveStyle)
		return;

	m_ActiveStyle = style;
	UpdateSprites();
}

void VergilHUD2::SetActiveStyleLevel(size_t level)
{
	level = glm::clamp(level, (size_t)1, (size_t)4);
	if (level == m_ActiveStyleLevel)
		return;

	m_ActiveStyleLevel = level;
	UpdateSprites();
}

void VergilHUD2::SetStyleLevelVisible(bool visible)
{
	if (visible == m_StyleLevelVisible)
		return;

	m_StyleLevelVisible = visible;
	m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetStyleExpBarLevelIdx(), visible ? 1.0f : 0.0f);
}

void VergilHUD2::SetActiveStyleExpFillAmount(float exp)
{
	m_StyleExpFill = glm::clamp(exp, 0.0f, 1.0f);

	UpdateStyleExpBar(m_StyleExpFill);
}

void VergilHUD2::SetHPLevel(size_t level)
{
	m_HPLevel = (level >= 1 && level <= 20) ? level : 20;

	UpdateSprites();
}

void VergilHUD2::SetDTLevel(size_t level)
{
	m_DTLevel = level <= 8 ? level : 0;

	UpdateSprites();
}

void VergilHUD2::SpriteIndices::UpdateSprites(size_t hpBarLevel, size_t dtBarLevel, Style_t style, size_t styleLevel)
{
	using namespace DC::Sprites;
	using VergilID = HC::Sprites::SpriteID_t;

	m_SpriteIds.clear();
	m_SpriteIds.reserve(64);

	const auto push = [&](size_t spriteId) {
		const size_t idx = m_SpriteIds.size();
		m_SpriteIds.push_back(spriteId);
		return idx;
	};

	// Order matters here, back to front. Compass group first, then EXP, DT and HP.
	m_CompassIdx = push(VergilSprite(VergilID::Compass));

	// Inactive orbs under the active ones, only one active orb shows at a time.
	for (size_t i = 0; i < (size_t)DoppelDelay_t::SIZE; i++)
		m_DoppelOrbInactiveIdxs[i] = push(VergilSprite(HC::Sprites::GetDoppelOrbSpriteID((DoppelDelay_t)i, false)));

	for (size_t i = 0; i < (size_t)DoppelDelay_t::SIZE; i++)
		m_DoppelOrbActiveIdxs[i] = push(VergilSprite(HC::Sprites::GetDoppelOrbSpriteID((DoppelDelay_t)i, true)));

	m_MirageFillIdx = push(VergilSprite(VergilID::Mirage_Fill));
	m_MirageFrameIdx = push(VergilSprite(VergilID::Mirage_Frame));

	m_StyleExpBarLevelIdx = push((size_t)GetStyleExpBarLevelNumberSpriteID(kTheme, styleLevel));
	m_StyleExpBarFrameIdx = push((size_t)GetStyleExpBarFrameSpriteID(kTheme));
	m_StyleExpBarIdx = push(GetStyleExpBarSprite(style));

	// DT bar orbs, each over its empty background
	m_DTBarBeginIdx = m_SpriteIds.size();
	m_DTBarOrbBGIdxs.clear();
	m_DTBarOrbsIdxs.clear();
	m_DTBarFrameIdxs.clear();
	m_DTBarOrbGlowIdxs.clear();
	m_DTBarOrbBurstIdxs.clear();

	if (dtBarLevel > 0)
	{
		for (size_t i = 0; i < dtBarLevel + 2; i++)
			m_DTBarOrbBGIdxs.push_back(push((size_t)GetDTBarOrbBGSpriteID(kTheme)));

		for (size_t i = 0; i < dtBarLevel + 2; i++)
			m_DTBarOrbsIdxs.push_back(push(VergilSprite(VergilID::Orb_DT)));

		for (size_t i = 0; i < dtBarLevel; i++)
			m_DTBarFrameIdxs.push_back(push((size_t)GetDTBarFrameSpriteID(kTheme, i + 1)));

		// Glows go after the frames so they draw over the metal
		for (size_t i = 0; i < dtBarLevel + 2; i++)
			m_DTBarOrbGlowIdxs.push_back(push(VergilSprite(HC::Sprites::GetDTBarOrbGlowSpriteID(i))));

		// Orb copies for the DT Explosion release burst, hidden otherwise
		for (size_t i = 0; i < dtBarLevel + 2; i++)
			m_DTBarOrbBurstIdxs.push_back(push(VergilSprite(VergilID::Orb_DT)));
	}

	// HP bars, a second one past level 10
	const size_t hpBars = hpBarLevel > 10 ? 2 : 1;

	m_HPBarBGIdxs.clear();
	for (size_t i = 0; i < hpBars; i++)
		m_HPBarBGIdxs.push_back(push((size_t)GetHPBarBGSpriteID(kTheme)));

	m_HPBarDamageIdxs.clear();
	for (size_t i = 0; i < hpBars; i++)
		m_HPBarDamageIdxs.push_back(push((size_t)GetHPBarDamageSpriteID(kTheme)));

	m_HPBarVitalityIdxs.clear();
	for (size_t i = 0; i < hpBars; i++)
		m_HPBarVitalityIdxs.push_back(push((size_t)GetHPBarVitalitySpriteID(kTheme)));

	m_VitalityEndGlowIdx = push((size_t)GetHPBarVitalityTailGlowSpriteID(kTheme, kStraightHP));

	m_HPBarFrameIdxs.clear();
	for (size_t i = 0; i < hpBarLevel; i++)
		m_HPBarFrameIdxs.push_back(push((size_t)GetHPBarFrameSpriteID(kTheme, GetHPFrameSpriteVarient(i, hpBarLevel), kStraightHP)));
}

void VergilHUD2::SetSpritePositions()
{
	using namespace DC::Transforms;

	const auto place = [&](size_t idx, glm::vec2 position, float scale) {
		m_pSpriteBatch->SetTransform(idx, { position.x, position.y, 0.0f }, glm::f32vec3(0.0f), glm::f32vec3(scale));
	};

	// Compass and Mirage gauge
	place(m_SpriteIndices.GetCompassIdx(), HC::Transforms::COMPASS.Position, HC::Transforms::COMPASS.Scale);
	place(m_SpriteIndices.GetMirageFillIdx(), HC::Transforms::MIRAGE_FILL.Position, HC::Transforms::MIRAGE_FILL.Scale);
	place(m_SpriteIndices.GetMirageFrameIdx(), HC::Transforms::MIRAGE_FRAME.Position, HC::Transforms::MIRAGE_FRAME.Scale);

	for (size_t i = 0; i < (size_t)DoppelDelay_t::SIZE; i++)
	{
		for (const bool active : { false, true })
		{
			const auto& transform = HC::Transforms::GetDoppelOrb((DoppelDelay_t)i, active);
			place(m_SpriteIndices.GetDoppelOrbIdx((DoppelDelay_t)i, active), transform.Position, transform.Scale);
		}
	}

	const BarPositionID_t dtBarPosition = m_HPLevel < 11 ? BarPositionID_t::Top : BarPositionID_t::Bottom;

	// Style EXP bar, every fill uses Trickster's placement on the colored theme
	{
		const auto& level = GetStyleExpBarLevelDigit(kTheme, m_ActiveStyleLevel);
		const auto& frame = GetStyleExpBarFrame(kTheme);
		const auto& bar = GetStyleExpBar(kTheme, m_ActiveStyle);

		place(m_SpriteIndices.GetStyleExpBarLevelIdx(), { level.Position.x, level.Position.y }, level.Scale);
		place(m_SpriteIndices.GetStyleExpBarFrameIdx(), { frame.Position.x, frame.Position.y }, frame.Scale);
		place(m_SpriteIndices.GetStyleExpBarIdx(), { bar.Position.x, bar.Position.y }, bar.Scale);
		m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetStyleExpBarLevelIdx(), m_StyleLevelVisible ? 1.0f : 0.0f);
	}

	// DT bar orbs
	if (m_DTLevel > 0)
	{
		const auto& bgIdxs = m_SpriteIndices.GetDTBarOrbBGIdxs();
		const auto& orbIdxs = m_SpriteIndices.GetDTBarOrbIdxs();
		const auto& glowIdxs = m_SpriteIndices.GetDTBarOrbGlowIdxs();
		const auto& burstIdxs = m_SpriteIndices.GetDTBarOrbBurstIdxs();

		for (size_t i = 0; i < m_DTLevel + 2; i++)
		{
			const auto& transform = GetDTBarOrb(kTheme, dtBarPosition, i + 1);
			const glm::vec2 position = { transform.Position.x, transform.Position.y };

			place(bgIdxs[i], position, transform.Scale);
			place(orbIdxs[i], position, transform.Scale);
			place(burstIdxs[i], position, transform.Scale);
			place(glowIdxs[i], position, transform.Scale * HC::Sprites::GetDTBarOrbGlowScale(i));
		}
	}

	// DT bar frames
	{
		const auto& idxs = m_SpriteIndices.GetDTBarFrameIdxs();
		for (size_t i = 0; i < m_DTLevel; i++)
		{
			const auto& transform = GetDTBarFrame(kTheme, dtBarPosition, (DTBarFrameID_t)i);
			place(idxs[i], { transform.Position.x, transform.Position.y }, transform.Scale);
		}
	}

	// HP bar bg, damage and vitality, top then bottom bar
	{
		const auto placeBars = [&](const std::vector<size_t>& idxs, HPBarVarient_t varient) {
			for (size_t i = 0; i < idxs.size(); i++)
			{
				const auto& transform = GetHPBar(kTheme, i == 0 ? BarPositionID_t::Top : BarPositionID_t::Bottom, varient);
				place(idxs[i], { transform.Position.x, transform.Position.y }, transform.Scale);
			}
		};

		placeBars(m_SpriteIndices.GetHPBarBGIdxs(), HPBarVarient_t::Background);
		placeBars(m_SpriteIndices.GetHPBarDamageIdxs(), HPBarVarient_t::Damage);
		placeBars(m_SpriteIndices.GetHPBarVitalityIdxs(), HPBarVarient_t::Vitality);
	}

	// HP bar vitality tail glow
	{
		const BarPositionID_t position = m_HPLevel < 11 ? BarPositionID_t::Top : BarPositionID_t::Bottom;
		const size_t level = m_HPLevel < 11 ? m_HPLevel : m_HPLevel - 10;
		const auto& glow = GetVitalityBarGlow(kTheme, position, GlowPos_t::Head);

		place(m_SpriteIndices.GetHPBarVitalityEndGlowIdx(),
			{ GetVitalityGlowX(position, level, 1.0f, kStraightHP), glow.Position.y }, glow.Scale);
	}

	// HP bar frames
	{
		const auto& idxs = m_SpriteIndices.GetHPBarFrameIdxs();
		for (size_t i = 0; i < m_HPLevel; i++)
		{
			const auto& transform = GetHPBarFrame(kTheme, i + 1, GetHPFrameTransformVarient(i, m_HPLevel));
			place(idxs[i], { transform.Position.x, transform.Position.y }, transform.Scale);
		}
	}
}

void VergilHUD2::SetSpriteMasks()
{
	const auto barFraction = [&](size_t bar) {
		return bar == 0 ? (m_HPLevel >= 10 ? 1.0f : m_HPLevel * 0.1f) : (m_HPLevel - 10) * 0.1f;
	};

	const auto& bgIdxs = m_SpriteIndices.GetHPBarBGIdxs();
	for (size_t i = 0; i < bgIdxs.size(); i++)
		m_pSpriteBatch->SetSpriteMasks(bgIdxs[i], DC::Masks::GetHPBarBackgroundMasks(barFraction(i), kStraightHP));

	const auto& dmgIdxs = m_SpriteIndices.GetHPBarDamageIdxs();
	for (size_t i = 0; i < dmgIdxs.size(); i++)
		m_pSpriteBatch->SetSpriteMasks(dmgIdxs[i], DC::Masks::GetHPBarDamageMasks(barFraction(i), kStraightHP));

	const auto& vitIdxs = m_SpriteIndices.GetHPBarVitalityIdxs();
	for (size_t i = 0; i < vitIdxs.size(); i++)
		m_pSpriteBatch->SetSpriteMasks(vitIdxs[i], DC::Masks::GetHPBarVitalityMasks(barFraction(i), kStraightHP));

	m_pSpriteBatch->SetSpriteMasks(m_SpriteIndices.GetHPBarVitalityEndGlowIdx(), DC::Masks::GetHPBarVitalityEndGlowMasks(kStraightHP));
	m_pSpriteBatch->SetSpriteMasks(m_SpriteIndices.GetMirageFillIdx(), { HC::Masks::MIRAGE_FILL });
	m_pSpriteBatch->SetSpriteMasks(m_SpriteIndices.GetStyleExpBarIdx(), DC::Masks::GetStyleExpBarMasks());

	UpdateHPBarVitality(m_HPFill);
	UpdateHPBarDamage(m_DMGFill);
	UpdateStyleExpBar(m_StyleExpFill);
}

void VergilHUD2::UpdateStyleExpBar(float fraction)
{
	auto mask = DC::Masks::GetMaskInfo(DC::Masks::MaskInfoID_t::StyleExpBarSlantedTail);
	mask.Slanted.Origin.x = glm::mix(0.0f, mask.Slanted.Origin.x, fraction);

	m_pSpriteBatch->UpdateSpriteMask(m_SpriteIndices.GetStyleExpBarIdx(), 0, mask);
}

void VergilHUD2::UpdateHPBarVitality(float fraction)
{
	using namespace DC::Transforms;
	using namespace DC::Masks;

	const auto& vitalIdxs = m_SpriteIndices.GetHPBarVitalityIdxs();
	const auto  headMask = GetMaskInfo(MaskInfoID_t::HPBarVitalitySlantedHead);
	const auto  tailMask = GetMaskInfo(MaskInfoID_t::HPBarVitalitySlantedTail);
	const float fullSpan = tailMask.Slanted.Origin.x - headMask.Slanted.Origin.x;

	const float glowMaskRangeX = DC::Processed::GetGlowMaskRangeX(kStraightHP);
	const size_t glowIdx = m_SpriteIndices.GetHPBarVitalityEndGlowIdx();

	const auto setBarFill = [&](size_t bar, float fill) {
		auto tailMaskInfo = m_pSpriteBatch->GetSpriteMaskInfo(vitalIdxs[bar], 1);
		tailMaskInfo.Slanted.Origin.x = headMask.Slanted.Origin.x + fill;
		m_pSpriteBatch->UpdateSpriteMask(vitalIdxs[bar], 1, tailMaskInfo);
	};

	const auto placeGlow = [&](BarPositionID_t position, size_t level, float barFraction, float fill) {
		const float x = GetVitalityGlowX(position, level, barFraction, kStraightHP);
		const float y = GetVitalityBarGlow(kTheme, position, GlowPos_t::Head).Position.y;
		auto maskInfo = m_pSpriteBatch->GetSpriteMaskInfo(glowIdx, 0);

		maskInfo.Slanted.Origin.x = glowMaskRangeX - fill;

		m_pSpriteBatch->TranslateTo(glowIdx, { x, y, 0.0f });
		m_pSpriteBatch->UpdateSpriteMask(glowIdx, 0, maskInfo);
	};

	if (m_HPLevel > 10)
	{
		const size_t bottomBarLevel = m_HPLevel - 10;
		const float fullSpanBottomBar = bottomBarLevel * 0.1f * fullSpan;
		const float fullSpanWhole = fullSpan + fullSpanBottomBar;
		const float currentFill = fraction * fullSpanWhole;
		const float bottomFill = currentFill - fullSpan;

		setBarFill(0, (std::min)(currentFill, fullSpan));
		setBarFill(1, (std::max)(bottomFill, 0.0f));

		if (bottomFill > 0.0f)
			placeGlow(BarPositionID_t::Bottom, bottomBarLevel, bottomFill / fullSpanBottomBar, bottomFill);
		else
			placeGlow(BarPositionID_t::Top, 10, currentFill / fullSpan, currentFill);

		// No glow at the end of a full bar
		m_pSpriteBatch->SetOpacity(glowIdx, Graphics::EpsilonEqual(currentFill, fullSpanWhole) ? 0.0f : 1.0f);
	}
	else
	{
		const float fullSpanBar = m_HPLevel * 0.1f * fullSpan;
		const float currentFill = fraction * fullSpanBar;

		setBarFill(0, currentFill);
		placeGlow(BarPositionID_t::Top, m_HPLevel, currentFill / fullSpanBar, currentFill);

		m_pSpriteBatch->SetOpacity(glowIdx, Graphics::EpsilonEqual(currentFill, fullSpanBar) ? 0.0f : 1.0f);
	}
}

void VergilHUD2::UpdateHPBarDamage(float fraction)
{
	using namespace DC::Masks;

	const auto& dmgIdxs = m_SpriteIndices.GetHPBarDamageIdxs();
	const auto  headMask = GetMaskInfo(MaskInfoID_t::HPBarDamageSlantedHead);
	const auto  tailMask = GetMaskInfo(MaskInfoID_t::HPBarDamageSlantedTail);
	const float fullSpan = tailMask.Slanted.Origin.x - headMask.Slanted.Origin.x;

	const auto setBarFill = [&](size_t bar, float fill) {
		auto tailMaskInfo = m_pSpriteBatch->GetSpriteMaskInfo(dmgIdxs[bar], 1);
		tailMaskInfo.Slanted.Origin.x = headMask.Slanted.Origin.x + fill;
		m_pSpriteBatch->UpdateSpriteMask(dmgIdxs[bar], 1, tailMaskInfo);
	};

	if (m_HPLevel > 10)
	{
		const float fullSpanWhole = fullSpan + (m_HPLevel - 10) * 0.1f * fullSpan;
		const float currentFill = fraction * fullSpanWhole;

		setBarFill(0, (std::min)(currentFill, fullSpan));
		setBarFill(1, (std::max)(currentFill - fullSpan, 0.0f));
	}
	else
	{
		setBarFill(0, fraction * m_HPLevel * 0.1f * fullSpan);
	}
}

void VergilHUD2::UpdateDTBarOrbs(float fraction)
{
	if (m_DTLevel == 0)
		return;

	const auto& orbIdxs = m_SpriteIndices.GetDTBarOrbIdxs();
	const float eachOrbShare = 1.0f / (m_DTLevel + 2);
	const size_t numFullOrbs = (std::min)((size_t)(fraction / eachOrbShare), orbIdxs.size());

	for (const auto& idx : orbIdxs)
		m_pSpriteBatch->SetOpacity(idx, 0.0f);

	// Glows only show through ApplyDTOrbPulse
	for (const auto& idx : m_SpriteIndices.GetDTBarOrbGlowIdxs())
		m_pSpriteBatch->SetOpacity(idx, 0.0f);

	// Bursts only show through ApplyDTExplosionBurst
	for (const auto& idx : m_SpriteIndices.GetDTBarOrbBurstIdxs())
		m_pSpriteBatch->SetOpacity(idx, 0.0f);

	for (size_t i = 0; i < numFullOrbs; i++)
		m_pSpriteBatch->SetOpacity(orbIdxs[i], 1.0f);

	if (numFullOrbs < orbIdxs.size())
		m_pSpriteBatch->SetOpacity(orbIdxs[numFullOrbs], glm::mod(fraction, eachOrbShare) / eachOrbShare);
}

void VergilHUD2::UpdateMirageGauge()
{
	const size_t fillIdx = m_SpriteIndices.GetMirageFillIdx();

	auto mask = HC::Masks::MIRAGE_FILL;
	mask.Radial.EndDeg = glm::mix(mask.Radial.StartDeg, mask.Radial.EndDeg, m_MirageFill);
	m_pSpriteBatch->UpdateSpriteMask(fillIdx, 0, mask);

	// An empty arc still leaves a one pixel line on its edge.
	m_pSpriteBatch->SetOpacity(fillIdx, (m_MirageGaugeVisible && m_MirageFill > 0.0f) ? 1.0f : 0.0f);
	m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetMirageFrameIdx(), m_MirageGaugeVisible ? 1.0f : 0.0f);

	for (size_t i = 0; i < (size_t)DoppelDelay_t::SIZE; i++)
	{
		const bool active = (DoppelDelay_t)i == m_ActiveDoppelDelay;
		m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetDoppelOrbIdx((DoppelDelay_t)i, true), active ? 1.0f : 0.0f);
		m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetDoppelOrbIdx((DoppelDelay_t)i, false), active ? 0.0f : 1.0f);
	}
}

void VergilHUD2::UpdateSprites()
{
	m_SpriteIndices.UpdateSprites(m_HPLevel, m_DTLevel, m_ActiveStyle, m_ActiveStyleLevel);

	m_pSpriteBatch->ChangeSprites(m_pD3D11Device, m_SpriteDescs, m_SpriteIndices.GetSpriteIds(), s_pAtlasTexture);

	SetSpritePositions();
	SetSpriteMasks();
	UpdateDTBarOrbs(m_DTFill);
	UpdateMirageGauge();
}

void VergilHUD2::DrawDebugControls()
{
	ImGui::PushID("VergilHUD2Debug");

	{
		int hpLevel = (int)m_HPLevel;
		if (ImGui::SliderInt("HP level", &hpLevel, 1, 20))
			SetHPLevel((size_t)hpLevel);

		int dtLevel = (int)m_DTLevel;
		if (ImGui::SliderInt("DT level", &dtLevel, 0, 8))
			SetDTLevel((size_t)dtLevel);

		float hp = m_HPFill;
		if (ImGui::SliderFloat("HP", &hp, 0.0f, 1.0f))
			SetHPVitalityAmount(hp);

		float dmg = m_DMGFill;
		if (ImGui::SliderFloat("Damage", &dmg, 0.0f, 1.0f))
			SetHPDamageAmount(dmg);

		float dt = m_DTFill;
		if (ImGui::SliderFloat("DT", &dt, 0.0f, 1.0f))
			SetDTFill(dt);

		float mirage = m_MirageFill;
		if (ImGui::SliderFloat("Mirage", &mirage, 0.0f, 1.0f))
			SetMirageFill(mirage);

		bool mirageReady = m_MirageReady;
		if (ImGui::Checkbox("Mirage ready", &mirageReady))
			SetMirageReady(mirageReady);

		constexpr const char* delayNames[] = { "Top (medium)", "Left (max)", "Right (haste)", "None" };
		int delay = (int)m_ActiveDoppelDelay;
		if (ImGui::Combo("Active doppel delay", &delay, delayNames, IM_ARRAYSIZE(delayNames)))
			SetActiveDoppelDelay((DoppelDelay_t)delay);

		constexpr const char* styleNames[] = { "Darkslayer", "Quicksilver", "Doppelganger" };
		int style = m_ActiveStyle == Style_t::Quicksilver ? 1 : m_ActiveStyle == Style_t::Doppleganger ? 2 : 0;
		if (ImGui::Combo("Style", &style, styleNames, IM_ARRAYSIZE(styleNames)))
			SetActiveStyle(style == 1 ? Style_t::Quicksilver : style == 2 ? Style_t::Doppleganger : Style_t::Darkslayer);

		int styleLevel = (int)m_ActiveStyleLevel;
		if (ImGui::SliderInt("Style level", &styleLevel, 1, 4))
			SetActiveStyleLevel((size_t)styleLevel);

		bool levelVisible = m_StyleLevelVisible;
		if (ImGui::Checkbox("Style level visible", &levelVisible))
			SetStyleLevelVisible(levelVisible);

		float exp = m_StyleExpFill;
		if (ImGui::SliderFloat("Style EXP", &exp, 0.0f, 1.0f))
			SetActiveStyleExpFillAmount(exp);
	}

	ImGui::Separator();
	ImGui::TextUnformatted("Sprite");
	{
		static int selected = 0;
		const int count = (int)m_SpriteIndices.GetSpriteIds().size();
		selected = glm::clamp(selected, 0, (std::max)(count - 1, 0));
		ImGui::SliderInt("Sprite index", &selected, 0, (std::max)(count - 1, 0));

		if (count > 0)
		{
			const size_t spriteId = m_SpriteIndices.GetSpriteIds()[selected];
			const char* name = spriteId >= (size_t)DC::Sprites::SpriteID_t::SIZE
				? HC::Sprites::GetTextureNameFromSpriteID((HC::Sprites::SpriteID_t)(spriteId - (size_t)DC::Sprites::SpriteID_t::SIZE))
				: DC::Sprites::GetTextureNameFromSpriteID((DC::Sprites::SpriteID_t)spriteId);
			ImGui::Text("%s", name);

			glm::vec3 translation = m_pSpriteBatch->GetTranslation(selected);
			if (ImGui::DragFloat2("Position XY", &translation.x, 0.001f, -1.5f, 1.5f))
				m_pSpriteBatch->TranslateTo(selected, translation);

			glm::vec3 scale = m_pSpriteBatch->GetScale(selected);
			if (ImGui::DragFloat("Scale XY", &scale.x, 0.001f, 0.0f, 10.0f))
				m_pSpriteBatch->ScaleTo(selected, glm::vec3(scale.x, scale.x, scale.z));
		}
	}

	ImGui::PopID();
}

std::string VergilHUD2::DumpSpriteLayout()
{
	using MaskType_t = Graphics::BatchedSprites::MaskType_t;

	std::string out = std::format("// HP level {}, DT level {}\n", m_HPLevel, m_DTLevel);
	out += std::format("// Fills: HP {:.4f}f, DMG {:.4f}f, DT {:.4f}f, Mirage {:.4f}f, Style EXP {:.4f}f\n", m_HPFill, m_DMGFill, m_DTFill, m_MirageFill, m_StyleExpFill);
	out += "// { sprite, index, { posX, posY }, scale }\n";

	const auto& spriteIds = m_SpriteIndices.GetSpriteIds();
	for (size_t i = 0; i < spriteIds.size(); i++)
	{
		const char* name = spriteIds[i] >= (size_t)DC::Sprites::SpriteID_t::SIZE
			? HC::Sprites::GetTextureNameFromSpriteID((HC::Sprites::SpriteID_t)(spriteIds[i] - (size_t)DC::Sprites::SpriteID_t::SIZE))
			: DC::Sprites::GetTextureNameFromSpriteID((DC::Sprites::SpriteID_t)spriteIds[i]);
		const auto& t = m_pSpriteBatch->GetTranslation(i);
		const auto& s = m_pSpriteBatch->GetScale(i);
		out += std::format("{{ \"{}\", {}, {{ {:.4f}f, {:.4f}f }}, {:.4f}f }},\n", name, i, t.x, t.y, s.x);

		const auto masks = m_pSpriteBatch->GetSpriteMasksInfo(i);
		for (size_t m = 0; m < masks.size(); m++)
		{
			const auto& mask = masks[m];
			if (mask.Type == MaskType_t::SlantedMask)
			{
				out += std::format("\t// mask {} slanted: origin {{ {:.4f}f, {:.4f}f }}, normal {:.2f}f, softness {{ {:.4f}f, {:.4f}f }}\n",
					m, mask.Slanted.Origin.x, mask.Slanted.Origin.y, mask.Slanted.LineNormalDeg,
					mask.Slanted.SoftnessRange.x, mask.Slanted.SoftnessRange.y);
			}
			else if (mask.Type == MaskType_t::RadialMask)
			{
				out += std::format("\t// mask {} radial: origin {{ {:.4f}f, {:.4f}f }}, radius {:.4f}f, start {:.2f}f, end {:.2f}f, keepPositive {}, flip {}\n",
					m, mask.Radial.Origin.x, mask.Radial.Origin.y, mask.Radial.Radius, mask.Radial.StartDeg,
					mask.Radial.EndDeg, mask.Radial.KeepPositive, mask.Radial.FlipMask);
			}
		}
	}
	return out;
}

}

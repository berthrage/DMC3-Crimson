#include "DanteHUD2.hpp"

#include <format>
#include <algorithm>
#include <cmath>

#include "DanteHUD2HardcodedData.hpp"
#include "DTExplosionHUD2.hpp"
#include "EffekseerLayer.hpp"
#include "imgui.h"

//#include "HUDDebug.hpp"

namespace HUD::Dante {

std::shared_ptr<Graphics::Texture2DArrayD3D11> DanteHUD2::s_pAtlasTexture;

constexpr const char* kAtlasPath = R"(Crimson\assets\crimsonhud\hud2_atlases\atlas1.png)";

namespace {

constexpr const wchar_t* kCompassSmokePath = LR"(Crimson\vfx\hud2\compass_smoke.efkefc)";
constexpr const wchar_t* kDTLightningPath = LR"(Crimson\vfx\hud2\dt_lightning_dante.efkefc)";

// The smoke is authored pointing down (Royalguard), turn it toward the active style's side of the compass.
float GetCompassSmokeRotation(Style_t style)
{
	constexpr float kQuarterTurn = 1.57079633f;
	switch (style)
	{
	case Style_t::Swordmaster:	return kQuarterTurn;
	case Style_t::Trickster:	return kQuarterTurn * 2.0f;
	case Style_t::Gunslinger:
	case Style_t::Doppleganger:	return -kQuarterTurn;
	default:					return 0.0f;
	}
}

struct AnimKey { float time; float value; };

// Smoothstep between keys.
template <size_t N>
float SampleKeys(const std::array<AnimKey, N>& keys, float t)
{
	if (t <= keys.front().time)
		return keys.front().value;

	for (size_t i = 1; i < N; i++)
	{
		if (t < keys[i].time)
		{
			const float k = (t - keys[i - 1].time) / (keys[i].time - keys[i - 1].time);
			return glm::mix(keys[i - 1].value, keys[i].value, k * k * (3.0f - 2.0f * k));
		}
	}

	return keys.back().value;
}

// Style switch timings, in seconds.
constexpr float kSwitchDuration = 0.25f;
constexpr float kCenterFlashDuration = 0.25f;
constexpr float kExitDuration = 0.09f;
constexpr float kExitFadeDuration = 0.05f;
constexpr float kExitMaxBrightness = 2.0f;

// Main styles pop along their slot's outward axis, in multiples of kPopAmplitude (sprite space, relative to the RT height).
constexpr float kPopAmplitude = 0.032f;
constexpr std::array<AnimKey, 3> kPopKeys{ { { 0.0f, -1.5f }, { 0.045f, 1.0f }, { 0.11f, 0.0f } } };

// Secondary styles fade and grow in place instead.
constexpr std::array<AnimKey, 2> kSecondaryFadeKeys{ { { 0.0f, 0.0f }, { 0.12f, 1.0f } } };
constexpr std::array<AnimKey, 3> kSecondaryScaleKeys{ { { 0.0f, 0.9f }, { 0.12f, 1.07f }, { 0.16f, 1.0f } } };

// Initials that change slot.
constexpr std::array<AnimKey, 2> kSlotFadeKeys{ { { 0.0f, 0.0f }, { 0.12f, 1.0f } } };

// Whole compass shake on switch: decaying jitter, amplitude in sprite space relative to the RT height.
constexpr float kCompassShakeDuration = 0.2f;
constexpr float kCompassShakeAmplitude = 0.01f;
constexpr float kCompassShakeFrequency = 30.0f;

// Cockpit hit shake tiers from CUIDCockpit00_UpdateHP_sub_14027D3F0: frames and jitter range by HP lost.
constexpr float kHPShakeBigHit = 1000.0f;
constexpr float kHPShakeMediumHit = 50.0f;
constexpr float kHPShakeBigFrames = 15.0f;
constexpr float kHPShakeFrames = 5.0f;
constexpr int kHPShakeBigRange = 10;
constexpr int kHPShakeMediumRange = 5;
constexpr int kHPShakeSmallRange = 3;
// One cockpit unit (1/448 of the screen height) in sprite space, for the 215 high RT shown at 1080p.
constexpr float kHPShakeUnit = 2.0f * 1080.0f / (448.0f * 215.0f);

// Full fade in and out of the ready DT orb glow, in seconds.
constexpr float kDTPulsePeriod = 0.9f;
constexpr float kDTPulseMinOpacity = 0.2f;

uint32_t NextShakeRandom(uint32_t& state)
{
	state ^= state << 13;
	state ^= state >> 17;
	state ^= state << 5;
	return state;
}

// Random sign, then rand % range, both rolled like the cockpit does.
float RollShakeOffset(uint32_t& state, int range)
{
	const bool up = NextShakeRandom(state) & 1;
	const float amount = (float)(NextShakeRandom(state) % (uint32_t)(std::max)(range, 1));
	return up ? amount : -amount;
}

// Center piece tint at the start of the flash, in Style_t order.
const glm::vec3 kCenterFlashTints[(size_t)Style_t::SIZE] = {
	{ 2.2f,  2.08f, 1.03f }, // Trickster
	{ 2.2f,  0.85f, 1.0f  }, // Swordmaster
	{ 1.35f, 2.2f,  1.16f }, // Royalguard
	{ 1.0f,  1.94f, 2.2f  }, // Gunslinger
	{ 1.5f,  1.34f, 2.05f }, // Darkslayer
	{ 2.06f, 1.05f, 2.2f  }, // Quicksilver
	{ 2.2f,  1.38f, 1.01f }, // Doppleganger
};

bool IsSecondaryStyle(Style_t style)
{
	return style == Style_t::Darkslayer || style == Style_t::Quicksilver || style == Style_t::Doppleganger;
}

CompassSlot_t GetStyleSlot(Style_t style)
{
	switch (style)
	{
	case Style_t::Swordmaster:
	case Style_t::Darkslayer:
		return CompassSlot_t::Right;
	case Style_t::Royalguard:
	case Style_t::Quicksilver:
		return CompassSlot_t::Bottom;
	case Style_t::Gunslinger:
	case Style_t::Doppleganger:
		return CompassSlot_t::Left;
	default:
		return CompassSlot_t::Top;
	}
}

// Style whose initials sit in a slot, the active style always takes its side's main slot.
Style_t GetSlotStyle(CompassSlot_t slot, Style_t activeStyle)
{
	switch (slot)
	{
	case CompassSlot_t::Right:			 return activeStyle == Style_t::Darkslayer ? Style_t::Darkslayer : Style_t::Swordmaster;
	case CompassSlot_t::RightSecondary:	 return activeStyle == Style_t::Darkslayer ? Style_t::Swordmaster : Style_t::Darkslayer;
	case CompassSlot_t::Bottom:			 return activeStyle == Style_t::Quicksilver ? Style_t::Quicksilver : Style_t::Royalguard;
	case CompassSlot_t::BottomSecondary: return activeStyle == Style_t::Quicksilver ? Style_t::Royalguard : Style_t::Quicksilver;
	case CompassSlot_t::Left:			 return activeStyle == Style_t::Doppleganger ? Style_t::Doppleganger : Style_t::Gunslinger;
	case CompassSlot_t::LeftSecondary:	 return activeStyle == Style_t::Doppleganger ? Style_t::Gunslinger : Style_t::Doppleganger;
	default:							 return Style_t::Trickster;
	}
}

glm::vec2 GetSlotOutwardDir(CompassSlot_t slot)
{
	switch (slot)
	{
	case CompassSlot_t::Right:	return {  1.0f,  0.0f };
	case CompassSlot_t::Bottom: return {  0.0f, -1.0f };
	case CompassSlot_t::Left:	return { -1.0f,  0.0f };
	default:					return {  0.0f,  1.0f };
	}
}

}

void DanteHUD2::PreloadTextures(ID3D11Device* pD3D11Device)
{
	if (!s_pAtlasTexture) {
		s_pAtlasTexture = std::make_shared<Graphics::Texture2DArrayD3D11>(kAtlasPath, pD3D11Device);
	}
}

DanteHUD2::DanteHUD2(ID3D11Device* pD3D11Device, ID3D11DeviceContext* pD3D11DeviceContext, UINT width, UINT height)
	: m_pD3D11Device(pD3D11Device), m_pD3D11DeviceContext(pD3D11DeviceContext), m_Width(width), m_Height(height)
{
	PreloadTextures(pD3D11Device);

	if (m_SpriteDescs.size() == 0) {
		HC::Sprites::LoadSpriteDescs(m_SpriteDescs, kAtlasPath);
	}

	m_SpriteIndices.UpdateSprites(m_ThemeID, m_ActiveStyle, m_ActiveStyleLevel,
								  m_HPLevel, m_DTLevel, m_RoyalGuardGaugeLevel, m_StraightHPBars);

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

	m_pCompassSmoke = std::make_unique<Graphics::EffekseerLayer>(m_pD3D11Device, m_pD3D11DeviceContext);
	m_pCompassSmoke->SetEffectPath(kCompassSmokePath);
	m_pCompassSmoke->SetPlaying(true);

	m_pDTLightning = std::make_unique<Graphics::EffekseerLayer>(m_pD3D11Device, m_pD3D11DeviceContext);
	m_pDTLightning->SetEffectPath(kDTLightningPath);

	m_pDTExplosion = std::make_unique<HUD::DTExplosionHUD2>(m_pD3D11Device, m_pD3D11DeviceContext);
}

DanteHUD2::~DanteHUD2()
{
}

void DanteHUD2::OnUpdate(double ts)
{
	const float dt = (float)ts;

	if (m_StyleSwitchActive)
	{
		m_StyleSwitchTime += dt;
		if (m_StyleSwitchTime >= kSwitchDuration)
			m_StyleSwitchActive = false;
	}

	if (m_StyleExitPending)
	{
		m_StyleExitTime += dt;
		if (m_StyleExitTime >= kExitDuration)
		{
			m_StyleExitPending = false;
			CommitActiveStyle(m_PendingStyle);
		}
	}

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

	if (m_pCompassSmoke && m_CompassSmokeEnabled)
	{
		// Follow the style being switched to right away, not after the compass animation commits it.
		const float rotation = GetCompassSmokeRotation(m_StyleExitPending ? m_PendingStyle : m_ActiveStyle);
		if (!m_CompassSmokeRotationSet || rotation != m_CompassSmokeRotation)
		{
			m_pCompassSmoke->SetRotation(rotation);
			// Old particles keep their spawn direction, so restart it already developed in the new one.
			if (m_CompassSmokeRotationSet)
				m_pCompassSmoke->Restart(m_CompassSmokeWarmupFrames);
			m_CompassSmokeRotation = rotation;
			m_CompassSmokeRotationSet = true;
		}
		m_pCompassSmoke->Update(dt);
	}

	if (m_pDTLightning)
	{
		m_pDTLightning->SetPlaying(m_DTLightningEnabled && (m_DTActive || m_DTLightningForce) && m_DTLevel > 0);
		m_pDTLightning->Update(dt);
	}

	if (m_pDTExplosion)
		m_pDTExplosion->Update(dt, m_DTLevel > 0 ? m_DTLevel + 2 : 0);
}

void DanteHUD2::TriggerHPShake(float hpLost)
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

void DanteHUD2::ApplyHPShake(std::vector<SavedSprite>& saved)
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
			const glm::vec3 translation = m_pSpriteBatch->GetTranslation(idx);
			saved.push_back({ idx, translation, m_pSpriteBatch->GetScale(idx),
				m_pSpriteBatch->GetOpacity(idx), m_pSpriteBatch->GetBrightness(idx), m_pSpriteBatch->GetTint(idx) });
			m_pSpriteBatch->TranslateTo(idx, { translation.x, translation.y + offsetY, translation.z });
		}
	}
}

void DanteHUD2::ApplyCompassShake(std::vector<SavedSprite>& saved)
{
	if (!m_StyleSwitchActive || m_StyleSwitchTime >= kCompassShakeDuration)
		return;

	size_t begin, end, anchor;
	if (!GetGroupRange(Group_t::CompassRoyalguard, begin, end, anchor))
		return;

	const auto& dims = m_pSpriteBatch->GetDims();
	const float aspectFix = (dims.x > 0) ? (float)dims.y / (float)dims.x : 1.0f;
	const float decay = 1.0f - m_StyleSwitchTime / kCompassShakeDuration;
	const float phase = 6.28318531f * kCompassShakeFrequency * m_StyleSwitchTime;
	const float amount = kCompassShakeAmplitude * decay * decay;
	// Off-ratio frequencies so it rattles instead of tracing a line.
	const glm::vec3 offset = { std::sin(phase) * amount * aspectFix, std::sin(phase * 1.37f + 1.1f) * amount, 0.0f };

	for (size_t idx = begin; idx < end; idx++)
	{
		const glm::vec3 translation = m_pSpriteBatch->GetTranslation(idx);
		saved.push_back({ idx, translation, m_pSpriteBatch->GetScale(idx),
			m_pSpriteBatch->GetOpacity(idx), m_pSpriteBatch->GetBrightness(idx), m_pSpriteBatch->GetTint(idx) });
		m_pSpriteBatch->TranslateTo(idx, translation + offset);
	}
}

void DanteHUD2::ApplyDTOrbPulse(std::vector<SavedSprite>& saved)
{
	if (!m_DTReady || m_DTLevel == 0)
		return;

	const auto& orbIdxs = m_SpriteIndices.GetDTBarOrbIdxs();
	const auto& glowIdxs = m_SpriteIndices.GetDTBarOrbGlowIdxs();
	const float wave = 0.5f - 0.5f * std::cos(6.28318531f * m_DTPulseTime / kDTPulsePeriod);
	// First fade in starts from zero, then the pulse bottoms out at the min opacity.
	const float minOpacity = m_DTPulseTime < kDTPulsePeriod * 0.5f ? 0.0f : kDTPulseMinOpacity;
	const float pulse = minOpacity + (1.0f - minOpacity) * wave;

	for (size_t i = 0; i < orbIdxs.size() && i < glowIdxs.size(); i++)
	{
		if (m_pSpriteBatch->GetOpacity(orbIdxs[i]) < 1.0f)
			continue;

		const size_t idx = glowIdxs[i];
		saved.push_back({ idx, m_pSpriteBatch->GetTranslation(idx), m_pSpriteBatch->GetScale(idx),
			m_pSpriteBatch->GetOpacity(idx), m_pSpriteBatch->GetBrightness(idx), m_pSpriteBatch->GetTint(idx) });
		m_pSpriteBatch->SetOpacity(idx, pulse);
	}
}

bool DanteHUD2::OnDraw()
{
	// Animation and group transforms only live for this draw, the HP updates keep re-translating sprites from the base layout.
	std::vector<SavedSprite> saved;

	ApplyStyleSwitchAnimation(saved);

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
			saved.push_back({ idx, translation, scale,
				m_pSpriteBatch->GetOpacity(idx), m_pSpriteBatch->GetBrightness(idx), m_pSpriteBatch->GetTint(idx) });

			m_pSpriteBatch->TranslateTo(idx, {
				pivot.x + (translation.x - pivot.x) * gt.scale + gt.offset.x,
				pivot.y + (translation.y - pivot.y) * gt.scale + gt.offset.y,
				translation.z });
			m_pSpriteBatch->ScaleTo(idx, scale * gt.scale);
		}
	}

	ApplyHPShake(saved);
	ApplyCompassShake(saved);
	ApplyDTOrbPulse(saved);
	ApplyDTExplosionBurst(saved);

	for (size_t g = 0; g < (size_t)Group_t::SIZE; g++)
	{
		size_t begin, end, anchor;
		if (m_GroupVisible[g] || !GetGroupRange((Group_t)g, begin, end, anchor))
			continue;

		for (size_t idx = begin; idx < end; idx++)
		{
			saved.push_back({ idx, m_pSpriteBatch->GetTranslation(idx), m_pSpriteBatch->GetScale(idx),
				m_pSpriteBatch->GetOpacity(idx), m_pSpriteBatch->GetBrightness(idx), m_pSpriteBatch->GetTint(idx) });
			m_pSpriteBatch->SetOpacity(idx, 0.0f);
		}
	}

	// The smoke goes into the target first so the compass draws over it.
	const bool smokeDrawn = RenderCompassSmoke();
	const bool result = m_pSpriteBatch->Draw(m_pD3D11DeviceContext, !smokeDrawn);
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

void DanteHUD2::ApplyStyleSwitchAnimation(std::vector<SavedSprite>& saved)
{
	if (!m_StyleSwitchActive && !m_StyleExitPending)
		return;

	const auto save = [&](size_t idx) {
		saved.push_back({ idx, m_pSpriteBatch->GetTranslation(idx), m_pSpriteBatch->GetScale(idx),
			m_pSpriteBatch->GetOpacity(idx), m_pSpriteBatch->GetBrightness(idx), m_pSpriteBatch->GetTint(idx) });
	};

	const CompassSlot_t activeSlot = GetStyleSlot(m_ActiveStyle);

	if (m_StyleExitPending)
	{
		const size_t idx = m_SpriteIndices.GetCompassNameSlotIdx(activeSlot);
		const float k = glm::clamp(m_StyleExitTime / kExitFadeDuration, 0.0f, 1.0f);

		save(idx);
		m_pSpriteBatch->SetOpacity(idx, m_pSpriteBatch->GetOpacity(idx) * (1.0f - k));
		m_pSpriteBatch->SetBrightness(idx, m_pSpriteBatch->GetBrightness(idx) * glm::mix(1.0f, kExitMaxBrightness, k));
	}

	if (!m_StyleSwitchActive)
		return;

	const float t = m_StyleSwitchTime;

	if (m_ActiveStyle < Style_t::SIZE)
	{
		const size_t idx = m_SpriteIndices.GetCompassCenterPieceIdx();
		const float k = glm::clamp(t / kCenterFlashDuration, 0.0f, 1.0f);
		const glm::vec3 tint = glm::mix(kCenterFlashTints[(size_t)m_ActiveStyle], glm::vec3(1.0f), k);

		save(idx);
		m_pSpriteBatch->TintTo(idx, m_pSpriteBatch->GetTint(idx) * glm::vec4(tint, 1.0f));
	}

	if (IsSecondaryStyle(m_ActiveStyle))
	{
		const size_t idx = m_SpriteIndices.GetCompassNameSlotIdx(activeSlot);

		save(idx);
		m_pSpriteBatch->SetOpacity(idx, m_pSpriteBatch->GetOpacity(idx) * SampleKeys(kSecondaryFadeKeys, t));
		m_pSpriteBatch->ScaleTo(idx, m_pSpriteBatch->GetScale(idx) * SampleKeys(kSecondaryScaleKeys, t));
	}
	else
	{
		// Sprite space is stretched to the RT, keep the pop the same on screen for both axes.
		const auto& dims = m_pSpriteBatch->GetDims();
		const float aspectFix = (dims.x > 0) ? (float)dims.y / (float)dims.x : 1.0f;
		const glm::vec2 dir = GetSlotOutwardDir(activeSlot);
		const float amount = SampleKeys(kPopKeys, t) * kPopAmplitude;
		const glm::vec3 offset = { dir.x * amount * aspectFix, dir.y * amount, 0.0f };

		for (const size_t idx : { m_SpriteIndices.GetCompassNameSlotIdx(activeSlot), m_SpriteIndices.GetCompassStyleShatteredBGIdx() })
		{
			save(idx);
			m_pSpriteBatch->TranslateTo(idx, m_pSpriteBatch->GetTranslation(idx) + offset);
		}
	}

	const float slotFade = SampleKeys(kSlotFadeKeys, t);
	for (size_t slot = 0; slot < (size_t)CompassSlot_t::SIZE; slot++)
	{
		if (!m_StyleSwitchFadeSlots[slot])
			continue;

		const size_t idx = m_SpriteIndices.GetCompassNameSlotIdx((CompassSlot_t)slot);
		save(idx);
		m_pSpriteBatch->SetOpacity(idx, m_pSpriteBatch->GetOpacity(idx) * slotFade);
	}
}

void DanteHUD2::SetGroupTransform(Group_t group, float scale, glm::vec2 offset)
{
	auto& gt = m_GroupTransforms[(size_t)group];
	gt.scale = scale;
	gt.offset = offset;
}

void DanteHUD2::SetGroupVisible(Group_t group, bool visible)
{
	m_GroupVisible[(size_t)group] = visible;
}

void DanteHUD2::SetCompassSmokeColor(glm::vec4 color)
{
	if (m_pCompassSmoke)
		m_pCompassSmoke->SetColor(color);
}

// Runs with the draw-time transforms applied, so the smoke follows the compass group scale, offset and shake.
bool DanteHUD2::RenderCompassSmoke()
{
	if (!m_pCompassSmoke || !m_CompassSmokeEnabled || !m_GroupVisible[(size_t)Group_t::CompassRoyalguard])
		return false;

	ID3D11RenderTargetView* pRTV = m_pSpriteBatch->GetRTV();
	const auto& dims = m_pSpriteBatch->GetDims();
	if (!pRTV || dims.x == 0 || dims.y == 0)
		return false;

	// Sprite translations are NDC of the unscaled HUD, the bounds scale doesn't move them in HUD pixels.
	const glm::vec3 frame = m_pSpriteBatch->GetTranslation(m_SpriteIndices.GetCompassFrameIdx());
	const glm::vec2 position = {
		frame.x * (float)m_Width * 0.5f + m_CompassSmokeOffset.x,
		frame.y * (float)m_Height * 0.5f + m_CompassSmokeOffset.y };
	const float scale = m_CompassSmokeScale * m_GroupTransforms[(size_t)Group_t::CompassRoyalguard].scale;

	m_pCompassSmoke->SetTransform(position, scale);
	m_pSpriteBatch->Clear(m_pD3D11DeviceContext);
	m_pCompassSmoke->Render(m_pD3D11DeviceContext, pRTV, dims.x, dims.y);
	return true;
}

void DanteHUD2::DrawCompassSmokeControls()
{
	if (!m_pCompassSmoke)
		return;

	const std::wstring& path = m_pCompassSmoke->GetEffectPath();
	ImGui::TextWrapped("Effect: %s", std::filesystem::path(path).string().c_str());
	if (m_pCompassSmoke->IsLoaded())
		ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "Loaded, reloads when the file is saved.");
	else
		ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.3f, 1.0f), "Not loaded, it loads as soon as the file exists.");

	ImGui::Checkbox("Enabled##CompassSmoke", &m_CompassSmokeEnabled);
	ImGui::SameLine();
	if (ImGui::Button("Restart##CompassSmoke"))
		m_pCompassSmoke->Restart();

	ImGui::DragFloat("HUD px per unit##CompassSmoke", &m_CompassSmokeScale, 0.1f, 0.1f, 200.0f, "%.2f");
	ImGui::DragFloat2("Offset (px, y up)##CompassSmoke", &m_CompassSmokeOffset.x, 0.5f, -1000.0f, 1000.0f, "%.1f");
	if (ImGui::DragFloat("Speed##CompassSmoke", &m_CompassSmokeSpeed, 0.01f, 0.0f, 10.0f, "%.2f"))
		m_pCompassSmoke->SetSpeed(m_CompassSmokeSpeed);
	ImGui::DragInt("Style switch warm-up (frames)##CompassSmoke", &m_CompassSmokeWarmupFrames, 1.0f, 0, 600);
}

void DanteHUD2::SetDTActive(bool active)
{
	m_DTActive = active;
}

// Same as the smoke, but anchored on the first DT orb and drawn over the sprites.
void DanteHUD2::RenderDTLightning()
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

void DanteHUD2::DrawDTLightningControls()
{
	if (!m_pDTLightning)
		return;

	const std::wstring& path = m_pDTLightning->GetEffectPath();
	ImGui::TextWrapped("Effect: %s", std::filesystem::path(path).string().c_str());
	if (m_pDTLightning->IsLoaded())
		ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "Loaded, reloads when the file is saved.");
	else
		ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.3f, 1.0f), "Not loaded, it loads as soon as the file exists.");

	ImGui::Checkbox("Enabled##DTLightning", &m_DTLightningEnabled);
	ImGui::SameLine();
	ImGui::Checkbox("Force on (ignore Devil Trigger)##DTLightning", &m_DTLightningForce);
	ImGui::SameLine();
	if (ImGui::Button("Restart##DTLightning"))
		m_pDTLightning->Restart();

	ImGui::DragFloat("HUD px per unit##DTLightning", &m_DTLightningScale, 0.1f, 0.1f, 200.0f, "%.2f");
	ImGui::DragFloat2("Offset (px, y up)##DTLightning", &m_DTLightningOffset.x, 0.5f, -1000.0f, 1000.0f, "%.1f");
	if (ImGui::DragFloat("Speed##DTLightning", &m_DTLightningSpeed, 0.01f, 0.0f, 10.0f, "%.2f"))
		m_pDTLightning->SetSpeed(m_DTLightningSpeed);
}

void DanteHUD2::SetDTExplosionCharge(float charge)
{
	if (m_pDTExplosion)
		m_pDTExplosion->SetCharge(charge);
}

void DanteHUD2::ApplyDTExplosionBurst(std::vector<SavedSprite>& saved)
{
	if (!m_pDTExplosion)
		return;

	const auto& burstIdxs = m_SpriteIndices.GetDTBarOrbBurstIdxs();
	for (size_t i = 0; i < burstIdxs.size(); i++)
	{
		float scale, opacity;
		if (!m_pDTExplosion->GetOrbBurst(i, scale, opacity))
			continue;

		const size_t idx = burstIdxs[i];
		saved.push_back({ idx, m_pSpriteBatch->GetTranslation(idx), m_pSpriteBatch->GetScale(idx),
			m_pSpriteBatch->GetOpacity(idx), m_pSpriteBatch->GetBrightness(idx), m_pSpriteBatch->GetTint(idx) });
		m_pSpriteBatch->ScaleTo(idx, m_pSpriteBatch->GetScale(idx) * scale);
		m_pSpriteBatch->SetOpacity(idx, opacity);
	}
}

void DanteHUD2::RenderDTExplosion()
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

void DanteHUD2::DrawDTExplosionControls()
{
	if (!m_pDTExplosion)
		return;

	ImGui::PushID("DanteDTExplosion");
	m_pDTExplosion->DrawControls();
	ImGui::PopID();
}

void DanteHUD2::SetBoundsScale(float scale)
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
bool DanteHUD2::GetGroupRange(Group_t group, size_t& begin, size_t& end, size_t& anchor) const
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
		anchor = m_SpriteIndices.GetCompassFrameIdx();
		break;
	case Group_t::ExpBarStyleName:
		begin = m_SpriteIndices.GetStyleExpBarLevelIdx();
		end = m_SpriteIndices.GetStyleExpBarIdx() + 1;
		anchor = m_SpriteIndices.GetStyleExpBarFrameIdx();
		break;
	case Group_t::DTBars:
		begin = m_SpriteIndices.GetStyleExpBarIdx() + 1;
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

ID3D11ShaderResourceView* DanteHUD2::GetSRV()
{
	return m_pSpriteBatch->GetRTSRV();
}

const std::string& DanteHUD2::GetLastRenderingErrorMsg()
{
	return m_pSpriteBatch->GetLastErrorMsg();
}

HRESULT DanteHUD2::GetLastRenderingErrorCode()
{
	return m_pSpriteBatch->GetLastErrorCode();
}

void DanteHUD2::SetActiveStyle(Style_t style)
{
	if (m_StyleExitPending)
	{
		m_PendingStyle = style;
		if (style == m_ActiveStyle)
			m_StyleExitPending = false;
		return;
	}

	if (!m_StyleApplied || style == m_ActiveStyle)
	{
		m_StyleApplied = true;
		m_ActiveStyle = style;
		UpdateSprites();
		return;
	}

	if (kExitDuration > 0.0f && IsSecondaryStyle(m_ActiveStyle) && GetStyleSlot(style) != GetStyleSlot(m_ActiveStyle))
	{
		m_StyleExitPending = true;
		m_StyleExitTime = 0.0f;
		m_PendingStyle = style;
		return;
	}

	CommitActiveStyle(style);
}

void DanteHUD2::CommitActiveStyle(Style_t style)
{
	const Style_t previousStyle = m_ActiveStyle;
	const CompassSlot_t activeSlot = GetStyleSlot(style);

	m_ActiveStyle = style;
	UpdateSprites();

	// Initials that stay in their slot swap instantly, the ones that moved fade in.
	for (size_t slot = 0; slot < (size_t)CompassSlot_t::SIZE; slot++)
	{
		m_StyleSwitchFadeSlots[slot] = (CompassSlot_t)slot != activeSlot
			&& GetSlotStyle((CompassSlot_t)slot, previousStyle) != GetSlotStyle((CompassSlot_t)slot, style);
	}

	m_StyleSwitchActive = true;
	m_StyleSwitchTime = 0.0f;
}

void DanteHUD2::SetHUDTheme(Theme_t theme)
{
	m_ThemeID = theme;

	UpdateSprites();
}

void DanteHUD2::SetStraightHPBars(bool straight)
{
	if (straight == m_StraightHPBars)
		return;

	m_StraightHPBars = straight;

	UpdateSprites();
}

void DanteHUD2::SetStraightHPBarOffset(bool offset)
{
	if (offset == m_StraightHPBarOffset)
		return;

	m_StraightHPBarOffset = offset;

	UpdateSprites();
}

void DanteHUD2::SetStyleLevelVisible(bool visible)
{
	if (visible == m_StyleLevelVisible)
		return;

	m_StyleLevelVisible = visible;
	m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetStyleExpBarLevelIdx(), visible ? 1.0f : 0.0f);
}

void DanteHUD2::SetStyleInitialsVisible(Style_t style, bool visible)
{
	if (style >= Style_t::SIZE || m_StyleInitialsVisible[(size_t)style] == visible)
		return;

	m_StyleInitialsVisible[(size_t)style] = visible;
	UpdateCompassInitialsVisibility();
}

// The secondary slots hold Darkslayer / Quicksilver / Doppleganger, or the main style of that side when the secondary one is active.
void DanteHUD2::UpdateCompassInitialsVisibility()
{
	struct Side { CompassSlot_t slot; Style_t main; Style_t secondary; };
	constexpr Side sides[] = {
		{ CompassSlot_t::RightSecondary,  Style_t::Swordmaster, Style_t::Darkslayer },
		{ CompassSlot_t::BottomSecondary, Style_t::Royalguard,  Style_t::Quicksilver },
		{ CompassSlot_t::LeftSecondary,   Style_t::Gunslinger,  Style_t::Doppleganger },
	};
	for (const auto& side : sides)
	{
		const Style_t shown = (m_ActiveStyle == side.secondary) ? side.main : side.secondary;
		m_pSpriteBatch->SetOpacity(m_SpriteIndices.GetCompassNameSlotIdx(side.slot), m_StyleInitialsVisible[(size_t)shown] ? 1.0f : 0.0f);
	}
}

void DanteHUD2::SetHPVitalityAmount(float vit)
{
	// Make sure the value is clamped
	m_HPFill = glm::clamp(vit, 0.0f, 1.0f);

	UpdateHPBarVitality(m_HPFill);
}

void DanteHUD2::SetHPDamageAmount(float dmg)
{
	// Make sure the value is clamped
	m_DMGFill = glm::clamp(dmg, 0.0f, 1.0f);

	UpdateHPBarDamage(m_DMGFill);
}

void DanteHUD2::SetRGGuageAmount(float rg)
{
	// Make sure the value is clamped
	m_RoyalguardGuageFill = glm::clamp(rg, 0.0f, 1.0f);

	UpdateRGGuage(m_RoyalguardGuageFill);
}

void DanteHUD2::SetActiveStyleExpFillAmount(float exp)
{
	// Make sure the value is clamped
	m_ActiveStyleExpFill = glm::clamp(exp, 0.0f, 1.0f);

	UpdateStyleExpBar(m_ActiveStyleExpFill);
}

void DanteHUD2::SetDTFill(float dt)
{
	// Make sure the value is clamped
	m_DTFill = glm::clamp(dt, 0.0f, 1.0f);

	UpdateDTBarOrbs(m_DTFill);
}

void DanteHUD2::SetDTReady(bool ready)
{
	if (ready == m_DTReady)
		return;

	// Restart from zero so the glow fades in.
	m_DTReady = ready;
	m_DTPulseTime = 0.0f;
}

void DanteHUD2::SetActiveStyleLevel(size_t level)
{
	if (level >= 1 && level <= 4)
	{
		m_ActiveStyleLevel = level;
	}
	else
	{
		m_ActiveStyleLevel = 1;
	}

	UpdateSprites();
}

void DanteHUD2::SetHPLevel(size_t level)
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

void DanteHUD2::SetDTLevel(size_t level)
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

void DanteHUD2::SetRGGaugeLevel(size_t level)
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

void DanteHUD2::SpriteIndices::UpdateSprites(Theme_t theme, Style_t activeStyle, size_t activeStyleLevel,
	size_t hpBarLevel, size_t dtBarLevel, size_t rgGuageLevel, bool straightHP)
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

		case 4:
			m_SpriteIds.push_back(
				(size_t)GetStyleExpBarLevelNumberSpriteID(theme, 4)
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

	// 11th: DT bar orbs, each over its empty background
	{
		if (dtBarLevel > 0)
		{
			m_DTBarOrbBGIdxs.clear();
			m_DTBarOrbBGIdxs.reserve(10);

			for (size_t i = 0; i < dtBarLevel + 2; i++)
			{
				m_DTBarOrbBGIdxs.push_back(m_SpriteIds.size());

				m_SpriteIds.push_back(
					(size_t)GetDTBarOrbBGSpriteID(theme)
				);
			}

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

	// 12th: DT bar frames
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

			// Glows go after the frames so they draw over the metal
			m_DTBarOrbGlowIdxs.clear();
			m_DTBarOrbGlowIdxs.reserve(10);

			for (size_t i = 0; i < dtBarLevel + 2; i++)
			{
				m_DTBarOrbGlowIdxs.push_back(m_SpriteIds.size());

				m_SpriteIds.push_back(
					(size_t)GetDTBarOrbGlowSpriteID(theme, i)
				);
			}

			// Orb copies for the DT Explosion release burst, hidden otherwise
			m_DTBarOrbBurstIdxs.clear();
			m_DTBarOrbBurstIdxs.reserve(10);

			for (size_t i = 0; i < dtBarLevel + 2; i++)
			{
				m_DTBarOrbBurstIdxs.push_back(m_SpriteIds.size());

				m_SpriteIds.push_back(
					(size_t)GetDTBarOrbSpriteID(theme, false)
				);
			}
		}
	}

	// 13th: HP bar bg
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

	// 14th: HP bar damage bar
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

	// 15th: HP bar vitality bar
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

	// 16th: HP bar vitality tail glow
	{
		m_VitalityEndGlowIdx = m_SpriteIds.size();

		m_SpriteIds.push_back(
			(size_t)GetHPBarVitalityTailGlowSpriteID(theme, straightHP)
		);
	}

	// 17th: HP bar frames
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
					(size_t)GetHPBarFrameSpriteID(theme, HPBarFrameVarient_t::Single, straightHP)
				);
			}
			else if (i == 0) // If higher than level 1, the 1st sprite is a head sprite
			{
				m_HPBarFrameIdxs.push_back(m_SpriteIds.size());

				m_SpriteIds.push_back(
					(size_t)GetHPBarFrameSpriteID(theme, HPBarFrameVarient_t::Head, straightHP)
				);
			}
			else if (i == 10 && hpBarLevel == 11) // If level 11, 11th sprite would be a single frame sprite
			{
				m_HPBarFrameIdxs.push_back(m_SpriteIds.size());

				m_SpriteIds.push_back(
					(size_t)GetHPBarFrameSpriteID(theme, HPBarFrameVarient_t::Single, straightHP)
				);
			}
			else if (i == hpBarLevel - 1) // Last one would be the tail sprite
			{
				m_HPBarFrameIdxs.push_back(m_SpriteIds.size());

				m_SpriteIds.push_back(
					(size_t)GetHPBarFrameSpriteID(theme, HPBarFrameVarient_t::Tail, straightHP)
				);
			}
			else if (i == 9) // If higher than level 10, 10th would be the tail of the top bar
			{
				m_HPBarFrameIdxs.push_back(m_SpriteIds.size());

				m_SpriteIds.push_back(
					(size_t)GetHPBarFrameSpriteID(theme, HPBarFrameVarient_t::Tail, straightHP)
				);
			}
			else if (i == 10) // If more than level 11, 11th sprite is the head of the bottom bar
			{
				m_HPBarFrameIdxs.push_back(m_SpriteIds.size());

				m_SpriteIds.push_back(
					(size_t)GetHPBarFrameSpriteID(theme, HPBarFrameVarient_t::Head, straightHP)
				);
			}
			else // Anything else would be the middle sprite
			{
				m_HPBarFrameIdxs.push_back(m_SpriteIds.size());

				m_SpriteIds.push_back(
					(size_t)GetHPBarFrameSpriteID(theme, HPBarFrameVarient_t::Mid, straightHP)
				);
			}
		}
	}
}

void DanteHUD2::SetSpritePositions()
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
		m_pSpriteBatch->SetOpacity(idx, m_StyleLevelVisible ? 1.0f : 0.0f);
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

	// DT bar follows the second HP bar when it's offset
	const float dtOffsetX = m_HPLevel < 11 ? 0.0f : GetStraightBottomOffsetX();

	// 10th: DT bar orbs
	{
		const auto idxs = m_SpriteIndices.GetDTBarOrbIdxs();
		const auto& bgIdxs = m_SpriteIndices.GetDTBarOrbBGIdxs();
		const auto& glowIdxs = m_SpriteIndices.GetDTBarOrbGlowIdxs();
		const auto& burstIdxs = m_SpriteIndices.GetDTBarOrbBurstIdxs();

		if (m_DTLevel > 0)
		{
			for (size_t i = 0; i < m_DTLevel + 2; i++)
			{
				const auto transform = GetDTBarOrb(m_ThemeID, m_HPLevel < 11 ? BarPositionID_t::Top : BarPositionID_t::Bottom, i + 1);
				const glm::f32vec3 position = { transform.Position.x + dtOffsetX, transform.Position.y, 0.0f };

				for (const size_t idx : { bgIdxs[i], idxs[i], burstIdxs[i] })
				{
					m_pSpriteBatch->SetTransform(
						idx,
						position,
						glm::f32vec3(0.0f),
						glm::f32vec3(transform.Scale)
					);
				}

				m_pSpriteBatch->SetTransform(
					glowIdxs[i],
					position,
					glm::f32vec3(0.0f),
					glm::f32vec3(transform.Scale * HC::Sprites::GetDTBarOrbGlowScale(i))
				);
			}
		}
	}

	// 11th: DT bar frames
	{
		const auto idxs = m_SpriteIndices.GetDTBarFrameIdxs();

		for (size_t i = 0; i < m_DTLevel; i++)
		{
			const auto transform = GetDTBarFrame(m_ThemeID, m_HPLevel < 11 ? BarPositionID_t::Top : BarPositionID_t::Bottom, (DTBarFrameID_t)i);

			m_pSpriteBatch->SetTransform(
				idxs[i], 
				{ transform.Position.x + dtOffsetX, transform.Position.y, 0.0f },
				glm::f32vec3(0.0f),
				glm::f32vec3(transform.Scale)
			);
		}
	}

	// 12th: HP bar bg
	{
		const auto& idxs = m_SpriteIndices.GetHPBarBGIdxs();

		const auto topTransform = GetHPBar(m_ThemeID, BarPositionID_t::Top, HPBarVarient_t::Background);

		m_pSpriteBatch->SetTransform(idxs[0], { topTransform.Position.x, topTransform.Position.y, 0.0f }, glm::f32vec3(0.0f), glm::f32vec3(topTransform.Scale));

		if (idxs.size() == 2)
		{
			const auto bottomTransform = GetHPBar(m_ThemeID, BarPositionID_t::Bottom, HPBarVarient_t::Background);
			const auto bottomX		   = m_StraightHPBars ? topTransform.Position.x + GetStraightBottomOffsetX() : bottomTransform.Position.x;

			m_pSpriteBatch->SetTransform(
				idxs[1],
				{ bottomX, bottomTransform.Position.y, 0.0f },
				glm::f32vec3(0.0f),
				glm::f32vec3(bottomTransform.Scale)
			);
		}
	}

	// 13th: HP bar damage bar
	{
		const auto& idxs = m_SpriteIndices.GetHPBarDamageIdxs();

		const auto topTransform = GetHPBar(m_ThemeID, BarPositionID_t::Top, HPBarVarient_t::Damage);

		m_pSpriteBatch->SetTransform(idxs[0], { topTransform.Position.x, topTransform.Position.y, 0.0f }, glm::f32vec3(0.0f), glm::f32vec3(topTransform.Scale));

		if (idxs.size() == 2)
		{
			const auto bottomTransform = GetHPBar(m_ThemeID, BarPositionID_t::Bottom, HPBarVarient_t::Damage);
			const auto bottomX		   = m_StraightHPBars ? topTransform.Position.x + GetStraightBottomOffsetX() : bottomTransform.Position.x;

			m_pSpriteBatch->SetTransform(
				idxs[1],
				{ bottomX, bottomTransform.Position.y, 0.0f },
				glm::f32vec3(0.0f),
				glm::f32vec3(bottomTransform.Scale)
			);
		}
	}

	// 14th: HP bar vitality bar
	{
		const auto& idxs = m_SpriteIndices.GetHPBarVitalityIdxs();

		const auto topTransform = GetHPBar(m_ThemeID, BarPositionID_t::Top, HPBarVarient_t::Vitality);

		m_pSpriteBatch->SetTransform(idxs[0], { topTransform.Position.x, topTransform.Position.y, 0.0f }, glm::f32vec3(0.0f), glm::f32vec3(topTransform.Scale));

		if (idxs.size() == 2)
		{
			const auto bottomTransform = GetHPBar(m_ThemeID, BarPositionID_t::Bottom, HPBarVarient_t::Vitality);
			const auto bottomX		   = m_StraightHPBars ? topTransform.Position.x + GetStraightBottomOffsetX() : bottomTransform.Position.x;

			m_pSpriteBatch->SetTransform(
				idxs[1],
				{ bottomX, bottomTransform.Position.y, 0.0f },
				glm::f32vec3(0.0f),
				glm::f32vec3(bottomTransform.Scale)
			);
		}
	}

	// 15th: HP bar vitality tail glow
	{
		const auto& idx = m_SpriteIndices.GetHPBarVitalityEndGlowIdx();

		if (m_HPLevel < 11)
		{
			const auto x	 = GetVitalityGlowX(BarPositionID_t::Top, m_HPLevel, 1.0f, m_StraightHPBars);
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
			const auto x	 = GetVitalityGlowX(BarPositionID_t::Bottom, m_HPLevel - 10, 1.0f, m_StraightHPBars) + GetStraightBottomOffsetX();
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

	// 16th: HP bar frames
	{
		const auto& idxs = m_SpriteIndices.GetHPBarFrameIdxs();

		const auto setFrame = [&](size_t i, HPBarFrameVarient_t varient)
		{
			const auto& transform = GetHPBarFrame(m_ThemeID, i + 1, varient);
			// Straight bottom bar sits at the top bar's x, plus the optional offset
			const auto  x		  = (m_StraightHPBars && i >= 10) ? GetHPBarFrame(m_ThemeID, i - 9, varient).Position.x + GetStraightBottomOffsetX() : transform.Position.x;

			m_pSpriteBatch->SetTransform(
				idxs[i],
				{ x, transform.Position.y, 0.0f },
				glm::f32vec3(0.0f),
				glm::f32vec3(transform.Scale)
			);
		};

		for (size_t i = 0; i < m_HPLevel; i++)
		{
			if (m_HPLevel == 1) // If level 1, only a signle frame sprite is used
			{
				setFrame(i, HPBarFrameVarient_t::Single);
			}
			else if (i == 0) // If higher than level 1, the 1st sprite is a head sprite
			{
				setFrame(i, HPBarFrameVarient_t::Head);
			}
			else if (i == 10 && m_HPLevel == 11) // If level 11, 11th sprite would be a single frame sprite
			{
				setFrame(i, HPBarFrameVarient_t::Single);
			}
			else if (i == m_HPLevel - 1) // Last one would be the tail sprite
			{
				setFrame(i, HPBarFrameVarient_t::Tail);
			}
			else if (i == 9) // If higher than level 10, 10th would be the tail of the top bar
			{
				setFrame(i, HPBarFrameVarient_t::Tail);
			}
			else if (i == 10) // If more than level 11, 11th sprite is the head of the bottom bar
			{
				setFrame(i, HPBarFrameVarient_t::Head);
			}
			else // Anything else would be the middle sprite
			{
				setFrame(i, HPBarFrameVarient_t::Middle);
			}
		}
	}
}

float DanteHUD2::GetStraightBottomOffsetX() const
{
	using namespace HC::Transforms;

	if (!m_StraightHPBars || !m_StraightHPBarOffset)
		return 0.0f;

	// Same offset the slanted second bar uses
	return GetHPBar(m_ThemeID, BarPositionID_t::Bottom, HPBarVarient_t::Background).Position.x
		 - GetHPBar(m_ThemeID, BarPositionID_t::Top, HPBarVarient_t::Background).Position.x;
}

void DanteHUD2::SetSpriteMasks()
{
	for (size_t i = 0; i < m_SpriteIndices.GetHPBarBGIdxs().size(); i++)
	{
		const auto idx = m_SpriteIndices.GetHPBarBGIdxs()[i];

		switch (i)
		{
		case 0:
			m_pSpriteBatch->SetSpriteMasks(idx, HC::Masks::GetHPBarBackgroundMasks(m_HPLevel >= 10 ? 1.0f : (m_HPLevel * 0.1f), m_StraightHPBars));
			break;

		case 1:
			m_pSpriteBatch->SetSpriteMasks(idx, HC::Masks::GetHPBarBackgroundMasks((m_HPLevel - 10) * 0.1f, m_StraightHPBars));
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
			m_pSpriteBatch->SetSpriteMasks(idx, HC::Masks::GetHPBarDamageMasks(m_HPLevel >= 10 ? 1.0f : (m_HPLevel * 0.1f), m_StraightHPBars));
			break;

		case 1:
			m_pSpriteBatch->SetSpriteMasks(idx, HC::Masks::GetHPBarDamageMasks((m_HPLevel - 10) * 0.1f, m_StraightHPBars));
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
			m_pSpriteBatch->SetSpriteMasks(idx, HC::Masks::GetHPBarVitalityMasks(m_HPLevel >= 10 ? 1.0f : (m_HPLevel * 0.1f), m_StraightHPBars));
			break;

		case 1:
			m_pSpriteBatch->SetSpriteMasks(idx, HC::Masks::GetHPBarVitalityMasks((m_HPLevel - 10) * 0.1f, m_StraightHPBars));
			break;

		default:
			break;
		}
	}

	m_pSpriteBatch->SetSpriteMasks(m_SpriteIndices.GetHPBarVitalityEndGlowIdx(), HC::Masks::GetHPBarVitalityEndGlowMasks(m_StraightHPBars));
	m_pSpriteBatch->SetSpriteMasks(m_SpriteIndices.GetRoyalguardGaugeIdx(), HC::Masks::GetRGGuageBarMasks(m_RoyalGuardGaugeLevel));
	m_pSpriteBatch->SetSpriteMasks(m_SpriteIndices.GetStyleExpBarIdx(), HC::Masks::GetStyleExpBarMasks());

	UpdateHPBarVitality(m_HPFill);
	UpdateHPBarDamage(m_DMGFill);
	UpdateRGGuage(m_RoyalguardGuageFill);
	UpdateStyleExpBar(m_ActiveStyleExpFill);
}

void DanteHUD2::UpdateHPBarVitality(float fraction)
{
	using namespace HC::Sprites;
	using namespace HC::Transforms;
	using namespace HC::Masks;

	// Vitality bar
	const     auto  vitalSpriteIdxs = m_SpriteIndices.GetHPBarVitalityIdxs();
	const     auto  headMask	    = GetMaskInfo(m_StraightHPBars ? MaskInfoID_t::HPBarVitalityStraightHead : MaskInfoID_t::HPBarVitalitySlantedHead);
	const     auto  tailMask	    = GetMaskInfo(m_StraightHPBars ? MaskInfoID_t::HPBarVitalityStraightTail : MaskInfoID_t::HPBarVitalitySlantedTail);
	const     float	fullSpan	    = tailMask.Slanted.Origin.x - headMask.Slanted.Origin.x;

	// End glow
	const auto glowMaskRangeX	  = HC::Processed::GetGlowMaskRangeX(m_StraightHPBars);
	const auto vitalityEndGlowIdx = m_SpriteIndices.GetHPBarVitalityEndGlowIdx();

	if (m_HPLevel > 10)
	{
		const     auto fullSpanTopBar		 = fullSpan;
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
					const     auto  newX		= GetVitalityGlowX(BarPositionID_t::Top, 10, expectedCurrentFill / fullSpanTopBar, m_StraightHPBars);
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
					const     auto  newX		= GetVitalityGlowX(BarPositionID_t::Top, 10, 1.0f, m_StraightHPBars);
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
					const     auto  newX		= GetVitalityGlowX(BarPositionID_t::Bottom, bottomBarLevel, expectedBottomFill / fullSpanBottomBar, m_StraightHPBars) + GetStraightBottomOffsetX();
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
			const     auto  newX		= GetVitalityGlowX(BarPositionID_t::Top, m_HPLevel, expectedCurrentFill / fullSpanBar, m_StraightHPBars);
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

void DanteHUD2::UpdateHPBarDamage(float fraction)
{
	using namespace HC::Sprites;
	using namespace HC::Transforms;
	using namespace HC::Masks;

	const     auto  dmgSpriteIdxs	= m_SpriteIndices.GetHPBarDamageIdxs();
	const     auto  headMask	    = GetMaskInfo(m_StraightHPBars ? MaskInfoID_t::HPBarDamageStraightHead : MaskInfoID_t::HPBarDamageSlantedHead);
	const     auto  tailMask	    = GetMaskInfo(m_StraightHPBars ? MaskInfoID_t::HPBarDamageStraightTail : MaskInfoID_t::HPBarDamageSlantedTail);
	const     float	fullSpan	    = tailMask.Slanted.Origin.x - headMask.Slanted.Origin.x;

	if (m_HPLevel > 10)
	{
		const     auto fullSpanTopBar		 = fullSpan;
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

void DanteHUD2::UpdateRGGuage(float fraction)
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

void DanteHUD2::UpdateStyleExpBar(float fraction)
{
	using namespace HC::Masks;

	const auto styleExpBarIdx  = m_SpriteIndices.GetRoyalguardGaugeIdx();
	auto	   styleExpBarMask = GetMaskInfo(MaskInfoID_t::StyleExpBarSlantedTail);

	styleExpBarMask.Slanted.Origin.x = glm::mix(0.0f, styleExpBarMask.Slanted.Origin.x, fraction);

	m_pSpriteBatch->UpdateSpriteMask(m_SpriteIndices.GetStyleExpBarIdx(), 0, styleExpBarMask);
}

void DanteHUD2::UpdateDTBarOrbs(float fraction)
{
	// If DT is not unlocked, do nothing
	if (m_DTLevel == 0)
		return;

	const auto orbTextureIdxs = m_SpriteIndices.GetDTBarOrbIdxs();
	const auto eachOrbShare   = 1.0f / (m_DTLevel + 2);

	const size_t numFullOrbs = fraction / eachOrbShare;

	// DT orbs should be invisible by default
	for (const auto& idx : orbTextureIdxs)
	{
		m_pSpriteBatch->SetOpacity(idx, 0.0f);
	}

	// Glows only show through ApplyDTOrbPulse
	for (const auto& idx : m_SpriteIndices.GetDTBarOrbGlowIdxs())
	{
		m_pSpriteBatch->SetOpacity(idx, 0.0f);
	}

	// Bursts only show through ApplyDTExplosionBurst
	for (const auto& idx : m_SpriteIndices.GetDTBarOrbBurstIdxs())
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

void DanteHUD2::UpdateSprites()
{
	m_SpriteIndices.UpdateSprites(m_ThemeID, m_ActiveStyle, m_ActiveStyleLevel,
		m_HPLevel, m_DTLevel, m_RoyalGuardGaugeLevel, m_StraightHPBars);

	m_pSpriteBatch->ChangeSprites(m_pD3D11Device, m_SpriteDescs, m_SpriteIndices.GetSpriteIds(), s_pAtlasTexture);

	SetSpritePositions();
	SetSpriteMasks();
	UpdateDTBarOrbs(m_DTFill);
	UpdateCompassInitialsVisibility();
}

void DanteHUD2::DrawDebugControls()
{
	using namespace HC::Sprites;

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

		if (ImGui::SliderInt("Style Level", &styleLevel, 1, 4))
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

		bool dtReady = m_DTReady;
		if (ImGui::Checkbox("DT Ready", &dtReady))
		{
			SetDTReady(dtReady);
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
				if (currentSelectedMask < 0 || currentSelectedMask >= (int)masks.size())
				{
					currentSelectedMask = 0;
				}
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
}

std::string DanteHUD2::DumpSpriteLayout()
{
	using namespace HC::Sprites;
	using MaskType_t = Graphics::BatchedSprites::MaskType_t;

	std::string out = std::format("// Theme {}, style {}, style level {}, HP level {}, DT level {}, RG level {}, straight HP {}, offset {}\n",
		(int)m_ThemeID, (int)m_ActiveStyle, m_ActiveStyleLevel, m_HPLevel, m_DTLevel, m_RoyalGuardGaugeLevel, m_StraightHPBars, m_StraightHPBarOffset);
	out += std::format("// Fills: HP {:.4f}f, DMG {:.4f}f, RG {:.4f}f, style exp {:.4f}f, DT {:.4f}f\n",
		m_HPFill, m_DMGFill, m_RoyalguardGuageFill, m_ActiveStyleExpFill, m_DTFill);
	out += "// { sprite, index, { posX, posY }, scale }\n";

	const auto& spriteIds = m_SpriteIndices.GetSpriteIds();
	for (size_t i = 0; i < spriteIds.size(); i++)
	{
		const auto& t = m_pSpriteBatch->GetTranslation(i);
		const auto& s = m_pSpriteBatch->GetScale(i);
		out += std::format("{{ \"{}\", {}, {{ {:.4f}f, {:.4f}f }}, {:.4f}f }},\n",
			GetTextureNameFromSpriteID((SpriteID_t)spriteIds[i]), i, t.x, t.y, s.x);

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
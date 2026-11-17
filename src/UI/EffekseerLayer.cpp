#include "EffekseerLayer.hpp"

#include <algorithm>
#include <cmath>
#include <exception>

namespace Graphics {

namespace {

constexpr float kReloadCheckInterval = 0.5f;
constexpr float kCameraDistance = 1000.0f;

uint8_t ToColorByte(float value)
{
	return (uint8_t)(std::clamp(value, 0.0f, 1.0f) * 255.0f + 0.5f);
}

}

EffekseerLayer::EffekseerLayer(ID3D11Device* pD3D11Device, ID3D11DeviceContext* pD3D11DeviceContext, int maxSprites)
{
	if (!pD3D11Device || !pD3D11DeviceContext)
		return;

	m_Renderer = EffekseerRendererDX11::Renderer::Create(pD3D11Device, pD3D11DeviceContext, maxSprites);
	m_Manager = Effekseer::Manager::Create(maxSprites);
	if (m_Renderer == nullptr || m_Manager == nullptr)
	{
		m_Manager.Reset();
		m_Renderer.Reset();
		return;
	}

	m_Manager->SetSpriteRenderer(m_Renderer->CreateSpriteRenderer());
	m_Manager->SetRibbonRenderer(m_Renderer->CreateRibbonRenderer());
	m_Manager->SetRingRenderer(m_Renderer->CreateRingRenderer());
	m_Manager->SetTrackRenderer(m_Renderer->CreateTrackRenderer());
	m_Manager->SetModelRenderer(m_Renderer->CreateModelRenderer());

	m_Manager->SetTextureLoader(m_Renderer->CreateTextureLoader());
	m_Manager->SetModelLoader(m_Renderer->CreateModelLoader());
	m_Manager->SetMaterialLoader(m_Renderer->CreateMaterialLoader());
	m_Manager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());
}

EffekseerLayer::~EffekseerLayer()
{
	Stop();
	m_Effect.Reset();
	m_Manager.Reset();
	m_Renderer.Reset();
}

void EffekseerLayer::SetEffectPath(const std::wstring& path)
{
	if (path == m_Path)
		return;

	m_Path = path;
	m_HasWriteTime = false;
	m_ReloadCheckTimer = kReloadCheckInterval;
	Stop();
	m_Effect.Reset();
	CheckReload(0.0f);
}

void EffekseerLayer::SetPlaying(bool playing)
{
	if (playing == m_Playing)
		return;

	m_Playing = playing;
	if (!playing)
		Stop();
}

void EffekseerLayer::Restart(int startFrame)
{
	Stop();
	if (m_Playing)
		Play(startFrame);
}

void EffekseerLayer::SetColor(glm::vec4 color)
{
	if (color == m_Color)
		return;

	m_Color = color;
	ApplyColor();
}

void EffekseerLayer::SetTransform(glm::vec2 position, float scale)
{
	if (position == m_Position && scale == m_Scale)
		return;

	m_Position = position;
	m_Scale = scale;
	ApplyTransform();
}

void EffekseerLayer::SetRotation(float rotation)
{
	if (rotation == m_Rotation)
		return;

	m_Rotation = rotation;
	ApplyTransform();
}

void EffekseerLayer::Update(float dt)
{
	if (m_Manager == nullptr)
		return;

	CheckReload(dt);

	if (m_Playing && (m_Handle < 0 || !m_Manager->Exists(m_Handle)))
		Play();

	Effekseer::Manager::UpdateParameter updateParam;
	updateParam.DeltaFrame = dt * 60.0f * m_Speed;
	m_Manager->Update(updateParam);
	m_Time += dt * m_Speed;
}

void EffekseerLayer::Render(ID3D11DeviceContext* pD3D11DeviceContext, ID3D11RenderTargetView* pRTV, UINT width, UINT height)
{
	if (m_Manager == nullptr || m_Handle < 0 || !pRTV || width == 0 || height == 0)
		return;

	ID3D11RenderTargetView* pOldRTV = nullptr;
	ID3D11DepthStencilView* pOldDSV = nullptr;
	pD3D11DeviceContext->OMGetRenderTargets(1, &pOldRTV, &pOldDSV);
	UINT oldViewportCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	D3D11_VIEWPORT oldViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	pD3D11DeviceContext->RSGetViewports(&oldViewportCount, oldViewports);

	pD3D11DeviceContext->OMSetRenderTargets(1, &pRTV, nullptr);
	const D3D11_VIEWPORT viewport = { 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f };
	pD3D11DeviceContext->RSSetViewports(1, &viewport);

	const Effekseer::Vector3D eye(0.0f, 0.0f, kCameraDistance);
	Effekseer::Matrix44 projection;
	projection.OrthographicRH((float)width, (float)height, 1.0f, kCameraDistance * 2.0f);
	Effekseer::Matrix44 camera;
	camera.LookAtRH(eye, Effekseer::Vector3D(0.0f, 0.0f, 0.0f), Effekseer::Vector3D(0.0f, 1.0f, 0.0f));

	m_Renderer->SetProjectionMatrix(projection);
	m_Renderer->SetCameraMatrix(camera);
	m_Renderer->SetTime(m_Time);

	Effekseer::Manager::LayerParameter layerParameter;
	layerParameter.ViewerPosition = eye;
	m_Manager->SetLayerParameter(0, layerParameter);

	Effekseer::Manager::DrawParameter drawParam;
	drawParam.ZNear = 0.0f;
	drawParam.ZFar = 1.0f;
	drawParam.ViewProjectionMatrix = m_Renderer->GetCameraProjectionMatrix();
	drawParam.CameraPosition = eye;
	drawParam.CameraFrontDirection = Effekseer::Vector3D(0.0f, 0.0f, -1.0f);

	m_Renderer->BeginRendering();
	m_Manager->Draw(drawParam);
	m_Renderer->EndRendering();

	pD3D11DeviceContext->OMSetRenderTargets(1, &pOldRTV, pOldDSV);
	pD3D11DeviceContext->RSSetViewports(oldViewportCount, oldViewports);
	if (pOldRTV) pOldRTV->Release();
	if (pOldDSV) pOldDSV->Release();
}

bool EffekseerLayer::Load()
{
	if (m_Manager == nullptr || m_Path.empty())
		return false;

	Effekseer::EffectRef effect;
	try {
		effect = Effekseer::Effect::Create(m_Manager, (const char16_t*)m_Path.c_str());
	} catch (const std::exception&) {
		return false;
	}

	if (effect == nullptr)
		return false;

	m_Effect = effect;
	Restart();
	return true;
}

void EffekseerLayer::CheckReload(float dt)
{
	if (m_Path.empty())
		return;

	m_ReloadCheckTimer += dt;
	if (m_ReloadCheckTimer < kReloadCheckInterval)
		return;
	m_ReloadCheckTimer = 0.0f;

	std::error_code errorCode;
	const auto writeTime = std::filesystem::last_write_time(m_Path, errorCode);
	if (errorCode)
		return;

	if (m_HasWriteTime && writeTime == m_LastWriteTime)
		return;

	// A failed load (the editor still writing the file) keeps the current effect and retries next check.
	if (!Load())
		return;

	m_LastWriteTime = writeTime;
	m_HasWriteTime = true;
}

void EffekseerLayer::Play(int startFrame)
{
	if (m_Manager == nullptr || m_Effect == nullptr)
		return;

	// The start frames are simulated on the next Update, after the transform below is set.
	m_Handle = m_Manager->Play(m_Effect, Effekseer::Vector3D(0.0f, 0.0f, 0.0f), startFrame);
	ApplyTransform();
	ApplyColor();
}

void EffekseerLayer::Stop()
{
	if (m_Manager != nullptr && m_Handle >= 0)
		m_Manager->StopEffect(m_Handle);
	m_Handle = -1;
}

void EffekseerLayer::ApplyTransform()
{
	if (m_Manager == nullptr || m_Handle < 0)
		return;

	// Row vectors: rows 0 and 1 are where the x and y axes end up.
	const float c = std::cos(m_Rotation) * m_Scale;
	const float s = std::sin(m_Rotation) * m_Scale;
	Effekseer::Matrix43 matrix;
	matrix.Scaling(m_Scale, m_Scale, m_Scale);
	matrix.Value[0][0] = c;
	matrix.Value[0][1] = s;
	matrix.Value[1][0] = -s;
	matrix.Value[1][1] = c;
	matrix.Value[3][0] = m_Position.x;
	matrix.Value[3][1] = m_Position.y;
	matrix.Value[3][2] = 0.0f;
	m_Manager->SetMatrix(m_Handle, matrix);
}

void EffekseerLayer::ApplyColor()
{
	if (m_Manager == nullptr || m_Handle < 0)
		return;

	m_Manager->SetAllColor(m_Handle, Effekseer::Color(ToColorByte(m_Color.r), ToColorByte(m_Color.g),
		ToColorByte(m_Color.b), ToColorByte(m_Color.a)));
}

}

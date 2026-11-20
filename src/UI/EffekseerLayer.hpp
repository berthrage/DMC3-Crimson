#pragma once

#include "GraphicsBase.hpp"

#include "Effekseer.h"
#include "EffekseerRendererDX11.h"

#include <filesystem>

namespace Graphics {

// One looping Effekseer effect drawn into a 2D render target, apart from the world effects (own manager, no game camera
// or game speed). Orthographic camera looking down -Z: world units are render target pixels, origin at the center, y up.
// The effect restarts whenever it ends and reloads whenever its file changes on disk.
class EffekseerLayer {
public:
	EffekseerLayer(ID3D11Device* pD3D11Device, ID3D11DeviceContext* pD3D11DeviceContext, int maxSprites = 2000);
	~EffekseerLayer();

	void SetEffectPath(const std::wstring& path);
	const std::wstring& GetEffectPath() const { return m_Path; }
	bool IsLoaded() const { return m_Effect != nullptr; }

	void SetPlaying(bool playing);
	bool IsPlaying() const { return m_Playing; }
	// startFrame > 0 simulates that many 60 Hz frames up front, spread over the next updates; the current effect keeps
	// playing until the new one is ready.
	void Restart(int startFrame = 0);

	// Multiplies every color in the effect, author it white to take this color as is.
	void SetColor(glm::vec4 color);
	void SetSpeed(float speed) { m_Speed = speed; }
	// Position in world units, scale in world units per effect unit.
	void SetTransform(glm::vec2 position, float scale);
	// Radians counter-clockwise. Particles bound to rotation "when creating" keep their old direction.
	void SetRotation(float rotation);

	void Update(float dt);
	// Draws on top of what is already in the target, no depth.
	void Render(ID3D11DeviceContext* pD3D11DeviceContext, ID3D11RenderTargetView* pRTV, UINT width, UINT height);

private:
	bool Load();
	void CheckReload(float dt);
	void Play(int startFrame = 0);
	void Stop();
	void StopWarmUp();
	void StepWarmUp(float dt);
	void ApplyTransform();
	void ApplyTransform(Effekseer::Handle handle, float rotation);
	void ApplyColor();
	void ApplyColor(Effekseer::Handle handle);

private:
	Effekseer::ManagerRef m_Manager;
	EffekseerRendererDX11::RendererRef m_Renderer;
	Effekseer::EffectRef m_Effect;
	Effekseer::Handle m_Handle{ -1 };
	// A restart with start frames is simulated in slices on a hidden handle, then swapped in, so it costs no hitch.
	Effekseer::Handle m_WarmHandle{ -1 };
	int m_WarmRemaining{ 0 };
	int m_WarmTotal{ 0 };
	float m_WarmBudget{ 0.0f };
	float m_ActiveRotation{ 0.0f };

	std::wstring m_Path;
	std::filesystem::file_time_type m_LastWriteTime{};
	bool m_HasWriteTime{ false };
	float m_ReloadCheckTimer{ 0.0f };

	bool m_Playing{ false };
	float m_Speed{ 1.0f };
	float m_Time{ 0.0f };
	float m_PendingTime{ 0.0f };
	glm::vec2 m_Position{ 0.0f, 0.0f };
	float m_Scale{ 1.0f };
	float m_Rotation{ 0.0f };
	glm::vec4 m_Color{ 1.0f, 1.0f, 1.0f, 1.0f };
};

}

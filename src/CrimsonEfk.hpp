#pragma once

#include "Effekseer.h"

typedef int EffekseerHandle;
typedef unsigned long long EffekseerRefHandle;
struct ID3D11Device;
struct ID3D11DeviceContext;

namespace CrimsonEfk {
	bool EffekInit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, int windowWidth, int windowHeight);
	void EffekTerminate();

	void EffekInitRenderHook();
	void EffekDrawImgui();

	EffekseerRefHandle LoadEffect(const wchar_t* path, float scale = 1.0f);
	EffekseerRefHandle LoadEffectFromMemory(const void* data, int size, float scale = 1.0f);
	EffekseerRefHandle ReloadEffect(EffekseerRefHandle hEffect, const wchar_t* path, float scale = 1.0f);
	EffekseerRefHandle LoadEffectAutoReload(EffekseerRefHandle hEffect, const wchar_t* path, float scale = 1.0f, int checkIntervalMs = 500);

	EffekseerHandle PlayEffect(EffekseerRefHandle pEffect, float* vec3, void* player = nullptr);
	EffekseerHandle PlayEffect(EffekseerRefHandle pEffect, float x, float y, float z, void* player = nullptr);
	EffekseerHandle PlayEffectAtMatrix(EffekseerRefHandle pEffect, float* mat44, void* player = nullptr);
	EffekseerHandle PlayEffectAtPos(EffekseerRefHandle pEffect, float* vec3, void* player = nullptr);

	void SetDynamicPos(EffekseerHandle handle, float x, float y, float z);
	void SetDynamicPos(EffekseerHandle handle, float* vec3);
	void StopEffect(EffekseerHandle handle);
	void StopAllEffects();
	bool IsPlaying(EffekseerHandle handle);
	void MoveEffect(EffekseerHandle handle, float x, float y, float z);
	void MoveEffect(EffekseerHandle handle, float* vec3);
	void SetMatrix(EffekseerHandle handle, Effekseer::Matrix43 mat43);
	void SetMatrix(EffekseerHandle handle, float* mat43);
	void SetAllColor(EffekseerHandle handle, uint32_t color);
  void SetVisible(EffekseerHandle handle, bool visible);
	void CaptureDepthStencilForPresent(ID3D11DeviceContext* pContext);

	void EffekRender(ID3D11DeviceContext* pContext, float deltaTime = 1.0f / 60.0f);
  void EffekRenderOnPresent(ID3D11DeviceContext* pContext);
	void EffekIncFrames();
}

//void EffekseerDrawImGui();


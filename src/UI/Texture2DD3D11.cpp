#include "Texture2DD3D11.hpp"

#include <comdef.h>

#include <string>
#include <format>

#include "..\ThirdParty\stb\stb_image.h"

Texture2DD3D11::Texture2DD3D11(const char* filename, ID3D11Device* pd3dDevice)
	: m_pD3DDevice(pd3dDevice)
{
	// Load from disk into a raw RGBA buffer
	m_ImageData = stbi_load(filename, &m_Width, &m_Height, NULL, 4);

	m_IsLoaded = Commit();

	stbi_image_free(m_ImageData);
}

Texture2DD3D11::Texture2DD3D11(const unsigned char* imageData, size_t imageDataSize, ID3D11Device* pd3dDevice)
	: m_pD3DDevice(pd3dDevice)
{
	// Load from memory as raw RGBA buffer
	m_ImageData = stbi_load_from_memory(imageData, imageDataSize, &m_Width, &m_Height, NULL, 4);

	m_IsLoaded = Commit();

	stbi_image_free(m_ImageData);
}

Texture2DD3D11::Texture2DD3D11(unsigned char* imageData, int width, int height, ID3D11Device* pd3dDevice)
	: m_ImageData(imageData), m_Width(width), m_Height(height), m_pD3DDevice(pd3dDevice)
{
	// Upload the RGBA buffer into VRam
	m_IsLoaded = Commit();
}

bool Texture2DD3D11::Commit()
{
	if (m_ImageData == nullptr) {
		m_LastError = "Invalid image data!";
		return false;
	}

	if (m_pD3DDevice == nullptr) {
		m_LastError = "Invalid device!";
		return false;
	}

	// An hresult we will use to make sure everything works
	HRESULT hr = S_OK;

	// Create texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = m_Width;
	desc.Height = m_Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = m_ImageData;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	hr = m_pD3DDevice->CreateTexture2D(&desc, &subResource, &pTexture);

	if (FAILED(hr)) {
		m_LastError = std::format("CreateTexture2D() failed! HResult = 0x{:08X} -> {:s}", hr, "_com_error(hr).ErrorMessage()");
		return false;
	}

	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	hr = m_pD3DDevice->CreateShaderResourceView(pTexture.Get(), &srvDesc, &m_SRV);

	if (FAILED(hr)) {
		m_LastError = std::format("CreateShaderResourceView() failed! HResult = 0x{:08X} -> {:s}", hr, "_com_error(hr).ErrorMessage()");
		return false;
	}

	return true;
}

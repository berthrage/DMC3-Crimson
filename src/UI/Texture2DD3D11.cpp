#include "Texture2DD3D11.hpp"

#include <comdef.h>

#include <string>
#include <format>
#include <vector>
#include <algorithm>

#include "stb_image.h"

// 2x2 box downsample with alpha-weighted color so transparent texels don't bleed dark fringes.
static void DownsampleRGBA(const unsigned char* src, int srcW, int srcH, unsigned char* dst, int dstW, int dstH)
{
	for (int y = 0; y < dstH; ++y) {
		const int y0 = (std::min)(y * 2, srcH - 1);
		const int y1 = (std::min)(y * 2 + 1, srcH - 1);
		for (int x = 0; x < dstW; ++x) {
			const int x0 = (std::min)(x * 2, srcW - 1);
			const int x1 = (std::min)(x * 2 + 1, srcW - 1);
			const unsigned char* p[4] = {
				src + (y0 * srcW + x0) * 4, src + (y0 * srcW + x1) * 4,
				src + (y1 * srcW + x0) * 4, src + (y1 * srcW + x1) * 4,
			};

			unsigned int a = 0, r = 0, g = 0, b = 0;
			for (auto* t : p) {
				a += t[3];
				r += t[0] * t[3];
				g += t[1] * t[3];
				b += t[2] * t[3];
			}

			unsigned char* out = dst + (y * dstW + x) * 4;
			if (a > 0) {
				out[0] = (unsigned char)((r + a / 2) / a);
				out[1] = (unsigned char)((g + a / 2) / a);
				out[2] = (unsigned char)((b + a / 2) / a);
			} else {
				out[0] = (unsigned char)((p[0][0] + p[1][0] + p[2][0] + p[3][0] + 2) / 4);
				out[1] = (unsigned char)((p[0][1] + p[1][1] + p[2][1] + p[3][1] + 2) / 4);
				out[2] = (unsigned char)((p[0][2] + p[1][2] + p[2][2] + p[3][2] + 2) / 4);
			}
			out[3] = (unsigned char)((a + 2) / 4);
		}
	}
}

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

	// Full mip chain built once at load; HUD art is drawn well below native size and
	// without mips the trilinear ImGui sampler aliases badly.
	std::vector<std::vector<unsigned char>> mips;
	std::vector<D3D11_SUBRESOURCE_DATA> subResources;
	subResources.push_back({ m_ImageData, (UINT)m_Width * 4, 0 });
	{
		const unsigned char* src = m_ImageData;
		int w = m_Width;
		int h = m_Height;
		while (w > 1 || h > 1) {
			const int nw = (std::max)(1, w / 2);
			const int nh = (std::max)(1, h / 2);
			auto& level = mips.emplace_back((size_t)nw * nh * 4);
			DownsampleRGBA(src, w, h, level.data(), nw, nh);
			src = level.data();
			w = nw;
			h = nh;
		}
		for (auto& level : mips) {
			const int levelIndex = (int)subResources.size();
			const UINT levelWidth = (UINT)(std::max)(1, m_Width >> levelIndex);
			subResources.push_back({ level.data(), levelWidth * 4, 0 });
		}
	}

	// Create texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = m_Width;
	desc.Height = m_Height;
	desc.MipLevels = (UINT)subResources.size();
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
	hr = m_pD3DDevice->CreateTexture2D(&desc, subResources.data(), &pTexture);

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

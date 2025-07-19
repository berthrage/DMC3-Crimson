#include "Texture2DArrayD3D11.hpp"

#include <format>
#include <comdef.h>

#include "stb\stb_image.h"

namespace Graphics
{
    Texture2DArrayD3D11::Texture2DArrayD3D11(const std::vector<std::string>& filePaths, ID3D11Device* pD3D11Device)
    {
        m_ImageDataPtrs.reserve(filePaths.size());

        // Load from disk into raw RGBA buffers
        for (const auto& filePath : filePaths)
            m_ImageDataPtrs.push_back(stbi_load(filePath.c_str(), &m_Width, &m_Height, nullptr, 4));

        m_IsLoaded = Commit(pD3D11Device);

        for (auto& imageDataPtr : m_ImageDataPtrs)
            stbi_image_free(imageDataPtr);
    }

    bool Texture2DArrayD3D11::Commit(ID3D11Device* pD3D11Device)
    {
        HRESULT hr = S_OK;

        // Create the texture array
        D3D11_TEXTURE2D_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.Width = m_Width;
        desc.Height = m_Height;
        desc.MipLevels = 1;
        desc.ArraySize = m_Count = (UINT)m_ImageDataPtrs.size();
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;

        ComPtr<ID3D11Texture2D> pTextureArray;
        std::vector<D3D11_SUBRESOURCE_DATA> subResources(m_ImageDataPtrs.size());

        for (size_t i = 0; i < subResources.size(); i++)
        {
            ZeroMemory(&subResources[i], sizeof(subResources[i]));
            subResources[i].pSysMem = m_ImageDataPtrs[i];
            subResources[i].SysMemPitch = desc.Width * 4;
            subResources[i].SysMemSlicePitch = 0;
        }

        hr = pD3D11Device->CreateTexture2D(&desc, subResources.data(), &pTextureArray);

        if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
            return false;

        // Create texture array view view
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(srvDesc));
        srvDesc.Format = desc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
        srvDesc.Texture2DArray.MipLevels = desc.MipLevels;
        srvDesc.Texture2DArray.MostDetailedMip = 0;
        srvDesc.Texture2DArray.FirstArraySlice = 0;
        srvDesc.Texture2DArray.ArraySize = desc.ArraySize;

        hr = pD3D11Device->CreateShaderResourceView(pTextureArray.Get(), &srvDesc, &m_SRV);

        if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
            return false;

        return true;
    }
}

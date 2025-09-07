#include "SingleSprite.hpp"
#include "Texture2DD3D11.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "..\ThirdParty\glm\gtc\matrix_transform.hpp"
#include "..\ThirdParty\glm\gtx\euler_angles.hpp"
#define Log(...) printf(__VA_ARGS__)

#include <d3dcompiler.h>
#include <array>
#include <wrl/client.h>
#include <mutex>

using Microsoft::WRL::ComPtr;

// Utility functions
inline bool ErrorCheck(HRESULT hr, const char* file, const char* function, int line)
{
    if (FAILED(hr)) {
        Log("D3D Error: HRESULT 0x%08X in %s::%s (line %d)", hr, file, function, line);
        return false;
    }
    return true;
}

inline std::string GetLastErrorMsg()
{
    DWORD error = GetLastError();
    if (error == 0) return "No error";
    
    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
    
    std::string message(messageBuffer, size);
    LocalFree(messageBuffer);
    return message;
}

namespace Graphics {

    // Static sprite indices (same for all sprites)
    static constexpr std::array<UINT, 6> s_SpriteIndices = { 0, 1, 2, 0, 2, 3 };
    
    // Static sampler cache for optimal performance - shared across all SingleSprite instances
    static ComPtr<ID3D11SamplerState> s_CachedOptimizedSampler;
    static std::mutex s_SamplerCacheMutex;

    SingleSprite::SingleSprite(ID3D11Device* pD3D11Device, UINT width, UINT height, const std::string& texturePath)
        : m_TexturePath(texturePath)
    {
        // Initialize sprite data with default values
        m_SpriteData.Opacity = 1.0f;
        m_SpriteData.Brightness = 1.0f;
        m_SpriteData.Translation = glm::vec3(0.0f, 0.0f, 0.0f);
        m_SpriteData.Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        m_SpriteData.Scale = glm::vec3(1.0f, 1.0f, 1.0f);

        // Initialize cached constant buffers to ensure first update
        memset(&m_CachedVertexCBuffer, 0, sizeof(VERTEX_CONSTANT_BUFFER));
        memset(&m_CachedPixelCBuffer, 0, sizeof(PIXEL_CONSTANT_BUFFER));

        bool initResult = Initialize(pD3D11Device, width, height);

        if (initResult && !texturePath.empty()) {
            // Load texture using your existing Texture2DD3D11 class
            m_pTexture = std::make_shared<::Texture2DD3D11>(texturePath.c_str(), pD3D11Device);
        }
    }

    SingleSprite::~SingleSprite()
    {}

    bool SingleSprite::SetTexture(ID3D11Device* pD3D11Device, const std::string& texturePath)
    {
        if (texturePath.empty()) {
            Log("SingleSprite: Empty texture path provided");
            return false;
        }

        // Use your existing Texture2DD3D11 class
        m_pTexture = std::make_shared<::Texture2DD3D11>(texturePath.c_str(), pD3D11Device);
        if (!m_pTexture) {
            Log("SingleSprite: Failed to load texture from path: %s", texturePath.c_str());
            return false;
        }

        m_TexturePath = texturePath;
        
        // Mark pixel constant buffer as dirty since texture properties may have changed
        m_PixelCBufferDirty = true;
        
        return true;
    }

    bool SingleSprite::SetTexture(std::shared_ptr<::Texture2DD3D11> pTexture)
    {
        if (!pTexture) {
            Log("SingleSprite: Null texture provided");
            return false;
        }

        m_pTexture = pTexture;
        m_TexturePath = ""; // Clear path since texture was provided directly
        
        // Mark pixel constant buffer as dirty since texture properties may have changed
        m_PixelCBufferDirty = true;
        
        return true;
    }

    bool SingleSprite::Initialize(ID3D11Device* pD3D11Device, UINT width, UINT height)
    {
        if (!CreateRenderTargetView(pD3D11Device, width, height)) {
            Log("SingleSprite: CreateRenderTargetView FAILED");
            return false;
        }

        if (!CreateShaders(pD3D11Device)) {
            Log("SingleSprite: CreateShaders FAILED");
            return false;
        }

        if (!CreateStates(pD3D11Device)) {
            Log("SingleSprite: CreateStates FAILED");
            return false;
        }

        if (!CreateVertexBuffer(pD3D11Device)) {
            Log("SingleSprite: CreateVertexBuffer FAILED");
            return false;
        }

        if (!CreateIndexBuffer(pD3D11Device)) {
            Log("SingleSprite: CreateIndexBuffer FAILED");
            return false;
        }

        if (!CreateAndUpdateSpriteInfoBuffer(pD3D11Device)) {
            Log("SingleSprite: CreateAndUpdateSpriteInfoBuffer FAILED");
            return false;
        }

        // Setup viewport
        memset(&m_ViewPort, 0, sizeof(D3D11_VIEWPORT));
        m_ViewPort.Width = (FLOAT)m_RTWidth;
        m_ViewPort.Height = (FLOAT)m_RTHeight;
        m_ViewPort.MinDepth = 0.0f;
        m_ViewPort.MaxDepth = 1.0f;
        m_ViewPort.TopLeftX = m_ViewPort.TopLeftY = 0;

        return true;
    }

    bool SingleSprite::Draw(ID3D11DeviceContext* pD3D11DeviceContext)
    {
        if (!m_pTexture) {
            Log("SingleSprite: No texture set, cannot draw");
            return false;
        }

        if (m_SpriteUpdateQueued) {
            if (!UpdateSpriteInfoBuffer(pD3D11DeviceContext)) {
                Log("SingleSprite: ERROR - Failed to update sprite info buffer");
                return false;
            }
            m_SpriteUpdateQueued = false;
        }

        // Update vertex shader constant buffer only when needed
        {
            VERTEX_CONSTANT_BUFFER cBuffer =
            {
                .MVP = glm::mat4(1.0f) // An orthographic view projection for sprite rendering
            };

            // Check if vertex constant buffer needs updating
            if (m_VertexCBufferDirty || memcmp(&m_CachedVertexCBuffer, &cBuffer, sizeof(VERTEX_CONSTANT_BUFFER)) != 0) {
                if (!UpdateVertexConstantBuffer(pD3D11DeviceContext, cBuffer)) {
                    Log("SingleSprite: Failed to update vertex shader constant buffer");
                    return false;
                }
                m_CachedVertexCBuffer = cBuffer;
                m_VertexCBufferDirty = false;
            }
        }

        // Update pixel shader constant buffer only when needed
        {
            PIXEL_CONSTANT_BUFFER cBuffer =
            {
                .SamplerCount = 1, // Currently only have one sampler
            };

            // Check if pixel constant buffer needs updating
            if (m_PixelCBufferDirty || memcmp(&m_CachedPixelCBuffer, &cBuffer, sizeof(PIXEL_CONSTANT_BUFFER)) != 0) {
                if (!UpdatePixelConstantBuffer(pD3D11DeviceContext, cBuffer)) {
                    Log("SingleSprite: Failed to update pixel shader constant buffer");
                    return false;
                }
                m_CachedPixelCBuffer = cBuffer;
                m_PixelCBufferDirty = false;
            }
        }

        BackupD3D11State(pD3D11DeviceContext);

        // Set Render Target, Viewport, and Scissor Rect
        const D3D11_RECT scissor = { 0, 0, m_RTWidth, m_RTHeight };
        pD3D11DeviceContext->RSSetScissorRects(1, &scissor);
        pD3D11DeviceContext->RSSetViewports(1, &m_ViewPort);
        pD3D11DeviceContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get());

        // Optimized buffer organization - bind pipeline resources in efficient order
        pD3D11DeviceContext->IASetInputLayout(m_pInputLayout.Get());
        pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // Batch vertex buffer binding for better performance
        {
            std::array<ID3D11Buffer*, 2> buffers = { m_pVertexBuffer.Get(), m_pSpriteInfoBuffer.Get() };
            std::array<UINT, 2> strides = { sizeof(Vertex_t), sizeof(VERTEX_SHADER_SPRITE_INFO) };
            std::array<UINT, 2> offsets = { 0, 0 };
            pD3D11DeviceContext->IASetVertexBuffers(0, buffers.size(), buffers.data(), strides.data(), offsets.data());
        }

        pD3D11DeviceContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

        // Set Vertex Shader and Resources
        pD3D11DeviceContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
        pD3D11DeviceContext->VSSetConstantBuffers(0, 1, m_pVertexConstantBuffer.GetAddressOf());

        // Set Pixel Shader and Resources (optimized for single texture sprite rendering)
        ID3D11ShaderResourceView* srv = m_pTexture->GetTexture();
        ID3D11SamplerState* sampler = m_pTextureSampler.Get();
        
        pD3D11DeviceContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
        pD3D11DeviceContext->PSSetConstantBuffers(0, 1, m_pPixelConstantBuffer.GetAddressOf());
        pD3D11DeviceContext->PSSetShaderResources(0, 1, &srv);
        pD3D11DeviceContext->PSSetSamplers(0, 1, &sampler);

        // Disable Unused Shader Stages
        pD3D11DeviceContext->GSSetShader(nullptr, nullptr, 0);
        pD3D11DeviceContext->HSSetShader(nullptr, nullptr, 0);
        pD3D11DeviceContext->DSSetShader(nullptr, nullptr, 0);
        pD3D11DeviceContext->CSSetShader(nullptr, nullptr, 0);

        // Set Blend, Depth-Stencil, and Rasterizer States
        const std::array<float, 4> blendFactor = { 0.0f, 0.0f, 0.0f, 0.0f };
        pD3D11DeviceContext->OMSetBlendState(m_pBlendState.Get(), blendFactor.data(), 0xffffffff);
        pD3D11DeviceContext->OMSetDepthStencilState(m_pDepthStencilStateTransparent.Get(), 0);
        pD3D11DeviceContext->RSSetState(m_pRasterizerState.Get());

        pD3D11DeviceContext->ClearRenderTargetView(m_pRenderTargetView.Get(), blendFactor.data());
        pD3D11DeviceContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        // **INSTANCED DRAW CALL** - Drawing single instance
        pD3D11DeviceContext->DrawIndexedInstanced(
            s_SpriteIndices.size(), // Indices per instance (6 for a quad: 2 triangles)
            1,                      // Number of instances (single sprite)
            0,                      // Start index location
            0,                      // Base vertex location
            0                       // Start instance location
        );

        RestoreD3D11State(pD3D11DeviceContext);
        return true;
    }

    void SingleSprite::Translate(glm::vec3 vector)
    {
        m_SpriteUpdateQueued = true;
        m_SpriteData.Translation += vector;
    }

    void SingleSprite::TranslateTo(glm::vec3 vector)
    {
        m_SpriteUpdateQueued = true;
        m_SpriteData.Translation = vector;
    }

    void SingleSprite::Rotate(glm::vec3 rotation)
    {
        m_SpriteUpdateQueued = true;
        m_SpriteData.Rotation += rotation;
    }

    void SingleSprite::RotateTo(glm::vec3 rotation)
    {
        m_SpriteUpdateQueued = true;
        m_SpriteData.Rotation = rotation;
    }

    void SingleSprite::Scale(glm::vec3 vector)
    {
        m_SpriteUpdateQueued = true;
        m_SpriteData.Scale *= vector;
    }

    void SingleSprite::ScaleTo(glm::vec3 vector)
    {
        m_SpriteUpdateQueued = true;
        m_SpriteData.Scale = vector;
    }

    void SingleSprite::SetTransform(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale)
    {
        m_SpriteUpdateQueued = true;
        m_SpriteData.Translation = translation;
        m_SpriteData.Rotation = rotation;
        m_SpriteData.Scale = scale;
    }

    void SingleSprite::SetOpacity(float opacity)
    {
        m_SpriteUpdateQueued = true;
        m_SpriteData.Opacity = opacity;
    }

    void SingleSprite::SetBrightness(float brightness)
    {
        m_SpriteUpdateQueued = true;
        m_SpriteData.Brightness = brightness;
    }

    bool SingleSprite::Resize(ID3D11Device* pD3D11Device, UINT width, UINT height)
    {
        if (width == m_RTWidth && height == m_RTHeight) {
            return true; // No change needed
        }

        // Release existing render target resources
        m_pRenderTargetView.Reset();
        m_pRenderTargetSRV.Reset();
        m_pDepthStencilBuffer.Reset();
        m_pDepthStencilView.Reset();

        // Recreate with new size
        if (!CreateRenderTargetView(pD3D11Device, width, height)) {
            Log("SingleSprite: Failed to recreate render target view");
            return false;
        }

        // Update viewport
        m_ViewPort.Width = (FLOAT)width;
        m_ViewPort.Height = (FLOAT)height;

        // Mark constant buffers as dirty since render target size may affect projection
        m_VertexCBufferDirty = true;
        m_PixelCBufferDirty = true;

        return true;
    }

    bool SingleSprite::CreateRenderTargetView(ID3D11Device* pD3D11Device, UINT width, UINT height)
    {
        HRESULT hr;

        D3D11_TEXTURE2D_DESC textureDesc = {};
        textureDesc.Width = m_RTWidth = width;
        textureDesc.Height = m_RTHeight = height;
        textureDesc.MipLevels = 1;
        textureDesc.ArraySize = 1;
        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // sRGB format for proper color reproduction
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        textureDesc.CPUAccessFlags = 0; // No CPU access for better GPU performance
        textureDesc.MiscFlags = 0; // No special flags needed for sprites

        ComPtr<ID3D11Texture2D> pRenderTargetTexture;
        hr = pD3D11Device->CreateTexture2D(&textureDesc, nullptr, &pRenderTargetTexture);

        if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
            return false;

        hr = pD3D11Device->CreateRenderTargetView(pRenderTargetTexture.Get(), nullptr, &m_pRenderTargetView);

        if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
            return false;

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        ZeroMemory(&srvDesc, sizeof(srvDesc));

        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // Match the sRGB format
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = 1;

        hr = pD3D11Device->CreateShaderResourceView(pRenderTargetTexture.Get(), &srvDesc, &m_pRenderTargetSRV);

        if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
            return false;

        // Create depth-stencil buffer
        D3D11_TEXTURE2D_DESC depthDesc;
        ZeroMemory(&depthDesc, sizeof(depthDesc));
        depthDesc.Width = m_RTWidth;
        depthDesc.Height = m_RTHeight;
        depthDesc.MipLevels = 1;
        depthDesc.ArraySize = 1;
        depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthDesc.SampleDesc.Count = 1;
        depthDesc.SampleDesc.Quality = 0;
        depthDesc.Usage = D3D11_USAGE_DEFAULT;
        depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthDesc.CPUAccessFlags = 0;
        depthDesc.MiscFlags = 0;

        hr = pD3D11Device->CreateTexture2D(&depthDesc, nullptr, &m_pDepthStencilBuffer);

        if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
            return false;

        hr = pD3D11Device->CreateDepthStencilView(m_pDepthStencilBuffer.Get(), nullptr, &m_pDepthStencilView);

        if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
            return false;

        return true;
    }

    bool SingleSprite::CreateVertexBuffer(ID3D11Device* pD3D11Device)
    {
        HRESULT hr;

        // Same vertices for the sprite quad
        static constexpr std::array<Vertex_t, 4> vertices = {
            Vertex_t{ { -1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } }, // Top-left
            Vertex_t{ {  1.0f,  1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } }, // Top-right  
            Vertex_t{ {  1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } }, // Bottom-right
            Vertex_t{ { -1.0f, -1.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } }  // Bottom-left
        };

        D3D11_BUFFER_DESC bufferDesc = {};
        ZeroMemory(&bufferDesc, sizeof(bufferDesc));

        bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; // Static data, doesn't need to change
        bufferDesc.ByteWidth = m_CurrentVBSize = (UINT)sizeof(vertices);
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufferDesc.CPUAccessFlags = 0; // No CPU access needed

        D3D11_SUBRESOURCE_DATA initData = {};
        ZeroMemory(&initData, sizeof(initData));
        initData.pSysMem = vertices.data();

        hr = pD3D11Device->CreateBuffer(&bufferDesc, &initData, &m_pVertexBuffer);

        if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__)) {
            m_CurrentVBSize = 0;
            return false;
        }

        return true;
    }

    bool SingleSprite::CreateIndexBuffer(ID3D11Device* pD3D11Device)
    {
        HRESULT hr;

        D3D11_BUFFER_DESC bufferDesc = {};
        ZeroMemory(&bufferDesc, sizeof(bufferDesc));

        bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; // Static data
        bufferDesc.ByteWidth = m_CurrentIBSize = (UINT)sizeof(s_SpriteIndices);
        bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bufferDesc.CPUAccessFlags = 0; // No CPU access needed

        D3D11_SUBRESOURCE_DATA initData = {};
        ZeroMemory(&initData, sizeof(initData));
        initData.pSysMem = s_SpriteIndices.data();

        hr = pD3D11Device->CreateBuffer(&bufferDesc, &initData, &m_pIndexBuffer);

        if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__)) {
            m_CurrentIBSize = 0;
            return false;
        }

        return true;
    }

    bool SingleSprite::CreateAndUpdateSpriteInfoBuffer(ID3D11Device* pD3D11Device)
    {
        HRESULT hr;

        // Update the shader info structure
        m_SpriteShaderInfo.Opacity = m_SpriteData.Opacity;
        m_SpriteShaderInfo.Brightness = m_SpriteData.Brightness;
        m_SpriteShaderInfo.Translation = m_SpriteData.Translation;
        m_SpriteShaderInfo.Rotation = m_SpriteData.Rotation;
        m_SpriteShaderInfo.Scale = m_SpriteData.Scale;

        size_t byteSize = sizeof(VERTEX_SHADER_SPRITE_INFO);

        D3D11_BUFFER_DESC bufferDesc;
        ZeroMemory(&bufferDesc, sizeof(bufferDesc));

        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.ByteWidth = m_CurrentSpriteInfoBufferSize = (UINT)byteSize;
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA initData = {};
        ZeroMemory(&initData, sizeof(initData));
        initData.pSysMem = &m_SpriteShaderInfo;

        hr = pD3D11Device->CreateBuffer(&bufferDesc, &initData, &m_pSpriteInfoBuffer);

        if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__)) {
            m_CurrentSpriteInfoBufferSize = 0;
            return false;
        }

        return true;
    }

    bool SingleSprite::UpdateSpriteInfoBuffer(ID3D11DeviceContext* pD3D11DeviceContext)
    {
        HRESULT hr;

        // Update the shader info structure
        m_SpriteShaderInfo.Opacity = m_SpriteData.Opacity;
        m_SpriteShaderInfo.Brightness = m_SpriteData.Brightness;
        m_SpriteShaderInfo.Translation = m_SpriteData.Translation;
        m_SpriteShaderInfo.Rotation = m_SpriteData.Rotation;
        m_SpriteShaderInfo.Scale = m_SpriteData.Scale;

        size_t byteSize = sizeof(VERTEX_SHADER_SPRITE_INFO);

        if (!m_pSpriteInfoBuffer || m_CurrentSpriteInfoBufferSize != byteSize) {
            ID3D11Device* pDevice;
            pD3D11DeviceContext->GetDevice(&pDevice);
            return CreateAndUpdateSpriteInfoBuffer(pDevice);
        }

        D3D11_MAPPED_SUBRESOURCE mappedResource;

        hr = pD3D11DeviceContext->Map(m_pSpriteInfoBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

        if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__)) {
            return false;
        }

        memcpy_s(mappedResource.pData, m_CurrentSpriteInfoBufferSize, &m_SpriteShaderInfo, byteSize);

        pD3D11DeviceContext->Unmap(m_pSpriteInfoBuffer.Get(), 0);
        return true;
    }

    bool SingleSprite::UpdateVertexConstantBuffer(ID3D11DeviceContext* pD3D11DeviceContext, const VERTEX_CONSTANT_BUFFER& cBuffer)
    {
        if (!m_pVertexConstantBuffer) {
            Log("SingleSprite: Vertex constant buffer is null!");
            return false;
        }

        HRESULT hr;
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        hr = pD3D11DeviceContext->Map(m_pVertexConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

        if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__)) {
            Log("SingleSprite: Failed to map vertex constant buffer");
            return false;
        }

        memcpy_s(mappedResource.pData, sizeof(VERTEX_CONSTANT_BUFFER), &cBuffer, sizeof(cBuffer));

        pD3D11DeviceContext->Unmap(m_pVertexConstantBuffer.Get(), 0);
        return true;
    }

    bool SingleSprite::UpdatePixelConstantBuffer(ID3D11DeviceContext* pD3D11DeviceContext, const PIXEL_CONSTANT_BUFFER& cBuffer)
    {
        if (!m_pPixelConstantBuffer) {
            return false;
        }

        HRESULT hr;

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        ZeroMemory(&mappedResource, sizeof(mappedResource));

        hr = pD3D11DeviceContext->Map(m_pPixelConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

        if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
            return false;

        memcpy_s(mappedResource.pData, sizeof(PIXEL_CONSTANT_BUFFER), &cBuffer, sizeof(cBuffer));

        pD3D11DeviceContext->Unmap(m_pPixelConstantBuffer.Get(), 0);

        return true;
    }

    void SingleSprite::InvalidateConstantBuffers()
    {
        m_VertexCBufferDirty = true;
        m_PixelCBufferDirty = true;
    }

    bool SingleSprite::CreateShaders(ID3D11Device* pD3D11Device)
    {
        HRESULT hr;

        // Create the vertex shader (modified from BatchedSprites to work with single texture)
        {
            static const char* vertexShader =
                R"(
                #pragma pack_matrix(row_major)

                cbuffer vertexBuffer : register(b0) 
                {
                    float4x4 ProjectionMatrix;
                };

                struct VS_INPUT
                {
                    // Per-vertex (Slot 0)
                    float3  Pos         : POSITION;
                    float2  UV          : TEXCOORD0;
                    float4  Col         : COLOR0;

                    // Per-instance (Slot 1)
                    float   Opacity     : OPACITY;
                    float   Brightness  : BRIGHTNESS;
                    float3  Translation : TRANSLATION;
                    float3  Rotation    : ROTATION;
                    float3  Scale       : SCALE;
                };

                struct PS_INPUT
                {
                    float4 Pos     : SV_POSITION;
                    float2 UV      : TEXCOORD0;
                    float4 Col     : COLOR0;
                    float  Opacity : OPACITY;
                    float  Bright  : BRIGHTNESS;
                };

                float4x4 CreateTransform(float3 translation, float3 eulerAngles, float3 scale)
                {
                    float4x4 scaleMatrix = float4x4(
                        scale.x, 0.0f,    0.0f,    0.0f,
                        0.0f,    scale.y, 0.0f,    0.0f,
                        0.0f,    0.0f,    scale.z, 0.0f,
                        0.0f,    0.0f,    0.0f,    1.0f
                    );
                    
                    // Convert Euler angles to rotation matrix
                    float cx = cos(eulerAngles.x);
                    float sx = sin(eulerAngles.x);
                    float cy = cos(eulerAngles.y);
                    float sy = sin(eulerAngles.y);
                    float cz = cos(eulerAngles.z);
                    float sz = sin(eulerAngles.z);
                    
                    float4x4 rotationMatrix = float4x4(
                        cy * cz,                -cy * sz,               sy,      0.0f,
                        sx * sy * cz + cx * sz, -sx * sy * sz + cx * cz, -sx * cy, 0.0f,
                        -cx * sy * cz + sx * sz, cx * sy * sz + sx * cz,  cx * cy,  0.0f,
                        0.0f,                   0.0f,                   0.0f,    1.0f
                    );

                    float4x4 translationMatrix = float4x4(
                        1.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 1.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 1.0f, 0.0f,
                        translation.x, translation.y, translation.z, 1.0f
                    );

                    return mul(mul(scaleMatrix, rotationMatrix), translationMatrix);
                }

                PS_INPUT main(VS_INPUT input)
                {
                    PS_INPUT output;

                    float4x4 TransformationMatrix = CreateTransform(input.Translation, input.Rotation, input.Scale);
                    float4   origin               = float4(input.Pos, 1.0f);

                    output.Pos     = mul(mul(origin, TransformationMatrix), ProjectionMatrix);
                    output.Col     = input.Col;
                    output.UV      = input.UV;
                    output.Opacity = input.Opacity;
                    output.Bright  = input.Brightness;

                    return output;
                }
                )";

            ComPtr<ID3DBlob> pVertexShaderBlob;
            ComPtr<ID3DBlob> pErrorBlob;

            hr = D3DCompile(vertexShader, strlen(vertexShader), NULL, NULL, NULL, "main", "vs_4_0", 0, 0, &pVertexShaderBlob, &pErrorBlob);

            if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__)) {
                Log(GetLastErrorMsg().c_str());
                if (pErrorBlob) {
                    Log("SingleSprite: Vertex shader compilation FAILED: %s", (const char*)pErrorBlob->GetBufferPointer());
                }
                return false;
            }

            hr = pD3D11Device->CreateVertexShader(pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), NULL, &m_pVertexShader);

            if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__)) {
                Log(GetLastErrorMsg().c_str());
                return false;
            }

            hr = pD3D11Device->CreateInputLayout(
                s_VertexInputLayout,
                sizeof(s_VertexInputLayout) / sizeof(D3D11_INPUT_ELEMENT_DESC),
                pVertexShaderBlob->GetBufferPointer(),
                pVertexShaderBlob->GetBufferSize(),
                &m_pInputLayout
            );

            if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__)) {
                Log(GetLastErrorMsg().c_str());
                return false;
            }
        }

        // Create the pixel shader (modified from BatchedSprites to work with single texture)
        {
            static const char* pixelShader =
                R"(
                cbuffer boundInfo : register(b0)
                {
                    uint SamplerCount;
                };

                struct PS_INPUT
                {
                    float4 Pos     : SV_POSITION;
                    float2 UV      : TEXCOORD0;
                    float4 Col     : COLOR0;
                    float  Opacity : OPACITY;
                    float  Bright  : BRIGHTNESS;
                };

                sampler sampler0 : register(s0);
                Texture2D texture0 : register(t0);
    
                float4 main(PS_INPUT input) : SV_Target
                {
                    float2 texelSize = 1.0 / 1024.0;
                    
                    float4 centerSample = texture0.Sample(sampler0, input.UV);
                    
                    float4 rightSample = texture0.Sample(sampler0, input.UV + float2(texelSize.x, 0));
                    float4 leftSample = texture0.Sample(sampler0, input.UV + float2(-texelSize.x, 0));
                    float4 upSample = texture0.Sample(sampler0, input.UV + float2(0, texelSize.y));
                    float4 downSample = texture0.Sample(sampler0, input.UV + float2(0, -texelSize.y));
                    
                    float4 result = centerSample * 0.45 + 
                                   (rightSample + leftSample + upSample + downSample) * 0.1375;
                    
                    return result;
                }
                )";

            ComPtr<ID3DBlob> pPixelShaderBlob;
            ComPtr<ID3DBlob> pErrorBlob;

            hr = D3DCompile(pixelShader, strlen(pixelShader), NULL, NULL, NULL, "main", "ps_4_0", 0, 0, &pPixelShaderBlob, &pErrorBlob);

            if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__)) {
                Log(GetLastErrorMsg().c_str());
                if (pErrorBlob) {
                    Log("SingleSprite: Pixel shader compilation FAILED: %s", (const char*)pErrorBlob->GetBufferPointer());
                }
                return false;
            }

            hr = pD3D11Device->CreatePixelShader(pPixelShaderBlob->GetBufferPointer(), pPixelShaderBlob->GetBufferSize(), NULL, &m_pPixelShader);

            if (FAILED(hr)) {
                Log("SingleSprite: CreatePixelShader FAILED with HRESULT: 0x%08X", hr);
                return false;
            }
        }

        // Create the constant buffer for the vertex shader
        {
            D3D11_BUFFER_DESC desc;
            desc.ByteWidth = sizeof(VERTEX_CONSTANT_BUFFER);
            desc.Usage = D3D11_USAGE_DYNAMIC;
            desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            desc.MiscFlags = 0;

            hr = pD3D11Device->CreateBuffer(&desc, NULL, &m_pVertexConstantBuffer);

            if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
                return false;
        }

        // Create the constant buffer for the pixel shader
        {
            D3D11_BUFFER_DESC desc;
            desc.ByteWidth = sizeof(PIXEL_CONSTANT_BUFFER);
            desc.Usage = D3D11_USAGE_DYNAMIC;
            desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            desc.MiscFlags = 0;

            hr = pD3D11Device->CreateBuffer(&desc, NULL, &m_pPixelConstantBuffer);

            if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
                return false;
        }

        return true;
    }

    bool SingleSprite::CreateStates(ID3D11Device* pD3D11Device)
    {
        HRESULT hr;

        // Create the blending setup
        {
            D3D11_BLEND_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.AlphaToCoverageEnable = false;
            desc.RenderTarget[0].BlendEnable = false; // Disable blending to test color reproduction
            desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
            desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
            desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
            desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
            desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

            hr = pD3D11Device->CreateBlendState(&desc, &m_pBlendState);

            if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
                return false;
        }

        // Create the rasterizer state
        {
            D3D11_RASTERIZER_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.FillMode = D3D11_FILL_SOLID;
            desc.CullMode = D3D11_CULL_NONE;
            desc.ScissorEnable = true;
            desc.DepthClipEnable = true;

            hr = pD3D11Device->CreateRasterizerState(&desc, &m_pRasterizerState);

            if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
                return false;
        }

        // Create depth-stencil States
        {
            D3D11_DEPTH_STENCIL_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.DepthEnable = true;
            desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

            hr = pD3D11Device->CreateDepthStencilState(&desc, &m_pDepthStencilStateOpaque);

            if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
                return false;

            ZeroMemory(&desc, sizeof(desc));
            desc.DepthEnable = true;
            desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
            desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
            hr = pD3D11Device->CreateDepthStencilState(&desc, &m_pDepthStencilStateTransparent);

            if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
                return false;
        }

        // Create ultra-optimized texture sampler specifically for PNG sprite rendering
        {
            std::lock_guard<std::mutex> lock(s_SamplerCacheMutex);
            
            // Check if we already have a cached optimized sampler
            if (!s_CachedOptimizedSampler) {
                D3D11_SAMPLER_DESC desc;
                ZeroMemory(&desc, sizeof(desc));
                
                // Use point filtering for pixel-perfect PNG sprite rendering
                // Since we have 5-sample multisampling in the shader, point filtering gives crisp results
                desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
                
                // Clamp addressing - perfect for PNG sprites, prevents wrapping artifacts
                desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
                desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
                desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
                
                // Minimal settings for maximum performance
                desc.MipLODBias = 0.0f;
                desc.MaxAnisotropy = 1; // Not used with point filtering
                desc.ComparisonFunc = D3D11_COMPARISON_NEVER; // Most efficient for sprites
                
                // PNG textures are single-mip, disable all LOD processing
                desc.MinLOD = 0.0f;
                desc.MaxLOD = 0.0f; // Force base mip only, eliminates LOD calculations entirely
                
                // Border color not needed with CLAMP addressing (optimization: removed unnecessary setup)

                hr = pD3D11Device->CreateSamplerState(&desc, &s_CachedOptimizedSampler);

                if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
                    return false;
            }
            
            // Use the cached sampler (AddRef for this instance)
            m_pTextureSampler = s_CachedOptimizedSampler;
        }

        return true;
    }

    void SingleSprite::BackupD3D11State(ID3D11DeviceContext* ctx)
    {
        m_D3D11StateBackup.ScissorRectsCount = m_D3D11StateBackup.ViewportsCount =
            D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;

        ctx->RSGetScissorRects(&m_D3D11StateBackup.ScissorRectsCount, m_D3D11StateBackup.ScissorRects);
        ctx->RSGetViewports(&m_D3D11StateBackup.ViewportsCount, m_D3D11StateBackup.Viewports);
        ctx->RSGetState(&m_D3D11StateBackup.RS);
        ctx->OMGetBlendState(&m_D3D11StateBackup.BlendState, m_D3D11StateBackup.BlendFactor, &m_D3D11StateBackup.SampleMask);
        ctx->OMGetDepthStencilState(&m_D3D11StateBackup.DepthStencilState, &m_D3D11StateBackup.StencilRef);
        ctx->PSGetShaderResources(0, 1, &m_D3D11StateBackup.PSShaderResource);
        ctx->PSGetSamplers(0, 1, &m_D3D11StateBackup.PSSampler);
        m_D3D11StateBackup.PSInstancesCount = m_D3D11StateBackup.VSInstancesCount = m_D3D11StateBackup.GSInstancesCount = 256;
        ctx->PSGetShader(&m_D3D11StateBackup.PS, m_D3D11StateBackup.PSInstances, &m_D3D11StateBackup.PSInstancesCount);
        ctx->VSGetShader(&m_D3D11StateBackup.VS, m_D3D11StateBackup.VSInstances, &m_D3D11StateBackup.VSInstancesCount);
        ctx->VSGetConstantBuffers(0, 1, &m_D3D11StateBackup.VSConstantBuffer);
        ctx->GSGetShader(&m_D3D11StateBackup.GS, m_D3D11StateBackup.GSInstances, &m_D3D11StateBackup.GSInstancesCount);

        ctx->IAGetPrimitiveTopology(&m_D3D11StateBackup.PrimitiveTopology);
        ctx->IAGetIndexBuffer(&m_D3D11StateBackup.IndexBuffer, &m_D3D11StateBackup.IndexBufferFormat, &m_D3D11StateBackup.IndexBufferOffset);
        ctx->IAGetVertexBuffers(0, 1, &m_D3D11StateBackup.VertexBuffer, &m_D3D11StateBackup.VertexBufferStride, &m_D3D11StateBackup.VertexBufferOffset);
        ctx->IAGetInputLayout(&m_D3D11StateBackup.InputLayout);
    }

    void SingleSprite::RestoreD3D11State(ID3D11DeviceContext* ctx)
    {
        ctx->RSSetScissorRects(m_D3D11StateBackup.ScissorRectsCount, m_D3D11StateBackup.ScissorRects);
        ctx->RSSetViewports(m_D3D11StateBackup.ViewportsCount, m_D3D11StateBackup.Viewports);
        ctx->RSSetState(m_D3D11StateBackup.RS); if (m_D3D11StateBackup.RS) m_D3D11StateBackup.RS->Release();
        ctx->OMSetBlendState(m_D3D11StateBackup.BlendState, m_D3D11StateBackup.BlendFactor, m_D3D11StateBackup.SampleMask); if (m_D3D11StateBackup.BlendState) m_D3D11StateBackup.BlendState->Release();
        ctx->OMSetDepthStencilState(m_D3D11StateBackup.DepthStencilState, m_D3D11StateBackup.StencilRef); if (m_D3D11StateBackup.DepthStencilState) m_D3D11StateBackup.DepthStencilState->Release();
        ctx->PSSetShaderResources(0, 1, &m_D3D11StateBackup.PSShaderResource); if (m_D3D11StateBackup.PSShaderResource) m_D3D11StateBackup.PSShaderResource->Release();
        ctx->PSSetSamplers(0, 1, &m_D3D11StateBackup.PSSampler); if (m_D3D11StateBackup.PSSampler) m_D3D11StateBackup.PSSampler->Release();
        ctx->PSSetShader(m_D3D11StateBackup.PS, m_D3D11StateBackup.PSInstances, m_D3D11StateBackup.PSInstancesCount); if (m_D3D11StateBackup.PS) m_D3D11StateBackup.PS->Release();
        for (UINT i = 0; i < m_D3D11StateBackup.PSInstancesCount; i++) if (m_D3D11StateBackup.PSInstances[i]) m_D3D11StateBackup.PSInstances[i]->Release();
        ctx->VSSetShader(m_D3D11StateBackup.VS, m_D3D11StateBackup.VSInstances, m_D3D11StateBackup.VSInstancesCount); if (m_D3D11StateBackup.VS) m_D3D11StateBackup.VS->Release();
        ctx->VSSetConstantBuffers(0, 1, &m_D3D11StateBackup.VSConstantBuffer); if (m_D3D11StateBackup.VSConstantBuffer) m_D3D11StateBackup.VSConstantBuffer->Release();
        ctx->GSSetShader(m_D3D11StateBackup.GS, m_D3D11StateBackup.GSInstances, m_D3D11StateBackup.GSInstancesCount); if (m_D3D11StateBackup.GS) m_D3D11StateBackup.GS->Release();
        for (UINT i = 0; i < m_D3D11StateBackup.VSInstancesCount; i++) if (m_D3D11StateBackup.VSInstances[i]) m_D3D11StateBackup.VSInstances[i]->Release();
        ctx->IASetPrimitiveTopology(m_D3D11StateBackup.PrimitiveTopology);
        ctx->IASetIndexBuffer(m_D3D11StateBackup.IndexBuffer, m_D3D11StateBackup.IndexBufferFormat, m_D3D11StateBackup.IndexBufferOffset); if (m_D3D11StateBackup.IndexBuffer) m_D3D11StateBackup.IndexBuffer->Release();
        ctx->IASetVertexBuffers(0, 1, &m_D3D11StateBackup.VertexBuffer, &m_D3D11StateBackup.VertexBufferStride, &m_D3D11StateBackup.VertexBufferOffset); if (m_D3D11StateBackup.VertexBuffer) m_D3D11StateBackup.VertexBuffer->Release();
        ctx->IASetInputLayout(m_D3D11StateBackup.InputLayout); if (m_D3D11StateBackup.InputLayout) m_D3D11StateBackup.InputLayout->Release();
    }

    // Simple ImGui rendering using our optimized texture directly
    void SingleSprite::RenderToInternalRT(ImVec2 size)
    {
        if (!m_pTexture) {
            return;
        }

        // Get current ImGui draw list
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        
        // Calculate position and size with transforms
        ImVec2 final_size = ImVec2(
            size.x * m_SpriteData.Scale.x, 
            size.y * m_SpriteData.Scale.y
        );

        // Get current cursor position as base position
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 final_pos = ImVec2(
            pos.x + m_SpriteData.Translation.x,
            pos.y + m_SpriteData.Translation.y
        );

        // Apply opacity (no colorize since you said to forget about it)
        ImU32 tint_color = IM_COL32(255, 255, 255, (int)(m_SpriteData.Opacity * 255.0f));

        // Render using ImGui (this will use our optimized sampler configs automatically)
        if (m_SpriteData.Rotation.z != 0.0f) {
            // Rotated rendering
            ImVec2 center_pos = ImVec2(final_pos.x + final_size.x * 0.5f, final_pos.y + final_size.y * 0.5f);
            float rotation = m_SpriteData.Rotation.z;
            
            draw_list->AddImageQuad(
                (ImTextureID)m_pTexture->GetTexture(),
                // Calculate rotated corners
                ImVec2(center_pos.x + cosf(rotation) * (-final_size.x * 0.5f) - sinf(rotation) * (-final_size.y * 0.5f),
                       center_pos.y + sinf(rotation) * (-final_size.x * 0.5f) + cosf(rotation) * (-final_size.y * 0.5f)),
                ImVec2(center_pos.x + cosf(rotation) * (final_size.x * 0.5f) - sinf(rotation) * (-final_size.y * 0.5f),
                       center_pos.y + sinf(rotation) * (final_size.x * 0.5f) + cosf(rotation) * (-final_size.y * 0.5f)),
                ImVec2(center_pos.x + cosf(rotation) * (final_size.x * 0.5f) - sinf(rotation) * (final_size.y * 0.5f),
                       center_pos.y + sinf(rotation) * (final_size.x * 0.5f) + cosf(rotation) * (final_size.y * 0.5f)),
                ImVec2(center_pos.x + cosf(rotation) * (-final_size.x * 0.5f) - sinf(rotation) * (final_size.y * 0.5f),
                       center_pos.y + sinf(rotation) * (-final_size.x * 0.5f) + cosf(rotation) * (final_size.y * 0.5f)),
                ImVec2(0, 0), ImVec2(1, 0), ImVec2(1, 1), ImVec2(0, 1),
                tint_color
            );
        } else {
            // Simple non-rotated rendering
            draw_list->AddImage(
                (ImTextureID)m_pTexture->GetTexture(),
                final_pos,
                ImVec2(final_pos.x + final_size.x, final_pos.y + final_size.y),
                ImVec2(0, 0), ImVec2(1, 1),
                tint_color
            );
        }

        // Advance ImGui cursor
        ImGui::SetCursorScreenPos(ImVec2(final_pos.x + final_size.x, final_pos.y + final_size.y));
    }

    void SingleSprite::CleanupStaticResources()
    {
        std::lock_guard<std::mutex> lock(s_SamplerCacheMutex);
        s_CachedOptimizedSampler.Reset();
    }

}

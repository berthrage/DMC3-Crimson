#include "BatchedSprites.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtx\euler_angles.hpp"

#include <d3dcompiler.h>

namespace Graphics
{
    BatchedSprites::BatchedSprites(ID3D11Device* pD3D11Device, UINT width, UINT height, const std::vector<Sprite>& sprites,
        const std::vector<size_t> spriteIndices)
    {
        Initialize(pD3D11Device, width, height);
        UpdateSprites(pD3D11Device, sprites, spriteIndices);
    }

    BatchedSprites::~BatchedSprites()
    {}

    void BatchedSprites::UpdateSprites(ID3D11Device* pD3D11Device, const std::vector<Sprite>& sprites,
        const std::vector<size_t> spriteIndices)
    {
        m_UpdateVertexBuffer = true;
        m_UpdateIndexBuffer = true;
        m_SpriteInfoUpdateQueued = true;

        m_Vertices.clear();
        m_Indices.clear();
        m_SpritesData.clear();
        m_TexturePaths.clear();
        m_pTextureArray.reset();

        const auto count = sprites.size();

        m_Vertices.reserve(count);
        m_Indices.reserve(count);
        m_SpritesData.reserve(count);

        for (size_t i = 0; i < count; i++) {
            const auto& spriteVb = sprites[i].GetVertices();
            const auto& spriteIb = sprites[i].GetIndices();
            const auto& spriteTr = sprites[i].GetTransformationmatrix();

            const auto& pathIdx = FindTexturePathIndex(sprites[i].GetTexturePath());

            if (pathIdx.has_value()) {
                m_SpritesData.push_back(
                    SpriteInfo{
                        pathIdx.value(),
                        m_Vertices.size(),
                        m_Indices.size(),
                        1.0f,
                        1.0f,
                        spriteTr
                    }
                );
            }
            else {
                m_SpritesData.push_back(
                    SpriteInfo{
                        m_TexturePaths.size(),
                        m_Vertices.size(),
                        m_Indices.size(),
                        1.0f,
                        1.0f,
                        spriteTr
                    }
                );

                m_TexturePaths.push_back(sprites[i].GetTexturePath());
            }

            auto newIb = spriteIb;

            for (auto& idx : newIb)
                idx += (uint32_t)m_Vertices.size();

            m_Vertices.insert(m_Vertices.end(), spriteVb.begin(), spriteVb.end());
            m_Indices.insert(m_Indices.end(), newIb.begin(), newIb.end());
        }

        m_pTextureArray = std::make_unique<Texture2DArrayD3D11>(m_TexturePaths, pD3D11Device);
        m_CustomSpriteIndices = spriteIndices;

        PopulateActiveIndices();
    }

    void BatchedSprites::SetActiveSprites(const std::vector<size_t> spriteIndices)
    {
        m_CustomSpriteIndices = spriteIndices;

        PopulateActiveIndices();
    }

    bool BatchedSprites::Initialize(ID3D11Device* pD3D11Device, UINT width, UINT height)
    {
        if (!CreateRenderTargetView(pD3D11Device, width, height))
        {
            return false;
        }

        HRESULT hr;

        // Create the vertex shader
        {
            static const char* vertexShader =
                R"(
                cbuffer vertexBuffer : register(b0) 
                {
                    float4x4 ProjectionMatrix;
                    uint     VertexPerSprite;
                    uint     IndexPerSprite;
                };

                struct SpriteInfo
                {
                    uint     TextureIdx;
                    float    Opacity;
                    float    Brightness;
                    float4x4 TransformationMatrix;
                };

                StructuredBuffer<SpriteInfo> spriteInfo: register(t0);

                struct VS_INPUT
                {
                    float3 Pos : POSITION;
                    float2 UV  : TEXCOORD0;
                    float4 Col : COLOR0;
                };
                
                struct PS_INPUT
                {
                    float4 Pos     : SV_POSITION;
                    float2 UV      : TEXCOORD0;
                    float4 Col     : COLOR0;
                    uint   Tid     : TEXTURE_IDX;
                    float  Opacity : OPACITY;
                    float  Bright  : BRIGHTNESS;
                };
                
                PS_INPUT main(VS_INPUT input, uint vertexID : SV_VertexID)
                {
                    SpriteInfo si = spriteInfo[vertexID / VertexPerSprite];

                    PS_INPUT output;
                    output.Pos = mul(ProjectionMatrix, mul(si.TransformationMatrix, float4(input.Pos, 1.0f)));
                    output.Col = input.Col;
                    output.UV  = input.UV;
                    output.Tid = si.TextureIdx;
                    output.Opacity = si.Opacity;
                    output.Bright = si.Brightness;

                    return output;
                }
                )";

            ComPtr<ID3DBlob> pVertexShaderBlob;

            hr = D3DCompile(vertexShader, strlen(vertexShader), NULL, NULL, NULL, "main", "vs_4_0", 0, 0, &pVertexShaderBlob, NULL);

            if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
                return false; // NB: Pass ID3DBlob* pErrorBlob to D3DCompile() to get error showing in (const char*)pErrorBlob->GetBufferPointer(). Make sure to Release() the blob!

            hr = pD3D11Device->CreateVertexShader(pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), NULL, &m_pVertexShader);

            if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
            {
                return false;
            }

            hr = pD3D11Device->CreateInputLayout(s_VertexInputLayout,
                sizeof(s_VertexInputLayout) / sizeof(D3D11_INPUT_ELEMENT_DESC),
                pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), &m_pInputLayout);

            if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
                return false;
        }

        // Create the pixel shader
        {
            static const char* pixelShader =
                R"(
                cbuffer MyCB : register(b0)
                {
                    uint TextureCount;
                    uint SamplerCount;
                };

                struct PS_INPUT
                {
                    float4 Pos     : SV_POSITION;
                    float2 UV      : TEXCOORD0;
                    float4 Col     : COLOR0;
                    uint   Tid     : TEXTURE_IDX;
                    float  Opacity : OPACITY;
                    float  Bright  : BRIGHTNESS;
                };

                sampler sampler0 : register(s0);
                Texture2DArray textureArray0 : register(t0);
                
                float4 main(PS_INPUT input) : SV_Target
                {
                    float4 outCol = input.Col * textureArray0.Sample(sampler0, float3(input.UV, input.Tid));

                    outCol = float4(outCol.xyz * input.Bright, outCol.w);

                    outCol.w *= input.Opacity;

                    return outCol;
                }
                )";

            ComPtr<ID3DBlob> pPixelShaderBlob;

            hr = D3DCompile(pixelShader, strlen(pixelShader), NULL, NULL, NULL, "main", "ps_4_0", 0, 0, &pPixelShaderBlob, NULL);

            if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
                return false; // NB: Pass ID3DBlob* pErrorBlob to D3DCompile() to get error showing in (const char*)pErrorBlob->GetBufferPointer(). Make sure to Release() the blob!

            hr = pD3D11Device->CreatePixelShader(pPixelShaderBlob->GetBufferPointer(), pPixelShaderBlob->GetBufferSize(), NULL, &m_pPixelShader);

            if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
                return false;
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

        // Create the constat buffer for the pixel shader
        {
            D3D11_BUFFER_DESC desc;
            desc.ByteWidth = sizeof(VERTEX_CONSTANT_BUFFER);
            desc.Usage = D3D11_USAGE_DYNAMIC;
            desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            desc.MiscFlags = 0;

            hr = pD3D11Device->CreateBuffer(&desc, NULL, &m_pPixelConstantBuffer);

            if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
                return false;
        }

        // Create the blending setup
        {
            D3D11_BLEND_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.AlphaToCoverageEnable = false;
            desc.RenderTarget[0].BlendEnable = true;
            desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; // Source color is blended based on its alpha
            desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; // Destination color is blended based on inverse source alpha
            desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; // Add source and destination colors
            desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE; // Source alpha is fully preserved
            desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE; // Destination alpha is fully preserved
            desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX; // Use maximum of source and destination alpha
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

        // Create depth-stencil State
        {
            D3D11_DEPTH_STENCIL_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.DepthEnable = true;
            desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
            //desc.StencilEnable = false;
            //desc.FrontFace.StencilFailOp = desc.FrontFace.StencilDepthFailOp = desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
            //desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
            //desc.BackFace = desc.FrontFace;

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

        // Create depth-stencil
        {
            D3D11_TEXTURE2D_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.Width = m_RTWidth;
            desc.Height = m_RTHeight;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
            desc.CPUAccessFlags = 0;
            desc.MiscFlags = 0;

            hr = pD3D11Device->CreateTexture2D(&desc, nullptr, &m_pDepthStincilBuffer);

            if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
                return false;

            hr = pD3D11Device->CreateDepthStencilView(m_pDepthStincilBuffer.Get(), nullptr, &m_pDepthStincilView);

            if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
                return false;
        }

        // Create texture sampler
        {
            D3D11_SAMPLER_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
            desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
            desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
            desc.MipLODBias = 0.0f;
            desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
            desc.MinLOD = 0.0f;
            desc.MaxLOD = 0.0f;

            hr = pD3D11Device->CreateSamplerState(&desc, &m_pTextureSampler);

            if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
                return false;
        }

        // Create viewport
        {
            // Setup viewport
            memset(&m_VP, 0, sizeof(D3D11_VIEWPORT));
            m_VP.Width = (FLOAT)m_RTWidth;
            m_VP.Height = (FLOAT)m_RTHeight;
            m_VP.MinDepth = 0.0f;
            m_VP.MaxDepth = 1.0f;
            m_VP.TopLeftX = m_VP.TopLeftY = 0;
        }

        return true;
    }

    bool BatchedSprites::Draw(ID3D11DeviceContext* pD3D11DeviceContext)
    {
        if (m_UpdateVertexBuffer)
        {
            if (!UpdateVertexBuffer(pD3D11DeviceContext))
                return false;

            m_UpdateVertexBuffer = false;
        }

        if (m_UpdateIndexBuffer)
        {
            if (!UpdateIndexBuffer(pD3D11DeviceContext))
                return false;

            m_UpdateIndexBuffer = false;
        }

        if (m_SpriteInfoUpdateQueued)
        {
            if (!UpdateSpriteInfoBuffer(pD3D11DeviceContext))
                return false;

            m_SpriteInfoUpdateQueued = false;
        }

        {
            VERTEX_CONSTANT_BUFFER cBuffer = { glm::mat4(1.0f), s_VertexPerSprite, s_IndexPerSprite };

            if (!UpdateVertexConstantBuffer(pD3D11DeviceContext, cBuffer))
                return false;
        }

        {
            PIXEL_CONSTANT_BUFFER cBuffer =
            {
                m_pTextureArray->GetCount(),
                1
            };

            if (!UpdatePixelConstantBuffer(pD3D11DeviceContext, cBuffer))
                return false;
        }

        BackupD3D11State(pD3D11DeviceContext);

        const D3D11_RECT scissor = { 0, 0, m_RTWidth, m_RTHeight };
        pD3D11DeviceContext->RSSetScissorRects(1, &scissor);
        pD3D11DeviceContext->RSSetViewports(1, &m_VP);

        pD3D11DeviceContext->IASetInputLayout(m_pInputLayout.Get());

        UINT stride = sizeof(decltype(m_Vertices)::value_type); // Size of each vertex
        UINT offset = 0; // Offset in the vertex buffer
        pD3D11DeviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
        pD3D11DeviceContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        pD3D11DeviceContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
        pD3D11DeviceContext->VSSetShaderResources(0, 1, m_pTIBSRV.GetAddressOf());
        pD3D11DeviceContext->VSSetConstantBuffers(0, 1, m_pVertexConstantBuffer.GetAddressOf());

        auto srv = m_pTextureArray->GetSrv();
        pD3D11DeviceContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
        pD3D11DeviceContext->PSSetConstantBuffers(0, 1, m_pPixelConstantBuffer.GetAddressOf());
        pD3D11DeviceContext->PSSetShaderResources(0, 1, &srv);
        pD3D11DeviceContext->PSSetSamplers(0, 1, m_pTextureSampler.GetAddressOf());

        pD3D11DeviceContext->GSSetShader(nullptr, nullptr, 0);
        pD3D11DeviceContext->HSSetShader(nullptr, nullptr, 0);
        pD3D11DeviceContext->DSSetShader(nullptr, nullptr, 0);
        pD3D11DeviceContext->CSSetShader(nullptr, nullptr, 0);

        const float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        pD3D11DeviceContext->OMSetBlendState(m_pBlendState.Get(), blendFactor, 0xffffffff);
        pD3D11DeviceContext->OMSetDepthStencilState(m_pDepthStencilStateTransparent.Get(), 0);
        pD3D11DeviceContext->RSSetState(m_pRasterizerState.Get());

        pD3D11DeviceContext->OMSetRenderTargets(1, m_pRTV.GetAddressOf(), m_pDepthStincilView.Get());

        const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        pD3D11DeviceContext->ClearRenderTargetView(m_pRTV.Get(), clearColor);
        pD3D11DeviceContext->ClearDepthStencilView(m_pDepthStincilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        pD3D11DeviceContext->DrawIndexed((UINT)m_ActiveIndices.size(), 0, 0);

        RestoreD3D11State(pD3D11DeviceContext);

        return true;
    }

    std::optional<size_t> BatchedSprites::FindTexturePathIndex(const std::string& path)
    {
        const auto& count = m_TexturePaths.size();
        for (size_t i = 0; i < count; i++)
        {
            if (m_TexturePaths[i] == path)
                return i;
        }

        return std::nullopt;
    }

    void BatchedSprites::PopulateActiveIndices()
    {
        m_ActiveIndices.clear();
        m_ActiveIndices.reserve(m_CustomSpriteIndices.size() * s_IndexPerSprite);

        if (m_CustomSpriteIndices.size())
        {
            for (const auto& spriteIdx : m_CustomSpriteIndices)
            {
                for (size_t indexIdx = 0; indexIdx < s_IndexPerSprite; indexIdx++)
                {
                    m_ActiveIndices.push_back(m_Indices[m_SpritesData[spriteIdx].IndicesOffset + indexIdx]);
                }
            }
        }
        else
        {
            m_ActiveIndices = m_Indices;
        }

        m_UpdateIndexBuffer = true;
    }

    //void BatchedSprites::SortByDepth()
    //{
    //
    //}

    bool BatchedSprites::CreateRenderTargetView(ID3D11Device* pD3D11Device, UINT width, UINT height)
    {
        HRESULT hr;

        D3D11_TEXTURE2D_DESC textureDesc = {};
        textureDesc.Width = m_RTWidth = width;
        textureDesc.Height = m_RTHeight = height;
        textureDesc.MipLevels = 1;
        textureDesc.ArraySize = 1;
        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        textureDesc.SampleDesc.Count = 1; // Set multisampling
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        textureDesc.CPUAccessFlags = 0; // No CPU access needed

        ComPtr<ID3D11Texture2D> pRenderTargetTexture;
        hr = pD3D11Device->CreateTexture2D(&textureDesc, nullptr, &pRenderTargetTexture);

        if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
            return false;

        hr = pD3D11Device->CreateRenderTargetView(pRenderTargetTexture.Get(), nullptr, &m_pRTV);

        if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
            return false;

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        ZeroMemory(&srvDesc, sizeof(srvDesc));

        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = 1;

        hr = pD3D11Device->CreateShaderResourceView(pRenderTargetTexture.Get(), &srvDesc, &m_pRTSRV);

        if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
            return false;

        return true;
    }

    bool BatchedSprites::CreateAndUpdateVertexBuffer(ID3D11Device* pD3D11Device)
    {
        HRESULT hr;

        D3D11_BUFFER_DESC bufferDesc = {};
        ZeroMemory(&bufferDesc, sizeof(bufferDesc));

        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.ByteWidth = m_CurrentVBSize = (UINT)m_Vertices.size() * sizeof(decltype(m_Vertices)::value_type);
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        D3D11_SUBRESOURCE_DATA initData = {};
        ZeroMemory(&initData, sizeof(initData));

        initData.pSysMem = m_Vertices.data();

        hr = pD3D11Device->CreateBuffer(&bufferDesc, &initData, &m_pVertexBuffer);

        if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
        {
            m_CurrentVBSize = 0;
            return false;
        }

        return true;
    }

    bool BatchedSprites::CreateAndUpdateIndexBuffer(ID3D11Device* pD3D11Device)
    {
        HRESULT hr;

        const size_t indicesSize = m_ActiveIndices.size() * sizeof(decltype(m_ActiveIndices)::value_type);

        D3D11_BUFFER_DESC bufferDesc = {};
        ZeroMemory(&bufferDesc, sizeof(bufferDesc));

        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.ByteWidth = m_CurrentIBSize = (UINT)indicesSize;
        bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        D3D11_SUBRESOURCE_DATA initData = {};
        ZeroMemory(&initData, sizeof(initData));

        initData.pSysMem = m_ActiveIndices.data();

        hr = pD3D11Device->CreateBuffer(&bufferDesc, &initData, &m_pIndexBuffer);

        if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
        {
            m_CurrentIBSize = 0;
            return false;
        }

        return true;
    }

    bool BatchedSprites::CreateAndUpdateSpriteInfoBuffer(ID3D11Device* pD3D11Device)
    {
        HRESULT hr;

        std::vector<VERTEX_SHADER_SPRITE_INFO> infoArray;
        infoArray.reserve(m_SpritesData.size());

        for (const auto& sd : m_SpritesData)
            infoArray.push_back(VERTEX_SHADER_SPRITE_INFO{ (uint32_t)sd.TexturePathIdx, sd.Opacity,
                sd.Brightness, sd.TransformationMatrix });

        D3D11_BUFFER_DESC bufferDesc;
        ZeroMemory(&bufferDesc, sizeof(bufferDesc));

        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.ByteWidth = m_CurrentSIBSize = (UINT)infoArray.size() * sizeof(decltype(infoArray)::value_type);
        bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        bufferDesc.StructureByteStride = sizeof(decltype(infoArray)::value_type);

        D3D11_SUBRESOURCE_DATA initData = {};
        ZeroMemory(&initData, sizeof(initData));

        initData.pSysMem = infoArray.data();

        hr = pD3D11Device->CreateBuffer(&bufferDesc, &initData, &m_pSpriteInfoBuffer);

        if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
        {
            m_CurrentSIBSize = 0;
            return false;
        }

        if (!CreateSpriteInfoBufferSrv(pD3D11Device))
        {
            m_CurrentSIBSize = 0;
            return false;
        }

        return true;
    }

    bool BatchedSprites::UpdateVertexBuffer(ID3D11DeviceContext* pD3D11DeviceContext)
    {
        HRESULT hr;

        const size_t verticesSize = m_Vertices.size() * sizeof(decltype(m_Vertices)::value_type);

        // If the buffer is not large enough, recreate it
        if (m_CurrentVBSize < verticesSize)
        {
            ID3D11Device* pDevice;
            pD3D11DeviceContext->GetDevice(&pDevice);

            return CreateAndUpdateVertexBuffer(pDevice);
        }

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        ZeroMemory(&mappedResource, sizeof(mappedResource));

        hr = pD3D11DeviceContext->Map(m_pVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

        if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
            return false;

        memcpy_s(mappedResource.pData, m_CurrentVBSize, m_Vertices.data(), verticesSize);

        pD3D11DeviceContext->Unmap(m_pVertexBuffer.Get(), 0);

        return true;
    }

    bool BatchedSprites::UpdateIndexBuffer(ID3D11DeviceContext* pD3D11DeviceContext)
    {
        HRESULT hr;

        const size_t indicesSize = m_ActiveIndices.size() * sizeof(decltype(m_ActiveIndices)::value_type);

        // If the buffer is not large enough, recreate it
        if (m_CurrentIBSize < indicesSize)
        {
            ID3D11Device* pDevice;
            pD3D11DeviceContext->GetDevice(&pDevice);

            return CreateAndUpdateIndexBuffer(pDevice);
        }

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        ZeroMemory(&mappedResource, sizeof(mappedResource));

        hr = pD3D11DeviceContext->Map(m_pIndexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

        if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
            return false;

        memcpy_s(mappedResource.pData, m_CurrentIBSize, m_ActiveIndices.data(), indicesSize);

        pD3D11DeviceContext->Unmap(m_pIndexBuffer.Get(), 0);

        return true;
    }

    bool BatchedSprites::UpdateSpriteInfoBuffer(ID3D11DeviceContext* pD3D11DeviceContext)
    {
        HRESULT hr;

        // If the buffer is not large enough, recreate it
        if (m_CurrentSIBSize < m_SpritesData.size() * sizeof(VERTEX_SHADER_SPRITE_INFO))
        {
            ID3D11Device* pDevice;
            pD3D11DeviceContext->GetDevice(&pDevice);

            return CreateAndUpdateSpriteInfoBuffer(pDevice);
        }

        std::vector<VERTEX_SHADER_SPRITE_INFO> infoArray;
        infoArray.reserve(m_SpritesData.size());

        for (const auto& sd : m_SpritesData)
            infoArray.push_back(VERTEX_SHADER_SPRITE_INFO{ (uint32_t)sd.TexturePathIdx, sd.Opacity,
                sd.Brightness, sd.TransformationMatrix });

        const size_t infoArraySize = infoArray.size() * sizeof(decltype(infoArray)::value_type);

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        ZeroMemory(&mappedResource, sizeof(mappedResource));

        hr = pD3D11DeviceContext->Map(m_pSpriteInfoBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

        if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
            return false;

        memcpy_s(mappedResource.pData, m_CurrentSIBSize, infoArray.data(), infoArraySize);

        pD3D11DeviceContext->Unmap(m_pSpriteInfoBuffer.Get(), 0);

        ID3D11Device* pDevice;
        pD3D11DeviceContext->GetDevice(&pDevice);

        if (!CreateSpriteInfoBufferSrv(pDevice))
            return false;

        return true;
    }

    bool BatchedSprites::UpdateVertexConstantBuffer(ID3D11DeviceContext* pD3D11DeviceContext, const VERTEX_CONSTANT_BUFFER& cBuffer)
    {
        HRESULT hr;

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        hr = pD3D11DeviceContext->Map(m_pVertexConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

        if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
            return false;

        VERTEX_CONSTANT_BUFFER* constantBuffer = (VERTEX_CONSTANT_BUFFER*)mappedResource.pData;

        *constantBuffer = cBuffer;

        pD3D11DeviceContext->Unmap(m_pVertexConstantBuffer.Get(), 0);

        return true;
    }

    bool BatchedSprites::UpdatePixelConstantBuffer(ID3D11DeviceContext* pD3D11DeviceContext, const PIXEL_CONSTANT_BUFFER& cBuffer)
    {
        HRESULT hr;

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        ZeroMemory(&mappedResource, sizeof(mappedResource));

        hr = pD3D11DeviceContext->Map(m_pPixelConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

        if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
            return false;

        auto constantBuffer = (PIXEL_CONSTANT_BUFFER*)mappedResource.pData;

        *constantBuffer = cBuffer;

        pD3D11DeviceContext->Unmap(m_pPixelConstantBuffer.Get(), 0);

        return true;
    }

    void BatchedSprites::Translate(size_t idx, glm::vec3 vector)
    {
        m_SpriteInfoUpdateQueued = true;

        m_SpritesData[idx].TransformationMatrix = glm::translate(m_SpritesData[idx].TransformationMatrix, vector);
    }

    void BatchedSprites::TranslateTo(size_t idx, glm::vec3 vector)
    {
        m_SpriteInfoUpdateQueued = true;

        glm::vec3 currentTranslation = glm::vec3(m_SpritesData[idx].TransformationMatrix[3]); // Extract the translation
        glm::vec3 currentScale = {
            glm::length(glm::vec3(m_SpritesData[idx].TransformationMatrix[0])),
            glm::length(glm::vec3(m_SpritesData[idx].TransformationMatrix[1])),
            glm::length(glm::vec3(m_SpritesData[idx].TransformationMatrix[2]))
        };
        glm::mat3 currentRotationMatrix = glm::mat3(m_SpritesData[idx].TransformationMatrix);
        currentRotationMatrix[0] = glm::normalize(m_SpritesData[idx].TransformationMatrix[0]);
        currentRotationMatrix[1] = glm::normalize(m_SpritesData[idx].TransformationMatrix[1]);
        currentRotationMatrix[2] = glm::normalize(m_SpritesData[idx].TransformationMatrix[2]);

        glm::mat4 newTransformationMatrix = glm::mat4(1.0f);
        newTransformationMatrix = glm::translate(newTransformationMatrix, vector);
        newTransformationMatrix *= glm::mat4(currentRotationMatrix);
        newTransformationMatrix = glm::scale(newTransformationMatrix, currentScale);

        m_SpritesData[idx].TransformationMatrix = newTransformationMatrix;
    }

    void BatchedSprites::Rotate(size_t idx, float rotation, glm::vec3 vector)
    {
        m_SpriteInfoUpdateQueued = true;

        m_SpritesData[idx].TransformationMatrix = glm::rotate(m_SpritesData[idx].TransformationMatrix, rotation, vector);
    }

    void BatchedSprites::RotateTo(size_t idx, float rotation, glm::vec3 axis)
    {
        m_SpriteInfoUpdateQueued = true;

        glm::vec3 currentTranslation = glm::vec3(m_SpritesData[idx].TransformationMatrix[3]); // Extract the translation
        glm::vec3 currentScale = {
            glm::length(glm::vec3(m_SpritesData[idx].TransformationMatrix[0])),
            glm::length(glm::vec3(m_SpritesData[idx].TransformationMatrix[1])),
            glm::length(glm::vec3(m_SpritesData[idx].TransformationMatrix[2]))
        };

        glm::vec3 targetEulerAngles = rotation * axis; // New rotation
        glm::mat3 newRotationMatrix = glm::yawPitchRoll(targetEulerAngles.y, targetEulerAngles.x, targetEulerAngles.z);

        glm::mat4 newTransformationMatrix = glm::mat4(1.0f);
        newTransformationMatrix = glm::translate(newTransformationMatrix, currentTranslation);
        newTransformationMatrix *= glm::mat4(newRotationMatrix);
        newTransformationMatrix = glm::scale(newTransformationMatrix, currentScale);

        m_SpritesData[idx].TransformationMatrix = newTransformationMatrix;
    }

    void BatchedSprites::Scale(size_t idx, glm::vec3 vector)
    {
        m_SpriteInfoUpdateQueued = true;

        m_SpritesData[idx].TransformationMatrix = glm::scale(m_SpritesData[idx].TransformationMatrix, vector);
    }

    void BatchedSprites::ScaleTo(size_t idx, glm::vec3 vector)
    {
        m_SpriteInfoUpdateQueued = true;

        glm::vec3 currentTranslation = glm::vec3(m_SpritesData[idx].TransformationMatrix[3]); // Extract the translation
        glm::mat3 currentRotationMatrix = glm::mat3(m_SpritesData[idx].TransformationMatrix);
        currentRotationMatrix[0] = glm::normalize(m_SpritesData[idx].TransformationMatrix[0]);
        currentRotationMatrix[1] = glm::normalize(m_SpritesData[idx].TransformationMatrix[1]);
        currentRotationMatrix[2] = glm::normalize(m_SpritesData[idx].TransformationMatrix[2]);

        glm::mat4 newTransformationMatrix = glm::mat4(1.0f);
        newTransformationMatrix = glm::translate(newTransformationMatrix, currentTranslation);
        newTransformationMatrix *= glm::mat4(currentRotationMatrix);
        newTransformationMatrix = glm::scale(newTransformationMatrix, vector);

        m_SpritesData[idx].TransformationMatrix = newTransformationMatrix;
    }

    void BatchedSprites::SetTransform(size_t idx, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale)
    {
        m_SpriteInfoUpdateQueued = true;

        glm::mat3 newRotationMatrix = glm::yawPitchRoll(rotation.y, rotation.x, rotation.z);

        glm::mat4 newTransformationMatrix = glm::mat4(1.0f);
        newTransformationMatrix = glm::translate(newTransformationMatrix, translation);
        newTransformationMatrix *= glm::mat4(newRotationMatrix);
        newTransformationMatrix = glm::scale(newTransformationMatrix, scale);

        m_SpritesData[idx].TransformationMatrix = newTransformationMatrix;
    }

    void BatchedSprites::SetOpacity(size_t idx, float opacity)
    {
        m_SpriteInfoUpdateQueued = true;

        m_SpritesData[idx].Opacity = opacity;
    }

    void BatchedSprites::SetBrightness(size_t idx, float brightness)
    {
        m_SpriteInfoUpdateQueued = true;

        m_SpritesData[idx].Brightness = brightness;
    }

    void BatchedSprites::Reorder(const std::vector<size_t>& spriteIndices)
    {
        // Todo: Order back to front
    }

    bool BatchedSprites::CreateSpriteInfoBufferSrv(ID3D11Device* pD3D11Device)
    {
        HRESULT hr;

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(srvDesc));

        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        srvDesc.Buffer.FirstElement = 0;
        srvDesc.Buffer.NumElements = (UINT)m_SpritesData.size();

        hr = pD3D11Device->CreateShaderResourceView(m_pSpriteInfoBuffer.Get(), &srvDesc, &m_pTIBSRV);

        if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
            return false;

        return true;
    }

    void BatchedSprites::BackupD3D11State(ID3D11DeviceContext* ctx)
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

    void BatchedSprites::RestoreD3D11State(ID3D11DeviceContext* ctx)
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
}
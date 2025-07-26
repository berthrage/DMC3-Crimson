#include "BatchedSprites.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "..\ThirdParty\glm\gtc\matrix_transform.hpp"
#include "..\ThirdParty\glm\gtx\euler_angles.hpp"
#define Log(...) printf(__VA_ARGS__)

#include <d3dcompiler.h>
#include <array>
#include <set>

namespace Graphics {
    BatchedSprites::BatchedSprites(ID3D11Device* pD3D11Device, UINT width, UINT height,
        const std::vector<SpriteDesc>& spriteDescs, std::shared_ptr<Texture2DArrayD3D11> pSpriteTextures,
        const std::vector<size_t> spriteIndices)
    {
        //Log("BatchedSprites: Path non-dependent constructor called with %zu spriteDescs, size %dx%d", spriteDescs.size(), width, height);
    
        bool initResult = Initialize(pD3D11Device, width, height);
        //Log("BatchedSprites: Initialize result: %s", initResult ? "SUCCESS" : "FAILED");
    
        if (initResult) {
            UpdateSprites(pD3D11Device, spriteDescs, spriteIndices, pSpriteTextures);
            //Log("BatchedSprites: UpdateSprites completed");
        }
    
        //Log("BatchedSprites: Path non-dependent constructor completed");
    }
    
    BatchedSprites::~BatchedSprites()
    {}
    
    void BatchedSprites::UpdateSprites(ID3D11Device* pD3D11Device, const std::vector<SpriteDesc>& spriteDescs,
        const std::vector<size_t> spriteIndices, std::shared_ptr<Texture2DArrayD3D11> pSpriteTextureArray)
    {
    	// Clear old data
    	m_SpriteInstanceData.clear();
    	m_pTextureArray.reset();
    
    	const auto count = spriteIndices.size();
    	m_SpriteInstanceData.reserve(count);
    
    	// Process sprite data (no longer need to store vertices/indices per sprite)
    	for (size_t i = 0; i < count; i++) {
    		const auto& textureIdx = spriteIndices[i];
    		const auto& spriteTr = spriteDescs[textureIdx].GetTransformationmatrix();
    
            glm::vec3 translation = glm::vec3(spriteTr[3]);
            glm::vec3 scale = {
                glm::length(glm::vec3(spriteTr[0])),
                glm::length(glm::vec3(spriteTr[1])),
                glm::length(glm::vec3(spriteTr[2]))
            };
    
            glm::mat3 rot = {
                glm::vec3(spriteTr[0]) / scale.x,
                glm::vec3(spriteTr[1]) / scale.y,
                glm::vec3(spriteTr[2]) / scale.z
            };
            glm::vec3 rotationEuler = glm::eulerAngles(glm::quat_cast(rot));
    
    		m_SpriteInstanceData.push_back(
    			SpriteInfo
                {
    				.TextureIdx     = textureIdx,
    				.Opacity        = 1.0f, // 1.0f by default
    				.Brightness     = 1.0f, // 1.0f by default
    				.Translation    = translation,
    				.Rotation       = rotationEuler,
    				.Scale          = scale
    			}
    		);
    	}

        m_pTextureArray = pSpriteTextureArray;
    
    	if (!m_pTextureArray) {
    		Log("BatchedSprites: ERROR - Texture array creation failed!");
    		return;
    	}
    
    	m_SpriteIndicesIntoTheTextureArray = spriteIndices;
    
    	// Create static vertex buffer if not already created
        if (!m_pVertexBuffer)
        {
            if (!CreateVertexBuffer(pD3D11Device))
            {
                Log("BatchedSprites: ERROR - Failed to create vertex buffer!");
                return;
            }
        }

        // Create static index buffer if not already created
        if (!m_pIndexBuffer)
        {
            if (!CreateIndexBuffer(pD3D11Device))
            {
                Log("BatchedSprites: ERROR - Failed to create index buffer!");
                return;
            }
        }
    
    	// Create/recreate the sprite info buffer
    	if (!CreateAndUpdateSpriteInfoBuffer(pD3D11Device)) {
    		Log("BatchedSprites: ERROR - Failed to create sprite info buffer during UpdateSprites!");
    		return;
    	}
    }
    
    void BatchedSprites::ChangeSprites(
        ID3D11Device* pD3D11Device, const
        std::vector<SpriteDesc>& spriteDescs,
        const std::vector<size_t> spriteIndices,
        std::shared_ptr<Texture2DArrayD3D11> pSpriteTextureArray)
    {
        UpdateSprites(pD3D11Device, spriteDescs, spriteIndices, pSpriteTextureArray);
    }
    
    bool BatchedSprites::Initialize(ID3D11Device* pD3D11Device, UINT width, UINT height)
    {
    	//Log("BatchedSprites: Initialize starting with device=%p, size=%dx%d", pD3D11Device, width, height);
    	// Add detailed struct size logging
        //Log("BatchedSprites: VERTEX_CONSTANT_BUFFER size: %zu bytes", sizeof(VERTEX_CONSTANT_BUFFER));
        //Log("BatchedSprites: PIXEL_CONSTANT_BUFFER size: %zu bytes", sizeof(PIXEL_CONSTANT_BUFFER));
        //Log("BatchedSprites: VERTEX_SHADER_SPRITEINSTANCE_INFO size: %zu bytes", sizeof(VERTEX_SHADER_SPRITEINSTANCE_INFO));
        //Log("BatchedSprites: Vertex_t size: %zu bytes", sizeof(Vertex_t));
    	// This saved my ass because these need to be pristine memory aligned. - Berth
    
    	if (!CreateRenderTargetView(pD3D11Device, width, height)) {
    		Log("BatchedSprites: CreateRenderTargetView FAILED");
    		return false;
    	}
    	//Log("BatchedSprites: CreateRenderTargetView SUCCESS");
    
    	HRESULT hr;
    
    
    	// Create the vertex shader
    	{
    		//Log("BatchedSprites: Starting vertex shader creation");
    
    
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
    				uint    TextureIdx  : TEXTURE_IDX;
    				float   Opacity     : OPACITY;
    				float   Brightness  : BRIGHTNESS;

    				float3  Translation : TRANSLATION;
                    float4  Rotation    : ROTATION;
                    float3  Scale       : SCALE;
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

                float4x4 CreateTransform(float3 translation, float4 rotation, float3 scale)
                {
                    float4x4 scaleMatrix = float4x4(
                        scale.x, 0.0f,    0.0f,    0.0f,
                        0.0f,    scale.y, 0.0f,    0.0f,
                        0.0f,    0.0f,    scale.z, 0.0f,
                        0.0f,    0.0f,    0.0f,    1.0f
                    );
                    
                    float x = rotation.x;
                    float y = rotation.y;
                    float z = rotation.z;
                    float w = rotation.w;
                    
                    float x2 = x + x;
                    float y2 = y + y;
                    float z2 = z + z;
                    
                    float xx = x * x2;
                    float xy = x * y2;
                    float xz = x * z2;
                    
                    float yy = y * y2;
                    float yz = y * z2;
                    float zz = z * z2;
                    
                    float wx = w * x2;
                    float wy = w * y2;
                    float wz = w * z2;
                    
                    float4x4 rotationMatrix = float4x4(
                        1.0f - (yy + zz), xy - wz,          xz + wy,          0.0f,
                        xy + wz,          1.0f - (xx + zz), yz - wx,          0.0f,
                        xz - wy,          yz + wx,          1.0f - (xx + yy), 0.0f,
                        0.0f,             0.0f,             0.0f,             1.0f
                    );

                    float4x4 translationMatrix = float4x4(
                        1.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 1.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 1.0f, 0.0f,
                        translation.x, translation.y, translation.z, 1.0f
                    );

                    return mul(mul(scaleMatrix, rotationMatrix), translationMatrix);
                }

                float4 EulerToQuaternion(float3 rotation)
                {
                    float cx = cos(rotation.x * 0.5f);
                    float sx = sin(rotation.x * 0.5f);
                    float cy = cos(rotation.y * 0.5f);
                    float sy = sin(rotation.y * 0.5f);
                    float cz = cos(rotation.z * 0.5f);
                    float sz = sin(rotation.z * 0.5f);
                
                    float4 q;
                    q.x = sx * cy * cz - cx * sy * sz;
                    q.y = cx * sy * cz + sx * cy * sz;
                    q.z = cx * cy * sz - sx * sy * cz;
                    q.w = cx * cy * cz + sx * sy * sz;
                
                    return normalize(q);
                }

    			PS_INPUT main(VS_INPUT input)
    			{
    				PS_INPUT output;

                    float3 translation  = input.Translation;
                    float4 rotationQuat = EulerToQuaternion(input.Rotation);
                    float3 scale        = input.Scale;

                    float4x4 TransformationMatrix = CreateTransform(translation, rotationQuat, scale);
                    float4   origin               = float4(input.Pos, 1.0f);

    				output.Pos     = mul(mul(origin, TransformationMatrix), ProjectionMatrix);
    				output.Col     = input.Col;
    				output.UV      = input.UV;
    				output.Tid     = input.TextureIdx;
    				output.Opacity = input.Opacity;
    				output.Bright  = input.Brightness;
    
    				return output;
    			}
    			)";
    
    		//Log("BatchedSprites: About to compile vertex shader");
    
    		ComPtr<ID3DBlob> pVertexShaderBlob;
    		ComPtr<ID3DBlob> pErrorBlob;
    
    		hr = D3DCompile(vertexShader, strlen(vertexShader), NULL, NULL, NULL, "main", "vs_4_0", 0, 0, &pVertexShaderBlob, &pErrorBlob);
    
            if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
            {
                Log(GetLastErrorMsg().c_str());
                Log("BatchedSprites: Vertex shader compilation FAILED: %s", (const char*)pErrorBlob->GetBufferPointer());

                return false; // NB: Pass ID3DBlob* pErrorBlob to D3DCompile() to get error showing in (const char*)pErrorBlob->GetBufferPointer(). Make sure to Release() the blob!
            }

    		//Log("BatchedSprites: Vertex shader compiled successfully");
    
    		hr = pD3D11Device->CreateVertexShader(pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), NULL, &m_pVertexShader);
    
    		if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__)) {
                Log(GetLastErrorMsg().c_str());
    			return false;
    		}
    
    		//Log("BatchedSprites: Vertex shader created successfully");
    
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
    
    		//Log("BatchedSprites: Input layout created successfully");
    	}
    	//Log("BatchedSprites: Vertex shader creation completed");
    
    	// Create the pixel shader
    	{
    		//Log("BatchedSprites: Starting pixel shader creation");
    
    		static const char* pixelShader =
    			R"(
    		    cbuffer boundInfo : register(b0)
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
    
    		//Log("BatchedSprites: About to compile pixel shader");
    
    		ComPtr<ID3DBlob> pPixelShaderBlob;
    		ComPtr<ID3DBlob> pErrorBlob;
    
    		hr = D3DCompile(pixelShader, strlen(pixelShader), NULL, NULL, NULL, "main", "ps_4_0", 0, 0, &pPixelShaderBlob, &pErrorBlob);
    
            if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
            {
                Log(GetLastErrorMsg().c_str());
                Log("BatchedSprites: Pixel shader compilation FAILED: %s", (const char*)pErrorBlob->GetBufferPointer());

                return false; // NB: Pass ID3DBlob* pErrorBlob to D3DCompile() to get error showing in (const char*)pErrorBlob->GetBufferPointer(). Make sure to Release() the blob!
            }
    
    		//Log("BatchedSprites: Pixel shader compiled successfully");
    
    		hr = pD3D11Device->CreatePixelShader(pPixelShaderBlob->GetBufferPointer(), pPixelShaderBlob->GetBufferSize(), NULL, &m_pPixelShader);
    
    		if (FAILED(hr)) {
    			Log("BatchedSprites: CreatePixelShader FAILED with HRESULT: 0x%08X", hr);
    			return false;
    		}
    
    		//Log("BatchedSprites: Pixel shader created successfully");
    	}
    	//Log("BatchedSprites: Pixel shader creation completed");
    
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
    		desc.ByteWidth = sizeof(PIXEL_CONSTANT_BUFFER);
    		desc.Usage = D3D11_USAGE_DYNAMIC;
    		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    		desc.MiscFlags = 0;
    
    		hr = pD3D11Device->CreateBuffer(&desc, NULL, &m_pPixelConstantBuffer);
    
    		if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
    			return false;
    	}
    	//Log("BatchedSprites: Constant buffers created");
    
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
            desc.Filter = D3D11_FILTER_ANISOTROPIC;//D3D11_FILTER_MIN_MAG_MIP_LINEAR;
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
    		memset(&m_ViewPort, 0, sizeof(D3D11_VIEWPORT));
    		m_ViewPort.Width = (FLOAT)m_RTWidth;
    		m_ViewPort.Height = (FLOAT)m_RTHeight;
    		m_ViewPort.MinDepth = 0.0f;
    		m_ViewPort.MaxDepth = 1.0f;
    		m_ViewPort.TopLeftX = m_ViewPort.TopLeftY = 0;
    	}
    
    	//Log("BatchedSprites: Initialize completed successfully");
    	return true;
    }
    
    bool BatchedSprites::Draw(ID3D11DeviceContext* pD3D11DeviceContext)
    {
        if (m_SpriteUpdateInfo.UpdateQueued)
        {
    		if (!UpdateSpriteInstanceInfoBuffer(pD3D11DeviceContext, m_SpriteUpdateInfo.Indices))
            {
    			Log("BatchedSprites: ERROR - Failed to update sprite info buffer");
    			return false;
    		}

            m_SpriteUpdateInfo.UpdateQueued = false;
            m_SpriteUpdateInfo.Indices.clear();
    	}

        // Update vertex shader constant buffer
        {
            VERTEX_CONSTANT_BUFFER cBuffer =
            {
                .MVP = glm::mat4(1.0f) // An orthographic view projection for sprite rendering
            };

            if (!UpdateVertexConstantBuffer(pD3D11DeviceContext, cBuffer))
            {
                Log("BatchedSprites: Failed to update vertex shader constant buffer");
                return false;
            }
        }

        // Update pixel shader constant buffer
        {
            PIXEL_CONSTANT_BUFFER cBuffer =
            {
                .TextureCount = (uint32_t)m_pTextureArray->GetCount(), // The count of the textures in the array
                .SamplerCount = 1, // Currently only have one sampler
            };

            if (!UpdatePixelConstantBuffer(pD3D11DeviceContext, cBuffer))
            {
                Log("BatchedSprites: Failed to update pixel shader constant buffer");
                return false;
            }
        }
    
    	BackupD3D11State(pD3D11DeviceContext);
    
    	// Set Render Target, Viewport, and Scissor Rect
    	const D3D11_RECT scissor = { 0, 0, m_RTWidth, m_RTHeight };
    	pD3D11DeviceContext->RSSetScissorRects(1, &scissor);
    	pD3D11DeviceContext->RSSetViewports(1, &m_ViewPort);
    	pD3D11DeviceContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_pDepthStincilView.Get());
    
    	// Bind Pipeline Resources
    	pD3D11DeviceContext->IASetInputLayout(m_pInputLayout.Get());

        {
            std::array<ID3D11Buffer*, 2> buffers = { m_pVertexBuffer.Get(), m_pSpriteInstanceInfoBuffer.Get() };
            std::array<UINT, 2> strides = { sizeof(Vertex_t), sizeof(VERTEX_SHADER_SPRITEINSTANCE_INFO) };
            std::array<UINT, 2> offsets = { 0, 0 };

            pD3D11DeviceContext->IASetVertexBuffers(0, buffers.size(), buffers.data(), strides.data(), offsets.data());
        }

        pD3D11DeviceContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    	pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    	// Set Vertex Shader and Resources
    	pD3D11DeviceContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
    	pD3D11DeviceContext->VSSetConstantBuffers(0, 1, m_pVertexConstantBuffer.GetAddressOf());
    
    	// Set Pixel Shader and Resources
    	ID3D11ShaderResourceView* srv = m_pTextureArray->GetSrv();
    	pD3D11DeviceContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
    	pD3D11DeviceContext->PSSetConstantBuffers(0, 1, m_pPixelConstantBuffer.GetAddressOf());
    	pD3D11DeviceContext->PSSetShaderResources(0, 1, &srv);
    	pD3D11DeviceContext->PSSetSamplers(0, 1, m_pTextureSampler.GetAddressOf());
    
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
    	pD3D11DeviceContext->ClearDepthStencilView(m_pDepthStincilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    
    	// **INSTANCED DRAW CALL**
    	pD3D11DeviceContext->DrawIndexedInstanced(
    		s_SpriteIndices.size(),      // Indices per instance (6 for a quad: 2 triangles)
    		m_SpriteInstanceData.size(), // Number of instances (sprites to draw)
    		0,                           // Start index location
    		0,                           // Base vertex location
    		0                            // Start instance location
    	);
    
    	RestoreD3D11State(pD3D11DeviceContext);
    	return true;
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
    
    	hr = pD3D11Device->CreateRenderTargetView(pRenderTargetTexture.Get(), nullptr, &m_pRenderTargetView);
    
    	if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
    		return false;
    
    	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    	ZeroMemory(&srvDesc, sizeof(srvDesc));
    
    	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    	srvDesc.Texture2D.MostDetailedMip = 0;
    	srvDesc.Texture2D.MipLevels = 1;
    
    	hr = pD3D11Device->CreateShaderResourceView(pRenderTargetTexture.Get(), &srvDesc, &m_pRenderTargetSRV);
    
    	if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
    		return false;
    
    	return true;
    }
    
    bool BatchedSprites::CreateVertexBuffer(ID3D11Device* pD3D11Device)
    {
    	HRESULT hr;

        // Same vertices for all instances
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
    
    bool BatchedSprites::CreateIndexBuffer(ID3D11Device* pD3D11Device)
    {
    	HRESULT hr;
    
    	D3D11_BUFFER_DESC bufferDesc = {};
    	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

        //static constexpr std::array<uint32_t, 6> indices{ 0, 1, 2, 0, 2, 3 };

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
    
    bool BatchedSprites::CreateAndUpdateSpriteInfoBuffer(ID3D11Device* pD3D11Device)
    {
    	HRESULT hr;
    
    	static std::vector<VERTEX_SHADER_SPRITEINSTANCE_INFO> instanceInfoArray; // Static to avoid re allocation each time
    	instanceInfoArray.reserve(m_SpriteInstanceData.size());

        instanceInfoArray.clear();
    	for (const auto& sd : m_SpriteInstanceData) {
    		instanceInfoArray.push_back(VERTEX_SHADER_SPRITEINSTANCE_INFO
                {
    			    .TextureIdx     = (uint32_t)sd.TextureIdx,
    			    .Opacity        = sd.Opacity,
    			    .Brightness     = sd.Brightness,
    			    .Translation    = sd.Translation,
                    .Rotation       = sd.Rotation,
                    .Scale          = sd.Scale
    			});
    	}

        size_t byteSize = instanceInfoArray.size() * sizeof(decltype(instanceInfoArray)::value_type);
    
    	D3D11_BUFFER_DESC bufferDesc;
    	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
    
    	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    	bufferDesc.ByteWidth = m_CurrentSpriteInstanceInfoBufferSize = (UINT)byteSize;
    	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    	bufferDesc.MiscFlags = 0;
    	bufferDesc.StructureByteStride = 0;
    
    	D3D11_SUBRESOURCE_DATA initData = {};
    	ZeroMemory(&initData, sizeof(initData));
    	initData.pSysMem = instanceInfoArray.data();
    
    	hr = pD3D11Device->CreateBuffer(&bufferDesc, &initData, &m_pSpriteInstanceInfoBuffer);
    
    	if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__)) {
    		m_CurrentSpriteInstanceInfoBufferSize = 0;
    		return false;
    	}
    
    	return true;
    }
    
    bool BatchedSprites::UpdateSpriteInstanceInfoBuffer(
        ID3D11DeviceContext* pD3D11DeviceContext,
        const std::vector<size_t>& indices)
    {
    	HRESULT hr;

        // Static so, it doesn't get reallocated every time
    	static std::vector<VERTEX_SHADER_SPRITEINSTANCE_INFO> infoArray;

        // Make sure it has enouch capacity
    	infoArray.reserve(m_SpriteInstanceData.size());

        // If no specific indices were given or buffer size doesn't match the info,
        // update the whole buffer
        if (indices.empty() || infoArray.size() != m_SpriteInstanceData.size())
        {
            infoArray.clear(); // Clear the old data
            for (const auto& sd : m_SpriteInstanceData)
            {
                infoArray.push_back(VERTEX_SHADER_SPRITEINSTANCE_INFO
                    {
                        .TextureIdx     = (uint32_t)sd.TextureIdx,
                        .Opacity        = sd.Opacity,
                        .Brightness     = sd.Brightness,
                        .Translation    = sd.Translation,
                        .Rotation       = sd.Rotation,
                        .Scale          = sd.Scale
                    });
            }
        }
        else // If specific indices were given and the buffer size is not changed, only update the given indices
        {
            for (const auto& idx : indices)
            {
                const auto& sd = m_SpriteInstanceData[idx];
                infoArray[idx] = VERTEX_SHADER_SPRITEINSTANCE_INFO
                    {
                        .TextureIdx     = (uint32_t)sd.TextureIdx,
                        .Opacity        = sd.Opacity,
                        .Brightness     = sd.Brightness,
                        .Translation    = sd.Translation,
                        .Rotation       = sd.Rotation,
                        .Scale          = sd.Scale
                    };
            }
        }

    	size_t byteSize = infoArray.size() * sizeof(decltype(infoArray)::value_type);
    
    	if (!m_pSpriteInstanceInfoBuffer || m_CurrentSpriteInstanceInfoBufferSize != byteSize)
        {
    		ID3D11Device* pDevice;
    		pD3D11DeviceContext->GetDevice(&pDevice);
    		return CreateAndUpdateSpriteInfoBuffer(pDevice);
    	}
    
    	D3D11_MAPPED_SUBRESOURCE mappedResource;
    
    	hr = pD3D11DeviceContext->Map(m_pSpriteInstanceInfoBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    
    	if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
        {
    		return false;
    	}
    
    	memcpy_s(mappedResource.pData, m_CurrentSpriteInstanceInfoBufferSize, infoArray.data(), byteSize);
    
    	pD3D11DeviceContext->Unmap(m_pSpriteInstanceInfoBuffer.Get(), 0);
    	return true;
    }
    
    bool BatchedSprites::UpdateVertexConstantBuffer(ID3D11DeviceContext* pD3D11DeviceContext, const VERTEX_CONSTANT_BUFFER& cBuffer)
    {
    	if (!m_pVertexConstantBuffer)
        {
    		Log("BatchedSprites: Vertex constant buffer is null!");
    		return false;
    	}
    
    	HRESULT hr;
    	D3D11_MAPPED_SUBRESOURCE mappedResource;
    	hr = pD3D11DeviceContext->Map(m_pVertexConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    
    	if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
        {
    		Log("BatchedSprites: Failed to map vertex constant buffer");
    		return false;
    	}

        memcpy_s(mappedResource.pData, sizeof(VERTEX_CONSTANT_BUFFER), &cBuffer, sizeof(cBuffer));
    
    	pD3D11DeviceContext->Unmap(m_pVertexConstantBuffer.Get(), 0);
    	return true;
    }
    
    bool BatchedSprites::UpdatePixelConstantBuffer(ID3D11DeviceContext* pD3D11DeviceContext, const PIXEL_CONSTANT_BUFFER& cBuffer)
    {
    	if (!m_pPixelConstantBuffer)
        {
    		// Log error or handle appropriately
    		return false;
    	}
    
    	HRESULT hr;
    
    	D3D11_MAPPED_SUBRESOURCE mappedResource;
    	ZeroMemory(&mappedResource, sizeof(mappedResource));
    
    	hr = pD3D11DeviceContext->Map(m_pPixelConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    
    	if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
    		return false;
    
        memcpy_s(mappedResource.pData, sizeof(VERTEX_CONSTANT_BUFFER), &cBuffer, sizeof(cBuffer));
    
    	pD3D11DeviceContext->Unmap(m_pPixelConstantBuffer.Get(), 0);
    
    	return true;
    }
    
    void BatchedSprites::Translate(size_t idx, glm::vec3 vector)
    {
        m_SpriteUpdateInfo.UpdateQueued = true;
        m_SpriteUpdateInfo.Indices.push_back(idx);
    
    	m_SpriteInstanceData[idx].Translation += vector;
    }
    
    void BatchedSprites::TranslateTo(size_t idx, glm::vec3 vector)
    {
        m_SpriteUpdateInfo.UpdateQueued = true;
        m_SpriteUpdateInfo.Indices.push_back(idx);
    
    	m_SpriteInstanceData[idx].Translation = vector;
    }
    
    void BatchedSprites::Rotate(size_t idx, glm::vec3 rotation)
    {
        m_SpriteUpdateInfo.UpdateQueued = true;
        m_SpriteUpdateInfo.Indices.push_back(idx);
    
        m_SpriteInstanceData[idx].Rotation += rotation;
    }
    
    void BatchedSprites::RotateTo(size_t idx, glm::vec3 rotation)
    {
        m_SpriteUpdateInfo.UpdateQueued = true;
        m_SpriteUpdateInfo.Indices.push_back(idx);

        m_SpriteInstanceData[idx].Rotation = rotation;
    }
    
    void BatchedSprites::Scale(size_t idx, glm::vec3 vector)
    {
        m_SpriteUpdateInfo.UpdateQueued = true;
        m_SpriteUpdateInfo.Indices.push_back(idx);
    
    	m_SpriteInstanceData[idx].Scale *= vector;
    }
    
    void BatchedSprites::ScaleTo(size_t idx, glm::vec3 vector)
    {
        m_SpriteUpdateInfo.UpdateQueued = true;
        m_SpriteUpdateInfo.Indices.push_back(idx);
    
    	m_SpriteInstanceData[idx].Scale = vector;
    }
    
    void BatchedSprites::SetTransform(size_t idx, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale)
    {
        m_SpriteUpdateInfo.UpdateQueued = true;
        m_SpriteUpdateInfo.Indices.push_back(idx);
    
    	m_SpriteInstanceData[idx].Translation = translation;
        m_SpriteInstanceData[idx].Rotation = rotation;
        m_SpriteInstanceData[idx].Scale = scale;
    }
    
    void BatchedSprites::SetOpacity(size_t idx, float opacity)
    {
        m_SpriteUpdateInfo.UpdateQueued = true;
        m_SpriteUpdateInfo.Indices.push_back(idx);
    
    	m_SpriteInstanceData[idx].Opacity = opacity;
    }
    
    void BatchedSprites::SetBrightness(size_t idx, float brightness)
    {
        m_SpriteUpdateInfo.UpdateQueued = true;
        m_SpriteUpdateInfo.Indices.push_back(idx);
    
    	m_SpriteInstanceData[idx].Brightness = brightness;
    }
    
    bool BatchedSprites::CreateSpriteInfoBufferSrv(ID3D11Device* pD3D11Device)
    {
    	HRESULT hr;
    
    	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    	ZeroMemory(&srvDesc, sizeof(srvDesc));
    
    	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    	srvDesc.Buffer.FirstElement = 0;
    	srvDesc.Buffer.NumElements = (UINT)m_SpriteInstanceData.size();
    
    	hr = pD3D11Device->CreateShaderResourceView(m_pSpriteInstanceInfoBuffer.Get(), &srvDesc, &m_pInstanceInfoBufferSRV);
    
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

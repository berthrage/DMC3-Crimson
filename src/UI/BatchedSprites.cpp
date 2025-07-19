#include "BatchedSprites.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "..\ThirdParty\glm\gtc\matrix_transform.hpp"
#include "..\ThirdParty\glm\gtx\euler_angles.hpp"
#include "..\Core\Core.hpp"

#include <d3dcompiler.h>
#include <array>
#include <set>

namespace Graphics {
BatchedSprites::BatchedSprites(ID3D11Device* pD3D11Device, UINT width, UINT height,
    const std::vector<SpriteDesc>& spriteDescs, const std::vector<size_t> spriteIndices) {
	//Log("BatchedSprites: Path dependent onstructor called with %zu spriteDescs, size %dx%d", spriteDescs.size(), width, height);

	bool initResult = Initialize(pD3D11Device, width, height);
	//Log("BatchedSprites: Initialize result: %s", initResult ? "SUCCESS" : "FAILED");

	if (initResult) {
		UpdateSprites(pD3D11Device, spriteDescs, spriteIndices);
		//Log("BatchedSprites: UpdateSprites completed");
	}

	//Log("BatchedSprites: Path dependent constructor completed");
}

BatchedSprites::BatchedSprites(ID3D11Device* pD3D11Device, UINT width, UINT height,
    const std::vector<SpriteDesc>& spriteDescs, std::shared_ptr<Texture2DArrayD3D11> pSpriteTextures,
    const std::vector<size_t> spriteIndices) {
    //Log("BatchedSprites: Path non-dependent constructor called with %zu spriteDescs, size %dx%d", spriteDescs.size(), width, height);

    bool initResult = Initialize(pD3D11Device, width, height);
    //Log("BatchedSprites: Initialize result: %s", initResult ? "SUCCESS" : "FAILED");

    if (initResult) {
        UpdateSprites(pD3D11Device, spriteDescs, spriteIndices, pSpriteTextures);
        //Log("BatchedSprites: UpdateSprites completed");
    }

    //Log("BatchedSprites: Path non-dependent constructor completed");
}

BatchedSprites::~BatchedSprites() {
}

void BatchedSprites::UpdateSprites(ID3D11Device* pD3D11Device, const std::vector<SpriteDesc>& spriteDescs,
    const std::vector<size_t> spriteIndices, std::shared_ptr<Texture2DArrayD3D11> pSpriteTextureArray) {
	m_UpdateVertexBuffer = true;
	m_UpdateIndexBuffer = true;
	m_SpriteInfoUpdateQueued = true;

	// Clear old data
	m_SpritesData.clear();
	m_TexturePaths.clear();
	m_pTextureArray.reset();

	const auto count = spriteDescs.size();
	m_SpritesData.reserve(count);

	// Process sprite data (no longer need to store vertices/indices per sprite)
	for (size_t i = 0; i < count; i++) {
		const auto& spriteTr = spriteDescs[i].GetTransformationmatrix();
		const auto& pathIdx = FindTexturePathIndex(spriteDescs[i].GetTexturePath());

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
        glm::quat rotationQuat = glm::quat_cast(rot);

		float debugOpacity = 1.0f;
		float debugBrightness = 1.0f;

		if (pathIdx.has_value()) {
			m_SpritesData.push_back(
				SpriteInfo{
					pathIdx.value(),
					0, // VerticesOffset not needed for instanced rendering
					0, // IndicesOffset not needed for instanced rendering  
					debugOpacity,
					debugBrightness,
                    translation,
                    rotationQuat,
                    scale
				}
			);
		} else {
			m_SpritesData.push_back(
				SpriteInfo{
					m_TexturePaths.size(),
					0, // VerticesOffset not needed
					0, // IndicesOffset not needed
                    debugOpacity,
                    debugBrightness,
                    translation,
                    rotationQuat,
                    scale
				}
			);

			m_TexturePaths.push_back(spriteDescs[i].GetTexturePath());
		}
	}

    if (pSpriteTextureArray) {
        m_pTextureArray = pSpriteTextureArray;
    }
    else {
        m_pTextureArray = std::make_shared<Texture2DArrayD3D11>(m_TexturePaths, pD3D11Device);
    }

	if (!m_pTextureArray) {
		Log("BatchedSprites: ERROR - Texture array creation failed!");
		return;
	}

	m_CustomSpriteIndices = spriteIndices;

	// Create static vertex and index buffers (single quad)
	if (!CreateAndUpdateVertexBuffer(pD3D11Device)) {
		Log("BatchedSprites: ERROR - Failed to create vertex buffer!");
		return;
	}

	if (!CreateAndUpdateIndexBuffer(pD3D11Device)) {
		Log("BatchedSprites: ERROR - Failed to create index buffer!");
		return;
	}

	// Create the sprite info buffer
	if (!CreateAndUpdateSpriteInfoBuffer(pD3D11Device)) {
		Log("BatchedSprites: ERROR - Failed to create sprite info buffer during UpdateSprites!");
		return;
	}

	m_SpriteInfoUpdateQueued = false;
	m_UpdateVertexBuffer = false;
	m_UpdateIndexBuffer = false;
}

void BatchedSprites::SetActiveSprites(const std::vector<size_t> spriteIndices) {
	m_CustomSpriteIndices = spriteIndices;

	PopulateActiveIndices();
}

bool BatchedSprites::Initialize(ID3D11Device* pD3D11Device, UINT width, UINT height) {
	//Log("BatchedSprites: Initialize starting with device=%p, size=%dx%d", pD3D11Device, width, height);
	// Add detailed struct size logging
// 	Log("BatchedSprites: VERTEX_CONSTANT_BUFFER size: %zu bytes", sizeof(VERTEX_CONSTANT_BUFFER));
// 	Log("BatchedSprites: PIXEL_CONSTANT_BUFFER size: %zu bytes", sizeof(PIXEL_CONSTANT_BUFFER));
// 	Log("BatchedSprites: VERTEX_SHADER_SPRITE_INFO size: %zu bytes", sizeof(VERTEX_SHADER_SPRITE_INFO));
// 	Log("BatchedSprites: Vertex_t size: %zu bytes", sizeof(Vertex_t));
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
		#pragma pack_matrix(column_major)
    
		cbuffer vertexBuffer : register(b0) 
		{
			float4x4 ProjectionMatrix;
			uint     VertexPerSprite;
			uint     IndexPerSprite;
		};

		cbuffer spriteInfoBuffer : register(b1)
		{
			struct SpriteInfo
			{
				uint     TextureIdx;
				float    Opacity;
				float    Brightness;
				column_major float4x4 TransformationMatrix;  
			};
			SpriteInfo spriteInfo[128];
		};

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
    
		PS_INPUT main(VS_INPUT input, uint instanceID : SV_InstanceID)
		{
			SpriteInfo si = spriteInfo[instanceID];

			PS_INPUT output;

			output.Pos = mul(si.TransformationMatrix, float4(input.Pos, 1.0f));
        
			output.Col = input.Col;
			output.UV  = input.UV;
			output.Tid = si.TextureIdx;
			output.Opacity = si.Opacity;
			output.Bright = si.Brightness;

			return output;
		}
		)";

		//Log("BatchedSprites: About to compile vertex shader");

		ComPtr<ID3DBlob> pVertexShaderBlob;
		ComPtr<ID3DBlob> pErrorBlob;

		hr = D3DCompile(vertexShader, strlen(vertexShader), NULL, NULL, NULL, "main", "vs_4_0", 0, 0, &pVertexShaderBlob, &pErrorBlob);

		if (FAILED(hr)) {
			if (pErrorBlob) {
				Log("BatchedSprites: Vertex shader compilation FAILED: %s", (const char*)pErrorBlob->GetBufferPointer());
			} else {
				Log("BatchedSprites: Vertex shader compilation FAILED with HRESULT: 0x%08X", hr);
			}
			return false;
		}

		//Log("BatchedSprites: Vertex shader compiled successfully");

		hr = pD3D11Device->CreateVertexShader(pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), NULL, &m_pVertexShader);

		if (FAILED(hr)) {
			Log("BatchedSprites: CreateVertexShader FAILED with HRESULT: 0x%08X", hr);
			return false;
		}

		//Log("BatchedSprites: Vertex shader created successfully");

		hr = pD3D11Device->CreateInputLayout(s_VertexInputLayout,
			sizeof(s_VertexInputLayout) / sizeof(D3D11_INPUT_ELEMENT_DESC),
			pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), &m_pInputLayout);

		if (FAILED(hr)) {
			Log("BatchedSprites: CreateInputLayout FAILED with HRESULT: 0x%08X", hr);
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

		//Log("BatchedSprites: About to compile pixel shader");

		ComPtr<ID3DBlob> pPixelShaderBlob;
		ComPtr<ID3DBlob> pErrorBlob;

		hr = D3DCompile(pixelShader, strlen(pixelShader), NULL, NULL, NULL, "main", "ps_4_0", 0, 0, &pPixelShaderBlob, &pErrorBlob);

		if (FAILED(hr)) {
			if (pErrorBlob) {
				Log("BatchedSprites: Pixel shader compilation FAILED: %s", (const char*)pErrorBlob->GetBufferPointer());
			} else {
				Log("BatchedSprites: Pixel shader compilation FAILED with HRESULT: 0x%08X", hr);
			}
			return false;
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

	//Log("BatchedSprites: Initialize completed successfully");
	return true;
}
bool BatchedSprites::Draw(ID3D11DeviceContext* pD3D11DeviceContext) {
	// Validate buffers before use
	if (!m_pVertexConstantBuffer || !m_pPixelConstantBuffer) {
		Log("BatchedSprites: ERROR - Constant buffers are null!");
		return false;
	}

	// Create sprite info buffer if it doesn't exist or needs updating
	if (!m_pSpriteInfoBuffer || m_SpriteInfoUpdateQueued) {
		if (!m_pSpriteInfoBuffer) {
			Log("BatchedSprites: Sprite info buffer is null, creating...");
			ID3D11Device* pDevice;
			pD3D11DeviceContext->GetDevice(&pDevice);
			if (!CreateAndUpdateSpriteInfoBuffer(pDevice)) {
				Log("BatchedSprites: ERROR - Failed to create sprite info buffer");
				return false;
			}
		} else if (m_SpriteInfoUpdateQueued) {
			if (!UpdateSpriteInfoBuffer(pD3D11DeviceContext)) {
				Log("BatchedSprites: ERROR - Failed to update sprite info buffer");
				return false;
			}
		}
	}

	m_SpriteInfoUpdateQueued = false;

	// Calculate how many instances (sprites) to draw
	size_t instanceCount;
	if (m_CustomSpriteIndices.empty()) {
		instanceCount = m_SpritesData.size(); // Draw all sprites
	} else {
		instanceCount = m_CustomSpriteIndices.size(); // Draw only specified sprites
	}

	glm::mat4 mvp = glm::mat4(1.0f); // Identity matrix, like original StructuredBuffer version

	if (!UpdateVertexConstantBuffer(pD3D11DeviceContext, { mvp, s_VertexPerSprite, s_IndexPerSprite })) {
		Log("BatchedSprites: Failed to update vertex constant buffer");
		return false;
	}


	BackupD3D11State(pD3D11DeviceContext);

	// Set Render Target, Viewport, and Scissor Rect
	const D3D11_RECT scissor = { 0, 0, m_RTWidth, m_RTHeight };
	pD3D11DeviceContext->RSSetScissorRects(1, &scissor);
	pD3D11DeviceContext->RSSetViewports(1, &m_VP);
	pD3D11DeviceContext->OMSetRenderTargets(1, m_pRTV.GetAddressOf(), m_pDepthStincilView.Get());

	// Bind Pipeline Resources
	pD3D11DeviceContext->IASetInputLayout(m_pInputLayout.Get());

	UINT stride = sizeof(Vertex_t);
	UINT offset = 0;
	pD3D11DeviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
	pD3D11DeviceContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set Vertex Shader and Resources
	pD3D11DeviceContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	pD3D11DeviceContext->VSSetConstantBuffers(0, 1, m_pVertexConstantBuffer.GetAddressOf());

	// Bind sprite info constant buffer to slot b1
	ID3D11Buffer* spriteInfoBuffers[] = { m_pSpriteInfoBuffer.Get() };
	pD3D11DeviceContext->VSSetConstantBuffers(1, 1, spriteInfoBuffers);

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

	pD3D11DeviceContext->ClearRenderTargetView(m_pRTV.Get(), blendFactor.data());
	pD3D11DeviceContext->ClearDepthStencilView(m_pDepthStincilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// **INSTANCED DRAW CALL**
	pD3D11DeviceContext->DrawIndexedInstanced(
		6,                           // Indices per instance (6 for a quad: 2 triangles)
		(UINT)instanceCount,         // Number of instances (sprites to draw)
		0,                           // Start index location
		0,                           // Base vertex location
		0                            // Start instance location
	);

	RestoreD3D11State(pD3D11DeviceContext);
	return true;
}

std::optional<size_t> BatchedSprites::FindTexturePathIndex(const std::string& path) {
	const auto& count = m_TexturePaths.size();
	for (size_t i = 0; i < count; i++) {
		if (m_TexturePaths[i] == path)
			return i;
	}

	return std::nullopt;
}

void BatchedSprites::PopulateActiveIndices() {
    // For instanced rendering, we don't need complex index management
    // We just track which sprites should be drawn via m_CustomSpriteIndices
    // The actual drawing is controlled by the instance count in DrawIndexedInstanced
    
    if (m_CustomSpriteIndices.empty()) {
        //Log("BatchedSprites: No custom sprite indices, will draw all %zu sprites", m_SpritesData.size());
    } 

    // No need to update index buffer since we use the same quad indices for all instances
    m_UpdateIndexBuffer = false;
}

//void BatchedSprites::SortByDepth()
//{
//
//}

bool BatchedSprites::CreateRenderTargetView(ID3D11Device* pD3D11Device, UINT width, UINT height) {
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

bool BatchedSprites::CreateAndUpdateVertexBuffer(ID3D11Device* pD3D11Device) {
	HRESULT hr;

	// For instanced rendering, we only need one quad (4 vertices)
	// Each instance will use the same quad but with different transformations
	std::vector<Vertex_t> vertices = {
		{ { -1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } }, // Top-left
		{ {  1.0f,  1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } }, // Top-right  
		{ {  1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } }, // Bottom-right
		{ { -1.0f, -1.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } }  // Bottom-left
	};

	D3D11_BUFFER_DESC bufferDesc = {};
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; // Static data, doesn't need to change
	bufferDesc.ByteWidth = m_CurrentVBSize = (UINT)(vertices.size() * sizeof(Vertex_t));
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

bool BatchedSprites::CreateAndUpdateIndexBuffer(ID3D11Device* pD3D11Device) {
	HRESULT hr;

	// For instanced rendering, we only need indices for one quad
	std::vector<uint32_t> indices = { 0, 1, 2, 0, 2, 3 };

	const size_t indicesSize = indices.size() * sizeof(uint32_t);

	D3D11_BUFFER_DESC bufferDesc = {};
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; // Static data
	bufferDesc.ByteWidth = m_CurrentIBSize = (UINT)indicesSize;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0; // No CPU access needed

	D3D11_SUBRESOURCE_DATA initData = {};
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = indices.data();

	hr = pD3D11Device->CreateBuffer(&bufferDesc, &initData, &m_pIndexBuffer);

	if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__)) {
		m_CurrentIBSize = 0;
		return false;
	}

	return true;
}

bool BatchedSprites::CreateAndUpdateSpriteInfoBuffer(ID3D11Device* pD3D11Device) {
	HRESULT hr;

	std::vector<VERTEX_SHADER_SPRITE_INFO> infoArray;
	infoArray.reserve(m_SpritesData.size());

	for (const auto& sd : m_SpritesData) {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), sd.Translation);
        transform *= glm::mat4_cast(sd.RotationQuat);
        transform = glm::scale(transform, sd.Scale);

		infoArray.push_back(VERTEX_SHADER_SPRITE_INFO{
			(uint32_t)sd.TexturePathIdx,
			sd.Opacity,
			sd.Brightness,
			0,
			transform  
			});
	}

	// Resize to 128 elements, filling with default identity matrices
	while (infoArray.size() < 128) {
		infoArray.push_back(VERTEX_SHADER_SPRITE_INFO{ 0, 1.0f, 1.0f, 0, glm::mat4(1.0f) });
	}

	size_t bufferSize = 128 * sizeof(VERTEX_SHADER_SPRITE_INFO);
	bufferSize = (bufferSize + 15) & ~15; // 16-byte alignment

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = m_CurrentSIBSize = (UINT)bufferSize;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = infoArray.data();

	hr = pD3D11Device->CreateBuffer(&bufferDesc, &initData, &m_pSpriteInfoBuffer);

	if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__)) {
		Log("BatchedSprites: Failed to create sprite info buffer, size was %u bytes", (UINT)bufferSize);
		m_CurrentSIBSize = 0;
		return false;
	}

	return true;
}

bool BatchedSprites::UpdateVertexBuffer(ID3D11DeviceContext* pD3D11DeviceContext) {
	HRESULT hr;

	const size_t verticesSize = m_Vertices.size() * sizeof(decltype(m_Vertices)::value_type);

	// If the buffer is not large enough, recreate it
	if (m_CurrentVBSize < verticesSize) {
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

bool BatchedSprites::UpdateIndexBuffer(ID3D11DeviceContext* pD3D11DeviceContext) {
	HRESULT hr;

	const size_t indicesSize = m_ActiveIndices.size() * sizeof(decltype(m_ActiveIndices)::value_type);

	// If the buffer is not large enough, recreate it
	if (m_CurrentIBSize < indicesSize) {
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

bool BatchedSprites::UpdateSpriteInfoBuffer(ID3D11DeviceContext* pD3D11DeviceContext) {
	HRESULT hr;

	std::vector<VERTEX_SHADER_SPRITE_INFO> infoArray;

	// **FIX: Remap sprites based on which ones we're actually drawing**
	if (m_CustomSpriteIndices.empty()) {
		// Draw all sprites in order
		infoArray.reserve(m_SpritesData.size());

		for (size_t i = 0; i < m_SpritesData.size(); i++) {
			const auto& sd = m_SpritesData[i];

            glm::mat4 transform = glm::translate(glm::mat4(1.0f), sd.Translation);
            transform *= glm::mat4_cast(sd.RotationQuat);
            transform = glm::scale(transform, sd.Scale);

			infoArray.push_back(VERTEX_SHADER_SPRITE_INFO{
				(uint32_t)sd.TexturePathIdx,
				sd.Opacity,
				sd.Brightness,
				0,
                transform
				});
		}
	} else {
		// **CRITICAL FIX: Remap custom sprite indices to sequential instances**
		infoArray.reserve(m_CustomSpriteIndices.size());


		for (size_t instanceIdx = 0; instanceIdx < m_CustomSpriteIndices.size(); instanceIdx++) {
			size_t spriteIdx = m_CustomSpriteIndices[instanceIdx];

			if (spriteIdx >= m_SpritesData.size()) {
				Log("BatchedSprites: ERROR - Invalid sprite index %zu", spriteIdx);
				continue;
			}

			const auto& sd = m_SpritesData[spriteIdx];

            glm::mat4 transform = glm::translate(glm::mat4(1.0f), sd.Translation);
            transform *= glm::mat4_cast(sd.RotationQuat);
            transform = glm::scale(transform, sd.Scale);

			infoArray.push_back(VERTEX_SHADER_SPRITE_INFO{
				(uint32_t)sd.TexturePathIdx,
				sd.Opacity,
				sd.Brightness,
				0,
                transform
				});
		}
	}

	// Pad to 128 elements
	while (infoArray.size() < 128) {
		infoArray.push_back(VERTEX_SHADER_SPRITE_INFO{ 0, 1.0f, 1.0f, 0, glm::mat4(1.0f) });
	}

	// Rest of the update logic stays the same...
	size_t bufferSize = 128 * sizeof(VERTEX_SHADER_SPRITE_INFO);

	if (m_CurrentSIBSize < bufferSize) {
		ID3D11Device* pDevice;
		pD3D11DeviceContext->GetDevice(&pDevice);
		return CreateAndUpdateSpriteInfoBuffer(pDevice);
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(mappedResource));

	hr = pD3D11DeviceContext->Map(m_pSpriteInfoBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__)) {
		return false;
	}

	memcpy(mappedResource.pData, infoArray.data(), bufferSize);

	pD3D11DeviceContext->Unmap(m_pSpriteInfoBuffer.Get(), 0);
	return true;
}

bool BatchedSprites::UpdateVertexConstantBuffer(ID3D11DeviceContext* pD3D11DeviceContext, const VERTEX_CONSTANT_BUFFER& cBuffer) {
	if (!m_pVertexConstantBuffer) {
		Log("BatchedSprites: Vertex constant buffer is null!");
		return false;
	}

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	hr = pD3D11DeviceContext->Map(m_pVertexConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__)) {
		Log("BatchedSprites: Failed to map vertex constant buffer");
		return false;
	}

	VERTEX_CONSTANT_BUFFER* constantBuffer = (VERTEX_CONSTANT_BUFFER*)mappedResource.pData;
	*constantBuffer = cBuffer;

	pD3D11DeviceContext->Unmap(m_pVertexConstantBuffer.Get(), 0);
	return true;
}

bool BatchedSprites::UpdatePixelConstantBuffer(ID3D11DeviceContext* pD3D11DeviceContext, const PIXEL_CONSTANT_BUFFER& cBuffer) {
	if (!m_pPixelConstantBuffer) {
		// Log error or handle appropriately
		return false;
	}

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

void BatchedSprites::Translate(size_t idx, glm::vec3 vector) {
	m_SpriteInfoUpdateQueued = true;

	m_SpritesData[idx].Translation += vector;
}

void BatchedSprites::TranslateTo(size_t idx, glm::vec3 vector) {
	m_SpriteInfoUpdateQueued = true;

	m_SpritesData[idx].Translation = vector;
}

void BatchedSprites::Rotate(size_t idx, float rotation, glm::vec3 axis) {
	m_SpriteInfoUpdateQueued = true;

    m_SpritesData[idx].RotationQuat = glm::angleAxis(rotation, glm::normalize(axis)) * m_SpritesData[idx].RotationQuat;
}

void BatchedSprites::RotateTo(size_t idx, float rotation, glm::vec3 axis) {
	m_SpriteInfoUpdateQueued = true;

    m_SpritesData[idx].RotationQuat = glm::angleAxis(rotation, glm::normalize(axis));
}

void BatchedSprites::Scale(size_t idx, glm::vec3 vector) {
	m_SpriteInfoUpdateQueued = true;

	m_SpritesData[idx].Scale *= vector;
}

void BatchedSprites::ScaleTo(size_t idx, glm::vec3 vector) {
	m_SpriteInfoUpdateQueued = true;

	m_SpritesData[idx].Scale = vector;
}

void BatchedSprites::SetTransform(size_t idx, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale) {
	m_SpriteInfoUpdateQueued = true;

	m_SpritesData[idx].Translation = translation;
    m_SpritesData[idx].RotationQuat = glm::quat(rotation);
    m_SpritesData[idx].Scale = scale;
}

void BatchedSprites::SetOpacity(size_t idx, float opacity) {
	m_SpriteInfoUpdateQueued = true;

	m_SpritesData[idx].Opacity = opacity;
}

void BatchedSprites::SetBrightness(size_t idx, float brightness) {
	m_SpriteInfoUpdateQueued = true;

	m_SpritesData[idx].Brightness = brightness;
}

void BatchedSprites::ForceUpdateSpriteInfoBuffer() {
	if (m_SpriteInfoUpdateQueued && m_pSpriteInfoBuffer) {
		// Get device context from the current device
		ComPtr<ID3D11Device> pDevice;
		m_pSpriteInfoBuffer->GetDevice(&pDevice);

		ComPtr<ID3D11DeviceContext> pContext;
		pDevice->GetImmediateContext(&pContext);

		UpdateSpriteInfoBuffer(pContext.Get());
		m_SpriteInfoUpdateQueued = false;
	}
}

void BatchedSprites::Reorder(const std::vector<size_t>& spriteIndices) {
	// Todo: Order back to front
}

bool BatchedSprites::CreateSpriteInfoBufferSrv(ID3D11Device* pD3D11Device) {
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

void BatchedSprites::BackupD3D11State(ID3D11DeviceContext* ctx) {
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

void BatchedSprites::RestoreD3D11State(ID3D11DeviceContext* ctx) {
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

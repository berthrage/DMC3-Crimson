#include "BatchedSprites.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtx\euler_angles.hpp"
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
        m_MaskInfoData.clear();
    	m_SpriteInstanceData.clear();
    	m_pTextureArray.reset();
    
    	const auto count = spriteIndices.size();
    	m_SpriteInstanceData.reserve(count);
        m_SpriteUpdateInfo.ResizeList(count);
        m_MaskUpdateInfo.ResizeList(count);
    
    	// Process sprite data
    	for (size_t i = 0; i < count; i++) {
    		const auto& spriteIdx   = spriteIndices[i];
            const auto& spriteInfo  = spriteDescs[spriteIdx];
            const auto& regionRect  = spriteInfo.GetRegionRect();
            const auto& tintCol     = spriteInfo.GetTint();
    
    		m_SpriteInstanceData.push_back(
    			SpriteInfo
                {
    				.TextureIdx     = spriteInfo.GetArrayIdx(),
    				.Opacity        = spriteInfo.GetOpacity(),
    				.Brightness     = spriteInfo.GetBrightness(),
                    .Tint           = { tintCol.R, tintCol.G, tintCol.B, tintCol.A },
                    .RectRegion     = { regionRect.L, regionRect.T, regionRect.R, regionRect.B },
                    .MaskStartIdx   = 0,
                    .MaskCount      = 0,
    				.Translation    = spriteInfo.GetTranslation(),
    				.Rotation       = spriteInfo.GetRotation(),
    				.Scale          = spriteInfo.GetScale()
    			}
    		);
    	}

        m_pTextureArray = pSpriteTextureArray;
    
    	if (!m_pTextureArray) {
    		Log("BatchedSprites: ERROR - Texture array invalid!");
    		return;
    	}
    
    	m_SpriteIndices = spriteIndices;
    
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
    
    
    		constexpr const char* VERTEX_SHADER =
    			R"(
                #pragma pack_matrix(row_major)

    			cbuffer vertexBuffer : register(b0) 
    			{
    				float4x4 projectionMatrix;
                    float    opacity;
                    float    brightness;
                    float2   vpDims;
                    int      ratioFlags;
    			};
    
    			struct VS_INPUT
    			{
                    // Per-vertex (Slot 0)
    				float3   Pos          : POSITION;
    				float2   UV           : TEXCOORD0;
    				float4   Col          : COLOR0;

                    // Per-instance (Slot 1)
    				uint     TextureIdx   : TEXTURE_IDX;
    				float    Opacity      : OPACITY;
    				float    Brightness   : BRIGHTNESS;
                    float2   TextureDims  : TEXDIMS;
                    float4   Tint         : TINT; // RGBA
                    float4   RectRegion   : RECT; // LTRB
                    uint     MaskStartIdx : MASK_START_IDX;
                    uint     MaskCount    : MASK_COUNT;
                                          
    				float4x4 Transformn   : TRANSFORM;
    			};
    
    			struct PS_INPUT
    			{
    				float4 Pos          : SV_POSITION;
    				float2 UV           : TEXCOORD0;
    				float4 Col          : COLOR0;
                    float4 RectRegion   : RECT; // LTRB
                    float  AspectRatio  : ASPECT;
                    uint   MaskStartIdx : MASK_START_IDX;
                    uint   MaskCount    : MASK_COUNT;
    				uint   Tid          : TEXTURE_IDX;
    				float  Opacity      : OPACITY;
    				float  Bright       : BRIGHTNESS;
    			};

    			PS_INPUT main(VS_INPUT input)
    			{
    				PS_INPUT output;

                    float2 rectSizeNormalized = float2(input.RectRegion.zw) - float2(input.RectRegion.xy);
                    float2 rectSizePixels     = input.TextureDims * rectSizeNormalized;
                    float  vpAspectRatio      = vpDims.x/vpDims.y;
                    float  spriteAspectRatio  = rectSizePixels.x/rectSizePixels.y;
                    float3 sizeFixScaleFactor = float3(1.0f, 1.0f, 1.0f);

                    const bool keepOrgSize   = (ratioFlags >> 0) & 1 == 1;
                    const bool fitToVPHeight = (ratioFlags >> 1) & 1 == 1;
                    const bool fitToVPWidth  = (ratioFlags >> 2) & 1 == 1;
                    const bool fitToVP       = (ratioFlags >> 3) & 1 == 1;

                    if (fitToVP)
                    {
                        float factor = spriteAspectRatio / vpAspectRatio;

                        if (factor > 1.0)
                        {
                            sizeFixScaleFactor.y /= factor;
                        }
                        else
                        {
                            sizeFixScaleFactor.x *= factor;
                        }
                    }
                    else if (fitToVPHeight)
                    {
                        float factor = spriteAspectRatio / vpAspectRatio;
                        sizeFixScaleFactor.x *= factor;
                    }
                    else if (fitToVPWidth)
                    {
                        float factor = spriteAspectRatio / vpAspectRatio;
                        sizeFixScaleFactor.y /= factor;
                    }
                    else if (keepOrgSize)
                    {
                        sizeFixScaleFactor.x *= rectSizePixels.x / vpDims.x;
                        sizeFixScaleFactor.y *= rectSizePixels.y / vpDims.y;
                    }

                    float4   origin      = float4(input.Pos * sizeFixScaleFactor, 1.0f);
                    float2   rectTopLeft = float2(input.RectRegion.x, input.RectRegion.y);

    				output.Pos          = mul(mul(origin, input.Transformn), projectionMatrix);
    				output.Col          = input.Col * input.Tint;
    				output.UV           = rectTopLeft + (input.UV * rectSizeNormalized);
    				output.RectRegion   = input.RectRegion;
                    output.AspectRatio  = spriteAspectRatio;
                    output.MaskStartIdx = input.MaskStartIdx;
                    output.MaskCount    = input.MaskCount;
    				output.Tid          = input.TextureIdx;
    				output.Opacity      = opacity * input.Opacity;
    				output.Bright       = brightness * input.Brightness;
    
    				return output;
    			}
    			)";
    
    		//Log("BatchedSprites: About to compile vertex shader");
    
    		ComPtr<ID3DBlob> pVertexShaderBlob;
    		ComPtr<ID3DBlob> pErrorBlob;
    
    		hr = D3DCompile(VERTEX_SHADER, strlen(VERTEX_SHADER), NULL, NULL, NULL, "main", "vs_4_0", 0, 0, &pVertexShaderBlob, &pErrorBlob);
    
            if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
            {
                Log(GetLastErrorMsg().c_str());
                Log("BatchedSprites: Vertex shader compilation FAILED: %s", (const char*)pErrorBlob->GetBufferPointer());

                return false;
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
    
    		constexpr const char* PIXEL_SHADER =
    			R"(
                float SlantedMask(
                    float2 p,
                    float2 origin,
                    float2 dirNormal,
                    float2 softnessRange
                )
                {
                    const float2 d       = p - origin;
                    const float  lineD   = dot(d, dirNormal); // Distance from the line
                    
                    return smoothstep(softnessRange.x, softnessRange.y, lineD);
                }

                float RadialMask(
                    float2 p,
                    float2 origin,
                    float  radiusSquared,
                    float2 startDir,
                    float2 endDir,
                    float  keepPositive,
                    float  flipMask
                )
                {
                    float2 d = p - origin;

                    const float dist2      = dot(d, d); // Distance squared from origin
                    const float radiusMask = step(dist2, radiusSquared); // Radius mask

                    const float crossStart = startDir.x * d.y - startDir.y * d.x;
                    const float crossEnd   = d.x * endDir.y - d.y * endDir.x;
                    const float posMask    = step(0.0f, crossStart) * step(0.0f, crossEnd);
                    const float negMask    = 1.0f - posMask;
                    const float angleMask  = lerp(negMask, posMask, keepPositive);
                    const float mask       = radiusMask * angleMask;
                    
                    return abs(flipMask - mask);
                }
    
    			struct PS_INPUT
    			{
    				float4 Pos          : SV_POSITION;
    				float2 UV           : TEXCOORD0;
    				float4 Col          : COLOR0;
                    float4 RectRegion   : RECT; // LTRB
                    float  AspectRatio  : ASPECT;
                    uint   MaskStartIdx : MASK_START_IDX;
                    uint   MaskCount    : MASK_COUNT;
    				uint   Tid          : TEXTURE_IDX;
    				float  Opacity      : OPACITY;
    				float  Bright       : BRIGHTNESS;
    			};

                struct MASK_INFO
                {
                    uint Type;
                    uint pad0;
                    uint pad1;
                    uint pad2;
                
                    float4 Param4Pack0;
                    float4 Param4Pack1;
                    float4 Param4Pack2;
                };

                static const uint MASK_TYPE_NONE    = 0;
                static const uint MASK_TYPE_SLANTED = 1;
                static const uint MASK_TYPE_RADIAL  = 2;

                static const uint MAX_MASK_COUNT    = 4;

        		cbuffer boundInfo : register(b0)
    		    {
    		    	uint TextureCount;
    		    	uint SamplerCount;
                    uint MaskCount;
    		    };
    		    sampler sampler0                   : register(s0);
    		    Texture2DArray textureArray0       : register(t0);
                StructuredBuffer<MASK_INFO> masks1 : register(t1);

    		    float4 main(PS_INPUT input) : SV_Target
    		    {
    		    	float4 outCol = input.Col * textureArray0.Sample(sampler0, float3(input.UV, input.Tid));
    		    	outCol        = float4(outCol.xyz * input.Bright, outCol.w);
    		    	outCol.w      *= input.Opacity;

                    float2 localUV = (input.UV - input.RectRegion.xy) / (input.RectRegion.zw - input.RectRegion.xy);
                    float2 pBase   = float2(localUV.x * input.AspectRatio, 1.0f - localUV.y);

                    float mask = 1.0f;

                    // Get rid of this attribute if mask counts above 4 are required
                    [unroll(MAX_MASK_COUNT)]
                    for(uint i = 0; i < input.MaskCount; i++)
                    {
                        if (i >= input.MaskCount)
                            break;

                        uint maskIdx = input.MaskStartIdx + i;

                        MASK_INFO m   = masks1[maskIdx];
                        float2 origin = m.Param4Pack0.xy;

                        float m0 = SlantedMask(
                            pBase,
                            origin,
                            m.Param4Pack0.zw, // Line normal direction
                            m.Param4Pack1.xy  // Softness range 
                        );

                        float m1 = RadialMask(
                            pBase,
                            origin,
                            m.Param4Pack0.z,                          // Radius squared
                            float2(m.Param4Pack0.w, m.Param4Pack1.x), // Start direction
                            m.Param4Pack1.yz,                         // End direction
                            m.Param4Pack1.w,                          // Keep positive side of the start dir
                            m.Param4Pack2.x                           // Flip mask
                        );

                        float use0 = (m.Type == MASK_TYPE_SLANTED);
                        float use1 = (m.Type == MASK_TYPE_RADIAL);

                        mask *= m0 * use0 + m1 * use1;

                        // Optimization: If a mask already 0ed the value, continuing has no point, it'll always be 0
                        if(mask <= 0.0f)
                            break;
                    }

                    return outCol * mask;
    		    }
    		    )";
    
    		//Log("BatchedSprites: About to compile pixel shader");
    
    		ComPtr<ID3DBlob> pPixelShaderBlob;
    		ComPtr<ID3DBlob> pErrorBlob;
    
    		hr = D3DCompile(PIXEL_SHADER, strlen(PIXEL_SHADER), NULL, NULL, NULL, "main", "ps_4_0", 0, 0, &pPixelShaderBlob, &pErrorBlob);
    
            if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
            {
                Log(GetLastErrorMsg().c_str());
                Log("BatchedSprites: Pixel shader compilation FAILED: %s", (const char*)pErrorBlob->GetBufferPointer());

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
    		desc.ByteWidth      = sizeof(VERTEX_CONSTANT_BUFFER);
    		desc.Usage          = D3D11_USAGE_DYNAMIC;
    		desc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    		desc.MiscFlags      = 0;
    
    		hr = pD3D11Device->CreateBuffer(&desc, NULL, &m_pVertexConstantBuffer);
    
    		if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
    			return false;
    	}
    	
    	// Create the constat buffer for the pixel shader
    	{
    		D3D11_BUFFER_DESC desc;
    		desc.ByteWidth      = sizeof(PIXEL_CONSTANT_BUFFER);
    		desc.Usage          = D3D11_USAGE_DYNAMIC;
    		desc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    		desc.MiscFlags      = 0;
    
    		hr = pD3D11Device->CreateBuffer(&desc, NULL, &m_pPixelConstantBuffer);
    
    		if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
    			return false;
    	}
    	//Log("BatchedSprites: Constant buffers created");
    
    	// Create the blending setup
    	{
    		D3D11_BLEND_DESC desc;
    		ZeroMemory(&desc, sizeof(desc));
    		desc.AlphaToCoverageEnable                 = false;
    		desc.RenderTarget[0].BlendEnable           = true;
    		desc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA; // Source color is blended based on its alpha
    		desc.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA; // Destination color is blended based on inverse source alpha
    		desc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD; // Add source and destination colors
    		desc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE; // Source alpha is fully preserved
    		desc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ONE; // Destination alpha is fully preserved
    		desc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_MAX; // Use maximum of source and destination alpha
    		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    
    		hr = pD3D11Device->CreateBlendState(&desc, &m_pBlendState);
    
    		if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
    			return false;
    	}
    
    	// Create the rasterizer state
    	{
    		D3D11_RASTERIZER_DESC desc;
    		ZeroMemory(&desc, sizeof(desc));
    		desc.FillMode        = D3D11_FILL_SOLID;
    		desc.CullMode        = D3D11_CULL_NONE;
    		desc.ScissorEnable   = true;
    		desc.DepthClipEnable = true;
    
    		hr = pD3D11Device->CreateRasterizerState(&desc, &m_pRasterizerState);
    
    		if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
    			return false;
    	}
    
    	// Create depth-stencil State
    	{
    		D3D11_DEPTH_STENCIL_DESC desc;
    		ZeroMemory(&desc, sizeof(desc));
    		desc.DepthEnable    = true;
    		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    		desc.DepthFunc      = D3D11_COMPARISON_LESS_EQUAL;
    		//desc.StencilEnable = false;
    		//desc.FrontFace.StencilFailOp = desc.FrontFace.StencilDepthFailOp = desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    		//desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    		//desc.BackFace = desc.FrontFace;
    
    		hr = pD3D11Device->CreateDepthStencilState(&desc, &m_pDepthStencilStateOpaque);
    
    		if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
    			return false;
    
    		ZeroMemory(&desc, sizeof(desc));
    		desc.DepthEnable    = true;
    		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    		desc.DepthFunc      = D3D11_COMPARISON_LESS_EQUAL;
    		hr = pD3D11Device->CreateDepthStencilState(&desc, &m_pDepthStencilStateTransparent);
            
    		if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
    			return false;
    	}
    
    	// Create depth-stencil
    	{
    		D3D11_TEXTURE2D_DESC desc;
    		ZeroMemory(&desc, sizeof(desc));
    		desc.Width              = m_RTDims.x;
    		desc.Height             = m_RTDims.y;
    		desc.MipLevels          = 1;
    		desc.ArraySize          = 1;
    		desc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    		desc.SampleDesc.Count   = 1;
    		desc.SampleDesc.Quality = 0;
    		desc.Usage              = D3D11_USAGE_DEFAULT;
    		desc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
    		desc.CPUAccessFlags     = 0;
    		desc.MiscFlags          = 0;
    
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
            desc.Filter         = D3D11_FILTER_ANISOTROPIC;//D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    		desc.AddressU       = D3D11_TEXTURE_ADDRESS_WRAP;
    		desc.AddressV       = D3D11_TEXTURE_ADDRESS_WRAP;
    		desc.AddressW       = D3D11_TEXTURE_ADDRESS_WRAP;
    		desc.MipLODBias     = 0.0f;
    		desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    		desc.MinLOD         = 0.0f;
    		desc.MaxLOD         = 0.0f;
    
    		hr = pD3D11Device->CreateSamplerState(&desc, &m_pTextureSampler);
    
    		if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
    			return false;
    	}
    
    	// Create viewport
    	{
    		// Setup viewport
    		memset(&m_ViewPort, 0, sizeof(D3D11_VIEWPORT));
    		m_ViewPort.Width    = (FLOAT)m_RTDims.x;
    		m_ViewPort.Height   = (FLOAT)m_RTDims.y;
    		m_ViewPort.MinDepth = 0.0f;
    		m_ViewPort.MaxDepth = 1.0f;
    		m_ViewPort.TopLeftX = m_ViewPort.TopLeftY = 0;
    	}
    
    	//Log("BatchedSprites: Initialize completed successfully");
    	return true;
    }
    
    bool BatchedSprites::Draw(ID3D11DeviceContext* pD3D11DeviceContext)
    {
    	if (!UpdateSpriteInstanceInfoBuffer(pD3D11DeviceContext))
        {
    		Log("BatchedSprites: ERROR - Failed to update sprite info buffer");
    		return false;
    	}

        if (!UpdatePixelMaskBuffer(pD3D11DeviceContext))
        {
            Log("BatchedSprites: ERROR - Failed to update mask info buffer");
            return false;
        }

        // Update vertex shader constant buffer
        {
            constexpr auto ORTHO_MVP = glm::mat4(1.0f); // An orthographic view projection for sprite rendering

            VERTEX_CONSTANT_BUFFER cBuffer =
            {
                .MVP            = ORTHO_MVP,
                .Opacity        = m_Opacity,
                .Brightness     = m_Brightness,
                .VPDims         = m_RTDims,
                .RatioFlags     = m_RatioFlags
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
                .TextureCount = (uint32_t)m_pTextureArray->GetCount(), // Number of textures in the array
                .SamplerCount = 1, // Currently only have one sampler
                .MaskCount    = (uint32_t)m_MaskInfoData.size(), // Number of masks in the mask buffer
            };

            if (!UpdatePixelConstantBuffer(pD3D11DeviceContext, cBuffer))
            {
                Log("BatchedSprites: Failed to update pixel shader constant buffer");
                return false;
            }
        }
    
    	BackupD3D11State(pD3D11DeviceContext);
    
    	// Set Render Target, Viewport, and Scissor Rect
    	const D3D11_RECT scissor = { 0, 0, m_RTDims.x, m_RTDims.y };
    	pD3D11DeviceContext->RSSetScissorRects(1, &scissor);
    	pD3D11DeviceContext->RSSetViewports(1, &m_ViewPort);
    	pD3D11DeviceContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_pDepthStincilView.Get());
    
    	// Bind Pipeline Resources
    	pD3D11DeviceContext->IASetInputLayout(m_pInputLayout.Get());

        {
            const std::array<ID3D11Buffer*, 2> buffers = { m_pVertexBuffer.Get(), m_pSpriteInstanceInfoBuffer.Get() };
            const std::array<UINT, 2> strides = { sizeof(Vertex_t), sizeof(VERTEX_SHADER_SPRITEINSTANCE_INFO) };
            const std::array<UINT, 2> offsets = { 0, 0 };

            pD3D11DeviceContext->IASetVertexBuffers(0, buffers.size(), buffers.data(), strides.data(), offsets.data());
        }

        pD3D11DeviceContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    	pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    	// Set Vertex Shader and Resources
    	pD3D11DeviceContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
    	pD3D11DeviceContext->VSSetConstantBuffers(0, 1, m_pVertexConstantBuffer.GetAddressOf());
    
    	// Set Pixel Shader and Resources
    	pD3D11DeviceContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
    	pD3D11DeviceContext->PSSetConstantBuffers(0, 1, m_pPixelConstantBuffer.GetAddressOf());

        {
            std::array<ID3D11ShaderResourceView*, 2> srvs = { m_pTextureArray->GetSrv(), m_pMaskInfoBufferSRV.Get() };
            pD3D11DeviceContext->PSSetShaderResources(0, srvs.size(), srvs.data());
        }

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
    	textureDesc.Width            = m_RTDims.x = width;
    	textureDesc.Height           = m_RTDims.y = height;
    	textureDesc.MipLevels        = 1;
    	textureDesc.ArraySize        = 1;
    	textureDesc.Format           = DXGI_FORMAT_R8G8B8A8_UNORM;
    	textureDesc.SampleDesc.Count = 1; // Set multisampling
    	textureDesc.Usage            = D3D11_USAGE_DEFAULT;
    	textureDesc.BindFlags        = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    	textureDesc.CPUAccessFlags   = 0; // No CPU access needed
    
    	ComPtr<ID3D11Texture2D> pRenderTargetTexture;
    	hr = pD3D11Device->CreateTexture2D(&textureDesc, nullptr, &pRenderTargetTexture);
    
    	if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
    		return false;
    
    	hr = pD3D11Device->CreateRenderTargetView(pRenderTargetTexture.Get(), nullptr, &m_pRenderTargetView);
    
    	if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
    		return false;
    
    	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    	ZeroMemory(&srvDesc, sizeof(srvDesc));
    
    	srvDesc.Format                    = DXGI_FORMAT_R8G8B8A8_UNORM;
    	srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
    	srvDesc.Texture2D.MostDetailedMip = 0;
    	srvDesc.Texture2D.MipLevels       = 1;
    
    	hr = pD3D11Device->CreateShaderResourceView(pRenderTargetTexture.Get(), &srvDesc, &m_pRenderTargetSRV);
    
    	if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
    		return false;
    
    	return true;
    }
    
    bool BatchedSprites::CreateVertexBuffer(ID3D11Device* pD3D11Device)
    {
    	HRESULT hr;

        // Same vertices for all instances
    	static constexpr std::array<Vertex_t, 4> VERTICES = {
    		Vertex_t{ { -1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } }, // Top-left
    		Vertex_t{ {  1.0f,  1.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } }, // Top-right  
    		Vertex_t{ {  1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } }, // Bottom-right
    		Vertex_t{ { -1.0f, -1.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } }  // Bottom-left
    	};
    
    	D3D11_BUFFER_DESC bufferDesc = {};
    	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
    
    	bufferDesc.Usage          = D3D11_USAGE_IMMUTABLE; // Static data, doesn't need to change
    	bufferDesc.ByteWidth      = m_CurrentVBSize = (UINT)sizeof(VERTICES);
    	bufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
    	bufferDesc.CPUAccessFlags = 0; // No CPU access needed
    
    	D3D11_SUBRESOURCE_DATA initData = {};
    	ZeroMemory(&initData, sizeof(initData));
    	initData.pSysMem = VERTICES.data();
    
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
    	bufferDesc.Usage          = D3D11_USAGE_IMMUTABLE; // Static data
    	bufferDesc.ByteWidth      = m_CurrentIBSize = (UINT)sizeof(s_SpriteIndices);
    	bufferDesc.BindFlags      = D3D11_BIND_INDEX_BUFFER;
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
    
		m_InstanceInfoBufferArray.reserve(m_SpriteInstanceData.size());
		m_InstanceInfoBufferArray.clear();

    	for (const auto& sd : m_SpriteInstanceData) {
            m_InstanceInfoBufferArray.push_back(VERTEX_SHADER_SPRITEINSTANCE_INFO
                {
    			    .TextureIdx     = (uint32_t)sd.TextureIdx,
    			    .Opacity        = sd.Opacity,
    			    .Brightness     = sd.Brightness,
                    .TextureDims    = { m_pTextureArray->GetWidth(), m_pTextureArray->GetHeight() },
                    .Tint           = sd.Tint,
                    .RectRegion     = sd.RectRegion,
                    .MaskStartIdx   = sd.MaskStartIdx,
                    .MaskCount      = sd.MaskCount,
                    .Transform      = CreateTransformMatrix(sd.Translation, sd.Rotation, sd.Scale)
    			});
    	}

        size_t byteSize = m_InstanceInfoBufferArray.size() * sizeof(decltype(m_InstanceInfoBufferArray)::value_type);
    
    	D3D11_BUFFER_DESC bufferDesc;
    	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
    	bufferDesc.Usage               = D3D11_USAGE_DYNAMIC;
    	bufferDesc.ByteWidth           = m_CurrentSpriteInstanceInfoBufferSize = (UINT)byteSize;
    	bufferDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
    	bufferDesc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
    	bufferDesc.MiscFlags           = 0;
    	bufferDesc.StructureByteStride = 0;
    
    	D3D11_SUBRESOURCE_DATA initData = {};
    	ZeroMemory(&initData, sizeof(initData));
    	initData.pSysMem = m_InstanceInfoBufferArray.data();
    
    	hr = pD3D11Device->CreateBuffer(&bufferDesc, &initData, &m_pSpriteInstanceInfoBuffer);
    
    	if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__)) {
    		m_CurrentSpriteInstanceInfoBufferSize = 0;
    		return false;
    	}
    
    	return true;
    }

    bool BatchedSprites::CreateAndUpdateMaskInfoBuffer(ID3D11Device* pD3D11Device)
    {
        HRESULT hr;

        m_MaskInfoBufferArray.reserve(m_MaskInfoData.size());
        m_MaskInfoBufferArray.clear();

        for (const auto& md : m_MaskInfoData) {
            switch (md.Type)
            {
            case MaskType_t::SlantedMask:
            {
                const auto lineNormalAngle = glm::radians(md.Slanted.LineNormalDeg);

                m_MaskInfoBufferArray.push_back(PIXEL_MASK_INFO
                    {
                        .Type = (uint32_t)md.Type,
                        .Slanted = 
                        {
                            .Origin        = md.Slanted.Origin,
                            .DirNormal     = { glm::cos(lineNormalAngle), glm::sin(lineNormalAngle) },
                            .SoftnessRange = md.Slanted.SoftnessRange
                        }
                    });
            } break;

            case MaskType_t::RadialMask:
            {
                const auto startAngle = glm::radians(md.Radial.StartDeg);
                const auto endAngle   = glm::radians(md.Radial.EndDeg);

                m_MaskInfoBufferArray.push_back(PIXEL_MASK_INFO
                    {
                        .Type = (uint32_t)md.Type,
                        .Radial = 
                        {
                            .Origin        = md.Radial.Origin,
                            .RadiusSquared = md.Radial.Radius * md.Radial.Radius, // Radius squared because more optimized for shader
                            .StartDir      = { glm::cos(startAngle), glm::sin(startAngle) },
                            .EndDir        = { glm::cos(endAngle), glm::sin(endAngle)},
                            .KeepPositive  = md.Radial.KeepPositive ? 1.0f : 0.0f,
                            .FlipMask      = md.Radial.FlipMask ? 1.0f : 0.0f
                        }
                    });
            } break;

            default:
            {
                m_MaskInfoBufferArray.push_back(PIXEL_MASK_INFO
                    {
                        .Type = (uint32_t)md.Type
                    });
            } break;
            }
        }

        size_t byteSize = m_MaskInfoBufferArray.size() * sizeof(decltype(m_MaskInfoBufferArray)::value_type);

        D3D11_BUFFER_DESC bufferDesc;
        ZeroMemory(&bufferDesc, sizeof(bufferDesc));
        bufferDesc.Usage               = D3D11_USAGE_DYNAMIC;
        bufferDesc.ByteWidth           = m_CurrentMaskInfoBufferSize = (UINT)byteSize;
        bufferDesc.BindFlags           = D3D11_BIND_SHADER_RESOURCE;
        bufferDesc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
        bufferDesc.MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        bufferDesc.StructureByteStride = sizeof(decltype(m_MaskInfoBufferArray)::value_type);

        D3D11_SUBRESOURCE_DATA initData = {};
        ZeroMemory(&initData, sizeof(initData));
        initData.pSysMem = m_MaskInfoBufferArray.data();

        hr = pD3D11Device->CreateBuffer(&bufferDesc, &initData, &m_pPixelMaskInfoStructuredBuffer);

        if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__)) {
            m_CurrentMaskInfoBufferSize = 0;
            return false;
        }

        return CreateMaskInfoBufferSrv(pD3D11Device);
    }
    
    bool BatchedSprites::UpdateSpriteInstanceInfoBuffer(ID3D11DeviceContext* pD3D11DeviceContext)
    {
        if (!m_SpriteUpdateInfo.IsUpdateQueued())
            return true;

    	HRESULT hr;

        m_InstanceInfoBufferArray.reserve(m_SpriteInstanceData.size());

        // If no specific indices were given or buffer size doesn't match the info,
        // update the whole buffer
        if (m_SpriteUpdateInfo.IsUpdateAllQueued() || m_InstanceInfoBufferArray.size() != m_SpriteInstanceData.size())
        {
            m_InstanceInfoBufferArray.clear(); // Clear the old data
            for (const auto& sd : m_SpriteInstanceData)
            {
                m_InstanceInfoBufferArray.push_back(VERTEX_SHADER_SPRITEINSTANCE_INFO
                    {
                        .TextureIdx     = (uint32_t)sd.TextureIdx,
                        .Opacity        = sd.Opacity,
                        .Brightness     = sd.Brightness,
                        .TextureDims    = { m_pTextureArray->GetWidth(), m_pTextureArray->GetHeight() },
                        .Tint           = sd.Tint,
                        .RectRegion     = sd.RectRegion,
                        .MaskStartIdx   = sd.MaskStartIdx,
                        .MaskCount      = sd.MaskCount,
                        .Transform      = CreateTransformMatrix(sd.Translation, sd.Rotation, sd.Scale)
                    });
            }
        }
        else // If specific indices were given and the buffer size is not changed, only update the given indices
        {
            for (const auto& idx : m_SpriteUpdateInfo.GetDirtyIndices())
            {
                // Only update dirty ones once
                if (!m_SpriteUpdateInfo.ConsumeUpdate(idx))
                    continue;

                const auto& sd = m_SpriteInstanceData[idx];
                m_InstanceInfoBufferArray[idx] = VERTEX_SHADER_SPRITEINSTANCE_INFO
                    {
                        .TextureIdx     = (uint32_t)sd.TextureIdx,
                        .Opacity        = sd.Opacity,
                        .Brightness     = sd.Brightness,
                        .TextureDims    = { m_pTextureArray->GetWidth(), m_pTextureArray->GetHeight() },
                        .Tint           = sd.Tint,
                        .RectRegion     = sd.RectRegion,
                        .MaskStartIdx   = sd.MaskStartIdx,
                        .MaskCount      = sd.MaskCount,
                        .Transform      = CreateTransformMatrix(sd.Translation, sd.Rotation, sd.Scale)
                    };
            }
        }

        m_SpriteUpdateInfo.DequeueUpdate();

    	size_t byteSize = m_InstanceInfoBufferArray.size() * sizeof(decltype(m_InstanceInfoBufferArray)::value_type);
    
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
    
    	::memcpy_s(mappedResource.pData, m_CurrentSpriteInstanceInfoBufferSize, m_InstanceInfoBufferArray.data(), byteSize);
    
    	pD3D11DeviceContext->Unmap(m_pSpriteInstanceInfoBuffer.Get(), 0);

    	return true;
    }
    
    bool BatchedSprites::UpdateVertexConstantBuffer(ID3D11DeviceContext* pD3D11DeviceContext, const VERTEX_CONSTANT_BUFFER& cBuffer)
    {
    	if (!m_pVertexConstantBuffer)
        {
    		Log("BatchedSprites: Vertex shader constant buffer is null!");
    		return false;
    	}
    
    	HRESULT hr;
    	D3D11_MAPPED_SUBRESOURCE mappedResource;
    	hr = pD3D11DeviceContext->Map(m_pVertexConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    
    	if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
        {
    		Log("BatchedSprites: Failed to map vertex shader constant buffer");
    		return false;
    	}

        ::memcpy_s(mappedResource.pData, sizeof(VERTEX_CONSTANT_BUFFER), &cBuffer, sizeof(cBuffer));
    
    	pD3D11DeviceContext->Unmap(m_pVertexConstantBuffer.Get(), 0);
    	return true;
    }
    
    bool BatchedSprites::UpdatePixelConstantBuffer(ID3D11DeviceContext* pD3D11DeviceContext, const PIXEL_CONSTANT_BUFFER& cBuffer)
    {
    	if (!m_pPixelConstantBuffer)
        {
            Log("BatchedSprites: Pixel shader constant buffer is null!");
    		return false;
    	}
    
    	HRESULT hr;
    
    	D3D11_MAPPED_SUBRESOURCE mappedResource;
    	ZeroMemory(&mappedResource, sizeof(mappedResource));
    
    	hr = pD3D11DeviceContext->Map(m_pPixelConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    
        if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
        {
            Log("BatchedSprites: Failed to map pixel shader constant buffer");
            return false;
        }

        ::memcpy_s(mappedResource.pData, sizeof(VERTEX_CONSTANT_BUFFER), &cBuffer, sizeof(cBuffer));
    
    	pD3D11DeviceContext->Unmap(m_pPixelConstantBuffer.Get(), 0);
    
    	return true;
    }

    bool BatchedSprites::UpdatePixelMaskBuffer(ID3D11DeviceContext* pD3D11DeviceContext)
    {
        if (!m_MaskUpdateInfo.IsUpdateQueued())
            return true;

        HRESULT hr;

        m_MaskInfoBufferArray.reserve(m_MaskInfoData.size());

        // If no specific indices were given or buffer size doesn't match the info,
        // update the whole buffer
        if (m_MaskUpdateInfo.IsUpdateAllQueued() || m_MaskInfoBufferArray.size() != m_MaskInfoData.size())
        {
            m_MaskInfoBufferArray.clear(); // Clear the old data
            for (const auto& md : m_MaskInfoData)
            {
                switch (md.Type)
                {
                case MaskType_t::SlantedMask:
                {
                    const auto lineNormalAngle = glm::radians(md.Slanted.LineNormalDeg);

                    m_MaskInfoBufferArray.push_back(PIXEL_MASK_INFO
                        {
                            .Type = (uint32_t)md.Type,
                            .Slanted = {
                                .Origin        = md.Slanted.Origin,
                                .DirNormal     = { glm::cos(lineNormalAngle), glm::sin(lineNormalAngle) },
                                .SoftnessRange = md.Slanted.SoftnessRange
                            }
                        });
                } break;

                case MaskType_t::RadialMask:
                {
                    const auto startAngle = glm::radians(md.Radial.StartDeg);
                    const auto endAngle   = glm::radians(md.Radial.EndDeg);

                    m_MaskInfoBufferArray.push_back(PIXEL_MASK_INFO
                        {
                            .Type = (uint32_t)md.Type,
                            .Radial = {
                                .Origin        = md.Radial.Origin,
                                .RadiusSquared = md.Radial.Radius * md.Radial.Radius, // Radius squared because more optimized for shader
                                .StartDir      = { glm::cos(startAngle), glm::sin(startAngle) },
                                .EndDir        = { glm::cos(endAngle), glm::sin(endAngle)},
                                .KeepPositive  = md.Radial.KeepPositive ? 1.0f : 0.0f,
                                .FlipMask      = md.Radial.FlipMask ? 1.0f : 0.0f
                            }
                        });
                } break;

                default:
                {
                    m_MaskInfoBufferArray.push_back(PIXEL_MASK_INFO
                        {
                            .Type = (uint32_t)md.Type
                        });
                } break;
                }
            }
        }
        else // If specific indices were given and the buffer size is not changed, only update the given indices
        {
            for (const auto& spriteIdx : m_MaskUpdateInfo.GetDirtyIndices())
            {
                // Update masks that are dirty only once
                if (!m_MaskUpdateInfo.ConsumeUpdate(spriteIdx))
                    continue;

                const auto& si = m_SpriteInstanceData[spriteIdx];

                for (size_t i = 0; i < si.MaskCount; i++)
                {
                    const size_t maskIdx = si.MaskStartIdx + i;

                    const auto& md = m_MaskInfoData[maskIdx];
                    switch (md.Type)
                    {
                    case MaskType_t::SlantedMask:
                    {
                        const auto lineNormalAngle = glm::radians(md.Slanted.LineNormalDeg);

                        m_MaskInfoBufferArray[maskIdx] =
                            PIXEL_MASK_INFO
                            {
                                .Type = (uint32_t)md.Type,
                                .Slanted = {
                                    .Origin = md.Slanted.Origin,
                                    .DirNormal = { glm::cos(lineNormalAngle), glm::sin(lineNormalAngle) },
                                    .SoftnessRange = md.Slanted.SoftnessRange
                                }
                            };
                    } break;

                    case MaskType_t::RadialMask:
                    {
                        const auto startAngle = glm::radians(md.Radial.StartDeg);
                        const auto endAngle = glm::radians(md.Radial.EndDeg);

                        m_MaskInfoBufferArray[maskIdx] =
                            PIXEL_MASK_INFO
                            {
                                .Type = (uint32_t)md.Type,
                                .Radial = {
                                    .Origin        = md.Radial.Origin,
                                    .RadiusSquared = md.Radial.Radius * md.Radial.Radius, // Radius squared because more optimized for shader
                                    .StartDir      = { glm::cos(startAngle), glm::sin(startAngle) },
                                    .EndDir        = { glm::cos(endAngle), glm::sin(endAngle)},
                                    .KeepPositive  = md.Radial.KeepPositive ? 1.0f : 0.0f,
                                    .FlipMask      = md.Radial.FlipMask ? 1.0f : 0.0f
                                }
                            };
                    } break;

                    default:
                    {
                        m_MaskInfoBufferArray[maskIdx] = 
                            PIXEL_MASK_INFO
                            {
                                .Type = (uint32_t)md.Type
                            };
                    } break;
                    }
                }
            }
        }

        m_MaskUpdateInfo.DequeueUpdate();

        size_t byteSize = m_MaskInfoBufferArray.size() * sizeof(decltype(m_MaskInfoBufferArray)::value_type);

        if (!m_pPixelMaskInfoStructuredBuffer || m_CurrentMaskInfoBufferSize != byteSize)
        {
            ID3D11Device* pDevice;
            pD3D11DeviceContext->GetDevice(&pDevice);
            return CreateAndUpdateMaskInfoBuffer(pDevice);
        }

        D3D11_MAPPED_SUBRESOURCE mappedResource;

        hr = pD3D11DeviceContext->Map(m_pPixelMaskInfoStructuredBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

        if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
        {
            return false;
        }

        ::memcpy_s(mappedResource.pData, m_CurrentMaskInfoBufferSize, m_MaskInfoBufferArray.data(), byteSize);

        pD3D11DeviceContext->Unmap(m_pPixelMaskInfoStructuredBuffer.Get(), 0);

        return true;
    }
    
    void BatchedSprites::Translate(size_t idx, glm::vec3 vector)
    {
        m_SpriteUpdateInfo.QueueUpdate(idx);
    
    	m_SpriteInstanceData[idx].Translation += vector;
    }
    
    void BatchedSprites::TranslateTo(size_t idx, glm::vec3 vector)
    {
        m_SpriteUpdateInfo.QueueUpdate(idx);
    
    	m_SpriteInstanceData[idx].Translation = vector;
    }
    
    void BatchedSprites::Rotate(size_t idx, glm::vec3 rotation)
    {
        m_SpriteUpdateInfo.QueueUpdate(idx);
    
        m_SpriteInstanceData[idx].Rotation += rotation;
    }
    
    void BatchedSprites::RotateTo(size_t idx, glm::vec3 rotation)
    {
        m_SpriteUpdateInfo.QueueUpdate(idx);

        m_SpriteInstanceData[idx].Rotation = rotation;
    }
    
    void BatchedSprites::Scale(size_t idx, glm::vec3 vector)
    {
        m_SpriteUpdateInfo.QueueUpdate(idx);
    
    	m_SpriteInstanceData[idx].Scale *= vector;
    }

    void BatchedSprites::AddMask(size_t spriteIdx, const MaskInfo& maskInfo)
    {
        m_SpriteUpdateInfo.QueueUpdate(spriteIdx);

        auto& si = m_SpriteInstanceData[spriteIdx];

        // If there is no mask associated with the sprite, just append to the mask buffer
        if (si.MaskCount == 0)
        {
            si.MaskStartIdx = m_MaskInfoData.size();
            si.MaskCount    = 1;
            m_MaskInfoData.push_back(maskInfo);

            m_MaskUpdateInfo.ResizeList(m_SpriteIndices.size());
            m_MaskUpdateInfo.QueueUpdate(spriteIdx);
        }
        else 
        {
            std::vector<MaskInfo> tmpRebuildMaskData;
            tmpRebuildMaskData.reserve(m_MaskInfoData.size() + 1);

            // Go through the sprites and add their masks to the new vector
            for (size_t i = 0; i < m_SpriteInstanceData.size(); i++)
            {
                const auto startIdx = m_SpriteInstanceData[i].MaskStartIdx;
                const auto count    = m_SpriteInstanceData[i].MaskCount;

                m_SpriteInstanceData[i].MaskStartIdx = tmpRebuildMaskData.size();

                tmpRebuildMaskData.insert(
                    tmpRebuildMaskData.end(),
                    m_MaskInfoData.begin() + startIdx,
                    m_MaskInfoData.begin() + startIdx + count
                );

                // When at the sprite that needs the new mask, add the mask to the temporary mask vector
                if (i == spriteIdx)
                {
                    tmpRebuildMaskData.push_back(maskInfo);
                    m_SpriteInstanceData[i].MaskCount++;
                }
            }

            // Use the new vector
            m_MaskInfoData = std::move(tmpRebuildMaskData);

            m_MaskUpdateInfo.ResizeList(m_SpriteIndices.size());
            m_MaskUpdateInfo.QueueAllForUpdate();
        }
    }

    const BatchedSprites::MaskInfo& BatchedSprites::GetSpriteMaskInfo(size_t spriteIdx, size_t maskIdx)
    {
        const auto& si = m_SpriteInstanceData[spriteIdx];

        return m_MaskInfoData[si.MaskStartIdx + maskIdx];
    }

    std::span<const BatchedSprites::MaskInfo> BatchedSprites::GetSpriteMasksInfo(size_t spriteIdx)
    {
        const auto& si = m_SpriteInstanceData[spriteIdx];

        if (si.MaskCount == 0)
        {
            return {};
        }

        return std::span<const MaskInfo>(&m_MaskInfoData[si.MaskStartIdx], si.MaskCount);
    }

    void BatchedSprites::SetSpriteMasks(size_t spriteIdx, const std::vector<MaskInfo>& maskInfos)
    {
        auto& si = m_SpriteInstanceData[spriteIdx];

        // If the number of masks aren't changed just update the old buffer in place
        if (si.MaskCount == maskInfos.size())
        {
            m_MaskUpdateInfo.QueueUpdate(spriteIdx);

            for (size_t i = 0; i < maskInfos.size(); i++)
            {
                const size_t maskIdxIntoTheArray = si.MaskStartIdx + i;

                m_MaskInfoData[maskIdxIntoTheArray] = maskInfos[i];
            }
        }
        else
        {
            // Mask count and maybe starting index changed, need to update the instance data
            m_SpriteUpdateInfo.QueueUpdate(spriteIdx);

            // If there is no mask associated with the sprite, just append to the mask buffer
            if (si.MaskCount == 0)
            {
                m_MaskInfoData.reserve(m_MaskInfoData.size() + maskInfos.size());

                si.MaskStartIdx = m_MaskInfoData.size();
                si.MaskCount    = maskInfos.size();
                m_MaskInfoData.insert(m_MaskInfoData.end(), maskInfos.begin(), maskInfos.end());
            }
            else
            {
                std::vector<MaskInfo> tmpRebuildMaskData;
                tmpRebuildMaskData.reserve(m_MaskInfoData.size() - si.MaskCount + maskInfos.size());

                // Go through the sprites and add their masks to the new vector
                for (size_t i = 0; i < m_SpriteInstanceData.size(); i++)
                {
                    const auto startIdx = m_SpriteInstanceData[i].MaskStartIdx;
                    const auto count    = m_SpriteInstanceData[i].MaskCount;

                    m_SpriteInstanceData[i].MaskStartIdx = tmpRebuildMaskData.size();

                    // For the sprite that is being updated just use the given masks instead of its old maks
                    if (i == spriteIdx)
                    {
                        m_SpriteInstanceData[i].MaskCount = maskInfos.size();

                        tmpRebuildMaskData.insert(tmpRebuildMaskData.end(), maskInfos.begin(), maskInfos.end());
                    }
                    else
                    {
                        tmpRebuildMaskData.insert(
                            tmpRebuildMaskData.end(), 
                            m_MaskInfoData.begin() + startIdx, 
                            m_MaskInfoData.begin() + startIdx + count
                        );
                    }
                }

                // Use the new vector
                m_MaskInfoData = std::move(tmpRebuildMaskData);
            }

            // Changed the size of mask buffer, need to re-create the buffer
            m_MaskUpdateInfo.QueueAllForUpdate();
        }
    }

    void BatchedSprites::UpdateSpriteMask(size_t spriteIdx, size_t maskIdx, const MaskInfo& maskInfo)
    {
        m_MaskUpdateInfo.QueueUpdate(spriteIdx);

        const size_t maskIdxIntoTheArray = m_SpriteInstanceData[spriteIdx].MaskStartIdx + maskIdx;

        m_MaskInfoData[maskIdxIntoTheArray] = maskInfo;
    }

    void BatchedSprites::RemoveSpriteMask(size_t spriteIdx, size_t maskIdx)
    {
        m_SpriteUpdateInfo.QueueUpdate(spriteIdx);

        std::vector<MaskInfo> tmpRebuildMaskData;
        tmpRebuildMaskData.reserve(m_MaskInfoData.size());

        // A flag to decrement the mask count if removed the mask with the given index
        bool removed = false;

        // Go through the sprites and add their masks to the new vector
        for (size_t i = 0; i < m_SpriteInstanceData.size(); i++)
        {
            const auto startIdx = m_SpriteInstanceData[i].MaskStartIdx;
            const auto count    = m_SpriteInstanceData[i].MaskCount;

            m_SpriteInstanceData[i].MaskStartIdx = tmpRebuildMaskData.size();

            for (size_t maskIdxIt = 0; maskIdxIt < count; maskIdxIt++)
            {
                const size_t maskIdxIntoTheArray = startIdx + maskIdxIt;

                // When found the mask to be removed, just don't add it to the new vector and mark the sprite for mask count decrement
                if (i == spriteIdx && maskIdxIt == maskIdx)
                {
                    removed = true;
                }
                else 
                {
                    tmpRebuildMaskData.push_back(m_MaskInfoData[maskIdxIntoTheArray]);
                }
            }
        }

        // If removed a mask from the list udate the count
        if (removed)
        {
            m_SpriteInstanceData[spriteIdx].MaskCount--;
        }

        // Use the new vector
        m_MaskInfoData = std::move(tmpRebuildMaskData);

        m_MaskUpdateInfo.ResizeList(m_SpriteIndices.size());
        m_MaskUpdateInfo.QueueAllForUpdate();
    }

    void BatchedSprites::ScaleTo(size_t idx, glm::vec3 vector)
    {
        m_SpriteUpdateInfo.QueueUpdate(idx);
    
    	m_SpriteInstanceData[idx].Scale = vector;
    }

    void BatchedSprites::Tint(size_t idx, glm::vec4 tintCol)
    {
        m_SpriteUpdateInfo.QueueUpdate(idx);

        m_SpriteInstanceData[idx].Tint *= tintCol;
    }

    void BatchedSprites::TintTo(size_t idx, glm::vec4 tintCol)
    {
        m_SpriteUpdateInfo.QueueUpdate(idx);

        m_SpriteInstanceData[idx].Tint = tintCol;
    }
    
    void BatchedSprites::SetTransform(size_t idx, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale)
    {
        m_SpriteUpdateInfo.QueueUpdate(idx);
    
    	m_SpriteInstanceData[idx].Translation = translation;
        m_SpriteInstanceData[idx].Rotation = rotation;
        m_SpriteInstanceData[idx].Scale = scale;
    }
    
    void BatchedSprites::SetOpacity(size_t idx, float opacity)
    {
        m_SpriteUpdateInfo.QueueUpdate(idx);
    
    	m_SpriteInstanceData[idx].Opacity = opacity;
    }
    
    void BatchedSprites::SetBrightness(size_t idx, float brightness)
    {
        m_SpriteUpdateInfo.QueueUpdate(idx);
    
    	m_SpriteInstanceData[idx].Brightness = brightness;
    }
    
    //bool BatchedSprites::CreateSpriteInfoBufferSrv(ID3D11Device* pD3D11Device)
    //{
    //	HRESULT hr;
    //
    //	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    //	ZeroMemory(&srvDesc, sizeof(srvDesc));
    //
    //	srvDesc.Format              = DXGI_FORMAT_UNKNOWN;
    //	srvDesc.ViewDimension       = D3D11_SRV_DIMENSION_BUFFER;
    //	srvDesc.Buffer.FirstElement = 0;
    //	srvDesc.Buffer.NumElements  = (UINT)m_SpriteInstanceData.size();
    //
    //	hr = pD3D11Device->CreateShaderResourceView(m_pSpriteInstanceInfoBuffer.Get(), &srvDesc, &m_pInstanceInfoBufferSRV);
    //
    //	if (!ErrorCheck(hr, __FILE__, __FUNCTION__, __LINE__))
    //		return false;
    //
    //	return true;
    //}

    bool BatchedSprites::CreateMaskInfoBufferSrv(ID3D11Device* pD3D11Device)
    {
        HRESULT hr;

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(srvDesc));

        srvDesc.Format              = DXGI_FORMAT_UNKNOWN;
        srvDesc.ViewDimension       = D3D11_SRV_DIMENSION_BUFFER;
        srvDesc.Buffer.FirstElement = 0;
        srvDesc.Buffer.NumElements  = (UINT)m_MaskInfoData.size();

        hr = pD3D11Device->CreateShaderResourceView(m_pPixelMaskInfoStructuredBuffer.Get(), &srvDesc, &m_pMaskInfoBufferSRV);

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

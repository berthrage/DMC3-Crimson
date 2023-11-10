
// ================================================================================================
// -*- C++ -*-
// File:   sample_d3d11.cpp
// Author: Guilherme R. Lampert
// Brief:  D3D11 Debug Draw sample. Also uses the newer explicit context API.
//
// This software is in the public domain. Where that dedication is not recognized,
// you are granted a perpetual, irrevocable license to copy, distribute, and modify
// this file as you see fit.
// ================================================================================================

#define DEBUG_DRAW_IMPLEMENTATION
#include "DebugDrawDX11.hpp"

#include <cstdlib>
#include <tuple>

#define NOIME
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <ShellScalingAPI.h>
#include <windows.h>

#include <dxgi.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <DirectXMath.h> 
#include "debug-draw/samples/vectormath/vectormath.h"

#pragma comment(lib, "Shcore")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "d3dcompiler")


static constexpr auto ddShaderCodeASCII = R"(
///////////////////////////////////////////////////////////////////////////////

Texture2D    glyphsTexture : register(t0);
SamplerState glyphsSampler : register(s0);

cbuffer ConstantBufferData : register(b0)
{
    matrix mvpMatrix;
    float4 screenDimensions;
};

///////////////////////////////////////////////////////////////////////////////

struct VertexInput
{
    float4 pos   : POSITION;
    float4 uv    : TEXCOORD;
    float4 color : COLOR;
};

struct VertexOutput
{
    float4 vpos  : SV_POSITION;
    float4 uv    : TEXCOORD;
    float4 color : COLOR;
};

///////////////////////////////////////////////////////////////////////////////
// Line/point drawing:
///////////////////////////////////////////////////////////////////////////////

VertexOutput VS_LinePoint(VertexInput input)
{
    VertexOutput output;
    output.vpos  = mul(input.pos, mvpMatrix);
    output.uv    = input.uv;
    output.color = input.color;
    return output;
}

float4 PS_LinePoint(VertexOutput input) : SV_TARGET
{
    return input.color;
}

///////////////////////////////////////////////////////////////////////////////
// Text glyphs drawing:
///////////////////////////////////////////////////////////////////////////////

VertexOutput VS_TextGlyph(VertexInput input)
{
    // Map to normalized clip coordinates:
    float x = ((2.0 * (input.pos.x - 0.5)) / screenDimensions.x) - 1.0;
    float y = 1.0 - ((2.0 * (input.pos.y - 0.5)) / screenDimensions.y);

    VertexOutput output;
    output.vpos  = float4(x, y, 0.0, 1.0);
    output.uv    = input.uv;
    output.color = input.color;
    return output;
}

float4 PS_TextGlyph(VertexOutput input) : SV_TARGET
{
    float4 texColor  = glyphsTexture.Sample(glyphsSampler, input.uv.xy);
    float4 fragColor = input.color;

    fragColor.a = texColor.r;
    return fragColor;
}
)";

namespace Devil3 {
    // Created with ReClass.NET 1.2 by KN4CK3R
#pragma pack(push, r1, 1)
    class cCameraControl
    {
    public:
        char pad_0008[8]; //0x0008
        Vector3 up; //0x0010
        float FOV; //0x0020
        float roll; //0x0024
        float unused0; //0x0028
        uint32_t unused1; //0x002C
        Matrix4 transform; //0x0030
        Vector4 eye; //0x0070
        Vector4 lookat; //0x0080
        char pad_0090[32]; //0x0090

        virtual void Function0();
        virtual void Function1();
        virtual void Function2();
        virtual void Function3();
        virtual void Function4();
        virtual void Function5();
        virtual void Function6();
        virtual void Function7();
        virtual void Function8();
        virtual void Function9();
    }; //Size: 0x00B0

    class sCameraCtrlPtr
    {
    public:
        class cCameraControl* pCameraControl; //0x0000
        char pad_0008[56]; //0x0008
    }; //Size: 0x0040

#pragma pack(pop, r1)
}

// ========================================================
// Helper functions
// ========================================================

static void panicF(const char* format, ...)
{
    va_list args;
    char buffer[2048] = { '\0' };

    va_start(args, format);
    std::vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    MessageBoxA(nullptr, buffer, "Fatal Error", MB_OK);
    std::abort();
}

// ========================================================
// DebugDraw Camera
// ========================================================
namespace dd {

    // Angle in degrees to angle in radians for sin/cos/etc.
    static inline float degToRad(const float degrees)
    {
        return (degrees * 3.1415926535897931f / 180.0f);
    }

    struct Camera
    {
        //
        // Camera Axes:
        //
        //    (up)
        //    +Y   +Z (forward)
        //    |   /
        //    |  /
        //    | /
        //    + ------ +X (right)
        //  (eye)
        //
        Vector3 right;
        Vector3 up;
        Vector3 forward;
        Vector3 eye;
        Matrix4 viewMatrix;
        Matrix4 projMatrix;
        Matrix4 vpMatrix;
        float windowDims[2];

        // Frustum planes for clipping:
        enum { A, B, C, D };
        Vector4 planes[6];

        // Tunable values:
        float movementSpeed = 3.0f;
        float lookSpeed = 6.0f;

        enum MoveDir
        {
            Forward, // Move forward relative to the camera's space
            Back,    // Move backward relative to the camera's space
            Left,    // Move left relative to the camera's space
            Right    // Move right relative to the camera's space
        };
        Camera() = delete;
        Camera(float WindowWidth, float WindowHeight)
        {
            right = Vector3(1.0f, 0.0f, 0.0f);
            up = Vector3(0.0f, 1.0f, 0.0f);
            forward = Vector3(0.0f, 0.0f, 1.0f);
            eye = Vector3(0.0f, 0.0f, 0.0f);
            viewMatrix = Matrix4::identity();
            vpMatrix = Matrix4::identity();

            //const float fovY = degToRad(60.0f);
            //const float aspect = static_cast<float>(WindowWidth) / static_cast<float>(WindowHeight);
            windowDims[0] = WindowWidth;
            windowDims[1] = WindowHeight;
            //projMatrix = Matrix4::perspective(fovY, aspect, 0.1f, 1000.0f);

            for (int i = 0; i < 6; ++i)
            {
                planes[i] = Vector4(0.0f);
            }
        }

        void updateMatrices(Devil3::cCameraControl* devil3cam)
        {
            const float fovY = devil3cam->FOV;
            const float aspect = windowDims[0] / windowDims[1];
            projMatrix = Matrix4::perspective(fovY, aspect, 0.1f, 100.0f);

            viewMatrix = Matrix4::lookAt(toPoint3(devil3cam->eye), toPoint3(devil3cam->lookat), devil3cam->up);
            vpMatrix = projMatrix * viewMatrix; // Vectormath lib uses column-major OGL style, so multiply P*V*M

            // Compute and normalize the 6 frustum planes:
            const float* const m = toFloatPtr(vpMatrix);
            planes[0][A] = m[3] - m[0];
            planes[0][B] = m[7] - m[4];
            planes[0][C] = m[11] - m[8];
            planes[0][D] = m[15] - m[12];
            planes[0] = normalize(planes[0]);
            planes[1][A] = m[3] + m[0];
            planes[1][B] = m[7] + m[4];
            planes[1][C] = m[11] + m[8];
            planes[1][D] = m[15] + m[12];
            planes[1] = normalize(planes[1]);
            planes[2][A] = m[3] + m[1];
            planes[2][B] = m[7] + m[5];
            planes[2][C] = m[11] + m[9];
            planes[2][D] = m[15] + m[13];
            planes[2] = normalize(planes[2]);
            planes[3][A] = m[3] - m[1];
            planes[3][B] = m[7] - m[5];
            planes[3][C] = m[11] - m[9];
            planes[3][D] = m[15] - m[13];
            planes[3] = normalize(planes[3]);
            planes[4][A] = m[3] - m[2];
            planes[4][B] = m[7] - m[6];
            planes[4][C] = m[11] - m[10];
            planes[4][D] = m[15] - m[14];
            planes[4] = normalize(planes[4]);
            planes[5][A] = m[3] + m[2];
            planes[5][B] = m[7] + m[6];
            planes[5][C] = m[11] + m[10];
            planes[5][D] = m[15] + m[14];
            planes[5] = normalize(planes[5]);
        }

        Point3 getTarget() const
        {
            return Point3(eye[0] + forward[0], eye[1] + forward[1], eye[2] + forward[2]);
        }

        bool isPointInsideFrustum(const float x, const float y, const float z) const
        {
            for (int i = 0; i < 6; ++i)
            {
                if ((planes[i][A] * x + planes[i][B] * y + planes[i][C] * z + planes[i][D]) <= 0.0f)
                {
                    return false;
                }
            }
            return true;
        }
    };

}

// ========================================================
// ShaderSetD3D11
// ========================================================

struct ShaderSetD3D11 final
{
    using InputLayoutDesc = std::tuple<const D3D11_INPUT_ELEMENT_DESC*, int>;

    ID3D11VertexShader* vs;
    ID3D11PixelShader*  ps;
    ID3D11InputLayout*  vertexLayout;

    ShaderSetD3D11() = default;

    void loadFromFxMemory(ID3D11Device* device, const char* vsEntry,
        const char* psEntry, const InputLayoutDesc& layout)
    {
        assert(vsEntry != nullptr && vsEntry[0] != '\0');
        assert(psEntry != nullptr && psEntry[0] != '\0');

        ID3DBlob* vsBlob;
        compileShaderFromMemory(vsEntry, "vs_4_0", &vsBlob);
        assert(vsBlob != nullptr);

        ID3DBlob* psBlob;
        compileShaderFromMemory(psEntry, "ps_4_0", &psBlob);
        assert(psBlob != nullptr);

        HRESULT hr;

        // Create the vertex shader:
        hr = device->CreateVertexShader(vsBlob->GetBufferPointer(),
            vsBlob->GetBufferSize(), nullptr,
            &vs);
        if (FAILED(hr))
        {
            panicF("Failed to create vertex shader '%s'", vsEntry);
        }

        // Create the pixel shader:
        hr = device->CreatePixelShader(psBlob->GetBufferPointer(),
            psBlob->GetBufferSize(), nullptr,
            &ps);
        if (FAILED(hr))
        {
            panicF("Failed to create pixel shader '%s'", psEntry);
        }

        // Create vertex input layout:
        hr = device->CreateInputLayout(std::get<0>(layout), std::get<1>(layout),
            vsBlob->GetBufferPointer(),
            vsBlob->GetBufferSize(),
            &vertexLayout);
        if (FAILED(hr))
        {
            panicF("Failed to create vertex layout!");
        }
    }

    static void compileShaderFromMemory(const char* entryPoint,
        const char* shaderModel, ID3DBlob** ppBlobOut)
    {
        UINT shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

        // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
        // Setting this flag improves the shader debugging experience, but still allows
        // the shaders to be optimized and to run exactly the way they will run in
        // the release configuration.
#if defined(DEBUG) || defined(_DEBUG)
        shaderFlags |= D3DCOMPILE_DEBUG;
#endif // DEBUG

        ID3DBlob* pErrorBlob;
        HRESULT hr = D3DCompile(ddShaderCodeASCII, strlen(ddShaderCodeASCII), nullptr, nullptr, nullptr, entryPoint, shaderModel, shaderFlags, 0, ppBlobOut, &pErrorBlob);
        /*HRESULT hr = D3DCompileFromFile(fileName, nullptr, nullptr, entryPoint, shaderModel,
            shaderFlags, 0, ppBlobOut, &pErrorBlob);*/
        if (FAILED(hr))
        {
            auto* details = (pErrorBlob ? static_cast<const char*>(pErrorBlob->GetBufferPointer()) : "<no info>");
            panicF("Failed to compile shader! Error info: %s", details);
        }
    }
};

// ========================================================
// RenderInterfaceD3D11
// ========================================================

class RenderInterfaceD3D11 final
    : public dd::RenderInterface
{
public:

    RenderInterfaceD3D11(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, float w, float h)
        : d3dDevice(pDevice)
        , deviceContext(pContext)
    {
        constantBufferData.screenDimensions = DirectX::XMFLOAT4{ w, h, 0.0f, 0.0f };
        initShaders();
        initBuffers();
    }

    void setMvpMatrixPtr(const float* const mtx)
    {
        constantBufferData.mvpMatrix = DirectX::XMMATRIX(mtx);
    }

    void setCameraFrame(const Vector3& up, const Vector3& right, const Vector3& origin)
    {
        camUp = up; camRight = right; camOrigin = origin;
    }

    //
    // dd::RenderInterface overrides:
    //

    void beginDraw() override
    {
        // Update and set the constant buffer for this frame
        deviceContext->UpdateSubresource(constantBuffer, 0, nullptr, &constantBufferData, 0, 0);
        deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);

        // Disable culling for the screen text
        deviceContext->RSSetState(rasterizerState);
    }

    void endDraw() override
    {
        // No work done here at the moment.
    }

    dd::GlyphTextureHandle createGlyphTexture(int width, int height, const void* pixels) override
    {
        UINT numQualityLevels = 0;
        d3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8_UNORM, 1, &numQualityLevels);

        D3D11_TEXTURE2D_DESC tex2dDesc = {};
        tex2dDesc.Usage = D3D11_USAGE_DEFAULT;
        tex2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        tex2dDesc.Format = DXGI_FORMAT_R8_UNORM;
        tex2dDesc.Width = width;
        tex2dDesc.Height = height;
        tex2dDesc.MipLevels = 1;
        tex2dDesc.ArraySize = 1;
        tex2dDesc.SampleDesc.Count = 1;
        tex2dDesc.SampleDesc.Quality = numQualityLevels - 1;

        D3D11_SAMPLER_DESC samplerDesc = {};
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        samplerDesc.MaxAnisotropy = 1;
        samplerDesc.MipLODBias = 0.0f;
        samplerDesc.MinLOD = 0.0f;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = pixels;
        initData.SysMemPitch = width;

        auto* texImpl = new TextureImpl{};

        if (FAILED(d3dDevice->CreateTexture2D(&tex2dDesc, &initData, &texImpl->d3dTexPtr)))
        {
            fprintf(stderr, "CreateTexture2D failed!\n");
            destroyGlyphTexture(texImpl);
            return nullptr;
        }
        if (FAILED(d3dDevice->CreateShaderResourceView(texImpl->d3dTexPtr, nullptr, &texImpl->d3dTexSRV)))
        {
            fprintf(stderr, "CreateShaderResourceView failed!\n");
            destroyGlyphTexture(texImpl);
            return nullptr;
        }
        if (FAILED(d3dDevice->CreateSamplerState(&samplerDesc, &texImpl->d3dSampler)))
        {
            fprintf(stderr, "CreateSamplerState failed!\n");
            destroyGlyphTexture(texImpl);
            return nullptr;
        }

        return static_cast<dd::GlyphTextureHandle>(texImpl);
    }

    void destroyGlyphTexture(dd::GlyphTextureHandle glyphTex) override
    {
        auto* texImpl = static_cast<TextureImpl*>(glyphTex);
        if (texImpl)
        {
            if (texImpl->d3dSampler) { texImpl->d3dSampler->Release(); }
            if (texImpl->d3dTexSRV) { texImpl->d3dTexSRV->Release(); }
            if (texImpl->d3dTexPtr) { texImpl->d3dTexPtr->Release(); }
            delete texImpl;
        }
    }

    void drawGlyphList(const dd::DrawVertex* glyphs, int count, dd::GlyphTextureHandle glyphTex) override
    {
        assert(glyphs != nullptr);
        assert(count > 0 && count <= DEBUG_DRAW_VERTEX_BUFFER_SIZE);

        auto* texImpl = static_cast<TextureImpl*>(glyphTex);
        assert(texImpl != nullptr);

        // Map the vertex buffer:
        D3D11_MAPPED_SUBRESOURCE mapInfo;
        if (FAILED(deviceContext->Map(glyphVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapInfo)))
        {
            panicF("Failed to map vertex buffer!");
        }

        // Copy into mapped buffer:
        auto* verts = static_cast<Vertex*>(mapInfo.pData);
        for (int v = 0; v < count; ++v)
        {
            verts[v].pos.x = glyphs[v].glyph.x;
            verts[v].pos.y = glyphs[v].glyph.y;
            verts[v].pos.z = 0.0f;
            verts[v].pos.w = 1.0f;

            verts[v].uv.x = glyphs[v].glyph.u;
            verts[v].uv.y = glyphs[v].glyph.v;
            verts[v].uv.z = 0.0f;
            verts[v].uv.w = 0.0f;

            verts[v].color.x = glyphs[v].glyph.r;
            verts[v].color.y = glyphs[v].glyph.g;
            verts[v].color.z = glyphs[v].glyph.b;
            verts[v].color.w = 1.0f;
        }

        // Unmap and draw:
        deviceContext->Unmap(glyphVertexBuffer, 0);

        // Bind texture & sampler (t0, s0):
        deviceContext->PSSetShaderResources(0, 1, &texImpl->d3dTexSRV);
        deviceContext->PSSetSamplers(0, 1, &texImpl->d3dSampler);

        const float blendFactor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        deviceContext->OMSetBlendState(blendStateText, blendFactor, 0xFFFFFFFF);

        // Draw with the current buffer:
        drawHelper(count, glyphShaders, glyphVertexBuffer, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // Restore default blend state.
        deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
    }

    void drawPointList(const dd::DrawVertex* points, int count, bool depthEnabled) override
    {
        (void)depthEnabled; // TODO: not implemented yet - not required by this sample

        // Emulating points as billboarded quads, so each point will use 6 vertexes.
        // D3D11 doesn't support "point sprites" like OpenGL (gl_PointSize).
        const int maxVerts = DEBUG_DRAW_VERTEX_BUFFER_SIZE / 6;

        // OpenGL point size scaling produces gigantic points with the billboarding fallback.
        // This is some arbitrary down-scaling factor to more or less match the OpenGL samples.
        const float D3DPointSpriteScalingFactor = 0.01f;

        assert(points != nullptr);
        assert(count > 0 && count <= maxVerts);

        // Map the vertex buffer:
        D3D11_MAPPED_SUBRESOURCE mapInfo;
        if (FAILED(deviceContext->Map(pointVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapInfo)))
        {
            panicF("Failed to map vertex buffer!");
        }

        const int numVerts = count * 6;
        const int indexes[6] = { 0, 1, 2, 2, 3, 0 };

        int v = 0;
        auto* verts = static_cast<Vertex*>(mapInfo.pData);

        // Expand each point into a quad:
        for (int p = 0; p < count; ++p)
        {
            const float ptSize = points[p].point.size * D3DPointSpriteScalingFactor;
            const Vector3 halfWidth = (ptSize * 0.5f) * camRight; // X
            const Vector3 halfHeigh = (ptSize * 0.5f) * camUp;    // Y
            const Vector3 origin = Vector3{ points[p].point.x, points[p].point.y, points[p].point.z };

            Vector3 corners[4];
            // Oh man did i ever tell you how much i love DirectXMath
            corners[0] = origin + halfWidth + halfHeigh;
            corners[1] = origin - halfWidth + halfHeigh;
            corners[2] = origin - halfWidth - halfHeigh;
            corners[3] = origin + halfWidth - halfHeigh;

            for (int i : indexes)
            {
                verts[v].pos.x = corners[i].getX();
                verts[v].pos.y = corners[i].getY();
                verts[v].pos.z = corners[i].getZ();
                verts[v].pos.w = 1.0f;

                verts[v].color.x = points[p].point.r;
                verts[v].color.y = points[p].point.g;
                verts[v].color.z = points[p].point.b;
                verts[v].color.w = 1.0f;

                ++v;
            }
        }
        assert(v == numVerts);

        // Unmap and draw:
        deviceContext->Unmap(pointVertexBuffer, 0);

        // Draw with the current buffer:
        drawHelper(numVerts, pointShaders, pointVertexBuffer, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }

    void drawLineList(const dd::DrawVertex* lines, int count, bool depthEnabled) override
    {
        (void)depthEnabled; // TODO: not implemented yet - not required by this sample

        assert(lines != nullptr);
        assert(count > 0 && count <= DEBUG_DRAW_VERTEX_BUFFER_SIZE);

        // Map the vertex buffer:
        D3D11_MAPPED_SUBRESOURCE mapInfo;
        if (FAILED(deviceContext->Map(lineVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapInfo)))
        {
            panicF("Failed to map vertex buffer!");
        }

        // Copy into mapped buffer:
        auto* verts = static_cast<Vertex*>(mapInfo.pData);
        for (int v = 0; v < count; ++v)
        {
            verts[v].pos.x = lines[v].line.x;
            verts[v].pos.y = lines[v].line.y;
            verts[v].pos.z = lines[v].line.z;
            verts[v].pos.w = 1.0f;

            verts[v].color.x = lines[v].line.r;
            verts[v].color.y = lines[v].line.g;
            verts[v].color.z = lines[v].line.b;
            verts[v].color.w = 1.0f;
        }

        // Unmap and draw:
        deviceContext->Unmap(lineVertexBuffer, 0);

        // Draw with the current buffer:
        drawHelper(count, lineShaders, lineVertexBuffer, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    }

private:

    //
    // Local types:
    //

    struct ConstantBufferData
    {
        DirectX::XMMATRIX mvpMatrix = DirectX::XMMatrixIdentity();
        DirectX::XMFLOAT4 screenDimensions = { };
    };

    struct Vertex
    {
        DirectX::XMFLOAT4A pos;   // 3D position
        DirectX::XMFLOAT4A uv;    // Texture coordinates
        DirectX::XMFLOAT4A color; // RGBA float
    };

    struct TextureImpl : public dd::OpaqueTextureType
    {
        ID3D11Texture2D* d3dTexPtr = nullptr;
        ID3D11ShaderResourceView* d3dTexSRV = nullptr;
        ID3D11SamplerState* d3dSampler = nullptr;
    };

    //
    // Members:
    //

    ID3D11Device*          d3dDevice;
    ID3D11DeviceContext*   deviceContext;
    ID3D11RasterizerState* rasterizerState;
    ID3D11BlendState*      blendStateText;

    ID3D11Buffer*          constantBuffer;
    ConstantBufferData            constantBufferData;

    ID3D11Buffer*          lineVertexBuffer;
    ShaderSetD3D11                lineShaders;

    ID3D11Buffer*          pointVertexBuffer;
    ShaderSetD3D11                pointShaders;

    ID3D11Buffer*          glyphVertexBuffer;
    ShaderSetD3D11                glyphShaders;

    // Camera vectors for the emulated point sprites
    Vector3                    camUp = Vector3{ 0.0f };
    Vector3                    camRight = Vector3{ 0.0f };
    Vector3                    camOrigin = Vector3{ 0.0f };

    void initShaders()
    {
        // Same vertex format used by all buffers to simplify things.
        const D3D11_INPUT_ELEMENT_DESC layout[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        const ShaderSetD3D11::InputLayoutDesc inputDesc{ layout, int(ARRAYSIZE(layout)) };

        // 3D lines shader:
        lineShaders.loadFromFxMemory(d3dDevice, "VS_LinePoint",
            "PS_LinePoint", inputDesc);

        // 3D points shader:
        pointShaders.loadFromFxMemory(d3dDevice, "VS_LinePoint",
            "PS_LinePoint", inputDesc);

        // 2D glyphs shader:
        glyphShaders.loadFromFxMemory(d3dDevice, "VS_TextGlyph",
            "PS_TextGlyph", inputDesc);

        // Rasterizer state for the screen text:
        D3D11_RASTERIZER_DESC rsDesc = {};
        rsDesc.FillMode = D3D11_FILL_SOLID;
        rsDesc.CullMode = D3D11_CULL_NONE;
        rsDesc.FrontCounterClockwise = true;
        rsDesc.DepthBias = 0;
        rsDesc.DepthBiasClamp = 0.0f;
        rsDesc.SlopeScaledDepthBias = 0.0f;
        rsDesc.DepthClipEnable = false;
        rsDesc.ScissorEnable = false;
        rsDesc.MultisampleEnable = false;
        rsDesc.AntialiasedLineEnable = false;
        if (FAILED(d3dDevice->CreateRasterizerState(&rsDesc, &rasterizerState)))
        {
            fprintf(stderr, "CreateRasterizerState failed!");
        }

        // Blend state for the screen text:
        D3D11_BLEND_DESC bsDesc = {};
        bsDesc.RenderTarget[0].BlendEnable = true;
        bsDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        bsDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        bsDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        bsDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        bsDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        bsDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        bsDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        if (FAILED(d3dDevice->CreateBlendState(&bsDesc, &blendStateText)))
        {
            fprintf(stderr, "CreateBlendState failed!");
        }
    }

    void initBuffers()
    {
        D3D11_BUFFER_DESC bd;

        // Create the shared constant buffer:
        bd = {};
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(ConstantBufferData);
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
        if (FAILED(d3dDevice->CreateBuffer(&bd, nullptr, &constantBuffer)))
        {
            panicF("Failed to create shader constant buffer!");
        }

        // Create the vertex buffers for lines/points/glyphs:
        bd = {};
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth = sizeof(Vertex) * DEBUG_DRAW_VERTEX_BUFFER_SIZE;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        if (FAILED(d3dDevice->CreateBuffer(&bd, nullptr, &lineVertexBuffer)))
        {
            panicF("Failed to create lines vertex buffer!");
        }
        if (FAILED(d3dDevice->CreateBuffer(&bd, nullptr, &pointVertexBuffer)))
        {
            panicF("Failed to create points vertex buffer!");
        }
        if (FAILED(d3dDevice->CreateBuffer(&bd, nullptr, &glyphVertexBuffer)))
        {
            panicF("Failed to create glyphs vertex buffer!");
        }
    }

    void drawHelper(const int numVerts, const ShaderSetD3D11& ss,
        ID3D11Buffer* vb, const D3D11_PRIMITIVE_TOPOLOGY topology)
    {
        const UINT offset = 0;
        const UINT stride = sizeof(Vertex);
        deviceContext->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
        deviceContext->IASetPrimitiveTopology(topology);
        deviceContext->IASetInputLayout(ss.vertexLayout);
        deviceContext->VSSetShader(ss.vs, nullptr, 0);
        deviceContext->PSSetShader(ss.ps, nullptr, 0);
        deviceContext->Draw(numVerts, 0);
    }
};

// ========================================================
// Sample drawing
// ========================================================

static void drawLabel(dd::ContextHandle ctx, dd::Camera& camera, ddVec3_In pos, const char* name)
{
    // Only draw labels inside the camera frustum.
    if (camera.isPointInsideFrustum(pos[0], pos[1], pos[2]))
    {
    const ddVec3 textColor = { 0.8f, 0.8f, 1.0f };
    dd::projectedText(ctx, name, pos, textColor, toFloatPtr(camera.vpMatrix),
        0, 0, camera.windowDims[0], camera.windowDims[1], 10.0f);
    }
}

static void drawMiscObjects(dd::ContextHandle ctx, dd::Camera& camera)
{
    // Start a row of objects at this position:
    ddVec3 origin = { 1500.0, 100.0, 2000.0f };

    // Box with a point at it's center:
    drawLabel(ctx, camera, origin, "box");
    dd::box(ctx, origin, dd::colors::Blue, 1.5f, 1.5f, 1.5f);
    dd::point(ctx, origin, dd::colors::White, 15.0f);
    origin[0] += 30.0f;

    // Sphere with a point at its center
    drawLabel(ctx, camera, origin, "sphere");
    dd::sphere(ctx, origin, dd::colors::Red, 1.0f);
    dd::point(ctx, origin, dd::colors::White, 15.0f);
    origin[0] += 40.0f;

    // Two cones, one open and one closed:
    origin[1] -= 10.0f;
    const ddVec3 coneo = { 800.0f, 0.0f, 1500.0f };
    const ddVec3 condeDir = { 0.0f, 1000.0f, 0.0f };
    drawLabel(ctx, camera, origin, "cone (open)");
    dd::cone(ctx, coneo, condeDir, dd::colors::Yellow, 200.0f, 200.0f);
    dd::point(ctx, origin, dd::colors::White, 15.0f);
    origin[0] += 40.0f;

    drawLabel(ctx, camera, origin, "cone (closed)");
    dd::cone(ctx, origin, condeDir, dd::colors::Cyan, 0.0f, 1.0f);
    dd::point(ctx, origin, dd::colors::White, 15.0f);
    origin[0] += 40.0f;

    // Axis-aligned bounding box:
    const ddVec3 bbMins = { 800.0f, 0.0f, 1500.0f };
    const ddVec3 bbMaxs = { 1000.0f, 100.0f, 2000.0f };
    const ddVec3 bbCenter = {
        (bbMins[0] + bbMaxs[0]) * 0.5f,
        (bbMins[1] + bbMaxs[1]) * 0.5f,
        (bbMins[2] + bbMaxs[2]) * 0.5f
    };
    drawLabel(ctx, camera, origin, "AABB");
    dd::aabb(ctx, bbMins, bbMaxs, dd::colors::Orange);
    dd::point(ctx, bbCenter, dd::colors::White, 15.0f);

    // Move along the Z for another row:
    origin[0] = -15.0f;
    origin[2] += 5.0f;

    // A big arrow pointing up:
    const ddVec3 arrowFrom = { origin[0], origin[1], origin[2] };
    const ddVec3 arrowTo = { origin[0], origin[1] + 5.0f, origin[2] };
    drawLabel(ctx, camera, arrowFrom, "arrow");
    dd::arrow(ctx, arrowFrom, arrowTo, dd::colors::Magenta, 1.0f);
    dd::point(ctx, arrowFrom, dd::colors::White, 15.0f);
    dd::point(ctx, arrowTo, dd::colors::White, 15.0f);
    origin[0] += 4.0f;

    // Plane with normal vector:
    const ddVec3 planeNormal = { 0.0f, 1.0f, 0.0f };
    drawLabel(ctx, camera, origin, "plane");
    dd::plane(ctx, origin, planeNormal, dd::colors::Yellow, dd::colors::Blue, 1.5f, 1.0f);
    dd::point(ctx, origin, dd::colors::White, 15.0f);
    origin[0] += 4.0f;

    // Circle on the Y plane:
    drawLabel(ctx, camera, origin, "circle");
    dd::circle(ctx, origin, planeNormal, dd::colors::Orange, 1.5f, 15.0f);
    dd::point(ctx, origin, dd::colors::White, 15.0f);
    origin[0] += 3.2f;

    // Tangent basis vectors:
    const ddVec3 normal = { 0.0f, 1.0f, 0.0f };
    const ddVec3 tangent = { 1.0f, 0.0f, 0.0f };
    const ddVec3 bitangent = { 0.0f, 0.0f, 1.0f };
    origin[1] += 0.1f;
    drawLabel(ctx, camera, origin, "tangent basis");
    dd::tangentBasis(ctx, origin, normal, tangent, bitangent, 2.5f);
    dd::point(ctx, origin, dd::colors::White, 15.0f);

    // And a set of intersecting axes:
    origin[0] += 4.0f;
    origin[1] += 1.0f;
    drawLabel(ctx, camera, origin, "cross");
    dd::cross(ctx, origin, 2.0f);
    dd::point(ctx, origin, dd::colors::White, 15.0f);
}

static void drawFrustum(dd::ContextHandle ctx, dd::Camera& camera)
{
    const ddVec3 color = { 0.8f, 0.3f, 1.0f };
    const ddVec3 origin = { -8.0f, 0.5f, 14.0f };
    drawLabel(ctx, camera, origin, "frustum + axes");

    // The frustum will depict a fake camera:
    const Matrix4 proj = Matrix4::perspective(dd::degToRad(45.0f), 800.0f / 600.0f, 0.5f, 4.0f);
    const Matrix4 view = Matrix4::lookAt(Point3(-8.0f, 0.5f, 14.0f), Point3(-8.0f, 0.5f, -14.0f), Vector3::yAxis());
    const Matrix4 clip = inverse(proj * view);
    dd::frustum(ctx, toFloatPtr(clip), color);

    // A white dot at the eye position:
    dd::point(ctx, origin, dd::colors::White, 15.0f);

    // A set of arrows at the camera's origin/eye:
    const Matrix4 transform = Matrix4::translation(Vector3(-8.0f, 0.5f, 14.0f)) * Matrix4::rotationZ(dd::degToRad(60.0f));
    dd::axisTriad(ctx, toFloatPtr(transform), 0.3f, 2.0f);
}

static void drawText(dd::ContextHandle ctx)
{
    // HUD text:
    const ddVec3 textColor = { 1.0f,  1.0f,  1.0f };
    const ddVec3 textPos2D = { 10.0f, 15.0f, 0.0f };
    dd::screenText(ctx, "Welcome to the D3D11 Debug Draw demo.\n\n",
        textPos2D, textColor, 0.55f);
}

// ========================================================
// DebugDraw Actual
// ========================================================
namespace dd {
    float g_time = 0.0f;
    struct DebugDrawContext {
        RenderInterfaceD3D11* ddRender;
        dd::Camera* camera;
        dd::ContextHandle ddContext{ nullptr };
        DebugDrawContext() = delete;

        DebugDrawContext(ID3D11Device* Device, ID3D11DeviceContext* DeviceContext, float width, float height) {
            ddRender = new RenderInterfaceD3D11(Device, DeviceContext, width, height);
            dd::initialize(&ddContext, ddRender);
            camera = new dd::Camera(width, height);
        };

        ~DebugDrawContext() {
            dd::shutdown(ddContext);
            delete camera;
            delete ddRender;
        }

        void update(float dt) {
            static uintptr_t appBase = (uintptr_t)GetModuleHandle(NULL);
            static Devil3::sCameraCtrlPtr* staticCameraCtrlPtr = (Devil3::sCameraCtrlPtr*)(appBase + 0xC8FBD0);
            if (!staticCameraCtrlPtr->pCameraControl) { return; }
            camera->updateMatrices(staticCameraCtrlPtr->pCameraControl);

            const Matrix4 mvpMatrix = transpose(camera->vpMatrix);
            ddRender->setMvpMatrixPtr(toFloatPtr(mvpMatrix));
            ddRender->setCameraFrame(camera->up, camera->right, camera->eye);

            // Call some DD functions to add stuff to the debug draw queues:
#if 0
            //dd::sphere(ddContext, toFloatPtr(staticCameraCtrlPtr->pCameraControl->lookat), dd::colors::AliceBlue, 10.0);
            char buffer[MAX_PATH]{};
            sprintf(buffer, "lookat: %f, %f, %f",
                staticCameraCtrlPtr->pCameraControl->lookat.getX(),
                staticCameraCtrlPtr->pCameraControl->lookat.getY(),
                staticCameraCtrlPtr->pCameraControl->lookat.getZ()
            );
            dd::projectedText(ddContext, buffer, toFloatPtr(staticCameraCtrlPtr->pCameraControl->lookat), dd::colors::Gold, toFloatPtr(camera->vpMatrix),
                0, 0, camera->windowDims.x, camera->windowDims.y, 2.0f);
#endif
            drawMiscObjects(ddContext, *camera);
            drawFrustum(ddContext, *camera);
            drawText(ddContext);
            const ddVec3 cols[16] = {
                {1.0f, 0.0f, 0.0f},
                {0.0f, 1.0f, 0.0f},
                {0.0f, 0.0f, 1.0f},
                {1.0f, 0.0f, 1.0f},
                {1.0f, 1.0f, 0.0f},
                {0.0f, 1.0f, 1.0f},
                {1.0f, 1.0f, 1.0f},
                {1.0f, 0.0f, 1.0f},
                {1.0f, 0.0f, 0.0f},
                {0.0f, 1.0f, 0.0f},
                {0.0f, 0.0f, 1.0f},
                {1.0f, 0.0f, 1.0f},
                {1.0f, 0.0f, 0.0f},
                {0.0f, 1.0f, 0.0f},
                {0.0f, 0.0f, 1.0f},
                {1.0f, 0.0f, 1.0f},
            };
            for (int i = 0; i < 16; i++) {
                const ddVec3 hehe = { 1800.0f + (100.0f * i), 300.0f + ((sinf(PI / 180.0f + i * 45.0f + g_time)) * 150.0f), 2000.0f};
                dd::sphere(ddContext, hehe, cols[i], 60.0);
            }

            // Flush the draw queues:
            dd::flush(ddContext);
            g_time += dt;
        }

    };
}

dd::DebugDrawContext* g_dd_ctx{ nullptr };

int debug_draw_init(void* device, void* deviceCtx, float width, float height) {
    g_dd_ctx = new dd::DebugDrawContext((ID3D11Device*)device, (ID3D11DeviceContext*)deviceCtx, width, height);
    return 0;
}

void debug_draw_update(float dt) {
    g_dd_ctx->update(dt);
}

void debug_draw_shutdown() {
    delete g_dd_ctx;
}

dd::ContextHandle dd_ctx() {
    return g_dd_ctx->ddContext;
}

void debug_draw_projected_text(const char* text, const float pos[], const float color[], float size) {
    dd::projectedText(g_dd_ctx->ddContext, text, pos, color, toFloatPtr(g_dd_ctx->camera->vpMatrix),
        0, 0, g_dd_ctx->camera->windowDims[0], g_dd_ctx->camera->windowDims[1], size);
}
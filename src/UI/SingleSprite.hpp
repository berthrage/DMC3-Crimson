#pragma once

#include <array>
#include <string>
#include <memory>
#include <vector>

#include "GraphicsBase.hpp"
#include "../ThirdParty/ImGui/imgui.h"

// Forward declaration for texture class
class Texture2DD3D11;

namespace Graphics {

    class SingleSprite : public ErrorHandled
    {
    private:
        struct SpriteInfo
        {
            float       Opacity;
            float       Brightness;
            glm::vec3   Translation;
            glm::vec3   Rotation;
            glm::vec3   Scale;
        };

    #pragma pack(push, 1)
        struct VERTEX_SHADER_SPRITE_INFO
        {
            float       Opacity;
            float       Brightness;
            glm::vec3   Translation;
            glm::vec3   Rotation;
            glm::vec3   Scale;
        };

        __declspec(align(16)) struct VERTEX_CONSTANT_BUFFER
        {
            alignas(16) glm::mat4 MVP;
        };

        __declspec(align(16)) struct PIXEL_CONSTANT_BUFFER
        {
            uint32_t SamplerCount;
            float TextureWidth;
            float TextureHeight;
            uint32_t SampleCount; // MSAA sample count
            float FXAAQualitySubpix;   // FXAA quality settings
            float FXAAQualityEdgeThreshold;
            float FXAAQualityEdgeThresholdMin;
            uint32_t EnableColorize; // 1 = enabled, 0 = disabled
            alignas(16) glm::vec3 ColorizeColor; // RGB color for tinting
            float ColorizeWhiteThreshold; // Threshold for white detection (0.88 = #e2e1e2)
        };
    #pragma pack(pop)

    public:
        /// <summary>
        /// Creates a single sprite with the specified texture path and render target size
        /// </summary>
        /// <param name="pD3D11Device">Pointer to the ID3D11Device</param>
        /// <param name="width">Width of the render target</param>
        /// <param name="height">Height of the render target</param>
        /// <param name="texturePath">Path to the texture file</param>
        SingleSprite(ID3D11Device* pD3D11Device, UINT width, UINT height, const std::string& texturePath = "");

        virtual ~SingleSprite();

        /// <summary>
        /// Sets the texture from a file path
        /// </summary>
        /// <param name="pD3D11Device">Pointer to the ID3D11Device</param>
        /// <param name="texturePath">Path to the texture file</param>
        /// <returns>True if successful, false otherwise</returns>
        bool SetTexture(ID3D11Device* pD3D11Device, const std::string& texturePath);

        /// <summary>
        /// Sets the texture from an existing texture object
        /// </summary>
        /// <param name="pTexture">Shared pointer to the texture</param>
        /// <returns>True if successful, false otherwise</returns>
        bool SetTexture(std::shared_ptr<::Texture2DD3D11> pTexture);

        /// <summary>
        /// Initialize the sprite renderer
        /// </summary>
        /// <param name="pD3D11Device">Pointer to the ID3D11Device</param>
        /// <param name="width">Width of the render target</param>
        /// <param name="height">Height of the render target</param>
        /// <returns>True if successful, false otherwise</returns>
        bool Initialize(ID3D11Device* pD3D11Device, UINT width, UINT height);

        /// <summary>
        /// Draw the sprite
        /// </summary>
        /// <param name="pD3D11DeviceContext">Pointer to the ID3D11DeviceContext</param>
        /// <returns>True if successful, false otherwise</returns>
        bool Draw(ID3D11DeviceContext* pD3D11DeviceContext);

        /// <summary>
        /// Efficiently draw multiple sprites in a batch with shared state setup
        /// </summary>
        /// <param name="pD3D11DeviceContext">Pointer to the ID3D11DeviceContext</param>
        /// <param name="sprites">Vector of sprite pointers to draw</param>
        /// <returns>True if successful, false otherwise</returns>
        static bool DrawBatch(ID3D11DeviceContext* pD3D11DeviceContext, const std::vector<SingleSprite*>& sprites);

        /// <summary>
        /// Clean up static resources (call when shutting down graphics system)
        /// </summary>
        static void CleanupStaticResources();

        // Transform methods
        void Translate(glm::vec3 vector);
        void TranslateTo(glm::vec3 vector);
        void Rotate(glm::vec3 rotation);
        void RotateTo(glm::vec3 rotation);
        void Scale(glm::vec3 vector);
        void ScaleTo(glm::vec3 vector);

        void SetTransform(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale);
        void SetOpacity(float opacity);
        void SetBrightness(float brightness);

        // Colorize methods
        /// <summary>
        /// Enable colorization with RGB values (0.0-1.0 range)
        /// </summary>
        /// <param name="r">Red component (0.0-1.0)</param>
        /// <param name="g">Green component (0.0-1.0)</param>
        /// <param name="b">Blue component (0.0-1.0)</param>
        void SetColorize(float r, float g, float b);
        
        /// <summary>
        /// Enable colorization with glm::vec3 color (0.0-1.0 range)
        /// </summary>
        /// <param name="color">RGB color vector</param>
        void SetColorize(const glm::vec3& color);
        
        /// <summary>
        /// Enable colorization with hex color string (e.g., "#FF0000" for red)
        /// </summary>
        /// <param name="hexColor">Hex color string with # prefix</param>
        void SetColorize(const std::string& hexColor);
        
        /// <summary>
        /// Enable colorization with 32-bit color value (0xRRGGBB format)
        /// </summary>
        /// <param name="color">32-bit color value</param>
        void SetColorize(uint32_t color);
        
        /// <summary>
        /// Disable colorization effect
        /// </summary>
        void DisableColorize();
        
        /// <summary>
        /// Check if colorization is enabled
        /// </summary>
        /// <returns>True if colorize is enabled</returns>
        bool IsColorizeEnabled() const;

        /// <summary>
        /// Renders the sprite using ImGui directly to the internal render target
        /// This combines ImGui's rendering capabilities with our optimized render target setup
        /// </summary>
        /// <param name="size">Size of the sprite to render</param>
        void RenderToInternalRT(ImVec2 size);

        // Getters
        const auto& GetTranslation() const { return m_SpriteData.Translation; }
        const auto& GetRotation() const { return m_SpriteData.Rotation; }
        const auto& GetScale() const { return m_SpriteData.Scale; }
        const auto& GetOpacity() const { return m_SpriteData.Opacity; }
        const auto& GetBrightness() const { return m_SpriteData.Brightness; }

        /// <summary>
        /// Get the render target shader resource view for use with ImGui
        /// </summary>
        /// <returns>Pointer to the shader resource view</returns>
        const auto GetSRV() const { return m_pRenderTargetSRV.Get(); }

        /// <summary>
        /// Get the texture path currently used
        /// </summary>
        /// <returns>Current texture path</returns>
        const std::string& GetTexturePath() const { return m_TexturePath; }

        /// <summary>
        /// Resize the render target
        /// </summary>
        /// <param name="pD3D11Device">Pointer to the ID3D11Device</param>
        /// <param name="width">New width</param>
        /// <param name="height">New height</param>
        /// <returns>True if successful, false otherwise</returns>
        bool Resize(ID3D11Device* pD3D11Device, UINT width, UINT height);

    private:
        bool CreateRenderTargetView(ID3D11Device* pD3D11Device, UINT width, UINT height);
        bool CreateVertexBuffer(ID3D11Device* pD3D11Device);
        bool CreateIndexBuffer(ID3D11Device* pD3D11Device);
        bool CreateAndUpdateSpriteInfoBuffer(ID3D11Device* pD3D11Device);

        bool UpdateSpriteInfoBuffer(ID3D11DeviceContext* pD3D11DeviceContext);
        bool UpdateVertexConstantBuffer(ID3D11DeviceContext* pD3D11DeviceContext, const VERTEX_CONSTANT_BUFFER& cBuffer);
        bool UpdatePixelConstantBuffer(ID3D11DeviceContext* pD3D11DeviceContext, const PIXEL_CONSTANT_BUFFER& cBuffer);
        
        // Force constant buffer updates on next draw
        void InvalidateConstantBuffers();

        bool CreateShaders(ID3D11Device* pD3D11Device);
        bool CreateStates(ID3D11Device* pD3D11Device);

        void BackupD3D11State(ID3D11DeviceContext* ctx);
        void RestoreD3D11State(ID3D11DeviceContext* ctx);

    private:
        UINT m_RTWidth{ 0 }, m_RTHeight{ 0 };
        UINT m_MSAASampleCount{  8 }; // 8x MSAA + FXAA for maximum anti-aliasing

        bool m_SpriteUpdateQueued{ false };
        std::string m_TexturePath{};

        SpriteInfo m_SpriteData{};

        D3D11_VIEWPORT m_ViewPort{};
        BACKUP_DX11_STATE m_D3D11StateBackup;

        ComPtr<ID3D11RenderTargetView>      m_pRenderTargetView{};
        ComPtr<ID3D11ShaderResourceView>    m_pRenderTargetSRV{};
        ComPtr<ID3D11Texture2D>             m_pMSAARenderTarget{};
        ComPtr<ID3D11Texture2D>             m_pResolvedTexture{};
        ComPtr<ID3D11VertexShader>          m_pVertexShader{};
        ComPtr<ID3D11InputLayout>           m_pInputLayout{};
        ComPtr<ID3D11PixelShader>           m_pPixelShader{};
        ComPtr<ID3D11Buffer>                m_pVertexBuffer{};
        ComPtr<ID3D11Buffer>                m_pIndexBuffer{};
        ComPtr<ID3D11Buffer>                m_pVertexConstantBuffer{};
        ComPtr<ID3D11Buffer>                m_pPixelConstantBuffer{};
        ComPtr<ID3D11Buffer>                m_pSpriteInfoBuffer{};
        ComPtr<ID3D11BlendState>            m_pBlendState{};
        ComPtr<ID3D11RasterizerState>       m_pRasterizerState{};
        ComPtr<ID3D11DepthStencilState>     m_pDepthStencilStateOpaque{};
        ComPtr<ID3D11DepthStencilState>     m_pDepthStencilStateTransparent{};
        ComPtr<ID3D11Texture2D>             m_pDepthStencilBuffer{};
        ComPtr<ID3D11DepthStencilView>      m_pDepthStencilView{};
        ComPtr<ID3D11SamplerState>          m_pTextureSampler{};

        std::shared_ptr<::Texture2DD3D11>     m_pTexture{};

        size_t m_CurrentVBSize{ 0 };
        size_t m_CurrentIBSize{ 0 };
        size_t m_CurrentSpriteInfoBufferSize{ 0 };

        VERTEX_SHADER_SPRITE_INFO m_SpriteShaderInfo{};
        
        // Constant buffer caching for performance optimization
        VERTEX_CONSTANT_BUFFER m_CachedVertexCBuffer{};
        PIXEL_CONSTANT_BUFFER m_CachedPixelCBuffer{};
        bool m_VertexCBufferDirty{ true };
        bool m_PixelCBufferDirty{ true };

        // Colorize state management
        bool m_ColorizeEnabled{ false };
        glm::vec3 m_ColorizeColor{ 1.0f, 1.0f, 1.0f };
        float m_ColorizeWhiteThreshold{ 0.88f }; // Corresponds to #e2e1e2

    private: // Statics
        static constexpr D3D11_INPUT_ELEMENT_DESC s_VertexInputLayout[] =
        {
            // Slot 0 - per-vertex data
            { "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT,     0, (UINT)offsetof(Vertex_t, Pos),                                   D3D11_INPUT_PER_VERTEX_DATA,   0 },
            { "TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT,        0, (UINT)offsetof(Vertex_t, UV),                                    D3D11_INPUT_PER_VERTEX_DATA,   0 },
            { "COLOR",      0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, (UINT)offsetof(Vertex_t, Col),                                   D3D11_INPUT_PER_VERTEX_DATA,   0 },

            // Slot 1 - per-instance data (for single sprite, this will be constant buffer data)
            { "OPACITY",    0, DXGI_FORMAT_R32_FLOAT,           1, (UINT)offsetof(VERTEX_SHADER_SPRITE_INFO, Opacity),      D3D11_INPUT_PER_INSTANCE_DATA, 1 },
            { "BRIGHTNESS", 0, DXGI_FORMAT_R32_FLOAT,           1, (UINT)offsetof(VERTEX_SHADER_SPRITE_INFO, Brightness),   D3D11_INPUT_PER_INSTANCE_DATA, 1 },
            { "TRANSLATION",0, DXGI_FORMAT_R32G32B32_FLOAT,     1, (UINT)offsetof(VERTEX_SHADER_SPRITE_INFO, Translation),  D3D11_INPUT_PER_INSTANCE_DATA, 1 },
            { "ROTATION",   0, DXGI_FORMAT_R32G32B32_FLOAT,     1, (UINT)offsetof(VERTEX_SHADER_SPRITE_INFO, Rotation),     D3D11_INPUT_PER_INSTANCE_DATA, 1 },
            { "SCALE",      0, DXGI_FORMAT_R32G32B32_FLOAT,     1, (UINT)offsetof(VERTEX_SHADER_SPRITE_INFO, Scale),        D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        };

        static constexpr std::array<uint32_t, 6> s_SpriteIndices{ 0, 1, 2, 0, 2, 3 };
    };
}

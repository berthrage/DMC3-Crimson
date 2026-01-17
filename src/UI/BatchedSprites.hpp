#pragma once

#include <array>

#include "GraphicsBase.hpp"
#include "Sprite.hpp"
#include "Texture2DArrayD3D11.hpp"

namespace Graphics {
    class BatchedSprites : public ErrorHandled
    {
    private:
    	struct SpriteInfo
        {
    		size_t		TextureIdx;
    		float		Opacity;
    		float		Brightness;
    		glm::vec3	Translation;
            glm::vec3   Rotation;
            glm::vec3   Scale;
    	};
    
    #pragma pack(push, 1)
    	struct VERTEX_SHADER_SPRITEINSTANCE_INFO
        {
    		uint32_t	TextureIdx;
    		float		Opacity;
    		float		Brightness;
            glm::vec3	Translation;
            glm::vec3	Rotation;
            glm::vec3	Scale;
    	};
    
    	__declspec(align(16)) struct VERTEX_CONSTANT_BUFFER
        {
    		alignas(16) glm::mat4 MVP;
    	};
    
    	__declspec(align(16)) struct PIXEL_CONSTANT_BUFFER
        {
    		uint32_t TextureCount;
    		uint32_t SamplerCount;
    	};
    #pragma pack(pop)
    
    public:
        BatchedSprites(ID3D11Device* pD3D11Device, UINT width, UINT height, const std::vector<SpriteDesc>& spriteDescs,
            std::shared_ptr<Texture2DArrayD3D11> pSpriteTextures, const std::vector<size_t> spriteIndices);
    
    	virtual ~BatchedSprites();
    
    	/// <summary>
    	///     Reads the sprite info list provided and prepares the necessary data for rendering,
        ///     if the last argument (spriteTextures) is NOT provided it will load the the textures
        ///     into video memory from disk, but if provided it will not try to reload the textures from the disk which is slow
    	/// </summary>
    	/// <param name="pD3D11Device">Pointer to the ID3D11Device to use for rendering</param>
    	/// <param name="spriteDescs">A vector tha contains the SpriteDesc objects that contain
        /// the information about the image of each sprite </param>
    	/// <param name="spriteIndices">(Optional) A list of indices of the sprites to be batched and rendered,
        /// if not provided, it will default to using all of the sprites</param>
    	/// <param name="pSpriteTextureArray">(Optional) A pointer to the already loaded into vram sprite texture array,
        /// if not provided it will load the sprites into vram by reading them from the disk which is slow</param>
    	void UpdateSprites(ID3D11Device* pD3D11Device, const std::vector<SpriteDesc>& spritesInfo,
            const std::vector<size_t> spriteIndices, std::shared_ptr<Texture2DArrayD3D11> pSpriteTextureArray);
    
    	void ChangeSprites(ID3D11Device* pD3D11Device, const std::vector<SpriteDesc>& spriteDescs,
            const std::vector<size_t> spriteIndices, std::shared_ptr<Texture2DArrayD3D11> pSpriteTextureArray);
    
    	bool Initialize(ID3D11Device* pD3D11Device, UINT width, UINT height);
    	bool Draw(ID3D11DeviceContext* pD3D11DeviceContext);
    
    	void Translate(size_t idx, glm::vec3 vector);
    	void TranslateTo(size_t idx, glm::vec3 vector);
    	void Rotate(size_t idx, glm::vec3 rotation);
    	void RotateTo(size_t idx, glm::vec3 rotation);
    	void Scale(size_t idx, glm::vec3 vector);
    	void ScaleTo(size_t idx, glm::vec3 vector);
    
    	void SetTransform(size_t idx, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale);
    	void SetOpacity(size_t idx, float opacity);
    	void SetBrightness(size_t idx, float brightness);
    
        const auto& GetTranslation(size_t idx) { return m_SpriteInstanceData[idx].Translation; }
        const auto& GetRotation(size_t idx) { return m_SpriteInstanceData[idx].Rotation; }
        const auto& GetScale(size_t idx) { return m_SpriteInstanceData[idx].Scale; }
        const auto& GetOpacity(size_t idx) { return m_SpriteInstanceData[idx].Opacity; }
        const auto& GetBrightness(size_t idx) { return m_SpriteInstanceData[idx].Brightness; }
    
    	//void Reorder(const std::vector<size_t>& spriteIndices);
    
    	const auto& GetCustomIndices() { return m_SpriteIndicesIntoTheTextureArray; }
    
    	const auto GetRTSRV() const { return m_pRenderTargetSRV.Get(); };
    
    private:
    	//void SortByDepth();
    
    	bool CreateRenderTargetView(ID3D11Device* pD3D11Device, UINT width, UINT height);
    
    	bool CreateVertexBuffer(ID3D11Device* pD3D11Device);
    	bool CreateIndexBuffer(ID3D11Device* pD3D11Device);
    	bool CreateAndUpdateSpriteInfoBuffer(ID3D11Device* pD3D11Device);
    
        bool UpdateSpriteInstanceInfoBuffer(ID3D11DeviceContext* pD3D11DeviceContext, const std::vector<size_t>& indices = {});
    	bool UpdateVertexConstantBuffer(ID3D11DeviceContext* pD3D11DeviceContext, const VERTEX_CONSTANT_BUFFER& cBuffer);
    	bool UpdatePixelConstantBuffer(ID3D11DeviceContext* pD3D11DeviceContext, const PIXEL_CONSTANT_BUFFER& cBuffer);
    
    	bool CreateSpriteInfoBufferSrv(ID3D11Device* pD3D11Device);
    
    	void BackupD3D11State(ID3D11DeviceContext* ctx);
    	void RestoreD3D11State(ID3D11DeviceContext* ctx);
    
    private:
    	UINT m_RTWidth{ 0 }, m_RTHeight{ 0 };

        struct {
            bool                UpdateQueued{ false };
            std::vector<size_t> Indices{};
        } m_SpriteUpdateInfo;

    	std::vector<Vertex_t> m_Vertices{};
    
        // Indices given by the user, to know which sprites inside the texture array to render
    	std::vector<size_t> m_SpriteIndicesIntoTheTextureArray{};
    
    	D3D11_VIEWPORT m_ViewPort{};
    
    	BACKUP_DX11_STATE m_D3D11StateBackup;
    
    	ComPtr<ID3D11RenderTargetView>		m_pRenderTargetView{};
    	ComPtr<ID3D11ShaderResourceView>	m_pRenderTargetSRV{};
    	ComPtr<ID3D11VertexShader>			m_pVertexShader{};
    	ComPtr<ID3D11InputLayout>			m_pInputLayout{};
    	ComPtr<ID3D11PixelShader>			m_pPixelShader{};
    	ComPtr<ID3D11Buffer>				m_pVertexBuffer{};
    	ComPtr<ID3D11Buffer>				m_pIndexBuffer{};
    	ComPtr<ID3D11Buffer>				m_pVertexConstantBuffer{};
    	ComPtr<ID3D11Buffer>				m_pPixelConstantBuffer{};
    	ComPtr<ID3D11Buffer>				m_pSpriteInstanceInfoBuffer{};
    	ComPtr<ID3D11ShaderResourceView>	m_pInstanceInfoBufferSRV{};
    	ComPtr<ID3D11BlendState>			m_pBlendState{};
    	ComPtr<ID3D11RasterizerState>		m_pRasterizerState{};
    	ComPtr<ID3D11DepthStencilState>		m_pDepthStencilStateOpaque{};
    	ComPtr<ID3D11DepthStencilState>		m_pDepthStencilStateTransparent{};
    	ComPtr<ID3D11Texture2D>				m_pDepthStincilBuffer{};
    	ComPtr<ID3D11DepthStencilView>		m_pDepthStincilView{};
    	ComPtr<ID3D11SamplerState>			m_pTextureSampler{};
    
    	std::shared_ptr<Texture2DArrayD3D11>	m_pTextureArray{};
    	std::vector<SpriteInfo>					m_SpriteInstanceData{};
    
    	size_t m_CurrentVBSize{ 0 };
    	size_t m_CurrentIBSize{ 0 };
    	size_t m_CurrentSpriteInstanceInfoBufferSize{ 0 };

		std::vector<VERTEX_SHADER_SPRITEINSTANCE_INFO> m_InstanceInfoArray;
		std::vector<VERTEX_SHADER_SPRITEINSTANCE_INFO> m_InfoArray;
    
    private: // Statics
        static constexpr D3D11_INPUT_ELEMENT_DESC s_VertexInputLayout[] =
        {
        // Slot 0 - per-vertex data
            { "POSITION",       0, DXGI_FORMAT_R32G32B32_FLOAT,     0, (UINT)offsetof(Vertex_t, Pos),                                   D3D11_INPUT_PER_VERTEX_DATA,   0 }, // 0 for advance per vertex
            { "TEXCOORD",       0, DXGI_FORMAT_R32G32_FLOAT,        0, (UINT)offsetof(Vertex_t, UV),                                    D3D11_INPUT_PER_VERTEX_DATA,   0 }, // 0 for advance per vertex
            { "COLOR",          0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, (UINT)offsetof(Vertex_t, Col),                                   D3D11_INPUT_PER_VERTEX_DATA,   0 }, // 0 for advance per vertex

        // Slot 1 - per-instance data
            { "TEXTURE_IDX",    0, DXGI_FORMAT_R32_UINT,            1, (UINT)offsetof(VERTEX_SHADER_SPRITEINSTANCE_INFO, TextureIdx),   D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 1 for advance per instance
            { "OPACITY",        0, DXGI_FORMAT_R32_FLOAT,           1, (UINT)offsetof(VERTEX_SHADER_SPRITEINSTANCE_INFO, Opacity),      D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 1 for advance per instance
            { "BRIGHTNESS",     0, DXGI_FORMAT_R32_FLOAT,           1, (UINT)offsetof(VERTEX_SHADER_SPRITEINSTANCE_INFO, Brightness),   D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 1 for advance per instance

            // Transformation matrix (split into 4 float4s)
            { "TRANSLATION",    0, DXGI_FORMAT_R32G32B32_FLOAT,     1, (UINT)offsetof(VERTEX_SHADER_SPRITEINSTANCE_INFO, Translation),  D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 1 for advance per instance
            { "ROTATION",       0, DXGI_FORMAT_R32G32B32_FLOAT,     1, (UINT)offsetof(VERTEX_SHADER_SPRITEINSTANCE_INFO, Rotation), D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 1 for advance per instance
            { "SCALE",          0, DXGI_FORMAT_R32G32B32_FLOAT,     1, (UINT)offsetof(VERTEX_SHADER_SPRITEINSTANCE_INFO, Scale),        D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 1 for advance per instance
        };

        static constexpr std::array<uint32_t, 6> s_SpriteIndices{ 0, 1, 2, 0, 2, 3 };
    };
}

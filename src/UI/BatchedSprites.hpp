#pragma once

#include "GraphicsBase.hpp"
#include "Sprite.hpp"
#include "Texture2DArrayD3D11.hpp"
#include "..\ThirdParty\glm\gtc\quaternion.hpp"

namespace Graphics {
class BatchedSprites : public ErrorHandled {
private:
	struct SpriteInfo {
		size_t		TexturePathIdx;
		size_t		VerticesOffset;
		size_t		IndicesOffset;
		float		Opacity;
		float		Brightness;
		glm::vec3	Translation;
        glm::quat   RotationQuat;
        glm::vec3   Scale;
	};

#pragma pack(push, 1)
	struct VERTEX_SHADER_SPRITE_INFO {
		uint32_t	TextureIdx;
		float		Opacity;
		float		Brightness;
		float padding;
		glm::mat4	TransformationMatrix;
	};

	__declspec(align(16)) struct VERTEX_CONSTANT_BUFFER {
		alignas(16) glm::mat4 MVP;
		uint32_t VertexPerSprite;
		uint32_t IndexPerSprite;
	};

	__declspec(align(16)) struct PIXEL_CONSTANT_BUFFER {
		uint32_t TextureCount;
		uint32_t SamplerCount;
	};
#pragma pack(pop)

public:
	BatchedSprites(ID3D11Device* pD3D11Device, UINT width, UINT height, const std::vector<SpriteDesc>& spriteDescs,
		const std::vector<size_t> spriteIndices = {});

    BatchedSprites(ID3D11Device* pD3D11Device, UINT width, UINT height, const std::vector<SpriteDesc>& spriteDescs,
        std::shared_ptr<Texture2DArrayD3D11> pSpriteTextures, const std::vector<size_t> spriteIndices = {});

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
        const std::vector<size_t> spriteIndices = {}, std::shared_ptr<Texture2DArrayD3D11> pSpriteTextureArray = {});

	void SetActiveSprites(const std::vector<size_t> spriteIndices = {});

	bool Initialize(ID3D11Device* pD3D11Device, UINT width, UINT height);
	bool Draw(ID3D11DeviceContext* pD3D11DeviceContext);

	void Translate(size_t idx, glm::vec3 vector);
	void TranslateTo(size_t idx, glm::vec3 vector);
	void Rotate(size_t idx, float rotation, glm::vec3 axis);
	void RotateTo(size_t idx, float rotation, glm::vec3 axis);
	void Scale(size_t idx, glm::vec3 vector);
	void ScaleTo(size_t idx, glm::vec3 vector);

	void SetTransform(size_t idx, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale);
	void SetOpacity(size_t idx, float opacity);
	void SetBrightness(size_t idx, float brightness);
	void ForceUpdateSpriteInfoBuffer();

    const auto& GetTranslation(size_t idx) { return m_SpritesData[idx].Translation; }
    const auto& GetRotationQuat(size_t idx) { return m_SpritesData[idx].RotationQuat; }
    const auto& GetScale(size_t idx) { return m_SpritesData[idx].Scale; }
    const auto& GetOpacity(size_t idx) { return m_SpritesData[idx].Opacity; }
    const auto& GetBrightness(size_t idx) { return m_SpritesData[idx].Brightness; }

	void Reorder(const std::vector<size_t>& spriteIndices);

	const auto GetRTSRV() const { return m_pRTSRV.Get(); };

private:
	std::optional<size_t> FindTexturePathIndex(const std::string& path);
	void PopulateActiveIndices();
	//void SortByDepth();

	bool CreateRenderTargetView(ID3D11Device* pD3D11Device, UINT width, UINT height);

	bool CreateAndUpdateVertexBuffer(ID3D11Device* pD3D11Device);
	bool CreateAndUpdateIndexBuffer(ID3D11Device* pD3D11Device);
	bool CreateAndUpdateSpriteInfoBuffer(ID3D11Device* pD3D11Device);

	bool UpdateVertexBuffer(ID3D11DeviceContext* pD3D11DeviceContext);
	bool UpdateIndexBuffer(ID3D11DeviceContext* pD3D11DeviceContext);
	bool UpdateSpriteInfoBuffer(ID3D11DeviceContext* pD3D11DeviceContext);
	bool UpdateVertexConstantBuffer(ID3D11DeviceContext* pD3D11DeviceContext, const VERTEX_CONSTANT_BUFFER& cBuffer);
	bool UpdatePixelConstantBuffer(ID3D11DeviceContext* pD3D11DeviceContext, const PIXEL_CONSTANT_BUFFER& cBuffer);

	bool CreateSpriteInfoBufferSrv(ID3D11Device* pD3D11Device);

	void BackupD3D11State(ID3D11DeviceContext* ctx);
	void RestoreD3D11State(ID3D11DeviceContext* ctx);

private:
	static constexpr D3D11_INPUT_ELEMENT_DESC s_VertexInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, (UINT)offsetof(Vertex_t, Pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, (UINT)offsetof(Vertex_t, UV),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, (UINT)offsetof(Vertex_t, Col), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	static constexpr size_t s_VertexPerSprite{ 4 }, s_IndexPerSprite{ 6 };

	UINT m_RTWidth{ 0 }, m_RTHeight{ 0 };

	bool m_UpdateVertexBuffer{ false };
	bool m_UpdateIndexBuffer{ false };
	bool m_SpriteInfoUpdateQueued{ false };

	std::vector<Vertex_t> m_Vertices{};
	std::vector<uint32_t> m_Indices{};
	std::vector<uint32_t> m_ActiveIndices{};

	std::vector<size_t> m_CustomSpriteIndices{};

	D3D11_VIEWPORT m_VP{};

	BACKUP_DX11_STATE m_D3D11StateBackup;

	ComPtr<ID3D11RenderTargetView>		m_pRTV{};
	ComPtr<ID3D11ShaderResourceView>	m_pRTSRV{};
	ComPtr<ID3D11VertexShader>			m_pVertexShader{};
	ComPtr<ID3D11InputLayout>			m_pInputLayout{};
	ComPtr<ID3D11PixelShader>			m_pPixelShader{};
	ComPtr<ID3D11Buffer>				m_pVertexBuffer{};
	ComPtr<ID3D11Buffer>				m_pIndexBuffer{};
	ComPtr<ID3D11Buffer>				m_pVertexConstantBuffer{};
	ComPtr<ID3D11Buffer>				m_pPixelConstantBuffer{};
	ComPtr<ID3D11Buffer>				m_pSpriteInfoBuffer{};
	ComPtr<ID3D11ShaderResourceView>	m_pTIBSRV{};
	ComPtr<ID3D11BlendState>			m_pBlendState{};
	ComPtr<ID3D11RasterizerState>		m_pRasterizerState{};
	ComPtr<ID3D11DepthStencilState>		m_pDepthStencilStateOpaque{};
	ComPtr<ID3D11DepthStencilState>		m_pDepthStencilStateTransparent{};
	ComPtr<ID3D11Texture2D>				m_pDepthStincilBuffer{};
	ComPtr<ID3D11DepthStencilView>		m_pDepthStincilView{};
	ComPtr<ID3D11SamplerState>			m_pTextureSampler{};

	std::vector<std::string>				m_TexturePaths{};
	std::shared_ptr<Texture2DArrayD3D11>	m_pTextureArray{};
	std::vector<SpriteInfo>					m_SpritesData{};

	size_t m_CurrentVBSize{ 0 };
	size_t m_CurrentIBSize{ 0 };
	size_t m_CurrentSIBSize{ 0 };
};
}
